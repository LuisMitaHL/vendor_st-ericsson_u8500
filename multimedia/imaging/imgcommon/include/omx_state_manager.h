/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _OMX_STATE_MANAGER_H
#define _OMX_STATE_MANAGER_H

#include "OMX_Core.h"
#include "osi_trace.h"
#include <cm/inc/cm_type.h>
#include <ENS_DBC.h>

class COmxStateMgr
{
	public:
		COmxStateMgr();
		OMX_STATETYPE updateState(void);
		OMX_ERRORTYPE changeState(OMX_STATETYPE aState);
		OMX_STATETYPE getState(void);

	private:
		OMX_STATETYPE iState[2];
		t_uint16      iCounter;

};
#endif
