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
#define DBGT_LAYER 1
#define DBGT_PREFIX "ExtIsp"

//System includes
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/android_pmem.h>
#include <linux/time.h>

//Multimedia includes
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <OMX_Symbian_ExtensionNames_Ste.h>
#include <IFM_Types.h>
#include <HardwareAPI.h>
#include <ui/GraphicBufferAllocator.h>
#include <ui/GraphicBuffer.h>

//Internal includes
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STEExifMng.h"
#include "STEJpegEnc.h"
#include "STECameraKeys.h"
#include "STECameraConfig.h"
#include "STEExtIspCamera.h"
#include "STECamMemoryHeapBase.h"
#include "STENativeWindow.h"
#include <linux/hwmem.h>
#include "STEArmIvProc.h"
#include "STES5K4ECGX.h"
#include "STEExtIspMyNative.h"

#define MMIO_CAM_FLASH_ENABLE  0x111111

namespace android {

// Define the number of consecutive start camera will be done after a  watchdog occurs.
#define NB_START_CAMERA_RETRY 1

// define several type of hardware error
#define CAMERA_DSP_PANIC 0
#define CAMERA_BOOT_FAILED 1

#define CAM_PROP_DELIMIT ","

OMX_ERRORTYPE omxExtIspCameraEventHandler(  OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
                                            OMX_EVENTTYPE eEvent,OMX_U32 nData1,
                                            OMX_U32 nData2, OMX_PTR pEventData    )
{
    STEExtIspCamera* pCamera = (STEExtIspCamera*) pAppData;

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

    switch (eEvent) {
        case OMX_EventCmdComplete:
        {
            OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;

            DBGT_ASSERT(NULL != pCamera, "Invalid camera handle");

            DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

            if (cmd == OMX_CommandStateSet) {
                OMX_STATETYPE newState = (OMX_STATETYPE) nData2;

                switch (newState) {
                case OMX_StateLoaded:
                    sem_post(&(pCamera->mStateCam_sem));
                    break;
                case OMX_StateIdle:
                    sem_post(&(pCamera->mStateCam_sem));
                    break;
                case OMX_StateExecuting:
                    sem_post(&(pCamera->mStateCam_sem));
                    break;
                case OMX_StatePause:
                    sem_post(&(pCamera->mStateCam_sem));
                    break;
                case OMX_StateInvalid:
                    break;
                default:
                    break;
                }
            } else if (OMX_CommandPortEnable == cmd) {
                sem_post(&(pCamera->mSetPortCam_sem));
            } else if (OMX_CommandPortDisable == cmd) {
                sem_post(&(pCamera->mSetPortCam_sem));
            }  else if ((OMX_CommandFlush == cmd) && (CAM_VPB + 0 == nData2)) {
                sem_post(&(pCamera->mSetPortCam_sem));
            }
            break;
        }
        case OMX_EventError:
        {
            OMX_ERRORTYPE error = (OMX_ERRORTYPE) nData1;

            if (error == OMX_ErrorTimeout) {
                DBGT_WARNING("Camera does not respond. Watchdog timeout occurs!");
                if ((pCamera->mRecordRunning) || (pCamera->mCamMode == true)) {
                    DBGT_CRITICAL("Timeout during a record session > kill camera session ");
                    pCamera->dispatchRequest(reqDispatchHdl::cleanUpDspPanicReq, NULL, false);
                    break;
                }

                pCamera->mTimeout = true;
                pCamera->mNbConsecutiveTimeout++;
                if ( pCamera->mNbConsecutiveTimeout <= NB_START_CAMERA_RETRY) {
                    DBGT_PTRACE("Camera does not respond. ESD start !%d " , pCamera->mNbConsecutiveTimeout);
                    pCamera->dispatchRequest(reqDispatchHdl::restartOnTimeout, NULL, false);
                } else {
                    DBGT_PTRACE("Camera does not respond. Error notify !%d " , pCamera->mNbConsecutiveTimeout);
                    pCamera->dispatchError(CAMERA_ERROR_UNKNOWN);
                }
            } else if (error == OMX_ErrorHardware) {
                // send a error message to upper layer
                pCamera->dispatchError(CAMERA_ERROR_UNKNOWN);

                if (nData2 == CAMERA_DSP_PANIC) {
                    DBGT_WARNING("OMX_ErrorHardware received (DSP Panic)!");
                    // send a request to treat the DSP panic
                    pCamera->dispatchRequest(reqDispatchHdl::cleanUpDspPanicReq, NULL, false);
                }

                if (nData2 == CAMERA_BOOT_FAILED) {
                    DBGT_WARNING("OMX_ErrorHardware received (Boot Error)!");
                    pCamera->mErrorOnBoot = true;
                    sem_post(&(pCamera->mStateCam_sem));
                }
            } else  if (error == OMX_ErrorNone && nData2 == OMX_FocusStatusReached) {
                DBGT_PTRACE("Camera OMX_FocusStatusRequest return: OMX_FocusStatusReached!");
                pCamera->dispatchRequest(reqDispatchHdl::focusStatusReached, NULL, false);
                //sem_post (&pCamera->mFocus_sem);
            } else if (error == OMX_ErrorNone && nData2 == OMX_FocusStatusUnableToReach) {
                DBGT_PTRACE("Camera OMX_FocusStatusRequest return: OMX_FocusStatusUnableToReach!");
                pCamera->dispatchRequest(reqDispatchHdl::focusStatusUnableToReach, NULL, false);
            }else {
                DBGT_CRITICAL("CameraEventHandler err:%x",(int)nData1);
                sem_post(&(pCamera->mSetPortCam_sem));
            }
            break;
        }
        case OMX_EventPortSettingsChanged:
        {
            DBGT_PTRACE("component is reported a port settings change");
            break;
        }
        case OMX_EventBufferFlag:
        {
            DBGT_PTRACE("component has detected an EOS");
            break;
        }
        default:
            DBGT_CRITICAL("Cant Happen");
        }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE omxExtIspCameraFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                            OMX_IN OMX_PTR pAppData,
                                            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer    )
{
    STEExtIspCamera* pCamera = (STEExtIspCamera*) pAppData;

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    OMX_STATETYPE state;
    OMX_GetState(hComponent,&state);

    if (pBuffer->nOutputPortIndex == CAM_VPB + 0 ) {
        DBGT_PDEBUG("omxExtIspCameraFillBufferDone state executing VPB0");
        if (pCamera!=0) {
            if (!pCamera->mIsFirstViewFinderFrame) {
                pCamera->mIsFirstViewFinderFrame = true;
                pCamera->mPerfManager.logAndDump(PerfManager::EFirstFrame);
            }

            // check if a still capture is still running
            if(pCamera->mIsStillInProgress == true){
                pCamera->mIsStillInProgress = false;
                // memorize the last buffer address
                pCamera->pLastRecordBuffer = pBuffer;
                // post the still semaphore
                sem_post(&pCamera->mStill_sem);
            }

            //queue buffer only in preview state enabled
            if (pCamera->mPreviewState == EPreviewEnabled)
                pCamera->mBuffQueue.add(pBuffer);
            sem_post (&pCamera->mPreview_sem);
        }
    }

    if (pBuffer->nOutputPortIndex == CAM_VPB + 1 ) {
        DBGT_PDEBUG("omxExtIspCameraFillBufferDone state executing VPB1");
        pCamera->mPerfManager.logAndDump(PerfManager::ECameraEOS);
        sem_post(&pCamera->mStill_sem);
    }

    return OMX_ErrorNone;
}

#undef _CNAME_
#define _CNAME_ STEExtIspCamera


OMX_ERRORTYPE STEExtIspCamera::init()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    char omxNamePrimary[]="OMX.ST.ExtHSMCamera";
    char omxNameSecondary[]="OMX.ST.ExtHSMCamera2";


    DBGT_PINFO("begin connect");
    //initialze perf manager
    int status = mPerfManager.connect();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("PerfManager connect failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    DBGT_PINFO("end connect");

    //Perf trace to initialize first Camera Instance
    mPerfManager.init(PerfManager::EFirstCameraInstance);
    //initialize omxutils
    err = mOmxUtils.init();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OmxUtils init failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = (mOmxUtils.interface()->GetpOMX_Init())();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_Init failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    int err1 = sem_init(& mStateCam_sem , 0 , 0);
    if (-1 == err1) {
        DBGT_CRITICAL("stateCam sem init failed OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = sem_init(&mSetPortCam_sem , 0 , 0);
    if (-1 == err1) {
        DBGT_CRITICAL("setPortCam init sem failed OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    err1 = sem_init(&mStill_sem , 0 , 0);
    if (-1 == err1) {
        DBGT_CRITICAL("mStill_sem init sem failed OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = sem_init(&mPreview_sem , 0 , 0);
    if (-1 == err1) {
        DBGT_CRITICAL("preview init sem failed OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = sem_init(&mFocus_sem , 0 , 0);
    if (-1 == err1) {
        DBGT_CRITICAL("focus init sem failed OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = mBuffQueue.init(kRecordBufferCount);
    if (NO_ERROR != err1) {
        DBGT_CRITICAL("BuffQueue init failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    camCallback.EmptyBufferDone = 0;
    camCallback.FillBufferDone = omxExtIspCameraFillBufferDone;
    camCallback.EventHandler = omxExtIspCameraEventHandler;

   if ( mCameraId == CAMERA_FACING_BACK ) {
        //initialize default params
        err = initPrimaryDefaultParameters();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("initPrimaryDefaultParameters failed");
            DBGT_EPILOG("");
            return err;
        }

        err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mCam, omxNamePrimary, this, &camCallback);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Camera GetHandle failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    } else {
        //initialize default params
        err = initSecondaryDefaultParameters();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("initSecondaryDefaultParameters failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mCam, omxNameSecondary, this, &camCallback);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Camera GetHandle failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
#ifdef ENABLE_FACE_DETECTION
        mParameters.set(CameraParameters::KEY_MAX_NUM_DETECTED_FACES_HW, "10");
#if (FACE_DETECTION_ALGO_TYPE == NEVEN)
        mFaceDetector = new NevenFaceDetector();
#endif
        if (mFaceDetector == NULL) {
            DBGT_PTRACE("Face detection object creation failed");
        }
#endif
    mOmxILExtIndex = new OmxILExtIndex(mCam);
    if (NULL == mOmxILExtIndex) {
        DBGT_CRITICAL("new failed for OmxILExtIndex - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    //init all indexes
    err = mOmxILExtIndex->init();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OmxILExtIndex init failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

#ifndef  STE_SENSOR_MT9P111 //fix no picture data bug after switch from picture preview mode to camera record preview mode
    mParameters.getPreviewSize( &mOldPreviewWidth, &mOldPreviewHeight);
#endif



    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::initPrimaryDefaultParameters()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    CameraParameters p;

    mIsPreviewHeapSet = false;
    mIsRecordHeapSet = false;
    mIsStillHeapSet = false;
    mIsVPB1PortDisabled = false;

    DBGT_PTRACE( "Default preview Witdh: %d Height: %d framerate %d",
            DEFAULT_PRIMARY_PREVIEW_WIDTH,
            DEFAULT_PRIMARY_PREVIEW_HEIGHT,
            DEFAULT_PRIMARY_PREVIEW_FRAMERATE );

    // set default preview resolution, stride, slice height, framerate & format
    p.setPreviewSize(DEFAULT_PRIMARY_PREVIEW_WIDTH, DEFAULT_PRIMARY_PREVIEW_HEIGHT);

    // set default values for thumbnail
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, DEFAULT_PRIMARY_JPEG_THUMBNAIL_QUALITY);
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, DEFAULT_PRIMARY_THUMBNAIL_HEIGHT);
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, DEFAULT_PRIMARY_THUMBNAIL_WIDTH);

    DBGT_PTRACE("Default preview framerate: %d", DEFAULT_PRIMARY_PREVIEW_FRAMERATE);
    p.setPreviewFrameRate(DEFAULT_PRIMARY_PREVIEW_FRAMERATE);

    // set suported preview resolution, framerates & formats
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES, SUPPORTED_PRIMARY_PREVIEW_SIZES);
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, SUPPORTED_PRIMARY_FRAMERATES);
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, SUPPORTED_PRIMARY_FPS_RANGES);

    // all supported format for preview
    // String8 spf;
    // spf.append(CameraParameters::PIXEL_FORMAT_YUV420SP); spf.append(CAM_PROP_DELIMIT);
    // spf.append(CameraParameters::PIXEL_FORMAT_RGB565);

    // DBGT_PTRACE("SUPPORTED_PREVIEW_FORMATS %s",spf.string());
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS, CameraParameters::PIXEL_FORMAT_YUV420SP);
    p.setPreviewFormat(CameraParameters::PIXEL_FORMAT_YUV420SP);

    // set video format
    // char pixFmt[32]; getKeyStrFromOmxPixFmt( pixFmt, mOmxRecordPixFmt );
    p.set(CameraParameters::KEY_VIDEO_FRAME_FORMAT, CameraParameters::PIXEL_FORMAT_YUV420MB);


    p.set(KEY_SUPPORTED_RECORD_SIZES,SUPPORTED_PRIMARY_RECORD_SIZES);
    p.set(KEY_RECORD_SIZE,DEFAULT_PRIMARY_RECORD_SIZE);

    // activate the feature snapshot during record
    p.set(CameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED, "true");

    DBGT_PTRACE( "Default picture Width: %d Height: %d quality; %d ",
            DEFAULT_PRIMARY_PICTURE_WIDTH,
            DEFAULT_PRIMARY_PICTURE_HEIGHT,
            DEFAULT_PRIMARY_JPEG_QUALITY);

    p.setPictureSize(DEFAULT_PRIMARY_PICTURE_WIDTH, DEFAULT_PRIMARY_PICTURE_HEIGHT);
    p.setPictureFormat(CameraParameters::PIXEL_FORMAT_JPEG);
    p.set(CameraParameters::KEY_JPEG_QUALITY, DEFAULT_PRIMARY_JPEG_QUALITY);
    p.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, SUPPORTED_PRIMARY_PICTURE_SIZES);
    p.set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS, CameraParameters::PIXEL_FORMAT_JPEG);
    p.set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, SUPPORTED_PRIMARY_THUMB_SIZES);

    p.set(CameraParameters::KEY_WHITE_BALANCE,DEFAULT_PRIMARY_WHITE_BALANCE);
    p.set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE,SUPPORTED_PRIMARY_WHITE_BALANCES);

    p.set(KEY_CONTRAST, DEFAULT_PRIMARY_CONTRAST);
    p.set(KEY_MAX_CONTRAST,CONTRAST_PRIMARY_MAX);
    p.set(KEY_MIN_CONTRAST,CONTRAST_PRIMARY_MIN);
    p.set(KEY_CONTRAST_STEP,CONTRAST_PRIMARY_STEP);

    p.set(KEY_BRIGHTNESS, DEFAULT_PRIMARY_BRIGHTNESS);
    p.set(KEY_MAX_BRIGHTNESS,BRIGHTNESS_PRIMARY_MAX);
    p.set(KEY_MIN_BRIGHTNESS,BRIGHTNESS_PRIMARY_MIN);
    p.set(KEY_BRIGHTNESS_STEP,BRIGHTNESS_PRIMARY_STEP);

    p.set(KEY_SHARPNESS, DEFAULT_PRIMARY_SHARPNESS);
    p.set(KEY_MAX_SHARPNESS,SHARPNESS_PRIMARY_MAX);
    p.set(KEY_MIN_SHARPNESS,SHARPNESS_PRIMARY_MIN);
    p.set(KEY_SHARPNESS_STEP,SHARPNESS_PRIMARY_STEP);

    p.set(KEY_SATURATION, DEFAULT_PRIMARY_SATURATION);
    p.set(KEY_MAX_SATURATION,SATURATION_PRIMARY_MAX);
    p.set(KEY_MIN_SATURATION,SATURATION_PRIMARY_MIN);
    p.set(KEY_SATURATION_STEP,SATURATION_PRIMARY_STEP);

    p.set(CameraParameters::KEY_EXPOSURE_COMPENSATION,DEFAULT_PRIMARY_EXPOSURE_COMPENSATION);
    p.set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION,EXPOSURE_PRIMARY_MAX);
    p.set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION,EXPOSURE_PRIMARY_MIN);
    p.set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP,EXPOSURE_PRIMARY_STEP);

    p.set(KEY_ISO, DEFAULT_PRIMARY_ISO);
    p.set(KEY_METERING_EXPOSURE, DEFAULT_PRIMARY_METERING_EXPOSURE);

    p.set(CameraParameters::KEY_FOCUS_MODE,DEFAULT_PRIMARY_FOCUS);
    p.set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES,SUPPORTED_PRIMARY_FOCUS);

