/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __OSTTRACE_H__
#define __OSTTRACE_H__

#define KBTraceCategoryOpenSystemTrace 0xA2u
#define KBTraceSubCategoryOpenSystemTrace 0x00u
#define OST_TRACE_ENTITY 0xAA


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __flexcc2__
#define TRACEMSGID_TYPE unsigned long
#else
#define TRACEMSGID_TYPE unsigned int
#endif


#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif

#if defined (__ARM_LINUX) || defined (WORKSTATION) || defined (MMDSP_TRACE_TO_ARM)
IMPORT_C void OSTTrace0 ( TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace1 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned int param, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace2 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned int param1, unsigned int param2, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace3 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned int param1, unsigned int param2, unsigned int param3, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace4 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceData (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, unsigned char *ptr, unsigned int length, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceFloat1 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, double param, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceFloat2 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, double param1, double param2, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceFloat3 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, double param1, double param2, double param3, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceFloat4 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char* aTraceText, double param1, double param2, double param3, double param4, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

#else
IMPORT_C void OSTTrace0 ( TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace1 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace2 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace3 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTrace4 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

IMPORT_C void OSTTraceData (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned char *ptr, unsigned int length, unsigned short trace_enable, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

#endif

#ifndef __flexcc2__
void OSTTraceInit(void);
void OSTTraceDeInit(void);
void OST_Printf(const char* fmt, ...); 
void OSTTraceSelectOutput(void);
#endif

#ifdef __cplusplus
 }
#endif

#endif /* __NOST_H__ */
