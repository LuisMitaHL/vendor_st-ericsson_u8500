/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WRAPPER_OPENMAX_LIB_H_
#define _WRAPPER_OPENMAX_LIB_H_

#ifdef _MSC_VER
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif

#ifndef WIN32
		#define WIN32 
	#endif
#endif

#include <stddef.h>
#include <OMX_Types.h>
#include <OMX_Core.h>

#include "wrapper_openmax_tools.h"
#include "openmax_component_proxy.h"
#include "openmax_component.h"
#include "openmax_processor/inc/openmax_processor.hpp"

#endif // _WRAPPER_OPENMAX_LIB_H_
