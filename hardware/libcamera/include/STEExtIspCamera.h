/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * SteExtIspCamera.h : external Isp camera part
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STEEXTISPCAMERA_H
#define STEEXTISPCAMERA_H

//System includes
//#include <camera/CameraHardwareInterface.h>
#include <binder/MemoryBase.h>
#include <utils/threads.h>

//System includes
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/android_pmem.h>
#include <linux/time.h>

//Multimedia includes
#include <unistd.h>
#include <dlfcn.h>
#include <OMX_Symbian_CameraExt_Ste.h>
#include <fcntl.h>
#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>
#include <camera/CameraParameters.h>

//Internal includes
#include "STECamOmxUtils.h"
#include "STECamPerfManager.h"
#include "STECamProp.h"
#include "STECamOmxILExtIndex.h"
#include "STECameraDeviceBase.h"
#include "STEJpegEnc.h"
#include "STEArmIvProc.h"
#include "STECamQueue.h"
#include "hwconversion/STECamHwConversion.h"
#include "STECameraKeys.h"
#ifdef ENABLE_FACE_DETECTION
#include "NevenFaceDetector.h"
#endif


#define uint32 unsigned int

#define CAM_VPB 0
#define uint32 unsigned int

namespace android {

//Forward declaration
class MyNativeWindow;
class GraphicBuffer;
#ifdef ENABLE_FACE_DETECTION
class STEFaceDetectorBase;
#endif
typedef struct buffer_info {
    sp<GraphicBuffer> mGraphicBuffer;
    OMX_BUFFERHEADERTYPE* mOmxBufferHdr;
    OMX_OSI_CONFIG_SHARED_CHUNK_METADATA mChunkData;
    int  mOmxBuffersState;
    camera_memory_t* mClientMemory;
} buffer_info_t;

typedef enum preview_state{
    EPreviewDisabled,
    EPreviewBeingDisabled,
    EPreviewEnabled
} preview_state;

#define IMGEE_MODE_DEFAULT 0

class STEExtIspCamera : public STECameraDeviceBase {
public:

    /** Set the notification and data callbacks */
    virtual void setCallbacks(camera_notify_callback notify_cb,
                              camera_data_callback data_cb,
                              camera_data_timestamp_callback data_cb_timestamp,
                              camera_request_memory get_memory,
                              void* user);

	int MMIO_Camera_fd;

	void MMIO_Camera_flash_control(int val);

    virtual int setPreviewWindow(
        struct preview_stream_ops *window);

    /**
     * The following three functions all take a msgtype,
     * which is a bitmask of the messages defined in
     * include/ui/Camera.h
     */

    /**
     * Enable a message, or set of messages, from CameraHardwareInterface
     */
    virtual void        enableMsgType(int32_t msgType);

    /**
     * Disable a message, or a set of messages, from CameraHardwareInterface
     */
    virtual void        disableMsgType(int32_t msgType);

    /**
     * Query whether a message, or a set of messages, is enabled.
     * Note that this is operates as an AND, if any of the messages
     * queried are off, this will return false.
     * From CameraHardwareInterface
     */
    virtual int msgTypeEnabled(int32_t msgType);

    /**
     * Start preview mode. When a preview image is available
     * preview_callback is called with the user parameter. The
     * call back parameter may be null.
     * From CameraHardwareInterface
     */
    virtual status_t startPreview();

    /**
     * Stop a previously started preview, from CameraHardwareInterface
     */
    virtual void stopPreview();

    /**
     * Returns true if preview is enabled, from CameraHardwareInterface
     */
    virtual int previewEnabled();

    /**
     * Start record mode. When a record image is available recording_callback()
     * is called with the user parameter.  Every record frame must be released
     * by calling releaseRecordingFrame().
     * From CameraHardwareInterface
     */
    virtual status_t startRecording();

    /**
     * Stop a previously started recording, from CameraHardwareInterface
     */
    virtual void stopRecording();

