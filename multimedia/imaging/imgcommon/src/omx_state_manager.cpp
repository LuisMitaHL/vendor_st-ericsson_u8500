/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define OMXCOMPONENT "OMX_STATE_MGR"
#include "omx_state_manager.h"

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

COmxStateMgr::COmxStateMgr()
{
	iCounter = 0;
	iState[0] = OMX_StateMax;
}

OMX_STATETYPE COmxStateMgr::updateState()
{
	if(iCounter>2) DBC_ASSERT(0); //this cannot ocur.

	if(iState[0] != OMX_StateMax) {
		iCounter--;
		OMX_STATETYPE state = iState[0];
		iState[0] = OMX_StateMax;
		return state;
	}
	else {
		return OMX_StateMax;
	}
}

OMX_STATETYPE COmxStateMgr::getState()
{
	return iState[iCounter];
}

OMX_ERRORTYPE COmxStateMgr::changeState(OMX_STATETYPE aState)
{
	iState[iCounter] = aState;
	iCounter++;
	return OMX_ErrorNone;
}
