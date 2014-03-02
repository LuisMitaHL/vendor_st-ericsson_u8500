/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* Author:     : @stericsson.com
* Filename:   : womDemoCpt_shared.h
* Description: shared header for all cpp files
*****************************************************************************/
#ifndef _WOMDEMOCPT_SHARED_H_
#define _WOMDEMOCPT_SHARED_H_

#ifndef WOMDEMOCPT_NAME0
	#define WOMDEMOCPT_NAME0   "OMX.STE.WOMDEMOCPT"
#endif
#ifndef WOMDEMOCPT_NAME1
	#define WOMDEMOCPT_NAME1   "OMX.STE.WOMDEMOCPT1"
#endif


//LANGUAGE_SC is defined with Symbian resource compiler
#ifdef LANGUAGE_SC
// LANGUAGE_SC is defined by Resource compiler

#else // NOT RESSOURCE COMPILER
enum enumwomDemoCptMode
{
	ewomDemoCpt_Type0,
	ewomDemoCpt_Type1,
};

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef WOMDEMOCPT_API
	#ifdef _MSC_VER
		#define WOMDEMOCPT_API __declspec(dllimport)
	#else
		#define WOMDEMOCPT_API 
	#endif
#endif


#endif //end of LANGUAGE_SC

#endif // _WOMDEMOCPT_SHARED_H_
