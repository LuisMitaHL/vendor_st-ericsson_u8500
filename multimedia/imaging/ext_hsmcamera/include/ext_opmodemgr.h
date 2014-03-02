/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _EXT_OPMODEMGR_H_
#define _EXT_OPMODEMGR_H_

#include "OMX_Index.h"
#include "ENS_Component.h"
#include "ext_grabctl/api/configure.hpp"
#include "ext_camport.h"
#include "IFM_Index.h"
#include "IFM_Types.h"

class CopModeMgr
{
public :

    CopModeMgr();
    void Init(t_uint16 CapturePort)
    {
        OpModeCapturePort = CapturePort;
    };
    void UpdateCurrentOpMode(void);
    void SetBurstFrameLimit(t_uint32 frameLimit)
    {
        u32_frameLimit = frameLimit;
    };
	void SetBurstLimited(bool bLimited)
    {
        bBurstFrameLimited = bLimited;
    };
	t_uint32 GetBurstFrameLimit()
    {
        return u32_frameLimit;
    };
	bool IsBurstLimited()
    {
        return bBurstFrameLimited;
    };

    t_operating_mode_camera CurrentOperatingMode;

    /* number of frames grabbed in burst on user point of view */
    /* linked to FrameLimit */
	t_uint16 UserBurstNbFrames;

    /* count how many StillCapture are ongoing */
    t_uint16 captureRequest[CAMERA_NB_OUT_PORTS];

    /* says if we have to wait for an EOS */
    OMX_BOOL waitingEOS[CAMERA_NB_OUT_PORTS];

    /* configure the CaptureOrRecord : ConfigCapturing/ConfigCapturePort
     * allows to know in which port the IndexConfigCapturing has been triggered thus
     * allows to know if we are recording or Capturing */
    OMX_BOOL ConfigCapturing[CAMERA_NB_OUT_PORTS];
    t_uint16 ConfigCapturePort;
    t_operating_mode_camera PreviousOperatingMode;
    t_uint16 nBufferAtMPC[CAMERA_NB_PORTS];

    /* Coming from OM API needed in core */
    OMX_BOOL AutoPauseAfterCapture;

protected:
    /* coming from Camera Constructor*/
    t_uint16 OpModeCapturePort;
private:
    t_uint32 u32_frameLimit;
    bool bBurstFrameLimited;
};

#endif /* _EXT_OPMODEMGR_H_ */
