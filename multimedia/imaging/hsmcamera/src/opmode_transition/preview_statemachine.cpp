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
#include "hsmcamera_src_opmode_transition_preview_statemachineTraces.h"
#endif

/* */
/* EnteringPreview state machine */
/**/

SCF_STATE CAM_SM::EnteringPreview(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::EnteringPreview_DoingPreviewConfiguration);
            return 0;
        }
        case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_ERROR("EnteringPreview-EVT_ISPCTL_ERROR_SIG\n");
            OstTraceFiltStatic0(TRACE_ERROR, "EnteringPreview-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
            if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR) {
                DBGT_ERROR("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;\n");
                OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
            }
            if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR) {
                DBGT_ERROR("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
                OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
            }
            DBC_ASSERT(0);
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        {
            MSG0("EnteringPreview-DEFERRED_EVENT;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringPreview-DEFERRED_EVENT;", (&mENSComponent));
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}


SCF_STATE CAM_SM::EnteringPreview_DoingPreviewConfiguration(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        case BUFFER_AVAILABLE_SIG:
        {
            ENTRY;
            /* do not wait that buffers are available on VPB0 to lock HW resource */
            MSG1("Lock %s\n", CError::stringRSRequesterId(CAMERA_VF));
            pResourceSharerManager->mRSLock.Lock(CAMERA_VF);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_LOCK_KO_SIG: return 0; /* nothing to do : wait in this state that HW resource is available */
        case Q_LOCK_OK_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_DoingPreviewConfiguration-Q_LOCK_OK_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_DoingPreviewConfiguration-Q_LOCK_OK_SIG;", (&mENSComponent));
            /* if time nudge is enabled, check that we have enough buffers on port (nFrameBefore + 1 needed) */
            camport * vpb1 = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            if ((OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
                && (vpb1->getParamPortDefinition().nBufferCountActual < (pOpModeMgr->GetFrameBefore()+1))) {
                MSG0("CAM_SM::EnteringPreview_DoingPreviewConfiguration-OMX_ErrorBadParameter\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_DoingPreviewConfiguration-OMX_ErrorBadParameter", (&mENSComponent));
                mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadParameter, 0);
                return 0;
            }

            pOpModeMgr->UpdateCurrentOpMode();
            extradataCircularBuffer_init();
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_ConfiguringZoom);
            ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_All;
            SCF_TRANSIT_TO(&CAM_SM::SHARED_ConfiguringISPPipe);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_ConfiguringZoom(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;

        case Q_PONG_SIG:
        {
            MSG0("EnteringPreview_ConfiguringZoom-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringPreview_ConfiguringZoom-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_DoingPreviewRate);
            pZoom->internalZoomConfig = internalZoomConfig_setCurrentFOVandCenter;
            SCF_TRANSIT_TO(&CAM_SM::ZoomingConfig);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_DoingPreviewRate(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;

        case Q_PONG_SIG:
        {
            MSG0("EnteringPreview_DoingPreviewRate-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringPreview_DoingPreviewRate-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_SendingPreviewIQSettings);
            SCF_TRANSIT_TO(&CAM_SM::Rating);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_SendingPreviewIQSettings(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_SendingPreviewIQSettings-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_SendingPreviewIQSettings-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_Start3A);
            SCF_TRANSIT_TO(&CAM_SM::CAMERA_LoadingIQSettings);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_Start3A(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_Start3A-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_Start3A-Q_PONG_SIG;", (&mENSComponent));
            COmxCamera* OMXCam = (COmxCamera*)&mENSComponent;
            if (OMX_FALSE == OMXCam->mSensorMode.bOneShot)
            {
                if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]) {
                    p3AComponentManager->setMode(SW3A_MODE_SET_VIDEO,0);
                }
                else {
                    p3AComponentManager->setMode(SW3A_MODE_SET_VIDEOVF,0);
                }
            }
            else
            {
                if (OMX_TRUE == pOpModeMgr->GetPrepareCapture()) {
                    p3AComponentManager->setMode(SW3A_MODE_SET_TIME_NUDGE,0);
                }
                else {
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                    if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]) {
                        p3AComponentManager->setMode(SW3A_MODE_SET_STILLVF,0);
                    }
                    else {
                        p3AComponentManager->setMode(SW3A_MODE_SET_VIDEOVF,0);
                    }
#else
                    p3AComponentManager->setMode(SW3A_MODE_SET_STILLVF,0);
#endif
                }
            }
            DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::EnteringPreview_SendBuffers);
#else
            p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::EnteringPreview_StartingVpip);
#endif
            SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_StartingVpip(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_StartingVpip-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_StartingVpip-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_ConfiguringZoom2);
            ControlingISP_ControlType = ControlingISP_GetState_GoRunning;
            SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);	
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_ConfiguringZoom2(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_ConfiguringZoom2-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_ConfiguringZoom2-Q_PONG_SIG;", (&mENSComponent));
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
#else
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_SendBuffers);
#endif
            SCF_TRANSIT_TO(&CAM_SM::ZoomBeforeStateExecuting);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


SCF_STATE CAM_SM::EnteringPreview_SendBuffers(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("\n timer TT_GRAB_CONFIG_REQ start\n");
            iTimed_Task[TT_GRAB_CONFIG_REQ].current_time = 0x00;
            iTimed_Task[TT_GRAB_CONFIG_REQ].timer_running = TRUE;
            camport * portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
            OMX_ERRORTYPE error = pGrabControl->configureStab(pGrabControl->mEsramSuspended,((COmxCamera*)&mENSComponent)->nRotation);
            if (error==OMX_ErrorNone)
            {
                pGrabControl->configureGrabResolutionFormat(portVF,1);
            }
            else
            {
                DBGT_ERROR("EnteringPreview_SendBuffers-Resources are suspended, cannot allocate in ESRAM\n");
                OstTraceFiltStatic0(TRACE_ERROR, "EnteringPreview_SendBuffers-Resources are suspended, cannot allocate in ESRAM", (&mENSComponent));
                pGrabControl->mWaitingForEsram=true;
                SM_PUSH_STATE_STATIC(&CAM_SM::EnteringPreview_SendBuffers);
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }
#else
            OMX_ERRORTYPE error = pGrabControl->configureStab(OMX_FALSE, ((COmxCamera*)&mENSComponent)->nRotation);
            if(OMX_ErrorNone != error)
            {
                DBGT_ERROR("CAM_SM::EnteringPreview_SendBuffers : cannot allocate in ESRAM\n");
                OstTraceFiltStatic0(TRACE_ERROR, "CAM_SM::EnteringPreview_SendBuffers : cannot allocate in ESRAM", (&mENSComponent));
                DBC_ASSERT(0);
                return 0;
            }
            pGrabControl->configureGrabResolutionFormat(portVF,1);
#endif
            return 0;
        }
        case CONFIGURE_SIG:
        {
            MSG0("CAM_SM::EnteringPreview_SendBuffers-CONFIGURE_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::EnteringPreview_SendBuffers-Q_PONG_SIG;", (&mENSComponent));
            if (CAMERA_PORT_OUT0 == e->type.other) {
                MSG0("\n timer TT_GRAB_CONFIG_REQ stop\n");
                iTimed_Task[TT_GRAB_CONFIG_REQ].timer_running = FALSE;
            }

            if ((CAMERA_PORT_OUT0 == e->type.other)
                && (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]))
            {
                camport * portRecord = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT2);
                pGrabControl->configureGrabResolutionFormat(portRecord,1,((COmxCamera*)&mENSComponent)->nRotation);
            }
            else if (((CAMERA_PORT_OUT2 == e->type.other)
                        || (CAMERA_PORT_OUT0 == e->type.other))
                    && (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()))
            {
                camport * portStill = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT1);
                pGrabControl->configureGrabResolutionFormat(portStill,(t_uint8)((COmxCamera*)&mENSComponent)->mVariableFramerateKeepRatio.nSelectRatio);
            }
            else
            {
                pGrabControl->setStartCapture(CAMERA_PORT_OUT0);
                if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]) {
                    pGrabControl->setStartCapture(CAMERA_PORT_OUT2);
                }
				COmxCamera* omxcam = (COmxCamera*)&mENSComponent;
				Camera *cam = (Camera *)&omxcam->getProcessingComponent();
                if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled() &&  !(cam->mDisplay720p&0x1)) {
                    pGrabControl->setStartHiddenCapture(CAMERA_PORT_OUT1,(t_uint16)pOpModeMgr->GetFrameBefore());
                }

                /* send callback */
                if ((CAMERA_IDLE_TO_EXECUTING_SIG == lastUserDefferedEventDequeued.sig)
                    && (OMX_TRUE == lastUserDefferedEventDequeued.args.sendCommand.nSendCallback)) {
                    lastUserDefferedEventDequeued.args.sendCommand.nSendCallback = OMX_FALSE;
                    if (OMX_StateExecuting == lastUserDefferedEventDequeued.args.sendCommand.nParam) {
                        pOmxStateMgr->changeState(OMX_StateExecuting);
                    }
                    else {
                        pOmxStateMgr->changeState(OMX_StatePause);
                    }
                }

                //performance traces start
                Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
                if (0 != Cam->latency_ShotToShotStart) {
                    latencyMeasure(&(mTime));
                    Cam->mlatency_Hsmcamera.e_OMXCAM_ShotToShot.t0 = mTime;
                    OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToShot %d", 0);
                    Cam->latency_ShotToShotStart = 0;
                }
                //performance traces end

				OMX_PARAM_PORTDEFINITIONTYPE paramPortDefinition;
				paramPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
				getOmxIlSpecVersion(&paramPortDefinition.nVersion);
				mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&paramPortDefinition);

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_StartingVpip);
#else
				if(((OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]) && (OMX_TRUE == paramPortDefinition.bEnabled) )||  (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled() &&  (cam->mDisplay720p&0x1)))  
    	            SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_HiddenBMS_DoingRawCaptureConfiguration);
				else
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
#endif
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


