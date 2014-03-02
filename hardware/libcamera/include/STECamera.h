 /*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMERA_H
#define STECAMERA_H

//System includes
#include <hardware/camera.h>
#include <ui/GraphicBuffer.h>
#include <ui/GraphicBufferAllocator.h>
#include <utils/threads.h>

//Multimedia includes
#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>

//Internal includes
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STECamOmxILExtIndex.h"
#include "STECamFrameRateDumper.h"
#include "STECamPerfManager.h"
#include "STECamMemoryHeapBase.h"
#include "STECamOmxBuffInfo.h"
#include "STECamThreadInfo.h"
#include "STECamOmxReqCbHandler.h"
#include "STECamFrameDumper.h"
#include "STECamFrameReplay.h"
#include "STECamImageInfo.h"
#include "STECamThumbnailHandler.h"
#include "STECameraDeviceBase.h"
#include "STENativeWindow.h"
#include "NevenFaceDetector.h"
#include "hwconversion/STECamHwConversion.h"

namespace android {

#define MAX_CAMERAS_SUPPORTED 2
#define MAX_SIMUL_CAMERAS_SUPPORTED 1


#define START_OF_STRUCT(ADDR,TYPE,MEMBER)   ((char*)(ADDR) - ((int)&((TYPE*)0)->MEMBER))

#define CAM_VPB 0

#define STE_MAX_PREVIEW_WIDTH   1024
#define STE_MAX_PREVIEW_HEIGHT  768

#define STE_MAX_HI_RES_PREVIEW_WIDTH    1600
#define STE_MAX_HI_RES_PREVIEW_HEIGHT   1200

#ifdef STE_SENSOR_8MP
#define DEFAULT_PREVIEW_WIDTH_REC_MODE 960
#define DEFAULT_PREVIEW_HEIGHT_REC_MODE 540
#else //! STE_SENSOR_8MP
#define DEFAULT_PREVIEW_WIDTH_REC_MODE 720
#define DEFAULT_PREVIEW_HEIGHT_REC_MODE 408
#endif //STE_SENSOR_8MP

#define STILL_WIDTH_2MP 1728
#define STILL_HEIGHT_2MP 976

#define OVERSCAN_STILL_WIDTH_2MP 1920
#define OVERSCAN_STILL_HEIGHT_2MP 1088

#define SNAPSHOT_WIDTH_2MP 1920
#define SNAPSHOT_HEIGHT_2MP 1080


typedef struct image_rect_struct
{
  uint32_t width;      /* Image width */
  uint32_t height;     /* Image height */
} image_rect_type;

//Typedef
typedef OMX_ERRORTYPE (STECamera::*reqHandler)(void* aArg);

//Forward declarations
class SwProcessingThreadData;
class AutoFocusThreadData;
class ReqHandlerThreadData;
template<typename T>
class ThreadContainer;
class FocusHandler;
class AutoFocusHandler;
class ContinuousAutoFocusHandler;
class FixedFocusHandler;
class ThumbnailHandler;
class EnableDataFlow;
class DisableDataFlow;
class ImageRotHandler;
class DeviceHwRotation;
class DeviceSwRotation;
class ExifRotation;
class STEFaceDetectorBase;

class STECamera : public STECameraDeviceBase
{
public:
    STECamera(int CamreID);
    ~STECamera();
    OMX_ERRORTYPE init();

/****************************************************************
 *
 *
 *          Interface APIs
 *
 *
 ****************************************************************/

    /* Set Active window for still and record view finder */
    int setPreviewWindow(struct preview_stream_ops *window);

    /** Set the notification and data callbacks */
    void setCallbacks(camera_notify_callback notify_cb,
                        camera_data_callback data_cb,
                        camera_data_timestamp_callback data_cb_timestamp,
                        camera_request_memory get_memory,
                              void* user);

    /**
     * The following three functions all take a msgtype,
     * which is a bitmask of the messages defined in
     * include/ui/Camera.h
     */

    /**
     * Enable a message, or set of messages, from CameraHardwareInterface
     */
    void enableMsgType(int32_t msgType);

    /**
     * Disable a message, or a set of messages, from CameraHardwareInterface
     */
    void disableMsgType(int32_t msgType);

    /**
     * Query whether a message, or a set of messages, is enabled.
     * Note that this is operates as an AND, if any of the messages
     * queried are off, this will return false.
     * From CameraHardwareInterface
     */
    int msgTypeEnabled(int32_t msgType);

    /**
     * Start preview mode. When a preview image is available
     * preview_callback is called with the user parameter. The
     * call back parameter may be null.
     * From CameraHardwareInterface
     */
    status_t startPreview();

    /**
     * Stop a previously started preview, from CameraHardwareInterface
     */
    void stopPreview();

    /**
     * Returns true if preview is enabled, from CameraHardwareInterface
     */
    int previewEnabled();

