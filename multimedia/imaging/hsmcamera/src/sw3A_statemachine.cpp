/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsmcam.h"
#include "MMIO_Camera.h"    // Gamma LUT programming
#include "pictor.h"         // Gamma LUT programming
#include "ImgConfig.h"
#include "IFM_Trace.h"
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#include "camera.h"
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_sw3A_statemachineTraces.h"
#endif
#ifndef GAMMA_FLAG
#define GAMMA_FLAG
#endif
/* */
/* CAM_SM::SW3A_Control
 */
/**/
SCF_STATE CAM_SM::SW3A_Control(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:   ENTRY;  return 0;
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        case SCF_STATE_INIT_SIG:
        {
            MSG0("SW3A_Control-SCF_STATE_INIT\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Control-SCF_STATE_INIT", (&mENSComponent));
            //if(OMX_FALSE == pOpModeMgr->IsTimeNudgeEnabled())
                // Configure the flash driver if a new config is available.
                // Pre-condition: any pre-capture flash sequence is complete.
                if (p3AComponentManager->mFlashController.isDirectiveNewAClear() == true) {
                    t_sw3A_FlashDirective flashDirective = p3AComponentManager->mFlashController.getDirective();
                    cam_flash_err_e err = p3AComponentManager->mFlashController.configure(&flashDirective);
                    if (CAM_FLASH_ERR_NONE != err) {
                    DBGT_ERROR("Error: Flash configuration failed.\n");
                    OstTraceFiltStatic0(TRACE_ERROR, "Flash configuration failed", (&mENSComponent));
                   // DBC_ASSERT(0);
                }
            }
            SCF_INIT(&CAM_SM::SW3A_StartGrab);
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_Control-Q_DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Control-Q_DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}


SCF_STATE CAM_SM::SW3A_StartGrab(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            MSG0("SW3A_StartGrab-SCF_STATE_ENTRY_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_StartGrab-SCF_STATE_ENTRY_SIG", (&mENSComponent));
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("SW3A_ControlComputed-Q_PONG_SIG\n");
            if (p3AComponentManager->bStillSynchro_GrabAllowed
                    && (! p3AComponentManager->bStillSynchro_GrabStarted)) {
            	MSG0("SW3A_StartGrab - Main Flash\n");
            	p3AComponentManager->bStillSynchro_GrabStarted = true;
            	if(OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled())
                {
                        //Inform grabctl to discard all hidden capture buffers
                        GrabAbortStatus = GRAB_ABORT_REQUESTED;
                        pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT1);
                        p3AComponentManager->whatNextAfterRendezvous= SCF_STATE_PTR(&CAM_SM::TimeNudge_StartCapture);
                        SM_PUSH_STATE_STATIC(&CAM_SM::SW3A_ControlComputed);
                        SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                }
                else
                {
                    SM_PUSH_STATE_STATIC(&CAM_SM::SW3A_ControlComputed);
                    SCF_TRANSIT_TO(&CAM_SM::EnteringRawCapture_SendBuffers);
                }
             }
             else {
                 SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlComputed);
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}	


