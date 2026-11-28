#pragma once
#include <stdint.h>

// Optional local credentials. Without this file, sensing still works offline.
#if __has_include("secrets.h")
#include "secrets.h"
#endif
#ifndef SECRET_WIFI_SSID
#define SECRET_WIFI_SSID ""
#endif
#ifndef SECRET_WIFI_PASSWORD
#define SECRET_WIFI_PASSWORD ""
#endif
#ifndef SECRET_MQTT_PASSWORD
#define SECRET_MQTT_PASSWORD ""
#endif

#define NODE_BUILDING "Z"
#define NODE_FLOOR "1"
#define NODE_ROOM "Z.1.09"
#define NODE_TOPIC "inhabitx/" NODE_BUILDING "/" NODE_FLOOR "/" NODE_ROOM "/state"
#define NODE_CLIENT_ID "inhabitx-node-" NODE_ROOM

constexpr char MQTT_HOST[] = "inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud";
constexpr uint16_t MQTT_PORT = 8883;
constexpr char MQTT_USER[] = "devices";
constexpr uint32_t NODE_SEND_EVERY_MS = 2000;
constexpr uint32_t NODE_RESULT_MAX_AGE_MS = 2000;
constexpr uint32_t NODE_RECONNECT_MS = 5000;

// Replace one with your router/local NTP server if public UDP/123 is blocked.
constexpr char NTP_SERVER_1[] = "time.cloudflare.com";
constexpr char NTP_SERVER_2[] = "time.google.com";
constexpr char NTP_SERVER_3[] = "pool.ntp.org";
constexpr uint32_t NTP_RETRY_MS = 60000;
