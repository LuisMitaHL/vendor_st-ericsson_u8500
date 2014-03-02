/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef OMXCAMERA_H_
#define OMXCAMERA_H_

#include "OMX_Core.h"
#include "OMX_CoreExt.h"
#include "ENS_Component.h"

#include "osi_trace.h"
#include "OMX_debug.h"
#include "IFM_Types.h"
#include "camport.h"
#include "sensor.h"
#include "selftest.h"
#include "rme_types.h"
#include "VhcElementDefs.h"

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#include "OMX_3A_CameraIndexExt.h"
#include "OMX_3A_CameraExt.h"
#define CAMERA_PORT_STATS_AEWB  (CAMERA_NB_PORTS + 0)
#define CAMERA_PORT_STATS_AF    (CAMERA_NB_PORTS + 1)
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

#ifdef CAMERA_ENABLE_OMX_STATS_PORT
#include "camportstats.h"
#define CAMERA_NB_STATS_PORTS 2
#endif // CAMERA_ENABLE_OMX_STATS_PORT
OMX_ERRORTYPE OMXCameraFactoryMethod(ENS_Component_p * ppENSComponent);

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(COmxCamera_RDB);
#endif

/* typedef for function pointer passed in OMX_OTHER_PARAM_PRIVATE_CONTEXT
 * for logging component's private OMX data.
 */
typedef void (*t_ContextDebugPrint)(OMX_PTR, const char*, ...);

class COmxCamera_RDB: public ENS_ResourcesDB {
    public:
        COmxCamera_RDB();
};



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(COmxCamera);
#endif
class COmxCamera: public ENS_Component {
    friend class Camera;
public :
    COmxCamera(enumCameraSlot cam=ePrimaryCamera);
    ~COmxCamera();

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

    virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;
    virtual OMX_ERRORTYPE createResourcesDB();
    virtual void NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData);
    RM_STATUS_E getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData);
    OMX_ERRORTYPE applyOMXConfig();
    virtual OMX_ERRORTYPE suspendResources(void);
    virtual OMX_ERRORTYPE unsuspendResources(void);

    // OMX Config structs local copies to enable configs in loaded state
    OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE mZoomFactorCmdType;      // OMX_SYMBIAN_IndexConfigCommonExtDigitalZoom
    OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE mZoomFactorStatusType;   // OMX_SYMBIAN_IndexConfigCommonExtDigitalZoom
    OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE mOpticalZoomFactor;      // OMX_Symbian_IndexConfigExtOpticalZoom
    OMX_SYMBIAN_CONFIG_POINTTYPE mCenterFieldOfViewCmdType;    // OMX_IndexConfigCenterFieldOfView
    OMX_SYMBIAN_CONFIG_POINTTYPE mCenterFieldOfViewStatusType; // OMX_IndexConfigCenterFieldOfView
    OMX_PARAM_SENSORMODETYPE mSensorMode;                      // OMX_IndexParamCommonSensorMode
    OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE mSceneOrientation;      // OMX_Symbian_IndexConfigOrientationScene
    OMX_CONFIG_WHITEBALCONTROLTYPE mWhiteBalControl;           // OMX_IndexConfigCommonWhiteBalance
    OMX_SYMBIAN_CONFIG_SCENEMODETYPE mSceneMode;               // OMX_Symbian_IndexConfigSceneMode
    OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE mFlashControlStruct;   // OMX_Symbian_IndexConfigFlashControl
    OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE mVideoLight;       // OMX_Symbian_IndexConfigVideoLight 
    OMX_CONFIG_EXPOSUREVALUETYPE mExposureValue;               // OMX_IndexConfigCommonExposureValue
    OMX_CONFIG_BRIGHTNESSTYPE mBrightness;                     // OMX_IndexConfigCommonBrightness
    OMX_CONFIG_CONTRASTTYPE mContrast;                         // OMX_IndexConfigCommonContrast
    OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL mDeviceMakeModel;        // OMX_Symbian_IndexParamDeviceMakeModel
    OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE mCameraUserSettings;
    OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE mColorPrimary_VPB0;     // OMX_Symbian_IndexParamColorPrimary
    OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE mColorPrimary_VPB2;     // OMX_Symbian_IndexParamColorPrimary
    OMX_CONFIG_CALLBACKREQUESTTYPE mExposureInitiatedClbkReq;
    OMX_SYMBIAN_CONFIG_BOOLEANTYPE mCapturingStruct_VPB1; // OMX_Symbian_IndexConfigCommonExtCapturing
    OMX_SYMBIAN_CONFIG_BOOLEANTYPE mCapturingStruct_VPB2; // OMX_Symbian_IndexConfigCommonExtCapturing
    OMX_CONFIG_IMAGEFILTERTYPE mImageFilterType;          // OMX_IndexConfigCommonImageFilter
    IFM_CONFIG_SOFTWAREVERSION mSoftwareVersion;	  //IFM_IndexConfigSoftware
    OMX_CONFIG_BOOLEANTYPE  mMeteringOn; // OMX_STE_IndexConfigMeteringOn
    OMX_STE_CONFIG_PICTURESIZETYPE mPictureSize;
    OMX_SYMBIAN_SCENEMODETYPE previousScenetype;

    union {
        OMX_SYMBIAN_CONFIG_BRACKETINGTYPE config;
        char rawdata[IFM_CONFIG_BRACKETING_SIZE(IFM_MAX_NUMBER_OF_BRACKETING_VALUES)];
    } mBracketing;                                             // OMX_Symbian_IndexConfigBracketing
    OMX_CONFIG_EXPOSURECONTROLTYPE mExposureControl;           // OMX_IndexConfigCommonExposure
    union {
        OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE config;
        char rawdata[IFM_CONFIG_EXTFOCUSSTATUS_SIZE(OMX_SYMBIAN_MAX_NUMBER_OF_ROIS)];
    } mExtFocusStatus;                                         // OMX_Symbian_IndexConfigExtFocusStatus
    struct {
        bool bFaceTrackerFeedbackReceived;
        IFM_CONFIG_FACETRACKER_FEEDBACKTYPE sFaceTracker;
        bool bObjectTrackerFeedbackReceived;
        IFM_CONFIG_OBJECTTRACKER_FEEDBACKTYPE sObjectTracker;
        bool bMotionLevelFeedbackReceived;
        IFM_CONFIG_MOTIONLEVEL_FEEDBACKTYPE sMotionLevel;
        bool bSceneDetectionFeedbackReceived;
        IFM_CONFIG_SCENEDETECTION_FEEDBACKTYPE sSceneDetection;
        int nNumberOfOpaqueFeedbacksReceived;
        bool bOpaqueFeedbackReceived[IFM_MAX_NUMBER_OF_OPAQUE_FEEDBACKS];
        IFM_CONFIG_OPAQUE_FEEDBACKTYPE sOpaque[IFM_MAX_NUMBER_OF_OPAQUE_FEEDBACKS];
    } mAnalyzersFeedbacks;                                     // OMX_Symbian_IndexConfigAnalyzerFeedback
    OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE mSelfTestType;
    OMX_CONFIG_BOOLEANTYPE mTestingType;
    OMX_STE_CONFIG_FRAMESELECTRATIOTYPE mVariableFramerateKeepRatio;
    OMX_STE_PARAM_FRAMERATERANGE mFrameRateRange; //OMX_STE_IndexParamFrameRateRange
    OMX_CONFIG_BOOLEANTYPE mZoomMode;

    //Variable required for ZSLHDR
    eZSLHDR_State eStateZSLHDR;
    OMX_U32 nFrameBeforeBackUp;
    OMX_BOOL bFrameLimitedBackUp;
    OMX_U32 nFrameLimitBackUp;

#ifdef FREQ_SCALING_ALLOWED
    IFM_CONFIG_SCALING_MODETYPE mfreqscaling;
