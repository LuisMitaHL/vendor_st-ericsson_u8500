/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsmcam.h"
#include "ImgConfig.h"
#include "camera.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_omxmode_statemachineTraces.h"
#endif


SCF_STATE CAM_SM::ExecutingToIdle(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG:
        {
            MSG0("ExecutingToIdle-SCF_STATE_INIT\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExecutingToIdle-SCF_STATE_INIT", (&mENSComponent));
            cam_flash_err_e cam_err = p3AComponentManager->mFlashController.unConfigure();
            if (cam_err != CAM_FLASH_ERR_NONE) {
                DBGT_ERROR("Error: Could not un-configure the flash driver.\n");
                OstTraceFiltStatic0(TRACE_ERROR, "Error: Could not un-configure the flash driver.", (&mENSComponent));
                DBC_ASSERT(0);
            }

            if (bLowPower){
                ControlingISP_ControlType = ControlingISP_SendWakeUp_WaitIspWorkenUp_SendIdle;
                bLowPower = FALSE;
				SM_PUSH_STATE_STATIC(&CAM_SM::ExecutingToIdle_3);	
            }
            else {
                ControlingISP_ControlType = ControlingISP_SendStop_WaitIspSensorStopStreaming_SendIdle_Stop3A;
		  		SM_PUSH_STATE_STATIC(&CAM_SM::RendezvousState);
		  		DBC_ASSERT(p3AComponentManager->whatNextAfterRendezvous== NULL); 
	        	p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::ExecutingToIdle_3);	
            }

		SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
	     
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        {
            MSG0("ExecutingToIdle-Q_DEFERRED_EVENT\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExecutingToIdle-Q_DEFERRED_EVENT", (&mENSComponent));
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}



SCF_STATE CAM_SM::ExecutingToIdle_3(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG:
        {
            pResourceSharerManager->mRSRelease.Release(CAMERA_VF);
            return 0;
        }
        case Q_RELEASED_SIG:
        {
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG: return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExecutingToIdle);
}


SCF_STATE CAM_SM::TimeNudge(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBC_ASSERT(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG: INIT; return 0;
        case Q_DEFERRED_EVENT_SIG: return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}


SCF_STATE CAM_SM::TimeNudge_StartCapture(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("CAM_SM::TimeNudge_StartCapture-SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::TimeNudge_StartCapture-Q_PONG_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::TimeNudge_StartCapture-SCF_STATE_ENTRY_SIG", (&mENSComponent));
            /* check that we have enough buffers on port (nFrameBefore + nFrameLimit needed) */
            /* NB : nFrameBefore + 1 should be enough but it is not supported right now */
            camport * vpb1 = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            t_sw3A_FlashDirective flashDirective = p3AComponentManager->mFlashController.getDirective();
            if(flashDirective.flashState == SW3A_FLASH_MAINFLASH)
            {
                //clear Extradata circular buffers
                extradataCircularBuffer_init();
            }
            if ((OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
                && (vpb1->getParamPortDefinition().nBufferCountActual < (pOpModeMgr->GetFrameBefore()+pOpModeMgr->GetBurstFrameLimit()))) {
                MSG0("CAM_SM::TimeNudge_StartCapture-OMX_ErrorBadParameter\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::TimeNudge_StartCapture-OMX_ErrorBadParameter", (&mENSComponent));
                mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadParameter, 0);
                return 0;
            }

            pOpModeMgr->captureRequest[CAMERA_PORT_OUT1] ++;
            
            OMX_ERRORTYPE err = OMX_ErrorNone;
            err = pOpModeMgr->Compute_NumberOfBuffersForStill();
            if (OMX_ErrorNone != err)
            {
               DBC_ASSERT(0);
            }
            
            if (1 == pOpModeMgr->NumberOfBuffersForStill)
            {
                pGrabControl->setStartOneShotCapture(CAMERA_PORT_OUT1);
            }
            else
            {
                pGrabControl->startBurstCapture(CAMERA_PORT_OUT1, (t_uint16)pOpModeMgr->NumberOfBuffersForStill);
            }
            
            SCF_PSTATE next;
            SM_POP_STATE(next);
            SCF_TRANSIT_TO_PSTATE(next);

            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}
SCF_STATE CAM_SM::TimeNudge_stop3A(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("CAM_SM::TimeNudge_stop3A- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            //inform Sw3A about still mode and wait for flash directive
            MSG0("CAM_SM::TimeNudge_stop3A - Q_PONG_SIG\n");
            p3AComponentManager->setMode(SW3A_MODE_SET_STILL,pOpModeMgr->NumberOfBuffersForStill);
            DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL);
            p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
            SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}


SCF_STATE CAM_SM::TimeNudge_FlashEnabledStartSw3A(s_scf_event const * e)
{
    switch (e->sig)
    {
       case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("CAM_SM::TimeNudge_FlashEnabledStartSw3A- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::TimeNudge_FlashEnabledStartSw3A - Q_PONG_SIG\n");
            extradataCircularBuffer_init();
            pIspctlCom->writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, GrabMode_e_FORCE_OK);
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("CAM_SM::TimeNudge_FlashEnabledStartSw3A - EVT_ISPCTL_INFO_SIG\n");
            p3AComponentManager->setMode(SW3A_MODE_SET_TIME_NUDGE, 0);
            DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL);
            p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::TimeNudge_StartHiddenCapture);
	            SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
	            return 0;
	        }
	        case SCF_STATE_EXIT_SIG: EXIT; return 0;
	        default: break;
	    }
	
	    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
	}
	
	SCF_STATE CAM_SM::TimeNudge_StartHiddenCapture(s_scf_event const * e)
	{
	    switch (e->sig)
	    {
	        case SCF_STATE_ENTRY_SIG:
	        {
	            ENTRY;
	            MSG0("CAM_SM::TimeNudge_StartHiddenCapture - SCF_STATE_ENTRY_SIG\n");
	            pResourceSharerManager->mRSPing.Ping(0);
	            return 0;
	        }
	        case Q_PONG_SIG:
	        {
	            MSG0("CAM_SM::TimeNudge_StartHiddenCapture - Q_PONG_SIG\n");
	            pGrabControl->setStartHiddenCapture(CAMERA_PORT_OUT1, (t_uint16)pOpModeMgr->GetFrameBefore());
	            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
	            return 0;
	        }
	        case SCF_STATE_EXIT_SIG: EXIT; return 0;
	        default: break;
	    }
	
	    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
	}

