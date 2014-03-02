/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_omxcamera.h"
#include "ext_camera.h"
#include "OMX_IndexExt.h"
#include "string.h"
#include "extradata.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 0
#undef  DBGT_PREFIX
#define DBGT_PREFIX "OMX "
#include "debug_trace.h"

OMX_ERRORTYPE OMXExtCameraFactoryMethod(ENS_Component_p * ppENSComponent)
{
    DBGT_PROLOG("");

    /* Construct Proxy */
    COmxCamera * omxcam = new COmxCamera();
    if (omxcam == 0)
    {
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    *ppENSComponent = omxcam;


    Camera * cam = new Camera(*omxcam, ePrimaryExtCamera);
    if (cam == 0)
    {
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    /* set Processing Component */
    (*ppENSComponent)->setProcessingComponent(cam);

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

/** OSI Factory method for secondary camera
*/
OMX_ERRORTYPE OMXExtCameraSecondaryFactoryMethod(ENS_Component_p * ppENSComponent)
{
    DBGT_PROLOG("");

    COmxCamera * omxcam = new COmxCamera();
    if (omxcam == 0)
    {
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    *ppENSComponent = omxcam;

    Camera * cam = new Camera(*omxcam, eSecondaryExtCamera);
    if (cam == 0)
    {
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    (*ppENSComponent)->setProcessingComponent(cam);

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}


COmxCamera_RDB::COmxCamera_RDB():ENS_ResourcesDB()
{
    DBGT_PROLOG("");

    setDefaultNMFDomainType(RM_NMFD_PROCSIA);

    DBGT_EPILOG("");
}


OMX_ERRORTYPE COmxCamera::createResourcesDB()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mRMP = new COmxCamera_RDB;
    if (mRMP == 0) {
        err = OMX_ErrorInsufficientResources;
    }

    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigFlashControl(
        OMX_PTR p)
{
    mFlashControlStruct = *(OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCaptureMode(
        OMX_PTR p)
{
    mCaptureModeType = *(OMX_CONFIG_CAPTUREMODETYPE*)p;

    if(mCaptureModeType.nPortIndex != CAMERA_PORT_OUT1)
        return OMX_ErrorBadParameter;
    if(( mCaptureModeType.nFrameLimit <= 1)&&(mCaptureModeType.bFrameLimited==OMX_TRUE)) return OMX_ErrorBadParameter;
    if( mCaptureModeType.bContinuous == OMX_FALSE) return OMX_ErrorBadParameter;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonColorBlend(
        OMX_PTR p)
{
    mColorBlendType = *(OMX_CONFIG_COLORBLENDTYPE*)p;
    return OMX_ErrorNotImplemented;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigExtDigitalZoom(
        OMX_PTR p)
{
    if (OMX_ALL != ((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p)->nPortIndex)
    {
        return OMX_ErrorBadParameter;
    }
    mZoomFactorCmdType = *(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p;
    ((Camera*)(&getProcessingComponent()))->applyConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom,(void *)p);
    return OMX_ErrorNone;

}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCenterFieldOfView(
        OMX_PTR p)
{

    if (OMX_ALL != ((OMX_SYMBIAN_CONFIG_POINTTYPE*)p)->nPortIndex)
    {
        return OMX_ErrorBadParameter;
    }

    mCenterFieldOfViewCmdType = *(OMX_SYMBIAN_CONFIG_POINTTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonLightness(
        OMX_PTR p)
{
    mLightnessType = *(OMX_CONFIG_LIGHTNESSTYPE*)p;
    return OMX_ErrorNotImplemented;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonExposureValue(
        OMX_PTR p)
{
    mExposureValueType = *(OMX_CONFIG_EXPOSUREVALUETYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonExposure(
        OMX_PTR p)
{
    mExposureControlType = *(OMX_CONFIG_EXPOSURECONTROLTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonSaturation(
        OMX_PTR p)
{
    mSaturationType = *(OMX_CONFIG_SATURATIONTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonGamma(
        OMX_PTR p)
{
    mGammaType = *(OMX_CONFIG_GAMMATYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonImageFilter(
        OMX_PTR p)
{
    mImageFilterType = *(OMX_CONFIG_IMAGEFILTERTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigFocusControl(
        OMX_PTR p)
{
    mFocusControlType = *(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkSymbianIndexConfigFocusRange(
        OMX_PTR p)
{
    mFocusRangeType = *(OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkSymbianIndexConfigFocusRegion(
        OMX_PTR p)
{
    mFocusRegionType = *(OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkSymbianIndexConfigExtFocusStatus(
        OMX_PTR p)
{
    mExtFocusStatusType = *(OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCallbackRequest(
        OMX_PTR p)
{
    OMX_STATETYPE mState = OMX_StateInvalid;
    GetState(this, &mState);
    if (OMX_StateLoaded == mState)
    {
        return OMX_ErrorNotImplemented;
    }
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonFrameStabilisation(
        OMX_PTR p)
{
#ifdef STAB
    mStabType = *(OMX_CONFIG_FRAMESTABTYPE*)p;
    /* apply directly, because we need this to be enabled before passing into Idle. Note:
       p is actually unused in applyConfig */
    ((Camera*)(&getProcessingComponent()))->applyConfig(OMX_IndexConfigCommonFrameStabilisation, p);
    return OMX_ErrorNone;
#else
    return OMX_ErrorNotImplemented;
#endif
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigMotionLevel(
        OMX_PTR p)
{
    mMotionLevel = *(OMX_SYMBIAN_CONFIG_U32TYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigNDFilterControl(
        OMX_PTR p)
{
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigExposureLock(
        OMX_PTR p)
{
    mExposureLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigWhiteBalanceLock(
        OMX_PTR p)
{
    mWhiteBalanceLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigFocusLock(
        OMX_PTR p)
{
    mFocusLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigAllLock(
        OMX_PTR p)
{
    mAllLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigROI(
        OMX_PTR p)
{
    mROI = *(OMX_SYMBIAN_CONFIG_ROITYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigRAWPreset
(OMX_PTR p)
{
    if (CAMERA_PORT_OUT1 != ((OMX_SYMBIAN_CONFIG_RAWPRESETTYPE*)p)->nPortIndex)
    {
        return OMX_ErrorBadParameter;
    }
    mRAWPreset = *(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE*)p;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::checkIndexConfigCommonMirror(
        OMX_PTR p)
{
    if (OMX_ALL != ((OMX_CONFIG_MIRRORTYPE*)p)->nPortIndex)
    {
        return OMX_ErrorBadParameter;
    }
    mMirror = *(OMX_CONFIG_MIRRORTYPE*)p;
    return OMX_ErrorNone;

}


OMX_ERRORTYPE COmxCamera::setConfig(
        OMX_INDEXTYPE nIndex,
        OMX_PTR p)
{
    switch ((uint32_t)nIndex)
    {
    case OMX_IndexConfigCommonBrightness :
        mBrightnessType = *(OMX_CONFIG_BRIGHTNESSTYPE*)p;
        break;
    case OMX_IndexConfigCommonContrast:
        mContrastType = *(OMX_CONFIG_CONTRASTTYPE*)p;
        break;
    case OMX_Symbian_IndexConfigSharpness:
        mSharpness = *(OMX_SYMBIAN_CONFIG_S32TYPE *)p;
        break;
    case OMX_IndexConfigCommonWhiteBalance :
        mWhiteBalControlType = *(OMX_CONFIG_WHITEBALCONTROLTYPE*)p;
        break;
    case OMX_Symbian_IndexConfigFlickerRemoval:
        mFlickerRemoval = *(OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE*)p;
        break;
    case OMX_Symbian_IndexConfigSceneMode:
        mSceneMode = *(OMX_SYMBIAN_CONFIG_SCENEMODETYPE*)p;
        break;
    case OMX_STE_IndexConfigCommonLensParameters:
            mLensParamType = *(OMX_STE_CONFIG_LENSPARAMETERTYPE*)p;
        break;
    case OMX_IndexConfigCommonImageFilter:
        mImageFilterType = *(OMX_CONFIG_IMAGEFILTERTYPE*)p;
        break;
    case OMX_IndexConfigCommonExposureValue:
        mExposureValueType = *(OMX_CONFIG_EXPOSUREVALUETYPE*)p;
        break;
    case OMX_IndexConfigCommonExposure:
        mExposureControlType = *(OMX_CONFIG_EXPOSURECONTROLTYPE*)p;
        break;
    case OMX_IndexConfigCommonSaturation:
        mSaturationType = *(OMX_CONFIG_SATURATIONTYPE*)p;
        break;
    case OMX_IndexConfigCommonGamma:
        mGammaType = *(OMX_CONFIG_GAMMATYPE*)p;
        break;
    case OMX_Symbian_IndexConfigFlashControl:
        return (checkIndexConfigFlashControl(p));
    case OMX_Symbian_IndexConfigCommonExtCapturing:
        mCapturingStruct = *(OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p;
        if(mCapturingStruct.nPortIndex != CAMERA_PORT_OUT1)
            return OMX_ErrorBadParameter;
        return(OMX_ErrorNone);
    case OMX_IndexAutoPauseAfterCapture :
        mAutoPauseAfterCapture = *(OMX_CONFIG_BOOLEANTYPE*)p;
        break;
    case OMX_IndexConfigCaptureMode:
    case OMX_Symbian_IndexConfigExtCaptureMode:
        return(checkIndexConfigCaptureMode(p));
    case OMX_IndexConfigCommonColorBlend:
        return(checkIndexConfigCommonColorBlend(p));
    case OMX_Symbian_IndexConfigExtDigitalZoom :
        return (checkIndexConfigExtDigitalZoom(p));
    case OMX_Symbian_IndexConfigCenterFieldOfView :
        return (checkIndexConfigCenterFieldOfView(p));
    case IFM_IndexConfigTestMode :
        mTestModeType = *(OMX_STE_CONFIG_SENSORTESTTYPE*)p;
        break;
    case OMX_IndexConfigCommonLightness :
        return (checkIndexConfigCommonLightness(p));
    case OMX_IndexConfigFocusControl:
        return(checkIndexConfigFocusControl(p));
    case OMX_IndexConfigCommonFocusRegion:
        return OMX_ErrorUnsupportedIndex;
    case OMX_Symbian_IndexConfigFocusRange:
        return (checkSymbianIndexConfigFocusRange(p));
    case OMX_Symbian_IndexConfigFocusRegion:
        return (checkSymbianIndexConfigFocusRegion(p));
    case OMX_Symbian_IndexConfigExtFocusStatus:
        return (checkSymbianIndexConfigExtFocusStatus(p));
    case OMX_IndexConfigCommonFrameStabilisation:
        return(checkIndexConfigCommonFrameStabilisation(p));
    case OMX_IndexConfigCallbackRequest:
        return(checkIndexConfigCallbackRequest(p));
    case IFM_IndexConfigCommonReadPe :
        mIfmReadPEType = *(IFM_CONFIG_READPETYPE*)p;
        break;
    case IFM_IndexConfigCommonWritePe :
        mIfmWritePEType = *(IFM_CONFIG_WRITEPETYPE*)p;
        break;
    case IFM_IndexConfig3ATraceDumpControl:
        mIfm3ATraceDumpControl = *(IFM_CONFIG_3A_TRACEDUMPCONTROL*)p;
        break;
    case OMX_Symbian_IndexConfigMotionLevel:
        return checkIndexConfigMotionLevel(p);
    case OMX_Symbian_IndexConfigNDFilterControl:
        return checkIndexConfigNDFilterControl(p);
    case OMX_Symbian_IndexConfigExposureLock:
        return checkIndexConfigExposureLock(p);
    case OMX_Symbian_IndexConfigWhiteBalanceLock:
        return checkIndexConfigWhiteBalanceLock(p);
    case OMX_Symbian_IndexConfigFocusLock:
        return checkIndexConfigFocusLock(p);
    case OMX_Symbian_IndexConfigAllLock:
        return checkIndexConfigAllLock(p);
    case OMX_Symbian_IndexConfigROI:
        return checkIndexConfigROI(p);
    case OMX_Symbian_IndexConfigRAWPreset:
        return checkIndexConfigRAWPreset(p);
    case OMX_IndexConfigCommonMirror:
        return checkIndexConfigCommonMirror(p);
    case OMX_Symbian_IndexConfigCameraSensorInfo:
        return OMX_ErrorBadParameter; // read only index
    case OMX_Symbian_IndexConfigFlashGunInfo:
        return OMX_ErrorBadParameter; // read only index
    case OMX_STE_IndexConfigExtradata:
        return OMX_ErrorBadParameter; // read only index
    case OMX_Symbian_IndexConfigOrientationScene:
        {
            OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE* pOrientation = (OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE*)p;
            if ((pOrientation->eOrientation < OMX_SYMBIAN_OrientationRowTopColumnLeft) ||
                    (pOrientation->eOrientation > OMX_SYMBIAN_OrientationRowLeftColumnBottom))
            {
                return OMX_ErrorBadParameter;
            }
            mSceneOrientation = *pOrientation;
            break;
        }
    case OMX_IndexParamQFactor:
        break;
    default :
        return ENS_Component::setConfig(nIndex,p);
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::getConfig(
        OMX_INDEXTYPE idx,
        OMX_PTR p) const
{
    switch ((uint32_t)idx)
    {
    case OMX_Symbian_IndexConfigFlashControl:
        memcpy(p, &mFlashControlStruct, sizeof(OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE));
        break;
    case OMX_Symbian_IndexConfigCommonExtCapturing:
        memcpy(p, &mCapturingStruct, sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE));
        break;
    case OMX_IndexAutoPauseAfterCapture :
        memcpy(p, &mAutoPauseAfterCapture, sizeof(OMX_CONFIG_BOOLEANTYPE));
        break;
    case OMX_IndexConfigCaptureMode:
        memcpy(p, &mCaptureModeType, sizeof(OMX_CONFIG_CAPTUREMODETYPE));
        break;
    case OMX_IndexConfigCommonColorBlend:
        memcpy(p, &mColorBlendType, sizeof(OMX_CONFIG_SCALEFACTORTYPE));
        break;
    case OMX_Symbian_IndexConfigExtDigitalZoom :
        memcpy(p, &mZoomFactorCmdType, sizeof(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE));
        break;
    case OMX_Symbian_IndexConfigCenterFieldOfView :
        memcpy(p, &mCenterFieldOfViewStatusType, sizeof(OMX_SYMBIAN_CONFIG_POINTTYPE));
        break;
    case IFM_IndexConfigTestMode :
        memcpy(p, &mTestModeType, sizeof(OMX_STE_CONFIG_SENSORTESTTYPE));
        break;
    case OMX_IndexConfigCommonWhiteBalance :
        memcpy(p, &mWhiteBalControlType, sizeof(OMX_CONFIG_WHITEBALCONTROLTYPE));
        break;
    case OMX_IndexConfigCommonBrightness :
        memcpy(p, &mBrightnessType, sizeof(OMX_CONFIG_BRIGHTNESSTYPE));
        break;
    case OMX_IndexConfigCommonLightness :
        memcpy(p, &mLightnessType, sizeof(OMX_CONFIG_LIGHTNESSTYPE));
        break;
    case OMX_IndexConfigCommonExposureValue:
        memcpy(p, &mExposureValueType, sizeof(OMX_CONFIG_EXPOSUREVALUETYPE));
        break;
    case OMX_IndexConfigCommonExposure:
        memcpy(p, &mExposureControlType, sizeof(OMX_CONFIG_EXPOSURECONTROLTYPE));
        break;
    case OMX_IndexConfigCommonContrast:
        memcpy(p, &mContrastType, sizeof(OMX_CONFIG_CONTRASTTYPE));
        break;
    case OMX_Symbian_IndexConfigSharpness:
        memcpy(p, &mSharpness, sizeof(OMX_SYMBIAN_CONFIG_S32TYPE ));
        break;
    case OMX_IndexConfigCommonSaturation:
        memcpy(p, &mSaturationType, sizeof(OMX_CONFIG_SATURATIONTYPE));
        break;
    case OMX_IndexConfigCommonGamma:
        memcpy(p, &mGammaType, sizeof(OMX_CONFIG_GAMMATYPE));
        break;
    case OMX_IndexConfigCommonImageFilter:
        memcpy(p, &mImageFilterType, sizeof(OMX_CONFIG_IMAGEFILTERTYPE));
        break;
    case OMX_IndexConfigFocusControl:
        memcpy(p, &mFocusControlType, sizeof(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE));
        break;
    case OMX_IndexConfigCommonFocusRegion:
        memcpy(p, &mFocusRegionType, sizeof(OMX_CONFIG_FOCUSREGIONTYPE));
        break;
    case OMX_Symbian_IndexConfigFocusRange:
        memcpy(p, &mFocusRangeType, sizeof(OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE));
        break;
    case OMX_Symbian_IndexConfigExtFocusStatus:
        memcpy(p, &mExtFocusStatusType, sizeof(OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE));
        break;
    case OMX_IndexConfigCallbackRequest:
        {
            OMX_CONFIG_CALLBACKREQUESTTYPE* pCallbackRequest = (OMX_CONFIG_CALLBACKREQUESTTYPE *) p;
            switch((uint32_t)pCallbackRequest->nIndex) {
            case OMX_Symbian_IndexConfigExposureLock:
                memcpy(p, &mExposureLockClbkReq, sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE));
                break;
            case OMX_Symbian_IndexConfigWhiteBalanceLock:
                memcpy(p, &mWhiteBalanceLockClbkReq, sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE));
                break;
            case OMX_Symbian_IndexConfigFocusLock:
                memcpy(p, &mFocusLockClbkReq, sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE));
                break;
            case OMX_Symbian_IndexConfigAllLock:
                memcpy(p, &mAllLockClbkReq, sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE));
                break;
            default: // callback is not supported/implemented
                ;
            }
            break;
        }
    case OMX_IndexConfigCommonFrameStabilisation:
        memcpy(p, &mStabType, sizeof(OMX_CONFIG_FRAMESTABTYPE));
        break;
    case IFM_IndexConfigCommonReadPe :
        memcpy(p, &mIfmReadPEType, sizeof(IFM_CONFIG_READPETYPE));
        break;
    case IFM_IndexConfigCommonWritePe :
        memcpy(p, &mIfmWritePEType, sizeof(IFM_CONFIG_WRITEPETYPE));
        break;
    case IFM_IndexConfigCommonOpMode_Check :
        {
            IFM_CONFIG_OPMODE_CHECK* tmpStruct = (IFM_CONFIG_OPMODE_CHECK*)p;
            CopModeMgr* tmpOpMode    = & ( ((Camera*)(&getProcessingComponent()))->iOpModeMgr );
            tmpStruct->eOpMode = tmpOpMode->CurrentOperatingMode;
        }
        break;
    case OMX_Symbian_IndexConfigRAWPreset:
        memcpy(p, &mRAWPreset, sizeof(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE));
        break;
    case OMX_IndexConfigCommonMirror:
        memcpy(p, &mMirror, sizeof(OMX_CONFIG_MIRRORTYPE));
        break;
    case OMX_Symbian_IndexConfigCameraSensorInfo:
        memcpy(p, &mSensorInfo, sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE));
        break;
    case OMX_Symbian_IndexConfigFlashGunInfo:
        memcpy(p, &mFlashgunInfo, sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE));
        break;
    case OMX_Symbian_IndexConfigSceneMode:
        memcpy(p, &mSceneMode, sizeof(OMX_SYMBIAN_CONFIG_SCENEMODETYPE));
        break;
    case OMX_STE_IndexConfigCommonLensParameters:
        memcpy(p, &mLensParamType, sizeof(OMX_STE_CONFIG_LENSPARAMETERTYPE));
        break;
    case OMX_Symbian_IndexConfigFlickerRemoval:
        memcpy(p, &mFlickerRemoval, sizeof(OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE));
        break;
    case OMX_STE_IndexConfigExtradata:
        {
            OMX_SYMBIAN_CONFIG_EXIFSETTINGSTYPE *pExifSettings = (OMX_SYMBIAN_CONFIG_EXIFSETTINGSTYPE *)p;

            if(pExifSettings->pOmxBufHdr == NULL){
                DBGT_PTRACE("getParameter ERROR No input buffer to process\n");
                return OMX_ErrorBadParameter;
            }

            IFM_EXIF_SETTINGS ExifSettings;

            DBGT_PTRACE("getParameter for Exif Settings\n");
            OMX_ERRORTYPE status = Extradata::GetExtradataField(pExifSettings->pOmxBufHdr, &ExifSettings, pExifSettings->nVersion);
            DBGT_PTRACE("getParameter for Exif Settings passed\n");
            pExifSettings->nSize = sizeof(OMX_SYMBIAN_CONFIG_EXIFSETTINGSTYPE);
            pExifSettings->pManufacturer = ExifSettings.pManufacturer;
            pExifSettings->pModel = ExifSettings.pModel;
            pExifSettings->pSoftware = ExifSettings.pSoftware;
            pExifSettings->sDateTime.year = ExifSettings.sDateTime.year;
            pExifSettings->sDateTime.month = ExifSettings.sDateTime.month;
            pExifSettings->sDateTime.day = ExifSettings.sDateTime.day;
            pExifSettings->sDateTime.hour = ExifSettings.sDateTime.hour;
            pExifSettings->sDateTime.minutes = ExifSettings.sDateTime.minutes;
            pExifSettings->sDateTime.seconds = ExifSettings.sDateTime.seconds;
            pExifSettings->eOrientation = ExifSettings.eOrientation;
            pExifSettings->nXResolution = ExifSettings.nXResolution;
            pExifSettings->nYResolution = ExifSettings.nYResolution;
            pExifSettings->eResolutionUnit = (OMX_SYMBIAN_RESOLUTION_UNITTYPE)ExifSettings.eResolutionUnit;
            pExifSettings->eYCbCrPositioning = (OMX_SYMBIAN_YCBCR_POSITIONINGTYPE)ExifSettings.eYCbCrPositioning;
            pExifSettings->nSensitivity = ExifSettings.nSensitivity;
            pExifSettings->eComponentsConfig = (OMX_SYMBIAN_COMPONENTS_CONFIGURATIONTYPE)ExifSettings.eComponentsConfig;
            pExifSettings->nPictureNumPixelsX = ExifSettings.nPictureNumPixelsX;
            pExifSettings->nPictureNumPixelsY = ExifSettings.nPictureNumPixelsY;
            pExifSettings->eWhiteBalanceControl = ExifSettings.eWhiteBalanceControl;
            pExifSettings->fFocalLength = ExifSettings.fFocalLength;
            pExifSettings->bPictureEnhanced = ExifSettings.bPictureEnhanced;
            pExifSettings->eSceneType = ExifSettings.eSceneType;
            pExifSettings->eMeteringMode = ExifSettings.eMeteringMode;
            pExifSettings->bFlashFired = ExifSettings.bFlashFired;
            pExifSettings->eFlashControl = ExifSettings.eFlashControl;
            pExifSettings->bFlashPresent = ExifSettings.bFlashPresent;
            pExifSettings->nMaxNumPixelsX = ExifSettings.nMaxNumPixelsX;
            pExifSettings->nExposureTimeUsec = ExifSettings.nExposureTimeUsec;
            pExifSettings->xFNumber = ExifSettings.xFNumber;
            pExifSettings->nBrightness = ExifSettings.nBrightness;
            pExifSettings->nSubjectDistance = ExifSettings.nSubjectDistance;
            pExifSettings->nEvCompensation = ExifSettings.nEvCompensation;
            pExifSettings->eExposureControl = ExifSettings.eExposureControl;
            pExifSettings->bBracketingEnabled = ExifSettings.bBracketingEnabled;
            pExifSettings->xAnalogGain = ExifSettings.xAnalogGain;
            pExifSettings->xDigitalGain = ExifSettings.xDigitalGain;
            pExifSettings->nContrast = ExifSettings.nContrast;
            pExifSettings->sAutoFocusROI.sTopLeft.nX = ExifSettings.sAutoFocusROI.sTopLeft.nX;
            pExifSettings->sAutoFocusROI.sTopLeft.nY = ExifSettings.sAutoFocusROI.sTopLeft.nY;
            pExifSettings->sAutoFocusROI.sSize.nWidth = ExifSettings.sAutoFocusROI.sSize.nWidth;
            pExifSettings->sAutoFocusROI.sSize.nHeight = ExifSettings.sAutoFocusROI.sSize.nHeight;
            pExifSettings->pMakerNote = ExifSettings.pMakerNote;
            pExifSettings->nMakerNoteSize = ExifSettings.nMakerNoteSize;
            pExifSettings->eColorSpace = (OMX_SYMBIAN_COLOR_SPACETYPE)ExifSettings.eColorSpace;
            break;
        }
    case OMX_Symbian_IndexConfigOrientationScene:
        memcpy(p, &mSceneOrientation, sizeof(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE));
        break;
    case OMX_Symbian_IndexConfigSupportedZoomResolutions:
        memcpy(p, &mZoomSupportedRes,sizeof(OMX_SYMBIAN_SUPPORTED_ZOOM_RESOLUTIOINS));
        break;
    default :
        return ENS_Component::getConfig(idx,p);
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE COmxCamera::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (pComponentParameterStructure == 0)
    {
        err = OMX_ErrorBadParameter;
        goto end;
    }
    DBGT_PTRACE("nParamIndex(OMX_INDEXTYPE) is %d\n",nParamIndex);
    switch ((uint32_t)nParamIndex)
    {
    case OMX_Symbian_IndexParamDeviceMakeModel:
        {
            mDeviceMakeModel = *(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL*)pComponentParameterStructure;
            DBGT_PTRACE("Device Make='%s' Model='%s'\n", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
            break;
        }
    case IFM_IndexConfigSoftware:
        {
            mSoftware= *(OMX_SYMBIAN_PARAM_SOFTWARE*)pComponentParameterStructure;
            DBGT_PTRACE("Device Software='%s\n", mSoftware.cSoftware);
            break;
        }
    default:
        err = ENS_Component::setParameter(nParamIndex,pComponentParameterStructure);
        break;
    }

end:
    DBGT_EPILOG("");
    return err;
}


OMX_BOOL COmxCamera::isPortSpecificParameter(
        OMX_INDEXTYPE nParamIndex) const
{
    switch (nParamIndex)
    {

    case OMX_IndexParamCommonSensorMode:
        DBGT_PTRACE("OMX_IndexParamCommonSensorMode: ") ;
        DBGT_PTRACE("isPortSpecificParameter TRUE\n") ;
        return OMX_TRUE;
    default :
        return(ENS_Component::isPortSpecificParameter(nParamIndex));
    }
}


OMX_ERRORTYPE COmxCamera::getParameter(
        OMX_INDEXTYPE idx,
        OMX_PTR p) const
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (p == 0)
    {
        err = OMX_ErrorBadParameter;
        goto end;
    }

    switch ((uint32_t)idx)
    {
    case OMX_IndexParamVideoInit :
        {
            OMX_PORT_PARAM_TYPE * portTypesParam = (OMX_PORT_PARAM_TYPE *) p;
            portTypesParam->nPorts = CAMERA_NB_OUT_PORTS;
            portTypesParam->nStartPortNumber = 0;
            portTypesParam->nSize = sizeof(OMX_PORT_PARAM_TYPE);
            portTypesParam->nVersion = getVersion();
        }
        break;
    case OMX_Symbian_IndexParamDeviceMakeModel:
        {
            memcpy(p, &mDeviceMakeModel, sizeof(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL));
            DBGT_PTRACE("Device Make='%s' Model='%s'\n", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
            break;
        }
    case IFM_IndexConfigSoftware:
        {
            memcpy(p, &mSoftware, sizeof(OMX_SYMBIAN_PARAM_SOFTWARE));
            DBGT_PTRACE("Software='%s'\n", mSoftware.cSoftware);
            break;
        }
    case OMX_IndexParamOtherPrivateContext:
        {
            OMX_OTHER_PARAM_PRIVATE_CONTEXT *param = (OMX_OTHER_PARAM_PRIVATE_CONTEXT *) p;

            ((Camera*)(&getProcessingComponent()))->printPrivateContext(param);

            break;
        }
    default :
        err = ENS_Component::getParameter(idx,p);
        break;
    }

end:
    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE COmxCamera::getExtensionIndex(
        OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_IMAGE_MAXJPEGSIZE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigImageMaxJpegSize;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERAFUNCTESTSELECT_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraFuncTestSelect;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERASELFTESTSELECT_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraSelfTestSelect;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERATESTING_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraTesting;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SCENEMODE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSceneMode;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_COMMON_LENSPARAMETER, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigCommonLensParameters;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_RGBHISTOGRAM_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRGBHistogram;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HISTOGRAMCONTROL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHistogramControl;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_XENONLIFECOUNTER_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigXenonLifeCounter;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ROISELECTION_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigROISelection;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_REDCOMPLEXITY_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigREDComplexity;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_REDEYEREMOVAL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRedEyeRemoval;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ORIENTATIONSCENE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigOrientationScene;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_PARAM_COMMON_REVERTASPECTRATIO_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexParamRevertAspectRatio;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_NDFILTERCONTROL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigNDFilterControl;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXPOSURELOCK_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureLock;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_WHITEBALANCELOCK_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigWhiteBalanceLock;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSLOCK_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusLock;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ALLLOCK_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAllLock;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CENTERFIELDOFVIEW_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCenterFieldOfView;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTDIGITALZOOM_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTOPTICALZOOM_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtOpticalZoom;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSRANGE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRange;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTFOCUSSTATUS_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtFocusStatus;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLICKERREMOVAL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlickerRemoval;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLASHCONTROL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashControl;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_AFASSISTANTLIGHT_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAFAssistantLight;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HINTPOWERVSQUALITY_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHintPowerVsQuality;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HINTDEPTHOFFIELD_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHintDepthOfField;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSREGION_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRegion;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ROI_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigROI;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERASENSORINFO_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraSensorInfo;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLASHGUNINFO_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashGunInfo;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_MOTIONLEVEL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigMotionLevel;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SHARPNESS_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSharpness;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTURING_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCommonExtCapturing;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTUREMODE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtCaptureMode;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_XENONFLASHSTATUS_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigXenonFlashStatus;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_PRECAPTUREEXPOSURETIME_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigPreCaptureExposureTime;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXPOSUREINITIATED_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureInitiated;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_BRACKETING_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigBracketing;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_RAWPRESET_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRAWPreset;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_APPLYUSERSETTINGS_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigApplyUserSettings;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_VIDEOLIGHT_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigVideoLight;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_PARAM_COMMON_DEVICEMAKEMODEL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexParamDeviceMakeModel;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTRADATA_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigExtradata;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_DEVICESOFTWARE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)IFM_IndexConfigSoftware;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_ZOOM_SUPPORTED_RESOLUTIONS, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSupportedZoomResolutions;
    }
    else
    {
        err = OMX_ErrorNotImplemented;
    }

    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE COmxCamera::createCamPort(
        OMX_U32 nPortIndex,
        OMX_DIRTYPE eDir,
        OMX_BUFFERSUPPLIERTYPE eSupplierPref)
{
    ENS_Port * port;
    EnsCommonPortData commonPortData(nPortIndex,eDir,1,0,OMX_PortDomainVideo,eSupplierPref);
    port = new camport(commonPortData, *this);
    if (port == NULL) return OMX_ErrorInsufficientResources;
    addPort(port);
    return OMX_ErrorNone;
}


COmxCamera::COmxCamera(): ENS_Component()
{

    ENS_Component::construct(CAMERA_NB_PORTS);
    OMX_ERRORTYPE error;
    error = createCamPort(CAMERA_PORT_OUT0, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createCamPort(CAMERA_PORT_OUT1, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!

    /* Initialise _all_ OMX config with default values */
    defaultConfig(0);
    defaultConfig(OMX_IndexConfigCommonBrightness);
    defaultConfig(OMX_IndexConfigCommonContrast);
    defaultConfig(OMX_Symbian_IndexConfigSharpness);
    defaultConfig(OMX_IndexConfigCommonWhiteBalance);
    defaultConfig(OMX_Symbian_IndexConfigFlickerRemoval);
    defaultConfig(OMX_Symbian_IndexConfigSceneMode);
    defaultConfig(OMX_IndexConfigCommonImageFilter);
    defaultConfig(OMX_IndexConfigCommonExposureValue);
    defaultConfig(OMX_IndexConfigCommonExposure);
    defaultConfig(OMX_IndexConfigCommonSaturation);
    defaultConfig(OMX_IndexConfigCommonGamma);
    defaultConfig(OMX_Symbian_IndexConfigFlashControl);
    defaultConfig(OMX_Symbian_IndexConfigCommonExtCapturing);
    defaultConfig(OMX_IndexAutoPauseAfterCapture);
    defaultConfig(OMX_IndexConfigCaptureMode);
    defaultConfig(OMX_Symbian_IndexConfigExtCaptureMode);
    defaultConfig(OMX_IndexConfigCommonColorBlend);
    defaultConfig(OMX_Symbian_IndexConfigExtDigitalZoom);
    defaultConfig(OMX_Symbian_IndexConfigCenterFieldOfView);
    defaultConfig(IFM_IndexConfigTestMode);
    defaultConfig(OMX_IndexConfigCommonLightness);
    defaultConfig(OMX_IndexConfigFocusControl);
    defaultConfig(OMX_IndexConfigCommonFocusRegion);
    defaultConfig(OMX_Symbian_IndexConfigFocusRegion);
    defaultConfig(OMX_Symbian_IndexConfigFocusRange);
    defaultConfig(OMX_Symbian_IndexConfigExtFocusStatus);
    defaultConfig(OMX_IndexConfigCommonFrameStabilisation);
    defaultConfig(OMX_IndexConfigCallbackRequest);
    defaultConfig(IFM_IndexConfigCommonReadPe);
    defaultConfig(IFM_IndexConfigCommonWritePe);
    defaultConfig(IFM_IndexConfig3ATraceDumpControl);
    defaultConfig(OMX_Symbian_IndexConfigMotionLevel);
    defaultConfig(OMX_Symbian_IndexConfigNDFilterControl);
    defaultConfig(OMX_Symbian_IndexConfigExposureLock);
    defaultConfig(OMX_Symbian_IndexConfigWhiteBalanceLock);
    defaultConfig(OMX_Symbian_IndexConfigFocusLock);
    defaultConfig(OMX_Symbian_IndexConfigAllLock);
    defaultConfig(OMX_Symbian_IndexConfigROI);
    defaultConfig(OMX_Symbian_IndexConfigRAWPreset);
    defaultConfig(OMX_IndexConfigCommonMirror);
    defaultConfig(OMX_Symbian_IndexConfigCameraSensorInfo);
    defaultConfig(OMX_Symbian_IndexConfigFlashGunInfo);
    defaultConfig(OMX_Symbian_IndexParamDeviceMakeModel);
    defaultConfig(IFM_IndexConfigSoftware);
    defaultConfig(OMX_Symbian_IndexConfigOrientationScene);
    defaultConfig(OMX_STE_IndexConfigCommonLensParameters);
}

/* COmxCamera::defaultConfig
 * This method initialises the OMX config related to the specified index with the default configuration, currently hardcoded.
 * It is expected to be called for all existing config at camera instantiation.
 * If the IL client makes a setConfig in LOADED with an erroneous config, the camera detects it only when switching to IDLE.
 * Then, this method can be called on the considered index to initialise it with the default value instead of the erroneous value.
 *
 * This method is also supposed to be a reference of all index used by the omxcamera, along with the type of their parameter and
 * the name of their associated config variable.
 * The setConfig or other methods below may use only a subset of them, but this method must list them all.
 *
 * TODO: nSize, nVersion, nPort
 */
#define OMX_VERSION_1_1_2 0x00020101 // Not using OMX_VERSION because it might change in future OMX headers before the camera implementation actually supports the new version
void COmxCamera::defaultConfig(unsigned int nIndex)
{
    switch (nIndex)
    {
    case 0: /* other initialisations */
        /* index init template
           mXxxYyy.nSize = sizeof(OMX_CONFIG_XXXYYYTYPE);
           mXxxYyy.nVersion.nVersion = OMX_VERSION_1_1_2;
           mXxxYyy.nPortIndex = OMX_ALL; // wrong?
           */
        break;
    case OMX_IndexConfigCommonBrightness:
        mBrightnessType.nSize = sizeof(OMX_CONFIG_BRIGHTNESSTYPE);
        mBrightnessType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mBrightnessType.nPortIndex = OMX_ALL; // wrong?
        mBrightnessType.nBrightness = 50;
        break;
    case OMX_IndexConfigCommonContrast:
        mContrastType.nSize = sizeof(OMX_CONFIG_CONTRASTTYPE);
        mContrastType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mContrastType.nPortIndex = OMX_ALL; // wrong?
        mContrastType.nContrast = 0;
        break;
    case OMX_Symbian_IndexConfigSharpness:
        mSharpness.nSize = sizeof(OMX_SYMBIAN_CONFIG_S32TYPE);
        mSharpness.nVersion.nVersion = OMX_VERSION_1_1_2;
        mSharpness.nPortIndex = OMX_ALL; // wrong?
        mSharpness.nValue = 0;
        break;
    case OMX_IndexConfigCommonWhiteBalance:
        mWhiteBalControlType.nSize = sizeof(OMX_CONFIG_WHITEBALCONTROLTYPE);
        mWhiteBalControlType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mWhiteBalControlType.nPortIndex = OMX_ALL; // wrong?
        mWhiteBalControlType.eWhiteBalControl = OMX_WhiteBalControlAuto;
        break;
    case OMX_Symbian_IndexConfigFlickerRemoval:
        mFlickerRemoval.nSize = sizeof(OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE);
        mFlickerRemoval.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFlickerRemoval.nPortIndex = OMX_ALL; // wrong?
        mFlickerRemoval.eFlickerRemoval = OMX_SYMBIAN_FlickerRemovalOff;
        break;
    case OMX_Symbian_IndexConfigSceneMode:
        mSceneMode.nSize = sizeof(OMX_SYMBIAN_CONFIG_SCENEMODETYPE);
        mSceneMode.nVersion.nVersion = OMX_VERSION_1_1_2;
        mSceneMode.nPortIndex = OMX_ALL; // wrong?
        mSceneMode.eSceneType = OMX_SYMBIAN_SceneAuto;
        break;
    case OMX_IndexConfigCommonImageFilter:
        mImageFilterType.nSize = sizeof(OMX_CONFIG_IMAGEFILTERTYPE);
        mImageFilterType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mImageFilterType.nPortIndex = OMX_ALL; // wrong?
        mImageFilterType.eImageFilter = OMX_ImageFilterNone;
        break;
    case OMX_IndexConfigCommonExposureValue:
        mExposureValueType.nSize = sizeof(OMX_CONFIG_EXPOSUREVALUETYPE);
        mExposureValueType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mExposureValueType.nPortIndex = OMX_ALL; // wrong?
        mExposureValueType.eMetering = OMX_MeteringModeSpot;
        mExposureValueType.xEVCompensation = 0 << 16;
        mExposureValueType.nApertureFNumber = 2;
        mExposureValueType.bAutoAperture = OMX_TRUE;
        mExposureValueType.nShutterSpeedMsec = 20;
        mExposureValueType.bAutoShutterSpeed = OMX_TRUE;
        mExposureValueType.nSensitivity = 100;
        mExposureValueType.bAutoSensitivity = OMX_TRUE;
        break;
    case OMX_IndexConfigCommonExposure:
        mExposureControlType.nSize = sizeof(OMX_CONFIG_EXPOSURECONTROLTYPE);
        mExposureControlType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mExposureControlType.nPortIndex = OMX_ALL; // wrong?
        mExposureControlType.eExposureControl = OMX_ExposureControlAuto;
        break;
    case OMX_IndexConfigCommonSaturation:
        mSaturationType.nSize = sizeof(OMX_CONFIG_SATURATIONTYPE);
        mSaturationType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mSaturationType.nPortIndex = OMX_ALL; // wrong?
        mSaturationType.nSaturation = 0;
        break;
    case OMX_IndexConfigCommonGamma:
        mGammaType.nSize = sizeof(OMX_CONFIG_GAMMATYPE);
        mGammaType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mGammaType.nPortIndex = OMX_ALL; // wrong?
        mGammaType.nGamma = 0;
        break;
    case OMX_Symbian_IndexConfigFlashControl:
        mFlashControlStruct.nSize = sizeof(OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE);
        mFlashControlStruct.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFlashControlStruct.nPortIndex = OMX_ALL; // wrong?
        mFlashControlStruct.eFlashControl = OMX_IMAGE_FlashControlAuto;
        break;
    case OMX_Symbian_IndexConfigCommonExtCapturing:
        mCapturingStruct.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
        mCapturingStruct.nVersion.nVersion = OMX_VERSION_1_1_2;
        mCapturingStruct.nPortIndex = CAMERA_PORT_OUT1;
        mCapturingStruct.bEnabled = OMX_FALSE;
        break;
    case OMX_IndexAutoPauseAfterCapture:
        mAutoPauseAfterCapture.nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
        mAutoPauseAfterCapture.nVersion.nVersion = OMX_VERSION_1_1_2;
        mAutoPauseAfterCapture.bEnabled = OMX_FALSE;
        break;
    case OMX_IndexConfigCaptureMode:
    case OMX_Symbian_IndexConfigExtCaptureMode:
        mCaptureModeType.nSize = sizeof(OMX_CONFIG_CAPTUREMODETYPE);
        mCaptureModeType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mCaptureModeType.nPortIndex = OMX_ALL; // wrong?
        mCaptureModeType.bContinuous = OMX_TRUE;
        mCaptureModeType.bFrameLimited = OMX_TRUE;
        mCaptureModeType.nFrameLimit = 1;
        break;
    case OMX_IndexConfigCommonColorBlend:
        mColorBlendType.nSize = sizeof(OMX_CONFIG_COLORBLENDTYPE);
        mColorBlendType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mColorBlendType.nPortIndex = OMX_ALL; // wrong?
        mColorBlendType.nRGBAlphaConstant = 0;
        mColorBlendType.eColorBlend = OMX_ColorBlendNone;
        break;
    case OMX_Symbian_IndexConfigExtDigitalZoom:
        // TODO: check that it is correct. Indeed, it is maybe also initialised somewhere else
        mZoomFactorCmdType.nSize = sizeof(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE);
        mZoomFactorCmdType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mZoomFactorCmdType.nPortIndex = OMX_ALL; // wrong?
        mZoomFactorCmdType.xZoomFactor.nMin   = 1 << 16;
        mZoomFactorCmdType.xZoomFactor.nMax   = 5 << 16;
        mZoomFactorCmdType.xZoomFactor.nValue = 1 << 16;
        break;
    case OMX_Symbian_IndexConfigCenterFieldOfView:
        // TODO: check that it is correct. Indeed, it is maybe also initialised somewhere else
        mCenterFieldOfViewCmdType.nSize = sizeof(OMX_SYMBIAN_CONFIG_POINTTYPE);
        mCenterFieldOfViewCmdType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mCenterFieldOfViewCmdType.nPortIndex = OMX_ALL; // wrong?
        mCenterFieldOfViewCmdType.sPoint.nX = 0;
        mCenterFieldOfViewCmdType.sPoint.nY = 0;
        break;
    case IFM_IndexConfigTestMode:
        mTestModeType.nSize = sizeof(OMX_STE_CONFIG_SENSORTESTTYPE);
        mTestModeType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mTestModeType.nPortIndex = OMX_ALL; // wrong?
        mTestModeType.eTestMode = OMX_STE_TestModeNone;
        mTestModeType.nSolidBar_R = 186; // (186,107,20) represent a rather warm brown color.
        mTestModeType.nSolidBar_G = 107;
        mTestModeType.nSolidBar_B = 20;
        break;
    case OMX_IndexConfigCommonLightness:
        mLightnessType.nSize = sizeof(OMX_CONFIG_LIGHTNESSTYPE);
        mLightnessType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mLightnessType.nPortIndex = OMX_ALL; // wrong?
        mLightnessType.nLightness = 0;
        break;
    case OMX_IndexConfigFocusControl:
        mFocusControlType.nSize = sizeof(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE);
        mFocusControlType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFocusControlType.nPortIndex = OMX_ALL; // wrong?
        mFocusControlType.eFocusControl = OMX_IMAGE_FocusControlAuto;
        mFocusControlType.nFocusSteps = 10;
        mFocusControlType.nFocusStepIndex = 0;
        break;
    case OMX_IndexConfigCommonFocusRegion:
    case OMX_Symbian_IndexConfigFocusRegion:
        mFocusRegionType.nSize = sizeof(OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE);
        mFocusRegionType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFocusRegionType.nPortIndex = OMX_ALL; // wrong?
        mFocusRegionType.eFocusRegionControl = OMX_SYMBIAN_FocusRegionAuto;
        mFocusRegionType.sFocusRegion.sRect.sTopLeft.nX = 0;
        mFocusRegionType.sFocusRegion.sRect.sTopLeft.nY = 0;
        mFocusRegionType.sFocusRegion.sRect.sSize.nWidth = 0;
        mFocusRegionType.sFocusRegion.sRect.sSize.nHeight = 0;
        mFocusRegionType.sFocusRegion.sReference.nWidth = 0;
        mFocusRegionType.sFocusRegion.sReference.nHeight = 0;
        break;
    case OMX_Symbian_IndexConfigFocusRange:
        mFocusRangeType.nSize = sizeof(OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE);
        mFocusRangeType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFocusRangeType.nPortIndex = OMX_ALL; // wrong?
        mFocusRangeType.eFocusRange = OMX_SYMBIAN_FocusRangeAuto;
        break;
    case OMX_Symbian_IndexConfigExtFocusStatus:
        mExtFocusStatusType.nSize = sizeof(OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE);
        mExtFocusStatusType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mExtFocusStatusType.bFocused = OMX_FALSE;
        mExtFocusStatusType.nMaxAFAreas = 10;
        mExtFocusStatusType.nAFAreas = 0;
        mExtFocusStatusType.sAFROIs[0].nReferenceWindow.nWidth = 50;
        mExtFocusStatusType.sAFROIs[0].nReferenceWindow.nHeight= 50;
        mExtFocusStatusType.sAFROIs[0].xFocusDistance = 1 << 16;
        mExtFocusStatusType.sAFROIs[0].nRect.sTopLeft.nX = 0;
        mExtFocusStatusType.sAFROIs[0].nRect.sTopLeft.nY = 0;
        mExtFocusStatusType.sAFROIs[0].nRect.sSize.nWidth  = 20;
        mExtFocusStatusType.sAFROIs[0].nRect.sSize.nHeight = 20;
        mExtFocusStatusType.sAFROIs[0].eFocusStatus = OMX_FocusStatusOff;
        break;
    case OMX_IndexConfigCommonFrameStabilisation:
        mStabType.nSize = sizeof(OMX_CONFIG_FRAMESTABTYPE);
        mStabType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mStabType.nPortIndex = OMX_ALL; // wrong?
        mStabType.bStab = OMX_FALSE;
        break;
    case OMX_IndexConfigCallbackRequest:
        // TODO: something
        break;
    case IFM_IndexConfigCommonReadPe:
        mIfmReadPEType.nSize = sizeof(IFM_CONFIG_READPETYPE);
        mIfmReadPEType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mIfmReadPEType.nPortIndex = OMX_ALL; // wrong?
        mIfmReadPEType.nPEToRead = 0;
        for (int i=0; i<IFM_SIZE_LIST_PE_READ; i++)
        {
            mIfmReadPEType.nPEList_addr[i]=0;
            mIfmReadPEType.nPEList_data[i]=0;
        }
        break;
    case IFM_IndexConfigCommonWritePe:
        mIfmWritePEType.nSize = sizeof(IFM_CONFIG_WRITEPETYPE);
        mIfmWritePEType.nVersion.nVersion = OMX_VERSION_1_1_2;
        mIfmWritePEType.nPortIndex = OMX_ALL; // wrong?
        mIfmWritePEType.nPEToWrite = 0;
        for (int i=0; i<IFM_SIZE_LIST_PE_WRITE; i++)
        {
            mIfmWritePEType.nPEList_addr[i]=0;
            mIfmWritePEType.nPEList_data[i]=0;
        }
        break;
    case IFM_IndexConfig3ATraceDumpControl:
        mIfm3ATraceDumpControl.nSize = sizeof(IFM_CONFIG_3A_TRACEDUMPCONTROL);
        mIfm3ATraceDumpControl.nVersion.nVersion = OMX_VERSION_1_1_2;
        mIfm3ATraceDumpControl.nPortIndex = OMX_ALL; // wrong?
        mIfm3ATraceDumpControl.bEnabled= OMX_FALSE;
        break;
    case OMX_Symbian_IndexConfigMotionLevel:
        mMotionLevel.nSize = sizeof(OMX_SYMBIAN_CONFIG_U32TYPE);
        mMotionLevel.nVersion.nVersion = OMX_VERSION_1_1_2;
        mMotionLevel.nPortIndex = OMX_ALL; // wrong?
        mMotionLevel.nValue = 0;
        break;
    case OMX_Symbian_IndexConfigNDFilterControl:
        break;
    case OMX_Symbian_IndexConfigExposureLock:
        mExposureLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
        mExposureLock.nVersion.nVersion = OMX_VERSION_1_1_2;
        mExposureLock.nPortIndex = OMX_ALL; // wrong?
        mExposureLock.eImageLock = OMX_SYMBIAN_LockOff;
        break;
    case OMX_Symbian_IndexConfigWhiteBalanceLock:
        mWhiteBalanceLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
        mWhiteBalanceLock.nVersion.nVersion = OMX_VERSION_1_1_2;
        mWhiteBalanceLock.nPortIndex = OMX_ALL; // wrong?
        mWhiteBalanceLock.eImageLock = OMX_SYMBIAN_LockOff;
        break;
    case OMX_Symbian_IndexConfigFocusLock:
        mFocusLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
        mFocusLock.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFocusLock.nPortIndex = OMX_ALL; // wrong?
        mFocusLock.eImageLock = OMX_SYMBIAN_LockOff;
        break;
    case OMX_Symbian_IndexConfigAllLock:
        mAllLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
        mAllLock.nVersion.nVersion = OMX_VERSION_1_1_2;
        mAllLock.nPortIndex = OMX_ALL; // wrong?
        mAllLock.eImageLock = OMX_SYMBIAN_LockOff;
        break;
    case OMX_Symbian_IndexConfigROI:
        mROI.nSize = sizeof(OMX_SYMBIAN_CONFIG_ROITYPE);
        mROI.nVersion.nVersion = OMX_VERSION_1_1_2;
        mROI.nPortIndex = OMX_ALL;
        mROI.nNumberOfROIs = 0;
        for (int i=0; i<OMX_SYMBIAN_MAX_NUMBER_OF_ROIS; i++)
        {
            mROI.sROIs[i].sROI.sRect.sTopLeft.nX = 0;
            mROI.sROIs[i].sROI.sRect.sTopLeft.nY = 0;
            mROI.sROIs[i].sROI.sRect.sSize.nWidth = 10;
            mROI.sROIs[i].sROI.sRect.sSize.nHeight = 10;
            mROI.sROIs[i].sROI.sReference.nWidth = 10;
            mROI.sROIs[i].sROI.sReference.nHeight = 10;
            mROI.sROIs[i].nROIID = 0;
            mROI.sROIs[i].nPriority = 0; // ROI priority, 0 being the highest priority
            mROI.sROIs[i].eObjectType = OMX_SYMBIAN_RoiObjectNone;
            mROI.sROIs[i].nOrientation.nYaw = 0xFFFFFFFF;
            mROI.sROIs[i].nOrientation.nPitch = 0xFFFFFFFF;
            mROI.sROIs[i].nOrientation.nRoll = 0xFFFFFFFF;
        }
        break;
    case OMX_Symbian_IndexConfigRAWPreset:
        mRAWPreset.nSize = sizeof(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE);
        mRAWPreset.nVersion.nVersion = OMX_VERSION_1_1_2;
        mRAWPreset.nPortIndex = 1; // VPB1
        mRAWPreset.ePreset = OMX_SYMBIAN_RawImageProcessed;
        break;
    case OMX_IndexConfigCommonMirror:
        mMirror.nSize = sizeof(OMX_CONFIG_MIRRORTYPE);
        mMirror.nVersion.nVersion = OMX_VERSION_1_1_2;
        mMirror.nPortIndex = OMX_ALL; // wrong?
        mMirror.eMirror = OMX_MirrorNone;
        break;
    case OMX_Symbian_IndexConfigCameraSensorInfo:
        mSensorInfo.nSize = sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE);
        mSensorInfo.nVersion.nVersion = OMX_VERSION_1_1_2;
        mSensorInfo.nPortIndex = OMX_ALL; // wrong?
        mSensorInfo.nVersion1 = 0;
        mSensorInfo.nVersion2 = 0;
        mSensorInfo.cInfoString = NULL;
        break;
    case OMX_Symbian_IndexConfigFlashGunInfo:
        mFlashgunInfo.nSize = sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE);
        mFlashgunInfo.nVersion.nVersion = OMX_VERSION_1_1_2;
        mFlashgunInfo.nPortIndex = OMX_ALL; // wrong ?
        mFlashgunInfo.nVersion1 = 0x00000101; // Info B, info A, Flash IC revision, Flash IC info
        mFlashgunInfo.nVersion2 = 0;
        mFlashgunInfo.cInfoString = NULL;
        break;
    case OMX_Symbian_IndexParamDeviceMakeModel:
        mDeviceMakeModel.nSize = sizeof(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL);
        mDeviceMakeModel.nVersion.nVersion = OMX_VERSION_1_1_2;
        mDeviceMakeModel.nPortIndex = OMX_ALL;
        snprintf((char*)mDeviceMakeModel.cMake, sizeof(mDeviceMakeModel.cMake), "ST-Ericsson");
        snprintf((char*)mDeviceMakeModel.cModel, sizeof(mDeviceMakeModel.cModel), "U8500");
        break;
    case IFM_IndexConfigSoftware:
        mSoftware.nSize = sizeof(OMX_SYMBIAN_PARAM_SOFTWARE);
        mSoftware.nVersion.nVersion = OMX_VERSION_1_1_2;
        mSoftware.nPortIndex = OMX_ALL;
        snprintf((char*)mSoftware.cSoftware, sizeof(mSoftware.cSoftware), "ST-Ericsson");
        break;
    case OMX_Symbian_IndexConfigOrientationScene:
        mSceneOrientation.nSize = sizeof(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE);
        mSceneOrientation.nVersion.nVersion = OMX_VERSION_1_1_2; // OMX IL 1.1.2
        mSceneOrientation.nPortIndex = OMX_ALL; // wrong ?
        mSceneOrientation.eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
        break;
    default :
        return;
    }
    return;
}


RM_STATUS_E COmxCamera::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    DBGT_PROLOG("");

    OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
    mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
    getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

    // resource estimation for Camera.
    // A big buffer is enough to support all yuv use-case
    pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIG;

    DBGT_EPILOG("");

    return RM_E_NONE;
}

/*
   This function is called by ENS when an NMF Panic happens in the system (either on SIA/SVA or on ARM-EE)
   In case of panic coming from SIA, our OMX component is supposed to destroy the MPC NMF network in order
   to free SIA and allow NMF to shutdown SIA and restart it again.
   */
void COmxCamera::NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
    switch (serviceType) {
    case NMF_SERVICE_PANIC:
        if (serviceData->panic.panicSource==HOST_EE || serviceData->panic.info.mpc.coreid!=((t_nmf_core_id)SIA_CORE_ID)) {
            return;   /* if the panic does not concern SIA, ignore it */
        }
        ((Camera*)&getProcessingComponent())->errorRecovery();
        /*
           just tell the IL client that an unrecoverable error has happened.
           From this point on, the IL client is supposed to DESTROY our OMX component.
           No state transitions allowed, no assumptions on returned buffers etc.
           At this point we are dead.
           */
        eventHandler(OMX_EventError,OMX_ErrorHardware,0);
        break;
    case NMF_SERVICE_SHUTDOWN:
        if (serviceData->shutdown.coreid != ((t_nmf_core_id)SIA_CORE_ID)) return;
        /*
           implement something here if you want to rebuild the DSP network once
           the DSP has waken up again beware that this event is also generated when
           the network is detroyed in the nominal case (go to loaded).
           */
        break;
    default:
        break;
    }
}
