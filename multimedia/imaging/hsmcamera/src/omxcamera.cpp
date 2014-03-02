/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define DBGT_PREFIX "OMXCAMERA_PROXY"
#include "omxcamera.h"
#include "camera.h"
#include "OMX_IndexExt.h"
#include "string.h"
#include "ImgConfig.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_omxcameraTraces.h"
#endif

#define OMX_CameraUserSettings 0

OMX_ERRORTYPE OMXCameraFactoryMethod(ENS_Component_p * ppENSComponent)
{
    IN0("\n");
    OstTraceInt0(TRACE_FLOW, "Entry OMXCameraFactoryMethod");
    /* Construct Proxy */
    COmxCamera * omxcam = new COmxCamera(ePrimaryCamera);
    if (omxcam == NULL)
    {
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXCameraFactoryMethod OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }
    *ppENSComponent = omxcam;

    Camera * cam = new Camera(*omxcam, 0, 0, 0, ePrimaryCamera);
    if (cam == NULL)
    {
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXCameraFactoryMethod OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }

    /* set Processing Component */
    (*ppENSComponent)->setProcessingComponent(cam);

    OUTR(" ",(OMX_ErrorNone));
    OstTraceInt1(TRACE_FLOW, "Exit OMXCameraFactoryMethod (%d)", (OMX_ErrorNone));
    return OMX_ErrorNone;
}

/** OSI Factory method for secondary camera
*/
OMX_ERRORTYPE OMXCameraSecondaryFactoryMethod(ENS_Component_p * ppENSComponent)
{
    IN0("\n");
    OstTraceInt0(TRACE_FLOW, "Entry OMXCameraSecondaryFactoryMethod");

    COmxCamera * omxcam = new COmxCamera(eSecondaryCamera);
    if (omxcam == NULL)
    {
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXCameraSecondaryFactoryMethod OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }

    *ppENSComponent = omxcam;

    Camera * cam = new Camera(*omxcam, 0, 0, 0, eSecondaryCamera);
    if (cam == NULL)
    {
        OUTR(" ",(OMX_ErrorInsufficientResources));
        OstTraceInt0(TRACE_FLOW, "Exit OMXCameraSecondaryFactoryMethod OMX_ErrorInsufficientResources");
        return OMX_ErrorInsufficientResources;
    }

    (*ppENSComponent)->setProcessingComponent(cam);

    OUTR(" ",(OMX_ErrorNone));
    OstTraceInt0(TRACE_FLOW, "Exit OMXCameraSecondaryFactoryMethod OMX_ErrorInsufficientResources");
    return OMX_ErrorNone;
}


COmxCamera_RDB::COmxCamera_RDB():ENS_ResourcesDB()
{
    //IN0("\n");
    setDefaultNMFDomainType(RM_NMFD_PROCSIA);
    //OUT0("\n");
}


OMX_ERRORTYPE COmxCamera::createResourcesDB() {
    //IN0("\n");
    mRMP = new COmxCamera_RDB;
    if (mRMP == 0) {
    // OUTR(" ",OMX_ErrorInsufficientResources);
        return OMX_ErrorInsufficientResources;
    } else {
        // OUTR(" ",OMX_ErrorNone);
        return OMX_ErrorNone;
    }
}

COmxCamera::~COmxCamera() { }

