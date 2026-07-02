#include "tool_tts.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_log.h"
#include "feedback/face_state.h"
#include "generated/brn_profile_config.h"

#ifndef BRN_PROFILE_TTS_I2C_SDA
#error "tool-tts requires tts.i2c_sda in the firmware Profile"
#endif
#ifndef BRN_PROFILE_TTS_I2C_SCL
#error "tool-tts requires tts.i2c_scl in the firmware Profile"
#endif

#define WONDERECHO_ADDRESS 0x34
#define WONDERECHO_RECOGNITION_REGISTER 0x64
#define WONDERECHO_BROADCAST_REGISTER 0x6E
#define WONDERECHO_COMMAND_PHRASE 0x00
#define WONDERECHO_GENERAL_PHRASE 0xFF
#define WONDERECHO_I2C_TIMEOUT_MS 500

static const char *TAG = "tool_tts";
static i2c_master_bus_handle_t s_bus;
static i2c_master_dev_handle_t s_device;
static bool s_ready;

static esp_err_t wonder_echo_probe(void)
{
    if (!s_bus) {
        s_ready = false;
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = i2c_master_probe(s_bus, WONDERECHO_ADDRESS, WONDERECHO_I2C_TIMEOUT_MS);
    s_ready = (err == ESP_OK);
    return err;
}

static esp_err_t wonder_echo_require_ready(char *output, size_t output_size)
{
    esp_err_t err = wonder_echo_probe();
    if (err != ESP_OK) {
        snprintf(output, output_size,
                 "Error: WonderEcho not connected at I2C address 0x34 (%s)",
                 esp_err_to_name(err));
    }
    return err;
}

static bool parse_phrase_type(const char *type, uint8_t *broadcast_type)
{
    if (!type || strcmp(type, "general") == 0 ||
        strcmp(type, "announcement") == 0 ||
        strcmp(type, "announcer") == 0) {
        *broadcast_type = WONDERECHO_GENERAL_PHRASE;
        return true;
    }
    if (strcmp(type, "command") == 0) {
        *broadcast_type = WONDERECHO_COMMAND_PHRASE;
        return true;
    }
    return false;
}

esp_err_t tool_tts_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = BRN_PROFILE_TTS_I2C_SDA,
        .scl_io_num = BRN_PROFILE_TTS_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &s_bus),
                        TAG, "initialize I2C bus");

    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = WONDERECHO_ADDRESS,
        .scl_speed_hz = 100000,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(s_bus, &device_config, &s_device),
                        TAG, "attach WonderEcho");
    esp_err_t probe_err = wonder_echo_probe();
    if (probe_err == ESP_OK) {
        ESP_LOGI(TAG, "WonderEcho ready at I2C address 0x34");
    } else {
        ESP_LOGW(TAG, "WonderEcho not found at 0x34 yet: %s",
                 esp_err_to_name(probe_err));
    }
    return ESP_OK;
}

esp_err_t tool_tts_status_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;
    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = wonder_echo_probe();
    if (err == ESP_OK) {
        snprintf(output, output_size,
                 "WonderEcho connected at I2C address 0x34; speak register=0x6E, recognition register=0x64");
    } else {
        snprintf(output, output_size,
                 "Error: WonderEcho not connected at I2C address 0x34 (%s)",
                 esp_err_to_name(err));
    }
    return err;
}

esp_err_t tool_tts_execute(const char *input_json, char *output, size_t output_size)
{
    if (!input_json || !output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = wonder_echo_require_ready(output, output_size);
    if (err != ESP_OK) {
        return err;
    }

    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }
    cJSON *phrase_id = cJSON_GetObjectItem(root, "phrase_id");
    cJSON *phrase_type = cJSON_GetObjectItem(root, "type");
    if (!cJSON_IsNumber(phrase_id) ||
        phrase_id->valuedouble < 0 ||
        phrase_id->valuedouble > 255 ||
        phrase_id->valuedouble != (int)phrase_id->valuedouble) {
        cJSON_Delete(root);
        snprintf(output, output_size, "Error: phrase_id must be an integer from 0 to 255");
        return ESP_ERR_INVALID_ARG;
    }

    const char *type = cJSON_IsString(phrase_type) ? phrase_type->valuestring : "general";
    uint8_t broadcast_type;
    if (!parse_phrase_type(type, &broadcast_type)) {
        cJSON_Delete(root);
        snprintf(output, output_size,
                 "Error: type must be 'general', 'announcement', 'announcer', or 'command'");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t packet[] = {
        WONDERECHO_BROADCAST_REGISTER,
        broadcast_type,
        (uint8_t)phrase_id->valueint,
    };
    err = i2c_master_transmit(s_device, packet, sizeof(packet), WONDERECHO_I2C_TIMEOUT_MS);
    if (err == ESP_OK) {
        brn_face_set("speaking", 3500);
        snprintf(output, output_size,
                 "WonderEcho broadcast started: type=%s, phrase_id=%d",
                 broadcast_type == WONDERECHO_COMMAND_PHRASE ? "command" : "announcement",
                 phrase_id->valueint);
    } else {
        snprintf(output, output_size, "Error: WonderEcho I2C write failed (%s)",
                 esp_err_to_name(err));
    }
    cJSON_Delete(root);
    return err;
}

esp_err_t tool_tts_read_recognition_execute(const char *input_json,
                                            char *output,
                                            size_t output_size)
{
    (void)input_json;
    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = wonder_echo_require_ready(output, output_size);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t reg = WONDERECHO_RECOGNITION_REGISTER;
    uint8_t result = 0;
    err = i2c_master_transmit_receive(s_device, &reg, sizeof(reg),
                                      &result, sizeof(result),
                                      WONDERECHO_I2C_TIMEOUT_MS);
    if (err == ESP_OK) {
        snprintf(output, output_size,
                 "WonderEcho recognition result: %u (0x%02X)",
                 result, result);
    } else {
        snprintf(output, output_size,
                 "Error: WonderEcho I2C read failed (%s)",
                 esp_err_to_name(err));
    }
    return err;
}
