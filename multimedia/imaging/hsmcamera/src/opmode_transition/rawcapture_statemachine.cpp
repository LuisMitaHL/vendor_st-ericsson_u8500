/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include "hsmcam.h"
#include "camera.h"
#include "extradata.h"
#include "omxilosalservices.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_opmode_transition_rawcapture_statemachineTraces.h"
#endif

/* */
/* EnteringRawCapture state machine */
/**/
SCF_STATE CAM_SM::EnteringRawCapture(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::EnteringRawCapture_DoingRawCaptureConfiguration);
            return 0;
        }
        case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_ERROR("EnteringRawCapture-EVT_ISPCTL_ERROR_SIG\n");
            OstTraceFiltStatic0(TRACE_ERROR, "EnteringRawCapture-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
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
            MSG0("EnteringRawCapture-DEFERRED_EVENT;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture-DEFERRED_EVENT;", (&mENSComponent));
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}


SCF_STATE CAM_SM::EnteringRawCapture_DoingRawCaptureConfiguration(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        case BUFFER_AVAILABLE_SIG:
        {
            /* wait that buffers are available on VPB1 to lock HW resource */
            /* on idle->executing transition buffers are not available yet => do not wait */
            /* in selftest camera is using an internal buffer, not the one on port => do not wait */
            if ((0 != pOpModeMgr->nBufferAtMPC[CAMERA_PORT_OUT1])
                || (CAMERA_IDLE_TO_EXECUTING_SIG == lastUserDefferedEventDequeued.sig)
                || (OMX_TRUE == pSelfTest->pTesting->bEnabled)) {
                MSG1("Lock %s\n", CError::stringRSRequesterId(CAMERA_RAW));
                pResourceSharerManager->mRSLock.Lock(CAMERA_RAW);
            }
            else {
                pOpModeMgr->nWaitingForBuffer[CAMERA_PORT_OUT1] = OMX_TRUE;
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_LOCK_KO_SIG: return 0; /* nothing to do : wait in this state that HW resource is available */
        case Q_LOCK_OK_SIG:
        {
            MSG0("EnteringRawCapture_DoingRawCaptureConfiguration-Q_LOCK_OK_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_DoingRawCaptureConfiguration-Q_LOCK_OK_SIG;", (&mENSComponent));
            extradataCircularBuffer_init();

            if(pOpModeMgr->UserBurstNbFrames==0)
            {
                if (CAMERA_IDLE_TO_EXECUTING_SIG == lastUserDefferedEventDequeued.sig) {
                    pOpModeMgr->captureRequest[CAMERA_PORT_OUT1] ++;
                }
                if(pOpModeMgr->ConfigCapturing[RawCapturePort] == OMX_FALSE)
                {
                    /*
                    A single still can be cancelled by the IL client (the client
                    can reset the capturing bit once the still capture is started
                    and not yet ended). In that case, the still capture is completed
                    at the end of the outstanding raw grabbing (if the ISP is busy).
                    The current grabbed frame is not returned along with a buffer
                    (no call to EmptyThisBuffer on a downstream tunneled component).
                    If auto-pause is set, the camera enters paused state. In the negative,
                    VF is resumed. Only the EOS event is sent by the camera to the IL
                    client. Then the IL client is expected to wait for this EOS before
                    moving the component to idle.
                    */
                    pOpModeMgr->ConfigCapturing[RawCapturePort] = OMX_TRUE;
                    pOpModeMgr->UpdateCurrentOpMode();
                    pOpModeMgr->ConfigCapturing[RawCapturePort] = OMX_FALSE;
                }
                else {
                    pOpModeMgr->UpdateCurrentOpMode();
                }
            }

            pOpModeMgr->CameraBurstNbFrames = 0;
            if (OMX_TRUE == pSelfTest->pTesting->bEnabled)
            {
                pOpModeMgr->NumberOfBuffersForStill = 1;
            }
            else
            {
                OMX_ERRORTYPE err = OMX_ErrorNone;
                err = pOpModeMgr->Compute_NumberOfBuffersForStill();
                if (OMX_ErrorNone != err)
                {
                    DBC_ASSERT(0);
                }
            }

            camport * port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
            /* tell coverity that isSupportedFmt return value does not need to be checked */
            /* coverity[check_return : FALSE] */
            DBC_ASSERT(port->isSupportedFmt(CAMERA_PORT_OUT1,mParamPortDefinition.format.video.eColorFormat));
            pGrabControl->configureGrabResolutionFormat(port,1);

            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringRawCapture_ConfiguringZoom);
            ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_All;
            SCF_TRANSIT_TO(&CAM_SM::SHARED_ConfiguringISPPipe);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_ConfiguringZoom(s_scf_event const *e) {
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
            MSG0("EnteringRawCapture_ConfiguringZoom-EVT_ISPCTL_INFO_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_ConfiguringZoom-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringRawCapture_DoingRate);
            pZoom->internalZoomConfig = internalZoomConfig_resetFOVandCenter;
            SCF_TRANSIT_TO(&CAM_SM::ZoomingConfig);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_DoingRate(s_scf_event const *e) {
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
            MSG0("EnteringRawCapture_DoingRate-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_DoingRate-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringRawCapture_SendingRawCaptureIQSettings);
            SCF_TRANSIT_TO(&CAM_SM::Rating);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_SendingRawCaptureIQSettings(s_scf_event const *e)
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
            MSG0("EnteringRawCapture_SendingRawCaptureIQSettings-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_SendingRawCaptureIQSettings-Q_PONG_SIG;", (&mENSComponent));
            SM_PUSH_STATE_STATIC(&CAM_SM::EnteringRawCapture_Start3A);
            SCF_TRANSIT_TO(&CAM_SM::CAMERA_LoadingIQSettings);
            return 0;
       }
       default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_Start3A(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pIspctlCom->queuePE(SMS_Status_e_Coin_Status_Byte0, 0);
            pIspctlCom->readQueue();
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case EVT_ISPCTL_LIST_INFO_SIG:
	   {
			MSG0("EnteringRawCapture_Start3A-EVT_ISPCTL_LIST_INFO_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_Start3A-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
            			OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
		            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
       		     getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
	       	     mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
			     if(mParamPortDefinition.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit)
				{
					// trial mode : get settings before starting streaming
					// status is read in SW3A_GetFwStatusDo
					// SMS_Control_u16_CsiRawFormat_Byte0 is set in sensor class
					pIspctlCom->queuePE(SMS_Control_u32_FrameRate_x100_Byte0, pFramerate->getSensorFramerate_x100());
					pIspctlCom->queuePE(SMS_Control_u16_WOI_X_size_Byte0, (t_uint32)currentStillSensorMode.u32_woi_res_width);
					pIspctlCom->queuePE(SMS_Control_u16_WOI_Y_size_Byte0, (t_uint32)currentStillSensorMode.u32_woi_res_height);
					pIspctlCom->queuePE(SMS_Control_u16_X_size_Byte0, (t_uint32)currentStillSensorMode.u32_output_res_width);
					pIspctlCom->queuePE(SMS_Control_u16_Y_size_Byte0, (t_uint32)currentStillSensorMode.u32_output_res_height);
					pIspctlCom->queuePE(SMS_Control_e_Coin_Ctrl_Byte0, (e->type.ispctlInfo.Listvalue[0].pe_data + 1)&0x1);
					pIspctlCom->processQueue();
				}
				else
				{
					DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL); // Overwriting a pending whatNext shall never happen.
					p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::EnteringRawCapture_StartStreaming);
					p3AComponentManager->setMode(SW3A_MODE_SET_STILL,pOpModeMgr->NumberOfBuffersForStill);
					SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
				}
				return 0;
			}
			else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				if (OMX_TRUE == pSelfTest->pTesting->bEnabled)
				{
					SM_PUSH_STATE_STATIC(&CAM_SM::EnteringRawCapture_StartStreaming);
					SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture_SendBuffers);
				}
				else
				{
					// 3A start must be done before programming grab
					// because 3A tells us whether we need flash or if exposure has converged
					// and we do not want to program grab for exposure metering frames.
                    // The decision to actually send the buffers to the DSP (thanks to EnteringRawCapture_SendBuffers)
                    // is made in CAM_SM::SW3A_StartGrab().
					DBC_ASSERT(p3AComponentManager->whatNextAfterStart == NULL); // Overwriting a pending whatNext shall never happen.
					p3AComponentManager->whatNextAfterStart = SCF_STATE_PTR(&CAM_SM::EnteringRawCapture_StartStreaming);
					p3AComponentManager->setMode(SW3A_MODE_SET_STILL,pOpModeMgr->NumberOfBuffersForStill);
					SCF_TRANSIT_TO(&CAM_SM::SW3A_Start);
				}
				return 0;
			}
			else
			{
				break;
			}
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_SendBuffers(s_scf_event const *e)
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
            camport * port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);	
            MSG0("EnteringRawCapture_SendBuffers-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_SendBuffers-Q_PONG_SIG;", (&mENSComponent));

			// camera selftest : bypass grabctl
			// always ignore user settings
			if (OMX_TRUE == pSelfTest->pTesting->bEnabled)
            {
                if (OMX_SYMBIAN_CameraSelfTestFocus != pSelfTest->currentSelftest) {
                    pGrab->execute(GRBPID_PIPE_RAW_OUT, pSelfTest->grabParams, 3);
                }
            }
            else
            {
                if (1 == pOpModeMgr->NumberOfBuffersForStill)
                {
                    pGrabControl->setStartOneShotCapture(RawCapturePort);
                }
                else
                {
                    pGrabControl->startBurstCapture(RawCapturePort, pOpModeMgr->NumberOfBuffersForStill);
                }
            }

            SCF_PSTATE next;
            SM_POP_STATE(next);
            SCF_TRANSIT_TO_PSTATE(next);
            return 0;
        }
    default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::EnteringRawCapture_StartStreaming(s_scf_event const *e)
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
            MSG0("EnteringRawCapture_StartPipe-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_StartPipe-Q_PONG_SIG;", (&mENSComponent));

            // reset synchro booleans
            stillSynchro_BufferReceived = false;
            stillSynchro_StatsReceived = false;

            if ((OMX_TRUE == pSelfTest->pTesting->bEnabled) && (OMX_SYMBIAN_CameraSelfTestSensor != pSelfTest->currentSelftest))
            {
                SM_PUSH_STATE_STATIC(&CAM_SM::ProcessingSelfTest);
            }
            else
            {
                SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);
            }

            if ((0 == pOpModeMgr->UserBurstNbFrames)
                && (CAMERA_IDLE_TO_EXECUTING_SIG == lastUserDefferedEventDequeued.sig)
                && (OMX_TRUE == lastUserDefferedEventDequeued.args.sendCommand.nSendCallback)) {
                lastUserDefferedEventDequeued.args.sendCommand.nSendCallback = OMX_FALSE;
                if (OMX_StateExecuting == lastUserDefferedEventDequeued.args.sendCommand.nParam) {
                    ControlingISP_ControlType = ControlingISP_SendRun_WaitIspStreaming_SendExecuting;
                }
                else {
                    ControlingISP_ControlType = ControlingISP_SendRun_WaitIspStreaming_SendPause;
                }
            }
            else {
                ControlingISP_ControlType = ControlingISP_SendRun_WaitIspStreaming;
            }
            SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::FillBufferDoneVPB1_StoreBMSContext(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            /* recover BMS context */
            /* done only at first frame in burst usecase */
            if (0 == pOpModeMgr->UserBurstNbFrames)
            {
                pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0, 0);
                pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0, 0);
                pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0, 0);
                pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0, 0);
                //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMin_Byte0, 0); /*CR 417253*/
                //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMin_Byte0, 0); /*CR 417253*/
                //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTXAddrMax_Byte0, 0); /*CR 417253*/
                //pIspctlCom->queuePE(SensorFrameConstraints_u16_VTYAddrMax_Byte0, 0); /*CR 417253*/
                pIspctlCom->queuePE(CurrentFrameDimension_f_PreScaleFactor_Byte0, 0);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrStart_Byte0, 0);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrStart_Byte0, 0);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0, 0);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0, 0);
                pIspctlCom->queuePE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0,0);
                pIspctlCom->queuePE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0,0);
                pIspctlCom->queuePE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0,0);
                pIspctlCom->queuePE(SystemConfig_Status_e_PixelOrder_Byte0,0);

                if (0 == pZoom->maxFOVXAtCurrentCenter)
                {
                    pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0, 0);
                }
            }
            else
            {
                pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);
            }
            pIspctlCom->readQueue();
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT;  return 0;

        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("EnteringRawCapture_StoreBMSContext-EVT_ISPCTL_LIST_INFO_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "EnteringRawCapture_StoreBMSContext-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;

            /* store BMS context */
            if (0 == pOpModeMgr->UserBurstNbFrames)
            {
                pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = e->type.ispctlInfo.Listvalue[0].pe_data;
                pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = e->type.ispctlInfo.Listvalue[1].pe_data;
                pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize = e->type.ispctlInfo.Listvalue[2].pe_data;
                pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize = e->type.ispctlInfo.Listvalue[3].pe_data;
                //pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMin = e->type.ispctlInfo.Listvalue[4].pe_data; /*CR 417253*/
                //pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMin = e->type.ispctlInfo.Listvalue[5].pe_data; /*CR 417253*/
                //pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTXAddrMax = e->type.ispctlInfo.Listvalue[6].pe_data; /*CR 417253*/
                //pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_VTYAddrMax = e->type.ispctlInfo.Listvalue[7].pe_data; /*CR 417253*/
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_f_PreScaleFactor = e->type.ispctlInfo.Listvalue[4].pe_data;
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrStart = e->type.ispctlInfo.Listvalue[5].pe_data;
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrStart = e->type.ispctlInfo.Listvalue[6].pe_data;
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTXAddrEnd = e->type.ispctlInfo.Listvalue[7].pe_data;
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_u16_VTYAddrEnd = e->type.ispctlInfo.Listvalue[8].pe_data;
                pCapture_context->nBMS_capture_context.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun= e->type.ispctlInfo.Listvalue[9].pe_data;
                pCapture_context->nBMS_capture_context.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun= e->type.ispctlInfo.Listvalue[10].pe_data;
                pCapture_context->nBMS_capture_context.CurrentFrameDimension_u8_NumberOfStatusLines= e->type.ispctlInfo.Listvalue[11].pe_data;
                pCapture_context->nBMS_capture_context.SystemConfig_Status_e_PixelOrder= e->type.ispctlInfo.Listvalue[12].pe_data;

                /* store maximum FOV X available if needed */
                if (0 == pZoom->maxFOVXAtCurrentCenter)
                {
                    pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[13].pe_data);
                }
            }

            if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()) {
                MSG0("still synchro : time nudge, no synchro with stats\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "still synchro : time nudge, no synchro with stats", (&mENSComponent));
                OMX_ERRORTYPE error = OMX_ErrorUndefined;
                /* buffer is kept in dsp => compute extradatas and store them in circular buffer */
                if (OMX_FALSE == extradataCircularBuffer_isEmptying()) {
                    error = setVPB1Extradata(NULL, false);
                    if (OMX_ErrorNone != error) {
                        DBC_ASSERT(0);
                    }
                    /* nothing else to do */
                    SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                }
                /* buffer is at proxy level */
                /* extradatas added in next CAM_SM state, no need to synchronise with stats */
                else {
                    SCF_TRANSIT_TO(&CAM_SM::FillBufferDoneVPB1_AddExtradata);
                }
            }
            /* "standard" still capture */
            else if (true == stillSynchro_StatsReceived||((COmxCamera*)&mENSComponent)->eStateZSLHDR==ZSLHDRState_TakeCapture)
            {
                MSG0("still synchro : buffer and stats received\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "still synchro : buffer and stats received", (&mENSComponent));
                SCF_TRANSIT_TO(&CAM_SM::FillBufferDoneVPB1_AddExtradata);
            }
            else
            {
                /* update synchro */
                stillSynchro_BufferReceived = true;
                /* waiting for stats */
                MSG0("still synchro : buffer received, waiting for stats\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "still synchro : buffer received, waiting for stats", (&mENSComponent));
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }
            return 0;
        }

        case Q_RELEASED_SIG:
        {
            SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
            return 0;
        }

        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}


SCF_STATE CAM_SM::FillBufferDoneVPB1_AddExtradata(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT;  return 0;

        case Q_PONG_SIG:
        {
            MSG0("CAM_SM::FillBufferDoneVPB1_AddExtradata-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::FillBufferDoneVPB1_AddExtradata-Q_PONG_SIG;", (&mENSComponent));
            /* get the OMX buffer address stored in fillBufferDone */
            OMX_BUFFERHEADERTYPE* pOmxBufHdr = NULL;
            OMX_ERRORTYPE error = pCapture_context->popOmxBufHdr(&pOmxBufHdr);
            bool bCancelStill = false;
            bool bEndOfStillSequence = true;
            camport * port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

            if ((OMX_ErrorNone != error) || (NULL == pOmxBufHdr))
            {
                DBC_ASSERT(0);
                return 0;
            }

            /* check that buffer and stats are of the same frame */
            /* frame id is hidden in timestamp */
            OMX_U64 frame_counter = pOmxBufHdr->nTimeStamp;
            frame_counter >>= 56;
            if (p3AComponentManager->frame_counter != frame_counter)
            {
                MSG2("bad frame id, buffer : %lld, stats : %ld\n", frame_counter, p3AComponentManager->frame_counter);
                OstTraceFiltStatic2(TRACE_DEBUG, "bad frame id, buffer : %lld, stats : %ld", (&mENSComponent), frame_counter, p3AComponentManager->frame_counter);
                //TODO : assert (or better ?) when frame IDs are ok in flash usecases
                //DBC_ASSERT(0);
            }

            stillSynchro_BufferReceived = false;
            stillSynchro_StatsReceived = false;

            /* increment frame number */
            pOpModeMgr->UserBurstNbFrames++;
            pOpModeMgr->CameraBurstNbFrames++;

            /* end of "user burst" */
            /* normal end of limited burst */
            if (((pOpModeMgr->GetBurstNberOfFrames() == pOpModeMgr->UserBurstNbFrames) && pOpModeMgr->IsLimited())
            /* end of infinite burst or end of limited burst before its normal end */
                || ((OMX_FALSE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1])
                    && ((pOpModeMgr->NumberOfBuffersForStill == pOpModeMgr->CameraBurstNbFrames)
                        || ((pOmxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) != 0))))
            {
                MSG0("CAM_SM::FillBufferDoneVPB1_AddExtradata : end of user burst\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::FillBufferDoneVPB1_AddExtradata : end of user burst", (&mENSComponent));
                /* "user burst" ends at end of "camera burst" */
                /* time nudge : we always need to stop 3A */
				if(pOpModeMgr->CurrentOperatingMode==OpMode_Cam_StillCaptureSingleInVideo)
				{					
				 SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_Config_Post_StillinVideo);
				}
                else if(((COmxCamera*)&mENSComponent)->eStateZSLHDR==ZSLHDRState_TakeCapture) //HDR
                {
                    MSG0("HDR: calling ResumeAfterZSLHDR\n");
                    ((COmxCamera*)&mENSComponent)->eStateZSLHDR=ZSLHDRState_CaptureFinished;
                    SCF_TRANSIT_TO(&CAM_SM::ResumeAfterZSLHDR);
                }
                else if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
                {
                      if(mParamPortDefinition.format.video.eColorFormat != (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
                      {
                             SCF_TRANSIT_TO(&CAM_SM::ExitingPreview);
                      }
                      else
                      {
                             extradataCircularBuffer_init();
                             pGrabControl->setStartHiddenCapture(CAMERA_PORT_OUT1,(t_uint16)pOpModeMgr->GetFrameBefore());
                             SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                      }
                }
                /* 3A and grab are already stopped */
                else if (OMX_FALSE == pOpModeMgr->NeedToStop3A)
                {
                    SCF_TRANSIT_TO(&CAM_SM::ExitingRawCapture);
                }
                /* "camera burst" as been stopped */
                /* DSP is stopped, we need to stop 3A now */
                else
                {
                    pOpModeMgr->NeedToStop3A = OMX_FALSE;
                    DBC_ASSERT(p3AComponentManager->whatNextAfterStop == NULL); // Overwriting a pending whatNext shall never happen.
                    p3AComponentManager->whatNextAfterStop = SCF_STATE_PTR(&CAM_SM::ExitingRawCapture);
                    SCF_TRANSIT_TO(&CAM_SM::SW3A_Stop);
                }

                /* reset variables */
                pOpModeMgr->NumberOfBuffersForStill = 0;
                pOpModeMgr->UserBurstNbFrames = 0;
                pOpModeMgr->CameraBurstNbFrames = 0;

                pOpModeMgr->ConfigCapturing[RawCapturePort] = OMX_FALSE;
                COmxCamera* pOmxCamera = (COmxCamera*)&mENSComponent;

                /* check if user has canceled the capture, exclude the case of infinite burst */
                bCancelStill = ((pOmxCamera->mCapturingStruct_VPB1.bEnabled == OMX_FALSE)
                               &&(pOpModeMgr->IsLimited()));

                pOmxCamera->mCapturingStruct_VPB1.bEnabled = OMX_FALSE;
                pOpModeMgr->captureRequest[RawCapturePort] --;

                /* we need to update opmode before sending EOS */
                if (OMX_FALSE == pOpModeMgr->IsTimeNudgeEnabled())
                {
                    pOpModeMgr->UpdateCurrentOpMode();
                }

                /* send EOS event */
                mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_EOS, 0);
            }

            /* end of "camera burst" */
            else if (pOpModeMgr->NumberOfBuffersForStill == pOpModeMgr->CameraBurstNbFrames)
            {
                MSG0("CAM_SM::FillBufferDoneVPB1_AddExtradata : end of camera burst\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::FillBufferDoneVPB1_AddExtradata : end of camera burst", (&mENSComponent));
                /* CAMERA_RAW resource lock is made in Camera::fillThisBuffer */

                if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
                {
                    SCF_TRANSIT_TO(&CAM_SM::ExitingPreview);
                }
                else
                {
                    SCF_TRANSIT_TO(&CAM_SM::ExitingRawCapture);
                }

                /* replace EOS flag set by DSP by an EOF flag */
                pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
                pOmxBufHdr->nFlags &= ~OMX_BUFFERFLAG_EOS;

                /* send EOF event */
                mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_ENDOFFRAME, 0);

                bEndOfStillSequence = false;
            }

            /* burst ongoing */
            else
            {
                MSG0("CAM_SM::FillBufferDoneVPB1_AddExtradata : burst ongoing\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "CAM_SM::FillBufferDoneVPB1_AddExtradata : burst ongoing", (&mENSComponent));
                /* user wants to stop an infinite burst or a finite burst before its end */
                /* if "camera burst" will not stop at next frame then force stop */
                /* could stop 1 frame faster if doing it in CAM_SM::ProcessPendingEvents ... */
                if ((OMX_FALSE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1])
                    && (OMX_FALSE == pOpModeMgr->IsTimeNudgeEnabled())
                    && (pOpModeMgr->NumberOfBuffersForStill - pOpModeMgr->CameraBurstNbFrames > 1)
                    && (OMX_FALSE == pOpModeMgr->NeedToStop3A))
                {
                    MSG0("CAM_SM::FillBufferDoneVPB1_AddExtradata : capturing bit disabled\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "FillBufferDoneVPB1_AddExtradata : capturing bit disabled", (&mENSComponent));
                    pGrabControl->setEndCapture(CAMERA_PORT_OUT1);
                    pOpModeMgr->NeedToStop3A = OMX_TRUE;
                }

                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);

                /* send EOF event */
                mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_ENDOFFRAME, 0);

                bEndOfStillSequence = false;
            }

            if ((!bCancelStill) && bEndOfStillSequence && p3AComponentManager->isPostCaptureFlashNeeded()) {
                p3AComponentManager->startPostCaptureFlashes();
            }

            /* append extradatas */
            error = setVPB1Extradata(pOmxBufHdr, true);
            if (OMX_ErrorNone != error) {
                DBC_ASSERT(0);
            }

            // fillBufferDone transmitted to the port
            if(!bCancelStill)
                mENSComponent.fillBufferDone(pOmxBufHdr);
            else
            {
                MSG0("Still capture has been canceled, recycle the buffer\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "Still capture has been canceled, recycle the buffer", (&mENSComponent));
                ((Camera*)&(mENSComponent.getProcessingComponent()))->fillThisBuffer(pOmxBufHdr);
            }
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringRawCapture);
}