COmxCamera::COmxCamera(enumCameraSlot cam):
    ENS_Component(),
    iLoadedSelfTest((ENS_Component*)this,cam)
{
	GET_AND_SET_TRACE_LEVEL(hsmcamera);
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
    ENS_Component::construct(CAMERA_NB_PORTS + CAMERA_NB_STATS_PORTS);
#else // CAMERA_ENABLE_OMX_STATS_PORT
    ENS_Component::construct(CAMERA_NB_PORTS);
#endif // CAMERA_ENABLE_OMX_STATS_PORT
    OMX_ERRORTYPE error;
    error = createCamPort(CAMERA_PORT_OUT0, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createCamPort(CAMERA_PORT_OUT1, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createCamPort(CAMERA_PORT_OUT2, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
    error = createStatsPort(CAMERA_PORT_STATS_AEWB, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
    error = createStatsPort(CAMERA_PORT_STATS_AF, OMX_DirOutput, OMX_BufferSupplyOutput);
    if(error != OMX_ErrorNone) DBC_ASSERT(0); //TODO: What to do with this error!
#endif // CAMERA_ENABLE_OMX_STATS_PORT

    //Initialize ZSLHDR related variables
    previousScenetype=OMX_SYMBIAN_SceneMax;
    eStateZSLHDR = ZSLHDRState_Invalid;
    nFrameBeforeBackUp = 0;
    bFrameLimitedBackUp = OMX_FALSE;
    nFrameLimitBackUp = 0;

    /* Initialise _all_ OMX config with default values */
    defaultConfig(IFM_IndexConfig3ATraceDumpControl);
    defaultConfig(IFM_IndexConfigCommonOpMode_Check);
    defaultConfig(IFM_IndexConfigCommonReadPe);
    defaultConfig(IFM_IndexConfigCommonWritePe);
    defaultConfig(IFM_IndexConfigTestMode);
    defaultConfig(OMX_IndexAutoPauseAfterCapture);
    defaultConfig(OMX_IndexConfigCallbackRequest);
    defaultConfig(OMX_IndexConfigCaptureMode);
    defaultConfig(OMX_IndexConfigCommonBrightness);
    defaultConfig(OMX_IndexConfigCommonColorBlend);
    defaultConfig(OMX_IndexConfigCommonContrast);
    defaultConfig(OMX_IndexConfigCommonExposure);
    defaultConfig(OMX_IndexConfigCommonExposureValue);
    defaultConfig(OMX_IndexConfigCommonFocusRegion);
    defaultConfig(OMX_IndexConfigCommonFrameStabilisation);
    defaultConfig(OMX_IndexConfigCommonImageFilter);
    defaultConfig(OMX_IndexConfigCommonLightness);
    defaultConfig(OMX_IndexConfigCommonMirror);
    defaultConfig(OMX_IndexConfigCommonRotate);
    defaultConfig(OMX_IndexConfigCommonSaturation);
    defaultConfig(OMX_IndexConfigCommonWhiteBalance);
    defaultConfig(OMX_IndexConfigFocusControl);
    defaultConfig(OMX_IndexParamCommonSensorMode);
    defaultConfig(OMX_IndexParamVideoInit);
    defaultConfig(OMX_Symbian_IndexConfigAFAssistantLight);
    defaultConfig(OMX_Symbian_IndexConfigAllLock);
    defaultConfig(OMX_Symbian_IndexConfigApplyUserSettings);
    defaultConfig(OMX_Symbian_IndexConfigBracketing);
    defaultConfig(OMX_Symbian_IndexConfigCameraFuncTestSelect);
    defaultConfig(OMX_Symbian_IndexConfigCameraSelfTestSelect);
    defaultConfig(OMX_Symbian_IndexConfigCameraSensorInfo);
    defaultConfig(OMX_Symbian_IndexConfigCameraTesting);
    defaultConfig(OMX_Symbian_IndexConfigCenterFieldOfView);
    defaultConfig(OMX_Symbian_IndexConfigCommonExtCapturing);
    defaultConfig(OMX_Symbian_IndexConfigExposureInitiated);
    defaultConfig(OMX_Symbian_IndexConfigExposureLock);
    defaultConfig(OMX_Symbian_IndexConfigExtCaptureMode);
    defaultConfig(OMX_Symbian_IndexConfigExtDigitalZoom);
    defaultConfig(OMX_Symbian_IndexConfigExtFocusStatus);
    defaultConfig(OMX_Symbian_IndexConfigExtOpticalZoom);
    defaultConfig(OMX_Symbian_IndexConfigFlashControl);
    defaultConfig(OMX_Symbian_IndexConfigFlashGunInfo);
    defaultConfig(OMX_Symbian_IndexConfigFlickerRemoval);
    defaultConfig(OMX_Symbian_IndexConfigFocusLock);
    defaultConfig(OMX_Symbian_IndexConfigFocusRange);
    defaultConfig(OMX_Symbian_IndexConfigFocusRegion);
    defaultConfig(OMX_Symbian_IndexConfigHintPowerVsQuality);
    defaultConfig(OMX_Symbian_IndexConfigHistogramControl);
    defaultConfig(OMX_Symbian_IndexConfigImageMaxJpegSize);
    defaultConfig(OMX_Symbian_IndexConfigMotionLevel);
    defaultConfig(OMX_Symbian_IndexConfigNDFilterControl);
    defaultConfig(OMX_Symbian_IndexConfigOrientationScene);
    defaultConfig(OMX_Symbian_IndexConfigPreCaptureExposureTime);
    defaultConfig(OMX_Symbian_IndexConfigRAWPreset);
    defaultConfig(OMX_Symbian_IndexConfigREDComplexity);
    defaultConfig(OMX_Symbian_IndexConfigRGBHistogram);
    defaultConfig(OMX_Symbian_IndexConfigROI);
    defaultConfig(OMX_Symbian_IndexConfigROISelection);
    defaultConfig(OMX_Symbian_IndexConfigRedEyeRemoval);
    defaultConfig(OMX_Symbian_IndexConfigSceneMode);
    defaultConfig(OMX_Symbian_IndexConfigSharpness);
    defaultConfig(OMX_Symbian_IndexConfigVideoLight);
    defaultConfig(OMX_Symbian_IndexConfigWhiteBalanceLock);
    defaultConfig(OMX_Symbian_IndexConfigXenonFlashStatus);
    defaultConfig(OMX_Symbian_IndexConfigXenonLifeCounter);
    defaultConfig(OMX_Symbian_IndexParamRevertAspectRatio);
    defaultConfig(IFM_IndexParamFirmwareTrace);
    defaultConfig(OMX_Symbian_IndexParamDeviceMakeModel);
    defaultConfig(IFM_IndexConfigSoftware);	
    defaultConfig(OMX_Symbian_IndexParamColorPrimary);
    defaultConfig(OMX_STE_IndexConfigFrameSelectRatio);
    defaultConfig(OMX_STE_IndexParamFrameRateRange);	
    defaultConfig(OMX_Symbian_IndexConfigAnalyzerFeedback);
    defaultConfig(OMX_STE_IndexConfigCommonZoomMode);
    defaultConfig(OMX_STE_IndexConfigMeteringOn);
	defaultConfig(OMX_STE_IndexConfigPictureSize);
    defaultConfig(OMX_CameraUserSettings); // Keep as last
#ifdef FREQ_SCALING_ALLOWED
    defaultConfig(OMX_STE_IndexConfigCommonScaling);
#endif //FREQ_SCALING_ALLOWED
    defaultConfig(OMX_STE_IndexConfigExifTimestamp);
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
        case OMX_CameraUserSettings:
            mCameraUserSettings.nX = mCenterFieldOfViewStatusType.sPoint.nX;
            mCameraUserSettings.nY = mCenterFieldOfViewStatusType.sPoint.nY;
            mCameraUserSettings.xDigitalZoomFactor.nMax = mZoomFactorStatusType.xZoomFactor.nMax;
            mCameraUserSettings.xDigitalZoomFactor.nMin = mZoomFactorStatusType.xZoomFactor.nMin;
            mCameraUserSettings.xDigitalZoomFactor.nValue = mZoomFactorStatusType.xZoomFactor.nValue;
            mCameraUserSettings.xOpticalZoomFactor.nMax = mOpticalZoomFactor.xZoomFactor.nMax;
            mCameraUserSettings.xOpticalZoomFactor.nMin = mOpticalZoomFactor.xZoomFactor.nMin;
            mCameraUserSettings.xOpticalZoomFactor.nValue = mOpticalZoomFactor.xZoomFactor.nValue;
            mCameraUserSettings.bOneShot = mSensorMode.bOneShot;
            mCameraUserSettings.bContinuous = mCaptureModeType.bContinuous;
            mCameraUserSettings.bFrameLimited = mCaptureModeType.bFrameLimited;
            mCameraUserSettings.nFrameLimit = mCaptureModeType.nFrameLimit;
            mCameraUserSettings.nFramesBefore = mExtCaptureModeType.nFrameBefore;
            mCameraUserSettings.nSceneModePreset = mSceneMode.eSceneType;
            mCameraUserSettings.eWhiteBalControl = mWhiteBalControl.eWhiteBalControl;
            mCameraUserSettings.eExposureControl = mExposureControl.eExposureControl;
            mCameraUserSettings.eFlashControl = mFlashControlStruct.eFlashControl;
            mCameraUserSettings.eFocusControl = mFocusControl.eFocusControl;
            mCameraUserSettings.nFocusSteps = mFocusControl.nFocusSteps;
            mCameraUserSettings.nFocusStepIndex = mFocusControl.nFocusStepIndex;
            mCameraUserSettings.nLockingStatus =   (mExposureLock.eImageLock != OMX_SYMBIAN_LockOff) << 0
                                                 | (mWhiteBalanceLock.eImageLock != OMX_SYMBIAN_LockOff) << 1
                                                 | (mFocusLock.eImageLock != OMX_SYMBIAN_LockOff) << 2;
            mCameraUserSettings.eMetering = mExposureValue.eMetering;
            mCameraUserSettings.xEVCompensation = mExposureValue.xEVCompensation;
            mCameraUserSettings.nApertureFNumber = mExposureValue.nApertureFNumber;
            mCameraUserSettings.bAutoAperture = mExposureValue.bAutoAperture;
            mCameraUserSettings.nShutterSpeedMsec = mExposureValue.nShutterSpeedMsec;
            mCameraUserSettings.bAutoShutterSpeed = mExposureValue.bAutoShutterSpeed;
            mCameraUserSettings.nSensitivity = mExposureValue.nSensitivity;
            mCameraUserSettings.bAutoSensitivity = mExposureValue.bAutoSensitivity;
            mCameraUserSettings.eImageFilter = mImageFilterType.eImageFilter;
            mCameraUserSettings.nContrast = mContrast.nContrast;
            mCameraUserSettings.nBrightness = mBrightness.nBrightness;
            mCameraUserSettings.nSaturation = mSaturation.nSaturation;
            mCameraUserSettings.nSharpness = mSharpness.nValue;
            mCameraUserSettings.bStab = mStabType.bStab;
            mCameraUserSettings.eDoFhint = OMX_SYMBIAN_DoFNotSpecified;
            mCameraUserSettings.eFocusRegionControl = mFocusRegion.eFocusRegionControl;
            mCameraUserSettings.sFocusRegion.sRect.sTopLeft.nX = mFocusRegion.sFocusRegion.sRect.sTopLeft.nX;
            mCameraUserSettings.sFocusRegion.sRect.sTopLeft.nY = mFocusRegion.sFocusRegion.sRect.sTopLeft.nY;
            mCameraUserSettings.sFocusRegion.sRect.sSize.nWidth = mFocusRegion.sFocusRegion.sRect.sSize.nWidth;
            mCameraUserSettings.sFocusRegion.sRect.sSize.nHeight = mFocusRegion.sFocusRegion.sRect.sSize.nHeight;
            mCameraUserSettings.sFocusRegion.sReference.nWidth = mFocusRegion.sFocusRegion.sReference.nWidth;
            mCameraUserSettings.sFocusRegion.sReference.nWidth = mFocusRegion.sFocusRegion.sReference.nHeight;
            break;
        case IFM_IndexConfig3ATraceDumpControl:
            mIfm3ATraceDumpControl.nSize = sizeof(IFM_CONFIG_3A_TRACEDUMPCONTROL);
            mIfm3ATraceDumpControl.nVersion.nVersion = OMX_VERSION_1_1_2;
            mIfm3ATraceDumpControl.nPortIndex = OMX_ALL;    // NB: This config is not port-sensitive.
            mIfm3ATraceDumpControl.bEnabled= OMX_TRUE;
            break;
        case IFM_IndexConfigCommonOpMode_Check:
            // GetConfig only. OMX config not stored.
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
        case IFM_IndexConfigTestMode:
            mTestModeType.nSize = sizeof(OMX_STE_CONFIG_SENSORTESTTYPE);
            mTestModeType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mTestModeType.nPortIndex = OMX_ALL; // wrong?
            mTestModeType.eTestMode = OMX_STE_TestModeNone;
            mTestModeType.nSolidBar_R = 186; // (186,107,20) represent a rather warm brown color.
            mTestModeType.nSolidBar_G = 107;
            mTestModeType.nSolidBar_B = 20;
            break;
        case OMX_IndexAutoPauseAfterCapture:
            mAutoPauseAfterCapture.nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
            mAutoPauseAfterCapture.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAutoPauseAfterCapture.bEnabled = OMX_FALSE;
            break;
        case OMX_IndexConfigCallbackRequest:
            mCallbackRequests.ExposureLock.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.ExposureLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.ExposureLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.ExposureLock.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigExposureLock;
            mCallbackRequests.ExposureLock.bEnable = OMX_FALSE;
            mCallbackRequests.WhiteBalanceLock.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.WhiteBalanceLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.WhiteBalanceLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.WhiteBalanceLock.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigWhiteBalanceLock;
            mCallbackRequests.WhiteBalanceLock.bEnable = OMX_FALSE;
            mCallbackRequests.FocusLock.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.FocusLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.FocusLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.FocusLock.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigFocusLock;
            mCallbackRequests.FocusLock.bEnable = OMX_FALSE;
            mCallbackRequests.AllLock.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.AllLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.AllLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.AllLock.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigAllLock;
            mCallbackRequests.AllLock.bEnable = OMX_FALSE;
			
			mCallbackRequests.PreCaptureExposureTime.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.PreCaptureExposureTime.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.PreCaptureExposureTime.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.PreCaptureExposureTime.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigPreCaptureExposureTime;
            mCallbackRequests.PreCaptureExposureTime.bEnable = OMX_FALSE;
			
            mCallbackRequests.ExtFocusStatus.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.ExtFocusStatus.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.ExtFocusStatus.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.ExtFocusStatus.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigExtFocusStatus;
            mCallbackRequests.ExtFocusStatus.bEnable = OMX_FALSE;
            mCallbackRequests.CameraTesting.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.CameraTesting.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.CameraTesting.nPortIndex = OMX_ALL; // wrong?
            mCallbackRequests.CameraTesting.nIndex = (OMX_INDEXTYPE) OMX_Symbian_IndexConfigCameraTesting;
            mCallbackRequests.CameraTesting.bEnable = OMX_FALSE;
            mExposureInitiatedClbkReq.nSize = sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mExposureInitiatedClbkReq.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExposureInitiatedClbkReq.nPortIndex = OMX_ALL;
            mExposureInitiatedClbkReq.nIndex = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureInitiated;
            mExposureInitiatedClbkReq.bEnable = OMX_FALSE;

            mZoomModeClbkReq.nSize = sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mZoomModeClbkReq.nVersion.nVersion = OMX_VERSION_1_1_2;
            mZoomModeClbkReq.nPortIndex = OMX_ALL;
            mZoomModeClbkReq.nIndex = (OMX_INDEXTYPE)OMX_STE_IndexConfigCommonZoomMode;
            mZoomModeClbkReq.bEnable = OMX_FALSE;

            mCallbackRequests.MeteringPrefashCallback.nSize= sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mCallbackRequests.MeteringPrefashCallback.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCallbackRequests.MeteringPrefashCallback.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mCallbackRequests.MeteringPrefashCallback.nIndex = (OMX_INDEXTYPE) OMX_STE_IndexConfigMeteringOn;
            mCallbackRequests.MeteringPrefashCallback.bEnable = OMX_FALSE;

            break;
        case OMX_IndexConfigCaptureMode:
            mCaptureModeType.nSize = sizeof(OMX_CONFIG_CAPTUREMODETYPE);
            mCaptureModeType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCaptureModeType.nPortIndex = OMX_ALL; // wrong?
            mCaptureModeType.bContinuous = OMX_FALSE;
            mCaptureModeType.bFrameLimited = OMX_TRUE;
            mCaptureModeType.nFrameLimit = 1;
            break;
        case OMX_IndexConfigCommonBrightness:
            mBrightness.nSize = sizeof(OMX_CONFIG_BRIGHTNESSTYPE);
            mBrightness.nVersion.nVersion = OMX_VERSION_1_1_2;
            mBrightness.nPortIndex = OMX_ALL; // wrong?
            mBrightness.nBrightness = 50;
            break;
        case OMX_IndexConfigCommonColorBlend:
            mColorBlendType.nSize = sizeof(OMX_CONFIG_COLORBLENDTYPE);
            mColorBlendType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mColorBlendType.nPortIndex = OMX_ALL; // wrong?
            mColorBlendType.nRGBAlphaConstant = 0;
            mColorBlendType.eColorBlend = OMX_ColorBlendNone;
            break;
        case OMX_IndexConfigCommonContrast:
            mContrast.nSize = sizeof(OMX_CONFIG_CONTRASTTYPE);
            mContrast.nVersion.nVersion = OMX_VERSION_1_1_2;
            mContrast.nPortIndex = OMX_ALL; // wrong?
            mContrast.nContrast = 0;
            break;
        case OMX_IndexConfigCommonExposure:
            mExposureControl.nSize = sizeof(OMX_CONFIG_EXPOSURECONTROLTYPE);
            mExposureControl.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExposureControl.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mExposureControl.eExposureControl = OMX_ExposureControlAuto; // SAS 1.6 p237
            break;
        case OMX_IndexConfigCommonExposureValue:
            mExposureValue.nSize = sizeof(OMX_CONFIG_EXPOSUREVALUETYPE);
            mExposureValue.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExposureValue.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive
            mExposureValue.eMetering = OMX_MeteringModeMatrix;
            mExposureValue.xEVCompensation = 0;
            mExposureValue.nApertureFNumber = 0;
            mExposureValue.bAutoAperture = OMX_TRUE;
            mExposureValue.nShutterSpeedMsec = 0;
            mExposureValue.bAutoShutterSpeed = OMX_TRUE;
            mExposureValue.nSensitivity = 0;
            mExposureValue.bAutoSensitivity = OMX_TRUE; // Auto ISO, see SAS 1.6 p239
            break;
        case OMX_IndexConfigCommonFrameStabilisation:
            mStabType.nSize = sizeof(OMX_CONFIG_FRAMESTABTYPE);
            mStabType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mStabType.nPortIndex = OMX_ALL; // wrong?
            mStabType.bStab = OMX_FALSE;
            break;
        case OMX_IndexConfigCommonImageFilter:
            mImageFilterType.nSize = sizeof(OMX_CONFIG_IMAGEFILTERTYPE);
            mImageFilterType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mImageFilterType.nPortIndex = OMX_ALL;
            mImageFilterType.eImageFilter = OMX_ImageFilterNone;
            break;
        case OMX_IndexConfigCommonMirror:
            mMirror.nSize = sizeof(OMX_CONFIG_MIRRORTYPE);
            mMirror.nVersion.nVersion = OMX_VERSION_1_1_2;
            mMirror.nPortIndex = OMX_ALL; // wrong?
            mMirror.eMirror = OMX_MirrorNone;
            break;
        case OMX_IndexConfigCommonRotate:
            mRotation.nSize = sizeof(OMX_CONFIG_ROTATIONTYPE);
            mRotation.nVersion.nVersion = OMX_VERSION_1_1_2;
            mRotation.nPortIndex = CAMERA_PORT_OUT2;
            mRotation.nRotation=0;
            nRotation = 0;
            break;
        case OMX_IndexConfigCommonSaturation:
            mSaturation.nSize = sizeof(OMX_CONFIG_SATURATIONTYPE);
            mSaturation.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSaturation.nPortIndex = OMX_ALL; // wrong?
            mSaturation.nSaturation = 0;
            break;
        case OMX_IndexConfigCommonWhiteBalance:
            mWhiteBalControl.nSize = sizeof(OMX_CONFIG_WHITEBALCONTROLTYPE);
            mWhiteBalControl.nVersion.nVersion = OMX_VERSION_1_1_2;
            mWhiteBalControl.nPortIndex = OMX_ALL; // wrong?
            mWhiteBalControl.eWhiteBalControl = OMX_WhiteBalControlAuto; // SAS 1.6 p241
            break;
        case OMX_IndexConfigFocusControl:
            mFocusControl.nSize = sizeof(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE);
            mFocusControl.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFocusControl.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFocusControl.eFocusControl = OMX_IMAGE_FocusControlOff; // see SAS 1.6 p241
            mFocusControl.nFocusSteps = 10;
            mFocusControl.nFocusStepIndex = 0;  // Move lens to infinity
            break;
        case OMX_IndexParamCommonSensorMode:
            // WARNING: is also initialised in camport.cpp
            mSensorMode.nSize = sizeof(OMX_PARAM_SENSORMODETYPE);
            mSensorMode.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSensorMode.nPortIndex = OMX_ALL;
            mSensorMode.nFrameRate = 0;
            mSensorMode.bOneShot = OMX_TRUE;
            mSensorMode.sFrameSize.nSize = sizeof(OMX_FRAMESIZETYPE);
            mSensorMode.sFrameSize.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSensorMode.sFrameSize.nPortIndex = OMX_ALL;
            mSensorMode.sFrameSize.nWidth  = 640;
            mSensorMode.sFrameSize.nHeight = 480;
            break;
        case OMX_IndexParamVideoInit:
            break;
        case OMX_Symbian_IndexConfigAFAssistantLight:
            mFlashAFAssist.nSize = sizeof(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE);
            mFlashAFAssist.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFlashAFAssist.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFlashAFAssist.eControl = OMX_SYMBIAN_ControlOff; // or OMX_SYMBIAN_ControlAuto ???
            break;
        case OMX_Symbian_IndexConfigAllLock:
            mAllLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
            mAllLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAllLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mAllLock.eImageLock = OMX_SYMBIAN_LockOff;
            break;
        case OMX_Symbian_IndexConfigApplyUserSettings:
            break;
        case OMX_Symbian_IndexConfigBracketing:
            memset(&mBracketing, 0, sizeof(mBracketing));
            mBracketing.config.nSize = IFM_CONFIG_BRACKETING_SIZE(IFM_MAX_NUMBER_OF_BRACKETING_VALUES);
            mBracketing.config.nVersion.nVersion = OMX_VERSION_1_1_2;
            mBracketing.config.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mBracketing.config.eBracketMode = OMX_SYMBIAN_BracketExposureRelativeInEV;
            mBracketing.config.bCumulativeBracketing = OMX_FALSE;
            mBracketing.config.nNbrBracketingValues = 0;    // No bracketing value is allowed by default, because every still capture would automatically be done in bracketing mode.
            for (int i=0; i < IFM_MAX_NUMBER_OF_BRACKETING_VALUES; i++) {
                mBracketing.config.nBracketValues[i] = 0;
            }
            break;
        case OMX_Symbian_IndexConfigCameraFuncTestSelect:
            break;
        case OMX_Symbian_IndexConfigCameraSelfTestSelect:
            mSelfTestType.nSize = sizeof(OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE);
            mSelfTestType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSelfTestType.nPortIndex = OMX_ALL; // wrong?
            mSelfTestType.nFlashHwFaultRegister1 = 0;
            mSelfTestType.nFlashHwFaultRegister1 = 0;
            mSelfTestType.nTestSelectMaxSizeUsed = 0;
            for (int i=0; i<OMX_SYMBIAN_MAX_SELF_TESTS_PER_REQUEST; i++)
            {
                mSelfTestType.eSelfTests[i].eTestId = OMX_SYMBIAN_CameraSelfTestNone;
                mSelfTestType.eSelfTests[i].eResult = OMX_ErrorUndefined;
            }
            break;
        case OMX_Symbian_IndexConfigCameraSensorInfo:
            mSensorInfo.nSize = sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE);
            mSensorInfo.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSensorInfo.nPortIndex = OMX_ALL; // wrong?
            mSensorInfo.nVersion1 = 0;
            mSensorInfo.nVersion2 = 0;
            mSensorInfo.cInfoString = NULL;
            break;
        case OMX_Symbian_IndexConfigCameraTesting:
            mTestingType.nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
            mTestingType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mTestingType.bEnabled = OMX_TRUE;
            break;
        case OMX_Symbian_IndexConfigCenterFieldOfView:
            // TODO: check that it is correct. Indeed, it is maybe also initialised somewhere else
            mCenterFieldOfViewStatusType.nSize = sizeof(OMX_SYMBIAN_CONFIG_POINTTYPE);
            mCenterFieldOfViewStatusType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCenterFieldOfViewStatusType.nPortIndex = OMX_ALL; // wrong?
            mCenterFieldOfViewStatusType.sPoint.nX = 0;
            mCenterFieldOfViewStatusType.sPoint.nY = 0;
            break;
        case OMX_Symbian_IndexConfigCommonExtCapturing:
            mCapturingStruct_VPB1.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
            mCapturingStruct_VPB1.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCapturingStruct_VPB1.nPortIndex = CAMERA_PORT_OUT1;
            mCapturingStruct_VPB1.bEnabled = OMX_FALSE;
            mCapturingStruct_VPB2.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
            mCapturingStruct_VPB2.nVersion.nVersion = OMX_VERSION_1_1_2;
            mCapturingStruct_VPB2.nPortIndex = CAMERA_PORT_OUT2;
            mCapturingStruct_VPB2.bEnabled = OMX_FALSE;
            break;
        case OMX_Symbian_IndexConfigExposureInitiated:
			mExposureInitiatedClbkReq.nSize = sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE);
            mExposureInitiatedClbkReq.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExposureInitiatedClbkReq.nPortIndex = OMX_ALL;
            mExposureInitiatedClbkReq.nIndex = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureInitiated;
            mExposureInitiatedClbkReq.bEnable = OMX_FALSE;
            break;
        case OMX_Symbian_IndexConfigExposureLock:
            mExposureLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
            mExposureLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExposureLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mExposureLock.eImageLock = OMX_SYMBIAN_LockOff;
            break;
        case OMX_Symbian_IndexConfigExtCaptureMode:
            mExtCaptureModeType.nSize = sizeof(OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE);
            mExtCaptureModeType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExtCaptureModeType.nPortIndex = OMX_ALL; // wrong?
            mExtCaptureModeType.nFrameBefore = 0;
            mExtCaptureModeType.bPrepareCapture = OMX_FALSE;
            break;
        case OMX_Symbian_IndexConfigExtDigitalZoom:
            // TODO: check that it is correct. Indeed, it is maybe also initialised somewhere else
            mZoomFactorStatusType.nSize = sizeof(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE);
            mZoomFactorStatusType.nVersion.nVersion = OMX_VERSION_1_1_2;
            mZoomFactorStatusType.nPortIndex = OMX_ALL; // wrong?
            mZoomFactorStatusType.xZoomFactor.nMin   = 1 << 16;
            mZoomFactorStatusType.xZoomFactor.nMax   = 5 << 16;
            mZoomFactorStatusType.xZoomFactor.nValue = 1 << 16;
            break;
        case OMX_Symbian_IndexConfigExtFocusStatus:
            memset(&mExtFocusStatus, 0, sizeof(mExtFocusStatus));
            mExtFocusStatus.config.nSize = IFM_CONFIG_EXTFOCUSSTATUS_SIZE(IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS);
            mExtFocusStatus.config.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExtFocusStatus.config.bFocused = OMX_FALSE;
            mExtFocusStatus.config.nMaxAFAreas = IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS;
            mExtFocusStatus.config.nAFAreas = 0;
            for (int i=0; i < IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS; i++) {
                mExtFocusStatus.config.sAFROIs[i].nReferenceWindow.nWidth = 10;
                mExtFocusStatus.config.sAFROIs[i].nReferenceWindow.nHeight= 10;
                mExtFocusStatus.config.sAFROIs[i].xFocusDistance = 0;
                mExtFocusStatus.config.sAFROIs[i].nRect.sTopLeft.nX = 0;
                mExtFocusStatus.config.sAFROIs[i].nRect.sTopLeft.nY = 0;
                mExtFocusStatus.config.sAFROIs[i].nRect.sSize.nWidth  = 0;
                mExtFocusStatus.config.sAFROIs[i].nRect.sSize.nHeight = 0;
                mExtFocusStatus.config.sAFROIs[i].eFocusStatus = OMX_FocusStatusOff;
            }
            break;
        case OMX_Symbian_IndexConfigExtOpticalZoom:
            mOpticalZoomFactor.nSize = sizeof(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE);
            mOpticalZoomFactor.nVersion.nVersion = OMX_VERSION_1_1_2;
            mOpticalZoomFactor.nPortIndex = OMX_ALL; // wrong?
            mOpticalZoomFactor.xZoomFactor.nMin   = 1 << 16;
            mOpticalZoomFactor.xZoomFactor.nMax   = 1 << 16;
            mOpticalZoomFactor.xZoomFactor.nValue = 1 << 16;
            break;
        case OMX_Symbian_IndexConfigFlashControl:
            mFlashControlStruct.nSize = sizeof(OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE);
            mFlashControlStruct.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFlashControlStruct.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFlashControlStruct.eFlashControl = OMX_IMAGE_FlashControlOff;
            break;
        case OMX_Symbian_IndexConfigFlashGunInfo:
            mFlashgunInfo.nSize = sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE);
            mFlashgunInfo.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFlashgunInfo.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFlashgunInfo.nVersion1 = 0x00000101; // Info B, info A, Flash IC revision, Flash IC info
            mFlashgunInfo.nVersion2 = 0;
            mFlashgunInfo.cInfoString = NULL;
            break;
        case OMX_Symbian_IndexConfigFlickerRemoval:
            mFlickerRemoval.nSize = sizeof(OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE);
            mFlickerRemoval.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFlickerRemoval.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFlickerRemoval.eFlickerRemoval = OMX_SYMBIAN_FlickerRemovalOff;
            break;
        case OMX_Symbian_IndexConfigFocusLock:
            mFocusLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
            mFocusLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFocusLock.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFocusLock.eImageLock = OMX_SYMBIAN_LockOff;
            break;
        case OMX_Symbian_IndexConfigFocusRange:
            mFocusRange.nSize = sizeof(OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE);
            mFocusRange.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFocusRange.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFocusRange.eFocusRange = OMX_SYMBIAN_FocusRangeNormal;	// see SAS 1.6 p242
            break;
        case OMX_Symbian_IndexConfigFocusRegion:
            mFocusRegion.nSize = sizeof(OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE);
            mFocusRegion.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFocusRegion.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mFocusRegion.eFocusRegionControl = OMX_SYMBIAN_FocusRegionAuto;
            mFocusRegion.sFocusRegion.sRect.sTopLeft.nX = 0;
            mFocusRegion.sFocusRegion.sRect.sTopLeft.nY = 0;
            mFocusRegion.sFocusRegion.sRect.sSize.nWidth = 0;
            mFocusRegion.sFocusRegion.sRect.sSize.nHeight = 0;
            mFocusRegion.sFocusRegion.sReference.nWidth = 10;
            mFocusRegion.sFocusRegion.sReference.nHeight = 10;
            break;
        case OMX_Symbian_IndexConfigHintPowerVsQuality:
            break;
        case OMX_Symbian_IndexConfigHistogramControl:
            break;
        case OMX_Symbian_IndexConfigImageMaxJpegSize:
            break;
        case OMX_Symbian_IndexConfigMotionLevel:
            mMotionLevel.nSize = sizeof(OMX_SYMBIAN_CONFIG_U32TYPE);
            mMotionLevel.nVersion.nVersion = OMX_VERSION_1_1_2;
            mMotionLevel.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mMotionLevel.nValue = 0;
            break;
        case OMX_Symbian_IndexConfigNDFilterControl:
            mNDFilterControl.nSize = sizeof(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE);
            mNDFilterControl.nVersion.nVersion = OMX_VERSION_1_1_2;
            mNDFilterControl.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mNDFilterControl.eControl = OMX_SYMBIAN_ControlAuto;
            break;
        case OMX_Symbian_IndexConfigOrientationScene:
            mSceneOrientation.nSize = sizeof(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE);
            mSceneOrientation.nVersion.nVersion = OMX_VERSION_1_1_2; // OMX IL 1.1.2
            mSceneOrientation.nPortIndex = OMX_ALL; // wrong ?
            mSceneOrientation.eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
            break;
        case OMX_Symbian_IndexConfigPreCaptureExposureTime:
            break;
        case OMX_Symbian_IndexConfigRAWPreset:
            mRAWPreset.nSize = sizeof(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE);
            mRAWPreset.nVersion.nVersion = OMX_VERSION_1_1_2;
            mRAWPreset.nPortIndex = 1; // VPB1
            mRAWPreset.ePreset = OMX_SYMBIAN_RawImageProcessed;
            break;
        case OMX_Symbian_IndexConfigREDComplexity:
            break;
        case OMX_Symbian_IndexConfigRGBHistogram:
	     mRGBHistogram.nSize = 	sizeof(OMX_SYMBIAN_CONFIG_RGBHISTOGRAM);
            mRGBHistogram.nVersion.nVersion = OMX_VERSION_1_1_2;
            mRGBHistogram.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            for (int i=0; i<HISTO_BIN_NB; i++)
            {
				mRGBHistogram.nRed[i] =  0; 
				mRGBHistogram.nGreen[i] =  0; 
				mRGBHistogram.nBlue[i] =  0; 
            }
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
                mROI.sROIs[i].sROI.sRect.sSize.nWidth = 0;
                mROI.sROIs[i].sROI.sRect.sSize.nHeight = 0;
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
        case OMX_Symbian_IndexConfigROISelection:
            break;
        case OMX_Symbian_IndexConfigRedEyeRemoval:
            break;
        case OMX_Symbian_IndexConfigSceneMode:
            mSceneMode.nSize = sizeof(OMX_SYMBIAN_CONFIG_SCENEMODETYPE);
            mSceneMode.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSceneMode.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mSceneMode.eSceneType = OMX_SYMBIAN_SceneAuto; // SAS 1.6 p248
            break;
        case OMX_Symbian_IndexConfigSharpness:
            mSharpness.nSize = sizeof(OMX_SYMBIAN_CONFIG_S32TYPE);
            mSharpness.nVersion.nVersion = OMX_VERSION_1_1_2;
            mSharpness.nPortIndex = OMX_ALL; // wrong?
            mSharpness.nValue = 0;
            break;
        case OMX_Symbian_IndexConfigVideoLight:
            mVideoLight.nSize = sizeof(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE);
            mVideoLight.nVersion.nVersion = OMX_VERSION_1_1_2;
            mVideoLight.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mVideoLight.eControl = OMX_SYMBIAN_ControlOff;
            break;
        case OMX_Symbian_IndexConfigWhiteBalanceLock:
            mWhiteBalanceLock.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
            mWhiteBalanceLock.nVersion.nVersion = OMX_VERSION_1_1_2;
            mWhiteBalanceLock.nPortIndex = OMX_ALL; // wrong?
            mWhiteBalanceLock.eImageLock = OMX_SYMBIAN_LockOff;
            break;
        case OMX_Symbian_IndexConfigXenonFlashStatus:
            break;
        case OMX_Symbian_IndexConfigXenonLifeCounter:
            break;
        case OMX_Symbian_IndexParamRevertAspectRatio:
	    mRevertAspectRatio.nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
            mRevertAspectRatio.nVersion.nVersion = OMX_VERSION_1_1_2;
            mRevertAspectRatio.bEnabled = OMX_FALSE;
            break;
		case IFM_IndexParamFirmwareTrace:
			mIfmFirmwareTrace.bEnabled = OMX_FALSE;
			mIfmFirmwareTrace.eTraceMechanism = IFM_FirmwareTraceMechanism_SharedMemory;
			mIfmFirmwareTrace.nLogLevel = 0;
			break;
        case OMX_Symbian_IndexParamDeviceMakeModel:
            mDeviceMakeModel.nSize = sizeof(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL);
            mDeviceMakeModel.nVersion.nVersion = OMX_VERSION_1_1_2;
            mDeviceMakeModel.nPortIndex = OMX_ALL;
            snprintf((char*)mDeviceMakeModel.cMake, sizeof(mDeviceMakeModel.cMake), "ST-Ericsson");
            snprintf((char*)mDeviceMakeModel.cModel, sizeof(mDeviceMakeModel.cModel), "U8500");
            break;
	case IFM_IndexConfigSoftware:
		mSoftwareVersion.nSize = sizeof(IFM_CONFIG_SOFTWAREVERSION);
		mSoftwareVersion.nVersion.nVersion = OMX_VERSION_1_1_2;
		mSoftwareVersion.nPortIndex = OMX_ALL;
		snprintf((char*)mSoftwareVersion.cSoftware, sizeof(mSoftwareVersion.cSoftware), "ST-Ericsson");
		break;
        case OMX_Symbian_IndexParamColorPrimary:
            mColorPrimary_VPB0.nSize = sizeof(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE);
            mColorPrimary_VPB0.nVersion.nVersion = OMX_VERSION_1_1_2;
            mColorPrimary_VPB0.nPortIndex = CAMERA_PORT_OUT2;
            mColorPrimary_VPB0.eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT601FullRange;

            mColorPrimary_VPB2.nSize = sizeof(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE);
            mColorPrimary_VPB2.nVersion.nVersion = OMX_VERSION_1_1_2;
            mColorPrimary_VPB2.nPortIndex = CAMERA_PORT_OUT2;
            mColorPrimary_VPB2.eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT601FullRange;
            break;
        case OMX_STE_IndexConfigFrameSelectRatio:
            mVariableFramerateKeepRatio.nSize = sizeof(OMX_STE_CONFIG_FRAMESELECTRATIOTYPE);
            mVariableFramerateKeepRatio.nVersion.nVersion = OMX_VERSION_1_1_2;
            mVariableFramerateKeepRatio.nPortIndex = CAMERA_PORT_OUT1;
            mVariableFramerateKeepRatio.nSelectRatio = 1;
            break;
        case OMX_STE_IndexParamFrameRateRange:
            mFrameRateRange.nSize = sizeof(OMX_STE_PARAM_FRAMERATERANGE);
            mFrameRateRange.nVersion.nVersion = OMX_VERSION_1_1_2;
            mFrameRateRange.nPortIndex = OMX_ALL;
            mFrameRateRange.xFrameRateMin = CAM_RATE_MIN*Q16;
            mFrameRateRange.xFrameRateMax = CAM_RATE_MAX*Q16;
            break;			
        case OMX_Symbian_IndexConfigAnalyzerFeedback:
            mAnalyzersFeedbacks.bFaceTrackerFeedbackReceived = false;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nSize = IFM_CONFIG_FEEDBACK_SIZE(sizeof(OMX_SYMBIAN_ROITYPE));
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nPortIndex = CAMERA_PORT_OUT2;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nOriginatorTimestamp = 0;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nOriginatorFrameNum = 0;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nFeedbackType = OMX_SYMBIAN_FeedbackFaceTracker;
            mAnalyzersFeedbacks.sFaceTracker.sFeedback.nFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
            memset(mAnalyzersFeedbacks.sFaceTracker.sFeedback.nFeedbackValue, 0, sizeof(OMX_SYMBIAN_ROITYPE));

            mAnalyzersFeedbacks.bObjectTrackerFeedbackReceived = false;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nSize = IFM_CONFIG_FEEDBACK_SIZE(sizeof(OMX_SYMBIAN_ROITYPE));
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nPortIndex = CAMERA_PORT_OUT2;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nOriginatorTimestamp = 0;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nOriginatorFrameNum = 0;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nFeedbackType = OMX_SYMBIAN_FeedbackObjectTracker;
            mAnalyzersFeedbacks.sObjectTracker.sFeedback.nFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
            memset(mAnalyzersFeedbacks.sObjectTracker.sFeedback.nFeedbackValue, 0, sizeof(OMX_SYMBIAN_ROITYPE));

            mAnalyzersFeedbacks.bMotionLevelFeedbackReceived = false;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nSize = IFM_CONFIG_FEEDBACK_SIZE(sizeof(OMX_U32));
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nPortIndex = CAMERA_PORT_OUT2;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nOriginatorTimestamp = 0;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nOriginatorFrameNum = 0;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nFeedbackType = OMX_SYMBIAN_FeedbackMotionLevel;
            mAnalyzersFeedbacks.sMotionLevel.sFeedback.nFeedbackValueSize = sizeof(OMX_U32);
            memset(mAnalyzersFeedbacks.sMotionLevel.sFeedback.nFeedbackValue, 0, sizeof(OMX_U32));

            mAnalyzersFeedbacks.bSceneDetectionFeedbackReceived = false;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nSize = IFM_CONFIG_FEEDBACK_SIZE(sizeof(OMX_SYMBIAN_SCENEMODETYPE));
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nVersion.nVersion = OMX_VERSION_1_1_2;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nPortIndex = CAMERA_PORT_OUT2;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nOriginatorTimestamp = 0;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nOriginatorFrameNum = 0;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nFeedbackType = OMX_SYMBIAN_FeedbackAutomaticSceneDetection;
            mAnalyzersFeedbacks.sSceneDetection.sFeedback.nFeedbackValueSize = sizeof(OMX_SYMBIAN_SCENEMODETYPE);
            memset(mAnalyzersFeedbacks.sSceneDetection.sFeedback.nFeedbackValue, 0, sizeof(OMX_SYMBIAN_SCENEMODETYPE));

            for(int i=0; i<IFM_MAX_NUMBER_OF_OPAQUE_FEEDBACKS; i++) {
                mAnalyzersFeedbacks.bOpaqueFeedbackReceived[i] = false;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nSize = IFM_CONFIG_FEEDBACK_SIZE(IFM_CONFIG_OPAQUE_FEEDBACK_SIZE);
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nVersion.nVersion = OMX_VERSION_1_1_2;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nPortIndex = CAMERA_PORT_OUT2;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nOriginatorTimestamp = 0;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nOriginatorFrameNum = 0;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackType = (OMX_SYMBIAN_FEEDBACKTYPE)-1;
                mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackValueSize = IFM_CONFIG_OPAQUE_FEEDBACK_SIZE;
                memset(mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackValue, 0, IFM_CONFIG_OPAQUE_FEEDBACK_SIZE);
            }
            mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived = 0;
            break;
	 case OMX_STE_IndexConfigCommonZoomMode:
	 	mZoomMode.nSize = sizeof(OMX_CONFIG_BOOLEANTYPE);
		mZoomMode.nVersion.nVersion= OMX_VERSION_1_1_2;
	 	mZoomMode.bEnabled = OMX_FALSE;		//Default behaviour is: OFF
//	 	MSG1("defaultConfig. [smooth zoom]\n");
	     break;
#ifdef FREQ_SCALING_ALLOWED
            case OMX_STE_IndexConfigCommonScaling:
            for(int i=0;i<TRESOURCEID_MAX; i++)
            {
                mfreqscaling.scalingFactor[i]=100; //Default Value of freq scaling is 100%
            }

     break;
#endif // FREQ_SCALING_ALLOWED
     case OMX_STE_IndexConfigMeteringOn:
            mMeteringOn.nSize = sizeof(OMX_SYMBIAN_CONFIG_LOCKTYPE);
            mMeteringOn.nVersion.nVersion = OMX_VERSION_1_1_2;
            //mMeteringOn.nPortIndex = OMX_ALL; // NB: This config is not port-sensitive.
            mMeteringOn.bEnabled = OMX_FALSE;
            break;
     case OMX_STE_IndexConfigExifTimestamp:
            mExifTimeStamp.nSize = sizeof(OMX_TIME_CONFIG_TIMESTAMPTYPE);
            mExifTimeStamp.nPortIndex = OMX_ALL;
            mExifTimeStamp.nVersion.nVersion = OMX_VERSION_1_1_2;
            mExifTimeStamp.nTimestamp = 0x0;
            break;
	 case OMX_STE_IndexConfigPictureSize:
            mPictureSize.nSize = sizeof(OMX_STE_CONFIG_PICTURESIZETYPE);
            mPictureSize.nVersion.nVersion = OMX_VERSION_1_1_2;
            mPictureSize.modenumber  = 0; /*First mode*/
            mPictureSize.nOutputResWidth = 0;
            mPictureSize.nOutputResHeight = 0;
     	     break;		
        default :
            return;
    }
    return;
}

