/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 0
#define DBGT_PREFIX "ExtIsp"
#include "STECamTrace.h"

//System includes
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/android_pmem.h>
#include <linux/time.h>
#include <linux/hwmem.h>

//Multimedia includes
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <OMX_Symbian_ExtensionNames_Ste.h>
#include <HardwareAPI.h>
#include <ui/GraphicBufferAllocator.h>
#include <ui/GraphicBuffer.h>

//Internal includes
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STEExifMng.h"
#include "STEJpegEnc.h"
#include "STECameraKeys.h"
#include "STECameraConfig.h"
#include "STEExtIspCamera.h"
#include "STECamMemoryHeapBase.h"
#include "STENativeWindow.h"
#include "STEArmIvProc.h"
#include "STES5K4ECGX.h"
#include "STEExtIspMyNative.h"
#include "ZXImgEnhance.h"

#define MMHWBUFFER_SO_FILE "/system/lib/libste_ensloader.so"

#define NB_OF_FRAME_COUNT 60
#define MAX_EXIF_BUFFER_SIZE  0x40000
#define MMIO_CAM_FLASH_ENABLE  0x111111

//#define DEBUG_EXTRA_PARAMETERS 1

namespace android {

#undef _CNAME_
#define _CNAME_ STEExtIspCamera

STEExtIspCamera::STEExtIspCamera(int cameraId)
                  :
                  MMIO_Camera_fd(-1),  //added by dyron
                    mTimeout(false),
                    mNbConsecutiveTimeout(0),
                    mErrorOnBoot(false),
                    mRecordRunning(false),
                    mPreviewState(EPreviewDisabled),
                    mIsFirstViewFinderFrame(false),
                    mIsStillInProgress(false),
                    mCam(0),
                    mParameters(),
                    mVPB1Heap(0),
                    mPreviewCurrentIndex(0),
                    mOnCancelFocus(false),
                    mNotifyCb(0),
                    mDataCb(0),
                    mDataCbTimestamp(0),
                    mCallbackCookie(0),
                    mMsgEnabled(0),
                    mPreviewDump(false),
                    mRecordDump(false),
                    mRecordReplay(false),
                    mCameraId(cameraId),
                    mCameraWithoutEncoder(true),
                    mOmxILExtIndex(NULL),
                    mJpegEncpict(NULL),
                    mJpegEncThumb(NULL),
                    mPreviewBlitter(NULL),
                    mRecordBlitterPass1(NULL),
                    mRecordBlitterPass2(NULL),
                    mPreviewWindow(NULL),
                    mTotalNumOfNativeBuff(0),
                    mIsRecordHeapSet(false),
                    mTempBuffHandle(NULL),
                    mTempNativeBuffer(NULL),
                    mImgEEMode(IMGEE_MODE_DEFAULT)
#ifdef ENABLE_FACE_DETECTION
                    ,mFaceDetection(false)
                    ,mFaceDetector(NULL)
#endif
{
    DBGT_PROLOG("");


    mLibHandle = dlopen(MMHWBUFFER_SO_FILE, RTLD_NOW);
    if (mLibHandle == NULL)    {
        DBGT_CRITICAL("Could not open handle to the mmhwbuffer library");
    }

    mCreateMMNativeBuffer = (PFMMCreateNativeBuffer)dlsym(mLibHandle, "create_mmnative_buffer");
    mDestroyMMNativeBuffer = (PFMMDestroyNativeBuffer)dlsym(mLibHandle, "destroy_mmnative_buffer");
    if (mCreateMMNativeBuffer == NULL || mDestroyMMNativeBuffer == NULL) {
        DBGT_CRITICAL("Could not get symbols from mmhwbuffer library");
    }

    memset(&mRecordBufferInfo, 0, sizeof(mRecordBufferInfo));
    memset(&mImageBufferInfo, 0, sizeof(mImageBufferInfo));

	/*added by dyron for flash control*/
    char *mmiodevice = (char *) "/dev/mmio_camera";
    MMIO_Camera_fd = open(mmiodevice, O_RDWR);

    // get the omx pixel format on record path
    char pixFmt[32]; DynSetting::get(DynSetting::EExtIspRecordPixFmt, pixFmt);
    mOmxRecordPixFmt = getOmxPixFmtFromKeyStr(pixFmt);
#ifdef ENABLE_FACE_DETECTION
    mfaceMetaData_yuv.number_of_faces = 0;
    mfaceMetaData_yuv.faces = mdemoFace_yuv;
#endif
exit:
    DBGT_EPILOG("");
}

void STEExtIspCamera::MMIO_Camera_flash_control(int val) 
{
        int  ioctl_arg = val; 
	    LOGE("MMIO_Camera_flash_control: %d", val);
	        if (MMIO_Camera_fd>=0)
		            ioctl(MMIO_Camera_fd, MMIO_CAM_FLASH_ENABLE, ioctl_arg);
}

int STEExtIspCamera::setPreviewWindow(struct preview_stream_ops *window)
{
    DBGT_PROLOG();

    DBGT_PTRACE("Preview Native window - %p", window);
    /* Lock the thread */
    Mutex::Autolock lock(mLock);

    // Preview window is not allocated yet,don't do anything and do not return error
    if (NULL == window) {
        if (mPreviewWindow != NULL) {
            mPreviewWindow->deinit();
            delete mPreviewWindow;
            mPreviewWindow = NULL;
        }
        mPreviewWindow = NULL;
        DBGT_PTRACE("return NO_ERROR");
        return NO_ERROR;
    }

    if (mPreviewWindow) {
        // preview window changed
        //@TOCHECK native buffers need to be released????
        mPreviewWindow->deinit();
        delete mPreviewWindow;
        mPreviewWindow = NULL;
    }

    DBGT_ASSERT( (0!=mCam), "OMX_Camera is not yet created");
    mPreviewWindow = new MyNativeWindow(mCreateMMNativeBuffer, mDestroyMMNativeBuffer);
    if (mPreviewWindow == NULL) {
        return (int)NO_MEMORY;
    }

    //check the key preview format
    int halPreviewFmt = HAL_PIXEL_FORMAT_YCbCr_420_SP;

    if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420MB) == 0) {
        halPreviewFmt = HAL_PIXEL_FORMAT_YCBCR42XMBN;
        goto end_fmt;
    }

    if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420SP) == 0) {
        halPreviewFmt = HAL_PIXEL_FORMAT_YCbCr_420_SP;
        goto end_fmt;
    }

    if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_RGB565) == 0) {
        halPreviewFmt = HAL_PIXEL_FORMAT_RGB_565;
        goto end_fmt;
    }
end_fmt:
    int err = mPreviewWindow->init(
              window,
              mRequestMemoryFunc,
              mPreviewWidth,
              mPreviewHeight,
              halPreviewFmt,
              CAMHAL_GRALLOC_USAGE);
    if (OK == err) {
        mIsPreviewHeapSet = true;
    }
    DBGT_EPILOG("");
    return err;
}

void STEExtIspCamera::cleanUp()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err;

    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mCam);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("In Camera destructor GetpOMX_FreeHandle camera passed ");
    } else {
        DBGT_CRITICAL("In Camera destructor GetpOMX_FreeHandle camera failed ");
    }
    mCam = NULL ;

    err = (mOmxUtils.interface()->GetpOMX_Deinit())();
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("In Camera destructor GetpOMX_Deinit  passed ");
    } else {
        DBGT_CRITICAL("In Camera destructor GetpOMX_Deinit failed ");
    }

    sem_destroy(&mStateCam_sem);
    sem_destroy(&mSetPortCam_sem);
    sem_destroy(&mStill_sem);
    sem_destroy(&mFocus_sem);
    sem_destroy(&mPreview_sem);
#ifdef ENABLE_FACE_DETECTION
    if (mFaceDetector) {
        delete mFaceDetector;
        mFaceDetector = NULL;
    }
#endif
    //close perf manager
    mPerfManager.close();

    //delete extension index
    delete mOmxILExtIndex;
    mOmxILExtIndex = NULL;
    DBGT_EPILOG("");
}


