/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __util_general_h__
#define __util_general_h__ (1)

#include <stdint.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#define C_ASSERT(expression, name) \
  typedef int c_assert_##name[-1 + (expression)]

/* fundamental data type definitions */
typedef unsigned char  util_uint8_t;
typedef unsigned short util_uint16_t;
typedef unsigned int   util_uint32_t;
typedef signed char    util_sint8_t;
typedef signed short   util_sint16_t;
typedef signed int     util_sint32_t;
typedef void           util_void_t;
typedef util_uint8_t   util_bool_t;
typedef uintptr_t      util_uintptr_t;

C_ASSERT(sizeof(util_uint8_t)  == 1, util_uint8_t);
C_ASSERT(sizeof(util_uint16_t) == 2, util_uint16_t);
C_ASSERT(sizeof(util_uint32_t) == 4, util_uint32_t);
C_ASSERT(sizeof(util_sint8_t)  == 1, util_sint8_t);
C_ASSERT(sizeof(util_sint16_t) == 2, util_sint16_t);
C_ASSERT(sizeof(util_sint32_t) == 4, util_sint32_t);


#endif /* __util_general_h__ */
