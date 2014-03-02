/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _SHAREDTOOLS_H_
#define _SHAREDTOOLS_H_

extern IMAGECONVERTER_API int (*_fnReportError)(int error_code, const char *, ...);

// Debug macro
#ifndef __SYMBIAN32__
	#define __MODULE__ __FILE__
#else
	//Remove symbiand definition of assert
	#undef ASSERT
#endif

//#define CHECK_ERROR(cdt) if (cdt) { _fnReportError(cdt, "CHECK_ERROR(%d) failed %s:%d\n", (cdt), __MODULE__, __LINE__);} else {}
#define CHECK_ERROR(cdt) (void) ( !(cdt) || ( _fnReportError(-1, "CHECK_ERROR(%d) failed %s:%d\n", (cdt), __MODULE__, __LINE__), 0) )

#ifdef _DEBUG
	#define DEBUG_ONLY(a) a
	#ifndef ASSERT
		// Use of !! to workaround overload of :! operator that can end at ||
		#define ASSERT(a) (void) ( (!!(a)) || (_fnReportError( eError_ASSERTION, "ASSERTION %s:%d", __MODULE__, __LINE__), 0) )
	#endif
	#define DBG_MSG printf
#else
	#define DEBUG_ONLY(a) 
	#ifndef ASSERT
		#define ASSERT(a)
	#endif
	#define DBG_MSG NullFunction
#endif


#ifndef S_OK
	#define S_OK 0
#endif

#ifndef TRACE
	#define TRACE NullFunction
#endif

inline void NullFunction(...) {}

//Helper for arrays
#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(a[0]))

IMAGECONVERTER_API bool IsPowerOfTwo(unsigned int val, unsigned int &pow);

#endif // End of #ifndef _SHAREDTOOLS_H_
