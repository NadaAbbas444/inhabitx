/*
 * ix_ipc.h - InhabitX main with screen: link between the two E84 cores.
 *
 * The CM33 (Wi-Fi) only talks to the broker; the CM55 runs the screen and
 * the main. Messages go both ways through an IPC pipe, set up like
 * Infineon's "IPC Pipes" example (mtb-example-psoc-edge-ipc-pipes):
 *   CM33 -> CM55: every message received from the broker, and
 *                 "connected" / "disconnected"
 *   CM55 -> CM33: "publish this" (building state, replies)
 * Copy this file into both proj_cm33_ns/ and proj_cm55/.
 */
#ifndef IX_IPC_H_
#define IX_IPC_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cy_pdl.h"
#include "cy_ipc_pipe.h"

/* ---- pipe set-up: same values as Infineon's IPC Pipes example ----------- */
#define IX_IPC_MAX_ENDPOINTS        (5UL)
#define IX_IPC_CLIENT_CNT           (8UL)

#define IX_IPC_CHAN_EP1             (4UL)       /* CM33 endpoint */
#define IX_IPC_INTR_EP1             (4UL)
#define IX_IPC_CHAN_EP2             (15UL)      /* CM55 endpoint */
#define IX_IPC_INTR_EP2             (5UL)
#define IX_IPC_INTR_PRIO            (1UL)

#define IX_IPC_CHAN_MASK_EP1        CY_IPC_CH_MASK(IX_IPC_CHAN_EP1)
#define IX_IPC_CHAN_MASK_EP2        CY_IPC_CH_MASK(IX_IPC_CHAN_EP2)
#define IX_IPC_INTR_MASK_EP1        CY_IPC_INTR_MASK(IX_IPC_INTR_EP1)
#define IX_IPC_INTR_MASK_EP2        CY_IPC_INTR_MASK(IX_IPC_INTR_EP2)
#define IX_IPC_INTR_MASK            (IX_IPC_CHAN_MASK_EP1 | IX_IPC_CHAN_MASK_EP2)
#define IX_IPC_INTR_MUX_EP1         (CY_IPC0_INTR_MUX(IX_IPC_INTR_EP1))
#define IX_IPC_INTR_MUX_EP2         (CY_IPC0_INTR_MUX(IX_IPC_INTR_EP2))

#define IX_IPC_CM33_EP_ADDR         (1UL)
#define IX_IPC_CM33_CLIENT_ID       (3UL)
#define IX_IPC_CM55_EP_ADDR         (2UL)
#define IX_IPC_CM55_CLIENT_ID       (5UL)

/* ---- messages ------------------------------------------------------------ */
#define IX_IPC_TOPIC_MAX            96
#define IX_IPC_UP_MAX               1024    /* broker -> screen core: bookings, node states */
#define IX_IPC_DOWN_MAX             2560    /* screen core -> broker: building state (retained) */

typedef enum {
    IX_IPC_MSG = 1,         /* CM33 -> CM55: a message from the broker */
    IX_IPC_CONNECTED,       /* CM33 -> CM55: (re)connected and subscribed */
    IX_IPC_DISCONNECTED,    /* CM33 -> CM55: connection lost */
    IX_IPC_PUBLISH,         /* CM55 -> CM33: publish this */
} ix_ipc_kind_t;

/* The first word is the pipe header (client id, release mask), like
 * Infineon's ipc_msg_t. */
#define IX_IPC_HEADER \
    uint8_t  client_id;     \
    uint16_t intr_mask;     \
    uint8_t  kind;          \
    uint8_t  retain;        \
    uint16_t topic_len;     \
    uint16_t payload_len;   \
    char     topic[IX_IPC_TOPIC_MAX];

typedef struct { IX_IPC_HEADER char payload[IX_IPC_UP_MAX];   } ix_ipc_up_t;
typedef struct { IX_IPC_HEADER char payload[IX_IPC_DOWN_MAX]; } ix_ipc_down_t;

/* ---- CM33 side (ix_ipc_cm33.c) ------------------------------------------- */
/* Call in main() before Cy_SysEnableCM55(), before the scheduler starts. */
void ix_ipc_cm33_init(void);
/* Send one message to the screen core; waits up to ~200 ms if the
 * previous one hasn't been picked up yet. */
bool ix_ipc_cm33_send(ix_ipc_kind_t kind, const char *topic, size_t topic_len,
                      const char *payload, size_t payload_len);
/* Publish requests from the screen core arrive here (FreeRTOS queue of
 * ix_ipc_down_t), created by ix_ipc_cm33_init(). */
extern void *ix_ipc_cm33_down_q;

/* ---- CM55 side (ix_ipc_cm55.c) ------------------------------------------- */
/* Call once the scheduler runs (from the screen task). */
void ix_ipc_cm55_init(void);
bool ix_ipc_cm55_publish(const char *topic, const char *payload, size_t len, bool retain);
/* Messages from the CM33 arrive here (FreeRTOS queue of ix_ipc_up_t). */
extern void *ix_ipc_cm55_up_q;

#endif /* IX_IPC_H_ */