    p.set(CameraParameters::KEY_FLASH_MODE,DEFAULT_PRIMARY_FLASH);
    p.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES,SUPPORTED_PRIMARY_FLASH);

#if 0
     // all supported format for preview
    String8 flash_mode;
    flash_mode.append(CameraParameters::FLASH_MODE_OFF);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_ON);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_RED_EYE);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_TORCH);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_AUTO);
    p.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES, flash_mode.string());
#endif
    
    p.set(CameraParameters::KEY_ANTIBANDING,DEFAULT_PRIMARY_ANTIBANDING);
    p.set(CameraParameters::KEY_SUPPORTED_ANTIBANDING,SUPPORTED_PRIMARY_ANTIBANDING);


    p.set(CameraParameters::KEY_SCENE_MODE,DEFAULT_PRIMARY_SCENE_MODE);
    p.set(CameraParameters::KEY_SUPPORTED_SCENE_MODES,SUPPORTED_PRIMARY_SCENE_MODES);

    p.set(CameraParameters::KEY_EFFECT,DEFAULT_PRIMARY_EFFECT);
    p.set(CameraParameters::KEY_SUPPORTED_EFFECTS,SUPPORTED_PRIMARY_EFFECTS);

    p.set(CameraParameters::KEY_FOCAL_LENGTH, "4.85");
    // Add Focus distances
    p.set(CameraParameters::KEY_FOCUS_DISTANCES, "0.95,1.9,Infinity");

    p.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, "15");
    p.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "15");

    p.set(KEY_AUTO_CONTRAST_MODE, DEFAULT_PRIMARY_AUTO_CONTRAST_MODE );
    p.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, DEFAULT_PRIMARY_FPS_RANGE);
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, SUPPORTED_PRIMARY_FPS_RANGES);
    p.set(CameraParameters::KEY_PREVIEW_FRAME_RATE, "30");

    p.set(CameraParameters::KEY_ZOOM, "0");
#ifdef PRIMARY_ENABLE_ZOOM
    p.set(CameraParameters::KEY_MAX_ZOOM, "5");
    p.set(CameraParameters::KEY_ZOOM_RATIOS, "100,150,200,250,300,350");
    p.set(CameraParameters::KEY_ZOOM_SUPPORTED, "true");
#else //PRIMARY_ENABLE_ZOOM
    p.set(CameraParameters::KEY_ZOOM_SUPPORTED, "false");
#endif //PRIMARY_ENABLE_ZOOM

    DBGT_PTRACE("KEY_AUTO_EXPOSURE_LOCK_SUPPORTED is true\n");
    p.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED,"true");
    p.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED,"true");
    p.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK,"false");
    p.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,"false");

#if DEBUG_EXTRA_PARAMETERS
    char value[PROPERTY_VALUE_MAX]="";

    property_get("debug.libcamera.brightness", value, "0");
    DBGT_PTRACE("debug.libcamera.brightness value = %s", value);
    p.set(KEY_BRIGHTNESS,value);

    property_get("debug.libcamera.contrast", value, "0");
    DBGT_PTRACE("debug.libcamera.contrast value = %s", value);
    p.set(KEY_CONTRAST,value);

    property_get("debug.libcamera.saturation", value, "0");
    DBGT_PTRACE("debug.libcamera.saturation value = %s", value);
    p.set(KEY_SATURATION,value);

    property_get("debug.libcamera.iso", value, ISO_AUTO);
    DBGT_PTRACE("debug.libcamera.iso value = %s", value);
    p.set(KEY_ISO,value);

    property_get("debug.libcamera.metering", value, METERING_EXPOSURE_MATRIX);
    DBGT_PTRACE("debug.libcamera.metering value = %s", value);
    p.set(KEY_METERING_EXPOSURE,value);

    property_get("debug.libcamera.sharpness", value, "0");
    DBGT_PTRACE("debug.libcamera.sharpness value = %s", value);
    p.set(KEY_SHARPNESS,value);
#endif

    if (NO_ERROR != setParameters(p) )
       DBGT_PTRACE("setParameters failed %x", OMX_ErrorInsufficientResources);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::initSecondaryDefaultParameters()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    CameraParameters p;

    mIsPreviewHeapSet = false;
    mIsRecordHeapSet = false;
    mIsStillHeapSet = false;
    mIsVPB1PortDisabled = false;

    DBGT_PTRACE( "Default preview Witdh: %d Height: %d framerate %d",
            DEFAULT_SECONDARY_PREVIEW_WIDTH,
            DEFAULT_SECONDARY_PREVIEW_HEIGHT,
            DEFAULT_SECONDARY_PREVIEW_FRAMERATE );

    // set default preview resolution, stride, slice height, framerate & format
    p.setPreviewSize(DEFAULT_SECONDARY_PREVIEW_WIDTH, DEFAULT_SECONDARY_PREVIEW_HEIGHT);
    p.setPreviewFrameRate(DEFAULT_SECONDARY_PREVIEW_FRAMERATE);

    // set default values for thumbnail
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, DEFAULT_SECONDARY_JPEG_THUMBNAIL_QUALITY);
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, DEFAULT_SECONDARY_THUMBNAIL_HEIGHT);
    p.set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, DEFAULT_SECONDARY_THUMBNAIL_WIDTH);

    // set suported preview resolution, framerates & formats
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES, SUPPORTED_SECONDARY_PREVIEW_SIZES);
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, SUPPORTED_SECONDARY_FRAMERATES);
    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, SUPPORTED_SECONDARY_FPS_RANGES);
    p.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, SUPPORTED_SECONDARY_FPS_RANGE);

    p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS, CameraParameters::PIXEL_FORMAT_YUV420SP);

    // // all supported format for preview
    // String8 spf;
    // spf.append(CameraParameters::PIXEL_FORMAT_YUV420SP); spf.append(CAM_PROP_DELIMIT);
    // spf.append(CameraParameters::PIXEL_FORMAT_RGB565);spf.append(CAM_PROP_DELIMIT);
    // spf.append(CameraParameters::PIXEL_FORMAT_YUV420P);
    // DBGT_PTRACE("SUPPORTED_PREVIEW_FORMATS %s",spf.string());
    // p.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS, spf.string());
    p.setPreviewFormat(CameraParameters::PIXEL_FORMAT_YUV420SP);

    // set video format
    // char pixFmt[32];getKeyStrFromOmxPixFmt( pixFmt, mOmxRecordPixFmt );
    p.set(CameraParameters::KEY_VIDEO_FRAME_FORMAT, CameraParameters::PIXEL_FORMAT_YUV420MB);

    // activate the feature snapshot during record
    p.set(CameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED, "true");

    DBGT_PTRACE( "Default picture Width: %d Height: %d quality; %d ",
            DEFAULT_SECONDARY_PICTURE_WIDTH,
            DEFAULT_SECONDARY_PICTURE_HEIGHT,
            DEFAULT_SECONDARY_JPEG_QUALITY);

    p.setPictureSize(DEFAULT_SECONDARY_PICTURE_WIDTH, DEFAULT_SECONDARY_PICTURE_HEIGHT);
    p.setPictureFormat(CameraParameters::PIXEL_FORMAT_JPEG);
    p.set(CameraParameters::KEY_JPEG_QUALITY, DEFAULT_SECONDARY_JPEG_QUALITY);
    p.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, SUPPORTED_SECONDARY_PICTURE_SIZES);
    p.set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS, CameraParameters::PIXEL_FORMAT_JPEG);
    p.set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, SUPPORTED_SECONDARY_THUMB_SIZES);

    p.set(CameraParameters::KEY_WHITE_BALANCE,DEFAULT_SECONDARY_WHITE_BALANCE);
    p.set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE,SUPPORTED_SECONDARY_WHITE_BALANCES);

    p.set(KEY_CONTRAST, DEFAULT_SECONDARY_CONTRAST);
    p.set(KEY_MAX_CONTRAST,CONTRAST_SECONDARY_MAX);
    p.set(KEY_MIN_CONTRAST,CONTRAST_SECONDARY_MIN);
    p.set(KEY_CONTRAST_STEP,CONTRAST_SECONDARY_STEP);

    p.set(KEY_BRIGHTNESS, DEFAULT_SECONDARY_BRIGHTNESS);
    p.set(KEY_MAX_BRIGHTNESS,BRIGHTNESS_SECONDARY_MAX);
    p.set(KEY_MIN_BRIGHTNESS,BRIGHTNESS_SECONDARY_MIN);
    p.set(KEY_BRIGHTNESS_STEP,BRIGHTNESS_SECONDARY_STEP);

    p.set(KEY_SHARPNESS, DEFAULT_SECONDARY_SHARPNESS);
    p.set(KEY_MAX_SHARPNESS,SHARPNESS_SECONDARY_MAX);
    p.set(KEY_MIN_SHARPNESS,SHARPNESS_SECONDARY_MIN);
    p.set(KEY_SHARPNESS_STEP,SHARPNESS_SECONDARY_STEP);

    p.set(KEY_SATURATION, DEFAULT_SECONDARY_SATURATION);
    p.set(KEY_MAX_SATURATION,SATURATION_SECONDARY_MAX);
    p.set(KEY_MIN_SATURATION,SATURATION_SECONDARY_MIN);
    p.set(KEY_SATURATION_STEP,SATURATION_SECONDARY_STEP);

    p.set(CameraParameters::KEY_EXPOSURE_COMPENSATION,DEFAULT_PRIMARY_EXPOSURE_COMPENSATION);
    p.set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION,EXPOSURE_SECONDARY_MAX);
    p.set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION,EXPOSURE_SECONDARY_MIN);
    p.set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP,EXPOSURE_SECONDARY_STEP);

    p.set(KEY_ISO, DEFAULT_SECONDARY_ISO);
    p.set(KEY_METERING_EXPOSURE, DEFAULT_SECONDARY_METERING_EXPOSURE);

    p.set(CameraParameters::KEY_FOCUS_MODE,DEFAULT_SECONDARY_FOCUS);
    p.set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES,SUPPORTED_SECONDARY_FOCUS);

    p.set(CameraParameters::KEY_FLASH_MODE,DEFAULT_SECONDARY_FLASH);
    p.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES,SUPPORTED_SECONDARY_FLASH);

    p.set(CameraParameters::KEY_ANTIBANDING,DEFAULT_SECONDARY_ANTIBANDING);
    p.set(CameraParameters::KEY_SUPPORTED_ANTIBANDING,SUPPORTED_SECONDARY_ANTIBANDING);

    p.set(CameraParameters::KEY_SCENE_MODE,DEFAULT_SECONDARY_SCENE_MODE);
    p.set(CameraParameters::KEY_SUPPORTED_SCENE_MODES,SUPPORTED_SECONDARY_SCENE_MODES);

    p.set(CameraParameters::KEY_EFFECT,DEFAULT_SECONDARY_EFFECT);
    p.set(CameraParameters::KEY_SUPPORTED_EFFECTS,SUPPORTED_SECONDARY_EFFECTS);

    p.set(CameraParameters::KEY_FOCUS_DISTANCES, "0.1,0.5,Infinity");
    p.set(CameraParameters::KEY_MAX_NUM_DETECTED_FACES_HW, "0");
    p.set(CameraParameters::KEY_MAX_NUM_DETECTED_FACES_SW, "0");
    p.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, "54.9");
    p.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "50.0");

    p.set(KEY_AUTO_CONTRAST_MODE, 0 );

    p.set(CameraParameters::KEY_FOCAL_LENGTH, DEFAULT_SECONDARY_FOCAL_LENGTH);
    p.set(CameraParameters::KEY_ZOOM, "0");
#ifdef SECONDARY_ENABLE_ZOOM
    p.set(CameraParameters::KEY_MAX_ZOOM, "6");
    p.set(CameraParameters::KEY_ZOOM_RATIOS, "100,150,200,250,300,350,400");
    p.set(CameraParameters::KEY_ZOOM_SUPPORTED, "true");
#else //SECONDARY_ENABLE_ZOOM
    p.set(CameraParameters::KEY_ZOOM_SUPPORTED, "false");
