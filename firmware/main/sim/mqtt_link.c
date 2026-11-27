/* MQTT connection of the PC simulator. See mqtt_link.h. */
#include "mqtt_link.h"

#include "core_mqtt.h"

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define CONNECT_TIMEOUT_MS  5000
#define RETRY_MS            5000
#define KEEPALIVE_S         30

/* ---- config -------------------------------------------------------------- */

static void trim(char *s)
{
    char *p = s;
    while (isspace((unsigned char)*p)) p++;
    memmove(s, p, strlen(p) + 1);
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
}

static void set(mqtt_link_conf_t *c, const char *k, const char *v)
{
    if      (!strcmp(k, "host"))     snprintf(c->host, sizeof c->host, "%s", v);
    else if (!strcmp(k, "port"))     c->port = atoi(v);
    else if (!strcmp(k, "user"))     snprintf(c->user, sizeof c->user, "%s", v);
    else if (!strcmp(k, "password")) snprintf(c->password, sizeof c->password, "%s", v);
    else if (!strcmp(k, "cafile"))   snprintf(c->cafile, sizeof c->cafile, "%s", v);
}

bool mqtt_link_load_conf(const char *path, mqtt_link_conf_t *c, char *why, size_t n)
{
    memset(c, 0, sizeof *c);
    c->port = 8883;
    FILE *f = path ? fopen(path, "r") : NULL;
    if (f) {
        char line[512];
        while (fgets(line, sizeof line, f)) {
            trim(line);
            if (!line[0] || line[0] == '#') continue;
            char *eq = strchr(line, '=');
            if (!eq) continue;
            *eq = 0;
            trim(line);
            trim(eq + 1);
            set(c, line, eq + 1);
        }
        fclose(f);
    }
    static const char *env[][2] = {
        { "IX_MQTT_HOST", "host" }, { "IX_MQTT_PORT", "port" }, { "IX_MQTT_USER", "user" },
        { "IX_MQTT_PASSWORD", "password" }, { "IX_MQTT_CAFILE", "cafile" },
    };
    for (size_t i = 0; i < sizeof env / sizeof env[0]; i++) {
        const char *v = getenv(env[i][0]);
        if (v && *v) set(c, env[i][1], v);
    }
    const char *miss = !c->host[0] ? "host" : !c->user[0] ? "user" : !c->password[0] ? "password" : NULL;
    if (miss) {
        snprintf(why, n, "no %s in %s%s or IX_MQTT_* variables", miss, path ? path : "(no file)",
                 f ? "" : " (file not found)");
        return false;
    }
    if (c->port <= 0 || c->port > 65535) { snprintf(why, n, "bad port"); return false; }
    return true;
}

/* ---- TLS transport for coreMQTT ------------------------------------------ */

struct NetworkContext { int fd; SSL *ssl; };

static SSL_CTX *s_ctx;
static NetworkContext_t s_net = { -1, NULL };

static int32_t tls_recv(NetworkContext_t *nc, void *buf, size_t n)
{
    int r = SSL_read(nc->ssl, buf, (int)n);
    if (r > 0) return r;
    int e = SSL_get_error(nc->ssl, r);
    if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) return 0;
    return -1;
}

static int32_t tls_send(NetworkContext_t *nc, const void *buf, size_t n)
{
    int r = SSL_write(nc->ssl, buf, (int)n);
    if (r > 0) return r;
    int e = SSL_get_error(nc->ssl, r);
    if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) return 0;
    return -1;
}

static void tls_close(void)
{
    if (s_net.ssl) { SSL_shutdown(s_net.ssl); SSL_free(s_net.ssl); s_net.ssl = NULL; }
    if (s_net.fd >= 0) { close(s_net.fd); s_net.fd = -1; }
}

static bool tcp_connect(const char *host, int port, char *why, size_t n)
{
    char ps[8];
    snprintf(ps, sizeof ps, "%d", port);
    struct addrinfo hints = { .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM }, *res, *ai;
    int g = getaddrinfo(host, ps, &hints, &res);
    if (g) { snprintf(why, n, "lookup failed: %s", gai_strerror(g)); return false; }
    for (ai = res; ai; ai = ai->ai_next) {
        int fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) continue;
        fcntl(fd, F_SETFL, O_NONBLOCK);
        if (connect(fd, ai->ai_addr, ai->ai_addrlen) == 0 || errno == EINPROGRESS) {
            struct pollfd p = { fd, POLLOUT, 0 };
            int err = 0;
            socklen_t el = sizeof err;
            if (poll(&p, 1, CONNECT_TIMEOUT_MS) == 1 && getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &el) == 0 && !err) {
                s_net.fd = fd;
                freeaddrinfo(res);
                return true;
            }
            snprintf(why, n, "connect: %s", err ? strerror(err) : "timeout");
        } else {
            snprintf(why, n, "connect: %s", strerror(errno));
        }
        close(fd);
    }
    freeaddrinfo(res);
    return false;
}