/* */
/* ExitingRawCapture state machine */
/**/
SCF_STATE CAM_SM::ExitingRawCapture(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::ExitingRawCapture_StoppingFlash);
            return 0;
        }
        case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_ERROR("ExitingRawCapture-EVT_ISPCTL_ERROR_SIG\n");
            OstTraceFiltStatic0(TRACE_ERROR, "ExitingRawCapture-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
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
            MSG0("ExitingRawCapture-DEFERRED_EVENT;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRawCapture-DEFERRED_EVENT;", (&mENSComponent));
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}


SCF_STATE CAM_SM::ExitingRawCapture_StoppingFlash(s_scf_event const *e)
{
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pDeferredEventMgr->onlyDequeuePriorEvents(true);
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;

        case Q_PONG_SIG:
        {
            MSG0("ExitingRawCapture_StoppingFlash-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRawCapture_StoppingFlash-Q_PONG_SIG;", (&mENSComponent));

            // disable flash mode
            cam_flash_err_e err = p3AComponentManager->mFlashController.unConfigure();
            if (err != CAM_FLASH_ERR_NONE)
            {
                DBGT_ERROR("Error: Flash unconfigure failed.\n");
                OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash unconfigure failed.", (&mENSComponent));
                DBC_ASSERT(0);
            }
            SCF_TRANSIT_TO(&CAM_SM::ExitingRawCapture_StoppingVpip);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingRawCapture);
}


