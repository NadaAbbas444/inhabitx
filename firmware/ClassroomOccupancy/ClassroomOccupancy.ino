/* Waveshare ESP32-S3-AUDIO-Board + DEEPCRAFT streaming export.
 * Arduino-ESP32 3.3.12. See README.txt before uploading.
 * No display, SD, speaker or external wiring required. Voice needs ESP SR 16M partition.
 */
#include <Arduino.h>
#include <math.h>
#include "driver/i2c.h"
#include "driver/i2s_tdm.h"
#include "esp_timer.h"
#include "src/es7210/es7210.h"
#include "ModelBridge.h"
#include "OccupancyFilter.h"
#include "SerialConfig.h"

constexpr int PIN_SDA = 11, PIN_SCL = 10;
constexpr int PIN_MCLK = 12, PIN_BCLK = 13, PIN_WS = 14, PIN_MIC_DATA = 15;
constexpr uint32_t SAMPLE_RATE = 16000;
constexpr size_t BLOCK_FRAMES = 256, TDM_SLOTS = 4, QUEUE_BLOCKS = 32;
// MIC1 is the first TDM slot. Other slots include a speaker reference; do not average all four.
constexpr size_t MIC_SLOT = 0;
constexpr es7210_mic_gain_t MIC_GAIN = ES7210_MIC_GAIN_24DB;
constexpr float INPUT_GAIN = 1.0f; // Keep fixed; calibrate against training recordings.

struct AudioBlock {
    uint32_t sequence;
    uint32_t epoch;
    uint32_t capturedMs;
    uint16_t frames;
    float rms[TDM_SLOTS];
    float clipFraction;
    int16_t pcm[BLOCK_FRAMES];
};
struct Result {
    float p[3];
    float rms[4];
    float clipFraction;
    uint32_t modelUs, queueDepth, capturedMs, epoch;
    RoomState state;
};
static QueueHandle_t audioQueue, resultQueue;
static i2s_chan_handle_t rxChannel;
static es7210_dev_handle_t adc;
static volatile uint32_t streamEpoch = 0, droppedBlocks = 0, dmaOverflows = 0, readErrors = 0;
#include "VoiceRecognizer.h"
#include "InhabitXNetwork.h"

static void must(esp_err_t error, const char* operation) {
    if (error == ESP_OK) return;
    Serial.printf("FATAL: %s: %s. Occupancy UNKNOWN.\n", operation, esp_err_to_name(error));
    for (;;) delay(1000);
}

static bool IRAM_ATTR onOverflow(i2s_chan_handle_t, i2s_event_data_t*, void*) {
    __atomic_add_fetch(&dmaOverflows, 1, __ATOMIC_RELAXED);
    __atomic_add_fetch(&streamEpoch, 1, __ATOMIC_RELAXED);
    return false;
}

static void initMicrophone() {
    // Do not also start Wire: the bundled codec uses the legacy ESP-IDF I2C driver.
    i2c_config_t bus = {};
    bus.mode = I2C_MODE_MASTER;
    bus.sda_io_num = PIN_SDA; bus.scl_io_num = PIN_SCL;
    bus.sda_pullup_en = GPIO_PULLUP_ENABLE; bus.scl_pullup_en = GPIO_PULLUP_ENABLE;
    bus.master.clk_speed = 100000;
    must(i2c_param_config(I2C_NUM_0, &bus), "I2C parameters");
    must(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0), "I2C install");
    // Explicitly probe ADC; wrong board/pins fail visibly rather than declaring a room empty.
    i2c_cmd_handle_t probe = i2c_cmd_link_create();
    if (!probe) must(ESP_ERR_NO_MEM, "I2C probe allocation");
    i2c_master_start(probe);
    i2c_master_write_byte(probe, ES7210_ADDRRES_00 << 1, true);
    i2c_master_stop(probe);
    const esp_err_t found = i2c_master_cmd_begin(I2C_NUM_0, probe, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(probe);
    must(found, "ES7210 at 0x40 (SDA11/SCL10)");

    i2s_chan_config_t channel = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    channel.dma_desc_num = 12;
    channel.dma_frame_num = BLOCK_FRAMES;
    must(i2s_new_channel(&channel, nullptr, &rxChannel), "I2S RX allocation");
    i2s_tdm_config_t config = {};
    config.clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(SAMPLE_RATE);
    config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
    config.slot_cfg = I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO,
        (i2s_tdm_slot_mask_t)(I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3));
    config.slot_cfg.total_slot = TDM_SLOTS;
    config.gpio_cfg.mclk = (gpio_num_t)PIN_MCLK;
    config.gpio_cfg.bclk = (gpio_num_t)PIN_BCLK;
    config.gpio_cfg.ws = (gpio_num_t)PIN_WS;
    config.gpio_cfg.dout = I2S_GPIO_UNUSED;
    config.gpio_cfg.din = (gpio_num_t)PIN_MIC_DATA;
    must(i2s_channel_init_tdm_mode(rxChannel, &config), "I2S TDM mode");
    i2s_event_callbacks_t callbacks = {};
    callbacks.on_recv_q_ovf = onOverflow;
    must(i2s_channel_register_event_callback(rxChannel, &callbacks, nullptr), "I2S overflow callback");
    // Enable clocks before configuring the external ADC.
    must(i2s_channel_enable(rxChannel), "I2S enable");
    es7210_i2c_config_t adcBus = {I2C_NUM_0, ES7210_ADDRRES_00};
    must(es7210_new_codec(&adcBus, &adc), "ES7210 create");
    es7210_codec_config_t codec = {};
    codec.sample_rate_hz = SAMPLE_RATE; codec.mclk_ratio = 256;
    codec.i2s_format = ES7210_I2S_FMT_I2S; codec.bit_width = ES7210_I2S_BITS_16B;
    codec.mic_bias = ES7210_MIC_BIAS_2V87; codec.mic_gain = MIC_GAIN;
    codec.flags.tdm_enable = true;
    must(es7210_config_codec(adc, &codec), "ES7210 configure");
    must(es7210_config_volume(adc, 0), "ES7210 digital gain 0dB");
    // Start with a fresh DMA stream after analog settling; no stale startup samples.
    delay(200);
    must(i2s_channel_disable(rxChannel), "I2S stop after settling");
    __atomic_store_n(&streamEpoch, 0, __ATOMIC_RELAXED);
    must(i2s_channel_enable(rxChannel), "I2S restart");
}

