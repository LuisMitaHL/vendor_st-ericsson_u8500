/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h" // rename_me sc_cam.h

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_isp_features_zoom_statemachineTraces.h"
#endif


SCF_STATE CAM_SM::Zooming(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
		case SCF_STATE_EXIT_SIG: EXIT; return 0;
		case SCF_STATE_INIT_SIG: INIT; SCF_INIT(&CAM_SM::ZoomingExecute); return 0;
		case Q_DEFERRED_EVENT_SIG:
			MSG0("Zooming-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Zooming-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::SetFeature);
}

SCF_STATE CAM_SM::ZoomingExecute(s_scf_event const *e)
{
	switch (e->sig)
	{
        case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* get command coin and MaxFOV */
			pIspctlCom->queuePE(Zoom_CommandStatus_u8_CommandCount_Byte0, 0);

            //if (0 == pZoom->maxFOVXAtCurrentCenter)
			{
				pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0, 0);
			}

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                        pIspctlCom->queuePE(Zoom_Status_f_FOVX_Byte0, 0);
#endif
			pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ZoomingExecute-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingExecute-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			static OMX_STATETYPE currentState;

			/* prepare zoom command */
			if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
				MSG0("ZoomingExecute- prepare zoom\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingExecute- prepare zoom", (&mENSComponent));

				OMX_ERRORTYPE error = mENSComponent.GetState(&mENSComponent, &currentState);
				if (OMX_ErrorNone != error)
				{
					SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
					return 0;
				}
				pZoom->changeoverIsNeeded = OMX_FALSE;
				pZoom->requestDenied = OMX_FALSE;
				pZoom->outOfRange = OMX_FALSE;

                /* store maximum FOV X available if needed */
				//if (0 == pZoom->maxFOVXAtCurrentCenter)
				{
					pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[1].pe_data);
				}
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                                /* Peformance tuning: Zoom should not be required if FOV and OffsetX and OffsetY are already applied */
                                float fCurrentFOV = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data);
                                if ((fCurrentFOV != pZoom->calculateFOV(pZoom->pCenterFOV)) ||
                                    (pZoom->pCenterFOVStatus->sPoint.nX != pZoom->pCenterFOV->sPoint.nX) ||
                                    (pZoom->pCenterFOVStatus->sPoint.nY != pZoom->pCenterFOV->sPoint.nY))
                                {
                                  pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, pZoom->calculateFOV(pZoom->pCenterFOV));
                                  pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, ZoomCommand_e_SetFOV);
                                  pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data+1);

                                  pIspctlCom->processQueue();
                                  MSG2("Apply zoom config %f, %f\n", fCurrentFOV, pZoom->calculateFOV(pZoom->pCenterFOV));
                                }
                                else
                                {
                                  MSG2("Don't need to apply zoom config %f, %f\n", fCurrentFOV, pZoom->calculateFOV(pZoom->pCenterFOV));
                                  pZoom->ZoomCmd = ZoomCommand_e_SetFOV;
                                  SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatus);
                                }
#else
				pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, pZoom->calculateFOV(&pZoom->CenterFOV));
				/*Even in case of pan-tilt we will set FOV*/
				pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, (t_uint32)ZoomCommand_e_SetFOV);
				pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, (t_uint32)e->type.ispctlInfo.Listvalue[0].pe_data+1);

				pIspctlCom->processQueue();
