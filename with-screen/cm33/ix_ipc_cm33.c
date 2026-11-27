/*
 * ix_ipc_cm33.c - InhabitX main with screen: the Wi-Fi core's end of the link.
 * Pipe set-up as in Infineon's IPC Pipes example (cm33_ipc_communication.c).
 * See ix_ipc.h.
 */
#include "ix_ipc.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <string.h>

#define DOWN_QUEUE_LEN      3
#define SEND_RETRIES        40          /* x 5 ms */

static cy_stc_ipc_pipe_ep_t s_ep_array[IX_IPC_MAX_ENDPOINTS];
static cy_ipc_pipe_callback_ptr_t s_cb_array[IX_IPC_CLIENT_CNT];

CY_SECTION_SHAREDMEM
static uint32_t s_sema_array[CY_IPC_SEMA_COUNT / CY_IPC_SEMA_PER_WORD];

/* What goes to the screen core lives in shared memory; the pipe only
 * carries its address. */
CY_SECTION_SHAREDMEM
static ix_ipc_up_t s_up;
static volatile bool s_up_busy;

void *ix_ipc_cm33_down_q;

static void pipe_isr(void)
{
    Cy_IPC_Pipe_ExecuteCallback(IX_IPC_CM33_EP_ADDR);
}

/* The screen core picked up our last message: clear it (a booking
 * request holds a name and an email) and free the buffer. */
static void up_released(void)
{
    memset(s_up.payload, 0, s_up.payload_len);
    s_up_busy = false;
}

/* A publish request from the screen core (interrupt): copy it out so the
 * screen core can reuse its buffer, the MQTT side publishes it later. */
static void on_down(uint32_t *msg)
{
    if (msg == NULL || ix_ipc_cm33_down_q == NULL)
    {
        return;
    }
    BaseType_t woken = pdFALSE;
    xQueueSendFromISR((QueueHandle_t)ix_ipc_cm33_down_q, msg, &woken);   /* full: dropped, sent again on the next change */
    portYIELD_FROM_ISR(woken);
}

void ix_ipc_cm33_init(void)
{
    static const cy_stc_ipc_pipe_config_t config =
    {
        {   /* this core */
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
        {   /* the screen core */
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
        .endpointClientsCount     = IX_IPC_CLIENT_CNT,
        .endpointsCallbacksArray  = s_cb_array,
        .userPipeIsrHandler       = &pipe_isr
    };

    ix_ipc_cm33_down_q = xQueueCreate(DOWN_QUEUE_LEN, sizeof(ix_ipc_down_t));
    Cy_IPC_Sema_Init(IPC0_SEMA_CH_NUM, CY_IPC_SEMA_COUNT, s_sema_array);
    Cy_IPC_Pipe_Config(s_ep_array);
    Cy_IPC_Pipe_Init(&config);
    Cy_IPC_Pipe_RegisterCallback(IX_IPC_CM33_EP_ADDR, &on_down, IX_IPC_CM33_CLIENT_ID);
}

bool ix_ipc_cm33_send(ix_ipc_kind_t kind, const char *topic, size_t topic_len,
                      const char *payload, size_t payload_len)
{
    if (topic_len >= IX_IPC_TOPIC_MAX || payload_len > IX_IPC_UP_MAX)
    {
        return false;
    }
    for (int i = 0; s_up_busy; i++)
    {
        if (i >= SEND_RETRIES)
        {
            return false;               /* screen core not there (yet) */
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    s_up.client_id   = IX_IPC_CM55_CLIENT_ID;
    s_up.intr_mask   = IX_IPC_INTR_MASK_EP1;
    s_up.kind        = (uint8_t)kind;
    s_up.retain      = 0;
    s_up.topic_len   = (uint16_t)topic_len;
    s_up.payload_len = (uint16_t)payload_len;
    if (topic_len)   memcpy(s_up.topic, topic, topic_len);
    if (payload_len) memcpy(s_up.payload, payload, payload_len);

    for (int i = 0; i < SEND_RETRIES; i++)
    {
        s_up_busy = true;
        if (CY_IPC_PIPE_SUCCESS == Cy_IPC_Pipe_SendMessage(IX_IPC_CM55_EP_ADDR, IX_IPC_CM33_EP_ADDR,
                                                           (void *)&s_up, &up_released))
        {
            return true;
        }
        s_up_busy = false;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    memset(s_up.payload, 0, payload_len);
    return false;
}