SCF_STATE CAM_SM::SW3A_ControlComputed(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("SW3A_ControlComputed-SCF_STATE_ENTRY_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlComputed-SCF_STATE_ENTRY_SIG", (&mENSComponent));

            bool bSomePEWritesWereQueued = false;

            OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pCaptureParameters = p3AComponentManager->getCaptureParameters(true);

#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
            if (pCaptureParameters != NULL) {   // New parameters have been received from 3A. Compute and apply new pipe settings.
                // Update the Gridiron live settings
                float fLiveCast = 0;
                if( pCaptureParameters->nColorTemperature != -1) {
                    // Convert colour temperature from Kelvin to Mired: M=(10^6)/T
                    fLiveCast = 1000000.0 / (float)(pCaptureParameters->nColorTemperature);
                }
                pIspctlCom->queuePE(GridironControl_f_LiveCast_Byte0, fLiveCast);

                // Picture settings related PEs
                t_sint16 iLumaOffset = pPictureSettings->iBrightness.ComputeLumaOffsetPE(pCaptureParameters->nBrightness);
                pIspctlCom->queuePE( CE_LumaOffset_0_s16_LumaOffset_Byte0, iLumaOffset);
                pIspctlCom->queuePE( CE_LumaOffset_1_s16_LumaOffset_Byte0, iLumaOffset);
                MSG2("[CAMERA] Brightness: Omx=%lu, LumaOffsetPE =%d\n", pCaptureParameters->nBrightness, iLumaOffset);
                OstTraceFiltStatic2(TRACE_DEBUG, "[CAMERA] Brightness: Omx=%lu, LumaOffsetPE =%d", (&mENSComponent), pCaptureParameters->nBrightness, iLumaOffset);

                t_uint8 iColourSaturation = pPictureSettings->iSaturation.ComputeColourSaturationPE(pCaptureParameters->nSaturation);
                pIspctlCom->queuePE( CE_YUVCoderControls_0_u8_ColourSaturation_Byte0,(t_uint32)iColourSaturation);
                pIspctlCom->queuePE( CE_YUVCoderControls_1_u8_ColourSaturation_Byte0, (t_uint32)iColourSaturation);
                MSG2("[CAMERA] Saturation: Omx=%ld, ColourSaturationPE=%d\n", pCaptureParameters->nSaturation, iColourSaturation);
                OstTraceFiltStatic2(TRACE_DEBUG, "[CAMERA] Saturation: Omx=%ld, ColourSaturationPE=%d", (&mENSComponent), pCaptureParameters->nSaturation, iColourSaturation);

                t_uint8 iContrast = pPictureSettings->iContrast.ComputeContrastPE(pCaptureParameters->nContrast);
                pIspctlCom->queuePE( CE_YUVCoderControls_0_u8_Contrast_Byte0, (t_uint32)iContrast);
                pIspctlCom->queuePE( CE_YUVCoderControls_1_u8_Contrast_Byte0, (t_uint32)iContrast);
                MSG2("[CAMERA] Contrast: Omx=%ld, ContrastPE=%d\n", pCaptureParameters->nContrast, iContrast);
                OstTraceFiltStatic2(TRACE_DEBUG, "[CAMERA] Contrast: Omx=%ld, ContrastPE=%d", (&mENSComponent), pCaptureParameters->nContrast, iContrast);

                bSomePEWritesWereQueued = true;
            }

            // Update all damped ISP live settings
            const IFM_DAMPERS_DATA* pDampersData = p3AComponentManager->getDampersData(true);
            if ((pDampersData != NULL ) && (pCaptureParameters != NULL)) {
                t_isp_dampers_error_code dErr = ISP_DAMPERS_OK;
                pIspDampers->SetSharpnessOmxSetting(pCaptureParameters->nSharpness);
                dErr = pIspDampers->ComputeAndQueuePEs( pIspctlCom, pDampersData);
                if(dErr!=ISP_DAMPERS_OK) {
                    DBGT_ERROR("Failed to compute ISP damped PEs: err=%d (%s)\n", dErr, CIspDampers::ErrorCode2String(dErr));
                    OstTraceFiltStatic1(TRACE_ERROR, "Failed to compute ISP damped PEs: err=%d", (&mENSComponent), dErr);
                    DBC_ASSERT(0);
                    return 0;
                }
                bSomePEWritesWereQueued = true;
            }
#endif //CAMERA_ENABLE_OMX_3A_EXTENSION

            if (bSomePEWritesWereQueued) {
                // Note: here we need to toggle the system coin so that the above settings are absorbed
                // But this is already handled further by SW3A for its own needs
                pIspctlCom->processQueue();
            } else {    // No PE to write, so self-trig a signal.
                pResourceSharerManager->mRSPing.Ping(0);
            }
            return 0;
        }
        case Q_PONG_SIG:
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_ControlComputed-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlComputed-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
            if (p3AComponentManager->bReachedLastOfListGroup) {
                SCF_PSTATE returnState = NULL;
                IMG_TIME_LOG((pPeList->flag == SW3A_LIST_FLAG_LAST_AEW) ? IMG_TIME_3A_STATS_AEW_REQUESTED : IMG_TIME_3A_STATS_AF_REQUESTED);
                MSG0("Processed the last-of-group PE list.\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "Processed the last-of-group PE list.", (&mENSComponent));
                p3AComponentManager->bReachedLastOfListGroup = false;
                // in still usecase
                // if buffer has already been received
                // we need to exit rawcapture
#ifdef CAM_STILL_SYNCHRO_DBG
                if(p3AComponentManager->isStillModeOn()) {
                    printf("CAM_STILL_SYNCHRO_DBG: CAM_SM::SW3A_ControlComputed hasStats=%d hasBuffer=%d.\n", p3AComponentManager->bStillSynchro_StatsReceived, stillSynchro_BufferReceived);
                }
#endif /*CAM_STILL_SYNCHRO_DBG */

                if(p3AComponentManager->bStillSynchro_StatsReceived) {
                    stillSynchro_StatsReceived = true;
                    p3AComponentManager->bStillSynchro_StatsReceived = false;
                    if (stillSynchro_BufferReceived) {
                        MSG0("Still synchro: buffer and stats received.\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "Still synchro: buffer and stats received.", (&mENSComponent));
                        returnState = SCF_STATE_PTR(&CAM_SM::FillBufferDoneVPB1_AddExtradata);
                    } else {
                        MSG0("Still synchro: stats received, waiting for buffer.\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "Still synchro: stats received, waiting for buffer.", (&mENSComponent));
                    }
                } else {
                    MSG0("Still synchro: The received stat is not the Still one yet.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "Still synchro: The received stat is not the Still one yet.", (&mENSComponent));
                }

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraCaptureParams = p3AComponentManager->getSteExtraCaptureParameters(false);
                bool bGammaLutChanged = FALSE;
                if (pSteExtraCaptureParams) {
                     bGammaLutChanged= pSteExtraCaptureParams->bValidGamma;
                }
                bool bLensgridChanged = ((Sw3AProcessingCompExt*)p3AComponentManager)->bValidLsc;
                bool bLinearizerChanged = ((Sw3AProcessingCompExt*)p3AComponentManager)->bValidLinearizer;
                if ((bGammaLutChanged) || (bLensgridChanged) || (bLinearizerChanged)) {
                    pSteExtraCaptureParams->bValidGamma = OMX_FALSE;
                    ((Sw3AProcessingCompExt*)p3AComponentManager)->bValidLsc = OMX_FALSE;
                    ((Sw3AProcessingCompExt*)p3AComponentManager)->bValidLinearizer = OMX_FALSE;
                    returnState = SCF_STATE_PTR(&CAM_SM::SW3A_ControlGammaLUT);
                }
#else
                // Check for Gamma LUT update need. NB: This should be ignored in still capture mode, because it is delegated to the ispproc component.
                OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraCaptureParams = p3AComponentManager->getSteExtraCaptureParameters(false);
                if (pSteExtraCaptureParams != NULL) {
                    MSG1("i AM hERE : bValidGamma: %d.\n",pSteExtraCaptureParams->bValidGamma);
                    if ((OMX_TRUE == pSteExtraCaptureParams->bValidGamma) && (! p3AComponentManager->isStillModeOn())) {
                        pSteExtraCaptureParams->bValidGamma = OMX_FALSE;
                        returnState = SCF_STATE_PTR(&CAM_SM::SW3A_ControlGammaLUT);
                    }
                }
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

                //  Transit to any defined Next state instead of ProcessPendingEvents, if no more sw3A state is to be entered.
                if (returnState == NULL) {
                    if (p3AComponentManager->whatNextAfterStart != NULL) {
                        returnState = p3AComponentManager->whatNextAfterStart;
                        p3AComponentManager->whatNextAfterStart = NULL;   // To avoid any confusion.
                    } else {
                        returnState = SCF_STATE_PTR(&CAM_SM::ProcessPendingEvents);
                    }
                }
                SCF_TRANSIT_TO_PSTATE(returnState);
            } else { // Write the remaining PE lists in group to FW.
                Sw3APeList_t *pPeList = NULL;
                pPeList = p3AComponentManager->peListQueue.pop();
                if (NULL != pPeList) { // Write next PE list.
                    if ((pPeList->flag == SW3A_LIST_FLAG_LAST_AF) || (pPeList->flag == SW3A_LIST_FLAG_LAST_AEW)) {
                        MSG0("Proceeding with a last-of-group PE list.\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "Processing with a last-of-group PE list.", (&mENSComponent));
                        p3AComponentManager->bReachedLastOfListGroup = true;
                    }
                    for (unsigned int i = 0; i < pPeList->list.size; i++) {
                        pIspctlCom->queuePE(pPeList->list.a_list[i].pe_addr, pPeList->list.a_list[i].pe_data);
                    }
                    if (pPeList->list.size == 0) { // Fill with at least one entry
                        pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);
                    }
                    p3AComponentManager->peListQueue.clear(pPeList);
                    pIspctlCom->processQueue();
                }
                else { // No more PE list.
                    MSG0("A committed PE lists group is not properly closed by a SW3A_LIST_FLAG_LAST.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "A committed PE lists group is not properly closed by a SW3A_LIST_FLAG_LAST.", (&mENSComponent));
                    DBC_ASSERT(0);
                }
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}	


SCF_STATE CAM_SM::SW3A_ControlGammaLUT(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {   // Got new gamma LUT.
            ENTRY;
            MSG0("SW3A_ControlGammaLUT-ENTRY: Got new gamma LUT to write to the ISP.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT-ENTRY: Got new gamma LUT to write to the ISP.", (&mENSComponent));
            // Check whether the pipe should be stopped.
            pIspctlCom->queuePE(HostInterface_Status_e_HostInterfaceLowLevelState_Current_Byte0, 0);
            pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, 0);
			pIspctlCom->readQueue();
            return 0;
        }
		case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT-EVT_ISPCTL_INFO\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT-EVT_ISPCTL_INFO", (&mENSComponent));

            if (e->type.ispctlInfo.info_id == ISP_READLIST_DONE)  {
				Configured_RxAbort_OnStop =(Flag_te) e->type.ispctlInfo.Listvalue[1].pe_data;
                mIspPrivateData.ISPLowLevelStatus = (t_uint32)e->type.ispctlInfo.value;
                switch (e->type.ispctlInfo.Listvalue[0].pe_data) {
                    case HostInterfaceLowLevelState_e_RUNNING:  // The pipe must be stopped before programming the LUT.
#ifdef GAMMA_FLAG
                        p3AComponentManager->bHasToStopBeforeGammaChange = false;
                        SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Write);
#else
                        p3AComponentManager->bHasToStopBeforeGammaChange = true;
                        SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Stop);
#endif
                        break;
                    case HostInterfaceLowLevelState_e_SLEPT:  // The pipe must be woken up before programming the LUT.
                        p3AComponentManager->bHasToStopBeforeGammaChange = false;
                        SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_WakeUp);
                        break;
                    case HostInterfaceLowLevelState_e_STOPPED:  // The pipe is already stopped; Can continue.
                    default:
                        p3AComponentManager->bHasToStopBeforeGammaChange = false;
                        SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Write);
                        break;
                }
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}


SCF_STATE CAM_SM::SW3A_ControlGammaLUT_WakeUp(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {   // wake up the pipe.
            ENTRY;
            MSG0("SW3A_ControlGammaLUT_WakeUp-ENTRY: Wake up the pipe.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp-ENTRY: Wake up the pipe.", (&mENSComponent));
            pIspctlCom->writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_WAKEUP);
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_WakeUp-EVT_ISPCTL_INFO\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp-EVT_ISPCTL_INFO", (&mENSComponent));
            if (ISP_READ_DONE == e->type.ispctlInfo.info_id) {
                PowerCommand_te ecommand = (PowerCommand_te)e->type.ispctlInfo.value;
                Flag_te status = Flag_e_FALSE;
                switch (ecommand)
                {
                    case PowerCommand_e_voltage_on:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_voltage_on\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_voltage_on", (&mENSComponent));
                        if(MMIO_Camera::powerSensor(OMX_TRUE))
                        {
                            DBGT_ERROR("ERROR in MMIO_Camera::powerSensor !\n");
                            OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::powerSensor !", (&mENSComponent));
                        }
                        else status = Flag_e_TRUE;
                        break;
                    case PowerCommand_e_voltage_off:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_voltage_off\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_voltage_off", (&mENSComponent));
                        if(MMIO_Camera::powerSensor(OMX_FALSE))
                        {
                            DBGT_ERROR("ERROR in MMIO_Camera::powerSensor !\n");
                            OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::powerSensor !", (&mENSComponent));
                        }
                        else status = Flag_e_TRUE;
                        break;
                    case PowerCommand_e_ext_clk_on:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_external_clock_on\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_external_clock_on", (&mENSComponent));
                        if(MMIO_Camera::setExtClk(OMX_TRUE))
                        {
                            DBGT_ERROR("ERROR in MMIO_Camera::setExtClk !\n");
                            OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk !", (&mENSComponent));
                        }
                        else status = Flag_e_TRUE; 
                        break;
                    case PowerCommand_e_ext_clk_off:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_external_clock_off\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_external_clock_off", (&mENSComponent));
                        if(MMIO_Camera::setExtClk(OMX_FALSE))
                        {
                            DBGT_ERROR("ERROR in MMIO_Camera::setExtClk !\n");
                            OstTraceFiltStatic0(TRACE_ERROR, "ERROR in MMIO_Camera::setExtClk !", (&mENSComponent));
                        }
                        else status = Flag_e_TRUE;
                        break;
                    case PowerCommand_e_x_shutdown_on:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_external_shutdown_on\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_external_shutdown_on", (&mENSComponent));
                        break;
                    case PowerCommand_e_x_shutdown_off:
                        MSG0("SW3A_ControlGammaLUT_WakeUp command_e_external_shutdown_off\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_WakeUp command_e_external_shutdown_off", (&mENSComponent));
                        break;
                    default:
                        break;
                }
                pIspctlCom->queuePE(SensorPowerManagement_Control_e_Flag_Result_Byte0,(t_uint32)status);
                pIspctlCom->queuePE(SensorPowerManagement_Control_e_PowerAction_Byte0,(t_uint32)PowerAction_e_complete);
                pIspctlCom->processQueue();
            }
            else if (ISP_POWER_NOTIFICATION == e->type.ispctlInfo.info_id) {
                pIspctlCom->requestPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
            }
            else if (ISP_WOKEN_UP == e->type.ispctlInfo.info_id) {
                SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Write);
            }
            return 0;
        }
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            return 0;
        }
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
    }
	return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}


