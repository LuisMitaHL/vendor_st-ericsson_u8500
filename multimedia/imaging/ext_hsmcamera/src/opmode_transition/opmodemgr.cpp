/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_opmodemgr.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "OPMD"
#include "debug_trace.h"

/* constructor */
CopModeMgr::CopModeMgr():
     UserBurstNbFrames(0)
    ,ConfigCapturePort(1)
    ,AutoPauseAfterCapture(OMX_FALSE)
{
    u32_frameLimit = 1;
    bBurstFrameLimited = true;

    CurrentOperatingMode = OpMode_Cam_StillPreview;
    captureRequest[CAMERA_PORT_OUT0] = 0;
    captureRequest[CAMERA_PORT_OUT1] = 0;
    ConfigCapturing[CAMERA_PORT_OUT0] =OMX_FALSE;
    ConfigCapturing[CAMERA_PORT_OUT1] =OMX_FALSE;

    waitingEOS[CAMERA_PORT_OUT0]=OMX_FALSE;
    waitingEOS[CAMERA_PORT_OUT1]=OMX_FALSE;
}


void CopModeMgr::UpdateCurrentOpMode() {

    switch(CurrentOperatingMode) {
    case OpMode_Cam_StillPreview:
        PreviousOperatingMode = OpMode_Cam_StillPreview;

        if((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && IsBurstLimited() && (1==GetBurstFrameLimit())) {
            // Single Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureSingle;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureSingle - %d", CurrentOperatingMode);
        } else if((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && IsBurstLimited()) {
            // Limited Burst Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d", CurrentOperatingMode);
        } else if ((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && !IsBurstLimited()) {
            // Infinite Burst Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d", CurrentOperatingMode);
        } else {
            DBC_ASSERT(0);
            // 1. Either port has been overwritten or burst has been activated because of memory corruption
        }
        break;

    case OpMode_Cam_VideoPreview:
        PreviousOperatingMode = OpMode_Cam_VideoPreview;

        if((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && IsBurstLimited() && (1==GetBurstFrameLimit())) {
            // Single Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureSingleInVideo;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureSingleInVideo - %d", CurrentOperatingMode);
        } else if((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && IsBurstLimited()) {
            // Limited Burst Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d", CurrentOperatingMode);
        } else if ((ConfigCapturing[OpModeCapturePort]==OMX_TRUE) && !IsBurstLimited()) {
            // Infinite Burst Capture
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d", CurrentOperatingMode);
        } else {
            DBC_ASSERT(0);
            // 1. portId has been overwritten
            // 2. ConfigCapturing set  to false on a VideoPreview => cannot be rejected at applyConfig but error eventHandler in processPendingEvent
        }
        break;

    case OpMode_Cam_StillCaptureSingle:
        PreviousOperatingMode = OpMode_Cam_StillCaptureSingle;

        if(ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_StillPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillPreview - %d", CurrentOperatingMode);
        }else {
            DBC_ASSERT(0);
            // 1. Another Capture is not yet done, this cannot not happen as it is prevented by the differed event mechanism
        }
        break;

    case OpMode_Cam_StillCaptureBurstLimited:
        PreviousOperatingMode = OpMode_Cam_StillCaptureBurstLimited;

        if(ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_StillPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillPreview - %d", CurrentOperatingMode);
        } else {
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfinite - %d", CurrentOperatingMode);
        }
        break;

    case OpMode_Cam_StillCaptureBurstInfinite:
        PreviousOperatingMode = OpMode_Cam_StillCaptureBurstInfinite;

        if (ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_StillPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillPreview - %d", CurrentOperatingMode);
        } else {
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimited;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimited - %d", CurrentOperatingMode);
        }
        break;

    case OpMode_Cam_StillCaptureSingleInVideo:
        PreviousOperatingMode = OpMode_Cam_StillCaptureSingleInVideo;

        if(ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_VideoPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", CurrentOperatingMode);
        }else {
            DBC_ASSERT(0);
            // 1. Another Capture is not yet done, this cannot not happen as it is prevented by the differed event mechanism
        }
        break;

    case OpMode_Cam_StillCaptureBurstLimitedInVideo:
        PreviousOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;

        if(ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_VideoPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", CurrentOperatingMode);
        } else {
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstInfiniteInVideo - %d", CurrentOperatingMode);
        }
        break;

    case OpMode_Cam_StillCaptureBurstInfiniteInVideo:
        PreviousOperatingMode = OpMode_Cam_StillCaptureBurstInfiniteInVideo;

        if (ConfigCapturing[OpModeCapturePort]==OMX_FALSE) {
            CurrentOperatingMode = OpMode_Cam_VideoPreview;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_VideoPreview - %d", CurrentOperatingMode);
        } else {
            CurrentOperatingMode = OpMode_Cam_StillCaptureBurstLimitedInVideo;
            DBGT_PTRACE("CurrentOperatingMode is OpMode_Cam_StillCaptureBurstLimitedInVideo - %d", CurrentOperatingMode);
        }
        break;

    case OpMode_Cam_VideoRecord:
    case OpMode_Cam_StillFaceTracking:
    default :
        DBC_ASSERT(0); //operating mode has been corrupted.
    }
}
