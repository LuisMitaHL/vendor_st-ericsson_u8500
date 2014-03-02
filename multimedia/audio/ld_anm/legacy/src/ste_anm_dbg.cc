/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_dbg.cc
    \brief Debugging utilities.

    Implementation of debugging utilities.
*/
#include "ste_anm_dbg.h"

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <cutils/log.h>
#include "cutils/properties.h"

int log_state[STE_ANM_LOG_LAST];

char *log_prefix[STE_ANM_LOG_LAST] = {
    (char*)"INFO",
    (char*)"INFO_V",
    (char*)"INFO_FL",
    (char*)"WARN",
    (char*)"ERR",
};

/*! \def LOG_INFO_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef LOG_INFO
#define LOG_INFO_DEFINED 0
#else
#define LOG_INFO_DEFINED 1
#endif

/*! \def LOG_INFO_VERBOSE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef LOG_INFO_VERBOSE
#define LOG_INFO_VERBOSE_DEFINED 0
#else
#define LOG_INFO_VERBOSE_DEFINED 1
#endif

/*! \def LOG_INFO_FL_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef LOG_INFO_FL
#define LOG_INFO_FL_DEFINED 0
#else
#define LOG_INFO_FL_DEFINED 1
#endif

/*! \def LOG_WARN_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef LOG_WARNINGS
#define LOG_WARN_DEFINED 0
#else
#define LOG_WARN_DEFINED 1
#endif

/*! \def LOG_ERR_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef LOG_ERRORS
#define LOG_ERR_DEFINED 0
#else
#define LOG_ERR_DEFINED 1
#endif

/* ste_anm_debug_setup_dlog */
int ste_anm_debug_setup_log(void)
{
    log_state[STE_ANM_LOG_INFO] = LOG_INFO_DEFINED;
    log_state[STE_ANM_LOG_INFO_VERBOSE] = LOG_INFO_VERBOSE_DEFINED;
    log_state[STE_ANM_LOG_INFO_FL] = LOG_INFO_FL_DEFINED;
    log_state[STE_ANM_LOG_WARN] = LOG_WARN_DEFINED;
    log_state[STE_ANM_LOG_ERR] = LOG_ERR_DEFINED;

    char prop[PROPERTY_VALUE_MAX];
    property_get("ste.debug.anm.log", prop, "");
    prop[PROPERTY_VALUE_MAX-1]=0;
    const char* name_start=prop;
    while(*name_start) {
        const char* name_end  = name_start;
        while (*name_end && *name_end != ' ') name_end++;

        int log_type;
        for (log_type=0 ; log_type < STE_ANM_LOG_LAST; log_type++) {
            if (name_end-name_start == (int)strlen(log_prefix[log_type])) {
                if (strncasecmp(
                    log_prefix[log_type], name_start, name_end-name_start) == 0)
                {
                    log_state[log_type] = 1;
                }
            }
        }

        name_start = name_end;
        while (*name_start && *name_start == ' ') name_start++;
    }

    return 0;
}

#define BUFFER_LENGTH 2048
int ste_anm_debug_print(enum STE_ANM_LOGLEVEL level,
    const char *file, int line, const char *function, const char *fmt, ...)
{
    if (!log_state[level]) {
        return 0;
    }

    va_list ap;
    char buffer[BUFFER_LENGTH];
    char insert_newline[2] = {0, 0};
    (void) function;

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    if (buffer[strlen(buffer)-1] != '\n') {
        insert_newline[0] = '\n';
    }

    switch (level) {
    case STE_ANM_LOG_ERR:
        LOGE("[%.8s] %s:%d %s%s", log_prefix[level],
            file, line, buffer, insert_newline);
        break;
    case STE_ANM_LOG_WARN:
        LOGW("[%.8s] %s:%d %s%s", log_prefix[level],
            file, line, buffer, insert_newline);
        break;
    case STE_ANM_LOG_INFO:
    case STE_ANM_LOG_INFO_VERBOSE:
    default:
        LOGI("[%.8s] %s:%d %s%s", log_prefix[level],
            file, line, buffer, insert_newline);
        break;
    }

  return 0;
}
