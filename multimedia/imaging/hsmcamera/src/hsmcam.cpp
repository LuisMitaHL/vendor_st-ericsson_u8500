/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** @file camera_sm.cpp
    @brief This file implement the state machine of the OMX camera.
*/
#define DBGT_PREFIX "HSMCAM"
#include "hsm.h" //rename_me as common_sm.h
#include "hsmcam.h" //rename_me as camera_sm.h
#include "camera.h"
#include <cm/inc/cm.hpp>
#include <stdio.h>
#include <los/api/los_api.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/uio.h>
#include<sys/stat.h>
#include <sys/time.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_hsmcamTraces.h"
#endif

/* */
/* CAM_SM::ProcessPendingEvents is responsible for processing the incoming events
 * This is the state where the all the events are dispatched to the appropriate state machine
 * At the end of the dedicated state machine, Go back to this state machine
 */
/**/
SCF_STATE CAM_SM::ProcessPendingEvents(s_scf_event const *e)
{
    switch (e->sig) {

        case SCF_STATE_ENTRY_SIG:
        {
            MSG0("CAM_SM::ProcessPendingEvents-ENTRY\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-ENTRY", (&mENSComponent));
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        {
            MSG0("CAM_SM::ProcessPendingEvents-Q_DEFERRED_EVENT_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
            return 0;
        }

        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::ProcessPendingEvents-Q_PONG_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-Q_PONG_SIG", (&mENSComponent));
            s_scf_event event;
            OMX_BOOL prior = OMX_TRUE;
            MSG0("NOW Dequeue prior event and process it\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "NOW Dequeue prior event and process it", (&mENSComponent));
            pDeferredEventMgr->dequeuePriorEvent(&event);
            if(event.sig == SHARED_RETURN_SIG) {
                prior = OMX_FALSE;
                MSG0("No prior events thus Dequeue pending event and process it\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "No prior events thus Dequeue pending event and process it", (&mENSComponent));
                pDeferredEventMgr->dequeuePendingEvent(&event);
            }

            if ((CAMERA_EXIT_PREVIEW_SIG == event.sig)
                || (CAMERA_INDEX_CONFIG_EXTCAPTUREMODE_SIG == event.sig)
                || (CAMERA_INDEX_CONFIG_ZOOM_SIG == event.sig)
                || (SHARED_READLISTPE_DEBUG_SIG == event.sig)
                || (SHARED_WRITELISTPE_SIG == event.sig)
                || (CAMERA_INDEX_CONFIG_TEST_MODE_SIG == event.sig)
                || (CAMERA_INDEX_CONFIG_MIRROR_SIG == event.sig)
                || (CAMERA_INDEX_CONFIG_SELFTEST_SIG == event.sig) /* there should be one event for user, an other one for internal use */
                || (CAMERA_EXECUTING_TO_IDLE_END_SIG == event.sig)
                || (CAMERA_IDLE_TO_EXECUTING_SIG == event.sig)
                || (CAMERA_EXECUTING_TO_PAUSE_SIG == event.sig)
                || (CAMERA_PAUSE_TO_EXECUTING_SIG == event.sig)
                || (SEND_COMMAND_SIG == event.sig)
                || (ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG == event.sig))
            {
                lastUserDefferedEventDequeued = event;
            }

            MSG2("Event to be processed  signal =%d (%s)\n", event.sig,CError::stringSMSig(event.sig));
            OstTraceFiltStatic1(TRACE_DEBUG, "Event to be processed signal =%d (%s)", (&mENSComponent), event.sig);
            switch (event.sig)
            {
          
			case START_HIDDEN_BMS_SIG:
				{
				MSG0("CAM_SM::ProcessPendingEvents-START_HIDDEN_BMS_SIG\n");								
				PERF_TRACE_LAT("Capture Request",tempo1);				
				GrabStatusNonDefferred=1;
				((Camera*)&mENSComponent.getProcessingComponent())->captureRequest=1; 			
				SCF_TRANSIT_TO(&CAM_SM::StopVideoRecord);
				p3AComponentManager->setMode(SW3A_MODE_SET_STILL_IN_VIDEO,0);
		
				//TODO start HiddenBMS						
				return 0;
				}
			case ISPPROC_EXIT_CAPTURE_SIG:
				{
				MSG0("CAM_SM::ProcessPendingEvents-ISPPROC_EXIT_CAPTURE_SIG\n");
				PERF_TRACE_LAT("Still Capture complete",tempo1);
				COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;
			 	pOmxCamera->mCapturingStruct_VPB1.bEnabled = OMX_FALSE;
				OMX_ERRORTYPE error;
				OMX_BUFFERHEADERTYPE  *pOmxBuf =NULL;
				error = pCapture_context->popOmxBufHdr(&pOmxBuf);
				if (OMX_ErrorNone != error)
					{
					DBC_ASSERT(0);
					}
				error = setVPB1Extradata(pOmxBuf, true);
				if (OMX_ErrorNone != error) 
					{
					DBC_ASSERT(0);
					}
				MSG0("CAM_SM::ProcessPendingEvents-AddedExtraData\n");
				pGrabControl->setStartHiddenBMS(0,0);
				if(pOmxBuf->nFlags & OMX_BUFFERFLAG_EOS) 
					{
					MSG0("OMX_EventBufferFlag OMX_BUFFERFLAG_EOS\n");
					mENSComponent.fillBufferDone(pOmxBuf);
					mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_ENDOFFRAME, 0);
					mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_EOS, 0);
					}
				SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_Config_Post_StillinVideo);
				//TODO Captures are over start video record allover again
				return 0;
				}

                case START_HDR_SIG:
                {
                    if ((((COmxCamera*)&mENSComponent)->mSceneMode.eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR) &&
                         OMX_TRUE==pOpModeMgr->IsTimeNudgeEnabled())
                    {
                        MSG0("HDR: START_HDR_SIG, eStateZSLHDR = ZSLHDRState_StartHDR\n");
                        ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_StartHDR;
                        SCF_TRANSIT_TO(&CAM_SM::ZSLtoHDR);
                    }
                }
                return 0;

                case RESUME_ZSL_SIG:
                {
                    MSG0("HDR: RESUME_ZSL_SIG: Resuming Grab, eStateZSLHDR = ZSLHDRState_StopHDR\n");
                    ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_StopHDR;
                    SCF_TRANSIT_TO(&CAM_SM::HDRtoZSL);
                }
                return 0;

		case TIMEOUT_SIG:
                    MSG0("CAM_SM::ProcessPendingEvents-TIMEOUT_SIG\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-TIMEOUT_SIG", (&mENSComponent));
                    if (timeoutAction() == FALSE) pResourceSharerManager->mRSPing.Ping(0);
                    return 0;

		case LOWPOWER_SIG:
		{
		     MSG0("CAM_SM::ProcessPendingEvents-LOWPOWER_SIG\n");
		     OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-LOWPOWER_SIG", (&mENSComponent));
		     if (bLowPower == FALSE)  {
		         OMX_STATETYPE state = OMX_StateInvalid;
        	         mENSComponent.GetState(&mENSComponent,&state);
			 MSG1("state %d\n",state);
			 OstTraceFiltStatic1(TRACE_DEBUG, "state %d", (&mENSComponent),state);
		         if (state == OMX_StateIdle) {
			    ControlingISP_ControlType = ControlingISP_GetState_GoSlept;
			    bLowPower = TRUE;
		         }
		         else if (state ==OMX_StatePause){
			     ControlingISP_ControlType = ControlingISP_GetState_GoSlept_Stop3A;
			     bLowPower = TRUE;
		         }
		         else if (state ==OMX_StateExecuting){
			     OMX_PARAM_PORTDEFINITIONTYPE mParamPort0Definition;
			     mParamPort0Definition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
			     getOmxIlSpecVersion(&mParamPort0Definition.nVersion);
			     OMX_PARAM_PORTDEFINITIONTYPE mParamPort2Definition;
			     mParamPort2Definition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
			     getOmxIlSpecVersion(&mParamPort2Definition.nVersion);
                             mENSComponent.getPort(CAMERA_PORT_OUT0)->getParameter(OMX_IndexParamPortDefinition,&mParamPort0Definition);
                             mENSComponent.getPort(CAMERA_PORT_OUT2)->getParameter(OMX_IndexParamPortDefinition,&mParamPort2Definition);
			     if ( (mParamPort0Definition.bEnabled == OMX_FALSE)
                  &&(mParamPort2Definition.bEnabled == OMX_FALSE)
                  &&(((COmxCamera*)&mENSComponent)->mSensorMode.bOneShot == OMX_FALSE)
                  &&(pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2] == OMX_FALSE))
			{
				MSG0("All ports are disabled, let's go to sleep\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "All ports are disabled, let's go to sleep", (&mENSComponent));
				ControlingISP_ControlType = ControlingISP_GetState_GoSlept_Stop3A;
				bLowPower = TRUE;
			}
		         }
		     }

		     if (bLowPower == TRUE){
				SM_PUSH_STATE_STATIC(&CAM_SM::RendezvousState);
				DBC_ASSERT(p3AComponentManager->whatNextAfterRendezvous== NULL); 
				p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents); 
				SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
		     } else pResourceSharerManager->mRSPing.Ping(0);
		     return 0;
		}

		case CAMERA_UNSUSPEND_RESOURCES_SIG:
		{
			/*
			this signal is issued by Camera::unsuspendResources if ESRAM
			resource is unsuspended while port state machine was stuck waiting for ESRAM
			*/
			MSG1("CAM_SM::ProcessPendingEvents: UNSUSPEND_RES, waiting ESRAM=%d\n",pGrabControl->mWaitingForEsram);
			OstTraceFiltStatic1(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents: UNSUSPEND_RES, waiting ESRAM=%d", (&mENSComponent),pGrabControl->mWaitingForEsram);
			if(pGrabControl->mWaitingForEsram)
			{
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                                pGrabControl->mWaitingForEsram = false;
                                SCF_PSTATE next;
                                SM_POP_STATE(next);
                                SCF_TRANSIT_TO_PSTATE(next);
#else
				SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_StartVpip);
#endif
			}
			else
			{
				/* in principle this case should never happen, but keep this for better robustness */
				SCF_TRANSIT_TO(&CAM_SM::WaitEvent); /* do nothing, go to sleep */
			}
			break;
		}
		/*For Shutterlag */
		case CAMERA_EOS_VPB02_SIG:
			MSG1("CAM_SM::ProcessPendingEvents - CAMERA_EOS_VPB02_SIG and PORT = %d \n",event.type.other) ;
		case GRAB_ABORTED_SIG:
		{
                    MSG1("CAM_SM::ProcessPendingEvents -  GRAB_ABORTED_SIG and PORT = %d\n",event.type.other);
                    if(((COmxCamera*)&mENSComponent)->mSceneMode.eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR)
                    {
                        if(((COmxCamera*)&mENSComponent)->eStateZSLHDR==ZSLHDRState_StopHiddenCapture)
                        {
                            MSG0("HDR: Hidden Capture aborted, eStateZSLHDR = ZSLHDRState_HiddenCaptureStopped\n");
                            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_HiddenCaptureStopped;
                            SCF_TRANSIT_TO(&CAM_SM::ZSLHDRConfigureCaptureMode);
                            break;
                        }
                        else if(((COmxCamera*)&mENSComponent)->eStateZSLHDR==ZSLHDRState_StopPreview)
                        {
                            MSG0("HDR: VF Capture aborted, eStateZSLHDR = ZSLHDRState_PreviewStopped\n");
                            ((COmxCamera*)&mENSComponent)->eStateZSLHDR = ZSLHDRState_PreviewStopped;
                            SCF_TRANSIT_TO(&CAM_SM::ZSLHDRCapture);
                            break;
                        }
                    }

			if ((CAMERA_EXIT_PREVIEW_SIG == lastUserDefferedEventDequeued.sig)
                            && (CAMERA_PORT_OUT2 == lastUserDefferedEventDequeued.type.capturingStruct.nPortIndex)
                            && (OMX_FALSE == lastUserDefferedEventDequeued.type.capturingStruct.bEnabled))
			{
				pOpModeMgr->isEOS[event.type.other] = OMX_TRUE; 
				if ((OMX_TRUE == pOpModeMgr->isEOS[CAMERA_PORT_OUT0]) && (OMX_TRUE == pOpModeMgr->isEOS[CAMERA_PORT_OUT2]))
				{
					GrabAbortStatus = GRAB_ABORT_SERVICED;
					
					if( (Sw3aStopReqStatus == SW3A_STOP_ACKNOWLEDGED) || (Sw3aStopReqStatus == SW3A_STOP_REQ_SERVICED))
					{
						SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStop);
						p3AComponentManager->whatNextAfterStop= NULL; 
					}
					else
					{
						SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
					}


				}
				else
				{
					SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
				}
			}
			else
			{
				GrabAbortStatus = GRAB_ABORT_SERVICED;
				SCF_TRANSIT_TO(&CAM_SM::RendezvousState);	
			}

			break;             
		}

		case SW3A_LOOP_CONTROL_SIG:
		{
			SCF_TRANSIT_TO(&CAM_SM::SW3A_Control);
			break;
		}
		/*For Shutterlag */     
		case SW3A_STOPPED_SIG:
		{   // The 3A component claims to be stopped. State up.
			MSG0("CAM_SM::ProcessPendingEvents - SW3A_STOPPED_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents // SW3A_STOPPED_SIG", (&mENSComponent));
			Sw3aStopReqStatus = SW3A_STOP_REQ_SERVICED;
			SCF_TRANSIT_TO(&CAM_SM::RendezvousState);
			break;
		}
		/*For Shutterlag */
		case SW3A_STOP_ACK_SIG:
		{
			MSG0("CAM_SM::ProcessPendingEvents - SW3A_STOP_ACK_SIG\n");
			Sw3aStopReqStatus = SW3A_STOP_ACKNOWLEDGED;

			if ((CAMERA_EXIT_PREVIEW_SIG == lastUserDefferedEventDequeued.sig)
                            && (CAMERA_PORT_OUT2 == lastUserDefferedEventDequeued.type.capturingStruct.nPortIndex)
                            && (OMX_FALSE == lastUserDefferedEventDequeued.type.capturingStruct.bEnabled)) 
			{	
				/*In case of low pow signal, OpMode is VideoRecord sw3A stop is requested but GrabAbort is not requested.
				Thus no need to wait for EOS */
				if(GrabAbortStatus == GRAB_ABORT_NOT_REQUESTED)
				{
					SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStop);
					p3AComponentManager->whatNextAfterStop = NULL;   // Reset the state pointer to avoid any confusion
				}
				else if ((OMX_TRUE == pOpModeMgr->isEOS[CAMERA_PORT_OUT0]) && (OMX_TRUE == pOpModeMgr->isEOS[CAMERA_PORT_OUT2]))
				{
					SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStop);
					p3AComponentManager->whatNextAfterStop = NULL;   // Reset the state pointer to avoid any confusion
				}
				else
				{
					MSG0("CAM_SM::ProcessPendingEvents - wait for EOS\n");
					SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
				}

			} 
			else 
			{	
				SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStop);
				p3AComponentManager->whatNextAfterStop = NULL;   // Reset the state pointer to avoid any confusion
			}

			break;
		}
	case CAMERA_EXIT_PREVIEW_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-EXIT_PREVIEW_SIG;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-EXIT_PREVIEW_SIG;", (&mENSComponent));
                    t_uint16 portIndex=event.type.capturingStruct.nPortIndex;
                    if(((COmxCamera*)&mENSComponent)->mSceneMode.eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR)
                    {
                        MSG0("HDR: port index %d, eStateZSLHDR %d", portIndex, ((COmxCamera*)&mENSComponent)->eStateZSLHDR);
                        if((CAMERA_PORT_OUT1==portIndex) && (((COmxCamera*)&mENSComponent)->eStateZSLHDR==ZSLHDRState_ReadyForCapture))
                        {
                            if(event.type.capturingStruct.bEnabled)
                            {
                                pOpModeMgr->ConfigCapturing[portIndex] = event.type.capturingStruct.bEnabled;
                                pOpModeMgr->ConfigCapturePort =  event.type.capturingStruct.nPortIndex;
                                SCF_TRANSIT_TO(&CAM_SM::ZSLHDRStopPreview);
                                break;
                            }
                        }
                        else
                        {
                            MSG0("HDR: eStateZSLHDR != ZSLHDRState_ReadyForCapture, so pushing event at same location\n");
                            pDeferredEventMgr->requeueEventAtSameLocation(&event);
                            SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            break;
                        }
                    }

                    if (CAMERA_PORT_OUT1 == portIndex) {
                        if(event.type.capturingStruct.bEnabled) {
				//for ER 427419 start
				if(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillFaceTracking)
				{
					//pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]=0;
					bMoveToVideo = OMX_TRUE;
					pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1]=OMX_TRUE;
					MSG1("\nMoving to do still capture for Opmode = OpMode_Cam_StillFaceTracking== %d\n",bMoveToVideo);
					SCF_TRANSIT_TO(&CAM_SM::ExitingRecord);	
				}
				else
				{
				//for ER 427419 end
                            if(pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]==0) {
                                pOpModeMgr->ConfigCapturing[portIndex] = event.type.capturingStruct.bEnabled;
                                pOpModeMgr->ConfigCapturePort =  event.type.capturingStruct.nPortIndex;
                                if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()) {
                                    COmxCamera* OMXCam = (COmxCamera*)&mENSComponent;
                                    MSG1("CAM_SM::ProcessPendingEvents-EXIT_PREVIEW_SIG;eFlashControl=%x\n",OMXCam->mCameraUserSettings.eFlashControl);
                                    t_sw3A_FlashDirective flashDirective = p3AComponentManager->mFlashController.getDirective();
                                    if((OMXCam->mCameraUserSettings.eFlashControl == OMX_IMAGE_FlashControlOn)||
                                        ((OMXCam->mCameraUserSettings.eFlashControl == OMX_IMAGE_FlashControlAuto)&&(flashDirective.flashState != SW3A_FLASH_OFF)))
                                        {
                                            //STOP SW3A then change the mode SW3A_MODE_SET_STILL
                                            p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::TimeNudge_stop3A);
                                            SW3A_stop(SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents));
                                        }
                                    else{
                                            //FLASH STATE is off then this is normal ZSL
                                            SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
                                            SCF_TRANSIT_TO(&CAM_SM::TimeNudge_StartCapture);
                                        }
                                }
                                else {
                                    SCF_TRANSIT_TO(&CAM_SM::ExitingPreview);
                                }
                            }
                            else {
                                /* if the expected capture is not yet done then do post it again as we expected to be woken up with this HR cb */
                                pDeferredEventMgr->requeueEventAtSameLocation(&event);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
				//for ER 427419 start                         
						    }
				//for ER 427419 end
                            }
                        }
                        else {
                            MSG0("End of capture has been triggered\n");
                            OstTraceFiltStatic0(TRACE_DEBUG, "End of capture has been triggered", (&mENSComponent));
                            pOpModeMgr->ConfigCapturing[portIndex] = event.type.capturingStruct.bEnabled;
                            pOpModeMgr->ConfigCapturePort =  event.type.capturingStruct.nPortIndex;
                            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                        }
                    }
                    else if (CAMERA_PORT_OUT2 == portIndex) {
                        pOpModeMgr->ConfigCapturing[portIndex] = event.type.capturingStruct.bEnabled;
                        pOpModeMgr->ConfigCapturePort =  event.type.capturingStruct.nPortIndex;
                        if (event.type.capturingStruct.bEnabled) {
                            SCF_TRANSIT_TO(&CAM_SM::ExitingPreview);
                        }
                        else {
                            SCF_TRANSIT_TO(&CAM_SM::ExitingRecord);
                        }
                    }
                    else {
                        mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadParameter, event.type.capturingStruct.nPortIndex);
                        SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                        break;
                    }
                    break;
                }
                
                case CAMERA_FILLBUFFERDONE_SIG :
                {
                    MSG0("CAM_SM::ProcessPendingEvents-CAMERA_FILLBUFFERDONE_SIG\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-CAMERA_FILLBUFFERDONE_SIG", (&mENSComponent));
                    SCF_TRANSIT_TO(&CAM_SM::FillBufferDoneVPB1_StoreBMSContext);

                    break;
                }
                case CAMERA_BUFFERED_SIG:
                {
                    if(pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]==0)
                        SCF_TRANSIT_TO(&CAM_SM::FillBufferDoneVPB1_StoreBMSContext);
                    else
                        SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                    break;
                }
                case SEND_COMMAND_SIG:
                case ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-SEND_COMMAND_SIG\n\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-SEND_COMMAND_SIG\n", (&mENSComponent));
                    CAM_SM::stopLowPowerTimer();
                    MSG1("----------CAPTURE_REQUEST =%d\n", pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]);
                    OstTraceFiltStatic1(TRACE_DEBUG, "----------CAPTURE_REQUEST =%d", (&mENSComponent), pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]);
                    /* if one expected Capture is on going */
                    if(pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]!=0)
                    {
                        if(  ((pOpModeMgr->CurrentOperatingMode==OpMode_Cam_VideoRecord)||
                                (pOpModeMgr->CurrentOperatingMode==OpMode_Cam_StillFaceTracking))
                                &&((event.args.sendCommand.Cmd == OMX_CommandPortEnable)
                                        ||( event.args.sendCommand.Cmd==OMX_CommandPortDisable))
                        ){

                            if(event.sig == ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG)
                            {
                                /* it's a port Ena with settings to be changed */
                                /* change them first then apply the port Ena */
                                enabledPort = event.args.sendCommand.nParam;
                                SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState);
                            }

                            /* TODO : we should manage case it is a gobackIdle */
                            if((event.args.sendCommand.Cmd == OMX_CommandStateSet)&&(event.args.sendCommand.nParam==OMX_StateIdle))
                            {
                                /* this is not supported yet :
                                 * what should be done ? : - self-trigger the end of the record ?
                                 *         - report error ?
                                 */
                                mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorIncorrectStateOperation, event.args.sendCommand.Cmd);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            }
                            else
                            {
                                MSG0("Forward SendCommand now in case of Ena/Disa in Record mode\n");
                                OstTraceFiltStatic0(TRACE_DEBUG, "Forward SendCommand now in case of Ena/Disa in Record mode", (&mENSComponent));
                                /* if it's a PortEna or PortDisa during a Record/Facetracking, then forward NOW */
                                Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
                                (Cam->getNmfSendCommandItf())->sendCommand(event.args.sendCommand.Cmd,event.args.sendCommand.nParam);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            }
                        }
                        else
                        {
                            /* else if it's a Still not yet done, then post the sendCommand again as we expected to be woken-up with its cb */
                            MSG1("Requeue sendCommand:%d\n",pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);
                            OstTraceFiltStatic1(TRACE_DEBUG, "Requeue sendCommand:%d", (&mENSComponent),pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);
                            pDeferredEventMgr->requeueEventAtSameLocation(&event);
                            SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                        }
                    }
                    else
                    {
                        if(event.sig == ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG)
                        {
                            /* it's a port Ena with settings to be changed */
                            /* change them first then apply the port Ena */
                            enabledPort = event.args.sendCommand.nParam;
                            OMX_STATETYPE exitedState = OMX_StateInvalid;
                            mENSComponent.GetState(&mENSComponent, &exitedState);
                            if ((bLowPower)&& (exitedState == OMX_StateExecuting)
                             &&(event.args.sendCommand.Cmd == OMX_CommandPortEnable))
                            {
                               bLowPower = FALSE;
                               SM_PUSH_STATE_STATIC(&CAM_SM::EnablePortInIdleExecutingState);
                               ControlingISP_ControlType = ControlingISP_GetState_GoRunning_Start3A;
                               MSG0("wake up from sleep!\n");
                               OstTraceFiltStatic0(TRACE_DEBUG, "wake up from sleep!", (&mENSComponent));
                               SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
                            }
                            else
                            { SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState); }
                        }
                        else {
                            /* Forward now the sendCommand as nothing is on-going*/
                            MSG0("Forward SendCommand now\n");
                            OstTraceFiltStatic0(TRACE_DEBUG, "Forward SendCommand now", (&mENSComponent));
                            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
                            (Cam->getNmfSendCommandItf())->sendCommand(event.args.sendCommand.Cmd,event.args.sendCommand.nParam);
                            OMX_STATETYPE exitedState = OMX_StateInvalid;
                            mENSComponent.GetState(&mENSComponent, &exitedState);
                            if ((bLowPower)&& (exitedState == OMX_StateExecuting)
                              &&(event.args.sendCommand.Cmd == OMX_CommandPortEnable))
                                  {
                                        bLowPower = FALSE;
                                        SM_PUSH_STATE_STATIC(&CAM_SM::WaitEvent);
                                        ControlingISP_ControlType = ControlingISP_GetState_GoRunning_Start3A;
                                        MSG0("wake up from sleep!\n");
                                        OstTraceFiltStatic0(TRACE_DEBUG, "wake up from sleep!", (&mENSComponent));
                                        SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
                                  }else if ((exitedState == OMX_StateExecuting) && (event.args.sendCommand.Cmd == OMX_CommandPortEnable))
                                  {
                                     SM_PUSH_STATE_STATIC(&CAM_SM::WaitEvent);
                                     SCF_TRANSIT_TO(&CAM_SM::Rating);
                                  }
                                  else SCF_TRANSIT_TO(&CAM_SM::WaitEvent);

                        }
                    }

		    if (event.args.sendCommand.Cmd == OMX_CommandPortDisable){
			OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
			mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
			getOmxIlSpecVersion(&mParamPortDefinition.nVersion);

    			if (event.args.sendCommand.nParam == CAMERA_PORT_OUT0){
				mENSComponent.getPort(CAMERA_PORT_OUT2)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
				if ((mParamPortDefinition.bEnabled == OMX_FALSE )
          &&(pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2] == OMX_FALSE)
          &&(((COmxCamera*)&mENSComponent)->mSensorMode.bOneShot == OMX_FALSE))
        { /* All port disabled */
					CAM_SM::startLowPowerTimer();
				}
			}
			else if (event.args.sendCommand.nParam == CAMERA_PORT_OUT2){
				mENSComponent.getPort(CAMERA_PORT_OUT0)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
				if ((mParamPortDefinition.bEnabled == OMX_FALSE )
          &&(pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2] == OMX_FALSE)
          &&(((COmxCamera*)&mENSComponent)->mSensorMode.bOneShot == OMX_FALSE))
        { /* All port disabled */
					CAM_SM::startLowPowerTimer();
				}
			}
		    }

                    break;
                }

                case CAMERA_EXECUTING_TO_IDLE_END_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-EXECUTING_TO_IDLE_SIG;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-EXECUTING_TO_IDLE_SIG;", (&mENSComponent));
                    CAM_SM::stopLowPowerTimer();
                    SCF_TRANSIT_TO(&CAM_SM::ExecutingToIdle);
                    break;
                }
                case CAMERA_EXECUTING_TO_PAUSE_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-CAMERA_EXECUTING_TO_PAUSE_SIG;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-EXECUTING_TO_PAUSE_SIG;", (&mENSComponent));
                    CAM_SM::startLowPowerTimer();
                    mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
                    pResourceSharerManager->mRSPing.Ping(0);
                    break;
                }
                case CAMERA_PAUSE_TO_EXECUTING_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-CAMERA_PAUSE_TO_EXECUTING_SIG;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-PAUSE_TO_EXECUTING_SIG;", (&mENSComponent));
		    CAM_SM::stopLowPowerTimer();
		    ControlingISP_ControlType = ControlingISP_GetState_GoRunning_SendExecuting;
		    if (bLowPower){
			bLowPower = FALSE;
			ControlingISP_ControlType = ControlingISP_GetState_GoRunning_SendExecuting_Start3A;
		    }else {
			ControlingISP_ControlType = ControlingISP_GetState_GoRunning_SendExecuting;
		    }
		    SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
		    SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
                    break;
                }

                case CAMERA_IDLE_TO_EXECUTING_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-CAMERA_IDLE_TO_EXECUTING_SIG;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-CAMERA_IDLE_TO_EXECUTING_SIG;", (&mENSComponent));
                    CAM_SM::stopLowPowerTimer();
                    bLowPower = FALSE;
                    if(OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1]) {
                        SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture);
                    }
                    else {
                        SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
                    }
                    break;
                }
               case CAMERA_INDEX_CONFIG_ZOOM_SIG:
               {
                  COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
                  MSG0("CAM_SM::ProcessPendingEvents-CAMERA_INDEX_CONFIG_ZOOM_SIG;\n");
                  OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-CAMERA_INDEX_CONFIG_ZOOM_SIG;", (&mENSComponent));
                  if ((enum OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom == event.args.omxsetConfigIndex) {
                      pZoom->ZoomCmd = ZoomCommand_e_SetFOV;
                      pZoom->ZoomFactor.xZoomFactor.nValue = OMXCam->mCameraUserSettings.xDigitalZoomFactor.nValue;
                      pZoom->ZoomFactor.xZoomFactor.nMin = OMXCam->mCameraUserSettings.xDigitalZoomFactor.nMin;
                      pZoom->ZoomFactor.xZoomFactor.nMax = OMXCam->mCameraUserSettings.xDigitalZoomFactor.nMax;
                      MSG3("Zoom: pZoom->ZoomFactor nValue = %lu, nMin = %lu, nMax = %lu\n",
                                 pZoom->ZoomFactor.xZoomFactor.nValue, pZoom->ZoomFactor.xZoomFactor.nMin, pZoom->ZoomFactor.xZoomFactor.nMax);
                      if (bLowPower) {
                          bLowPower = FALSE;
                          SM_PUSH_STATE_STATIC(&CAM_SM::Zooming);
                          ControlingISP_ControlType = ControlingISP_GetState_GoRunning_Start3A;
                          MSG0("wake up from sleep!\n");
                          OstTraceFiltStatic0(TRACE_DEBUG, "wake up from sleep!", (&mENSComponent));
                          SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
                      } else {
                          SCF_TRANSIT_TO(&CAM_SM::Zooming);
                      }
                  }
                  else if ((enum OMX_INDEXTYPE)OMX_Symbian_IndexConfigCenterFieldOfView == event.args.omxsetConfigIndex) {
                      pZoom->ZoomCmd = ZoomCommand_e_SetCenter;
                      pZoom->CenterFOV.sPoint.nX = OMXCam->mCameraUserSettings.nX;
                      pZoom->CenterFOV.sPoint.nY = OMXCam->mCameraUserSettings.nY;
                      MSG2("Zoom: pZoom->CenterFOV.sPoint nX = %lu, nY = %lu\n", pZoom->CenterFOV.sPoint.nX, pZoom->CenterFOV.sPoint.nY);
                      if (bLowPower){
                          bLowPower = FALSE;
                          SM_PUSH_STATE_STATIC(&CAM_SM::Zooming);
                          ControlingISP_ControlType = ControlingISP_GetState_GoRunning_Start3A;
                          MSG0("wake up from sleep!\n");
                          OstTraceFiltStatic0(TRACE_DEBUG, "wake up from sleep!", (&mENSComponent));
                          SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
                      } else {
                          SCF_TRANSIT_TO(&CAM_SM::Zooming);
                      }
                  } else {
                      MSG0("CAM_SM::ProcessPendingEvents-ZOOM Unexpected case\n");
                      OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-ZOOM Unexpected case", (&mENSComponent));
                      SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                  }
                  break;
               }
                case CAMERA_INDEX_CONFIG_TEST_MODE_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-TEST_MODE;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-TEST_MODE;", (&mENSComponent));
                    pTestMode->eCamTestMode = event.type.testModeStruct.eTestMode;
                    SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
                    SCF_TRANSIT_TO(&CAM_SM::SettingTestMode);
                    break;
                }
                case CAMERA_INDEX_CONFIG_MIRROR_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-Mirror;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-Mirror;", (&mENSComponent));
                    SCF_TRANSIT_TO(&CAM_SM::Mirroring);
                    break;
                }
                case SHARED_RETURN_SIG:
                {
                    OMX_STATETYPE state = pOmxStateMgr->updateState();
                    if(state != OMX_StateMax) {
                        MSG1("state change %d (%s)\n",state,CError::stringOMXstate(state));
                        OstTraceFiltStatic1(TRACE_DEBUG, "state change %d", (&mENSComponent),state);
                        mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, state);
                        if ( (state == OMX_StateIdle) || (state == OMX_StatePause) ){
                           Camera* pCam = (Camera*)&mENSComponent.getProcessingComponent();
                           pCam->iCheckTimerRunning = true;
                        }
                        else if( state == OMX_StateExecuting ) {
                           Camera* pCam = (Camera*)&mENSComponent.getProcessingComponent();
                           pCam->setExecutingStateStartTime();
                        }
                    }
                    SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                    break;
                }
                case CAMERA_INDEX_CONFIG_SELFTEST_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-SELFTEST;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-SELFTEST;", (&mENSComponent));
      		          CAM_SM::stopLowPowerTimer();
      		          if(bLowPower == TRUE) {
      		             bLowPower = FALSE;
                       ControlingISP_ControlType = ControlingISP_SendWakeUp_WaitIspWorkenUp;
      		             SM_PUSH_STATE_STATIC(&CAM_SM::EnteringSelfTest);
		                   SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
		                } else { SCF_TRANSIT_TO(&CAM_SM::EnteringSelfTest); }
                    break;
                }
                case CAMERA_INDEX_CONFIG_EXTCAPTUREMODE_SIG:
                {
                    MSG0("CAM_SM::ProcessPendingEvents-EXTCAPTUREMODE;\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::ProcessPendingEvents-EXTCAPTUREMODE;", (&mENSComponent));
                    SCF_TRANSIT_TO(&CAM_SM::TimeNudge);
                    break;
                }


                default:
                {
                    if(prior == OMX_FALSE) pDeferredEventMgr->requeueEventAtSameLocation(&event);
                    else pDeferredEventMgr->requeuePriorEventAtSameLocation(&event);
                    // generate a LIST_INFO_SIG so that the event is not known here and directed to the mother class ;-)
                    pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0,0);
                    pIspctlCom->readQueue();
                    SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
                    break;
                }
            }

            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT;return 0;

        default: break;
    }
    return COM_SM::ProcessPendingEvents(e);
}


