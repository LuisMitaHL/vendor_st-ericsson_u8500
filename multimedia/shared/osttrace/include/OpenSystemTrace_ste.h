// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Trace API
//
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Adaptation to support instance filtering with lightweise feature set to support
 * STE OpenMAx code instrumentation
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __OPEN_SYSTEM_TRACE_STE_H__
#define __OPEN_SYSTEM_TRACE_STE_H__

#include <osttrace.h>
// Constants

/**
BTrace category ID for traces
*/
//const unsigned int KBTraceCategoryOpenSystemTrace = 0xA2u;
#define KBTraceCategoryOpenSystemTrace 0xA2u

#define GET_LINE                  __LINE__
#define GEN2(GET_FILE, GET_LINE)  FILE_##GET_FILE##_##GET_LINE
#define GEN1(GET_FILE, GET_LINE)  GEN2(GET_FILE, GET_LINE)
#define GEN_ID                    GEN1(GET_FILE, GET_LINE)


// Macros

/**
Preprocessor category for all traces off.
This should not be used from traces
*/
#define OST_TRACE_CATEGORY_NONE 0x00000000

/**
Preprocessor category for production traces
*/
#define OST_TRACE_CATEGORY_PRODUCTION 0x00000001

/**
Preprocessor category for RnD traces
*/
#define OST_TRACE_CATEGORY_RND 0x00000002

/**
Preprocessor category for performance measurement traces
*/
#define OST_TRACE_CATEGORY_PERFORMANCE_MEASUREMENT 0x00000004

/**
Preprocessor level for all traces on. 
This should not be used from traces
*/
#ifndef __flexcc2__
/* The constant is too large for an 'int' on MMDSP; so, since we
aren't supposed to use it, we don't define it!*/
#define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
#endif

/**
A flag, which specifies if the compiler has been run for the component
*/
#if defined( OST_TRACE_COMPILER_IN_USE )


/**
The default preprocessor category is RND
Component may override this by defining 
OST_TRACE_LEVEL before including this file
*/
#if !defined (OST_TRACE_CATEGORY)
#define OST_TRACE_CATEGORY OST_TRACE_CATEGORY_RND
#endif

#if !defined (OST_TRACE_CHANNEL)
#define OST_TRACE_CHANNEL 99
#endif

#if !defined (OST_TRACE_ENTITY)
#define OST_TRACE_ENTITY 0x01
#endif

#if !defined (OST_TRACE_GROUP)
#define OST_TRACE_GROUP TRACE_FLOW
#endif

#define QUOTEME(x) #x
#define MK_STR(x) QUOTEME(x)

#ifndef MY_OSTTRACE_LOG_TAG
#define MY_OSTTRACE_LOG_TAG
#endif

#ifdef ANDROID
#define LOG_PREFIX MY_OSTTRACE_LOG_TAG 
#else
#define LOG_PREFIX "V/" "OSTTRACE" ": " MY_OSTTRACE_LOG_TAG
#endif

#ifndef __MYFILE__
#define __MYFILE__ __FILE__
#endif

#define OMX_HANDLE_PREFIX " p=0x%x: ID %d:"

#if defined (__ARM_LINUX) || defined (WORKSTATION)
#if 0
  #define AT(x) LOG_PREFIX ":" __MYFILE__ ":" MK_STR(__LINE__) ":" x "\n"
#else
  #define AT(x) LOG_PREFIX ":" x
#endif
#elif defined (__flexcc2__)
  #define AT(x) LOG_PREFIX x "\n"
//#define EXTMEM
#else
#endif