    /**
     * Returns true if recording is enabled, from CameraHardwareInterface
     */
    virtual int recordingEnabled();

    /**
     * Release a record frame previously returned by the recording_callback()
     * passed to startRecord().
     * From CameraHardwareInterface
     */
    virtual void releaseRecordingFrame(const void*);

    /**
     * Start auto focus, the callback routine is called
     * once when focusing is complete. autoFocus() will
     * be called again if another auto focus is needed.
     * From CameraHardwareInterface
     */
    virtual status_t autoFocus();

    /**
     * Cancels auto-focus function. If the auto-focus is still in progress,
     * this function will cancel it. Whether the auto-focus is in progress
     * or not, this function will return the focus position to the default.
     * If the camera does not support auto-focus, this is a no-op.
     * From CameraHardwareInterface
     */
    virtual status_t    cancelAutoFocus();

    /**
     * Take a picture. The raw_callback is called when
     * the uncompressed image is available. The jpeg_callback
     * is called when the compressed image is available. These
     * call backs may be null. The user parameter is passed
     * to each of the call back routines.
     * From CameraHardwareInterface
     */
    virtual status_t takePicture();

    /**
     * Cancel a picture that was started with takePicture.  You may cancel any
     * of the shutter, raw, or jpeg callbacks.  Calling this method when no
     * picture is being taken is a no-op.
     * From CameraHardwareInterface
     */
    virtual status_t cancelPicture();


    virtual int setParameters(const char*);

    virtual char* getParameters();

    /** The memory allocated by Camera HAL for returning parameters in
    getParameters() is released through this function. If this is not called
    then it is the resposibility of the Camera HAL client to free the memory.
    */
    virtual void putParameters(char *);

    /**
     * Send command to camera driver.
     * From CameraHardwareInterface
     */
    virtual status_t sendCommand(int32_t cmd, int32_t arg1, int32_t arg2);

    /**
     * Release the hardware resources owned by this object.  Note that this is
     * *not* done in the destructor.
     * From CameraHardwareInterface
     */
    virtual void release();

    virtual status_t dump(int fd) const;

//@TODO: needs to be implemented for recording???
    status_t storeMetaDataInBuffers(int enable) {
        return NO_ERROR;
    }

    void CameraFillBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

public:
    STEExtIspCamera(int cameraId);
    virtual ~STEExtIspCamera();
    static const int kRecordBufferCount = CAMERA_RECORD_BUFFER_COUNT;
    sem_t mStateCam_sem;
    sem_t mSetPortCam_sem;
    sem_t mStill_sem;
    sem_t mFocus_sem;
    sem_t mPreview_sem;
    Queue<OMX_BUFFERHEADERTYPE> mBuffQueue; /**< buffer queue */
    bool mTimeout;              /* Variable used to warn a camera timeout occurs */
    int  mNbConsecutiveTimeout; /* Variable to know the number of consecutive timeout that occurs */

    OMX_ERRORTYPE cleanUpDspPanic(void* aArg);

    void dispatchNotification(int32_t aMsg, int32_t aInfo1, int32_t aInfo2 = 0);
    void dispatchRequest(int32_t req, void *aArg = NULL , bool aWaitForRequest = true);
    void dispatchError(int32_t aInfo1, int32_t aInfo2 = 0);
    bool mErrorOnBoot;
    bool mRecordRunning;
    preview_state mPreviewState;
    PerfManager mPerfManager;
    bool mIsFirstViewFinderFrame;
    int mOldPreviewHeight;
    int mOldPreviewWidth;
    bool mCamMode;
    OMX_IN OMX_BUFFERHEADERTYPE* pLastRecordBuffer;
    bool mIsStillInProgress;

private:
    enum CAM_MODE {
        PREVIEW_MODE = 0,
        CAMCORDER_MODE = 1,
        COMMON_MODE = 2
    };
    status_t setParameters(const CameraParameters& params);
    static Mutex mCreateLock;
    static int beginPreviewThread(void *cookie);
    static int beginPictureThread(void *cookie);
    static int beginDispatchThread(void *cookie);