/* */
/* CAM_SM::timeoutAction
 * actions taken if timeout happens
 */
/**/
bool CAM_SM::timeoutAction (void)
{
	if (OMX_TRUE == pSelfTest->pTesting->bEnabled)
	{
		MSG0("\t  selftest : Timeout Event\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "\t  selftest : Timeout Event", (&mENSComponent));
		pSelfTest->selftestError = OMX_ErrorHardware;
		SCF_TRANSIT_TO(&CAM_SM::ProcessingSelfTest);
		return TRUE;
	}
	else{
		MSG0("Discarded Timeout Event\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "Discarded Timeout Event", (&mENSComponent));
		return FALSE;
	}
}


void CAM_SM::SW3A_start(SCF_PSTATE whatNext) {
    DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL); // Overwriting a pending whatNext shall never happen.
    p3AComponentManager->whatNextAfterStart = whatNext;
    whatNext = NULL; // To avoid any confusion.
    SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
}


void CAM_SM::SW3A_stop(SCF_PSTATE whatNext) {
    DBC_ASSERT(p3AComponentManager->whatNextAfterStop == NULL); // Overwriting a pending whatNext shall never happen.
    p3AComponentManager->whatNextAfterStop = whatNext;
    whatNext = NULL; // To avoid any confusion.
    SCF_TRANSIT_TO(&CAM_SM::SW3A_Stop);
}

SCF_STATE CAM_SM::StopVideoRecord(s_scf_event const *e)
{
	switch (e->sig) 
	{
		case SCF_STATE_ENTRY_SIG: ENTRY; 
		return 0;
		
		case SCF_STATE_INIT_SIG: INIT; return 0;
		
		case SCF_STATE_EXIT_SIG: EXIT; return 0;	
		case HIDDEN_BMS_LAUNCHED_SIG: 
			PERF_TRACE_LAT("Stopping Pipe",tempo1);
			StillinVideo=1;
			GrabStatusNonDefferred=0;
			pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0,(t_uint32) Flag_e_FALSE);
			pIspctlCom->queuePE(SystemSetup_u8_NumOfFramesTobeSkipped_Byte0,(t_uint32) 0);
			pIspctlCom->queuePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, (t_uint32) HostInterfaceCommand_e_STOP_ISP);
			pIspctlCom->processQueue();
			pDeferredEventMgr->onlyDequeuePriorEvents(true);
			pGrabControl->setEndCaptureforHiddenBMS();
			return 0;
			
		case EVT_ISPCTL_LIST_INFO_SIG:
		case EVT_ISPCTL_INFO_SIG:
	    {
			if(e->type.ispctlInfo.info_id == ISP_STOP_STREAMING)
			{
				PERF_TRACE_LAT("Pipe Stopped (No Rx abort)",tempo1);
				SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture_HiddenBMS_DoingRawCaptureBMLConfiguration);
			}			
			return 0;			
		}		
		return 0;
		default:
			OUTR("Default State", 0); break;
	 	}
	 return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
OUTR("StopVideoRecord", 0);
}

