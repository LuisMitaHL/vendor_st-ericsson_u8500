/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/******************************************************************************
* $Workfile: t_basicdefinitions.h $
*
* Copyright (c) 1999 Ericsson Mobile Platform AB, Lund, Sweden
*
* DESCRIPTION:
*
*     Portable types used for a consistent target platform.
*     The name should make it totally clear as to what they are used for.
*
*	    For calculations:
*     -----------------
*     sint8   -   signed integer 8 bits 
*     uint8   -   unsigned integer 8bits
*     sint16  -   signed integer 16 bits
*     uint16  -   unsigned integer 16 bits
*     sint32  -   signed integer 32 bits
*     uint32  -   unsigned integer 32 bits
*			MAX and MIN values for all integer types are also supported.
*
*			Unsigned integer types for other purposes than calculations:
*     ------------------------------------------------------------
*     boolean    -  TRUE or FALSE
*
*
*     Bitmasks:
*     ---------
*     BIT_0      - unsigned integer of values 0x00000001
*     ...
*     BIT_31     - unsigned integer of values 0x80000000
*
*     Enumeration:
*     ------------
*     TYPEDEF_ENUM     - This macro should be used to start the defenition of a enumerated type
*     ENUM8(t)         - uint8 enum
*     ENUM16(t)        - uint16 enum
*     ENUM32(t)        - uint32 enum
*     SIGNED_ENUM8(t)  - sint8 enum
*     SIGNED_ENUM16(t) - sint16 enum
*     SIGNED_ENUM32(t) - sint32 enum
*/

#ifndef _T_BASICDEFINITIONS_H
#define _T_BASICDEFINITIONS_H

/********************
* Include files
*********************/

#include <limits.h>

/********************
* Portable data types
*********************/


/** Type definition for a signed 8 bit data entity. */
typedef signed char sint8;
/** Type definition for an unsigned 8 bit data entity. */
typedef unsigned char uint8;
/** Type definition for a signed 16 bit data entity. */
typedef signed short sint16;
/** Type definition for an unsigned 16 bit data entity. */
typedef unsigned short uint16;
/** Type definition for a signed 32 bit data entity. */
typedef signed long sint32;
/** Type definition for an unsigned 32 bit data entity. */
typedef unsigned long uint32;

/*******************
* MAX and MIN values
********************/
/** Minimum value for a entity of type sint8 */
#define MIN_SINT8  (SCHAR_MIN)
/** Maximum value for a entity of type sint8 */
#define MAX_SINT8  (SCHAR_MAX)
/** Maximum value for a entity of type uint8 */
#define MAX_UINT8  (UCHAR_MAX)
/** Minimum value for a entity of type sint16 */
#define MIN_SINT16 (SHRT_MIN)
/** Maximum value for a entity of type sint16 */
#define MAX_SINT16 (SHRT_MAX)
/** Maximum value for a entity of type uint16 */
#define MAX_UINT16 (USHRT_MAX)
/** Minimum value for a entity of type sint32 */
#define MIN_SINT32 (LONG_MIN)
/** Maximum value for a entity of type sint32 */
#define MAX_SINT32 (LONG_MAX)
/** Maximum value for a entity of type uint32 */
#define MAX_UINT32 (ULONG_MAX)


/********************
* boolean: TRUE/FALSE
*********************/
/** Type definition for a boolean/logical value */
typedef uint8 boolean;

#ifndef TRUE
/** Value representing the boolean/logical value false. */
#define FALSE 0
/** Value representing the boolean/logical value true. */
#define TRUE (!FALSE)
#endif

/*************************
* Bit mask definitions
**************************/

 /**
 * This sections defines a set of masks implemented as scalar unsigned values
 * that can be used to mask out bits of a scalar entity. The definitions are
 * named BIT_0 through BIT_31 and each implements the unsigned value of two to
 * the power of the value in the definitions name. E.g. BIT_0 implements the value of 1 while BIT_10 implements the value of 0x0400 (equals 1024 in decimal form).
 */
#define BIT_0  0x0001U
#define BIT_1  0x0002U
#define BIT_2  0x0004U
#define BIT_3  0x0008U
#define BIT_4  0x0010U
#define BIT_5  0x0020U
#define BIT_6  0x0040U
#define BIT_7  0x0080U
#define BIT_8  0x0100U
#define BIT_9  0x0200U
#define BIT_10 0x0400U
#define BIT_11 0x0800U
#define BIT_12 0x1000U
#define BIT_13 0x2000U
#define BIT_14 0x4000U
#define BIT_15 0x8000U
#define BIT_16 0x00010000UL
#define BIT_17 0x00020000UL
#define BIT_18 0x00040000UL
#define BIT_19 0x00080000UL
#define BIT_20 0x00100000UL
#define BIT_21 0x00200000UL
#define BIT_22 0x00400000UL
#define BIT_23 0x00800000UL
#define BIT_24 0x01000000UL
#define BIT_25 0x02000000UL
#define BIT_26 0x04000000UL
#define BIT_27 0x08000000UL
#define BIT_28 0x10000000UL
#define BIT_29 0x20000000UL
#define BIT_30 0x40000000UL
#define BIT_31 0x80000000UL

/*****************************
* Macro's for fixed size enums
*
* Example of use:
*
*   TYPEDEF_ENUM {  
*     Def1,
*     ...,
*     Defn
*   }ENUM8 (MyEnum_t); // Or ENUM16 or ENUM32
*
******************************/
/** This macro should be used to start the definition of an enumerated type. */
#define TYPEDEF_ENUM enum
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the uint8 data type described in section 3.1.1.
 */
#define ENUM8(t) ;typedef uint8 t
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the uint16 data type described in section 3.1.1.
 */
#define ENUM16(t) ;typedef uint16 t
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the uint32 data type described in section 3.1.1. 
 */
#define ENUM32(t) ;typedef uint32 t
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the sint8 data type described in section 3.1.1. 
 */
#define SIGNED_ENUM8(t) ;typedef sint8 t
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the sint16 data type described in section 3.1.1. 
 */
#define SIGNED_ENUM16(t) ;typedef sint16 t
/**
 * This macro should be used to finalise the definition of an enumerated type
 * "t" compatible with the sint32 data type described in section 3.1.1. 
 */
#define SIGNED_ENUM32(t) ;typedef sint32 t


#ifndef IDENTIFIER_NOT_USED
#define IDENTIFIER_NOT_USED(p)	if (sizeof(&p)) {}
#endif

/* Returns the maximum of two values. */
#ifndef MAX
#define MAX(a,b) (a > b ? a : b)
#endif

/* Returns the minimum of two values. */
#ifndef MIN
#define MIN(a,b) (a < b ? a : b)
#endif


#endif //_T_BASICDEFINITIONS_H
