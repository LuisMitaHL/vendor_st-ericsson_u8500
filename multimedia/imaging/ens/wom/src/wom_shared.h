/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_SHARED_H_
#define _WOM_SHARED_H_

#ifdef _MSC_VER
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif
	#ifndef WINVER
		#define WINVER 0x800
	#endif
	#include <afx.h>
	#include <windows.h>
	#include <crtdbg.h>
	#pragma warning (disable: 4512) //C4512:  : assignment operator could not be generated
	#define DEBUG_TRACE_H //Avoid inclusion of incompatible files
	//WORKSTATION is used in X86 compil
	#define WORKSTATION
	#ifndef WOM_API
		#define WOM_API __declspec(dllimport)
	#endif
#else
	#define WOM_API
#endif
// standard libraries
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
// St ones
#include "osi_toolbox_lib.h"
#include "omx_toolbox_lib.h"
#include "Img_EnsWrapper_Lib.h"

WOM_API int WomReportCodeError(const char *file = "", unsigned int line=0);

#if (!defined(__SYMBIAN32__) && !defined(__MODULE__))
	#define __MODULE__ __FILE__
#endif


#ifndef WOM_ASSERT
#if (defined(_DEBUG) )
	#if defined(_MSC_VER)
		#define WOM_ASSERT(a) _ASSERT(a)
	#else
		#define WOM_ASSERT(a) (void) ( (a) || WomReportCodeError(__MODULE__ , __LINE__) )
	#endif
#else
//nothing in release
	#define WOM_ASSERT(a)
#endif

#endif

#define OmxEvent_FSMSIGnal(evt) evt->fsmEvent.signal

#ifndef OstTraceFiltInst0
	#define OstTraceFiltInst0(a,b)
#endif

#ifndef OstTraceFiltInst1
	#define OstTraceFiltInst1(a,b,c)
#endif

#ifndef OstTraceFiltInst2
	#define OstTraceFiltInst2(a,b,c1,c2)
#endif

#ifndef OstTraceFiltInst3
	#define OstTraceFiltInst3(a,b,c1,c2,c3)
#endif

typedef long unsigned int  t_uint32;
typedef unsigned int       t_uword;

#ifndef S_OK
	#define S_OK 0
#endif

enum enum_wom_error
{
	eError_WomSpecific       =-500,
	eError_UnsupportedDomain,
	//eError_CannotAllocateHeaderList,
	//eError_CannotAllocateHeader,
	eError_WrongPortIndex,
};
#define WOM_MAX_NAME_SIZE 32

#ifdef _MSC_VER
// avoid inclusion of incompatible headers
	#define COMMON_INTERFACE_TYPE_H
	#define COMMON_STUB_REQUIREAPI_H
	#define PROXY_COMMUNICATION_H
	#define COMMON_INTERFACE_TYPE_H
#endif


#endif //_WOM_SHARED_H_
