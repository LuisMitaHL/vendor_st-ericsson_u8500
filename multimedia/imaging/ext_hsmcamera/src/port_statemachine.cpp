/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hsmcam.h"
#undef LOG_TAG
#include "IFM_HostNmfProcessingComp.h"

#undef   LOG_TAG
#define  LOG_TAG DBGT_TAG
#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "STMA"
#include "debug_trace.h"

SCF_STATE CAM_SM::EnablePortInIdleExecutingState(s_scf_event const *e) {
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
            SCF_INIT(&CAM_SM::EnablePortInIdleExecutingState_StopVpip);
            return 0;
        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("EnablePortInIdleExecutingState-Q_DEFERRED_EVENT_SIG");
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}

SCF_STATE CAM_SM::EnablePortInIdleExecutingState_StopVpip(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
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
            DBGT_PTRACE("EnablePortInIdleExecutingState_StopVpip-EVT_ISPCTL_INFO_SIG");

            OMX_STATETYPE currentState;
            mENSComponent.GetState(&mENSComponent, &currentState);
            SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_ConfigurePipe);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}



SCF_STATE CAM_SM::EnablePortInIdleExecutingState_ConfigurePipe(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
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
            DBGT_PTRACE("EnablePortInIdleExecutingState_ConfigurePipe-EVT_ISPCTL_INFO_SIG");
            SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_CfgGrabStartVpip);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_CfgGrabStartVpip(s_scf_event const *e)
{
    camport *     portVF = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT0);
    camport *     portSS = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);

    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("EnablePortInIdleExecutingState_CfgGrabStartVpip-ENTRY");
            pGrabControl->configureGrabResolutionFormat(portVF);
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
            DBGT_PTRACE("EnablePortInIdleExecutingState_CfgGrabStartVpip-EVT_ISPCTL_INFO_SIG");

            OMX_STATETYPE currentState;
            mENSComponent.GetState(&mENSComponent, &currentState);

            mENSComponent.eventHandler(OMX_EventPortSettingsChanged, (OMX_U32)0, portVF->getPortIndex());
            mENSComponent.eventHandler(OMX_EventPortSettingsChanged, (OMX_U32)0, portSS->getPortIndex());
            SCF_TRANSIT_TO(&CAM_SM::EnablePortInIdleExecutingState_SendOmxCb);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}


SCF_STATE CAM_SM::EnablePortInIdleExecutingState_SendOmxCb(s_scf_event const *e) {
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
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
            DBGT_PTRACE("EnablePortInIdleExecutingState_SendOmxCb-EVT_ISPCTL_INFO_SIG");
            IFM_HostNmfProcessingComp* Cam = (IFM_HostNmfProcessingComp*)&mENSComponent.getProcessingComponent();
            (Cam->getNmfSendCommandItf())->sendCommand(OMX_CommandPortEnable,enabledPort);

            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnablePortInIdleExecutingState);
}
