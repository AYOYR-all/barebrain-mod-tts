#include "core/mod/brn_mod.h"
#include "tool_tts.h"
#include "tools/tool_registry.h"

static const char *const tool_tts_deps[] = {
    "core.tool_registry",
    NULL,
};

static const brn_tool_t tts_speak_tool = {
    .name = "tts_speak",
    .description = "Play a preconfigured WonderEcho phrase by ID.",
    .input_schema_json =
        "{\"type\":\"object\","
        "\"properties\":{"
        "\"phrase_id\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255},"
        "\"type\":{\"type\":\"string\",\"enum\":[\"general\",\"command\"]}"
        "},"
        "\"required\":[\"phrase_id\"]}",
    .execute = tool_tts_execute,
};

static esp_err_t tool_tts_mod_init(void)
{
    esp_err_t err = tool_tts_init();
    if (err != ESP_OK) {
        return err;
    }
    return brn_tool_register(&tts_speak_tool);
}

const brn_mod_t brn_mod_tool_tts = {
    .id = "tool-tts",
    .name = "Text To Speech Tool",
    .version = "0.1.1",
    .deps = tool_tts_deps,
    .init = tool_tts_mod_init,
};
