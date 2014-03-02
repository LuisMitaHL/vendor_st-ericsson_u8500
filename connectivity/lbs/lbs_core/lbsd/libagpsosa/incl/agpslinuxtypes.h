/*
* Copyright (C) ST-Ericsson 2009
*
* agpslinuxtypes.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_LINUX_TYPES_H
#define AGPS_LINUX_TYPES_H

#include <stdint.h>

/* + Basic Types OSA */
/*! \addtogroup Linux_BasicDataTypes
 *      \details
 */
/*@{*/
/**
    Unsigned 8 bits
*/
//typedef uint8_t     uint8_t;
/**
    Signed 8 bits
*/
//typedef int8_t      int8_t;
/**
    Unsigned 16 bits
*/
//typedef uint16_t    uint16_t;
/**
    Signed 16 bits
*/
//typedef int16_t     int16_t;
/**
    Unsigned 32 bits
*/
//typedef uint32_t    uint32_t;
/**
    Signed 32 bits
*/
//typedef int32_t     int32_t;
/**
    Unsigned 64 bits
*/
//typedef uint64_t    uint64_t;
/**
    Signed 64 bits
*/
//typedef int64_t     int64_t;
typedef int         BOOL;
#ifdef LBS_LINUX_UBUNTU_FTR
typedef unsigned char          bool;
#else
typedef uint8_t          bool;
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/*@}*/
/* - Basic Types OSA */ 

#endif /* AGPS_LINUX_TYPES_H */