#endif //SECONDARY_ENABLE_ZOOM

    DBGT_PTRACE("KEY_AUTO_EXPOSURE_LOCK_SUPPORTED is false\n");
    p.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED,"false");
    p.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED,"false");
    p.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK,"false");
    p.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,"false");

#if DEBUG_EXTRA_PARAMETERS
    char value[PROPERTY_VALUE_MAX]="";

    property_get("debug.libcamera.brightness", value, "0");
    DBGT_PTRACE("debug.libcamera.brightness value = %s", value);
    p.set(KEY_BRIGHTNESS,value);

    property_get("debug.libcamera.contrast", value, "0");
    DBGT_PTRACE("debug.libcamera.contrast value = %s", value);
    p.set(KEY_CONTRAST,value);

    property_get("debug.libcamera.saturation", value, "0");
    DBGT_PTRACE("debug.libcamera.saturation value = %s", value);
    p.set(KEY_SATURATION,value);

    property_get("debug.libcamera.iso", value, ISO_AUTO);
    DBGT_PTRACE("debug.libcamera.iso value = %s", value);
    p.set(KEY_ISO,value);

    property_get("debug.libcamera.metering", value, METERING_EXPOSURE_MATRIX);
    DBGT_PTRACE("debug.libcamera.metering value = %s", value);
    p.set(KEY_METERING_EXPOSURE,value);

    property_get("debug.libcamera.sharpness", value, "0");
    DBGT_PTRACE("debug.libcamera.sharpness value = %s", value);
    p.set(KEY_SHARPNESS,value);
#endif

    if (NO_ERROR != setParameters(p) )
       DBGT_PTRACE("setParameters failed %x", OMX_ErrorInsufficientResources);

    DBGT_EPILOG("");
    return err;
}


// ---------------------------------------------------------------------------

void STEExtIspCamera::CameraFillBufferDone(OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    DBGT_PDEBUG("> CameraFillBufferDone");
    int index = 0;
    OMX_ERRORTYPE err;
    NBuffer* buffer = NULL;

    if (mPreviewState == EPreviewEnabled) {
        // reset mNbConsecutiveTimeout variable
        mNbConsecutiveTimeout = 0;

        // get the frame index
        while( (index<kRecordBufferCount)&&(pBuffer!=mRecordBufferInfo[index].mOmxBufferHdr) ) {
            index++;
        }
        // check validity of index
        if (index>=kRecordBufferCount) {
            DBGT_CRITICAL("Bad frame index");
            index = 0;
        }

        mRecordLock.lock();

        if ((mMsgEnabled & CAMERA_MSG_VIDEO_FRAME) && (mRecordRunning == true) && (mRecordBufferInfo[index].mOmxBuffersState == 0)) {
            // set buffer to state write
            mRecordBufferInfo[index].mOmxBuffersState = 1;
            mRecordLock.unlock();

            if (mRecordBlitterPass1) {
                mRecordBlitterPass1->transform( mRecordBufferInfo[index].mChunkData.nFd,
                                                0,
                                                mRecordBufferInfo[index].mChunkData.nChunkSize,
                                                mTempNativeBuffer->getHandle(),
                                                0,
                                                mTempNativeBuffer->getBufferSize());
            }
            if (mRecordBlitterPass2) {
                mRecordBlitterPass2->transform( mTempNativeBuffer->getHandle(),
                                                0,
                                                mTempNativeBuffer->getBufferSize(),
                                                mRecordBufferInfo[index].mChunkData.nFd,
                                                0,
                                                mRecordBufferInfo[index].mChunkData.nChunkSize);
            }

            // check if record frame has to be replay
            if ((mRecordReplay==true) && (mRecordFile!=0)) {
                int size = 0;
                    size = fread((uint8_t *)mRecordBufferInfo[index].mChunkData.nBaseLogicalAddr,
                        (int)mRecordWidth*mRecordHeight*3/2, 1, mRecordFile);
                DBGT_PDEBUG("replay on record path buffer %d %d",index, size);
            }

            // check if record frame has to be dump
            if (mRecordDump) {
                fwrite((uint8_t *)mRecordBufferInfo[index].mChunkData.nBaseLogicalAddr,
                    (int)mRecordBufferInfo[index].mChunkData.nChunkSize, 1, mRecordFile);
            }

            DBGT_PDEBUG("Record push frame index:%d time %llu",index,(nsecs_t)pBuffer->nTimeStamp);
            mDataCbTimestamp((nsecs_t)pBuffer->nTimeStamp, CAMERA_MSG_VIDEO_FRAME, mRecordBufferInfo[index].mClientMemory, 0, mCallbackCookie);
        } else {
            mRecordLock.unlock();

            // Push the buffer back to the camera
            err =OMX_FillThisBuffer(mCam,pBuffer);
            if (err<0) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed:%d",index);
            } else {
                DBGT_PDEBUG("FillThisBuffer Ok");
            }
        }

        if (mPreviewBlitter) {
            // don't blit the preview buffer when the snaphsot is in progress
            if (NULL != mPreviewWindow) {
                buffer = mPreviewWindow->dequeueNBuffer();  //if buffer not avialable then this will be blocked
                if (buffer != NULL) {
                    mPreviewBlitter->transform(
                        mRecordBufferInfo[index].mChunkData.nFd,
                        0,
                        mRecordBufferInfo[index].mChunkData.nChunkSize,
                        buffer->getMMNativeBuffer()->getHandle(),
                        0,
                        buffer->getMMNativeBuffer()->getBufferSize());
                } else {
                    DBGT_CRITICAL("dequeueNativeBuffer failed");
                }
            }
        } else {
            DBGT_CRITICAL("Missing preview blitter");
        }
#ifdef ENABLE_FACE_DETECTION
        if (mMsgEnabled & CAMERA_MSG_PREVIEW_METADATA) {
            if (isFaceDetectionEnable()) {
            if ( (NULL != buffer)&&
                 (buffer->getCameraMemory()->data)&&
                 (buffer->getCameraMemory()->size) ) {
                DBGT_PTRACE("face detection enable");
                int status = mFaceDetector->postInputFrame(
                       (char*)(buffer->getCameraMemory()->data),
                        buffer->getCameraMemory()->size,
                        mPreviewWidth,
                        mPreviewHeight);
                if (status == 0) {
                    mFaceDetector->getFaces(&mfaceMetaData_yuv);
                    mDataCb(CAMERA_MSG_PREVIEW_METADATA, buffer->getCameraMemory(), 0, &mfaceMetaData_yuv,mCallbackCookie);
                }
            }
            } else {
                DBGT_PTRACE("face detection disable");
            }
        }
#endif

        // Notify the client of a new frame.
        if ( mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME) {
            DBGT_PDEBUG("Preview push frame index:%d",index);
            if (NULL != mPreviewWindow) {
                DBGT_CRITICAL("send Preview frame");
                mDataCb(CAMERA_MSG_PREVIEW_FRAME, buffer->getCameraMemory(), 0, NULL,mCallbackCookie);
            }
        }
        if (NULL != buffer) {
            DBGT_PDEBUG("renderNativebuffer to preview window");
            status_t err = mPreviewWindow->renderNBuffer(*buffer);
            if (err != OK) {
                DBGT_CRITICAL("renderNativebuffer failed err %d", (int)err);
            } else if (mPreviewDump) {          // check if preview frame has to be dump
                fwrite((uint8_t*)buffer->getCameraMemory()->data,
                    (int)mPreviewWidth*mPreviewHeight*3/2, 1, mPreviewFile);
            }
        } else {
            DBGT_WARNING("ERROR buffer==NULL");
        }

        if (index==0) {
            DBGT_PDEBUG("Framerate %f time between frames %f s",
                1000000000.0 / (mRecordBufferInfo[0].mOmxBufferHdr->nTimeStamp -
                mRecordBufferInfo[kRecordBufferCount-1].mOmxBufferHdr->nTimeStamp),
                (mRecordBufferInfo[0].mOmxBufferHdr->nTimeStamp -
                mRecordBufferInfo[kRecordBufferCount-1].mOmxBufferHdr->nTimeStamp) / 1000000000.0 );
        } else {
            DBGT_PDEBUG("Framerate %f time between frames %f s",
                1000000000.0 / (mRecordBufferInfo[index].mOmxBufferHdr->nTimeStamp -
                mRecordBufferInfo[index-1].mOmxBufferHdr->nTimeStamp),
                (mRecordBufferInfo[index].mOmxBufferHdr->nTimeStamp -
                mRecordBufferInfo[index-1].mOmxBufferHdr->nTimeStamp) / 1000000000.0 );
        }
    }
    DBGT_PDEBUG("< CameraFillBufferDone");
}

void STEExtIspCamera::cameraConfigSet()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;
    OMX_PARAM_SENSORMODETYPE sensorModeVPB0, sensorModeVPB1;

    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(mParamPortVPB0);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(mParamPortVPB1);
    OmxUtils::StructWrapper<OMX_PARAM_SENSORMODETYPE>::init(sensorModeVPB0);
    OmxUtils::StructWrapper<OMX_PARAM_SENSORMODETYPE>::init(sensorModeVPB1);

    int framerate = mParameters.getPreviewFrameRate();
    int min_fpsRange = 0, max_fpsRange = 0;
    mParameters.getPreviewFpsRange(&min_fpsRange, &max_fpsRange);
    updateFrameRate(framerate, min_fpsRange, max_fpsRange);
    mbOneShot = OMX_TRUE;

    mCamMode = false;

    const char *cmode = mParameters.get(CameraParameters::KEY_RECORDING_HINT);
    if (cmode != NULL) {
        if (strcmp(cmode, "true") == 0) {
            DBGT_PTRACE("cameraConfigSet  cam_mode ");
            mCamMode = true;
            mbOneShot= OMX_FALSE;
        } else if(strcmp(cmode, "false") == 0) {
            DBGT_PTRACE("cameraConfigSet  preview_mode ");
            mbOneShot= OMX_TRUE;
        }
    }

    //0 checking
    DBGT_ASSERT(0 != mPreviewWidth, "Preview width 0");
    DBGT_ASSERT(0 != mPreviewHeight,"Preview height 0");
    DBGT_ASSERT(0 != mPictureWidth, "Picture width 0");
    DBGT_ASSERT(0 != mPictureHeight, "Picture height 0");

    OmxUtils::initialize(mParamPortVPB0, OMX_PortDomainVideo, CAM_VPB+0);
    err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }

    // the index of the input port. Should be modified.
    mParamPortVPB0.nBufferCountActual = kRecordBufferCount;

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE    *pt_video0 = &(mParamPortVPB0.format.video);

    pt_video0->cMIMEType = (OMX_STRING)"";
    pt_video0->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video0->nFrameWidth = (OMX_U32) mVPB0Width;
    pt_video0->nFrameHeight = (OMX_U32)mVPB0Height;
    pt_video0->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_video0->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingUnused;

    // set video format of pipe 0
    pt_video0->eColorFormat = mOmxRecordPixFmt;

    pt_video0->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video0->xFramerate= framerate;
    DBGT_PTRACE("Set xFramerate: %d ",(int)pt_video0->xFramerate);

    err =OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }

    err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }
    DBGT_PTRACE("VPB0 port Size %dx%d stride %d slice height %d framerate %d",
        (int)mVPB0Width,(int)mVPB0Height,(int)mParamPortVPB0.format.video.nStride,
        (int)mParamPortVPB0.format.video.nSliceHeight, (int)framerate);

    //dump
    DBGT_PTRACE("Dumping params for Camera VPB0");
    OmxUtils::dump(mParamPortVPB0);

     sensorModeVPB0.nPortIndex = CAM_VPB+0;
    err =OMX_GetParameter( mCam, OMX_IndexParamCommonSensorMode,  &sensorModeVPB0 );
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamCommonSensorMode port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamCommonSensorMode port 0 Failed");
    }

    sensorModeVPB0.nFrameRate =  framerate;
    sensorModeVPB0.bOneShot = mbOneShot;
    err =OMX_SetParameter( mCam, OMX_IndexParamCommonSensorMode,  &sensorModeVPB0 );
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamCommonSensorMode port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamCommonSensorMode port 0 Failed");
    }

    err = setMakeAndModel();
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("setMakeAndModel passed ");
    } else {
        DBGT_CRITICAL("setMakeAndModel Failed");
    }

    err = SetSoftwareVersion();
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("SetSoftwareVersion passed ");
    } else {
        DBGT_CRITICAL("SetSoftwareVersion Failed");
    }
    if (mIsVPB1PortDisabled == false)
    {
        err =OMX_SendCommand(mCam,OMX_CommandPortDisable,CAM_VPB+1, NULL);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SendCommand OMX_CommandPortDisable Port 1 passed ");
            camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);
        } else {
            DBGT_CRITICAL("OMX_SendCommand OMX_CommandPortDisable Port 1failed ");
        }

    } else {
        DBGT_PTRACE("VPB 1 is already disabled. No need to disable it again.");
    }

    mIsVPB1PortDisabled = true;

    OMX_STATETYPE state;
    OMX_GetState(mCam, &state);
    DBGT_PTRACE("state before sending idle for camera= %s", OmxUtils::name(state));

    err = OMX_SendCommand(mCam,OMX_CommandStateSet,OMX_StateIdle,NULL);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SendCommand OMX_StateIdle passed ");
    } else {
        DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed with error 0x%x ",err);
    }

    AllocateRecordHeapLocked();

    DBGT_PTRACE("Going Waiting for IDLE CAMERA");
    camera_sem_wait(& mStateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle CAMERA");

    DBGT_EPILOG("");
}

void STEExtIspCamera::ConfigureStill()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;
    OMX_PARAM_SENSORMODETYPE sensorModeVPB1;
#ifdef ENABLE_FACE_DETECTION
    mFdLock.lock();
    //Disable face detection since FD is not needed while taking picture
    mFaceDetection = false;
    mFdLock.unlock();
#endif

    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(mParamPortVPB1);
    OmxUtils::StructWrapper<OMX_PARAM_SENSORMODETYPE>::init(sensorModeVPB1);

    int preview_width, preview_height;
    int picture_width ,picture_height;

    mParameters.getPreviewSize(&preview_width, &preview_height);
    mParameters.getPictureSize(&picture_width, &picture_height);
    int framerate = mParameters.getPreviewFrameRate();
    int min_fpsRange = 0, max_fpsRange = 0;

    mParameters.getPreviewFpsRange(&min_fpsRange, &max_fpsRange);
    updateFrameRate(framerate, min_fpsRange, max_fpsRange);