#endif
			}
			/* skip changeover linked unused notifications */
			else if ((ISP_SENSOR_START_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_STOP_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_SENSOR_STOP_STREAMING == e->type.ispctlInfo.info_id))
			{
				/* nothing to do */
			}
			/* nothing else to do if we XP70 is not running */
			else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				/*nothing to do*/
			}
			/* a sensor changeover is needed : zoom command will end with ISP_STREAMING notification */
			else if (ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED == e->type.ispctlInfo.info_id)
			{
				pZoom->changeoverIsNeeded = OMX_TRUE;
			}
			/* something went wrong, client must be warned */
			else if (ISP_ZOOM_CONFIG_REQUEST_DENIED == e->type.ispctlInfo.info_id)
			{
				pZoom->requestDenied = OMX_TRUE;
			}
			/* set center command out of range => command was moderated */
			else if (ISP_ZOOM_SET_OUT_OF_RANGE == e->type.ispctlInfo.info_id)
			{
				pZoom->outOfRange = OMX_TRUE;
			}
			/* wait for zoom command end */
			else if (((ISP_ZOOM_STEP_COMPLETE == e->type.ispctlInfo.info_id) && (OMX_FALSE == pZoom->changeoverIsNeeded))
				|| (ISP_STREAMING == e->type.ispctlInfo.info_id))
			{
				MSG0("ZoomingExecute- zoom complete\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingExecute- zoom complete", (&mENSComponent));
				if (OMX_TRUE == pZoom->requestDenied)
				{
					MSG0("ZoomingExecute. Request Denied\n");
					pZoom->ZoomCmd = ZoomCommand_e_None;
                    // Notify the IL client
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorUnsupportedSetting, (OMX_U32)OMX_Symbian_IndexConfigExtDigitalZoom);
					SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
				}
				else if (ZoomCommand_e_SetCenter == pZoom->ZoomCmd)
			    {
					SCF_TRANSIT_TO(&CAM_SM::PanTiltExecute);
				}/* zoom command has or could have been moderated : need to retrieve zoom status */
				else
				{
					COmxCamera * OMXCam = (COmxCamera*)&mENSComponent;
					if ((OMXCam->mZoomModeClbkReq.bEnable == OMX_TRUE) && (OMXCam->mZoomMode.bEnabled == OMX_TRUE) && (ZoomCommand_e_SetFOV == pZoom->ZoomCmd))
					{
						MSG1("ZoomingExecute. [smooth zoom] Sending Smooth Zoom callback event Event\n");
						OMX_STE_SMOOTH_ZOOM_RETURNTYPE smoothZoomEventData;
						if (pZoom->requestDenied || pZoom->outOfRange)
						{
							MSG1("ZoomingExecute. [smooth zoom] zoom was not successful.\n");
							smoothZoomEventData.smoothZoomError = OMX_ErrorUndefined;
						}
						else
						{
							smoothZoomEventData.smoothZoomError = OMX_ErrorNone;
						}

                        smoothZoomEventData.cCurrentZoomFactor = pZoom->ZoomFactor.xZoomFactor.nValue;
						MSG1("ZoomingExecute. [smooth zoom] smoothZoomEventData.cCurrentZoomFactor = %d\n", (int)smoothZoomEventData.cCurrentZoomFactor);
						MSG1("ZoomingExecute. [smooth zoom] smoothZoomEventData.smoothZoomError = %d\n", (int)smoothZoomEventData.smoothZoomError);
						OMXCam->eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, OMX_ALL,
							OMX_STE_IndexConfigCommonZoomMode, (OMX_PTR)&smoothZoomEventData);
					}
					SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatus);
				}
			}
			/* let the parent handle it */
			else
			{
				break;
			}

			return 0;
		}

		case SCF_STATE_EXIT_SIG:
			MSG0("ZoomingExecute-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingExecute-EXIT;", (&mENSComponent));
			return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Zooming);
}

