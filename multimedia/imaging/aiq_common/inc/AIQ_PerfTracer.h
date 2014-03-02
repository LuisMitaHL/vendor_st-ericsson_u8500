/*****************************************************************************
* filename : AIQ_PerfTracer.h
* in       : AIQ_Common
* author   : Olivier Pothier (olivier.pothier@stericsson.com)
*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifdef _MSC_VER
	#pragma once
#endif
#ifndef _AIQ_PERFTRACER_H_
#define _AIQ_PERFTRACER_H_

#include "AIQ_CommonTypes.h"

#ifndef AIQ_COMMON_API
	#ifdef __SYMBIAN32__
		#define AIQ_COMMON_API IMPORT_C
	#else
		#define AIQ_COMMON_API 
	#endif
#endif

#if ( defined(AIQ_PERFTRACE_ENABLE) && !defined(__ARM_THINK) )
/*****************************************************************************
*  when AIQ_PERFTRACE_ENABLE is defined, 
*
*  - Initialization of the Performance Tracer:
*  AIQ_Error AIQ_InitPerfTrace(char *fileName)
*    fileName    file name where the performance tracer will log all ellapsed time measures.
*    this function must be called prior to any other functions of the AIQ performance tracer.
*
*  - other functions to be detailed when rush hours are over...
*
*  - De-initialization of the Performance tracer
*  AIQ_DeInitPerfTrace()
*    This function logs a final status in the trace file and close it.
* 
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
extern AIQ_COMMON_API AIQ_Error AIQINT_InitPerfTrace         (char *fileName);
extern AIQ_COMMON_API AIQ_Error AIQINT_StartPerfCounter      (AIQ_U8 id);
extern AIQ_COMMON_API AIQ_Error AIQINT_StopPerfCounter       (AIQ_U8 id,char* description);
extern AIQ_COMMON_API void      AIQINT_FlushPerfTrace        (void);
extern AIQ_COMMON_API void      AIQINT_DeInitPerfTrace       (void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#define AIQ_InitPerfTrace(fileName)         AIQINT_InitPerfTrace((fileName))
#define AIQ_StartPerfCounter(id)            AIQINT_StartPerfCounter((id))
#define AIQ_StopPerfCounter(id,description) AIQINT_StopPerfCounter((id),(description))
#define AIQ_FlushPerfTrace()                AIQINT_FlushPerfTrace()
#define AIQ_DeInitPerfTrace()               AIQINT_DeInitPerfTrace()

#else
/*****************************************************************************
* when AIQ_MEMTRACE_ENABLE is not defined, memory (de)allocation process is 
* not traced, and memory tracer functions are mapped to stdlib functions.
*****************************************************************************/

#define AIQ_InitPerfTrace(fileName)         AIQ_NullFunc()
#define AIQ_StartPerfCounter(id)            AIQ_NullFunc()
#define AIQ_StopPerfCounter(id,description) AIQ_NullFunc()
#define AIQ_FlushPerfTrace()             
#define AIQ_DeInitPerfTrace()            

#endif /* AIQ_PERFTRACE_ENABLE */


#endif // _AIQ_PERFTRACER_H_

