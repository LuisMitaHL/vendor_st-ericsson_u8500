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
#define DBGT_PREFIX "BOOT"


#include "debug_trace.h"

/* */
/* Boot state machine */
/**/
SCF_STATE CAM_SM::Booting(s_scf_event const *e)
{
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
            SCF_INIT(&CAM_SM::InitingYUVCameraModule);
            return 0;
        }
    case SCF_PARENT_SIG:
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}


SCF_STATE  CAM_SM::InitingYUVCameraModule(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            DBGT_PTRACE("InitingYUVCameraModule-ENTRY");
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case SCF_STATE_EXIT_SIG:
        {
            DBGT_PTRACE("InitingYUVCameraModule-SCF_STATE_EXIT_SIG");
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG :
        {
            DBGT_PTRACE("SHARED_InitingYUVCameraModule-EVT_ISPCTL_INFO_SIG");

            // Inderpal: Change this logic
                int bSuccess;
                int DelayTime;
                Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
                if (Cam->cam_Ctrllib != NULL) {

                    bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_IsAlive(Cam->cam_h);
                    bSuccess = true;
                    if (!bSuccess) {

                        DBGT_PTRACE("Error from Camera_CtrlLib_IsAlive");

                        mENSComponent.eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorHardware,CAMERA_BOOT_FAILED,0);

                        return 0;
                    } else {
                        Cam->cam_Ctrllib->Camera_CtrlLib_FirmwareDownload();

                        bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_Initialize(Cam->cam_h, &DelayTime);
                        if (!bSuccess) {
                            DBGT_CRITICAL("Error from Camera_CtrlLib_Initialize");
                        }

                        t_uint8* Name_p = new t_uint8[50];
                        const t_uint32 NameSize=50;
                        t_uint8* Vendor_p = new t_uint8[50];
                        const t_uint32 VendorSize=50;
                        t_uint8* MajorVersion_p = new t_uint8[10];
                        t_uint8* MinorVersion_p = new t_uint8[10];
                        t_uint8* CameraOrientation_p = new t_uint8[10];

                        Cam->cam_Ctrllib->Camera_CtrlLib_GetDriverInfo(Cam->cam_h,
                                NameSize,
                                VendorSize,
                                Name_p,
                                Vendor_p,
                                MajorVersion_p,
                                MinorVersion_p,
                                CameraOrientation_p
                                );

                        DBGT_PTRACE("Name_p %s", Name_p);
                        DBGT_PTRACE("Vendor_p %s", Vendor_p);
                        DBGT_PTRACE("MajorVersion_p %d", *MajorVersion_p);
                        DBGT_PTRACE("MinorVersion_p %d", *MinorVersion_p);
                        DBGT_PTRACE("CameraOrientation_p %s", (char*)CameraOrientation_p);

                        delete Name_p;
                        delete Vendor_p;
                        delete MajorVersion_p;
                        delete MinorVersion_p;
                        delete CameraOrientation_p;
                    }

                    SCF_TRANSIT_TO(&CAM_SM::EnteringPreview);
                    return 0;
                }
        }
    default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::Booting);
}
