/*
 * inhabitx_screen.c - InhabitX main with screen, screen core (CM55).
 *
 * Runs the main (main_app: rooms, bookings, found items) and its touch
 * screen UI (ui_init) in Infineon's LVGL task. The UI and the main share
 * the same data and both only ever run in this one task. The broker is
 * reached through the Wi-Fi core (ix_ipc).
 */
#include "inhabitx_screen.h"

#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ui.h"
#include "main_app.h"
#include "ix_ipc.h"

#include <stdio.h>
#include <string.h>

/* The UI is laid out for UI_HOR_RES x UI_VER_RES (set in proj_cm55/Makefile);
 * Infineon's display set-up uses MY_DISP_HOR_RES x MY_DISP_VER_RES. */
#if __has_include("lv_port_disp.h")
#include "lv_port_disp.h"
#if (UI_HOR_RES != MY_DISP_HOR_RES) || (UI_VER_RES != MY_DISP_VER_RES)
#error "UI_HOR_RES/UI_VER_RES in proj_cm55/Makefile don't match the display: 4.3-inch = 832 480, 7-inch and 10.1-inch = 1024 600"
#endif
#endif

#define POLL_EVERY_MS   500u
#define MAX_WAIT_MS     20u     /* check for broker messages at least this often */

static bool publish(const char *topic, const char *payload, size_t len, bool retain)
{
    return ix_ipc_cm55_publish(topic, payload, len, retain);
}

void inhabitx_screen_start(void)
{
    main_app_init();
    ix_ipc_cm55_init();
    ui_init();
    printf("main: screen %dx%d up, waiting for the Wi-Fi core\n", UI_HOR_RES, UI_VER_RES);
}

uint32_t inhabitx_screen_loop(void)
{
    static ix_ipc_up_t m;
    static TickType_t next_poll;

    uint32_t wait = lv_timer_handler();

    while (ix_ipc_cm55_up_q != NULL && pdTRUE == xQueueReceive((QueueHandle_t)ix_ipc_cm55_up_q, &m, 0))
    {
        switch (m.kind)
        {
        case IX_IPC_MSG:
            main_app_on_message(m.topic, m.topic_len, m.payload, m.payload_len, publish);
            memset(m.payload, 0, m.payload_len);    /* may have held a name and an email */
            break;
        case IX_IPC_CONNECTED:
            printf("main: broker connected\n");
            main_app_connected();
            break;
        case IX_IPC_DISCONNECTED:
            printf("main: broker connection lost, the Wi-Fi core reconnects\n");
            break;
        default:
            break;
        }
    }

    if ((int32_t)(xTaskGetTickCount() - next_poll) >= 0)
    {
        next_poll = xTaskGetTickCount() + pdMS_TO_TICKS(POLL_EVERY_MS);
        main_app_poll(publish);
    }
    return wait < MAX_WAIT_MS ? wait : MAX_WAIT_MS;
}