SCF_STATE CAM_SM::ExitingRawCapture_StoppingVpip(s_scf_event const *e)
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
            MSG0("ExitingRawCapture_StoppingVpip-Q_PONG_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRawCapture_StoppingVpip-Q_PONG_SIG;", (&mENSComponent));

            SM_PUSH_STATE_STATIC(&CAM_SM::ExitingRawCapture_FreeCamera);
            ControlingISP_ControlType = ControlingISP_SendStop_WaitIspSensorStopStreaming;
            SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingRawCapture);
}


SCF_STATE CAM_SM::ExitingRawCapture_FreeCamera(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            if(OMX_TRUE == pOpModeMgr->AutoPauseAfterCapture) {
                // here the camera component calls itself through OMX interface
                // It is important to do this internally (through mENSComponent)
                // and NOT through OMX_SendCommand, to avoid going through ENS wrapper 
                // thus taking twice the mutex !
                mENSComponent.SendCommand(&mENSComponent, OMX_CommandStateSet, OMX_StatePause, NULL);
            }

            pResourceSharerManager->mRSRelease.Release(CAMERA_RAW);
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        case Q_RELEASED_SIG:
        {
            MSG0("ExitingRawCapture_FreeCamera-Q_RELEASED_SIG;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ExitingRawCapture_FreeCamera-Q_RELEASED_SIG;", (&mENSComponent));
            pDeferredEventMgr->onlyDequeuePriorEvents(false);
			//for ER 427419 start
			if(OMX_TRUE == bMoveToVideo)
			{
				pOpModeMgr->ConfigCapturePort = 2;
				pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort] ++;	
				bMoveToVideo = OMX_FALSE;
				SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);	
			}//for ER 427419 end
            else if (OMX_TRUE == pSelfTest->pTesting->bEnabled)
            {
                SCF_TRANSIT_TO(&CAM_SM::ProcessingSelfTest);
            }
            else if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
            {
                SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
            }
            else if (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1])
            {
                SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture);
            }
            else {
			if(StillinVideo ==0)
                SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
			else
				SCF_TRANSIT_TO(&CAM_SM::EnteringPreview_Config_Post_StillinVideo);
            }
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingRawCapture);
}



