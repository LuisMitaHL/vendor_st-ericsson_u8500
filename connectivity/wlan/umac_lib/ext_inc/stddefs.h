/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup GLOBAL
*
* \brief Standard Definitions
*/

#if defined(__GNUC__)
#include<linux/types.h>
#endif
#ifndef _STDDEFS_H_
#define _STDDEFS_H_

#define PUBLIC
#define PRIVATE	    static

typedef unsigned char uint8;
typedef signed char sint8;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned long uint32;
typedef signed long sint32;

#ifndef __cplusplus
#if !defined(__GNUC__)
typedef int bool;
#endif
#endif

#if !defined(__GNUC__)
#ifndef offsetof
#define offsetof(_s, _f)  ((uint8 *)&((_s*)0)->_f - (uint8 *)0)
#endif
#endif

#if defined(_WIN32)
typedef unsigned __int64 uint64;
typedef signed __int64 sint64;
typedef uint32 UINTPTR;
#define  PACKED
#define GCC_PACKED
#define ALIGN4
#else
#define ALIGN4 __attribute__ ((aligned (4)))
#endif

#if defined(__GNUC__)
typedef __u64 uint64;
typedef __s64 sint64;
typedef uint32 UINTPTR;
#define PACKED
#define GCC_PACKED __attribute__((packed))
#else
#if defined(__arm)
typedef unsigned long long uint64;
typedef signed long long sint64;
typedef uint32 UINTPTR;
#define  PACKED  __packed
#define GCC_PACKED
#ifndef FIELD_OFFSET
	/*#define FIELD_OFFSET(type, field) ((UINTPTR)(&((type *)0)->field))*/
#define FIELD_OFFSET(type, field) offsetof(_s, _f)
#endif
#endif // __arm
#endif // __GNUC__

#if !defined(__GNUC__)
#undef NULL
#if !defined(NULL)
#define NULL	    0
#endif
#endif


#if !defined(USE_BITFIELD_MACROS)
#define USE_BITFIELD_MACROS 0
#endif

#define TRUE	    1
#define FALSE	   0

#define forever()   for (;;)

#ifndef ENTRIES
#define ENTRIES(x)  (sizeof(x)/sizeof((x)[0]))
#endif

#define ALIGN_DOWN_16BIT(_x)      (((UINTPTR)(_x))&~(sizeof(uint16)-1))
#define ALIGN_DOWN_32BIT(_x)      (((UINTPTR)(_x))&~(sizeof(uint32)-1))

#define ALIGN_UP_16BIT(_x)      ALIGN_DOWN_16BIT(_x+sizeof(uint16)-1)
#define ALIGN_UP_32BIT(_x)      ALIGN_DOWN_32BIT(_x+sizeof(uint32)-1)

#ifndef INITIALISE_TO_FIELD
/*
 * Do not be tempted by the Windows-style "CONTAINING_RECORD()" macro.  It is
 * quite clever, but doesn't check that the address you give it is a pointer
 * to the correct type.  This can hide some nasty programming errors.
 *
 * Also, there is a "feature" in the MS compiler that doesn't properly check
 * the types of pointer that are cast from constants.
 *   e.g.
 *      STRUCT_A *myAPtr = NULL;
 *      STRUCT_B *myBPtr = NULL;
 *      Warning as expected
 *      int Test1 = (myAPtr == myBPtr);
 *      No warning!
 *      int Test2 = (&((STRUCT_A *)0) == myBPtr);
 *
 * Hence, we have to have a slightly different macro to get around the above
 *problem, perform the casts AND check the type.
 */
#define INITIALISE_TO_FIELD(var, address, type, field) \
    { var = ((type *)((char *)(address) - (UINTPTR)(&((type *)0)->field))), (&var->field == address); }
#endif

#if USE_BITFIELD_MACROS
/************************
* BITFIELD_VALUE(_bitmap)
*************************
* Converts a uint32 bitfield structure to/from a uint32 value
*/
#define BITFIELD_VALUE(_bitfield)  *((uint32 *) &(_bitfield))
#endif

#if 0
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) < (b)) ? (b) : (a))
#endif
#endif
#define MIN(a,b) ( ((a)>(b)) ? (b):(a) )
#define MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define ABS(a) (((a) < 0) ? (-(a)) : (a))

typedef struct REGVAL_S {
	uint32 addr;
	uint32 val;
} REGVAL;

#endif	/* Sentry */			/* #ifndef _STDDEFS_H_*/
