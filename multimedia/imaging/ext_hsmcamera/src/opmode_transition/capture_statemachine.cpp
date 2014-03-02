/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hsmcam.h"
#include "ext_omxcamera.h"
#include "ext_camera.h"
#include "extradata.h"
#include "omxilosalservices.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "CAPT"
#include "debug_trace.h"

/* */
/* EnteringCapture state machine */
/**/
SCF_STATE CAM_SM::EnteringCapture(s_scf_event const *e)
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
            SCF_INIT(&CAM_SM::EnteringCapture_DoingCaptureConfiguration);
            return 0;
        }
    case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_PTRACE("EnteringCapture-EVT_ISPCTL_ERROR_SIG");
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
            DBGT_PTRACE("EnteringCapture-DEFERRED_EVENT;");
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringOpMode);
}


SCF_STATE CAM_SM::EnteringCapture_DoingCaptureConfiguration(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;

            DBGT_PTRACE("EnteringCapture_DoingCaptureConfiguration-SCF_STATE_ENTRY_SIG");

            pOpModeMgr->UpdateCurrentOpMode();

            camport *     port = (camport *) mENSComponent.getPort(CAMERA_PORT_OUT1);
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

            DBC_ASSERT(port->isSupportedFmt(CAMERA_PORT_OUT1,mParamPortDefinition.format.image.eColorFormat,(t_uint32)mParamPortDefinition.format.image.eCompressionFormat));
            pGrabControl->configureGrabResolutionFormat(port);

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
            DBGT_PTRACE("EnteringCapture_DoingCaptureConfiguration-EVT_ISPCTL_INFO_SIG;");
            SCF_TRANSIT_TO(&CAM_SM::EnteringCapture_SendBuffers);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringCapture);
}


SCF_STATE CAM_SM::EnteringCapture_SendBuffers(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("EnteringCapture_SendBuffers-SCF_STATE_ENTRY_SIG;");
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
            DBGT_PTRACE("EnteringCapture_SendBuffers-EVT_ISPCTL_INFO_SIG;");

            int bSuccess;
            int DelayTime;
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();

            if(Cam->cam_Ctrllib->Camera_CtrlLib_GetPipeNumber(Cam->cam_h) <= 1)
            {
                //Configure for right size
                camport * port = (camport *)  mENSComponent.getPort(CapturePort);

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
                    if((SizeConfig_p->Height == (int)port->mInternalFrameHeight) &&
                            (SizeConfig_p->Width == (int)port->mInternalFrameWidth) &&
                            (SizeConfig_p->Type == 0)) /*For raw capture table contains 0*/
                        found = 1;
                    else
                        Id++;
                }

                bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_SetSnapshotSize(Cam->cam_h, Id, &DelayTime);
                if(!bSuccess)
                {
                    DBGT_PTRACE("Error from Camera_CtrlLib_SetSnapshotSize");
                    DBC_ASSERT(0);
                }
            }

            bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_StartSnapshot(Cam->cam_h, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_StartSnapshot");
                DBC_ASSERT(0);
            }

            if((pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureSingle)
                    ||(pOpModeMgr->CurrentOperatingMode == OpMode_Cam_StillCaptureSingleInVideo))
            {
                pGrabControl->setStartOneShotCapture(CapturePort);
            }
            else
            {
                pGrabControl->setStartCapture(CapturePort);
            }
            if (Cam->m_auto_flash_on)
            {
                t_uint32 sensor_gain;
                //get light condition
                Cam->cam_Ctrllib->Camera_CtrlLib_GetProperty(Cam->cam_h,CAMCTRL_GET_LIGHT_VALUE,(int*)&sensor_gain);

                //This light value is the sensor gain i.e more the sensor gain darker the the light condition.
                if (sensor_gain >= Cam->auto_flash_on_threshold) {
                    Cam->m_is_need_flash_on=1;
                }
                else
                    Cam->m_is_need_flash_on=0;
            }
            if (Cam->m_is_need_flash_on){
                bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_StartFlash(
                Cam->cam_h,
                &DelayTime );
            }

            bSuccess = Cam->cam_Ctrllib->Camera_CtrlLib_TakeSnapshot(Cam->cam_h, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_TakeSnapshot");
                DBC_ASSERT(0);
            }

            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringCapture);
}