//this for configuring BML
SCF_STATE CAM_SM::EnteringRawCapture_HiddenBMS_DoingRawCaptureBMLConfiguration(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
   			{
		   	pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		   	}
	    case Q_PONG_SIG:
			{

					OMX_PARAM_PORTDEFINITIONTYPE port_params;
					COmxCamera* omxcam= (COmxCamera*)&mENSComponent.getProcessingComponent();
					PERF_TRACE_LAT("Configuring Grab",tempo1);
					//Now setting the still BML configuration. 
					camport * 	port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
					port_params = port->getParamPortDefinition();
					if ((port_params.format.video.eColorFormat == OMX_COLOR_FormatYUV420Planar)||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) ||(port_params.format.video.eColorFormat ==(OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar)) {			
						MSG0("Allocation for Grab Cache here !!!");	
						t_cm_error cm_error;
						cm_error = pGrabControl->allocateGrabCache(port->mInternalFrameWidth);
						if (cm_error != CM_OK)
							{
							MSG1("Unable to allocate grab cache in hsmcamera, error=%d", cm_error);
							OstTraceFiltStatic1 (TRACE_ERROR, "Unable to allocate grab cache in hsmcamera, error=%d", (&mENSComponent), cm_error);
							}
						}
					pGrabControl->configureGrabResolutionFormat(port,(t_uint8)((COmxCamera*)&mENSComponent)->mVariableFramerateKeepRatio.nSelectRatio,0);
					return 0;								
			}
        case SCF_STATE_EXIT_SIG:
    	    {
            MSG0("EnteringRawCapture_HiddenBMS_DoingRawCaptureConfiguration-EXIT;\n");
            OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_HiddenBMS_DoingRawCaptureConfiguration-EXIT;", (&mENSComponent));
            return 0;
			}
		case SCF_STATE_INIT_SIG:
			{
			return 0;
			}
        case CONFIGURE_SIG:
			{
			//enabling BML path only no settings changed taking still on the same path.
			OMX_ERRORTYPE error = OMX_ErrorUndefined;
			/* Can be modified and remain as is because it's set after in the Camera */
			PERF_TRACE_LAT("Configuring BML",tempo1);
			pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0,(t_uint32) Flag_e_FALSE);
			pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
			pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0,(t_uint32) Flag_e_FALSE);
			pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0,(t_uint32) Flag_e_FALSE);
			pIspctlCom->queuePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, (t_uint32)FrameDimensionProgMode_e_Manual);
			pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0, (t_uint32)InputImageSource_e_BayerLoad1);

			MSG0("In pPreset == OMX_SYMBIAN_RawImageProcessed\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "In pPreset == OMX_SYMBIAN_RawImageProcessed", (&mENSComponent));
			/* RSO = channel offset */
			pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0,(t_uint32) Flag_e_FALSE);
			pIspDampers->RsoControl(ISP_DAMPERS_DISABLE);
			/* linearization = SDL */
			pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0, (t_uint32)SDL_Disable);
			/* LSC = Lens Shading Correction = gridiron */
			pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0,(t_uint32) Flag_e_FALSE);
			/* digital gain = channel gains */
			pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, (t_uint32)Flag_e_FALSE);
			/* green imbalance = scorpio */
			pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
			/* noise correction = arctic = duster (noise filtering + defect correction) */
			/* noise correction = arctic = duster: enable gaussian only */
			pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0,(t_uint32) Flag_e_TRUE);
			pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, (t_uint32)0x348);
			/* duster_defcor_bypass | bypass_scythe */
			pIspDampers->NoiseModelControl(ISP_DAMPERS_DISABLE);
			pIspDampers->DefCorControl(ISP_DAMPERS_DISABLE);
			pIspDampers->NoiseFilterControl(ISP_DAMPERS_ENABLE);
			pIspDampers->ScytheControl(ISP_DAMPERS_DISABLE);
			/* binning repair */
			pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
			/* Sharpening / Peaking = adsoc */
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, (t_uint32)Flag_e_TRUE);
			pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0,(t_uint32) Flag_e_TRUE);
			pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);
			pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
			COmxCamera* omxcam= (COmxCamera*)&mENSComponent.getProcessingComponent();


			//Now setting the still BML configuration. 
			camport * 	port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
			pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,(t_uint32)OutputFormat_e_YUV);
			pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, (t_uint32)port->mInternalFrameWidth);
			pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, (t_uint32)port->mInternalFrameHeight);
			pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0,(t_uint32)0);


			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0,(t_uint32) pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVX );// 2608);
			pIspctlCom->queuePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0,(t_uint32) pCapture_context->nBMS_capture_context.FrameDimensionStatus_u16_MaximumUsableSensorFOVY );//1960
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0,(t_uint32)	pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPXOutputSize);// 2608);	
			pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0,(t_uint32)	pCapture_context->nBMS_capture_context.SensorFrameConstraints_u16_MaxOPYOutputSize);//1960
			pIspctlCom->queuePE(CurrentFrameDimension_f_PreScaleFactor_Byte0,(t_uint32)  pCapture_context->nBMS_capture_context.CurrentFrameDimension_f_PreScaleFactor);
			pIspctlCom->processQueue();		
			return 0;
        	}
        case EVT_ISPCTL_LIST_INFO_SIG:
			{		
			SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture_HiddenBML_Start);		
			return 0;
			}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::StopVideoRecord);
}