#if defined (__ARM_LINUX) || defined (WORKSTATION)
  #define OSTTrace0_wrp(a, b, str, c, d, e)                    OSTTrace0(a, b, AT(str), c, d, e)
  #define OSTTrace1_wrp(a, b, str, c, d, e, f)                 OSTTrace1(a, b, AT(str), c, d, e, f)
  #define OSTTrace2_wrp(a, b, str, c, d, e, f, g)              OSTTrace2(a, b, AT(str), c, d, e, f, g)
  #define OSTTrace3_wrp(a, b, str, c, d, e, f, g, h)           OSTTrace3(a, b, AT(str), c, d, e, f, g, h)
  #define OSTTrace4_wrp(a, b, str, c, d, e, f, g, h, i)        OSTTrace4(a, b, AT(str), c, d, e, f, g, h, i)
  #define OSTTraceData_wrp(a, b, str, c, d, e, f, g)           OSTTraceData(a, b, AT(str), c, d, e, f, g)

  #define OSTTraceFloat1_wrp(a, b, str, c, d, e, f)            OSTTraceFloat1(a, b, AT(str), c, d, e, f)
  #define OSTTraceFloat2_wrp(a, b, str, c, d, e, f, g)         OSTTraceFloat2(a, b, AT(str), c, d, e, f, g)
  #define OSTTraceFloat3_wrp(a, b, str, c, d, e, f, g, h)      OSTTraceFloat3(a, b, AT(str), c, d, e, f, g, h)
  #define OSTTraceFloat4_wrp(a, b, str, c, d, e, f, g, h, i)   OSTTraceFloat4(a, b, AT(str), c, d, e, f, g, h, i)
#else
  #define OSTTrace0_wrp(a, b, str, c, d, e)                    OSTTrace0(a, b, c, d, e)
  #define OSTTrace1_wrp(a, b, str, c, d, e, f)                 OSTTrace1(a, b, c, d, e, f)
  #define OSTTrace2_wrp(a, b, str, c, d, e, f, g)              OSTTrace2(a, b, c, d, e, f, g)
  #define OSTTrace3_wrp(a, b, str, c, d, e, f, g, h)           OSTTrace3(a, b, c, d, e, f, g, h)
  #define OSTTrace4_wrp(a, b, str, c, d, e, f, g, h, i)        OSTTrace4(a, b, c, d, e, f, g, h, i)
  #define OSTTraceData_wrp(a, b, str, c, d, e, f, g)           OSTTraceData(a, b, c, d, e, f, g)
#endif



/**
Trace with no parameters

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
*/
#define OstTraceDef0(aCategory, aGroupName, aTraceName, aTraceText) \
  if (aCategory & OST_TRACE_CATEGORY)  \
    OSTTrace0_wrp(KOstTraceComponentID, aTraceName, aTraceText, 0, 0, 0)

/**
Trace with one 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam The 32-bit value to be traced
*/
#define OstTraceDef1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTrace1_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam, 0, 0, 0)

#define OstTraceDefFloat1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTraceFloat1_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam, 0, 0, 0)


/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
#define OstTraceDef2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTrace2_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, 0, 0, 0)

#define OstTraceDefFloat2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTraceFloat2_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, 0, 0, 0)

/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
#define OstTraceDef3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTrace3_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, aParam3, 0, 0, 0)

#define OstTraceDefFloat3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTraceFloat3_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, aParam3, 0, 0, 0)

/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
@param aParam4 The 32-bit value to be traced
*/
#define OstTraceDef4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTrace4_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4, 0, 0, 0)

#define OstTraceDefFloat4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTraceFloat4_wrp(KOstTraceComponentID, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4, 0, 0, 0)


/**
Trace with more than 32 bits of data

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
#define OstTraceDefData(aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength) \
  if (aCategory & OST_TRACE_CATEGORY) \
    OSTTraceData_wrp(KOstTraceComponentID, aTraceName, aTraceText, aPtr, aLength, 0, 0, 0)


/**
*************** RnD trace macros ***************
*/

/**
RnD trace with no parameters

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
*/
#define OstTraceInt0(aGroupName, aTraceText) \
  OstTraceDef0 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText)

/**
RnD trace with one 32-bit parameter

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam The 32-bit value to be traced
*/
#define OstTraceInt1(aGroupName, aTraceText, aParam) \
  OstTraceDef1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam)

#define OstTraceFloat1(aGroupName, aTraceText, aParam) \
  OstTraceDefFloat1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam)


