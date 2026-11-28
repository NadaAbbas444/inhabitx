/* InhabitX classroom audio pipeline for the KIT_PSE84_EVAL_EPC2 BSP.
 * Included once by the active core's audio.c. No Arduino/ESP32 dependencies. */
#ifndef INHABITX_OCCUPANCY_AUDIO_H
#define INHABITX_OCCUPANCY_AUDIO_H
#include "cybsp.h"
#include "cy_pdl.h"
#include "audio.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "occupancy_filter.h"
#define AUDIO_MEMORY_BARRIER() __DMB()
#include "audio_queue.h"

#define OCC_SAMPLE_RATE 16000u
#define OCC_PDM_CHANNEL 3u
#define OCC_PDM_ERRORS (CY_PDM_PCM_INTR_RX_FIR_OVERFLOW | \
    CY_PDM_PCM_INTR_RX_OVERFLOW | CY_PDM_PCM_INTR_RX_IF_OVERFLOW | \
    CY_PDM_PCM_INTR_RX_UNDERFLOW)
#ifndef INHABITX_ROOM
#define INHABITX_ROOM "Z.1.09"
#endif
_Static_assert(IMAI_DATA_IN_COUNT == 1, "Model must take streaming mono PCM");
_Static_assert(IMAI_DATA_OUT_COUNT == 3, "Expected unlabeled/background/human_activity");

static audio_queue_t audio_queue;
static audio_block_t capture_block;
static uint32_t capture_used;
static volatile uint32_t wall_ms, stream_epoch, dropped_blocks, fifo_errors;
static volatile uint32_t last_capture_ms;
static occupancy_filter_t room_filter;
static uint32_t model_epoch, repeated_samples;
static uint64_t sample_count;
static uint32_t last_report_ms;
static int16_t last_pcm;
static bool have_last_pcm, have_scores;
static bool stuck_latched;
static int16_t stuck_value;
static float latest_scores[3], mic_dbfs;
static const char *health_reason = "warming_up";

void SysTick_Handler(void)
{
    ++wall_ms;
}

static void occupancy_stream_reset(const char *reason)
{
    IMAI_init();
    occupancy_reset(&room_filter);
    sample_count = 0u;
    repeated_samples = 0u;
    have_last_pcm = false;
    have_scores = false;
    memset(latest_scores, 0, sizeof(latest_scores));
    health_reason = reason;
}

static void occupancy_pdm_isr(void)
{
    const uint32_t status = Cy_PDM_PCM_Channel_GetInterruptStatusMasked(
        CYBSP_PDM_HW, OCC_PDM_CHANNEL);
    if (status & OCC_PDM_ERRORS) {
        ++fifo_errors;
        ++stream_epoch;
        capture_used = 0u;
        /* Flush only the currently buffered samples, with a bounded ISR. */
        const unsigned count = Cy_PDM_PCM_Channel_GetNumInFifo(
            CYBSP_PDM_HW, OCC_PDM_CHANNEL);
        for (unsigned i = 0; i < count; ++i)
            (void)Cy_PDM_PCM_Channel_ReadFifo(CYBSP_PDM_HW, OCC_PDM_CHANNEL);
        Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, OCC_PDM_CHANNEL, status);
        return;
    }
    if (status & CY_PDM_PCM_INTR_RX_TRIGGER) {
        const unsigned count = Cy_PDM_PCM_Channel_GetNumInFifo(
            CYBSP_PDM_HW, OCC_PDM_CHANNEL);
        for (unsigned i = 0; i < count; ++i) {
            capture_block.pcm[capture_used++] = (int16_t)
                Cy_PDM_PCM_Channel_ReadFifo(CYBSP_PDM_HW, OCC_PDM_CHANNEL);
            if (capture_used == AUDIO_FRAME_SAMPLES) {
                capture_block.epoch = stream_epoch;
                capture_block.captured_ms = wall_ms;
                last_capture_ms = wall_ms;
                if (!audio_queue_push(&audio_queue, &capture_block)) {
                    ++dropped_blocks;
                    ++stream_epoch;
                }
                capture_used = 0u;
            }
        }
        Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, OCC_PDM_CHANNEL,
                                         CY_PDM_PCM_INTR_RX_TRIGGER);
    }
}