    /**
     * Start record mode. When a record image is available recording_callback()
     * is called with the user parameter.  Every record frame must be released
     * by calling releaseRecordingFrame().
     * From CameraHardwareInterface
     */
    status_t startRecording();

    /**
     * Stop a previously started recording, from CameraHardwareInterface
     */
    void stopRecording();

    /**
     * Returns true if recording is enabled, from CameraHardwareInterface
     */
    int recordingEnabled();

    /**
     * Release a record frame previously returned by the recording_callback()
     * passed to startRecord().
     * From CameraHardwareInterface
     */
    void releaseRecordingFrame(const void *opaque);

    /**
     * Take a picture. The raw_callback is called when
     * the uncompressed image is available. The jpeg_callback
     * is called when the compressed image is available. These
     * call backs may be null. The user parameter is passed
     * to each of the call back routines.
     * From CameraHardwareInterface
     */
    status_t takePicture();

    /**
     * Cancel a picture that was started with takePicture.  You may cancel any
     * of the shutter, raw, or jpeg callbacks.  Calling this method when no
     * picture is being taken is a no-op.
     * From CameraHardwareInterface
     */
    status_t cancelPicture();

    /**
     * Start auto focus, the callback routine is called
     * once when focusing is complete. autoFocus() will
     * be called again if another auto focus is needed.
     * From CameraHardwareInterface
     */
    status_t autoFocus();

    /**
     * Cancels auto-focus function. If the auto-focus is still in progress,
     * this function will cancel it. Whether the auto-focus is in progress
     * or not, this function will return the focus position to the default.
     * If the camera does not support auto-focus, this is a no-op.
     * From CameraHardwareInterface
     */
    status_t cancelAutoFocus();

    /** Set the camera parameters, from CameraHardwareInterface */
    status_t setParameters(const CameraParameters& params);
    status_t setParameters(const char* parameters);


    /** Return the camera parameters, from CameraHardwareInterface */
    CameraParameters getParameters() const;
    char* getParameters();
    void putParameters(char *parms);
    /**
     * Send command to camera driver.
     * From CameraHardwareInterface
     */
    status_t sendCommand(int32_t cmd, int32_t arg1, int32_t arg2);

    /**
     * Release the hardware resources owned by this object.  Note that this is
     * *not* done in the destructor.
     * From CameraHardwareInterface
     */
    void release();

    /**
     * Dump state of the camera hardware, from CameraHardwareInterface
     */
    int dump(int fd) const;

    status_t storeMetaDataInBuffers(int enable)
    {
        return NO_ERROR;
    }

/****************************************************************
 *
 *
 *        Camera HAL Internal APIs
 *
 *
 ****************************************************************/

public:
    static inline OMX_ERRORTYPE cropOutputSize(OMX_HANDLETYPE aHandle, const int aWidth, const int aHeight);

    template<typename T>
    friend class SwProcessingThread;
    template<typename T>
    friend class PreviewThread;
    template<typename T>
    friend class RecordThread;
    template<typename T>
    friend class AutoFocusThread;
    template<typename T>
    friend class ReqHandlerThread;
    friend OMX_ERRORTYPE CameraFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                              OMX_IN OMX_PTR pAppData,
                                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

    friend OMX_ERRORTYPE IsprocFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                              OMX_IN OMX_PTR pAppData,
                                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
    friend OMX_ERRORTYPE JpegFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                            OMX_IN OMX_PTR pAppData,
                                            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
    friend OMX_ERRORTYPE JpegEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                             OMX_IN OMX_PTR pAppData,
                                             OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
    friend OMX_ERRORTYPE CameraEventHandler(OMX_HANDLETYPE hComponent,
                                            OMX_PTR pAppData,
                                            OMX_EVENTTYPE eEvent,
                                            OMX_U32 nData1,
                                            OMX_U32 nData2,
                                            OMX_PTR pEventData);
    friend OMX_ERRORTYPE IspprocEventHandler(OMX_HANDLETYPE hComponent,
                                             OMX_PTR pAppData,
                                             OMX_EVENTTYPE eEvent,
                                             OMX_U32 nData1,
                                             OMX_U32 nData2,
                                             OMX_PTR pEventData);
    friend OMX_ERRORTYPE JpegEventHandler(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_EVENTTYPE eEvent,
                                          OMX_U32 nData1,
                                          OMX_U32 nData2,
                                          OMX_PTR pEventData);
    friend OMX_ERRORTYPE SWJpegEventHandler(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_EVENTTYPE eEvent,
                                          OMX_U32 nData1,
                                          OMX_U32 nData2,
                                          OMX_PTR pEventData);
    friend OMX_ERRORTYPE ArmIvEventHandler(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_EVENTTYPE eEvent,
                                          OMX_U32 nData1,
                                          OMX_U32 nData2,
                                          OMX_PTR pEventData);
    friend OMX_ERRORTYPE ExifEventHandler(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_EVENTTYPE eEvent,
                                          OMX_U32 nData1,
                                          OMX_U32 nData2,
                                          OMX_PTR pEventData);
    friend class CFrameDump;
    friend class CFrameReplay;
    friend class FocusHandler;
    friend class AutoFocusHandler;
    friend class ContinuousAutoFocusHandler;
    friend class FixedFocusHandler;
    friend class ImageRotHandler;
    friend class DeviceRotation;
    friend class DeviceSwRotation;
    friend class DeviceHwRotation;
    friend class ExifRotation;

public:
    static const int kRecordBufferCount = CAMERA_RECORD_BUFFER_COUNT;
    static const int kMaxStillBuffers = 6;
    static const int kTotalPreviewBuffCount=5;

private:
    class PreviewThreadData
    {
     public:
        /* CTOR */
        inline PreviewThreadData();