/**
RnD trace with more than 32 bits of data

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
#define OstTraceArray(aGroupName, aTraceText, aPtr, aLength) \
  OstTraceDefData (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aPtr, aLength)


/**
RnD trace with two parameters.
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
*/
#define OstTraceInt2( aGroupName, aTraceText, aParam1, aParam2 ) \
    OstTraceDef2( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2 )

#define OstTraceFloat2( aGroupName, aTraceText, aParam1, aParam2 ) \
    OstTraceDefFloat2( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2 )

/**
RnD trace with three parameters.
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
@param aParam3 The third parameter to be traced
*/
#define OstTraceInt3( aGroupName, aTraceText, aParam1, aParam2, aParam3 ) \
    OstTraceDef3( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3 )

#define OstTraceFloat3( aGroupName, aTraceText, aParam1, aParam2, aParam3 ) \
    OstTraceDefFloat3( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3 )


/**
RnD trace with four parameters.  
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text itself is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
@param aParam3 The third parameter to be traced
@param aParam4 The fourth parameter to be traced
*/
#define OstTraceInt4( aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) \
    OstTraceDef4( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4 )

#define OstTraceFloat4( aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) \
    OstTraceDefFloat4( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4 )


/**
Filtered Trace with no parameters. Filtering is done using simple bitfield matching

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
*/
#if defined(__cplusplus)
  #define OstTraceFiltInstDef0(aCategory, aGroupName, aTraceName, aTraceText) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace0_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDef0(aCategory, aGroupName, aTraceName, aTraceText) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace0_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

/**
Filtered Trace with no parameters.

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aInstPtr: Component Instance
*/
#if defined(__cplusplus)
  #define OstTraceFiltStaticDef0(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace0_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
  #define OstTraceFiltStaticDef0(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace0_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif


/**
Fitlered Trace with one 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam The 32-bit value to be traced
*/
#if defined( __cplusplus)
  #define OstTraceFiltInstDef1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDef1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
  #define OstTraceFiltInstDefFloat1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTraceFloat1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDefFloat1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTraceFloat1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif


/**
Fitlered Trace with one 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aInstPtr: Component Instance
@param aParam The 32-bit value to be traced

*/
#if defined( __cplusplus)
  #define OstTraceFiltStaticDef1(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aParam, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
  #define OstTraceFiltStaticDef1(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aParam, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
  #define OstTraceFiltStaticDefFloat1(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
      OSTTraceFloat1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aParam, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
  #define OstTraceFiltStaticDefFloat1(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTraceFloat1_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aParam, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif


/**
Filtered Trace with two 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
#if defined( __cplusplus)
  #define OstTraceFiltInstDef2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDef2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
  #define OstTraceFiltInstDefFloat2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTraceFloat2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDefFloat2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTraceFloat2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

/**
Filtered Trace with two 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aInstPtr: Component Instance
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
#if defined( __cplusplus)
  #define OstTraceFiltStaticDef2(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
  #define OstTraceFiltStaticDef2(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
  #define OstTraceFiltStaticDefFloat2(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
      OSTTraceFloat2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
  #define OstTraceFiltStaticDefFloat2(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2) \
    if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTraceFloat2_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif


/**
Filtered Trace with three 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
*/
#if defined( __cplusplus)
  #define OstTraceFiltInstDef3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTrace3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, aParam3, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDef3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTrace3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, aParam3, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
  #define OstTraceFiltInstDefFloat3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
    if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
      OSTTraceFloat3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, aParam3, 1, this->getId1(), this->getParentHandle())
#else
  #define OstTraceFiltInstDefFloat3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
    if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
      OSTTraceFloat3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
          aParam1, aParam2, aParam3, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

/**
Filtered Trace with three 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aInstPtr: Component Instance
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
*/
#if defined( __cplusplus)
#define OstTraceFiltStaticDef3(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
    OSTTrace3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
#define OstTraceFiltStaticDef3(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTrace3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
#define OstTraceFiltStaticDefFloat3(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
    OSTTraceFloat3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