SCF_STATE CAM_SM::PanTiltExecute(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* get command coin and MaxFOV */
			pIspctlCom->queuePE(Zoom_CommandStatus_u8_CommandCount_Byte0, 0);
			pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("PanTiltExecute-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PanTiltExecute-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));

			/* prepare zoom command */
			if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
				MSG0("PanTiltExecute- prepare zoom\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "PanTiltExecute- prepare zoom", (&mENSComponent));

				pZoom->changeoverIsNeeded = OMX_FALSE;
				pZoom->requestDenied = OMX_FALSE;
				pZoom->outOfRange = OMX_FALSE;
                
				pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0, pZoom->CenterFOV.sPoint.nX);
				pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0, pZoom->CenterFOV.sPoint.nY);
				pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, (t_uint32)pZoom->ZoomCmd);
				pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data+1);
				pIspctlCom->processQueue();
					
			}
			/* skip changeover linked unused notifications */
			else if ((ISP_SENSOR_START_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_STOP_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_SENSOR_STOP_STREAMING == e->type.ispctlInfo.info_id))
			{
				/* nothing to do */
			}
			/* nothing else to do if we XP70 is not running */
			else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				/*nothing to do*/
			}
			/* a sensor changeover is needed : zoom command will end with ISP_STREAMING notification */
			else if (ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED == e->type.ispctlInfo.info_id)
			{
				pZoom->changeoverIsNeeded = OMX_TRUE;
			}
			/* something went wrong, client must be warned */
			else if (ISP_ZOOM_CONFIG_REQUEST_DENIED == e->type.ispctlInfo.info_id)
			{
				pZoom->requestDenied = OMX_TRUE;
			}
			/* set center command out of range => command was moderated */
			else if (ISP_ZOOM_SET_OUT_OF_RANGE == e->type.ispctlInfo.info_id)
			{
				pZoom->outOfRange = OMX_TRUE;
			}
			/* wait for zoom command end */
			else if (((ISP_ZOOM_STEP_COMPLETE == e->type.ispctlInfo.info_id) && (OMX_FALSE == pZoom->changeoverIsNeeded))
				|| (ISP_STREAMING == e->type.ispctlInfo.info_id))
			{
				MSG0("PanTiltExecute- zoom complete\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "PanTiltExecute- zoom complete", (&mENSComponent));
				if (OMX_TRUE == pZoom->requestDenied)
				{
					pZoom->ZoomCmd = ZoomCommand_e_None;
                    // Notify the IL client
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorUnsupportedSetting, (OMX_U32)OMX_Symbian_IndexConfigExtDigitalZoom);
					SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
				}/* zoom command has or could have been moderated */
				/* need to retrieve zoom status */
				else
				{
					SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatus);
				}
			}
			/* let the parent handle it */
			else
			{
				break;
			}

			return 0;
		}

		case SCF_STATE_EXIT_SIG:
			MSG0("PanTiltExecute-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PanTiltExecute-EXIT;", (&mENSComponent));
			return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Zooming);
}

SCF_STATE CAM_SM::ZoomingGetStatus(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;

			if (ZoomCommand_e_SetFOV == pZoom->ZoomCmd)
			{
				pIspctlCom->queuePE(Zoom_Status_f_FOVX_Byte0, 0);
			}
			else if (ZoomCommand_e_SetCenter == pZoom->ZoomCmd)
			{
				pIspctlCom->queuePE(Zoom_Status_s16_CenterOffsetX_Byte0, 0);
				pIspctlCom->queuePE(Zoom_Status_s16_CenterOffsetY_Byte0, 0);
				pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0, 0);
			}
			pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ZoomingGetStatus-EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingGetStatus-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (ISP_READLIST_DONE != e->type.ispctlInfo.info_id) break;

			if (ZoomCommand_e_SetFOV == pZoom->ZoomCmd)
			{
				/* Rq : xZoomFactor is in Q16 format */
				pZoom->pZoomFactorStatus->xZoomFactor.nValue = pZoom->maxFOVXAtCurrentCenter * 0x10000 / CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[0].pe_data);
				MSG1("ZoomingGetStatus-zoom factor : %ld\n", pZoom->pZoomFactorStatus->xZoomFactor.nValue);
				OstTraceFiltStatic1(TRACE_DEBUG, "ZoomingGetStatus-zoom factor : %ld", (&mENSComponent), pZoom->pZoomFactorStatus->xZoomFactor.nValue);
			}
			else if (ZoomCommand_e_SetCenter == pZoom->ZoomCmd)
			{
				/*Currently Fw does not return the Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0 if center is shifted*/
				pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data);
				pZoom->pCenterFOVStatus->sPoint.nX = e->type.ispctlInfo.Listvalue[0].pe_data;
				pZoom->pCenterFOVStatus->sPoint.nY = e->type.ispctlInfo.Listvalue[1].pe_data;
				
				/* update zoom factor as maximum in case center is shifted*/
				pZoom->pZoomFactorStatus->xZoomFactor.nValue = 0x10000;
            
				MSG2("ZoomingGetStatus-center : %ld %ld\n", pZoom->pCenterFOVStatus->sPoint.nX, pZoom->pCenterFOVStatus->sPoint.nY);
				OstTraceFiltStatic2(TRACE_DEBUG, "ZoomingGetStatus-center : %ld %ld", (&mENSComponent), pZoom->pCenterFOVStatus->sPoint.nX, pZoom->pCenterFOVStatus->sPoint.nY);
			}

            p3AComponentManager->setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom, (OMX_PTR)(pZoom->pZoomFactorStatus));

			pZoom->ZoomCmd = ZoomCommand_e_None;
			SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
			return 0;
		}

		case SCF_STATE_EXIT_SIG: EXIT; return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Zooming);
}