SCF_STATE CAM_SM::ZSLtoHDR(s_scf_event const *e)
{
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::ZSLtoHDR- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("HDR: Setting force nok\n");
            pIspctlCom->writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, GrabMode_e_FORCE_NOK);
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("HDR: Aborting hidden captures on port 1, eStateZSLHDR = ZSLHDRState_StopHiddenCapture\n");
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_StopHiddenCapture;
            pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT1);
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

SCF_STATE CAM_SM::ZSLHDRConfigureCaptureMode(s_scf_event const *e)
{
    COmxCamera *pOMXCam = (COmxCamera*)&mENSComponent;

    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::ZSLHDRConfigureCaptureMode- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("HDR: CAM_SM::ZSLHDRConfigureCaptureMode - Q_PONG_SIG\n");
            //Save Backup for ZSL Resume
            pOMXCam->nFrameBeforeBackUp = pOMXCam->mCameraUserSettings.nFramesBefore;
            pOMXCam->bFrameLimitedBackUp = pOMXCam->mCameraUserSettings.bFrameLimited;
            pOMXCam->nFrameLimitBackUp = pOMXCam->mCameraUserSettings.nFrameLimit;
            MSG3("HDR: Backup Values: nFrameBefore = %ld, bFrameLimited = %d, nFrameLimit = %ld\n",
                       pOMXCam->nFrameBeforeBackUp, pOMXCam->bFrameLimitedBackUp, pOMXCam->nFrameLimitBackUp);

            //Setting for HDR
            pOMXCam->mCameraUserSettings.bFrameLimited = OMX_TRUE;
            pOMXCam->mCameraUserSettings.nFrameLimit = ZSLHDR_MAX_FRAMELIMIT;
            pOMXCam->mCameraUserSettings.nFramesBefore = 0;
            pOpModeMgr->SetBurstLimited(pOMXCam->mCameraUserSettings.bFrameLimited == OMX_TRUE);
            pOpModeMgr->SetBurstFrameLimit(pOMXCam->mCameraUserSettings.nFrameLimit);
            pOpModeMgr->SetFrameBefore(pOMXCam->mCameraUserSettings.nFramesBefore);
            pOpModeMgr->SetPrepareCapture(OMX_FALSE);
            pOpModeMgr->UpdateCurrentOpMode();

            MSG0("HDR: eStateZSLHDR = ZSLHDRState_ReadyForCapture\n");
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_ReadyForCapture;
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}


