/* Widget helpers: cards, buttons, pills, steppers. All styling lives here. */
#include "ui_internal.h"

#include <stdio.h>

lv_obj_t *ui_box(lv_obj_t *parent)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    return o;
}

lv_obj_t *ui_card(lv_obj_t *parent)
{
    lv_obj_t *o = ui_box(parent);
    lv_obj_set_style_bg_color(o, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(o, UI_C_LINE, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_radius(o, UI_R_CARD, 0);
    lv_obj_set_style_pad_all(o, 16, 0);
    lv_obj_set_style_clip_corner(o, true, 0);
    return o;
}

lv_obj_t *ui_label(lv_obj_t *parent, const char *txt, const lv_font_t *f, lv_color_t c)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, txt ? txt : "");
    lv_obj_set_style_text_font(l, f, 0);
    lv_obj_set_style_text_color(l, c, 0);
    return l;
}

lv_obj_t *ui_caption(lv_obj_t *parent, const char *txt)
{
    lv_obj_t *l = ui_label(parent, txt, ui_f_caption, UI_C_MUTED);
    lv_obj_set_style_text_letter_space(l, 1, 0);
    return l;
}

lv_obj_t *ui_row(lv_obj_t *parent, int gap)
{
    lv_obj_t *r = ui_box(parent);
    lv_obj_set_flex_flow(r, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(r, gap, 0);
    return r;
}

lv_obj_t *ui_col(lv_obj_t *parent, int gap)
{
    lv_obj_t *c = ui_box(parent);
    lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(c, gap, 0);
    return c;
}

lv_obj_t *ui_spacer(lv_obj_t *parent)
{
    lv_obj_t *s = ui_box(parent);
    lv_obj_set_flex_grow(s, 1);
    return s;
}

lv_obj_t *ui_btn(lv_obj_t *parent, const char *txt, ui_btn_kind_t kind,
                 lv_event_cb_t cb, void *user)
{
    lv_obj_t *b = lv_button_create(parent);
    lv_obj_remove_style_all(b);
    lv_obj_set_size(b, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(b, kind == UI_BTN_CHIP ? LV_RADIUS_CIRCLE : UI_R_CTRL, 0);
    lv_obj_set_style_pad_hor(b, kind == UI_BTN_CHIP ? 14 : 20, 0);
    lv_obj_set_style_pad_ver(b, kind == UI_BTN_CHIP ? 8 : 13, 0);
    lv_obj_set_style_text_font(b, kind == UI_BTN_CHIP ? ui_f_small : ui_f_label, 0);

    switch (kind) {
    case UI_BTN_PRIMARY:
        lv_obj_set_style_bg_color(b, UI_C_ACCENT, 0);
        lv_obj_set_style_bg_grad_color(b, UI_C_ACCENT2, 0);
        lv_obj_set_style_bg_grad_dir(b, LV_GRAD_DIR_HOR, 0);
        lv_obj_set_style_text_color(b, lv_color_white(), 0);
        lv_obj_set_style_shadow_color(b, UI_C_ACCENT, 0);
        lv_obj_set_style_shadow_width(b, 18, 0);
        lv_obj_set_style_shadow_opa(b, LV_OPA_30, 0);
        lv_obj_set_style_shadow_offset_y(b, 4, 0);
        lv_obj_set_style_bg_color(b, UI_C_CARD2, LV_STATE_DISABLED);
        lv_obj_set_style_bg_grad_dir(b, LV_GRAD_DIR_NONE, LV_STATE_DISABLED);
        lv_obj_set_style_text_color(b, UI_C_FAINT, LV_STATE_DISABLED);
        lv_obj_set_style_shadow_width(b, 0, LV_STATE_DISABLED);
        break;
    case UI_BTN_GHOST:
        lv_obj_set_style_bg_opa(b, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_color(b, UI_C_MUTED, 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(b, UI_C_CARD2, LV_STATE_PRESSED);
        break;
    default:
        lv_obj_set_style_bg_color(b, UI_C_CARD2, 0);
        lv_obj_set_style_text_color(b, UI_C_TEXT, 0);
        lv_obj_set_style_border_color(b, UI_C_LINE, 0);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_bg_color(b, UI_C_CARD3, LV_STATE_PRESSED);
        if (kind == UI_BTN_CHIP) {
            lv_obj_set_style_bg_color(b, UI_C_ACCENT, LV_STATE_CHECKED);
            lv_obj_set_style_border_color(b, UI_C_ACCENT, LV_STATE_CHECKED);
            lv_obj_set_style_text_color(b, lv_color_white(), LV_STATE_CHECKED);
        } else {
            lv_obj_set_style_bg_color(b, UI_C_CARD3, LV_STATE_CHECKED);
            lv_obj_set_style_border_color(b, UI_C_ACCENT, LV_STATE_CHECKED);
            lv_obj_set_style_border_width(b, 2, LV_STATE_CHECKED);
        }
        lv_obj_set_style_text_color(b, UI_C_FAINT, LV_STATE_DISABLED);
        lv_obj_set_style_bg_opa(b, LV_OPA_50, LV_STATE_DISABLED);
        break;
    }

    lv_obj_t *l = lv_label_create(b);
    lv_label_set_text(l, txt ? txt : "");
    lv_obj_center(l);
    if (cb) lv_obj_add_event_cb(b, cb, LV_EVENT_CLICKED, user);
    return b;
}

void ui_btn_set_text(lv_obj_t *btn, const char *txt)
{
    lv_obj_t *l = lv_obj_get_child(btn, 0);
    if (l) lv_label_set_text(l, txt);
}

lv_obj_t *ui_round_btn(lv_obj_t *parent, const char *symbol, lv_event_cb_t cb, void *user)
{
    lv_obj_t *b = ui_btn(parent, symbol, UI_BTN_SECONDARY, cb, user);
    lv_obj_set_size(b, 44, 44);
    lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(b, 0, 0);
    lv_obj_set_style_text_font(b, &lv_font_montserrat_16, 0);
    return b;
}

lv_obj_t *ui_pill(lv_obj_t *parent)
{
    lv_obj_t *p = ui_row(parent, 6);
    lv_obj_set_style_radius(p, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(p, 38, 0);
    lv_obj_set_style_pad_hor(p, 10, 0);
    lv_obj_set_style_pad_ver(p, 4, 0);
    lv_obj_t *dot = ui_box(p);
    lv_obj_set_size(dot, 7, 7);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    ui_label(p, "", ui_f_caption, UI_C_TEXT);
    return p;
}

void ui_pill_set(lv_obj_t *pill, lv_color_t c, const char *txt)
{
    lv_obj_set_style_bg_color(pill, c, 0);
    lv_obj_set_style_bg_color(lv_obj_get_child(pill, 0), c, 0);
    lv_obj_t *l = lv_obj_get_child(pill, 1);
    lv_obj_set_style_text_color(l, lv_color_mix(c, lv_color_white(), 160), 0);
    lv_label_set_text(l, txt);
}

lv_obj_t *ui_icon_tile(lv_obj_t *parent, const char *symbol, lv_color_t c, int size)
{
    lv_obj_t *t = ui_box(parent);
    lv_obj_set_size(t, size, size);
    lv_obj_set_style_radius(t, size / 3, 0);
    lv_obj_set_style_bg_color(t, c, 0);
    lv_obj_set_style_bg_opa(t, 40, 0);
    lv_obj_t *i = ui_label(t, symbol, size >= 48 ? &lv_font_montserrat_24 : &lv_font_montserrat_16, c);
    lv_obj_center(i);
    return t;
}

static void scrim_cb(lv_event_t *e)
{
    if (lv_event_get_target(e) == lv_event_get_current_target(e)) ui_close_overlays();
}

lv_obj_t *ui_overlay(bool scrim)
{
    lv_obj_t *o = ui_box(lv_layer_top());
    lv_obj_set_size(o, UI_HOR_RES, UI_VER_RES);
    lv_obj_set_style_bg_color(o, scrim ? lv_color_black() : UI_C_BG, 0);
    lv_obj_set_style_bg_opa(o, scrim ? LV_OPA_60 : LV_OPA_COVER, 0);
    lv_obj_add_flag(o, LV_OBJ_FLAG_CLICKABLE);
    if (scrim) lv_obj_add_event_cb(o, scrim_cb, LV_EVENT_CLICKED, NULL);
    ui_mark_busy();
    return o;
}

void ui_close_overlays(void) { lv_obj_clean(lv_layer_top()); }

lv_obj_t *ui_stepper(lv_obj_t *parent, const char *title, const char *icon,
                     lv_obj_t **value_label, lv_event_cb_t cb)
{
    lv_obj_t *row = ui_row(parent, 10);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_style_bg_color(row, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(row, 14, 0);
    lv_obj_set_style_pad_all(row, 8, 0);
    lv_obj_set_style_pad_left(row, 12, 0);

    ui_icon_tile(row, icon, UI_C_ACCENT, 34);
    ui_label(row, title, ui_f_label, UI_C_MUTED);
    ui_spacer(row);
    lv_obj_t *m = ui_round_btn(row, LV_SYMBOL_MINUS, cb, (void *)(intptr_t)-1);
    lv_obj_set_size(m, 40, 40);
    lv_obj_set_style_bg_color(m, UI_C_CARD3, 0);
    lv_obj_t *v = ui_label(row, "", ui_f_h2, UI_C_TEXT);
    lv_obj_set_width(v, 70);
    lv_obj_set_style_text_align(v, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *p = ui_round_btn(row, LV_SYMBOL_PLUS, cb, (void *)(intptr_t)1);
    lv_obj_set_size(p, 40, 40);
    lv_obj_set_style_bg_color(p, UI_C_CARD3, 0);
    if (value_label) *value_label = v;
    return row;
}

lv_color_t ui_state_color(ix_state_t s)
{
    switch (s) {
    case IX_FREE:     return UI_C_FREE;
    case IX_OCCUPIED: return UI_C_BUSY;
    case IX_GHOST:    return UI_C_GHOST;
    case IX_BOOKED:   return UI_C_BOOKED;
    default:          return UI_C_OFFLINE;
    }
}

const char *ui_state_text(ix_state_t s)
{
    switch (s) {
    case IX_FREE:     return "Free";
    case IX_OCCUPIED: return "In use";
    case IX_GHOST:    return "No-show";
    case IX_BOOKED:   return "Booked";
    default:          return "Offline";
    }
}

void ui_fmt_hm(char *buf, size_t n, time_t t)
{
    struct tm tm;
    localtime_r(&t, &tm);
    snprintf(buf, n, "%02d:%02d", tm.tm_hour, tm.tm_min);
}

void ui_fmt_dur(char *buf, size_t n, long secs)
{
    long m = secs / 60, h = m / 60;
    m %= 60;
    if (h && m) snprintf(buf, n, "%ld h %ld min", h, m);
    else if (h) snprintf(buf, n, "%ld h", h);
    else        snprintf(buf, n, "%ld min", m);
}

void ui_status_detail(const ix_status_t *st, char *buf, size_t n)
{
    char t[8];
    switch (st->state) {
    case IX_FREE:
        if (st->free_until) { ui_fmt_hm(t, sizeof t, st->free_until); snprintf(buf, n, "until %s", t); }
        else snprintf(buf, n, "rest of the day");
        break;
    case IX_BOOKED:
        ui_fmt_hm(t, sizeof t, st->until);
        snprintf(buf, n, "booked until %s", t);
        break;
    case IX_GHOST:
        snprintf(buf, n, "frees in %d:%02d", (int)(st->release_in_s / 60), (int)(st->release_in_s % 60));
        break;
    case IX_OCCUPIED:
        if (st->until) { ui_fmt_hm(t, sizeof t, st->until); snprintf(buf, n, "until %s", t); }
        else snprintf(buf, n, "walk-in");
        break;
    default:
        snprintf(buf, n, "no sensor signal");
        break;
    }
}
