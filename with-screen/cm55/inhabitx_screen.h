/*
 * inhabitx_screen.h - InhabitX main with screen, screen core (CM55).
 *
 * Two hooks into Infineon's "Graphics LVGL demo" main.c (cm55_gfx_task):
 *   lv_demo_music();                      ->  inhabitx_screen_start();
 *   time_till_next = lv_timer_handler();  ->  time_till_next = inhabitx_screen_loop();
 * Everything else in Infineon's display start-up stays as it is.
 */
#ifndef INHABITX_SCREEN_H_
#define INHABITX_SCREEN_H_

#include <stdint.h>

/* After lv_init(), lv_port_disp_init() and lv_port_indev_init(). */
void inhabitx_screen_start(void);

/* Instead of lv_timer_handler(): draws the screen, runs the main, returns
 * the milliseconds until it wants to be called again. */
uint32_t inhabitx_screen_loop(void);

#endif /* INHABITX_SCREEN_H_ */