SCF_STATE CAM_SM::EnteringCapture_FillBufferDone(s_scf_event const *e)
{

    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("EnteringCapture_FillBufferDone-SCF_STATE_ENTRY_SIG;");
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
            DBGT_PTRACE("EnteringCapture_FillBufferDone-EVT_ISPCTL_INFO_SIG;");
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
            int bSuccess;
            int DelayTime;

            bSuccess = ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_VerifySnapshot(Cam->cam_h, &DelayTime);
            if(!bSuccess)
            {
                DBGT_PTRACE("Error from Camera_CtrlLib_VerifySnapshot");
                DBC_ASSERT(0);
            }

            /* get OMX buffer pointer stored in fillBufferDone */
            OMX_U32 nBufferCountActual = mENSComponent.getPort(CAMERA_PORT_OUT1)->getBufferCountActual();
            OMX_BUFFERHEADERTYPE* pOmxBufHdr = pCapture_context->ppOmxBufHdr[pOpModeMgr->UserBurstNbFrames % nBufferCountActual];

            /* increment frame number */
            pOpModeMgr->UserBurstNbFrames++; /*Not Used in case of Infinite Burst */

            /* dispatch the EXIT_CAPTURE signal as soon as we detect the right condition
             * depending the operating mode we are */
            OMX_BOOL isFiniteStillDone = OMX_BOOL((pOpModeMgr->UserBurstNbFrames == pOpModeMgr->GetBurstFrameLimit())&&((pOpModeMgr->CurrentOperatingMode !=OpMode_Cam_StillCaptureBurstInfinite)&&(pOpModeMgr->CurrentOperatingMode !=OpMode_Cam_StillCaptureBurstInfiniteInVideo)));
            OMX_BOOL isInfiniteDone = OMX_BOOL((!pOpModeMgr->ConfigCapturing[CapturePort])&&(pOmxBufHdr->nFlags==OMX_BUFFERFLAG_EOS));
            OMX_BOOL isOmxStateOk = OMX_BOOL((pOpModeMgr->CurrentOperatingMode != OpMode_Cam_StillPreview)&&(pOpModeMgr->CurrentOperatingMode !=OpMode_Cam_VideoPreview));

            if(isOmxStateOk&&(isFiniteStillDone||isInfiniteDone))
            {
                DBGT_PTRACE("End Capture");
                pOmxBufHdr->nFlags |=  OMX_BUFFERFLAG_EOS;
                DBGT_PTRACE("OMX_BUFFERFLAG_EOS, OMX_EventBufferFlag 1");
                mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CAMERA_PORT_OUT1, OMX_BUFFERFLAG_EOS, 0);

                pOpModeMgr->UserBurstNbFrames=0;
                pOpModeMgr->SetBurstFrameLimit(1);
                pOpModeMgr->SetBurstLimited(OMX_TRUE);

                pCapture_context->nfillBufferDoneCount = 0;
                pOpModeMgr->ConfigCapturing[CapturePort] = OMX_FALSE;
                pOpModeMgr->captureRequest[CapturePort] --;
                DBGT_PTRACE("----------CAPTURE_REQUEST =%d", pOpModeMgr->captureRequest[CapturePort]);

                /* block next buffer at grabctl, will be enabled at next raw */
                pGrabControl->setEndCaptureNow(CAMERA_PORT_OUT1);

                SCF_TRANSIT_TO(&CAM_SM::ExitingCapture);
            }
            else
            {
                DBGT_PTRACE("OMX_BUFFERFLAG_EOS, OMX_EventBufferFlag 3");
                pOmxBufHdr->nFlags |=  OMX_BUFFERFLAG_EOS;
                mENSComponent.eventHandlerCB(OMX_EventBufferFlag, CapturePort, OMX_BUFFERFLAG_EOS, 0);

                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }

            //Add extradata
            IFM_EXIF_SETTINGS ExifParams;
            OMX_ERRORTYPE err = OMX_ErrorNone;
            OMX_VERSIONTYPE version;

            camport* pCamPort = (camport*) mENSComponent.getPort(CAMERA_PORT_OUT1);
            COmxCamera* pOmxCamera          = (COmxCamera*)&mENSComponent;
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize              = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

            // Get current Date/Time
            omxilosalservices::OmxIlOsalDateTime::OmxIlOsalDateTimeType OmxOsalDateTime;
            omxilosalservices::OmxIlOsalDateTime::OmxIlOsalGetDateTime(&OmxOsalDateTime);
            IFM_DATE_TIME CurrentDateTime;
            CurrentDateTime.year     = OmxOsalDateTime.year;
            CurrentDateTime.month    = OmxOsalDateTime.month;
            CurrentDateTime.day      = OmxOsalDateTime.day;
            CurrentDateTime.hour     = OmxOsalDateTime.hour;
            CurrentDateTime.minutes  = OmxOsalDateTime.minutes;
            CurrentDateTime.seconds  = OmxOsalDateTime.seconds;

            version.nVersion = OMX_VERSION;

            // Get default information from sensor driver
            const t_uint32 MakerNoteSize=30*1024;
            t_uint8* MakerNote_p = new t_uint8[MakerNoteSize];
            t_uint8* XResolution_p = new t_uint8[10];
            t_uint8* YResolution_p = new t_uint8[10];
            t_uint8* MaxNumPixelsX_p = new t_uint8[10];
            t_uint8* AnalogGain_p = new t_uint8[10];
            t_uint8* DigitalGain_p = new t_uint8[10];

            Cam->cam_Ctrllib->Camera_CtrlLib_GetExifInfo(
                    Cam->cam_h,
                    MakerNoteSize,
                    MakerNote_p,
                    XResolution_p,
                    YResolution_p,
                    MaxNumPixelsX_p,
                    AnalogGain_p,
                    DigitalGain_p);

            ExifParams.pManufacturer        = (const char*)pOmxCamera->mDeviceMakeModel.cMake;
            ExifParams.pModel               = (const char*)pOmxCamera->mDeviceMakeModel.cModel;
            ExifParams.pSoftware            = (const char*)pOmxCamera->mSoftware.cSoftware;
            ExifParams.sDateTime            = CurrentDateTime;
            ExifParams.eOrientation         = pOmxCamera->mSceneOrientation.eOrientation;
            ExifParams.nXResolution         = *XResolution_p;
            ExifParams.nYResolution         = *YResolution_p;
            ExifParams.eResolutionUnit      = IFM_ResolutionUnit_Inch;
            ExifParams.eYCbCrPositioning    = IFM_YCbCrPositioning_Centered;
            ExifParams.nSensitivity         = pOmxCamera->mExposureValueType.nSensitivity;
            ExifParams.eComponentsConfig    = IFM_ComponentsConfiguration_YCbCr;
            ExifParams.nPictureNumPixelsX   = (OMX_U32)mParamPortDefinition.format.image.nFrameWidth;
            ExifParams.nPictureNumPixelsY   = (OMX_U32)mParamPortDefinition.format.image.nFrameHeight;
            ExifParams.eWhiteBalanceControl = pOmxCamera->mWhiteBalControlType.eWhiteBalControl;
            ExifParams.fFocalLength         = ((pOmxCamera->mLensParamType.nFocalLength)/(0x10000 *1.0));
            ExifParams.bPictureEnhanced     = OMX_TRUE;
            ExifParams.eSceneType           = pOmxCamera->mSceneMode.eSceneType;
            ExifParams.eMeteringMode        = pOmxCamera->mExposureValueType.eMetering;
            ExifParams.bFlashFired          = OMX_FALSE;
            ExifParams.eFlashControl        = pOmxCamera->mFlashControlStruct.eFlashControl;
            ExifParams.bFlashPresent        = OMX_FALSE;
            ExifParams.nMaxNumPixelsX       = *MaxNumPixelsX_p;
            ExifParams.nExposureTimeUsec    = pOmxCamera->mExposureValueType.nShutterSpeedMsec*1000;
            ExifParams.xFNumber             = pOmxCamera->mExposureValueType.nApertureFNumber;
            ExifParams.nBrightness          = pOmxCamera->mBrightnessType.nBrightness;
            ExifParams.nSubjectDistance     = pOmxCamera->mExtFocusStatusType.sAFROIs[0].xFocusDistance;
            ExifParams.nEvCompensation      = pOmxCamera->mExposureValueType.xEVCompensation;
            ExifParams.eExposureControl     = pOmxCamera->mExposureControlType.eExposureControl;
            ExifParams.bBracketingEnabled   = OMX_FALSE;
            ExifParams.xAnalogGain          = *AnalogGain_p;
            ExifParams.xDigitalGain         = *DigitalGain_p;
            ExifParams.nContrast            = pOmxCamera->mContrastType.nContrast;
            ExifParams.sAutoFocusROI        = pOmxCamera->mExtFocusStatusType.sAFROIs[0].nRect;
            ExifParams.pMakerNote           = (const void*)MakerNote_p;
            ExifParams.nMakerNoteSize       = sizeof(ExifParams.pMakerNote);
            ExifParams.eColorSpace          = IFM_ColorSpace_sRGB;

            DBGT_PTRACE("EnteringCapture_FillBufferDone:Set EXIF extradata flag");
            pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;

            // Add EXIF extradata to current frame
            if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
                DBGT_PTRACE("EnteringCapture_FillBufferDone: EXIF Error on pOmxBufHdr");
            else
                DBGT_PTRACE("EnteringCapture_FillBufferDone: EXIF pOmxBufHdr->nAllocLen= %d",(int)pOmxBufHdr->nAllocLen);

            DBGT_PTRACE("EnteringCapture_FillBufferDone: Set the EXIF extradata");
            err = Extradata::SetExtradataField(pOmxBufHdr, &ExifParams, version);
            if (err != OMX_ErrorNone)
            {
                DBGT_PTRACE("EnteringCapture_FillBufferDone: Failed to set the EXIF extradata err= 0x%x",err);
                DBC_ASSERT(0);
                return 0;
            }

            /* fillBufferDone transmitted to the port */
            mENSComponent.fillBufferDone(pOmxBufHdr);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::EnteringCapture);
}