/* */
/* ExitingPreview state machine */
/**/
SCF_STATE CAM_SM::ExitingPreview(s_scf_event const *e) {
   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
	   case SCF_STATE_EXIT_SIG: EXIT; return 0;
	   case SCF_STATE_INIT_SIG: INIT;
		   SCF_INIT(&CAM_SM::ExitingPreview_StoppingVpip);
		   return 0;
	   case EVT_ISPCTL_ERROR_SIG:
		{
			DBGT_ERROR("ExitingPreview-EVT_ISPCTL_ERROR_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "ExitingPreview-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
			if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR) {
				DBGT_ERROR("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
			}
			if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR) {
				DBGT_ERROR("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
			}
            DBC_ASSERT(0);
            return 0;
		}
	   case Q_DEFERRED_EVENT_SIG:
		   MSG0("ExitingPreview-DEFERRED_EVENT;\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "ExitingPreview-DEFERRED_EVENT;", (&mENSComponent));
		   return 0;
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}


SCF_STATE CAM_SM::ExitingPreview_StoppingVpip(s_scf_event const *e)
{
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG: EXIT; return 0;
		case Q_PONG_SIG:
		{
			MSG0("ExitingPreview_StoppingVpip-Q_PONG_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExitingPreview_StoppingVpip-Q_PONG_SIG;", (&mENSComponent));
			pOpModeMgr->UpdateCurrentOpMode();           
			pDeferredEventMgr->onlyDequeuePriorEvents(true);

			if(CAMERA_PORT_OUT2 == pOpModeMgr->ConfigCapturePort) //making request for video record
			{
				pOpModeMgr->captureRequest[CAMERA_PORT_OUT2] ++;
			}
			else if(CAMERA_PORT_OUT1 == pOpModeMgr->ConfigCapturePort)
			{
				if (OMX_FALSE == pOpModeMgr->IsTimeNudgeEnabled()) {
					pOpModeMgr->captureRequest[CAMERA_PORT_OUT1] ++;
				}
				MSG1("----------CAPTURE_REQUEST =%d\n", pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);
				OstTraceFiltStatic1(TRACE_DEBUG, "----------CAPTURE_REQUEST =%d", (&mENSComponent), pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);

			}
			else
			{
				MSG0("ExitingPreview_StoppingVpip-error\n");
				DBC_ASSERT(0);
			}

			// we do not need a buffer with EOS flag on VPB0 => stop grab now
			pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT0);
			GrabAbortStatus = GRAB_ABORT_REQUESTED;

			SM_PUSH_STATE_STATIC(&CAM_SM::RendezvousState);
			ControlingISP_ControlType = ControlingISP_SendStop_WaitIspSensorStopStreaming_Stop3A;
			p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::ExitingPreview_StoppingFlash);
			SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);


			return 0;
		}
		default: break;

	}
	
	return SCF_STATE_PTR(&CAM_SM::ExitingPreview);

}


