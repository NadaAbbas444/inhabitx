#pragma once
#include "esp32-hal-sr.h"
#include "VoiceTiming.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*voice_engine_event_cb)(void*, sr_event_t, int, int, float);
esp_err_t voice_engine_start(sr_fill_cb fill_cb, void* fill_arg, sr_channels_t channels,
    sr_mode_t mode, const char* input_format, const sr_cmd_t* commands, size_t command_count,
    voice_engine_event_cb callback, void* callback_arg);
esp_err_t voice_engine_stop(void);
esp_err_t voice_engine_set_mode(sr_mode_t mode);
esp_err_t voice_engine_pause(void);
esp_err_t voice_engine_resume(void);
#ifdef __cplusplus
}
#endif