STEExtIspCamera::~STEExtIspCamera()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;
    if (mIsRecordHeapSet) {
        DBGT_PTRACE("In Camera cleanup Sending Command OMX_StateLoaded  for Camera ");
        err =OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateLoaded,NULL);
        FreeRecordHeapLocked();
        FreeStillHeapLocked();

        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("In Camera stopPreview Sending CommandLoad passed for Camera");
            camera_sem_wait(& mStateCam_sem, SEM_WAIT_TIMEOUT);
        } else {
            DBGT_CRITICAL(" Error in Sending Camera to Load Error 0x%x  in stopPreview",err);
        }
        mIsRecordHeapSet = false;
    }
    // preview buffers De-allocation
    if (NULL != mPreviewWindow) {
        mPreviewWindow->deinit();
        delete mPreviewWindow;
        mPreviewWindow = NULL;
    }
	/*added by dyron for disable flash*/
	
    if (MMIO_Camera_fd>=0) {
        MMIO_Camera_flash_control(0);
//        close(MMIO_Camera_fd);
    }

    cleanUp();
    if (NULL != mLibHandle) dlclose(mLibHandle);
    DBGT_EPILOG("");
}
void STEExtIspCamera::setCallbacks(camera_notify_callback notify_cb,
                                   camera_data_callback data_cb,
                                   camera_data_timestamp_callback data_cb_timestamp,
                                   camera_request_memory get_memory,
                                   void* user)
{
    Mutex::Autolock lock(mLock);
    mNotifyCb = notify_cb;
    mDataCb = data_cb;
    mDataCbTimestamp = data_cb_timestamp;
    mRequestMemoryFunc = get_memory;
    mCallbackCookie = user;
}

void STEExtIspCamera::enableMsgType(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    mMsgEnabled |= msgType;
}

void STEExtIspCamera::disableMsgType(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    mMsgEnabled &= ~msgType;
}

int STEExtIspCamera::msgTypeEnabled(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    return (mMsgEnabled & msgType);
}


status_t STEExtIspCamera::doStartPreview()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err;

    // Initialise the variable that warn boot is correcly executed
    mErrorOnBoot = false;

    // reset flag of Still capture
    if (mPreviewState == EPreviewEnabled){
        doStopPreview();
    }
    if (EPreviewDisabled == mPreviewState) {
        if (mIsRecordHeapSet == false) {
            // set camera configuration
            cameraConfigSet();
            mIsRecordHeapSet = true;
        } else {
            // configure only the preview path in case of restart
            ReconfigurePreview();
        }

        if (mErrorOnBoot==true) {
            DBGT_CRITICAL("Error during the boot detected");
            return OMX_ErrorUndefined;
        } else {
            DBGT_PTRACE("No error during the boot");
        }

        //check the key preview format
        OMX_COLOR_FORMATTYPE omxPreviewFmt = OMX_COLOR_FormatYUV420SemiPlanar;

        if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420MB) == 0) {
            omxPreviewFmt = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
            goto end_fmt;
        }

        if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420SP) == 0) {
            omxPreviewFmt = OMX_COLOR_FormatYUV420SemiPlanar;
            goto end_fmt;
        }

        if (strcmp( mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_RGB565) == 0) {
            omxPreviewFmt = OMX_COLOR_Format16bitRGB565;
            goto end_fmt;
        }
end_fmt:
        DBGT_PTRACE("Create new preview blitter session in %d %d out %d %d",
            mVPB0Width,
            mVPB0Height,
            mPreviewWidth,
            mPreviewHeight );

        mPreviewBlitter = new SteHwConv(
            0, // rotation
            mVPB0Width,
            mVPB0Height,
            mOmxRecordPixFmt,
            mPreviewWidth,
            mPreviewHeight,
            omxPreviewFmt );

        // set source cropping zone
        mPreviewBlitter->setCropSrc(0, 0, mVPB0Width, mVPB0Height);

        DBGT_PTRACE("Sending the camera an Executing State set command in startPreview");
        err =OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateExecuting,NULL);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SendCommand OMX_StateExecuting passed for camera in startPreview");
            // fill all buffers
            supplyRecordBuffersToCam();
            camera_sem_wait(& mStateCam_sem, SEM_WAIT_TIMEOUT);
        } else {
            DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for camera error 0x%x",err);
        }

        mPreviewState = EPreviewEnabled;

        if (createThread(beginPreviewThread, this) == false) {
            DBGT_CRITICAL("Can't create a new thread for Preview");
            mLock.unlock();
            DBGT_EPILOG("");
            return -1;
        }

        // configure the still path
        ConfigureStill();

        // set parameters for the 1st time
        if ( mbOneShot == OMX_TRUE )
            setFirstFrameParameters(PREVIEW_MODE);

        if ( mbOneShot == OMX_FALSE )
            setFirstFrameParameters(CAMCORDER_MODE);

        setFirstFrameParameters(COMMON_MODE);

        // check if preview frame has to be dump
        char value[PROPERTY_VALUE_MAX]; char path[64];
        property_get("debug.libcamera.preview.dump", value, "0");
        DBGT_PTRACE("debug.libcamera.preview.dump %s", value);

        if (value[0] == '1') {
            // open the preview dump file with default path
            snprintf(path,64,"/data/preview_%s_%dx%d.dump",
                mParameters.getPreviewFormat(),
                mPreviewWidth,
                mPreviewHeight );

            mPreviewFile = fopen(path, "wb");
            if (mPreviewFile) {
                mPreviewDump = true;
            } else {
                DBGT_CRITICAL("can't open preview dump file: path %s",path);
                mPreviewDump = false;
            }
        } else {
            if (value[0] == '0') {
                // don't dump preview frames
                mPreviewDump = false;
            } else {
                // open the preview dump file with specific path
                mPreviewFile = fopen(value, "wb");
                if (mPreviewFile) {
                    mPreviewDump = true;
                } else {
                    DBGT_CRITICAL("can't open preview dump file: path %s",value);
                    mPreviewDump = false;
                }
            }
        }
    }
    mLock.unlock();

    DBGT_EPILOG("");

    return NO_ERROR;
}

status_t STEExtIspCamera::startPreview()
{
    DBGT_PROLOG("");
    status_t status = NO_ERROR;

    Mutex::Autolock lockTakePic(mLockTakePic);
    Mutex::Autolock lock(mLock);

    mPerfManager.logAndDump(PerfManager::EViewFinderRequested);

    // reset boolean which a first has been received
    mIsFirstViewFinderFrame = false;

    int preview_width, preview_height;
    mParameters.getPreviewSize(&preview_width, &preview_height);


#ifndef  STE_SENSOR_MT9P111  //fix no picture data bug after switch from picture preview mode to camera record preview mode
    if (mOldPreviewWidth  != preview_width  || mOldPreviewHeight != preview_height) {
        if (mPreviewState == EPreviewEnabled)
            doStopPreview();

        if (mIsRecordHeapSet == true) {
            OMX_ERRORTYPE err;

            DBGT_PTRACE("startPreview ratio changed OMX_StateLoaded for Camera ");
            err =OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateLoaded,NULL);

            /* preview buffers De-allocation */
            FreeRecordHeapLocked();

            if (err == OMX_ErrorNone) {
                DBGT_PTRACE("In Camera stopPreview Sending CommandLoad passed for Camera");
                sem_wait(& mStateCam_sem);
            } else {
                DBGT_CRITICAL(" Error in Sending Camera to Load Error 0x%x  in stopPreview",err);
            }
            mIsRecordHeapSet = false;
        }
    }

    mOldPreviewWidth  = preview_width;
    mOldPreviewHeight = preview_height;
#endif

    // call of internal function to stop preview
    status = doStartPreview();
#ifdef ENABLE_FACE_DETECTION
    OMX_COLOR_FORMATTYPE omxColorFormat = OMX_COLOR_FormatUnused;
#if(FACE_DETECTION_ALGO_TYPE == NEVEN)
    omxColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
#endif
    //Max input frame size 640x480 for YUV Camera
    status = mFaceDetector->init(
             mPreviewWidth,
             mPreviewHeight,
             MAX_FACES,
             omxColorFormat);
    if(status != 0) {
        DBGT_CRITICAL("face detection init failed %d previewWidth=%d previewHeight=%d", status, mPreviewWidth, mPreviewHeight);
    }
    else {
        //Enable face detection
        mFdLock.lock();
        mFaceDetection = true;
        mFdLock.unlock();
    }