/* */
/* ExitingCapture state machine */
/**/
SCF_STATE CAM_SM::ExitingCapture(s_scf_event const *e)
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
            SCF_INIT(&CAM_SM::ExitingCapture_StoppingVpip);
            return 0;
        }
    case EVT_ISPCTL_ERROR_SIG:
        {
            DBGT_PTRACE("ExitingCapture-EVT_ISPCTL_ERROR_SIG");
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
            DBGT_PTRACE("ExitingCapture-DEFERRED_EVENT;");
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingOpMode);
}


SCF_STATE CAM_SM::ExitingCapture_StoppingVpip(s_scf_event const *e)
{
    switch (e->sig) {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("ExitingCapture_StoppingVpip-SCF_STATE_ENTRY_SIG;");
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
            DBGT_PTRACE("ExitingCapture_StoppingVpip-EVT_ISPCTL_INFO_SIG;");
            Camera* Cam = (Camera*)&mENSComponent.getProcessingComponent();
            int DelayTime;
            ((Camera*)(&mENSComponent.getProcessingComponent()))->cam_Ctrllib->Camera_CtrlLib_StopSnapshot(Cam->cam_h, &DelayTime);

            SCF_TRANSIT_TO(&CAM_SM::ExitingCapture_CancelCaptureConfiguration);
            return 0;
        }
    default:
        break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingCapture);
}


SCF_STATE CAM_SM::ExitingCapture_CancelCaptureConfiguration(s_scf_event const *e)
{
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            DBGT_PTRACE("ExitingCapture_CancelCaptureConfiguration-SCF_STATE_ENTRY_SIG;");

            pOpModeMgr->UpdateCurrentOpMode();

            camport *port = (camport *)  mENSComponent.getPort(CAMERA_PORT_OUT0);
            pGrabControl->configureGrabResolutionFormat(port);

            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:  EXIT;  return 0;

        case EVT_ISPCTL_INFO_SIG:
        {
                DBGT_PTRACE("ExitingCapture_CancelCaptureConfiguration-EVT_ISPCTL_INFO_SIG;");

                if((pOpModeMgr->CurrentOperatingMode != OpMode_Cam_VideoPreview) &&
                        (pOpModeMgr->CurrentOperatingMode != OpMode_Cam_StillPreview)) {
                    // Should not occurs
                    DBGT_WARNING("ExitingCapture_CancelCaptureConfiguration: Wrong OperatingMode detected!");
                }

                SCF_TRANSIT_TO(&CAM_SM::BackPreview);
                return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::ExitingCapture);
}
