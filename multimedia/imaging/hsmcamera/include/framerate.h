/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _FRAMERATE_H_
#define _FRAMERATE_H_

#include "OMX_Types.h"
#include "camport.h"

#define CAM_RATE_MAX 30
#define CAM_RATE_MIN 1
#define Q16 65536

class CFramerate
{
    public:
        CFramerate(OMX_U32 defaultFramerate_x100);
        void setPortParam(OMX_U32 portNr, OMX_U32 targetFramerate_x100, bool bActivePort,
						OMX_U32 lower_bound_framerate,OMX_U32 upper_bound_framerate);
        OMX_S32 getSensorFramerate_x100();
	 OMX_S32 getSensorFramerateLowerBound_x100();
        OMX_S32 getFrameSkipRatio(OMX_U32 portNr);
        bool isFixedFramerate();
        void setMaxFramerate(OMX_U32 portNr, OMX_U32 maxFramerate_x100);
    private:
        void computeSensorFramerate(OMX_U32 lower_bound_framerate,OMX_U32 upper_bound_framerate);
        OMX_U32 mDefaultFramerate_x100;
        // Inputs
        OMX_U32 maPortFramerate_x100[CAMERA_NB_OUT_PORTS];
        bool mabActivePort[CAMERA_NB_OUT_PORTS];
        OMX_U32 maMaxFramerate_x100[CAMERA_NB_OUT_PORTS];
        // Outputs
        OMX_U32 mSensorFramerate_x100;
	 OMX_U32 mSensorFramerateLowerBound_x100;	
        bool mbFixedFramerate;
};

#endif /* _FRAMERATE_H_ */
