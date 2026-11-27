/* LVGL config for the PC simulator. Anything not set here uses LVGL's default.
 * Keep the colour depth and the font list in sync with the board's lv_conf.h. */
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH          16          /* the E84 kit panel is RGB565 */
#define LV_MEM_SIZE             (2 * 1024 * 1024)  /* fonts are rasterised at runtime; keep the same on the board */
#define LV_DEF_REFR_PERIOD      16
#define LV_USE_OS               LV_OS_NONE

#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF           0

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

/* PC only */
#define LV_USE_X11              1
#define LV_X11_DIRECT_EXIT      1
#define LV_X11_DOUBLE_BUFFER    1
#define LV_X11_RENDER_MODE_PARTIAL 1
#define LV_X11_RENDER_MODE_DIRECT  0
#define LV_X11_RENDER_MODE_FULL    0

#endif
