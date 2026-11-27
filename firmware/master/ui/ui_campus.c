/* Campus map: the real Universität Klagenfurt campus (OpenStreetMap),
 * with the live InhabitX building pulsing and labelled. */
#include "ui_internal.h"

#include <stdio.h>

static lv_obj_t *s_live_pill, *s_info_free;

static void open_overview_cb(lv_event_t *e)
{
    ui_overview_reset();
    ui_show_page(UI_PAGE_OVERVIEW);
}

static void pulse_cb(void *obj, int32_t v)
{
    lv_obj_t *o = obj;
    int d = 18 + v * 50 / 255;
    lv_obj_set_size(o, d, d);
    lv_obj_set_style_border_opa(o, (lv_opa_t)(255 - v), 0);
    lv_obj_align(o, LV_ALIGN_CENTER, 0, 0);
}

static void building_chip(lv_obj_t *map, const ui_campus_label_t *l)
{
    lv_obj_t *c = ui_row(map, 6);
    lv_obj_set_style_bg_color(c, UI_C_BG, 0);
    lv_obj_set_style_bg_opa(c, 210, 0);
    lv_obj_set_style_radius(c, 8, 0);
    lv_obj_set_style_pad_hor(c, 7, 0);
    lv_obj_set_style_pad_ver(c, 4, 0);
    lv_obj_set_style_border_color(c, UI_C_LINE, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    if (l->code[0]) {
        lv_obj_t *b = ui_label(c, l->code, ui_f_caption, UI_C_MUTED);
        lv_obj_set_style_bg_color(b, UI_C_CARD3, 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(b, 4, 0);
        lv_obj_set_style_pad_hor(b, 4, 0);
    }
    ui_label(c, l->name, ui_f_caption, UI_C_MUTED);
    lv_obj_update_layout(c);
    lv_obj_set_pos(c, l->x - lv_obj_get_width(c) / 2, l->y - lv_obj_get_height(c) / 2);
}

static void live_marker(lv_obj_t *map, const ui_campus_label_t *l)
{
    /* pulsing ring + dot on the building */
    lv_obj_t *anchor = ui_box(map);
    lv_obj_set_size(anchor, 70, 70);
    lv_obj_set_pos(anchor, l->x - 35, l->y - 35);
    lv_obj_t *ring = ui_box(anchor);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(ring, lv_color_white(), 0);
    lv_obj_set_style_border_width(ring, 2, 0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ring);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, 1600);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, pulse_cb);
    lv_anim_start(&a);
    lv_obj_t *dot = ui_box(anchor);
    lv_obj_set_size(dot, 14, 14);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(dot, UI_C_ACCENT, 0);
    lv_obj_set_style_border_width(dot, 3, 0);
    lv_obj_center(dot);

    /* callout above the building */
    lv_obj_t *c = ui_col(map, 4);
    lv_obj_set_style_bg_color(c, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(c, 12, 0);
    lv_obj_set_style_pad_all(c, 10, 0);
    lv_obj_set_style_pad_hor(c, 12, 0);
    lv_obj_set_style_border_color(c, UI_C_ACCENT, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    lv_obj_set_style_shadow_color(c, UI_C_ACCENT, 0);
    lv_obj_set_style_shadow_width(c, 24, 0);
    lv_obj_set_style_shadow_opa(c, LV_OPA_30, 0);
    lv_obj_add_flag(c, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(c, open_overview_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *t = ui_row(c, 6);
    ui_label(t, LV_SYMBOL_GPS, &lv_font_montserrat_14, UI_C_ACCENT);
    ui_label(t, "You are here", ui_f_caption, UI_C_ACCENT);
    ui_label(c, l->name, ui_f_title, UI_C_TEXT);
    s_live_pill = ui_pill(c);
    lv_obj_update_layout(c);
    lv_obj_set_pos(c, l->x + 78, l->y - lv_obj_get_height(c) / 2 + 4);
}

static lv_obj_t *legend_row(lv_obj_t *parent, lv_color_t c, const char *title, const char *sub)
{
    lv_obj_t *r = ui_row(parent, 10);
    lv_obj_t *d = ui_box(r);
    lv_obj_set_size(d, 10, 10);
    lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(d, c, 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
    lv_obj_t *col = ui_col(r, 0);
    ui_label(col, title, ui_f_label, UI_C_TEXT);
    return ui_label(col, sub, ui_f_small, UI_C_MUTED);
}

void ui_campus_create(lv_obj_t *page)
{
    lv_obj_t *card = ui_card(page);
    lv_obj_set_size(card, UI_PAGE_W, UI_PAGE_H);
    lv_obj_set_style_pad_all(card, 0, 0);

    lv_obj_t *map = ui_box(card);
    lv_obj_set_size(map, UI_PAGE_W, UI_PAGE_H);
    lv_obj_t *img = lv_image_create(map);
    lv_image_set_src(img, &ui_img_campus);

    const ui_campus_label_t *live = NULL;
    for (int i = 0; i < ui_campus_label_count; i++) {
        if (ui_campus_labels[i].live) live = &ui_campus_labels[i];
        else building_chip(map, &ui_campus_labels[i]);
    }
    if (live) live_marker(map, live);

    /* info card */
    lv_obj_t *info = ui_col(card, 12);
    lv_obj_set_width(info, 250);
    lv_obj_set_style_bg_color(info, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(info, 235, 0);
    lv_obj_set_style_radius(info, 16, 0);
    lv_obj_set_style_pad_all(info, 16, 0);
    lv_obj_set_style_border_color(info, UI_C_LINE, 0);
    lv_obj_set_style_border_width(info, 1, 0);
    ui_caption(info, "CAMPUS");
    lv_obj_t *t = ui_label(info, "Universität Klagenfurt", ui_f_title, UI_C_TEXT);
    lv_obj_set_style_pad_bottom(t, 2, 0);
    s_info_free = legend_row(info, UI_C_ACCENT, "Zentralgebäude", "");
    legend_row(info, UI_C_FAINT, "Other buildings", "Sensors coming soon");
    lv_obj_t *b = ui_btn(info, "Open floor plan  " LV_SYMBOL_RIGHT, UI_BTN_PRIMARY, open_overview_cb, NULL);
    lv_obj_set_width(b, lv_pct(100));
    lv_obj_align(info, LV_ALIGN_BOTTOM_LEFT, 16, -16);

    lv_obj_t *attr = ui_label(card, "© OpenStreetMap contributors", ui_f_caption, UI_C_MUTED);
    lv_obj_set_style_bg_color(attr, UI_C_BG, 0);
    lv_obj_set_style_bg_opa(attr, 200, 0);
    lv_obj_set_style_pad_hor(attr, 8, 0);
    lv_obj_set_style_pad_ver(attr, 3, 0);
    lv_obj_set_style_radius(attr, 6, 0);
    lv_obj_align(attr, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
}

void ui_campus_refresh(time_t now)
{
    int free = 0, n = ix_room_count();
    for (int i = 0; i < n; i++) {
        ix_status_t st;
        ix_room_status(i, now, &st);
        if (st.state == IX_FREE) free++;
    }
    char buf[48];
    snprintf(buf, sizeof buf, free == 1 ? "1 room free now" : "%d rooms free now", free);
    if (s_live_pill) ui_pill_set(s_live_pill, free ? UI_C_FREE : UI_C_BUSY, buf);
    snprintf(buf, sizeof buf, "Live  ·  %d of %d rooms free", free, n);
    lv_label_set_text(s_info_free, buf);
}