RM_STATUS_E COmxCamera::getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData)
{
    // resource estimation for Camera. See SAS.

    pEstimationData->sMcpsSva.nMax = 0;
    pEstimationData->sMcpsSva.nAvg = 0;
    pEstimationData->sMcpsSia.nMax = 3;  // 3 MIPS estimated for GRAB/GRAB DD/ISPCTL. You might have more concrete data (I would expect much less, this is assumed conservative)
    pEstimationData->sMcpsSia.nAvg = 3;  // same as max
    pEstimationData->sTcmSva.nSizeX = 0;
    pEstimationData->sTcmSva.nSizeY = 0;
    pEstimationData->sTcmSia.nSizeX = 144+87+60; /* pulled from CM_GetComponentMemoryInfo(), ispctl=144, grab=87, grabctl=60 */
    pEstimationData->sTcmSia.nSizeY = 0;
    pEstimationData->sEsramBufs.nSvaHwPipe = RM_ESBUFID_EMPTY;
    pEstimationData->sEsramBufs.nSiaHwXP70 = RM_ESBUFID_DEFAULT;
    pEstimationData->eUccBitmap = 0; // 0 for HDR. or RM_UCC_VISUAL_FHD;

    OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition[CAMERA_NB_OUT_PORTS];
    for (int i=CAMERA_PORT_OUT0; i<=CAMERA_PORT_OUT2; i++) {
        mParamPortDefinition[i].nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        getOmxIlSpecVersion(&mParamPortDefinition[i].nVersion);
        getPort(i)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition[i]);
    }

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    t_operating_mode_camera opmode = ((Camera*)(&getProcessingComponent()))->iOpModeMgr.CurrentOperatingMode;
    camport* pPort = (camport*)getPort(CAMERA_PORT_OUT2);
    if ((opmode >= OpMode_Cam_StillPreview) && pPort->isEnabled())
    {
        // Using VPB2 for still capture use-case.
	pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIGGEST;
    }
    else
    {
        // Using VPB2 for other use-case including video recording.
	pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIG;
    }
    MSG3("resource estimation changed %d, %d, %d\n", opmode, pPort->isEnabled(), pEstimationData->sEsramBufs.nSiaHwPipe);