SCF_STATE CAM_SM::ZoomingConfig(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
		case SCF_STATE_EXIT_SIG: EXIT; return 0;
		case SCF_STATE_INIT_SIG: INIT; SCF_INIT(&CAM_SM::ZoomingConfigExecute); return 0;
		case Q_DEFERRED_EVENT_SIG:
			MSG0("ZoomingConfig-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingConfig-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}


SCF_STATE CAM_SM::ZoomingConfigExecute(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ZoomingConfigExecute-EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingConfigExecute-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				pZoom->internalZoomConfig = internalZoomConfig_none;
                SCF_PSTATE next;
                SM_POP_STATE(next);
                SCF_TRANSIT_TO_PSTATE(next);
			}
			else if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
				/* go to max field of view available at current center, not hidden to user (port setting change or enter preview) */
				if (internalZoomConfig_resetFOV == pZoom->internalZoomConfig)
				{
					pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[0].pe_data);
                    
					/* port settings have changed => maxZoom has changed */
					float zoomRange = pZoom->getMaxZoomForUsecase(pSensor);
#ifndef CAMERA_ENABLE_OMX_3A_EXTENSION
					pZoom->pZoomFactorStatus->xZoomFactor.nValue = 0x10000;
#endif
                    
					pIspctlCom->queuePE(Zoom_Params_f_ZoomRange_Byte0, zoomRange);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
					pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0,pZoom->calculateFOV(&pZoom->CenterFOV));
#else
					pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0,0);
#endif
					//pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data);
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0, pZoom->pCenterFOVStatus->sPoint.nX);
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0, pZoom->pCenterFOVStatus->sPoint.nY);
				}
				/* go to FFOV, hidden to user (BMS grab) */
				else if (internalZoomConfig_resetFOVandCenter == pZoom->internalZoomConfig)
				{
					pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, 0);
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0, 0);
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0, 0);
				}
				/* go to previous FOV/center, hidden to user (back from BMS grab) */
				else if (internalZoomConfig_setCurrentFOVandCenter == pZoom->internalZoomConfig)
				{

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
					pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, 0);
#else
					/* Reset FOV in Video to Still transition */
					float FieldOfView = pZoom->maxFOVXAtCurrentCenter * (0x10000/(float)pZoom->pZoomFactorStatus->xZoomFactor.nValue);
					//pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, FieldOfView);
					pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, 0);
#endif
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0, pZoom->pCenterFOVStatus->sPoint.nX);
					pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0, pZoom->pCenterFOVStatus->sPoint.nY);

					float zoomRange = pZoom->getMaxZoomForUsecase(pSensor);
					pIspctlCom->queuePE(Zoom_Params_f_ZoomRange_Byte0, zoomRange);
				}
				pIspctlCom->processQueue();
			}
			else break;

			return 0;
		}

		case SCF_STATE_EXIT_SIG: EXIT; return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ZoomingConfig);
}


