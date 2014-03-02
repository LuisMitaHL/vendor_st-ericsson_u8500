/*-----------------------------------------------------------------------------
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *---------------------------------------------------------------------------*/

/*
 * This file provides what C99 standard requires in
 * 7.18 interger types <stdint.h>
 */

#ifndef STDINT_H
#define STDINT_H

/* 7.18.1.1 Exact-width interger types */

/* These typedefs are architecture dependent */

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
#ifdef HAVE_INT64_SUPPORT
typedef long long int64_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
#ifdef HAVE_INT64_SUPPORT
typedef unsigned long long uint64_t;
#endif


/* ssize_t doesn't really belong here, but it's convenient */
typedef int32_t ssize_t;

/* 7.18.1.2 Minimum-width integer types */

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
#ifdef HAVE_INT64_SUPPORT
typedef int64_t int_least64_t;
#endif
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
#ifdef HAVE_INT64_SUPPORT
typedef uint64_t uint_least64_t;
#endif

/* 7.18.1.3 Fastest minimum-width integer types */

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
#ifdef HAVE_INT64_SUPPORT
typedef int64_t int_fast64_t;
#endif
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
#ifdef HAVE_INT64_SUPPORT
typedef uint64_t uint_fast64_t;
#endif

/* 7.18.1.4 Integer types capable of holding object pointers */

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

/* 7.18.1.5 Greatest-width integer types */
#ifdef HAVE_INT64_SUPPORT
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
#else
typedef int32_t intmax_t;
typedef uint32_t uintmax_t;
#endif

/*
 * 7.18.2 Limits of specified-width integer types
 */

/* 7.18.2.1 Limits of exact-width interger types */

#define INT8_MIN    (-0x7f-1)
#define INT16_MIN   (-0x7fff-1)
#define INT32_MIN   (-0x7fffffff-1)
#define INT64_MIN   (-0x7fffffffffffffffL-1)

#define INT8_MAX    0x7f
#define INT16_MAX   0x7fff
#define INT32_MAX   0x7fffffff
#define INT64_MAX   0x7fffffffffffffffL

#define UINT8_MAX    0xff
#define UINT16_MAX   0xffff
#define UINT32_MAX   0xffffffffU
#define UINT64_MAX   0xffffffffffffffffUL

/* 7.18.2.2 Limits of minimum-width integer types */

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MIN INT64_MIN
#define INT_LEAST64_MAX INT64_MAX


#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* 7.18.2.3 Limits of fastest minimum-width integer types */

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MIN INT16_MIN
#define INT_FAST16_MAX INT16_MAX
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MIN INT64_MIN
#define INT_FAST64_MAX INT64_MAX


#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#define INTPTR_MIN  INT32_MIN
#define INTPTR_MAX  INT32_MAX
#define UINTPTR_MAX UINT32_MAX

/* 7.18.2.5 Limits of greatest-width integer types */

#ifdef HAVE_INT64_SUPPORT
#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX
#else
#define INTMAX_MIN  INT32_MIN
#define INTMAX_MAX  INT32_MAX
#define UINTMAX_MAX UINT32_MAX
#endif


#endif /*STDINT_H*/