SCF_STATE CAM_SM::ExitingPreview_StoppingFlash(s_scf_event const *e)
{
	switch (e->sig)
	{
	   	case SCF_STATE_ENTRY_SIG:
	   	{
			ENTRY;
			OstTraceFiltStatic0(TRACE_DEBUG, "ExitingPreview_StoppingFlash-Q_PONG_SIG;", (&mENSComponent));

            //Fix for ER 413992: The light will off for a short time when start record and end record under camcorder mode
            t_sw3A_FlashDirective flashDirective = p3AComponentManager->mFlashController.getDirective();
            if((pOpModeMgr->CurrentOperatingMode != OpMode_Cam_VideoRecord) || (flashDirective.flashState != SW3A_FLASH_VIDEOLED))
            {
                // disable flash mode
                cam_flash_err_e err = p3AComponentManager->mFlashController.unConfigure();
                if (err != CAM_FLASH_ERR_NONE)
                {
                    DBGT_ERROR("Error: Flash unconfigure failed.\n");
                    OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash unconfigure failed.", (&mENSComponent));
                    DBC_ASSERT(0);
                }
            }
            else
            {
                MSG0("from VideoVF to VideoRecord, if VIDEOLED is on ... skipping flash unConfigure\n");
            }

            if((OMX_TRUE == pOpModeMgr->AutoPauseAfterCapture) && (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())){
				// here the camera component calls itself through OMX interface
				// It is important to do this internally (through mENSComponent)
				// and NOT through OMX_SendCommand, to avoid going through ENS wrapper
				// thus taking twice the mutex !
				mENSComponent.SendCommand(&mENSComponent, OMX_CommandStateSet, OMX_StatePause, NULL);
			}
			pResourceSharerManager->mRSRelease.Release(CAMERA_VF);
			return 0;
	   	}
		case SCF_STATE_EXIT_SIG: EXIT; return 0;
	   	case Q_RELEASED_SIG:
		{
			MSG0("ExitingPreview_StoppingFlash-Q_PONG_SIG;\n");
            pDeferredEventMgr->onlyDequeuePriorEvents(false);
            if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()) {
                SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
            }
			else if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1])
			{
			//for ER 427419 start
			if(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureSingle && bMoveToVideo == OMX_TRUE)
			{
				pOpModeMgr->ConfigCapturePort = 1;
				pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort] ++;		
			}

				MSG1("\nSending to rawcapture bMoveToVideo = %d\n",bMoveToVideo);
				//for ER 427419 end
				SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture);
			}
			else if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2])
			{
				SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
			}
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ExitingPreview);
}




