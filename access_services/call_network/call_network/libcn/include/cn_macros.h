/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Macro definitions that standardize CN implementation
 * Its main purpose is to reduce the visible lines of code
 * needed for error handling in a function (so that doesn't
 * fill up half the function).
 */

#ifndef __cn_macros_h__
#define __cn_macros_h__ (1)

#include "cn_log.h"

#define NULL_CHECK_AND_GOTO_EXIT(pointer, pointer_name) if (!(pointer))  { CN_LOG_E("%s is NULL!", (pointer_name)); goto exit;}
#define NULL_CHECK_AND_GOTO_ERROR(pointer, pointer_name) if (!(pointer)) { CN_LOG_E("%s is NULL!", (pointer_name)); goto error;}

#define CN_ASSERT(e) if (!(e)) { int line = __LINE__; \
                         CN_LOG_E("CN_ASSERT failed! File: %s line: %d", __FILE__, line); \
                         return REQUEST_STATUS_ERROR; \
                     }

/**
 * Macro that return the smaller value of two values.
 *
 * This version of the macro is double-evaluation safe, e.g.
 * calls similar to MIN(++a, b++) are safe and behave as expected.
 */
#define CN_MIN(x, y) ({ \
            __typeof__ (x) _x = (x); \
            __typeof__ (y) _y = (y); \
            _x < _y ? _x : _y; })

/**
 * Macro that return the larger value of two values.
 *
 * This version of the macro is double-evaluation safe, e.g.
 * calls similar to MAX(++a, b++) are safe and behave as expected.
 */
#define CN_MAX(x, y) ({ \
            __typeof__ (x) _x = (x); \
            __typeof__ (y) _y = (y); \
            _x > _y ? _x : _y; })


/**
 * Macros that convert 16-bit words to/from host to/from network byte order.
 *
 * This macro depends on the compiler defining a macro to a specific value.
 * In the case of GCC it sets the macro __BYTE_ORDER__ to either
 * __ORDER_LITTLE_ENDIAN__ or __ORDER_BIG_ENDIAN__ depending on the byte
 * order of the target platform.
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CN_HTONS(x)            (((x & 0xFF) << 8) | (x >> 8))
#define CN_NTOHS(x)            (((x & 0xFF) << 8) | (x >> 8))
#else
#define CN_HTONS(x)            (x)
#define CN_NTOHS(x)            (x)
#endif

#endif /* __cn_macros_h__ */
