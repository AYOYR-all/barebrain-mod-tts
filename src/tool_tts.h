#pragma once

#include <stddef.h>

#include "esp_err.h"

esp_err_t tool_tts_init(void);
esp_err_t tool_tts_start(void);
esp_err_t tool_tts_status_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_tts_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_tts_read_recognition_execute(const char *input_json, char *output, size_t output_size);