cy_rslt_t pdm_init(void)
{
    const cy_stc_sysint_t irq = {
        .intrSrc = (IRQn_Type)CYBSP_PDM_CHANNEL_3_IRQ,
        .intrPriority = 2u
    };
    wall_ms = stream_epoch = dropped_blocks = fifo_errors = last_capture_ms = 0u;
    capture_used = 0u;
    memset(&audio_queue, 0, sizeof(audio_queue));
    model_epoch = 0u;
    stuck_latched = false;
    last_report_ms = 0u;
    mic_dbfs = -120.0f;
    occupancy_stream_reset("warming_up");
    if (channel_3_config.wordSize != CY_PDM_PCM_WSIZE_16_BIT ||
        !channel_3_config.signExtension || channel_3_config.rxFifoTriggerLevel != 31u ||
        channel_3_config.cic_decim_code != CY_PDM_PCM_CHAN_CIC_DECIM_32 ||
        channel_3_config.fir0_enable ||
        channel_3_config.fir0_decim_code != CY_PDM_PCM_CHAN_FIR0_DECIM_1 ||
        channel_3_config.fir1_decim_code != CY_PDM_PCM_CHAN_FIR1_DECIM_3 ||
        CYBSP_PDM_config.clkDiv != 7u ||
        CYBSP_PDM_config.clksel != CY_PDM_PCM_SEL_SRSS_CLOCK ||
        CYBSP_PDM_config.halverate != CY_PDM_PCM_RATE_FULL) {
        printf("FATAL: restore the BSP's 16 kHz signed PCM configuration. Room: UNKNOWN\r\n");
        return (cy_rslt_t)CY_PDM_PCM_BAD_PARAM;
    }
    cy_rslt_t result = (cy_rslt_t)Cy_PDM_PCM_Init(CYBSP_PDM_HW, &CYBSP_PDM_config);
    if (result != CY_PDM_PCM_SUCCESS) return result;
    Cy_PDM_PCM_Channel_Enable(CYBSP_PDM_HW, OCC_PDM_CHANNEL);
    result = (cy_rslt_t)Cy_PDM_PCM_Channel_Init(
        CYBSP_PDM_HW, &channel_3_config, OCC_PDM_CHANNEL);
    if (result != CY_PDM_PCM_SUCCESS) return result;
    /* This PDM gain is independent of the ESP32's ES7210 analog gain. */
    Cy_PDM_PCM_SetGain(CYBSP_PDM_HW, OCC_PDM_CHANNEL, CY_PDM_PCM_SEL_GAIN_5DB);
    Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, OCC_PDM_CHANNEL, CY_PDM_PCM_INTR_MASK);
    Cy_PDM_PCM_Channel_SetInterruptMask(CYBSP_PDM_HW, OCC_PDM_CHANNEL, CY_PDM_PCM_INTR_MASK);
    result = Cy_SysInt_Init(&irq, occupancy_pdm_isr);
    if (result != CY_SYSINT_SUCCESS) return result;
    if (SysTick_Config(SystemCoreClock / 1000u) != 0u) return (cy_rslt_t)CY_PDM_PCM_BAD_PARAM;
    NVIC_ClearPendingIRQ(irq.intrSrc);
    NVIC_EnableIRQ(irq.intrSrc);
    printf("InhabitX classroom occupancy | 16000 Hz mono | model e48a1b9d\r\n");
    printf("Room: UNKNOWN | warming up; silence does not prove an empty room.\r\n");
    Cy_PDM_PCM_Activate_Channel(CYBSP_PDM_HW, OCC_PDM_CHANNEL);
    return CY_RSLT_SUCCESS;
}

static void occupancy_report(void)
{
    const uint32_t now = wall_ms;
    if ((uint32_t)(now - last_report_ms) < 1000u) return;
    last_report_ms = now;
    /* A gap may have occurred during inference. Never emit its old result. */
    if (model_epoch != stream_epoch) {
        model_epoch = stream_epoch;
        occupancy_stream_reset("audio_gap");
    }
    printf("Room: %s | activity %.0f%% | background %.0f%% | mic %.0f dBFS"
           " | dropped %lu | fifo errors %lu | %s\r\n",
           occupancy_name(room_filter.state), (double)(latest_scores[2] * 100.0f),
           (double)(latest_scores[1] * 100.0f), (double)mic_dbfs,
           (unsigned long)dropped_blocks, (unsigned long)fifo_errors, health_reason);
    if (have_scores && room_filter.state != ROOM_UNKNOWN && model_epoch == stream_epoch &&
        (uint32_t)(now - last_capture_ms) <= 1000u) {
        /* Existing InhabitX node contract, emitted on UART for a host/gateway.
         * UNKNOWN is suppressed. This baseline has no Wi-Fi/MQTT stack. */
        printf("{\"room\":\"%s\",\"occupied\":%s,\"ts\":0}\r\n", INHABITX_ROOM,
               room_filter.state == ROOM_OCCUPIED ? "true" : "false");
    }
}