SCF_STATE CAM_SM::ZoomBeforeStateExecuting(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
		case SCF_STATE_EXIT_SIG: EXIT; return 0;
		case SCF_STATE_INIT_SIG: INIT; SCF_INIT(&CAM_SM::ExecuteZoomBeforeStateExecuting); return 0;
		case Q_DEFERRED_EVENT_SIG:
			MSG0("ZoomBeforeStateExecuting-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomBeforeStateExecuting-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::ExecuteZoomBeforeStateExecuting(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* get command coin and MaxFOV */
			pIspctlCom->queuePE(Zoom_CommandStatus_u8_CommandCount_Byte0, 0);
			pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0, 0);
			pIspctlCom->queuePE(Zoom_Status_f_MaxFOVYAvailableToDevice_Byte0,0);
			pIspctlCom->queuePE(Zoom_Status_f_MinFOVXAtArrayCenter_Byte0,0);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                        pIspctlCom->queuePE(Zoom_Status_f_FOVX_Byte0, 0);
#endif
			pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ExecuteZoomBeforeStateExecuting-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExecuteZoomBeforeStateExecuting-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			/* prepare zoom command */
			if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
				MSG0("ExecuteZoomBeforeStateExecuting- prepare zoom\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ExecuteZoomBeforeStateExecuting- prepare zoom", (&mENSComponent));

				pZoom->changeoverIsNeeded = OMX_FALSE;
				pZoom->requestDenied = OMX_FALSE;
				pZoom->outOfRange = OMX_FALSE;
                
				/* store maximum FOV X available if needed */
				//if (0 == pZoom->maxFOVXAtCurrentCenter)
				{
					pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[1].pe_data);
					pZoom->maxFOVXAtZeroCenter = pZoom->maxFOVXAtCurrentCenter;
				}
				if (0 == pZoom->maxFOVYAtZeroCenter)
				{
					pZoom->maxFOVYAtZeroCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data);
				}	
				if (0 == pZoom->minXFieldOfView)
				{
					pZoom->minXFieldOfView = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[3].pe_data);
				}

				MSG1("pZoom->maxFOVXAtCurrentCenter is %f \n",pZoom->maxFOVXAtZeroCenter );
				OstTraceFiltStatic1(TRACE_DEBUG, "pZoom->maxFOVXAtCurrentCenter is %f ", (&mENSComponent),pZoom->maxFOVXAtZeroCenter );
				MSG1("pZoom->maxFOVYAtCurrentCenter is %f \n ",pZoom->maxFOVYAtZeroCenter );
				OstTraceFiltStatic1(TRACE_DEBUG, "pZoom->maxFOVYAtCurrentCenter is %f \n ", (&mENSComponent),pZoom->maxFOVYAtZeroCenter );
		
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
                                /* Peformance tuning: Zoom should not be required if FOV and OffsetX and OffsetY are already applied */
                                float fCurrentFOV = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[4].pe_data);
                                if ((fCurrentFOV != pZoom->calculateFOV(pZoom->pCenterFOV)) ||
                                    (pZoom->pCenterFOVStatus->sPoint.nX != pZoom->pCenterFOV->sPoint.nX) ||
                                    (pZoom->pCenterFOVStatus->sPoint.nY != pZoom->pCenterFOV->sPoint.nY))
                                {
                                  pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, pZoom->calculateFOV(pZoom->pCenterFOV));
                                  pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, ZoomCommand_e_SetFOV);
                                  pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data+1);

                                  pIspctlCom->processQueue();
                                  MSG2("Apply zoom config %f, %f\n", fCurrentFOV, pZoom->calculateFOV(pZoom->pCenterFOV));
                                }
                                else
                                {
                                  MSG2("Don't need to apply zoom config %f, %f\n", fCurrentFOV, pZoom->calculateFOV(pZoom->pCenterFOV));
                                  pZoom->ZoomCmd = ZoomCommand_e_SetFOV;
                                  SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatusBeforeExecuting);
                                }
#else
				/*First set newly calculated Field of view*/
				pIspctlCom->queuePE(Zoom_Control_f_SetFOVX_Byte0, pZoom->calculateFOV(pZoom->pCenterFOV));
				pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, (t_uint32)ZoomCommand_e_SetFOV);
				pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data+1);
				
				pIspctlCom->processQueue();