static void captureTask(void*) {
    int16_t raw[BLOCK_FRAMES * TDM_SLOTS];
    AudioBlock block = {};
    uint32_t sequence = 0;
    for (;;) {
        size_t bytes = 0;
        uint32_t before = __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED);
        const esp_err_t error = i2s_channel_read(rxChannel, raw, sizeof(raw), &bytes, 1000);
        if (error != ESP_OK || bytes == 0 || bytes % (TDM_SLOTS * sizeof(int16_t))) {
            __atomic_add_fetch(&readErrors, 1, __ATOMIC_RELAXED);
            __atomic_add_fetch(&streamEpoch, 1, __ATOMIC_RELAXED);
            vTaskDelay(1); continue;
        }
        if (before != __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED)) continue;
        block.sequence = sequence++;
        block.epoch = before;
        block.capturedMs = millis();
        block.frames = bytes / (TDM_SLOTS * sizeof(int16_t));
        double energy[TDM_SLOTS] = {};
        unsigned clipped = 0;
        for (unsigned i = 0; i < block.frames; ++i) {
            for (unsigned ch = 0; ch < TDM_SLOTS; ++ch) {
                float sample = raw[i * TDM_SLOTS + ch] / 32768.0f;
                energy[ch] += sample * sample;
            }
            block.pcm[i] = raw[i * TDM_SLOTS + MIC_SLOT];
            if (block.pcm[i] >= 32760 || block.pcm[i] <= -32760) ++clipped;
        }
        for (unsigned ch = 0; ch < TDM_SLOTS; ++ch) block.rms[ch] = sqrt(energy[ch] / block.frames);
        block.clipFraction = (float)clipped / block.frames;
        if (voiceRunning && !__atomic_load_n(&voiceFault, __ATOMIC_RELAXED) &&
            xQueueSend(voiceAudioQueue, &block, 0) != pdTRUE) {
            __atomic_store_n(&voiceFault, 1, __ATOMIC_RELAXED);
        }
        if (xQueueSend(audioQueue, &block, 0) != pdTRUE) {
            __atomic_add_fetch(&droppedBlocks, 1, __ATOMIC_RELAXED);
            __atomic_add_fetch(&streamEpoch, 1, __ATOMIC_RELAXED);
        }
    }
}