#else
    t_operating_mode_camera opmode = ((Camera*)(&getProcessingComponent()))->iOpModeMgr.CurrentOperatingMode;
    if (opmode > OpMode_Cam_StillPreview) {
        /* if we might be in still capture, need biggest ESRAM buffer for grab cache
           for sensor FFOV capture */
        pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIGGEST;
    } else if (mParamPortDefinition[CAMERA_PORT_OUT1].format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) {
        /* for YUV ZSL mode */
        pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIG;
    } else if (mParamPortDefinition[CAMERA_PORT_OUT2].format.video.nFrameWidth > 1280) {
        /* for 1080p (>720p), need big ESRAM buffer */
        pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_BIG;
    }  else {
         /* for lower resolutions, or if stab is activated, need smallest buffer */
        pEstimationData->sEsramBufs.nSiaHwPipe = RM_ESBUFID_DEFAULT;
    }
    MSG1("HSMCamera pEstimationData->sEsramBufs.nSiaHwPipe: %#x",(unsigned int)pEstimationData->sEsramBufs.nSiaHwPipe);
#endif
    return RM_E_NONE;
}

OMX_ERRORTYPE COmxCamera::setConfig(OMX_INDEXTYPE nIndex,OMX_PTR p)
{
	Camera* cam = (Camera*)(&getProcessingComponent());
    switch ((t_uint32)nIndex)
    {
        case OMX_Symbian_IndexConfigAFAssistantLight:
            mFlashAFAssist = *(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE*)p;
            break;
        case OMX_Symbian_IndexConfigFlashControl:
            mFlashControlStruct = *(OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigVideoLight:
            mVideoLight = *(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigCommonExtCapturing:
        {
			MSG2(" %s OMX_Symbian_IndexConfigCommonExtCapturing nPortIndex=%lu \n", __FUNCTION__,((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p)->nPortIndex);
			switch(((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p)->nPortIndex)
            {
                case CAMERA_PORT_OUT1:
                    mCapturingStruct_VPB1 = *(OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p;

                    //performance traces start
                    cam->p_cam_sm->latencyMeasure(&(cam->p_cam_sm->mTime));
                    cam->mlatency_Hsmcamera.e_OMXCAM_ShotToCapture.t1 = cam->p_cam_sm->mTime;
                    cam->mlatency_Hsmcamera.e_OMXCAM_ShutterReleaseLag.t1 = cam->p_cam_sm->mTime;
                    cam->mlatency_Hsmcamera.e_OMXCAM_ShotToVF.t1 = cam->p_cam_sm->mTime;

            		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToCapture %d", 1);
            		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShutterReleaseLag %d", 1);

                    cam->latency_ShotToVF = 1;
            		OstTraceInt1 (TRACE_LATENCY, "e_OMXCAM_ShotToVF %d", 1);
            		//performance traces end

                    break;
                case CAMERA_PORT_OUT2:
                    mCapturingStruct_VPB2 = *(OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p;
                    break;
                default:
                    return OMX_ErrorBadParameter;
            }
            return OMX_ErrorNone;
        }
        case OMX_IndexAutoPauseAfterCapture :
            mAutoPauseAfterCapture = *(OMX_CONFIG_BOOLEANTYPE*)p;
            break;
        case OMX_IndexConfigCaptureMode:
        {
            OMX_CONFIG_CAPTUREMODETYPE tempCaptureModeType = *(OMX_CONFIG_CAPTUREMODETYPE*)p;
            MSG4("nPortIndex: %ld, nFrameLimit: %ld, bFrameLimited: %d, bPrepareCapture: %d",tempCaptureModeType.nPortIndex, tempCaptureModeType.nFrameLimit,
             tempCaptureModeType.bFrameLimited, mExtCaptureModeType.bPrepareCapture);
            if((tempCaptureModeType.nPortIndex != CAMERA_PORT_OUT1)&&(tempCaptureModeType.nPortIndex != CAMERA_PORT_OUT2))
            {
                return OMX_ErrorBadParameter;
            }
            if((tempCaptureModeType.nFrameLimit < 1)&&(tempCaptureModeType.bFrameLimited==OMX_TRUE)
                &&(OMX_FALSE == mExtCaptureModeType.bPrepareCapture))
            {
                return OMX_ErrorBadParameter;
            }
            mCaptureModeType = *(OMX_CONFIG_CAPTUREMODETYPE*)p;
            return OMX_ErrorNone;
        }
        case OMX_Symbian_IndexConfigExtCaptureMode:
        {
            OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE tempExtCaptureModeType = *(OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE*)p;
            if(CAMERA_PORT_OUT1 != tempExtCaptureModeType.nPortIndex)
            {
                return OMX_ErrorBadParameter;
            }
            mExtCaptureModeType = *(OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE*)p;
            break;
        }
        case OMX_IndexConfigCommonColorBlend:
            mColorBlendType = *(OMX_CONFIG_COLORBLENDTYPE*)p;
            return OMX_ErrorNotImplemented;
	case OMX_Symbian_IndexConfigRGBHistogram:
            return OMX_ErrorNotImplemented;		

        case OMX_Symbian_IndexConfigExtDigitalZoom :
            if (OMX_ALL != ((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p)->nPortIndex)
            {
                return OMX_ErrorBadParameter;
            }
            mZoomFactorCmdType = *(OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigCenterFieldOfView :
            if (OMX_ALL != ((OMX_SYMBIAN_CONFIG_POINTTYPE*)p)->nPortIndex)
            {
                return OMX_ErrorBadParameter;
            }
            mCenterFieldOfViewCmdType = *(OMX_SYMBIAN_CONFIG_POINTTYPE*)p;
            return OMX_ErrorNone;
        case IFM_IndexConfigTestMode :
            mTestModeType = *(OMX_STE_CONFIG_SENSORTESTTYPE*)p;
            break;
        case OMX_IndexConfigCommonWhiteBalance :
            mWhiteBalControl = *(OMX_CONFIG_WHITEBALCONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigCommonBrightness :
        {
            OMX_CONFIG_BRIGHTNESSTYPE *pBrightness = (OMX_CONFIG_BRIGHTNESSTYPE*) p;
            if (pBrightness->nBrightness > 100) { // Out of range; NB: cannot be < 0 because it is unsigned
                return OMX_ErrorBadParameter;
            } else {
                mBrightness = *pBrightness;
                return OMX_ErrorNone;
            }
        }
        case OMX_IndexConfigCommonLightness :
            return OMX_ErrorUnsupportedIndex;
        case OMX_IndexConfigCommonExposureValue:
            mExposureValue = *(OMX_CONFIG_EXPOSUREVALUETYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigCommonExposure:
            mExposureControl = *(OMX_CONFIG_EXPOSURECONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigCommonContrast:
        {
            OMX_CONFIG_CONTRASTTYPE *pContrast = (OMX_CONFIG_CONTRASTTYPE*) p;
            if ((pContrast->nContrast < -100) || (pContrast->nContrast > 100)) { // Out of range
                return OMX_ErrorBadParameter;
            } else {
                mContrast = *pContrast;
                return OMX_ErrorNone;
            }
        }
        case OMX_IndexConfigCommonSaturation:
        {
            OMX_CONFIG_SATURATIONTYPE *pSaturation = (OMX_CONFIG_SATURATIONTYPE*) p;
             if ((pSaturation->nSaturation < -100) || (pSaturation->nSaturation > 100)) { // Out of range
                 return OMX_ErrorBadParameter;
             } else {
                 mSaturation = *pSaturation;
                 return OMX_ErrorNone;
             }
        }
        case OMX_IndexConfigCommonGamma:
            return OMX_ErrorUnsupportedIndex;
        case OMX_IndexConfigCommonImageFilter:
            mImageFilterType = *(OMX_CONFIG_IMAGEFILTERTYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigFocusControl:
            mFocusControl = *(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigCommonFocusRegion:
            return OMX_ErrorUnsupportedIndex;    // Use OMX_Symbian_IndexConfigFocusRegion instead.
        case OMX_Symbian_IndexConfigFocusRange:
            mFocusRange = *(OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigFocusRegion:
            mFocusRegion = *(OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigExtFocusStatus: // read-only
            DBGT_ERROR("Error: Trying to write the read-only ExtFocusStatus index.\n");
            OstTraceFiltInst0(TRACE_ERROR, "Error: Trying to write the read-only ExtFocusStatus index.");
            return OMX_ErrorUnsupportedIndex;
        case OMX_IndexConfigCommonFrameStabilisation:
#ifdef STAB
            mStabType = *(OMX_CONFIG_FRAMESTABTYPE*)p;
            /* apply directly, because we need this to be enabled before passing into Idle.
             * Note: p is actually unused in applyConfig */
            ((Camera*)(&getProcessingComponent()))->applyConfig(OMX_IndexConfigCommonFrameStabilisation, p);
            return OMX_ErrorNone;
#else
            return OMX_ErrorNotImplemented;
#endif
        case OMX_IndexConfigCallbackRequest:
        {
            OMX_CONFIG_CALLBACKREQUESTTYPE* pCallbackRequest = (OMX_CONFIG_CALLBACKREQUESTTYPE *) p;
            switch((t_uint32)(pCallbackRequest->nIndex)) {
                case OMX_Symbian_IndexConfigExposureInitiated:
                    mExposureInitiatedClbkReq = *pCallbackRequest;
                break;
                case OMX_Symbian_IndexConfigCameraTesting:
                {
                    OMX_STATETYPE mState = OMX_StateInvalid;
                    GetState(this, &mState);
                    if (OMX_StateLoaded == mState)
                    {
                        iLoadedSelfTest.callbackEnabled = pCallbackRequest->bEnable;
                    }
                    else
                    {
                        mCallbackRequests.CameraTesting = *pCallbackRequest;
                    }
                }
                break;
                case OMX_Symbian_IndexConfigExposureLock:
                    mCallbackRequests.ExposureLock = *pCallbackRequest;
                    break;
                case OMX_Symbian_IndexConfigWhiteBalanceLock:
                    mCallbackRequests.WhiteBalanceLock = *pCallbackRequest;
                    break;
                case OMX_Symbian_IndexConfigFocusLock:
                    mCallbackRequests.FocusLock = *pCallbackRequest;
                    break;
                case OMX_Symbian_IndexConfigAllLock:
                    mCallbackRequests.AllLock = *pCallbackRequest;
                    break;
                case OMX_Symbian_IndexConfigExtFocusStatus:
                    mCallbackRequests.ExtFocusStatus = *pCallbackRequest;
                    break;
				case OMX_Symbian_IndexConfigPreCaptureExposureTime:
                    mCallbackRequests.PreCaptureExposureTime = *pCallbackRequest;
                    break;
                case OMX_STE_IndexConfigCommonZoomMode:
                    mZoomModeClbkReq = *pCallbackRequest;
                    break;
                case OMX_STE_IndexConfigMeteringOn:
                    mCallbackRequests.MeteringPrefashCallback = *pCallbackRequest;
                    break;

                default:
                    return OMX_ErrorUnsupportedSetting;
            }
            return OMX_ErrorNone;
        }
        case IFM_IndexConfigCommonReadPe:
            mIfmReadPEType = *(IFM_CONFIG_READPETYPE*)p;
            break;
        case IFM_IndexConfigCommonWritePe:
            mIfmWritePEType = *(IFM_CONFIG_WRITEPETYPE*)p;
            break;
        case IFM_IndexConfig3ATraceDumpControl:
            mIfm3ATraceDumpControl = *(IFM_CONFIG_3A_TRACEDUMPCONTROL*)p;
            break;
        case IFM_IndexConfigCheckAlive:
	    mIfmCheckAlive = *(IFM_CONFIG_CHECK_ALIVE*)p;
	    break;
        case OMX_Symbian_IndexConfigMotionLevel:
            mMotionLevel = *(OMX_SYMBIAN_CONFIG_U32TYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigNDFilterControl:
            mNDFilterControl = *(OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigExposureLock:
            mExposureLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigWhiteBalanceLock:
            mWhiteBalanceLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigFocusLock:
            mFocusLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigAllLock:
            mAllLock = *(OMX_SYMBIAN_CONFIG_LOCKTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigROI:
            mROI = *(OMX_SYMBIAN_CONFIG_ROITYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigRAWPreset:
            if (CAMERA_PORT_OUT1 != ((OMX_SYMBIAN_CONFIG_RAWPRESETTYPE*)p)->nPortIndex)
            {
                return OMX_ErrorBadParameter;
            }
            mRAWPreset = *(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE*)p;
            return OMX_ErrorNone;
        case OMX_IndexConfigCommonMirror:
            if (OMX_ALL != ((OMX_CONFIG_MIRRORTYPE*)p)->nPortIndex)
              {
                return OMX_ErrorBadParameter;
              }
              mMirror = *(OMX_CONFIG_MIRRORTYPE*)p;
              return OMX_ErrorNone;

        case OMX_Symbian_IndexParamRevertAspectRatio:
        {
            mRevertAspectRatio = *(OMX_CONFIG_BOOLEANTYPE*)p;
            OMX_STATETYPE mState = OMX_StateInvalid;
            GetState(this, &mState);

            OMX_PARAM_PORTDEFINITIONTYPE mRecord0_params,mRecord2_params;
            mRecord0_params.nSize = mRecord2_params.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mRecord0_params.nVersion);
            getOmxIlSpecVersion(&mRecord2_params.nVersion);
            getPort(CAMERA_PORT_OUT0)->getParameter(OMX_IndexParamPortDefinition,&mRecord0_params);
            getPort(CAMERA_PORT_OUT2)->getParameter(OMX_IndexParamPortDefinition,&mRecord2_params);
            /* Check if VPB0 and VPB2 port are disabled or in Loaded state*/
            if ( ((mRecord0_params.bEnabled == OMX_FALSE) && (mRecord2_params.bEnabled == OMX_FALSE)) || (OMX_StateLoaded == mState) ){
                if (mRevertAspectRatio.bEnabled == OMX_TRUE){
                    /* swap nFrameWidth and nFrameHeight */
                    OMX_U32 temp = mRecord0_params.format.video.nFrameWidth;
                    mRecord0_params.format.video.nFrameWidth = mRecord0_params.format.video.nFrameHeight;
                    mRecord0_params.format.video.nFrameHeight = temp;
                    mRecord0_params.format.video.nStride=0;	 /*Recalculate by setParameter */
                    mRecord0_params.format.video.nSliceHeight =0; /*recalculate by setParameter */
                    temp = mRecord2_params.format.video.nFrameWidth;
                    mRecord2_params.format.video.nFrameWidth = mRecord2_params.format.video.nFrameHeight;
                    mRecord2_params.format.video.nFrameHeight = temp;
                    mRecord2_params.format.video.nStride=0;	 /*Recalculate by setParameter */
                    mRecord2_params.format.video.nSliceHeight =0; /*recalculate by setParameter */
                }
                /* Update the port0 parameters */
                getPort(CAMERA_PORT_OUT0)->setParameter(OMX_IndexParamPortDefinition,&mRecord0_params);
                /* Send an OMX_EventPortSettingsChanged to the client */
                eventHandler(OMX_EventPortSettingsChanged, CAMERA_PORT_OUT0,(OMX_U32)0);
                /* Update the port2 parameters */
                getPort(CAMERA_PORT_OUT2)->setParameter(OMX_IndexParamPortDefinition,&mRecord2_params);
                /* Send an OMX_EventPortSettingsChanged to the client */
                eventHandler(OMX_EventPortSettingsChanged, CAMERA_PORT_OUT2,(OMX_U32)0);
                return OMX_ErrorNone;
            }
            else {
                return OMX_ErrorInvalidState;
            }
        }

        case OMX_IndexConfigCommonRotate:
        {
            mRotation = *(OMX_CONFIG_ROTATIONTYPE*)p;
            if (CAMERA_PORT_OUT2 == mRotation.nPortIndex)
            {
                OMX_S32 rotation= mRotation.nRotation;
                if (rotation == 0 || rotation == 90 || rotation == 270 || rotation == -90 || rotation == -270 || rotation == 360)
                {
                    OMX_STATETYPE mState = OMX_StateInvalid;
                    GetState(this, &mState);
                    OMX_PARAM_PORTDEFINITIONTYPE mRecord_params;
                    mRecord_params.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                    getOmxIlSpecVersion(&mRecord_params.nVersion);
                    getPort(CAMERA_PORT_OUT2)->getParameter(OMX_IndexParamPortDefinition,&mRecord_params);
                    /* Check if the port is disabled or in Loaded state*/
                    if ( (mRecord_params.bEnabled == OMX_FALSE) || (OMX_StateLoaded == mState) ){
// nRot| Rot | Swap | => Swap = nRot^Rot
//-----|-----|------|
//    0|    0|    0 |
//   90|    0|    1 |
//  270|    0|    1 |
//    0|   90|    1 |
//   90|   90|    0 |
//  270|   90|    0 |
//    0|  270|    1 |
//   90|  270|    0 |
//  270|  270|    0 |
                        if ( (nRotation!=0)^(rotation!=0) ){
                            /* Check if permutation port format.frame nedeed */
                            /* swap nFrameWidth and nFrameHeight */
                            OMX_U32 temp = mRecord_params.format.video.nFrameWidth;
                            mRecord_params.format.video.nFrameWidth = mRecord_params.format.video.nFrameHeight;
                            mRecord_params.format.video.nFrameHeight = temp;
                            mRecord_params.format.video.nStride=0;	 /*Recalculate by setParameter */
                            mRecord_params.format.video.nSliceHeight =0; /*recalculate by setParameter */
                        }
                        /* Update the port parameters and portRecord->portSettingsChanged = OMX_TRUE; */
                        getPort(CAMERA_PORT_OUT2)->setParameter(OMX_IndexParamPortDefinition,&mRecord_params);
                        /* Send an OMX_EventPortSettingsChanged to the client */
                        eventHandler(OMX_EventPortSettingsChanged,CAMERA_PORT_OUT2,(OMX_U32)0);
                        /* Update the nRotation parameter */
                        nRotation = rotation;
                    } else {
                        return OMX_ErrorInvalidState;
                    }
                } else if (rotation == 180) {
                    return OMX_ErrorHardware;                   /*Rotation value not supported by hardware */
                } else {
                    return OMX_ErrorUnsupportedSetting; /*Or rotation value not supported */
                }
            } else if (CAMERA_PORT_OUT1 == mRotation.nPortIndex) {
                OMX_S32 rotation = mRotation.nRotation;
                if (rotation == 0 || rotation == 90 || rotation == 270 || rotation == -90 || rotation == -270 || rotation == 360) {
                    OMX_STATETYPE mState = OMX_StateInvalid;
                    GetState(this, &mState);
                    OMX_PARAM_PORTDEFINITIONTYPE mRecord_params;
                    mRecord_params.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                    getOmxIlSpecVersion(&mRecord_params.nVersion);
                    getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition, &mRecord_params);
                    if (mRecord_params.format.video.eColorFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) {
                        /* Check if the port is disabled or in Loaded state*/
                        if ((mRecord_params.bEnabled == OMX_FALSE) || (OMX_StateLoaded == mState)) {
// nRot| Rot | Swap | => Swap = nRot^Rot
//-----|-----|------|
//    0|    0|    0 |
//   90|    0|    1 |
//  270|    0|    1 |
//    0|   90|    1 |
//   90|   90|    0 |
//  270|   90|    0 |
//    0|  270|    1 |
//   90|  270|    0 |
//  270|  270|    0 |
                            if ((nRotation != 0) ^ (rotation != 0)) {
                                /* Check if permutation port format.frame nedeed */
                                /* swap nFrameWidth and nFrameHeight */
                                OMX_U32 temp = mRecord_params.format.video.nFrameWidth;
                                mRecord_params.format.video.nFrameWidth = mRecord_params.format.video.nFrameHeight;
                                mRecord_params.format.video.nFrameHeight = temp;
                                mRecord_params.format.video.nStride = 0; /*Recalculate by setParameter */
                                mRecord_params.format.video.nSliceHeight = 0; /*recalculate by setParameter */
                            }
                            /* Update the port parameters and portRecord->portSettingsChanged = OMX_TRUE; */
                            getPort(CAMERA_PORT_OUT1)->setParameter(OMX_IndexParamPortDefinition, &mRecord_params);
                            /* Send an OMX_EventPortSettingsChanged to the client */
                            eventHandler(OMX_EventPortSettingsChanged, CAMERA_PORT_OUT1, (OMX_U32)0);
                            /* Update the nRotation parameter */
                            nRotation = rotation;
                        } else {
                            return OMX_ErrorInvalidState;
                        }
                    } else {
                        return OMX_ErrorUnsupportedSetting;     //WRONG SETTING FOR RIGHT PORT
                    }
                } else if (rotation == 180) {
                    return OMX_ErrorHardware;                   /*Rotation value not supported by hardware */
                } else {
                    return OMX_ErrorUnsupportedSetting; /*Or rotation value not supported */
                }
            } else {
                return OMX_ErrorBadPortIndex; /*Only rotation on VPB2*/
            }
            return OMX_ErrorNone;;
        }
        case OMX_Symbian_IndexConfigCameraSensorInfo:
            return OMX_ErrorBadParameter; // read only index
        case OMX_Symbian_IndexConfigFlashGunInfo:
            return OMX_ErrorBadParameter; // read only index
        case OMX_Symbian_IndexConfigSceneMode:
        {
            if(((OMX_SYMBIAN_CONFIG_SCENEMODETYPE*)p)->eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR)
            {
                OMX_PARAM_PORTDEFINITIONTYPE mHDRParams;
                mHDRParams.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                getOmxIlSpecVersion(&mHDRParams.nVersion);
                getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition, &mHDRParams);
                //Check for nBufferCountActual
                MSG1("HDR: OMX_STE_Scene_HDR: ParamPortDefinition nBufferCountActual = %ld\n", mHDRParams.nBufferCountActual);
                if(mHDRParams.nBufferCountActual < ZSLHDR_MAX_FRAMELIMIT)
                {
                    MSG1("HDR: nBufferCountActual is less than %d, generating OMX_EventPortSettingsChanged event \n", ZSLHDR_MAX_FRAMELIMIT);
                    mHDRParams.nBufferCountActual = ZSLHDR_MAX_FRAMELIMIT;
                    getPort(CAMERA_PORT_OUT1)->setParameter(OMX_IndexParamPortDefinition, &mHDRParams);
                    /* Send an OMX_EventPortSettingsChanged to the client */
                    eventHandler(OMX_EventPortSettingsChanged, CAMERA_PORT_OUT1, (OMX_U32)0);
                    return OMX_ErrorNone;
                }
            }
            mSceneMode = *(OMX_SYMBIAN_CONFIG_SCENEMODETYPE*)p;
            return OMX_ErrorNone;
        }
        case OMX_Symbian_IndexConfigSharpness:
        {
            OMX_SYMBIAN_CONFIG_S32TYPE *pSharpness = (OMX_SYMBIAN_CONFIG_S32TYPE*) p;
            if (pSharpness->nValue < -100 || pSharpness->nValue > 100) {
                return OMX_ErrorBadParameter;
            } else {
                mSharpness = *pSharpness;
                return OMX_ErrorNone;
            }
        }
        case OMX_Symbian_IndexConfigFlickerRemoval:
            mFlickerRemoval = *(OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE*)p;
            break;
        case OMX_Symbian_IndexConfigOrientationScene:
        {
            OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE* pOrientation = (OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE*)p;
            if(    pOrientation->eOrientation != OMX_SYMBIAN_OrientationNotSpecified
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowTopColumnLeft
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowTopColumnRight
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowBottomColumnRight
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowBottomColumnLeft
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowLeftColumnTop
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowRightColumnTop
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowRightColumnBottom
                && pOrientation->eOrientation != OMX_SYMBIAN_OrientationRowLeftColumnBottom )
            {
                return OMX_ErrorBadParameter;
            }
            mSceneOrientation = *pOrientation;
            return OMX_ErrorNone;
        }
        case OMX_Symbian_IndexConfigBracketing:
        {
            OMX_SYMBIAN_CONFIG_BRACKETINGTYPE *pBracket = (OMX_SYMBIAN_CONFIG_BRACKETINGTYPE*)p;
            if    ((pBracket->nSize > IFM_CONFIG_BRACKETING_SIZE(IFM_MAX_NUMBER_OF_BRACKETING_VALUES))
               || (pBracket->nNbrBracketingValues > IFM_MAX_NUMBER_OF_BRACKETING_VALUES)){
                DBGT_ERROR("Error: OMX_SYMBIAN_CONFIG_BRACKETINGTYPE variable size structure is too big. nSize=%lu nNbrBracketingValues=%lu. The maximum bracketing value is %d.\n", pBracket->nSize, pBracket->nNbrBracketingValues, IFM_MAX_NUMBER_OF_BRACKETING_VALUES);
                OstTraceFiltInst2(TRACE_ERROR, "Error: OMX_SYMBIAN_CONFIG_BRACKETINGTYPE variable size structure is too big. nSize=%lu nNbrBracketingValues=%lu. The maximum bracketing value is 3.", pBracket->nSize, pBracket->nNbrBracketingValues);
                return OMX_ErrorBadParameter;
            }
            if (pBracket->nSize < IFM_CONFIG_BRACKETING_SIZE(pBracket->nNbrBracketingValues )) {
                DBGT_ERROR("Error: OMX_SYMBIAN_CONFIG_BRACKETINGTYPE nSize=%lu is inconsistent with nNbrBracketingValues=%lu\n", pBracket->nSize, pBracket->nNbrBracketingValues);
                OstTraceFiltInst2(TRACE_ERROR, "Error: OMX_SYMBIAN_CONFIG_BRACKETINGTYPE nSize=%lu is inconsistent with nNbrBracketingValues=%lu", pBracket->nSize, pBracket->nNbrBracketingValues);
                return OMX_ErrorBadParameter; // Do not return an error until mmte is fixed: it currently reports a wrong nSize although the rest of the structure is sane.
            }
            if(mSceneMode.eSceneType==(OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_HDR) {
                if(pBracket->nNbrBracketingValues > ZSLHDR_MAX_FRAMELIMIT) {
                    pBracket->nNbrBracketingValues = ZSLHDR_MAX_FRAMELIMIT;
                }
            }
            memcpy((void*)&mBracketing, pBracket, IFM_CONFIG_BRACKETING_SIZE(pBracket->nNbrBracketingValues));
            return OMX_ErrorNone;
        }
        case OMX_Symbian_IndexConfigCameraSelfTestSelect:
            mSelfTestType = *(OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE*)p;
            return OMX_ErrorNone;
        case OMX_Symbian_IndexConfigCameraTesting:
        {
            mTestingType = *(OMX_CONFIG_BOOLEANTYPE*)p;
            if (getCompFsm()->getOMXState() ==  OMX_StateLoaded)
            {
                iLoadedSelfTest.StartFlashSelftests();
            }
            return OMX_ErrorNone;
        }
        case IFM_IndexLatency_Hsmcamera:
        {
            return OMX_ErrorNone;
        }
        case OMX_STE_IndexConfigFrameSelectRatio:
        {
            if ((CAMERA_PORT_OUT1 != ((OMX_STE_CONFIG_FRAMESELECTRATIOTYPE*)p)->nPortIndex)
                || (0 == ((OMX_STE_CONFIG_FRAMESELECTRATIOTYPE*)p)->nSelectRatio)) {
                return OMX_ErrorBadParameter;
            }
            mVariableFramerateKeepRatio = *(OMX_STE_CONFIG_FRAMESELECTRATIOTYPE*)p;
            return OMX_ErrorNone;
        }

        case OMX_Symbian_IndexConfigAnalyzerFeedback:
        {
            OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pILFeedback = (OMX_SYMBIAN_CONFIG_FEEDBACKTYPE*)p;
            OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pCamFeedback = NULL;
            OMX_U32 nCamFeedbackValueSize = 0;
            bool* pCamFeedbackChanged = NULL;

            switch( pILFeedback->nFeedbackType )
            {
                case OMX_SYMBIAN_FeedbackFaceTracker:
                    pCamFeedback = &mAnalyzersFeedbacks.sFaceTracker.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
                    pCamFeedbackChanged = &mAnalyzersFeedbacks.bFaceTrackerFeedbackReceived;
                    break;
                case OMX_SYMBIAN_FeedbackObjectTracker:
                    pCamFeedback = &mAnalyzersFeedbacks.sObjectTracker.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
                    pCamFeedbackChanged = &mAnalyzersFeedbacks.bObjectTrackerFeedbackReceived;
                    break;
                case OMX_SYMBIAN_FeedbackMotionLevel:
                    pCamFeedback = &mAnalyzersFeedbacks.sMotionLevel.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_U32);
                    pCamFeedbackChanged = &mAnalyzersFeedbacks.bMotionLevelFeedbackReceived;
                    break;
                case OMX_SYMBIAN_FeedbackAutomaticSceneDetection:
                    pCamFeedback = &mAnalyzersFeedbacks.sSceneDetection.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_SCENEMODETYPE);
                    pCamFeedbackChanged = &mAnalyzersFeedbacks.bSceneDetectionFeedbackReceived;
                    break;
                default:
                    // Search if opaque feedback was already received
                    for(int i=0; i<mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived; i++) {
                       if( mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackType == pILFeedback->nFeedbackType) {
                           // Opaque feedback was already received: reuse this slot
                           pCamFeedback = &mAnalyzersFeedbacks.sOpaque[i].sFeedback;
                           nCamFeedbackValueSize = IFM_CONFIG_OPAQUE_FEEDBACK_SIZE;
                           pCamFeedbackChanged = &mAnalyzersFeedbacks.bOpaqueFeedbackReceived[i];
                           break;
                       }
                    }
                    // If opaque feedback was not yet received, then use an empty slot
                    if( pCamFeedback == NULL ) {
                        if( mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived < IFM_MAX_NUMBER_OF_OPAQUE_FEEDBACKS ) {
                           // An empty slot is available
                           pCamFeedback = &mAnalyzersFeedbacks.sOpaque[mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived].sFeedback;
                           nCamFeedbackValueSize = IFM_CONFIG_OPAQUE_FEEDBACK_SIZE;
                           pCamFeedbackChanged = &mAnalyzersFeedbacks.bOpaqueFeedbackReceived[mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived];
                           mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived++;
                        }
                        else {
                            // No more empty slot
                            return OMX_ErrorInsufficientResources;
                        }
                    }
                    break;
            }

            // Check if IL client's feedback is correctly sized
            if ( pILFeedback->nFeedbackValueSize < nCamFeedbackValueSize )
            {
                // IL client value size is too small
                return OMX_ErrorBadParameter;
            }
            else
            {
                // Save IL client's feeback
                *pCamFeedback = *pILFeedback;
                memcpy(pCamFeedback->nFeedbackValue, pILFeedback->nFeedbackValue, nCamFeedbackValueSize);
                pCamFeedback->nSize = IFM_CONFIG_FEEDBACK_SIZE(nCamFeedbackValueSize);
                pCamFeedback->nFeedbackValueSize = nCamFeedbackValueSize;
                *pCamFeedbackChanged = true;
            }
            break;
        }
	 case OMX_STE_IndexConfigCommonZoomMode:
	 	mZoomMode = *(OMX_CONFIG_BOOLEANTYPE *)p;
//		MSG1("COmxCamera::setConfig. [smooth zoom] SmoothZoom = %s\n", (mZoomMode.bEnabled == OMX_TRUE)?"ENABLED":"DISABLED");
	     break;
	case IFM_IndexConfigSoftware:
		mSoftwareVersion= *(IFM_CONFIG_SOFTWAREVERSION*)p;									
		break;
#ifdef FREQ_SCALING_ALLOWED
        case OMX_STE_IndexConfigCommonScaling:
        {
            IFM_CONFIG_SCALING_MODETYPE *freqscaling = (IFM_CONFIG_SCALING_MODETYPE *)p;
            if(freqscaling->sResource >= TRESOURCEID_MAX)
            {
                return OMX_ErrorBadParameter;
            }
            else
            {
                OMX_HANDLETYPE aClientHandle;
                aClientHandle = getOMXHandle();
                if(freqscaling->scalingFactor[freqscaling->sResource] == 100 )
                {
                    omxilosalservices::OmxILOsalPrm::ReleaseResourceState(freqscaling->sResource,aClientHandle);
                }
                else
                {
                    omxilosalservices::OmxILOsalPrm::ChangeResourceState(freqscaling->sResource,freqscaling->scalingFactor[freqscaling->sResource], aClientHandle);
                }
                mfreqscaling.scalingFactor[freqscaling->sResource]=freqscaling->scalingFactor[freqscaling->sResource];
            }
                break;
        }
#endif //FREQ_SCALING_ALLOWED
    case OMX_STE_IndexConfigMeteringOn:
            mMeteringOn = *(OMX_CONFIG_BOOLEANTYPE*)p;
            return OMX_ErrorNone;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
          case OMX_3A_IndexConfigIspParameters:
              mIspParameters = *(OMX_3A_CONFIG_ISPPARAMETERSTYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigExposure:
              mExposure = *(OMX_3A_CONFIG_EXPOSURETYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigGamma:
              mGamma = *(OMX_3A_CONFIG_GAMMATABLE_TYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigLsc:
              mLsc = *(OMX_3A_CONFIG_LSCTABLE_TYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigLinearizer:
              mLinearizer = *(OMX_3A_CONFIG_LINEARIZERTABLE_TYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigFocus:
              mFocus = *(OMX_3A_CONFIG_FOCUSTYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigFlashSynchro:
              mFlashSynchro = *(OMX_3A_CONFIG_FLASHSYNCHROTYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigMaxFramerate:
              mMaxFramerate = *(OMX_3A_CONFIG_MAXFRAMERATETYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigSensorMode:
              mSensorModeparam = *(OMX_3A_CONFIG_SENSORMODETYPE*)p;
              return OMX_ErrorNone;
          case OMX_3A_IndexConfigEEPROMData:
              mEepromdata = *(OMX_3A_CONFIG_EEPROMDATATYPE*)p;
              cam->iSkipNVM = OMX_TRUE;
              return OMX_ErrorNone;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
     case OMX_STE_IndexConfigExifTimestamp:
	 	mExifTimeStamp = *(OMX_TIME_CONFIG_TIMESTAMPTYPE *)p;
	 	return OMX_ErrorNone;
        default :
            return ENS_Component::setConfig(nIndex,p);
    }
    return OMX_ErrorNone;
}

/* @brief applies to the camera the locally stored OMX config of virtually all indexes
 */
OMX_ERRORTYPE COmxCamera::applyOMXConfig()
{
    Camera* Cam = (Camera*)(&getProcessingComponent());
#define stringify(a) #a
#define APPLY_CONFIG(_index_,_omxconfig_) \
do { \
    /*printf("APPLY_CONFIG(%s,?)\n",stringify(_index_));*/ \
    Cam->applyConfig((OMX_INDEXTYPE)_index_, (OMX_PTR)&_omxconfig_); \
} while(0)

    // GPL APPLY_CONFIG(IFM_IndexConfig3ATraceDumpControl ,mIfm3ATraceDumpControl);
    // APPLY_CONFIG(IFM_IndexConfigCommonOpMode_Check); Read-only
    // APPLY_CONFIG(IFM_IndexConfigCommonReadPe, mIfmReadPEType); // Not applicable because we don't have a particular PE we would want to read
    // APPLY_CONFIG(IFM_IndexConfigCommonWritePe, mIfmWritePEType); // Not applicable because we don't have a particular PE we would want to write
    // JDU APPLY_CONFIG(IFM_IndexConfigTestMode, mTestModeType);
    // JDU APPLY_CONFIG(OMX_IndexAutoPauseAfterCapture, mAutoPauseAfterCapture);
    APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.ExposureLock);
    APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.WhiteBalanceLock);
    APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.FocusLock);
    APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.AllLock);
    APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.ExtFocusStatus);
	APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.PreCaptureExposureTime);
    // APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mCallbackRequests.CameraTesting); // No! It is handled differently in loaded
    //APPLY_CONFIG(OMX_IndexConfigCallbackRequest, mZoomModeClbkReq);

    APPLY_CONFIG(OMX_IndexConfigCaptureMode, mCaptureModeType);
    APPLY_CONFIG(OMX_IndexConfigCommonBrightness, mBrightness);
    APPLY_CONFIG(OMX_IndexConfigCommonColorBlend, mColorBlendType);
    APPLY_CONFIG(OMX_IndexConfigCommonContrast, mContrast);
    APPLY_CONFIG(OMX_IndexConfigCommonExposure, mExposureControl);
    APPLY_CONFIG(OMX_IndexConfigCommonExposureValue, mExposureValue);

    // APPLY_CONFIG(OMX_IndexConfigCommonFocusRegion); Deprecated by OMX_Symbian_IndexConfigFocusRegion
    APPLY_CONFIG(OMX_IndexConfigCommonFrameStabilisation, mStabType);
    APPLY_CONFIG(OMX_IndexConfigCommonImageFilter, mImageFilterType);
    APPLY_CONFIG(OMX_IndexConfigCommonMirror, mMirror);

    APPLY_CONFIG(OMX_IndexConfigCommonSaturation, mSaturation);
    APPLY_CONFIG(OMX_IndexConfigCommonWhiteBalance, mWhiteBalControl); // ER317047
    APPLY_CONFIG(OMX_IndexConfigFocusControl, mFocusControl);
	
    // APPLY_CONFIG(OMX_IndexParamCommonSensorMode, mSensorMode); Is a param, not an index. TODO: something
    // APPLY_CONFIG(OMX_IndexParamVideoInit, portTypesParam); Is a param, not an index. TODO: something
    APPLY_CONFIG(OMX_Symbian_IndexConfigAFAssistantLight, mFlashAFAssist);
    APPLY_CONFIG(OMX_Symbian_IndexConfigAllLock, mAllLock);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigApplyUserSettings); Unused

    APPLY_CONFIG(OMX_Symbian_IndexConfigBracketing, mBracketing.config);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigCameraFuncTestSelect); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigCameraSelfTestSelect, mSelfTestType);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigCameraSensorInfo, mSensorInfo); Read-only
    APPLY_CONFIG(OMX_Symbian_IndexConfigCameraTesting, mTestingType);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigCenterFieldOfView, mCenterFieldOfViewStatusType); // Not applicable because pipe not streaming. Will probably be done in Idle-to-Executing transition.
    APPLY_CONFIG(OMX_Symbian_IndexConfigCommonExtCapturing, mCapturingStruct_VPB1);
    APPLY_CONFIG(OMX_Symbian_IndexConfigCommonExtCapturing, mCapturingStruct_VPB2);

    // APPLY_CONFIG(OMX_Symbian_IndexConfigExposureInitiated); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigExposureLock, mExposureLock);
    APPLY_CONFIG(OMX_Symbian_IndexConfigExtCaptureMode, mExtCaptureModeType);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigExtDigitalZoom, mZoomFactorStatusType); // Not applicable because pipe not streaming. Done in Idle-to-Executing transition.
    // APPLY_CONFIG(OMX_Symbian_IndexConfigExtFocusStatus, mExtFocusStatus); // read-only
    APPLY_CONFIG(OMX_Symbian_IndexConfigExtOpticalZoom, mOpticalZoomFactor);
    APPLY_CONFIG(OMX_Symbian_IndexConfigFlashControl, mFlashControlStruct); // fix ER 320662, ER317615
    // APPLY_CONFIG(OMX_Symbian_IndexConfigFlashGunInfo, mFlashgunInfo); Read-only

    APPLY_CONFIG(OMX_Symbian_IndexConfigFlickerRemoval, mFlickerRemoval);
    APPLY_CONFIG(OMX_Symbian_IndexConfigFocusLock, mFocusLock);
    APPLY_CONFIG(OMX_Symbian_IndexConfigFocusRange, mFocusRange);
    APPLY_CONFIG(OMX_Symbian_IndexConfigFocusRegion, mFocusRegion);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigHintPowerVsQuality); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigHistogramControl); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigImageMaxJpegSize); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigMotionLevel, mMotionLevel);
    APPLY_CONFIG(OMX_Symbian_IndexConfigNDFilterControl, mNDFilterControl);

    // JDU APPLY_CONFIG(OMX_Symbian_IndexConfigOrientationScene, mSceneOrientation);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigPreCaptureExposureTime); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigRAWPreset, mRAWPreset);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigREDComplexity); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigRGBHistogram); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigROI, mROI);

    // APPLY_CONFIG(OMX_Symbian_IndexConfigROISelection); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigRedEyeRemoval); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigSceneMode, mSceneMode);
    APPLY_CONFIG(OMX_Symbian_IndexConfigSharpness, mSharpness);
    APPLY_CONFIG(OMX_Symbian_IndexConfigVideoLight, mVideoLight);
    APPLY_CONFIG(OMX_Symbian_IndexConfigWhiteBalanceLock, mWhiteBalanceLock);
    // APPLY_CONFIG(OMX_Symbian_IndexConfigXenonFlashStatus); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexConfigXenonLifeCounter); Unused
    // APPLY_CONFIG(OMX_Symbian_IndexParamRevertAspectRatio); Unused
    APPLY_CONFIG(OMX_Symbian_IndexConfigAnalyzerFeedback , mAnalyzersFeedbacks.sFaceTracker.sFeedback);
    APPLY_CONFIG(OMX_Symbian_IndexConfigAnalyzerFeedback , mAnalyzersFeedbacks.sObjectTracker.sFeedback);
    APPLY_CONFIG(OMX_Symbian_IndexConfigAnalyzerFeedback , mAnalyzersFeedbacks.sMotionLevel.sFeedback);
    APPLY_CONFIG(OMX_Symbian_IndexConfigAnalyzerFeedback , mAnalyzersFeedbacks.sSceneDetection.sFeedback);
    APPLY_CONFIG(OMX_STE_IndexConfigMeteringOn, mMeteringOn);

    //APPLY_CONFIG(OMX_STE_IndexConfigCommonZoomMode, mZoomMode);

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    APPLY_CONFIG(OMX_3A_IndexConfigIspParameters , mIspParameters);
    APPLY_CONFIG(OMX_3A_IndexConfigGamma , mGamma);
    APPLY_CONFIG(OMX_3A_IndexConfigLsc , mLsc);
    APPLY_CONFIG(OMX_3A_IndexConfigLinearizer , mLinearizer);
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
   APPLY_CONFIG(OMX_STE_IndexConfigExifTimestamp, mExifTimeStamp);
    return OMX_ErrorNone;
}

#define CHECK_INPUT_SIZE_VERSION(_indextype_) \
{ \
    (_indextype_) *extconfig = ((_indextype_)*)p; \
    if (extconfig->nSize < sizeof(_indextype_)) return OMX_ErrorBadParameter; \
    if (OMX_VERSION_1_1_2 != extconfig->nVersion.nVersion) return OMX_ErrorBadParameter; \
}
#define GET_CONFIG_COMMON(_omxcamconfig_,_indextype_) \
{ \
    CHECK_INPUT_SIZE_VERSION(_indextype_); \
    memcpy(p, &(_omxcamconfig_), sizeof(_indextype_)); \
}




OMX_ERRORTYPE COmxCamera::getConfig(OMX_INDEXTYPE idx, OMX_PTR p) const
{
    switch ((t_uint32)idx)
    {
        case OMX_Symbian_IndexConfigAFAssistantLight:
            *((OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE *) p) = mFlashAFAssist;
            break;
        case OMX_Symbian_IndexConfigFlashControl:
            *((OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE *) p) = mFlashControlStruct;
            break;
        case OMX_Symbian_IndexConfigCommonExtCapturing:
            switch(((OMX_SYMBIAN_CONFIG_BOOLEANTYPE*)p)->nPortIndex)
            {
                case CAMERA_PORT_OUT1:
                    *((OMX_SYMBIAN_CONFIG_BOOLEANTYPE *) p) = mCapturingStruct_VPB1;
                    break;
                case CAMERA_PORT_OUT2:
                    *((OMX_SYMBIAN_CONFIG_BOOLEANTYPE *) p) = mCapturingStruct_VPB2;
                    break;
                default:
                    // Following OMX norm, we should return an error because the OMX client is supposed to have initialised the config correctly.
                    // A non standard permissive implementation would be to copy mCapturingStruct_VPB1.
                    return OMX_ErrorBadParameter;
            }
            break;
        case OMX_IndexAutoPauseAfterCapture:
            *((OMX_CONFIG_BOOLEANTYPE *) p) = mAutoPauseAfterCapture;
            break;
        case OMX_IndexConfigCaptureMode:
            {
            OMX_CONFIG_CAPTUREMODETYPE * tempCaptureMode = (OMX_CONFIG_CAPTUREMODETYPE *) p;
            OMX_U32 portIdx = tempCaptureMode->nPortIndex;
            *(tempCaptureMode) = mCaptureModeType;
            tempCaptureMode->nPortIndex = portIdx;
            }
            break;
        case OMX_Symbian_IndexConfigExtCaptureMode:
            if(CAMERA_PORT_OUT1 != ((OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE*)p)->nPortIndex)
            {
                return OMX_ErrorBadParameter;
            }
            *((OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE *) p) = mExtCaptureModeType;
            break;
        case OMX_IndexConfigCommonColorBlend:
            *((OMX_CONFIG_COLORBLENDTYPE *) p) = mColorBlendType;
            break;
	  case OMX_Symbian_IndexConfigRGBHistogram:
            *((OMX_SYMBIAN_CONFIG_RGBHISTOGRAM *) p) = mRGBHistogram;
            break;		
        case OMX_Symbian_IndexConfigExtDigitalZoom :
            *((OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE *) p) = mZoomFactorStatusType;
            break;
        case OMX_Symbian_IndexConfigCenterFieldOfView :
            *((OMX_SYMBIAN_CONFIG_POINTTYPE *) p) = mCenterFieldOfViewStatusType;
            break;
        case IFM_IndexConfigTestMode :
            *((OMX_STE_CONFIG_SENSORTESTTYPE *) p) = mTestModeType;
            break;
        case OMX_IndexConfigCommonWhiteBalance :
            *((OMX_CONFIG_WHITEBALCONTROLTYPE *) p) = mWhiteBalControl;
            break;
        case OMX_IndexConfigCommonBrightness :
            *((OMX_CONFIG_BRIGHTNESSTYPE *) p) = mBrightness;
            break;
        case OMX_IndexConfigCommonExposureValue:
            *((OMX_CONFIG_EXPOSUREVALUETYPE *) p) = mExposureValue;
            break;
        case OMX_IndexConfigCommonExposure:
            *((OMX_CONFIG_EXPOSURECONTROLTYPE *) p) = mExposureControl;
            break;
        case OMX_IndexConfigCommonContrast:
            *((OMX_CONFIG_CONTRASTTYPE *) p) = mContrast;
            break;
        case OMX_IndexConfigCommonSaturation:
            *((OMX_CONFIG_SATURATIONTYPE *) p) = mSaturation;
            break;
        case OMX_IndexConfigCommonImageFilter:
            *((OMX_CONFIG_IMAGEFILTERTYPE *) p) = mImageFilterType;
            break;
        case OMX_IndexConfigFocusControl:
            *((OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE *) p) = mFocusControl;
            break;
        case OMX_Symbian_IndexConfigFocusRange:
            *((OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE *) p) = mFocusRange;
            break;
        case OMX_Symbian_IndexConfigExtFocusStatus:
            {
                OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE *pExtFocusStatus = (OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE*)p;
                /* Check that IL client structure size is consistent with number of AF areas it pretends having */
                if (pExtFocusStatus->nSize < IFM_CONFIG_EXTFOCUSSTATUS_SIZE(pExtFocusStatus->nMaxAFAreas)) {
                    return OMX_ErrorBadParameter;
                }
                /* From OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE description:
                 * "In case client has set to zero when calling GetConfig component updates to the max number of focus regions and
                 *  no other fields are updated by component. In case client has set this to non-zero when calling GetConfig component
                 *  does not update it and only reports AF regions up to this number" */
                else if (0 == pExtFocusStatus->nMaxAFAreas ) {
                    pExtFocusStatus->nMaxAFAreas = mExtFocusStatus.config.nMaxAFAreas;
                } else {
                    DBC_ASSERT(OMX_VERSION_1_1_2 == pExtFocusStatus->nVersion.nVersion) ;
                    pExtFocusStatus->nAFAreas = MIN(mExtFocusStatus.config.nAFAreas, pExtFocusStatus->nMaxAFAreas);
                    pExtFocusStatus->bFocused = mExtFocusStatus.config.bFocused;
                    for (unsigned int i=0; i < pExtFocusStatus->nAFAreas; i++) {
                        pExtFocusStatus->sAFROIs[i] = mExtFocusStatus.config.sAFROIs[i];
                    }
                }
            }
            break;
        case OMX_Symbian_IndexConfigROI: 
	    memcpy(p, &mROI, sizeof(OMX_SYMBIAN_CONFIG_ROITYPE));
	    break;
        case OMX_IndexConfigCallbackRequest:
            { // Fixme: if set in Loaded state, check how the sub-configs can be applied.
                OMX_CONFIG_CALLBACKREQUESTTYPE* pCallbackRequest = (OMX_CONFIG_CALLBACKREQUESTTYPE *) p;
                switch((t_uint32)pCallbackRequest->nIndex) {
                    case OMX_Symbian_IndexConfigExposureLock:
                        *pCallbackRequest = mCallbackRequests.ExposureLock;
                        break;
                    case OMX_Symbian_IndexConfigWhiteBalanceLock:
                        *pCallbackRequest = mCallbackRequests.WhiteBalanceLock;
                        break;
                    case OMX_Symbian_IndexConfigFocusLock:
                        *pCallbackRequest = mCallbackRequests.FocusLock;
                        break;
                    case OMX_Symbian_IndexConfigAllLock:
                        *pCallbackRequest = mCallbackRequests.AllLock;
                        break;
                    case OMX_Symbian_IndexConfigExtFocusStatus:
                         *pCallbackRequest = mCallbackRequests.ExtFocusStatus;
                          break;
                    case OMX_Symbian_IndexConfigCameraTesting:
                        pCallbackRequest->bEnable = iLoadedSelfTest.callbackEnabled;
                        break;
                    case OMX_Symbian_IndexConfigExposureInitiated:
                        memcpy(p, &mExposureInitiatedClbkReq, sizeof(OMX_CONFIG_CALLBACKREQUESTTYPE));
                        break;
                	case OMX_Symbian_IndexConfigPreCaptureExposureTime:
                        *pCallbackRequest = mCallbackRequests.PreCaptureExposureTime;
                        break;
                     case OMX_STE_IndexConfigCommonZoomMode:
                        *pCallbackRequest = mZoomModeClbkReq;
                        break;
                    case OMX_STE_IndexConfigMeteringOn:
                        *pCallbackRequest = mCallbackRequests.MeteringPrefashCallback;
                        break;
                    default: // callback is not supported/implemented
                        return OMX_ErrorUnsupportedSetting;
                }
                break;
            }
        case OMX_IndexConfigCommonFrameStabilisation:
            *((OMX_CONFIG_FRAMESTABTYPE *) p) = mStabType;
            break;
        case IFM_IndexConfigCommonReadPe :
            *((IFM_CONFIG_READPETYPE *) p) = mIfmReadPEType;
            break;
        case IFM_IndexConfigCommonWritePe :
            *((IFM_CONFIG_WRITEPETYPE *) p) = mIfmWritePEType;
            break;
        case IFM_IndexConfigCheckAlive:
            *((IFM_CONFIG_CHECK_ALIVE *) p) = mIfmCheckAlive;
            break;
        case IFM_IndexConfigCommonOpMode_Check :
            {
                IFM_CONFIG_OPMODE_CHECK* tmpStruct = (IFM_CONFIG_OPMODE_CHECK*)p;
                CopModeMgr* tmpOpMode = & ( ((Camera*)(&getProcessingComponent()))->iOpModeMgr );
                tmpStruct->nSize = sizeof(IFM_CONFIG_OPMODE_CHECK);
                tmpStruct->nVersion.nVersion = OMX_VERSION_1_1_2;
                tmpStruct->nPortIndex = OMX_ALL;
                tmpStruct->eOpMode = tmpOpMode->CurrentOperatingMode;
            }
            break;
        case OMX_Symbian_IndexConfigRAWPreset:
            *((OMX_SYMBIAN_CONFIG_RAWPRESETTYPE *) p) = mRAWPreset;
            break;
        case OMX_IndexConfigCommonMirror:
            *((OMX_CONFIG_MIRRORTYPE *) p) = mMirror;
            break;
        case OMX_IndexConfigCommonRotate:
            *((OMX_CONFIG_ROTATIONTYPE *) p) = mRotation;
            break;
        case OMX_Symbian_IndexConfigOrientationScene:
            *(OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE*)p = mSceneOrientation;
            break;
        case OMX_Symbian_IndexParamRevertAspectRatio:
            memcpy(p, &mRevertAspectRatio, sizeof(OMX_CONFIG_BOOLEANTYPE));
            break;
        case OMX_Symbian_IndexConfigCameraSensorInfo:
        {
            OMX_SYMBIAN_CONFIG_HWINFOTYPE * pSensorInfoType = (OMX_SYMBIAN_CONFIG_HWINFOTYPE*)p;
            *((OMX_SYMBIAN_CONFIG_HWINFOTYPE *) p) = mSensorInfo;
            Camera* cam = (Camera*)&getProcessingComponent();
            pSensorInfoType->nVersion1 =  (cam->iSensor->GetSensorManufacturer()&0xff)
                                        |((cam->iSensor->GetSensorRevNumber()&0xff)<<8)
                                        |((cam->iSensor->GetSensorModel()&0xffff)<<16);
            MSG1("OMX_Symbian_IndexConfigCameraSensorInfo nVersion1=0x%08lx\n", pSensorInfoType->nVersion1);
            OstTraceFiltInst1(TRACE_DEBUG, "OMX_Symbian_IndexConfigCameraSensorInfo nVersion1=0x%08lx", pSensorInfoType->nVersion1);
            break;
        }
        case OMX_Symbian_IndexConfigFlashGunInfo:
            *((OMX_SYMBIAN_CONFIG_HWINFOTYPE *) p) = mFlashgunInfo;
            break;
        case OMX_Symbian_IndexConfigSceneMode:
            *((OMX_SYMBIAN_CONFIG_SCENEMODETYPE *) p) = mSceneMode;
            break;
        case OMX_Symbian_IndexConfigFlickerRemoval:
            *((OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE *) p) = mFlickerRemoval;
            break;
        case OMX_Symbian_IndexConfigBracketing:
            {
                OMX_SYMBIAN_CONFIG_BRACKETINGTYPE *pBracket = (OMX_SYMBIAN_CONFIG_BRACKETINGTYPE *)p;
                int valuesCount = MIN(mBracketing.config.nNbrBracketingValues, pBracket->nNbrBracketingValues);
                memcpy(pBracket, &mBracketing.config, IFM_CONFIG_BRACKETING_SIZE(valuesCount));
                pBracket->nNbrBracketingValues = valuesCount;
                break;
            }
        case OMX_Symbian_IndexConfigCameraSelfTestSelect:
            *((OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE *) p) = mSelfTestType;
            break;
        case OMX_Symbian_IndexConfigCameraTesting:
            *((OMX_CONFIG_BOOLEANTYPE *) p) = mTestingType;
            break;
        case OMX_Symbian_IndexConfigFocusRegion:
            *((OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE *) p) = mFocusRegion;
            break;
        case OMX_Symbian_IndexConfigSupportedResolutions:
        {
           OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS *pRes = (OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS *)p;
           Camera* cam = (Camera*)&getProcessingComponent();
           return cam->getSupportedResolutions(pRes);
        }
        case OMX_Symbian_IndexConfigSharpness:
            *((OMX_SYMBIAN_CONFIG_S32TYPE *) p) = mSharpness;
            break;
        case OMX_Symbian_IndexConfigSensorNvmData:
            // OmxCamera is in LOADED state: sensor NVM is not yet loaded.
            return OMX_ErrorNotReady;
        case IFM_IndexLatency_Hsmcamera:
        {
            Camera* cam = (Camera*)&getProcessingComponent();
            cam->mlatency_Hsmcamera.e_OMXCAM_StartStreaming.t0 = cam->p_cam_sm->mTime_e_OMXCAM_StartStreaming_t0;
            cam->mlatency_Hsmcamera.e_OMXCAM_StartStreaming.t1 = cam->p_cam_sm->mTime_e_OMXCAM_StartStreaming_t1;
            cam->mlatency_Hsmcamera.e_OMXCAM_StopISP.t0 = cam->p_cam_sm->mTime_e_OMXCAM_StopISP_t0;
            cam->mlatency_Hsmcamera.e_OMXCAM_StopISP.t1 = cam->p_cam_sm->mTime_e_OMXCAM_StopISP_t1;
            memcpy(p, &(cam->mlatency_Hsmcamera), sizeof(IFM_LATENCY_HSMCAMERA));
            break;
        }
        case OMX_STE_IndexConfigFrameSelectRatio:
        {
            if (CAMERA_PORT_OUT1 != ((OMX_STE_CONFIG_FRAMESELECTRATIOTYPE*)p)->nPortIndex) {
                return OMX_ErrorBadParameter;
            }
            *(OMX_STE_CONFIG_FRAMESELECTRATIOTYPE*)p = mVariableFramerateKeepRatio;
            break;
        }
		
        case OMX_Symbian_IndexConfigAnalyzerFeedback:
        {
            OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pILFeedback = (OMX_SYMBIAN_CONFIG_FEEDBACKTYPE*)p;
            const OMX_SYMBIAN_CONFIG_FEEDBACKTYPE* pCamFeedback = NULL;
            OMX_U32 nCamFeedbackValueSize = 0;

            /*
             *  From OMX_SYMBIAN_CONFIG_FEEDBACKTYPE description:
             *  nFeedbackValueSize: The size of provided feedback data (in bytes).
             *  In case client has set this to zero when calling GetConfig component updates to the number of bytes
             *  needed for nFeedbackValue and no other fields are updated by component. In case client has set this
             *  to non-zero it means the number of bytes client has allocated for nFeedbackValue field.
             *  In case not enough space has been allocated for nFeedbackValue component may return an error.
             */

            switch( pILFeedback->nFeedbackType )
            {
                case OMX_SYMBIAN_FeedbackFaceTracker:
                    pCamFeedback = &mAnalyzersFeedbacks.sFaceTracker.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
                    break;
                case OMX_SYMBIAN_FeedbackObjectTracker:
                    pCamFeedback = &mAnalyzersFeedbacks.sObjectTracker.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_ROITYPE);
                    break;
                case OMX_SYMBIAN_FeedbackMotionLevel:
                    pCamFeedback = &mAnalyzersFeedbacks.sMotionLevel.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_U32);
                    break;
                case OMX_SYMBIAN_FeedbackAutomaticSceneDetection:
                    pCamFeedback = &mAnalyzersFeedbacks.sSceneDetection.sFeedback;
                    nCamFeedbackValueSize = sizeof(OMX_SYMBIAN_SCENEMODETYPE);
                    break;
                default:
                    // Search if opaque feedback was already received
                    for(int i=0; i<mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived; i++) {
                       if( mAnalyzersFeedbacks.sOpaque[i].sFeedback.nFeedbackType == pILFeedback->nFeedbackType) {
                           // Opaque feedback was already received: reuse this slot
                           pCamFeedback = &mAnalyzersFeedbacks.sOpaque[i].sFeedback;
                           nCamFeedbackValueSize = IFM_CONFIG_OPAQUE_FEEDBACK_SIZE;
                           break;
                       }
                    }
                    // If opaque feedback was not yet received, then use any empty slot
                    if( pCamFeedback == NULL ) {
                        if( mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived < IFM_MAX_NUMBER_OF_OPAQUE_FEEDBACKS ) {
                            // An empty slot is available
                            pCamFeedback = &mAnalyzersFeedbacks.sOpaque[mAnalyzersFeedbacks.nNumberOfOpaqueFeedbacksReceived].sFeedback;
                            nCamFeedbackValueSize = IFM_CONFIG_OPAQUE_FEEDBACK_SIZE;
                        }
                        else {
                            // No empty slot
                            return OMX_ErrorInsufficientResources;
                        }
                    }
                    break;
            }

            // If IL clients value size is zero, then return our camera storage size
            if( pILFeedback->nFeedbackValueSize == 0 )
            {
                pILFeedback->nFeedbackValueSize = nCamFeedbackValueSize;
            }
            // Check if IL client's feedback is correctly sized
            else if ( pILFeedback->nFeedbackValueSize < nCamFeedbackValueSize )
            {
                // IL client value size is too small
                return OMX_ErrorBadParameter;
            }
            // Save IL client's feeback
            else
            {
                OMX_U32 nILSize = pILFeedback->nSize;
                OMX_U32 nILFeedbackValueSize = pILFeedback->nFeedbackValueSize;
                *pILFeedback = *pCamFeedback;
                memcpy(pILFeedback->nFeedbackValue, pCamFeedback->nFeedbackValue, nCamFeedbackValueSize);
                pILFeedback->nSize = nILSize;
                pILFeedback->nFeedbackValueSize = nILFeedbackValueSize;
            }
            break;
        }
	 case OMX_STE_IndexConfigCommonZoomMode:
	 	(*(OMX_CONFIG_BOOLEANTYPE *)p) = mZoomMode;
//		MSG1("COmxCamera::getConfig. [smooth zoom] Smooth Zoom = %s\n", (mZoomMode.bEnabled == OMX_TRUE)?"ENABLED":"DISABLED");
	     break;
	case OMX_STE_IndexConfigCommonLensParameters:
    {
        Camera* cam = (Camera*)(&getProcessingComponent());
        (*(OMX_STE_CONFIG_LENSPARAMETERTYPE *)p) = cam->mLensParameters;
        break;
    }
	case OMX_STE_IndexConfigPictureSize:
    {
        *((OMX_STE_CONFIG_PICTURESIZETYPE *) p) = mPictureSize;              
	    break;
    }
#ifdef FREQ_SCALING_ALLOWED
    case OMX_STE_IndexConfigCommonScaling:
        (*(IFM_CONFIG_SCALING_MODETYPE *)p)= mfreqscaling;
        break;
#endif //FREQ_SCALING_ALLOWED
	case IFM_IndexConfigSoftware:
		(*(IFM_CONFIG_SOFTWAREVERSION *)p) = mSoftwareVersion;									
		break;
    case OMX_STE_IndexConfigMeteringOn:
	 	(*(OMX_CONFIG_BOOLEANTYPE *)p) = mMeteringOn;
	     break;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
          case OMX_3A_IndexConfigIspParameters:
              memcpy(p, &mIspParameters, sizeof(OMX_3A_CONFIG_ISPPARAMETERSTYPE));
              break;
          case OMX_3A_IndexConfigExposure:
              memcpy(p, &mExposure, sizeof(OMX_3A_CONFIG_EXPOSURETYPE));
              break;
          case OMX_3A_IndexConfigFocus:
              memcpy(p, &mFocus, sizeof(OMX_3A_CONFIG_FOCUSTYPE));
              break;
          case OMX_3A_IndexConfigFlashSynchro:
              memcpy(p, &mFlashSynchro, sizeof(OMX_3A_CONFIG_FLASHSYNCHROTYPE));
              break;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
    case OMX_STE_IndexConfigExifTimestamp:
		memcpy(p, &mExifTimeStamp, sizeof(OMX_TIME_CONFIG_TIMESTAMPTYPE));
		break;
        default :
            return ENS_Component::getConfig(idx,p);
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE COmxCamera::setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure)
{
	IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry COmxCamera::setParameter");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (pComponentParameterStructure == NULL)
	{
		OUTR(" ",(OMX_ErrorBadParameter));
        OstTraceFiltInst0(TRACE_FLOW, "Exit COmxCamera::setParameter OMX_ErrorBadParameter");
		return OMX_ErrorBadParameter;
	}
	MSG1("nParamIndex(OMX_INDEXTYPE) is %d\n",nParamIndex);
	OstTraceFiltInst1(TRACE_DEBUG, "nParamIndex(OMX_INDEXTYPE) is %d",nParamIndex);
	switch ((t_uint32)nParamIndex)
	{
		// OMX_IndexParamCommonSensorMode is managed in camport.cpp

		case OMX_Symbian_IndexParamDeviceMakeModel:
		{
			mDeviceMakeModel = *(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL*)pComponentParameterStructure;
			MSG2("Device Make='%s' Model='%s'\n", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
			//OstTraceFiltInst2(TRACE_DEBUG, "Device Make='%s' Model='%s'", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
			break;
		}
		case IFM_IndexParamFirmwareTrace:
		{
#ifndef TraceLogsControl_u32_BufferAddr_Byte0
			/* no hardware support in firmware */
			err = OMX_ErrorHardware;
#else
			IFM_PARAM_FIRMWARETRACETYPE *pFirmwareTrace = (IFM_PARAM_FIRMWARETRACETYPE *)pComponentParameterStructure;
			mIfmFirmwareTrace.bEnabled = pFirmwareTrace->bEnabled;
			mIfmFirmwareTrace.eTraceMechanism = pFirmwareTrace->eTraceMechanism;
			mIfmFirmwareTrace.nLogLevel = pFirmwareTrace->nLogLevel;
			err = OMX_ErrorNone;
#endif
			break;
		}
		case OMX_Symbian_IndexParamColorPrimary:
		{
			OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE* pColorPrimaryStruct = (OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*)pComponentParameterStructure;

			// Check parameter
			if(    pColorPrimaryStruct->eColorPrimary != OMX_SYMBIAN_ColorPrimaryBT601FullRange
			    && pColorPrimaryStruct->eColorPrimary != OMX_SYMBIAN_ColorPrimaryBT601LimitedRange
			    && pColorPrimaryStruct->eColorPrimary != OMX_SYMBIAN_ColorPrimaryBT709FullRange
			    && pColorPrimaryStruct->eColorPrimary != OMX_SYMBIAN_ColorPrimaryBT709LimitedRange)
			{
				MSG1("Invalid ColorPrimary setting: %d\n", pColorPrimaryStruct->eColorPrimary);
				OstTraceFiltInst1(TRACE_DEBUG, "Invalid ColorPrimary setting: %d", pColorPrimaryStruct->eColorPrimary);
				err = OMX_ErrorUnsupportedSetting;
			}
			else
			{
				// Check if operation is supported on this port
				if(    pColorPrimaryStruct->nPortIndex != CAMERA_PORT_OUT0
				    && pColorPrimaryStruct->nPortIndex != CAMERA_PORT_OUT2)
				{
					// Setting ColorPrimary on VPB0 or VPB1 not supported
					MSG1("Port VPB%lu: ColorPrimary operation not permitted on this port\n", pColorPrimaryStruct->nPortIndex);
					OstTraceFiltInst1(TRACE_DEBUG, "Port VPB%lu: ColorPrimary operation not permitted on this port", pColorPrimaryStruct->nPortIndex);
					err = OMX_ErrorBadParameter;
				}
				else
				{
					// Apply to port only if port is in state "disabled"
					camport* pPort = (camport*)getPort(pColorPrimaryStruct->nPortIndex);
					if(pPort->isEnabled() == OMX_TRUE)
					{
						MSG1("Port VPB%lu is not disabled: rejecting ColorPrimary request\n", pColorPrimaryStruct->nPortIndex);
						OstTraceFiltInst1(TRACE_DEBUG, "Port VPB%lu is not disabled: rejecting ColorPrimary request", pColorPrimaryStruct->nPortIndex);
						err = OMX_ErrorIncorrectStateOperation;
					}
					else
					{
						// Store port ColorPrimary setting
						if(pColorPrimaryStruct->nPortIndex == CAMERA_PORT_OUT0)
						{
							mColorPrimary_VPB0 = *pColorPrimaryStruct;
						}
						else
						{
							mColorPrimary_VPB2 = *pColorPrimaryStruct;
						}
						err = OMX_ErrorNone;
					}
				}
			}
			break;
		}
		default:
			err = ENS_Component::setParameter(nParamIndex,pComponentParameterStructure);
			break;
	}
	OUTR(" ",(err));
    OstTraceFiltInst1(TRACE_FLOW, "Exit COmxCamera::setParameter (%d)", (err));
	return err;
}



OMX_BOOL COmxCamera::isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const
{
    switch ((t_uint32)nParamIndex)
    {
        case OMX_IndexParamCommonSensorMode:
            MSG0("isPortSpecificParameter TRUE\n") ;
            OstTraceFiltInst0(TRACE_DEBUG, "isPortSpecificParameter TRUE") ;
            return OMX_TRUE;
	case OMX_STE_IndexParamFrameRateRange:
            MSG0("isPortSpecificParameter TRUE\n") ;
            OstTraceFiltInst0(TRACE_DEBUG, "isPortSpecificParameter TRUE") ;
            return OMX_TRUE;
			
        default :
            return(ENS_Component::isPortSpecificParameter(nParamIndex));
    }

}



OMX_ERRORTYPE COmxCamera::getParameter(OMX_INDEXTYPE idx, OMX_PTR p) const {
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry COmxCamera::getParameter");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    if (p == NULL)
    {
        OUTR(" ",(OMX_ErrorBadParameter));
        OstTraceFiltInst0(TRACE_FLOW, "Exit COmxCamera::getParameter OMX_ErrorBadParameter");
        return OMX_ErrorBadParameter;
    }

	switch ((t_uint32)idx)
	{
		// Mandatory Parameters (1.1.1 @ 307)
		case OMX_IndexParamVideoInit :
			{
				OMX_PORT_PARAM_TYPE * portTypesParam = (OMX_PORT_PARAM_TYPE *) p;
				portTypesParam->nSize = sizeof(OMX_PORT_PARAM_TYPE);
				portTypesParam->nVersion = getVersion();
				portTypesParam->nPorts = CAMERA_NB_OUT_PORTS;
				portTypesParam->nStartPortNumber = 0;
			}
			break;
		case IFM_IndexParamFirmwareTrace :
		{
	        IFM_PARAM_FIRMWARETRACETYPE *pFirmwareTrace = (IFM_PARAM_FIRMWARETRACETYPE *)p;
	        pFirmwareTrace->bEnabled = mIfmFirmwareTrace.bEnabled;
			pFirmwareTrace->eTraceMechanism = mIfmFirmwareTrace.eTraceMechanism;
			pFirmwareTrace->nLogLevel = mIfmFirmwareTrace.nLogLevel;
			break;
		}
		case OMX_Symbian_IndexParamDeviceMakeModel:
		{
			memcpy(p, &mDeviceMakeModel, sizeof(OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL));
			MSG2("Device Make='%s' Model='%s'\n", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
			//OstTraceFiltInst2(TRACE_DEBUG, "Device Make='%s' Model='%s'", mDeviceMakeModel.cMake, mDeviceMakeModel.cModel);
			break;
		}
		case OMX_Symbian_IndexParamColorPrimary:
		{
			OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE* pColorPrimaryStruct = (OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*)p;
			if(    pColorPrimaryStruct->nPortIndex != CAMERA_PORT_OUT0
			    && pColorPrimaryStruct->nPortIndex != CAMERA_PORT_OUT2)
			{
				// Setting ColorPrimary on VPB0 or VPB1 not supported
				MSG1("Port VPB%lu: ColorPrimary operation not permitted on this port\n", pColorPrimaryStruct->nPortIndex);
				OstTraceFiltInst1(TRACE_DEBUG, "Port VPB%lu: ColorPrimary operation not permitted on this port", pColorPrimaryStruct->nPortIndex);
				err = OMX_ErrorBadParameter;
			}
			else
			{
				if(pColorPrimaryStruct->nPortIndex == CAMERA_PORT_OUT0)
				{
					*pColorPrimaryStruct = mColorPrimary_VPB0;
				}
				else
				{
					*pColorPrimaryStruct = mColorPrimary_VPB2;
				}
			}
			break;
		}
        case OMX_IndexParamOtherPrivateContext:
            LOGE("Getting OMX_IndexParamOtherPrivateContext");
            err =((Camera*)&getProcessingComponent())->printPrivateContext((OMX_OTHER_PARAM_PRIVATE_CONTEXT *)p);
            break;
		default :
			err = ENS_Component::getParameter(idx,p);
			break;
	}
    OUTR(" ",(err));
    OstTraceFiltInst1(TRACE_FLOW, "Exit COmxCamera::getParameter (%d)", (err));
    return err;
}

OMX_ERRORTYPE COmxCamera::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
{
    IN0("\n");
    OstTraceFiltInst0(TRACE_FLOW, "Entry COmxCamera::getExtensionIndex");
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
	else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SUPPORTEDRESOLUTIONS_NAME, 128))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSupportedResolutions;
	}
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLICKERREMOVAL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlickerRemoval;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_PARAM_COMMON_DEVICEMAKEMODEL_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexParamDeviceMakeModel;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SENSORNVMDATA_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSensorNvmData;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_PARAM_COMMON_COLORPRIMARY_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexParamColorPrimary;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_FRAMESELECTRATIO_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigFrameSelectRatio;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_PARAM_FRAMERATERANGE_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexParamFrameRateRange;
    }
    else if (0 == strncmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ANALYZERFEEDBACK_NAME, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAnalyzerFeedback;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_COMMON_ZOOMMODE_NAME, 128))
    {
		*pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigCommonZoomMode;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_COMMON_LENSPARAMETER, 128))
    {
		*pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigCommonLensParameters;
    }
#ifdef FREQ_SCALING_ALLOWED
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_COMMON_SCALING, 128))
    {
        *pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigCommonScaling;
    }
#endif //FREQ_SCALING_ALLOWED
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_METERINGON_NAME, 128))
    {
		*pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigMeteringOn;
    }
    else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_EXIF_TIMESTAMP_NAME, 128))
    {
		*pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigExifTimestamp;
    }
	else if (0 == strncmp(cParameterName,OMX_STE_INDEX_CONFIG_PICTURE_SIZE, 128))
    {
		*pIndexType = (OMX_INDEXTYPE)OMX_STE_IndexConfigPictureSize;
    }
    else
    {
        err = ENS_Component::getExtensionIndex(cParameterName, pIndexType);
    }

    OUTR(" ",(err));
    OstTraceFiltInst1(TRACE_FLOW, "Exit COmxCamera::getExtensionIndex (%d)", (err));
    return err;
}


