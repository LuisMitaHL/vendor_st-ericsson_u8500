/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#undef OMXCOMPONENT
#define OMXCOMPONENT "TEST_MODE"

#include "test_mode.h"
#include "osi_trace.h"

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


CTestMode::CTestMode() {
	eCamTestMode = OMX_STE_TestModeNone;
}