#if 0
    if ( mCameraId == 0 && mPreviewHeight < mPictureHeight && mPreviewWidth < mPictureWidth && !mRecordRunning ) {
        mCameraWithoutEncoder = false;
    } else {
        mCameraWithoutEncoder = true;
    }
#endif

    //0 checking
    DBGT_ASSERT(0 != preview_width, "Preview width 0");
    DBGT_ASSERT(0 != preview_height,"Preview height 0");
    DBGT_ASSERT(0 != picture_width, "Picture width 0");
    DBGT_ASSERT(0 != picture_height, "Picture height 0");

    // create thumbnail
    int thumbQuality = 80;
    int thumbWidth = 640;
    int thumbHeight = 480;
    int rotation = 0;

    if (NULL != mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY)) {
        thumbQuality = mParameters.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY);
    }
    if (NULL != mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT)) {
        thumbHeight = mParameters.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
    }
    if (NULL != mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH)) {
        thumbWidth = mParameters.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
    }
	
    // if( strcmp(mParameters.get(CameraParameters::KEY_FLASH_MODE),
    //                            CameraParameters::FLASH_MODE_ON ) == 0 )
    // {    
    //     LOGE("ConfigStill+++++++++++flash fire");
    //     MMIO_Camera_flash_control(3);
    //     LOGE("ConfigStill+++++++++++fire return");
    // }


#if ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_DEVICE
    // get the key rotation
    if (mParameters.get(CameraParameters::KEY_ROTATION) != NULL) {
        rotation = mParameters.getInt(CameraParameters::KEY_ROTATION);
        DBGT_PTRACE("Rotation:%d",rotation);
    }
#elif ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_EXIF
    err = setOrientation();
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("setOrientation passed");
    } else {
        DBGT_CRITICAL("setOrientation Failed");
    }
#endif

    if (!mCameraWithoutEncoder) {
        // camera contain a jpeg encdoder > initalize VPB1 with jpeg format
        OmxUtils::initialize(mParamPortVPB1, OMX_PortDomainImage, CAM_VPB+1);
        err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }
        if((mParamPortVPB1.format.image.nFrameWidth!=(uint32_t)picture_width) ||
            (mParamPortVPB1.format.image.nFrameHeight!=(uint32_t)picture_height)){
               DBGT_PTRACE("Picture Size is changed call setrpop zoom");
            SetPropZoom();
        }
        mParamPortVPB1.nBufferCountActual = 1;
        OMX_IMAGE_PORTDEFINITIONTYPE    *pt_image1 = &(mParamPortVPB1.format.image);

        pt_image1->cMIMEType = (OMX_STRING)"";
        pt_image1->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
        pt_image1->nFrameWidth = (OMX_U32) picture_width;
        pt_image1->nFrameHeight = (OMX_U32) picture_height;
        pt_image1->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
        pt_image1->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingJPEG;
        pt_image1->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY;
        pt_image1->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

        err =OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }

        err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }

        DBGT_PTRACE( "Capture %dx%d Size:%d",(int)picture_width,(int)picture_height,
                (int)mParamPortVPB1.nBufferSize);

        //dump
        DBGT_PTRACE("Dumping params for Camera VPB1");
        OmxUtils::dump(mParamPortVPB1);
    } else {
        // camera does not contain a jpeg encdoder > initalize VPB1 with YUV format
        OmxUtils::initialize(mParamPortVPB1, OMX_PortDomainImage, CAM_VPB+1);
        err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }

        mParamPortVPB1.nBufferCountActual = 1;

        OMX_IMAGE_PORTDEFINITIONTYPE    *pt_image1 = &(mParamPortVPB1.format.image);

        pt_image1->cMIMEType = (OMX_STRING)"";
        pt_image1->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
        pt_image1->nFrameWidth = (OMX_U32) picture_width;
        pt_image1->nFrameHeight = (OMX_U32) picture_height;
        pt_image1->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
        pt_image1->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
        pt_image1->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
        pt_image1->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

        err =OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }

        err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB1);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 passed ");
        } else {
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 Failed");
        }

        DBGT_PTRACE( "Capture %dx%d Size:%d",(int)picture_width,(int)picture_height,
                (int)mParamPortVPB1.nBufferSize);

        //dump
        DBGT_PTRACE("Dumping params for Camera VPB1");
        OmxUtils::dump(mParamPortVPB1);
    }

    sensorModeVPB1.nPortIndex = CAM_VPB+1;
    sensorModeVPB1.bOneShot = OMX_TRUE;
    err =OMX_GetParameter( mCam, OMX_IndexParamCommonSensorMode,  &sensorModeVPB1 );
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamCommonSensorMode port 1 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamCommonSensorMode port 1 Failed");
    }

    sensorModeVPB1.nFrameRate =  framerate;
    sensorModeVPB1.bOneShot = OMX_TRUE;
    err =OMX_SetParameter( mCam, OMX_IndexParamCommonSensorMode,  &sensorModeVPB1 );
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamCommonSensorMode port 1 passed ");
    } else {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 Failed");
    }

    if (mIsVPB1PortDisabled == true) {
        //Enable VPB1 port
        DBGT_PTRACE("ConfigureStill, Enable VPB1 port");
        err = OMX_SendCommand(mCam,OMX_CommandPortEnable,CAM_VPB+1,NULL);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SendCommand OMX_CommandPortEnable Port 1 passed");
            AllocateStillHeapLocked();
            camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);
        } else {
            DBGT_CRITICAL("OMX_SendCommand OMX_CommandPortEnable Port 1 failed 0x%x " , err);
        }


    } else {
        LOGE("ConfigureStill: VPB 1 is not disabled. Still buffer cannot be allocated.");
    }

    mIsVPB1PortDisabled = false;

    if ((thumbHeight)&&(thumbWidth)) {
        // jpeg encoder session for thumbnail
        DBGT_ASSERT(mJpegEncThumb == NULL, "mJpegEncThumb already exists");
        mJpegEncThumb = new SteJpegEnc(
                thumbWidth,
                thumbHeight,
                thumbQuality,
                mThumbFrameSize,
                OMX_IMAGE_CodingJPEG,
                (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar );
    } else {
        mJpegEncThumb= NULL;
    }

    if (mCameraWithoutEncoder) {
        int quality = 80;
        if (NULL != mParameters.get(CameraParameters::KEY_JPEG_QUALITY)) {
            quality = mParameters.getInt(CameraParameters::KEY_JPEG_QUALITY);
        }
        DBGT_ASSERT(mJpegEncpict == NULL, "mJpegEncpict already exists");
        // camera without jpeg encoder
        mJpegEncpict = new SteJpegEnc(
                picture_width,
                picture_height,
                quality,
                mStillFrameSize,

#ifndef ENABLE_EXIF_MIXER //JHEAD
                OMX_IMAGE_CodingJPEG,
#else //EXIF_MIXER
                OMX_IMAGE_CodingEXIF,
#endif
                (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar );
    }

    DBGT_EPILOG("");
}

void STEExtIspCamera::UnconfigureStill()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;
#ifdef ENABLE_FACE_DETECTION
    //Enable face detection
    mFaceDetection = true;
#endif

    if (mIsVPB1PortDisabled == false)
    {
        //Disable VPB1 port
        DBGT_PTRACE("UnconfigureStill, disable VPB1 port\n");
        err = OMX_SendCommand(mCam,OMX_CommandPortDisable,CAM_VPB+1,NULL);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("In UnconfigureStill OMX_CommandPortDisable Port 1 passed\n");
        } else {
            DBGT_PTRACE("Port Disable mcam failed 0x%x\n",err);
        }

        //Free Jpeg buffer
        FreeStillHeapLocked();

        camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);
    } else {
        DBGT_CRITICAL("UnconfigureStill: VPB 1 is not enabled. Still buffer cannot be freed.");
    }

    mIsVPB1PortDisabled = true;

    if (mJpegEncpict) {
        delete mJpegEncpict;
        mJpegEncpict=NULL;
    }

    if (mJpegEncThumb) {
        delete mJpegEncThumb;
        mJpegEncThumb=NULL;
    }

    DBGT_EPILOG("");
}

void STEExtIspCamera::supplyRecordBuffersToCam()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;

    for (int i=0;i<kRecordBufferCount;i++)    {
        err =OMX_FillThisBuffer(mCam,mRecordBufferInfo[i].mOmxBufferHdr);
        if (err ==OMX_ErrorNone ) {
            DBGT_PTRACE("OMX_FillThisBuffer Done preview Buffer Logical 0x%x  ", (unsigned int) mRecordBufferInfo[i].mOmxBufferHdr->pBuffer);
        } else {
            DBGT_CRITICAL("OMX_FillThisBuffer error: %s", OmxUtils::name(err));
        }
    }

    DBGT_EPILOG("");
}


int STEExtIspCamera::beginPreviewThread(void *cookie)
{
    STEExtIspCamera *c = (STEExtIspCamera *)cookie;
    return c->previewThread();
}

int STEExtIspCamera::previewThread()
{
    DBGT_PROLOG("");
    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer;
    // this Preview is availabel till a stop preview is required
    while(mPreviewState == EPreviewEnabled) {
        camera_sem_wait(&mPreview_sem, SEM_WAIT_TIMEOUT);

        // Check if stop preview has been requested while waiting the
        // semaphore to be released.
        while(!mBuffQueue.isEmpty()) {
            mPreviewLock.lock();
            if (mPreviewState != EPreviewEnabled)   {
                mPreviewLock.unlock();
                break;
            }
            pBuffer = mBuffQueue.remove();
            CameraFillBufferDone(pBuffer);
            mPreviewLock.unlock();
        }
    }
    DBGT_EPILOG("");
    return NO_ERROR;
}

static status_t allocNativeBuffer(
    buffer_handle_t* handle,
    int32_t* stride,
    uint32_t w,
    uint32_t h,
    PixelFormat format = HAL_PIXEL_FORMAT_YCBCR42XMBN,
    int usage = CAMHAL_GRALLOC_USAGE)
{

    GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();
    return GrallocAlloc.alloc(w, h, format, usage, handle, stride);
}

static status_t freeNativeBuffer(
    buffer_handle_t handle)
{
    GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();
    return GrallocAlloc.free(handle);
}

static sp<GraphicBuffer> allocGraphicBuffer(
    native_handle_t& inHandle,
    uint32_t w, uint32_t h,
    uint32_t inStride,
    PixelFormat format = HAL_PIXEL_FORMAT_YCBCR42XMBN,
    int usage = CAMHAL_GRALLOC_USAGE,
    bool keepOwnership = false)
{

    sp<GraphicBuffer> graphicBuffer = new GraphicBuffer(w, h,
                                                        format,
                                                        usage,
                                                        inStride,
                                                        &inHandle,
                                                        keepOwnership);
    return graphicBuffer;
}

status_t STEExtIspCamera::shareBufferWithCamera(buffer_info_t& aBuffer, int aPortIndex)
{
    OMX_PARAM_PORTDEFINITIONTYPE dummyport;
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);

    aBuffer.mOmxBufferHdr = NULL;
    UseAndroidNativeBufferParams params= {
        sizeof(UseAndroidNativeBufferParams),
        dummyport.nVersion,
        aPortIndex,
        (void*) NULL,
        &(aBuffer.mOmxBufferHdr),
        aBuffer.mGraphicBuffer.get()
    };

    OMX_ERRORTYPE err = OMX_SetParameter(mCam,
                                         (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer,
                                         &params);

    DBGT_PTRACE("OMX Hdr = %p buff = %p", aBuffer.mOmxBufferHdr, aBuffer.mOmxBufferHdr->pBuffer);
    DBGT_ASSERT(NULL != aBuffer.mOmxBufferHdr->pBuffer, "Buffer is NULL");

    aBuffer.mChunkData.nSize = sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);
    aBuffer.mChunkData.nVersion = dummyport.nVersion;
    aBuffer.mChunkData.nPortIndex = aPortIndex;

    err = OMX_GetConfig(mCam,
                        (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                        &aBuffer.mChunkData);

    if (OMX_ErrorNone != err) {
        DBGT_WARNING("GetConfig failed in sharednativebuff");
        return INVALID_OPERATION;
    }
    return OK;
}

status_t STEExtIspCamera::setupVideoMetadata(buffer_info_t& aBuffer)
{
    camera_memory_t* clientMemory = mRequestMemoryFunc(-1,
                                               sizeof(video_metadata_t),
                                               1,
                                               mCallbackCookie);
    if (NULL == clientMemory) return NO_MEMORY;

    video_metadata_t* metaData = (video_metadata_t*)clientMemory->data;
    metaData->offset = 0;
    metaData->metadataBufferType = 1;    // KMetadataBufferTypeGrallocSource
    metaData->handle = (void*)aBuffer.mGraphicBuffer->handle;

    aBuffer.mClientMemory = clientMemory;
    return OK;
}



status_t STEExtIspCamera::AllocateRecordHeapLocked()
{
    DBGT_PROLOG("");
    status_t ret = UNKNOWN_ERROR;
    if (NULL != mCam && mIsRecordHeapSet == false) {
        int i,k;

        enableNativeBuffOnOMXComp(mCam, CAM_VPB+0);
        for (int i = 0; i< kRecordBufferCount; i++) {
            buffer_handle_t buf;
            int32_t stride;

            int tempVPB0Width= mVPB0Width;
            int tempVPB0Height= mVPB0Height;
            AlignPow2<int>::up(tempVPB0Width, 16);
            AlignPow2<int>::up(tempVPB0Height, 16);

            ret = allocNativeBuffer(&buf, &stride, tempVPB0Width, tempVPB0Height);
            if (OK != ret) {
                LOGE("Buffer Allocation Failed!!");
                return ret;
            }
            DBGT_PTRACE("Record width %d height %d", tempVPB0Width, tempVPB0Height);

            mRecordBufferInfo[i].mGraphicBuffer.clear();
            mRecordBufferInfo[i].mGraphicBuffer =
                    allocGraphicBuffer(*(native_handle_t*)buf,
                                        tempVPB0Width,
                                        tempVPB0Height,
                                        stride);

            DBGT_ASSERT(mRecordBufferInfo[i].mOmxBufferHdr == NULL, "Previous buffer not released");

            ret = shareBufferWithCamera(mRecordBufferInfo[i], CAM_VPB+0);
            if (OK != ret) {
                LOGE("Buffer sharing with Camera Failed!!");
                return ret;
            }

            ret = setupVideoMetadata(mRecordBufferInfo[i]);
            if (OK != ret) {
                LOGE("No Memory for video metadata!!");
                return ret;
            }

        }

#ifdef ENABLE_VIDEO_ROTATION
        DBGT_ASSERT(NULL == mTempBuffHandle, "Previous buffer not deallocated");

        int stride;
        ret = allocNativeBuffer(&mTempBuffHandle, &stride, mVPB0Width, mVPB0Height,
                                HAL_PIXEL_FORMAT_RGB_888);
        if (OK != ret) {
            LOGE("No Memory for temp video buffer!!");
            return ret;
        }
        DBGT_PTRACE("buf = %p", mTempBuffHandle);

        mTempNativeBuffer = mCreateMMNativeBuffer((void*)mTempBuffHandle);
        mTempNativeBuffer->init();
#endif

        mIsRecordHeapSet = true;
    }
    DBGT_EPILOG("");
    return ret;
}

