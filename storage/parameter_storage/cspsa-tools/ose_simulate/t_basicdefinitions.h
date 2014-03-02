/* **************************************************************************
 *
 * t_basicdefinitions.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 **************************************************************************** */

#ifndef T_BASICDEFINITIONS_H
#define T_BASICDEFINITIONS_H

#include <stdint.h>

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef unsigned char boolean;
typedef int32_t sint32;

#define UTF8_FILENAME_MAX    64

#ifndef MIN
#define MIN(Val1, Val2) (((Val1) < (Val2)) ? (Val1) : (Val2))
#endif

#ifndef MAX
#define MAX(Val1, Val2) (((Val1) > (Val2)) ? (Val1) : (Val2))
#endif

#ifndef ElementsOf
#define ElementsOf(Array) (sizeof(Array) / sizeof(Array[0]))
#endif

#define FALSE 0
#define TRUE 1

#endif

