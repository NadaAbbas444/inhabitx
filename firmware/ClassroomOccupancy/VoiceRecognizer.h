#pragma once
#include <ESP_SR.h> // Also lets Arduino discover/build the ESP_SR library and copy srmodels.bin.
#include "esp_partition.h"
#include "esp_mn_models.h"
#include "esp_wn_models.h"
#include "model_path.h"
#include "VoicePolicy.h"

// MultiNet recognizes a custom activation phrase; this is NOT a trained WakeNet
// model for "Hey Infineon". Accuracy/CPU use need validation on the actual board.
constexpr bool VOICE_ENABLED = true;
constexpr uint32_t VOICE_AUDIO_MAX_AGE_MS = 1000;
struct VoiceEvent { VoiceCommand command; uint32_t capturedMs, epoch; };
static QueueHandle_t voiceAudioQueue, voiceEventQueue;
static bool voiceRunning = false;
static volatile uint32_t voiceFeedMs = 0, voiceFeedEpoch = 0, voiceGaps = 0;
static volatile uint32_t voiceFault = 0;

static esp_err_t fillVoiceAudio(void*, void* out, size_t len, size_t* bytesRead, uint32_t) {
    // Only this callback consumes this queue. Capture remains the sole I2S owner.
    static AudioBlock pending = {};
    static size_t position = 0;
    static bool receivedAudio = false;
    *bytesRead = 0;
    auto* destination = static_cast<uint8_t*>(out);
    while (*bytesRead < len) {
        if (__atomic_load_n(&voiceFault, __ATOMIC_RELAXED)) return ESP_ERR_INVALID_STATE;
        if (position >= pending.frames) {
            if (xQueueReceive(voiceAudioQueue, &pending, pdMS_TO_TICKS(100)) != pdTRUE) {
                if (receivedAudio) __atomic_store_n(&voiceFault, 1, __ATOMIC_RELAXED);
                return ESP_ERR_TIMEOUT;
            }
            receivedAudio = true;
            position = 0;
        }
        const uint32_t epoch = __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED);
        if (pending.epoch != epoch || uint32_t(millis() - pending.capturedMs) > VOICE_AUDIO_MAX_AGE_MS) {
            position = pending.frames;
            __atomic_add_fetch(&voiceGaps, 1, __ATOMIC_RELAXED);
            __atomic_store_n(&voiceFault, 1, __ATOMIC_RELAXED);
            *bytesRead = 0;
            return ESP_ERR_TIMEOUT;
        }
        const size_t available = (pending.frames - position) * sizeof(int16_t);
        const size_t count = available < len - *bytesRead ? available : len - *bytesRead;
        memcpy(destination + *bytesRead, pending.pcm + position, count);
        position += count / sizeof(int16_t);
        *bytesRead += count;
        __atomic_store_n(&voiceFeedMs, pending.capturedMs, __ATOMIC_RELAXED);
        __atomic_store_n(&voiceFeedEpoch, pending.epoch, __ATOMIC_RELAXED);
    }
    return ESP_OK;
}

static void onVoiceEvent(void*, sr_event_t event, int commandId, int) {
    if (!__atomic_load_n(&voiceFault, __ATOMIC_RELAXED) && event == SR_EVENT_COMMAND && commandId >= 1 && commandId <= 3) {
        const VoiceEvent command = {
            static_cast<VoiceCommand>(commandId),
            __atomic_load_n(&voiceFeedMs, __ATOMIC_RELAXED),
            __atomic_load_n(&voiceFeedEpoch, __ATOMIC_RELAXED)
        };
        if (xQueueSend(voiceEventQueue, &command, 0) != pdTRUE) {
            __atomic_add_fetch(&voiceGaps, 1, __ATOMIC_RELAXED);
            __atomic_store_n(&voiceFault, 1, __ATOMIC_RELAXED);
        }
    }
    // Continue recognizing the three phrases. Application policy gates room
    // commands behind the custom activation phrase, with an 8-second deadline.
    if (event == SR_EVENT_COMMAND || event == SR_EVENT_TIMEOUT) sr_set_mode(SR_MODE_COMMAND);
}

static void initVoiceRecognition() {
    if (!VOICE_ENABLED) return;
    if (!psramFound() || !esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "model")) {
        Serial.println("Voice: disabled; enable OPI PSRAM and select ESP SR 16M partition, then upload.");
        return;
    }
    // sr_start assumes valid models; check first to avoid dereferencing a missing model.
    srmodel_list_t* models = esp_srmodel_init("model");
    const bool valid = models && esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_ENGLISH) &&
                       esp_srmodel_filter(models, ESP_WN_PREFIX, nullptr);
    if (models) esp_srmodel_deinit(models);
    if (!valid) {
        Serial.println("Voice: disabled; missing ESP-SR English models. Upload with ESP SR 16M selected.");
        return;
    }
    voiceAudioQueue = xQueueCreateWithCaps(16, sizeof(AudioBlock), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    voiceEventQueue = xQueueCreate(8, sizeof(VoiceEvent));
    if (!voiceAudioQueue || !voiceEventQueue) {
        if (voiceAudioQueue) { vQueueDeleteWithCaps(voiceAudioQueue); voiceAudioQueue = nullptr; }
        if (voiceEventQueue) { vQueueDelete(voiceEventQueue); voiceEventQueue = nullptr; }
        Serial.println("Voice: disabled; insufficient queue memory.");
        return;
    }
    static const sr_cmd_t commands[] = {
        {1, "Hey Infineon"},
        {2, "I am in the room"},
        {3, "I am leaving the room"}
    };
    const esp_err_t status = sr_start(fillVoiceAudio, nullptr, SR_CHANNELS_MONO,
        SR_MODE_COMMAND, "M", commands, sizeof commands / sizeof commands[0], onVoiceEvent, nullptr);
    if (status != ESP_OK) {
        Serial.printf("Voice: start failed (%s); automatic sensing continues.\n", esp_err_to_name(status));
        return;
    }
    voiceRunning = true;
    Serial.println("Voice: ready | say Hey Infineon, pause, then your room command");
}