OMX_ERRORTYPE STEExtIspCamera::enableNativeBuffOnOMXComp
(
    OMX_HANDLETYPE aOmxComponent,
    int aPortIndex
)
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != aOmxComponent, "Invalid handle");
    DBGT_PTRACE("Port index = %d", aPortIndex);

    OMX_PARAM_PORTDEFINITIONTYPE dummyport;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    // Initialize OMX version
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);

    EnableAndroidNativeBuffersParams params = {
        sizeof(EnableAndroidNativeBuffersParams), dummyport.nVersion,
        aPortIndex,(OMX_BOOL)0x1
    };

    err = OMX_SetConfig(aOmxComponent,
                        (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers,
                        &params);

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetConfig enable native buffer failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}



status_t STEExtIspCamera::AllocateStillHeapLocked()
{
    DBGT_PROLOG("");
    status_t ret = OK;

    if (NULL != mCam && mIsStillHeapSet == false) {
        //********************************************************************************
        // allocation of the picture buffer & the snap buffer
        //********************************************************************************
        int thumbWidth = 0;
        int thumbHeight = 0;
        int picture_width = 0;
        int picture_height = 0;

        mParameters.getPictureSize(&picture_width, &picture_height);

        if (NULL != mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT)) {
            thumbHeight = mParameters.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
            // align thumbnail resolution to 16 pixels
            AlignPow2<int>::up(thumbHeight, 16);
        }
        if (NULL != mParameters.get(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH)) {
            thumbWidth = mParameters.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
            // align thumbnail resolution to 16 pixels
            AlignPow2<int>::up(thumbWidth, 16);
        }
        //check buffer size exits
        DBGT_ASSERT(0 != mParamPortVPB1.nBufferSize, "Buffer Size Null");

        mStillFrameSize = mParamPortVPB1.nBufferSize;
        // compute thumbnail size (yuv420MB format)
        mThumbFrameSize = thumbWidth * thumbHeight * 3 / 2;

#ifdef USE_HWMEM
        AlignPow2<int>::up(mStillFrameSize, 4096); //hwmem enforces 4096 alignment
        AlignPow2<int>::up(mThumbFrameSize, 4096); //hwmem enforces 4096 alignment
#else
        AlignPow2<int>::up(mStillFrameSize, 256);
        AlignPow2<int>::up(mThumbFrameSize, 256);
#endif
        DBGT_PTRACE("mStillFrameSize = %d",mStillFrameSize);

        // the allocator will reserve 2 buffers (picture+thumbnail & snapshot+thumbnail)
        //
        //   ****************  ****************  ****************  ****************
        //   *     snap     *  *   thumbnail  *  *     picture  *  *   thumbnail  *
        //   ****************  ****************  ****************  ****************
        int bufferSize = (mStillFrameSize + mThumbFrameSize) * 2;
        DBGT_PTRACE("bufferSize=%d",bufferSize);
        int virtualHeight = bufferSize/picture_width;
        if (0 != bufferSize%thumbWidth) {
            virtualHeight++;
        }

        enableNativeBuffOnOMXComp(mCam, CAM_VPB+1);

        buffer_handle_t buf;
        int stride;
        ret = allocNativeBuffer(&buf, &stride, picture_width, virtualHeight);
        if (OK != ret) {
            LOGE("Image Buffer Allocation Failed!!");
            return ret;
        }
        mImageBufferInfo.mGraphicBuffer.clear();
        mImageBufferInfo.mGraphicBuffer = allocGraphicBuffer(*(native_handle_t*)buf,
                                            thumbWidth,
                                            virtualHeight,
                                            stride);

        DBGT_ASSERT(mImageBufferInfo.mOmxBufferHdr == NULL, "Previous buffer not released");

        ret = shareBufferWithCamera(mImageBufferInfo, CAM_VPB+1);
        if (OK != ret) {
            LOGE("Buffer sharing with Camera Failed!!");
            return ret;
        }

        mVPB1Heap.clear();
        mVPB1Heap = new CamMemoryHeapBase((int)mImageBufferInfo.mChunkData.nFd,
                                          (unsigned int) mImageBufferInfo.mChunkData.nBaseLogicalAddr,
                                          (size_t)mImageBufferInfo.mChunkData.nChunkSize);
    }

    mIsStillHeapSet = true;

    DBGT_EPILOG("");
    return ret;
}

void STEExtIspCamera::FreeRecordHeapLocked()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (mIsRecordHeapSet) {
        for (int i=0; i<kRecordBufferCount; i++) {
            err = OMX_FreeBuffer(mCam,  (OMX_U32)(CAM_VPB+0), mRecordBufferInfo[i].mOmxBufferHdr);// to see the logic

            if (err == OMX_ErrorNone) {
                DBGT_PTRACE("In Camera destructor OMX_FreeBuffer for cam passed ");
            }

            mRecordBufferInfo[i].mOmxBufferHdr = NULL;
            mRecordBufferInfo[i].mClientMemory->release(mRecordBufferInfo[i].mClientMemory);
            mRecordBufferInfo[i].mClientMemory = NULL;
            freeNativeBuffer(mRecordBufferInfo[i].mGraphicBuffer.get()->handle);
        }

        if (mTempNativeBuffer) {
            mDestroyMMNativeBuffer(mTempNativeBuffer);
            mTempNativeBuffer = NULL;

        }
        if (NULL != mTempBuffHandle) {
            GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();
            GrallocAlloc.free(mTempBuffHandle);
            mTempBuffHandle = NULL;
        }
        mIsRecordHeapSet = false;

    }
    DBGT_EPILOG("");
}

void STEExtIspCamera::FreeStillHeapLocked()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if ( mIsStillHeapSet ) {
        err =OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB+1), mImageBufferInfo.mOmxBufferHdr);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_FreeBuffer passed for Jpeg");
            mImageBufferInfo.mOmxBufferHdr = NULL;
        } else {
            DBGT_PTRACE("OMX_FreeBuffer mCam failed 0x%x", err);
        }

        mVPB1Heap.clear();
        freeNativeBuffer(mImageBufferInfo.mGraphicBuffer.get()->handle);
        mIsStillHeapSet = false;
    }

    DBGT_EPILOG("");
}



int STEExtIspCamera::SetPropContrast()
{
    OmxUtils::StructContainer<OMX_CONFIG_CONTRASTTYPE> contrast_Config;
    OMX_ERRORTYPE err = OMX_ErrorNone;
    int wdr = 0;

    wdr = mParameters.getInt(KEY_AUTO_CONTRAST_MODE);

    contrast_Config.ptr()->nPortIndex = OMX_ALL;
    contrast_Config.ptr()->nContrast =  wdr; // 1: auto contrast on , 0 : off

    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonContrast), contrast_Config.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonContrast passed for camera %lu",contrast_Config.ptr()->nContrast);
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonContrast %lu",err,contrast_Config.ptr()->nContrast);
    }

    return err;
}

int STEExtIspCamera::SetPropBrightness()
{
    OmxUtils::StructContainer<OMX_CONFIG_BRIGHTNESSTYPE> brightness_Config;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonBrightness), brightness_Config.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonBrightness passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonBrightness ",err);
    }

    brightness_Config.ptr()->nPortIndex = OMX_ALL;
    brightness_Config.ptr()->nBrightness =  mParameters.getInt(KEY_BRIGHTNESS)+BRIGHTNESS_OFFSET;

    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonBrightness), brightness_Config.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonBrightness passed for camera %lu",brightness_Config.ptr()->nBrightness);
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonBrightness %lu",err,brightness_Config.ptr()->nBrightness);
    }

    return err;
}

int STEExtIspCamera::SetPropSharpness()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_S32TYPE> Sharpness;

    err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness),
        Sharpness.ptr());

    if (err == OMX_ErrorNone) {
        DBGT_PTRACE ("OMX_GetConfig Sharpness passed for camera");
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig Sharpness ",err);
    }

    Sharpness.ptr()->nPortIndex = OMX_ALL;
    Sharpness.ptr()->nValue = (OMX_S32)mParameters.getInt(KEY_SHARPNESS)+SHARPNESS_OFFSET;

    err = OMX_SetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness), &Sharpness);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE ("OMX_SetConfig Sharpness passed for camera %d",(int)Sharpness.ptr()->nValue );
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig Sharpness %d",err,(int)Sharpness.ptr()->nValue );
    }

    return err;
}

int STEExtIspCamera::SetPropSaturation()
{
    OmxUtils::StructContainer<OMX_CONFIG_SATURATIONTYPE>  saturation_Config;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonSaturation), saturation_Config.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonSaturation passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonSaturation ",err);
    }

    saturation_Config.ptr()->nPortIndex = OMX_ALL;
    saturation_Config.ptr()->nSaturation = mParameters.getInt(KEY_SATURATION)+SATURATION_OFFSET;

    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonSaturation), saturation_Config.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonSaturation passed for camera %lu",saturation_Config.ptr()->nSaturation);
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonSaturation %lu",err,saturation_Config.ptr()->nSaturation);
    }

    return err;
}

int STEExtIspCamera::SetPropWhiteBalance()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    wbproperty g_wbprop[]={
        {0,CameraParameters::WHITE_BALANCE_AUTO,OMX_WhiteBalControlAuto},
        {0,CameraParameters::WHITE_BALANCE_DAYLIGHT,OMX_WhiteBalControlSunLight},
        {0,CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT,OMX_WhiteBalControlCloudy},
        {0,CameraParameters::WHITE_BALANCE_FLUORESCENT,OMX_WhiteBalControlFluorescent},
        {1,CameraParameters::WHITE_BALANCE_INCANDESCENT,OMX_WhiteBalControlIncandescent},
    };

    int id=0;
    int max_id = sizeof(g_wbprop) / sizeof(g_wbprop[0]);

    if (NULL != mParameters.get(CameraParameters::KEY_WHITE_BALANCE)) {
        const char *wb = mParameters.get(CameraParameters::KEY_WHITE_BALANCE);

        while(  (g_wbprop[id].iMarkendofdata != 1) &&
                (strcmp(g_wbprop[id].wb, wb)!=0)&&
                (id<max_id) ) {
            id++;
        }

        OmxUtils::StructContainer<OMX_CONFIG_WHITEBALCONTROLTYPE> wbConfig;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonWhiteBalance passed for camera" );
        } else {
            DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonWhiteBalance ",err);
        }

        wbConfig.ptr()->nPortIndex = OMX_ALL;
        wbConfig.ptr()->eWhiteBalControl = g_wbprop[id].eWBType;
        err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonWhiteBalance passed for camera %s %x",wb,g_wbprop[id].eWBType);
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonWhiteBalance %s ",err,wb);
            return -1;
        }
    }
    return err;
}

int STEExtIspCamera::SetPropTouchAF()
{
    DBGT_PTRACE(" in SetPropTouchAF");
    OMX_ERRORTYPE err;
    int previewWidth , previewHeight;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE> focusRegionControl;
    mParameters.getPreviewSize(&previewWidth, &previewHeight);
    OmxUtils::StructContainer<OMX_PARAM_FOCUSSTATUSTYPE> FocusStatus;
    focusRegionControl.ptr()->nPortIndex = OMX_ALL;
    focusRegionControl.ptr()->eFocusRegionControl = OMX_SYMBIAN_FocusRegionManual;
    const char *str = mParameters.get(CameraParameters::KEY_FOCUS_AREAS);
    if ((str != NULL))
       DBGT_PTRACE("Focus type is %s",str);
    else
       DBGT_CRITICAL("NULL String");
    if ((str != NULL)) {
        sscanf(str, "(%d,%d,%d,%d)",
                (int*) &focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nX,
                (int*) &focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nY,
                (int*) &focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nWidth,
                (int*) &focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nHeight
                );
        DBGT_PTRACE("Coordinates for ROI recieved from Application are %d %d %d %d",
                (int) focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nX,
                (int) focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nY,
                (int) focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nWidth,
                (int) focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nHeight
                );
        int x0=((focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nX+1000)*previewWidth)/2000;
        int y0=((focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nY+1000)*previewHeight)/2000;

        int x1=((focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nWidth+1000)*previewWidth)/2000;
        int y1=((focusRegionControl.ptr()->sFocusRegion.sRect.sSize.nHeight+1000)*previewHeight)/2000;

        focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nX=x0+((x1-x0)/2);
        focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nY=y0+((y1-y0)/2);

        DBGT_PTRACE("In HAL UI Mapping:%d %d ",
        (int) focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nX,
        (int) focusRegionControl.ptr()->sFocusRegion.sRect.sTopLeft.nY);
    }
    focusRegionControl.ptr()->sFocusRegion.sReference.nWidth = previewWidth;
    focusRegionControl.ptr()->sFocusRegion.sReference.nHeight = previewHeight;

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRegion), focusRegionControl.ptr());

    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE passed for camera");
    } else {
        DBGT_PTRACE("Error %x in OMX_SetConfig OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE", err);
    }
 return err;

}

