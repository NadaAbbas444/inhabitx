/*
 * InhabitX main on a XIAO ESP32-S3 (Sense), with the laptop as its screen.
 *
 * Same main as main_sim on the laptop: rooms, bookings, ghost release,
 * lost & found, answering the web page over the broker. Its touch screen UI
 * is drawn on the board and shown in a browser: open http://<board IP>/
 * (printed on the serial monitor) on any laptop or phone in the same Wi-Fi.
 *
 * Board: "XIAO_ESP32S3", Tools > PSRAM: "OPI PSRAM". Libraries: lvgl 9.2.2,
 * PubSubClient 2.8, WebSockets 2.6.1 (Links2004). See README.md.
 */
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <lvgl.h>
#include <time.h>

#include "secrets.h"
#include "remote_screen.h"
#include "src/inhabitx/main_app.h"
#include "src/inhabitx/main_rooms.h"
#include "src/inhabitx/ix_msg.h"
#include "src/inhabitx/ui.h"

/* ---- settings ----------------------------------------------------------- */
#define BROKER_HOST     "inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud"
#define BROKER_PORT     8883
#define BROKER_USER     "devices"
#define HOSTNAME        "inhabitx-main"         /* http://inhabitx-main.local/ */

/* ISRG Root X1 (Let's Encrypt), the root of HiveMQ Cloud's certificate, valid until 2035 */
static const char ROOT_CA[] PROGMEM = R"PEM(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)PEM";

static WiFiClientSecure s_tls;
static PubSubClient s_mqtt(s_tls);

/* ---- broker --------------------------------------------------------------- */

static bool publish(const char *topic, const char *payload, size_t len, bool retain)
{
    if (!s_mqtt.connected()) return false;
    return s_mqtt.publish(topic, (const uint8_t *)payload, len, retain);
}

/* Every message under inhabitx/Z/. Never print the payload: booking
 * requests carry names and emails. */
static void on_message(char *topic, uint8_t *payload, unsigned int len)
{
    main_app_on_message(topic, strlen(topic), (const char *)payload, len, publish);
}

static void mqtt_connect(void)
{
    static uint32_t last_try;
    if (s_mqtt.connected() || (last_try && millis() - last_try < 5000)) return;
    last_try = millis();
    Serial.printf("mqtt: connecting to %s ...\n", BROKER_HOST);
    /* last will: every room offline + "main":"offline", retained */
    if (s_mqtt.connect(MAIN_CLIENT_ID, BROKER_USER, SECRET_MQTT_PASSWORD,
                       MAIN_WILL_TOPIC, 1, true, MAIN_WILL_PAYLOAD)) {
        s_mqtt.subscribe(IX_TOPIC_SUB, 1);
        main_app_connected();
        Serial.printf("mqtt: connected as %s, listening on %s\n", BROKER_USER, IX_TOPIC_SUB);
    } else {
        Serial.printf("mqtt: failed (state %d)%s\n", s_mqtt.state(),
                      s_mqtt.state() == 4 || s_mqtt.state() == 5 ? ": check the broker password" : "");
    }
}

/* ---- Wi-Fi and time -------------------------------------------------------- */

static void wifi_connect(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(HOSTNAME);
    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
    Serial.printf("wifi: connecting to %s", SECRET_WIFI_SSID);
    for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
        delay(500);
        Serial.print(".");
        if (i == 40) Serial.print(" (2.4 GHz, WPA2? check secrets.h)");
    }
    Serial.printf("\nwifi: connected, screen at http://%s/ or http://%s.local/\n",
                  WiFi.localIP().toString().c_str(), HOSTNAME);
    MDNS.begin(HOSTNAME);
}

static void time_sync(void)
{
    configTzTime(MAIN_TZ, "pool.ntp.org", "time.google.com");
    struct tm tm;
    if (getLocalTime(&tm, 10000)) {
        main_app_set_time(time(NULL));
        Serial.printf("time: %02d:%02d\n", tm.tm_hour, tm.tm_min);
    } else {
        Serial.println("time: no NTP answer yet, using the fake nodes' time instead");
    }
}

/* ---- LVGL ------------------------------------------------------------------ */

static uint32_t tick_ms(void) { return millis(); }

/* ---- Arduino --------------------------------------------------------------- */

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("\nInhabitX main (XIAO ESP32-S3)");

    main_app_init();
    wifi_connect();
    time_sync();

    lv_init();
    lv_tick_set_cb(tick_ms);
    remote_screen_begin(UI_HOR_RES, UI_VER_RES);
    ui_init();

    s_tls.setCACert(ROOT_CA);
    s_mqtt.setServer(BROKER_HOST, BROKER_PORT);
    s_mqtt.setBufferSize(4096);         /* bookings, and our own building state echoed back */
    s_mqtt.setKeepAlive(30);
    s_mqtt.setCallback(on_message);
}

void loop()
{
    static uint32_t next_poll;

    if (WiFi.status() == WL_CONNECTED) {
        mqtt_connect();
        s_mqtt.loop();
    }
    remote_screen_loop();
    uint32_t wait = lv_timer_handler();

    if ((int32_t)(millis() - next_poll) >= 0) {
        next_poll = millis() + 500;
        /* once NTP has set the board clock it wins over the nodes' time
         * (otherwise an earlier node time and NTP would add up) */
        if (time(NULL) > 1700000000) main_app_set_time(time(NULL));
        main_app_poll(publish);
    }
    delay(wait < 5 ? wait : 5);
}
