#ifndef DEBUG_H
#define DEBUG_H
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifndef BINARY_PREFIX
/* Let's have BASS (bass_app) as default prefix. */
#define BINARY_PREFIX "BASS"
#endif

/*
 * Debug levels.
 *
 * CRITICAL is used when something really bad happens.
 *
 * ERROR is used when some kind of error has happened, this is most likely the
 * print you will use most of the time when you report some kind of error.
 *
 * ALWAYS is used when you always want a print to be seen, but it is not any
 * error.
 *
 * INFO is used when you want to debug print some text that is not enabled by
 * default.
 *
 * FLOOD is used when you want to debug print some text that typically floods
 * the print buffer.
 */
#define CRITICAL 0
#define ERROR 1
#define ALWAYS 2
#define INFO 3
#define FLOOD 4

#if defined(DEBUGLEVEL_0) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL ERROR
#endif

#if defined(DEBUGLEVEL_1) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL CRITICAL
#endif

#if defined(DEBUGLEVEL_2) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL ALWAYS
#endif

#if defined(DEBUGLEVEL_3) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL INFO
#endif

/*
 * If someone has been using BASS_APP_DEBUG_PRINT=true on commandline while
 * building we want it to be considered as INFO.
 */
#if defined(DEBUGLEVEL_true) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL INFO
#endif

#if defined(DEBUGLEVEL_4) && !defined(DEBUGLEVEL)
#define DEBUGLEVEL FLOOD
#endif

#ifndef DEBUGLEVEL
/* Default debug level. */
#define DEBUGLEVEL ALWAYS
#endif

/*
 * This define make sure that parameters are checked in the same manner as it
 * is done in the normal printf function.
 */
#define __PRINTFLIKE(__fmt, __varargs) __attribute__\
    ((__format__(__printf__, __fmt, __varargs)))

int _dprintf(const char *function, int flen, int line, int level,
             const char *prefix, const char *fmt, ...) __PRINTFLIKE(6, 7);

#define dprintf(level, x...) do { \
        if ((level) <= DEBUGLEVEL) { \
            _dprintf(__func__, strlen(__func__), __LINE__, level, \
                     BINARY_PREFIX, x); \
        } \
    } while (0)

/*
 * This function will hex and ascii dump a buffer.
 *
 * Note that this function will only print if debug flag
 * DEBUGLEVEL is INFO or FLOOD.
 *
 * @param bname     Information string describing the buffer.
 * @param buffer    Pointer to the buffer.
 * @param blen      Length of the buffer.
 *
 * @return void
 */
void dump_buffer(const char *bname, const uint8_t *buffer, size_t blen);
#endif