SCF_STATE CAM_SM::EnteringRawCapture_HiddenBML_Start(s_scf_event const *e)
{
   switch (e->sig) {

		case SCF_STATE_ENTRY_SIG:
			{
			MSG0("EnteringRawCapture_HiddenBML_Start-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-ENTRY;", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
			}
		case SCF_STATE_INIT_SIG:
			{
			INIT;
			MSG0("EnteringRawCapture_HiddenBML_Start-INIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ControlingISPState_Run-INIT;", (&mENSComponent));
			return 0;
			}

	   case SCF_STATE_EXIT_SIG:
		   	{
		   	MSG0("EnteringRawCapture_HiddenBML_Start-EXIT;\n");
		  	OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-EXIT;", (&mENSComponent));
			return 0;
		   	}

	   case Q_PONG_SIG:
			{
			pDeferredEventMgr->onlyDequeuePriorEvents(true);
			PERF_TRACE_LAT("Starting BML",tempo1);
			MSG0("EnteringRawCapture_HiddenBML_Start-Q_PONG_SIG;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringRawCapture_Start-Q_PONG_SIG;", (&mENSComponent));
			pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_RUN);
			return 0;
			}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG:
			{
			if(e->type.ispctlInfo.info_id == ISP_LOAD_READY)
				{
				MSG0(" EnteringRawCapture_HiddenBML_Start-ISP_LOAD_READY\n");	   
				PERF_TRACE_LAT("Starting BML ISP_LOAD_READY received ",tempo1);
				pDeferredEventMgr->onlyDequeuePriorEvents(false);	
				MSG0("EnteringRawCapture_SendBuffers ;\n"); 
				pGrabControl->setBMLCapture(CAMERA_PORT_OUT1);
				SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);//TODO This is fine here.	  
				return 0;
				}
			return 0;
			}
		default: break;
	}

   return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);

}



