#pragma once
#include <ESP_SR.h> // Also lets Arduino discover/build the ESP_SR library and copy srmodels.bin.
#include "esp_partition.h"
#include "esp_mn_models.h"
#include "esp_wn_models.h"
#include "model_path.h"
#include "VoicePolicy.h"
#include "VoiceContinuity.h"

// MultiNet recognizes a custom activation phrase; this is NOT a trained WakeNet
// model for "Hey Infineon". Accuracy/CPU use need validation on the actual board.
constexpr bool VOICE_ENABLED = true;
constexpr uint32_t VOICE_AUDIO_MAX_AGE_MS = 1000;
constexpr uint32_t VOICE_RECOVERY_SAMPLES = SAMPLE_RATE * 6;
struct VoiceEvent { VoiceCommand command; uint32_t capturedMs, epoch, generation; };
static QueueHandle_t voiceAudioQueue, voiceEventQueue;
static bool voiceRunning = false;
static volatile uint32_t voiceFeedMs = 0, voiceFeedEpoch = 0, voiceGaps = 0;
// 0=normal, 1=recovery requested, 2=feeding silence to drain AFE/MultiNet context.
static volatile uint32_t voiceRecovering = 0, voiceGeneration = 0;
static volatile uint32_t voiceSilenceSamples = 0, voiceRecoveredMs = 0;
static volatile uint32_t voiceRecoveryStartedMs = 0;

static void requestVoiceRecovery() {
    uint32_t expected = 0;
    if (__atomic_compare_exchange_n(&voiceRecovering, &expected, 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        __atomic_add_fetch(&voiceGeneration, 1, __ATOMIC_SEQ_CST);
        __atomic_add_fetch(&voiceGaps, 1, __ATOMIC_RELAXED);
    }
}

static esp_err_t fillVoiceAudio(void*, void* out, size_t len, size_t* bytesRead, uint32_t) {
    // Only this callback consumes this queue. Capture remains the sole I2S owner.
    static AudioBlock pending = {};
    static size_t position = 0;
    static VoiceContinuity continuity;
    *bytesRead = 0;
    auto* destination = static_cast<uint8_t*>(out);
    while (*bytesRead < len) {
        if (__atomic_load_n(&voiceRecovering, __ATOMIC_SEQ_CST) == 1) {
            // Discard a partially assembled frame; do not splice a command over a gap.
            position = pending.frames;
            *bytesRead = 0;
            __atomic_store_n(&voiceSilenceSamples, 0, __ATOMIC_SEQ_CST);
            __atomic_store_n(&voiceRecoveryStartedMs, millis(), __ATOMIC_SEQ_CST);
            __atomic_store_n(&voiceRecovering, 2, __ATOMIC_SEQ_CST);
        }
        if (position >= pending.frames) {
            if (xQueueReceive(voiceAudioQueue, &pending, pdMS_TO_TICKS(100)) != pdTRUE) {
                // A delayed task is not proof that PCM was lost. Wait for a block;
                // its physical epoch and sequence establish actual continuity.
                continue;
            }
            position = 0;
            if (!continuity.check(pending.sequence, pending.captureEpoch)) requestVoiceRecovery();
        }
        const uint32_t epoch = __atomic_load_n(&captureEpoch, __ATOMIC_RELAXED);
        if (pending.captureEpoch != epoch || uint32_t(millis() - pending.capturedMs) > VOICE_AUDIO_MAX_AGE_MS) {
            position = pending.frames;
            requestVoiceRecovery();
            *bytesRead = 0;
            continue;
        }
        if (__atomic_load_n(&voiceRecovering, __ATOMIC_SEQ_CST) == 1) continue;
        const size_t available = (pending.frames - position) * sizeof(int16_t);
        const size_t count = available < len - *bytesRead ? available : len - *bytesRead;
        if (__atomic_load_n(&voiceRecovering, __ATOMIC_SEQ_CST)) {
            memset(destination + *bytesRead, 0, count);
            // Saturation avoids rollover during a prolonged microphone fault.
            const uint32_t samples = __atomic_load_n(&voiceSilenceSamples, __ATOMIC_SEQ_CST);
            if (samples < VOICE_RECOVERY_SAMPLES)
                __atomic_add_fetch(&voiceSilenceSamples, count / sizeof(int16_t), __ATOMIC_SEQ_CST);
        } else {
            memcpy(destination + *bytesRead, pending.pcm + position, count);
        }
        position += count / sizeof(int16_t);
        *bytesRead += count;
        __atomic_store_n(&voiceFeedMs, pending.capturedMs, __ATOMIC_RELAXED);
        __atomic_store_n(&voiceFeedEpoch, pending.captureEpoch, __ATOMIC_RELAXED);
    }
    return ESP_OK;
}

static void onVoiceEvent(void*, sr_event_t event, int commandId, int) {
    if (__atomic_load_n(&voiceRecovering, __ATOMIC_SEQ_CST) == 2 &&
        voiceRecoveryReady(__atomic_load_n(&voiceSilenceSamples, __ATOMIC_SEQ_CST), SAMPLE_RATE,
            uint32_t(millis() - __atomic_load_n(&voiceRecoveryStartedMs, __ATOMIC_SEQ_CST)),
            event == SR_EVENT_TIMEOUT)) {
        // Wait for an actual decoder timeout after at least six seconds of
        // synthetic silence, rather than simply accepting the next delayed event.
        __atomic_store_n(&voiceRecoveredMs, millis(), __ATOMIC_SEQ_CST);
        __atomic_store_n(&voiceRecovering, 0, __ATOMIC_SEQ_CST);
    }
    if (!__atomic_load_n(&voiceRecovering, __ATOMIC_SEQ_CST) &&
        uint32_t(millis() - __atomic_load_n(&voiceRecoveredMs, __ATOMIC_SEQ_CST)) >= 500 &&
        event == SR_EVENT_COMMAND && commandId >= 1 && commandId <= 3) {
        const VoiceEvent command = {
            static_cast<VoiceCommand>(commandId),
            __atomic_load_n(&voiceFeedMs, __ATOMIC_RELAXED),
            __atomic_load_n(&voiceFeedEpoch, __ATOMIC_RELAXED),
            __atomic_load_n(&voiceGeneration, __ATOMIC_SEQ_CST)
        };
        if (xQueueSend(voiceEventQueue, &command, 0) != pdTRUE) {
            requestVoiceRecovery();
            xQueueReset(voiceEventQueue);
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
    voiceAudioQueue = xQueueCreateWithCaps(32, sizeof(AudioBlock), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
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