#define OstTraceFiltStaticDefFloat3(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTraceFloat3_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif

/**
Filtered Trace with four 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
@param aParam4 The 32-bit value to be traced
*/
#if defined( __cplusplus)
#define OstTraceFiltInstDef4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
    OSTTrace4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, aParam4, 1, this->getId1(), this->getParentHandle())
#else
#define OstTraceFiltInstDef4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTrace4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, aParam4, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
#define OstTraceFiltInstDefFloat4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
    OSTTraceFloat4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, aParam4, 1, this->getId1(), this->getParentHandle())
#else
#define OstTraceFiltInstDefFloat4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTraceFloat4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
        aParam1, aParam2, aParam3, aParam4, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif


/**
Filtered Trace with four 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aInstPtr: Component Instance
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
@param aParam4 The 32-bit value to be traced
*/
#if defined( __cplusplus)
#define OstTraceFiltStaticDef4(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
OSTTrace4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
    aParam1, aParam2, aParam3, aParam4, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())

#else
#define OstTraceFiltStaticDef4(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
OSTTrace4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
    aParam1, aParam2, aParam3, aParam4, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif

#if defined( __cplusplus)
#define OstTraceFiltStaticDefFloat4(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
OSTTraceFloat4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
    aParam1, aParam2, aParam3, aParam4, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())

#else
#define OstTraceFiltStaticDefFloat4(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
OSTTraceFloat4_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, \
    aParam1, aParam2, aParam3, aParam4, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif



/**
Filtered Trace with more than 32 bits of data

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
#if defined( __cplusplus)
#define OstTraceFiltInstDefData(aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength) \
  if ((aCategory & OST_TRACE_CATEGORY) && (this->getTraceEnable() & (aTraceName >> 16))) \
    OSTTraceData_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aPtr, \
        aLength, 1, this->getId1(), this->getParentHandle())
#else

#define OstTraceFiltInstDefData(aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength) \
  if ((aCategory & OST_TRACE_CATEGORY) && (((TRACE_t *)this)->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTraceData_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aPtr, \
        aLength, 1, ((TRACE_t *)this)->mId1, ((TRACE_t *)this)->mTraceInfoPtr->parentHandle)
#endif


/**
Filtered Trace with more than 32 bits of data

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aInstPtr: Component Instance
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
#if defined( __cplusplus)
#define OstTraceFiltStaticDefData(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aPtr, aLength) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->getTraceEnable() & (aTraceName >> 16))) \
    OSTTraceData_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aPtr, \
        aLength, 1, aInstPtr->getId1(), aInstPtr->getParentHandle())
#else
#define OstTraceFiltStaticDefData(aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aPtr, aLength) \
  if ((aCategory & OST_TRACE_CATEGORY) && (aInstPtr->mTraceInfoPtr->traceEnable & (aTraceName >> 16))) \
    OSTTraceData_wrp(KOstTraceComponentID, aTraceName, OMX_HANDLE_PREFIX aTraceText, aPtr, \
        aLength, 1, aInstPtr->mId1, aInstPtr->mTraceInfoPtr->parentHandle)
#endif


#define OstTraceFiltInst0(aGroupName, aTraceText) \
  OstTraceFiltInstDef0 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText)

#define OstTraceFiltStatic0(aGroupName, aTraceText, aInstPtr) \
  OstTraceFiltStaticDef0 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr))

#define OstTraceFiltInst1(aGroupName, aTraceText, aParam) \
  OstTraceFiltInstDef1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam)

#define OstTraceFiltStatic1(aGroupName, aTraceText, aInstPtr, aParam) \
  OstTraceFiltStaticDef1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam)

#define OstTraceFiltInstFloat1(aGroupName, aTraceText, aParam) \
  OstTraceFiltInstDefFloat1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam)

#define OstTraceFiltStaticFloat1(aGroupName, aTraceText, aInstPtr, aParam) \
  OstTraceFiltStaticDefFloat1 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam)

#define OstTraceFiltInst2(aGroupName, aTraceText, aParam1, aParam2) \
  OstTraceFiltInstDef2 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2)

#define OstTraceFiltStatic2(aGroupName, aTraceText, aInstPtr, aParam1, aParam2) \
  OstTraceFiltStaticDef2 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2)

#define OstTraceFiltInstFloat2(aGroupName, aTraceText, aParam1, aParam2) \
  OstTraceFiltInstDefFloat2 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2)

#define OstTraceFiltStaticFloat2(aGroupName, aTraceText, aInstPtr, aParam1, aParam2) \
  OstTraceFiltStaticDefFloat2 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2)

#define OstTraceFiltInst3(aGroupName, aTraceText, aParam1, aParam2, aParam3) \
  OstTraceFiltInstDef3 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3)

#define OstTraceFiltStatic3(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  OstTraceFiltStaticDef3 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2, aParam3)

#define OstTraceFiltInstFloat3(aGroupName, aTraceText, aParam1, aParam2, aParam3) \
  OstTraceFiltInstDefFloat3 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3)

#define OstTraceFiltStaticFloat3(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3) \
  OstTraceFiltStaticDefFloat3 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2, aParam3)

#define OstTraceFiltInst4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  OstTraceFiltInstDef4 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4)

#define OstTraceFiltStatic4(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  OstTraceFiltStaticDef4 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2, aParam3, aParam4)

#define OstTraceFiltInstFloat4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
  OstTraceFiltInstDefFloat4 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4)

#define OstTraceFiltStaticFloat4(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4) \
  OstTraceFiltStaticDefFloat4 (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aParam1, aParam2, aParam3, aParam4)

#define OstTraceFiltInstData(aGroupName, aTraceText, aPtr, aLength) \
  OstTraceFiltInstDefData (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aPtr, aLength)

#define OstTraceFiltStaticData(aGroupName, aTraceText, aInstPtr, aPtr, aLength) \
  OstTraceFiltStaticDefData (OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, (aInstPtr), aPtr, aLength)

#else // OST_TRACE_COMPILER_IN_USE

/**
API is defined empty if the trace compiler has not been run
*/

