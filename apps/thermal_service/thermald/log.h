/*
 * Thermal Service Logger
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __LOG_H__
#define __LOG_H_

#include <string.h>
#include <errno.h>

/**
 * Different log levels avaliable in platform
 */
typedef enum {
	LOG_LEVEL_DEFAULT = 0, /*< Show ERR/INF prints */
	LOG_LEVEL_WARNINGS, /*< Show ERR/INF/WRN prints */
	LOG_LEVEL_DEBUG /*< Show ERR/INF/WRN/DBG prints */
} LOG_levels_t;

extern int LOG_level;

/**
 * Sets the platform log level.
 *
 * This is deliberately left as an int as the user can specify this
 * from the comand line with -v, so it could be -vvvvvvv and thus would
 * break the LOG_levels_t enum.
 *
 * @param level Platform log level
 */
void log_level(const int level);

#ifdef ANDROID_BUILD
#ifndef LOG_TAG
#define LOG_TAG "thermald"
#endif
#include <cutils/log.h>
#define ERR(fmt, ...) ALOGE("[%s:%d] " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define WRN(fmt, ...) if (LOG_level >= LOG_LEVEL_WARNINGS) { ALOGW("[%s:%d] " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__); }
#define DBG(fmt, ...) if (LOG_level >= LOG_LEVEL_DEBUG) { ALOGD("[%s:%d] " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__); }
#define INF(fmt, ...) ALOGI("%s(): " fmt, __FUNCTION__, ## __VA_ARGS__)
#else
#define WRN(fmt, ...) if (LOG_level >= LOG_LEVEL_WARNINGS) { (void) fprintf(stderr, "WRN[%s:%d] " fmt, __FUNCTION__, \
								     __LINE__, ## __VA_ARGS__); }

#define DBG(fmt, ...) if (LOG_level >= LOG_LEVEL_DEBUG) { (void) fprintf(stdout, "[%s:%d] " fmt, __FUNCTION__, \
								  __LINE__, ## __VA_ARGS__); }

#define INF(fmt, ...) (void) printf("%s(): "fmt, __FUNCTION__, ## __VA_ARGS__);

#define ERR(fmt, ...) (void) fprintf(stderr, "ERR[%s:%d] %s: " fmt ,\
					__FUNCTION__, __LINE__,\
					strerror(errno), ## __VA_ARGS__)
#endif
#endif

