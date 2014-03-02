/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* Author:     : laurent.regnier@st.com
* Filename:   : omx_toolbox_shared.h
* Description : shared header for all cpp files
*****************************************************************************/
#ifndef _OMX_TOOLBOX_SHARED_H_
#define _OMX_TOOLBOX_SHARED_H_

#if !defined(OMX_TOOLBOX_API)
	#define OMX_TOOLBOX_API
#endif

//LANGUAGE_SC is defined with Symbian resource compiler
#ifdef LANGUAGE_SC  // LANGUAGE_SC is defined by Resource compiler

#else // NOT RESSOURCE COMPILER
	#include "osi_toolbox_lib.h"
	#ifdef _MSC_VER

	#endif

#endif //end of LANGUAGE_SC



#endif // _OMX_TOOLBOX_SHARED_H_
