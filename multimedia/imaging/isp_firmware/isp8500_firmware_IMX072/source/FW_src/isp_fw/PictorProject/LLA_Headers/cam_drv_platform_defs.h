/*
Nokia Corporation

Filename:       cam_drv_platform_defs.h


---------------------------------------------------------------------------
LEGAL NOTICE
The contents of this document are proprietary and confidential property of Nokia. This document is provided
subject to confidentiality obligations of the applicable agreement(s).

This document is intended for use of Nokia’s customers and collaborators only for the purpose for which this
document is submitted by Nokia. No part of this document may be reproduced or made available to the public
or to any third party in any form or means without the prior written permission of Nokia. This document is to be
used by properly trained professional personnel. Any use of the contents in this document is limited strictly to
the use(s) specifically authorized in the applicable agreement(s) under which the document is submitted. The
user of this document may voluntarily provide suggestions, comments or other feedback to Nokia in respect of
the contents of this document ("Feedback"). Such Feedback may be used in Nokia products and related
specifications or other documentation. Accordingly, if the user of this document gives Nokia Feedback on the
contents of this document, Nokia may freely use, disclose, reproduce, license, distribute and otherwise
commercialize the Feedback in any Nokia product, technology, service, specification or other documentation.

Nokia operates a policy of ongoing development. Nokia reserves the right to make changes and improvements
to any of the products and/or services described in this document or withdraw this document at any time without
prior notice.

The contents of this document are provided "as is". Except as required by applicable law, no warranties of any
kind, either express or implied, including, but not limited to, the implied warranties of merchantability and fitness
for a particular purpose, are made in relation to the accuracy, reliability or contents of this document. NOKIA
SHALL NOT BE RESPONSIBLE IN ANY EVENT FOR ERRORS IN THIS DOCUMENT or for any loss of data
or income or any special, incidental, consequential, indirect or direct damages howsoever caused, that might
arise from the use of this document or any contents of this document.

This document and the product(s) it describes are protected by copyright according to the applicable laws.

Nokia is a registered trademark of Nokia Corporation. Other product and company names mentioned herein
may be trademarks or trade names of their respective owners.

Copyright © Nokia Corporation 2010. All rights reserved.
---------------------------------------------------------------------------*/

#ifndef CAM_DRV_PLATFORM_DEFS_H
#define CAM_DRV_PLATFORM_DEFS_H

#include "Platform.h"

#if USE_LOW_LEVEL_API
// Do not include any file
#define NO_LIBS 1

#else
#include <string.h>
#include <stdio.h>
#include <math.h>
#endif
/*------------------------------------------------------------------------------
*   @1 ABSTRACT
*-----------------------------------------------------------------------------*/
/**   @file     cam_drv_platform_defs.h
*     @brief    Common configurations & defines used by many camera driver files.
*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*  @LEVEL2

    @2 @CONTENTS

     1 ABSTRACT
     2 CONTENTS
     3 FLAGS AND CONSTANTS
     4 EXTERN STRUCTS
     5 VARIABLES
     6 FUNCTION PROTOTYPES
         6.1 Public function prototypes
@                                                                             */
/*============================================================================*/

/*------------------------------------------------------------------------------
*  @3 FLAGS AND CONSTANTS
*-----------------------------------------------------------------------------*/


/***************************************
 * Data Types *
 ***************************************/

#define LOCAL static
#define NULL 0

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
//typedef unsigned int TUint;
typedef char int8;
typedef short int16;
typedef int int32;
typedef _Bool bool8;

// -- TO DO --
// Get rid of platform specific timers
typedef uint32 HAL_TIMER_HANDLE;
// similarly HAL_TIMER_ONESHOT needs to be checked.
#define HAL_TIMER_ONESHOT 0

#define CAMERA_HAL_DEBUG_PRINT_TRACE FALSE 

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)