     public:
        SwProcessingThreadData* mSwProcessingThreadData;

     private:
        uint32_t mState; /**< Buffer state */
    };

    /**
      * Camera Mode is used to select between Still:0, Video:1, Still + Video : -1.
      * When selecting a particular mode, resources relevant to only that are allocated.
      */
    enum CameraMode { EInvalid = -2, EStill = 0, EVideo = 1};

private:
    static int beginPictureThread(void *cookie);

private:
    /**** Define all inline functions here ****/

    /*Get the camera sensor mode*/
    inline OMX_BOOL getCamSensorMode();

    /* Camera Format Info */
    inline const CameraFormatInfo& cameraFormatInfo() const;

    /* Enable callbacks  */
    inline void enableCallbacks(OmxBuffInfo* aOmxBuffInfo, uint32_t aCount);

    /* Disable callbacks  */
    inline void disableCallbacks(OmxBuffInfo* aOmxBuffInfo, uint32_t aCount);

    /* update extended capturing */
    inline OMX_ERRORTYPE updateCapturing(OMX_U32 aPortIndex, OMX_BOOL aEnable);

    /* update autopause */
    inline OMX_ERRORTYPE updateAutoPause(OMX_BOOL aEnable);

    /* update smoothzoom */
    inline OMX_ERRORTYPE updateSmoothZoom(OMX_BOOL aEnable);

    void getCropVectorForZoom(int& lx, int& ly, size_t& w, size_t& h, bool preview);

    /* Synch cache */
    inline OMX_ERRORTYPE synchCBData(MMHwBuffer::TSyncCacheOperation aOp,
                                     MMHwBuffer& aMMHwBuffer,
                                     OMX_U8* aBuffer,
                                     OMX_U32 aAllocLen);
    /* Dispatch error */
    inline void dispatchError(int32_t aInfo1, int32_t aInfo2 = 0);

    /* Preview callback size */
    inline uint32_t previewCallbackSize(uint32_t aTotalSize, uint32_t aOldFrameSize, uint32_t aNewFrameSize) const;

    /* Focus handler */
    inline FocusHandler* focusHandler();

    /* Push preview buffer */
    inline OMX_ERRORTYPE pushPreviewBuffer(const OmxBuffInfo& aOmxBuffInfo);

    inline ThumbnailHandler* getThumbnailHandler();
    inline ImageRotHandler* getRotationHandler();

    /* Check validity of preview size */
    inline bool checkPreviewSizeValid(const char* aPreviewSize);

#ifdef ENABLE_FACE_DETECTION
    inline bool isFaceDetectionEnable(void);
#endif

private:
    /**** Define all member functions here ****/

    status_t initGlobalParams();
    OMX_ERRORTYPE initDefaultParameters();
    OMX_ERRORTYPE initPrimaryDefaultParameters();
    OMX_ERRORTYPE initSecondaryDefaultParameters();
    OMX_ERRORTYPE doZoom(uint32_t aZoomKey);
    OMX_ERRORTYPE smoothZoomComplete();
    OMX_ERRORTYPE startSmoothZoom(int32_t aZoomKey);
    OMX_ERRORTYPE stopSmoothZoom();

    status_t initThreads();
    void destroyThreads();
    void destroySemaphores();
    void destroyHeaps(bool aPreviewRunning = false);

    /** Free buffers that were allocated when configuring video vf */
    OMX_ERRORTYPE freeVideoBuffers();
    /** Free buffers that were allocated when configuring still vf */
    OMX_ERRORTYPE freeStillBuffers();
    OMX_ERRORTYPE freeVFBuffers(bool aPreviewRuning = false);

    OMX_ERRORTYPE sendStillComponentsToLoadedState();
    OMX_ERRORTYPE sendVideoComponentsToLoadedState();

    OMX_ERRORTYPE sendStillComponentsToIdleState();
    OMX_ERRORTYPE sendVideoComponentsToIdleState();