    OMX_ERRORTYPE init();
    OMX_ERRORTYPE initPrimaryDefaultParameters();
    OMX_ERRORTYPE initSecondaryDefaultParameters();

    status_t AllocateRecordHeapLocked();
    status_t AllocateStillHeapLocked();
    void FreeRecordHeapLocked();
    void FreeStillHeapLocked();
    void cameraConfigSet();
    void ConfigureStill();
    void UnconfigureStill();
    void supplyRecordBuffersToCam();
    void cleanUp();
    int previewThread();
    int pictureThread();
    int dispatchThread( void* cookie );
    void SetAllparameters();
    int SetPropContrast();
    int SetPropBrightness();
    int SetPropSharpness();
    int SetPropSaturation();
    int SetPropEffect();
    int SetPropZoom();
    int SetPropWhiteBalance();
    int SetPropFocus();
    int SetPropTouchAF();
    int SetPropFlash(const CameraParameters &params);
    int SetPropFlickerRemoval();
    int SetPropSceneModes();
    int SetPropExposure();
    void setFirstFrameParameters( int mode );
    OMX_ERRORTYPE getLensParameters();
    int SetPropAWBLock();
    int SetPropAELock();
    int SetPropTorch(bool on);
    int SetPropFocusRange(const CameraParameters &params);
    int SetPropJPEGQuality();
    OMX_ERRORTYPE setLensParameters(void *aArg=NULL);
    OMX_ERRORTYPE setMakeAndModel();
    OMX_ERRORTYPE SetSoftwareVersion();
    OMX_ERRORTYPE setOrientation();
    OMX_ERRORTYPE GetExtradataField(OMX_BUFFERHEADERTYPE *pOmxBufferHdr, OMX_SYMBIAN_CONFIG_EXIFSETTINGSTYPE *pExifSettings);
    void ReconfigurePreview();
    status_t doStartPreview();
    void     doStopPreview();
    void cameraReleaseOnTimeout();

    /**< To get message name */
    static const char* msgName(int32_t aParam);

    mutable Mutex       mFdLock;
    mutable Mutex       mLock;
    mutable Mutex       mLockTakePic;
    OMX_HANDLETYPE      mCam;
    CameraParameters    mParameters;
    sp<IMemoryHeap>        mVPB1Heap;
    int                 mPreviewCurrentIndex;
    int                 mStillFrameSize;
    int                 mThumbFrameSize;
    FILE*               mPreviewFile;
    FILE*               mRecordFile;
    bool                mOnCancelFocus;
    camera_notify_callback     mNotifyCb;
    camera_data_callback       mDataCb;
    camera_data_timestamp_callback mDataCbTimestamp;
    camera_request_memory mRequestMemoryFunc;
    void               *mCallbackCookie;
    int32_t             mMsgEnabled;

    // only used from PreviewThread
    unsigned int        mPreviewHeapBaseLogical;

    // only used from RecordThread
    unsigned int        mRecordHeapBaseLogical;

    MMHwBuffer* m_pHwBuffer_VPB0;
    MMHwBuffer* m_pHwBuffer_VPB1;
    PFMMHwBufferCreate    m_pfnMMHwBufferCreate;
    PFMMHwBufferDestroy   m_pfnMMHwBufferDestroy;
    MMHwBuffer::TBufferInfo bufInfo_VPB0;
    MMHwBuffer::TBufferInfo bufInfo_VPB1;
    OmxUtils            mOmxUtils; /**< OMX utils */
    DynSetting          mDynSetting; /**< Dynamic Setting */

    bool mIsPreviewHeapSet;
    bool mIsStillHeapSet;
    bool mIsVPB1PortDisabled;

