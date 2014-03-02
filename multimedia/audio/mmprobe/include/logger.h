/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include "../include/utils.h"
#include "../include/common.h"

#ifdef LOG_PRI
#undef LOG_PRI
#endif

#ifdef ANDROID

#define LOG_TAG "MMPROBE_SERVER"
#include <cutils/log.h>

#define L_INFO    LOG_INFO
#define L_WARN    LOG_WARN
#define L_ERR     LOG_ERROR
#define L_VERBOSE LOG_VERBOSE
#define L_DEBUG   LOG_DEBUG

#ifdef LOG_INFO
#undef LOG_INFO
#endif

#define start_log(appname, logfile)

#define log_message(level, ...) ALOG(level, LOG_TAG, __VA_ARGS__)

#define stop_log()

#else /* ANDROID */

/**
 * Defines the used log levels.
 */
typedef enum LOGLEVELS {
	L_INFO = LOG_INFO,
	L_WARN = LOG_WARNING,
	L_ERR = LOG_ERR
}loglevel_t;

/**
 * Starts the logger.
 */
boolean start_log(const char* appname, const char* logfile);

/**
 * Logs a message.
 */
void log_message(loglevel_t level, const char* logmessage, ...);

#if LOG_NDEBUG
#define ALOGV(...) log_message(LOG_INFO, __VA_ARGS__)
#else
#define ALOGV(...)   ((void)0)
#endif

/**
 * Stops the logger.
 */
void stop_log(void);

#endif /* ANDROID*/

#endif /* LOGGER_H_ */