cy_rslt_t pdm_data_process(void)
{
    audio_block_t block;
    if (model_epoch != stream_epoch) {
        model_epoch = stream_epoch;
        occupancy_stream_reset("audio_gap");
    }
    if ((uint32_t)(wall_ms - last_capture_ms) > 1000u) {
        if (strcmp(health_reason, "microphone_timeout") != 0)
            occupancy_stream_reset("microphone_timeout");
        occupancy_report();
        return (cy_rslt_t)PDM_PCM_DATA_NOT_READY;
    }
    if (!audio_queue_pop(&audio_queue, &block)) {
        occupancy_report();
        return (cy_rslt_t)PDM_PCM_DATA_NOT_READY;
    }
    if (block.epoch != stream_epoch || (uint32_t)(wall_ms - block.captured_ms) > 1000u) {
        occupancy_stream_reset("stale_audio");
        occupancy_report();
        return CY_RSLT_SUCCESS;
    }
    unsigned clipped = 0u;
    float energy = 0.0f;
    bool unstuck = false;
    for (unsigned i = 0; i < AUDIO_FRAME_SAMPLES; ++i) {
        const float sample = block.pcm[i] / 32768.0f;
        energy += sample * sample;
        if (block.pcm[i] >= 32760 || block.pcm[i] <= -32760) ++clipped;
        if (block.pcm[i] != stuck_value) unstuck = true;
    }
    mic_dbfs = 10.0f * log10f(fmaxf(energy / AUDIO_FRAME_SAMPLES, 1.0e-12f));
    if (stuck_latched) {
        if (!unstuck) {
            occupancy_report();
            return CY_RSLT_SUCCESS;
        }
        stuck_latched = false;
        occupancy_stream_reset("warming_up");
    }
    if (clipped * 20u >= AUDIO_FRAME_SAMPLES) {
        occupancy_stream_reset("microphone_clipping");
        occupancy_report();
        return CY_RSLT_SUCCESS;
    }
    for (unsigned i = 0; i < AUDIO_FRAME_SAMPLES; ++i) {
        if (block.epoch != stream_epoch) {
            model_epoch = stream_epoch;
            occupancy_stream_reset("audio_gap");
            break;
        }
        const int16_t pcm = block.pcm[i];
        if (have_last_pcm && pcm == last_pcm) {
            if (repeated_samples < OCC_SAMPLE_RATE) ++repeated_samples;
        } else repeated_samples = 0u;
        have_last_pcm = true;
        last_pcm = pcm;
        if (repeated_samples >= OCC_SAMPLE_RATE) {
            stuck_latched = true;
            stuck_value = pcm;
            occupancy_stream_reset("microphone_stuck");
            break;
        }
        float input = pcm / 32768.0f;
        float scores[IMAI_DATA_OUT_COUNT];
        if (IMAI_enqueue(&input) != IMAI_RET_SUCCESS) {
            occupancy_stream_reset("model_enqueue_error");
            break;
        }
        ++sample_count;
        const int status = IMAI_dequeue(scores);
        if (status == IMAI_RET_SUCCESS) {
            bool valid = true;
            for (unsigned k = 0; k < IMAI_DATA_OUT_COUNT; ++k)
                if (!isfinite(scores[k]) || scores[k] < 0.0f || scores[k] > 1.0f) valid = false;
            if (!valid) {
                occupancy_stream_reset("invalid_model_output");
                break;
            }
            occupancy_update(&room_filter, scores,
                (uint32_t)((uint64_t)sample_count * 1000u / OCC_SAMPLE_RATE));
            memcpy(latest_scores, scores, sizeof(scores));
            have_scores = true;
            health_reason = "ok";
        } else if (status != IMAI_RET_NODATA) {
            occupancy_stream_reset("model_dequeue_error");
            break;
        }
    }
    occupancy_report();
    return CY_RSLT_SUCCESS;
}
#endif