SCF_STATE CAM_SM::SW3A_ControlGammaLUT_Stop(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {   // Stop the pipe.
            ENTRY;
            MSG0("SW3A_ControlGammaLUT_Stop-ENTRY: Stop the pipe.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Stop-ENTRY: Stop the pipe.", (&mENSComponent));
            MSG0("Disabling abort-rx functionality before gamma correction\n");
            pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, (t_uint32)Flag_e_FALSE);
            pIspctlCom->queuePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, (t_uint32)HostInterfaceCommand_e_STOP);
            pIspctlCom->processQueue();
            return 0;
        }
        case EVT_ISPCTL_LIST_INFO_SIG:
        {   // (HW block disable and pipe stop commands) OR (HW block enable and pipe start commands) acknowledged.
            MSG0("SW3A_ControlGammaLUT_Stop-EVT_ISPCTL_LIST_INFO\n");
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_Stop-EVT_ISPCTL_INFO\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Stop-EVT_ISPCTL_INFO", (&mENSComponent));
            switch(e->type.ispctlListInfo.info_id) {
                //case ISP_STOP_STREAMING:
                case ISP_SENSOR_STOP_STREAMING:
                    SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Write);
                    break;
                default: break;
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
	return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}


SCF_STATE CAM_SM::SW3A_ControlGammaLUT_Start(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {   // Start the pipe.
            ENTRY;
            MSG0("SW3A_ControlGammaLUT_Start-ENTRY: Start the pipe.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Start-ENTRY: Start the pipe.", (&mENSComponent));
            pIspctlCom->queuePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, (t_uint32)Configured_RxAbort_OnStop);
            pIspctlCom->queuePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, (t_uint32)HostInterfaceCommand_e_RUN);
            pIspctlCom->processQueue();
            return 0;
        }
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_Start-EVT_ISPCTL_LIST_INFO\n");
            return 0;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_Start-EVT_ISPCTL_INFO\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Start-EVT_ISPCTL_INFO", (&mENSComponent));
            if(e->type.ispctlListInfo.info_id == ISP_STREAMING) {
                if (p3AComponentManager->whatNextAfterStart == NULL) {
                    SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                } else {
                    MSG0("Jumping out of SW3A state machine.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "Jumping out of SW3A state machine.", (&mENSComponent));
                    SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStart);
                    p3AComponentManager->whatNextAfterStart = NULL;   //  To avoid any confusion.
                }
            } else {
                MSG1("Ignoring out of context info_id=%d\n", e->type.ispctlListInfo.info_id);
                OstTraceFiltStatic1(TRACE_DEBUG, "Ignoring out of context info_id=%d", (&mENSComponent), e->type.ispctlListInfo.info_id);
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
	return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}

