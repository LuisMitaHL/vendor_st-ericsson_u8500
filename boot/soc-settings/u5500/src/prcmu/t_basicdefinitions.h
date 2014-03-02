#ifndef INCLUSION_GUARD_T_BASICDEFINITIONS_H
#define INCLUSION_GUARD_T_BASICDEFINITIONS_H

/******************************************************************************
* $Copyright ST-Ericsson 2010 $
*******************************************************************************
*
* $Workfile: t_basicdefinitions.h $
*
*******************************************************************************
*
* DESCRIPTION:
*
*     Portable types used for a consistent target platform.
*     The name should make it totally clear as to what they are used for.
*
*     For calculations:
*     -----------------
*     s8   -   signed integer 8 bits
*     u8   -   unsigned integer 8bits
*     s16  -   signed integer 16 bits
*     u16  -   unsigned integer 16 bits
*     s32  -   signed integer 32 bits
*     u32  -   unsigned integer 32 bits
*     MAX and MIN values for all integer types are also supported.
*
*     Unsigned integer types for other purposes than calculations:
*     ------------------------------------------------------------
*     boolean    -  TRUE or FALSE
*
*     Bitfield types to use in packed structs:
*     ----------------------------------------
*     sbitfield  -  signed bitfield
*     ubitfield  -  unsigned bitfield
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
*     ENUM8(t)         - u8 enum
*     ENUM16(t)        - u16 enum
*     ENUM32(t)        - u32 enum
*     SIGNED_ENUM8(t)  - s8 enum
*     SIGNED_ENUM16(t) - s16 enum
*     SIGNED_ENUM32(t) - s32 enum
*
******************************************************************************/

/********************
* Include files
*********************/

// #include "c_compiler.h"
/* #include <limits.h> */

/********************
* Portable data types
*********************/

#if defined(COMPILER_IAR_ARM) && (__VER__ >= 300)
  #define S64_SUPPORTED
  #define U64_SUPPORTED
  #define INT64_BASE_TYPE   long long
#elif defined(COMPILER_ARM_ARM)
  #define S64_SUPPORTED
  #define U64_SUPPORTED
  #define INT64_BASE_TYPE   long long
#elif defined(_WIN32)
  #define S64_SUPPORTED
  #define U64_SUPPORTED
  #define INT64_BASE_TYPE   __int64
#endif


/** Type definition for a signed 8 bit data entity. */
typedef signed char s8;
/** Type definition for an unsigned 8 bit data entity. */
typedef unsigned char u8;
/** Type definition for a signed 16 bit data entity. */
typedef signed short s16;
/** Type definition for an unsigned 16 bit data entity. */
typedef unsigned short u16;
/** Type definition for a signed 32 bit data entity. */
typedef signed long s32;
/** Type definition for an unsigned 32 bit data entity. */
typedef unsigned long u32;

/**
 * Type definition for a signed 64 bit data entity. Only available if the switch
 * s64_SUPPORTED is defined.
 */
#ifdef s64_SUPPORTED
  typedef signed INT64_BASE_TYPE s64;
#endif

/**
 * Type definition for an unsigned 64 bit data entity. Only available if the
 * switch u64_SUPPORTED is defined.
 */
#ifdef u64_SUPPORTED
  typedef unsigned INT64_BASE_TYPE u64;
#endif

/*******************
* MAX and MIN values
********************/
/** Minimum value for an entity of type s8 */
#define MIN_S8  (SCHAR_MIN)
/** Maximum value for an entity of type s8 */
#define MAX_S8  (SCHAR_MAX)
/** Maximum value for an entity of type u8 */
#define MAX_U8  (UCHAR_MAX)
/** Minimum value for an entity of type s16 */
#define MIN_S16 (SHRT_MIN)
/** Maximum value for an entity of type s16 */
#define MAX_S16 (SHRT_MAX)
/** Maximum value for an entity of type u16 */
#define MAX_U16 (USHRT_MAX)
/** Minimum value for an entity of type s32 */
#define MIN_S32 (LONG_MIN)
/** Maximum value for an entity of type s32 */
#define MAX_S32 (LONG_MAX)
/** Maximum value for an entity of type u32 */
#define MAX_U32 (ULONG_MAX)


#ifdef S64_SUPPORTED
/**
 * Minimum value for an entity of type s64. Only available if the switch
 * s64_SUPPORTED is defined.
 */
  #define MIN_S64 (-0x8000000000000000)
/**
 * Maximum value for an entity of type s64. Only available if the switch
 * s64_SUPPORTED is defined.
 */
  #define MAX_S64 (0x7fffffffffffffff)
#endif

#ifdef U64_SUPPORTED
/**
 * Maximum value for an entity of type u64. Only available if the switch
 * u64_SUPPORTED is defined.
 */
  #define MAX_U64 (0xffffffffffffffff)
#endif

/********************
* boolean: TRUE/FALSE
*********************/
/** Type definition for a boolean/logical value */
typedef u8 boolean;

#ifndef TRUE
/** Value representing the boolean/logical value false. */
#define FALSE 0
/** Value representing the boolean/logical value true. */
#define TRUE (!FALSE)
#endif

/******************************************
* Portable bitfield definitions
*******************************************/

#if 0
#if defined(COMPILER_IAR_AVR) || defined(COMPILER_IAR_ARM) || defined(_WIN32) || defined (COMPILER_ARM_ARM)
/** Type definition to be used when implementing bit-fields that should hold
 * signed values.
 */
  typedef s8 sbitfield;
/** Type definition to be used when implementing bit-fields that should hold
 * unsigned values.
 */
  typedef u8 ubitfield;
#elif defined(_lint)
  typedef signed int  sbitfield;
  typedef unsigned int  ubitfield;
#elif defined(_FLEX_ASIC)
  /*
   * there is no bitfield definition for FlexDSP
   */
  //typedef signed int  sbitfield;
  //typedef unsigned int  ubitfield;#else
#else
  #error Unknown preferred bitfield definition for this compiler
#endif

#endif // if 0

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
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the u8 data type described in section 3.1.1.
 */
#define ENUM8(t) ;typedef u8 t
/**
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the u16 data type described in section 3.1.1.
 */
#define ENUM16(t) ;typedef u16 t
/**
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the u32 data type described in section 3.1.1.
 */
#define ENUM32(t) ;typedef u32 t
/**
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the s8 data type described in section 3.1.1.
 */
#define SIGNED_ENUM8(t) ;typedef s8 t
/**
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the s16 data type described in section 3.1.1.
 */
#define SIGNED_ENUM16(t) ;typedef s16 t
/**
 * This macro should be used to finalize the definition of an enumerated type
 * "t" compatible with the s32 data type described in section 3.1.1.
 */
#define SIGNED_ENUM32(t) ;typedef s32 t

#ifndef VECTOR_LEN
#define VECTOR_LEN( x ) (sizeof( (x) )/sizeof( (x)[0] ))
#endif

#endif // INCLUSION_GUARD_T_BASICDEFINITIONS_H

