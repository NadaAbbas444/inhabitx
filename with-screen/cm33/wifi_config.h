/*
 * wifi_config.h - InhabitX node: Wi-Fi settings.
 *
 * Replaces wifi_config.h of Infineon's PSOC Edge Wi-Fi MQTT client example.
 * SSID and password come from secrets.h (not committed). Use a phone
 * hotspot: campus Wi-Fi needs a login page the board can't handle.
 */
#ifndef WIFI_CONFIG_H_
#define WIFI_CONFIG_H_

#include "cy_wcm.h"
#include "secrets.h"

#define WIFI_SSID                         SECRET_WIFI_SSID
#define WIFI_PASSWORD                     SECRET_WIFI_PASSWORD
#define WIFI_SECURITY                     CY_WCM_SECURITY_WPA2_AES_PSK
#define MAX_WIFI_CONN_RETRIES             ( 120u )
#define WIFI_CONN_RETRY_INTERVAL_MS       ( 5000 )

#endif /* WIFI_CONFIG_H_ */