#endif

    mPerfManager.logAndDump(PerfManager::EViewFinderStarted);

    DBGT_EPILOG("");
    return status;
}

void STEExtIspCamera::doStopPreview()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err;
    OMX_STATETYPE state;

    if ((EPreviewEnabled == mPreviewState) || (mTimeout == true)) {

        mPreviewLock.lock();
        mPreviewState = EPreviewBeingDisabled;
        mPreviewLock.unlock();

        // stop dump of preview frames
        if (mPreviewDump) {
            mPreviewDump = false;
            fclose(mPreviewFile);
        }

        OMX_GetState(mCam,&state);
        if (state == OMX_StateExecuting) {
            // Flush all pending buffers to avoid hang up
            DBGT_PTRACE("Sending Flush frames of VPB0 to camera");
            err = OMX_SendCommand(mCam, OMX_CommandFlush, CAM_VPB + 0, NULL);
            if (err == OMX_ErrorNone) {
                DBGT_PTRACE("Flush frames of VPB0 passed");
                camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);
            } else {
                DBGT_CRITICAL("Flush frames of VPB0 failed Error 0x%x ",err);
            }

            DBGT_PTRACE("In Camera StopPreview  Sending Command IDLE to camera");
            err = OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateIdle,NULL);
            if (err == OMX_ErrorNone) {
                DBGT_PTRACE("In Camera stopPreview Sending CommandIDLE passed for Camera");
                camera_sem_wait(& mStateCam_sem, SEM_WAIT_TIMEOUT);
            } else {
                // send a error message to upper layer
                dispatchError(err);
                DBGT_CRITICAL(" Error in Sending Camera to Idle Error 0x%x  in stopPreview",err);
            }
            // unconfigure the still path
            UnconfigureStill();
        }
        mPreviewState = EPreviewDisabled;
        // flush the buffer queue
        while (!mBuffQueue.isEmpty()) {
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer;
            pBuffer = mBuffQueue.remove();
            DBGT_PDEBUG("flush queue buffer %p",pBuffer);
        }

        delete mPreviewBlitter;
        mPreviewBlitter = NULL;
    }

    if (mTimeout == true) {
        if (mIsRecordHeapSet) {
            OMX_ERRORTYPE err;
            OMX_GetState(mCam,&state);
            if (state == OMX_StateIdle) {
                DBGT_PTRACE("In Camera cleanup Sending Command OMX_StateLoaded  for Camera ");
                err =OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateLoaded,NULL);
                // preview buffers De-allocation
                FreeRecordHeapLocked();
                if (err == OMX_ErrorNone) {
                    DBGT_PTRACE("In Camera stopPreview Sending CommandLoad passed for Camera");
                    camera_sem_wait(& mStateCam_sem, SEM_WAIT_TIMEOUT);
                } else {
                    DBGT_CRITICAL(" Error in Sending Camera to Load Error 0x%x  in stopPreview",err);
                }
            } else {
                /* preview buffers De-allocation */
                FreeRecordHeapLocked();
            }
            mIsRecordHeapSet = false;
        }
        mTimeout =false;
    }

    DBGT_EPILOG("");
}

void STEExtIspCamera::stopPreview()
{
    DBGT_PROLOG("");
    // Mutex::Autolock lockTakePic(mLockTakePic);
    Mutex::Autolock lock(mLock);
#ifdef ENABLE_FACE_DETECTION
    mFdLock.lock();
    //Disable face detection since FD is not needed while taking picture
    mFaceDetection = false;
    mFdLock.unlock();
#endif

    mPerfManager.logAndDump(PerfManager::EViewFinderRequested);

    // call of internal function to stop preview
    doStopPreview();

    mPerfManager.logAndDump(PerfManager::EViewFinderStopped);

    DBGT_EPILOG("");
}

int STEExtIspCamera::previewEnabled()
{
    DBGT_PROLOG("");
    DBGT_EPILOG("%d",mPreviewState);
    return (mPreviewState == EPreviewEnabled);
}