    int  isFrameRateSupported(const char *fps_list, int framerate);
    bool updateFrameRate(int &aFramerate, int &aMinFramerate, int &aMaxFramerate);
    bool updateHiResVF(int &aPreviewWidth, int &aPreviewHeight, int aHiResVFView);
    bool updatePreviewSize(int &aPreviewWidth, int &aPreviewHeight);
    bool updateRecordSize(int &aRecordWidth, int &aRecordHeight);
    bool updatePictureSize(int &aPictureWidth, int &aPictureHeight);
    bool updateThumbnailSize(int &aThumbnailWidth, int &aThumbnailHeight);
    bool updatePictureQuality(int &aPictureQuality);
    bool updateThumbnailQuality(int &aThumbnailQuality);

    /** configure heap required for allocating buffers to be used for capturing a still image */
    void initStillHeap();
    void initStillRawHeap();
    void initStillZSLHeap();
    /** configure heap required for allocating buffers to be used for capturing a video */
    void configureVideoHeap();
    /** configure heap required for allocating buffers to be used for capturing and preview frames */
    OMX_ERRORTYPE initPreviewHeap(bool aPreviewRunning = false);
    OMX_ERRORTYPE initSnapshotHeap();

    OMX_ERRORTYPE disableAllCamPorts(); /* Disable all hsmcamera ports, VPB0, VPB1, VBPB2 */

    OMX_ERRORTYPE configureCamMode(int aCamMode, bool aStillZSL);

    /**
      * Do configuration needed for still vf and still capture use case.
      * All needed OMX components are configured, tunneled and set in executing
      * state.
      * Note: There is one exception for hsmcamera, which is only sent to OMX_StateIdle
      * and sent to OMX_StateExecuting, only when camera application calls startPreview().
      */
    OMX_ERRORTYPE doStillModeConfig(bool aPreviewRunning = false);
    /**
      * Clear configurations done in doStillModeConfig(), so that another mode can be
      * setup correctly. Cleaning up involves releasing all the resources and transitioning
      * to OMX_StateLoaded. Also, hsmcamera ports are disabled which were enabled in
      * doStillModeConfig() i.e. VPB0 and VPB1.
      */
    OMX_ERRORTYPE clearStillModeConfig();

    /**
      * Disable hsmcamera ports involved in still vf and still capture use cases
      * i.e. VPB0 and VPB1.
      */
    OMX_ERRORTYPE disableStillPorts();

    OMX_ERRORTYPE doVideoModeConfig(bool aPreviewRunning = false);
    OMX_ERRORTYPE clearVideoModeConfig();
    /**
      * Disable hsmcamera ports involved in video vf and video capture use cases
      * i.e. VPB0 and VPB2.
      */
    OMX_ERRORTYPE disableVideoPorts();
    OMX_ERRORTYPE disablePreviewPort(bool aPreviewRunning = false);
    OMX_ERRORTYPE enablePreviewPort(bool aPreviewRunning = false);
    bool isPreviewPortEnabled();

    OMX_ERRORTYPE supplyPreviewBuffers();
    OMX_ERRORTYPE supplyRecordBuffers();
    OMX_ERRORTYPE cleanUp();
    OMX_ERRORTYPE preOmxCompCleanUp();
    OMX_ERRORTYPE postOmxCompCleanUp();
    OMX_ERRORTYPE cleanUpDspPanic(void* aArg);
    OMX_ERRORTYPE freeComponentHandles();
    int previewThread();
    int recordThread();
    int pictureThread();
    OMX_ERRORTYPE ReconfigureStill(const ImageInfo& aPictureInfo, bool aReconfigStillHeap);
    OMX_ERRORTYPE ReconfigureThumbnail(const ImageInfo& aPicInfo ,const ImageInfo& aThumbnailInfo);
    OMX_ERRORTYPE ReconfigureJpegQuality(const ImageInfo& aPictureInfo);
    OMX_ERRORTYPE ReconfigureThumbnailQuality(const ImageInfo& aPicInfo ,const ImageInfo& aThumbnailInfo);

