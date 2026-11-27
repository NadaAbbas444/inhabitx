/*
 * lv_conf.h - LVGL 9.2.2 settings for the InhabitX main on the XIAO ESP32-S3.
 *
 * Arduino looks for this file NEXT TO the lvgl library folder:
 *   <sketchbook>/libraries/lv_conf.h      (e.g. ~/Arduino/libraries/lv_conf.h)
 * Anything not set here uses LVGL's default.
 */
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH          16
#define LV_DEF_REFR_PERIOD      33          /* 30 fps is plenty for a screen sent over Wi-Fi */
#define LV_USE_OS               LV_OS_NONE  /* everything runs in loop() */

/* LVGL's memory: 2 MB in PSRAM (measured peak on the PC: ~300 KB) */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_MEM_SIZE             (2 * 1024 * 1024)
#define LV_MEM_POOL_INCLUDE     <esp32-hal-psram.h>
#define LV_MEM_POOL_ALLOC       ps_malloc

#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF           1

/* Text uses Roboto via TinyTTF (ui_fonts.c); Montserrat only supplies icons */
#define LV_USE_TINY_TTF         1
#define LV_TINY_TTF_FILE_SUPPORT 0
#define LV_TINY_TTF_CACHE_GLYPH_CNT 512
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_32   1
#define LV_FONT_DEFAULT         &lv_font_montserrat_16

/* not needed: keep the build small */
#define LV_BUILD_EXAMPLES       0
#define LV_USE_DEMO_WIDGETS     0

#endif /* LV_CONF_H */
