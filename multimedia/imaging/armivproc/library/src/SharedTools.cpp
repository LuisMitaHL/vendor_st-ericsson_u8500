/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "stdafx.h"
#include <stdarg.h>
#include "SharedTools.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

int DefaultReportError(int error_code, const char *Fmt, ...)
//*************************************************************************************
{// Default reporting function
	va_list args;
	va_start(args, Fmt);
	static char ErrorString[1024];
	vsprintf(ErrorString, Fmt, args);
	TRACE( "\n***********************\n***********************Error %d - %s\n***********************", error_code, ErrorString);
#ifndef __SYMBIAN32__
//#if 1
	fprintf(stderr, "\n***********************\n***********************Error %d: %s\n***********************", error_code, ErrorString);
#else
	RDebug::Printf("\nError %d: %s", error_code, ErrorString);

#endif
	va_end(args);
	return(0);
}

IMAGECONVERTER_API int (*_fnReportError)(int error_code, const char *, ...)=DefaultReportError;

bool IsPowerOfTwo(unsigned int val, unsigned int &pow)
//*************************************************************************************
{// return true when val is a power of two
	unsigned int mask=1;
	pow=0;
	while (val < mask)
	{
		if (val==mask)
			return(true);
		mask<<=1;
		++pow;
	}
	return(false);
}

