/*
 * InhabitX main: screen UI (LVGL 9, 800x480 touch).
 *
 * Portable: the same files build in the PC simulator and on the E84.
 * The platform provides ix_now() and calls ui_init() once after LVGL and the
 * display are up, then keeps calling lv_timer_handler() as usual.
 */
#ifndef UI_H
#define UI_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Default 1024x600: Waveshare 7" DSI LCD (C) or the 10.1" panel, both
 * supported on the E84 eval kit over MIPI-DSI. Build with 800x480 for the
 * kit's own 4.3" panel; the layout scales. */
#ifndef UI_HOR_RES
#define UI_HOR_RES 1024
#endif
#ifndef UI_VER_RES
#define UI_VER_RES 600
#endif

/* Building shown in the top bar */
#ifndef UI_BUILDING_NAME
#define UI_BUILDING_NAME "Building Z"
#endif

/* Where found items are handed in (shown after "I found something") */
#ifndef UI_LOSTFOUND_DESK
#define UI_LOSTFOUND_DESK "the Lost & Found desk at the main entrance"
#endif

/* Back to the room map and forget typed names/emails after this long idle */
#ifndef UI_IDLE_RESET_MS
#define UI_IDLE_RESET_MS 60000
#endif

/* Provided by the platform: current Unix time (local clock set via SNTP/RTC) */
time_t ix_now(void);

void ui_init(void);

#ifdef __cplusplus
}
#endif
#endif