/* Handshake on the non-blocking socket, waiting at most CONNECT_TIMEOUT_MS. */
static bool tls_connect(const mqtt_link_conf_t *c, char *why, size_t n)
{
    if (!s_ctx) {
        s_ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_min_proto_version(s_ctx, TLS1_2_VERSION);
        SSL_CTX_set_verify(s_ctx, SSL_VERIFY_PEER, NULL);
        if (c->cafile[0] ? !SSL_CTX_load_verify_locations(s_ctx, c->cafile, NULL)
                         : !SSL_CTX_set_default_verify_paths(s_ctx)) {
            snprintf(why, n, "can't load CA certificates");
            SSL_CTX_free(s_ctx);
            s_ctx = NULL;
            return false;
        }
    }
    if (!tcp_connect(c->host, c->port, why, n)) return false;
    s_net.ssl = SSL_new(s_ctx);
    SSL_set_mode(s_net.ssl, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
    SSL_set_tlsext_host_name(s_net.ssl, c->host);       /* SNI: HiveMQ Cloud needs it */
    SSL_set1_host(s_net.ssl, c->host);                  /* certificate must be for this host */
    SSL_set_fd(s_net.ssl, s_net.fd);
    for (;;) {
        int r = SSL_connect(s_net.ssl);
        if (r == 1) return true;
        int e = SSL_get_error(s_net.ssl, r);
        struct pollfd p = { s_net.fd, e == SSL_ERROR_WANT_WRITE ? POLLOUT : POLLIN, 0 };
        if ((e != SSL_ERROR_WANT_READ && e != SSL_ERROR_WANT_WRITE) || poll(&p, 1, CONNECT_TIMEOUT_MS) != 1) {
            long v = SSL_get_verify_result(s_net.ssl);
            if (v != X509_V_OK) snprintf(why, n, "TLS: certificate not accepted (%s)", X509_verify_cert_error_string(v));
            else snprintf(why, n, "TLS handshake failed");
            tls_close();
            return false;
        }
    }
}

/* ---- MQTT ---------------------------------------------------------------- */

static mqtt_link_conf_t   s_conf;
static char               s_client_id[64], s_sub[128], s_will_topic[128];
static char               s_will_payload[4096];
static mqtt_link_msg_cb_t s_on_msg;
static mqtt_link_up_cb_t  s_on_up;
static MQTTContext_t      s_mqtt;
static uint8_t            s_buf[16 * 1024];
static MQTTPubAckInfo_t   s_out_rec[16], s_in_rec[16];
static bool               s_started, s_up, s_in_loop;
static uint32_t           s_last_try;
static bool               s_tried;

/* replies published from inside the callback wait here until it returns */
#define QUEUE_LEN 8
static struct { char topic[128]; char payload[256]; size_t len; bool retain; } s_q[QUEUE_LEN];
static int s_qn;

static uint32_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static void event_cb(MQTTContext_t *ctx, MQTTPacketInfo_t *pkt, MQTTDeserializedInfo_t *info)
{
    uint8_t type = pkt->type & 0xF0U;
    if (type == MQTT_PACKET_TYPE_PUBLISH && info->pPublishInfo && s_on_msg) {
        const MQTTPublishInfo_t *p = info->pPublishInfo;
        s_on_msg(p->pTopicName, p->topicNameLength, p->pPayload, p->payloadLength);
    } else if (type == MQTT_PACKET_TYPE_SUBACK) {
        uint8_t *codes;
        size_t nc;
        if (MQTT_GetSubAckStatusCodes(pkt, &codes, &nc) == MQTTSuccess && nc && codes[0] == 0x80)
            printf("mqtt: broker refused the subscription to %s (check the login's permissions)\n", s_sub);
    }
}

static uint32_t s_up_since;

static void drop(const char *why)
{
    if (s_up && now_ms() - s_up_since < 60000)
        printf("mqtt: connection lost again after a short time. Is another main running with the same login?\n");
    if (s_up) printf("mqtt: connection lost (%s), retrying every %d s\n", why, RETRY_MS / 1000);
    s_up = false;
    tls_close();
    s_last_try = now_ms();
}

static void try_connect(void)
{
    char why[160] = "";
    s_tried = true;
    s_last_try = now_ms();
    if (!tls_connect(&s_conf, why, sizeof why)) {
        printf("mqtt: can't reach %s:%d: %s\n", s_conf.host, s_conf.port, why);
        return;
    }
    TransportInterface_t tr = { .recv = tls_recv, .send = tls_send, .writev = NULL, .pNetworkContext = &s_net };
    MQTTFixedBuffer_t fb = { s_buf, sizeof s_buf };
    MQTT_Init(&s_mqtt, &tr, now_ms, event_cb, &fb);
    MQTT_InitStatefulQoS(&s_mqtt, s_out_rec, 16, s_in_rec, 16);

    MQTTConnectInfo_t ci = {
        .cleanSession = true, .keepAliveSeconds = KEEPALIVE_S,
        .pClientIdentifier = s_client_id, .clientIdentifierLength = (uint16_t)strlen(s_client_id),
        .pUserName = s_conf.user, .userNameLength = (uint16_t)strlen(s_conf.user),
        .pPassword = s_conf.password, .passwordLength = (uint16_t)strlen(s_conf.password),
    };
    MQTTPublishInfo_t will = {
        .qos = MQTTQoS1, .retain = true,
        .pTopicName = s_will_topic, .topicNameLength = (uint16_t)strlen(s_will_topic),
        .pPayload = s_will_payload, .payloadLength = strlen(s_will_payload),
    };
    bool session;
    MQTTStatus_t st = MQTT_Connect(&s_mqtt, &ci, s_will_topic[0] ? &will : NULL, CONNECT_TIMEOUT_MS, &session);
    if (st != MQTTSuccess) {
        printf("mqtt: broker said no: %s%s\n", MQTT_Status_strerror(st),
               st == MQTTServerRefused ? " (wrong user or password?)" : "");
        tls_close();
        return;
    }
    MQTTSubscribeInfo_t sub = { .qos = MQTTQoS1, .pTopicFilter = s_sub, .topicFilterLength = (uint16_t)strlen(s_sub) };
    if (MQTT_Subscribe(&s_mqtt, &sub, 1, MQTT_GetPacketId(&s_mqtt)) != MQTTSuccess) {
        printf("mqtt: subscribe failed\n");
        tls_close();
        return;
    }
    s_up = true;
    s_up_since = now_ms();
    printf("mqtt: connected to %s as %s, listening on %s\n", s_conf.host, s_conf.user, s_sub);
    if (s_on_up) s_on_up();
}

void mqtt_link_start(const mqtt_link_conf_t *c, const char *client_id, const char *sub_filter,
                     const char *will_topic, const char *will_payload,
                     mqtt_link_msg_cb_t on_msg, mqtt_link_up_cb_t on_up)
{
    snprintf(s_will_topic, sizeof s_will_topic, "%s", will_topic && will_payload ? will_topic : "");
    snprintf(s_will_payload, sizeof s_will_payload, "%s", will_payload ? will_payload : "");
    s_conf = *c;
    snprintf(s_client_id, sizeof s_client_id, "%s", client_id);
    snprintf(s_sub, sizeof s_sub, "%s", sub_filter);
    s_on_msg = on_msg;
    s_on_up = on_up;
    s_started = true;
    /* a write to a connection the broker just closed must fail, not kill us */
    signal(SIGPIPE, SIG_IGN);
}

bool mqtt_link_connected(void) { return s_up; }

static bool publish_now(const char *topic, const void *payload, size_t len, bool retain)
{
    MQTTPublishInfo_t p = {
        .qos = MQTTQoS1, .retain = retain,
        .pTopicName = topic, .topicNameLength = (uint16_t)strlen(topic),
        .pPayload = payload, .payloadLength = len,
    };
    MQTTStatus_t st = MQTT_Publish(&s_mqtt, &p, MQTT_GetPacketId(&s_mqtt));
    if (st != MQTTSuccess) { drop(MQTT_Status_strerror(st)); return false; }
    return true;
}

bool mqtt_link_publish(const char *topic, const char *payload, size_t len, bool retain)
{
    if (!s_up) return false;
    if (!s_in_loop) return publish_now(topic, payload, len, retain);
    if (s_qn >= QUEUE_LEN || len > sizeof s_q[0].payload || strlen(topic) >= sizeof s_q[0].topic) return false;
    snprintf(s_q[s_qn].topic, sizeof s_q[0].topic, "%s", topic);
    memcpy(s_q[s_qn].payload, payload, len);
    s_q[s_qn].len = len;
    s_q[s_qn].retain = retain;
    s_qn++;
    return true;
}

void mqtt_link_poll(void)
{
    if (!s_started) return;
    if (!s_up) {
        if (!s_tried || now_ms() - s_last_try >= RETRY_MS) try_connect();
        return;
    }
    s_in_loop = true;
    MQTTStatus_t st = MQTT_ProcessLoop(&s_mqtt);
    s_in_loop = false;
    if (st != MQTTSuccess && st != MQTTNeedMoreBytes) { s_qn = 0; drop(MQTT_Status_strerror(st)); return; }
    for (int i = 0; i < s_qn && s_up; i++) publish_now(s_q[i].topic, s_q[i].payload, s_q[i].len, s_q[i].retain);
    s_qn = 0;
}
