/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hsm.h"
#include "ext_hsmcam.h"
#include "ext_camera.h"

#undef  DBGT_LAYER
#define DBGT_LAYER 3
#undef  DBGT_PREFIX
#define DBGT_PREFIX "HSMC"
#include "debug_trace.h"

/* *
 * HSMCAM::ProcessPendingEvents is responsible for processing the incoming events
 * This is the state where the all the events are dispatched to the appropriate state machine
 * At the end of the dedicated state machine, Go back to this state machine
 * */
SCF_STATE CAM_SM::ProcessPendingEvents(s_scf_event const *e)
{
    Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();

    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            DBGT_PTRACE("CAM_SM::ProcessPendingEvents-ENTRY");
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("ProcessPendingEvents-Q_DEFERRED_EVENT_SIG");
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("ProcessPendingEvents-EVT_ISPCTL_INFO_SIG");
            s_scf_event * event;
            OMX_BOOL prior = OMX_TRUE;
            DBGT_PTRACE("NOW Dequeue prior event and process it");
            event = pDeferredEventMgr->dequeuePriorEvent();
            if(event->sig == SHARED_RETURN_SIG) {
                prior = OMX_FALSE;
                DBGT_PTRACE("No prior events thus Dequeue pending event and process it");
                event = pDeferredEventMgr->dequeuePendingEvent();
            }

            DBGT_PTRACE("Event to be processed =%d", event->sig);
            switch (event->sig) {
            case Q_LOCK_OK_SIG:
                {
                    break;
                }
            case CAMERA_EOS_VPB02_SIG:
                {
                    if (pOpModeMgr->ConfigCapturing[CAMERA_PORT_OUT1]) {
                        DBGT_PTRACE("CAM_DO_RAWCAPTURE_SIG :- CAM_SM::EnteringCapture");
                        int DelayTime;

                        /* Stop watchdog timer */
                        watchdogStop();

                        ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_StopViewFind(Cam->cam_h, &DelayTime);

                        SCF_TRANSIT_TO(&CAM_SM::EnteringCapture);
                    }
                    break;
                }
            case CAMERA_EXIT_PREVIEW_SIG:
                {
                    DBGT_PTRACE("CAM_SM::ProcessPendingEvents-EXIT_PREVIEW_SIG;");;
                    t_uint16 portIndex=event->type.capturingStruct.nPortIndex;
                    if (CAMERA_PORT_OUT1 == portIndex) {
                        if(event->type.capturingStruct.bEnabled) {
                            if(pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]==0) {
                                pOpModeMgr->ConfigCapturing[portIndex] = event->type.capturingStruct.bEnabled;
                                pOpModeMgr->ConfigCapturePort =  event->type.capturingStruct.nPortIndex;
                                SCF_TRANSIT_TO(&CAM_SM::ExitingPreview);
                            } else {
                                /* if the expected capture is not yet done then do post it again as we expected to be woken up with this HR cb */
                                pDeferredEventMgr->requeueEventAtSameLocation(event);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            }
                        } else {
                            DBGT_PTRACE("End of capture has been triggered");
                            pOpModeMgr->ConfigCapturing[portIndex] = event->type.capturingStruct.bEnabled;
                            pOpModeMgr->ConfigCapturePort =  event->type.capturingStruct.nPortIndex;
                            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                        }
                    } else {
                        mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadParameter, event->type.capturingStruct.nPortIndex);
                        SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
                        break;
                    }
                    break;
                }
            case CAMERA_FILLBUFFERDONE_SIG :
                {
                    DBGT_PTRACE("ProcessPendingEvents-CAMERA_FILLBUFFERDONE_SIG");
                    SCF_TRANSIT_TO(&CAM_SM::EnteringCapture_FillBufferDone);
                    break;
                }
            case SEND_COMMAND_SIG:
            case ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-SEND_COMMAND_SIG");

                    DBGT_PTRACE("----------CAPTURE_REQUEST =%d", pOpModeMgr->captureRequest[pOpModeMgr->ConfigCapturePort]);
                    /* if one expected Capture is on going */
                    if(pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]!=0)
                    {

                        if( ((pOpModeMgr->CurrentOperatingMode==OpMode_Cam_VideoRecord)||
                                    (pOpModeMgr->CurrentOperatingMode==OpMode_Cam_StillFaceTracking))&&
                                ((event->args.sendCommand.Cmd == OMX_CommandPortEnable)||
                                 ( event->args.sendCommand.Cmd==OMX_CommandPortDisable)) ){

                            if(event->sig == ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG) {
                                /* it's a port Ena with settings to be changed */
                                /* change them first then apply the port Ena */
                                enabledPort = event->args.sendCommand.nParam;
                                SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState);
                            }

                            /* TODO : we should manage case it is a gobackIdle */
                            if((event->args.sendCommand.Cmd == OMX_CommandStateSet)&&
                                    (event->args.sendCommand.nParam==OMX_StateIdle)) {
                                /* this is not supported yet :
                                 * what should be done ? : - self-trigger the end of the record ?
                                 *         - report error ?
                                 */
                                mENSComponent.eventHandler(OMX_EventError,
                                        (OMX_U32)OMX_ErrorIncorrectStateOperation,
                                        event->args.sendCommand.Cmd);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            } else {
                                DBGT_PTRACE("Forward SendCommand now in case of Ena/Disa in Record mode");
                                /* if it's a PortEna or PortDisa during a Record/Facetracking, then forward NOW */
                                (Cam->getNmfSendCommandItf())->sendCommand(event->args.sendCommand.Cmd,event->args.sendCommand.nParam);
                                SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                            }
                        } else {
                            /* else if it's a Still not yet done, then post the sendCommand again as we expected to be woken-up with its cb */
                            DBGT_PTRACE("Requeue sendCommand:%d",pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);
                            pDeferredEventMgr->requeueEventAtSameLocation(event);
                            SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                        }
                    } else {
                        if(event->sig == ENABLE_PORT_IN_IDLE_OR_EXECUTING_SIG)
                        {
                            /* it's a port Ena with settings to be changed */
                            /* change them first then apply the port Ena */
                            enabledPort = event->args.sendCommand.nParam;
                            SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState);
                        }
                        else {
                            /* Forward now the sendCommand as nothing is on-going*/
                            DBGT_PTRACE("Forward SendCommand now");
                            (Cam->getNmfSendCommandItf())->sendCommand(event->args.sendCommand.Cmd,event->args.sendCommand.nParam);
                            SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                        }
                    }
                    break;
                }

            case CAMERA_EXECUTING_TO_IDLE_END_SIG:
                {
                    DBGT_PTRACE("WaitEvent-EXECUTING_TO_IDLE_SIG;");

                    int DelayTime;

                    pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT0); // block next fillthisbuffer at grabctl

                    /* Stop watchdog timer */
                    watchdogStop();

                    ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_StopViewFind(Cam->cam_h, &DelayTime);

                    mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);

                    SCF_TRANSIT_TO(&CAM_SM::WaitEvent);

                    break;
                }
            case CAMERA_IDLE_TO_EXECUTING_SIG:
                {
                    int bSuccess;
                    int DelayTime;

                    //Configure for right size
                    camport * port = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);

                    OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
                    mParamPortDefinition.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                    getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
                    mENSComponent.getPort(port->getPortIndex())->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

                    HAL_Gr_Camera_SizeConfig_t* SizeConfig_p = new HAL_Gr_Camera_SizeConfig_t;
                    t_uint8 Id = 0;
                    bool found = 0;

                    while(!found)
                    {
                        bSuccess = ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_GetSizeConfig(Cam->cam_h, Id, SizeConfig_p);
                        if(!bSuccess)
                        {
                            DBGT_PTRACE("Error from Camera_CtrlLib_GetSizeConfig: Id not found");
                            DBC_ASSERT(0);
                            break;
                        }
                        if(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillPreview)
                        {    // preview mode
                            if((SizeConfig_p->Height == (int)port->mInternalFrameHeight) &&
                                    (SizeConfig_p->Width == (int)port->mInternalFrameWidth) &&
                                    (SizeConfig_p->Type == 1))
                                found = 1;
                            else
                                Id++;
                        }
                        else {   // video mode
                            if((SizeConfig_p->Height == (int)port->mInternalFrameHeight) &&
                                    (SizeConfig_p->Width == (int)port->mInternalFrameWidth) &&
                                    (SizeConfig_p->Type == 2))
                                found = 1;
                            else
                                Id++;
                        }
                    }

                    bSuccess = ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_SetViewfindSize(Cam->cam_h, Id, &DelayTime);
                    if(!bSuccess)
                    {
                        DBGT_PTRACE("Error from Camera_CtrlLib_SetViewfindSize");
                        DBC_ASSERT(0);
                    }
                    bSuccess = ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_SetProperty(Cam->cam_h, CAMCTRL_FRAMERATE, mParamPortDefinition.format.video.xFramerate, &DelayTime);
                    if(!bSuccess)
                    {
                        DBGT_PTRACE("Error from Camera_CtrlLib_SetProperty CAMCTRL_FRAMERATE");
                        DBC_ASSERT(0);
                    }
                    delete SizeConfig_p;

                    //start viewfinder
                    bSuccess = ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_StartViewFind(Cam->cam_h, &DelayTime);
                    if(!bSuccess)
                    {
                        DBGT_PTRACE("Error from Camera_CtrlLib_StartViewFind");
                        DBC_ASSERT(0);
                    }

                    pGrabControl->setStartCapture(CAMERA_PORT_OUT0);

                    /* Start watchdog timer */
                    watchdogStart();

                    mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);

                    SCF_TRANSIT_TO(&CAM_SM::WaitEvent);

                    break;
                }
            case CAMERA_INDEX_CONFIG_ZOOM_SIG:
                {
                    break;
                }
            case CAMERA_INDEX_CONFIG_TEST_MODE_SIG:
                {
                    break;
                }
            case CAMERA_INDEX_CONFIG_WHITE_BALANCE_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-WHITE_BALANCE_SIG;");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_FOCUS_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-FOCUS;");
                    DBC_ASSERT(0);
                    break;
                }

            case CAMERA_INDEX_CONFIG_IMGFILTER_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-IMGFILTER;");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_GAMMA_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-GAMMA;");
                    DBC_ASSERT(0);
                    break;

                }
            case CAMERA_INDEX_CONFIG_BRIGHTNESS_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-BRIGHTNESS");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_LIGHTNESS_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-LIGHTNESS");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_GET_EXPOSUREVALUE_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-GET_EXPOSUREVALUE;");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_EXPOSURECONTROL_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-EXPOSURECONTROL;");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_CONTRAST_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-CONTRAST");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_SATURATION_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-SATURATION");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_STATUS_FOCUS_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-FOCUS");
                    DBC_ASSERT(0);
                    break;
                }
            case CAMERA_INDEX_CONFIG_FLICKER_REMOVAL_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-FLICKER_REMOVAL_SIG;");
                    DBC_ASSERT(0);
                    break;
                }
            case SHARED_RETURN_SIG:
                {
                    DBGT_PTRACE("ProcessPendingEvents-SHARED_RETURN_SIG");
                    SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                    break;
                }
            default:
                {
                    if(prior == OMX_FALSE) pDeferredEventMgr->requeueEventAtSameLocation(event);
                    else pDeferredEventMgr->requeuePriorEventAtSameLocation(event);
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


/* *
 * HSMCAM::PreparingGrabConfig
 * */
SCF_STATE CAM_SM::PreparingGrabConfig(s_scf_event const *e) {
    Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("PreparingGrabConfig-ENTRY");
            camport *  port = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);
            /* sets the Frame Skip values */
            if (port->bOneShot== OMX_TRUE) { /* preview mode */
                pGrabControl->setFrameSkipValues(
                        Cam->cam_Ctrllib->Camera_CtrlLib_GetVfFrameSkip(Cam->cam_h),
                        Cam->cam_Ctrllib->Camera_CtrlLib_GetSsFrameSkip(Cam->cam_h));
            } else { /* camcorder mode */
                pGrabControl->setFrameSkipValues(
                        Cam->cam_Ctrllib->Camera_CtrlLib_GetCcFrameSkip(Cam->cam_h),
                        Cam->cam_Ctrllib->Camera_CtrlLib_GetSsFrameSkip(Cam->cam_h));
            }
            pGrabControl->configureGrabResolutionFormat(port);
            return 0;
        }
    case CONFIGURE_SIG:
        {
            DBGT_PTRACE("PreparingGrabConfig-CONFIGURE_SIG");
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("PreparingGrabConfig-EVT_ISPCTL_INFO_SIG");
            mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
            camport * port = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);
            port->portSettingsChanged = OMX_FALSE;
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::OpModeTransition);
}

/* *
 * CAM_SM::timeoutAction
 * actions taken if timeout happens
 * */
void CAM_SM::timeoutAction (void)
{
    DBGT_PTRACE("Discarded Timeout Event");
}


/* This function starts or stops the watchdog timer
 */
void CAM_SM::watchdogStop()
{
    if (isWatchdogTimerRunning)
    {
        mTimer.stopTimer();
        isWatchdogTimerRunning = false;
        DBGT_PDEBUG("Watchdog timer stopped\n");
    }
}

void CAM_SM::watchdogStart()
{
    if ((watchdogIntervalMsec > 0) && (isWatchdogTimerRunning == false))
    {
        if (mTimer.startTimer(watchdogIntervalMsec*1000, watchdogIntervalMsec*1000))
        {
            DBGT_PDEBUG("Watchdog timer startup failed\n");
        }
        else
        {
            DBGT_PDEBUG("Watchdog timer started / reloaded\n");
        }
        isWatchdogTimerRunning = true;
    }
}