#ifdef GAMMA_FLAG
SCF_STATE CAM_SM::SW3A_ControlGammaLUT_Write(s_scf_event const *e)
{
MSG1("SW3A_ControlGammaLUT_Write-ENTRY: %d.\n",e->sig);
     switch (e->sig) {
        case SCF_PARENT_SIG:break;
        case SCF_STATE_ENTRY_SIG:
        {
	 ENTRY;
            MSG0("SW3A_ControlGammaLUT_Write-ENTRY: program it.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write-ENTRY: program it.", (&mENSComponent));
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
	break;
        case Q_PONG_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_Write-Q_PONG_SIG: Get Coin.\n");
            p3AComponentManager->bLUTWritten = false;
            pIspctlCom->queuePE(CE_GammaStatus_0_e_Coin_Status_Byte0, (t_uint32)0);
            pIspctlCom->queuePE(CE_GammaStatus_1_e_Coin_Status_Byte0, (t_uint32)0);
            pIspctlCom->queuePE(CE_GammaStatus_0_e_GammaCurve_Byte0, (t_uint32)0);
            pIspctlCom->queuePE(CE_GammaStatus_1_e_GammaCurve_Byte0, (t_uint32)0);
            pIspctlCom->readQueue();
            return 0;
         }
        break;
	case EVT_ISPCTL_LIST_INFO_SIG:
	{
               MSG0("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG\n");
              if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)  {
                t_uint32 Gamma_HR_Coin = 0;
                t_uint32 Gamma_LR_Coin = 0;

              Gamma_HR_Coin =  (t_uint32)e->type.ispctlInfo.Listvalue[0].pe_data;
              Gamma_LR_Coin =  (t_uint32)e->type.ispctlInfo.Listvalue[1].pe_data;
               MSG2("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG:Got Coin Status: LR: %lu; HR: %lu\n",Gamma_LR_Coin,Gamma_HR_Coin);
              if (p3AComponentManager->bLUTWritten) {
                if (p3AComponentManager->whatNextAfterStart == NULL) {
                MSG0("SW3A_ControlGammaLUT_Write:Q_PONG_SIG: Going to Process Pending state machine.\n");
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }
            else {
                    MSG0("Jumping out of SW3A state machine.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "Jumping out of SW3A state machine.", (&mENSComponent));
                    SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStart);
                    p3AComponentManager->whatNextAfterStart = NULL;   //  To avoid any confusion.
                }
                }
            else
                {
                    p3AComponentManager->bLUTWritten = true;

                    // Program the LUT and enable it. The LUT is a word array.
                    MSG0("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG: Program new LUT and enable it.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG: Program new LUT and enable it.", (&mENSComponent));
                    OMX_ERRORTYPE e_ret = OMX_ErrorNone;
                    bool bHasErr = false;

                    OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraData = p3AComponentManager->getSteExtraCaptureParameters(false);
                    if (NULL == pSteExtraData) {
                        DBGT_ERROR("SW3A_ControlGammaLUT_Write- ERROR can not getSteExtraCaptureParameters\n");
                        OstTraceFiltStatic0(TRACE_ERROR, "SW3A_ControlGammaLUT_Write- ERROR can not getSteExtraCaptureParameters ", (&mENSComponent));
                        DBC_ASSERT(0);
                        return 0;
                    }
              long * lutAddr;
              int lutCount;
              t_uint16 u16_last;

              // Program the sharpened and unsharpened LUT memory of all color channels.
              /* Write Red Gamma Lut = According to PictorBug #111096, Red PEs should be programmed by values corresponding to Green channel */
             lutAddr = (long *)&(pSteExtraData->nGreenGammaLUT[0]);
             lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1;
             u16_last = pSteExtraData->nGreenGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_SharpRedLutAddress_Byte0,(t_uint32)pGammaSharpRed_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_UnSharpRedLutAddress_Byte0,(t_uint32)pGammaUnSharpRed_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_SharpRedLutAddress_Byte0,(t_uint32)pGammaSharpRed_LR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_UnSharpRedLutAddress_Byte0,(t_uint32)pGammaUnSharpRed_LR->ispLogicalAddress);
            memcpy(pGammaSharpRed_LR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
            memcpy(pGammaUnSharpRed_LR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
            memcpy(pGammaSharpRed_HR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
            memcpy(pGammaUnSharpRed_HR->armLogicalAddress,lutAddr,lutCount*sizeof(long));

                    /* Write Green Gamma Lut = According to PictorBug #111096, Green PEs should be programmed by values corresponding to Blue channel*/
              lutAddr = (long *)&(pSteExtraData->nBlueGammaLUT[0]);
              u16_last = pSteExtraData->nBlueGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
              pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_SharpGreenLutAddress_Byte0,(t_uint32)pGammaSharpGreen_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_UnSharpGreenLutAddress_Byte0,(t_uint32)pGammaUnSharpGreen_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_SharpGreenLutAddress_Byte0,(t_uint32)pGammaSharpGreen_LR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_UnSharpGreenLutAddress_Byte0,(t_uint32)pGammaUnSharpGreen_LR->ispLogicalAddress);
             memcpy(pGammaSharpGreen_LR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
             memcpy(pGammaUnSharpGreen_LR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
             memcpy(pGammaSharpGreen_HR->armLogicalAddress,lutAddr,lutCount*sizeof(long));
             memcpy(pGammaUnSharpGreen_HR->armLogicalAddress,lutAddr,lutCount*sizeof(long));

                  /* Write Blue Gamma Lut = According to PictorBug #111096, Blue PEs should be programmed by values corresponding to Red channel*/
              lutAddr = (long *)&(pSteExtraData->nRedGammaLUT[0]);
              u16_last = pSteExtraData->nRedGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_SharpBlueLutAddress_Byte0,(t_uint32)pGammaSharpBlue_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_0_u32_UnSharpBlueLutAddress_Byte0,(t_uint32)pGammaUnSharpBlue_HR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_SharpBlueLutAddress_Byte0,(t_uint32)pGammaSharpBlue_LR->ispLogicalAddress);
             pIspctlCom->queuePE(CE_GammaMemLutAddress_1_u32_UnSharpBlueLutAddress_Byte0,(t_uint32)pGammaUnSharpBlue_LR->ispLogicalAddress);
             memcpy(pGammaSharpBlue_LR->armLogicalAddress, lutAddr,lutCount*sizeof(long));
             memcpy(pGammaUnSharpBlue_LR->armLogicalAddress, lutAddr, lutCount*sizeof(long));
             memcpy(pGammaSharpBlue_HR->armLogicalAddress, lutAddr, lutCount*sizeof(long));
             memcpy(pGammaUnSharpBlue_HR->armLogicalAddress, lutAddr, lutCount*sizeof(long));

             pIspctlCom->queuePE(CE_GammaControl_0_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
             pIspctlCom->queuePE(CE_GammaControl_1_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
             pIspctlCom->queuePE(CE_GammaControl_0_e_Coin_Ctrl_Byte0, (t_uint32)((Gamma_HR_Coin+1)&0x01));
             pIspctlCom->queuePE(CE_GammaControl_1_e_Coin_Ctrl_Byte0, (t_uint32)((Gamma_LR_Coin+1)&0x01));
            pIspctlCom->processQueue();
            MSG2("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG:Got Coin Status: LR: %lu; HR: %lu\n",(Gamma_LR_Coin+1)&0x01,(Gamma_HR_Coin +1)&0x01);
            MSG2("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG:Got Curve Status: LR: %lu; HR: %lu\n",e->type.ispctlInfo.Listvalue[2].pe_data,e->type.ispctlInfo.Listvalue[3].pe_data);
                }
            return 0;
              }
		else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id){
		MSG0("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG:Write Done \n");
               if (p3AComponentManager->bLUTWritten) {
                if (p3AComponentManager->whatNextAfterStart == NULL) {
                MSG0("SW3A_ControlGammaLUT_Write:ISP_WRITELIST_DONE: Going to Process Pending state machine.\n");
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }
            else {
                    MSG0("Jumping out of SW3A state machine2.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "Jumping out of SW3A state machine.", (&mENSComponent));
                    SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStart);
                    p3AComponentManager->whatNextAfterStart = NULL;   //  To avoid any confusion.
                }
                }
               else
                {
                    MSG0("CAM_SM SW3A_ControlGammaLUT_Write:Gamma Something Wrong!!");
                }
		}
		return 0;
       }
       return 0;
       case SCF_STATE_EXIT_SIG: EXIT; return 0;
         default:
              MSG1("SW3A_ControlGammaLUT_Write:Ignoring out of context e->sig=%d\n", e->sig);
              OstTraceFiltStatic1(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write:Ignoring out of context info_id=%d", (&mENSComponent), e->type.ispctlListInfo.info_id);
              break;
    }
         return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}

#else
SCF_STATE CAM_SM::SW3A_ControlGammaLUT_Write(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {   // Pre-condition: pipe is stopped. Disable HW block and program it.
            ENTRY;
            MSG0("SW3A_ControlGammaLUT_Write-ENTRY: Disable HW block and program it.\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write-ENTRY: Disable HW block and program it.", (&mENSComponent));
            p3AComponentManager->bLUTWritten = false;
            pIspctlCom->queuePE(CE_GammaControl_0_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Disable);
            pIspctlCom->queuePE(CE_GammaControl_1_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Disable);
            pIspctlCom->processQueue();
            return 0;
        }
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_ControlGammaLUT_Write-SW3A_ControlGammaLUT_Write\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write-SW3A_ControlGammaLUT_Write", (&mENSComponent));
            if(e->type.ispctlListInfo.info_id == ISP_WRITELIST_DONE) {
                if (p3AComponentManager->bLUTWritten) {
                    // Gamma already programmed. Can go to next state.
                    if (p3AComponentManager->bHasToStopBeforeGammaChange) {
                        SCF_TRANSIT_TO(&CAM_SM::SW3A_ControlGammaLUT_Start);
                    } else {
                        DBC_ASSERT(p3AComponentManager->whatNextAfterStart);
                        MSG0("Jumping out of SW3A state machine.\n");
                        OstTraceFiltStatic0(TRACE_DEBUG, "Jumping out of SW3A state machine.", (&mENSComponent));
                        SCF_TRANSIT_TO_PSTATE(p3AComponentManager->whatNextAfterStart);
                        p3AComponentManager->whatNextAfterStart = NULL;   // To avoid any confusion.
                    }
                }
                else {
                    // Program gamma.
                    p3AComponentManager->bLUTWritten = true;

                    // Program the LUT and enable it. The LUT is a word array.
                    MSG0("SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG: Program new LUT and enable it.\n");
                    OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_ControlGammaLUT_Write-EVT_ISPCTL_LIST_INFO_SIG: Program new LUT and enable it.", (&mENSComponent));
                    OMX_ERRORTYPE e_ret = OMX_ErrorNone;
                    bool bHasErr = false;

                    OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraData = p3AComponentManager->getSteExtraCaptureParameters(false);
                    if (NULL == pSteExtraData) {
                        DBGT_ERROR("SW3A_ControlGammaLUT_Write- ERROR can not getSteExtraCaptureParameters\n");
                        OstTraceFiltStatic0(TRACE_ERROR, "SW3A_ControlGammaLUT_Write- ERROR can not getSteExtraCaptureParameters ", (&mENSComponent));
                        DBC_ASSERT(0);
                        return 0;
                    }
                    long * lutAddr;
                    int lutCount;
                    t_uint16 u16_last;
              
                    // Program the sharpened and unsharpened LUT memory of all color channels.
                    // Program the LUT last elements, which do not fit into the memory.
                    /* Write Red Gamma Lut = According to PictorBug #111096, Red PEs should be programmed by values corresponding to Green channel */
                        lutAddr = (long *)&(pSteExtraData->nGreenGammaLUT[0]); 
                        lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1;
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        u16_last = pSteExtraData->nGreenGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte0, (t_uint32)u16_last);
                    
                    /* Write Green Gamma Lut = According to PictorBug #111096, Green PEs should be programmed by values corresponding to Blue channel*/
                        lutAddr = (long *)&(pSteExtraData->nBlueGammaLUT[0]); 
                        lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1;
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        u16_last = pSteExtraData->nBlueGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte0, (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte0, (t_uint32)u16_last);
                    
                    /* Write Blue Gamma Lut = According to PictorBug #111096, Blue PEs should be programmed by values corresponding to Red channel*/
                        lutAddr = (long *)&(pSteExtraData->nRedGammaLUT[0]); 
                        lutCount = OMX_STE_GAMMA_LUT_SAMPLE_COUNT - 1;
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount); bHasErr |= (e_ret != OMX_ErrorNone);
                        u16_last = pSteExtraData->nRedGammaLUT[OMX_STE_GAMMA_LUT_SAMPLE_COUNT-1];
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte0,   (t_uint32)u16_last);
                        pIspctlCom->queuePE(CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte0, (t_uint32)u16_last);
                    
                    if (bHasErr) {
                        DBGT_ERROR("SW3A_ControlGammaLUT_Write- ERROR: could not write LUT to ISP memory.\n");
                        OstTraceFiltStatic0(TRACE_ERROR, "SW3A_ControlGammaLUT_Write- ERROR: could not write LUT to ISP memory.", (&mENSComponent));
                        DBC_ASSERT(e_ret);
                        return SCF_STATE_PTR(&CAM_SM::SW3A_ControlGammaLUT);
                    }
                    pIspctlCom->queuePE(CE_GammaControl_0_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
                    pIspctlCom->queuePE(CE_GammaControl_1_e_GammaCurve_Byte0, (t_uint32)GammaCurve_Custom);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                  ((Sw3AProcessingCompExt*)p3AComponentManager)->SetLscConfigtoIsp();
                  ((Sw3AProcessingCompExt*)p3AComponentManager)->SetLinearizerConfigtoIsp();
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
                    pIspctlCom->processQueue();
                }
            }
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Control);
}
#endif