    OMX_PARAM_PORTDEFINITIONTYPE mParamPortVPB0;
    OMX_PARAM_PORTDEFINITIONTYPE mParamPortVPB1;
    OMX_CALLBACKTYPE camCallback;
    OMX_CALLBACKTYPE jpegCallback;

    int mPreviewBufferIndex;
    bool mPreviewDump;
    bool mRecordDump;
    bool mRecordReplay;
    int mCameraId;
    bool mCameraWithoutEncoder;

    OmxILExtIndex*      mOmxILExtIndex;  /**< Extension index */
    SteJpegEnc*         mJpegEncpict; /**< jpeg encoder session for picture encoding */
    SteJpegEnc*         mJpegEncThumb; /**< jpeg encoder session for thumbnail encoding */
    SteArmIvProc*       ArmIvProc; /**< ArmIvProc session */
    SteHwConv* mPreviewBlitter;  /**< session of hardware blitter for preview path */
    SteHwConv* mRecordBlitterPass1;  /**< session of hardware blitter for record path */
    SteHwConv* mRecordBlitterPass2;  /**< session of hardware blitter for record path */
    SteHwConv* mStillBlitter;  /**< session of hardware blitter for Capture path */

    int mPreviewWidth;
    int mPreviewHeight;
    int mRecordWidth;
    int mRecordHeight;
    int mPictureWidth;
    int mPictureHeight;
    int mVPB0Width;
    int mVPB0Height;
    int  isFrameRateSupported(const char *fps_list, int framerate);
    void updateFrameRate(int &aFramerate, int &aMinFramerate, int &aMaxFramerate);
    void setPreviewFpsRange(int min, int max);
    Mutex mPreviewLock;
    Mutex mRecordLock;
    MyNativeWindow* mPreviewWindow;
    int mTotalNumOfNativeBuff;
    buffer_info_t mRecordBufferInfo[kRecordBufferCount];
    buffer_info_t mImageBufferInfo;
    bool mIsRecordHeapSet;
    buffer_handle_t mTempBuffHandle;
    MMNativeBuffer* mTempNativeBuffer;
    int mImgEEMode;
    PFMMCreateNativeBuffer mCreateMMNativeBuffer;
    PFMMDestroyNativeBuffer mDestroyMMNativeBuffer;
    void* mLibHandle;
    OMX_COLOR_FORMATTYPE mOmxRecordPixFmt;
#ifdef ENABLE_FACE_DETECTION
    bool mFaceDetection; /**<true - if face detection enable else false */
    enum faceDetectionAlgoType {
        NEVEN=0,
        STE
    };
    camera_face_t mdemoFace_yuv[10];
    camera_frame_metadata_t mfaceMetaData_yuv;
    STEFaceDetectorBase* mFaceDetector;
    inline bool isFaceDetectionEnable(void);
#endif

    OMX_ERRORTYPE enableNativeBuffOnOMXComp(OMX_HANDLETYPE aOmxComponent,int aportIndex);
    status_t shareBufferWithCamera(buffer_info_t& aBuffer, int aPortIndex);
    status_t allocatePreviewHeap();
    status_t setupVideoMetadata(buffer_info_t& aBuffer);
    OMX_BOOL mbOneShot;
};

#ifdef ENABLE_FACE_DETECTION
inline bool STEExtIspCamera::isFaceDetectionEnable(void)
{
    return mFaceDetection;
}
#endif  //ENABLE_FACE_DETECTION

class reqDispatchHdl{
public:
    // enumeration of request identifier
    enum reqId {
        NoReq = 0,
        cleanUpDspPanicReq = 1,
        restartOnTimeout = 2,
        setLensParametersReq = 3,
        focusStatusReached = 4,
        focusStatusUnableToReach = 5
    };
    int32_t aMsg;
    int32_t aInfo1;
    int32_t aInfo2;
    int32_t aReq;
    void *aArg;
    bool aWaitForRequest;
    STEExtIspCamera* camera;
};
}; // namespace android

#endif