static void inferenceTask(void*) {
    AudioBlock block;
    OccupancyFilter occupancy;
    uint32_t epoch = UINT32_MAX, expected = 0, samples = 0, stuckSamples = 0;
    int16_t lastPcm = 0;
    uint32_t worstBlockUs = 0;
    float scores[IMAI_DATA_OUT_COUNT];
    IMAI_init();
    for (;;) {
        if (xQueueReceive(audioQueue, &block, pdMS_TO_TICKS(1000)) != pdTRUE) continue;
        // Discard old queued blocks after an overflow instead of reporting delayed occupancy.
        if (block.epoch != __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED)) continue;
        if (block.epoch != epoch || block.sequence != expected) {
            IMAI_init(); occupancy.reset(); samples = 0; stuckSamples = 0;
            epoch = block.epoch;
        }
        expected = block.sequence + 1;
        const int64_t start = esp_timer_get_time();
        for (unsigned i = 0; i < block.frames; ++i) {
            ++samples;
            if (block.pcm[i] == lastPcm) ++stuckSamples; else stuckSamples = 0;
            lastPcm = block.pcm[i];
            float input = (block.pcm[i] / 32768.0f) * INPUT_GAIN;
            const int enqueued = IMAI_enqueue(&input);
            const int dequeued = enqueued == IMAI_RET_SUCCESS ? IMAI_dequeue(scores) : IMAI_RET_NOMEM;
            if (enqueued != IMAI_RET_SUCCESS || (dequeued != IMAI_RET_SUCCESS && dequeued != IMAI_RET_NODATA)) {
                __atomic_add_fetch(&streamEpoch, 1, __ATOMIC_RELAXED); break;
            }
            if (dequeued == IMAI_RET_SUCCESS) {
                const uint32_t audioMs = (uint32_t)((uint64_t)samples * 1000 / SAMPLE_RATE);
                const bool healthy = stuckSamples < SAMPLE_RATE && block.clipFraction < 0.05f;
                if (healthy) occupancy.update(scores, audioMs); else occupancy.reset();
                Result result = {};
                memcpy(result.p, scores, sizeof(scores)); memcpy(result.rms, block.rms, sizeof(result.rms));
                result.clipFraction = block.clipFraction;
                result.modelUs = (uint32_t)(esp_timer_get_time() - start);
                if (worstBlockUs > result.modelUs) result.modelUs = worstBlockUs;
                result.queueDepth = uxQueueMessagesWaiting(audioQueue);
                result.capturedMs = block.capturedMs; result.state = occupancy.state;
                result.epoch = block.epoch;
                xQueueOverwrite(resultQueue, &result);
                worstBlockUs = 0;
            }
        }
        uint32_t elapsed = (uint32_t)(esp_timer_get_time() - start);
        if (elapsed > worstBlockUs) worstBlockUs = elapsed;
        // Let the core's idle task run even when inference has created a backlog.
        vTaskDelay(1);
    }
}

void setup() {
    Serial.begin(115200);
    const uint32_t started = millis();
    while (!Serial && millis() - started < 2000) delay(10);
    Serial.println("ClassroomOccupancy | starting microphone and model...");
    if (SERIAL_VERBOSE)
        Serial.println("Model: 16kHz mono, 1s context; 0=unlabeled 1=background 2=human_activity");
    audioQueue = xQueueCreate(QUEUE_BLOCKS, sizeof(AudioBlock));
    resultQueue = xQueueCreate(1, sizeof(Result));
    nodeQueue = xQueueCreate(1, sizeof(NodeSnapshot));
    if (!audioQueue || !resultQueue || !nodeQueue) must(ESP_ERR_NO_MEM, "queues");
    initMicrophone();
    initVoiceRecognition();
    if (xTaskCreatePinnedToCore(captureTask, "mic-capture", 8192, nullptr, 5, nullptr, 0) != pdPASS)
        must(ESP_ERR_NO_MEM, "capture task");
    if (xTaskCreatePinnedToCore(inferenceTask, "model", 16384, nullptr, 3, nullptr, 1) != pdPASS)
        must(ESP_ERR_NO_MEM, "inference task");
    if (xTaskCreatePinnedToCore(networkTask, "inhabitx-mqtt", 12288, nullptr, 1, nullptr, 0) != pdPASS)
        must(ESP_ERR_NO_MEM, "network task");
}

