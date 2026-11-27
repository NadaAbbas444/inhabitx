/* Shared between the UI files. Not used outside ui/. */
#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "lvgl.h"
#include "ui.h"
#include "ix_model.h"

/* ---- layout ------------------------------------------------------------
 * Sidebar on the left, header with title + clock, one page area. */
#define UI_SIDEBAR_W    208
#define UI_GAP          16
#define UI_MAIN_PAD     20
#define UI_HEADER_H     92
#define UI_PAGE_X       (UI_SIDEBAR_W + UI_MAIN_PAD)
#define UI_PAGE_Y       UI_HEADER_H
#define UI_PAGE_W       (UI_HOR_RES - UI_SIDEBAR_W - 2 * UI_MAIN_PAD)
#define UI_PAGE_H       (UI_VER_RES - UI_HEADER_H - UI_MAIN_PAD)
#define UI_R_CARD       18
#define UI_R_CTRL       12

/* ---- colours: dark navy from the InhabitX logo, blue accent ------------ */
#define UI_C_BG         lv_color_hex(0x0A0F1C)
#define UI_C_SIDEBAR    lv_color_hex(0x0D1426)
#define UI_C_CARD       lv_color_hex(0x121A2C)
#define UI_C_CARD2      lv_color_hex(0x19233A)   /* controls on cards  */
#define UI_C_CARD3      lv_color_hex(0x22304E)   /* active / pressed   */
#define UI_C_LINE       lv_color_hex(0x1E2942)
#define UI_C_TEXT       lv_color_hex(0xF1F5FB)
#define UI_C_MUTED      lv_color_hex(0x8B97AE)
#define UI_C_FAINT      lv_color_hex(0x56627C)
#define UI_C_ACCENT     lv_color_hex(0x3B7BFF)   /* logo blue, lifted for dark bg */
#define UI_C_ACCENT2    lv_color_hex(0x6D5BFF)   /* gradient partner   */

/* room states */
#define UI_C_FREE       lv_color_hex(0x22C55E)
#define UI_C_BUSY       lv_color_hex(0xF43F5E)
#define UI_C_GHOST      lv_color_hex(0xF59E0B)
#define UI_C_BOOKED     lv_color_hex(0x818CF8)
#define UI_C_OFFLINE    lv_color_hex(0x64748B)

/* ---- fonts: Roboto via TinyTTF (ui_fonts.c) ---------------------------- */
extern const lv_font_t *ui_f_caption;  /* 12 medium, used upper-case */
extern const lv_font_t *ui_f_small;    /* 13 regular */
extern const lv_font_t *ui_f_body;     /* 15 regular */
extern const lv_font_t *ui_f_label;    /* 15 medium  */
extern const lv_font_t *ui_f_title;    /* 18 bold    */
extern const lv_font_t *ui_f_h2;       /* 22 bold    */
extern const lv_font_t *ui_f_h1;       /* 30 bold    */
extern const lv_font_t *ui_f_hero;     /* 56 bold    */
void ui_fonts_init(void);

/* ---- pages ------------------------------------------------------------ */
typedef enum { UI_PAGE_OVERVIEW, UI_PAGE_CAMPUS, UI_PAGE_BOOK, UI_PAGE_LOSTFOUND, UI_PAGE_COUNT } ui_page_t;

void ui_show_page(ui_page_t p);
void ui_toast(const char *msg);
void ui_mark_busy(void);            /* something to reset when idle */

void ui_overview_create(lv_obj_t *parent);
void ui_overview_refresh(time_t now);
void ui_overview_reset(void);
void ui_room_drawer(int room);      /* room detail panel sliding in */

void ui_campus_create(lv_obj_t *parent);
void ui_campus_refresh(time_t now);

void ui_book_create(lv_obj_t *parent);
void ui_book_refresh(time_t now);
void ui_book_start(int room, uint8_t people);   /* room < 0: any free room */
void ui_book_reset(void);

void ui_lf_create(lv_obj_t *parent);
void ui_lf_refresh(time_t now);
void ui_lf_reset(void);

/* floor plan asset (floorplan.c, traced from the public AAU campus plan) */
enum { UI_PLAN_LIVE, UI_PLAN_ROOM, UI_PLAN_LABEL, UI_PLAN_HALL };
typedef struct { const char *id, *name; uint8_t kind; int16_t x, y, w, h; } ui_plan_room_t;
extern const lv_image_dsc_t ui_img_floorplan;
extern const ui_plan_room_t ui_plan_rooms[];
extern const int ui_plan_room_count;
extern const lv_point_t ui_plan_here;
extern const lv_image_dsc_t ui_img_floorplan_full;          /* whole floor, full-screen view */
extern const ui_plan_room_t ui_plan_full_rooms[];
extern const int ui_plan_full_room_count;
extern const lv_point_t ui_plan_full_here;

/* campus map asset (campus_map.c, generated from OpenStreetMap) */
typedef struct { const char *code, *name; int16_t x, y; bool live; } ui_campus_label_t;
extern const lv_image_dsc_t ui_img_campus;
extern const ui_campus_label_t ui_campus_labels[];
extern const int ui_campus_label_count;

/* ---- widget helpers (ui_widgets.c) ------------------------------------ */
typedef enum { UI_BTN_PRIMARY, UI_BTN_SECONDARY, UI_BTN_CHIP, UI_BTN_GHOST } ui_btn_kind_t;

lv_obj_t *ui_box(lv_obj_t *parent);                       /* style-less container */
lv_obj_t *ui_card(lv_obj_t *parent);                      /* dark rounded card    */
lv_obj_t *ui_label(lv_obj_t *parent, const char *txt, const lv_font_t *f, lv_color_t c);
lv_obj_t *ui_caption(lv_obj_t *parent, const char *txt);  /* SMALL CAPS heading   */
lv_obj_t *ui_btn(lv_obj_t *parent, const char *txt, ui_btn_kind_t kind,
                 lv_event_cb_t cb, void *user);
void      ui_btn_set_text(lv_obj_t *btn, const char *txt);
lv_obj_t *ui_round_btn(lv_obj_t *parent, const char *symbol, lv_event_cb_t cb, void *user);
lv_obj_t *ui_pill(lv_obj_t *parent);                      /* dot + text status chip */
void      ui_pill_set(lv_obj_t *pill, lv_color_t c, const char *txt);
lv_obj_t *ui_icon_tile(lv_obj_t *parent, const char *symbol, lv_color_t c, int size);
lv_obj_t *ui_row(lv_obj_t *parent, int gap);              /* flex row, centred */
lv_obj_t *ui_col(lv_obj_t *parent, int gap);              /* flex column       */
lv_obj_t *ui_spacer(lv_obj_t *parent);                    /* flex-grow filler  */
lv_obj_t *ui_overlay(bool scrim);                         /* modal layer       */
void      ui_close_overlays(void);
lv_obj_t *ui_stepper(lv_obj_t *parent, const char *title, const char *icon,
                     lv_obj_t **value_label, lv_event_cb_t cb);  /* user data -1 / +1 */

lv_color_t  ui_state_color(ix_state_t s);
const char *ui_state_text(ix_state_t s);

void ui_fmt_hm(char *buf, size_t n, time_t t);            /* "14:05"      */
void ui_fmt_dur(char *buf, size_t n, long secs);          /* "1 h 15 min" */
void ui_status_detail(const ix_status_t *st, char *buf, size_t n);  /* "until 12:00" */

#endif