int STEExtIspCamera::SetPropFocus()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    const FocusMode g_STECamFocusProp[] = {
            {0, CameraParameters::FOCUS_MODE_AUTO, OMX_IMAGE_FocusControlOn, OMX_SYMBIAN_FocusRangeAuto},
            {0, CameraParameters::FOCUS_MODE_INFINITY, OMX_IMAGE_FocusControlOn,OMX_SYMBIAN_FocusRangeInfinity},
            {0, CameraParameters::FOCUS_MODE_FIXED, OMX_IMAGE_FocusControlOff,(OMX_SYMBIAN_FOCUSRANGETYPE)NULL},
#ifdef ENABLE_CONTINUOUS_AUTOFOCUS
            {0, CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO,OMX_IMAGE_FocusControlAuto,OMX_SYMBIAN_FocusRangeAuto},
#endif //ENABLE_CONTINUOUS_AUTOFOCUS
            {1, CameraParameters::FOCUS_MODE_MACRO, OMX_IMAGE_FocusControlOn,OMX_SYMBIAN_FocusRangeMacro}
    };

    if (NULL != mParameters.get(CameraParameters::KEY_FOCUS_MODE)) {
        int i=0, value;
        const char *focus = mParameters.get(CameraParameters::KEY_FOCUS_MODE);

        do {
            value = strcmp(g_STECamFocusProp[i].focus, focus);
            if (!value)
                break;
            else
                i++;
        } while(g_STECamFocusProp[i-1].iMarkendofdata != 1);

        if (value != 0) {
             DBGT_WARNING("\n OMX_ErrorNotImplemented for Focus mode");
        }

        OmxUtils::StructContainer<OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE> focusControl;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_CRITICAL("OMX_GetConfig OMX_IndexConfigFocusControl passed for camera");
        }

        focusControl.ptr()->nPortIndex = OMX_ALL;
        focusControl.ptr()->eFocusControl = g_STECamFocusProp[i].eFocusType;

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE> rangeControl;
        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), rangeControl.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_CRITICAL("OMX_GetConfig EFocusRange passed for camera");
        }

        rangeControl.ptr()->nPortIndex = OMX_ALL;

        if (!strcmp(focus, CameraParameters::FOCUS_MODE_AUTO)) {
            rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeAuto;
        } else if (!strcmp(focus, CameraParameters::FOCUS_MODE_MACRO)) {
            rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeMacro;
        }

        DBGT_CRITICAL("rangeControl.ptr()->eFocusRange: %x", rangeControl.ptr()->eFocusRange);

        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), rangeControl.ptr());
        if (err == OMX_ErrorNone) { 
            DBGT_PTRACE("OMX_SetConfig EFocusRange passed for camera \n ");
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig EFocusRange ", err);
        }

        if( strcmp(mParameters.get(CameraParameters::KEY_FLASH_MODE),
                                                    CameraParameters::FLASH_MODE_ON ) == 0 )
        {    
             MMIO_Camera_flash_control(1);
             LOGE("checkIndexConfigFlashControl+++++++++++SetPropFocus+++++++++++open++torch");
        }


        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigFocusControl), focusControl.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigFocusControl passed for camera \n ");
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigFocusControl ", err);
        }
    }
    return err;
}

int STEExtIspCamera::SetPropFlickerRemoval()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    flickerproperty g_flickerprop[]={
        {0,CameraParameters::ANTIBANDING_OFF,OMX_SYMBIAN_FlickerRemovalOff},
        {0,CameraParameters::ANTIBANDING_AUTO,OMX_SYMBIAN_FlickerRemovalAuto},
        {0,CameraParameters::ANTIBANDING_50HZ,OMX_SYMBIAN_FlickerRemoval50},
        {1,CameraParameters::ANTIBANDING_60HZ,OMX_SYMBIAN_FlickerRemoval60},
    };

    if (NULL != mParameters.get(CameraParameters::KEY_ANTIBANDING)) {
        int id=0;
        int max_id = sizeof(g_flickerprop) / sizeof(g_flickerprop[0]);
        const char *flicker = mParameters.get(CameraParameters::KEY_ANTIBANDING);

        while(  (g_flickerprop[id].iMarkendofdata != 1) &&
                (strcmp(g_flickerprop[id].flicker, flicker)!=0)&&
                (id<max_id) ) {
            id++;
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE> flickerConfig;

        flickerConfig.ptr()->nPortIndex = OMX_ALL;
        flickerConfig.ptr()->eFlickerRemoval = g_flickerprop[id].eflickerType;

        err = OMX_SetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::EFlickerRemoval), &flickerConfig);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE ("OMX_SetConfig FlickerRemoval passed for camera %s",flicker);
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig FlickerRemoval %s",err,flicker);
        }
    }
    return err;
}

int STEExtIspCamera::SetPropSceneModes()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    scenemodeproperty g_scenemodeprop[]={
        {0,CameraParameters::SCENE_MODE_AUTO,           (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneAuto},
        {0,CameraParameters::SCENE_MODE_PORTRAIT,       (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_ScenePortrait},
        {0,CameraParameters::SCENE_MODE_LANDSCAPE,      (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneLandscape},
        {0,CameraParameters::SCENE_MODE_NIGHT,          (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneNight},
        {0,CameraParameters::SCENE_MODE_NIGHT_PORTRAIT, (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneNightPortrait},
        {0,CameraParameters::SCENE_MODE_SPORTS,         (OMX_SYMBIAN_SCENEMODETYPE)OMX_SYMBIAN_SceneSport},
        {0,CameraParameters::SCENE_MODE_BEACH,          (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Beach},
        {0,CameraParameters::SCENE_MODE_SNOW,           (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Snow},
        {0,CameraParameters::SCENE_MODE_PARTY,          (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Party},
        {0,CameraParameters::SCENE_MODE_SUNSET,         (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Sunset},
        {1,CameraParameters::SCENE_MODE_CANDLELIGHT,    (OMX_SYMBIAN_SCENEMODETYPE)OMX_STE_Scene_Candlelight},
    };

    if (NULL != mParameters.get(CameraParameters::KEY_SCENE_MODE)) {
        int id=0;
        int max_id = sizeof(g_scenemodeprop) / sizeof(g_scenemodeprop[0]);
        const char *scenemode = mParameters.get(CameraParameters::KEY_SCENE_MODE);

        while(  (g_scenemodeprop[id].iMarkendofdata != 1) &&
                (strcmp(g_scenemodeprop[id].scenemode, scenemode)!=0)&&
                (id<max_id) ) {
            id++;
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_SCENEMODETYPE> sceneModeConfig;
        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneModeConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig SceneMode passed for camera" );
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig SceneMode ",err);
        }

        sceneModeConfig.ptr()->nPortIndex = OMX_ALL;
        sceneModeConfig.ptr()->eSceneType = g_scenemodeprop[id].eScenemodeType;
        err = OMX_SetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneModeConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig SceneMode passed for camera %s ",scenemode);
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig SceneMode %s",err,scenemode);
        }
    }
    return err;
}

int STEExtIspCamera::SetPropFlash(const CameraParameters &params)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    flashproperty g_flashprop[]={
        {0,CameraParameters::FLASH_MODE_ON,OMX_IMAGE_FlashControlOn},
        {0,CameraParameters::FLASH_MODE_RED_EYE,OMX_IMAGE_FlashControlRedEyeReduction},
        {0,CameraParameters::FLASH_MODE_TORCH,OMX_IMAGE_FlashControlTorch},
        {0,CameraParameters::FLASH_MODE_AUTO,OMX_IMAGE_FlashControlAuto},
        {1,CameraParameters::FLASH_MODE_OFF,OMX_IMAGE_FlashControlOff}
    };

    if (NULL != params.get(CameraParameters::KEY_FLASH_MODE)) {
        int id=0;
        int max_id = sizeof(g_flashprop) / sizeof(g_flashprop[0]);
        const char *flash = params.get(CameraParameters::KEY_FLASH_MODE);

        int value;
        while( ((value = strcmp(g_flashprop[id].flash, flash))!=0) &&
               (g_flashprop[id].iMarkendofdata != 1) &&
               (id<max_id) ) {
            id++;
        }

        if (NO_ERROR != value) {
            DBGT_CRITICAL("Invalid flash value");
            return UNKNOWN_ERROR;
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE> flashConfig;
        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl), flashConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig FlashControl passed for camera" );
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig FlashControl ",err);
        }

		if(g_flashprop[id].eflashType==5)//torch
		 {	  
			 MMIO_Camera_flash_control(1);
			 LOGE("checkIndexConfigFlashControl++++++++++++++++++++++open++torch");
		 }	  
		 else if(g_flashprop[id].eflashType==0)//on
		 {	  
			 if( mPreviewWidth ==1280)
			 {	  
				 LOGE("checkIndexConfigFlashControl+++++++++++++++++isvideomode++++++on+");
				 MMIO_Camera_flash_control(1);
			 }	  
			 else 
			 {	  
				 LOGE("checkIndexConfigFlashControl+++++++++++++++++iscameramode++++++close+torch");
				 MMIO_Camera_flash_control(0);
			 }	  
		 }	  
		 else 
		 {	  
			 LOGE("checkIndexConfigFlashControl+++++++++++++++++++++++close+torch");
			 MMIO_Camera_flash_control(0);
		 }	  


        flashConfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode
        flashConfig.ptr()->eFlashControl = g_flashprop[id].eflashType;
        err = OMX_SetConfig(mCam,mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl), &flashConfig);
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig FlashControl passed for camera %s ",flash);
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig FlashControl %s",err,flash);
        }
    }
    return err;
}
int STEExtIspCamera::SetPropAELock()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_LOCKTYPE> autoexposurelockconfig;
    err = OMX_GetConfig(mCam,(OMX_INDEXTYPE) OMX_Symbian_IndexConfigExposureLock,autoexposurelockconfig.ptr());

    if (err == OMX_ErrorNone) {
        autoexposurelockconfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode

        DBGT_PTRACE("OMX_GetConfig ExposureLock passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig ExposureLock",err);
    }
    if (!strcmp(mParameters.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK),"true")) {
        DBGT_PTRACE("Set up Auto exposure Lock ");
        autoexposurelockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockImmediate;
    } else {
        DBGT_PTRACE("Removing Exposure Lock");
        autoexposurelockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockOff;

    }
    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE) OMX_Symbian_IndexConfigExposureLock,autoexposurelockconfig.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig ExposureLock passed for camera");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig ExposureLock ",err);
    }

    return err;

}
int STEExtIspCamera::SetPropAWBLock()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_LOCKTYPE> autoWhiteBalanceLockconfig;
    err = OMX_GetConfig(mCam,(OMX_INDEXTYPE) OMX_Symbian_IndexConfigWhiteBalanceLock,autoWhiteBalanceLockconfig.ptr());

    if (err == OMX_ErrorNone) {
        autoWhiteBalanceLockconfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode

        DBGT_PTRACE("OMX_GetConfig White Balance passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig White Balance ",err);
    }
    if (!strcmp(mParameters.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK),"true")) {
        DBGT_PTRACE("Set up Auto White Balance Lock");
        autoWhiteBalanceLockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockImmediate;
    } else {
        DBGT_PTRACE("Removing White Balance Lock");
        autoWhiteBalanceLockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockOff;

    }
    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE) OMX_Symbian_IndexConfigWhiteBalanceLock,autoWhiteBalanceLockconfig.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig White Balance passed for camera");
    } else {
       DBGT_CRITICAL("Error %x in OMX_SetConfig White Balance",err);
    }
    return err;
}

int STEExtIspCamera::SetPropEffect()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    imagefilterproperty g_imagefilterproperty[]={
        {0,CameraParameters::EFFECT_NONE,       (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNone},
        {0,CameraParameters::EFFECT_MONO,       (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterGrayScale},
        {0,CameraParameters::EFFECT_NEGATIVE,   (OMX_IMAGEFILTERTYPE)OMX_ImageFilterNegative},
        {0,CameraParameters::EFFECT_SOLARIZE,   (OMX_IMAGEFILTERTYPE)OMX_ImageFilterSolarize},
        {0,CameraParameters::EFFECT_SEPIA,      (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterSepia},
        {0,CameraParameters::EFFECT_POSTERIZE,  (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterPosterize},
        {1,CameraParameters::EFFECT_AQUA,       (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterWatercolor}
    };


    if (NULL != mParameters.get(CameraParameters::KEY_EFFECT)) {
        int id=0;
        int max_id = sizeof(g_imagefilterproperty) / sizeof(g_imagefilterproperty[0]);
        const char *imageFilter = mParameters.get(CameraParameters::KEY_EFFECT);

        while(  (g_imagefilterproperty[id].iMarkendofdata != 1) &&
                (strcmp(g_imagefilterproperty[id].imagefilter, imageFilter)!=0)&&
                (id<max_id) ) {
            id++;
        }

        OmxUtils::StructContainer<OMX_CONFIG_IMAGEFILTERTYPE> imageFilterConfig;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonImageFilter), imageFilterConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonImageFilter passed for camera" );
        } else {
            DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonImageFilter ",err);
        }

        imageFilterConfig.ptr()->nPortIndex = OMX_ALL;
        imageFilterConfig.ptr()->eImageFilter = g_imagefilterproperty[id].eImageFilter;
        err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonImageFilter), imageFilterConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonImageFilter passed for camera %s ",imageFilter);
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonImageFilter %s ",err,imageFilter);
        }
    }
    return err;
}

int STEExtIspCamera::SetPropExposure()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_CONFIG_EXPOSURECONTROLTYPE> exposureControl;

    err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), exposureControl.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonExposure passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonImageFilter ",err);
    }

    exposureControl.ptr()->nPortIndex = OMX_ALL;
    exposureControl.ptr()->eExposureControl = OMX_ExposureControlAuto;

    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), exposureControl.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonExposure passed for camera");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonExposure",err);
    }

    OmxUtils::StructContainer<OMX_CONFIG_EXPOSUREVALUETYPE> exposure;

    err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), exposure.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetConfig OMX_IndexConfigCommonExposureValue passed for camera" );
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig IndexConfigCommonExposureValue ",err);
    }

     // get the metering type
    if (NULL != mParameters.get(KEY_METERING_EXPOSURE)) {
        meteringProperty g_metering[]={
            {0, METERING_EXPOSURE_AVERAGE,  OMX_MeteringModeAverage},
            {0, METERING_EXPOSURE_SPOT,     OMX_MeteringModeSpot},
            {1, METERING_EXPOSURE_MATRIX,   OMX_MeteringModeMatrix}
        };

        int id=0;
        int max_id = sizeof(g_metering) / sizeof(g_metering[0]);
        const char *metering = mParameters.get(KEY_METERING_EXPOSURE);

        while(  (g_metering[id].iMarkendofdata != 1) &&
                (strcmp(g_metering[id].metering, metering)!=0)&&
                (id<max_id) ) {
            id++;
        }
         exposure.ptr()->eMetering = g_metering[id].eMetering;
    }

     // get the iso value
    if (NULL != mParameters.get(KEY_ISO)) {
        const char *iso = mParameters.get(KEY_ISO);
        if ( strcmp(ISO_AUTO, iso)==0 ) {
            exposure.ptr()->bAutoSensitivity = (OMX_BOOL)true;
        } else {
            exposure.ptr()->bAutoSensitivity = (OMX_BOOL)false;
            exposure.ptr()->nSensitivity = (OMX_U32)mParameters.getInt(KEY_ISO);
        }
    }

    if (NULL != mParameters.get(CameraParameters::KEY_EXPOSURE_COMPENSATION)) {
        exposure.ptr()->xEVCompensation =
            (OMX_S32)mParameters.getInt(CameraParameters::KEY_EXPOSURE_COMPENSATION) + EXPOSURE_OFFSET;
    }

    exposure.ptr()->nPortIndex = OMX_ALL;

    err = OMX_SetConfig(mCam,(OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), exposure.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCommonExposureValue passed for camera");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig OMX_IndexConfigCommonExposureValue",err);
    }

    return err;
}

