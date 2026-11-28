/*
 * ix_sntp.c - the time from the internet (SNTP), for the Wi-Fi core.
 *
 * One request to an NTP server over UDP with Infineon's secure-sockets
 * library (already set up by the MQTT connection). Used once the board is
 * on Wi-Fi, so the main knows the date and time without any node sending it.
 */
#include "ix_sntp.h"
#include "cy_secure_sockets.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NTP_PORT            (123U)
#define NTP_TIMEOUT_MS      (3000U)
#define NTP_TO_UNIX         (2208988800UL)  /* seconds from 1900 to 1970 */

static const char *const s_servers[] = { "pool.ntp.org", "time.google.com", "time.cloudflare.com" };

static time_t ask(const char *server)
{
    cy_socket_ip_address_t ip;
    memset(&ip, 0, sizeof ip);
    if (cy_socket_gethostbyname(server, CY_SOCKET_IP_VER_V4, &ip) != CY_RSLT_SUCCESS)
    {
        return 0;
    }

    cy_socket_t s;
    if (cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, &s) != CY_RSLT_SUCCESS)
    {
        return 0;
    }
    uint32_t timeout = NTP_TIMEOUT_MS;
    (void)cy_socket_setsockopt(s, CY_SOCKET_SOL_SOCKET, CY_SOCKET_SO_RCVTIMEO, &timeout, sizeof timeout);

    uint8_t pkt[48];
    memset(pkt, 0, sizeof pkt);
    pkt[0] = 0x1B;                          /* version 3, client */
    cy_socket_sockaddr_t to;
    memset(&to, 0, sizeof to);
    to.port = NTP_PORT;
    to.ip_address = ip;

    time_t t = 0;
    uint32_t n = 0;
    if (cy_socket_sendto(s, pkt, sizeof pkt, CY_SOCKET_FLAGS_NONE, &to, sizeof to, &n) == CY_RSLT_SUCCESS && n == sizeof pkt)
    {
        n = 0;
        if (cy_socket_recvfrom(s, pkt, sizeof pkt, CY_SOCKET_FLAGS_RECVFROM_NONE, NULL, NULL, &n) == CY_RSLT_SUCCESS
            && n >= 48 && (pkt[0] & 0x07) == 4 && pkt[1] != 0)     /* server answer, synchronised */
        {
            uint32_t secs = ((uint32_t)pkt[40] << 24) | ((uint32_t)pkt[41] << 16) |
                            ((uint32_t)pkt[42] << 8) | (uint32_t)pkt[43];
            if (secs > NTP_TO_UNIX)
            {
                t = (time_t)(secs - NTP_TO_UNIX);
            }
        }
    }
    (void)cy_socket_delete(s);
    return t;
}

time_t ix_sntp_get(void)
{
    for (size_t i = 0; i < sizeof s_servers / sizeof s_servers[0]; i++)
    {
        time_t t = ask(s_servers[i]);
        if (t > 1700000000)
        {
            printf("sntp: time from %s\n", s_servers[i]);
            return t;
        }
    }
    printf("sntp: no answer from the time servers (UDP port 123 blocked?), trying again later\n");
    return 0;
}