#endif
			}
			/* skip changeover linked unused notifications */
			else if ((ISP_SENSOR_START_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_STOP_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_SENSOR_STOP_STREAMING == e->type.ispctlInfo.info_id))
			{
				/* nothing to do */
			}
			/* nothing else to do if we XP70 is not running */
			else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				/*nothing to do*/
			}
			/* a sensor changeover is needed : zoom command will end with ISP_STREAMING notification */
			else if (ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED == e->type.ispctlInfo.info_id)
			{
				pZoom->changeoverIsNeeded = OMX_TRUE;
			}
			/* something went wrong, client must be warned */
			else if (ISP_ZOOM_CONFIG_REQUEST_DENIED == e->type.ispctlInfo.info_id)
			{
				pZoom->requestDenied = OMX_TRUE;
			}
			/* set center command out of range => command was moderated */
			else if (ISP_ZOOM_SET_OUT_OF_RANGE == e->type.ispctlInfo.info_id)
			{
				pZoom->outOfRange = OMX_TRUE;
			}
			/* wait for zoom command end */
			else if (((ISP_ZOOM_STEP_COMPLETE == e->type.ispctlInfo.info_id) && (OMX_FALSE == pZoom->changeoverIsNeeded))
				|| (ISP_STREAMING == e->type.ispctlInfo.info_id))
			{
				MSG0("ExecuteZoomBeforeStateExecuting- zoom complete\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ExecuteZoomBeforeStateExecuting- zoom complete", (&mENSComponent));
				if (OMX_TRUE == pZoom->requestDenied)
				{
					MSG0("ExecuteZoomBeforeStateExecuting- zoom denied\n");
                    // Notify the IL client
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorUnsupportedSetting, (OMX_U32)OMX_Symbian_IndexConfigExtDigitalZoom);
					SCF_PSTATE next;
					SM_POP_STATE(next);
					SCF_TRANSIT_TO_PSTATE(next);
					OstTraceFiltStatic0(TRACE_DEBUG, "1", (&mENSComponent));
				}
				else 
				{
					OstTraceFiltStatic0(TRACE_DEBUG, "4", (&mENSComponent));
					if ((pZoom->pCenterFOVStatus->sPoint.nX == pZoom->pCenterFOV->sPoint.nX ) && 
						(pZoom->pCenterFOVStatus->sPoint.nY == pZoom->pCenterFOV->sPoint.nY  ))
					{
						pZoom->ZoomCmd = ZoomCommand_e_SetFOV;
						SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatusBeforeExecuting);
					}
					else
					{
						SCF_TRANSIT_TO(&CAM_SM::ExecutePanTiltBeforeStateExecuting);
					}
				}
			}
			/* let the parent handle it */
			else
			{
				break;
			}

			return 0;
		}

		case SCF_STATE_EXIT_SIG:
			MSG0("ExecuteZoomBeforeStateExecuting-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExecuteZoomBeforeStateExecuting-EXIT;", (&mENSComponent));
			return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ZoomBeforeStateExecuting);
}

SCF_STATE CAM_SM::ExecutePanTiltBeforeStateExecuting(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* get command coin and MaxFOV */
			pIspctlCom->queuePE(Zoom_CommandStatus_u8_CommandCount_Byte0, 0);
			pIspctlCom->queuePE(Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte0,0);
			pIspctlCom->queuePE(Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_INFO_SIG:
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ExecutePanTiltBeforeStateExecuting-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExecutePanTiltBeforeStateExecuting-EVT_ISPCTL_INFO_SIG/EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			/* prepare zoom command */
			if (ISP_READLIST_DONE == e->type.ispctlInfo.info_id)
			{
				MSG0("ExecutePanTiltBeforeStateExecuting- prepare zoom\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ExecutePanTiltBeforeStateExecuting- prepare zoom", (&mENSComponent));

				MSG1("Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte0 is %f",CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[1].pe_data ));
				OstTraceFiltStatic1(TRACE_DEBUG, "Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte0 is %f", (&mENSComponent),CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[1].pe_data ));
				MSG1("Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte0 is %f",CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data ));
				OstTraceFiltStatic1(TRACE_DEBUG, "Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte0 is %f", (&mENSComponent),CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data ));
				

				pZoom->changeoverIsNeeded = OMX_FALSE;
				pZoom->requestDenied = OMX_FALSE;
				pZoom->outOfRange = OMX_FALSE;

				pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetX_Byte0, pZoom->pCenterFOV->sPoint.nX);
				pIspctlCom->queuePE(Zoom_Control_s16_CenterOffsetY_Byte0, pZoom->pCenterFOV->sPoint.nY);
				pIspctlCom->queuePE(Zoom_CommandControl_e_ZoomCommand_Byte0, (t_uint32)ZoomCommand_e_SetCenter);
				pIspctlCom->queuePE(Zoom_CommandControl_u8_CommandCount_Byte0, e->type.ispctlInfo.Listvalue[0].pe_data+1);
				pIspctlCom->processQueue();
			}
			/* skip changeover linked unused notifications */
			else if ((ISP_SENSOR_START_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_STOP_STREAMING == e->type.ispctlInfo.info_id)
				|| (ISP_SENSOR_STOP_STREAMING == e->type.ispctlInfo.info_id))
			{
				/* nothing to do */
			}
			/* nothing else to do if we XP70 is not running */
			else if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				/*nothing to do*/
			}
			/* a sensor changeover is needed : zoom command will end with ISP_STREAMING notification */
			else if (ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED == e->type.ispctlInfo.info_id)
			{
				pZoom->changeoverIsNeeded = OMX_TRUE;
			}
			/* something went wrong, client must be warned */
			else if (ISP_ZOOM_CONFIG_REQUEST_DENIED == e->type.ispctlInfo.info_id)
			{
				pZoom->requestDenied = OMX_TRUE;
			}
			/* set center command out of range => command was moderated */
			else if (ISP_ZOOM_SET_OUT_OF_RANGE == e->type.ispctlInfo.info_id)
			{
				pZoom->outOfRange = OMX_TRUE;
			}
			/* wait for zoom command end */
			else if (((ISP_ZOOM_STEP_COMPLETE == e->type.ispctlInfo.info_id) && (OMX_FALSE == pZoom->changeoverIsNeeded))
				|| (ISP_STREAMING == e->type.ispctlInfo.info_id))
			{
				MSG0("ExecuteZoomBeforeStateExecuting- zoom complete\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "ExecuteZoomBeforeStateExecuting- zoom complete", (&mENSComponent));
				if (OMX_TRUE == pZoom->requestDenied)
				{
                    // Notify the IL client
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorUnsupportedSetting, (OMX_U32)OMX_Symbian_IndexConfigExtDigitalZoom);
					SCF_PSTATE next;
					SM_POP_STATE(next);
					SCF_TRANSIT_TO_PSTATE(next);
					OstTraceFiltStatic0(TRACE_DEBUG, "1", (&mENSComponent));
				}
				else
				{
					OstTraceFiltStatic0(TRACE_DEBUG, "4", (&mENSComponent));
					pZoom->ZoomCmd = ZoomCommand_e_SetCenter;
					SCF_TRANSIT_TO(&CAM_SM::ZoomingGetStatusBeforeExecuting);
				}
			}
			/* let the parent handle it */
			else
			{
				break;
			}

			return 0;
		}

		case SCF_STATE_EXIT_SIG:
			MSG0("ExecutePanTiltBeforeStateExecuting-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ExecutePanTiltBeforeStateExecuting-EXIT;", (&mENSComponent));
			return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ZoomBeforeStateExecuting);
}

