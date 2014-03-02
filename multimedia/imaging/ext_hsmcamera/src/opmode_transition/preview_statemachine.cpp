/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hsmcam.h"
#include "ext_camera.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "PREV"
#include "debug_trace.h"

/* */
/* EnteringPreview state machine */
/**/
SCF_STATE CAM_SM::EnteringPreview(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::EnteringPreview_DoingPreviewConfiguration);
            return 0;
        }
    case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_PTRACE("\t  Error: e->type.ispctlInfo.info_id 0x%x", e->type.ispctlInfo.info_id);
            DBGT_PTRACE("EnteringPreview-EVT_ISPCTL_ERROR_SIG");
            if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR) {
                DBGT_PTRACE("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;");
                DBC_ASSERT(0);
            }
            if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR) {
                DBGT_PTRACE("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;");
                DBC_ASSERT(0);
            }
        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("EnteringPreview-DEFERRED_EVENT;");
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);//Up_And_Running
}


SCF_STATE CAM_SM::EnteringPreview_DoingPreviewConfiguration(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("EnteringPreview_DoingPreviewConfiguration-SCF_STATE_ENTRY_SIG;");
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("EnteringPreview_DoingPreviewConfiguration-EVT_ISPCTL_INFO_SIG;");
            SCF_TRANSIT_TO(&CAM_SM::PreparingGrabConfig);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringPreview);
}


/* */
/* ExitingPreview state machine */
/**/
SCF_STATE CAM_SM::ExitingPreview(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::ExitingPreview_StoppingVpip);
            return 0;
        }
    case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_PTRACE("ExitingPreview-EVT_ISPCTL_ERROR_SIG");
            if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
                DBGT_PTRACE("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;");
                DBC_ASSERT(0);
            }
            if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
                DBGT_PTRACE("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;");
                DBC_ASSERT(0);
            }
        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("ExitingPreview-DEFERRED_EVENT;");
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}


SCF_STATE CAM_SM::ExitingPreview_StoppingVpip(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("ExitingPreview_StoppingVpip-SCF_STATE_ENTRY_SIG;");
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("ExitingPreview_StoppingVpip-EVT_ISPCTL_INFO_SIG;");
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();

            if(pOpModeMgr->ConfigCapturePort == CapturePort)
            {
                pOpModeMgr->captureRequest[CAMERA_PORT_OUT1] ++;
                DBGT_PTRACE("----------CAPTURE_REQUEST =%d", pOpModeMgr->captureRequest[CAMERA_PORT_OUT1]);

                OMX_PARAM_PORTDEFINITIONTYPE mParamVFPortDefinition;
                mParamVFPortDefinition.nSize               = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                getOmxIlSpecVersion(&mParamVFPortDefinition.nVersion);
                mENSComponent.getPort(CAMERA_PORT_OUT0)->getParameter(OMX_IndexParamPortDefinition,&mParamVFPortDefinition);

                if((mParamVFPortDefinition.bEnabled)) {
                    if(pOpModeMgr->nBufferAtMPC[CAMERA_PORT_OUT0]==0) {
                        /* all buffers are at component : no EOS can be generated*/
                        DBGT_PTRACE("Buffers are at component");
                        pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT0); // block next fillthisbuffer at grabctl

                        /* Stop watchdog timer */
                        watchdogStop();

                        int DelayTime;
                        ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_StopViewFind(Cam->cam_h, &DelayTime);

                        SCF_TRANSIT_TO(&CAM_SM::EnteringCapture);
                    } else {
                        /* there are remaining buffers at MPC : we need to generate an EOS */
                        DBGT_PTRACE("Buffers are at MPC, wait for self-triggered EOS");
                        pOpModeMgr->waitingEOS[CAMERA_PORT_OUT0] = OMX_TRUE;
                        pGrabControl->setEndCapture(CAMERA_PORT_OUT0);
                        SCF_TRANSIT_TO(&CAM_SM::WaitEvent);
                    }
                } else {
                    //if port disa VF then directly trigger the Capture now
                    SCF_TRANSIT_TO(&CAM_SM::EnteringCapture);
                }
            }
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingPreview);
}


/* */
/* BackPreview state machine */
/**/
SCF_STATE CAM_SM::BackPreview(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case SCF_STATE_INIT_SIG:
        {
            INIT;
            SCF_INIT(&CAM_SM::BackPreview_StartingVpip);
            return 0;
        }
    case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_PTRACE("BackPreview-EVT_ISPCTL_ERROR_SIG");
            if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
                DBGT_PTRACE("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;");
                DBC_ASSERT(0);
            }
            if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
                DBGT_PTRACE("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;");
                DBC_ASSERT(0);
            }

        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("BackPreview-DEFERRED_EVENT;");
            return 0;
        }
    default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}


SCF_STATE CAM_SM::BackPreview_StartingVpip(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("BackPreview_StartingVpip-SCF_STATE_ENTRY_SIG;");

            pOpModeMgr->waitingEOS[CAMERA_PORT_OUT0]=OMX_FALSE;

            int bSuccess;
            int DelayTime;
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
            //Configure for right size
            camport * port = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);

            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize               = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(port->getPortIndex())->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

            HAL_Gr_Camera_SizeConfig_t* SizeConfig_p = new HAL_Gr_Camera_SizeConfig_t;
            t_uint8 Id = 0;
            bool found = 0;

            while(!found)
            {
                bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_GetSizeConfig(Cam->cam_h, Id, SizeConfig_p);
                if(!bSuccess){
                    DBGT_PTRACE("Error from Camera_CtrlLib_GetSizeConfig: Id not found");
                    DBC_ASSERT(0);
                    break;
                }
                if(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillPreview){
                    if((SizeConfig_p->Height == (int)port->mInternalFrameHeight) &&
                            (SizeConfig_p->Width == (int)port->mInternalFrameWidth) &&
                            (SizeConfig_p->Type ==1))
                        found = 1;
                    else
                        Id++;
                }
                else
                {   // variable framerate is required
                    if((SizeConfig_p->Height == (int)port->mInternalFrameHeight) &&
                            (SizeConfig_p->Width == (int)port->mInternalFrameWidth) &&
                            (SizeConfig_p->Type == 2))
                        found = 1;
                    else
                        Id++;
                }
            }

            bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_SetViewfindSize(Cam->cam_h, Id, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_SetViewfindSize");
                DBC_ASSERT(0);
            }

            bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_SetProperty(Cam->cam_h, CAMCTRL_FRAMERATE, mParamPortDefinition.format.video.xFramerate, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_SetProperty CAMCTRL_FRAMERATE");
                DBC_ASSERT(0);
            }
            bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_ReturnViewFind(Cam->cam_h, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_ReturnViewFind");
                DBC_ASSERT(0);
            }

            pGrabControl->setStartCapture(CAMERA_PORT_OUT0);

            /* Start watchdog timer */
            watchdogStart();

            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("BackPreview_StartingVpip-EVT_ISPCTL_INFO_SIG;");
            if(pOpModeMgr->AutoPauseAfterCapture)
                SCF_TRANSIT_TO(&CAM_SM::BackPreview_AutoPause);
            else
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::BackPreview);
}


SCF_STATE CAM_SM::BackPreview_AutoPause(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("BackPreview_AutoPause-SCF_STATE_ENTRY_SIG;");

            mENSComponent.SendCommand(&mENSComponent,OMX_CommandStateSet, OMX_StatePause, NULL);

            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("BackPreview_AutoPause-EVT_ISPCTL_INFO_SIG;");
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::BackPreview);
}