#define OstTraceDef0 (aCategory, aGroupName, aTraceName, aTraceText )
#define OstTraceDef1 (aCategory, aGroupName, aTraceName, aTraceText, aParam )
#define OstTraceDef2 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2 )
#define OstTraceDef3 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceDef4 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceDefData (aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength )

#define OstTraceInt0(aGroupName, aTraceText )
#define OstTraceInt1(aGroupName, aTraceText, aParam )
#define OstTraceInt2(aGroupName, aTraceText, aParam1, aParam2 )
#define OstTraceInt3(aGroupName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceInt4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceArray(aGroupName, aTraceText, aPtr, aLength )

#define OstTraceFiltInstDef0 (aCategory, aGroupName, aTraceName, aTraceText )
#define OstTraceFiltInstDef1 (aCategory, aGroupName, aTraceName, aTraceText, aParam )
#define OstTraceFiltInstDef2 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2 )
#define OstTraceFiltInstDef3 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceFiltInstDef4 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltInstDefData (aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength )

#define OstTraceFiltInst0(aGroupName, aTraceText )
#define OstTraceFiltInst1(aGroupName, aTraceText, aParam )
#define OstTraceFiltInst2(aGroupName, aTraceText, aParam1, aParam2 )
#define OstTraceFiltInst3(aGroupName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceFiltInst4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltInstData(aGroupName, aTraceText, aPtr, aLength )

#define OstTraceFiltStaticDef0 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr)
#define OstTraceFiltStaticDef1 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam )
#define OstTraceFiltStaticDef2 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2 )
#define OstTraceFiltStaticDef3 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3 )
#define OstTraceFiltStaticDef4 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltStaticDefData (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aPtr, aLength )

