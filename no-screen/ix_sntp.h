/*
 * ix_sntp.h - the time from the internet (SNTP), for the Wi-Fi core.
 */
#ifndef IX_SNTP_H_
#define IX_SNTP_H_

#include <time.h>

/* Asks pool.ntp.org (then two others), up to ~3 s each.
 * Returns the time (UTC, seconds since 1970), or 0 if none answered.
 * Only after the broker connection is up (secure sockets initialised). */
time_t ix_sntp_get(void);

#endif /* IX_SNTP_H_ */
