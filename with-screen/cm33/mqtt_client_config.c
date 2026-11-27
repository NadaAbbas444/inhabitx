/*
 * mqtt_client_config.c - InhabitX main on the E84: broker, TLS and last will.
 *
 * Replaces mqtt_client_config.c of Infineon's PSOC Edge Wi-Fi MQTT client
 * example (same global variables, used by its mqtt_task.c). Differences:
 * no client certificate, and a retained last will (main_rooms.h).
 */
#include "mqtt_client_config.h"
#include "cy_mqtt_api.h"
#include "main_rooms.h"

cy_mqtt_broker_info_t broker_info =
{
    .hostname = MQTT_BROKER_ADDRESS,
    .hostname_len = sizeof(MQTT_BROKER_ADDRESS) - 1,
    .port = MQTT_PORT
};

static cy_awsport_ssl_credentials_t credentials =
{
    .client_cert = NULL,
    .client_cert_size = 0,
    .private_key = NULL,
    .private_key_size = 0,
    .root_ca = (const char *)ROOT_CA_CERTIFICATE,
    .root_ca_size = sizeof(ROOT_CA_CERTIFICATE),
    .alpnprotos = NULL,
    .alpnprotoslen = 0,
    .sni_host_name = (const char *)MQTT_SNI_HOSTNAME,
    .sni_host_name_size = sizeof(MQTT_SNI_HOSTNAME)
};
cy_awsport_ssl_credentials_t *security_info = &credentials;

static cy_mqtt_publish_info_t will_msg_info =
{
    .qos = CY_MQTT_QOS1,
    .retain = true,         /* stays on inhabitx/Z/rooms until the main is back */
    .dup = false,
    .topic = MAIN_WILL_TOPIC,
    .topic_len = (uint16_t)(sizeof(MAIN_WILL_TOPIC) - 1),
    .payload = MAIN_WILL_PAYLOAD,
    .payload_len = sizeof(MAIN_WILL_PAYLOAD) - 1
};

cy_mqtt_connect_info_t connection_info =
{
    .client_id = NULL,
    .client_id_len = 0,
    .username = NULL,
    .username_len = 0,
    .password = NULL,
    .password_len = 0,
    .clean_session = true,
    .keep_alive_sec = MQTT_KEEP_ALIVE_SECONDS,
    .will_info = &will_msg_info
};