#endif //FREQ_SCALING_ALLOWED
    CLoadedSelfTest iLoadedSelfTest;
    OMX_S32 nRotation;
    OMX_CONFIG_FRAMESTABTYPE& getConfigFrameStabType(void) {return mStabType;};
    OMX_CONFIG_CALLBACKREQUESTTYPE mZoomModeClbkReq; //OMX_STE_IndexConfigCommonZoomMode
    
	struct
	{
		t_uint32 sensor_X;
		t_uint32 sensor_Y;
		t_uint32 totalBuffers;
		t_uint32 *physicalBuffersAddress;		
		MMHwBuffer * hiddenBMSBuffers;
	}mHiddenBuffersInfo;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
      OMX_3A_CONFIG_ISPPARAMETERSTYPE    mIspParameters;      // OMX_3A_IndexConfigIspParameters
      OMX_3A_CONFIG_EXPOSURETYPE         mExposure;           // OMX_3A_IndexConfigExposure
      OMX_3A_CONFIG_GAMMATABLE_TYPE      mGamma;              // OMX_3A_IndexConfigGamma
      OMX_3A_CONFIG_LSCTABLE_TYPE        mLsc;                // OMX_3A_IndexConfigLsc
      OMX_3A_CONFIG_LINEARIZERTABLE_TYPE mLinearizer;         // OMX_3A_IndexConfigLsc
      OMX_3A_CONFIG_FOCUSTYPE            mFocus;              // OMX_3A_IndexConfigFocus
      OMX_3A_CONFIG_FLASHSYNCHROTYPE     mFlashSynchro;       // OMX_3A_IndexConfigFlashSynchro
      OMX_3A_CONFIG_MAXFRAMERATETYPE     mMaxFramerate;       // OMX_3A_IndexConfigMaxFramerate
      OMX_3A_PARAM_SENSORMODETYPE        mSensorModeparam;    // OMX_3A_IndexParamSensorMode
      OMX_3A_CONFIG_EEPROMDATATYPE       mEepromdata;         // OMX_3A_CONFIG_EEPROMDATATYPE
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
    OMX_TIME_CONFIG_TIMESTAMPTYPE mExifTimeStamp;    //OMX_STE_IndexConfigExifTimestamp