status_t STEExtIspCamera::startRecording()
{
    DBGT_PROLOG("");
    Mutex::Autolock lockTakePic(mLockTakePic);

    SetPropTorch(true);

    mRecordRunning = true;
    DBGT_ASSERT(mRecordBlitterPass1 == NULL, "mRecordBlitterPass1 already exists");
    DBGT_ASSERT(mRecordBlitterPass2 == NULL, "mRecordBlitterPass2 already exists");
    mPerfManager.logAndDump(PerfManager::EVideoRequested);

    // check if record frame has to be dump
    char value[PROPERTY_VALUE_MAX]; char path[64];

    mRecordBlitterPass1 = NULL;
    mRecordBlitterPass2 = NULL;

    for (int i =0; i < kRecordBufferCount; i++)   {
        mRecordBufferInfo[i].mOmxBuffersState = 0;
    }

    // get the key rotation
    int rotation = 0;
    if (mParameters.get(CameraParameters::KEY_ROTATION) != NULL) {
        rotation = mParameters.getInt(CameraParameters::KEY_ROTATION);
        DBGT_PTRACE("Rotation:%d",rotation);
    }

#ifdef ENABLE_VIDEO_ROTATION

    if ( rotation==90 || rotation==270) {
        mRecordBlitterPass1 = new SteHwConv(
                rotation,
                mVPB0Width,
                mVPB0Height,
                (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                mRecordHeight,
                mRecordWidth,
                (OMX_COLOR_FORMATTYPE)OMX_COLOR_Format24bitRGB888);

        mRecordBlitterPass2 = new SteHwConv(
                0,
                mRecordHeight,
                mRecordWidth,
                (OMX_COLOR_FORMATTYPE)OMX_COLOR_Format24bitRGB888,
                mRecordHeight,
                mRecordWidth,
                (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar);

    } else {
        if ( rotation==180) {
            mRecordBlitterPass1 = new SteHwConv(
                    rotation,
                    mVPB0Width,
                    mVPB0Height,
                    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                    mRecordWidth,
                    mRecordHeight,
                    (OMX_COLOR_FORMATTYPE)OMX_COLOR_Format24bitRGB888);

            mRecordBlitterPass2 = new SteHwConv(
                    0,
                    mRecordWidth,
                    mRecordHeight,
                    (OMX_COLOR_FORMATTYPE)OMX_COLOR_Format24bitRGB888,
                    mRecordWidth,
                    mRecordHeight,
                    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar);
        }
    }
#endif


    property_get("debug.libcamera.record.dump", value, "0");
    DBGT_PTRACE("debug.libcamera.record.dump %s", value);

    if (value[0] == '1') {
        // open the preview dump file with default path
        snprintf(path,64,"/data/record_%dx%d_YUV420MB.dump",
            mRecordWidth,
            mRecordHeight );

        mRecordFile = fopen(path, "wb");

        if (mRecordFile) {
            mRecordDump = true;
            DBGT_PTRACE("Dump activate on record path %s",path);
        } else {
            mRecordDump = false;
            DBGT_CRITICAL("can't open record dump file: path %s",path);
        }
    } else {
        if (value[0] == '0') {
            // don't dump record frames
            mRecordDump = false;
            DBGT_PTRACE("dump deactivate on record path");
        } else {
            // open the record dump file with specific path
            mRecordFile = fopen(value, "wb");
            if (mRecordFile) {
                mRecordDump = true;
                DBGT_PTRACE("Dump activate on record path");
            } else {
                mRecordDump = false;
                DBGT_CRITICAL("can't open record dump file: path %s",value);
            }
        }
    }

    property_get("debug.libcamera.record.push", value, "0");
    DBGT_PTRACE("debug.libcamera.record.push %s", value);

     if (value[0] == '1') {
        // open the record replay file with default path
        snprintf(path,64,"/data/record_%dx%d_YUV420MB.dump",
            mRecordWidth,
            mRecordHeight );

        mRecordFile = fopen(path, "rb");

        if (mRecordFile) {
            mRecordReplay = true;
            DBGT_PTRACE("Replay activate on record path");
        } else {
            DBGT_CRITICAL("can't open record replay file: path %s",path);
            mRecordReplay = false;
        }
    } else {
        if (value[0] == '0') {
            // don't replay record frames
            mRecordReplay = false;
            DBGT_PTRACE("Replay deactivate on record path");
        } else {
            // open the record replay file with specific path
            mRecordFile = fopen(value, "rb");
            if (mRecordFile) {
                DBGT_PTRACE("Replay activate on record path");
                mRecordReplay = true;
            } else {
                DBGT_CRITICAL("can't open record replay file: path %s",value);
                mRecordReplay = false;
            }
        }
    }

    mPerfManager.logAndDump(PerfManager::EVideoStarted);
    DBGT_EPILOG("");
    return NO_ERROR;
}


void STEExtIspCamera::stopRecording()
{
    DBGT_PROLOG("");
    Mutex::Autolock lockTakePic(mLockTakePic);
    Mutex::Autolock lock(mRecordLock);
    mPerfManager.logAndDump(PerfManager::EVideoRequested);
    mRecordRunning = false;

    SetPropTorch(false);

    // stop dump recording
    if (mRecordDump) {
        mRecordDump = false;
        DBGT_PTRACE("close dump record file" );
        fclose(mRecordFile);
    }

    // stop replay recording
    if (mRecordReplay) {
        mRecordReplay = false;
        DBGT_PTRACE("close replay record file" );
        fclose(mRecordFile);
    }

    if (mRecordBlitterPass1) {
        delete mRecordBlitterPass1;
        mRecordBlitterPass1 = NULL;
    }

    if (mRecordBlitterPass2) {
        delete mRecordBlitterPass2;
        mRecordBlitterPass2=NULL;
    }

    for (int i =0; i < kRecordBufferCount; i++) {
        for ( int retry = 0 ; retry < 7 ; retry++) {
            if (mRecordBufferInfo[i].mOmxBuffersState) {
                usleep(20000);
            } else {
                DBGT_PTRACE("SteExtIspCamera::stopRecording mOmxBuffersState[%d] released", i);
                break;
            }
        }
    }

    for (int i =0; i < kRecordBufferCount; i++)   {
        DBGT_PTRACE("stop recording mOmxBuffersState[%d] = %d", i, mRecordBufferInfo[i].mOmxBuffersState);
        if (mRecordBufferInfo[i].mOmxBuffersState) {
            OMX_FillThisBuffer(mCam,mRecordBufferInfo[i].mOmxBufferHdr);
            mRecordBufferInfo[i].mOmxBuffersState = 0;
            DBGT_CRITICAL("stop recording Force to release buffer with index %d", i);
        }
    }

    mPerfManager.logAndDump(PerfManager::EVideoStopped);
    DBGT_EPILOG("");
}

int STEExtIspCamera::recordingEnabled()
{
    DBGT_PROLOG("");
    Mutex::Autolock lock(mRecordLock);
    DBGT_EPILOG("%d",mRecordRunning);
    return (mRecordRunning == true);
}

void STEExtIspCamera::releaseRecordingFrame(const void* mem)
{

    video_metadata_t *metaData = (video_metadata_t*)mem;
    buffer_handle_t buf = (buffer_handle_t)metaData->handle;

    DBGT_PDEBUG("record buffer handle =%p", buf);

    // Find index
    int index = -1;
    for (int i=0 ;i<kRecordBufferCount; i++) {
        if (buf == mRecordBufferInfo[i].mGraphicBuffer->handle) {
            index=i;
            break;
        }
    }
    if (index == -1) {
        DBGT_CRITICAL("Bad frame index");
        return;
    }

    if (mRecordBufferInfo[index].mOmxBuffersState) {
        mRecordBufferInfo[index].mOmxBuffersState = 0;
        OMX_IN OMX_BUFFERHEADERTYPE* omx_buffer = mRecordBufferInfo[index].mOmxBufferHdr;
        if (omx_buffer != 0) {
            DBGT_PDEBUG("release frame:%d",index);
            // Push the buffer back to the camera
            OMX_FillThisBuffer(mCam,omx_buffer);
        }
    } else {
        DBGT_CRITICAL("Record : This frame has been already release");
    }
}

// ---------------------------------------------------------------------------

status_t STEExtIspCamera::autoFocus()
{
    DBGT_PROLOG("");
    mPerfManager.init(PerfManager::EAutoFocusRequested);
    Mutex::Autolock lock(mLock);

    if ( mCameraId == CAMERA_FACING_BACK ) {
        if (NULL != mParameters.get(CameraParameters::KEY_FOCUS_MODE)) {
            if (strcmp(CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO, mParameters.get(CameraParameters::KEY_FOCUS_MODE))) {
                DBGT_PTRACE("Call touch focus");
                SetPropTouchAF();	// Call SetPropTouchAF() function to calculate focus region and transfer data to OMX_CORE
            }
        }
        SetPropFocus();
    } else {
        DBGT_PTRACE("No focus on secondary camera.");
        dispatchRequest(reqDispatchHdl::focusStatusUnableToReach, NULL, false);
    }

    DBGT_EPILOG("");

    return NO_ERROR;
}

status_t STEExtIspCamera::cancelAutoFocus()
{
    DBGT_PROLOG("");
    Mutex::Autolock lock(mLock);
    mOnCancelFocus = true;
    DBGT_EPILOG("");
    return NO_ERROR;
}

int STEExtIspCamera::beginPictureThread(void *cookie)
{
    STEExtIspCamera *c = (STEExtIspCamera *)cookie;
    return c->pictureThread();
}

bool doZXImgEnhance(unsigned char *inBuf, int w, int h, int mode)
{
    int handle = 0;

    LOGD("doZXImgEnhance: inBuf %p, w %d, h %d, mode %d", inBuf, w, h, mode);

    if (mode != IMGEE_MODE_HDR && mode != IMGEE_MODE_BEAUTY) {
        mode = IMGEE_MODE_AUTO;
    }

    if (ZXImgEnhanceInit(&handle, IMGEE_FORMAT_S1/*YUVNV21*/, mode) != ZXIMGCORE_OK) {
        LOGE("doZXImgEnhance: init failed!");
        goto errquit0;
    }
    if (ZXImgEnhanceProcess(handle, inBuf, w, h) != ZXIMGCORE_OK) {
        LOGE("doZXImgEnhance: process failed!");
        goto errquit1;
    }

    LOGD("doZXImgEnhance: process success!");
    ZXImgEnhanceRelease(handle);
    return true;

errquit1:
    ZXImgEnhanceRelease(handle);
errquit0:
    return false;
}

int STEExtIspCamera::pictureThread()
{
    DBGT_PROLOG("");
    Mutex::Autolock lockTakePic(mLockTakePic);

    OMX_ERRORTYPE err;
    int thumbWidth = 0; int thumbHeight = 0;
    char* jpegBuf = NULL; char* thumbBuf = NULL;
    int jpegSize = 0; int thumbSize = 0;
    bool isThumbnailInterlaced = false;
    int  embeddedThumbWidth    = mPreviewWidth;  // embedded thumbnail width is equal to preview width
    int  embeddedThumbHeight   = mPreviewHeight; // embedded thumbnail height is equal to preview height
    sp<MemoryBase> snapMem = 0;
    sp<MemoryBase> thumbMem = 0;
    char* yuvSnapBuf = NULL;
    char* yuvThumbBuf = NULL;
    int  embedded422IBufSize  = 0;
    camera_memory_t* callbackData = NULL;
    IFM_EXIF_SETTINGS ExifParams;
    OMX_VERSIONTYPE nVersion;
    bool encodeYuvFrame = false;

    if (NULL != mParameters.get( CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH)) {
        thumbWidth = mParameters.getInt( CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
    }
    if (NULL != mParameters.get( CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT)) {
        thumbHeight = mParameters.getInt( CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
    }

    nsecs_t timestamp_Begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

    if (mRecordRunning) {
        // unconfigure the still path
        UnconfigureStill();
        // configure the still path
        ConfigureStill();
    }

    // check if a record session is running
    if ((mRecordRunning)||((mPreviewHeight >= mPictureHeight)&&(mPreviewWidth >= mPictureWidth))) {
        // transform the last record frame to the picture frame
        DBGT_PTRACE("transform record frame to picture frame in %d %d, out %d %d",
            mVPB0Width, mVPB0Height, mPictureWidth, mPictureHeight);
        int frameSize = mVPB0Width*mVPB0Height*3/2; // YUV format expected
        sp<MemoryBase> out_mem = new MemoryBase( mVPB1Heap, 0, frameSize);
        SteHwConv* blitter;
        ssize_t out_offset;
        size_t out_len;
        int index = 0;

        // waiting for yuv frame capture
        mIsStillInProgress = true;
        camera_sem_wait(&mStill_sem, SEM_WAIT_TIMEOUT);

        // get the frame index
        while( (index<kRecordBufferCount)&&(pLastRecordBuffer!=mRecordBufferInfo[index].mOmxBufferHdr) ) {
            index++;
        }
        // check validity of index
        if (index>=kRecordBufferCount) {
            DBGT_CRITICAL("Bad frame index");
            index = 0;
        }

        out_mem->getMemory( (ssize_t*)&out_offset, (size_t*)&out_len );

        blitter = new SteHwConv(
            0, //rotation
            mVPB0Width,
            mVPB0Height,
            (OMX_COLOR_FORMATTYPE)mOmxRecordPixFmt,
            mPictureWidth,
            mPictureHeight,
            (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar);

        // set source cropping zone
        blitter->setCropSrc(0, 0, mVPB0Width, mVPB0Height);

        // set destination cropping zone
        blitter->setCropDst(0, 0, mPictureWidth, mPictureHeight);

        blitter->transform( mRecordBufferInfo[index].mChunkData.nFd, 0,
                            mRecordBufferInfo[index].mChunkData.nChunkSize,
                            mImageBufferInfo.mChunkData.nFd,
                            out_offset, out_len );

        delete blitter;
        blitter = NULL;
        out_mem.clear();
    } else {
        // catch a new frame on VPB1
        // Reset the Jpeg buffer
        memset(mImageBufferInfo.mOmxBufferHdr->pBuffer, 0, mParamPortVPB1.nBufferSize);

        // Push the buffer back to the camera
        err = OMX_FillThisBuffer(mCam, mImageBufferInfo.mOmxBufferHdr);
        if (err<0) {
            DBGT_CRITICAL("OMX_FillThisBuffer mOmxStillBuffer");
        } else {
            DBGT_PTRACE("OMX_FillThisBuffer mOmxStillBuffer Pass");
        }

        OmxUtils::StructContainer<OMX_CONFIG_BOOLEANTYPE> autoPause;
        err = OMX_GetConfig(mCam, OMX_IndexAutoPauseAfterCapture, autoPause.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig OMX_IndexAutoPauseAfterCapture passed for camera");
        } else {
            DBGT_CRITICAL("OMX_GetConfig OMX_IndexAutoPauseAfterCapture Failed");
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_BOOLEANTYPE> extCapturing;
        extCapturing.ptr()->nPortIndex = CAM_VPB+1;
        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ECapturing), extCapturing.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig ECapturing passed for camera");
        } else {
            DBGT_CRITICAL("OMX_GetConfig ECapturing Failed");
        }

        autoPause.ptr()->bEnabled = OMX_FALSE;
        err = OMX_SetConfig(mCam, OMX_IndexAutoPauseAfterCapture, autoPause.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig OMX_IndexAutoPauseAfterCapture passed for camera");
        } else {
            DBGT_CRITICAL("OMX_SetConfig OMX_IndexAutoPauseAfterCapture Failed");
        }

        extCapturing.ptr()->nPortIndex = CAM_VPB+1;
        extCapturing.ptr()->bEnabled = OMX_TRUE;
        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ECapturing), extCapturing.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig ECapturing passed for camera ");
        } else {
            DBGT_CRITICAL("OMX_SetConfig ECapturing Failed");
        }

        // waiting for jpeg capture
        camera_sem_wait(&mStill_sem, SEM_WAIT_TIMEOUT);
    }


    if (mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE) {

        if (mMsgEnabled & CAMERA_MSG_SHUTTER) {
            // use of shutter callback
            DBGT_PTRACE("Sending Shutter Callback");
            mPerfManager.logAndDump(PerfManager::EShutterCallback);
            mNotifyCb(CAMERA_MSG_SHUTTER, 0, 0, mCallbackCookie);
        }

        if ((!mCameraWithoutEncoder)&&(!mRecordRunning)) {
            /* For every Jpeg file recover from a camera with external ISP we call
             * the deinterlacing funtion.
             * The function returns:
             *  - false if the Jpeg don't contains any yuv thumbnail interlaced.
             *  - true  if the jpeg contains yuv thumbnail interlaced
             *    it extracts the yuv stream, converts the pixel format to be
             *    YUV420MB and to store it in the yuvThumbBuf.
             *
             * WARNING!!!!!!
             * Embedded thumbnail size should be necessary equal to the
             * preview size => Camera driver file need to be in line. */

            // Embedded yuv422I thumbnail buffer size
            embedded422IBufSize  = embeddedThumbWidth * embeddedThumbHeight * 2;

            snapMem = new MemoryBase(  mVPB1Heap, mStillFrameSize, embedded422IBufSize);
            thumbMem = new MemoryBase(  mVPB1Heap, mStillFrameSize + embedded422IBufSize, mThumbFrameSize );

            yuvSnapBuf = (char*)mVPB1Heap->base() + mStillFrameSize;
            yuvThumbBuf = (char*)mVPB1Heap->base() + mStillFrameSize + embedded422IBufSize;

            /* Call the de-interlacing function. */
            DBGT_PTRACE("Call of the YUV thumbnail de-interlacing function.");
            Deinterlace DeintlcThumb;
            isThumbnailInterlaced = DeintlcThumb.deinterlacingYuvThumbnailFromJpeg(
                    (char*)mImageBufferInfo.mOmxBufferHdr->pBuffer,
                    (int*)&(mImageBufferInfo.mOmxBufferHdr->nFilledLen),
                    (char*)yuvSnapBuf,
                    embedded422IBufSize,
                    embeddedThumbWidth,
                    embeddedThumbHeight);

            mPerfManager.logAndDump(PerfManager::EJpegEOS);

            jpegBuf = (char*)mImageBufferInfo.mOmxBufferHdr->pBuffer;
            jpegSize = (int)mImageBufferInfo.mOmxBufferHdr->nFilledLen;
            DBGT_PTRACE("Jpeg size %d Bytes.",(int)jpegSize);

            if (isThumbnailInterlaced == true) {
                // Convert Pixel format YUV422I into YUV420MB
                if ( (thumbWidth != 0) && (thumbHeight != 0) && (mJpegEncThumb)) {
                    DBGT_PTRACE("Add thumbnail to jpeg");
                    SteHwConv* blitter;

                    blitter = new SteHwConv(
                            0, // rotation
                            embeddedThumbWidth,
                            embeddedThumbHeight,
                            (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY,
                            thumbWidth,
                            thumbHeight,
                            (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar );

                    blitter->transform( snapMem,
                            thumbMem,
                            mImageBufferInfo.mChunkData.nFd,
                            mImageBufferInfo.mChunkData.nFd );

                    delete blitter;
                    blitter = NULL;

                    DBGT_PTRACE("Thumbnail encode");
                    mJpegEncThumb->encode((char*)yuvThumbBuf,
                            thumbBuf,
                            thumbSize );
                }
            }

        } else {
            snapMem = new MemoryBase(   mVPB1Heap, 0, mStillFrameSize);
            thumbMem = new MemoryBase(  mVPB1Heap, mStillFrameSize, mThumbFrameSize);

            // get logical address of thumbnail buffer & picture buffer
            yuvSnapBuf = (char*)mVPB1Heap->base();
            yuvThumbBuf= (char*)mVPB1Heap->base() + mStillFrameSize;

            if (mMsgEnabled & CAMERA_MSG_RAW_IMAGE) {
                // use of raw callback
                DBGT_PTRACE("Sending Raw Image Callback - RAW");
                callbackData = mRequestMemoryFunc(-1,mStillFrameSize,1,mCallbackCookie);
                memcpy(callbackData->data, yuvSnapBuf, mStillFrameSize);
                mDataCb(CAMERA_MSG_RAW_IMAGE,callbackData, 0, NULL,mCallbackCookie);
                callbackData->release(callbackData);
                callbackData = NULL;
            }

            if (mMsgEnabled & CAMERA_MSG_POSTVIEW_FRAME) {
                // use of raw callback
                DBGT_PTRACE("Sending Raw Image Callback - POSTVIEW");
                callbackData = mRequestMemoryFunc(-1,mStillFrameSize,1,mCallbackCookie);
                memcpy(callbackData->data, yuvSnapBuf, mStillFrameSize);
                mDataCb(CAMERA_MSG_POSTVIEW_FRAME,callbackData, 0, NULL,mCallbackCookie);
                callbackData->release(callbackData);
                callbackData = NULL;
            }

            if (mImgEEMode != IMGEE_MODE_DEFAULT) {
                doZXImgEnhance((unsigned char *)yuvSnapBuf, mPictureWidth, mPictureHeight, mImgEEMode);
            }

            // encode the picture
            mJpegEncpict->encode(
                yuvSnapBuf,
                jpegBuf,
                jpegSize );

            mPerfManager.logAndDump(PerfManager::EJpegEOS);

            if ( (thumbWidth != 0) && (thumbHeight != 0) && (mJpegEncThumb)) {
                DBGT_PTRACE("Add thumbnail to jpeg");
                SteHwConv* blitter;

                blitter = new SteHwConv(
                    0, //rotation
                    mPictureWidth,
                    mPictureHeight,
                    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                    thumbWidth,
                    thumbHeight,
                    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar);

                blitter->transform(     snapMem,
                                        thumbMem,
                                        mImageBufferInfo.mChunkData.nFd,
                                        mImageBufferInfo.mChunkData.nFd );

                delete blitter;
                blitter = NULL;

                DBGT_PTRACE("Thumbnail encode");
                mJpegEncThumb->encode(
                    yuvThumbBuf,
                    thumbBuf,
                    thumbSize );
            }
        }

        //allocate jpeg memory: need to allocate jpeg size + exif size
        int outSize = jpegSize + thumbSize + 4*1024;
        callbackData = mRequestMemoryFunc(-1,outSize,1,mCallbackCookie);

        DBGT_PTRACE("Add exif header");

        // Add exif to jpeg
#ifndef ENABLE_EXIF_MIXER //Using JHEAD
        SteExifMng* ExifMng = new SteExifMng( );

        ExifMng->UpdateExifField(
            mParameters,
            jpegBuf,
            jpegSize,
            (char*)callbackData->data,
            outSize,
            thumbBuf,
            thumbSize,
            mCameraId);
#else //Using EXIF_MIXER
            char* ExifMixerTempOutBuf = NULL;
            SteExifMng* ExifMng = new SteExifMng(
                    (char*)thumbBuf,
                    thumbSize,
                    thumbWidth,
                    thumbHeight,
                    jpegBuf,
                    jpegSize,
                    mPictureWidth,
                    mPictureHeight,
                    mParameters,
                    ExifMixerTempOutBuf,
                    &jpegSize);

            // recopy jpeg from omx to android
            memcpy(callbackData->data, (char*)ExifMixerTempOutBuf, jpegSize );
#endif

        delete ExifMng;
        ExifMng = NULL;


        // free raw memories
        snapMem.clear();
        thumbMem.clear();

        DBGT_PTRACE("SENDING COMPRESSED IMAGE Event");
        nsecs_t timestamp_End = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        DBGT_PINFO("take a shot time: %llu ms size %d", (timestamp_End-timestamp_Begin)/1000000, jpegSize);
        mDataCb(CAMERA_MSG_COMPRESSED_IMAGE,callbackData, 0, NULL,mCallbackCookie);
        callbackData->release(callbackData);
        callbackData = NULL;
    }

    DBGT_EPILOG("");
    return NO_ERROR;
}

status_t STEExtIspCamera::takePicture()
{
    DBGT_PROLOG("");
    mPerfManager.init(PerfManager::ETakePictureRequested);

	if( strcmp(mParameters.get(CameraParameters::KEY_FLASH_MODE),
												CameraParameters::FLASH_MODE_ON ) == 0 )
	{	 
		MMIO_Camera_flash_control(3);
		 LOGE("takePicture  +++++++++++SetPropFocus+++++++++++open++takepicture");
	}



    if (createThread(beginPictureThread, this) == false)
        return -1;
    DBGT_EPILOG("");
    return NO_ERROR;
}

status_t STEExtIspCamera::cancelPicture()
{
    DBGT_PROLOG("");
    DBGT_EPILOG("");
    return NO_ERROR;
}

status_t STEExtIspCamera::dump(int fd) const
{
    DBGT_PROLOG("Fd: %d", fd);

    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    AutoMutex lock(&mLock);

    write(fd, result.string(), result.size());

    DBGT_EPILOG("");

    return NO_ERROR;
}

int STEExtIspCamera::setParameters(const char* parameters)
{
    DBGT_PROLOG("");
    CameraParameters params;

    String8 str_params(parameters);
    params.unflatten(str_params);

    DBGT_EPILOG("");
    return setParameters(params);
}

status_t STEExtIspCamera::setParameters(const CameraParameters& params)
{
    DBGT_PROLOG("");
    Mutex::Autolock lockTakePic(mLockTakePic);
    AutoMutex lock(&mLock);

    const char *imgeemode = params.get("imgeemode");
    LOGD("SteExtIspCamera::setParameters imgeemode is %s", (imgeemode == NULL ? "NULL" : imgeemode));
    if (imgeemode == NULL) {
        mImgEEMode = IMGEE_MODE_DEFAULT; 
    } else if (!strcmp(imgeemode, "auto")) {
        mImgEEMode = IMGEE_MODE_AUTO;
    } else if (!strcmp(imgeemode, "hdr")) {
        mImgEEMode = IMGEE_MODE_HDR;
    } else if (!strcmp(imgeemode, "beauty")) {
        mImgEEMode = IMGEE_MODE_BEAUTY;
    } else {
        mImgEEMode = IMGEE_MODE_DEFAULT;
    }

    if (mCam) {
        if (mParameters.getInt(KEY_CONTRAST)!=params.getInt(KEY_CONTRAST)) {
            mParameters.set (KEY_CONTRAST, params.get (KEY_CONTRAST));
            if (NO_ERROR !=  SetPropContrast()) {
                DBGT_CRITICAL("SetPropContrast failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if (mParameters.getInt(KEY_BRIGHTNESS)!=params.getInt(KEY_BRIGHTNESS)) {
            mParameters.set (KEY_BRIGHTNESS, params.get (KEY_BRIGHTNESS));
            if (NO_ERROR !=  SetPropBrightness()) {
                DBGT_CRITICAL("SetPropBrightness failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if (mParameters.getInt(KEY_SATURATION)!=params.getInt(KEY_SATURATION)) {
            mParameters.set (KEY_SATURATION, params.get (KEY_SATURATION));
            if (NO_ERROR !=  SetPropSaturation()) {
                DBGT_CRITICAL("SetPropSaturation failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_WHITE_BALANCE),
                params.get(CameraParameters::KEY_WHITE_BALANCE) ) != 0 ) {
            mParameters.set ( CameraParameters::KEY_WHITE_BALANCE,
                params.get (CameraParameters::KEY_WHITE_BALANCE));
            if (NO_ERROR !=  SetPropWhiteBalance()) {
                DBGT_CRITICAL("SetPropWhiteBalance failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_ANTIBANDING),
                params.get(CameraParameters::KEY_ANTIBANDING) ) != 0 ) {
            mParameters.set (CameraParameters::KEY_ANTIBANDING,
                params.get (CameraParameters::KEY_ANTIBANDING));
            if (NO_ERROR !=  SetPropFlickerRemoval()) {
                DBGT_CRITICAL("SetPropFlickerRemoval failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_SCENE_MODE),
                params.get(CameraParameters::KEY_SCENE_MODE) ) != 0 ) {
            mParameters.set (CameraParameters::KEY_SCENE_MODE,
                params.get (CameraParameters::KEY_SCENE_MODE));
            if (NO_ERROR !=  SetPropSceneModes()) {
                DBGT_CRITICAL("SetPropSceneModes failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ((mParameters.get(CameraParameters::KEY_FOCAL_LENGTH)) == NULL
                || ( strcmp(mParameters.get(CameraParameters::KEY_FOCAL_LENGTH),
                        params.get(CameraParameters::KEY_FOCAL_LENGTH)) !=0)) {
            mParameters.set (CameraParameters::KEY_FOCAL_LENGTH, params.get (CameraParameters::KEY_FOCAL_LENGTH));
            if (OMX_ErrorNone != setLensParameters()) {
                DBGT_CRITICAL("SetLensParameters failed");
                DBGT_EPILOG("");
                return -1;
            }
        }
        if ( strcmp(mParameters.get(CameraParameters::KEY_EFFECT),
                params.get(CameraParameters::KEY_EFFECT) ) != 0 ) {
            mParameters.set (CameraParameters::KEY_EFFECT,
                params.get (CameraParameters::KEY_EFFECT));
            if (NO_ERROR !=  SetPropEffect()) {
                DBGT_CRITICAL("SetPropEffect failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( (strcmp(mParameters.get(KEY_ISO),
            params.get(KEY_ISO) ) != 0 )||
            (strcmp(mParameters.get(CameraParameters::KEY_EXPOSURE_COMPENSATION),
            params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION) ) != 0 )||
            (strcmp(mParameters.get(KEY_METERING_EXPOSURE),
            params.get(KEY_METERING_EXPOSURE) ) != 0 )
        ) {
            mParameters.set (CameraParameters::KEY_EXPOSURE_COMPENSATION,
                params.get (CameraParameters::KEY_EXPOSURE_COMPENSATION));
            mParameters.set (KEY_METERING_EXPOSURE,
                params.get (KEY_METERING_EXPOSURE));
            mParameters.set (KEY_ISO,
                params.get (KEY_ISO));
            if (NO_ERROR !=  SetPropExposure()) {
                DBGT_CRITICAL("SetPropExposure failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if (mParameters.getInt(KEY_SHARPNESS)!=params.getInt(KEY_SHARPNESS)) {
            mParameters.set (KEY_SHARPNESS, params.get (KEY_SHARPNESS));
            if (NO_ERROR !=  SetPropSharpness()) {
                DBGT_CRITICAL("SetPropSharpness failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_ZOOM),
                params.get(CameraParameters::KEY_ZOOM) ) != 0 ) {
            // Take the backup of the zoom Key
            int zoom = mParameters.getInt(CameraParameters::KEY_ZOOM);
            mParameters.set (CameraParameters::KEY_ZOOM,
                params.get (CameraParameters::KEY_ZOOM));
            if (NO_ERROR !=  SetPropZoom()) {
                //Set the old zoom key in case it fails
                mParameters.set(CameraParameters::KEY_ZOOM, zoom);
                DBGT_CRITICAL("SetPropZoom failed");
                DBGT_EPILOG("");
                return -1;
            }
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_FLASH_MODE),
                    params.get(CameraParameters::KEY_FLASH_MODE) ) != 0 ) {
            if (NO_ERROR != SetPropFlash(params)) {
                DBGT_CRITICAL("SetPropFlash failed");
                DBGT_EPILOG("");
                return -1;
            }
            mParameters.set (CameraParameters::KEY_FLASH_MODE,
                    params.get (CameraParameters::KEY_FLASH_MODE));
        }

        if ( strcmp(mParameters.get(CameraParameters::KEY_FOCUS_MODE),
                params.get(CameraParameters::KEY_FOCUS_MODE) ) != 0 ) {
            if (NO_ERROR !=  SetPropFocusRange(params)) {
                DBGT_CRITICAL("SetPropFocusRange failed");
                DBGT_EPILOG("");
                return -1;
            }
            mParameters.set (CameraParameters::KEY_FOCUS_MODE,
                    params.get (CameraParameters::KEY_FOCUS_MODE));
        }

        if (!strcmp(params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED),"true")) {
            if ( strcmp(mParameters.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK),
                        params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK) ) != 0 ) {
                mParameters.set (CameraParameters::KEY_AUTO_EXPOSURE_LOCK,
                        params.get (CameraParameters::KEY_AUTO_EXPOSURE_LOCK));
                if (NO_ERROR !=  SetPropAELock()) {
                    DBGT_CRITICAL("SetPropAELock failed");
                    DBGT_EPILOG("");
                    return -1;
                }
            }

        }

        if (!strcmp(params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED),"true")) {
            if ( strcmp(mParameters.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK),
                        params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK) ) != 0 ) {
                mParameters.set (CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,
                        params.get (CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK));
                if (NO_ERROR !=  SetPropAWBLock()) {
                    DBGT_CRITICAL("SetPropExposureLock failed");
                    DBGT_EPILOG("");
                    return -1;
                }
            }
        }
        //Validate fps range
        int min_fpsRange = 0, max_fpsRange = 0,framerate;
        params.getPreviewFpsRange(&min_fpsRange, &max_fpsRange);

        if ((min_fpsRange < 0) || (max_fpsRange < 0)
            || (min_fpsRange > max_fpsRange)
            || (min_fpsRange < SUPPORTED_MIN_FPS_RANGE)
            || (max_fpsRange > SUPPORTED_MAX_FPS_RANGE)) {
            DBGT_CRITICAL("Invalid fps range min,max %d,%d ",min_fpsRange,max_fpsRange);
            DBGT_EPILOG("");
            return -1;
        }

        if (mCam) {
            if ((strcmp(mParameters.get(CameraParameters::KEY_PICTURE_SIZE),
                params.get (CameraParameters::KEY_PICTURE_SIZE) )!= 0 ) |
                (strcmp(mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH),
                params.get (CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH) )!= 0 ) |
                (strcmp(mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT),
                params.get (CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT) )!= 0 ) |
                (strcmp(mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY),
                params.get (CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY) )!= 0)  ) {

                mParameters.set (CameraParameters::KEY_PICTURE_SIZE,
                    params.get (CameraParameters::KEY_PICTURE_SIZE));

                mParameters.set (CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH,
                    params.get (CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH));

                mParameters.set (CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT,
                    params.get (CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT));

                mParameters.set (CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY,
                    params.get (CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY));

            }
        }
    }
    // update the framerate
    DBGT_PTRACE("update the framerate");
    int min_fpsRange = 0,max_fpsRange = 0,framerate = mParameters.getPreviewFrameRate();
    mParameters.getPreviewFpsRange(&min_fpsRange, &max_fpsRange);
    updateFrameRate(framerate, min_fpsRange, max_fpsRange);
    setPreviewFpsRange(min_fpsRange, max_fpsRange);

    // get picture , record & preview size
    int previewWidth = 0; int previewHeight = 0;
    int pictureWidth = 0; int pictureHeight = 0;
    params.getPreviewSize(&previewWidth, &previewHeight);
    params.getPictureSize(&pictureWidth, &pictureHeight);
    params.getPreviewSize(&mRecordWidth, &mRecordHeight);

    if ((previewWidth <= 0 ) || (previewHeight <= 0)) {
        DBGT_PTRACE("Invalid Preview Size");
        DBGT_EPILOG("");
        return -1;
    }
    if ((pictureWidth <= 0 ) || (pictureHeight <= 0)) {
        DBGT_PTRACE("Invalid Picture Size");
        DBGT_EPILOG("");
        return -1;
    }
    if ((mRecordWidth <= 0 ) || (mRecordHeight <= 0)) {
        DBGT_PTRACE("Invalid Record Size");
        DBGT_EPILOG("");
        return -1;
    }

    if (EPreviewEnabled == mPreviewState) {
        if ((pictureWidth !=  mPictureWidth) || (pictureHeight != mPictureHeight)) {

            // unconfigure the still path
            UnconfigureStill();
            // configure the still path
            ConfigureStill();
        }
    }

    mParameters = params;
    // set video format
    char pixFmt[32];getKeyStrFromOmxPixFmt( pixFmt, mOmxRecordPixFmt );
    mParameters.set(CameraParameters::KEY_VIDEO_FRAME_FORMAT, pixFmt);
    // limit the preview resolution
    //  to reduce the use of memory bandwith memory allocation
    if ( previewWidth*previewHeight > LIMIT_PREVIEW_WIDTH*LIMIT_PREVIEW_HEIGHT ) {
        previewWidth = LIMIT_PREVIEW_WIDTH;
        previewHeight = (previewWidth*mRecordHeight)/mRecordWidth;

        AlignPow2<int>::up(mPreviewWidth,2);
        AlignPow2<int>::up(mPreviewHeight,2);

        DBGT_PTRACE("Limit the preview frame resolution (%d*%d)",
            previewWidth, mPreviewHeight );
    }

    if ( previewWidth*previewHeight > mRecordWidth*mRecordHeight ) {
        mVPB0Width = previewWidth;
        mVPB0Height = previewHeight;
    } else {
        mVPB0Width = mRecordWidth;
        mVPB0Height = mRecordHeight;
    }

    if (previewEnabled() != true) {
        // update preview size
        mPreviewWidth = previewWidth;
        mPreviewHeight = previewHeight;
        // update picture size
        mPictureWidth = pictureWidth;
        mPictureHeight = pictureHeight;
    } else {
        if ( mPreviewWidth*mPreviewHeight != previewWidth*previewHeight ) {
            DBGT_PTRACE("reconfigure all paths");
            // update preview size
            mPreviewWidth = previewWidth;
            mPreviewHeight = previewHeight;
            // update picture size
            mPictureWidth = pictureWidth;
            mPictureHeight = pictureHeight;
        }
        if (mPictureWidth*mPictureHeight != pictureWidth*pictureHeight) {
            DBGT_PTRACE("reconfigure still path");
            // update picture size
            mPictureWidth = pictureWidth;
            mPictureHeight = pictureHeight;
        }
    }

    framerate = mParameters.getPreviewFrameRate();

    if (!(isFrameRateSupported(mParameters.get(
          CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES), framerate))) {
        DBGT_PTRACE("Invalid Frame Rate");
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_PTRACE("Preview Width: %d Height: %d Framerate: %d Format : %s",
        mPreviewWidth, mPreviewHeight,framerate,mParameters.getPreviewFormat() );
    DBGT_PTRACE("Record Width: %d Height: %d", mRecordWidth, mRecordHeight);
    DBGT_PTRACE("VPB0 Width: %d Height: %d", mVPB0Width, mVPB0Height);

    // set the stride & slice of record path
    if (mIsRecordHeapSet == false){
        mParameters.set(CameraParameters::KEY_RECORD_STRIDE, mRecordWidth*3/2);
        mParameters.set(CameraParameters::KEY_RECORD_SLICE_HEIGHT, mRecordHeight);
    } else {
        mParameters.set(CameraParameters::KEY_RECORD_STRIDE, (&(mParamPortVPB0.format.video))->nStride);
        mParameters.set(CameraParameters::KEY_RECORD_SLICE_HEIGHT, (&(mParamPortVPB0.format.video))->nSliceHeight);
    }

    if (NULL != mParameters.get(CameraParameters::KEY_JPEG_QUALITY) ) {
        DBGT_PTRACE("Picture Width: %d Height: %d Format : %s Quality: %d",
            mPictureWidth, mPictureHeight, mParameters.getPictureFormat(),
            mParameters.getInt(CameraParameters::KEY_JPEG_QUALITY));
    }

    DBGT_PTRACE("Thumbnail Width: %s Height: %s Quality: %s",
            mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH),
            mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT),
            mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY));

    DBGT_PTRACE("Rotation: %s WhiteBalance: %s Effect: %s Antibanding: %s",
            mParameters.get(CameraParameters::KEY_ROTATION),
            mParameters.get(CameraParameters::KEY_WHITE_BALANCE),
            mParameters.get(CameraParameters::KEY_EFFECT),
            mParameters.get(CameraParameters::KEY_ANTIBANDING));

    DBGT_PTRACE("SceneMode: %s Flash: %s Focus: %s",
            mParameters.get(CameraParameters::KEY_SCENE_MODE),
            mParameters.get(CameraParameters::KEY_FLASH_MODE),
            mParameters.get(CameraParameters::KEY_FOCUS_MODE));

    if ( (NULL != mParameters.get(CameraParameters::KEY_ZOOM) ) &&
        (NULL != mParameters.get(CameraParameters::KEY_MAX_ZOOM)) &&
        (NULL != mParameters.get(CameraParameters::KEY_ZOOM_RATIOS)) ) {

         DBGT_PTRACE( "Zoom Parameters: KEY_ZOOM = %s KEY_MAX_ZOOM = %s KEY_ZOOM_RATIOS: %s",
                 mParameters.get(CameraParameters::KEY_ZOOM),
                 mParameters.get(CameraParameters::KEY_MAX_ZOOM),
                 mParameters.get(CameraParameters::KEY_ZOOM_RATIOS));
    }

    if ( (strcmp(mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420MB) != 0)&&
        (strcmp(mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420SP) != 0)&&
        (strcmp(mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_YUV420P) != 0)&&
        (strcmp(mParameters.getPreviewFormat(), CameraParameters::PIXEL_FORMAT_RGB565) != 0) ) {
        DBGT_CRITICAL("Only yuv & rgb565 preview are supported");
        DBGT_EPILOG("");
        return -1;
    }

    if (strcmp(mParameters.getPictureFormat(), CameraParameters::PIXEL_FORMAT_JPEG) != 0) {
        DBGT_CRITICAL("Only jpeg still pictures are supported");
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");

    return NO_ERROR;
}

char* STEExtIspCamera::getParameters()
{
    DBGT_PROLOG("");
    String8 params_str8;
    char* params_string;
    if (NO_ERROR != (int)getLensParameters()) {
        DBGT_CRITICAL("getLensParameters failed");
    }
    params_str8 = mParameters.flatten();

    // camera service frees this string...
    params_string = (char*) malloc(sizeof(char) * (params_str8.length()+1));
    strcpy(params_string, params_str8.string());
    DBGT_EPILOG("");
    return params_string;
}

void STEExtIspCamera::putParameters(char *parms)
{
    DBGT_PROLOG("");
    if (NULL != parms) free(parms);
    DBGT_EPILOG("");
}

status_t STEExtIspCamera::sendCommand(int32_t command, int32_t arg1, int32_t arg2)
{
    status_t status = NO_ERROR;
    DBGT_PROLOG("");

    switch(command) {
#ifdef ENABLE_FACE_DETECTION
    case CAMERA_CMD_START_FACE_DETECTION:
        {
            DBGT_PTRACE("CAMERA_CMD_START_FACE_DETECTION received");
            if (arg1) {
                DBGT_PTRACE("CAMERA_FACE_DETECTION_SW");
            } else {
                DBGT_PTRACE("CAMERA_FACE_DETECTION_HW");
            }

            Mutex::Autolock lock(mFdLock);
            mFaceDetection = true;
        }
        break;

    case CAMERA_CMD_STOP_FACE_DETECTION:
        {
            DBGT_PTRACE("CAMERA_CMD_STOP_FACE_DETECTION received");
            Mutex::Autolock lock(mFdLock);
            mFaceDetection = false;
        }
        break;
#else
    // Below is added for CTS Test Cases
    // in case of ENABLE_FACE_DETECTION is
    // disable
    case CAMERA_CMD_START_FACE_DETECTION:
    case CAMERA_CMD_STOP_FACE_DETECTION:
        status = BAD_VALUE;
        break;
#endif //ENABLE_FACE_DETECTION
    case CAMERA_CMD_START_SMOOTH_ZOOM:
    case CAMERA_CMD_STOP_SMOOTH_ZOOM:
    case CAMERA_CMD_SET_DISPLAY_ORIENTATION:
        //not yet implemented
        status = BAD_VALUE;
        break;
    default:
        status = BAD_VALUE;
        break;
    }

    DBGT_EPILOG("");
    return status;
}

void STEExtIspCamera::release()
{
    // release Camera session
    DBGT_PROLOG("");
    DBGT_EPILOG("");
}
#define STRINGIFY_PARAM(aParam) #aParam
const char* STEExtIspCamera::msgName(int32_t aParam)
{
    const char *retval = NULL;
    switch (aParam) {

            case CAMERA_MSG_ERROR:
            case CAMERA_MSG_SHUTTER:
            case CAMERA_MSG_FOCUS:
            case CAMERA_MSG_ZOOM:
            case CAMERA_MSG_PREVIEW_FRAME:
            case CAMERA_MSG_VIDEO_FRAME:
            case CAMERA_MSG_POSTVIEW_FRAME:
            case CAMERA_MSG_RAW_IMAGE:
            case CAMERA_MSG_COMPRESSED_IMAGE:
            case CAMERA_MSG_ALL_MSGS:
                retval = STRINGIFY_PARAM(aParam);
        default:
            retval = "unknown";
    }
    return retval;
}


}; // namespace android
