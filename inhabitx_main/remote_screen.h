/*
 * remote_screen.h - the laptop as the main's screen.
 *
 * The XIAO draws the UI into memory with LVGL and sends every changed
 * rectangle to a browser page it serves itself (http://<board>/). Mouse
 * clicks on that page come back as touches.
 *
 * Messages on the WebSocket (port 81), little-endian:
 *   board -> browser, binary:
 *     'H' w:u16 h:u16                         screen size (sent on connect)
 *     'R' x:u16 y:u16 w:u16 h:u16 runs...     rectangle, RGB565, run-length:
 *                                             each run = count:u16 colour:u16
 *   browser -> board, text:
 *     "d x y" press, "m x y" move while pressed, "u x y" release
 */
#ifndef REMOTE_SCREEN_H_
#define REMOTE_SCREEN_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Run-length encodes w*h RGB565 pixels into out as count:u16 colour:u16
 * pairs. Returns the bytes written, or 0 if out (n bytes) is too small.
 * Worst case: 2 * w * h * 2 bytes. Plain C so it can be tested on a PC. */
size_t remote_screen_rle(const uint16_t *px, uint32_t w, uint32_t h, uint8_t *out, size_t n);

#ifdef __cplusplus
}

/* Arduino side (remote_screen.cpp) */
void remote_screen_begin(uint16_t w, uint16_t h);      /* HTTP on 80, WebSocket on 81 */
void remote_screen_loop(void);                          /* call often from loop() */
int  remote_screen_viewers(void);
#endif

#endif /* REMOTE_SCREEN_H_ */
