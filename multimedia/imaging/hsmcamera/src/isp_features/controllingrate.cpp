/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "camera.h"
#include "ImgConfig.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_isp_features_controllingrateTraces.h"
#endif
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#include "framerate_ext.h"
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

SCF_STATE CAM_SM::Rating(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG: return 0;
   case SCF_STATE_EXIT_SIG:	 return 0;
   case SCF_STATE_INIT_SIG: SCF_INIT(&CAM_SM::Rating_SetFrameRate); return 0;
   case Q_DEFERRED_EVENT_SIG:
	   MSG0("SHARED_Rating-Q_DEFERRED_EVENT_SIG\n");
	   OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_Rating-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
	   return 0;
   case SCF_PARENT_SIG:break;
   default:
	   MSG1("SHARED_Rating-default : %d\n",e->sig);
	   OstTraceFiltStatic1(TRACE_DEBUG, "SHARED_Rating-default : %d", (&mENSComponent),e->sig);
	   break;
   }
   return SCF_STATE_PTR(&CAM_SM::Streaming);
}
/*
Changes made to function to Support Framerate in Q16 format.
This change is done to support framerates in fractions.
Converting the Fractions to Q16 removes issue with ratio of VPB2 and VPB0 framerates.
*/

SCF_STATE CAM_SM::Rating_SetFrameRate(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            camport * a_port[CAMERA_NB_OUT_PORTS];
            OMX_PARAM_PORTDEFINITIONTYPE a_mParamPortDefinition[CAMERA_NB_OUT_PORTS];
	     COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;		
	     OMX_U32 lower_bound_framerate = OMXCam->mFrameRateRange.xFrameRateMin;	
	     OMX_U32 upper_bound_framerate = OMXCam->mFrameRateRange.xFrameRateMax;
		    //MSG2("OMXCam->mFrameRateRange.xFrameRateMin %d, OMXCam->mFrameRateRange.xFrameRateMax %d", OMXCam->mFrameRateRange.xFrameRateMin, OMXCam->mFrameRateRange.xFrameRateMax);
            // if we are in time nudge VF and BMS will run together in "FFOV" sensor mode
            // => max framerate is the one of BMS sensor mode
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
            if (OMX_TRUE == Cam->iOpModeMgr.IsTimeNudgeEnabled()) {
                pFramerate->setMaxFramerate(0, currentStillSensorMode.u32_max_frame_rate_x100*Q16/100);
                pFramerate->setMaxFramerate(2, currentStillSensorMode.u32_max_frame_rate_x100*Q16/100);
            }
            // no time nudge : stream at max framerate -- client provided
            else {
                pFramerate->setMaxFramerate(0, upper_bound_framerate);
                pFramerate->setMaxFramerate(2, upper_bound_framerate);
            }

            pFramerate->setMaxFramerate(1, currentStillSensorMode.u32_max_frame_rate_x100*Q16/100);

            for(int i=0; i < CAMERA_NB_OUT_PORTS; i++) {
                a_port[i] = (camport *) mENSComponent.getPort(i);
                a_mParamPortDefinition[i].nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                getOmxIlSpecVersion(&a_mParamPortDefinition[i].nVersion);
                mENSComponent.getPort(a_port[i]->getPortIndex())->getParameter(OMX_IndexParamPortDefinition,&a_mParamPortDefinition[i]);
                if (a_mParamPortDefinition[i].format.video.xFramerate < (1<<16) / 100) {
                    // In case the framerate is toooo low ( < 0.01 Hz), assume it is using the Q16 format.
                    // Compensate for it.
                    a_mParamPortDefinition[i].format.video.xFramerate = a_mParamPortDefinition[i].format.video.xFramerate << 16;
                }
                bool bActivePort = a_port[i]->isEnabled();
                if (2 == i) {   // For VPB2, the port must be enabled AND capturing for its framerate settings be taken into account.
                    bActivePort &= pOpModeMgr->ConfigCapturing[i];
                }
                else if (1 == i) {   // For VPB1, the port must be enabled AND (capturing bit is set OR time nudge is started)
                    if ((OpMode_Cam_StillPreview == Cam->iOpModeMgr.CurrentOperatingMode)
                    || (OpMode_Cam_StillFaceTracking == Cam->iOpModeMgr.CurrentOperatingMode)) {
                        /* tell coverity that GetPrepareCapture return value does not need to be checked */
                        /* coverity[check_return : FALSE] */
                        bActivePort &= (pOpModeMgr->ConfigCapturing[i] | pOpModeMgr->GetPrepareCapture());
                    }
                    else {
                        bActivePort &= pOpModeMgr->ConfigCapturing[i];
                    }
                }


		MSG2("CAM_SM::Rating_SetFrameRate--Trying to apply -> lower_bound_framerate= %d  upper_bound_framerate= %d \n",(int)lower_bound_framerate,(int)upper_bound_framerate);
                // Q16 format in OMX
                MSG1("Is port: %d active: %d",i,bActivePort);
                pFramerate->setPortParam(i, a_mParamPortDefinition[i].format.video.xFramerate , bActivePort,lower_bound_framerate,upper_bound_framerate);
            }

            /* In selftest mode, the streaming is started with no enabled port.
             * A VPB1-compatible framerate must nevertheless be configured.
             * Declaring an enabled VPB1 in variable framerate mode allows this.
             */
            if(OMX_TRUE == pSelfTest->pTesting->bEnabled) {
                pFramerate->setPortParam(1, 0, true,lower_bound_framerate,upper_bound_framerate);
            }
            
            OMX_U32 u32_framerate_lowerbound_x100 = pFramerate->getSensorFramerateLowerBound_x100();
	     OMX_U32 u32_framerate_upperbound_x100 = pFramerate->getSensorFramerate_x100();
		 
	     MSG2("CAM_SM::Rating_SetFrameRate --Applied  Max Frame Rate = %d and Min Frame Rate = %d \n",(int)u32_framerate_upperbound_x100,(int)u32_framerate_lowerbound_x100); 
	     MSG1("CAM_SM::Rating_SetFrameRate -- isFixedFramerate = %d \n", pFramerate->isFixedFramerate());
            p3AComponentManager->setFramerate(u32_framerate_upperbound_x100 * 100/Q16,u32_framerate_lowerbound_x100 * 100/Q16,pFramerate->isFixedFramerate());

            /* Setting the max value before RUN defines the sensor mode.
             * sw3A will be allowed to decrease the framerate while streaming, but not to go above this initial value. */
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            float f_rate = ((CFramerateExt*)pFramerate)->getSensorMaxFrameRate();
            MSG1("Changing sensor framerate to %ffps(extension).\n", f_rate);
            pIspctlCom->queuePE(VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0, f_rate);
            pIspctlCom->queuePE(VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte0, f_rate);
            OMX_3A_SENSORMODE_USAGE_TYPE sensorUsage = ((CFramerateExt*)pFramerate)->getSensorUsageMode();
            MSG1("Changing sensor usage mode to %d (extension).\n", sensorUsage);
            pIspctlCom->queuePE(RunMode_Control_e_LLD_USAGE_MODE_usagemode_Byte0, (t_uint32)sensorUsage);
            ((Sw3AProcessingCompExt*)p3AComponentManager)->bSensorModechanged = false;
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
            float f_rate = ((float) u32_framerate_upperbound_x100) /(float)Q16;
            /* short term workaround (framerate given by sensor mode does not seem to be supported) in time nudge */
#if (IMG_CONFIG_SW_PLATFORM == 1)
            if (OMX_TRUE == Cam->iOpModeMgr.IsTimeNudgeEnabled()) {
                f_rate = 22.0;
            }
#endif
            MSG1("Changing sensor framerate to %ffps.\n", f_rate);
            OstTraceFiltStatic1(TRACE_DEBUG, "Changing sensor framerate to %ffps.", (&mENSComponent), f_rate);
            pIspctlCom->queuePE(VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0, f_rate);
            pIspctlCom->queuePE(VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte0, f_rate);
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
            pIspctlCom->processQueue();
            return 0;
        }
        case SCF_STATE_EXIT_SIG:   return 0;
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SHARED_Rating_SetFrameRate-EVT_ISPCTL_LIST_INFO_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_Rating_SetFrameRate-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
            SCF_PSTATE next;
            SM_POP_STATE(next);
            SCF_TRANSIT_TO_PSTATE(next);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::Rating);

}

