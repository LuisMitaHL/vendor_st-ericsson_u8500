/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _EXT_OMXCAMERA_H_
#define _EXT_OMXCAMERA_H_

#include "OMX_Core.h"
#include "OMX_CoreExt.h"
#include "ENS_Component.h"

#include "IFM_Types.h"
#include "ext_camport.h"

class COmxCamera_RDB: public ENS_ResourcesDB {
public:
    COmxCamera_RDB();
};


class COmxCamera: public ENS_Component {
    friend class Camera;
public :
    COmxCamera();

    virtual OMX_ERRORTYPE getParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure) const;

    virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);

    virtual OMX_ERRORTYPE setConfig(
            OMX_INDEXTYPE nIndex,
            OMX_PTR pStructure);

    virtual OMX_ERRORTYPE getConfig(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure) const;

    virtual OMX_ERRORTYPE getExtensionIndex(
            OMX_STRING cParameterName,
            OMX_INDEXTYPE* pIndexType) const;

    virtual OMX_BOOL isPortSpecificParameter(
            OMX_INDEXTYPE nParamIndex) const;

    virtual OMX_ERRORTYPE createResourcesDB();

    RM_STATUS_E getResourcesEstimation(
            OMX_IN const OMX_PTR pCompHdl,
            OMX_INOUT RM_EMDATA_T* pEstimationData);

    virtual void NmfPanicCallback(
            void *contextHandler,
            t_nmf_service_type serviceType,
            t_nmf_service_data *serviceData);

    OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE mZoomFactorCmdType;      // OMX_SYMBIAN_IndexConfigCommonExtDigitalZoom
    OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE mZoomFactorStatusType;   // OMX_SYMBIAN_IndexConfigCommonExtDigitalZoom
    OMX_SYMBIAN_CONFIG_POINTTYPE mCenterFieldOfViewCmdType;    // OMX_IndexConfigCenterFieldOfView
    OMX_SYMBIAN_CONFIG_POINTTYPE mCenterFieldOfViewStatusType; // OMX_IndexConfigCenterFieldOfView
    OMX_PARAM_SENSORMODETYPE mSensorMode;                       // OMX_IndexParamCommonSensorMode
    OMX_CONFIG_BRIGHTNESSTYPE mBrightnessType;                 // OMX_IndexConfigCommonBrightness
    OMX_CONFIG_CONTRASTTYPE mContrastType;                     // OMX_IndexConfigCommonContrast
    OMX_CONFIG_WHITEBALCONTROLTYPE mWhiteBalControlType;       // OMX_IndexConfigCommonWhiteBalance
    OMX_SYMBIAN_CONFIG_SCENEMODETYPE mSceneMode;               // OMX_Symbian_IndexConfigSceneMode
    OMX_CONFIG_EXPOSUREVALUETYPE mExposureValueType;           // OMX_IndexConfigCommonExposureValue
    OMX_CONFIG_EXPOSURECONTROLTYPE mExposureControlType;       // OMX_IndexConfigCommonExposure
    OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE mFlashControlStruct;   // OMX_SymbianDraft_IndexConfigFlashControl
    OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE mExtFocusStatusType; // OMX_Symbian_IndexConfigExtFocusStatus
    OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL mDeviceMakeModel;        // OMX_Symbian_IndexParamDeviceMakeModel
    OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE mSceneOrientation;      // OMX_Symbian_IndexConfigOrientationScene
    OMX_SYMBIAN_PARAM_SOFTWARE mSoftware;                      // OMX_Symbian_IndexParamSoftware
    OMX_STE_CONFIG_LENSPARAMETERTYPE mLensParamType;           // OMX_STE_IndexConfigCommonLensParameters
    OMX_SYMBIAN_SUPPORTED_ZOOM_RESOLUTIOINS mZoomSupportedRes; //OMX_Symbian_IndexConfigSupportedZoomResolutions
    OMX_OTHER_PARAM_PRIVATE_CONTEXT mPrivateContext;           // OMX_IndexParamOtherPrivateContext