#define OstTraceFiltStatic0(aGroupName, aTraceText, aInstPtr )
#define OstTraceFiltStatic1(aGroupName, aTraceText, aInstPtr, aParam )
#define OstTraceFiltStatic2(aGroupName, aTraceText, aInstPtr, aParam1, aParam2 )
#define OstTraceFiltStatic3(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3 )
#define OstTraceFiltStatic4(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltStaticData(aGroupName, aTraceText, aInstPtr, aPtr, aLength )

#define OSTTrace0_wrp(a, b, str, c, d, e)
#define OSTTrace1_wrp(a, b, str, c, d, e, f)
#define OSTTrace2_wrp(a, b, str, c, d, e, f, g)
#define OSTTrace3_wrp(a, b, str, c, d, e, f, g, h)
#define OSTTrace4_wrp(a, b, str, c, d, e, f, g, h, i)
#define OSTTraceData_wrp(a, b, str, c, d, e, f, g)



#define OstTraceDefFloat0 (aCategory, aGroupName, aTraceName, aTraceText )
#define OstTraceDefFloat1 (aCategory, aGroupName, aTraceName, aTraceText, aParam )
#define OstTraceDefFloat2 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2 )
#define OstTraceDefFloat3 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceDefFloat4 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceDefFloatData (aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength )

#define OstTraceFloat0(aGroupName, aTraceText )
#define OstTraceFloat1(aGroupName, aTraceText, aParam )
#define OstTraceFloat2(aGroupName, aTraceText, aParam1, aParam2 )
#define OstTraceFloat3(aGroupName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceFloat4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFloatArray(aGroupName, aTraceText, aPtr, aLength )

#define OstTraceFiltInstDefFloat0 (aCategory, aGroupName, aTraceName, aTraceText )
#define OstTraceFiltInstDefFloat1 (aCategory, aGroupName, aTraceName, aTraceText, aParam )
#define OstTraceFiltInstDefFloat2 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2 )
#define OstTraceFiltInstDefFloat3 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceFiltInstDefFloat4 (aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltInstDefFloatData (aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength )

#define OstTraceFiltInstFloat0(aGroupName, aTraceText )
#define OstTraceFiltInstFloat1(aGroupName, aTraceText, aParam )
#define OstTraceFiltInstFloat2(aGroupName, aTraceText, aParam1, aParam2 )
#define OstTraceFiltInstFloat3(aGroupName, aTraceText, aParam1, aParam2, aParam3 )
#define OstTraceFiltInstFloat4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltInstFloatData(aGroupName, aTraceText, aPtr, aLength )

#define OstTraceFiltStaticDefFloat0 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr)
#define OstTraceFiltStaticDefFloat1 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam )
#define OstTraceFiltStaticDefFloat2 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2 )
#define OstTraceFiltStaticDefFloat3 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3 )
#define OstTraceFiltStaticDefFloat4 (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltStaticDefFloatData (aCategory, aGroupName, aTraceName, aTraceText, aInstPtr, aPtr, aLength )

#define OstTraceFiltStaticFloat0(aGroupName, aTraceText, aInstPtr )
#define OstTraceFiltStaticFloat1(aGroupName, aTraceText, aInstPtr, aParam )
#define OstTraceFiltStaticFloat2(aGroupName, aTraceText, aInstPtr, aParam1, aParam2 )
#define OstTraceFiltStaticFloat3(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3 )
#define OstTraceFiltStaticFloat4(aGroupName, aTraceText, aInstPtr, aParam1, aParam2, aParam3, aParam4 )
#define OstTraceFiltStaticFloatData(aGroupName, aTraceText, aInstPtr, aPtr, aLength )

#define OSTTraceFloat0_wrp(a, b, str, c, d, e)
#define OSTTraceFloat1_wrp(a, b, str, c, d, e, f)
#define OSTTraceFloat2_wrp(a, b, str, c, d, e, f, g)
#define OSTTraceFloat3_wrp(a, b, str, c, d, e, f, g, h)
#define OSTTraceFloat4_wrp(a, b, str, c, d, e, f, g, h, i)


#endif // OST_TRACE_COMPILER_IN_USE



#endif /* __OPEN_SYSTEM_TRACE_H__ */
