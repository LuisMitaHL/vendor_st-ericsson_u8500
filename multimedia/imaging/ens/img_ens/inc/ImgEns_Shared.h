/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _IMG_ENS_SHARED_H_
#define _IMG_ENS_SHARED_H_

#ifdef _MSC_VER
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif
	#pragma warning (disable: 4512) // C4512:  : assignment operator could not be generated
	#pragma warning (disable: 4251) // C4251: ' class xx needs to have dll-interface to be used by clients of class 
	#define DEBUG_TRACE_H
	#define DBGT_PTRACE printf
	//Enable alternate ost trace
	#define USE_ALTERNATE_OST 1
	#define snprintf _snprintf
	#include <crtdbg.h>
	#if (!defined(IMGENS_API) && defined(_DLL))
		#define IMGENS_API __declspec(dllimport)
	#endif
#else
#endif

#ifndef IMGENS_API 
	#define IMGENS_API 
#endif

/// max length for component name
#define IMG_ENS_COMPONENT_NAME_MAX_LENGTH 128

#include <stdio.h>
#include "omxil/OMX_Types.h"
#include "osi_toolbox_lib.h"

IMGENS_API void getImgOmxIlSpecVersion(OMX_VERSIONTYPE *pVersion);
IMGENS_API const OMX_VERSIONTYPE &getImgOmxIlSpecVersion();

class ImgEns_Component;

//IMGENS_API ImgEns_Component * getImgEnsComponent(OMX_HANDLETYPE hComp);

enum
{
	eImgEns_NoError =0,
};

//Tracing interface
IMGENS_API int ImgEns_SetTraceCallback(void (*logfn)(const char *name, unsigned int level));
IMGENS_API int ImgEns_SetTraceLevel(unsigned int nTraceLevel);
IMGENS_API int ImgEns_GetTraceLevel(unsigned int *pTraceLevel);

IMGENS_API int ImgEns_Report(int what, const char *file, size_t line, const char *fmt, ...);

#ifdef _MSC_VER
	#ifdef _DEBUG
		#define IMGENS_ASSERT(a) (void) ( (a) || ImgEns_Report(0, __FILE__, __LINE__, "ASSERTION") , _ASSERT(a) )
	#else
		#define IMGENS_ASSERT(a) (void) ( (a) || printf("\n\n ERROR ************************* %s:%d", __FILE__, __LINE__) || ImgEns_Report(0, , __FILE__, __LINE__, "ASSERTION"))
	#endif
#else
		#define IMGENS_ASSERT(a) (void) ( (a) || printf("\n\n ERROR ************************* %s:%d", __FILE__, __LINE__) )
#endif

#define DBC_PRECONDITION  IMGENS_ASSERT
#define DBC_POSTCONDITION IMGENS_ASSERT

// ER338925: do not check struct version and size in all configurations
#ifdef ENS_DONT_CHECK_STRUCT_SIZE_AND_VERSION
	#define CHECK_STRUCT_SIZE_AND_VERSION(ptr,type);
#else
	#define CHECK_STRUCT_SIZE_AND_VERSION(ptr,type) \
		{type * _tempstruct = (type *)ptr; OMX_VERSIONTYPE version; version.nVersion=0; getImgOmxIlSpecVersion(&version);\
		if (!(_tempstruct->nSize == sizeof(type) && ((_tempstruct->nVersion.nVersion & 0xFFFF) == (version.nVersion & 0xFFFF)))) IMGENS_ASSERT(0);}
#endif


IMGENS_API void ost_printf(const char *name, const char *format, ...);
inline void null_print(int , ...) {}

#ifdef USE_ALTERNATE_OST
// Alternate definition for OST trace
	#undef OMX_API_TRACE
	#undef MSG0
	#undef MSG1
	#undef MSG2
	#undef MSG3
	#undef MSG4
	#undef IN0
	#undef OUT0
	#undef OUTR

	#undef TRACE_DEBUG
	#undef TRACE_ERROR
	#undef TRACE_FLOW
	#undef MAX_OST_SIZE
	#undef OstTraceFiltInst0
	#undef OstTraceFiltInst1
	#undef OstTraceFiltInst2
	#undef OstTraceFiltInst3
	#undef OstTraceFiltInst4
	#undef OstTraceFiltInstData
	#undef OstTraceInt1

	// Enable ost trace
	#define OMX_API_TRACE 1
	#define MSG0 printf
	#define MSG1 printf
	#define MSG2 printf
	#define MSG3 printf
	#define MSG4 printf
	#define MSG4 printf
	#define MSG5 printf
	#define MSG6 printf
	#define MSG7 printf
	#define MSG_ printf
	#define IN0  printf
	#define OUT0 printf
	#define OUTR printf


	#define MAX_OST_SIZE 56
	#define OstTraceFiltInst0    ost_printf
	#define OstTraceFiltInst1    ost_printf
	#define OstTraceFiltInst2    ost_printf
	#define OstTraceFiltInst3    ost_printf
	#define OstTraceFiltInst4    ost_printf
	#define OstTraceFiltInstData ost_printf
	#define OstTraceInt1

	//Avoid inclusion of header
	#define __OSI_TRACE_H               // For osi-trace.h
	#define __OPEN_SYSTEM_TRACE_STE_H__ // for OpenSystemTrace_ste.h
#endif

#define TRACE_DEBUG  "debug"
#define TRACE_ERROR  "ERROR"
#define TRACE_FLOW   "flow "
#define TRACE_API    "Api  "

#ifdef _MSC_VER
	#include <omxil/OMX_Component.h>

	class IMGENS_API MMHwBuffer
	{ //Fake MMHwBuffer implementation
	public:
		static OMX_ERRORTYPE GetIndexExtension(OMX_STRING /*aParameterName*/, OMX_INDEXTYPE* /*apIndexType*/);
		static OMX_ERRORTYPE GetPortIndexExtension(enum OMX_INDEXTYPE,void *,unsigned long &);
		static OMX_ERRORTYPE Open(void *,enum OMX_INDEXTYPE,void *,class MMHwBuffer * &);
		static OMX_ERRORTYPE Destroy(class MMHwBuffer * &);
		static OMX_ERRORTYPE Close(class MMHwBuffer * &);
		virtual OMX_ERRORTYPE SetConfigExtension(OMX_HANDLETYPE aTunneledComponent, OMX_U32 aPortIndex);
	};
	//for blocking original include file
	#define _MMHWBUFFER_H_
#endif


#endif      // _IMG_ENS_SHARED_H_