OMX_ERRORTYPE COmxCamera::createCamPort(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref)
{
    ENS_Port * port;
    EnsCommonPortData commonPortData(nPortIndex,eDir,1,0,OMX_PortDomainVideo,eSupplierPref);
    port = new camport(commonPortData, *this);
    if (port == NULL) return OMX_ErrorInsufficientResources;
    addPort(port);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE COmxCamera::suspendResources(void)
{
    Camera * cam =  (Camera*)&getProcessingComponent();
    return cam->suspendResources();
}

OMX_ERRORTYPE COmxCamera::unsuspendResources(void)
{
    Camera * cam =  (Camera*)&getProcessingComponent();
    return cam->unsuspendResources();
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
     At this point we are dead
    */
    eventHandler(OMX_EventError,(t_uint32)OMX_ErrorHardware,0);
    break;
  case NMF_SERVICE_SHUTDOWN:
    if (serviceData->shutdown.coreid != ((t_nmf_core_id)SIA_CORE_ID)) return;
    /*
       implement something here if you want to rebuild the DSP network once the DSP has waken up again
       beware that this event is also generated when the network is detroyed in the nominal case (go to loaded)
    */
    break;
  default:
    break;
  }
}
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
OMX_ERRORTYPE COmxCamera::createStatsPort(OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref)
{
  ENS_Port * port;
  if (nPortIndex == CAMERA_PORT_STATS_AEWB)
  {
    port = new camportstats(nPortIndex, eDir, eSupplierPref, OMX_3A_OTHER_FormatStatsAewb, *this);
  }
  else if (nPortIndex == CAMERA_PORT_STATS_AF)
  {
    port = new camportstats(nPortIndex, eDir, eSupplierPref, OMX_3A_OTHER_FormatStatsAf, *this);
  }
  else
  {
    port = NULL;
  }
  if (port == NULL) return OMX_ErrorInsufficientResources;
  addPort(port);
  return OMX_ErrorNone;
}
#endif // CAMERA_ENABLE_OMX_STATS_PORT