/*For Shutterlag*/

SCF_STATE CAM_SM::RendezvousState(s_scf_event const *e)
{

	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("CAM_SM::RendezvousState-ENTRY\n");
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT;  return 0;

		case Q_PONG_SIG:
		{
			MSG0("CAM_SM::RendezvousState-Q_PONG_SIG;\n");
			MSG1("CAM_SM::RendezvousState --Sw3aStopReqStatus= %s\n", CError::stringSW3AStatusid(Sw3aStopReqStatus));
			MSG1("CAM_SM::RendezvousState --isp_state= %s\n", CError::stringISPStateid(isp_state));
			MSG1("CAM_SM::RendezvousState --GrabAbortStatus= %s\n", CError::stringGrabStatusid(GrabAbortStatus));

			if(CheckRendezvousCondition())
			{
				MSG0("CAM_SM::RendezvousState--Rendezvous Criteria Met \n");
				if(isp_state == ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE)
				{
					pOmxStateMgr->changeState(OMX_StateIdle);
				}
				pDeferredEventMgr->onlyDequeuePriorEvents(false);
				Sw3aStopReqStatus = SW3A_STOP_NOT_REQUESTED;
				GrabAbortStatus = GRAB_ABORT_NOT_REQUESTED;
				DBC_ASSERT(p3AComponentManager->whatNextAfterRendezvous != NULL); 
				SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterRendezvous);
				p3AComponentManager->whatNextAfterRendezvous = NULL;
			}
			else
			{
				MSG0("CAM_SM::RendezvousState--Rendezvous Criteria Not Met ;\n");
				SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);

			}
			
			return 0;
		}

		default: break;
	}

	return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);	   
}

