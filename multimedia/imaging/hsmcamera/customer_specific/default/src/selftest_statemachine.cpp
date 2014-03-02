/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "camera.h"
#include "hsmcam.h" //rename_me camera_sm.h
#include "omxcamera.h"
#include "OMX_Core.h"
#include "OMX_CoreExt.h"

SCF_STATE CAM_SM::EnteringSelfTest(s_scf_event const *e)
{
	DBC_ASSERT(0);
	return SCF_STATE_PTR(&CAM_SM::SetFeature);

}


SCF_STATE CAM_SM::ProcessingSelfTest(s_scf_event const *e)
{
	DBC_ASSERT(0);
	return SCF_STATE_PTR(&CAM_SM::EnteringSelfTest);
}