SCF_STATE CAM_SM::ZSLHDRStopPreview(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::ZSLHDRStopPreview- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("HDR: CAM_SM::ZSLHDRStopPreview - Q_PONG_SIG\n");
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_StopPreview;
            pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT0);
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

SCF_STATE CAM_SM::ZSLHDRCapture(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::ZSLHDRCapture- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_TakeCapture;
            MSG0("HDR: CAM_SM::ZSLHDRCapture - Q_PONG_SIG\n");
            pOpModeMgr->ConfigCapturing[RawCapturePort] = OMX_TRUE;
            pOpModeMgr->UpdateCurrentOpMode();
            OMX_ERRORTYPE err = OMX_ErrorNone;
            err = pOpModeMgr->Compute_NumberOfBuffersForStill();
            if (OMX_ErrorNone != err)
            {
                DBC_ASSERT(0);
            }
            MSG1("HDR: pOpModeMgr->NumberOfBuffersForStill = %d\n", pOpModeMgr->NumberOfBuffersForStill);
            pGrabControl->startBurstCapture(CAMERA_PORT_OUT1, pOpModeMgr->NumberOfBuffersForStill);
            p3AComponentManager->setMode(SW3A_MODE_SET_HDR_CAPTURE, 0);
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

SCF_STATE  CAM_SM::ResumeAfterZSLHDR(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::ResumeAfterZSLHDR- SCF_STATE_ENTRY_SIG\n");
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("HDR: CAM_SM::ResumeAfterZSLHDR - Q_PONG_SIG\n");
            pOpModeMgr->SetBracketingCount(0);
            pOpModeMgr->ConfigCapturing[RawCapturePort] = OMX_FALSE;
            pOpModeMgr->CurrentOperatingMode = OpMode_Cam_StillPreview;
            pOpModeMgr->UpdateCurrentOpMode();
            p3AComponentManager->setMode(SW3A_MODE_SET_TIME_NUDGE, 0);
            MSG0("HDR: eStateZSLHDR = ZSLHDRState_ReadyForCapture\n");
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_ReadyForCapture;
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

SCF_STATE  CAM_SM::HDRtoZSL(s_scf_event const *e)
{
    COmxCamera *pOMXCam = (COmxCamera*)&mENSComponent;

    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("HDR: CAM_SM::HDRtoZSL- SCF_STATE_ENTRY_SIG\n");
            //Restore backup for ZSL
            pOMXCam->mCameraUserSettings.nFramesBefore = pOMXCam->nFrameBeforeBackUp;
            pOMXCam->mCameraUserSettings.bFrameLimited = pOMXCam->bFrameLimitedBackUp;
            pOMXCam->mCameraUserSettings.nFrameLimit = pOMXCam->nFrameLimitBackUp;
            MSG3("HDR: Backup Restore Values: nFrameBefore = %ld, bFrameLimited = %d, nFrameLimit = %ld\n",
                       pOMXCam->nFrameBeforeBackUp, pOMXCam->bFrameLimitedBackUp, pOMXCam->nFrameLimitBackUp);

            pOpModeMgr->SetBurstLimited(pOMXCam->mCameraUserSettings.bFrameLimited == OMX_TRUE);
            pOpModeMgr->SetBurstFrameLimit(pOMXCam->mCameraUserSettings.nFrameLimit);
            pOpModeMgr->SetFrameBefore(pOMXCam->mCameraUserSettings.nFramesBefore);
            pOpModeMgr->SetPrepareCapture(OMX_TRUE);
            pOpModeMgr->UpdateCurrentOpMode();

            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("HDR: Setting force ok\n");
            pIspctlCom->writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, GrabMode_e_FORCE_OK);
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("HDR: CAM_SM::HDRtoZSL - EVT_ISPCTL_INFO_SIG\n");
            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_Invalid;
            pOpModeMgr->captureRequest[CAMERA_PORT_OUT1] = 0;
            pGrabControl->setStartCapture(CAMERA_PORT_OUT0);
            pGrabControl->setStartHiddenCapture(CAMERA_PORT_OUT1, (t_uint16)pOpModeMgr->GetFrameBefore());
            MSG0("HDR: setting grab mode to normal\n");
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        default: break;
    }

    return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}


