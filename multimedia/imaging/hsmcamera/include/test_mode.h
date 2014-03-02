/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TEST_MODE_H_
#define _TEST_MODE_H_

#include "osi_trace.h"
#include <cm/inc/cm_type.h>
#include "IFM_Types.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTestMode);
#endif
class CTestMode
{
	public :
		CTestMode();
		OMX_STE_TESTMODETYPE eCamTestMode;
};
#endif /**/
