/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGECONVERTER_AFX
#define _IMAGECONVERTER_AFX

#ifdef _MSC_VER
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS 1
	#ifdef _AFXDLL
		#include <afx.h>
	#else
		// #define DEBUG_NEW new
	#endif
	#include <stdio.h>
	#include "targetver.h"
	#include <tchar.h>
	#include <crtdbg.h>
	#include <windows.h>
	#include <windows.h>
	#include "lr_foundation/lr_foundation.h"
	#define IMAGECONVERTER_API __declspec(dllexport)
	#define strcasecmp _stricmp
#else //Not visual win32 compil
	#include <stdio.h>
	#define IMAGECONVERTER_API
#endif

#ifdef __SYMBIAN32__
	#include "e32debug.h"
#endif

#endif // _IMAGECONVERTER_AFX
