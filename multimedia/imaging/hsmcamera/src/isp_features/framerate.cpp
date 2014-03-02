/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "framerate.h"
#include <stdio.h>  // printf
#define DBGT_PREFIX "CFramerate"

#define MINI(_a,_b) ((_a) < (_b) ? (_a) : (_b))
#define MAXI(_a,_b) ((_a) > (_b) ? (_a) : (_b))

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CFramerate);
#endif

CFramerate::CFramerate(OMX_U32 defaultFramerate_x100) {
    for(int i=0; i < CAMERA_NB_OUT_PORTS; i++) {
        maPortFramerate_x100[i]  = 0;
        mabActivePort[i]     = false;
        maMaxFramerate_x100[i] = CAM_RATE_MAX * Q16;
    }
    mDefaultFramerate_x100   = (defaultFramerate_x100/100)*Q16;
    mSensorFramerate_x100    = 0;
    mSensorFramerateLowerBound_x100 =0;	
    mbFixedFramerate         = false;
    computeSensorFramerate(CAM_RATE_MIN * Q16,CAM_RATE_MAX* Q16);
}


void CFramerate::setPortParam(OMX_U32 portNr, OMX_U32 targetFramerate_x100, bool bActivePort,
								OMX_U32 lower_bound_framerate,OMX_U32 upper_bound_framerate) {
    // printf("FRC: port %u, fr %u, enabled %d\n", (unsigned int) portNr, (unsigned int) targetFramerate_x100/100, bActivePort);
    maPortFramerate_x100[portNr] = targetFramerate_x100;
    mabActivePort[portNr]        = bActivePort;
    computeSensorFramerate(lower_bound_framerate ,upper_bound_framerate) ;
}


OMX_S32 CFramerate::getSensorFramerate_x100() {
	
    OMX_S32 ret = MAXI(mSensorFramerate_x100, CAM_RATE_MIN * Q16);
	
    // printf("FRC: current target is %u, clipped to %d\n", (unsigned int) mSensorFramerate_x100, (int) ret);
    return ret;
}

OMX_S32 CFramerate::getSensorFramerateLowerBound_x100() {
    // Clip the framerate to the allowed range
    OMX_S32 ret = MAXI(mSensorFramerateLowerBound_x100, CAM_RATE_MIN * Q16);
    return ret;
}



OMX_S32 CFramerate::getFrameSkipRatio(OMX_U32 portNr) {
    OMX_S32 i32_ratio = 0;
    // If port framerate is fixed, get the buffer skip modulo.
    if (maPortFramerate_x100[portNr] != 0) {
        /* NB: The master port can only be in variable framerate mode if the slave port is in variable framerate too.
         *     Therefore, mSensorFramerate_x100 cannot be null here.
         *
         *     However, although it should not happen, VPB0 or VPB2 might have a higher framerate than VPB1 (it is not checked).
         *     Returning 1 in that case prevents any streaming issue.    */
        i32_ratio = MAX(mSensorFramerate_x100 / maPortFramerate_x100[portNr], 1);
    }
    // If variable framerate on this port, stream all buffers out.
    else {
        i32_ratio = 1;
    }
    // printf("FRC: port %u, ratio %d\n", (unsigned int) portNr, (int) i32_ratio);
    return i32_ratio;
}


bool CFramerate::isFixedFramerate() {
    return mbFixedFramerate;
}


void CFramerate::computeSensorFramerate(OMX_U32 lower_bound_framerate_x100,OMX_U32 upper_bound_framerate_x100) {
    // Discard disabled ports framerate requirements.
    mbFixedFramerate = false;
    for(int portNr = 0; portNr < CAMERA_NB_OUT_PORTS; portNr++) {
        if (mabActivePort[portNr]) {   // If one active port has a non-null framerate, we are in fixed framerate mode.
            mbFixedFramerate |= (maPortFramerate_x100[portNr] > 0);
        } else {
            maPortFramerate_x100[portNr] = 0;
        }
    }
    // If VPB1 is on, it defines the master framerate. VPB0 and VPB2 will have the same.
    if (mabActivePort[1]) {
        if (maPortFramerate_x100[1] == 0) { // VPB1 is enabled in variable framerate mode.
            mbFixedFramerate = false;
	     if(lower_bound_framerate_x100 > maMaxFramerate_x100[1])
	     	{
	     	       /* The frame rate range is outside the permissible -- Let There Be Streaming Error*/
			mSensorFramerate_x100=upper_bound_framerate_x100; 
		 }
	     else
		{     /*Else clip the frame rate range to the permissible one -- if lower bound value falls in b/w the permissible range*/
            		mSensorFramerate_x100 = MIN(upper_bound_framerate_x100,maMaxFramerate_x100[1]);
		}
			
        } else {
            mSensorFramerate_x100 = MIN(maPortFramerate_x100[1], maMaxFramerate_x100[1]);
        }
    }
    // If VPB1 is off, the highest framerate is master, provided that it is a multiple of the other one.
    else {
        if((maPortFramerate_x100[0] == 0) || (maPortFramerate_x100[2] == 0)) {
            mSensorFramerate_x100 = MAX(maPortFramerate_x100[0], maPortFramerate_x100[2]);
        } else if(! (maPortFramerate_x100[0] % maPortFramerate_x100[2])) {
            mSensorFramerate_x100 = maPortFramerate_x100[0];
        } else if(! (maPortFramerate_x100[2] % maPortFramerate_x100[0])) {
            mSensorFramerate_x100 = maPortFramerate_x100[2];
			
        } else {
            mSensorFramerate_x100 = mDefaultFramerate_x100; // if not a multiple default fame rate is set - these all are the cases of fixed framerate
        }
        // If no non-null framerate is configured on the active ports,
        // or if they are not multiple form each other, use the default value.
        if (mSensorFramerate_x100 == 0) {
            mSensorFramerate_x100 = upper_bound_framerate_x100; //setting client provided upperbound.
        }		
	 mSensorFramerateLowerBound_x100 = MAX(CAM_RATE_MIN * Q16,lower_bound_framerate_x100);//setting lower frm rate to client provided, if it is greater than the  CAM_RATE_MIN
    }
    // printf("FRC: computed target is %u\n", (unsigned int) mSensorFramerate_x100);
}


void CFramerate::setMaxFramerate(OMX_U32 portNr, OMX_U32 maxFramerate_x100)
{
    maMaxFramerate_x100[portNr] = maxFramerate_x100;
}
