/*****************************************************************************
* filename : AIQ_Memory.h
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
#ifndef _AIQ_MEMORY_H_
#define _AIQ_MEMORY_H_

#include "AIQ_CommonTypes.h"

#ifndef AIQ_COMMON_API
	#ifdef __SYMBIAN32__
		#define AIQ_COMMON_API IMPORT_C
	#else
		#define AIQ_COMMON_API 
	#endif
#endif

#ifdef AIQ_MEMTRACE_ENABLE
/*****************************************************************************
*  when AIQ_MEMTRACE_ENABLE is defined, memory (de)allocation process can be
*  traced thanks to these function, that shall be used instead of standard
*  library calls:
*
*  - Initialization of the Memory Tracer:
*  AIQ_Error AIQINT_InitMemTrace(char *fileName)
*    fileName    file name where the memory tracer will log all (de-)allocations.
*    this function must be called prior to any other functions of the AIQ memory tracer
*
*  - Memory allocation
*  AIQ_malloc(size,identifier,description)
*    size        size (in byte) to allocate
*    identifier  integer (ID) used to identify the caller
*    description character string helping identifying the allocation   
*    this function allocate size bytes and return a (void*) pointer on the allocated
*    address. It mimics stdlib's malloc function.
*
*  - Memory allocation and clearing
*  AIQ_calloc(nobj,size,identifier,description)
*    nobj        number of objects to allocate
*    size        size (in byte) of one object to allocate
*    identifier  integer (ID) used to identify the caller
*    description character string helping identifying the allocation   
*    this function allocate an array of nobj elements (of individual size size),
*    set the allocated memory with zeros and return a (void*) pointer on array start.
*    It mimics stdlib's calloc function.
*
*  - Memory de-allocation
*  AIQ_free(address)
*    address     address of the memory to be released.
*    This function releases the memory previously allocated at address address.
*    it mimics stdlib's free function.
*
*  - Comment insertion
*  AIQ_MemTraceInsertComment(comment)
*    comment     comment to be insterted in the trace file.
*    This function add a comment in the trace file.
*
*  - Current memory usage status report
*  AIQ_MemTraceStatus(comment)
*    comment     comment to be inserted before the memory status in the trace file.
*    This function logs in the trace file the current status of the memory allocations.
*    It prefixes the status by the comment.
*
*  - De-initialization of the Memory tracer
*  AIQ_DeInitMemTrace()
*    This function logs a final status in the trace file and close it.
* 
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
extern AIQ_COMMON_API AIQ_Error AIQINT_InitMemTrace          (char *fileName);
extern AIQ_COMMON_API void*     AIQINT_malloc                (AIQ_U32 size,AIQ_U32 identifier,char* description);
extern AIQ_COMMON_API void*     AIQINT_calloc                (AIQ_U32 nobj,AIQ_U32 size,AIQ_U32 identifier,char* description);
extern AIQ_COMMON_API void      AIQINT_free                  (void* address);
extern AIQ_COMMON_API void      AIQINT_MemTraceInsertComment (char* comment);
extern AIQ_COMMON_API void      AIQINT_MemTraceStatus        (char* comment);
extern AIQ_COMMON_API void      AIQINT_DeInitMemTrace        (void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#define AIQ_InitMemTrace(fileName)                   AIQINT_InitMemTrace((fileName))
#define AIQ_malloc(size,identifier,description)      AIQINT_malloc((size),(identifier),(description))
#define AIQ_calloc(nobj,size,identifier,description) AIQINT_calloc((nobj),(size),(identifier),(description))
#define AIQ_free(address)                            AIQINT_free((address))
#define AIQ_MemTraceInsertComment(comment)           AIQINT_MemTraceInsertComment((comment))
#define AIQ_MemTraceStatus(comment)                  AIQINT_MemTraceStatus((comment))
#define AIQ_DeInitMemTrace()                         AIQINT_DeInitMemTrace() 

#else
/*****************************************************************************
* when AIQ_MEMTRACE_ENABLE is not defined, memory (de)allocation process is 
* not traced, and memory tracer functions are mapped to stdlib functions.
*****************************************************************************/

#include <stdlib.h>

#define AIQ_InitMemTrace(fileName)                   AIQ_NullFunc()
#define AIQ_malloc(size,identifier,description)      malloc((size))
#define AIQ_calloc(nobj,size,identifier,description) calloc((nobj),(size))
#define AIQ_free(address)                            free((address))
#define AIQ_MemTraceInsertComment(comment)           
#define AIQ_MemTraceStatus(comment)                  
#define AIQ_DeInitMemTrace()                         


#endif /* AIQ_MEMTRACE_ENABLE */
#endif // _AIQ_MEMORY_H_