int STEExtIspCamera::SetPropZoom()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != mParameters.get(CameraParameters::KEY_ZOOM)) {
        uint32 zoomkey = (uint32)mParameters.getInt(CameraParameters::KEY_ZOOM);

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE> zoomConfig;
        OmxUtils::StructContainer<OMX_SYMBIAN_SUPPORTED_ZOOM_RESOLUTIOINS> zoomSupported;
        int preview_width, preview_height;
        int picture_width ,picture_height;
        mParameters.getPreviewSize(&preview_width, &preview_height);
        mParameters.getPictureSize(&picture_width, &picture_height);

        DBGT_PTRACE("picture_width =%d,picture_height=%d",picture_width,picture_height);
#if 0 //added by dyron
        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EMaxZoomSupportedRes),zoomSupported.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig EMaxZoomSupportedRes passed for camera");
            DBGT_PTRACE("OMX_GetConfig EMaxZoomSupportedRes zoomSupported.ptr()->nPreviewWidth =%d",(int)zoomSupported.ptr()->nPreviewWidth);
            DBGT_PTRACE("OMX_GetConfig EMaxZoomSupportedRes zoomSupported.ptr()->nPreviewHeight =%d",(int)zoomSupported.ptr()->nPreviewHeight);
            DBGT_PTRACE("OMX_GetConfig EMaxZoomSupportedRes zoomSupported.ptr()->nPictureWidth =%d",(int)zoomSupported.ptr()->nPictureWidth);
            DBGT_PTRACE("OMX_GetConfig EMaxZoomSupportedRes zoomSupported.ptr()->nPictureHeight =%d",(int)zoomSupported.ptr()->nPictureHeight);
        } else {
            DBGT_CRITICAL("Error %x in OMX_GetConfig EMaxZoomSupportedRes ",err);
         }
        if(((uint32_t)preview_width <= zoomSupported.ptr()->nPreviewWidth) && ((uint32_t)preview_height <=zoomSupported.ptr()->nPreviewHeight) &&
           ((uint32_t)picture_width <=  zoomSupported.ptr()->nPictureWidth) && ((uint32_t)picture_height <= zoomSupported.ptr()->nPictureHeight)){

            DBGT_PTRACE("Zoom steps are supported");

        }else{
             DBGT_CRITICAL("Zoom steps are not supported");
        }
        zoomConfig.ptr()->nPortIndex = OMX_ALL;
        zoomConfig.ptr()->xZoomFactor.nValue = zoomkey;
        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EDigitalZoom), zoomConfig.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SetConfig EDigitalZoom passed for camera");
        } else {
            DBGT_CRITICAL("Error %x in OMX_SetConfig EDigitalZoom ",err);
        }

#endif
    }
    return err;
}

void STEExtIspCamera::setFirstFrameParameters( int mode  )
{
    DBGT_PROLOG("mode: %d", mode);

    switch (mode) {
        case PREVIEW_MODE :
          if ( mCameraId == CAMERA_FACING_BACK ) {
              if ( strcmp(mParameters.get(CameraParameters::KEY_SCENE_MODE),
                          CameraParameters::SCENE_MODE_AUTO ) == 0 ) {
                  if (NO_ERROR !=  SetPropSceneModes()) {
                    DBGT_CRITICAL("SetPropSceneModes failed");
                  }

                  if (NO_ERROR !=  SetPropEffect()) {
                      DBGT_CRITICAL("SetPropEffect failed");
                  }

                  if (NO_ERROR !=  SetPropExposure()) {
                      DBGT_CRITICAL("SetPropExposure failed");
                  }

                  if (NO_ERROR !=  SetPropContrast()) {
                      DBGT_CRITICAL("SetPropContrast failed");
                  }

                  if (NO_ERROR !=  SetPropWhiteBalance() ) {
                      DBGT_CRITICAL("SetPropWhiteBalance failed");
                  }
              } else {
                  if (NO_ERROR !=  SetPropSceneModes()) {
                      DBGT_CRITICAL("SetPropSceneModes failed");
                  }

                  if (NO_ERROR !=  SetPropExposure()) {
                      DBGT_CRITICAL("SetPropExposure failed");
                  }

                  if (NO_ERROR !=  SetPropWhiteBalance()) {
                      DBGT_CRITICAL("SetPropWhiteBalance failed");
                  }
              }

              if (NO_ERROR !=  SetPropJPEGQuality()) {
                  DBGT_CRITICAL("SetPropJPEGQuality failed");
              }

              if (NO_ERROR !=  SetPropFlash(mParameters)) {
                  DBGT_CRITICAL("SetPropFlash failed");
              }
          } else {
              if (NO_ERROR !=  SetPropExposure()) {
                  DBGT_CRITICAL("SetPropExposure failed");
              }

              if (NO_ERROR !=  SetPropWhiteBalance()) {
                  DBGT_CRITICAL("SetPropWhiteBalance failed");
              }
          }
        break;
        case CAMCORDER_MODE:
          if (NO_ERROR !=  SetPropEffect()) {
              DBGT_CRITICAL("SetPropEffect failed");
          }

          if (NO_ERROR !=  SetPropExposure()) {
              DBGT_CRITICAL("SetPropExposure failed");
          }

          if (NO_ERROR !=  SetPropWhiteBalance()) {
              DBGT_CRITICAL("SetPropWhiteBalance failed");
          }

          if( mCameraId == CAMERA_FACING_BACK ) {
              if (NO_ERROR !=  SetPropFocus()) {
                  DBGT_CRITICAL("SetPropFocus failed");
              }
          }
        break;
        case COMMON_MODE:
          if ( mCameraId == CAMERA_FACING_BACK ) {
              if (NO_ERROR !=  SetPropFocusRange(mParameters)) {
                  DBGT_CRITICAL("SetPropFocusRange failed");
              }
          }

          if (NO_ERROR !=  SetPropFlickerRemoval()) {
              DBGT_CRITICAL("SetPropFlickerRemoval failed");
          }

          if (NO_ERROR !=  SetPropZoom()) {
              DBGT_CRITICAL("SetPropZoom failed");
          }

          if (NO_ERROR != SetPropFlash(mParameters)) {
              DBGT_CRITICAL("SetPropFlash failed");
          }
        break;
    }

    DBGT_EPILOG("");
    return;
}

OMX_ERRORTYPE STEExtIspCamera::getLensParameters()
{
    DBGT_PROLOG("Inside getLensParameters");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    float final_focallength;
    char str_focal[10];
    if (NULL != mCam) {
        OmxUtils::StructContainer<OMX_STE_CONFIG_LENSPARAMETERTYPE> lensParameters;

        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ELensParams), lensParameters.ptr());
        if (err == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_GetConfig getLensParameters passed for camera");
        } else {
            DBGT_CRITICAL("Error %x in OMX_GetConfig getLensParameters ",err);
            goto err;
        }

        if (0 > (int)lensParameters.ptr()->nFocalLength) {
            DBGT_CRITICAL("Invalid Focal Length %d",(int)lensParameters.ptr()->nFocalLength);
            mParameters.set(CameraParameters::KEY_FOCAL_LENGTH, "4.31");
        } else {
           final_focallength = (float)(lensParameters.ptr()->nFocalLength/(100 *1.0)) ;
           DBGT_PTRACE("FocalLength intermediate float  = %f", final_focallength);
           mParameters.setFloat(CameraParameters::KEY_FOCAL_LENGTH, final_focallength);
        }

        if (0 > (int)lensParameters.ptr()->nHorizontolViewAngle) {
            DBGT_CRITICAL( "Invalid Horizontal View Angle %d",(int)lensParameters.ptr()->nHorizontolViewAngle);
            mParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, "54.8");
        } else {
            mParameters.setFloat(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, (1.0*lensParameters.ptr()->nHorizontolViewAngle)/100);
        }

        if (0 > (int)lensParameters.ptr()->nVerticalViewAngle) {
            DBGT_CRITICAL( "Invalid Vertical View Angle %d", (int)lensParameters.ptr()->nVerticalViewAngle);
            mParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "42.5");
        } else {
            mParameters.setFloat(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, (1.0*lensParameters.ptr()->nVerticalViewAngle)/100);
        }
    } else {
        // set default values
        DBGT_PTRACE("Setting default values");
        mParameters.set(CameraParameters::KEY_FOCAL_LENGTH, "4.31");
        mParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, "54.8");
        mParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "42.5");
    }
    snprintf(str_focal, 10, "%f", mParameters.getFloat(CameraParameters::KEY_FOCAL_LENGTH));
    DBGT_PTRACE("focal string = %s",str_focal);
    DBGT_PTRACE("FocalLength = %s", mParameters.get(CameraParameters::KEY_FOCAL_LENGTH));
    DBGT_PTRACE("HorizontolViewAngle = %s", mParameters.get(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE));
    DBGT_PTRACE("VerticalViewAngle = %s", mParameters.get(CameraParameters::KEY_VERTICAL_VIEW_ANGLE));

err:
    DBGT_EPILOG();
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::setLensParameters(void *aArg)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    DBGT_ASSERT(NULL != mCam, "Camera Handle is Null");
    OmxUtils::StructContainer<OMX_STE_CONFIG_LENSPARAMETERTYPE> lensParameters;
    err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ELensParams), lensParameters.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetConfig setLensParameters passed for camera");
    } else {
        DBGT_CRITICAL("Error %x in OMX_GetConfig setLensParameters ",err);
    }
    DBGT_ASSERT(0 < lensParameters.ptr()->nFocalLength, "Invalid Focal Length");
    DBGT_ASSERT(0 < lensParameters.ptr()->nHorizontolViewAngle, "Invalid Horizontal View Angle");
    DBGT_ASSERT(0 < lensParameters.ptr()->nVerticalViewAngle, "Invalid Vertical View Angle");
    DBGT_PTRACE("FocalLength = %d", (int)lensParameters.ptr()->nFocalLength);
    DBGT_PTRACE("HorizontolViewAngle = %d", (int)lensParameters.ptr()->nHorizontolViewAngle);
    DBGT_PTRACE("VerticalViewAngle = %d", (int)lensParameters.ptr()->nVerticalViewAngle);
    lensParameters.ptr()->nFocalLength  = (mParameters.getInt(CameraParameters::KEY_FOCAL_LENGTH)) << 16;
    err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ELensParams), lensParameters.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig setLensParameters passed for camera for KEY_FOCAL_LENGTH");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig setLensParameters for KEY_FOCAL_LENGTH",err);
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::setMakeAndModel()
{
    DBGT_PROLOG();

    DBGT_ASSERT(mCam != NULL, "Handle is Null");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OmxUtils::StructContainer<OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL> makeModel;

    err = OMX_GetParameter(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EMakeModel), makeModel.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Make Model OMX_GetParameter passed ");
    } else {
        DBGT_CRITICAL("Make Model OMX_GetParameter failed");
    }

// Get Manufacturer value
#ifdef CAM_EXIF_TAG_MAKE
    strcpy((char *)&makeModel.ptr()->cMake[0], CAM_EXIF_TAG_MAKE);
#else
    DynSetting::get(DynSetting::EManufacturer, (char *)&makeModel.ptr()->cMake[0]);
#endif //CAM_EXIF_TAG_MAKE

    // Get Model value
#ifdef CAM_EXIF_TAG_MODEL
    strcpy((char *)&makeModel.ptr()->cModel[0], CAM_EXIF_TAG_MODEL);
#else
    DynSetting::get(DynSetting::EModel, (char *)&makeModel.ptr()->cModel[0]);
#endif //CAM_EXIF_TAG_MODEL
    makeModel.ptr()->nPortIndex = OMX_ALL;

    err = OMX_SetParameter(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EMakeModel), makeModel.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Make Model OMX_SetParameter passed ");
    } else {
        DBGT_CRITICAL("Make Model OMX_SetParameter failed");
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::SetSoftwareVersion()
{
    DBGT_PROLOG();
    DBGT_ASSERT(mCam != NULL, "Handle is Null");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_PARAM_SOFTWARE> software;
    err = OMX_GetParameter(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ESoftware), software.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Software OMX_GetParameter passed ");
    } else {
        DBGT_CRITICAL("Software OMX_GetParameter failed");
    }
#ifdef CAM_EXIF_TAG_SOFTWARE
    strcpy((char *)&software.ptr()->cSoftware[0], CAM_EXIF_TAG_SOFTWARE);
#else
    DynSetting::get(DynSetting::ESoftware, (char *)&software.ptr()->cSoftware[0]);
#endif //CAM_EXIF_TAG_SOFTWARE
    software.ptr()->nPortIndex = OMX_ALL;
    err = OMX_SetParameter(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ESoftware), software.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Software OMX_SetParameter passed ");
    } else {
        DBGT_CRITICAL("Software OMX_SetParameter failed");
    }
    DBGT_EPILOG("");
    return err;
}
OMX_ERRORTYPE STEExtIspCamera::setOrientation()
{
    DBGT_PROLOG();
    DBGT_ASSERT(mCam != NULL, "Handle is Null");
    int exif_rotation = mParameters.getInt(CameraParameters::KEY_ROTATION);
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ORIENTATIONTYPE> stillOrientationConfig;
    stillOrientationConfig.ptr()->nPortIndex = OMX_ALL;
    err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_Symbian_IndexConfigOrientationScene), stillOrientationConfig.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Orientation OMX_GetConfig passed ");
    } else {
        DBGT_CRITICAL("Orientation OMX_GetConfig failed");
    }
    if (0 == exif_rotation) {
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
    } else if (90 == exif_rotation) {   /*< Frame is 90 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowRightColumnTop;
    } else if (180 == exif_rotation) {   /*< Frame is 180 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowBottomColumnRight;
    } else if (270 == exif_rotation) {  /*< Frame is 270 degrees CW rotated */
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowLeftColumnBottom;
    } else {
        stillOrientationConfig.ptr()->eOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
    }
    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_Symbian_IndexConfigOrientationScene), stillOrientationConfig.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Orientation OMX_SetConfig passed ");
    } else {
        DBGT_CRITICAL("Orientation OMX_SetConfig failed");
    }
    DBGT_EPILOG();
    return err;
}

