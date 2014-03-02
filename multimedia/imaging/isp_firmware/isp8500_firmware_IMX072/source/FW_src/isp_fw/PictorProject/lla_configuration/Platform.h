/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  ConfigurationHeaders Configuration Headers
 \details   This module contains all the confguration files for the project.
            Usually all files that contribute to the project/platform specific
            portion of a module will be part of this module.
*/

/**
 \file      Platform.h
 \brief     The file asbtracts the keywords that are specific to the STXP70.
            The ensures that the individual firmware modules are immune to
            processor specific keywords and details.

 \ingroup   ConfigurationHeaders
*/
#ifndef PLATFORM_H_
#   define PLATFORM_H_

#   include "stxp70_system.h"
#   if __FPX
#      include "models/fpx.h"
#   endif

/*
*  Included for malloc/free function procedures
*/
#   include "stdlib.h"
#   include "cam_drv_platform_hardcoding.h"

//MAX SIZE supported by HW
#   define MAX_OUTPUT_SIZEX_PIPE0      4094
#   define MAX_OUTPUT_SIZEX_PIPE1      1024
#   define MAX_OUTPUT_SIZEY            8190

//#   define DEBUG_PROFILER      
//#   define PROFILER_USING_XTI  

// define the DEBUG macro to 1 to enable debug capabilities in firmware
#   define DEBUG           (1)


#   define HOST_COMMS_DEBUG      1

//Trace log enable
#   define TRACE_LOG 1

// Test Idle Modes
#   define TEST_IDLE_MODES    1

// including isl dump
#   define SEND_ISL_DUMP      1

//  including auto focus
#   define INCLUDE_FOCUS_MODULES 1

// Use Low level API's Framework
#   define USE_LOW_LEVEL_API    1

 //define this macro for trace compiler
#   define  OST_TRACE_ASCII_IN_USE                0

 //define this macro for trace compiler
#   define  OST_TRACE_COMPILER_IN_USE             1


// Use Customer  LLA 1 for IMG_CONFIG 100
// 2 for others IMG_CONFIG 201,
// 3 IMG Config 102
#define USE_LLA_CUSTOMER        2

//PEDebug interface, comment below line to disable
#define PEDEBUG_ENABLE						1

#if (OST_TRACE_ASCII_IN_USE)  /*Use Ascii Traces*/
#define USE_OST_TRACES                                                          PROTOCOL_ID_ASCII

#elif (OST_TRACE_COMPILER_IN_USE) /*Use Binary Traces*/
#define USE_OST_TRACES                                                          PROTOCOL_ID_BINARY

#else /*Dont use any trace method */
#define USE_OST_TRACES                                                          0
#endif

#define USE_TRACE_ERROR (1)

#if (USE_LLA_CUSTOMER == 3)  // Enable memory traces except IMG_CONFIG 102
#define TRACE_BUFF_ENABLE (0)
#else
#define TRACE_BUFF_ENABLE (1)
#endif


// Removes malloc usage from the API's
#   define USE_MALLOC_FREE_IMPLEMETATION    1

// macro to trap unexpected conditions in firmware
#   if DEBUG
#      define ASSERT_XP70()   while (1)
#   else
#      define ASSERT_XP70()
#   endif
#   define FALSE   (0)
#   define TRUE    (!FALSE)
/// Data type definitions specific to STXP70 Processor
/// Unsigned 8 bit data type
typedef unsigned char   uint8_t;

/// Signed 8 bit data type
typedef signed char         int8_t;

/// Signed 16 bit data type
typedef short               int16_t;

/// Unsigned 16 bit data type
typedef unsigned short      uint16_t;

/// Signed 32 bit data type
typedef int                 int32_t;

/// Unsigned 32 bit data type
typedef unsigned int        uint32_t;

/// Signed 32 bit data type
typedef long long           int64_t;

/// Unsigned 32 bit data type
typedef unsigned long long  uint64_t;

/// 32 bit IEEE 754 floating point data type
typedef float               float_t;

/// Boolean data type. Occupies 8 bits.
typedef uint8_t             bool_t;

/// Define to direct a symbol to section named X
#   define TOSECTION(X)    __attribute__((section(""#X "")))

/// Define to direct a symbol to section SDA
// #   define SDA __attribute__((memory("sda")))
#   define SDA

/// Define to direct a symbol to section TDA
#   define TDA __attribute__((memory("tda")))

/// Define to identify a function as an interrupt service routine
#   define INTERRUPT   __attribute__((interrupt))

/// Define to ensure that an exported variable from a module remains readonly
#   define READONLY(x) (x + 0)

/// Define to direct a symbol to external program memory
#   define TO_EXT_PRGM_MEM      __attribute__ ((section(".exttext")))

/// Define to direct a symbol to external SDRAM program memory
#   define TO_EXT_DDR_PRGM_MEM      __attribute__((section(".exttext2")))

/// Define to direct a symbol to external program memory
#   define TO_EXT_DATA_MEM      __attribute__((section(".extdata")))