//Todo stop isp_stop and reconfigure pipe for video record use case 
SCF_STATE CAM_SM::EnteringPreview_Config_Post_StillinVideo(s_scf_event const *e)
{
	struct timeval tempo;
	switch (e->sig) 
		{
		case SCF_STATE_ENTRY_SIG:
			MSG0("EnteringPreview_Config_Post_StillinVideo-ENTRY;\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview_Config_Post_StillinVideo-ENTRY;", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;

	   case SCF_STATE_EXIT_SIG:
		   MSG0("EnteringPreview_Config_Post_StillinVideo-EXIT;\n");
		   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview_Config_Post_StillinVideo-EXIT;", (&mENSComponent));
		   return 0;

	   case Q_PONG_SIG:
			{
			MSG0("EnteringPreview_Config_Post_StillinVideo-Q_PONG_SIG\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview_Config_Post_StillinVideo-Q_PONG_SIG", (&mENSComponent));	        
			pDeferredEventMgr->onlyDequeuePriorEvents(true);
			PERF_TRACE_LAT("Stopping Pipe after BML",tempo1);
			pipeStartedAfterCapture=0;
			pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_STOP_ISP);
			}
			return 0;	
		case EVT_ISPCTL_LIST_INFO_SIG:
		case EVT_ISPCTL_INFO_SIG:
			if((e->type.ispctlInfo.info_id == ISP_WRITE_DONE)||(e->type.ispctlInfo.info_id ==ISP_WRITELIST_DONE))
				{
				if(pipeStartedAfterCapture==1 && (e->type.ispctlInfo.info_id ==ISP_WRITELIST_DONE))
				   	{
				   	PERF_TRACE_LAT("Pipe Running requested(Restoration).",tempo1);
					pipeStartedAfterCapture=2;			
					pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, (t_uint32)Configured_RxAbort_OnStop);
					pIspctlCom->queuePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, (t_uint32)HostInterfaceCommand_e_RUN);
					pIspctlCom->processQueue();
					}
	            return 0;
				}
			else if(e->type.ispctlInfo.info_id == ISP_STOP_STREAMING)
				{
			
		        OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
        		mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		        getOmxIlSpecVersion(&mParamPortDefinition.nVersion);		
        		mENSComponent.getPort(CAMERA_PORT_OUT2)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

				if(mParamPortDefinition.bEnabled==OMX_TRUE &&  (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT2]))
				{
					pGrabControl->setStartCapture(CAMERA_PORT_OUT0); // DMA for HR
					pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, (t_uint32)Flag_e_TRUE);
				}
				else
				{
					pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe0Enable_Byte0,(t_uint32) Flag_e_FALSE);
				}

        		mENSComponent.getPort(CAMERA_PORT_OUT0)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
				if(mParamPortDefinition.bEnabled==OMX_TRUE &&  (OMX_TRUE == pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT0]))
				{
				pGrabControl->setStartCapture(CAMERA_PORT_OUT0); // DMA for LR
					pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, (t_uint32)Flag_e_TRUE);
				}
				else
				{
					pIspctlCom->queuePE(DataPathControl_e_Flag_Pipe1Enable_Byte0,(t_uint32) Flag_e_FALSE);
				}
								
				pipeStartedAfterCapture=1;
				MSG0("CAM_SM::EnteringPreview_Config_Post_StillinVideo Restoring Pipe Configuration before starting it");
				Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
				camport *port;
				/* select source */
				if (pSensor->GetCameraSlot() == ePrimaryCamera) pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0,(t_uint32) InputImageSource_e_Sensor0);
				if (pSensor->GetCameraSlot() == eSecondaryCamera) pIspctlCom->queuePE(SystemSetup_e_InputImageSource_Byte0, (t_uint32)InputImageSource_e_Sensor1);		
				pIspctlCom->queuePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0,(t_uint32) FrameDimensionProgMode_e_Auto);
				 /* enable LR pipe */
        		pIspctlCom->queuePE(Pipe_1_e_Flag_TogglePixValid_Byte0,(t_uint32) Flag_e_FALSE);
        	     /* enable HR pipe */
            	pIspctlCom->queuePE(Pipe_0_e_Flag_TogglePixValid_Byte0, (t_uint32)Flag_e_FALSE);
				pIspctlCom->queuePE(DataPathControl_e_BayerStore2Source_Byte0, (t_uint32)BayerStore2Source_e_BayerCrop);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, (t_uint32)Flag_e_FALSE);
            	pIspctlCom->queuePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0,(t_uint32) Flag_e_TRUE);
		        /* program image quality blocks */
        		/* RSO = channel offset */		
		        pIspctlCom->queuePE(RSO_Control_e_Flag_EnableRSO_Byte0, (t_uint32)Flag_e_TRUE);
		        pIspctlCom->queuePE(RSO_Control_e_RSO_Mode_Control_Byte0,(t_uint32) RSO_Mode_e_Manual);
		        pIspDampers->RsoControl(ISP_DAMPERS_ENABLE);
        		/* linearization = SDL */
		        pIspctlCom->queuePE(SDL_Control_e_SDLMode_Control_Byte0,(t_uint32) SDL_Custom);
		        /* LSC = Lens Shading Correction = gridiron */
		        pIspctlCom->queuePE(GridironControl_e_Flag_Enable_Byte0,(t_uint32) Flag_e_TRUE);
			 	pIspctlCom->queuePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0,(t_uint32) Flag_e_TRUE);
		        /* green imbalance = scorpio */
		        /*Scorpio enabled*/
		        pIspctlCom->queuePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, (t_uint32)Flag_e_TRUE);
		        pIspctlCom->queuePE(Scorpio_Ctrl_e_ScorpioMode_Byte0, (t_uint32)ScorpioMode_e_Manual);
		        pIspDampers->ScorpioControl(ISP_DAMPERS_ENABLE);
		        /* noise correction = arctic = duster (noise filtering + defect correction) */
		        pIspctlCom->queuePE(DusterControl_e_Flag_DusterEnable_Byte0,(t_uint32) Flag_e_TRUE);
		        pIspctlCom->queuePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0,(t_uint32) 0x140);
		        /* duster_rc_enablegrad | bypass_scythe */
		        pIspDampers->DefCorControl(ISP_DAMPERS_ENABLE);
		        pIspDampers->NoiseFilterControl(ISP_DAMPERS_ENABLE);
		        pIspDampers->NoiseModelControl(ISP_DAMPERS_ENABLE);
		        pIspDampers->ScytheControl(ISP_DAMPERS_ENABLE);
		        /* binning repair */
		        pIspctlCom->queuePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, (t_uint32)Flag_e_FALSE);
		        /* Sharpening / Peaking = adsoc */
		        pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0,(t_uint32) Flag_e_TRUE);
		        pIspctlCom->queuePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0, (t_uint32)Flag_e_FALSE);
		        pIspDampers->SharpeningHrControl(ISP_DAMPERS_ENABLE);
		        pIspctlCom->queuePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0,(t_uint32) Flag_e_TRUE);
		        pIspDampers->SharpeningLrControl(ISP_DAMPERS_ENABLE);				
				int xSize, ySize;
				PERF_TRACE_LAT("Pipe Streaming Stopped. Configuring grab",tempo1);
				port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);		 
				pIspctlCom->queuePE(Pipe_1_e_OutputFormat_Pipe_Byte0,(t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat));
				/* configure pipe resolutions */
    		    pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
            	pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                            1.0, xSize, ySize, CAMERA_PORT_OUT0);
				pIspctlCom->queuePE(Pipe_1_u16_X_size_Byte0, (t_uint32)xSize);
				pIspctlCom->queuePE(Pipe_1_u16_Y_size_Byte0,(t_uint32) ySize);
				port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT2);
				camport * portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
				pIspctlCom->queuePE(Pipe_0_e_OutputFormat_Pipe_Byte0,(t_uint32)convertFormatForISP(port->getParamPortDefinition().format.video.eColorFormat));
				pGrabControl->getOverscanResolution(port->mInternalFrameWidth, port->mInternalFrameHeight,
				pGrabControl->gconvertFormatForFW(port->getParamPortDefinition().format.video.eColorFormat, false),
                                                1.0, xSize, ySize,CAMERA_PORT_OUT2);
				pIspctlCom->queuePE(Pipe_0_u16_X_size_Byte0, (t_uint32)xSize);
				pIspctlCom->queuePE(Pipe_0_u16_Y_size_Byte0, (t_uint32)ySize);
				pIspctlCom->processQueue();				
				
				return 0;
				}
			else if(e->type.ispctlInfo.info_id == ISP_STREAMING)
				{
				pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, (t_uint32)Configured_RxAbort_OnStop);
				pIspctlCom->writePE(SystemSetup_u8_NumOfFramesTobeSkipped_Byte0,(t_uint32)nFramesTobeSkipped);
				pIspctlCom->processQueue();
				OMX_PARAM_PORTDEFINITIONTYPE paramPortDefinition;
				paramPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
				getOmxIlSpecVersion(&paramPortDefinition.nVersion);
				mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&paramPortDefinition);
				p3AComponentManager->setMode(SW3A_MODE_SET_VIDEO,0);	
				
				if(OMX_TRUE == paramPortDefinition.bEnabled)
				{
					MSG0("Starting Hidden BMS");
					pGrabControl->setStartHiddenBMS(1,0);
				}				
				StillinVideo=0;
				pDeferredEventMgr->onlyDequeuePriorEvents(false); 
				MSG0("Pipe has started streaming again.");	
				PERF_TRACE_LAT("Pipe ISP_STREAMING(Restoration).",tempo1);
				SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
				return 0;
				}
			return 0;
   		default: break;	
	}
	return SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
}

