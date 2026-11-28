#pragma once
#include <WiFi.h>
#include <NetworkClientSecure.h>
#include <time.h>
#include "src/mqtt/PubSubClient.h"
#include "InhabitXConfig.h"
#include "BrokerCertificate.h"
#include "NodeMessage.h"
#include "SerialConfig.h"

// Only this task owns Wi-Fi/TLS/MQTT. Inference overwrites a one-item mailbox.
// TLS and DNS can block here without holding the model or capture task.
static QueueHandle_t nodeQueue;

static void networkTask(void*) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (!SECRET_WIFI_SSID[0] || !SECRET_MQTT_PASSWORD[0]) {
        Serial.println("InhabitX: offline sensing; copy secrets.h.example to secrets.h and fill credentials.");
        vTaskDelete(nullptr);
        return;
    }
    NetworkClientSecure tls;
    tls.setCACert(INHABITX_ROOT_CA);
    tls.setHandshakeTimeout(5);
    tls.setConnectionTimeout(5000);
    tls.setTimeout(2000);
    PubSubClient mqtt(tls);
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    mqtt.setKeepAlive(15);
    mqtt.setSocketTimeout(2);

    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
    uint32_t lastWifiTry = millis();
    uint32_t lastMqttTry = millis() - NODE_RECONNECT_MS;
    uint32_t lastSend = millis() - NODE_SEND_EVERY_MS;
    uint32_t lastNtpTry = 0;
    bool wifiWasConnected = false, clockReady = false, ntpWarningPrinted = false;
    bool hadValidState = false;
    uint32_t lastConfirmationId = 0;
    for (;;) {
        uint32_t now = millis();
        if (WiFi.status() != WL_CONNECTED) {
            if (wifiWasConnected) Serial.println("Wi-Fi: disconnected; reconnecting");
            wifiWasConnected = false;
            if (mqtt.connected()) mqtt.disconnect();
            tls.stop();
            if (uint32_t(now - lastWifiTry) >= 15000) {
                lastWifiTry = now;
                WiFi.reconnect();
                Serial.println("InhabitX: retrying Wi-Fi");
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (!wifiWasConnected) {
            wifiWasConnected = true;
            Serial.printf("Wi-Fi: connected | IP %s\n", WiFi.localIP().toString().c_str());
            // Start SNTP only after an IP is available. Restart after reconnects
            // so an early failed DNS/request does not leave us waiting on backoff.
            if (time(nullptr) < 1700000000) {
                configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
                lastNtpTry = millis();
                ntpWarningPrinted = false;
                Serial.println("Clock: syncing internet time...");
            }
        }
        now = millis();
        if (time(nullptr) < 1700000000) {
            clockReady = false;
            if (uint32_t(now - lastNtpTry) >= NTP_RETRY_MS) {
                configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
                lastNtpTry = millis();
                if (!ntpWarningPrinted || SERIAL_VERBOSE)
                    Serial.println("Clock: no NTP reply; check internet / UDP 123. Retrying every 60s.");
                ntpWarningPrinted = true;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (!clockReady) {
            clockReady = true;
            Serial.println("Clock: ready; connecting MQTT");
        }
        if (!mqtt.connected()) {
            if (uint32_t(now - lastMqttTry) >= NODE_RECONNECT_MS) {
                lastMqttTry = now;
                tls.stop();
                if (mqtt.connect(NODE_CLIENT_ID, MQTT_USER, SECRET_MQTT_PASSWORD)) {
                    Serial.printf("MQTT: connected | room %s\n", NODE_ROOM);
                    lastSend = millis() - NODE_SEND_EVERY_MS;
                } else {
                    Serial.printf("InhabitX: MQTT connect failed (%d); retrying\n", mqtt.state());
                }
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (!mqtt.loop()) {
            tls.stop();
            continue;
        }
        // Fetch AFTER potentially blocking network operations. Never replay old states.
        NodeSnapshot snapshot = {};
        now = millis();
        const bool valid = xQueuePeek(nodeQueue, &snapshot, 0) == pdTRUE &&
            nodeSnapshotValid(snapshot, now, __atomic_load_n(&streamEpoch, __ATOMIC_RELAXED),
                              NODE_RESULT_MAX_AGE_MS);
        if (!valid) {
            if (hadValidState) Serial.println("MQTT: paused (occupancy unknown)");
            hadValidState = false;
        } else if (!hadValidState || snapshot.confirmationId != lastConfirmationId || uint32_t(now - lastSend) >= NODE_SEND_EVERY_MS) {
            char payload[256];
            const size_t length = nodeMessage(NODE_ROOM, snapshot.state, payload, sizeof payload, snapshot.userConfirmed);
            if (length && mqtt.publish(NODE_TOPIC, reinterpret_cast<const uint8_t*>(payload), length, false)) {
                lastSend = millis();
                hadValidState = true;
                if (snapshot.userConfirmed && snapshot.confirmationId != lastConfirmationId)
                    Serial.println(snapshot.state == RoomState::Occupied ?
                        "MQTT: sent user-confirmed OCCUPIED" : "MQTT: sent user-declared EMPTY");
                lastConfirmationId = snapshot.confirmationId;
                if (SERIAL_VERBOSE) Serial.printf("node: %s -> %s\n", NODE_TOPIC, payload);
            } else {
                // Force a new broker connection even if Wi-Fi is still up.
                mqtt.disconnect();
                tls.stop();
                hadValidState = false;
                Serial.println("InhabitX: publish failed; reconnecting");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