void loop() {
    static uint32_t lastReport = millis();
    static Result latest = {};
    static bool haveResult = false, havePrinted = false;
    static RoomState printedState = RoomState::Unknown;
    static VoicePolicy voicePolicy;
    static bool wasListening = false, wasManual = false, voiceFaultPrinted = false;
    static uint32_t policyEpoch = 0;
    static uint32_t printedDrops = 0, printedOverflows = 0, printedErrors = 0;
    Result result;
    if (xQueueReceive(resultQueue, &result, pdMS_TO_TICKS(100)) == pdTRUE) {
        latest = result;
        haveResult = true;
        const bool stale = millis() - result.capturedMs > 2000;
        if (SERIAL_VERBOSE) Serial.printf("{\"state\":\"%s\",\"human_activity\":%.4f,\"background\":%.4f,\"unlabeled\":%.4f,"
                      "\"mic_rms_dbfs\":%.1f,\"slot_rms\":[%.5f,%.5f,%.5f,%.5f],\"clipped\":%.4f,"
                      "\"max_block_us\":%lu,\"queued_blocks\":%lu,\"dropped\":%lu,\"dma_overflows\":%lu,\"read_errors\":%lu}\n",
                      stale ? "UNKNOWN" : stateName(result.state), result.p[2], result.p[1], result.p[0],
                      20.0f * log10f(fmaxf(result.rms[MIC_SLOT], 1e-9f)),
                      result.rms[0], result.rms[1], result.rms[2], result.rms[3], result.clipFraction,
                      (unsigned long)result.modelUs, (unsigned long)result.queueDepth,
                      (unsigned long)__atomic_load_n(&droppedBlocks, __ATOMIC_RELAXED),
                      (unsigned long)__atomic_load_n(&dmaOverflows, __ATOMIC_RELAXED),
                      (unsigned long)__atomic_load_n(&readErrors, __ATOMIC_RELAXED));
    }
    const uint32_t now = millis();
    const uint32_t currentEpoch = __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED);
    if (policyEpoch != currentEpoch || __atomic_load_n(&voiceFault, __ATOMIC_RELAXED)) {
        voicePolicy.cancelListening();
        policyEpoch = currentEpoch;
    }
    if (voiceRunning && __atomic_load_n(&voiceFault, __ATOMIC_RELAXED) && !voiceFaultPrinted) {
        Serial.println("Voice: audio gap detected; voice disabled until reboot. Automatic sensing continues.");
        voiceFaultPrinted = true;
    }
    VoiceEvent event;
    while (voiceEventQueue && xQueueReceive(voiceEventQueue, &event, 0) == pdTRUE) {
        if (__atomic_load_n(&voiceFault, __ATOMIC_RELAXED) || event.epoch != currentEpoch ||
            uint32_t(now - event.capturedMs) > VOICE_AUDIO_MAX_AGE_MS) continue;
        if (voicePolicy.command(event.command, now)) {
            Serial.println(event.command == VoiceCommand::Arrive ?
                "Voice: room occupied confirmed by user" : "Voice: room marked empty for 10 minutes");
        } else if (event.command == VoiceCommand::Wake) {
            Serial.println("Voice: listening (8 seconds)...");
        }
    }
    const bool listening = voicePolicy.listening(now);
    if (wasListening && !listening && !voicePolicy.active(now)) Serial.println("Voice: command window closed");
    wasListening = listening;
    const bool manual = voicePolicy.active(now);
    if (wasManual && !manual) Serial.println("Voice: override expired; automatic detection resumed");
    wasManual = manual;
    const bool fresh = haveResult && latest.epoch == currentEpoch && uint32_t(now - latest.capturedMs) <= 2000;
    const RoomState state = fresh ? voicePolicy.state(latest.state, now) : RoomState::Unknown;
    const NodeSnapshot snapshot = {state, latest.capturedMs, latest.epoch, manual, manual ? voicePolicy.eventId() : 0};
    xQueueOverwrite(nodeQueue, &snapshot);
    if (!havePrinted || state != printedState || uint32_t(now - lastReport) >= SERIAL_STATUS_EVERY_MS) {
        const char* label = state == RoomState::Occupied ? "OCCUPIED" :
                            state == RoomState::Unoccupied ? (manual ? "EMPTY" : "EMPTY (inferred)") : "UNKNOWN";
        if (fresh) {
            Serial.printf("Room: %s%s | activity %.0f%% | mic %.0f dBFS\n", label,
                          manual ? " (voice override)" : "",
                          latest.p[2] * 100.0f,
                          20.0f * log10f(fmaxf(latest.rms[MIC_SLOT], 1e-9f)));
        } else {
            Serial.printf("Room: %s | %s\n", label,
                          haveResult ? "waiting for fresh audio" : "warming up");
        }
        const uint32_t drops = __atomic_load_n(&droppedBlocks, __ATOMIC_RELAXED);
        const uint32_t overflows = __atomic_load_n(&dmaOverflows, __ATOMIC_RELAXED);
        const uint32_t errors = __atomic_load_n(&readErrors, __ATOMIC_RELAXED);
        if (drops != printedDrops || overflows != printedOverflows || errors != printedErrors) {
            Serial.printf("Audio warnings (total): dropped %lu | overflow %lu | read errors %lu\n",
                          (unsigned long)drops, (unsigned long)overflows, (unsigned long)errors);
            printedDrops = drops;
            printedOverflows = overflows;
            printedErrors = errors;
        }
        printedState = state;
        havePrinted = true;
        lastReport = now;
    }
}