SCF_STATE CAM_SM::SW3A_GetFwStatus(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            if (p3AComponentManager->fwStatus.size == 0) { // Fill with at least one entry
                pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);
            }
            for (unsigned int i=0; i < p3AComponentManager->fwStatus.size; i++) {
                pIspctlCom->queuePE(p3AComponentManager->fwStatus.a_list[i].pe_addr, 0);
            }
            pIspctlCom->readQueue();
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_FwInitStatus-DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_FwInitStatus-DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_GetFwStatus-EVT_ISPCTL_LIST_INFO;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_GetFwStatus-EVT_ISPCTL_LIST_INFO;", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) { // This should not happen: we requested a list read, we must get a list read.
                DBC_ASSERT(0);
                break;
            }
            // Copy the status information to the sw3A proxy context.
            for (unsigned int i=0; i < p3AComponentManager->fwStatus.size; i++) {
                p3AComponentManager->fwStatus.a_list[i].pe_addr = e->type.ispctlInfo.Listvalue[i].pe_addr;
                p3AComponentManager->fwStatus.a_list[i].pe_data = e->type.ispctlInfo.Listvalue[i].pe_data;
            }

            SCF_TRANSIT_TO(&CAM_SM::SW3A_SetFwStatusOn);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return  SCF_STATE_PTR(&CAM_SM::SW3A_Start);
}


