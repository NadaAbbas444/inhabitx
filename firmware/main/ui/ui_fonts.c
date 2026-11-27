/* Roboto in three weights, rendered by LVGL's TinyTTF.
 * Icons (LV_SYMBOL_*) come from the built-in Montserrat as fallback. */
#include "ui_internal.h"

extern const uint8_t ui_ttf_regular[], ui_ttf_medium[], ui_ttf_bold[];
extern const size_t ui_ttf_regular_size, ui_ttf_medium_size, ui_ttf_bold_size;

const lv_font_t *ui_f_caption, *ui_f_small, *ui_f_body, *ui_f_label,
                *ui_f_title, *ui_f_h2, *ui_f_h1, *ui_f_hero;

static const lv_font_t *mk(const uint8_t *data, size_t size, int px)
{
    lv_font_t *f = lv_tiny_ttf_create_data(data, size, px);
    if (!f) return &lv_font_montserrat_16;
    f->fallback = px <= 14 ? &lv_font_montserrat_14
                : px <= 17 ? &lv_font_montserrat_16
                : px <= 21 ? &lv_font_montserrat_20
                : px <= 27 ? &lv_font_montserrat_24
                           : &lv_font_montserrat_32;
    return f;
}

void ui_fonts_init(void)
{
    ui_f_caption = mk(ui_ttf_medium, ui_ttf_medium_size, 12);
    ui_f_small   = mk(ui_ttf_regular, ui_ttf_regular_size, 13);
    ui_f_body    = mk(ui_ttf_regular, ui_ttf_regular_size, 15);
    ui_f_label   = mk(ui_ttf_medium, ui_ttf_medium_size, 15);
    ui_f_title   = mk(ui_ttf_bold, ui_ttf_bold_size, 18);
    ui_f_h2      = mk(ui_ttf_bold, ui_ttf_bold_size, 22);
    ui_f_h1      = mk(ui_ttf_bold, ui_ttf_bold_size, 30);
    ui_f_hero    = mk(ui_ttf_bold, ui_ttf_bold_size, 56);
}
