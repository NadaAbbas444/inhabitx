/*
 * ix_ipc_cm55.c - InhabitX main with screen: the screen core's end of the link.
 * Pipe set-up as in Infineon's IPC Pipes example (cm55_ipc_communication.c).
 * See ix_ipc.h.
 */
#include "ix_ipc.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <string.h>

#define UP_QUEUE_LEN        6
#define SEND_RETRIES        40          /* x 5 ms */

static cy_stc_ipc_pipe_ep_t s_ep_array[IX_IPC_MAX_ENDPOINTS];
static cy_ipc_pipe_callback_ptr_t s_cb_array[IX_IPC_CLIENT_CNT];

CY_SECTION_SHAREDMEM
static ix_ipc_down_t s_down;
static volatile bool s_down_busy;

void *ix_ipc_cm55_up_q;

static void pipe_isr(void)
{
    Cy_IPC_Pipe_ExecuteCallback(IX_IPC_CM55_EP_ADDR);
}

static void down_released(void)
{
    s_down_busy = false;
}

/* A message from the Wi-Fi core (interrupt): copy it into the queue that
 * the screen task reads, so the Wi-Fi core can reuse its buffer. */
static void on_up(uint32_t *msg)
{
    if (msg == NULL || ix_ipc_cm55_up_q == NULL)
    {
        return;
    }
    BaseType_t woken = pdFALSE;
    if (pdTRUE != xQueueSendFromISR((QueueHandle_t)ix_ipc_cm55_up_q, msg, &woken))
    {
        /* full: node states come again in 2 s; a lost booking request
         * makes the web page say "didn't answer" after 8 s */
    }
    portYIELD_FROM_ISR(woken);
}

void ix_ipc_cm55_init(void)
{
    static const cy_stc_ipc_pipe_config_t config =
    {
        {   /* this core */
            .ipcNotifierNumber    = IX_IPC_INTR_EP2,
            .ipcNotifierPriority  = IX_IPC_INTR_PRIO,
            .ipcNotifierMuxNumber = IX_IPC_INTR_MUX_EP2,
            .epAddress            = IX_IPC_CM55_EP_ADDR,
            {
                .epChannel        = IX_IPC_CHAN_EP2,
                .epIntr           = IX_IPC_INTR_EP2,
                .epIntrmask       = IX_IPC_INTR_MASK
            }
        },
        {   /* the Wi-Fi core */
            .ipcNotifierNumber    = IX_IPC_INTR_EP1,
            .ipcNotifierPriority  = IX_IPC_INTR_PRIO,
            .ipcNotifierMuxNumber = IX_IPC_INTR_MUX_EP1,
            .epAddress            = IX_IPC_CM33_EP_ADDR,
            {
                .epChannel        = IX_IPC_CHAN_EP1,
                .epIntr           = IX_IPC_INTR_EP1,
                .epIntrmask       = IX_IPC_INTR_MASK
            }
        },
        .endpointClientsCount     = IX_IPC_CLIENT_CNT,
        .endpointsCallbacksArray  = s_cb_array,
        .userPipeIsrHandler       = &pipe_isr
    };

    ix_ipc_cm55_up_q = xQueueCreate(UP_QUEUE_LEN, sizeof(ix_ipc_up_t));
    Cy_IPC_Pipe_Config(s_ep_array);
    Cy_IPC_Pipe_Init(&config);
    Cy_IPC_Pipe_RegisterCallback(IX_IPC_CM55_EP_ADDR, &on_up, IX_IPC_CM55_CLIENT_ID);
}

bool ix_ipc_cm55_publish(const char *topic, const char *payload, size_t len, bool retain)
{
    size_t tl = strlen(topic);
    if (tl >= IX_IPC_TOPIC_MAX || len > IX_IPC_DOWN_MAX)
    {
        return false;
    }
    for (int i = 0; s_down_busy; i++)
    {
        if (i >= SEND_RETRIES)
        {
            return false;               /* tried again on the next poll */
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    s_down.client_id   = IX_IPC_CM33_CLIENT_ID;
    s_down.intr_mask   = IX_IPC_INTR_MASK_EP2;
    s_down.kind        = (uint8_t)IX_IPC_PUBLISH;
    s_down.retain      = retain ? 1 : 0;
    s_down.topic_len   = (uint16_t)tl;
    s_down.payload_len = (uint16_t)len;
    memcpy(s_down.topic, topic, tl);
    memcpy(s_down.payload, payload, len);

    for (int i = 0; i < SEND_RETRIES; i++)
    {
        s_down_busy = true;
        if (CY_IPC_PIPE_SUCCESS == Cy_IPC_Pipe_SendMessage(IX_IPC_CM33_EP_ADDR, IX_IPC_CM55_EP_ADDR,
                                                           (void *)&s_down, &down_released))
        {
            return true;
        }
        s_down_busy = false;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return false;
}