SCF_STATE CAM_SM::ZoomingGetStatusBeforeExecuting(s_scf_event const *e)
{
	switch (e->sig)
	{
   		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
				pIspctlCom->queuePE(Zoom_Status_s16_CenterOffsetX_Byte0, 0);
				pIspctlCom->queuePE(Zoom_Status_s16_CenterOffsetY_Byte0, 0);
				pIspctlCom->queuePE(Zoom_Status_f_FOVX_Byte0, 0);
				pIspctlCom->queuePE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0, 0);
				pIspctlCom->readQueue();
			return 0;
		}

		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ZoomingGetStatus-EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ZoomingGetStatus-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (ISP_READLIST_DONE != e->type.ispctlInfo.info_id) break;

			if (ZoomCommand_e_SetFOV == pZoom->ZoomCmd)
			{
				pZoom->pZoomFactorStatus->xZoomFactor.nValue = pZoom->maxFOVXAtCurrentCenter * 0x10000 / CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data);
			}
			else if (ZoomCommand_e_SetCenter == pZoom->ZoomCmd)
			{
              
			  pZoom->pCenterFOVStatus->sPoint.nX = e->type.ispctlInfo.Listvalue[0].pe_data;
			  pZoom->pCenterFOVStatus->sPoint.nY = e->type.ispctlInfo.Listvalue[1].pe_data;
			  /*As center is shifted reset zoom factor from user point of view*/
			  pZoom->pZoomFactorStatus->xZoomFactor.nValue = 0x10000;
			  
			  /* Currently there is a bug in Fw that Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0 get changed */
			  pZoom->maxFOVXAtCurrentCenter = CIspctlCom::u32tofloat(e->type.ispctlInfo.Listvalue[2].pe_data);
			  MSG2("ZoomingGetStatus-center : %ld %ld\n", pZoom->pCenterFOVStatus->sPoint.nX, pZoom->pCenterFOVStatus->sPoint.nY);
			  OstTraceFiltStatic2(TRACE_DEBUG, "ZoomingGetStatus-center : %ld %ld", (&mENSComponent), pZoom->pCenterFOVStatus->sPoint.nX, pZoom->pCenterFOVStatus->sPoint.nY);
			}
			
			pZoom->ZoomCmd = ZoomCommand_e_None;
			SCF_PSTATE next;
			SM_POP_STATE(next);
			SCF_TRANSIT_TO_PSTATE(next);
			return 0;
		}

		case SCF_STATE_EXIT_SIG: EXIT; return 0;

		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::ZoomBeforeStateExecuting);
}


