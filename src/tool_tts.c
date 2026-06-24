#include "tool_tts.h"

#include <stdio.h>

#include "cJSON.h"

esp_err_t tool_tts_init(void)
{
    /*
     * The I2C bus and module command protocol are configured by the firmware
     * Profile after the user selects SDA and SCL in BareBrain Manager.
     */
    return ESP_OK;
}

esp_err_t tool_tts_execute(const char *input_json, char *output, size_t output_size)
{
    if (!input_json || !output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    const char *text = cJSON_GetStringValue(cJSON_GetObjectItem(root, "text"));
    if (!text || text[0] == '\0') {
        cJSON_Delete(root);
        snprintf(output, output_size, "Error: 'text' is required");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);
    snprintf(output, output_size,
             "Error: Hiwonder voice module SDA and SCL are not configured");
    return ESP_ERR_NOT_SUPPORTED;
}
