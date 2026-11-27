/*
 * mqtt_client_config.h - InhabitX node: broker settings.
 *
 * Replaces mqtt_client_config.h of Infineon's PSOC Edge Wi-Fi MQTT client
 * example. Same macro names, InhabitX values: HiveMQ Cloud over TLS, login
 * "devices", one topic for this room. Passwords come from secrets.h, which
 * is not committed (copy secrets.h.example to secrets.h).
 */
#ifndef MQTT_CLIENT_CONFIG_H_
#define MQTT_CLIENT_CONFIG_H_

#include "cy_mqtt_api.h"
#include "secrets.h"
#include "node_message.h"

/* ---- broker ------------------------------------------------------------ */
#define MQTT_BROKER_ADDRESS               "inhabitx-46341c9d.a03.euc1.aws.hivemq.cloud"
#define MQTT_PORT                         8883
#define MQTT_SECURE_CONNECTION            ( 1 )
#define MQTT_USERNAME                     "devices"
#define MQTT_PASSWORD                     SECRET_MQTT_PASSWORD

/* ---- topics ------------------------------------------------------------ */
#define MQTT_PUB_TOPIC                    NODE_TOPIC
/* The example's subscriber task still subscribes to something; nothing is
 * sent on this topic, so it stays quiet. */
#define MQTT_SUB_TOPIC                    "inhabitx/" NODE_BUILDING "/" NODE_FLOOR "/" NODE_ROOM "/cmd"
#define MQTT_MESSAGES_QOS                 ( 0 )

/* No last will: if the node goes silent, the main shows the room offline after 10 s. */
#define ENABLE_LWT_MESSAGE                ( 0 )

/* Used by the example's subscriber task (LED on/off); not used by InhabitX. */
#define MQTT_DEVICE_ON_MESSAGE            "TURN ON"
#define MQTT_DEVICE_OFF_MESSAGE           "TURN OFF"

/* ---- client ------------------------------------------------------------ */
#define MQTT_CLIENT_IDENTIFIER            NODE_CLIENT_ID
#define GENERATE_UNIQUE_CLIENT_ID         ( 0 )   /* keep the fixed id above */
#define MQTT_CLIENT_IDENTIFIER_MAX_LEN    ( 23 )
#define MQTT_TIMEOUT_MS                   ( 5000 )
#define MQTT_KEEP_ALIVE_SECONDS           ( 60 )
#define MQTT_NETWORK_BUFFER_SIZE          ( 2 * CY_MQTT_MIN_NETWORK_BUFFER_SIZE )
#define MAX_MQTT_CONN_RETRIES             ( 150u )
#define MQTT_CONN_RETRY_INTERVAL_MS       ( 2000 )

/* ---- TLS ----------------------------------------------------------------
 * HiveMQ Cloud checks the password, not a client certificate, so
 * CLIENT_CERTIFICATE and CLIENT_PRIVATE_KEY must stay undefined (the
 * example's placeholders would break the handshake).
 * The broker's certificate chains up to ISRG Root X1 (Let's Encrypt), valid
 * until 2035; same as ../isrg-root-x1.pem. SNI is required by HiveMQ Cloud. */
#define MQTT_SNI_HOSTNAME                 MQTT_BROKER_ADDRESS

#define ROOT_CA_CERTIFICATE \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----\n"

extern cy_mqtt_broker_info_t broker_info;
extern cy_awsport_ssl_credentials_t  *security_info;
extern cy_mqtt_connect_info_t connection_info;

#endif /* MQTT_CLIENT_CONFIG_H_ */
