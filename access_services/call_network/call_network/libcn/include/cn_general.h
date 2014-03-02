/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_general_h__
#define __cn_general_h__ (1)

#include <stdint.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef HAVE_ANDROID_OS
#define C_ASSERT(expression, name) \
  typedef int c_assert_##name[-1 + (expression)]
#else
#define C_ASSERT(expression, name) \
  typedef int c_assert_##name[-1 + (expression)]
#endif

/* fundamental data type definitions.
 * A cn prefix is used so that they are not colliding with (potential) compiler
 * definitions with the same names.
 *
 * assumption: a char is 8 bits (not always true).
 */
typedef unsigned char  cn_uint8_t;
typedef unsigned short cn_uint16_t;
typedef unsigned int   cn_uint32_t;
typedef signed char    cn_sint8_t;
typedef signed short   cn_sint16_t;
typedef signed int     cn_sint32_t;
typedef void           cn_void_t;
typedef cn_uint8_t     cn_bool_t;
typedef uintptr_t      cn_uintptr_t;

C_ASSERT(sizeof(cn_uint8_t)  == 1, cn_uint8_t);
C_ASSERT(sizeof(cn_uint16_t) == 2, cn_uint16_t);
C_ASSERT(sizeof(cn_uint32_t) == 4, cn_uint32_t);
C_ASSERT(sizeof(cn_sint8_t)  == 1, cn_sint8_t);
C_ASSERT(sizeof(cn_sint16_t) == 2, cn_sint16_t);
C_ASSERT(sizeof(cn_sint32_t) == 4, cn_sint32_t);

#define PARAMETER_NOT_USED(p) (void)(p)

#ifdef UNUSED
#elif defined(__GNUC__)
#define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(S_SPLINT_S)
#define UNUSED(x) /*@unused@*/ x
#else
#define UNUSED(x) x
#endif


#endif /* __cn_general_h__ */
