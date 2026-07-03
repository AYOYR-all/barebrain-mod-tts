#include "core/mod/brn_mod.h"
#include "tool_tts.h"
#include "tools/tool_registry.h"

static const char *const tool_tts_deps[] = {
    "core.tool_registry",
    "core.message_bus",
    NULL,
};

static const brn_tool_t tts_speak_tool = {
    .name = "tts_speak",
    .description = "Play a preconfigured WonderEcho Chinese firmware phrase by ID over I2C.",
    .input_schema_json =
        "{\"type\":\"object\","
        "\"properties\":{"
        "\"phrase_id\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255},"
        "\"type\":{\"type\":\"string\",\"enum\":[\"general\",\"announcement\",\"announcer\",\"command\"]}"
        "},"
        "\"required\":[\"phrase_id\"]}",
    .execute = tool_tts_execute,
};

static const brn_tool_t tts_status_tool = {
    .name = "tts_status",
    .description = "Check whether the Hiwonder WonderEcho module is connected on I2C address 0x34.",
    .input_schema_json = "{\"type\":\"object\",\"properties\":{}}",
    .execute = tool_tts_status_execute,
};

static const brn_tool_t tts_read_recognition_tool = {
    .name = "tts_read_recognition",
    .description = "Read the latest WonderEcho speech recognition result byte from I2C register 0x64.",
    .input_schema_json = "{\"type\":\"object\",\"properties\":{}}",
    .execute = tool_tts_read_recognition_execute,
};

static esp_err_t tool_tts_mod_init(void)
{
    esp_err_t err = tool_tts_init();
    if (err != ESP_OK) {
        return err;
    }
    err = brn_tool_register(&tts_status_tool);
    if (err != ESP_OK) {
        return err;
    }
    err = brn_tool_register(&tts_speak_tool);
    if (err != ESP_OK) {
        return err;
    }
    return brn_tool_register(&tts_read_recognition_tool);
}

const brn_mod_t brn_mod_tool_tts = {
    .id = "tool-tts",
    .name = "Text To Speech Tool",
    .version = "0.1.4",
    .deps = tool_tts_deps,
    .init = tool_tts_mod_init,
    .start = tool_tts_start,
};
