/*
 * remote_screen.cpp - the laptop as the main's screen (see remote_screen.h).
 *
 * An LVGL display that sends what it draws to the browser page, and an LVGL
 * pointer that reads the page's clicks. Everything runs in loop(): LVGL,
 * the web server, the WebSocket and MQTT never run at the same time.
 */
#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <lvgl.h>

#include "remote_screen.h"
#include "viewer_html.h"

static WebServer s_http(80);
static WebSocketsServer s_ws(81);

static lv_display_t *s_disp;
static uint16_t s_w, s_h;
static int s_viewers;

static uint8_t *s_out;          /* 'R' header + run-length data, in PSRAM */
static size_t s_out_size;

static volatile bool s_pressed;
static volatile int32_t s_px, s_py;

int remote_screen_viewers(void) { return s_viewers; }

/* ---- LVGL display: send every flushed rectangle -------------------------- */

static void put16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)(v & 0xFF); p[1] = (uint8_t)(v >> 8); }

static void flush_cb(lv_display_t *disp, const lv_area_t *a, uint8_t *px)
{
    if (s_viewers > 0) {
        uint32_t w = lv_area_get_width(a), h = lv_area_get_height(a);
        s_out[0] = 'R';
        put16(s_out + 1, (uint16_t)a->x1);
        put16(s_out + 3, (uint16_t)a->y1);
        put16(s_out + 5, (uint16_t)w);
        put16(s_out + 7, (uint16_t)h);
        size_t n = remote_screen_rle((const uint16_t *)px, w, h, s_out + 9, s_out_size - 9);
        if (n) s_ws.broadcastBIN(s_out, 9 + n);
    }
    lv_display_flush_ready(disp);
}

/* ---- LVGL pointer: the page's clicks -------------------------------------- */

static void read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->point.x = s_px;
    data->point.y = s_py;
    data->state = s_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void on_ws(uint8_t num, WStype_t type, uint8_t *payload, size_t len)
{
    switch (type) {
    case WStype_CONNECTED: {
        s_viewers++;
        uint8_t hello[5] = { 'H' };
        put16(hello + 1, s_w);
        put16(hello + 3, s_h);
        s_ws.sendBIN(num, hello, sizeof hello);
        lv_obj_invalidate(lv_screen_active());     /* send the whole screen to the new viewer */
        lv_obj_invalidate(lv_layer_top());
        Serial.printf("screen: viewer connected (%d)\n", s_viewers);
        break;
    }
    case WStype_DISCONNECTED:
        if (s_viewers > 0) s_viewers--;
        s_pressed = false;
        break;
    case WStype_TEXT: {
        char kind = 0;
        int x = 0, y = 0;
        char buf[24];
        size_t n = len < sizeof buf - 1 ? len : sizeof buf - 1;
        memcpy(buf, payload, n);
        buf[n] = 0;
        if (sscanf(buf, "%c %d %d", &kind, &x, &y) == 3 && x >= 0 && y >= 0 && x < s_w && y < s_h) {
            s_px = x;
            s_py = y;
            if (kind == 'd' || kind == 'm') s_pressed = true;
            else if (kind == 'u') s_pressed = false;
        }
        break;
    }
    default:
        break;
    }
}

void remote_screen_begin(uint16_t w, uint16_t h)
{
    s_w = w;
    s_h = h;

    /* draw buffers: 1/10 of the screen, two of them, in PSRAM */
    size_t lines = h / 10;
    size_t buf_bytes = (size_t)w * lines * 2;
    void *b1 = ps_malloc(buf_bytes), *b2 = ps_malloc(buf_bytes);
    s_out_size = 9 + 2 * buf_bytes;                 /* worst case of the run-length code */
    s_out = (uint8_t *)ps_malloc(s_out_size);
    if (!b1 || !b2 || !s_out) {
        Serial.println("screen: no PSRAM? Tools > PSRAM must be \"OPI PSRAM\"");
        while (true) delay(1000);
    }

    s_disp = lv_display_create(w, h);
    lv_display_set_color_format(s_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(s_disp, b1, b2, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(s_disp, flush_cb);

    lv_indev_t *in = lv_indev_create();
    lv_indev_set_type(in, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(in, read_cb);

    s_http.on("/", []() { s_http.send_P(200, "text/html", VIEWER_HTML); });
    s_http.onNotFound([]() { s_http.send(404, "text/plain", "not found"); });
    s_http.begin();
    s_ws.begin();
    s_ws.onEvent(on_ws);
}

void remote_screen_loop(void)
{
    s_http.handleClient();
    s_ws.loop();
}