void STEExtIspCamera::ReconfigurePreview()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err;
    OMX_STATETYPE state;
    OMX_PARAM_SENSORMODETYPE sensorModeVPB0;
    int framerate = mParameters.getPreviewFrameRate();
    int min_fpsRange = 0, max_fpsRange = 0;

    mParameters.getPreviewFpsRange(&min_fpsRange, &max_fpsRange);
    updateFrameRate(framerate, min_fpsRange, max_fpsRange);

    OmxUtils::StructWrapper<OMX_PARAM_SENSORMODETYPE>::init(sensorModeVPB0);

    //Disable VPB0 port
    DBGT_PTRACE("disable VPB0 port");
    err = OMX_SendCommand(mCam,OMX_CommandPortDisable,CAM_VPB+0,NULL);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("In ReconfigureStill OMX_CommandPortDisable Port 0 passed");
    } else {
        DBGT_PTRACE("Port Disable mCam failed 0x%x",err);
    }

    //Free preview buffers
    FreeRecordHeapLocked();

    camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);

    OmxUtils::initialize(mParamPortVPB0, OMX_PortDomainVideo, CAM_VPB+0);
    err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }

    mCamMode = false;

    const char *cmode = mParameters.get(CameraParameters::KEY_RECORDING_HINT);
    if (cmode != NULL) {
        if (strcmp(cmode, "true") == 0) {
            DBGT_PTRACE("cameraConfigSet  cam_mode ");
            mCamMode = true;
            mbOneShot= OMX_FALSE;
        } else if(strcmp(cmode, "false") == 0) {
            DBGT_PTRACE("cameraConfigSet  preview_mode ");
            mbOneShot= OMX_TRUE;
        }
    }

    // the index of the input port. Should be modified.
    mParamPortVPB0.nBufferCountActual = kRecordBufferCount;

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE    *pt_video0 = &(mParamPortVPB0.format.video);

    pt_video0->cMIMEType = (OMX_STRING)"";
    pt_video0->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video0->nFrameWidth = (OMX_U32) mVPB0Width;
    pt_video0->nFrameHeight = (OMX_U32)mVPB0Height;

    pt_video0->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_video0->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingUnused;

    // set video format of pipe 0
    pt_video0->eColorFormat = mOmxRecordPixFmt;

    pt_video0->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video0->xFramerate= framerate;
    DBGT_PTRACE("Set xFramerate: %d ",(int)pt_video0->xFramerate);

    err =OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }

    err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &mParamPortVPB0);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 Failed");
    }

    DBGT_PTRACE("VPB0 port Size %dx%d stride %d slice height %d framerate %d",
        (int)mVPB0Width,(int)mVPB0Height,(int)mParamPortVPB0.format.video.nStride,
        (int)mParamPortVPB0.format.video.nSliceHeight, (int)framerate);

    //dump
    DBGT_PTRACE("Dumping params for Camera VPB0");
    OmxUtils::dump(mParamPortVPB0);

    //Enable VPB0 port
    DBGT_PTRACE("Enable VPB0 port");
    err = OMX_SendCommand(mCam,OMX_CommandPortEnable,CAM_VPB+0,NULL);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SendCommand OMX_CommandPortEnable Port 0 passed");
    } else {
        DBGT_CRITICAL("OMX_SendCommand OMX_CommandPortEnable Port 0 failed 0x%x " , err);
    }

    //Allocate preview buffer
    AllocateRecordHeapLocked();

    camera_sem_wait(&mSetPortCam_sem, SEM_WAIT_TIMEOUT);

    DBGT_EPILOG("");
}

OMX_ERRORTYPE STEExtIspCamera::cleanUpDspPanic(void* aArg)
{
    DBGT_PROLOG("");

    if (mIsRecordHeapSet) {
        FreeRecordHeapLocked();
    }

    // still buffers De-allocation
    FreeStillHeapLocked();

    if (mPreviewBlitter) {
        delete mPreviewBlitter;
        mPreviewBlitter = NULL;
    }
    if (mRecordBlitterPass1) {
        delete mRecordBlitterPass1;
        mRecordBlitterPass1 = NULL;
    }
    if (mRecordBlitterPass2) {
        delete mRecordBlitterPass2;
        mRecordBlitterPass2 = NULL;
    }

    cleanUp();

    usleep(1000*1000);
    LOGE("Kill mediaserver process due to an unrecoverable error");
    _exit(1);

    DBGT_EPILOG("");
}

int STEExtIspCamera::beginDispatchThread(void *cookie)
{
    status_t err = NO_ERROR;
    reqDispatchHdl *c = (reqDispatchHdl *)cookie;

    if ((c!=NULL) && (c->camera!=NULL))
        err = c->camera->dispatchThread( c );

    return err;
}

int STEExtIspCamera::dispatchThread( void* cookie )
{
    reqDispatchHdl *hdl = (reqDispatchHdl *)cookie;
    int32_t aMsg = hdl->aMsg;
    int32_t aInfo1 = hdl->aInfo1;
    int32_t aInfo2 = hdl->aInfo2;
    int32_t aReq = hdl->aReq;
    void *aArg = hdl->aArg;
    bool aWaitForRequest = hdl->aWaitForRequest;

    DBGT_PROLOG("Msg: 0x%08x(%d) Info1: %d Info2: %d", aMsg, aMsg, aInfo1, aInfo2);

    status_t err = NO_ERROR;

    Mutex::Autolock lock(mLock);

    switch (aReq) {
    case reqDispatchHdl::cleanUpDspPanicReq:
        // call of clean up dsp panic
        cleanUpDspPanic(aArg);
        break;
    case reqDispatchHdl::restartOnTimeout:
        doStopPreview();
        doStartPreview();
        break;
    case reqDispatchHdl::focusStatusReached:
        if (mMsgEnabled & CAMERA_MSG_FOCUS) {
                mLock.unlock();
                mNotifyCb(CAMERA_MSG_FOCUS, true, 0, mCallbackCookie);
                mLock.lock();
        }
        break;
    case reqDispatchHdl::focusStatusUnableToReach:
        if (mMsgEnabled & CAMERA_MSG_FOCUS) {
                mLock.unlock();
                mNotifyCb(CAMERA_MSG_FOCUS, false, 0, mCallbackCookie);
                mLock.lock();
        }
        break;
    default:
        DBGT_PTRACE("MsgEnabled: %d(0x%08x)", mMsgEnabled, mMsgEnabled);

        if (mMsgEnabled & aMsg) {
            /* Dispatch callback with out lock. This will allow
             * callback to call cameraHAL APIs. Otherwise there
             * would be a deadlock
             */
            mLock.unlock();
            mNotifyCb(aMsg, aInfo1, aInfo2, mCallbackCookie);
            mLock.lock();
        }
        break;
    }
    delete hdl;
    DBGT_EPILOG("");
    return err;
}

// send a new request
void STEExtIspCamera::dispatchRequest(int32_t aReq, void *aArg /*= NULL */, bool aWaitForRequest /*= true*/)
{
    DBGT_PROLOG("Arg: %p WaitForRequest: %d", aArg, aWaitForRequest);

    // create a new request handler to dispatch
    reqDispatchHdl *hdl = new reqDispatchHdl();

    // fill fields of the new request
    hdl->aReq = aReq;
    hdl->aArg = aArg;
    hdl->aWaitForRequest = aWaitForRequest;
    hdl->aMsg = 0;
    hdl->aInfo1 = 0;
    hdl->aInfo2 = 0;
    hdl->camera = this;

    if (createThread(beginDispatchThread, (void*)hdl) == false) {
        DBGT_CRITICAL("Can't create a new thread for dispatch ");
    }

    DBGT_EPILOG("");
}

// send a message to the upper layer
void STEExtIspCamera::dispatchNotification(int32_t aMsg, int32_t aInfo1, int32_t aInfo2 /*= 0*/)
{
    DBGT_PROLOG("Msg: 0x%08x(%d) Info1: %d Info2: %d", aMsg, aMsg, aInfo1, aInfo2);

    // create a new request handler to dispatch
    reqDispatchHdl *hdl = new reqDispatchHdl();

    // fill fields of the new request
    hdl->aReq = 0;
    hdl->aArg = 0;
    hdl->aWaitForRequest = false;
    hdl->aMsg = aMsg;
    hdl->aInfo1 = aInfo1;
    hdl->aInfo2 = aInfo2;
    hdl->camera = this;

    if (createThread(beginDispatchThread, (void*)hdl) == false) {
        DBGT_CRITICAL("Can't create a new thread for dispatch ");
    }

    DBGT_EPILOG("");
}

// send a error message to the upper layer
void STEExtIspCamera::dispatchError(int32_t aInfo1, int32_t aInfo2 /*= 0*/)
{
    DBGT_PROLOG("Info1: %d Info2: %d", aInfo1, aInfo2);

    dispatchNotification(CAMERA_MSG_ERROR, aInfo1, aInfo2);

    DBGT_EPILOG("");
}

int STEExtIspCamera::isFrameRateSupported(const char *fps_list, int framerate)
 {
    char fps[3];
    snprintf(fps, strlen(fps), "%d", framerate);

    if (strstr(fps_list, fps))
        return 1;
    else
        return 0;
}

void  STEExtIspCamera::updateFrameRate(int &aFramerate, int &aMinFramerate, int &aMaxFramerate)
{
    DBGT_PROLOG("Framerate: %d Min Framerate: %d MaxFramerate: %d ", aFramerate, aMinFramerate, aMaxFramerate);
    /*Static FrameRate for reference tag*/
    if ( aMaxFramerate != -1) {
        if (isFrameRateSupported(mParameters.get(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES),
            aFramerate)) {
            DBGT_EPILOG("");
            return ;
        }
        aFramerate = aMaxFramerate / 1000;
        if (aFramerate > MAX_PREVIEW_FRAMERATE) {
            DBGT_PINFO("Requested framerate is %d & Maximum supported frame rate = %d",
                   aFramerate,
                    MAX_PREVIEW_FRAMERATE);
            aMinFramerate = MAX_PREVIEW_FRAMERATE * 1000;
            aMaxFramerate = MAX_PREVIEW_FRAMERATE * 1000;
            aFramerate = MAX_PREVIEW_FRAMERATE;
        }
    } else {
        if ((aFramerate > MAX_PREVIEW_FRAMERATE) ||
            (!isFrameRateSupported(mParameters.get(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES),
                    aFramerate))) {
            aFramerate = MAX_PREVIEW_FRAMERATE;
        }
        //set default Min/Max to framerate
        aMinFramerate = aFramerate*1000;
        aMaxFramerate = aFramerate*1000;
    }
    DBGT_EPILOG("");
}

void STEExtIspCamera::setPreviewFpsRange(int min, int max)
{
    DBGT_PROLOG("");

    char fpsrange[32];
    sprintf(fpsrange, "%d,%d", min , max );
    DBGT_PTRACE("Setting range to %s", fpsrange);
    mParameters.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, fpsrange);

    DBGT_EPILOG("");
}

int STEExtIspCamera::SetPropTorch(bool  on)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE> flashConfig;

    if ( strcmp(CameraParameters::FLASH_MODE_ON ,
                 mParameters.get(CameraParameters::KEY_FLASH_MODE))) {
        goto err;
    }

    flashConfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode
    if ( on ) {
        flashConfig.ptr()->eFlashControl = OMX_IMAGE_FlashControlTorch;
    } else {
        DBGT_CRITICAL("Exif Settings OMX_GetConfig failed");
    }

err:
    DBGT_EPILOG("");
    return err;
}

int STEExtIspCamera::SetPropFocusRange(const CameraParameters &params)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    const char *focus = params.get(CameraParameters::KEY_FOCUS_MODE);

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE> rangeControl;
    rangeControl.ptr()->nPortIndex = OMX_ALL;

    if (!strcmp(focus, CameraParameters::FOCUS_MODE_AUTO)) {
        rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeAuto;
    } else if (!strcmp(focus, CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO)) {
        rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeAuto;
    } else if (!strcmp(focus, CameraParameters::FOCUS_MODE_MACRO)) {
        rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeMacro;
    } else {
        DBGT_CRITICAL("Invalid focus range:%s", focus);
#if 0
        return UNKNOWN_ERROR;
#else
        rangeControl.ptr()->eFocusRange= OMX_SYMBIAN_FocusRangeAuto;
#endif
    }

    err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), rangeControl.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig EFocusRange passed for camera \n ");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetConfig EFocusRange ", err);
    }

    DBGT_EPILOG("  eFocusRange : %d" , rangeControl.ptr()->eFocusRange);
    return err;
}

int STEExtIspCamera::SetPropJPEGQuality()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    int quality = 100;
    if (NULL ==  mParameters.get(CameraParameters::KEY_JPEG_QUALITY)) {
        return NO_ERROR;
    }

    quality = mParameters.getInt(CameraParameters::KEY_JPEG_QUALITY);

    OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
    qfactor.ptr()->nPortIndex = OMX_ALL;
    qfactor.ptr()->nQFactor = quality<<16;

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_IndexParamQFactor, qfactor.ptr());
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamQFactor passed for camera \n ");
    } else {
        DBGT_CRITICAL("Error %x in OMX_SetParameter OMX_IndexParamQFactor ", err);
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STEExtIspCamera::GetExtradataField(OMX_BUFFERHEADERTYPE *pOmxBufferHdr, OMX_SYMBIAN_CONFIG_EXIFSETTINGSTYPE *pExifSettings)
{
    DBGT_PROLOG();

    DBGT_ASSERT(mCam != NULL, "Handle is Null");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    pExifSettings->pOmxBufHdr = pOmxBufferHdr;
    err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EExtradata), pExifSettings);

    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("Exif Settings OMX_GetConfig passed ");
    } else {
        DBGT_CRITICAL("Exif Settings OMX_GetConfig failed");
    }

    DBGT_EPILOG("");
    return err;
}

}; // namespace android
