/*
* Copyright (C) ST-Ericsson 2009
*
* agpswintypes.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_WIN_TYPES_H
#define AGPS_WIN_TYPES_H
#include <windows.h>

/* + Basic Types OSA */
/*! \addtogroup Windows_BasicDataTypes
 *      \details
 */
/*@{*/
/**
    Unsigned 8 bits
*/
typedef UINT8   uint8_t;
/**
    Signed 8 bits
*/
typedef INT8    int8_t;
/**
    Unsigned 16 bits
*/
typedef UINT16  uint16_t;
/**
    Signed 16 bits
*/
typedef INT16   int16_t;
/**
    Unsigned 32 bits
*/
typedef UINT32  uint32_t;
/**
    Signed 32 bits
*/
typedef INT32   int32_t;
/**
    Unsigned 64 bits
*/
typedef UINT64  uint64_t;
/**
    Signed 64 bits
*/
typedef INT64   int64_t;
typedef uint8_t      bool;

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif
/*@}*/
/* - Basic Types OSA */

#endif /* AGPS_WIN_TYPES_H */