/// Define to direct a symbol to external program memory
#   define TO_FIXED_DATA_ADDR_MEM      __attribute__((section(".fixedadd")))

/// Define to direct a symbol to external program memory
#   define TO_FIXED_EXT_DATA_2_ADDR_MEM      __attribute__((section(".fixedadd_2")))

/// Define to direct a symbol to direct in external memory
#   define TO_EXT_DATA_MEM_NONE      __attribute__((memory("none")))

/// Define to direct a symbol to direct in external memory
#   define TO_EXT_DATA_TO_EXT_MEM    TO_EXT_DATA_MEM  TO_EXT_DATA_MEM_NONE


//Do not move the position of the below header file inclusion
//Reason : TraceMsg.h uses all typedefs of identifiers(int,char,...),had we place this inclusion
//at the beginning of file we would not get the typedefs of identifiers and Tracemsg.h is needed by
//all the files those are using TRACELOGs.Instead of adding Tracemsg.h everywhere it is added here
#include "osttrace.h"
#include "EventManager.h"
//#include "TraceMsg.h"


/**
\enum Flag_te
\brief Enum for flag False(0), True(1)
\ingroup
*/
typedef enum
{
    Flag_e_FALSE,
    Flag_e_TRUE
} Flag_te;

/**
 \enum Result_te
 \brief Enum for Result Failure(0), Success(1)
 \ingroup
*/
typedef enum
{
    Result_e_Failure,
    Result_e_Success
} Result_te;

/**
 \enum Coin_te
 \brief enum used to create test coins.
        Coin machanisam is used by firmware to accept user commands.
 \ingroup
*/
typedef enum
{
    Coin_e_Heads,
    Coin_e_Tails
} Coin_te;

/**
 \enum Pipe_te
 \brief enum to select within pipe0 & pipe1
 \ingroup
*/
typedef enum
{
    Pipe_0,
    Pipe_1
} Pipe_te;

#   define COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION   (2)
#   define COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION  (2)
#   define ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION       (2)
#   define ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION    (2)

#   define NO_OF_HARDWARE_PIPE_IN_ISP                              (2)

#   define CRM_CSI2_CLOCK_DETECTION    (1)

#   define BSWAP_16(x)     (((x & 0xFF00) >> 8) | ((x & 0xFF) << 8))
#   define BSWAP_32(x) \
        (((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24))
#   define max(a, b)           (((a) > (b)) ? (a) : (b))
#   define min(a, b)           (((a) < (b)) ? (a) : (b))
#   define clip(a, min, max)   ((a) <= (min) ? (min) : ((a) >= (max) ? (max) : (a)))
#   if __FPX
#      define FMIN(x, y)              (__builtin_fpx_fmin(x, y))
#      define FMAX(x, y)              (__builtin_fpx_fmax(x, y))
#      define FCLIP1(value, min, max) (FMIN(max, FMAX(value, min)))
#      define FCLIP2(value, min, max) (value = (FMIN(max, FMAX(value, min))))
#      define SQUARE_ROOT(x)          __builtin_fpx_fsqrt(x)
#   else //if __FPX
#      define FMIN(x, y)              (min(x, y))
#      define FMAX(x, y)              (max(x, y))
#      define FCLIP1(value, min, max) (FMIN(max, FMAX(value, min)))
#      define FCLIP2(value, min, max) (value = (FMIN(max, FMAX(value, min))))
#      define fpx_fmax(a, b, c)       (a = max(b, c))
#      define fpx_fmin(a, b, c)       (a = min(b, c))
#      define mod(a)                  ((a > 0) ? (a) : (-a))

//#define fpx_fscalb(a,b,c)      (a = b*((mod(c)==c)?(1<<c):(1/(1<<c))))
#      define fpx_fscalb(a, b, c) (a = ((mod(c) == c) ? (b * (1 << c)) : (b / (1 << -c))))

//#define fpx_fscalb(a,b,c)      (a = b*(1<<c))
#      define fpx_fasub(a, b, c)  (a = ((b > c) ? (b - c) : (c - b)))

// Need to change to library
#      define SQUARE_ROOT(x)  __builtin_fpx_fsqrt(x)
#   endif //if __FPX


/// Compile time control to include MWWB
#define PLATFORM_MWWB_INCLUDE_MWWB                  0

/// Compile time control to include Weighted Statistics Processor
#define PLATFORM_WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR      0

/// Compile time control to include 8x6 Exposure block
#define PLATFORM_STATS_PROCESSOR_6x8_ZONES 0

/// Defines for flash type. LLCD should have internal define that can have these two as possible value.
#define FLASHTYPE_SENSORDRIVEN                                    (0)
#define FLASHTYPE_PINDRIVEN                                       (1)

/// [NON_ISL_SUPPORT] - version information for non SMIA sensors
#define SENSOR_VERSION_NON_SMIA                                   (255)

//[NON_ISL_SUPPORT]
int non_smia_correction_factor;


#endif /*PLATFORM_H_*/