typedef enum
{
TRACE_ERROR,
TRACE_WARNING,
TRACE_FLOW,   // used to print Entry Exit Traces
TRACE_DEBUG,
TRACE_API,
TRACE_OMX_API,
TRACE_OMX_BUFFER,
TRACE_RESERVED,
TRACE_USER1,   //used to print config traces
TRACE_USER2,   // used to print sensor traces
TRACE_USER3,	// used to print HW traces
TRACE_USER4,	// used to print DCC traces
TRACE_USER5,	//used to print Ancilliary data traces
TRACE_USER6, // used to print focus traces
TRACE_USER7, // used to print NVM traces
TRACE_USER8,
} OST_TraceTypes_te;
#endif
/***************************************
 * Trace Macros *
 ***************************************/

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE) 
#undef  OstTraceInt0
#define OstTraceInt0(aGroupName, aTraceText)  \
                {char * debugBuffer[512]; _snprintf(debugBuffer,512,aTraceText,"\n");DebugPrint(debugBuffer);}
#else // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#ifndef OstTraceInt0
#define OstTraceInt0
#endif
#endif // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#undef  OstTraceInt1    
#define OstTraceInt1(aGroupName, aTraceText,aParam)  \
                {char * debugBuffer[512]; _snprintf(debugBuffer,512,aTraceText,aParam,"\n");DebugPrint(debugBuffer);}        
#else // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#ifndef OstTraceInt1
#define OstTraceInt1
#endif
#endif // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#undef  OstTraceInt2      
#define OstTraceInt2(aGroupName, aTraceText, aParam1, aParam2)  \
                {char * debugBuffer[512]; _snprintf(debugBuffer,512,aTraceText,aParam1,aParam2,"\n");DebugPrint(debugBuffer);}  
#else // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#ifndef OstTraceInt2
#define OstTraceInt2
#endif
#endif // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)      

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#undef  OstTraceInt3 
#define OstTraceInt3(aGroupName, aTraceText, aParam1, aParam2, aParam3)  \
                {char * debugBuffer[512]; _snprintf(debugBuffer,512,aTraceText,aParam1,aParam2,aParam3,"\n");DebugPrint(debugBuffer);}     
#else // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#ifndef OstTraceInt3
#define OstTraceInt3
#endif
#endif // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)

#if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#undef  OstTraceInt4         
#define OstTraceInt4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4)  \
                {char * debugBuffer[512]; _snprintf(debugBuffer,512,aTraceText,aParam1,aParam2,aParam3,aParam4,"\n");DebugPrint(debugBuffer);}   
#else // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)
#ifndef OstTraceInt4
#define OstTraceInt4
#endif
#endif // #if (CAMERA_HAL_DEBUG_PRINT_TRACE == TRUE)       	                

#define SWAP16(x) (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8) )
#define SWAP32(x) ( ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24) )

#ifdef  CAM_DRV_ON_BIG_ENDIAN
#define FROM_BIG16(x) (x)
#define FROM_BIG32(x) (x)
#else
#define FROM_BIG16(x) SWAP16(x)
#define FROM_BIG32(x) SWAP32(x)
#endif

#define ON  1
#define OFF 0

#define TO_EXT_RAM1         TO_EXT_PRGM_MEM
#define TO_EXT_RAM2         TO_EXT_DDR_PRGM_MEM

#define TO_EXT_DATA_RAM1         TO_EXT_DATA_MEM
//#define TRUE  1
//#define FALSE 0
/*------------------------------------------------------------------------------
*  @4 EXTERN STRUCTS
*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
*  @5 VARIABLES
*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
*  @6 FUNCTION PROTOTYPES
*-----------------------------------------------------------------------------*/

HAL_TIMER_HANDLE hal_timer_start(void (*aFuncPtr)(), uint32 aDummyId, uint32 aTime);
#if NO_LIBS
extern int sprintf(char * aOut, const char * aFormat, ...);
#endif

extern void asm_delay(uint32 )TO_EXT_RAM2;
#endif  /* End of file */