SCF_STATE CAM_SM::EnteringPreview_HiddenBMS_DoingRawCaptureConfiguration(s_scf_event const *e) {
	switch (e->sig) {   	
		case SCF_STATE_ENTRY_SIG:   	
			pResourceSharerManager->mRSPing.Ping(0);
		   	return 0;
	   	case SCF_STATE_INIT_SIG:  INIT;  return 0;		
		case Q_PONG_SIG:   	
	   		{
			pIspctlCom->queuePE(Zoom_Status_LLA_u16_output_resX_Byte0,0);
			pIspctlCom->queuePE(Zoom_Status_LLA_u16_output_resY_Byte0,0);
			pIspctlCom->queuePE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0,0);
			pIspctlCom->queuePE(SystemConfig_Status_e_PixelOrder_Byte0,0);
			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0, 0);
			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMin_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMin_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMax_Byte0, 0);
			pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMax_Byte0, 0);
			pIspctlCom->queuePE(CurrentFrameDimension_f_PreScaleFactor_Byte0, 0);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrStart_Byte0, 0);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrStart_Byte0, 0);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0, 0);
			pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0, 0);
			pIspctlCom->queuePE(SystemSetup_u8_NumOfFramesTobeSkipped_Byte0 , 0);
			pIspctlCom->readQueue();
	   		return 0;
		   }
	   case EVT_ISPCTL_INFO_SIG:
	   case EVT_ISPCTL_LIST_INFO_SIG:   	
		{
		   	t_uint16 xsize,ysize,lineLengh,pixelOrder;
		   	if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
			xsize = 2608;//(t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
			ysize = (t_uint16)e->type.ispctlInfo.Listvalue[1].pe_data;
			lineLengh = (t_uint16)e->type.ispctlInfo.Listvalue[2].pe_data;
		    pixelOrder = (t_uint16)e->type.ispctlInfo.Listvalue[3].pe_data;
			pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = e->type.ispctlInfo.Listvalue[4].pe_data;
			pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = e->type.ispctlInfo.Listvalue[5].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize = e->type.ispctlInfo.Listvalue[6].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize = e->type.ispctlInfo.Listvalue[7].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin = e->type.ispctlInfo.Listvalue[8].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin = e->type.ispctlInfo.Listvalue[9].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax = e->type.ispctlInfo.Listvalue[10].pe_data;
			pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax = e->type.ispctlInfo.Listvalue[11].pe_data;
			pCapture_context->nBMS_capture_context.CurrentFrameDimension_f_PreScaleFactor = e->type.ispctlInfo.Listvalue[12].pe_data;
			pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrStart = e->type.ispctlInfo.Listvalue[13].pe_data;
			pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrStart = e->type.ispctlInfo.Listvalue[14].pe_data;
			pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrEnd = e->type.ispctlInfo.Listvalue[15].pe_data;
			pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrEnd = e->type.ispctlInfo.Listvalue[16].pe_data;
			nFramesTobeSkipped=e->type.ispctlInfo.Listvalue[17].pe_data;			
		   	MSG0("EnteringRawCapture_HiddenBMS_DoingRawCaptureConfiguration");
			MSG0("EnteringRawCapture_HiddenBMS_DoingRawCaptureConfiguration-ENTRY \n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_HiddenBMS_DoingRawCaptureConfiguration-ENTRY;", (&mENSComponent));
			OMX_PARAM_PORTDEFINITIONTYPE port_params;
			Camera* cam= (Camera*)&mENSComponent.getProcessingComponent();			
			OMX_ERRORTYPE error ;
			//Now setting the still configuration. 
			 camport * port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
            /* tell coverity that isSupportedFmt return value does not need to be checked */
            /* coverity[check_return : FALSE] */
            DBC_ASSERT(port->isSupportedFmt(CAMERA_PORT_OUT1,mParamPortDefinition.format.video.eColorFormat));
			//Buffer Allocation.
			int bufferCount = mParamPortDefinition.nBufferCountActual + 1;//1;//mParamPortDefinition.nBufferCountActual;
			LOGD("AMVE: xsize %d",xsize );
			PERF_TRACE_LAT("Allocating Buffers for hidden bms",tempo1);

			if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled() &&  !(cam->mDisplay720p&0x1)) 
			{
				
			}
			COmxCamera* pOMXCam = (COmxCamera*)&mENSComponent;				
		 	if(((t_uint16)pOMXCam->mHiddenBuffersInfo.sensor_X)!=xsize||((t_uint16)pOMXCam->mHiddenBuffersInfo.sensor_Y)!=ysize||pOMXCam->mHiddenBuffersInfo.totalBuffers!=((t_uint16)bufferCount))		
			{
				pOMXCam->mHiddenBuffersInfo.totalBuffers = bufferCount;
				if(pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers!=NULL)
				{
					COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;
					error = MMHwBuffer::Destroy(pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers);
				    if (error != OMX_ErrorNone) 
					{
						MSG1("Problem freeing BMS/BML SDRAM buffer (err=%d)\n", error);
						OstTraceFiltStatic1(TRACE_ERROR, "Problem freeing BMS/BML SDRAM buffer (err=%d)", (&mENSComponent), error);
						DBC_ASSERT(0);
				  	}
					pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers=NULL;
				}
				if(pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress!=NULL)
				{
					free(pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress);
					pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress=NULL;
				}
				
				/*Allocate hidden buffers here and send it to grab*/
				MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
				poolAttrs.iBuffers = pOMXCam->mHiddenBuffersInfo.totalBuffers;
				
				pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress = (t_uint32 *)malloc(sizeof(t_uint32) * pOMXCam->mHiddenBuffersInfo.totalBuffers );
				pOMXCam->mHiddenBuffersInfo.sensor_X=xsize;
				pOMXCam->mHiddenBuffersInfo.sensor_Y=ysize;

				poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;		// Memory ty		
				poolAttrs.iDomainID = 0;								// MPC Domain ID (only requested for MPC memory type)
				poolAttrs.iSize = ((((191+xsize * 8)/192) *192)/8)*ysize; 	  // Size (in byte) of a buffer
				poolAttrs.iAlignment = 256; 							// Alignment applied to the base address of each buffer in the pool
		
				//	 1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
				poolAttrs.iCacheAttr = MMHwBuffer::ENormalUnCached;
				pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers = NULL;
				error = MMHwBuffer::Create(poolAttrs, NULL, pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers);
				if (error != OMX_ErrorNone) {
				MSG1("Unable to allocate SDRAM buffer for BMS/BML (OMX err %d)\n", error);
				OstTraceFiltStatic1(TRACE_ERROR, "Unable to allocate SDRAM	buffer for BMS/BML (OMX err %d)", (&mENSComponent), error);
				DBC_ASSERT(0);
				}
				MMHwBuffer::TBufferInfo bufferInfo;
				for(t_uint32 i=0;i<pOMXCam->mHiddenBuffersInfo.totalBuffers; i++)
				{			
					error = pOMXCam->mHiddenBuffersInfo.hiddenBMSBuffers->BufferInfo(i, bufferInfo);
					if (error != OMX_ErrorNone) {
						DBC_ASSERT(0);
					}
					pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress[i]=bufferInfo.iPhyAddr;				
				}
		   	}
			pGrabControl->setHiddenBMSBuffers(pOMXCam->mHiddenBuffersInfo.physicalBuffersAddress,pOMXCam->mHiddenBuffersInfo.totalBuffers,xsize,ysize,lineLengh,pixelOrder);			
			SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_HiddenBMS_Start);
			return 0;
	   	}
	 case SCF_STATE_EXIT_SIG:
 		return 0;
	 default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

