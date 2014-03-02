/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <rt.nmf>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define LOG_TAG "HostEE"
#include "log-utils.h"

extern void construct(void);
extern void start(void);
extern void stop(void);
extern void destroy(void);

__attribute__ ((constructor)) void beforeMain()
{
    construct();
    start();
}

__attribute__ ((destructor)) void afterMain(void)
{
    stop();
    destroy();
}

EXPORT_SHARED void NMF_LOG(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
#ifdef ANDROID
    LOG_PRI_VA(ANDROID_LOG_WARN, LOG_TAG, fmt, args);
#else
    vprintf(fmt, args);
#endif
    va_end(args);
}

EXPORT_SHARED void NMF_PANIC(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
#ifdef ANDROID
    LOG_PRI_VA(ANDROID_LOG_ERROR, LOG_TAG, fmt, args);
#else
    vfprintf(stderr, fmt, args);
#endif
    va_end(args);

    exit(-1);
}