/* CAM_SM::SW3A_Start
 */
/**/
SCF_STATE CAM_SM::SW3A_Start(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:   ENTRY;  return 0;
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        case SCF_STATE_INIT_SIG:
        	MSG0("SW3A_Start-SCF_STATE_INIT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Start-SCF_STATE_INIT", (&mENSComponent));
        	SCF_INIT(&CAM_SM::SW3A_GetFwStatus);
        	return 0;
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_Start-Q_DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Start-Q_DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::SW3A_Starting(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("SW3A_Starting-Q_PONG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Starting-Q_PONG", (&mENSComponent));
            p3AComponentManager->start3A();
            return 0;
        }
        case SW3A_LOOP_CONTROL_SIG:
        {
            MSG0("SW3A_Starting-SW3A_LOOP_CONTROL_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Starting-SW3A_LOOP_CONTROL_SIG", (&mENSComponent));
            /* Depeding on a 3AComponentmanager status, enter the pregrab flashes state or continue to the 3A control part.
             *       This must be done before SW3A start controls are executed so that:
             *          1/ flash controls (from SW3A and from flash sequencer) do not collide with eachother; 
             *          2/ the last viewfinder stat is not altered by the flash sequencer */
            if (p3AComponentManager->isPreCaptureFlashNeeded()) {
                SCF_TRANSIT_TO(&CAM_SM::SW3A_RunPreCaptureFlashes);
            } else {
                SCF_TRANSIT_TO(&CAM_SM::SW3A_Control);
            }
            
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Start);
}