protected :
    // OMX Config structs local copies to enable configs in loaded state
    OMX_SYMBIAN_CONFIG_BOOLEANTYPE mCapturingStruct;           // OMX_Symbian_IndexConfigCommonExtCapturing
    OMX_CONFIG_BOOLEANTYPE mAutoPauseAfterCapture;             // OMX_IndexAutoPauseAfterCapture
    OMX_CONFIG_CAPTUREMODETYPE mCaptureModeType;               // OMX_IndexConfigCaptureMode
    OMX_CONFIG_COLORBLENDTYPE mColorBlendType;                 // OMX_IndexConfigCommonColorBlend
    OMX_STE_CONFIG_SENSORTESTTYPE mTestModeType;               // IFM_IndexConfigTestMode
    OMX_CONFIG_LIGHTNESSTYPE mLightnessType;                   // OMX_IndexConfigCommonLightness
    OMX_CONFIG_SATURATIONTYPE mSaturationType;                 // OMX_IndexConfigCommonSaturation
    OMX_CONFIG_GAMMATYPE mGammaType;                           // OMX_IndexConfigCommonGamma
    OMX_CONFIG_IMAGEFILTERTYPE mImageFilterType;               // OMX_IndexConfigCommonImageFilter
    OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE mFocusRangeType;         // OMX_Symbian_IndexConfigFocusRange
    OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE mFocusControlType;       // OMX_IndexConfigFocusControl
    OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE mFocusRegionType;       // OMX_IndexConfigCommonFocusRegion
    OMX_CONFIG_FRAMESTABTYPE mStabType;                        // OMX_IndexConfigCommonFrameStabilization
    OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE mFlickerRemoval;     // OMX_Symbian_IndexConfigFlickerRemoval
    OMX_SYMBIAN_CONFIG_U32TYPE mMotionLevel;                   // OMX_SYMBIAN_IndexConfigMotionLevel
    OMX_SYMBIAN_CONFIG_LOCKTYPE mExposureLock;                 // OMX_SYMBIAN_IndexConfigExposureLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mWhiteBalanceLock;             // OMX_SYMBIAN_IndexConfigWhiteBalanceLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mFocusLock;                    // OMX_SYMBIAN_IndexConfigFocusLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mAllLock;                      // OMX_SYMBIAN_IndexConfigAllLock
    OMX_CONFIG_CALLBACKREQUESTTYPE mExposureLockClbkReq;       // OMX_IndexConfigCallbackRequest
    OMX_CONFIG_CALLBACKREQUESTTYPE mWhiteBalanceLockClbkReq;   // OMX_IndexConfigCallbackRequest
    OMX_CONFIG_CALLBACKREQUESTTYPE mFocusLockClbkReq;          // OMX_IndexConfigCallbackRequest
    OMX_CONFIG_CALLBACKREQUESTTYPE mAllLockClbkReq;            // OMX_IndexConfigCallbackRequest
    OMX_SYMBIAN_CONFIG_ROITYPE mROI;                           // OMX_SYMBIAN_IndexConfigROI
    OMX_SYMBIAN_CONFIG_RAWPRESETTYPE mRAWPreset;               // OMX_SYMBIAN_IndexConfigRAWPreset
    OMX_CONFIG_MIRRORTYPE mMirror;                             // OMX_IndexConfigCommonMirror
    OMX_SYMBIAN_CONFIG_HWINFOTYPE mSensorInfo;                 // OMX_Symbian_IndexConfigCameraSensorInfo
    OMX_SYMBIAN_CONFIG_HWINFOTYPE mFlashgunInfo;               // OMX_Symbian_IndexConfigFlashGunInfo
    // Specific indexes
    IFM_CONFIG_READPETYPE mIfmReadPEType;                      // IFM_IndexConfigCommonReadPe
    IFM_CONFIG_WRITEPETYPE mIfmWritePEType;                    // IFM_IndexConfigCommonWritePe
    IFM_CONFIG_OPMODE_CHECK mIfmOpMode;                        // IFM_IndexConfigCommonOpMode_Check
    IFM_CONFIG_3A_TRACEDUMPCONTROL mIfm3ATraceDumpControl;     // IFM_IndexConfig3ATraceDumpControl
    OMX_SYMBIAN_CONFIG_S32TYPE mSharpness;                     // OMX_Symbian_IndexConfigSharpness

private :
    OMX_ERRORTYPE createCamPort(
            OMX_U32 nPortIndex,
            OMX_DIRTYPE eDir,
            OMX_BUFFERSUPPLIERTYPE eSupplierPref);
    OMX_ERRORTYPE checkIndexConfigFlashControl(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCaptureMode(
            OMX_PTR);
    OMX_ERRORTYPE checkIndexConfigCommonColorBlend(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigExtDigitalZoom(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCenterFieldOfView(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonWhiteBalance(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonBrightness(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonLightness(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonExposureValue(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonExposure(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonContrast(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonSaturation(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonGamma(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonImageFilter(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigFocusControl(
            OMX_PTR p);
    OMX_ERRORTYPE checkSymbianIndexConfigFocusRange(
            OMX_PTR p);
    OMX_ERRORTYPE checkSymbianIndexConfigFocusRegion(
            OMX_PTR p);
    OMX_ERRORTYPE checkSymbianIndexConfigExtFocusStatus(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonFrameStabilisation(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCallbackRequest(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigMotionLevel(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigNDFilterControl(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigExposureLock(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigWhiteBalanceLock(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigFocusLock(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigAllLock(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigROI(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigRAWPreset(
            OMX_PTR p);
    OMX_ERRORTYPE checkIndexConfigCommonMirror(
            OMX_PTR p);

    void defaultConfig(
            unsigned int nIndex);
};

#endif /* _EXT_OMXCAMERA_H_ */