     /**
     * Reconfigures involved omx components and heaps for still use case, i.e. still viewfinder
     * and a part of still capture pipe
     */
    OMX_ERRORTYPE ReconfigureVFStill(bool aReconfigureHeap = true);
    /**
      * Reconfigure involved omx components and heaps for video use case, i.e. video viewfinder
      * and video capture pipes.
      */
    OMX_ERRORTYPE ReconfigureVFVideo(bool aReconfigureHeap = true,
                                     bool aReconfigureVPB0 = true);
    void initPreviewOmxColorFormat(const char* const aPreviewFormat);
    bool checkPreviewFormat(const char* const aPreviewFormat);
    /* Select appropriate dataflow (EnableDataFlow/DisableDataFlow) depending
     * upon given thumbnail size */
    inline void selectThumbnailHandler(int aThumbnailWidth, int aThumbnailHeight);
    /**< To get message name */
    static const char* msgName(int32_t aParam);
    OMX_BOOL convertToDegrees(OMX_U64& aDegrees, OMX_U64& aMinutes, OMX_U64& aSeconds, char const* const aGPSKey );
    void convertUTCToTime(OMX_U64& aHours, OMX_U64& aMinutes, OMX_U64& aSeconds, char const* const aGPSKey );
    float extractSubUnitLocation(float& aWholeValue);
    OMX_U64 extractSubUnitTime(OMX_U64 &tmpTime);
    void setFocusDistance();
    OMX_ERRORTYPE  setLensParameters(void *aArg=NULL);
    OMX_ERRORTYPE  setGPSTags(const CameraParameters& aParams, OMX_HANDLETYPE aHandle, int aPort);
    OMX_ERRORTYPE  setMakeAndModel();
    OMX_ERRORTYPE  configVideoRevertAspectRatio(OMX_HANDLETYPE hComponent, OMX_BOOL aEnable);
    OMX_ERRORTYPE  setPropVideoRotate(OMX_HANDLETYPE aCam, int aRotation);
    status_t  SetPropZoom(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropWhiteBalance(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropSceneModes(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropFocus(const CameraParameters& params,OMX_HANDLETYPE mCam, bool aSetControl=false);
    status_t  SetPropFlash(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropFlickerRemoval(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetExposureCompensation(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropContrast(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropBrightness(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropSaturation(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropColorEffect(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropSharpness(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropRegionControl(const CameraParameters& params,OMX_HANDLETYPE mCam);
    status_t  SetPropMeteringArea(const CameraParameters& params, OMX_HANDLETYPE mCam);
    status_t  SetPropFocusArea(const CameraParameters& params, OMX_HANDLETYPE mCam);
    status_t  SetPropAWBLock(const CameraParameters& params, OMX_HANDLETYPE mCam);
    status_t  SetPropAELock(const CameraParameters& params, OMX_HANDLETYPE mCam);
    void Append_WhiteBalance(String8 &wb,String8 &wb_set);
    void Append_SceneMode(String8 &scene_mode,String8 &scene_mode_set);
    void Append_FlashMode(String8 &flash_mode,String8 &flash_mode_set);
    void Append_FocusMode(String8 &focus_mode,String8 &focus_mode_set);
    void Append_FlickerRemovalMode(String8 &flicker_mode,String8 &flicker_mode_set);
    void Append_PreviewFormats(String8 &spf);
    void Append_ColorEffect(String8 &effect,String8 &effect_set);
    void Append_MeteringMode(String8 &metering_mode,String8 &metering_mode_set);
    void Append_RegionControl(String8 &aRegion, String8 &aRegion_set);
    void Append_Val(const String8 &aKey,const String8 &val);
    void resetProperties();
    int getSceneMode(const CameraParameters& params);
#ifdef ENABLE_FACE_DETECTION
    int sendFaceROI(camera_frame_metadata_t aFaceMetaData);
#endif
    /* Apply the Camera Properties/Effects */
    status_t applyCameraProperties(const CameraParameters& aParams);

    /* Setup sw rotation processing for preview/picture/video */
    void setupPreviewSwRotProcessing(int aRotation);
    void setupVideoSwRotProcessing(int aRotation);
    /* Setup sw conversion processing for preview*/
    status_t setupPreviewSwConProcessing(const char* const aFormat);
    status_t setupPreviewSwBuffers(int aWidth, int aHeight);
    status_t setupRawSwConProcessing(
            const char* const aFormat,
            int aWidth, int aHeight);

    /* Do sw processing */
    sp<MemoryBase>& doPreviewSwRotProcessing(
            const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
            /* MMHwBuffer* const aMMHwBuffer, */
            int aPrevWidth, int aPrevHeight);
    sp<MemoryBase>& doPreviewSwConProcessing(
            const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
            /* MMHwBuffer* const aMMHwBuffer, */
            int aPrevWidth, int aPrevHeight);

       /* Do picture sw processing */
    void doPictureSwRotProcessing(
            const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
            MMHwBuffer* const aMMHwBuffer);

    /* Do Smooth Zoom Processing */
    OMX_ERRORTYPE doSmoothZoomProcessing(void* aArg);

    /* Do preview processing */
    status_t doPreviewProcessing(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags);

    /* Do record processing */
    status_t doRecordProcessing(OmxBuffInfo& aData);

    /* Do autofocus processing */
    status_t doAutoFocusProcessing(AutoFocusThreadData& aData);

    /* Do callback processing */
    status_t doCallbackProcessing(ReqHandlerThreadData& aData);

    /* Raw callback processing */
    OMX_ERRORTYPE doRawProcessing(void* aArg);

    /* setup XP70 traces */
    void setupXP70Traces();

    /* flush viewfidner buffers */
    status_t flushViewFinderBuffers(bool aFlushCamera = true);

    /* flush video buffers */
    status_t flushVideoBuffers(bool aFlushCamera = true);

    /* Set request callback handlers */
    void setupReqCbHandlers();

    /* Start recording */
    OMX_ERRORTYPE doStartRecording(void* aArg);

    /* Stop recording */
    OMX_ERRORTYPE doStopRecording();

    /* Stop preview */
    OMX_ERRORTYPE doStopPreview();

    /* Unlocked StartPreview API*/
    status_t doStartPreview();

    /* Configure and Start Preview for Hi Resolution View Finder */
    void configureAndStartHiResVF();

    /* SetPreviewFpsrange */
    void setPreviewFpsRange(int min, int max);

    /* Get TColorFmt used in dump & replay from OMX_COLOR_FORMATTYPE */
    CFrameDumpReplay::TColorFmt getDumpReplayColorFmt(OMX_COLOR_FORMATTYPE aOmxColorFmt);

    /* Dispatch notification */
    void dispatchNotification(int32_t aMsg, int32_t aInfo1, int32_t aInfo2 = 0);

    /* Dispatch Request */
    void dispatchRequest(reqHandler aReqHandler, void* aArg = NULL, bool aWaitForRequest = true);

    /* find free slot for request */
    ReqHandlerThreadData* findFreeReqSlot();

    /* Check is shke detection notification is to be given */
    OMX_ERRORTYPE checkForShakeDetection(void* aArg);

    /* Setup all extra keys */
    void setupExtraKeys();

    /* Viewfinder steps post omx config */
    OMX_ERRORTYPE postOMXConfigStartViewfinder(bool aIsFirstVF = false);

    /* Videorecord steps post omx config */
    OMX_ERRORTYPE postOMXConfigStartVideoRecord();

    /* Allocate processing buffer */
    status_t initProcessingBuffers();

    /* Initialize mAutoFocusHandler, mContinuousAutoFocusHandler instances*/
    status_t initFocusHandler();

    /* Initialize mEnableDataFlow, mDisableDataFlow instances*/
    OMX_ERRORTYPE initThumbnailHandler();

    /* Initialize mImageRotHandler, mDeviceHwRotation, mDeviceSwRotation instances*/
    status_t initImageRotHandler();

    /* Check if preview is enabled */
    bool checkPreviewEnabled();

    /* Check if record is enabled */
    bool checkRecordingEnabled();

    status_t getNativeBuffFromNativeWindow(void);
    status_t sendCancelReqToNativeWindow(int index);
    status_t sendCancelReqToCameraHalNativeWindow(int index);
    status_t shareNativeBuffWithOMXPorts(bool aPreviewRunning = false);
    status_t getLockForNativeBuffer(int index);
    int dequeueNativeBuffer(void);
    int  findNativeBuffIndex(buffer_handle_t *buf);
    status_t renderNativeBuffer(int index);
    void cancelAllNativeBuffers(void);
    void cancelCameraHalNativeBuffers(void);
    OMX_ERRORTYPE enableNativeBuffOnOMXComp(OMX_HANDLETYPE aOmxComponent,
                                            int aportIndex);

    /* Set Video Stab Config */
    OMX_ERRORTYPE setConfigVideoStab(OMX_BOOL aStabState = OMX_TRUE);

    /* Get the crop vector for video stabilization */
    void getCropVector(
            OMX_U8* apTmp, uint32_t& aLx, uint32_t& aLy,
            uint32_t& aW, uint32_t& aH);

    /* Updates the Video Stab Status when application
     * dynamically switches the video stabilization feature
     */
    status_t UpdateVideoStabStatus();

    /* Initializes Ports of all components.
     * The function added to removed the duplicate code in
     * different Configuration functions
     */
    OMX_ERRORTYPE InitializePort(
            OMX_PARAM_PORTDEFINITIONTYPE& aPortDefType,
            OMX_PORTDOMAINTYPE aPortDomain,
            OMX_HANDLETYPE aOmxComponent,
            int aPortIndex,
            bool aStillMode = true);

    /* All B2R2 operations to be handled in a seperate thread
     */
    static void* B2R2ThreadWrapper(void* me);
    void B2R2ThreadEntry();
    void doB2R2Processing(int aSrcIndex);

private:
    // SceneMode Controller Class
    class SceneModeController {
        public:
            // some enums and types
            enum Mode {
                EAuto,
                EBeach,
                ESnow,
                EParty,
                EPortrait,
                ENightPortrait,
                ELandscape,
                ENight,
                EAction,
                ESunset,
                ECandlelight,
                EBacklight,
                ECloseup,
                ESports,
                EDocument,
                EMaxMode
            };

            enum Property {
                ESceneMode,
                EWhiteBalance,
                EAEMetering,
                EExpControl,
                EExpSensitivity,
                EExpEvComp,
                EFlashControl,
                EFocusRange,
                ESaturation,
                EImageFilter,
                ESharpness,
                EMaxProperty
            };

            enum {
                kInvalidValue = -1,
                kAutoSensitivity = -2
            };

            struct TData {
                inline TData(int a, int b, int c, int d, int e, int f,
                             int g, int h, int i, int j, int k)
                {
                    mData[0] = a;
                    mData[1] = b;
                    mData[2] = c;
                    mData[3] = d;
                    mData[4] = e;
                    mData[5] = f;
                    mData[6] = g;
                    mData[7]  = h;
                    mData[8]  = i;
                    mData[9]  = j;
                    mData[10] = k;
                }
                int mData[EMaxProperty];
            private:
                inline TData() { }
            };

        public:
            SceneModeController(STECamera *pCameraHwItf);
            ~SceneModeController();
            int initialize();
            OMX_ERRORTYPE setSceneMode(Mode aMode);
            OMX_ERRORTYPE setGamma(Mode aMode);

        private:
            inline static int value(Mode aMode, Property aProperty) {
                DBGT_ASSERT(aMode < EMaxMode, "Invalid scene mode %d specified", aMode);
                DBGT_ASSERT(aProperty < EMaxProperty, "Invalid property %d specified", aProperty);

                return sProperties[aMode].mData[aProperty];
            }

        private:
            STECamera *mCameraHwItf;
            bool mInitialized;
            static TData sProperties[];
    };

    friend class SceneModeController;
    SceneModeController *mSceneCtrl;

private:
    /**** Define all member variables here ****/

    //Interfaces are not thread-safe, client needs to take care
    DynSetting          mDynSetting; /**< Dynamic Setting */
    mutable Mutex       mLock;
    mutable Mutex       mTakePictureLock;
    mutable Mutex       mPrevLock;

    OMX_HANDLETYPE      mCam;
    OMX_HANDLETYPE      mExifMixer;
#if defined(SWISPPROC) || defined(HWISPPROC)
    OMX_HANDLETYPE      mISPProc;
#endif // SWISPPROC || HWISPPROC
#ifdef JPEGENC
    OMX_HANDLETYPE      mJpegEnc;
#endif

    CameraParameters    mParameters;

    STENativeWindow      *mPreviewWindow;
    sp<IMemoryHeap>       mPreviewHeap[kTotalPreviewBuffCount];
    sp<GraphicBuffer>     mGraphicBuffer[kTotalPreviewBuffCount];
    sp<GraphicBuffer>     mRecordGraphicBuffer[kRecordBufferCount];
    video_metadata_t     *mRecordMetaData[kRecordBufferCount];

    sp<MemoryHeapBase>  mRecordHeap;
    sp<MemoryHeapBase>  mRawHeap;
    sp<MemoryHeapBase>  mISPImageHeap;
    sp<MemoryHeapBase>  mStillHeap;

    OmxBuffInfo         mPreviewOmxBuffInfo[kTotalPreviewBuffCount];
    OmxBuffInfo         mRecordOmxBuffInfo[kRecordBufferCount];
    OmxBuffInfo         mRawOmxBuffInfo;
    OmxBuffInfo         mStillOmxBuffInfo[kMaxStillBuffers];
    OmxBuffInfo         mISPRawOmxBuffInfo;
    OmxBuffInfo         mISPImageOmxBuffInfo;
    OmxBuffInfo         mJpegOmxBuffInfo[kMaxStillBuffers];
    OmxBuffInfo         mArmIvOmxBuffInfo;
    OmxBuffInfo         mTempStillOmxBuffInfo;

    ImageInfo           mPreviewInfo;
    ImageInfo           mPictureInfo;
    ImageInfo           mThumbnailInfo;
    ImageInfo           mRecordInfo;
    sp<MemoryHeapBase>  mPreviewTempBuffer; // Temp buffer for preview

  /** mCamMode contains mode in which camera is currently operating.
    * Camera application can choose to supply camera hal with
    * mode it wants to operate in using the key "KEY_CAM_MODE"
    * with values : 0 for Still and 1 for Video.
    * If this key is supplied, resources are allocated only for the
    * requested mode, thereby resulting in conservation of resources.
    */
    int                 mCamMode;
    int                 mStillZslBufferCount;
    volatile bool       mPreviewRunning;
    int                 mPreviewFrameSize;
    int                 mRecordFrameSize;
    volatile bool       mRecordRunning;
    int                 mPreviousCamMode;
    int                 mContrast;
    int                 mBrightness;
    int                 mSaturation;
    int                 mCurrentZoomLevel;
    char                mCurrentSceneMode[100];
    char                mCurrentWbMode[100];
    char                mCurrentMeteringMode[100];
    char                mCurrentExpSensitivityMode[100];
    char                mCurrentEVCompMode[100];
    char                mCurrentImageFilterMode[100];
    char                mCurrentSaturationMode[100];
    char                mCurrentSharpnessMode[100];
    char                mCurrentFocusMode[100];
    //Focus Region
    char                mCurrentRegControl[100];
    char                mCurrentRegCoordinate[300];
    int                 mCurrentRegCoordinateNum;
    int32_t             mSmoothZoomLevel; /**< Requested smooth level */
    bool                mSmoothZoomStopPending; /**< Smooth zoom stop pending */
    bool                mUpdateAutoWhiteBalanceLock;
    bool                mUpdateAutoExposureLock;
    // protected by mLock
    camera_notify_callback     mNotifyCb;
    camera_data_callback       mDataCb;
    camera_data_timestamp_callback mDataCbTimestamp;
    camera_request_memory mRequestMemory;
    void               *mCallbackCookie;
    volatile int32_t    mMsgEnabled;

    int                 mIsCameraConfigured;
    bool                mIsPictureCancelled;
    bool                mIspreviewsizechanged;
    bool                mIsrecordsizechanged;
    int                 mConfigPreviewHeight;
    int                 mConfigPreviewWidth;
    int                 mConfigFrameRate;
    int                 mCameraId;
    bool                mIsStillZSL;
    bool                mIsTakeStillZSL;

    OmxUtils            mOmxUtils; /**< OMX utils */
    FrameRateDumper     mFrameRateDumperVF; /**< Frame rate dumper for VF*/
    FrameRateDumper     mFrameRateDumperVideo; /**< Frame rate dumper for Video*/
    FrameRateDumper     mFrameRateDumperOverlay; /**< Frame rate dumper for Overlay*/
    PerfManager         mPerfManager; /**< PerfManager*/
    OmxILExtIndex*      mOmxILExtIndex;  /**< Extension index */
    int mCameraFormatInfoIndex; /**< Index for camera format info */
    bool                mIsFirstInstanceViewFinder; /**< First viewfinder request */
    bool                mIsFirstViewFinderFrame; /**< First viewfinder frame */
    bool                mIsFirstVideoRecordFrame; /**< First video record frame */
    PreviewThreadData      mPreviewThreadData[kTotalPreviewBuffCount]; /**< Preview Overlay thread data */
    ThreadInfo mThreadInfo; /**< ThreadInfo */
    ThreadContainer<SwProcessingThreadData>* mSwProcessingThread; /**< SwProcessing thread */
    ThreadContainer<OmxBuffInfo>* mPreviewThread; /**< Preview thread */
    ThreadContainer<OmxBuffInfo>* mRecordThread; /**< Record thread */
    AutoFocusThreadData*    mAutoFocusThreadData; /**< Autofocus thread data */
    ThreadContainer<AutoFocusThreadData>* mAutoFocusThread; /**< AutoFocus thread */
    ReqHandlerThreadData*    mReqHandlerThreadData; /**< Error thread data */
    ThreadContainer<ReqHandlerThreadData>* mReqHandlerThread; /**< Error thread */
    OmxReqCbHandler mOmxReqCbHandler; /**< OMX Request Callback handler */
    CFrameDump mFrameDumper;
    CFrameReplay mFrameReplay;
    bool mHiResVFEnabled; /**< HiResVF Feature Enabled */
    image_rect_struct mRawImage; /**< Snapshot/Raw Image */
    FocusHandler *mFocusHandler; /**< Generic focus handler */
    AutoFocusHandler *mAutoFocusHandler; /**< Focus handler for single auto focus mode */
    ContinuousAutoFocusHandler *mContinuousAutoFocusHandler; /**< Focus handler for Continuous auto focus mode */
    FixedFocusHandler *mFixedFocusHandler; /**< Focus handler for Fixed focus mode */
    ThumbnailHandler *mThumbnailHandler; /**< Thumbnail handler: Parent class*/
    EnableDataFlow *mEnableDataFlow; /**< Handle thumnail enabled case*/
    DisableDataFlow *mDisableDataFlow; /**< Handle thumnail disabled case*/
    ImageRotHandler *mImageRotHandler; /**< Generic Image Rotation handler */
    DeviceHwRotation *mDeviceHwRotation; /**< Image Rotation handler for Device Hw Rotation (90/270 degree)*/
    DeviceSwRotation *mDeviceSwRotation; /**< Image Rotation handler for Device Sw Rotation (180 degree) */
    // For Video Stabiliztion
    bool      mVideoStabEnabled; /**< Video Stabilization Enabled */
    OMX_COLOR_FORMATTYPE mOmxRecordPixFmt; /**< Pixel format of record path */
#ifdef ENABLE_FACE_DETECTION
    enum faceDetectionAlgoType {
        NEVEN=0,
        STE
    };
    STEFaceDetectorBase *mFaceDetector;
    bool mFaceDetection; /**<true - if face detection enable else false */
#endif
    // For Snapshot During Recording
    bool                mSnapshotDuringRecord;
    int                 mSnapshotDuringRecIdx;
    int                 mRecordIdx;
    MMNativeBuffer*     mRecordMMNativeBuf[kRecordBufferCount];
    pthread_t           mB2R2Thread;
    int                 mSlowMotionFPS;
    int64_t             mVideoTS;
    bool                mEnableB2R2DuringRecord;
   SteHwConv*           mRecordBlitter;
};

#include "STECamera.inl"

}; // namespace android

#endif