SCF_STATE CAM_SM::SW3A_SetFwStatusOn(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
                pIspctlCom->queuePE(SystemSetup_e_Flag_ZoomdStatsOnly_Byte0, 1);
                pIspctlCom->processQueue();
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_SetFwStatusOn-DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_SetFwStatusOn-DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_SetFwStatusOn-EVT_ISPCTL_LIST_INFO;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_SetFwStatusOn-EVT_ISPCTL_LIST_INFO;", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) { // This should not happen: we requested a list read, we must get a list read.
                DBC_ASSERT(0);
                break;
            }
            SCF_TRANSIT_TO(&CAM_SM::SW3A_Starting);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return  SCF_STATE_PTR(&CAM_SM::SW3A_Start);
}
SCF_STATE CAM_SM::SW3A_RunPreCaptureFlashes(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
        {
            MSG0("SW3A_RunPreGrabFlashes-Q_PONG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_RunPreGrabFlashes-Q_PONG", (&mENSComponent));
            p3AComponentManager->startPreCaptureFlashes();
            return 0;
        }
        case SW3A_PRECAPTURE_FLASH_END:
        {
            SCF_TRANSIT_TO(&CAM_SM::SW3A_Control);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Start);
}


/* CAM_SM::SW3A_Stop
 */
/**/
SCF_STATE CAM_SM::SW3A_Stop(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:   ENTRY;  return 0;
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        case SCF_STATE_INIT_SIG:
        	MSG0("SW3A_Stop-SCF_STATE_INIT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Stop-SCF_STATE_INIT", (&mENSComponent));
        	SCF_TRANSIT_TO(&CAM_SM::SW3A_SetFwStatusOff);
        	return 0;
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_Stop-Q_DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Stop-Q_DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::SW3A_Stopping(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            pResourceSharerManager->mRSPing.Ping(0);
            return 0;
        }
        case Q_PONG_SIG:
	{
		MSG0("SW3A_Stopping-Q_PONG\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_Stopping-Q_PONG", (&mENSComponent));

		/*For Shutterlag */
		MSG0("Setting  Sw3aStopReqStatus = SW3A_STOP_REQUESTED;\n");	
		Sw3aStopReqStatus = SW3A_STOP_REQUESTED;

		p3AComponentManager->stop3A();
		/* Need to go back to ProcessPendingEvent until all pending sw3A deferred events
		* are processed. The 3A component manager will send SW3A_STOPPED_SIG upon stop done
		* sw3A notification. The post-stop actions will then be executed from
		* ProcessPendingEvent. */
		SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
		return 0;
	}
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::SW3A_Stop);
}
SCF_STATE CAM_SM::SW3A_SetFwStatusOff(s_scf_event const *e)
{
   switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
                pIspctlCom->queuePE(SystemSetup_e_Flag_ZoomdStatsOnly_Byte0, 0);
                pIspctlCom->processQueue();
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:
        	MSG0("SW3A_SetFwStatusOff-DEFERRED_EVENT\n");
        	OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_SetFwStatusOff-DEFERRED_EVENT", (&mENSComponent));
        	return 0;
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SW3A_SetFwStatusOff-EVT_ISPCTL_LIST_INFO;\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SW3A_SetFwStatusOff-EVT_ISPCTL_LIST_INFO;", (&mENSComponent));
            if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) { // This should not happen: we requested a list read, we must get a list read.
                DBC_ASSERT(0);
                break;
            }
            SCF_TRANSIT_TO(&CAM_SM::SW3A_Stopping);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:    EXIT;   return 0;
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}


