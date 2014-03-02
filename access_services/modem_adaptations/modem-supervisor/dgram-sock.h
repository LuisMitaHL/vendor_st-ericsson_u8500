/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef dgram_sock_h
#define dgram_sock_h (1)

#define BINNAME "MSUP"
int syslog_trace;
int printf_trace;

#ifdef HAVE_ANDROID_OS
#define LOG_TAG BINNAME
#include <cutils/log.h>
#define OPENLOG(facility) ((void)0)
#define EXTRADEBUG(fmt , __args__...) \
		do { \
				if (printf_trace) \
					ALOGD(fmt, ##__args__); \
			} while (0)
#else
#include <syslog.h>
#define SYSLOG(prio, fmt , __args__...) \
	do { \
		if (syslog_trace) \
			syslog(prio, "%s: " fmt "\r\n", __func__, ##__args__); \
		if (printf_trace) \
			printf("%s: " fmt "\r\n", __func__, ##__args__); \
	} while (0)

#define OPENLOG(facility) openlog(BINNAME, LOG_PID | LOG_CONS, facility)
#define LOG(priority, format, ...) SYSLOG(priority, format, ##__VA_ARGS__)
#define ALOGV(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGD(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define ALOGI(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGW(format, ...)   LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define ALOGE(format, ...)   LOG(LOG_ERR, format, ##__VA_ARGS__)
#define EXTRADEBUG(format, ...)  LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#endif

typedef enum {
	DGRAM_SOCK_SERVER,
	DGRAM_SOCK_CLIENT
} dgram_sock_type_t;


void *dgram_sock_init(dgram_sock_type_t type, char *name);
int dgram_sock_get_fd(void *dgram_p);
void dgram_sock_destroy(void *dgram_p);
int dgram_sock_send(void *dgram_p, char *msg, int len);
int dgram_sock_recv(void *dgram_p, char *msg, int msg_len);

#endif /* dgram_sock_h */
