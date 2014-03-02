/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h" // rename_me sc_cam.h
#include "camera.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_lowpower_statemachineTraces.h"
#endif

/*Start low power timer */
void CAM_SM::startLowPowerTimer()
{
    MSG0("Start LowPower timer\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Start LowPower timer", (&mENSComponent));
    bLowPower = FALSE;		
    Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
    Cam->iCheckTimerRunning = true;

}
/*Stop low power timer */
void CAM_SM::stopLowPowerTimer()
{
    MSG0("Stop LowPower timer\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Stop LowPower timer", (&mENSComponent));
    Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
    Cam->iCheckTimerRunning = false;
    Cam->iNbLowPowerInterval500Msec=0;
}