protected :
    // OMX Config structs local copies to enable configs in loaded state
    OMX_CONFIG_BOOLEANTYPE mAutoPauseAfterCapture;        // OMX_IndexAutoPauseAfterCapture
    OMX_CONFIG_CAPTUREMODETYPE mCaptureModeType;          // OMX_IndexConfigCaptureMode
    OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE mExtCaptureModeType; // OMX_Symbian_IndexConfigExtCaptureMode
    OMX_CONFIG_COLORBLENDTYPE mColorBlendType;            // OMX_IndexConfigCommonColorBlend
    OMX_STE_CONFIG_SENSORTESTTYPE mTestModeType;          // IFM_IndexConfigTestMode
    OMX_SYMBIAN_CONFIG_S32TYPE mSharpness;                // OMX_Symbian_IndexConfigSharpness
    OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE mFocusRange;        // OMX_Symbian_IndexConfigFocusRange
    OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE mFocusRegion;           // OMX_Symbian_IndexConfigFocusRegion
    OMX_SYMBIAN_CONFIG_RGBHISTOGRAM mRGBHistogram;             // OMX_Symbian_IndexConfigRGBHistogram
    OMX_SYMBIAN_CONFIG_HISTOGRAMCONTROLTYPE mHistogramControl; // OMX_Symbian_IndexConfigHistogramControl
    OMX_CONFIG_SATURATIONTYPE mSaturation;                // OMX_IndexConfigCommonSaturation
    OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE mFocusControl;      // OMX_IndexConfigFocusControl
    OMX_CONFIG_FRAMESTABTYPE mStabType;                   // OMX_IndexConfigCommonFrameStabilization

    OMX_SYMBIAN_CONFIG_U32TYPE mMotionLevel;                    // OMX_SYMBIAN_IndexConfigMotionLevel
    OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE mNDFilterControl;   // OMX_SYMBIAN_IndexConfigNDFilterControl
    OMX_SYMBIAN_CONFIG_LOCKTYPE mExposureLock;       // OMX_Symbian_IndexConfigExposureLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mWhiteBalanceLock;   // OMX_Symbian_IndexConfigWhiteBalanceLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mFocusLock;          // OMX_Symbian_IndexConfigFocusLock
    OMX_SYMBIAN_CONFIG_LOCKTYPE mAllLock;            // OMX_Symbian_IndexConfigAllLock
    OMX_SYMBIAN_CONFIG_ROITYPE mROI;                      // OMX_SYMBIAN_IndexConfigROI
    OMX_SYMBIAN_CONFIG_RAWPRESETTYPE mRAWPreset;          // OMX_SYMBIAN_IndexConfigRAWPreset
    OMX_CONFIG_MIRRORTYPE mMirror;                        // OMX_IndexConfigCommonMirror
    OMX_CONFIG_ROTATIONTYPE mRotation;                    // OMX_IndexConfigCommonRotate
    OMX_CONFIG_BOOLEANTYPE mRevertAspectRatio;            // OMX_Symbian_IndexParamRevertAspectRatio
    OMX_SYMBIAN_CONFIG_HWINFOTYPE mSensorInfo;            // OMX_Symbian_IndexConfigCameraSensorInfo
    OMX_SYMBIAN_CONFIG_HWINFOTYPE mFlashgunInfo;          // OMX_Symbian_IndexConfigFlashGunInfo
    OMX_OTHER_PARAM_PRIVATE_CONTEXT mPrivateContext;      // OMX_IndexParamOtherPrivateContext
    struct {                                              // OMX_IndexConfigCallbackRequest
            OMX_CONFIG_CALLBACKREQUESTTYPE ExposureLock;     // - OMX_Symbian_IndexConfigExposureLock
            OMX_CONFIG_CALLBACKREQUESTTYPE WhiteBalanceLock; // - OMX_Symbian_IndexConfigWhiteBalanceLock
            OMX_CONFIG_CALLBACKREQUESTTYPE FocusLock;        // - OMX_Symbian_IndexConfigFocusLock
            OMX_CONFIG_CALLBACKREQUESTTYPE AllLock;          // - OMX_Symbian_IndexConfigAllLock
            OMX_CONFIG_CALLBACKREQUESTTYPE ExtFocusStatus;   // - OMX_Symbian_IndexConfigExtFocusStatus
            OMX_CONFIG_CALLBACKREQUESTTYPE CameraTesting;    // - OMX_Symbian_IndexConfigCameraTesting
            OMX_CONFIG_CALLBACKREQUESTTYPE PreCaptureExposureTime; //OMX_Symbian_IndexConfigPreCaptureExposureTime
            //OMX_CONFIG_CALLBACKREQUESTTYPE ExposureValue     // - OMX_IndexConfigCommonExposureValue
            OMX_CONFIG_CALLBACKREQUESTTYPE MeteringPrefashCallback;
    } mCallbackRequests;                                  // OMX_IndexConfigCallbackRequest

    // Specific indexes
    IFM_CONFIG_READPETYPE mIfmReadPEType;                 // IFM_IndexConfigCommonReadPe
    IFM_CONFIG_WRITEPETYPE mIfmWritePEType;               // IFM_IndexConfigCommonWritePe
    IFM_CONFIG_OPMODE_CHECK mIfmOpMode;                   // IFM_IndexConfigCommonOpMode_Check
    IFM_CONFIG_3A_TRACEDUMPCONTROL mIfm3ATraceDumpControl;// IFM_IndexConfig3ATraceDumpControl
    IFM_CONFIG_CHECK_ALIVE mIfmCheckAlive;                // IFM_IndexConfigCheckAlive

    OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE mFlickerRemoval;     // OMX_Symbian_IndexConfigFlickerRemoval
    OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE mFlashAFAssist;    // OMX_Symbian_IndexConfigAFAssistantLight
    IFM_PARAM_FIRMWARETRACETYPE mIfmFirmwareTrace;
    OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS mSupportedResolutions; // OMX_Symbian_IndexConfigSupportedResolutions
private :
    OMX_ERRORTYPE createCamPort(OMX_U32 nPortIndex,
            OMX_DIRTYPE eDir,
            OMX_BUFFERSUPPLIERTYPE eSupplierPref);

    void defaultConfig(unsigned int nIndex);

#ifdef CAMERA_ENABLE_OMX_STATS_PORT
    OMX_ERRORTYPE createStatsPort(OMX_U32 nPortIndex,
            OMX_DIRTYPE eDir,
            OMX_BUFFERSUPPLIERTYPE eSupplierPref);
#endif // CAMERA_ENABLE_OMX_STATS_PORT
};

#endif /* OMXCAMERA_H_ */