SCF_STATE CAM_SM::EnteringPreview_HiddenBMS_Start(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("CAM_SM::RendezvousState-ENTRY\n");
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case SCF_STATE_INIT_SIG:  INIT;  return 0;
		case SCF_STATE_EXIT_SIG:  EXIT;  return 0;

		case Q_PONG_SIG:
		{			
			pGrabControl->setStartHiddenBMS(1,0);
			
			SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);	
			return 0;
		}

		default: break;
	}

	return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);	   
}


OMX_BOOL CAM_SM::CheckRendezvousCondition()
{

	OMX_BOOL rendezvous = OMX_FALSE;

	if (Sw3aStopReqStatus == SW3A_STOP_REQ_SERVICED)			
	{
		if(((GrabAbortStatus == GRAB_ABORT_SERVICED) ||(GrabAbortStatus == GRAB_ABORT_NOT_REQUESTED) )
		&& ((isp_state == ISP_STATE_STOP_STREAMING_STOP3A) ||(isp_state == ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE)
		 ||(isp_state == ISP_STATE_SLEPT)||(isp_state == ISP_STATE_STREAMING)))
		{
			rendezvous = OMX_TRUE;
		}

	}
	else if (Sw3aStopReqStatus == SW3A_STOP_NOT_REQUESTED)
	{
		if(((GrabAbortStatus == GRAB_ABORT_SERVICED) ||(GrabAbortStatus == GRAB_ABORT_NOT_REQUESTED))
		&& ((isp_state == ISP_STATE_STOP_STREAMING_STOP3A) || (isp_state == ISP_STATE_STOP_STREAMING_STOP3A_SENDIDLE)
		||(isp_state == ISP_STATE_SLEPT)))
		{
			rendezvous = OMX_TRUE;
		}
		else if((isp_state == ISP_STATE_STREAMING)&& (GrabAbortStatus == GRAB_ABORT_SERVICED))
		{
			rendezvous = OMX_TRUE;
		}
	}
	return (rendezvous);

}




