/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//System includes
#include <sys/ioctl.h>
#include <dlfcn.h>
#ifdef USE_HWMEM
#include <linux/hwmem.h>
#else //!USE_HWMEM
#include <linux/android_pmem.h>
#endif //USE_HWMEM

//Multimedia Includes
#include <IFM_Types.h>
#include <IFM_Index.h>
#include <OMX_Symbian_ComponentExt_Ste.h>

#define DBGT_LAYER 0
#define DBGT_PREFIX "IntIsp"

//Internal includes
#include "STECamTrace.h"
#include "STECamOmxUtils.h"
#include "STECamFocus.h"
#include "STECamera.h"
#include "STECamProp.h"
#include "STECameraKeys.h"
#include "STECameraMsg.h"
#include "STECamSwRoutines.h"
#include "STECamThreadContainer.h"
#include "STECamSwProcessingThread.h"
#include "STECamPreviewThread.h"
#include "STECamRecordThread.h"
#include "STECamAutoFocusThread.h"
#include "STECamReqHandlerThread.h"
#include "STECamFlickerUpdate.h"
#include "STECamSwBuffer.h"
#include "STECamHwBuffer.h"
#include "STECamServiceBuffer.h"
#include "STECamFocusHandler.h"
#include "STECamThumbnailHandler.h"
#include "STECamImageRotHandler.h"
#include "STECameraConfig.h"
#include "hwconversion/STECamHwConversion.h"
#include "mmhwbuffers/STECamMMHwBuffer.h"
#include "b2r2/STECamB2R2Utils.h"

extern "C" double ceil(double a);
#include <HardwareAPI.h>

#define MMHWBUFFER_SO_FILE "/system/lib/libste_ensloader.so"

#define COORDINATES 5

namespace android
{

OMX_CALLBACKTYPE camCallback, ispCallback;
OMX_CALLBACKTYPE jpegCallback, exifCallback;
OMX_PARAM_PORTDEFINITIONTYPE paramJPEGinput, paramJPEGoutput;
OMX_PARAM_PORTDEFINITIONTYPE paramEXIFinput2, paramEXIFoutput;
OMX_PARAM_PORTDEFINITIONTYPE paramPortVPB0, paramPortVPB1, paramPortVPB2;
OMX_PARAM_PORTDEFINITIONTYPE paramISPinput, paramISPoutput, paramISPoutput1;
OMX_PARAM_SENSORMODETYPE sensorMode;

char zslOff[]="zsl-off";
char zslSingle[]="zsl-single";
char zslBurst[]="zsl-burst";

char omxNamePrimary[]="OMX.ST.HSMCamera";
char omxNameSecondary[]="OMX.ST.HSMCamera2";
#ifdef HWISPPROC
char ispproc[] = "OMX.ST.IspProc";
#elif SWISPPROC
char ispproc[] = "OMX.ST.SwissProc";
#endif // HWISPPROC
char jpegenc[] = "OMX.ST.VFM.JPEGEnc";

char exifmixer[] = "OMX.ST.VFM.EXIFMixer";

OMX_BUFFERHEADERTYPE *pVFBuffer[STECamera::kTotalPreviewBuffCount];
OMX_BUFFERHEADERTYPE *pRecBuffer[STECamera::kRecordBufferCount];
OMX_BUFFERHEADERTYPE *pStillBuffer[STECamera::kMaxStillBuffers];
OMX_BUFFERHEADERTYPE *pVPB1ImageBuffer = NULL;
OMX_BUFFERHEADERTYPE *pJPEGinBuffer[STECamera::kMaxStillBuffers];
OMX_BUFFERHEADERTYPE *pISPinBuffer, *pISP1outBuffer, *pISP2outBuffer;
OMX_BUFFERHEADERTYPE *pEXIFoutBuffer;

volatile bool g_IsPreviewConfig;
volatile bool isStartPreview = false;
volatile OmxBuffInfo* g_lastPreviewOMXBuffInfo = NULL;

uint32 zoom_factor[] = {0x10000, 0x18000, 0x20000, 0x28000, 0x30000, 0x38000, 0x40000};
#ifdef ENABLE_FACE_DETECTION
camera_face_t demoFace[10];

camera_frame_metadata_t faceMetaData = {
    0,
    demoFace
};
#endif
int gRotation;

sem_t jpeg_sem;
sem_t stateCam_sem;
sem_t stateISP_sem;
sem_t stateJpeg_sem;
sem_t record_eos_sem;
sem_t video_rot_sem;
sem_t shutter_eos;
sem_t isp_snap_sem;
sem_t isp_image_sem;
sem_t jpeg_eos_sem;
sem_t stateEXIF_sem;
sem_t exif_sem;
sem_t exif_eos_sem;
sem_t still_during_record;
sem_t sem_b2r2;
sem_t sem_b2r2_reset;
sem_t sem_crop_buffer;

#if defined(SWISPPROC) || defined(HWISPPROC)

OMX_ERRORTYPE IspprocFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                    OMX_IN OMX_PTR pAppData,
                                    OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pBuffer->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
    DBGT_ASSERT( (buffInfo->mFlags & OmxBuffInfo::ESentToComp),
                "Invlid buffer got, not sent to comp Flags: 0x%08x", buffInfo->mFlags);
    //reset flag
    buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    if (pBuffer->nOutputPortIndex == 1) {
        sem_post(&isp_snap_sem);
        DBGT_PTRACE("ISpProc Fill Buffer Size: %lu\n", pBuffer->nFilledLen);
    }

    if (pBuffer->nOutputPortIndex == 2) {
        sem_post(&isp_image_sem);
        DBGT_PTRACE("ISpProc Image Buffer Size: %lu\n", pBuffer->nFilledLen);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IspprocEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                  OMX_IN OMX_PTR pAppData,
                                  OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pBuffer->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");

    buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
    buffInfo->mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
    buffInfo->mFlags |= OmxBuffInfo::ECameraHal;

    OmxBuffInfo *buffInfoCam = static_cast<OmxBuffInfo *>(pStillBuffer[0]->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Cam Buffinfo is NULL");

    buffInfoCam->mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
    buffInfoCam->mFlags |= OmxBuffInfo::ECameraHal;

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}
#endif // SWISPPROC || HWISPPROC

#ifdef JPEGENC
OMX_ERRORTYPE JpegFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                 OMX_IN OMX_PTR pAppData,
                                 OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");
    DBGT_ASSERT(NULL != pAppData, "Invalid aAppData");
    STECamera *parent = static_cast<STECamera *>(pAppData);

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    if (pBuffer->nOutputPortIndex == 1) {
        sem_post(&jpeg_sem);
        parent->mPerfManager.logAndDump(PerfManager::EJpegFBD);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JpegEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                  OMX_IN OMX_PTR pAppData,
                                  OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    if (pBuffer->nOutputPortIndex == 0) {
        sem_post(&jpeg_sem);
        DBGT_PTRACE("JPEG Empty Buffer Done");
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}
#endif // JPEGENC

OMX_ERRORTYPE CameraEventHandler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);

    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
            OmxUtils::name(eEvent),
            eEvent,
            nData1,
            nData2,
            pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pAppData, "Invalid aAppData");
    STECamera *parent = static_cast<STECamera *>(pAppData);

    switch (eEvent) {
        case OMX_EventCmdComplete:
            {
                OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
                DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

                if (cmd == OMX_CommandStateSet) {
                    OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                    DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                    switch (newState) {
                        case OMX_StateLoaded:
                            sem_post(&stateCam_sem);
                            break;
                        case OMX_StateIdle:
                            sem_post(&stateCam_sem);
                            break;
                        case OMX_StateExecuting:
                            sem_post(&stateCam_sem);
                            break;
                        case OMX_StatePause:
                            sem_post(&stateCam_sem);
                            break;
                        case OMX_StateInvalid:
                            DBGT_CRITICAL("OMX_StateInvalid..");
                            break;
                        default:
                            break;
                    }
                } else if (OMX_CommandPortEnable == cmd) {
                    sem_post(&stateCam_sem);
                } else if (OMX_CommandPortDisable == cmd) {
                    sem_post(&stateCam_sem);
                } else if (OMX_CommandFlush == cmd) {
                    sem_post(&stateCam_sem);
                }
                break;
            }
        case OMX_EventBufferFlag:
            {
                if ((nData1 == CAM_VPB + 2) && (nData2 & OMX_BUFFERFLAG_EOS)) {
                    sem_post(&record_eos_sem);
                }
                if ((nData1 == CAM_VPB + 1) && (nData2 & OMX_BUFFERFLAG_EOS)) {
                    parent->mPerfManager.logAndDump(PerfManager::ECameraEOS);
                }
                break;
            }
        case OMX_EventError:
            if (nData1 == (OMX_U32)OMX_ErrorHardware) {
                DBGT_CRITICAL("OMX_ErrorHardware received, dispatch error to the application");
                parent->dispatchError(OMX_ErrorHardware);
                parent->dispatchRequest(&STECamera::cleanUpDspPanic, NULL, false);
            } else if ((nData1 == (OMX_U32)OMX_ErrorUnsupportedSetting)
                    && (nData2 == (OMX_U32)OMX_Symbian_IndexConfigExtDigitalZoom)) {
                DBGT_WARNING("Zoom step is not supported by sensor");
            }
            break;
        case OMX_EventPortSettingsChanged:
            if ((nData1 == CAM_VPB + 0) || (nData1 == CAM_VPB + 2)) {
                sem_post(&video_rot_sem);
            }
            break;

        case OMX_EventIndexSettingChanged:
#if 0 // added by dyron
	    if ((OMX_ALL == nData1) &&
                    ((OMX_U32)parent->mOmxILExtIndex->getIndex(OmxILExtIndex::EZoomMode) == nData2)) {
                parent->dispatchRequest(&STECamera::doSmoothZoomProcessing,NULL, false);
            }
#endif
            if ((OMX_ALL == nData1) &&
                    ((OMX_U32)parent->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusLock) == nData2)) {
                parent->mPerfManager.logAndDump(PerfManager::EAutoFocusEvent);
                parent->mAutoFocusThread->get()->request(*parent->mAutoFocusThreadData);
            }
#if 0 //added by dyron
            if ((OMX_ALL == nData1) &&
                    ((OMX_U32)parent->mOmxILExtIndex->getIndex(OmxILExtIndex::EMeteringLock) == nData2)) {
                parent->mPerfManager.logAndDump(PerfManager::EMeteringEvent);
                parent->mAutoFocusThread->get()->request(*parent->mAutoFocusThreadData);
            }
#endif
            if ((OMX_ALL == nData1) &&
                ((OMX_U32)parent->mOmxILExtIndex->getIndex(OmxILExtIndex::EPreCaptureExposureTime) == nData2)) {
                parent->mPerfManager.logAndDump(PerfManager::EPreCaptureExposureTimeEvent);
                //check for shake detection
                parent->dispatchRequest(&STECamera::checkForShakeDetection, NULL, false);
            }

            if ((OMX_Symbian_IndexConfigExtFocusStatus == nData2)
                    &&((parent->mMsgEnabled & CAMERA_MSG_FOCUS_MOVE))) {
                parent->mAutoFocusThread->get()->request(*parent->mAutoFocusThreadData);
            }
            if ((OMX_ALL == nData1) &&
                    ((OMX_U32)parent->mOmxILExtIndex->getIndex(OmxILExtIndex::EExposureInitiated) == nData2)) {
#ifdef DISABLE_EXPOSURE_INITIATED_EVENT_AFTER_CALLBACK
                parent->mOmxReqCbHandler.disable(OmxReqCbHandler::EExposureInitiated);
#endif //DISABLE_EXPOSURE_INITIATED_EVENT_AFTER_CALLBACK
                parent->mPerfManager.logAndDump(PerfManager::EExposureInitiatedEvent);
                sem_post(&shutter_eos);
            }
            break;
        default:
            DBGT_CRITICAL("Cant Happen");
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IspprocEventHandler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);

    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
            OmxUtils::name(eEvent),
            eEvent,
            nData1,
            nData2,
            pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pAppData, "Invalid aAppData");
    STECamera *parent = static_cast<STECamera *>(pAppData);

    switch (eEvent) {
        case OMX_EventCmdComplete:
            {
                OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
                DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

                if (cmd == OMX_CommandStateSet) {
                    OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                    DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                    switch (newState) {
                        case OMX_StateLoaded:
                            sem_post(&stateISP_sem);
                            break;
                        case OMX_StateIdle:
                            sem_post(&stateISP_sem);
                            break;
                        case OMX_StateExecuting:
                            sem_post(&stateISP_sem);
                            break;
                        case OMX_StatePause:
                            break;
                        case OMX_StateInvalid:
                            DBGT_CRITICAL("OMX_StateInvalid");
                            break;
                        default:
                            break;
                    }
                } else if (OMX_CommandPortEnable == cmd) {
                    sem_post(&stateISP_sem);
                } else if (OMX_CommandPortDisable == cmd) {
                    sem_post(&stateISP_sem);
                }
                break;
            }
        case OMX_EventBufferFlag:
            if ((nData1 == CAM_VPB + 2) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                DBGT_PTRACE("Got IspProc HR Port EOS");
                parent->mPerfManager.logAndDump(PerfManager::EIspProcHREOS);
            }

            if ((nData1 == CAM_VPB + 1) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                DBGT_PTRACE("Got IspProc LR Port EOS");
                parent->mPerfManager.logAndDump(PerfManager::EIspProcLREOS);
            }
            break;
        case OMX_EventError:
            DBGT_PTRACE("Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ",
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData2,
                    (OMX_ERRORTYPE) nData2);
            break;
        default:
            DBGT_PTRACE("Unknown event: %d", eEvent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JpegEventHandler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);

    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
            OmxUtils::name(eEvent),
            eEvent,
            nData1,
            nData2,
            pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    STECamera *parent = static_cast<STECamera *>(pAppData);

    switch (eEvent) {
        case OMX_EventCmdComplete:
            {
                OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
                DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

                if (cmd == OMX_CommandStateSet) {
                    OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                    DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                    switch (newState) {
                        case OMX_StateLoaded:
                            sem_post(&stateJpeg_sem);
                            break;
                        case OMX_StateIdle:
                            sem_post(&stateJpeg_sem);
                            break;
                        case OMX_StateExecuting:
                            sem_post(&stateJpeg_sem);
                            break;
                        case OMX_StatePause:
                            DBGT_PTRACE("OMX_StatePause");
                            break;
                        case OMX_StateInvalid:
                            DBGT_CRITICAL("OMX_StateInvalid");
                            break;
                        default:
                            break;
                    }
                } else if (OMX_CommandPortEnable == cmd) {
                    sem_post(&stateJpeg_sem);
                } else if (OMX_CommandPortDisable == cmd) {
                    sem_post(&stateJpeg_sem);
                }
                break;
            }
        case OMX_EventError:
            DBGT_PTRACE("Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ",
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData2,
                    (OMX_ERRORTYPE) nData2);
            break;
        case OMX_EventBufferFlag:
            {
                if ((nData1 == CAM_VPB + 1) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                    sem_post(&jpeg_eos_sem);
                    parent->mPerfManager.logAndDump(PerfManager::EJpegEOS);
                }
                break;
            }
        default:
            DBGT_PTRACE("Unknown event: %d", eEvent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CameraFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_PTR pAppData,
                                   OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);
    DBGT_PTRACE("PortIndex: %d", (int)pBuffer->nOutputPortIndex);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pBuffer->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");

    //ignore flush buffers
    if (0 == pBuffer->nFilledLen) {
        DBGT_ASSERT( (buffInfo->mFlags & OmxBuffInfo::ESentToComp),
                   "Invalid buffer got, not sent to comp Flags: 0x%08x", buffInfo->mFlags);
        //reset flag
        buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
        buffInfo->mFlags |= OmxBuffInfo::ECameraHal;    /* Now own by camera HAL */

        DBGT_PTRACE("Empty/Flush Buffer hsmcamera Port %lu", pBuffer->nOutputPortIndex);
        DBGT_EPILOG("");
        return err;
    }

    //check buffer not already sent
    DBGT_ASSERT( !(buffInfo->mFlags & OmxBuffInfo::ESentToHandlerThread),
                "Buffer: %p already sent to handler thread flags: 0x%08x",
                pBuffer, buffInfo->mFlags);

    //setup flag
    buffInfo->mFlags |= OmxBuffInfo::ESentToHandlerThread;

    DBGT_ASSERT(NULL != pAppData, "Invalid aAppData");
    STECamera *parent = static_cast<STECamera *>(pAppData);

    if (pBuffer->nOutputPortIndex == CAM_VPB + 0) {
        if (!parent->mIsFirstViewFinderFrame) {
            parent->mIsFirstViewFinderFrame = true;
            parent->mPerfManager.logAndDump(PerfManager::EFirstFrame);
            parent->dispatchRequest(&STECamera::setLensParameters, NULL, false);
        }
        g_lastPreviewOMXBuffInfo = buffInfo;

        if (parent->mIsTakeStillZSL) {
            buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
            buffInfo->mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
            err = OmxBuffInfo::fillBuffer(parent->mCam, pBuffer);
            DBGT_PTRACE("OMX_FillThisBuffer on mCam 0 pBuffer: %#x err: %#x",(unsigned int)pBuffer,err);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed err = %d", (int)err);
                return err;
            }
        } else {
            //request
            parent->mPreviewThread->get()->request(*buffInfo);
        }
    } else if (pBuffer->nOutputPortIndex == CAM_VPB + 1) {
        DBGT_PTRACE("mIsStillZSL: %d",parent->mIsStillZSL);
        if (parent->mIsStillZSL) {
            DBGT_PTRACE("mIsTakeStillZSL: %d",parent->mIsTakeStillZSL);
            if (parent->mIsTakeStillZSL) {
                DBGT_PTRACE("Send to Picture thread: %d",parent->mIsTakeStillZSL);
                parent->mIsTakeStillZSL = false;
                pVPB1ImageBuffer = pBuffer;
                buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
                sem_post(&isp_image_sem);

                B2R2Utills::TImageData srcImage, intImage;
                srcImage.mWidth = parent->mPreviewInfo.getOverScannedWidth();
                srcImage.mHeight = parent->mPreviewInfo.getOverScannedHeight();
                srcImage.mBufSize = paramPortVPB0.nBufferSize;
                srcImage.mLogAddr = (void*)g_lastPreviewOMXBuffInfo->mMMHwBufferInfo.iLogAddr;
                srcImage.mPhyAddr = (void*)g_lastPreviewOMXBuffInfo->mMMHwBufferInfo.iPhyAddr;
                srcImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;


                intImage.mWidth = parent->mConfigPreviewWidth;
                intImage.mHeight = parent->mConfigPreviewHeight;
                intImage.mBufSize = paramISPoutput1.nBufferSize;
                intImage.mLogAddr = (void*)parent->mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
                intImage.mPhyAddr = (void*)parent->mRawOmxBuffInfo.mMMHwBufferInfo.iPhyAddr;
                intImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;

                if (parent->mCurrentZoomLevel != 0) {
                    B2R2Utills::TRect cropRect;
                    parent->getCropVectorForZoom(cropRect.mX, cropRect.mY, cropRect.mWidth, cropRect.mHeight, true);

                    err = (OMX_ERRORTYPE)B2R2Utills::crop_resize(&srcImage, &intImage, &intImage, &cropRect, 1, false);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("B2R2Utills::crop_resize() failed");
                        DBGT_EPILOG("");
                        return OMX_ErrorUndefined;
                    }
                } else {
                    err = (OMX_ERRORTYPE)B2R2Utills::resize(&srcImage, &intImage);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("B2R2Utills::resize() failed");
                        DBGT_EPILOG("");
                        return OMX_ErrorUndefined;
                    }
                }
                sem_post(&isp_snap_sem);
            } else {
                buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
                buffInfo->mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
                err = OmxBuffInfo::fillBuffer(parent->mCam, pBuffer);
                DBGT_PTRACE("OMX_FillThisBuffer on mCam 1 pBuffer: %#x err: %#x",(unsigned int)pBuffer,err);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("OMX_FillThisBuffer failed err = %d", (int)err);
                    return err;
                }
            }
        } else {
            buffInfo->mFlags &= ~OmxBuffInfo::ESentToComp;
            pISPinBuffer->nFilledLen = pBuffer->nFilledLen;
            pISPinBuffer->nFlags = pBuffer->nFlags;
            err = OmxBuffInfo::emptyBuffer(parent->mISPProc,pISPinBuffer);
            DBGT_PTRACE("OMX_EmptyThisBuffer on mISPProc 0 pISPinBuffer: %p err: %#x",pISPinBuffer,(unsigned int)err);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_EmptyThisBuffer failed err = %d", (int)err);
                return err;
            }
        }
    } else if (pBuffer->nOutputPortIndex == CAM_VPB + 2) {
        if (!parent->mIsFirstVideoRecordFrame) {
            parent->mIsFirstVideoRecordFrame = true;
            parent->mPerfManager.logAndDump(PerfManager::EFirstFrame);
        }

        //request
        if (parent->mHiResVFEnabled) {
            parent->mPreviewThread->get()->request(*buffInfo);
        } else {
            parent->mRecordThread->get()->request(*buffInfo);
            video_metadata_t *meta_data = (video_metadata_t *)((*buffInfo).mCamHandle->data);
            buffer_handle_t buf = (buffer_handle_t)meta_data->handle;

            for (int i = 0 ; i < parent->kRecordBufferCount; i++) {
                if (buf == parent->mRecordGraphicBuffer[i].get()->handle) {
                    parent->mRecordIdx = i;
                    break;
                }
            }

            if (parent->mEnableB2R2DuringRecord) {
                // Signal the B2R2Thread to do the processing of the buffer
                sem_post(&sem_b2r2);
                // Wait for B2R2Thread to start processing
                sem_wait(&sem_b2r2_reset);
            }

            //Special handling for 1080 video record case with snapshot
            if (parent->mSnapshotDuringRecord
                    && (parent->mSnapshotDuringRecIdx == -1)
                    && (parent->mRecordInfo.getHeight() == SNAPSHOT_HEIGHT_2MP)) {
                sem_post(&sem_b2r2);
            }
        }
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE ExifFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                 OMX_IN OMX_PTR pAppData,
                                 OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    DBGT_PTRACE("Size: %lu\n", pBuffer->nFilledLen);

    sem_post(&exif_sem);

    DBGT_PTRACE("EXIF FIll buffer Size: %lu\n", pBuffer->nFilledLen);

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ExifEventHandler(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,
        OMX_U32 nData1,
        OMX_U32 nData2,
        OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);

    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
            OmxUtils::name(eEvent),
            eEvent,
            nData1,
            nData2,
            pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

    STECamera *parent = static_cast<STECamera *>(pAppData);
    switch (eEvent) {
        case OMX_EventCmdComplete:
            {
                OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
                DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

                if (cmd == OMX_CommandStateSet) {
                    OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                    DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                    switch (newState) {
                        case OMX_StateLoaded:
                            sem_post(&stateEXIF_sem);
                            break;
                        case OMX_StateIdle:
                            sem_post(&stateEXIF_sem);
                            break;
                        case OMX_StateExecuting:
                            sem_post(&stateEXIF_sem);
                            break;
                        case OMX_StatePause:
                            DBGT_PTRACE("OMX_StatePause");
                            break;
                        case OMX_StateInvalid:
                            DBGT_CRITICAL("OMX_StateInvalid");
                            break;
                        default:
                            break;
                    }
                } else if (OMX_CommandPortEnable == cmd) {
                    sem_post(&stateEXIF_sem);
                } else if (OMX_CommandPortDisable == cmd) {
                    sem_post(&stateEXIF_sem);
                }
                break;
            }
        case OMX_EventError:
            DBGT_CRITICAL("Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ",
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData2,
                    (OMX_ERRORTYPE) nData2);
            break;
        case OMX_EventBufferFlag:
            {
                if ((nData1 == 2) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                    sem_post(&exif_eos_sem);
                    parent->mPerfManager.logAndDump(PerfManager::EExifMixerEOS);
                }
                break;
            }
        default:
            DBGT_PTRACE("Unknown event: %d", eEvent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

#undef _CNAME_
#define _CNAME_ STECamera
STECamera::STECamera( int cameraId ) : mLock("STECamera::Lock"),
    mTakePictureLock("STECamera::TakePictureLock"),
    mPrevLock("STECamera::PreviewLock"),
    mCam(0),
    mParameters(),
    mPreviewWindow(NULL),
    mISPImageHeap(0),
    mPreviewTempBuffer(0),
    mCamMode(EStill),
    mStillZslBufferCount(2),
    mPreviewRunning(false),
    mPreviewFrameSize(0),
    mRecordFrameSize(0),
    mRecordRunning(false),
    mPreviousCamMode(EInvalid),
    mCurrentRegCoordinateNum(0),
    mSmoothZoomLevel(0),
    mSmoothZoomStopPending(false),
    mUpdateAutoWhiteBalanceLock(false),
    mUpdateAutoExposureLock(false),
    mNotifyCb(0),
    mDataCb(0),
    mDataCbTimestamp(0),
    mCallbackCookie(0),
    mMsgEnabled(0),
    mIsCameraConfigured(0),
    mIsPictureCancelled(false),
    mIspreviewsizechanged(false),
    mIsrecordsizechanged(false),
    mConfigPreviewHeight(0),
    mConfigPreviewWidth(0),
    mConfigFrameRate(0),
    mCameraId(cameraId),
    mIsStillZSL(false),
    mIsTakeStillZSL(false),
#ifdef DUMP_FPS_FOR_VF
    mFrameRateDumperVF("ViewFinder", DynSetting::EVfFpsLogger),
#else //!DUMP_FPS_FOR_VF
    mFrameRateDumperVF(),
#endif //DUMP_FPS_FOR_VF
#ifdef DUMP_FPS_FOR_VIDEO
    mFrameRateDumperVideo("Video", DynSetting::EVideoFpsLogger),
#else //!DUMP_FPS_FOR_VIDEO
    mFrameRateDumperVideo(),
#endif //DUMP_FPS_FOR_VIDEO
    mOmxILExtIndex(NULL),
    mCameraFormatInfoIndex(-1),
    mIsFirstInstanceViewFinder(false),
    mIsFirstViewFinderFrame(false),
    mIsFirstVideoRecordFrame(false),
    mSwProcessingThread(NULL),
    mPreviewThread(NULL),
    mRecordThread(NULL),
    mAutoFocusThreadData(NULL),
    mAutoFocusThread(NULL),
    mReqHandlerThreadData(NULL),
    mReqHandlerThread(NULL),
    mHiResVFEnabled(false),
    mFocusHandler(NULL),
    mAutoFocusHandler(NULL),
    mContinuousAutoFocusHandler(NULL),
    mFixedFocusHandler(NULL),
    mThumbnailHandler(NULL),
    mEnableDataFlow(NULL),
    mDisableDataFlow(NULL),
    mImageRotHandler(NULL),
    mDeviceHwRotation(NULL),
    mDeviceSwRotation(NULL),
    mVideoStabEnabled(false),
#ifdef ENABLE_FACE_DETECTION
    mFaceDetector(NULL),
    mFaceDetection(false),
#endif
    mSnapshotDuringRecord(false),
    mSnapshotDuringRecIdx(-1),
    mRecordIdx(-1),
    mSlowMotionFPS(0),
    mVideoTS(0),
    mEnableB2R2DuringRecord(true),
    mRecordBlitter(NULL)
{
    DBGT_PROLOG("");

    mSceneCtrl = NULL;

    memset(&mPreviewHeap,0x00, sizeof(mPreviewHeap));
    memset(&mRecordHeap,0x00, sizeof(mRecordHeap));
    memset(&mGraphicBuffer,0x00, sizeof(mGraphicBuffer));
    if (mEnableB2R2DuringRecord) {
        for (int i = 0; i < kRecordBufferCount; i++) {
            mRecordMMNativeBuf[i] = NULL;
        }
    }

    // get the omx pixel format on record path
    char pixFmt[32]; DynSetting::get(DynSetting::ERecordPixFmt, pixFmt);
    mOmxRecordPixFmt = getOmxPixFmtFromKeyStr(pixFmt);

    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::init()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    int status = NO_ERROR;
    int seminit_err = -1;

    //initialze perf manager
    status = mPerfManager.connect();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - PerfManager connect failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    //Perf trace to initialize first Camera Instance
    mPerfManager.init(PerfManager::EFirstCameraInstance);

    //initialze omxutils
    err = mOmxUtils.init();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OmxUtils init failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }

    status = initThreads();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - InitThreads failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err = (mOmxUtils.interface()->GetpOMX_Init())();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_Init init failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }

    seminit_err = sem_init(&jpeg_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - jpeg_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&stateCam_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - stateCam_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&stateISP_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - stateISP_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&stateJpeg_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - stateJpeg_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&record_eos_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - record_eos_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&video_rot_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - video_rot_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&shutter_eos, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - shutter_eos sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&isp_snap_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - isp_snap_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&isp_image_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - isp_image_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&jpeg_eos_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - jpeg_eos_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&stateEXIF_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - stateEXIF_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&exif_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - exif_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&exif_eos_sem, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - exif_eos_sem sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&still_during_record, 0, 0);
    if (-1 == seminit_err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - still_during_record sem_init failed");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&sem_b2r2, 0, 0);
    if (-1 == seminit_err) {
       DBGT_CRITICAL("sem_b2r2 sem init failed err = %d", (int)seminit_err);
       DBGT_EPILOG("");
       return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&sem_b2r2_reset, 0, 0);
    if (-1 == seminit_err) {
       DBGT_CRITICAL("sem_b2r2_reset sem init failed err = %d", (int)seminit_err);
       DBGT_EPILOG("");
       return OMX_ErrorInsufficientResources;
    }

    seminit_err = sem_init(&sem_crop_buffer, 0, 0);
    if (-1 == seminit_err) {
       DBGT_CRITICAL("sem_crop_buffer sem init failed err = %d", (int)seminit_err);
       DBGT_EPILOG("");
       return OMX_ErrorInsufficientResources;
    }

    camCallback.EmptyBufferDone = 0;
    camCallback.FillBufferDone = CameraFillBufferDone;
    camCallback.EventHandler = CameraEventHandler;

    if (mCameraId == CAMERA_FACING_BACK) {
        //initialize default params
        err = initPrimaryDefaultParameters();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("initPrimaryDefaultParameters failed err = %d", (int)err);
            DBGT_EPILOG("");
            return err;
        }

        err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mCam, omxNamePrimary, this, &camCallback);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Primary Camera GetHandle failed err = %d", (int)err);
            DBGT_EPILOG("");
            return err;
        }
        OmxUtils::setOMXComponentName(mCam, omxNamePrimary, OmxUtils::EOMXCamera);
    } else {
        //initialize default params
        err = initSecondaryDefaultParameters();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("initSecondaryDefaultParameters failed err = %d", (int)err);
            DBGT_EPILOG("");
            return err;
        }

        err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mCam, omxNameSecondary, this, &camCallback);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Secondary Camera GetHandle failed err = %d", (int)err);
            DBGT_EPILOG("");
            return err;
        }
        OmxUtils::setOMXComponentName(mCam, omxNameSecondary, OmxUtils::EOMXCamera);
    }

#ifdef ENABLE_FACE_DETECTION
    mParameters.set(CameraParameters::KEY_MAX_NUM_DETECTED_FACES_HW, "10");
#endif //ENABLE_FACE_DETECTION

#if defined(SWISPPROC) || defined(HWISPPROC)
    ispCallback.EmptyBufferDone = IspprocEmptyBufferDone;
    ispCallback.FillBufferDone = IspprocFillBufferDone;
    ispCallback.EventHandler = IspprocEventHandler;

    err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mISPProc, ispproc, this, &ispCallback);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("ISPProc GetHandle failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }
    OmxUtils::setOMXComponentName(mISPProc, ispproc, OmxUtils::EOMXISPProc);
#endif // SWISPPROC || HWISPPROC

#ifdef JPEGENC
    jpegCallback.EmptyBufferDone = JpegEmptyBufferDone;
    jpegCallback.FillBufferDone = JpegFillBufferDone;
    jpegCallback.EventHandler = JpegEventHandler;

    err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mJpegEnc, jpegenc, this, &jpegCallback);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("JpegEnc GetHandle failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }
    OmxUtils::setOMXComponentName(mJpegEnc, jpegenc, OmxUtils::EOMXJPEGEnc);
#endif // JPEGENC

    exifCallback.EmptyBufferDone  = NULL;
    exifCallback.FillBufferDone   = ExifFillBufferDone;
    exifCallback.EventHandler     = ExifEventHandler;

    err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mExifMixer, exifmixer, this, &exifCallback);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("exifmixer GetHandle failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }
    OmxUtils::setOMXComponentName(mExifMixer, exifmixer, OmxUtils::EOMXExifMixer);

    mOmxILExtIndex = new OmxILExtIndex(mCam);
    if (NULL == mOmxILExtIndex) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for OmxILExtIndex");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    //init all indexes
    err = mOmxILExtIndex->init();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL(" OmxILExtIndex init failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }

    //init all global param
    status = initGlobalParams();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - initGlobalParams failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    //init processing buffers
    status = initProcessingBuffers();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - initProcessingBuffer failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    //setup request callback handler
    setupReqCbHandlers();

    err = setLensParameters();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_ErrorUndefined - setLensParameters failed err = %d", (int)err);
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }


#ifdef ENABLE_FACE_DETECTION

#if (FACE_DETECTION_ALGO_TYPE == NEVEN)
    mFaceDetector = new NevenFaceDetector();
#endif

    if (mFaceDetector == NULL) {
        DBGT_CRITICAL("Face detection object creation failed");
    }
#endif
    status = CamSwRoutines::init();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorUndefined - failed to load SwRoutine Symbols failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }

    status = STECamMMHwBuffer::init();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("failed to load MMHwBuffers Symbols");
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }

    err = initThumbnailHandler();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - initThumbnailHandler failed err = %d", (int)err);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    status = initFocusHandler();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - initFocusHandler failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    status = initImageRotHandler();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - initImageRotHandler failed status = %d", status);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err = disableAllCamPorts();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_ErrorUndefined - disableAllCamPorts failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::initThumbnailHandler()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mEnableDataFlow = new EnableDataFlow(mExifMixer, mArmIvOmxBuffInfo, mArmIvOmxBuffInfo.mMMHwChunkMetaData,
                                         mOmxUtils, cameraFormatInfo().mPreviewOmxColorFormat,
                                         mPerfManager, stateEXIF_sem);
    if (NULL == mEnableDataFlow) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for EnableDataFlow");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err = mEnableDataFlow->init(mThumbnailInfo, mPreviewInfo);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - mEnableDataFlow->init() failed err = %d", (int)err);
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    mDisableDataFlow = new DisableDataFlow();
    if (NULL == mDisableDataFlow) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for mDisableDataFlow");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    mThumbnailHandler = mEnableDataFlow;

    mThumbnailInfo.setupImageInfo(DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, DEFAULT_PICTURE_QUALITY, DEFAULT_PICTURE_ROTATION);

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::initFocusHandler()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;

    mAutoFocusHandler = new AutoFocusHandler(this);
    if (NULL == mAutoFocusHandler) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mAutoFocusHandler");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    mContinuousAutoFocusHandler = new ContinuousAutoFocusHandler(this);
    if (NULL == mContinuousAutoFocusHandler) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mContinuousAutoFocusHandler");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    mFixedFocusHandler = new FixedFocusHandler(this);
    if (NULL == mFixedFocusHandler) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mFixedFocusHandler");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    //Set default focus mode to auto focus.
    mFocusHandler = mAutoFocusHandler;

    DBGT_EPILOG("");
    return status;
}

status_t STECamera::initImageRotHandler()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;

#if ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_DEVICE
    mDeviceHwRotation = new DeviceHwRotation(this);
    if (NULL == mDeviceHwRotation) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mDeviceHwRotation");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    mDeviceSwRotation = new DeviceSwRotation(this);
    if (NULL == mDeviceSwRotation) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mDeviceSwRotation");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }
#elif ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_EXIF
    mImageRotHandler = new ExifRotation(this);
    if (NULL == mImageRotHandler) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mImageRotHandler");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }
#else
    mImageRotHandler = new ImageRotHandler(this);
    if (NULL == mImageRotHandler) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mImageRotHandler");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }
#endif //ENABLE_IMAGE_ROTATION

    DBGT_EPILOG("");
    return status;
}

void STECamera::setupReqCbHandlers()
{
    DBGT_PROLOG("");

    //Auto focus status
    mOmxReqCbHandler.init(OmxReqCbHandler::EAutoFocusStatus,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusStatus));
    //Auto focus lock
    mOmxReqCbHandler.init(OmxReqCbHandler::EAutoFocusLock,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusLock));

    //Exposure Initiated
    mOmxReqCbHandler.init(OmxReqCbHandler::EExposureInitiated,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EExposureInitiated));

    //PreCapture Exposure time
    mOmxReqCbHandler.init(OmxReqCbHandler::EPreCaptureExposureTime,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EPreCaptureExposureTime));

#if 0 //added by dyron
    //Smooth Zoom Status
    mOmxReqCbHandler.init(OmxReqCbHandler::ESmoothZoomStatus,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EZoomMode));
    //Metering Lock
    mOmxReqCbHandler.init(OmxReqCbHandler::EMeteringLock,
                          mCam,
                          CAM_VPB + 1,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EMeteringLock));

#endif
    //Continuous focus status
    mOmxReqCbHandler.init(OmxReqCbHandler::EExtFocusStatus,
                          mCam,
                          CAM_VPB + 0,
                          mOmxILExtIndex->getIndex(OmxILExtIndex::EExtFocusStatus));

    DBGT_EPILOG("");
}

status_t STECamera::initGlobalParams()
{
    DBGT_PROLOG("");

    status_t err = NO_ERROR;

    //Port definitions
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramPortVPB0);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramPortVPB1);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramPortVPB2);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramISPinput);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramISPoutput);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramJPEGinput);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramJPEGoutput);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramISPoutput1);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramEXIFinput2);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramEXIFoutput);


    //Sensor mode type
    OmxUtils::StructWrapper<OMX_PARAM_SENSORMODETYPE>::init(sensorMode);

    //init global variables
    isStartPreview = false;

    resetProperties();
    mParameters.set(KEY_ZERO_SHUTTER_LAG_MODE, zslOff);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::initPrimaryDefaultParameters()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;


    g_IsPreviewConfig = false;

    DBGT_PTRACE("Default preview Witdh: %d Height: %d", DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT);

    mParameters.setPreviewSize(DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT);
    // Set the overscanned preview width and height as default preview width and height

    DBGT_PTRACE("Default preview framerate: %d", DEFAULT_PREVIEW_FRAMERATE);
    mConfigFrameRate = DEFAULT_PRIMARY_PREVIEW_FRAMERATE;

    //setPreviewFrameRate to the default framerate in case client does not set any framerate
    mParameters.setPreviewFrameRate(mConfigFrameRate);

    mParameters.setVideoSize(DEFAULT_VIDEO_RECORD_WIDTH,DEFAULT_VIDEO_RECORD_HEIGHT);
    DBGT_PTRACE("Default video record Witdh: %d Height: %d", DEFAULT_VIDEO_RECORD_WIDTH, DEFAULT_VIDEO_RECORD_HEIGHT);

    /* This is SAS list.Customer specific list need to be added  */
    mParameters.set(CameraParameters::KEY_SUPPORTED_VIDEO_SIZES, "1920x1080,1280x720,720x480,640x480,352x288,320x240,176x144,128x96");
    mParameters.set(CameraParameters::KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO, "1024x768");

    DBGT_PTRACE("Default picture Width: %d Height: %d Quality %d ", DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT, DEFAULT_PICTURE_QUALITY);
    mParameters.setPictureSize(DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT);
    mParameters.set(CameraParameters::KEY_JPEG_QUALITY, DEFAULT_PICTURE_QUALITY);


    DBGT_PTRACE("Default thumbnail Width: %d  Height: %d  Quality: %d", DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, DEFAULT_PICTURE_QUALITY);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_WIDTH);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, DEFAULT_THUMBNAIL_HEIGHT);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, DEFAULT_PICTURE_QUALITY);

    //All supported vales
    //Preview
    String8 spf;
    Append_PreviewFormats(spf);
    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS, spf.string());
    mParameters.setPreviewFormat(DEFAULT_PREVIEW_FORMAT);

    //init preview format
    initPreviewOmxColorFormat(DEFAULT_PREVIEW_FORMAT);

    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES,
        CAM_SUPPORTED_PREVIEW_SIZES);

#ifdef ENABLE_HIGH_RESOLUTION_VF
    Append_Val((String8)CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES,
        (String8)CAM_EXTENDED_PREVIEW_SIZES);
#endif //ENABLE_HIGH_RESOLUTION_VF

    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, "0,10,15,20,30");
    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, "(10000,10000),(15000,15000),(20000,20000),(10000,30000),(30000,30000)");
    //Do not set the new fps API, to get to know if IL client based on new API is present
    mParameters.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, "30000,30000");

    //Picture
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS, CameraParameters::PIXEL_FORMAT_JPEG);

#if defined(STE_SENSOR_TOSHIBA)
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, "3264x2448,2592x1944,2048x1536,1600x1200");
#elif defined(STE_SENSOR_8MP) //STE_SENSOR_8MP
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES,
                    "3264x2448,3264x1856,2592x1600,2592x1592,2592x1456,2048x1536,2048x1152,1280x960,1280x720,1024x768,640x480,640x368,512x384,400x400,272x272,176x144");
#else
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES,
                    "2592x1944,2560x1440,2592x1458,2048x1536,1920x1080,1632x1224,1280x720,1600x1200,1024x768,640x480,352x288,320x240,320x200,176x144");
#endif

    mParameters.set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, "0x0,320x240,160x120");
    mParameters.set(CameraParameters::KEY_JPEG_QUALITY, "90");
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, "90");
    mParameters.setPictureFormat(CameraParameters::PIXEL_FORMAT_JPEG);

    //Video
    char pixFmt[32];
    getKeyStrFromOmxPixFmt(pixFmt, mOmxRecordPixFmt);
    mParameters.set(CameraParameters::KEY_VIDEO_FRAME_FORMAT, pixFmt);

    mParameters.set(CameraParameters::KEY_ZOOM, "0");
    mParameters.set(CameraParameters::KEY_MAX_ZOOM, "6");
    mParameters.set(CameraParameters::KEY_ZOOM_RATIOS, "100,150,200,250,300,350,400");
    mParameters.set(CameraParameters::KEY_ZOOM_SUPPORTED, "true");

#ifdef ENABLE_SMOOTH_ZOOM
    mParameters.set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED, "true");
#else //!ENABLE_SMOOTH_ZOOM
    mParameters.set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED, "false");
#endif//ENABLE_SMOOTH_ZOOM

#ifdef ENABLE_VIDEO_STAB
    mParameters.set(CameraParameters::KEY_VIDEO_STABILIZATION_SUPPORTED, "true");
#else // !ENABLE_VIDEO_STAB
    mParameters.set(CameraParameters::KEY_VIDEO_STABILIZATION_SUPPORTED, "false");
#endif // ENABLE_VIDEO_STAB

    String8 wb, scene_mode, flash_mode, focus_mode, flicker_mode, color_effect, metering_mode, region_control;
    String8 wb_set, scene_mode_set, flash_mode_set, focus_mode_set, flicker_mode_set, color_effect_set, metering_mode_set, region_control_set ;

    Append_WhiteBalance(wb, wb_set);
    Append_SceneMode(scene_mode, scene_mode_set);
    Append_FlashMode(flash_mode, flash_mode_set);
    Append_FocusMode(focus_mode, focus_mode_set);
    Append_FlickerRemovalMode(flicker_mode, flicker_mode_set);
    Append_ColorEffect(color_effect, color_effect_set);
    Append_MeteringMode(metering_mode, metering_mode_set);
    Append_RegionControl(region_control, region_control_set);

    mParameters.set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE, wb.string());
    mParameters.set(CameraParameters::KEY_WHITE_BALANCE, wb_set.string());

    mParameters.set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES, focus_mode.string()); //infinity not supported in OMX. fixed, edof not supported in the array/string.xml
    mParameters.set(CameraParameters::KEY_FOCUS_MODE, focus_mode_set.string());
    mParameters.set(CameraParameters::KEY_FOCUS_DISTANCES, "0.10,Infinity,Infinity");

    mParameters.set(KEY_CONTRAST, DEFAULT_SENSOR_CONTRAST);
    mParameters.set(KEY_MAX_CONTRAST, "100");

    mParameters.set(KEY_MIN_CONTRAST, "-100");
    mParameters.set(KEY_CONTRAST_STEP, "1");

    mParameters.set(KEY_BRIGHTNESS, DEFAULT_SENSOR_BRIGHTNESS);
    mParameters.set(KEY_MAX_BRIGHTNESS, "100");

    mParameters.set(KEY_MIN_BRIGHTNESS, "0");
    mParameters.set(KEY_BRIGHTNESS_STEP, "1");

    mParameters.set(KEY_SATURATION, DEFAULT_SENSOR_SATURATION);
    mParameters.set(KEY_MAX_SATURATION, "100");

    mParameters.set(KEY_MIN_SATURATION, "-100");
    mParameters.set(KEY_SATURATION_STEP, "1");

    mParameters.set(KEY_SHARPNESS, DEFAULT_SENSOR_SHARPNESS);
    mParameters.set(KEY_MAX_SHARPNESS, "100");

    mParameters.set(KEY_MIN_SHARPNESS, "-100");
    mParameters.set(KEY_SHARPNESS_STEP, "1");

    mParameters.set(KEY_ISO, DEFAULT_SENSOR_ISO);
    mParameters.set(KEY_SUPPORTED_ISO, "ISO_AUTO,100,200,400,800,1600,3200");

    mParameters.set(KEY_REGION_CONTROL, region_control_set.string());
    mParameters.set(KEY_SUPPORTED_REGION_CONTROL, region_control.string());

#ifdef PROP_COLOR_EFFECT
    mParameters.set(CameraParameters::KEY_SUPPORTED_EFFECTS, color_effect.string());
    mParameters.set(CameraParameters::KEY_EFFECT, color_effect_set.string());
#endif

// ER 325568 [UCV_MM] - TakePic/ET Crash when selecting restore
// The crash issue could be suppress with support of auto setting or remove flash mode.
#ifdef PROP_FLASH
    mParameters.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES, flash_mode.string()); //auto, red-eye, torch yet to be supported
    mParameters.set(CameraParameters::KEY_FLASH_MODE, flash_mode_set.string());
#endif

    mParameters.set(CameraParameters::KEY_SUPPORTED_ANTIBANDING, flicker_mode.string());
    mParameters.set(CameraParameters::KEY_ANTIBANDING, flicker_mode_set.string());

    mParameters.set(CameraParameters::KEY_SUPPORTED_SCENE_MODES, scene_mode.string());
    mParameters.set(CameraParameters::KEY_SCENE_MODE, scene_mode_set.string());

    mParameters.set(KEY_SUPPORTED_METERING, metering_mode.string());
    mParameters.set(KEY_METERING_EXPOSURE, metering_mode_set.string());

    mParameters.set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION, "6");
    mParameters.set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION, "-6");
    mParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP, "0.333334");
    mParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION, "0");

    mParameters.set(CameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED, "true");

    mParameters.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED,"true");
    mParameters.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED,"true");
    mParameters.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK,"false");
    mParameters.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,"false");

#ifdef ENABLE_HIGH_RESOLUTION_VF
    mParameters.set(KEY_HIRES_VIEWFINDER, "0");
#endif //ENABLE_HIGH_RESOLUTION_VF

    // for Panaroma View
    mParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE,"54.8");
    mParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "42.5");

    mParameters.set(CameraParameters::KEY_MAX_NUM_FOCUS_AREAS, "1");
    mParameters.set(CameraParameters::KEY_MAX_NUM_METERING_AREAS, "1");

    mPreviewInfo.setupImageInfo(DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT, 0, 0, cameraFormatInfo().mWidthAlignment, cameraFormatInfo().mHeightAlignment);
    mThumbnailInfo.setupImageInfo(DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, DEFAULT_PICTURE_QUALITY, DEFAULT_PICTURE_ROTATION);
    mPictureInfo.setupImageInfo(DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT, DEFAULT_PICTURE_QUALITY, DEFAULT_PICTURE_ROTATION);
    mRecordInfo.setupImageInfo(DEFAULT_PREVIEW_WIDTH,DEFAULT_PREVIEW_HEIGHT, 0, 0);

    setupExtraKeys();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::initSecondaryDefaultParameters()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;


    g_IsPreviewConfig = false;

    DBGT_PTRACE("Default preview Witdh: %d Height: %d", DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT);

    mParameters.setPreviewSize(DEFAULT_SECONDARY_PREVIEW_WIDTH, DEFAULT_SECONDARY_PREVIEW_HEIGHT);

    DBGT_PTRACE("Default preview framerate: %d", DEFAULT_SECONDARY_PREVIEW_FRAMERATE);
    mConfigFrameRate = DEFAULT_SECONDARY_PREVIEW_FRAMERATE;

    //setPreviewFrameRate to the default framerate in case client does not set any framerate
    mParameters.setPreviewFrameRate(mConfigFrameRate);

    mParameters.setVideoSize(DEFAULT_SECONDARY_VIDEO_RECORD_WIDTH,DEFAULT_SECONDARY_VIDEO_RECORD_HEIGHT);
    DBGT_PTRACE("Default video record Witdh: %d Height: %d", DEFAULT_SECONDARY_VIDEO_RECORD_WIDTH,DEFAULT_SECONDARY_VIDEO_RECORD_HEIGHT);

    /* This is SAS list.Customer specific list need to be added  */
    mParameters.set(CameraParameters::KEY_SUPPORTED_VIDEO_SIZES, "720x480,640x480,352x288,320x240,176x144,128x96");


    mParameters.set(CameraParameters::KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO, "640x480");

    DBGT_PTRACE("Default picture Width: %d Height: %d Quality %d ", DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT, DEFAULT_PICTURE_QUALITY);
    mParameters.setPictureSize(DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT);
    mParameters.set(CameraParameters::KEY_JPEG_QUALITY, DEFAULT_PICTURE_QUALITY);


    DBGT_PTRACE("Default thumbnail Width: %d  Height: %d  Quality: %d", DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, DEFAULT_PICTURE_QUALITY);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_WIDTH);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, DEFAULT_THUMBNAIL_HEIGHT);
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, DEFAULT_PICTURE_QUALITY);

    //All supported vales
    //Preview
    String8 spf;
    Append_PreviewFormats(spf);
    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS, spf.string());
    mParameters.setPreviewFormat(DEFAULT_PREVIEW_FORMAT);

    //init preview format
    initPreviewOmxColorFormat(DEFAULT_PREVIEW_FORMAT);
    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES, "640x480,352x288,320x240,176x144");

    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, "0,10,15,20,30");
    mParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, "(10000,10000),(15000,15000),(20000,20000),(10000,30000),(30000,30000)");
    //Do not set the new fps API, to get to know if IL client based on new API is present
    mParameters.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, "30000,30000");

    //Picture
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS, CameraParameters::PIXEL_FORMAT_JPEG);
    mParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES,
            "2592x1944,2560x1440,2592x1458,2048x1536,1920x1080,1632x1224,1280x720,1600x1200,1024x768,640x480,352x288,320x240,176x144");
    mParameters.set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, "0x0,320x240,160x120");
    mParameters.set(CameraParameters::KEY_JPEG_QUALITY, "90");
    mParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, "90");
    mParameters.setPictureFormat(CameraParameters::PIXEL_FORMAT_JPEG);

    //Video
    char pixFmt[32];
    getKeyStrFromOmxPixFmt(pixFmt, mOmxRecordPixFmt);
    mParameters.set(CameraParameters::KEY_VIDEO_FRAME_FORMAT, pixFmt);

    mParameters.set(CameraParameters::KEY_ZOOM, "0");
    mParameters.set(CameraParameters::KEY_MAX_ZOOM, "6");
    mParameters.set(CameraParameters::KEY_ZOOM_RATIOS, "100,150,200,250,300,350,400");
    mParameters.set(CameraParameters::KEY_ZOOM_SUPPORTED, "true");

#ifdef ENABLE_SMOOTH_ZOOM
    mParameters.set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED, "true");
#else //!ENABLE_SMOOTH_ZOOM
    mParameters.set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED, "false");
#endif//ENABLE_SMOOTH_ZOOM

#ifdef ENABLE_VIDEO_STAB
    mParameters.set(CameraParameters::KEY_VIDEO_STABILIZATION_SUPPORTED, "true");
#else // !ENABLE_VIDEO_STAB
    mParameters.set(CameraParameters::KEY_VIDEO_STABILIZATION_SUPPORTED, "false");
#endif // ENABLE_VIDEO_STAB

    String8 wb, scene_mode, flash_mode, focus_mode, flicker_mode, color_effect, metering_mode, region_control;
    String8 wb_set, scene_mode_set, flash_mode_set, focus_mode_set, flicker_mode_set, color_effect_set, metering_mode_set, region_control_set ;

    Append_WhiteBalance(wb, wb_set);
    Append_SceneMode(scene_mode, scene_mode_set);
    Append_FlashMode(flash_mode, flash_mode_set);
    Append_FocusMode(focus_mode, focus_mode_set);
    Append_FlickerRemovalMode(flicker_mode, flicker_mode_set);
    Append_ColorEffect(color_effect, color_effect_set);
    Append_MeteringMode(metering_mode, metering_mode_set);
    Append_RegionControl(region_control, region_control_set);

    mParameters.set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE, wb.string());
    mParameters.set(CameraParameters::KEY_WHITE_BALANCE, wb_set.string());

    mParameters.set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES, focus_mode.string()); //infinity not supported in OMX. fixed, edof not supported in the array/string.xml
    mParameters.set(CameraParameters::KEY_FOCUS_MODE, focus_mode_set.string());
    mParameters.set(CameraParameters::KEY_FOCUS_DISTANCES, "0.10,Infinity,Infinity");

    mParameters.set(KEY_CONTRAST, DEFAULT_SENSOR_CONTRAST);
    mParameters.set(KEY_MAX_CONTRAST, "100");

    mParameters.set(KEY_MIN_CONTRAST, "-100");
    mParameters.set(KEY_CONTRAST_STEP, "1");

    mParameters.set(KEY_BRIGHTNESS, DEFAULT_SENSOR_BRIGHTNESS);
    mParameters.set(KEY_MAX_BRIGHTNESS, "100");

    mParameters.set(KEY_MIN_BRIGHTNESS, "0");
    mParameters.set(KEY_BRIGHTNESS_STEP, "1");

    mParameters.set(KEY_SATURATION, DEFAULT_SENSOR_SATURATION);
    mParameters.set(KEY_MAX_SATURATION, "100");

    mParameters.set(KEY_MIN_SATURATION, "-100");
    mParameters.set(KEY_SATURATION_STEP, "1");

    mParameters.set(KEY_SHARPNESS, DEFAULT_SENSOR_SHARPNESS);
    mParameters.set(KEY_MAX_SHARPNESS, "100");

    mParameters.set(KEY_MIN_SHARPNESS, "-100");
    mParameters.set(KEY_SHARPNESS_STEP, "1");

    mParameters.set(KEY_ISO, DEFAULT_SENSOR_ISO);
    mParameters.set(KEY_SUPPORTED_ISO, "ISO_AUTO,100,200,400,800,1600,3200");

    mParameters.set(KEY_REGION_CONTROL, region_control_set.string());
    mParameters.set(KEY_SUPPORTED_REGION_CONTROL, region_control.string());

#ifdef PROP_COLOR_EFFECT
    mParameters.set(CameraParameters::KEY_SUPPORTED_EFFECTS, color_effect.string());
    mParameters.set(CameraParameters::KEY_EFFECT, color_effect_set.string());
#endif

    // ER 325568 [UCV_MM] - TakePic/ET Crash when selecting restore
    // The crash issue could be suppress with support of auto setting or remove flash mode.
#ifdef PROP_FLASH
    mParameters.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES, flash_mode.string()); //auto, red-eye, torch yet to be supported
    mParameters.set(CameraParameters::KEY_FLASH_MODE, flash_mode_set.string());
#endif

    mParameters.set(CameraParameters::KEY_SUPPORTED_ANTIBANDING, flicker_mode.string());
    mParameters.set(CameraParameters::KEY_ANTIBANDING, flicker_mode_set.string());

    mParameters.set(CameraParameters::KEY_SUPPORTED_SCENE_MODES, scene_mode.string());
    mParameters.set(CameraParameters::KEY_SCENE_MODE, scene_mode_set.string());

    mParameters.set(KEY_SUPPORTED_METERING, metering_mode.string());
    mParameters.set(KEY_METERING_EXPOSURE, metering_mode_set.string());

    mParameters.set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION, "6");
    mParameters.set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION, "-6");
    mParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP, "0.333334");
    mParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION, "0");

    mParameters.set(CameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED, "true");

    mParameters.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED,"true");
    mParameters.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED,"true");
    mParameters.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK,"false");
    mParameters.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,"false");

#ifdef ENABLE_HIGH_RESOLUTION_VF
    mParameters.set(KEY_HIRES_VIEWFINDER, "0");
#endif //ENABLE_HIGH_RESOLUTION_VF

    // for Panaroma View
    mParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE,"54.8");
    mParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "42.5");

    mPreviewInfo.setupImageInfo(DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT, 0, 0, cameraFormatInfo().mWidthAlignment, cameraFormatInfo().mHeightAlignment);
    mThumbnailInfo.setupImageInfo(DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, DEFAULT_PICTURE_QUALITY, DEFAULT_PICTURE_ROTATION);
    mPictureInfo.setupImageInfo(DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT, DEFAULT_PICTURE_QUALITY, DEFAULT_PICTURE_ROTATION);
    mRecordInfo.setupImageInfo(DEFAULT_PREVIEW_WIDTH,DEFAULT_PREVIEW_HEIGHT, 0, 0);

    setupExtraKeys();

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::initThreads()
{
    DBGT_PROLOG("");

    status_t err = NO_ERROR;

#define ALLOC_THREAD(type, data) {\
    type##Thread<data>* thread = new type##Thread<data>(this);\
    if (NULL == thread) { DBGT_CRITICAL("Thread alloc failed"); return (int)NO_MEMORY;}\
    m##type##Thread = new ThreadContainer<data>(thread);\
    if (NULL == m##type##Thread) { DBGT_CRITICAL("thread container alloc failed"); return (int)NO_MEMORY;}\
    err = m##type##Thread->get()->init(mThreadInfo.maxRequests(ThreadInfo::E##type),\
                                       mThreadInfo.waitForCompletion(ThreadInfo::E##type),\
                                       mThreadInfo.name(ThreadInfo::E##type),\
                                       mThreadInfo.priority(ThreadInfo::E##type),\
                                       mThreadInfo.stackSize(ThreadInfo::E##type));\
    if (NO_ERROR != err) { DBGT_CRITICAL("thread init failed"); return (int)INVALID_OPERATION;}\
}

    ALLOC_THREAD(SwProcessing, SwProcessingThreadData);
    ALLOC_THREAD(Preview, OmxBuffInfo);
    ALLOC_THREAD(Record, OmxBuffInfo);
    ALLOC_THREAD(AutoFocus, AutoFocusThreadData);
    ALLOC_THREAD(ReqHandler, ReqHandlerThreadData);

    //setup preview data
    for (int32_t i = 0; i < kTotalPreviewBuffCount; i++) {

        mPreviewThreadData[i].mSwProcessingThreadData = new SwProcessingThreadData;
        if (NULL == mPreviewThreadData[i].mSwProcessingThreadData) {
            DBGT_CRITICAL("NO_MEMORY - new failed for mPreviewThreadData[%d].mSwProcessingThreadData", i);
            DBGT_EPILOG("");
            return NO_MEMORY;
        }

        mPreviewOmxBuffInfo[i].setUserData((void * )&mPreviewThreadData[i]);
    }

    mAutoFocusThreadData = new AutoFocusThreadData;
    if (NULL == mAutoFocusThreadData) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mAutoFocusThreadData");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    mReqHandlerThreadData = new ReqHandlerThreadData[kMaxRequests];
    if (NULL == mReqHandlerThreadData) {
        DBGT_CRITICAL("NO_MEMORY - new failed for mReqHandlerThreadData");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    DBGT_EPILOG("");
    return err;
}

void STECamera::destroyThreads()
{
    DBGT_PROLOG("");

    delete mReqHandlerThread;
    delete mSwProcessingThread;
    delete mPreviewThread;
    delete mRecordThread;
    delete mAutoFocusThread;

    for (int32_t i = 0; i < kTotalPreviewBuffCount; i++) {
        delete mPreviewThreadData[i].mSwProcessingThreadData;
    }

    delete mAutoFocusThreadData;
    delete [] mReqHandlerThreadData;

    DBGT_EPILOG("");
}

void STECamera::destroySemaphores()
{
    DBGT_PROLOG("");

    sem_destroy(&jpeg_sem);
    sem_destroy(&stateCam_sem);
    sem_destroy(&stateISP_sem);
    sem_destroy(&stateJpeg_sem);
    sem_destroy(&record_eos_sem);
    sem_destroy(&shutter_eos);
    sem_destroy(&isp_snap_sem);
    sem_destroy(&isp_image_sem);
    sem_destroy(&jpeg_eos_sem);
    sem_destroy(&stateEXIF_sem);
    sem_destroy(&exif_sem);
    sem_destroy(&exif_eos_sem);
    sem_destroy(&still_during_record);
    sem_destroy(&sem_b2r2);
    sem_destroy(&sem_b2r2_reset);
    sem_destroy(&sem_crop_buffer);
    sem_destroy(&video_rot_sem);

    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::freeVideoBuffers()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    // If Hi Res VF is enabled Port 2 will be using VFBuffers
    if (!mHiResVFEnabled) {
        for (int i = 0; i < kRecordBufferCount; i++) {
            if (NULL != pRecBuffer[i]) {
                err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 2), pRecBuffer[i]);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("OMX_FreeBuffer failed for Camera Record buffer");
                    DBGT_EPILOG("");
                    return err;
                }
                mRecordOmxBuffInfo[i].mCamHandle->release(mRecordOmxBuffInfo[i].mCamHandle);
                GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();
                GrallocAlloc.free(mRecordGraphicBuffer[i].get()->handle);
                mRecordOmxBuffInfo[i].clear();
            }
        }
#ifdef JPEGENC
        // Free Jpeg Enc buffers
        for (unsigned int i = 0; i < paramJPEGinput.nBufferCountActual; i++) {
            err = OMX_FreeBuffer(mJpegEnc, (OMX_U32)paramJPEGinput.nPortIndex, pJPEGinBuffer[i]);
            DBGT_PTRACE("OMX_FreeBuffer for pJPEGinBuffer[%d] err: %#x",i,err);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FreeBuffer failed for Jpeg input port ");
                DBGT_EPILOG("");
                return err;
            }
            mJpegOmxBuffInfo[i].clear();
        }
#endif
        // Free Exif Mixer Buffer
        OMX_U8* exifBuf = pEXIFoutBuffer->pBuffer;
        err = OMX_FreeBuffer(mExifMixer, (OMX_U32)paramEXIFoutput.nPortIndex, pEXIFoutBuffer);
        DBGT_PTRACE("OMX_FreeBuffer for pEXIFoutBuffer err: %#x",err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for Exif Out port ");
            DBGT_EPILOG("");
            return err;
        }
        delete[] exifBuf;

        // Delete the ISP buffer header type allocated for Thumbnail
        delete pISP1outBuffer;
    }

    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE STECamera::freeVFBuffers(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (mPreviewWindow) {
        /* Do not cancle the native buffers here
           as there can be possibilty that app might
           have close the window and sent NULL.
           Cancleing of the native buffers can be done in
           setpreviewwindow call.
        */

        /* Camera Buffer De-allocation */
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            int portIndex = CAM_VPB + 0;

            if (mHiResVFEnabled) {
                portIndex = CAM_VPB + 2;
            }
            DBGT_PTRACE("portIndex: %d, pBuffer: %p",portIndex,pVFBuffer[i]);

            /* do the freeing of the buffers only
               when we do have OMXBUffers with us
            */
            if (NULL != pVFBuffer[i]) {
                err = OMX_FreeBuffer(mCam, (OMX_U32)portIndex, pVFBuffer[i]);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("OMX_FreeBuffer failed for Camera Preview buffer");
                    DBGT_EPILOG("");
                    return err;
                }
                if (!aPreviewRunning) {
                    mPreviewOmxBuffInfo[i].mCamHandle->release(mPreviewOmxBuffInfo[i].mCamHandle);
                    /* Need not to clear mGraphicBuffer here
                       as we don't call cancelAllNativeBuffers
                    */
                    mPreviewOmxBuffInfo[i].clear();
                    pVFBuffer[i] = NULL;
                }
            }
        }
    }
    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::freeStillBuffers()
{
    DBGT_PROLOG("mIsStillZSL: %d",mIsStillZSL);

    OMX_U8* exifBuf = pEXIFoutBuffer->pBuffer;
    OMX_ERRORTYPE err = OMX_FreeBuffer(mExifMixer, 2, pEXIFoutBuffer);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Free buffer for mExifMixer failed  ");
        DBGT_EPILOG("");
        return err;
    }
    DBGT_PTRACE("deleting exif out software buffer: %p",exifBuf);
    delete[] exifBuf;

    mArmIvOmxBuffInfo.clear();

    for (unsigned int i = 0; i < paramPortVPB1.nBufferCountActual; i++) {
        err = OMX_FreeBuffer(mCam, (OMX_U32)paramPortVPB1.nPortIndex, pStillBuffer[i]);
        DBGT_PTRACE("OMX_FreeBuffer for pStillBuffer[%d] err: %#x",i,err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for camera port 1");
            DBGT_EPILOG("");
            return err;
        }
        mStillOmxBuffInfo[i].clear();
    }

#ifdef JPEGENC

    for (unsigned int i = 0; i < paramPortVPB1.nBufferCountActual; i++) {
        err = OMX_FreeBuffer(mJpegEnc, (OMX_U32)paramJPEGinput.nPortIndex, pJPEGinBuffer[i]);
        DBGT_PTRACE("OMX_FreeBuffer for pJPEGinBuffer[%d] err: %#x",i,err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for Jpeg input port ");
            DBGT_EPILOG("");
            return err;
        }
        mJpegOmxBuffInfo[i].clear();
    }

#endif // JPEGENC

#if defined(SWISPPROC) || defined(HWISPPROC)

    if (!mIsStillZSL) {

        err = OMX_FreeBuffer(mISPProc, (OMX_U32)(CAM_VPB + 0), pISPinBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for ISPProc port 0 ");
            DBGT_EPILOG("");
            return err;
        }

        mISPRawOmxBuffInfo.clear();

        err = OMX_FreeBuffer(mISPProc, (OMX_U32)(CAM_VPB + 1), pISP1outBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for ISPProc LR por ");
            DBGT_EPILOG("");
            return err;
        }

        mRawOmxBuffInfo.clear();

        err = OMX_FreeBuffer(mISPProc, (OMX_U32)paramISPoutput.nPortIndex, pISP2outBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for ISPProc HR port");
            DBGT_EPILOG("");
            return err;
        }

        mISPImageOmxBuffInfo.clear();
    } else {
        delete pISP1outBuffer;
    }

#endif // SWISPPROC || HWISPPROC

    if (OMX_ErrorSameState == err) {
        err = OMX_ErrorNone;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::sendVideoComponentsToIdleState()
{
    DBGT_PROLOG("");

    OMX_STATETYPE state;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    OMX_GetState(mCam, &state);

    if (state == OMX_StateIdle) {
        DBGT_PTRACE("Camera Already in IDLE \n");
    } else {
        err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateIdle, NULL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Sending OMX_StateIdle failed for Camera");
            DBGT_EPILOG("");
            return err;
        }
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for JpegEnc");
        DBGT_EPILOG("");
        return err;
    }
#endif // JPEGENC

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for ExifMixer");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Wait for Exif Mixer IDLE");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Exif Mixer IDLE");

#ifdef JPEGENC
    DBGT_PTRACE("Wait for JpegEnc IDLE");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("JpegEnc IDLE");
#endif // JPEGENC

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::sendStillComponentsToIdleState()
{
    DBGT_PROLOG("mIsStillZSL: %d",mIsStillZSL);

    OMX_STATETYPE state;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    OMX_GetState(mCam, &state);
    DBGT_PTRACE("mCam state: %s",OmxUtils::name(state));
    if (state == OMX_StateIdle) {
        DBGT_PROLOG("Camera Already in IDLE \n");
    } else {
        err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateIdle, NULL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Sending OMX_StateIdle failed for Camera");
            DBGT_EPILOG("");
            return err;
        }
    }

#if defined(SWISPPROC) || defined(HWISPPROC)
    if (!mIsStillZSL) {
        err = OMX_SendCommand(mISPProc, OMX_CommandStateSet, OMX_StateIdle, NULL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Sending OMX_StateIdle failed for Ispproc");
            DBGT_EPILOG("");
            return err;
        }
    }

#endif // SWISPPROC || HWISPPROC

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for JpegEnc");
        DBGT_EPILOG("");
        return err;
    }
#endif // JPEGENC

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for ExifMixer");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("waiting at stateEXIF_sem");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    if (!mIsStillZSL) {
        DBGT_PTRACE("waiting at stateISP_sem");
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }
#ifdef JPEGENC
    DBGT_PTRACE("waiting at stateJpeg_sem");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
#endif // JPEGENC

    if (state != OMX_StateIdle) {
        DBGT_PTRACE("waiting at stateCam_sem");
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::sendVideoComponentsToLoadedState()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for ExifMixer");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for JpegEnc");
        DBGT_EPILOG("");
        return err;
    }
#endif // JPEGENC

    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for Camera");
        DBGT_EPILOG("");
        return err;
    }

    err = freeVideoBuffers();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("freeVideoBuffers failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Wait for Exif Mixer LOADED");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Exif Mixer LOADED");

#ifdef JPEGENC
    DBGT_PTRACE("Wait for JpegEnc LOADED");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("JpegEnc LOADED");
#endif // JPEGENC

    DBGT_PTRACE("Wait for Cam LOADED");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Cam LOADED");

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::sendStillComponentsToLoadedState()
{
    DBGT_PROLOG("mIsStillZSL: %d",mIsStillZSL);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for ExifMixer");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for JpegEnc");
        DBGT_EPILOG("");
        return err;
    }
#endif // JPEGENC

#if defined(SWISPPROC) || defined(HWISPPROC)
    if (!mIsStillZSL) {
        err = OMX_SendCommand(mISPProc, OMX_CommandStateSet, OMX_StateLoaded, NULL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Sending OMX_StateLoaded failed for Ispproc");
            DBGT_EPILOG("");
            return err;
        }
    }

#endif // SWISPPROC || HWISPPROC

    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for Camera");
        DBGT_EPILOG("");
        return err;
    }

   err = freeStillBuffers();
   if (OMX_ErrorNone != err) {
       DBGT_CRITICAL("freeStillBuffers failed");
       DBGT_EPILOG("");
       return err;
   }

    DBGT_PTRACE("waiting for stateEXIF_sem");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
#ifdef JPEGENC
    DBGT_PTRACE("waiting for stateJpeg_sem");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
#endif // JPEGENC
    DBGT_PTRACE("waiting for stateCam_sem");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    if (!mIsStillZSL) {
        DBGT_PTRACE("waiting for stateISP_sem");
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }

    DBGT_EPILOG("");
    return err;
}

void STECamera::destroyHeaps(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning: %d", aPreviewRunning);

    mRawHeap.clear();
    mStillHeap.clear();
    mISPImageHeap.clear();
    mRecordHeap.clear();

    if (!aPreviewRunning)
        STECamMMHwBuffer::freeHwBuffer(mPreviewOmxBuffInfo,true);
    STECamMMHwBuffer::freeHwBuffer(mRecordOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(&mRawOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(mStillOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(&mISPRawOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(&mISPImageOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(mJpegOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(&mArmIvOmxBuffInfo);
    STECamMMHwBuffer::freeHwBuffer(&mTempStillOmxBuffInfo);

    if (mEnableB2R2DuringRecord) {
        // Free the record native buffers wrapped in HwMem
        for (int i = 0; i < kRecordBufferCount; i++) {
            if (mRecordMMNativeBuf[i] != NULL) {
                DBGT_PTRACE("mRecordMMNativeBuf[%d] = %p", i, mRecordMMNativeBuf[i]);
                delete mRecordMMNativeBuf[i];
                mRecordMMNativeBuf[i] = NULL;
            }
        }
    }

    mPreviewTempBuffer.clear();

    if (!aPreviewRunning) {
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            DBGT_PTRACE("mPreviewHeap[%d] = %p", i, mPreviewHeap[i].get());
            mPreviewHeap[i].clear();
        }
    }

    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::freeComponentHandles()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mCam);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle Cam failed");
        DBGT_EPILOG("");
        return err;
    }

    mCam = NULL;

#if defined(SWISPPROC) || defined(HWISPPROC)

    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mISPProc);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle Ispproc failed");
        DBGT_EPILOG("");
        return err;
    }

    mISPProc = NULL ;

#endif // SWISPPROC || HWISPPROC

#ifdef JPEGENC

    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mJpegEnc);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle Jpegenc failed");
        DBGT_EPILOG("");
        return err;
    }

    mJpegEnc = NULL ;

#endif // JPEGENC

    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mExifMixer);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle ExifMixer failed");
        DBGT_EPILOG("");
        return err;
    }

    mExifMixer = NULL ;

    err = mEnableDataFlow->freeHandles();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("mEnableDataFlow->freeHandle() failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::disablePreviewPort(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning %d", aPreviewRunning);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /* Disable the viewfinder ports of previous sets*/
    int portIndex = CAM_VPB + 0;
    if (mHiResVFEnabled)
        portIndex = CAM_VPB + 2;

    DBGT_PTRACE("OMX_CommandportDisable for VPB%d", portIndex);

    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,portIndex,NULL,NULL);
    if ((err == OMX_ErrorNone) || (err == OMX_ErrorSameState)) {
        DBGT_PTRACE("OMX_CommandPortDisable for cam passed \n");
    } else {
        DBGT_CRITICAL("OMX_CommandPortDisable failed for VPB%d", portIndex);
    }
    if (OMX_ErrorSameState != err) {
        err = freeVFBuffers(aPreviewRunning);
        if (err != OMX_ErrorNone) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for mCam");
        } else {
            camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
        }
    }

    if (OMX_ErrorSameState == err)
        err = OMX_ErrorNone;
    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::enablePreviewPort(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning %d", aPreviewRunning);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    int portIndex = CAM_VPB + 0;
    if (mHiResVFEnabled)
        portIndex = CAM_VPB + 2;

    err = OmxUtils::setPortState(mCam, OMX_CommandPortEnable, portIndex, NULL, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 2 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }
    err = initPreviewHeap(aPreviewRunning);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 0 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::initPreviewHeap(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    int preview_width = 0, preview_height = 0;

    if (!aPreviewRunning) {

        mParameters.getPreviewSize(&preview_width, &preview_height);

        //check buffer size exits
        DBGT_ASSERT(0 != paramPortVPB0.nBufferSize, "Buffer Size Null");


        DBGT_PTRACE("Buffer Size VPB0: %lu VPB2: %lu", paramPortVPB0.nBufferSize, paramPortVPB2.nBufferSize);

        mPreviewFrameSize = paramPortVPB0.nBufferSize;

#ifdef USE_HWMEM
        AlignPow2<int>::up(mPreviewFrameSize, 4096); //hwmem enforces 4096 alignment
#else
        AlignPow2<int>::up(mPreviewFrameSize, 256);
#endif

        DBGT_PTRACE("mPreviewFrameSize = %d", mPreviewFrameSize);

        // For HiResVF feature we need to have Bigger Frame size
        if (mHiResVFEnabled) {
            mPreviewFrameSize = mRecordFrameSize;
        }

        getNativeBuffFromNativeWindow();
        shareNativeBuffWithOMXPorts();

        mParameters.getPreviewSize(&preview_width, &preview_height);
        mConfigPreviewHeight = preview_height;
        mConfigPreviewWidth = preview_width;

        //setup sw processing for preview
        setupPreviewSwConProcessing(mParameters.getPreviewFormat());

        //allocate sw buffers corresponding to all preview hw buffers
        setupPreviewSwBuffers(preview_width, preview_height);

        preview_width = mPreviewInfo.getOverScannedWidth();
        preview_height = mPreviewInfo.getOverScannedHeight();

        setupPreviewSwRotProcessing(0);

        /* Is port 0 enabled? else wait till we get reply for cmd complete */
        if (OMX_ErrorSameState != err) {
            camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
        }

#ifdef ENABLE_FACE_DETECTION
        OMX_COLOR_FORMATTYPE omxColorFormat = OMX_COLOR_FormatUnused;
#if (FACE_DETECTION_ALGO_TYPE == NEVEN)
        omxColorFormat = cameraFormatInfo().mPreviewOmxColorFormat;
#endif
        //Max input frame size 1280x720 for LR pipe
        int status = mFaceDetector->init(
                1280,
                720,
                MAX_FACES,
                omxColorFormat);
        if (status != 0) {
            DBGT_PTRACE("face detection init failed %d", status);
        }
#endif

    } else {
        shareNativeBuffWithOMXPorts(aPreviewRunning);
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }
    DBGT_EPILOG("");
    return err;
}

void STECamera::configureVideoHeap()
{
    DBGT_PROLOG("");

    if (NULL != mCam) {
        //check buffer size exits
        DBGT_ASSERT(0 != paramPortVPB2.nBufferSize, "Cam Port 2 Buffer Size Null");
        DBGT_ASSERT(0 != paramJPEGinput.nBufferSize, "Jpeg Port 0 Buffer Size Null");

        DBGT_PTRACE("Buffer SizeVPB2: %lu", paramPortVPB2.nBufferSize);
        DBGT_PTRACE("Buffer Size JPEGin: %lu", paramJPEGinput.nBufferSize);

        mRecordFrameSize = paramPortVPB2.nBufferSize;
        if ((int)paramJPEGinput.nBufferSize > mRecordFrameSize) {
            mRecordFrameSize = paramJPEGinput.nBufferSize;
        }
        AlignPow2<int>::up(mRecordFrameSize, 4096); //hwmem enforces 4096 alignment

        DBGT_PTRACE("mRecordFrameSize = %d", mRecordFrameSize);
        int omxCompFrameSize = mRecordFrameSize;
        if (!mHiResVFEnabled) {
            OMX_ERRORTYPE err = OMX_ErrorNone;
            OMX_PARAM_PORTDEFINITIONTYPE dummyport;
            int portIndex = CAM_VPB + 2;
            float bpp = (float)3/2; /* for record bpp is 3/2=1.5 */
            int w, h;
            w = mRecordInfo.getOverScannedWidth();
            h = mRecordInfo.getOverScannedHeight();
            PixelFormat recordPixFmt = HAL_PIXEL_FORMAT_YCBCR42XMBN;

            if (mEnableB2R2DuringRecord) {
                // For 2Mp Still During Record actual RecordFrameSize
                // will be different from what is being stored in
                // paramPortVPB2.nBufferSize
                // Since here we use different buffers for Encoder and Camera
                // Buffers from Gralloc are passed to Encoder, whereas
                // Camera buffers are allocated here
                w = mRecordInfo.getAlignedWidth();
                h = mRecordInfo.getAlignedHeight();

                mRecordFrameSize = w * h * bpp;
                char pixFmt[32]; DynSetting::get(DynSetting::ERecordPixFmt, pixFmt);
                if (getOmxPixFmtFromKeyStr(pixFmt) == OMX_COLOR_FormatYUV420SemiPlanar) {
                    recordPixFmt = HAL_PIXEL_FORMAT_YCbCr_420_SP;
                } else {
                    DBGT_PTRACE("Record Pixel Format read from the property %s, Default is YUV420 MB Tiled"
                            , pixFmt);
                }
                err = STECamMMHwBuffer::allocateHwBuffer(kRecordBufferCount, omxCompFrameSize, true,
                                                         mRecordOmxBuffInfo, (CAM_VPB + 2), &mRecordHeap,
                                                         mJpegOmxBuffInfo, (CAM_VPB + 0));

                err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mRecordOmxBuffInfo[0].mMMHwChunkMetaData);
                if (err != OMX_ErrorNone) {
                    DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
                }
#ifdef JPEGENC
                err = OMX_SetConfig(mJpegEnc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mJpegOmxBuffInfo[0].mMMHwChunkMetaData);
                if (err != OMX_ErrorNone) {
                    DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
                }
#endif //JPEGENC
            } else {
                enableNativeBuffOnOMXComp(mCam, portIndex);
                enableNativeBuffOnOMXComp(mJpegEnc, CAM_VPB + 0);
            }

            for (int i = 0; i< kRecordBufferCount; i++) {
                GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();

                int extraDataHeight
                    = (ceil(((float)mRecordFrameSize
                                    / (float)(mRecordInfo.getAlignedWidth()* bpp)))
                            - mRecordInfo.getAlignedHeight());

                DBGT_PTRACE("Extra height = %d", extraDataHeight);

                int stride;
                buffer_handle_t buf;
                int ret = GrallocAlloc.alloc(
                        w,
                        h+extraDataHeight,
                        recordPixFmt,
                        CAMHAL_GRALLOC_USAGE,
                        &buf,
                        &stride);

                DBGT_PTRACE("buf = %p", buf);

                mRecordGraphicBuffer[i].clear();
                mRecordGraphicBuffer[i]
                    = new GraphicBuffer(
                            w,
                            h,
                            recordPixFmt,
                            CAMHAL_GRALLOC_USAGE,
                            stride,
                            (native_handle_t*)buf,
                            false);

                pRecBuffer[i] = NULL;
                pJPEGinBuffer[i] = NULL;

                if (mEnableB2R2DuringRecord) {
                    // Wrap the Graphic Buffer from Native Window into HwMem Buffer
                    err = STECamMMHwBuffer::allocateMMNativeBuf(&mRecordMMNativeBuf[i], (void*)buf);
                    DBGT_ASSERT(OMX_ErrorNone == err, "MMNativeBuffer Allocation Failed");

                    // Share the Camera buffers with Openmax components
                    err = OMX_UseBuffer(mCam, // hComponent
                                        &pRecBuffer[i], // address where ptr to buffer header will be stored
                                        CAM_VPB + 2, // port index (for port for which buffer is provided)
                                        (void *)(&mRecordOmxBuffInfo[i]), // App. private data = pointer to beginning of allocated data
                                        // to have a context when component returns with a callback (i.e. to know
                                        // what to free etc.
                                        (OMX_U32)(omxCompFrameSize), // buffer size
                                        (OMX_U8 *)(mRecordOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr) // buffer data ptr
                                       );
                    DBGT_ASSERT(OMX_ErrorNone == err, "OMX_UseBuffer failed for mCam port 2");

#ifdef JPEGENC
                    DBGT_PTRACE("Usebuffer for mJpegEnc input port");
                    err = OMX_UseBuffer(mJpegEnc,    // hComponent
                                        &pJPEGinBuffer[i],    // address where ptr to buffer header will be stored
                                        paramJPEGinput.nPortIndex,          // port index (for port for which buffer is provided)
                                        (void *)(&mJpegOmxBuffInfo[i]),    // App. private data = pointer to beginning of allocated data
                                        // to have a context when component returns with a callback (i.e. to know
                                        // what to free etc.
                                        (OMX_U32)(omxCompFrameSize),        // buffer size
                                        (OMX_U8 *)(mJpegOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr) // buffer data ptr
                                        );
                    DBGT_PTRACE("OMX_UseBuffer for pJPEGinBuffer[%d] err: %#x",i,err);
                    DBGT_ASSERT(NULL != pJPEGinBuffer[i]->pBuffer, "Buffer is NULL");
#endif //JPEGENC
                } else {
                    mRecordOmxBuffInfo[i].clear();

                    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);
                    UseAndroidNativeBufferParams params= {
                        sizeof(UseAndroidNativeBufferParams),
                        dummyport.nVersion,
                        portIndex,
                        (void*) &mRecordOmxBuffInfo[i],
                        &pRecBuffer[i],
                        mRecordGraphicBuffer[i].get()
                    };

                    err = OMX_SetParameter(mCam,
                            (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer,
                            &params);

                    DBGT_PTRACE("OMX Hdr = %p buff = %p", pRecBuffer[i], pRecBuffer[i]->pBuffer);
                    DBGT_ASSERT(NULL != pRecBuffer[i]->pBuffer, "Buffer is NULL");

                    mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nSize = sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);
                    mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nVersion = dummyport.nVersion;
                    mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nPortIndex = portIndex;

                    err = OMX_GetConfig(mCam,
                            (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                            &mRecordOmxBuffInfo[i].mMMHwChunkMetaData);

                    DBGT_PTRACE("Getconfig done for record chunk data...");
                    DBGT_PTRACE("fd = %d baselogicaladdr = %p size = %d",
                            (int)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                            (void*) mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr,
                            (int)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nChunkSize);

                    mRecordOmxBuffInfo[i].m_pMMHwBuffer = (MMHwBuffer*)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nHandleId;

                    mRecordOmxBuffInfo[i].m_pMMHwBuffer->BufferInfo(i, mRecordOmxBuffInfo[i].mMMHwBufferInfo);

                    mRecordHeap.clear();
                    mRecordHeap = new CamMemoryHeapBase((int)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                                      (unsigned int)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr,
                                      (size_t)mRecordOmxBuffInfo[i].mMMHwChunkMetaData.nChunkSize);

                    DBGT_PTRACE("mRecordHeap = 0x%x",(unsigned int)(mRecordHeap.get()));

                    int heapOffset = 0;
                    mRecordOmxBuffInfo[i].mCamMemoryBase
                        = new CamMemoryBase(mRecordHeap, heapOffset, mRecordFrameSize);

#ifdef JPEGENC
                    DBGT_PTRACE("Calling Use Android Native Params for JpegEnc");
                    mJpegOmxBuffInfo[i].clear();

                    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);
                    UseAndroidNativeBufferParams jpegParams= {
                        sizeof(UseAndroidNativeBufferParams),
                        dummyport.nVersion,
                        CAM_VPB + 0,
                        (void*) &mJpegOmxBuffInfo[i],
                        &pJPEGinBuffer[i],
                        mRecordGraphicBuffer[i].get()
                    };

                    err = OMX_SetParameter(mJpegEnc,
                            (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer,
                            &jpegParams);

                    DBGT_PTRACE("JPEG OMX Hdr = %p buff = %p", pJPEGinBuffer[i], pJPEGinBuffer[i]->pBuffer);
                    DBGT_ASSERT(NULL != pJPEGinBuffer[i]->pBuffer, "JPEG Buffer is NULL");

                    mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nSize = sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);
                    mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nVersion = dummyport.nVersion;
                    mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nPortIndex = CAM_VPB + 0;

                    err = OMX_GetConfig(mJpegEnc,
                            (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                            &mJpegOmxBuffInfo[i].mMMHwChunkMetaData);

                    DBGT_PTRACE("Getconfig done for jpeg enc chunk data...");
                    DBGT_PTRACE("fd = %d baselogicaladdr = %p size = %d",
                            (int)mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                            (void*) mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr,
                            (int)mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nChunkSize);

                    // use the ISP Image heap for Jpeg Enc
                    mISPImageHeap.clear();
                    mISPImageHeap = new CamMemoryHeapBase((int)mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                                                          (unsigned int) mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr,
                                                          (size_t)mJpegOmxBuffInfo[i].mMMHwChunkMetaData.nChunkSize);

                    DBGT_PTRACE("mISPImageHeap = 0x%x",(unsigned int)(mISPImageHeap.get()));

                    mJpegOmxBuffInfo[i].mCamMemoryBase
                         = new CamMemoryBase(mISPImageHeap, 0, mRecordFrameSize);
#endif //JEPGENC
                }

                video_metadata_t *metaData;
                mRecordOmxBuffInfo[i].mCamHandle = mRequestMemory(
                        -1,     //(int)mchunkData_record[i].nFd,
                        sizeof(video_metadata_t),
                        1,
                        mCallbackCookie);
                mRecordMetaData[i] = (video_metadata_t *)(mRecordOmxBuffInfo[i].mCamHandle->data);
                mRecordMetaData[i]->offset = 0;
                mRecordMetaData[i]->metadataBufferType = 1;    /* KMetadataBufferTypeGrallocSource */
                mRecordMetaData[i]->handle = (void*)buf;
                mRecordOmxBuffInfo[i].nativebuffIndex = i;
                mRecordOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pRecBuffer[i]);
#ifdef JPEGENC
                mJpegOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pJPEGinBuffer[i]);
#endif //JPEGENC
            }
        }
    }
    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::initSnapshotHeap()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE omxerr = OMX_ErrorNone;
    unsigned int heapOffset;
    if (((NULL != mISPProc)|| mIsStillZSL) && NULL != mJpegEnc) {

        mRawHeap.clear();
        omxerr = STECamMMHwBuffer::allocateHwBuffer(1, paramISPoutput1.nBufferSize, true,
                                                                  &mRawOmxBuffInfo, (CAM_VPB + 1), &mRawHeap,
                                                                  &mArmIvOmxBuffInfo, (CAM_VPB + 0));

        if (omxerr != OMX_ErrorNone) {
            DBGT_CRITICAL("STECamMMHwBuffer::allocateHwBuffer() failed err 0x%08X\n", omxerr);
        }

        setupRawSwConProcessing(mParameters.getPreviewFormat(),
                                mPreviewInfo.getAlignedWidth(),
                                mPreviewInfo.getAlignedHeight());
    }
    DBGT_EPILOG("");
    return omxerr;
}

void STECamera::initStillHeap()
{
    DBGT_PROLOG("");

    if (mIsStillZSL) {
        initStillZSLHeap();
    } else {
        initStillRawHeap();
    }
    DBGT_EPILOG("");
}

void STECamera::initStillRawHeap()
{
    DBGT_PROLOG("");

    if (NULL != mCam && NULL != mISPProc) {
        OMX_U32 bufSize = 0;
        DBGT_PTRACE("nBufferSize paramISPinput: %u, paramPortVPB1: %u",
            (unsigned int)paramISPinput.nBufferSize,
            (unsigned int)paramPortVPB1.nBufferSize );
        if (paramPortVPB1.nBufferSize >= paramISPinput.nBufferSize) {
            bufSize = paramPortVPB1.nBufferSize;
            paramISPinput.nBufferSize = bufSize;
        } else {
            bufSize = paramISPinput.nBufferSize;
            paramPortVPB1.nBufferSize = bufSize;
        }
        DBGT_ASSERT(0 != bufSize, "Buffer Size Null");
        DBGT_PTRACE("Buffer Size : %lu", bufSize);

        mStillHeap.clear();
        OMX_ERRORTYPE omxerr = STECamMMHwBuffer::allocateHwBuffer(1, bufSize, true,
                                                                  mStillOmxBuffInfo, (CAM_VPB + 1), &mStillHeap,
                                                                  &mISPRawOmxBuffInfo, (CAM_VPB + 0));

        if (omxerr != OMX_ErrorNone) {
            DBGT_CRITICAL("STECamMMHwBuffer::allocateHwBuffer() failed err %#x\n", omxerr);
        }
    }

    if (NULL != mISPProc && NULL != mJpegEnc) {
        OMX_U32 bufSize = 0;
        DBGT_PTRACE("nBufferSize paramISPoutput: %u, paramJPEGinput: %u",(unsigned int)paramISPoutput.nBufferSize,(unsigned int)paramJPEGinput.nBufferSize);
        if (paramISPoutput.nBufferSize >= paramJPEGinput.nBufferSize) {
            bufSize = paramISPoutput.nBufferSize;
            paramJPEGinput.nBufferSize = bufSize;
        } else {
            bufSize = paramJPEGinput.nBufferSize;
            paramISPoutput.nBufferSize = bufSize;
        }
        DBGT_ASSERT(0 != bufSize, "Buffer Size Null");
        DBGT_PTRACE("Buffer Size : %lu", bufSize);

        mISPImageHeap.clear();
        OMX_ERRORTYPE omxerr = STECamMMHwBuffer::allocateHwBuffer(1, bufSize, true,
                                                                  &mISPImageOmxBuffInfo, (CAM_VPB + 2), &mISPImageHeap,
                                                                  mJpegOmxBuffInfo, (CAM_VPB + 0));

        if (omxerr != OMX_ErrorNone) {
            DBGT_CRITICAL("STECamMMHwBuffer::allocateHwBuffer() failed err %#x\n", omxerr);
        }
    }
    DBGT_EPILOG("");
}


void STECamera::initStillZSLHeap()
{
    DBGT_PROLOG("");

    if (NULL != mCam && NULL != mJpegEnc) {

        OMX_U32 bufSize = 0;
        DBGT_PTRACE("nBufferSize paramJPEGinput: %u, paramPortVPB1: %u",(unsigned int)paramJPEGinput.nBufferSize,(unsigned int)paramPortVPB1.nBufferSize);
        if (paramPortVPB1.nBufferSize >= paramJPEGinput.nBufferSize) {
            bufSize = paramPortVPB1.nBufferSize;
            paramJPEGinput.nBufferSize = bufSize;
        } else {
            bufSize = paramJPEGinput.nBufferSize;
            paramPortVPB1.nBufferSize = bufSize;
        }
        DBGT_ASSERT(0 != bufSize, "Buffer Size Null");
        DBGT_PTRACE("Buffer Size : %lu", bufSize);

        mStillHeap.clear();
        OMX_ERRORTYPE omxerr = STECamMMHwBuffer::allocateHwBuffer(paramPortVPB1.nBufferCountActual, bufSize, true,
                                                                  mStillOmxBuffInfo, (CAM_VPB + 1), &mStillHeap,
                                                                  mJpegOmxBuffInfo, (CAM_VPB + 0));

        if (omxerr != OMX_ErrorNone) {
            DBGT_CRITICAL("STECamMMHwBuffer::allocateHwBuffer() failed err %#x\n", omxerr);
        }
        omxerr = STECamMMHwBuffer::allocateHwBuffer(1, bufSize, true,
                                                    &mTempStillOmxBuffInfo, (CAM_VPB + 1), NULL,
                                                    NULL, 0);

        if (omxerr != OMX_ErrorNone) {
            DBGT_CRITICAL("STECamMMHwBuffer::allocateHwBuffer() failed err %#x\n", omxerr);
        }
    }
    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::cleanUp()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (isPreviewPortEnabled()) {
        err = disablePreviewPort();
        if (err != OMX_ErrorNone) {
            DBGT_CRITICAL("disablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
    }

    err = preOmxCompCleanUp();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("preOmxCompCleanUp failed");
        DBGT_EPILOG("");
        return err;
    }

    err = mEnableDataFlow->resetComponents();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("mEnableDataFlow->resetComponentsToLoaded() failed");
        DBGT_EPILOG("");
        return err;
    }

    err = postOmxCompCleanUp();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("postOmxCompCleanUp failed");
        DBGT_EPILOG("");
        return err;
    }

    if (mPreviewWindow) {
        delete mPreviewWindow;
        mPreviewWindow = NULL;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::preOmxCompCleanUp()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (isStartPreview || g_IsPreviewConfig) {
        switch (mCamMode) {
            case EStill:
                //Disable Thumbnail Data Flow
                err = getThumbnailHandler()->release();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("getThumbnailHandler()->release() failed");
                    DBGT_EPILOG("");
                    return err;
                }

                err = clearStillModeConfig();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("clearStillModeConfig failed");
                    DBGT_EPILOG("");
                    return err;
                }
                break;
            case EVideo:
                //Disable Thumbnail Data Flow
                err = getThumbnailHandler()->release();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("getThumbnailHandler()->release() failed");
                    DBGT_EPILOG("");
                    return err;
                }

                err = clearVideoModeConfig();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("clearVideoModeConfig failed");
                    DBGT_EPILOG("");
                    return err;
                }

                break;
            default:
                DBGT_CRITICAL("Cant Happen");
                break;
        }
        mIsCameraConfigured = false;
        g_IsPreviewConfig = false;
    }
#ifndef CAM_REINIT_CAM_PROP
    resetProperties();
#endif
    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::postOmxCompCleanUp()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = freeComponentHandles();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("freeComponentHandles failed");
        DBGT_EPILOG("");
        return err;
    }

    err = (mOmxUtils.interface()->GetpOMX_Deinit())();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_Deinit failed");
        DBGT_EPILOG("");
        return err;
    }

    destroySemaphores();
    destroyHeaps();
    destroyThreads();

    delete mAutoFocusHandler;
    delete mContinuousAutoFocusHandler;
    delete mFixedFocusHandler;
    mFocusHandler = NULL;

    delete mEnableDataFlow;
    delete mDisableDataFlow;
    mThumbnailHandler = NULL;

#ifdef ENABLE_FACE_DETECTION
    if (mFaceDetector) {
        delete mFaceDetector;
        mFaceDetector = NULL;
    }
#endif

#if ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_DEVICE
    delete mDeviceHwRotation;
    delete mDeviceSwRotation;
#else
    if ( NULL != mImageRotHandler) {
        delete mImageRotHandler;
    }
#endif //ENABLE_IMAGE_ROTATION
    mImageRotHandler = NULL;

    CamSwRoutines::deinit();

    STECamMMHwBuffer::deinit();

    //close perf manager
    mPerfManager.close();

    //delete scene mode controller
    if (mSceneCtrl != NULL) {
        delete mSceneCtrl;
        mSceneCtrl = NULL;
    }
    //delete extension index
    delete mOmxILExtIndex;
    mHiResVFEnabled = false;

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::cleanUpDspPanic(void* aArg)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (mCamMode) {
        case EStill:
            err = freeStillBuffers();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("freeStillBuffers failed");
                DBGT_EPILOG("");
                return err;
            }
            err = mEnableDataFlow->freeBuffers();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("mEnableDataFlow->freeBuffer() failed");
                DBGT_EPILOG("");
                return err;
            }
            break;
        case EVideo:
            err = freeVideoBuffers();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("freeVideoBuffers failed");
                DBGT_EPILOG("");
                return err;
            }
            break;
        default:
            DBGT_CRITICAL("Received invalid mCamMode %d", mCamMode);
            break;
    }

    //ignore error
    err = postOmxCompCleanUp();

    usleep(1000*1000);
    DBGT_CRITICAL("Kill mediaserver process due to unrecoverable error");
    _exit(1);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::ReconfigureVFStill(bool aReconfigureHeap /*= true*/)
{
    DBGT_PROLOG("ReconfigureHeap: %d", aReconfigureHeap);

    if (false == mIsCameraConfigured) {
        DBGT_CRITICAL("Camera is not configured even once. This should not happen");
    }

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_ERRORTYPE stateChangeErr = OMX_ErrorNone;

    g_IsPreviewConfig = true;

    bool previewPortEnabled = isPreviewPortEnabled();
    bool previewRunning = checkPreviewEnabled();

    if (previewPortEnabled) {
        if (previewRunning) {
            status_t status = flushViewFinderBuffers();
            if (NO_ERROR != status) {
                DBGT_CRITICAL("OMX_ErrorUndefined - flushViewFinderBuffers failed");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }
        }
        err = disablePreviewPort(previewRunning);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("disablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
    }
    DBGT_PTRACE("In ReconfigureVFStill, sending Command OMX_CommandPortDisable for mISPProc VPB1");
    err = OmxUtils::setPortState(mISPProc, OMX_CommandPortDisable,CAM_VPB + 1, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err )) {
        DBGT_CRITICAL("Port Disable VPB1 mISPProc failed");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        err = OMX_FreeBuffer(mISPProc, (OMX_U32)(CAM_VPB + 1), pISP1outBuffer); // to see the logic
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Free Buffer port VPB1 mISPProc failed");
            DBGT_EPILOG("");
            return err;
        }
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }

    if (aReconfigureHeap) {
        /*  Destroy the buffer heap*/
        DBGT_PTRACE("In ReconfigureVFStill,Destroying Buffer Heap for Preview and Record");
        STECamMMHwBuffer::freeHwBuffer(&mRawOmxBuffInfo);
    }

    // Hi Resolution View Finder has been disabled completely now, reset the boolean here
    mHiResVFEnabled = false;


    err = InitializePort(paramPortVPB0, OMX_PortDomainVideo, mCam, CAM_VPB + 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB0 failed");
        DBGT_EPILOG("");
        return err;
    }
#ifdef HWISPPROC
    err = InitializePort(paramISPoutput1, OMX_PortDomainVideo, mISPProc, 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mISPProc port 1 failed");
        DBGT_EPILOG("");
        return err;
    }
#endif //HWISPPROC

    if (aReconfigureHeap) {
        initSnapshotHeap();
    }

    if (previewPortEnabled) {
        if (previewRunning) {
            err = enablePreviewPort(true);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("enablePreviewPort failed");
                DBGT_EPILOG("");
                return err;
            }

            err = supplyPreviewBuffers();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("supplyPreviewBuffers failed err = %d", err);
                DBGT_EPILOG("");
                return err;
            }
        }
    }


    DBGT_PTRACE("In ReconfigureVFStill, sending Command Port Enable for mISPProc Port VPB1");
    stateChangeErr  = OmxUtils::setPortState(mISPProc, OMX_CommandPortEnable,CAM_VPB + 1,NULL, NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr )) {
        DBGT_CRITICAL("Port Enable 0 mCam failed");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    err = OMX_SetConfig(mISPProc,
            (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
            &mRawOmxBuffInfo.mMMHwChunkMetaData);
    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x \n", (OMX_ERRORTYPE) err);
    }

    pISP1outBuffer = NULL;
    DBGT_PTRACE("Filling data for usebuffer mISPProc ");

    err = OMX_UseBuffer(mISPProc, // hComponent
                        &pISP1outBuffer, // address where ptr to buffer header will be stored
                        CAM_VPB + 1, // port index (for port for which buffer is provided)
                        (void *)(&mRawOmxBuffInfo), // App. private data = pointer to beginning of allocated data
                        // to have a context when component returns with a callback (i.e. to know
                        // what to free etc.
                        (OMX_U32)(paramISPoutput1.nBufferSize), // buffer size
                        (OMX_U8 *)(mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
                       );
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mISPProc port 1  ");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(NULL != pISP1outBuffer->pBuffer, "Buffer is NULL");
    mRawOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(pISP1outBuffer);
    if (OMX_ErrorSameState != stateChangeErr ) {
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }
    err = getThumbnailHandler()->updateSnapShot(mPreviewInfo);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("getThumbnailHandler()->updateSnapShot failed ");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::ReconfigureVFVideo(bool aReconfigureHeap /*= true*/,
                                            bool aReconfigureVPB0 /*= true*/)
{
    DBGT_PROLOG("ReconfigureHeap: %d ReconfigureVPB0 %d", aReconfigureHeap, aReconfigureVPB0);

    DBGT_ASSERT(true == mIsCameraConfigured, "Camera is not configured even once. This should not happen");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    bool previewPortEnabled = isPreviewPortEnabled();
    bool previewRunning = checkPreviewEnabled();

    if (aReconfigureVPB0) {
        if (previewPortEnabled)
            if (previewRunning) {
                status_t status = flushViewFinderBuffers();
                if (NO_ERROR != status) {
                    DBGT_CRITICAL("OMX_ErrorUndefined - flushViewFinderBuffers failed");
                    DBGT_EPILOG("");
                    return OMX_ErrorUndefined;
                }
            }
            err = disablePreviewPort(previewRunning);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("disablePreviewPort failed");
                DBGT_EPILOG("");
                return err;
            }
        }


    // If Hi Res VF is enabled then port 2 is already disabled above
    if (!mHiResVFEnabled) {
        //flush pending buffers
        status_t status = flushVideoBuffers();
        if (NO_ERROR != status) {
            DBGT_CRITICAL("OMX_ErrorUndefined - flushVideoBuffers failed");
            DBGT_EPILOG("");
            return OMX_ErrorUndefined;
        }

        DBGT_PTRACE("In ReconfigureVFVideo, sending Command OMX_CommandPortDisable for mCam VPB2");
        err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 2, NULL,NULL);
        if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
            DBGT_CRITICAL("Port Disable VPB2 mCam failed");
            DBGT_EPILOG("");
            return err;
        }
        if ( OMX_ErrorSameState != err ) {
            for (int i = 0; i < kRecordBufferCount; i++) {
                if ( NULL != pRecBuffer[i]) {
                    err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 2), pRecBuffer[i]); // to see the logic
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("Free Buffer port VPB2 mCam failed");
                        DBGT_EPILOG("");
                        return err;
                    }

                    mRecordOmxBuffInfo[i].mCamHandle->release(mRecordOmxBuffInfo[i].mCamHandle);
                    GraphicBufferAllocator &GrallocAlloc = GraphicBufferAllocator::get();
                    GrallocAlloc.free(mRecordGraphicBuffer[i].get()->handle);
                    mRecordOmxBuffInfo[i].clear();
                }
            }
            camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
        }

#ifdef JPEGENC
        // Disable Jpeg Enc Ports
        DBGT_PTRACE("In ReconfigureVFVideo, sending Command OMX_CommandPortDisable for mJepgEnc Port Input");
        err = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortDisable, 0, NULL, NULL);
        if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
            DBGT_CRITICAL("Port Disable Port Input mJpegEnc failed");
            DBGT_EPILOG("");
            return err;
        }
        if (OMX_ErrorSameState != err) {
            for (unsigned int i = 0; i < paramJPEGinput.nBufferCountActual; i++) {
                err = OMX_FreeBuffer(mJpegEnc, (OMX_U32)paramJPEGinput.nPortIndex, pJPEGinBuffer[i]);
                DBGT_PTRACE("OMX_FreeBuffer for pJPEGinBuffer[%d] err: %#x",i,err);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("OMX_FreeBuffer failed for Jpeg input port ");
                    DBGT_EPILOG("");
                    return err;
                }
                mJpegOmxBuffInfo[i].clear();
            }
            camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
        }
#endif
        // Disable Exif Mixer Ports
        DBGT_PTRACE("In ReconfigureVFVideo, sending Command OMX_CommandPortDisable for mExifMixer Port Output");
        OMX_ERRORTYPE errExif = OmxUtils::setPortState(mExifMixer, OMX_CommandPortDisable, 2, NULL, NULL);
        if ((OMX_ErrorNone != errExif) && (OMX_ErrorSameState != errExif)) {
            DBGT_CRITICAL("Port Disable Output mExifMixer failed");
            DBGT_EPILOG("");
            return errExif;
        }
        if (OMX_ErrorSameState != errExif) {
            OMX_U8* exifBuf = pEXIFoutBuffer->pBuffer;
            errExif = OMX_FreeBuffer(mExifMixer, (OMX_U32)paramEXIFoutput.nPortIndex, pEXIFoutBuffer);
            DBGT_PTRACE("OMX_FreeBuffer for pEXIFoutBuffer err: %#x",errExif);
            if (OMX_ErrorNone != errExif) {
                DBGT_CRITICAL("OMX_FreeBuffer failed for Exif Out port ");
                DBGT_EPILOG("");
                return errExif;
            }
            delete[] exifBuf;
            camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
        }

        // Disable Jpeg and Exif tunneled ports
#ifdef JPEGENC
        DBGT_PTRACE("In ReconfigureVFVideo, sending Command OMX_CommandPortDisable for mJepgEnc Port Output");
        err = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortDisable, 1, NULL, NULL);
        if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
            DBGT_CRITICAL("Port Disable Port Output mJpegEnc failed");
            DBGT_EPILOG("");
            return err;
        }
#endif
        DBGT_PTRACE("In ReconfigureVFVideo, sending Command OMX_CommandPortDisable for mExifMixer Port Input");
        errExif = OmxUtils::setPortState(mExifMixer, OMX_CommandPortDisable, 1, NULL, NULL);
        if ((OMX_ErrorNone != errExif) && (OMX_ErrorSameState != errExif)) {
            DBGT_CRITICAL("Port Disable Input mExifMixer failed");
            DBGT_EPILOG("");
            return errExif;
        }
        if (OMX_ErrorSameState != errExif) {
            camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
        }
#ifdef JPEGENC
        if (OMX_ErrorSameState != err) {
            camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
        }
#endif
    }

    if (aReconfigureHeap) {
        // Clear Heaps
        if (aReconfigureVPB0) {
            // If preview is changing clear out the heaps allocated
            // for Thumbnail.
            // New heaps will be created with new Preview dimensions
            mRawHeap.clear();
            STECamMMHwBuffer::freeHwBuffer(&mRawOmxBuffInfo);
            STECamMMHwBuffer::freeHwBuffer(&mArmIvOmxBuffInfo);

            delete pISP1outBuffer;
        }

        // Heaps allocated only for 2MP Still During Recording
        mRecordHeap.clear();

        STECamMMHwBuffer::freeHwBuffer(mRecordOmxBuffInfo);
        STECamMMHwBuffer::freeHwBuffer(mJpegOmxBuffInfo);
        // Free the record native buffers wrapped in HwMem
        for (int i = 0; i < kRecordBufferCount; i++) {
            if (mRecordMMNativeBuf[i] != NULL) {
                DBGT_PTRACE("mRecordMMNativeBuf[%d] = %p", i, mRecordMMNativeBuf[i]);
                delete mRecordMMNativeBuf[i];
                mRecordMMNativeBuf[i] = NULL;
            }
        }
    }

#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    mRecordInfo.setRotation(0);
    //Before omx setparameter first Reset the Rotation to zero
    err = configVideoRevertAspectRatio(mCam, OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Aspect Ratio failed");
        DBGT_EPILOG("");
        return err;
    }

    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
#elif ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
    mRecordInfo.setRotation(gRotation);
#endif //ENABLE_VIDEO_ROTATION

    //set Common Rotate to zero
    err = setPropVideoRotate(mCam, 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Common Rotate failed");
        DBGT_EPILOG("");
        return err;
    }

    // Hi Resolution View Finder has been disabled completely now, reset the boolean here
    mHiResVFEnabled = false;

    if (aReconfigureVPB0) {
        err = InitializePort(paramPortVPB0, OMX_PortDomainVideo, mCam, CAM_VPB + 0, false);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mCam VPB0 failed");
            DBGT_EPILOG("");
            return err;
        }

        if (previewPortEnabled) {
            if (previewRunning) {
                err = enablePreviewPort(true);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("enablePreviewPort failed");
                    DBGT_EPILOG("");
                    return err;
                }
                err = supplyPreviewBuffers();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("supplyPreviewBuffers failed err = %d", err);
                    DBGT_EPILOG("");
                    return err;
                }
            }
        }
    }

    /*  Reconfigure port 2 */
    err = InitializePort(paramPortVPB2, OMX_PortDomainVideo, mCam, CAM_VPB + 2, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB2 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Reconfigure Jpeg Enc Ports
#ifdef JPEGENC
    // Reconfigure JpegEnc port 0
    err = InitializePort(paramJPEGinput, OMX_PortDomainImage, mJpegEnc, 0, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 0 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Reconfigure JpegEnc port 1
    err = InitializePort(paramJPEGoutput, OMX_PortDomainImage, mJpegEnc, 1, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

#endif // JPEGENC
    // Reconfigure EXIF input 2
    err = InitializePort(paramEXIFinput2, OMX_PortDomainImage, mExifMixer, 1, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Reconfigure EXIF output
    err = InitializePort(paramEXIFoutput, OMX_PortDomainImage, mExifMixer, 2, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 2 failed");
        DBGT_EPILOG("");
        return err;
    }

#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    setupVideoSwRotProcessing(0);
#elif ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
    err = setPropVideoRotate(mCam, mRecordInfo.getRotation());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Common Rotate failed");
        DBGT_EPILOG("");
        return err;
    }

    setupVideoSwRotProcessing(mRecordInfo.getRotation());
#endif //ENABLE_VIDEO_ROTATION

    /*  Emabling Port 2*/
    DBGT_PTRACE("In ReconfigureVFVideo, sending Command Port Enable for mCam Port VPB2");
    err = OmxUtils::setPortState(mCam, OMX_CommandPortEnable,CAM_VPB + 2, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Enable VPB2 mCam failed");
        DBGT_EPILOG("");
        return err;
    }

    // Enable Jpeg and Exif tunneled ports
    DBGT_PTRACE("In ReconfigureVFVideo, sending Command Port Enable for mExifMixer Port Input");
    err = OmxUtils::setPortState(mExifMixer, OMX_CommandPortEnable, 1, NULL, NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port 1 Enable mExifMixer failed");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    DBGT_PTRACE("In ReconfigureVFVideo, sending Command Port Enable for mJpegEnc Port Output");
    err = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortEnable, 1, NULL, NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port 2 Enable mJpegEnc failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("ReconfigureVFVideo: Jpeg Enc Port Output Wait Enabled");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ReconfigureVFVideo: Jpeg Enc Enabled");
#endif

    DBGT_PTRACE("ReconfigureVFVideo: Exif Mixer Port Input Wait Enabled");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ReconfigureVFVideo: Exif Mixer Enabled");

    // Enable Exif Mixer Output port
    DBGT_PTRACE("In ReconfigureVFVideo, sending Command Port Enable for mExifMixer Port Output");
    err = OmxUtils::setPortState(mExifMixer, OMX_CommandPortEnable, 2, NULL, NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port 2 Enable mExifMixer failed");
        DBGT_EPILOG("");
        return err;
    }

    // Allocating EXIF output buffer
    pEXIFoutBuffer = NULL;
    OMX_U8* exifBuf = new OMX_U8[paramEXIFoutput.nBufferSize];
    DBGT_PTRACE("exif out Buffer: %p",exifBuf);
    if (NULL == exifBuf) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for exifBuf");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    err = OMX_UseBuffer(mExifMixer, &pEXIFoutBuffer, 2, NULL, (OMX_U32)(paramEXIFoutput.nBufferSize), exifBuf);
    if ((OMX_ErrorNone != err) || (pEXIFoutBuffer->pBuffer == 0)) {
        DBGT_CRITICAL("OMX_AllocateBuffer failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }
    if ((err != OMX_ErrorNone) && (exifBuf != NULL)) {
         delete[] exifBuf;
    }

    // Enable Jpeg Input Port
#ifdef JPEGENC
    DBGT_PTRACE("In ReconfigureVFVideo, sending Command Port Enable for mJpegEnc Port Input");
    err = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortEnable, 0, NULL, NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port 1 Enable mJpegEnc failed");
        DBGT_EPILOG("");
        return err;
    }
#endif

    configureVideoHeap();

    if (aReconfigureVPB0) {
        // Initialize heap for Thumbnail of Still taken during recording
        paramISPoutput1.nBufferSize = paramPortVPB0.nBufferSize;

        DBGT_PTRACE("dump paramISPoutput1 Buffer Size = %d", (unsigned int)paramISPoutput1.nBufferSize);
        pISP1outBuffer = new OMX_BUFFERHEADERTYPE;

        initSnapshotHeap();

        err = getThumbnailHandler()->updateSnapShot(mPreviewInfo);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("getThumbnailHandler()->updateSnapShot failed ");
            DBGT_EPILOG("");
            return err;
        }
        // Assign the Raw Buff Info logical address to ISP buffer
        // The same will be passed to ARM IV Proc for thumbnail processing
        pISP1outBuffer->pBuffer = (OMX_U8*)mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
    }

    // All ports have been disbaled before reconfiguration above
    // Enable all them here
    DBGT_PTRACE("ReconfigureVFVideo: Cam Wait Enabled");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ReconfigureVFVideo: Cam Enabled");

    DBGT_PTRACE("ReconfigureVFVideo: Exif Mixer Port Output Wait Enabled");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ReconfigureVFVideo: Exif Mixer Enabled");

#ifdef JPEGENC
    DBGT_PTRACE("ReconfigureVFVideo: Jpeg Enc Port Input Wait Enabled");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ReconfigureVFVideo: Jpeg Enc Enabled");
#endif

    //push video buffers if running
    if (checkRecordingEnabled()) {
        //supply buffers
        err = supplyRecordBuffers();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("supplyRecordBuffers failed");
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;

}

STECamera::~STECamera()
{
    DBGT_PROLOG("");

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    cleanUp();

    DBGT_EPILOG("");
}


status_t STECamera::setPreviewWindow(struct preview_stream_ops *window)
{
    DBGT_PROLOG("");
    DBGT_PTRACE("Preview Native window - %p", window);
    OMX_ERRORTYPE err = OMX_ErrorNone;

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    Mutex::Autolock _l(mLock);

    /*
       Check whether preview window is allocated or not
       return with freeing up the previuos window in case
       window is not allocted in current call.
    */
    if (NULL == window) {
        if (mPreviewWindow) {
            if (isPreviewPortEnabled()) {
                err = disablePreviewPort(true);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("disablePreviewPort failed");
                    DBGT_EPILOG("");
                    return UNKNOWN_ERROR;
                }
            } else {
                delete mPreviewWindow;
                mPreviewWindow = NULL;
                DBGT_EPILOG("");
                return -1;
            }
            delete mPreviewWindow;
            mPreviewWindow = NULL;
        }
        DBGT_EPILOG("");
        return NO_ERROR;
    }
    if(mPreviewRunning)
    {
        if((mCamMode == EVideo) && (mPreviousCamMode == EVideo) && !mIsrecordsizechanged  && !mIspreviewsizechanged)
        {
            DBGT_PTRACE("!!!!!!!This was a dummy setPreviewWindow , need to return without doing anything");
            return NO_ERROR;
        }
    }

    if (mPreviewWindow) {
        // App passed the same preview window as before. Normally seen in case of
        // Take Picture, in such case return without any changes.
        // In all the rest cases preview window should be re-instantiated.
        // Check if ViewFinderPort is enable then only return else
        // cancel all native buffers and allocate again

        if (mPreviewWindow->getNativeWindow() == window) {

            if (isPreviewPortEnabled()) {
              DBGT_PTRACE("native window is same as previous!!!!!");
              DBGT_EPILOG("");
              return NO_ERROR;
            } else {
              cancelAllNativeBuffers();
            }
        } else {
            cancelAllNativeBuffers();
        }
    }
    /*
    Deallocate the preview window and get the latest window
    allocted to camera.
    */
    if (mPreviewWindow) {
        delete mPreviewWindow;
        mPreviewWindow = NULL;
    }
    mPreviewWindow = new STENativeWindow(window);
    DBGT_PTRACE("Preview Native window - %p", window);

    if (mPreviewWindow != NULL) {
        OMX_ERRORTYPE err = OMX_ErrorNone;
        OMX_STATETYPE state;
        /*Configure here if the setPreviewWindow called before setParameters*/
        DBGT_PTRACE("g_IsPreviewConfig: %d",g_IsPreviewConfig);
        if (!g_IsPreviewConfig) {
            err = configureCamMode(mCamMode, mIsStillZSL);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("configureCamMode failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
        }

        err = enablePreviewPort();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("enablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }

        /* If state is not EXECUTING, that means SetPreviewWindow is
           called before startpreview.And we can't do fillBuffer till
           camera component is in executing state.
        */
        if (checkPreviewEnabled()) {
            err = postOMXConfigStartViewfinder(true);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("UNKNOWN_ERROR - postOMXConfigStartViewfinder failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
        }
    }
    DBGT_EPILOG("");
    return NO_ERROR;
}

OMX_ERRORTYPE STECamera::enableNativeBuffOnOMXComp(
        OMX_HANDLETYPE aOmxComponent,
        int aportIndex)
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != aOmxComponent, "Invalid handle");
    DBGT_PTRACE("Port index = %d", aportIndex);

    /* Initialize OMX version */
    OMX_PARAM_PORTDEFINITIONTYPE dummyport;
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);

    EnableAndroidNativeBuffersParams params = {
        sizeof(EnableAndroidNativeBuffersParams),
        dummyport.nVersion,
        aportIndex,
        (OMX_BOOL)0x1
    };

    OMX_ERRORTYPE err
        = OMX_SetConfig(
                aOmxComponent,
                (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers,
                &params);

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetConfig enable native buffer failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}
status_t STECamera::getNativeBuffFromNativeWindow(void)
{
    DBGT_PROLOG("");

    /* This API should not be called until OMX Camera component is created */
    DBGT_ASSERT( (0!=mCam), "OMX_Camera is not yet created");
    DBGT_ASSERT((mPreviewWindow!=0), "Native window is NULL");

    DBGT_PTRACE("native window (%p)", mPreviewWindow->getNativeWindow());

    /* Initialize OMX version */
    OMX_PARAM_PORTDEFINITIONTYPE dummyport;
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);

    int portIndex;
    if (mHiResVFEnabled)
        portIndex = (CAM_VPB + 2);
    else
        portIndex = (CAM_VPB + 0);

    DBGT_PTRACE("Enabling native buffer for port (%d)", portIndex);

    EnableAndroidNativeBuffersParams params = {
        sizeof(EnableAndroidNativeBuffersParams), dummyport.nVersion,
        portIndex,(OMX_BOOL)0x1
    };

    OMX_ERRORTYPE omxErr = OMX_SetConfig(mCam,
            (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers,
            &params);

    /* Get aligned height and width */
    int preview_width = mPreviewInfo.getOverScannedWidth();
    int preview_height = mPreviewInfo.getOverScannedHeight();

    /* Native buffer format should be specified in Android pixel format */
    int omxColor = cameraFormatInfo().mPreviewOmxColorFormat;
    int pixelColor = cameraFormatInfo().mAndroidPixelColorFormat;

    DBGT_PTRACE("OMX color format :%s", OmxUtils::name((OMX_COLOR_FORMATTYPE)omxColor));

    DBGT_PTRACE("Width = %d Height = %d Pixel Format = %d",
            preview_width,
            preview_height,
            pixelColor);

    /* Calculate height for extra data area */
    OmxUtils::initialize(dummyport, OMX_PortDomainVideo, portIndex);
    omxErr = OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &dummyport);
    if (OMX_ErrorNone != omxErr) {
        DBGT_CRITICAL("Getparam failed in getnativebuff");
        DBGT_EPILOG("");
        return omxErr;
    }
    int buffSize = dummyport.nBufferSize;
    int bpp = 2; /* for RGB565/YUV422Itd bits per pixel (bpp) is 16 bits =2 bytes*/
    int extraDataHeight
        = (ceil(((float)buffSize / (float)(preview_width * bpp))) - preview_height);

    DBGT_PTRACE("nbuffsize = %d bpp = %d extradataheight = %d",
            buffSize,
            bpp,
            extraDataHeight);

    /* Inform display framework about preview frame h, w and format */
    status_t err = mPreviewWindow->init(
            preview_width,
            preview_height + extraDataHeight,
            pixelColor,
            CAMHAL_GRALLOC_USAGE);

    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::init() failed");
        DBGT_EPILOG("");
        return err;
    }

    /* Crop is added to get only image data from buffer.
       Since buffer also contain EXTRA data
       */
    err = mPreviewWindow->setCrop(0, 0, mPreviewInfo.getAlignedWidth()-1,
                                  mPreviewInfo.getAlignedHeight()-1);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::setCrop() failed");
        DBGT_EPILOG("");
        return err;
    }

    err = mPreviewWindow->setBufferCount(kTotalPreviewBuffCount);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::setBufferCount() failed");
        DBGT_EPILOG("");
        return err;
    }

    /* Now dequeue buffers from Native window */
    int i;
    for (i = 0; i < kTotalPreviewBuffCount; i++) {
        buffer_handle_t *buffHandle;
        err = mPreviewWindow->dequeueBuffer(&buffHandle, NULL);
        if (err != OK) {
            DBGT_CRITICAL("STENativeWindow::dequeueBuffer() failed buffnum (%d) err (%d)",i, err);
            goto CANCEL_BUFFERS;
        }
        DBGT_PTRACE("buffhandle =%p", *buffHandle);

        ANativeWindowBuffer* nativeWin
            = (ANativeWindowBuffer*)
              START_OF_STRUCT(buffHandle,ANativeWindowBuffer,handle);

        DBGT_PTRACE("nativeWin =%p", nativeWin);
        mGraphicBuffer[i].clear();
        mGraphicBuffer[i] = new GraphicBuffer(nativeWin, false);
        if (NULL == mGraphicBuffer[i].get()) {
            DBGT_CRITICAL("Not enough memory to create graphic buffer at Index %d", i);
            goto CANCEL_BUFFERS;
        }
    }

    DBGT_PTRACE("Dequeue buffer done....");
    DBGT_PTRACE("Native handles:");
    for (i = 0; i < kTotalPreviewBuffCount; i++)
        DBGT_PTRACE("0x%x ",(unsigned int)(mGraphicBuffer[i].get()->handle));

    DBGT_EPILOG("");

    return err; //err should be OK

CANCEL_BUFFERS:
    /* Since we get error while dequeue ith buffer
       that means (i-1) buffer are already dequeued,so return
       them back to native window
       */
    int cancelEnd = i;
    for (i = 0; i < cancelEnd; i++) { /* return back (i-1) buffers, see condition */
        err = sendCancelReqToNativeWindow(i);
        if (err != OK) {
            DBGT_CRITICAL("Cancelbuffer error on buff num (%d), but still we continue...",i);
        }
    }

    DBGT_EPILOG("exit with err (-1)");
    return -1;  //unable to get required native buffers from native window
}

status_t STECamera::shareNativeBuffWithOMXPorts(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning %d", aPreviewRunning);

    int portIndex;
    if (mHiResVFEnabled)
        portIndex = (CAM_VPB + 2);
    else
        portIndex = (CAM_VPB + 0);

    DBGT_PTRACE("PortIndex =%d", portIndex);

    OMX_PARAM_PORTDEFINITIONTYPE dummyport;
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(dummyport);

    EnableAndroidNativeBuffersParams params = {
        sizeof(EnableAndroidNativeBuffersParams), dummyport.nVersion,
        portIndex,(OMX_BOOL)0x1
    };

    OMX_ERRORTYPE omxErr = OMX_SetConfig(mCam,
            (OMX_INDEXTYPE)OMX_google_android_index_enableAndroidNativeBuffers,
            &params);

    /* Each frame is itself a heap.So offset is zero */
    unsigned int heapOffset=0;
    DBGT_PTRACE("Total num of native buff = %d", kTotalPreviewBuffCount);
    for (int i = 0 ; i < kTotalPreviewBuffCount; i++) {
        /* We are freeing previous allocated memory
           here itself.So no need to free it else where
           like in ReconfigureVideo
           */
        int flags = 0;
        if (aPreviewRunning) {
            flags = mPreviewOmxBuffInfo[i].mFlags;
        } else {
            mPreviewOmxBuffInfo[i].clear();
            mPreviewHeap[i].clear();
        };
        pVFBuffer[i] = NULL;
        UseAndroidNativeBufferParams params= {
            sizeof(UseAndroidNativeBufferParams),
            dummyport.nVersion,
            portIndex,
            (void*) &mPreviewOmxBuffInfo[i],
            &pVFBuffer[i],
            mGraphicBuffer[i].get()
        };

        OMX_ERRORTYPE err = OMX_SetParameter(mCam,
                (OMX_INDEXTYPE)OMX_google_android_index_useAndroidNativeBuffer,
                &params);

        DBGT_PTRACE("OMX Hdr = %p buff = %p", pVFBuffer[i], pVFBuffer[i]->pBuffer);

        mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nSize = sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);
        mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nVersion = dummyport.nVersion;
        mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nPortIndex = portIndex;

        err = OMX_GetConfig(mCam,
                (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata,
                &mPreviewOmxBuffInfo[i].mMMHwChunkMetaData);

        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("GetConfig failed in sharednativebuff");
            DBGT_EPILOG("");
            return err;
        }

        DBGT_PTRACE("Getconfig done...");

        if (!aPreviewRunning) {
            mPreviewOmxBuffInfo[i].m_pMMHwBuffer = (MMHwBuffer*)mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nHandleId;

            mPreviewOmxBuffInfo[i].m_pMMHwBuffer->BufferInfo(i, mPreviewOmxBuffInfo[i].mMMHwBufferInfo);

            DBGT_PTRACE("preview buffer %d - iLogAddr: %#x, iPhyAddr: %#x, iAllocatedSize: %d", i, (unsigned int)mPreviewOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr,
                                                                     (unsigned int)mPreviewOmxBuffInfo[i].mMMHwBufferInfo.iPhyAddr,
                                                                     (int)mPreviewOmxBuffInfo[i].mMMHwBufferInfo.iAllocatedSize);

            mPreviewHeap[i] = new CamMemoryHeapBase((int)mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                    (unsigned int) mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nBaseLogicalAddr,
                    (size_t)mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nChunkSize);

            DBGT_PTRACE("mPreviewHeap = 0x%x",(unsigned int)(mPreviewHeap[i].get()));


            mPreviewOmxBuffInfo[i].mCamMemoryBase
                = new CamMemoryBase(mPreviewHeap[i], heapOffset, mPreviewFrameSize);

            DBGT_PTRACE("fd = %d", (int)mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nFd);
            mPreviewOmxBuffInfo[i].mCamHandle = mRequestMemory(
                    (int)mPreviewOmxBuffInfo[i].mMMHwChunkMetaData.nFd,
                    (size_t)(mPreviewInfo.getAlignedWidth() * mPreviewInfo.getAlignedHeight() * 2),
                    1,
                    mCallbackCookie);

            DBGT_ASSERT(NULL != pVFBuffer[i]->pBuffer, "Buffer is NULL");
        }
        mPreviewOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pVFBuffer[i]);
        /* Camera hal is now buffer owner */

        if (aPreviewRunning) {
            mPreviewOmxBuffInfo[i].mFlags = flags;
        } else {
            mPreviewOmxBuffInfo[i].mFlags = OmxBuffInfo::ECameraHal;
        }
        DBGT_PTRACE("buffinfo flag = 0x%x",mPreviewOmxBuffInfo[i].mFlags);

        mPreviewThreadData[i].mSwProcessingThreadData->mOmxBufferHeader = pVFBuffer[i];
        mPreviewOmxBuffInfo[i].nativebuffIndex = i;
        mPreviewThreadData[i].mSwProcessingThreadData->mPrevWidth = mPreviewInfo.getAlignedWidth();
        mPreviewThreadData[i].mSwProcessingThreadData->mPrevHeight = mPreviewInfo.getAlignedHeight();
    }

    if (!aPreviewRunning) {
        DBGT_PTRACE("Cancel minQueue buffers");
        int cancelStart
            = kTotalPreviewBuffCount - 1; // index of last preview buffer

        // take care of MinUndequeuedBufferCount as well as 1 snapshot
        // buffer
        int cancelEnd
            = kTotalPreviewBuffCount
                - mPreviewWindow->getMinUndequeuedBufferCount() - 1;


        DBGT_PTRACE("Cancel start=%d end=%d", cancelStart,cancelEnd);

        for (int i = cancelStart ; i >= cancelEnd; i--) {
            sendCancelReqToNativeWindow(i);
        }
    }

    DBGT_PTRACE("Native handle                    OMXHdr");
    for (int i = 0; i < kTotalPreviewBuffCount; i++)
        DBGT_PTRACE("%p                                %p",
              mGraphicBuffer[i].get()->handle, pVFBuffer[i]);

    DBGT_EPILOG("");
    return OK;
}

void  STECamera::cancelAllNativeBuffers(void)
{
    DBGT_PROLOG("");

    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            sendCancelReqToNativeWindow(i);
    }

    DBGT_EPILOG("");
}

status_t STECamera::sendCancelReqToNativeWindow(int index)
{
    DBGT_PROLOG("");

    DBGT_ASSERT((mPreviewWindow != NULL), "Native window NULL");
    DBGT_ASSERT((index < kTotalPreviewBuffCount), "Invalid Index (%d)",index);
    DBGT_ASSERT((NULL != mGraphicBuffer[index].get()), "Graphic buffer NULL@Index (%d)",
            index);

    DBGT_PTRACE("Nativewin @Index %d is %p owner = 0x%x", index,
            mGraphicBuffer[index]->handle,
            mPreviewOmxBuffInfo[index].mFlags);

    status_t err = mPreviewWindow->cancelBuffer(mGraphicBuffer[index]->handle);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::cancelBuffer() failed");
        DBGT_EPILOG("");
        return err;
    }

    mPreviewOmxBuffInfo[index].mFlags &= OmxBuffInfo::ENone;

    DBGT_EPILOG("");
    return err;
}

void  STECamera::cancelCameraHalNativeBuffers(void)
{
    DBGT_PROLOG("");

    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
        if (mPreviewOmxBuffInfo[i].mFlags & OmxBuffInfo::ECameraHal)
            sendCancelReqToCameraHalNativeWindow(i);
    }

    DBGT_EPILOG("");
}

status_t STECamera::sendCancelReqToCameraHalNativeWindow(int index)
{
    DBGT_PROLOG("");

    DBGT_ASSERT((mPreviewWindow != NULL), "Native window NULL");
    DBGT_ASSERT((index < kTotalPreviewBuffCount), "Invalid Index (%d)",index);
    DBGT_ASSERT((NULL != mGraphicBuffer[index].get()), "Graphic buffer NULL@Index (%d)",
            index);

    DBGT_ASSERT((mPreviewOmxBuffInfo[index].mFlags & OmxBuffInfo::ECameraHal),
            "Buffer: %d does not owned by Camera HAL, flags=0x%x",
            index, mPreviewOmxBuffInfo[index].mFlags);

    DBGT_PTRACE("Nativewin @Index %d is %p owner = 0x%x", index,
            mGraphicBuffer[index]->handle,
            mPreviewOmxBuffInfo[index].mFlags);

    status_t err = mPreviewWindow->cancelBuffer(mGraphicBuffer[index]->handle);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::cancelBuffer() failed");
        DBGT_EPILOG("");
        return err;
    }

    /* Change owner ship from Camera HAL to native window */
    mPreviewOmxBuffInfo[index].mFlags &= ~OmxBuffInfo::ECameraHal;
    mPreviewOmxBuffInfo[index].mFlags |= OmxBuffInfo::ESentToNativeWindow;

    DBGT_EPILOG("");
    return err;
}

/* We need to lock native window buffer before it can be used to
     collect preview frame.This API should be called from
     "fillthisbuffer" or "fillbuffer" function.
*/
status_t STECamera::getLockForNativeBuffer(int index)
{
    DBGT_PROLOG("");

    DBGT_ASSERT((mPreviewWindow != NULL), "Native window NULL");
    DBGT_ASSERT((index < kTotalPreviewBuffCount), "Invalid index (%d)",index);
    DBGT_ASSERT((NULL != mGraphicBuffer[index].get()), "Graphic buffer NULL@index (%d)",
            index);

    DBGT_ASSERT((mPreviewOmxBuffInfo[index].mFlags & OmxBuffInfo::ECameraHal),
            "Buffer: %d does not owned by Camera HAL, flags=0x%x",
            index, mPreviewOmxBuffInfo[index].mFlags);

    DBGT_PTRACE("Nativewin @index %d is %p owner = 0x%x",
            index,
            mGraphicBuffer[index]->handle,
            mPreviewOmxBuffInfo[index].mFlags);

    status_t err = mPreviewWindow->lockBuffer(mGraphicBuffer[index]->handle);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::lockBuffer() failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::dequeueNativeBuffer(void)
{
    DBGT_PROLOG("");

    DBGT_ASSERT((mPreviewWindow != NULL), "Native window NULL");

    /* Get one native buffer from display framework. */
    buffer_handle_t *buf;
    status_t err = mPreviewWindow->dequeueBuffer(
                    &buf, NULL);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::dequeueBuffer() failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("dequeued native handle = %p", buf);

    /* Find where to insert this native buffer in our database */
    int index = findNativeBuffIndex(buf);
    DBGT_PTRACE("Index=%d", index);
    if (index == -1) {
        DBGT_CRITICAL("Error index not found");
        return index;
    }

    if (!(mPreviewOmxBuffInfo[index].mFlags & OmxBuffInfo::ESentToNativeWindow)) {
        DBGT_WARNING("Buffer: %d does not owned by native window, flags=0x%x",
            index, mPreviewOmxBuffInfo[index].mFlags);
    }

    mPreviewOmxBuffInfo[index].mFlags &= ~OmxBuffInfo::ESentToNativeWindow;
    mPreviewOmxBuffInfo[index].mFlags |= OmxBuffInfo::ECameraHal;

    DBGT_EPILOG("");
    return index;
}


int  STECamera::findNativeBuffIndex(buffer_handle_t *bufHandle)
{
    DBGT_PROLOG("");

    int index = -1;
    for (int i = 0; i < (int)kTotalPreviewBuffCount; i++) {
        if (mGraphicBuffer[i]->handle == *bufHandle) {
            index = i;
            break;
        }
    }

    DBGT_PTRACE("Index =%d Native handle =%p", index,  bufHandle);

    DBGT_EPILOG("");
    return index;
}

status_t STECamera::renderNativeBuffer(int index)
{
    DBGT_PROLOG("");

    if(mPreviewWindow == NULL) {
        DBGT_EPILOG("Native window NULL");
        return BAD_VALUE;
    }
    DBGT_ASSERT((index < kTotalPreviewBuffCount), "Invalid index (%d)",index);
    DBGT_ASSERT((NULL != mGraphicBuffer[index].get()), "Graphic buffer NULL@index (%d)",
            index);

    DBGT_ASSERT((mPreviewOmxBuffInfo[index].mFlags & OmxBuffInfo::ECameraHal),
            "Buffer: %d does not owned by Camera HAL, flags=0x%x",
            index, mPreviewOmxBuffInfo[index].mFlags);

    DBGT_PTRACE("Nativewin @index %d is %p owner = 0x%x",
            index,
            mGraphicBuffer[index]->handle,
            mPreviewOmxBuffInfo[index].mFlags);

    status_t err = mPreviewWindow->enqueueBuffer(mGraphicBuffer[index]->handle);
    if (OK != err) {
        DBGT_CRITICAL("STENativeWindow::enqueueBuffer() failed");
        DBGT_EPILOG("");
        return err;
    }

    mPreviewOmxBuffInfo[index].mFlags &= ~OmxBuffInfo::ECameraHal;
    mPreviewOmxBuffInfo[index].mFlags |= OmxBuffInfo::ESentToNativeWindow;

    DBGT_EPILOG("");
    return err;
}

void STECamera::setCallbacks(camera_notify_callback notify_cb,
                            camera_data_callback data_cb,
                            camera_data_timestamp_callback data_cb_timestamp,
                            camera_request_memory get_memory,
                             void *user)
{
    DBGT_PROLOG("Notify CB: %p Data CB: %p DataTimestamp CB: %p User: %p" ,
               (void *)(notify_cb), (void *)(data_cb), (void *)(data_cb_timestamp), user);

    Mutex::Autolock lock(mLock);
    mNotifyCb = notify_cb;
    mDataCb = data_cb;
    mDataCbTimestamp = data_cb_timestamp;
    mRequestMemory = get_memory;
    mCallbackCookie = user;

    DBGT_EPILOG("");
}

void STECamera::enableMsgType(int32_t msgType)
{
    DBGT_PROLOG("MsgType: %s(0x%04x)", msgName(msgType), msgType);

    Mutex::Autolock lock(mLock);

    mMsgEnabled |= msgType;
    DBGT_EPILOG("");
}

void STECamera::disableMsgType(int32_t msgType)
{
    DBGT_PROLOG("MsgType: %s(0x%04x)", msgName(msgType), msgType);

    Mutex::Autolock lock(mLock);
    mMsgEnabled &= ~msgType;

    DBGT_EPILOG("");
}

int STECamera::msgTypeEnabled(int32_t msgType)
{
    DBGT_PROLOG("MsgType: %s(0x%04x)", msgName(msgType), msgType);
    Mutex::Autolock lock(mLock);

    DBGT_EPILOG("Enabled: %d", (mMsgEnabled & msgType));

    return (mMsgEnabled & msgType);
}

// ---------------------------------------------------------------------------
status_t STECamera::doPreviewProcessing(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags)
{
    DBGT_PROLOG("");

    int  index;
    status_t err = NO_ERROR;

    Mutex::Autolock prevLock(mPrevLock);
    Mutex::Autolock lock(mLock);

    //return if callback disabled
    if (!(aData.mFlags & OmxBuffInfo::EEnableClientCallback)) {

        mLock.unlock();
        aUpdateFlags = OMX_FALSE;
        aData.mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
        aData.mFlags |= OmxBuffInfo::ECameraHal;    /* Now own by camera HAL */

        DBGT_CRITICAL("callback disable, err =%d", (int)err);
        return err;
    }

    //get OMX buffer header
    CamMemoryBase *ptr = static_cast<CamMemoryBase *>(aData.mCamMemoryBase.get());
    DBGT_ASSERT(NULL != ptr, "CamMemoryBase is NULL");
    OMX_BUFFERHEADERTYPE *omxBuffHeader = ptr->omxBufferHeaderType();
    DBGT_ASSERT(NULL != omxBuffHeader, "omxBuffHeader is NULL");

    PreviewThreadData *data = static_cast<PreviewThreadData *>(aData.userData());
    DBGT_ASSERT(NULL != data, "PreviewThreadData is NULL");

    //update state
//    data->updateBufferState(PreviewThreadData::EPreviewThread);

#if (ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE)
    data->mSwProcessingThreadData->mReqHandler = &STECamera::doPreviewSwRotProcessing;
    DBGT_ASSERT(NULL != data->mSwProcessingThreadData, "SwProcessingThreadData is NULL");
    mSwProcessingThread->get()->request(*data->mSwProcessingThreadData);
    mSwProcessingThread->get()->waitForCompletion();
#endif //ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE

    mFrameDumper.process(omxBuffHeader->pBuffer + omxBuffHeader->nOffset, CFrameDump::EPreview);
    mFrameReplay.process(omxBuffHeader->pBuffer + omxBuffHeader->nOffset, CFrameReplay::EPreview);

    index = aData.nativebuffIndex;
    camera_memory_t *callBackMem = aData.mCamHandle;

    //Is conversion required?
    if (aData.mSwConversion) {
        //Do conversion only if application has enabled callback msgs
        if ((mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME)
#ifdef ENABLE_FACE_DETECTION
                || (mMsgEnabled & CAMERA_MSG_PREVIEW_METADATA)
#endif
            ) {
            DBGT_ASSERT(NULL != data->mSwProcessingThreadData, "SwProcessingThreadData is NULL");
            data->mSwProcessingThreadData->mReqHandler = &STECamera::doPreviewSwConProcessing;
            mSwProcessingThread->get()->request(*data->mSwProcessingThreadData);
            mSwProcessingThread->get()->waitForCompletion();
            CamServiceBuffer *temp = static_cast<CamServiceBuffer *>(aData.mProcessingBuffer);
            callBackMem  = temp->mCamHandle;
        }
    }
#ifdef ENABLE_FACE_DETECTION
    if (mMsgEnabled & CAMERA_MSG_PREVIEW_METADATA) {
        if (isFaceDetectionEnable()) {
            DBGT_PTRACE("face detection enable");
            int status = mFaceDetector->postInputFrame(
                    (char*)omxBuffHeader->pBuffer,
                    omxBuffHeader->nFilledLen,
                    mPreviewInfo.getAlignedWidth(),
                    mPreviewInfo.getAlignedHeight());

            if (status == 0) {
                mFaceDetector->getFaces(&faceMetaData);
                DBGT_PTRACE("num of face %d", faceMetaData.number_of_faces);
                sendFaceROI(faceMetaData);
            }
            mLock.unlock();
            mDataCb(CAMERA_MSG_PREVIEW_METADATA,callBackMem,0,&faceMetaData, mCallbackCookie);
            if (!previewEnabled()) {
                // Preview is not running
                // Exit from here
                DBGT_EPILOG("Preview is not running, dont do further processing on preview frame");
                return err;
            }
            mLock.lock();
        } else {
            DBGT_PTRACE("face detection disable");
            //faceMetaData.number_of_faces = 0;
        }
    }
#endif

    if (mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME) {
        /* Dispatch callback with out lock. This will allow
        * callback to call cameraHAL APIs. Otherwise there
        * would be a deadlock */
        mLock.unlock();
        mDataCb(CAMERA_MSG_PREVIEW_FRAME,callBackMem,0,NULL, mCallbackCookie);
        if (!previewEnabled()) {
            // Preview is not running
            // Exit from here
            DBGT_EPILOG("Preview is not running, dont do further processing on preview frame");
            return err;
        }
        mLock.lock();
    }

    //reset flag
    aUpdateFlags = OMX_FALSE;
    aData.mFlags &= ~OmxBuffInfo::ESentToHandlerThread;

    /* Camera client already done with buffer we sent to it */
    /* Now owner is CAMERA_HAL */

    aData.mFlags |= OmxBuffInfo::ECameraHal;    /* Now own by camera HAL */
    mLock.unlock();
    if (checkPreviewEnabled()) {
        uint32_t lx = 0;
        uint32_t ly = 0;
        uint32_t w = mConfigPreviewWidth;
        uint32_t h = mConfigPreviewHeight;
#ifdef ENABLE_VIDEO_STAB
        if ((mCamMode == EVideo) &&
                (mVideoStabEnabled == true)) {
            uint32_t stride = 2; // For RGB565
            int allignedFrameSize = mPreviewInfo.getOverScannedWidth() *
                                    mPreviewInfo.getOverScannedHeight() * stride;
            AlignPow2<int>::up(allignedFrameSize, 4);
            DBGT_PTRACE("allignedFrameSize %d", allignedFrameSize);
            OMX_U8* pTmp = ((OMX_U8*)(omxBuffHeader->pBuffer) + allignedFrameSize);
            getCropVector(pTmp, lx, ly, w, h);
        }
#endif //ENABLE_VIDEO_STAB
        mLock.lock();
        if ((mCurrentZoomLevel != 0) &&
             (mIsStillZSL ||
             (mEnableB2R2DuringRecord && mRecordRunning))) {
            mLock.unlock();
            getCropVectorForZoom((int&)lx, (int&)ly, (size_t&)w, (size_t&)h, true);
            mLock.lock();
        }
        mPreviewWindow->setCrop(lx, ly, w+lx-1, h+ly-1);

#ifdef DUMP_FPS_FOR_VF
        mFrameRateDumperVF.logAndDump();
#endif //DUMP_FPS_FOR_VF

        err = renderNativeBuffer(index);
        if (err != OK) {
            DBGT_CRITICAL("renderNativebuffer failed - err = %d", (int)err);
            mLock.unlock();
            return err;
        }

        /* Send this buffer to display to draw on screen */
        /* Get next buffer from display framework */
        index = dequeueNativeBuffer();//if buffer not avialable then this will be blocked
        if (index == -1) {
            DBGT_EPILOG("dequeueNativeBuffer failed index - %d", (int)index);
            mLock.unlock();
            return err;
        }

        err = getLockForNativeBuffer(index);
        if (err != OK) {
            DBGT_CRITICAL("getLockForNativeBuffer failed - err = %d", (int)err);
            mLock.unlock();
            return err;
        }
        {

            OmxBuffInfo *buffInfo;
            buffInfo = &mPreviewOmxBuffInfo[index];

            err = pushPreviewBuffer(*buffInfo);
        }

    }

    mLock.unlock();
    DBGT_EPILOG("callback enable err- %d", (int)err);
    return err;
}

void STECamera::getCropVectorForZoom(int& lx, int& ly, size_t& w, size_t& h, bool preview) {
    DBGT_PROLOG("mCurrentZoomLevel: %d, preview: %d",mCurrentZoomLevel, preview);
    int zoom_factors[] = {100,150,200,250,300,350,400};

    if (preview) {
        int prev_w = mPreviewInfo.getOverScannedWidth();
        int prev_h = mPreviewInfo.getOverScannedHeight();
        DBGT_PTRACE("preview -> PreviewHeight: %d, PreviewWidth: %d", prev_h, prev_w);
        w = ((prev_w * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;
        h = ((prev_h * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;

        lx += (prev_w - w)/2;
        ly += (prev_h - h)/2;
    } else if (mIsStillZSL) {
        DBGT_PTRACE("Still -> nFrameWidth: %d, nFrameHeight: %d",(int)paramPortVPB1.format.video.nFrameWidth, (int)paramPortVPB1.format.video.nFrameHeight);
        w = ((paramPortVPB1.format.video.nFrameWidth * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;
        h = ((paramPortVPB1.format.video.nFrameHeight * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;

        lx = (paramPortVPB1.format.video.nFrameWidth - w)/2;
        ly = (paramPortVPB1.format.video.nFrameHeight - h)/2;
    } else if (mEnableB2R2DuringRecord) {
        DBGT_PTRACE("Video -> nFrameWidth: %d, nFrameHeight: %d", mRecordInfo.getAlignedWidth(), mRecordInfo.getAlignedHeight());
        w = ((mRecordInfo.getAlignedWidth() * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;
        h = ((mRecordInfo.getAlignedHeight() * 100 / zoom_factors[mCurrentZoomLevel]) >> 1) <<1;

        lx += (mRecordInfo.getAlignedWidth() - w)/2;
        ly += (mRecordInfo.getAlignedHeight() - h)/2;
    }

    DBGT_PTRACE("lx: %d, ly: %d, w: %d, h: %d",lx,ly,w,h);
    DBGT_EPILOG();
}

status_t STECamera::doRecordProcessing(OmxBuffInfo& aData)
{
    DBGT_PROLOG("");

    status_t err = NO_ERROR;

    Mutex::Autolock lock(mLock);

    //return if callback disabled
    if (!(aData.mFlags & OmxBuffInfo::EEnableClientCallback)) {
        if(mEnableB2R2DuringRecord){
            sem_wait(&sem_crop_buffer);
        }
        mLock.unlock();
        aData.mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
        DBGT_EPILOG("");
        return err;
    }

    //get OMX buffer header
    CamMemoryBase *ptr = static_cast<CamMemoryBase *>(aData.mCamMemoryBase.get());
    DBGT_ASSERT(NULL != ptr, "CamMemoryBase is NULL");
    OMX_BUFFERHEADERTYPE *omxBuffHeader = ptr->omxBufferHeaderType();
    DBGT_ASSERT(NULL != omxBuffHeader, "omxBuffHeader is NULL");

    if (aData.mSwRotation) {
        //synch buffer
        synchCBData(MMHwBuffer::ESyncAfterWriteHwOperation, *mRecordOmxBuffInfo[0].m_pMMHwBuffer,
                    omxBuffHeader->pBuffer, omxBuffHeader->nAllocLen);

        //perform rotation
        aData.mSwRotation(omxBuffHeader->pBuffer,
                          NULL,
                          mRecordInfo.getAlignedWidth(),
                          mRecordInfo.getAlignedHeight());

        //synch buffer
        synchCBData(MMHwBuffer::ESyncBeforeReadHwOperation, *mRecordOmxBuffInfo[0].m_pMMHwBuffer,
                    omxBuffHeader->pBuffer, omxBuffHeader->nAllocLen);
    }

    mFrameDumper.process(omxBuffHeader->pBuffer, CFrameDump::ERecording);
    mFrameReplay.process(omxBuffHeader->pBuffer, CFrameReplay::ERecording);

    if (mMsgEnabled & CAMERA_MSG_VIDEO_FRAME) {
        // Use the system time for 1st video frame and
        // if slow motion is not enabled.
        if (mVideoTS == 0 || (mSlowMotionFPS == 0)) {
            mVideoTS = systemTime(SYSTEM_TIME_MONOTONIC);
        } else {
            // For slow motion video timestamp needs to be modified
            // Video Timestamp for playback at 30 fps giving the feel
            // of 3x slow motion during playback.
            mVideoTS += 33000000;
        }
        DBGT_PTRACE("Timestamp: %lld", mVideoTS);

        //check buffer not already sent
        DBGT_ASSERT(!(aData.mFlags & OmxBuffInfo::ESentToClient),
                   "Buffer: %p already sent to client flags: 0x%08x",
                   &aData, aData.mFlags);

        //setup flag
        aData.mFlags |= OmxBuffInfo::ESentToClient;

        if (mEnableB2R2DuringRecord) {
            sem_wait(&sem_crop_buffer);
        }

        /* Dispatch callback with out lock. This will allow
         * callback to call cameraHAL APIs. Otherwise there
         * would be a deadlock
         */
        //reset flags
        aData.mFlags &= ~OmxBuffInfo::ESentToHandlerThread;

        mLock.unlock();
        //mDataCbTimestamp(timestamp, CAMERA_MSG_VIDEO_FRAME, aData.mCamMemoryBase, mCallbackCookie);

        mDataCbTimestamp(mVideoTS, CAMERA_MSG_VIDEO_FRAME,
                         aData.mCamHandle, 0, mCallbackCookie);

#ifdef DUMP_FPS_FOR_VIDEO
        mFrameRateDumperVideo.logAndDump();
#endif //DUMP_FPS_FOR_VIDEO

        mLock.lock();
    }

    DBGT_EPILOG("");
    return err;
}

bool STECamera::isPreviewPortEnabled()
{
    DBGT_PROLOG("");

    OMX_PARAM_PORTDEFINITIONTYPE ViewFinderPort0;
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(ViewFinderPort0);
    ViewFinderPort0.nPortIndex = CAM_VPB + 0;
    OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &ViewFinderPort0);

    DBGT_EPILOG("Port Status %d", ViewFinderPort0.bEnabled);

    return (bool)ViewFinderPort0.bEnabled;
}

OMX_ERRORTYPE STECamera::configureCamMode(int aCamMode, bool aStillZSL)
{
    DBGT_PROLOG("aCamMode:%d aStillZSL:%d", aCamMode, aStillZSL);
    DBGT_ASSERT((aCamMode == EStill) || (aCamMode == EVideo), "Invalide Camera Mode");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    status_t status = NO_ERROR;

    DBGT_PINFO("CONFIGURING CAMERA - MODE %d", aCamMode);

    bool previewPortEnabled = isPreviewPortEnabled();
    bool previewRunning = checkPreviewEnabled();

    if (previewPortEnabled) {
        if (previewRunning) {
            err = doStopPreview();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("doStopPreview failed");
                DBGT_EPILOG("");
                return err;
            }
        }
        err = disablePreviewPort(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("disablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
    }

    if (g_IsPreviewConfig) { //Mode changed
        err = preOmxCompCleanUp();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("preOmxCompCleanUp failed");
            DBGT_EPILOG("");
            return err;
        }
    }
    mCamMode = aCamMode;
    switch (mCamMode) {
        case EStill:
            mIsStillZSL = aStillZSL;
            selectThumbnailHandler(mThumbnailInfo.getWidth(), mThumbnailInfo.getHeight());

            err = doStillModeConfig(previewPortEnabled);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("doStillModeConfig failed");
                DBGT_EPILOG("");
                return err;
            }

            err = getThumbnailHandler()->configure(mThumbnailInfo, mPreviewInfo);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("getThumbnailHandler()->configure() failed");
                DBGT_EPILOG("");
                return err;
            }
            break;
        case EVideo:
            mIsStillZSL = false;

            selectThumbnailHandler(mThumbnailInfo.getWidth(), mThumbnailInfo.getHeight());

            err = doVideoModeConfig(previewPortEnabled);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("doVideoModeConfig failed");
                DBGT_EPILOG("");
                return err;
            }

            err = getThumbnailHandler()->configure(mThumbnailInfo, mPreviewInfo);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("getThumbnailHandler()->configure() failed");
                DBGT_EPILOG("");
                return err;
            }
            break;
        default:
            DBGT_CRITICAL("Received invalid cam-mode %d", mCamMode);
            break;
    }
    g_IsPreviewConfig = true;

    if (previewPortEnabled) {
        err = enablePreviewPort(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("enablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
        if (previewRunning) {
            status = doStartPreview();
            if (NO_ERROR != status) {
                DBGT_CRITICAL("doStartPreview failed");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }
        }
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doStillModeConfig(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning: %d", aPreviewRunning);

#ifdef CAM_REINIT_CAM_PROP
    resetProperties();
#endif

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mIsCameraConfigured = true;
    destroyHeaps(aPreviewRunning);

    err  = OmxUtils::setPortState(mCam, OMX_CommandPortEnable,CAM_VPB + 1, NULL,&stateCam_sem);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Enable 1 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

    // First set the Sensor Mode for the Camera for all ports
    sensorMode.nPortIndex = OMX_ALL;
    err = OMX_GetParameter(mCam, OMX_IndexParamCommonSensorMode,  &sensorMode);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamCommonSensorMode passed for camera");
        DBGT_EPILOG("");
        return err;
    }

    sensorMode.nFrameRate = mConfigFrameRate * (1 << 16); /*in Q16*/

    sensorMode.bOneShot = getCamSensorMode();

    err = OMX_SetParameter(mCam, OMX_IndexParamCommonSensorMode,  &sensorMode);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamCommonSensorMode passed for camera");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize CAM_VPB 0
    err = InitializePort(paramPortVPB0, OMX_PortDomainVideo, mCam, CAM_VPB + 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB0 failed");
        DBGT_EPILOG("");
        return err;
    }

    //setup xp70 traces
    setupXP70Traces();

    // Initialize CAM_VPB 1
    err = InitializePort(paramPortVPB1, OMX_PortDomainVideo, mCam, CAM_VPB + 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB0 failed");
        DBGT_EPILOG("");
        return err;
    }

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_EXTCAPTUREMODETYPE> extCapModeConfig;
    extCapModeConfig.ptr()->nPortIndex = CAM_VPB +1;
    OMX_INDEXTYPE captureIndex = mOmxILExtIndex->getIndex(OmxILExtIndex::EExtCaptureMode);

    err = OMX_GetConfig(mCam, captureIndex, extCapModeConfig.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("ExtCaptureMode  Cam OMX_GetConfig failed");
        DBGT_EPILOG("");
        return err;
    }

    extCapModeConfig.ptr()->nPortIndex = CAM_VPB +1;
    if (mIsStillZSL) {
        extCapModeConfig.ptr()->nFrameBefore = 1;
        extCapModeConfig.ptr()->bPrepareCapture = OMX_TRUE;
    } else {
        extCapModeConfig.ptr()->nFrameBefore = 0;
        extCapModeConfig.ptr()->bPrepareCapture = OMX_FALSE;
    }

    err = OMX_SetConfig(mCam, captureIndex, extCapModeConfig.ptr());
    DBGT_PTRACE("OMX_SetConfig extCapModeConfig: %#x",err);

    err = OMX_GetConfig(mCam, captureIndex, extCapModeConfig.ptr());

    DBGT_PTRACE("nFrameBefore: %d, bPrepareCapture: %d",(int)extCapModeConfig.ptr()->nFrameBefore,extCapModeConfig.ptr()->bPrepareCapture);

    OmxUtils::StructContainer<OMX_CONFIG_CAPTUREMODETYPE> capModeConfig;
    capModeConfig.ptr()->nPortIndex = CAM_VPB +1;
    err = OMX_GetConfig(mCam, OMX_IndexConfigCaptureMode, capModeConfig.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_IndexConfigCaptureMode Cam OMX_GetConfig failed");
        DBGT_EPILOG("");
        return err;
    }
    DBGT_PTRACE("nPortIndex: %d, bFrameLimited: %d, nFrameLimit: %d",(int)capModeConfig.ptr()->nPortIndex, capModeConfig.ptr()->bFrameLimited, (int)capModeConfig.ptr()->nFrameLimit);
    capModeConfig.ptr()->nPortIndex = CAM_VPB +1;

    capModeConfig.ptr()->bContinuous = OMX_FALSE;
    capModeConfig.ptr()->bFrameLimited = OMX_TRUE;
    if (mIsStillZSL) {
        capModeConfig.ptr()->nFrameLimit = 0;
    } else {
        capModeConfig.ptr()->nFrameLimit = 1;
    }

    err = OMX_SetConfig(mCam, OMX_IndexConfigCaptureMode, capModeConfig.ptr());
    DBGT_PTRACE("OMX_SetConfig OMX_IndexConfigCaptureMode: %#x",err);

    err = OMX_GetConfig(mCam, OMX_IndexConfigCaptureMode, capModeConfig.ptr());

    DBGT_PTRACE("nPortIndex: %d, bFrameLimited: %d, nFrameLimit: %d",(int)capModeConfig.ptr()->nPortIndex, capModeConfig.ptr()->bFrameLimited, (int)capModeConfig.ptr()->nFrameLimit);

    if (!mIsStillZSL) {
#if defined(SWISPPROC) || defined(HWISPPROC)
        /* Same Input port configuration as VPB1 of Camera */
        // Initialize ISP proc port 0
        err = InitializePort(paramISPinput, OMX_PortDomainVideo, mISPProc, 0);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mISPProc port 0 failed");
            DBGT_EPILOG("");
            return err;
        }

        // Initialize ISP proc port 2
#ifdef SWISPPROC
        err = InitializePort(paramISPoutput, OMX_PortDomainVideo, mISPProc, 1);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mISPProc port 1 failed");
            DBGT_EPILOG("");
            return err;
        }

#elif HWISPPROC
        err = InitializePort(paramISPoutput, OMX_PortDomainVideo, mISPProc, 2);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mISPProc port 2 failed");
            DBGT_EPILOG("");
            return err;
        }

        // Initialize ISP Port 1
        err = InitializePort(paramISPoutput1, OMX_PortDomainVideo, mISPProc, 1);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mISPProc port 1 failed");
            DBGT_EPILOG("");
            return err;
        }
#endif
#endif // SWISPPROC || HWISPPROC
    }

#ifdef JPEGENC
    // Initialize JpegEnc port 0
    err = InitializePort(paramJPEGinput, OMX_PortDomainImage, mJpegEnc, 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 0 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize JpegEnc port 1
    err = InitializePort(paramJPEGoutput, OMX_PortDomainImage, mJpegEnc, 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    if (mPictureInfo.invalidQuality() != mPictureInfo.getQuality()) {
        //setup jpeg quality
        OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
        qfactor.ptr()->nPortIndex = 1;
        qfactor.ptr()->nQFactor = mPictureInfo.getQuality();

        err = OMX_SetParameter(mJpegEnc, OMX_IndexParamQFactor, qfactor.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Setparameter OMX_IndexParamQFactor for mJpegEnc failed");
            DBGT_EPILOG("");
            return err;
        }
    }
#endif // JPEGENC

    // Initialize EXIF input 2
    err = InitializePort(paramEXIFinput2, OMX_PortDomainImage, mExifMixer, 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize EXIF output
    err = InitializePort(paramEXIFoutput, OMX_PortDomainImage, mExifMixer, 2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 2 failed");
        DBGT_EPILOG("");
        return err;
    }

    err = setMakeAndModel();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("setMakeandModel failed");
        DBGT_EPILOG("");
        return err;
    }

    // Configure ISPproc and ARMIV for auto-rotation

    err = getRotationHandler()->configAutoRotate();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("getRotationHandler->configAutoRotate() failed");
        DBGT_EPILOG("");
        return err;
    }

    err = getRotationHandler()->setupRotationProcessing();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("getRotationHandler->setupRotationProcessing()");
        DBGT_EPILOG("");
        return err;
    }

    //make tunnel between Output port of  JPEGENC with input port 1 of EXIF MIxer
    DBGT_PTRACE("JPEGENC port: %lu EXIF Mixer POrt: %lu", paramJPEGoutput.nPortIndex, paramEXIFinput2.nPortIndex);
    err = (mOmxUtils.interface()->GetpOMX_SetupTunnel())(mJpegEnc, paramJPEGoutput.nPortIndex,
        mExifMixer, paramEXIFinput2.nPortIndex);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetupTunnel failed between JPEGENC and EXIFMIXER");
        DBGT_EPILOG("");
        return err;
    }

    if (!mIsStillZSL) {
#if !defined(SWISPPROC) && !defined(HWISPPROC)
    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 1,NULL,&stateCam_sem);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable VPB1 failed ");
        DBGT_EPILOG("");
        return err;
    }
#endif // SWISPPROC || HWISPPROC
    }

    OMX_STATETYPE state;
    OMX_GetState(mCam, &state);
    DBGT_PTRACE("state before sending idle for camera= %s", OmxUtils::name(state));

    if (!mIsStillZSL) {
#if defined(SWISPPROC) || defined(HWISPPROC)
    OMX_GetState(mISPProc, &state);
    DBGT_PTRACE("state before sending idle for ISPPROC= %s", OmxUtils::name(state));
#endif // SWISPPROC || HWISPPROC
    }

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    OMX_GetState(mJpegEnc, &state);
    DBGT_PTRACE("state before sending idle for JPEGENC= %s", OmxUtils::name(state));

    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mJpegEnc");
        DBGT_EPILOG("");
        return err;
    }
#endif // JPEGENC

    if (!mIsStillZSL) {
#if defined(SWISPPROC) || defined(HWISPPROC)

#ifdef HWISPPROC

    // We need the port 1 enabled now for Raw Image
    // err = OMX_SendCommand(mISPProc, OMX_CommandPortDisable, 1, NULL);

#endif // HWISPPROC

    err = OMX_SendCommand(mISPProc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mISPProc");
        DBGT_EPILOG("");
        return err;
    }
#endif // SWISPPROC || HWISPPROC
    }

    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mCam");
        DBGT_EPILOG("");
        return err;
    }

    if (mIsStillZSL) {
        // Initialize heaps
        paramISPoutput1.nBufferSize = paramPortVPB0.nBufferSize;

        DBGT_PTRACE("dump paramISPoutput1 Buffer Size = %d", (unsigned int)paramISPoutput1.nBufferSize);
        pISP1outBuffer = new OMX_BUFFERHEADERTYPE;
    }
    initSnapshotHeap();
    if (mIsStillZSL) {
        pISP1outBuffer->pBuffer = (OMX_U8*)mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
    }

    initStillHeap();

    // Allocating EXIF output buffer
    pEXIFoutBuffer = NULL;
    OMX_U8* exifBuf = new OMX_U8[paramEXIFoutput.nBufferSize];
    DBGT_PTRACE("exif out Buffer: %p",exifBuf);
    if (NULL == exifBuf) {
        DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for exifBuf");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }
    err = OMX_UseBuffer(mExifMixer, &pEXIFoutBuffer, 2, NULL, (OMX_U32)(paramEXIFoutput.nBufferSize), exifBuf);
    if (err != OMX_ErrorNone ||  pEXIFoutBuffer->pBuffer == 0) {
        DBGT_CRITICAL("OMX_AllocateBuffer failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }
    if ((err != OMX_ErrorNone) && (exifBuf != NULL)) {
         delete[] exifBuf;
    }

    //CAM VPB 1
    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer for mCam port 1\n");

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mStillOmxBuffInfo[0].mMMHwChunkMetaData);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
    }

    DBGT_PTRACE("Usebuffer for mCam port 1");

    for (unsigned int i = 0; i < paramPortVPB1.nBufferCountActual; i++) {
        pStillBuffer[i] = NULL;
        err = OMX_UseBuffer(mCam,    // hComponent
            &pStillBuffer[i],    // address where ptr to buffer header will be stored
            paramPortVPB1.nPortIndex,          // port index (for port for which buffer is provided)
            (void *)(&mStillOmxBuffInfo[i]),    // App. private data = pointer to beginning of allocated data
            // to have a context when component returns with a callback (i.e. to know
            // what to free etc.
            (OMX_U32)(paramPortVPB1.nBufferSize),        // buffer size
            (OMX_U8 *)(mStillOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr) // buffer data ptr
);
        DBGT_PTRACE("OMX_UseBuffer for pStillBuffer[%d] err: %#x",i,err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_UseBuffer failed for cam port 1 ");
            DBGT_EPILOG("");
            return err;
        }

        DBGT_ASSERT(NULL != pStillBuffer[i]->pBuffer, "Buffer is NULL");

        mStillOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pStillBuffer[i]);
    }

#ifdef JPEGENC
    // Allocate Buffers through HwBuffer
    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer for JPEGENC input port\n");

    err = OMX_SetConfig(mJpegEnc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mJpegOmxBuffInfo[0].mMMHwChunkMetaData);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
    }

    DBGT_PTRACE("Usebuffer for mJpegEnc input port");

    for (unsigned int i = 0; i < paramPortVPB1.nBufferCountActual; i++) {
        pJPEGinBuffer[i] = NULL;
        err = OMX_UseBuffer(mJpegEnc,    // hComponent
            &pJPEGinBuffer[i],    // address where ptr to buffer header will be stored
            paramJPEGinput.nPortIndex,          // port index (for port for which buffer is provided)
            (void *)(&mJpegOmxBuffInfo[i]),    // App. private data = pointer to beginning of allocated data
            // to have a context when component returns with a callback (i.e. to know
            // what to free etc.
            (OMX_U32)(paramJPEGinput.nBufferSize),        // buffer size
            (OMX_U8 *)(mJpegOmxBuffInfo[i].mMMHwBufferInfo.iLogAddr) // buffer data ptr
            );
        DBGT_PTRACE("OMX_UseBuffer for pJPEGinBuffer[%d] err: %#x",i,err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_UseBuffer failed for mJpegEnc port 0 ");
            DBGT_EPILOG("");
            return err;
        }

        DBGT_ASSERT(NULL != pJPEGinBuffer[i]->pBuffer, "Buffer is NULL");

        mJpegOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pJPEGinBuffer[i]);
    }

#endif

    if (!mIsStillZSL) {
#ifdef HWISPPROC

    // Allocate Buffers through HwBuffer
    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer Raw Chunk\n");
    err = OMX_SetConfig(mISPProc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mRawOmxBuffInfo.mMMHwChunkMetaData);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x \n", (OMX_ERRORTYPE)err);
    }

    pISP1outBuffer = NULL;

    DBGT_PTRACE("usebuffer mISPProc ");
    err = OMX_UseBuffer(mISPProc,    // hComponent
        &pISP1outBuffer,    // address where ptr to buffer header will be stored
        CAM_VPB + 1,        // port index (for port for which buffer is provided)
        (void *)(&mRawOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
        // to have a context when component returns with a callback (i.e. to know
        // what to free etc.
        (OMX_U32)(paramISPoutput1.nBufferSize),        // buffer size
        (OMX_U8 *)(mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
        );

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mISPProc port 1 ");
        DBGT_EPILOG("");
        return err;
    }
    DBGT_ASSERT(NULL != pISP1outBuffer->pBuffer, "Buffer is NULL");

    mRawOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(pISP1outBuffer);
#endif

#if defined(SWISPPROC) || defined(HWISPPROC)
    //ISP VPB 0
    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer for isp port 0\n");

    DBGT_PTRACE("nPortIndex: %d, nHandleId: %d, nFd: %d, nBufferSize: %d",
        (int)mISPRawOmxBuffInfo.mMMHwChunkMetaData.nPortIndex,
        (int)mISPRawOmxBuffInfo.mMMHwChunkMetaData.nHandleId,
        (int)mISPRawOmxBuffInfo.mMMHwChunkMetaData.nFd,
        (int)mISPRawOmxBuffInfo.mMMHwChunkMetaData.nBufferSize);

    err = OMX_SetConfig(mISPProc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mISPRawOmxBuffInfo.mMMHwChunkMetaData);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
    }

    pISPinBuffer = NULL;

    DBGT_PTRACE("Usebuffer for isp port 0");

    err = OMX_UseBuffer(mISPProc,    // hComponent
        &pISPinBuffer,    // address where ptr to buffer header will be stored
        paramISPinput.nPortIndex,          // port index (for port for which buffer is provided)
        (void *)(&mISPRawOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
        // to have a context when component returns with a callback (i.e. to know
        // what to free etc.
        (OMX_U32)(paramISPinput.nBufferSize),        // buffer size
        (OMX_U8 *)(mISPRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
        );

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for cam port 1 ");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(NULL != pISPinBuffer->pBuffer, "Buffer is NULL");

    mISPRawOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(pISPinBuffer);

    // ISP VPB 2
    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer for ISPPROC HR port\n");
    err = OMX_SetConfig(mISPProc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mISPImageOmxBuffInfo.mMMHwChunkMetaData);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("OMX_SetConfig Failed errorcode : 0x%x\n", (OMX_ERRORTYPE)err);
    }

    pISP2outBuffer = NULL;

    DBGT_PTRACE("Usebuffer for mISPProc HR port");

    err = OMX_UseBuffer(mISPProc,    // hComponent
        &pISP2outBuffer,    // address where ptr to buffer header will be stored
        paramISPoutput.nPortIndex,          // port index (for port for which buffer is provided)
        (void *)(&mISPImageOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
        // to have a context when component returns with a callback (i.e. to know
        // what to free etc.
        (OMX_U32)(paramISPoutput.nBufferSize),        // buffer size
        (OMX_U8 *)(mISPImageOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
        );

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mISPProc port 2 ");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(NULL != pISP2outBuffer->pBuffer, "Buffer is NULL");

    mISPImageOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(pISP2outBuffer);
#endif
    }

    DBGT_PTRACE("Going Waiting for IDLE CAMERA\n");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle CAMERA\n");

    if (!mIsStillZSL) {
#if defined(SWISPPROC) || defined(HWISPPROC)
    DBGT_PTRACE("Going Waiting for IDLE ISPPROC\n");
    camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle ISPPROC\n");
#endif // SWISPPROC || HWISPPROC
    }

#ifdef JPEGENC
    DBGT_PTRACE("Going Waiting for IDLE JPEGENC\n");
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle \n");
#endif // JPEGENC

    DBGT_PTRACE("Going Waiting for IDLE mExifMixer\n");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle mExifMixer\n");

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mJpegEnc");
        DBGT_EPILOG("");
        return err;
    }
    camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);

#endif // JPEGENC

    if (!mIsStillZSL) {
#if defined(SWISPPROC) || defined(HWISPPROC)
    err = OMX_SendCommand(mISPProc, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mISPProc");
        DBGT_EPILOG("");
        return err;
    }
    camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
#endif // SWISPPROC || HWISPPROC
    }

    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);

    //send to executing
    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for Camera");
        DBGT_EPILOG("");
        return err;
    }
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);

#ifdef CAM_REINIT_CAM_PROP
    status_t status = applyCameraProperties(mParameters);
    if (NO_ERROR != status) {
        DBGT_CRITICAL("OMX_ErrorUndefined - applyCameraProperties failed");
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }
#endif

    //Always setup all extra keys
    setupExtraKeys();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::disableVideoPorts()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 2,NULL,&stateCam_sem);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port disable 2 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::disableAllCamPorts()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable, OMX_ALL, NULL, NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port disable OMX_ALL mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::disableStillPorts()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 1,NULL,&stateCam_sem);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port disable 1 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::clearStillModeConfig()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = sendStillComponentsToIdleState();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("sendStillComponentsToIdleState failed");
        DBGT_EPILOG("");
        return err;
    }

    err = sendStillComponentsToLoadedState();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("sendStillComponentsToLoadedState failed");
        DBGT_EPILOG("");
        return err;
    }

    err = disableStillPorts();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("disableStillPorts failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::clearVideoModeConfig()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = sendVideoComponentsToIdleState();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("sendVideoComponentsToIdleState failed");
        DBGT_EPILOG("");
        return err;
    }

    err = sendVideoComponentsToLoadedState();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("sendVideoComponentsToLoadedState failed");
        DBGT_EPILOG("");
        return err;
    }

    err = disableVideoPorts();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("disableVideoPorts failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doVideoModeConfig(bool aPreviewRunning /*false*/)
{
    DBGT_PROLOG("aPreviewRunning: %d", aPreviewRunning);
    OMX_ERRORTYPE err = OMX_ErrorNone;

#ifdef CAM_REINIT_CAM_PROP
    resetProperties();
#endif

    mIsCameraConfigured = true;
    mSnapshotDuringRecord = false;
    mSnapshotDuringRecIdx = -1;

    destroyHeaps(aPreviewRunning);


    err  = OmxUtils::setPortState(mCam, OMX_CommandPortEnable,CAM_VPB + 2,NULL,&stateCam_sem);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Enable 2 mHSMCamera failed");
        DBGT_EPILOG("");
        return err;
    }

#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    mRecordInfo.setRotation(0);
    //Before omx setparameter first Reset the Rotation to zero
    err = configVideoRevertAspectRatio(mCam, OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Aspect Ratio failed");
        DBGT_EPILOG("");
        return err;
    }

    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
#elif ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
    mRecordInfo.setRotation(gRotation);
#endif //ENABLE_VIDEO_ROTATION

    // First set the Sensor Mode for the Camera for all ports
    sensorMode.nPortIndex = OMX_ALL;
    err = OMX_GetParameter(mCam, OMX_IndexParamCommonSensorMode,  &sensorMode);

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamCommonSensorMode passed for camera");
        DBGT_EPILOG("");
        return err;
    }

    sensorMode.nFrameRate = mConfigFrameRate * (1 << 16); /*in Q16*/

    sensorMode.bOneShot = getCamSensorMode();

    err = OMX_SetParameter(mCam, OMX_IndexParamCommonSensorMode,  &sensorMode);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamCommonSensorMode passed for camera");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize CAM_VPB 0
    err = InitializePort(paramPortVPB0, OMX_PortDomainVideo, mCam, CAM_VPB + 0, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB0 failed");
        DBGT_EPILOG("");
        return err;
    }

    //setup xp70 traces
    setupXP70Traces();

    // Initialize CAM_VPB 2
    err = InitializePort(paramPortVPB2, OMX_PortDomainVideo, mCam, CAM_VPB + 2, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mCam VPB2 failed");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    // Initialize JpegEnc port 0
    err = InitializePort(paramJPEGinput, OMX_PortDomainImage, mJpegEnc, 0, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 0 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize JpegEnc port 1
    err = InitializePort(paramJPEGoutput, OMX_PortDomainImage, mJpegEnc, 1, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    if (mPictureInfo.invalidQuality() != mPictureInfo.getQuality()) {
        //setup jpeg quality
        OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
        qfactor.ptr()->nPortIndex = 1;
        qfactor.ptr()->nQFactor = mPictureInfo.getQuality();

        err = OMX_SetParameter(mJpegEnc, OMX_IndexParamQFactor, qfactor.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Setparameter OMX_IndexParamQFactor for mJpegEnc failed");
            DBGT_EPILOG("");
            return err;
        }

    }
#endif // JPEGENC
    // Initialize EXIF input 2
    err = InitializePort(paramEXIFinput2, OMX_PortDomainImage, mExifMixer, 1, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Initialize EXIF output
    err = InitializePort(paramEXIFoutput, OMX_PortDomainImage, mExifMixer, 2, false);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 2 failed");
        DBGT_EPILOG("");
        return err;
    }

    /****************************************************************************/

    //make tunnel between Output port of  JPEGENC with input port 1 of EXIF MIxer
    DBGT_PTRACE("JPEGENC port: %lu EXIF Mixer POrt: %lu", paramJPEGoutput.nPortIndex, paramEXIFinput2.nPortIndex);
    err = (mOmxUtils.interface()->GetpOMX_SetupTunnel())(mJpegEnc, paramJPEGoutput.nPortIndex,
        mExifMixer, paramEXIFinput2.nPortIndex);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetupTunnel failed between JPEGENC and EXIFMIXER");
        DBGT_EPILOG("");
        return err;
    }

#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    setupVideoSwRotProcessing(0);
#elif ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
    err = setPropVideoRotate(mCam, mRecordInfo.getRotation());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Common Rotate failed");
        DBGT_EPILOG("");
        return err;
    }

    setupVideoSwRotProcessing(mRecordInfo.getRotation());
#endif //ENABLE_VIDEO_ROTATION

    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mCam");
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("disableVideoPorts failed");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mCam");
        DBGT_EPILOG("");
        return err;
    }
#endif

    // Allocating EXIF output buffer
    pEXIFoutBuffer = NULL;
    OMX_U8* exifBuf = new OMX_U8[paramEXIFoutput.nBufferSize];
    DBGT_PTRACE("exif out Buffer: %p",exifBuf);
    if (NULL == exifBuf) {
        DBGT_CRITICAL("new failed for exifBuf");
        DBGT_EPILOG("");
        return  OMX_ErrorInsufficientResources;
    }
    err = OMX_UseBuffer(mExifMixer, &pEXIFoutBuffer, 2, NULL, (OMX_U32)(paramEXIFoutput.nBufferSize), exifBuf);
    if ((OMX_ErrorNone != err) ||  (pEXIFoutBuffer->pBuffer == 0)) {
        DBGT_CRITICAL("OMX_AllocateBuffer failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }
    if ((err != OMX_ErrorNone) && (exifBuf != NULL)) {
         delete[] exifBuf;
    }

    // Configure the video Heap
    configureVideoHeap();

    paramISPoutput1.nBufferSize = paramPortVPB0.nBufferSize;

    DBGT_PTRACE("dump paramISPoutput1 Buffer Size = %d", (unsigned int)paramISPoutput1.nBufferSize);
    pISP1outBuffer = new OMX_BUFFERHEADERTYPE;

    initSnapshotHeap();

    // Assign the Raw Buff Info logical address to ISP buffer
    // The same will be passed to ARM IV Proc for thumbnail processing
    pISP1outBuffer->pBuffer = (OMX_U8*)mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;

#ifdef ENABLE_VIDEO_STAB
    // If Video Stab is Enabled set the Config Parameter before initializing ports
    if ((mCamMode == EVideo) && (mVideoStabEnabled == true)) {
        DBGT_PTRACE("Enable the video stab using SetConfig");
        err = setConfigVideoStab();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Video Stab Config for mCam failed");
            DBGT_EPILOG("");
            return err;
        }
    }
#endif

    DBGT_PTRACE("Going Waiting for IDLE CAMERA\n");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle CAMERA\n");

#ifdef JPEGENC
    DBGT_PTRACE("Going Waiting for IDLE JPEGENC\n");
    sem_wait(&stateJpeg_sem);
    DBGT_PTRACE("Came Out of Wait State Idle \n");
#endif // JPEGENC

    DBGT_PTRACE("Going Waiting for IDLE mExifMixer\n");
    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("Came Out of Wait State Idle mExifMixer\n");

    err = OMX_SendCommand(mExifMixer, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mExifMixer");
        DBGT_EPILOG("");
        return err;
    }

#ifdef JPEGENC
    DBGT_PTRACE("Sending JpegEnc in Executing State");
    err = OMX_SendCommand(mJpegEnc, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mJpegEnc");
        DBGT_EPILOG("");
        return err;
    }
    sem_wait(&stateJpeg_sem);
    DBGT_PTRACE("JpegEnc now in Executing State");
#endif // JPEGENC

    camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("ExifMixer now in Executing State");

    //send to executing
    err = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for Camera");
        DBGT_EPILOG("");
        return err;
    }
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);

#ifdef CAM_REINIT_CAM_PROP
    status_t status = applyCameraProperties(mParameters);
    if (NO_ERROR != status) {
        DBGT_CRITICAL("applyCameraProperties failed - OMX_ErrorUndefined");
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }
#endif

    //Always setup all extra keys
    setupExtraKeys();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::supplyPreviewBuffers()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
        /* Push buffer only if not already pushed */
        OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pVFBuffer[i]->pAppPrivate);
        DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");

        /* buffinfo should be owned by camera hal and not owned
           by native window */
        DBGT_PTRACE("mflags = %d", buffInfo->mFlags);
        if ((OmxBuffInfo::ECameraHal & buffInfo->mFlags) && !(OmxBuffInfo::ESentToComp & buffInfo->mFlags)) {
            DBGT_PTRACE("own by camera hal.....sent to component index: %d",i);
            getLockForNativeBuffer(i);  //lock buffer before sending it to omx components
            err = OmxBuffInfo::fillBuffer(mCam, pVFBuffer[i]);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("fillBuffer failed");
                DBGT_EPILOG("");
                return err;
            }
            /* Now buff owner is OMX component, reset camera hal ownership */
            buffInfo->mFlags &= ~OmxBuffInfo::ECameraHal;
        }
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::supplyRecordBuffers()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    for (int i = 0; i < kRecordBufferCount; i++) {
        /* There is a bug in Stagefright that sometime all buffers are not
         * returned back to CameraHAL on stopRecording.
         *
         * But internally stagefright assumes that all buffers have been
         * returned. So its safe to reset all flags for client
         */
        OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pRecBuffer[i]->pAppPrivate);
        DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
        //reset flags
        buffInfo->mFlags &= ~OmxBuffInfo::ESentToClient;

        err = OmxBuffInfo::fillBuffer(mCam, pRecBuffer[i]);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("fillBuffer failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::startPreview()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;
    if(mPreviewRunning)
    {
        if((mCamMode == EVideo) && (mPreviousCamMode == EVideo) && !mIsrecordsizechanged  && !mIspreviewsizechanged)
        {
            DBGT_PTRACE("!!!!!!!This was a dummy startPreview, need to return without doing anything");
            return NO_ERROR;
        }
    }
    //perf trace
    if (!mIsFirstInstanceViewFinder) {
        mIsFirstInstanceViewFinder = true;
        mPerfManager.logAndDump(PerfManager::EViewFinderRequested);
    } else {
        mPerfManager.logAndDump(PerfManager::EViewFinderRequested);
    }

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    Mutex::Autolock _l(mLock);

    status = doStartPreview();

    DBGT_EPILOG("status = %d", status);
    return status;
}

status_t STECamera::doStartPreview()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    status_t status = NO_ERROR;

    isStartPreview = true;

    DBGT_PTRACE("mPreviewRunning %d mIsCameraConfigured %d",
            mPreviewRunning, mIsCameraConfigured);

    DBGT_PTRACE("g_IsPreviewConfig %d mCamMode %d",
            g_IsPreviewConfig, mCamMode);

    DBGT_PTRACE("mIsStillZSL: %d",mIsStillZSL);

    //Return error if preview is already running
    if (mPreviewRunning) {
        DBGT_CRITICAL("Preview should not be running");
        DBGT_EPILOG("");
        return INVALID_OPERATION;
    }

    /*Configure here if the startPreview called before setParameters*/
    DBGT_PTRACE("g_IsPreviewConfig: %d",g_IsPreviewConfig);
    if (!g_IsPreviewConfig) {
        err = configureCamMode(mCamMode, mIsStillZSL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("configureCamMode failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    OMX_STATETYPE state;
    OMX_GetState(mCam, &state);
    DBGT_ASSERT(state == OMX_StateExecuting, "Invalid State");

    DBGT_PTRACE("mIsStillZSL: %d",mIsStillZSL);
    if (mIsStillZSL) {
        for (unsigned int i = 0; i < paramPortVPB1.nBufferCountActual; i++) {
            err = OmxBuffInfo::fillBuffer(mCam, pStillBuffer[i]);
            DBGT_PTRACE("OMX_FillThisBuffer on mCam %d err: %#x",i,err);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
        }
    }

    // For Hi Resolution View finder need to enable capturing bit on Port 2
    if (mHiResVFEnabled) {
        err = updateCapturing(CAM_VPB + 2, OMX_TRUE);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("updateCapturing failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    /* If preview window is present that means set preview
       window is called before startpreview.In that we will be
       sending buffer to PORT 0 here.
       */
    if (mPreviewWindow) {
        err = postOMXConfigStartViewfinder(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("postOMXConfigStartViewfinder failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    mPreviewRunning = true;

    DBGT_EPILOG("");
    return status;
}

void STECamera::stopPreview()
{
    DBGT_PROLOG("");

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    Mutex::Autolock prevLock(mPrevLock);
    Mutex::Autolock _l(mLock);

    //perf trace
    mPerfManager.logAndDump(PerfManager::EViewFinderRequested);

    if (isStartPreview == false) {
        DBGT_PTRACE("Preview Not Enabled");
        return;
    }
    if((mCamMode == EVideo) && (mPreviousCamMode == EVideo) && mIspreviewsizechanged)
    {
        DBGT_PTRACE("!!!!!!!This was a dummy stoppreview , need to return without doing anything");
        return ;
    }
    //stop only if running
    if (mPreviewRunning) {
        OMX_ERRORTYPE err = doStopPreview();

        // Disable the port only if application calls StopPreview
        // Since in such case only will the application call SetPreviewWindow
        // along with StartPreview
        // Disabling should not happen in case StopPreview is called as part of
        // TakePicture.
        if (mPreviewWindow) {
            // cancel all native buffers and then disable camera port 0
            cancelCameraHalNativeBuffers();

            err = disablePreviewPort();
            if (err != OMX_ErrorNone) {
                DBGT_CRITICAL("disablePreviewPort failed");
            }
        }

        if (OMX_ErrorNone != err)
            dispatchError(err);
    }
    //perf trace
    mPerfManager.logAndDump(PerfManager::EViewFinderStopped);

#ifdef ENABLE_VIDEO_STAB
    if (mPreviewWindow != NULL) {
        // Reset the crop Vector
        mPreviewWindow->setCrop(0, 0, mConfigPreviewWidth-1, mConfigPreviewHeight-1);
    }
#endif //ENABLE_VIDEO_STAB

    resetProperties();
    isStartPreview = false;

    DBGT_EPILOG("");
    return;
}

int STECamera::previewEnabled()
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    bool previewRunning = checkPreviewEnabled();

    DBGT_EPILOG("previewRunning - %d", previewRunning);
    return previewRunning;
}

bool STECamera::checkPreviewEnabled()
{
    DBGT_PROLOG("");

    bool previewRunning = mPreviewRunning;

    DBGT_EPILOG("");
    return previewRunning;
}

status_t STECamera::startRecording()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;
    OMX_ERRORTYPE err = OMX_ErrorNone;
    int top, left, width, height;
    OMX_COLOR_FORMATTYPE eColorFormat;

    if (mOmxRecordPixFmt == OMX_COLOR_FormatYUV420SemiPlanar) {
        eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
    } else {
        eColorFormat = mOmxRecordPixFmt;
    }

    mRecordBlitter = new SteHwConv(
        0, // rotation
        mRecordInfo.getOverScannedWidth(),
        mRecordInfo.getOverScannedHeight(),
        (OMX_COLOR_FORMATTYPE)eColorFormat,
        mRecordInfo.getAlignedWidth(),
        mRecordInfo.getAlignedHeight(),
        (OMX_COLOR_FORMATTYPE)mOmxRecordPixFmt );

    // set the source cropping zone
    if ((mRecordInfo.getOverScannedWidth() * mRecordInfo.getHeight()) > (mRecordInfo.getWidth() * mRecordInfo.getOverScannedHeight())) {
        height = mRecordInfo.getOverScannedHeight();
        width = (height * mRecordInfo.getWidth()) / mRecordInfo.getHeight();
        left = (mRecordInfo.getOverScannedWidth() - width)/2;
        top =  0;
    } else if ((mRecordInfo.getOverScannedWidth() * mRecordInfo.getHeight()) < (mRecordInfo.getWidth() * mRecordInfo.getOverScannedHeight())) {
        width = mRecordInfo.getOverScannedWidth();
        height = (width * mRecordInfo.getHeight()) / mRecordInfo.getWidth();
        left = 0;
        top = (mRecordInfo.getOverScannedHeight() - height)/2;
    } else {
        width =   mRecordInfo.getOverScannedWidth();
        height =  mRecordInfo.getOverScannedHeight();
        left = 0;
        top = 0;
    }

    // set destination cropping zone
    mRecordBlitter->setCropSrc(left, top, width, height);

    // set destination cropping zone
    mRecordBlitter->setCropDst(0, 0, mRecordInfo.getWidth(), mRecordInfo.getHeight());

    if (mHiResVFEnabled) {
        DBGT_PTRACE("HiResVF Feature Enabled, StartRecording is a Invalid Operation");
        status = INVALID_OPERATION;

        DBGT_EPILOG("");
        return status;
    }

    //if already enabled, return error
    if (recordingEnabled()) {
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    Mutex::Autolock lock(mLock);

    if (EVideo != mCamMode) {
        DBGT_ASSERT(EStill == mCamMode, "Invalid Camera Mode");

        err = configureCamMode(EVideo, mIsStillZSL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("configureCamMode failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

    }

    //perf trace
    mPerfManager.logAndDump(PerfManager::EVideoRequested);

#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    /*Check if the current orientation requires any configuration changes*/
    if (gRotation != mRecordInfo.getRotation()) {

        mPreviewRunning = false;

        disableCallbacks(mPreviewOmxBuffInfo, kTotalPreviewBuffCount);

        //flush pending buffers
        status = flushViewFinderBuffers();
        if (NO_ERROR != status) {
            DBGT_CRITICAL("flushViewFinderBuffers failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        //Stop duming frames
        mFrameDumper.stop(CFrameDump::EPreview);
        mFrameReplay.stop(CFrameReplay::EPreview);

        dispatchRequest(&STECamera::doStartRecording, NULL);

    } else
#endif //ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    {
        err = postOMXConfigStartVideoRecord();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("postOMXConfigStartVideoRecord failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    // Start the B2R2Thread for B2R2 Operations
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&mB2R2Thread, NULL, B2R2ThreadWrapper, this);
    pthread_attr_destroy(&attr);


    DBGT_EPILOG("");
    return status;
}

OMX_ERRORTYPE STECamera::doStartRecording(void* aArg)
{
    DBGT_PROLOG("");

    int preview_width = 0, preview_height = 0;
    int rotation = 0;
    OMX_ERRORTYPE err,stateChangeErr;

    mParameters.getPreviewSize(&preview_width, &preview_height);
    mRecordInfo.setRotation(gRotation);
    rotation = mRecordInfo.getRotation();

    //wait for last preview request to complete
    while (mPreviewThread->get()->isRequestPending() != NULL)
        usleep(5000);

#if 0

    DBGT_PTRACE("In doStartRecording, sending Command Port Disable for mCam Port 0");
    OMX_ERRORTYPE err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable, CAM_VPB + 0,NULL, NULL);
    if (OMX_ErrorNone != err && OMX_ErrorSameState == err) {
        DBGT_CRITICAL("Port Disable 0 mCam failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    /*  Freeing the buffers */
    if (OMX_ErrorSameState != err) {
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 0), pVFBuffer[i]); // to see the logic
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("Free Buffer port 0 mCam failed err = %d", err);
                DBGT_EPILOG("");
                return err;
            }
        }
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }
#endif


    DBGT_PTRACE("In doStartRecording, sending Command Port Disable for mCam VPB2");
    err = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 2, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable VPB2 mCam failed");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        for (int i = 0; i < kRecordBufferCount; i++) {
            if ( NULL != pRecBuffer[i]) {
                err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 2), pRecBuffer[i]); // to see the logic
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("Free Buffer port VPB2 mCam failed");
                    DBGT_EPILOG("");
                    return err;
                }
            }
        }
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }

    //Before omx setparameter first Reset the Rotation to zero
    err = configVideoRevertAspectRatio(mCam, OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Aspect Ratio failed");
        DBGT_EPILOG("");
        return err;
    }

    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);

    //set Common Rotate to zero
    err = setPropVideoRotate(mCam, 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Config Common Rotate failed");
        DBGT_EPILOG("");
        return err;
    }

#if 0
    //Port COnfiguration
    err = OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Getparameter OMX_IndexParamPortDefinition mCam port VPBO failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    /*  Reconfigure port 0 */
    paramPortVPB0.nBufferCountActual = kTotalPreviewBuffCount;

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE    *pt_video = &(paramPortVPB0.format.video);
    pt_video->cMIMEType = (OMX_STRING)"";
    pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video->nFrameWidth = (OMX_U32) mPreviewInfo.getAlignedWidth();
    pt_video->nFrameHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
    pt_video->nStride = (OMX_U32)(2 * mPreviewInfo.getAlignedWidth());
    pt_video->nSliceHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
    pt_video->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_video->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_video->eColorFormat = cameraFormatInfo().mPreviewOmxColorFormat;// for video/still preview
    pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video->xFramerate = mConfigFrameRate;

    err = OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Setparameter OMX_IndexParamPortDefinition mCam port VPBO failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }
#endif


    /*  Reconfigure port 2 */
    err = OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Getparameter OMX_IndexParamPortDefinition mCam port VPB2 failed ");
        DBGT_EPILOG("");
        return err;
    }

    paramPortVPB2.nBufferCountActual = kRecordBufferCount;     // the index of the input port. Should be modified.

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE    *pt_video2 = &(paramPortVPB2.format.video);
    pt_video2->cMIMEType = (OMX_STRING)"";
    pt_video2->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video2->nFrameWidth = (OMX_U32)mRecordInfo.getAlignedWidth();
    pt_video2->nFrameHeight = (OMX_U32)mRecordInfo.getAlignedHeight();
    pt_video2->nStride = (OMX_U32)(1.5 * mRecordInfo.getAlignedWidth());
    pt_video2->nSliceHeight = (OMX_U32)mRecordInfo.getAlignedHeight();
    pt_video2->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_video2->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_video2->eColorFormat = mOmxRecordPixFmt;
    pt_video2->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video2->xFramerate = mConfigFrameRate * (1 << 16); /*in Q16*/

    err = OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Getparameter OMX_IndexParamPortDefinition mCam port VPB2 failed ");
        DBGT_EPILOG("");
        return err;
    }

    if (rotation == 90 || rotation == 270) {
        //Before omx setparameter first Reset the Rotation to zero
        err = configVideoRevertAspectRatio(mCam, OMX_TRUE);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Video Config Aspect Ratio failed");
            DBGT_EPILOG("");
            return err;
        }

        camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);
        camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);

        //set Common Rotate to zero
        err = setPropVideoRotate(mCam, rotation);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Video Config Common Rotate failed");
            DBGT_EPILOG("");
            return err;
        }
    }

    //set if any SW Processing need to be done for current orientation
    setupPreviewSwRotProcessing(rotation);
    setupVideoSwRotProcessing(rotation);

    err = OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Getparameter OMX_IndexParamPortDefinition mCam port VPBO failed ");
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("In doStartRecording, Getparameter OMX_IndexParamPortDefinition mCam port VPB2 failed ");
        DBGT_EPILOG("");
        return err;
    }

    //dump
    DBGT_PTRACE("Dumping params for Camera VPB0 after Rotation:%d", rotation);
    OmxUtils::dump(paramPortVPB0);

    //dump
    DBGT_PTRACE("Dumping params for Camera VPB2 after Rotation:%d", rotation);
    OmxUtils::dump(paramPortVPB2);

#if 0
    DBGT_PTRACE("In doStartRecording, sending Command Port Enable for mCam Port VPB0");
    err = OmxUtils::setPortState(mCam, OMX_CommandPortEnable, CAM_VPB + 0,NULL, NULL);
    if (OMX_ErrorNone != err && OMX_ErrorSameState == err) {
        DBGT_CRITICAL("Port Disable 0 mCam failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mChunkDataPreview);
    if (OMX_ErrorSameState != err) {
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            pVFBuffer[i] = NULL;
            DBGT_PTRACE("Filling data for usebuffer 1 ");
            err = OMX_UseBuffer(mCam,    // hComponent
                            & (pVFBuffer[i]),   // address where ptr to buffer header will be stored
                            CAM_VPB + 0,        // port index (for port for which buffer is provided)
                            (void *)(&mPreviewOmxBuffInfo[i]), // App. private data = pointer to beginning of allocated data
                            // to have a context when component returns with a callback (i.e. to know
                            // what to free etc.
                            (OMX_U32)(mPreviewFrameSize),        // buffer size
                            (OMX_U8 *)(mPreviewHeapBaseLogical + i * mPreviewFrameSize) // buffer data ptr
                            );

            if (err == OMX_ErrorNone &&  pVFBuffer[i]->pBuffer != 0) {
            DBGT_PTRACE("In doStartRecording, Usebuffer Done\n");
            //setup buffer header
            mPreviewOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pVFBuffer[i]);
            mPreviewThreadData[i].mSwProcessingThreadData->mOmxBufferHeader = pVFBuffer[i];
            } else {
                DBGT_CRITICAL("UseBuffer err= %d (0x%x)  \n", (OMX_ERRORTYPE) err, (OMX_ERRORTYPE) err);
            }
        }
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }
#endif //0


    DBGT_PTRACE("In doStartRecording, sending Command Port Enable for mCam Port VPB2");
    stateChangeErr  = OmxUtils::setPortState(mCam, OMX_CommandPortEnable,CAM_VPB + 2, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Enable VPB2 mCam failed");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer\n");
    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mRecordOmxBuffInfo[0].mMMHwChunkMetaData);

#if 0
    // Allocate Buffers through HwBuffer
    for (int i = 0; i < kRecordBufferCount; i++) {
        pRecBuffer[i] = NULL;

        //pBufMetaData[i] = (BufferMetaData *) malloc(sizeof(BufferMetaData));
        DBGT_PTRACE("Filling data for usebuffer 1 ");
        err = OMX_UseBuffer(mCam,    // hComponent
                            & (pRecBuffer[i]),   // address where ptr to buffer header will be stored
                            CAM_VPB + 2,         // port index (for port for which buffer is provided)
                            (void *)(&mRecordOmxBuffInfo[i]), // App. private data = pointer to beginning of allocated data
                            // to have a context when component returns with a callback (i.e. to know
                            // what to free etc.
                            (OMX_U32)(mRecordFrameSize),        // buffer size
                            (OMX_U8 *)(mRecordHeapBaseLogical + i * mRecordFrameSize) // buffer data ptr
                           );

        if (err == OMX_ErrorNone &&  pRecBuffer[i]->pBuffer != 0) {
            DBGT_PTRACE("Usebuffer Done  \n");
            //setup buffer header
            mRecordOmxBuffInfo[i].mCamMemoryBase.get()->setOmxBufferHeaderType(pRecBuffer[i]);
        } else {
            DBGT_CRITICAL("UseBuffer err= %d (0x%x)  \n", (OMX_ERRORTYPE) err, (OMX_ERRORTYPE) err);
        }
    }
#endif //0
    if (OMX_ErrorSameState != stateChangeErr) {
        camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    }

    {
        Mutex::Autolock _l(mLock);

        err = postOMXConfigStartViewfinder(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("postOMXConfigStartViewfinder failed");
            DBGT_EPILOG("");
            return err;
        }

        err = postOMXConfigStartVideoRecord();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("postOMXConfigStartVideoRecord failed");
            DBGT_EPILOG("");
            return err;
        }
    }

#ifdef DISABLE_FPS_CHANGE_EVENT_AFTER_VIDEO_ROTATION
    /*Port Settings Changed event Workaroud ER346039*/
    sem_trywait(&video_rot_sem);
    sem_trywait(&video_rot_sem);
#endif //DISABLE_FPS_CHANGE_EVENT_AFTER_VIDEO_ROTATION

    DBGT_EPILOG("");
    return err;
}

void STECamera::stopRecording()
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    //perf trace
    mPerfManager.logAndDump(PerfManager::EVideoRequested);

    //stop only if running
    if (mRecordRunning) {
        OMX_ERRORTYPE err = doStopRecording();

        if (OMX_ErrorNone != err) {
            dispatchError(err);
        }
    }

    mRecordRunning = false;
    mVideoTS = 0;
    mSlowMotionFPS = 0;

    // Signal the B2R2Thread for exit
    sem_post(&sem_b2r2);
    void* dummy;
    pthread_join(mB2R2Thread, &dummy);

    //perf trace
    mPerfManager.logAndDump(PerfManager::EVideoStopped);

    // remove the record blitter
    delete mRecordBlitter;

    DBGT_EPILOG("");
    return;
}

int STECamera::recordingEnabled()
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    bool recordRunning = checkRecordingEnabled();

    DBGT_PTRACE("Enabled: %d", recordRunning);

    DBGT_EPILOG("recordRunning = %d", (bool)recordRunning);
    return recordRunning;
}

bool STECamera::checkRecordingEnabled()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("mRecordRunning = %d", (bool)mRecordRunning);
    return mRecordRunning;
}

void STECamera::releaseRecordingFrame(const void *mem)
{
    DBGT_PROLOG("Mem: %p", mem);

    Mutex::Autolock lock(mLock);

    if (!mRecordRunning) {
        mLock.unlock();
        DBGT_PTRACE("Recording is not enabled return from here, no release needed");
        return;
    }

    video_metadata_t *metaData = (video_metadata_t*)mem;
    buffer_handle_t buf = (buffer_handle_t)metaData->handle;

    DBGT_PTRACE("record buffer handle =%p", buf);

    /* Find index */
    int index = 0;
    for (int i = 0 ; i < kRecordBufferCount; i++) {
        if (buf == mRecordGraphicBuffer[i].get()->handle) {
            index = i;
            break;
        }
    }

    DBGT_PTRACE("record index =%d", index);

    OmxBuffInfo *buffInfo
        = (OmxBuffInfo *)mRecordOmxBuffInfo[index].mCamMemoryBase->omxBufferHeaderType()->pAppPrivate;

    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
    DBGT_ASSERT((buffInfo->mFlags & OmxBuffInfo::ESentToClient),
               "Invlid buffer returned from client Flags: 0x%08x", buffInfo->mFlags);

    //reset flags
    buffInfo->mFlags &= ~OmxBuffInfo::ESentToClient;

    //push back buffer only if record running
    if (mRecordRunning && (buffInfo->mFlags & OmxBuffInfo::EEnableClientCallback) )
        OMX_ERRORTYPE err
            = OmxBuffInfo::fillBuffer(mCam,
                    mRecordOmxBuffInfo[index].mCamMemoryBase->omxBufferHeaderType());

    DBGT_EPILOG("");
}

status_t STECamera::doAutoFocusProcessing(AutoFocusThreadData &aData)
{
    DBGT_PROLOG("Enabled: %d", aData.mEnabled);

    status_t err = NO_ERROR;

    Mutex::Autolock lock(mLock);

#ifdef ENABLE_AUTOFOCUS
    //if request cancelled
    if (!(aData.mEnabled)
            && !(mMsgEnabled & CAMERA_MSG_FOCUS_MOVE)) {
        DBGT_EPILOG("");
        return err;
    }
#endif //ENABLE_AUTOFOCUS

    OMX_ERRORTYPE omxerr = focusHandler()->handleAFEvent();
    if (OMX_ErrorNone != omxerr) {
        DBGT_CRITICAL("handleAFEvent failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }
    setFocusDistance();

#ifdef SHAKE_DETECTION
    if (mMsgEnabled & CAMERA_MSG_SHAKE_DETECTED) {
        mPerfManager.init(PerfManager::EShakeDetectionRequested);

        /* Shake Detection needs to be enabled after Focus is converged*/
        omxerr = mOmxReqCbHandler.enable(OmxReqCbHandler::EPreCaptureExposureTime);
        if (OMX_ErrorNone != omxerr) {
            DBGT_CRITICAL("enable reqcb failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }
#endif //SHAKE_DETECTION

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::checkForShakeDetection(void* aArg)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    Mutex::Autolock lock(mLock);

    //dont dispatch any past event
    if ((mMsgEnabled & CAMERA_MSG_SHAKE_DETECTED) &&
            mOmxReqCbHandler.isEnabled(OmxReqCbHandler::EPreCaptureExposureTime)) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_PRECAPTUREEXPOSURETIMETYPE> exposureTime;
        exposureTime.ptr()->nPortIndex = CAM_VPB + 1;

        err = OMX_GetConfig(mCam,
                            mOmxILExtIndex->getIndex(OmxILExtIndex::EPreCaptureExposureTime),
                            exposureTime.ptr());

        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("GetConfig failed for ShakeDet");
            DBGT_EPILOG("");
            return err;
        }

        DBGT_PTRACE("Expsoure Event %d", exposureTime.ptr()->eExposureTime);

        if (OMX_SYMBIAN_PreCaptureExposureLong == exposureTime.ptr()->eExposureTime) {
            mPerfManager.logAndDump(PerfManager::EShakeDetected);

            /* Dispatch callback with out lock. This will allow
             * callback to call cameraHAL APIs. Otherwise there
             * would be a deadlock
             */
            mLock.unlock();
            mNotifyCb(CAMERA_MSG_SHAKE_DETECTED, true, 0, mCallbackCookie);
            mLock.lock();

            err = mOmxReqCbHandler.disable(OmxReqCbHandler::EPreCaptureExposureTime);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("enable reqcb failed");
                DBGT_EPILOG("");
                return err;
            }
        }
    }

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::autoFocus()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;
    OMX_ERRORTYPE err ;
    Mutex::Autolock lock(mLock);

    mPerfManager.init(PerfManager::EAutoFocusRequested);

#ifdef ENABLE_FACE_DETECTION
    //Disable face detection if enable
    if (isFaceDetectionEnable()) {
        mFaceDetection = false;
        //this will be again enable by
        //application via sendcommand
    }
#endif

    mMsgEnabled &= ~CAMERA_MSG_FOCUS_MOVE;
#ifdef ENABLE_CONTINUOUS_AUTOFOCUS
    err = mOmxReqCbHandler.disable(OmxReqCbHandler::EExtFocusStatus);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Enable ExtFocusStatus reqcbfailed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    if (mAutoFocusThreadData->mContinuousFocusState
            == EFocusReached) {
        dispatchNotification(CAMERA_MSG_FOCUS, 1, 0);
        return status;
    }
#endif

    mMsgEnabled |= CAMERA_MSG_FOCUS;
#ifdef ENABLE_AUTOFOCUS
    err = focusHandler()->doFocus();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("focusHandler()->doFocus failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }
#else //!ENABLE_AUTOFOCUS
    //send dummy request
    mAutoFocusThread->get()->request(*mAutoFocusThreadData);
#endif //ENABLE_AUTOFOCUS

    DBGT_EPILOG("");
    return status;
}

status_t STECamera::cancelAutoFocus()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    Mutex::Autolock lock(mLock);

    mMsgEnabled &= ~CAMERA_MSG_FOCUS;

    if (mAutoFocusThreadData->mEnabled) {
        mAutoFocusThreadData->mEnabled = OMX_FALSE;
        DBGT_PTRACE("AF Stopped");

    err = focusHandler()->moveLensToDefaultPos();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("moveLensToDefaultPos() failed, Lens didn't go to default position");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

        dispatchNotification(CAMERA_MSG_FOCUS, 0, 0);
    } else {
        DBGT_PTRACE("AF already completed or stopped");
    }

    //disable callback
    err = mOmxReqCbHandler.disable(OmxReqCbHandler::EAutoFocusLock);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Disable reqcbfailed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    err = mOmxReqCbHandler.disable(OmxReqCbHandler::EPreCaptureExposureTime);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Disable reqcbfailed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    err = focusHandler()->cancelFocus();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("cancelFocus() failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    setFocusDistance();

    DBGT_EPILOG("");
    return status;
}

/*static*/ int STECamera::beginPictureThread(void *cookie)
{
    STECamera *c = (STECamera *)cookie;
    return c->pictureThread();
}

int STECamera::pictureThread()
{
    DBGT_PROLOG("mIsStillZSL: %d",mIsStillZSL);

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    // Still During Record
    // Wait here to get the Record Buffer Index
    // which will be sent to JpegEnc
    if (mSnapshotDuringRecord) {
        DBGT_PTRACE("Wait for semaphore during record");
        sem_wait(&still_during_record);
        DBGT_PTRACE("semaphore post during record");
    }

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (!mSnapshotDuringRecord) {
        if (!mIsStillZSL) {
            camera_sem_wait(&shutter_eos, SEM_WAIT_TIMEOUT);
        } else {
            DBGT_PTRACE("no wait for shutter callback in zsl");
        }
    }
    //shutter available
    mPerfManager.logAndDump(PerfManager::EShutterCallback);
    dispatchNotification(CAMERA_MSG_SHUTTER, (unsigned int)&mRawImage);

    if (!mSnapshotDuringRecord) {
        camera_sem_wait(&isp_image_sem, SEM_WAIT_TIMEOUT);
        DBGT_PTRACE("Got Image Buffer");
    }

    OMX_BUFFERHEADERTYPE* pBufYuvImage = NULL;
    OMX_BUFFERHEADERTYPE* pBufJpeg = NULL;
    OmxBuffInfo* pBufInfoYuvImage = NULL;
    if (mIsStillZSL) {
        pBufYuvImage = pVPB1ImageBuffer;
        int bufIdx = -1;
        for (unsigned int i = 0 ; i < paramPortVPB1.nBufferCountActual; i++) {
            if (pStillBuffer[i] == pBufYuvImage) {
                bufIdx = i;
                break;
            }
        }
        DBGT_PTRACE("bufIdx: %d",bufIdx);
        pBufJpeg = pJPEGinBuffer[bufIdx];
        pBufInfoYuvImage = &mStillOmxBuffInfo[bufIdx];
    } else {
        if (mSnapshotDuringRecord) {
            pBufYuvImage = mRecordOmxBuffInfo[mSnapshotDuringRecIdx].mCamMemoryBase->omxBufferHeaderType();
            pBufJpeg = pJPEGinBuffer[mSnapshotDuringRecIdx];
        } else {
            pBufYuvImage = pISP2outBuffer;
            pBufJpeg = pJPEGinBuffer[0];
        }
    }

    //Raw callback processing
    if (mIsStillZSL || mSnapshotDuringRecord) {
        pISP1outBuffer->nFilledLen = paramISPoutput1.nBufferSize;
        pISP1outBuffer->nFlags = pBufYuvImage->nFlags | OMX_BUFFERFLAG_EOS;
    }
    dispatchRequest(&STECamera::doRawProcessing, NULL, false);

    if (!mSnapshotDuringRecord) {
        err = getRotationHandler()->handleBuffer(pBufYuvImage);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("getRotationHandler->handleBuffer() failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    if (mIsStillZSL && (mCurrentZoomLevel != 0)) {
        B2R2Utills::TRect cropRect;
        getCropVectorForZoom(cropRect.mX, cropRect.mY, cropRect.mWidth, cropRect.mHeight, false);

        B2R2Utills::TImageData srcImage, intImage;
        srcImage.mWidth = paramPortVPB1.format.video.nFrameWidth;
        srcImage.mHeight = paramPortVPB1.format.video.nFrameHeight;
        srcImage.mBufSize = paramPortVPB1.nBufferSize;
        srcImage.mLogAddr = (void*)pBufInfoYuvImage->mMMHwBufferInfo.iLogAddr;
        srcImage.mPhyAddr = (void*)pBufInfoYuvImage->mMMHwBufferInfo.iPhyAddr;
        srcImage.mColorFmt = paramPortVPB1.format.video.eColorFormat;


        intImage.mWidth = cropRect.mWidth;
        intImage.mHeight = cropRect.mHeight;
        intImage.mBufSize = paramPortVPB1.nBufferSize;
        intImage.mLogAddr = (void*)mTempStillOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
        intImage.mPhyAddr = (void*)mTempStillOmxBuffInfo.mMMHwBufferInfo.iPhyAddr;
        intImage.mColorFmt = paramPortVPB1.format.video.eColorFormat;

        err = (OMX_ERRORTYPE)B2R2Utills::crop_resize(&srcImage, &intImage, &srcImage, &cropRect, 2, true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("B2R2Utills::crop_resize() failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    pBufJpeg->nFilledLen = pBufYuvImage->nFilledLen;
    pBufJpeg->nFlags = pBufYuvImage->nFlags | OMX_BUFFERFLAG_EOS;
    err = OMX_EmptyThisBuffer(mJpegEnc, pBufJpeg);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_EmptyThisBuffer failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    camera_sem_wait(&jpeg_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("JPEGEnc empty buffer done");

#ifdef ENABLE_OVERLAYS
    getThumbnailHandler()->handleEvent();
#endif //ENABLE_OVERLAYS

    camera_sem_wait(&jpeg_eos_sem, SEM_WAIT_TIMEOUT);

    if (mIsStillZSL) {
        OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pBufYuvImage->pAppPrivate);
        buffInfo->mFlags &= ~OmxBuffInfo::ESentToHandlerThread;

        /* Camera client already done with buffer we sent to it */
        /* Now owner is CAMERA_HAL */

        buffInfo->mFlags |= OmxBuffInfo::ECameraHal;    /* Now own by camera HAL */
        err = OmxBuffInfo::fillBuffer(mCam, pBufYuvImage);
        DBGT_PTRACE("OMX_FillThisBuffer on mCam err: %#x",err);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FillThisBuffer failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    camera_sem_wait(&exif_sem, SEM_WAIT_TIMEOUT);

    if (!mSnapshotDuringRecord) {
        err = focusHandler()->cancelFocus();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("cancelFocus() failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        mLock.lock();
        setFocusDistance();
        mLock.unlock();
    }

    if (mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE) {
        camera_memory_t *camHandle;

        camHandle = mRequestMemory(-1,pEXIFoutBuffer->nFilledLen,1,mCallbackCookie);
        memcpy(camHandle->data,pEXIFoutBuffer->pBuffer,pEXIFoutBuffer->nFilledLen);
        mLock.lock();
        if (mIsPictureCancelled) {
            mLock.unlock();
            mDataCb(CAMERA_MSG_COMPRESSED_IMAGE, NULL, 0, NULL, mCallbackCookie);
        } else {
            mLock.unlock();
            //perf trace
            mPerfManager.logAndDump(PerfManager::EBeforeExifCallback);

            DBGT_PTRACE("EXIF file written from PictureThread len = %lu\n", pEXIFoutBuffer->nFilledLen);
            mDataCb(CAMERA_MSG_COMPRESSED_IMAGE, camHandle, 0,NULL, mCallbackCookie);

            //perf trace
            mPerfManager.logAndDump(PerfManager::EAfterExifCallback);
        }
        camHandle->release(camHandle);
    }

    camera_sem_wait(&exif_eos_sem, SEM_WAIT_TIMEOUT);
    //camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    mSnapshotDuringRecord = false;
    mSnapshotDuringRecIdx = -1;

    DBGT_EPILOG("");
    return NO_ERROR;
}

status_t STECamera::takePicture()
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    status_t status = NO_ERROR;

    // Check if HiResVF Feature is enabled OR
    // Check if Preview not enabled, dispatch error
    if (mHiResVFEnabled || !previewEnabled()) {
        if (mHiResVFEnabled) {
            DBGT_CRITICAL("HiResVF Feature Enabled, TakePicture is a Invalid Operation");
        } else {
            DBGT_CRITICAL("Preview not enabled");
        }
        status = INVALID_OPERATION;

        DBGT_EPILOG("");
        return status;
    }

    // if Record is running, it is snapshot during video record
    if (recordingEnabled()) {
        DBGT_PTRACE("take picture in recording");
        mSnapshotDuringRecord = true;
    }

    Mutex::Autolock _l(mLock);

    if (EStill != mCamMode && (!mSnapshotDuringRecord)) {
        DBGT_ASSERT(EVideo == mCamMode, "Invalid Camera Mode");

        err = configureCamMode(EStill, mIsStillZSL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("configureCamMode failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

    }

    mIsPictureCancelled = false;

    err = setGPSTags(mParameters, mExifMixer, 2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("setGPSTags Failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    //perf trace
    mPerfManager.init(PerfManager::ETakePictureRequested);

    if (!mSnapshotDuringRecord) {
        DBGT_PTRACE("mIsStillZSL: %d",mIsStillZSL);
        if (!mIsStillZSL) {
            //enable callback
            err = mOmxReqCbHandler.enable(OmxReqCbHandler::EExposureInitiated);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("Enable reqcb failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

//          err = updateAutoPause(OMX_TRUE);
//          if (OMX_ErrorNone != err) {
//              DBGT_CRITICAL("updateAutoPause failed");
//              DBGT_EPILOG("");
//              return UNKNOWN_ERROR;
//          }

            err = OmxBuffInfo::fillBuffer(mCam, pStillBuffer[0]);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

            err = updateCapturing(CAM_VPB + 1, OMX_TRUE);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("updateCapturing failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

#ifdef HWISPPROC
            // Push the buffer back to the camera
            err = OmxBuffInfo::fillBuffer(mISPProc, pISP1outBuffer);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

            err = OmxBuffInfo::fillBuffer(mISPProc, pISP2outBuffer);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_FillThisBuffer failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

#endif //HWISPPROC
        } else {
            mIsTakeStillZSL = true;
            err = updateCapturing(CAM_VPB + 1, OMX_TRUE);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("updateCapturing failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
        }
    }

    err = OMX_FillThisBuffer(mExifMixer, pEXIFoutBuffer);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_FillThisBuffer failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    if (createThread(beginPictureThread, this) == false) {
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    DBGT_EPILOG("");
    return status;
}

status_t STECamera::cancelPicture()
{
    DBGT_PROLOG("");

    Mutex::Autolock _l(mLock);
    mIsPictureCancelled = true;

    DBGT_EPILOG("");
    return NO_ERROR;
}

int STECamera::dump(int fd) const
{
    DBGT_PROLOG("");
#if 0
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    AutoMutex lock(&mLock);
    write(fd, result.string(), result.size());
#endif //0

    DBGT_EPILOG("");
    return NO_ERROR;
}

OMX_ERRORTYPE STECamera::ReconfigureStill(const ImageInfo &aPictureInfo, bool aReconfigStillHeap)
{
    DBGT_PROLOG("Picture Width %d  Height %d ReconfigHeap %d",
            aPictureInfo.getWidth(),
            aPictureInfo.getHeight(),
            aReconfigStillHeap);

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_ERRORTYPE stateChangeErr = OMX_ErrorNone;

    //Disable the affected ports
#ifdef JPEGENC
    DBGT_PROLOG("In ReconfigureStill, sending Command OMX_CommandPortDisable for mJpegEnc Port 0");
    err = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortDisable,0, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable 0 mJpegEnc failed ");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        err = OMX_FreeBuffer(mJpegEnc, (OMX_U32)(CAM_VPB + 0), pJPEGinBuffer[0]);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Free buffer for mJpegEnc port0 failed  ");
            DBGT_EPILOG("");
            return err;
        }
        camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    }
#endif // JPEGENC

#ifdef HWISPPROC
    DBGT_PTRACE("In ReconfigureStill, sending Command OMX_CommandPortDisable for mISPProc Port 2");
    err = OmxUtils::setPortState(mISPProc, OMX_CommandPortDisable,2, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable 2 mISPProc failed ");
        DBGT_EPILOG("");
        return err;
    }

    if (OMX_ErrorSameState != err) {
        err =OMX_FreeBuffer(mISPProc, (OMX_U32)(CAM_VPB + 2), pISP2outBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Free buffer for mISPProc port2 failed  ");
            DBGT_EPILOG("");
            return err;
        }
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }
#elif SWISPPROC
    DBGT_PTRACE("In ReconfigureStill, sending Command OMX_CommandPortDisable for mISPProc Port 1");
    err  = OmxUtils::setPortState(mISPProc, OMX_CommandPortDisable,1, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable 1 mISPProc failed ");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        err = OMX_FreeBuffer(mISPProc, (OMX_U32)(CAM_VPB + 1), pISP2outBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Free buffer for mISPProc port1 failed  ");
            DBGT_EPILOG("");
            return err;
        }
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }
#endif // HWISPPROC

    // Reconfigure affected ports
    // First Reconfigure IspProc output
#ifdef SWISPPROC
        err = InitializePort(paramISPoutput, OMX_PortDomainVideo, mISPProc, 1);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort for mISPProc port 1 failed");
            DBGT_EPILOG("");
            return err;
        }
#elif HWISPPROC
        err = InitializePort(paramISPoutput, OMX_PortDomainVideo, mISPProc, 2);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("InitializePort failed");
            DBGT_EPILOG("");
            return err;
        }
#endif

#ifdef JPEGENC
    // Next Reconfigure HW Jpeg input
    err = InitializePort(paramJPEGinput, OMX_PortDomainImage, mJpegEnc, 0);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 0 failed");
        DBGT_EPILOG("");
        return err;
    }
#endif// JPEGENC

   //Reconfigure quality related ports/
    err = ReconfigureJpegQuality(aPictureInfo);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("ReconfigureJpegQuality failed ");
        DBGT_EPILOG("");
        return err;
    }

    if (aReconfigStillHeap) {
        STECamMMHwBuffer::freeHwBuffer(&mISPImageOmxBuffInfo);
        STECamMMHwBuffer::freeHwBuffer(mStillOmxBuffInfo); /*Release owner buffer instead of shared buffer*/
        STECamMMHwBuffer::freeHwBuffer(&mISPRawOmxBuffInfo);
        STECamMMHwBuffer::freeHwBuffer(mStillOmxBuffInfo);
        initStillHeap();
    }

#if defined(SWISPPROC) || defined(HWISPPROC)
    // Allocate Buffers through HwBuffer
    DBGT_PTRACE("In ReconfigureStill, sending Command Port Enable for mISPProc Port 1");

    stateChangeErr  = OmxUtils::setPortState(mISPProc, OMX_CommandPortEnable,paramISPoutput.nPortIndex, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Enable 1 mISPProc failed ");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    DBGT_PTRACE("\nCalling OMX_SetConfig before OMX_UseBuffer\n");
    err = OMX_SetConfig(mISPProc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mISPImageOmxBuffInfo.mMMHwChunkMetaData);

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed Error 0x%08x", err);
    }

    pISP2outBuffer = NULL;

    DBGT_PTRACE("Usebuffer for mISPProc");

    err = OMX_UseBuffer(mISPProc,    // hComponent
                        &pISP2outBuffer,    // address where ptr to buffer header will be stored
                        paramISPoutput.nPortIndex,          // port index (for port for which buffer is provided)
                        (void *)(&mISPImageOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
                        // to have a context when component returns with a callback (i.e. to know
                        // what to free etc.
                        (OMX_U32)(paramISPoutput.nBufferSize),        // buffer size
                        (OMX_U8 *)(mISPImageOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
                       );

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mISPProc port 2");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(NULL != pISP2outBuffer->pBuffer, "Buffer is NULL");

    mISPImageOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(pISP2outBuffer);
    if (OMX_ErrorSameState != stateChangeErr) {
        camera_sem_wait(&stateISP_sem, SEM_WAIT_TIMEOUT);
    }
#endif // SWISPPROC || HWISPPROC

#ifdef JPEGENC
    DBGT_PTRACE("In ReconfigureStill, sending Command Enable for mJpegEnc Port 0");
    stateChangeErr  = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortEnable,0, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Enable 0 mJpegEnc failed ");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    // Allocate Buffers through HwBuffer
    DBGT_PTRACE("\nCalling OMX_SetConfig before OMX_UseBuffer JPEGENC\n");
    err = OMX_SetConfig(mJpegEnc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mJpegOmxBuffInfo[0].mMMHwChunkMetaData);

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed Error 0x%08x", err);
    }

    pJPEGinBuffer[0] = NULL;

    DBGT_PTRACE("Usebuffer for mJpegEnc");

    err = OMX_UseBuffer(mJpegEnc,    // hComponent
                        &pJPEGinBuffer[0],    // address where ptr to buffer header will be stored
                        CAM_VPB + 0,        // port index (for port for which buffer is provided)
                        (void *)(&mJpegOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
                        // to have a context when component returns with a callback (i.e. to know
                        // what to free etc.
                        (OMX_U32)(paramISPoutput.nBufferSize),        // buffer size
                        (OMX_U8 *)(mJpegOmxBuffInfo[0].mMMHwBufferInfo.iLogAddr) // buffer data ptr
                       );

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mJpegEnc port 0 ");
        DBGT_EPILOG("");
        return err;
    }
    DBGT_ASSERT(NULL != pJPEGinBuffer[0]->pBuffer, "Buffer is NULL");

    mJpegOmxBuffInfo[0].mCamMemoryBase.get()->setOmxBufferHeaderType(pJPEGinBuffer[0]);
    if (OMX_ErrorSameState != stateChangeErr ) {
        camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    }
#endif

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::ReconfigureJpegQuality(const ImageInfo &aPictureInfo)
{
    DBGT_PROLOG("Picture Quality %d", aPictureInfo.getQuality());

    DBGT_PTRACE("In ReconfigureJpegQuality, sending Command OMX_CommandPortDisable for mExifMixer Port 2");
    OMX_ERRORTYPE stateChangeErr = OMX_ErrorNone;
    OMX_ERRORTYPE err  = OmxUtils::setPortState(mExifMixer, OMX_CommandPortDisable,2, NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable 2 mExifMixer failed ");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        OMX_U8* exifBuf = pEXIFoutBuffer->pBuffer;
        err = OMX_FreeBuffer(mExifMixer, 2, pEXIFoutBuffer);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Free buffer for mExifMixer failed  ");
            DBGT_EPILOG("");
            return err;
        }

        DBGT_PTRACE("deleting exif out software buffer: %p",exifBuf);
        delete[] exifBuf;
        camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    }

    DBGT_PTRACE("In ReconfigureJpegQuality, sending Command OMX_CommandPortDisable for mJpegEnc Port 1");
    stateChangeErr  = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortDisable,1, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Disable 1 mJpegEnc failed ");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    DBGT_PTRACE("In ReconfigureJpegQuality, sending Command OMX_CommandPortDisable for mExifMixer Port 1");
    err  = OmxUtils::setPortState(mExifMixer, OMX_CommandPortDisable,1,NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Disable 1 mJpegEnc failed ");
        DBGT_EPILOG("");
        return err;
    }
    if (OMX_ErrorSameState != err) {
        camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    }
    if (OMX_ErrorSameState != stateChangeErr) {
        camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    }

#ifdef JPEGENC
    // Reconfigure HW JPEG output
    err = InitializePort(paramJPEGoutput, OMX_PortDomainImage, mJpegEnc, 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mJpegEnc Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    if (aPictureInfo.invalidQuality() != aPictureInfo.getQuality()) {
        OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
        qfactor.ptr()->nPortIndex = 1;
        qfactor.ptr()->nQFactor = aPictureInfo.getQuality();

        err = OMX_SetConfig(mJpegEnc, OMX_IndexParamQFactor, qfactor.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Setting JPEG Quality: Failed ");
            DBGT_EPILOG("");
            return err;
        }

    }
#endif //JPEGENC

    // Reconfigure EXIF input 2
    err = InitializePort(paramEXIFinput2, OMX_PortDomainImage, mExifMixer, 1);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 1 failed");
        DBGT_EPILOG("");
        return err;
    }

    // Reconfigure EXIF output
    err = InitializePort(paramEXIFoutput, OMX_PortDomainImage, mExifMixer, 2);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("InitializePort for mExifMixer Port 2 failed");
        DBGT_EPILOG("");
        return err;
    }

    //Enable ports
    DBGT_PTRACE("In ReconfigureStill, sending Command Enable for mExifMixer Port 1");
    err  = OmxUtils::setPortState(mExifMixer, OMX_CommandPortEnable,1,NULL,NULL);
    if ((OMX_ErrorNone != err) && (OMX_ErrorSameState != err)) {
        DBGT_CRITICAL("Port Enable 1 mExifMixer failed ");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("In ReconfigureStill, sending Command Enable for mJpegEnc Port 1");
    stateChangeErr  = OmxUtils::setPortState(mJpegEnc, OMX_CommandPortEnable,1, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Enable 1 mJpegEnc failed ");
        DBGT_EPILOG("");
        return stateChangeErr;
    }

    if (OMX_ErrorSameState != stateChangeErr) {
        camera_sem_wait(&stateJpeg_sem, SEM_WAIT_TIMEOUT);
    }

    if (OMX_ErrorSameState != err) {
        camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    }

    DBGT_PTRACE("Enable port 2 for mExifMixer");
    stateChangeErr  = OmxUtils::setPortState(mExifMixer, OMX_CommandPortEnable,2, NULL,NULL);
    if ((OMX_ErrorNone != stateChangeErr) && (OMX_ErrorSameState != stateChangeErr)) {
        DBGT_CRITICAL("Port Enable 2 mExifMixer failed ");
        DBGT_EPILOG("");
        return stateChangeErr;
    }
    /*Allocating EXIF output buffer*/
    pEXIFoutBuffer = NULL;
    if (OMX_ErrorSameState != stateChangeErr) {
        OMX_U8 *exifBuf = new OMX_U8[paramEXIFoutput.nBufferSize];
        DBGT_PTRACE("exif out Buffer: %p",exifBuf);
        if (NULL == exifBuf) {
            DBGT_CRITICAL("OMX_ErrorInsufficientResources - new failed for exifBuf");
            DBGT_EPILOG("");
            return OMX_ErrorInsufficientResources;
        }
        err = OMX_UseBuffer(mExifMixer, &pEXIFoutBuffer, 2, NULL, (OMX_U32)(paramEXIFoutput.nBufferSize), exifBuf);
        if ((OMX_ErrorNone != err) || (pEXIFoutBuffer->pBuffer == 0)) {
            DBGT_CRITICAL("OMX_AllocateBuffer failed for mExifMixer");
            DBGT_EPILOG("");
            return err;
        }
        if ((err != OMX_ErrorNone) && (exifBuf != NULL)) {
            delete[] exifBuf;
        }
        camera_sem_wait(&stateEXIF_sem, SEM_WAIT_TIMEOUT);
    }

    DBGT_EPILOG("");
    return err;
    //mLock.unlock();
}


status_t STECamera::setParameters(const char* parameters)
{
    CameraParameters params;

    String8 str_params(parameters);
    params.unflatten(str_params);


    return setParameters(params);
}

status_t STECamera::setParameters(const CameraParameters &params)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    status_t status = NO_ERROR;
    bool previewRunning = previewEnabled();
    bool recordRunning = recordingEnabled();

    // wait for takepicture completion
    Mutex::Autolock lock(mTakePictureLock);

    Mutex::Autolock _l(mLock);

    //check preview size is valid
    bool previewSizeValid = checkPreviewSizeValid(params.get(CameraParameters::KEY_PREVIEW_SIZE));
    if (0 == previewSizeValid) {
        DBGT_CRITICAL("Invalid preview size");
        DBGT_EPILOG("");
        return BAD_VALUE;
    }

    int min_framerate = 0, max_framerate = 0;
    int preview_width = 0, preview_height = 0;
    int record_width = 0, record_height = 0;
    int picture_width = 0, picture_height = 0;

    /*Get all the parameters which may need to reconfigure*/
    params.getPreviewSize(&preview_width, &preview_height);
    if ((0 == preview_width) || (0 == preview_height)) {
        DBGT_CRITICAL("bad preview size params (0,0)");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    params.getVideoSize(&record_width, &record_height);

    if ((0 == record_width) || (0 == record_height)) {
        DBGT_CRITICAL("bad record size params (0,0)");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    params.getPictureSize(&picture_width, &picture_height);
    if ((0 == picture_width) || (0 == picture_height)) {
        DBGT_CRITICAL("bad picture size params (0,0)");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    params.getPreviewFpsRange(&min_framerate, &max_framerate);

    DBGT_PINFO("Recording Hint: %s", params.get(CameraParameters::KEY_RECORDING_HINT));
    DBGT_PINFO("Thumbnail Width: %s Height: %s Quality: %s",
                params.get(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH),
                params.get(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT),
                params.get(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY));
    DBGT_PINFO("Rotation: %s WhiteBalance: %s Effect: %s Antibanding: %s",
                params.get(CameraParameters::KEY_ROTATION),
                params.get(CameraParameters::KEY_WHITE_BALANCE),
                params.get(CameraParameters::KEY_EFFECT),
                params.get(CameraParameters::KEY_ANTIBANDING));
    DBGT_PINFO("SceneMode: %s Flash: %s Focus: %s",
                params.get(CameraParameters::KEY_SCENE_MODE),
                params.get(CameraParameters::KEY_FLASH_MODE),
                params.get(CameraParameters::KEY_FOCUS_MODE));
    DBGT_PINFO("Preview Width: %d Height: %d FrameRate: %d Framerate(New API): (%d,%d) Format : %s",
                preview_width,
                preview_height,
                params.getPreviewFrameRate(),
                min_framerate,
                max_framerate,
                params.getPreviewFormat());

    if (NULL != params.get(CameraParameters::KEY_JPEG_QUALITY)) {
        DBGT_PINFO("Picture Width: %d Height: %d Format : %s Quality: %d",
                    picture_width,
                    picture_height,
                    params.getPictureFormat(),
                    params.getInt(CameraParameters::KEY_JPEG_QUALITY));

        DBGT_PINFO("Contrast: %s Brightness: %s Saturation: %s",
                params.get(KEY_CONTRAST),
                params.get(KEY_BRIGHTNESS),
                params.get(KEY_SATURATION));
        DBGT_PINFO("Sharpness: %s Region Control: %s",
                params.get(KEY_SHARPNESS),
                params.get(KEY_REGION_CONTROL));
    }

#ifdef ZOOM_ENABLE
    if ((NULL != params.get(CameraParameters::KEY_ZOOM)) &&
            (NULL != params.get(CameraParameters::KEY_MAX_ZOOM)) &&
            (NULL != params.get(CameraParameters::KEY_ZOOM_RATIOS))) {
        DBGT_PINFO("Zoom Parameters: KEY_ZOOM = %s KEY_MAX_ZOOM = %s KEY_ZOOM_RATIOS: %s",
                    params.get(CameraParameters::KEY_ZOOM),
                    params.get(CameraParameters::KEY_MAX_ZOOM),
                    params.get(CameraParameters::KEY_ZOOM_RATIOS));
    }
#endif //ZOOM_ENABLE

    // Check if White Balance lock is required
    if (!strcmp(params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED),"true")) {
        if ( strcmp(mParameters.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK),
                    params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK) ) != 0 ) {
            mUpdateAutoWhiteBalanceLock = true;
        }
    }
    // Check if Exposure lock is required
    if (!strcmp(params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED),"true")) {
        if ( strcmp(mParameters.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK),
                    params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK) ) != 0 ) {
            mUpdateAutoExposureLock = true;
        }
    }
    /*Get the Camera Mode EStill : 0/ EVideo : 1*/
    int camMode = 0;
    bool isZSLModeChanged = false;
    bool stillZSL = false;

    const char *cmode = params.get(CameraParameters::KEY_RECORDING_HINT);
    if (cmode != NULL) {
        if (strcmp(cmode, "true") == 0)
            camMode = EVideo;
        else if(strcmp(cmode, "false") == 0)
            camMode = EStill;
    } else
        camMode = mCamMode;

    // In still mode check for ZSL.
    if (camMode == EStill) {
        mStillZslBufferCount = 2;
        const char *zslstr = params.get(KEY_ZERO_SHUTTER_LAG_MODE);
        DBGT_PTRACE("value for zsl key: %s",zslstr);
        if (!strcmp(params.get(KEY_ZERO_SHUTTER_LAG_MODE), zslSingle)) {
            stillZSL = true;
            mStillZslBufferCount = 2;
        } else if (!strcmp(params.get(KEY_ZERO_SHUTTER_LAG_MODE), zslBurst)) {
            stillZSL = true;
            mStillZslBufferCount = 6;
        }
    }

    DBGT_PTRACE("stillZSL: %d",stillZSL);
    if ((camMode == EStill) && (mCamMode == EStill) && (stillZSL != mIsStillZSL)) {
        isZSLModeChanged = true;
    }

    DBGT_PTRACE("camMode: %d, stillZSL: %d, mIsStillZSL: %d", camMode, stillZSL, mIsStillZSL);

    if (camMode != mCamMode && previewRunning) {
        camMode = mCamMode;
    }
    if (camMode != EVideo && camMode != EStill) {
        DBGT_CRITICAL("camMode: %d is not supported", camMode);
        return -1;
    }
    DBGT_PTRACE("camera mode = %d", camMode);

    int picture_quality = params.getInt(CameraParameters::KEY_JPEG_QUALITY);

    int thumbnail_width = params.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
    int thumbnail_height = params.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);

    int thumbnail_quality = params.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY);

    mParameters = params;

    int framerate = params.getPreviewFrameRate();
    params.getPreviewFpsRange(&min_framerate, &max_framerate);

    if ((min_framerate >= 0) || (max_framerate >= 0)) {
        if ((min_framerate > max_framerate)
            || (min_framerate < SUPPORTED_MIN_FPS_RANGE)
            || (max_framerate > SUPPORTED_MAX_FPS_RANGE)) {
            DBGT_CRITICAL("Invalid FpsRange %u %u",min_framerate,max_framerate);
            return BAD_VALUE;
        }
    } else {
        DBGT_CRITICAL("Unsupported Framerate value");
        return BAD_VALUE;
    }

    int hiResVFView = params.getInt(KEY_HIRES_VIEWFINDER);
    bool videoStabEnabled = false;
    bool isVideoStabStatusChanged = false;

    gRotation = params.getInt(CameraParameters::KEY_ROTATION);
    /*check if rotation is set*/
    if (-1 == gRotation) {
        gRotation = 0;
    }
#if ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_DEVICE
    if (gRotation == 180)
        mImageRotHandler = mDeviceSwRotation;
    else
        mImageRotHandler = mDeviceHwRotation;
#endif //ENABLE_IMAGE_ROTATION==CAM_IMAGE_ROTATION_DEVICE

    status = applyCameraProperties(params);
    if (NO_ERROR != status) {
        DBGT_CRITICAL("applyCameraProperties failed");
        DBGT_EPILOG("");
        return status;
    }
    bool isHiResVF = false;
    bool ispreviewsizechanged = false;
#ifdef ENABLE_HIGH_RESOLUTION_VF
    isHiResVF = updateHiResVF(preview_width, preview_height, hiResVFView);
#endif //ENABLE_HIGH_RESOLUTION_VF
    if ((isHiResVF == false) || (mHiResVFEnabled == false)) {
        ispreviewsizechanged = updatePreviewSize(preview_width, preview_height);
        mIspreviewsizechanged = ispreviewsizechanged;
    }
    mIsrecordsizechanged = updateRecordSize(record_width, record_height);
    bool isframeratechanged = updateFrameRate(framerate, min_framerate, max_framerate);
    /*  On the fly Preview/Record Resolution change is not supported.
        So if the Application Requests for resolution change while Preview is Running
        wait for StopPreview() call */
    if (previewRunning) {
        if ((mIsrecordsizechanged && recordRunning) || ispreviewsizechanged) {
            DBGT_PTRACE(" Resolution change to %dx%d from %dx%d not possible when Preview is running",
                 preview_width, preview_height, mConfigPreviewWidth, mConfigPreviewHeight);
            status = applyCameraProperties(params);
            if (NO_ERROR != status) {
                DBGT_CRITICAL("applyCameraProperties failed");
                DBGT_EPILOG("");
                return status;
            }
            DBGT_EPILOG("");
            return NO_ERROR;
        }
    }

    //setup previewomx color format
    initPreviewOmxColorFormat(mParameters.getPreviewFormat());
    setupPreviewSwConProcessing(mParameters.getPreviewFormat());

    /*check if some configuration need to be done, if any parameter got changed*/
    bool ispicturesizechanged = updatePictureSize(picture_width, picture_height);
    bool isthumbnailsizechanged = updateThumbnailSize(thumbnail_width, thumbnail_height);
    bool ispicturequalitychanged = updatePictureQuality(picture_quality);
    bool isthumbnailqualitychanged = updateThumbnailQuality(thumbnail_quality);

    mConfigFrameRate = framerate;

    /*set up preview, record, picture and thumbnail parameters in our local struct.*/
#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    int previewHeightAlignment = cameraFormatInfo().mHeightAlignment;
    if ( (90 == gRotation || 270 == gRotation) && (EVideo == camMode))
        previewHeightAlignment = 8;
#else //!ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    int previewHeightAlignment = cameraFormatInfo().mHeightAlignment;
#endif //ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_LANDSCAPE
    mPreviewInfo.setupImageInfo((uint32_t)preview_width,
                                (uint32_t)preview_height,
                                0,
                                0,
                                cameraFormatInfo().mWidthAlignment,
                                previewHeightAlignment);

    mRecordInfo.setupImageInfo((uint32_t)record_width,
            (uint32_t)record_height,
            0,
            mRecordInfo.getRotation());
    mPictureInfo.setupImageInfo((uint32_t)picture_width,
            (uint32_t)picture_height,
            picture_quality,
            mPictureInfo.getRotation());
    mThumbnailInfo.setupImageInfo((uint32_t)thumbnail_width,
            (uint32_t)thumbnail_height,
            thumbnail_quality,
            mThumbnailInfo.getRotation());

    mEnableB2R2DuringRecord = true;
    if (((record_width == 1920) &&
         (mOmxRecordPixFmt == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)) ||
        (mSlowMotionFPS > 0)) {
        // B2R2 processing During Record should be disabled in following cases,
        // When doing 1080p recording and Record Fmt is YUV420 MB Tiled
        // When using 3x Slow Motion feature
        mEnableB2R2DuringRecord = false;
    }

    status = applyCameraProperties(params);
    if (NO_ERROR != status) {
        DBGT_CRITICAL("applyCameraProperties failed");
        DBGT_EPILOG("");
        return status;
    }
    /* update copy of params i.e. mParameters at out side*/
    mParameters.setPreviewSize(mPreviewInfo.getAlignedWidth(), mPreviewInfo.getAlignedHeight());
    mParameters.setVideoSize(mRecordInfo.getWidth(), mRecordInfo.getHeight());
    DBGT_PTRACE("record width =%d height = %d", mRecordInfo.getWidth(),mRecordInfo.getHeight());
#ifdef ENABLE_HIGH_RESOLUTION_VF
    /* Do configuration for Hi Resolution VF -required in case of panorama*/
    if (isHiResVF && mHiResVFEnabled) {
        // If Preview is already configured, need to setup a new datapath
        // after destroying the previous one.
        configureAndStartHiResVF();
    }
#endif //ENABLE_HIGH_RESOLUTION_VF

#ifdef ENABLE_VIDEO_STAB
    // Check if Video Stabilization is enabled
    if (params.get(CameraParameters::KEY_VIDEO_STABILIZATION) != NULL) {
        if (strcmp(params.get(CameraParameters::KEY_VIDEO_STABILIZATION), "true") == 0)
            videoStabEnabled = true;
    }

    DBGT_PTRACE("VideoStabEnabled %d", videoStabEnabled);
    if (videoStabEnabled != mVideoStabEnabled) {
        isVideoStabStatusChanged = true;
        mVideoStabEnabled = videoStabEnabled;
    }

    if (previewRunning && isVideoStabStatusChanged && (EVideo == camMode)) {
        DBGT_PTRACE("Video Stab Status Changed, Video Stab %d", mVideoStabEnabled);
        status = UpdateVideoStabStatus();
        if (NO_ERROR != status) {
            DBGT_CRITICAL("UpdateVideoStabFeature failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }
#endif //ENABLE_VIDEO_STAB

    if ((ispicturesizechanged || ispicturequalitychanged) && (mIsStillZSL) && (stillZSL)) {
        DBGT_PTRACE("still size or quality changed");
        isZSLModeChanged = true;
    }

    bool isConfigRequired = false;
    if ((camMode != mCamMode) || (mIsCameraConfigured == false) ||
        (mIsStillZSL != stillZSL) || isZSLModeChanged) {
        isConfigRequired = true;
    }
    mPreviousCamMode = mCamMode;
    DBGT_PTRACE("current camera mode     = %d", camMode);
    DBGT_PTRACE("previous camera mode    = %d", mCamMode);
    DBGT_PTRACE("Is Cam Config required  = %d", isConfigRequired);
    DBGT_PTRACE("Is PreviewSize changed  = %d", ispreviewsizechanged);
    DBGT_PTRACE("g_IsPreviewConfig       = %d", g_IsPreviewConfig);
    DBGT_PTRACE("isthumbnailsizechanged  = %d", isthumbnailsizechanged);
    DBGT_PTRACE("ispicturesizechanged    = %d", ispicturesizechanged);
    DBGT_PTRACE("mIsrecordsizechanged    = %d", mIsrecordsizechanged);

    getRotationHandler()->updateImageInfo(gRotation);

    /*do camera configuration in case if camera is not configured or*/
    /*change in camera mode (still-video) for Auto Frame Rate Control*/
    if (isConfigRequired) {

        err = configureCamMode(camMode, stillZSL);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("configureCamMode failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

    } else {
        /* Reconfigure preview, record  */
        /* Preview should be stopped before reconfuring preview, record or static framerate in record mode */
        if ((mIsrecordsizechanged && ispreviewsizechanged) ||
                ispreviewsizechanged || isframeratechanged) {

            DBGT_PINFO("RECONFIGURE VF & RECORD");

            bool reconfigureHeap = false;

            /* Free and re-allocate preview buffers in case
            preview size is changed */
            if (ispreviewsizechanged)
                reconfigureHeap = true;

            switch (mCamMode) {
                case EStill:
                    err = ReconfigureVFStill(reconfigureHeap);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("ReconfigureVFStill failed");
                        DBGT_EPILOG("");
                        return UNKNOWN_ERROR;
                    }

                    break;
                case EVideo:
                    err = ReconfigureVFVideo(reconfigureHeap);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("ReconfigureVFVideo failed");
                        DBGT_EPILOG("");
                        return UNKNOWN_ERROR;
                    }

                    break;
                default:
                    DBGT_CRITICAL("Cant Happen");
                    break;
            }

            DBGT_PTRACE("New Preview Size %dx%d", preview_width, preview_height);
            DBGT_PTRACE("New Record Size %dx%d", record_width, record_height);

        } else if (mIsrecordsizechanged
#if ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
            || (gRotation != mRecordInfo.getRotation() && !mRecordRunning)
#endif //ENABLE_VIDEO_ROTATION==CAM_VIDEO_ROTATION_PORTRAIT
            ) {
            DBGT_PINFO("RECONFIGURE RECORD");

            bool reconfigureHeap = true;
            bool reconfigureVPB0 = false;

            switch (mCamMode) {
                case EVideo:
                    err = ReconfigureVFVideo(reconfigureHeap, reconfigureVPB0);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("ReconfigureVFVideo failed");
                        DBGT_EPILOG("");
                        return UNKNOWN_ERROR;
                    }
                    break;
                default:
                    DBGT_CRITICAL("Received invalid cam-mode %d", mCamMode);
            }

            DBGT_PTRACE("New Record Size %dx%d", record_width, record_height);
        }
        if (isthumbnailsizechanged || isthumbnailqualitychanged || mThumbnailInfo.swapParams()) {
            err = getThumbnailHandler()->release();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("getThumbnailHandler()->release() Failed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

            //check if thumbnail handler is to be updated
            selectThumbnailHandler(thumbnail_width, thumbnail_height);
        }

        /* RotationHandler will take care of thumbnail/still reconfigurations */
        err = getRotationHandler()->doConfiguration();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("getRotationHandler->doConfiguration Failed");
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        /* still/thumbnail configuration will be taken care of by ReconfigureStill()/
         * ReconfigureJpegQuality/ThumbnailHandler */
        if (!mThumbnailInfo.swapParams()) {
            if (isthumbnailsizechanged || isthumbnailqualitychanged) {
                err = getThumbnailHandler()->configure(mThumbnailInfo, mPreviewInfo);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("doConfiguration Failed");
                    DBGT_EPILOG("");
                    return UNKNOWN_ERROR;
                }

                DBGT_PTRACE("New Thumbnail Size %dx%d", thumbnail_width, thumbnail_height);
                DBGT_PTRACE("New Thumbnail Quality %d", thumbnail_quality);
            }

            if ((mCamMode == EStill) && (!mIsStillZSL)) {
                if (ispicturesizechanged) {
                    DBGT_PINFO("Still size/resolution changed. ReconfigureStill \n");

                    err = ReconfigureStill(mPictureInfo, true);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("ReconfigureStill failed");
                        DBGT_EPILOG("");
                        return UNKNOWN_ERROR;
                    }

                    DBGT_PTRACE("New Picture Size %dx%d", picture_width, picture_height);
                    DBGT_PTRACE("New Picture Quality %d", picture_quality);

                } else if (ispicturequalitychanged) {
                    DBGT_PINFO("Still Quality changed. ReconfigureJpegQuality \n");

                    err = ReconfigureJpegQuality(mPictureInfo);
                    if (OMX_ErrorNone != err) {
                        DBGT_CRITICAL("ReconfigureJpegQuality failed");
                        DBGT_EPILOG("");
                        return UNKNOWN_ERROR;
                    }

                    DBGT_PTRACE("New Picture Quality %d", picture_quality);
                }
            }
        }
    }
    const char *valstr
         = params.get(CameraParameters::KEY_FOCUS_MODE);
    if ( (valstr = params.get(CameraParameters::KEY_METERING_AREAS)) != NULL ) {
            mParameters.set(CameraParameters::KEY_METERING_AREAS, valstr);
    }
    if ( (valstr = params.get(CameraParameters::KEY_FOCUS_AREAS)) != NULL ) {
            mParameters.set(CameraParameters::KEY_FOCUS_AREAS, valstr);
    }

    status = applyCameraProperties(params);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("applyCameraProperties failed");
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    if (!checkPreviewFormat(params.getPreviewFormat())) {
        DBGT_CRITICAL("Format: %s is not supported", params.getPreviewFormat());
        return -1;
    }

    if (strcmp(params.getPictureFormat(), CameraParameters::PIXEL_FORMAT_JPEG) != 0) {
        DBGT_CRITICAL("Only jpeg still pictures are supported");
        return -1;
    }

    //Always setup all extra keys
    setupExtraKeys();

    DBGT_EPILOG("");
    return NO_ERROR;
}

int STECamera::isFrameRateSupported(
        const char *fps_list,
        int framerate)
{
    char fps[3];
    snprintf(fps, strlen(fps), "%d", framerate);
    if (strstr(fps_list, fps)) {
        return 1;
    } else {
        return 0;
    }
}

bool STECamera::updateFrameRate(
        int &aFramerate,
        int &aMinFramerate,
        int &aMaxFramerate)
{
    DBGT_PROLOG("Framerate: %d - FPSrange[%d;%d] - lastFrameRate:%d", aFramerate,aMinFramerate,aMaxFramerate,mConfigFrameRate);

    if( ((aMinFramerate == aMaxFramerate) && (aMinFramerate!=(aFramerate*1000))) ||
        ((aMinFramerate < aMaxFramerate) && (aFramerate!=0)) ) {
        if(aFramerate!=mConfigFrameRate) {
        /* updated by the OLD API */
            DBGT_PTRACE("updated by the OLD API");
            if(aFramerate==0) {
                /* Set variable framerate */
                aMinFramerate = 10000;
                aMaxFramerate = 30000;
            }
            else {
                /* set fixed frame rate */
                aMinFramerate = aFramerate*1000;
                aMaxFramerate = aFramerate*1000;
            }
        }
        else {
        /* updated by the NEW API */
            DBGT_PTRACE("updated by the NEW API");
            if(aMinFramerate < aMaxFramerate) {
                /* Set variable framerate */
                aFramerate = 0;
            }
            else {
                /* set fixed frame rate */
                aFramerate = aMinFramerate / 1000;
            }
        }
        mParameters.setPreviewFrameRate(aFramerate);
        setPreviewFpsRange(aMinFramerate, aMaxFramerate);
    }
    else if (aFramerate!=mConfigFrameRate) {
        mParameters.setPreviewFrameRate(aFramerate);
        setPreviewFpsRange(aMinFramerate, aMaxFramerate);
    }
    else {
        DBGT_PTRACE("No Change in FrameRate");
        DBGT_EPILOG("");
        return false;
    }

    if ((aFramerate > MAX_PREVIEW_FRAMERATE) ||
            (!isFrameRateSupported(mParameters.get(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES),
                                   aFramerate))) {
        mSlowMotionFPS = aFramerate;
        aFramerate = MAX_PREVIEW_FRAMERATE;
    }

    DBGT_PTRACE("Updated FrameRate is %d - FPSrange[%d;%d]", aFramerate,aMinFramerate,aMaxFramerate);
    DBGT_PTRACE("FrameRate changed");
    DBGT_EPILOG("");
    return true;
}

bool STECamera::updateHiResVF(int &aPreviewWidth, int &aPreviewHeight, int aHiResVFView)
{
    DBGT_PROLOG("Preview Width: %d Preview Height: %d HiResVFView: %d",
            aPreviewWidth,
            aPreviewHeight,
            aHiResVFView);

    if (aHiResVFView == 1) {
        // Check if HiResVF Feature is already enabled or not
        if (!mHiResVFEnabled) {
            DBGT_PTRACE("Switching to Hi Res View Finder");
            mHiResVFEnabled = true;
            aPreviewWidth = STE_MAX_HI_RES_PREVIEW_WIDTH;
            aPreviewHeight = STE_MAX_HI_RES_PREVIEW_HEIGHT;
        }
        // If already enabled nothing needs to be done
    }

    if ((aPreviewWidth != (int)mPreviewInfo.getWidth()) ||
            (aPreviewHeight != (int)mPreviewInfo.getHeight())) {
        DBGT_PTRACE("Preview Size Changed");
        DBGT_EPILOG("");
        return true;
    }

    DBGT_PTRACE("No Change in Preview Size");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updatePreviewSize(int &aPreviewWidth, int &aPreviewHeight)
{
    DBGT_PROLOG("Preview Width: %d Preview Height: %d", aPreviewWidth, aPreviewHeight);

    if ((aPreviewWidth > STE_MAX_PREVIEW_WIDTH) || (aPreviewHeight > STE_MAX_PREVIEW_HEIGHT)) {
        if (Comparef( (float)16 /(float) 9,  (float)aPreviewWidth / (float)aPreviewHeight)) {
            //16:9
            //Always run preview at Default size
            DBGT_PTRACE("Set 16:9 Preview");
            aPreviewWidth = DEFAULT_PREVIEW_WIDTH_REC_MODE;
            aPreviewHeight = DEFAULT_PREVIEW_HEIGHT_REC_MODE;
        } else {
            //4:3
            //Always run preview at Default size
            DBGT_PTRACE("Set 4:3 Preview");
            aPreviewWidth = DEFAULT_PREVIEW_WIDTH;
            aPreviewHeight = DEFAULT_PREVIEW_HEIGHT;
        }
    }
    if (aPreviewWidth != (int)mPreviewInfo.getWidth() || aPreviewHeight != (int)mPreviewInfo.getHeight()) {
        DBGT_PTRACE("Preview Size Changed");
        DBGT_EPILOG("");
        return true;
    }

    DBGT_PTRACE("No Change in Preview Size");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updateRecordSize(int &aRecordWidth, int &aRecordHeight)
{
    DBGT_PROLOG("Record Width: %d Record Height: %d", aRecordWidth, aRecordHeight);

    if ((mRecordInfo.getWidth() != (uint32_t)aRecordWidth) ||
            (mRecordInfo.getHeight() != (uint32_t)aRecordHeight)) {
        DBGT_PTRACE("Record Size Changed");
        DBGT_EPILOG("");
        return true;
    }

    DBGT_PTRACE("No Change in Record Size");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updatePictureSize(int &aPictureWidth, int &aPictureHeight)
{
    DBGT_PROLOG("Picture Width: %d Picture Height: %d", aPictureWidth, aPictureHeight);

    if (false == mPictureInfo.getEnableSwapParams()) {
        if ((mPictureInfo.getWidth() != (uint32_t)aPictureWidth) ||
                (mPictureInfo.getHeight() != (uint32_t)aPictureHeight)) {
            DBGT_EPILOG("Picture Size Changed");
            return true;
        }
    } else {
        if ((0 == mPictureInfo.getRotation()) || (180 == mPictureInfo.getRotation())) {
            if ((mPictureInfo.getWidth() != (uint32_t)aPictureWidth) ||
                    (mPictureInfo.getHeight() != (uint32_t)aPictureHeight)) {
                DBGT_PTRACE("Picture Size Changed");
                DBGT_EPILOG("");
                return true;
            }
        } else {
            if ((mPictureInfo.getHeight() != (uint32_t)aPictureWidth) ||
                    (mPictureInfo.getWidth() != (uint32_t)aPictureHeight)) {
                DBGT_PTRACE("Picture Size Changed");
                DBGT_EPILOG("");
                return true;
            }
        }
    }

    DBGT_PTRACE("No Change in Picture Size");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updateThumbnailSize(int &aThumbnailWidth, int &aThumbnailHeight)
{
    DBGT_PROLOG("Thumbnail Width: %d Thumbnail Height: %d", aThumbnailWidth, aThumbnailHeight);

    if (false == mThumbnailInfo.getEnableSwapParams()) {
        if ((mThumbnailInfo.getWidth() != (uint32_t)aThumbnailWidth) ||
                (mThumbnailInfo.getHeight() != (uint32_t)aThumbnailHeight)) {
            DBGT_PTRACE("Thumbnail Size Changed");
            DBGT_EPILOG("");
            return true;
        }
    } else {
        if ((0 == mThumbnailInfo.getRotation()) || (180 == mThumbnailInfo.getRotation())) {
            if ((mThumbnailInfo.getWidth() != (uint32_t)aThumbnailWidth) ||
                    (mThumbnailInfo.getHeight() != (uint32_t)aThumbnailHeight)) {
                DBGT_PTRACE("Thumbnail Size Changed");
                DBGT_EPILOG("");
                return true;
            }
        } else {
            if ((mThumbnailInfo.getHeight() != (uint32_t)aThumbnailWidth) ||
                    (mThumbnailInfo.getWidth() != (uint32_t)aThumbnailHeight)) {
                DBGT_PTRACE("Thumbnail Size Changed");
                DBGT_EPILOG("");
                return true;
            }
        }
    }

    DBGT_PTRACE("No Change in Thumbnail Size");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updatePictureQuality(int &aPictureQuality)
{
    DBGT_PROLOG("Picture Quality: %d", aPictureQuality);

    if ((mPictureInfo.getQuality() != aPictureQuality)) {
        DBGT_PTRACE("Picture Quality Changed");
        DBGT_EPILOG("");
        return true;
    }

    DBGT_PTRACE("No Change in Picture Quality");
    DBGT_EPILOG("");
    return false;
}

bool STECamera::updateThumbnailQuality(int &aThumbnailQuality)
{
    DBGT_PROLOG("Thumbnail Quality: %d", aThumbnailQuality);

    if ((mThumbnailInfo.getQuality() != aThumbnailQuality)) {
        DBGT_PTRACE("Thumbnail Quality Changed");
        DBGT_EPILOG("");
        return true;

    }

    DBGT_PTRACE("No Change in Thumbnail Quality");
    DBGT_EPILOG("");
    return false;
}

status_t STECamera::applyCameraProperties(const CameraParameters &aParams)
{
    DBGT_PROLOG("");

    if (mCam) {

        if (!strcmp(CameraParameters::SCENE_MODE_AUTO,aParams.get(CameraParameters::KEY_SCENE_MODE)))
        {
#ifdef PROP_WB_SCENE_MODE
            if (NO_ERROR !=  SetPropSceneModes(aParams, mCam)) {
                DBGT_CRITICAL("SetPropSceneModes failed");
                return -1;
            }
#endif
        }
#ifdef ZOOM_ENABLE
        if (NO_ERROR != SetPropZoom(aParams, mCam)) {
            DBGT_CRITICAL("SetPropZoom failed");
            return -1;
        }

#endif

#ifdef PROP_FLASH
        if (NO_ERROR !=  SetPropFlash(aParams, mCam)) {
            DBGT_CRITICAL("SetPropFlash failed");
            return -1;
        }

#endif

        if (NO_ERROR !=  SetExposureCompensation(aParams, mCam)) {
            DBGT_CRITICAL("SetExposureCompensation failed");
            return -1;
        }

        if (NO_ERROR !=  SetPropFlickerRemoval(aParams, mCam)) {
            DBGT_CRITICAL("SetPropFlickerRemoval failed");
            return -1;
        }

#ifdef PROP_BCS
        if (NO_ERROR !=  SetPropContrast(aParams, mCam)) {
            DBGT_CRITICAL("SetPropContrast failed");
            return -1;
        }

        if (NO_ERROR !=  SetPropBrightness(aParams, mCam)) {
            DBGT_CRITICAL("SetPropBrightness failed");
            return -1;
        }

        if (NO_ERROR !=  SetPropSaturation(aParams, mCam)) {
            DBGT_CRITICAL("SetPropSaturation failed");
            return -1;
        }

#endif

#ifdef PROP_COLOR_EFFECT
        if (NO_ERROR !=  SetPropColorEffect(aParams, mCam)) {
            DBGT_CRITICAL("SetPropEffect failed");
            return -1;
        }

#endif

#ifdef PROP_SHARPNESS
        if (NO_ERROR !=  SetPropSharpness(aParams, mCam)) {
            DBGT_CRITICAL("SetPropSharpness failed");
            return -1;
        }

#endif

#ifdef REGION_CONTROL
        if (NO_ERROR !=  SetPropRegionControl(aParams, mCam)) {
            DBGT_CRITICAL("SetPropEffect failed");
            return -1;
        }
        if (NO_ERROR !=  SetPropMeteringArea(aParams, mCam)) {
            DBGT_CRITICAL("SetPropMeteringArea failed");
            return -1;
        }
        if (NO_ERROR !=  SetPropFocusArea(aParams, mCam)) {
            DBGT_CRITICAL("SetPropFocusArea failed");
            return -1;
        }

#endif
        if (NO_ERROR !=  SetPropFocus(aParams, mCam, false)) {
            DBGT_CRITICAL("SetPropFocus failed");
            return -1;
        }

        //@FIXME: Workaround SetWb fails sometimes
#ifdef PROP_WB_SCENE_MODE
        if (NO_ERROR !=  SetPropWhiteBalance(aParams, mCam)) {
            DBGT_CRITICAL("SetPropWhiteBalance failed");
            return -1;
        }
        if (strcmp(CameraParameters::SCENE_MODE_AUTO,
                    aParams.get(CameraParameters::KEY_SCENE_MODE))) {
            if (NO_ERROR != SetPropSceneModes(aParams, mCam)) {
                DBGT_CRITICAL("SetPropSceneModes failed");
                return -1;
            }
        }

#endif

        if (mUpdateAutoWhiteBalanceLock) {
            //reset
            mUpdateAutoWhiteBalanceLock = false;
            if (NO_ERROR !=  SetPropAWBLock(aParams, mCam)) {
                DBGT_CRITICAL("SetPropAWBLock failed");
                return -1;
            }
        }

        if (mUpdateAutoExposureLock) {
            //reset
            mUpdateAutoExposureLock = false;
            if (NO_ERROR !=  SetPropAELock(aParams, mCam)) {
                DBGT_CRITICAL("SetPropAELock failed");
                return -1;
            }
        }

    }
    DBGT_EPILOG("");
    return NO_ERROR;
}

CameraParameters STECamera::getParameters() const
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    DBGT_EPILOG("");

    return mParameters;
}

char* STECamera::getParameters()
{
    String8 params_str8;
    char* params_string;

    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    params_str8 = mParameters.flatten();

    // camera service frees this string...
    params_string = (char*) malloc(sizeof(char) * (params_str8.length()+1));
    strcpy(params_string, params_str8.string());

    DBGT_EPILOG("");

    return params_string;
}

void STECamera::putParameters(char *parms)
{
    free(parms);
}

void STECamera::release()
{
    DBGT_PROLOG("");




    DBGT_EPILOG("");
}
#define STRINGIFY_PARAM(aParam) #aParam
const char *STECamera::msgName(int32_t aParam)
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

/*****************************************************************************/
//  GPS keys provide decimal values which are to converted to
//  GPS degree/minute/seconds.  Say GPS latitude key value is 23.19543.
//  ** The whole units of degrees = 23
//  Multiply the decimal by 60 (0.19543 * 60 = 11.7258)
//  ** The whole number provides the minutes  =  11
//  Again multiply the remaining decimal by 60 (0.7258* 60 = 43.548)
//  ** The resulting number becomes the seconds (43.548).
//  Seconds field can have decimal values till 3 points after decimal.
//  These values appear as rationals in the exif viewer as:
//       23/1 11/1 43548/1000
// For the OMX_U64 values, the first 32 bits are the numerator and the last 32
// the denominator .
/*****************************************************************************/
OMX_BOOL STECamera::convertToDegrees(
        OMX_U64 &aDegrees,
        OMX_U64 &aMinutes,
        OMX_U64 &aSeconds,
        char const *const aGPSKey )
{
    DBGT_PROLOG("GPS key %s", aGPSKey);

    float tmpVal;
    OMX_U32 absVal = 0;
    OMX_BOOL referenceDir = OMX_TRUE;

    if ( NULL != aGPSKey ) {
        tmpVal = atof(aGPSKey);

        if ( 0 > tmpVal) {
            // If the valur of key is negative , then the direction is opposite to
            // whatever the reference direction is.
            DBGT_PTRACE("Direction opposite of reference direction");
            referenceDir = OMX_FALSE;
            tmpVal = tmpVal * (-1);
        }

        aDegrees = ((OMX_U64)tmpVal<<32)|1;
        aMinutes = ((OMX_U64)abs(extractSubUnitLocation(tmpVal)) << 32) | 1;
        aSeconds = (extractSubUnitLocation(tmpVal) * 1000) ;
        aSeconds = ((aSeconds<< 32)|(0x3E8));
    }

    DBGT_EPILOG("");
    return referenceDir;
}

/**********************************************************************************/
//  KEY_GPS_TIMESTAMP provides time in the format of seconds since January 1, 1970.
//  This has to be converted to real time which can be done as  follows:
//  Say GPS time stamp value is tS= 1420724859.
//  (tS %( 24x60x60)) leaves us with the absolute number of seconds elapsed today.
//  Hence,ts now is 49659. This can be converted easily to hours , minutes and seconds.
//  These values appear as rationals in the exif viewer as:
//     13/1 47/1 39/1
//  For the OMX_U64 values, the first 32 bits are the numerator and the last 32
//  the denominator.
/*****************************************************************************/
void STECamera::convertUTCToTime(
        OMX_U64& aHours,
        OMX_U64& aMinutes,
        OMX_U64& aSeconds,
        char const* const aGPSKey )
{
    DBGT_PROLOG("UTC time %s", aGPSKey);
    OMX_U64 tmpTime;
    if ( NULL != aGPSKey )
    {
        tmpTime = atoi(aGPSKey);
        tmpTime = (tmpTime % 86400);

        aSeconds = ((OMX_U64)extractSubUnitTime(tmpTime)<<32)| 1;
        aMinutes = ((OMX_U64)extractSubUnitTime(tmpTime)<<32)| 1;
        aHours =  (tmpTime<<32)| 1;
    }
    DBGT_EPILOG("");
    return;
}

OMX_U64 STECamera::extractSubUnitTime(OMX_U64 &aTime)
{
    DBGT_PROLOG("Time Value %lld", aTime);

    OMX_U64 tmp;
    tmp = aTime % 60;
    aTime = aTime / 60;

    DBGT_EPILOG("");
    return tmp;
}

float STECamera::extractSubUnitLocation(float &aWholeValue)
{
    DBGT_PROLOG("Whole Value %f", aWholeValue);

    aWholeValue -= abs(aWholeValue);
    aWholeValue *= 60;

    DBGT_PROLOG("Whole Value %f", aWholeValue);
    return aWholeValue;
}

void STECamera::setFocusDistance()
{
    DBGT_PROLOG("");

    char focusDistances[32];

    float optimalFocusDistance = focusHandler()->getFocusDistance();

    if (0 != optimalFocusDistance)
        sprintf(focusDistances, "%f,%f,%s", 0.10, optimalFocusDistance, "Infinity");
    else
        sprintf(focusDistances, "%f,%s,%s", 0.10, "Infinity", "Infinity");

    mParameters.set(CameraParameters::KEY_FOCUS_DISTANCES, focusDistances);

    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::setLensParameters(void *aArg)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_ASSERT(NULL != mCam, "Camera Handle is Null");

    OmxUtils::StructContainer<OMX_STE_CONFIG_LENSPARAMETERTYPE> lensParameters;

    err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ELensParams), lensParameters.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("setLensParameters OMX_GetConfig failed");
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(0 < lensParameters.ptr()->nFocalLength, "Invalid Focal Length");
    DBGT_ASSERT(0 < lensParameters.ptr()->nHorizontolViewAngle, "Invalid Horizontal View Angle");
    DBGT_ASSERT(0 < lensParameters.ptr()->nVerticalViewAngle, "Invalid Vertical View Angle");

    DBGT_PTRACE("FocalLength = %lu", lensParameters.ptr()->nFocalLength);
    DBGT_PTRACE("HorizontolViewAngle = %lu", lensParameters.ptr()->nHorizontolViewAngle);
    DBGT_PTRACE("VerticalViewAngle = %lu", lensParameters.ptr()->nVerticalViewAngle);

    mLock.lock();
    mParameters.setFloat(CameraParameters::KEY_FOCAL_LENGTH, lensParameters.ptr()->nFocalLength/(65536.0));
    mParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, lensParameters.ptr()->nHorizontolViewAngle);
    mParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, lensParameters.ptr()->nVerticalViewAngle);
    mLock.unlock();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::setGPSTags(const CameraParameters &aParams, OMX_HANDLETYPE aHandle, int aPort)
{
    DBGT_PROLOG("Latitude: %s Longitude: %s Altitude: %s TimeStamp: %s",
            aParams.get(CameraParameters::KEY_GPS_LATITUDE),
            aParams.get(CameraParameters::KEY_GPS_LONGITUDE),
            aParams.get(CameraParameters::KEY_GPS_ALTITUDE),
            aParams.get(CameraParameters::KEY_GPS_TIMESTAMP));

    DBGT_ASSERT(NULL != aHandle, "Handle is Null");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_INDEXTYPE index;


    if ((NULL != aParams.get(CameraParameters::KEY_GPS_LATITUDE)) ||
            (NULL != aParams.get(CameraParameters::KEY_GPS_LONGITUDE)) ||
            (NULL != aParams.get(CameraParameters::KEY_GPS_ALTITUDE)) ||
            (NULL != aParams.get(CameraParameters::KEY_GPS_TIMESTAMP)) ||
            (NULL != aParams.get(CameraParameters::KEY_GPS_PROCESSING_METHOD))) {

        char gpsIndex[] = "OMX.Symbian.Index.Config.GpsLocation";
        DBGT_PTRACE("Setting GPS tags");

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_GPSLOCATIONTYPE> gpsLocation(OMX_TRUE);
        err = OMX_GetExtensionIndex(aHandle, gpsIndex, &index);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("GPS tag OMX_GetExtensionIndex failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        gpsLocation.ptr()->bLatitudeRefNorth
            = convertToDegrees(gpsLocation.ptr()->nLatitudeDegrees,
                    gpsLocation.ptr()->nLatitudeMinutes,
                    gpsLocation.ptr()->nLatitudeSeconds,
                    aParams.get(CameraParameters::KEY_GPS_LATITUDE));

        gpsLocation.ptr()->bLongitudeRefEast
            = convertToDegrees(gpsLocation.ptr()->nLongitudeDegrees,
                    gpsLocation.ptr()->nLongitudeMinutes,
                    gpsLocation.ptr()->nLongitudeSeconds,
                    aParams.get(CameraParameters::KEY_GPS_LONGITUDE));

        if (NULL != aParams.get(CameraParameters::KEY_GPS_ALTITUDE) ) {

            gpsLocation.ptr()->nAltitudeMeters
                = atof(aParams.get(CameraParameters::KEY_GPS_ALTITUDE));

            if ( 0 < gpsLocation.ptr()->nAltitudeMeters ) {
                gpsLocation.ptr()->bAltitudeRefAboveSea = OMX_TRUE;
            } else {
                gpsLocation.ptr()->bAltitudeRefAboveSea = OMX_FALSE;
                gpsLocation.ptr()->nAltitudeMeters *= -1;
            }
            gpsLocation.ptr()->nAltitudeMeters
                = (((gpsLocation.ptr()->nAltitudeMeters * 1000)<<32) | (0x3E8));
        }

        if(NULL != aParams.get(CameraParameters::KEY_GPS_TIMESTAMP)){
            time_t rawtime = atol(aParams.get(CameraParameters::KEY_GPS_TIMESTAMP));

            struct tm *tmDateTime;
            tmDateTime = localtime(&rawtime);

            gpsLocation.ptr()->nYear = tmDateTime->tm_year + 1900; // tm_year gives the number of year after 1900
            gpsLocation.ptr()->nMonth = tmDateTime->tm_mon + 1;    // tm_mon assumes Jan to be 0.
            gpsLocation.ptr()->nDay = tmDateTime->tm_mday;

            DBGT_PTRACE("GPS date Year: %lld Month: %lld Day : %lld",
                gpsLocation.ptr()->nYear, gpsLocation.ptr()->nMonth, gpsLocation.ptr()->nDay );
        }

        convertUTCToTime(gpsLocation.ptr()->nHours,
                gpsLocation.ptr()->nMinutes,
                gpsLocation.ptr()->nSeconds,
                aParams.get(CameraParameters::KEY_GPS_TIMESTAMP));

        //GPS Processing Method
        if (NULL != aParams.get(CameraParameters::KEY_GPS_PROCESSING_METHOD)) {
            strcpy((char*)gpsLocation.ptr()->nPrMethodDataValue,
                    aParams.get(CameraParameters::KEY_GPS_PROCESSING_METHOD));
            gpsLocation.ptr()->processingValue = OMX_ASCII_Format;
        }

        gpsLocation.ptr()->bLocationKnown = OMX_TRUE;
        gpsLocation.ptr()->nPortIndex = aPort;

        err = OMX_SetConfig(aHandle, index , gpsLocation.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig for GPS failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    } else {
        DBGT_PTRACE("No GPS info available");
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::setMakeAndModel()
{
    DBGT_PROLOG();

    DBGT_ASSERT(mCam != NULL, "Handle is Null");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OmxUtils::StructContainer<OMX_SYMBIAN_PARAM_DEVICEMAKEMODEL> makeModel;

    err = OMX_GetParameter(mCam,
            mOmxILExtIndex->getIndex(OmxILExtIndex::EMakeModel),
            makeModel.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Make Model OMX_GetParameter failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    /* Get Manufacturer value */
#ifdef CAM_EXIF_TAG_MAKE
    strcpy((char *)&makeModel.ptr()->cMake[0], CAM_EXIF_TAG_MAKE);
#else
    DynSetting::get(DynSetting::EManufacturer, (char *)&makeModel.ptr()->cMake[0]);
#endif //CAM_EXIF_TAG_MAKE

    /* Get Model value */
#ifdef CAM_EXIF_TAG_MODEL
    strcpy((char *)&makeModel.ptr()->cModel[0], CAM_EXIF_TAG_MODEL);
#else
    DynSetting::get(DynSetting::EModel, (char *)&makeModel.ptr()->cModel[0]);
#endif //CAM_EXIF_TAG_MODEL

    makeModel.ptr()->nPortIndex = OMX_ALL;

    err = OMX_SetParameter(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EMakeModel), makeModel.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Make Model OMX_SetParameter failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::configVideoRevertAspectRatio(OMX_HANDLETYPE hComponent, OMX_BOOL aEnable)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OmxUtils::StructContainer<OMX_CONFIG_BOOLEANTYPE> revertAspectRatio;

    err = OMX_GetConfig(hComponent,
            (OMX_INDEXTYPE)OMX_Symbian_IndexParamRevertAspectRatio,
            revertAspectRatio.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Revert Aspect Ratio OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    revertAspectRatio.ptr()->bEnabled = aEnable;

    err = OMX_SetConfig(hComponent,
            (OMX_INDEXTYPE)OMX_Symbian_IndexParamRevertAspectRatio,
            revertAspectRatio.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Revert Aspect Ratio OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::setPropVideoRotate(OMX_HANDLETYPE aCam, int aRotation)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

#ifdef ENABLE_VIDEO_ROTATION
    OmxUtils::StructContainer<OMX_CONFIG_ROTATIONTYPE> commonRotateConfig;
    commonRotateConfig.ptr()->nPortIndex = 2;

    err = OMX_GetConfig(aCam, (OMX_INDEXTYPE)OMX_IndexConfigCommonRotate, commonRotateConfig.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Rotation OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (aRotation == 0) {
        commonRotateConfig.ptr()->nRotation = 0;
    } else if (aRotation == 90) {   /*< Frame is 90 degrees CW rotated */
        commonRotateConfig.ptr()->nRotation = 90;
    } else if (aRotation == 180) {  /*do nothing in SIA*/
        commonRotateConfig.ptr()->nRotation = 0;
    } else if (aRotation == 270) {  /*< Frame is 270 degrees CW rotated */
        commonRotateConfig.ptr()->nRotation = 270;
    } else
        DBGT_CRITICAL("Cant Happen");

    DBGT_PTRACE("Set Video Rotation: %d",commonRotateConfig.ptr()->nRotation);

    err = OMX_SetConfig(aCam, (OMX_INDEXTYPE)OMX_IndexConfigCommonRotate, commonRotateConfig.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Rotation OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }
    camera_sem_wait(&video_rot_sem, SEM_WAIT_TIMEOUT);

#endif //ENABLE_VIDEO_ROTATION

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropContrast(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OmxUtils::StructContainer<OMX_CONFIG_CONTRASTTYPE> contrast_Config;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != params.get(KEY_CONTRAST)) {
        if ((params.getInt(KEY_CONTRAST) < -100) || (params.getInt(KEY_CONTRAST) > 100)) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        } else {
            mContrast = params.getInt(KEY_CONTRAST);
        }

        contrast_Config.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonContrast), contrast_Config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        contrast_Config.ptr()->nContrast =  mContrast;
        DBGT_PTRACE("Set Contrast: %ld",contrast_Config.ptr()->nContrast);

        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonContrast), contrast_Config.ptr() );
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current contrast
        mParameters.set(KEY_CONTRAST,params.get(KEY_CONTRAST));
    }
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropBrightness(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OmxUtils::StructContainer<OMX_CONFIG_BRIGHTNESSTYPE> brightness_Config;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != params.get(KEY_BRIGHTNESS)) {
        if ( (params.getInt(KEY_BRIGHTNESS) > 100 ) || (params.getInt(KEY_BRIGHTNESS) < 0 ) ) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        } else {
            mBrightness = params.getInt(KEY_BRIGHTNESS);
        }

        brightness_Config.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam,
                (OMX_INDEXTYPE)(OMX_IndexConfigCommonBrightness),
                brightness_Config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        brightness_Config.ptr()->nBrightness =  mBrightness;
        DBGT_PTRACE("Set brightness: %ld",brightness_Config.ptr()->nBrightness);

        err = OMX_SetConfig(mCam,
                (OMX_INDEXTYPE)(OMX_IndexConfigCommonBrightness),
                brightness_Config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current brightness
        mParameters.set(KEY_BRIGHTNESS,params.get(KEY_BRIGHTNESS));
    }
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropSaturation(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    if (!strcmp(mCurrentSaturationMode,params.get(KEY_SATURATION)))
    {
        // same mode as previous so no need to change
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != params.get(KEY_SATURATION)) {
        OmxUtils::StructContainer<OMX_CONFIG_SATURATIONTYPE> saturation_Config;

        if ((params.getInt(KEY_SATURATION) < -100) || (params.getInt(KEY_SATURATION) > 100)) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        } else {
            mSaturation = params.getInt(KEY_SATURATION);
        }

        saturation_Config.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam,
                (OMX_INDEXTYPE)(OMX_IndexConfigCommonSaturation),
                saturation_Config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        saturation_Config.ptr()->nSaturation =   mSaturation;
        DBGT_PTRACE("Set saturation: %ld",saturation_Config.ptr()->nSaturation);

        err = OMX_SetConfig(mCam,
                (OMX_INDEXTYPE)(OMX_IndexConfigCommonSaturation),
                saturation_Config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current saturation
        mParameters.set(KEY_SATURATION,params.get(KEY_SATURATION));
        strcpy(mCurrentSaturationMode,params.get(KEY_SATURATION));
    }
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropSharpness(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    if (!strcmp(mCurrentSharpnessMode,params.get(KEY_SHARPNESS)))
    {
        // same mode as previous so no need to change
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != params.get(KEY_SHARPNESS)) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_S32TYPE> Sharpness_config;
        Sharpness_config.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness),
                Sharpness_config.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        Sharpness_config.ptr()->nValue = (OMX_S32)params.getInt(KEY_SHARPNESS);
        DBGT_PTRACE("Set Sharpness: %ld",Sharpness_config.ptr()->nValue);

        err = OMX_SetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness),
                &Sharpness_config);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current sharpness
        mParameters.set(KEY_SHARPNESS,params.get(KEY_SHARPNESS));
        strcpy(mCurrentSharpnessMode,params.get(KEY_SHARPNESS));
    }
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropRegionControl(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    RegionControlProperty regioncontrolproperty[] = {
        {0, REGION_CONTROL_OFF,      OMX_SYMBIAN_FocusRegionAuto},
        {0, REGION_CENTER,           (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionCenter},
        {0, REGION_MULTI,            OMX_SYMBIAN_FocusRegionAuto},
        {0, REGION_USER,             OMX_SYMBIAN_FocusRegionManual},
        {0, REGION_TOUCH,            (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionTouchPriority},
        {0, REGION_OBJECT,           OMX_SYMBIAN_FocusRegionObjectPriority},
        {1, REGION_FACE,             OMX_SYMBIAN_FocusRegionFacePriority},
    };

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE> Focus_Region;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ROITYPE> roi_type;
    int number_roi=0;
    int roi_coordinate[ROI_MAX];
    if (NULL != params.get(REGION_ROI_NUMBER)) {
        number_roi = params.getInt(REGION_ROI_NUMBER);
    } else {
        number_roi = 0;
    }

    if (NULL != params.get(KEY_REGION_CONTROL)) {
        int id = 0;
        int max_id = sizeof(regioncontrolproperty) / sizeof(regioncontrolproperty[0]);

        //focus region received from the application
        const char *selRegionControl = params.get(KEY_REGION_CONTROL);

        while ( (id < max_id) &&
                (strcmp(regioncontrolproperty[id].regioncontrol, selRegionControl) != 0) ) {
            id++;
        }

        if (id >= max_id) {
            DBGT_CRITICAL("Invalid Region Control error = %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        roi_type.ptr()->nPortIndex = OMX_ALL;
        Focus_Region.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRegion),
                Focus_Region.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed error = %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        err = OMX_GetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
                roi_type.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed error = %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        roi_type.ptr()->nNumberOfROIs = number_roi;

        if ((regioncontrolproperty[id].eRegion == (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionTouchPriority) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionObjectPriority) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionFacePriority) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionManual)) {

            if ((NULL == params.get(REGION_COORDINATES)) || (number_roi == 0)) {
                DBGT_CRITICAL("Error in Region Coordinate error = %d", (int)UNKNOWN_ERROR);
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

        }

        /* Region type is set again only if it has changed. This is due to the fact
         * that while taking picture in CAF mode, setting focus region along with
         * focus control restarts the sw3A loop, resulting in infinite wait for focus
         * lock. */
        if (strcmp(mCurrentRegControl, selRegionControl) == 0) {
            DBGT_PTRACE("Received same region control from application.\n");
            if ((regioncontrolproperty[id].eRegion == (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionTouchPriority) ||
                    (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionObjectPriority) ||
                    (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionFacePriority) ||
                    (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionManual)) {
                char *regionCoordinate = (char *)params.get(REGION_COORDINATES);
                if ((strcmp(mCurrentRegCoordinate,regionCoordinate) == 0) &&
                    (mCurrentRegCoordinateNum == number_roi)) {
                    DBGT_PTRACE("Received same region ROI from application\n");
                    DBGT_EPILOG("");
                    return err;
                }
            } else {
                DBGT_EPILOG("");
                return err;
            }
        }

        if ((regioncontrolproperty[id].eRegion == (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionTouchPriority) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionObjectPriority) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionManual) ||
                (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionFacePriority)) {

            char *region_coordinate = (char *)params.get(REGION_COORDINATES);

            for (int i = 0; i < number_roi; i++) {
                if (true != getROICoordinates( &region_coordinate,roi_coordinate)) {
                    DBGT_CRITICAL("Error in ROI error = %d", (int)UNKNOWN_ERROR);
                    DBGT_EPILOG("");
                    return UNKNOWN_ERROR;
                }

                DBGT_PTRACE("Coordinates for %d ROI are:",i);
                DBGT_PTRACE("%d %d %d %d",
                    roi_coordinate[ROI_LEFT], roi_coordinate[ROI_TOP], roi_coordinate[ROI_WIDTH],
                    roi_coordinate[ROI_HEIGHT]);
                DBGT_PTRACE("%d %d",
                    roi_coordinate[ROI_REFERENCE_WIDTH],roi_coordinate[ROI_REFERENCE_HEIGHT]);

                if (i < (number_roi - 1)) {
                    if ( *region_coordinate != ',') {
                        DBGT_CRITICAL("Error in SetPropRegionControl - OMX_ErrorBadParameter");
                        DBGT_EPILOG("");
                        return OMX_ErrorBadParameter;
                    }
                    region_coordinate++;
                }

                roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nX = roi_coordinate[ROI_LEFT];
                roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nY = roi_coordinate[ROI_TOP];
                roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nWidth = roi_coordinate[ROI_WIDTH];
                roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nHeight = roi_coordinate[ROI_HEIGHT];

                roi_type.ptr()->sROIs[i].sROI.sReference.nWidth = roi_coordinate[ROI_REFERENCE_WIDTH];
                roi_type.ptr()->sROIs[i].sROI.sReference.nHeight = roi_coordinate[ROI_REFERENCE_HEIGHT];

                if (regioncontrolproperty[id].eRegion == (OMX_SYMBIAN_FOCUSREGIONCONTROL)OMX_STE_FocusRegionTouchPriority) {
                        roi_type.ptr()->sROIs[i].eObjectType = (OMX_SYMBIAN_ROIOBJECTTYPE)OMX_STE_RoiObjectTypeTouch;
                } else if (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionObjectPriority) {
                        roi_type.ptr()->sROIs[i].eObjectType = OMX_SYMBIAN_RoiObjectTypeObject;
                } else if (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionFacePriority) {
                        roi_type.ptr()->sROIs[i].eObjectType = OMX_SYMBIAN_RoiObjectTypeFace;
                }
            }

            err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI), roi_type.ptr());
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_SetConfig failed error = %d", (int)UNKNOWN_ERROR);
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
        }

        Focus_Region.ptr()->eFocusRegionControl = regioncontrolproperty[id].eRegion;
        DBGT_PTRACE("Set Focus Region %s",OmxUtils::name((OMX_SYMBIAN_FOCUSREGIONCONTROL)Focus_Region.ptr()->eFocusRegionControl));

        //For manual mode only Focus_Region.ptr()->sFocusRegion
        if (regioncontrolproperty[id].eRegion == OMX_SYMBIAN_FocusRegionManual) {
            char *region_coordinate = (char *)params.get(REGION_COORDINATES);

            if ( (number_roi != 1) || (true != getROICoordinates( &region_coordinate,roi_coordinate) ) ) {
                DBGT_CRITICAL("Error in SetPropRegionControl error = %d", (int)UNKNOWN_ERROR);
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }

            DBGT_PTRACE("Coordinates  are %d %d %d %d",
                roi_coordinate[ROI_LEFT], roi_coordinate[ROI_TOP], roi_coordinate[ROI_WIDTH],
                roi_coordinate[ROI_HEIGHT]);
            DBGT_PTRACE("%d %d",
                roi_coordinate[ROI_REFERENCE_WIDTH],roi_coordinate[ROI_REFERENCE_HEIGHT]);

            Focus_Region.ptr()->sFocusRegion.sRect.sTopLeft.nX = roi_coordinate[ROI_LEFT];
            Focus_Region.ptr()->sFocusRegion.sRect.sTopLeft.nY = roi_coordinate[ROI_TOP];
            Focus_Region.ptr()->sFocusRegion.sRect.sSize.nWidth = roi_coordinate[ROI_WIDTH];
            Focus_Region.ptr()->sFocusRegion.sRect.sSize.nHeight = roi_coordinate[ROI_HEIGHT];
            Focus_Region.ptr()->sFocusRegion.sReference.nWidth = roi_coordinate[ROI_REFERENCE_WIDTH];
            Focus_Region.ptr()->sFocusRegion.sReference.nHeight = roi_coordinate[ROI_REFERENCE_HEIGHT];
        }

        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRegion), Focus_Region.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed error = %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        //set current region contol mode & region coordinate
        mParameters.set(KEY_REGION_CONTROL,params.get(KEY_REGION_CONTROL));

        strcpy(mCurrentRegControl,params.get(KEY_REGION_CONTROL));
        mCurrentRegCoordinateNum = number_roi;

        if (NULL!=params.get(REGION_COORDINATES) ) {
            mParameters.set(REGION_COORDINATES,params.get(REGION_COORDINATES));
            strcpy(mCurrentRegCoordinate,params.get(REGION_COORDINATES));
        }
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropMeteringArea(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ROITYPE> roi_type;

#ifdef ENABLE_FACE_DETECTION
    if (isFaceDetectionEnable()) {
        DBGT_PTRACE("Face detection enabled...set metering area is not allowed");
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }
#endif

    if (!(params.get(CameraParameters::KEY_METERING_AREAS))) {
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    int MAX_METERING_AREAS;

    int roi_coordinate[COORDINATES];
    if ( params.getInt(CameraParameters::KEY_MAX_NUM_METERING_AREAS)) {
        MAX_METERING_AREAS
            = atoi(params.get(CameraParameters::KEY_MAX_NUM_METERING_AREAS));
    } else {
        MAX_METERING_AREAS = 0;
    }
    roi_type.ptr()->nNumberOfROIs = MAX_METERING_AREAS;

    const char *str = params.get(CameraParameters::KEY_METERING_AREAS);
    if (str != NULL) {
        roi_type.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
                roi_type.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
        int maxAreasLessOne = 0;
        for (int i = 0; i < MAX_METERING_AREAS; i++) {
            sscanf(str, "(%d,%d,%d,%d,%d)",&roi_coordinate[0],&roi_coordinate[1],
                    &roi_coordinate[2],&roi_coordinate[3],&roi_coordinate[4] );

            if (!(roi_coordinate[0] == 0 &&
                        roi_coordinate[1] == 0 &&
                        roi_coordinate[2] == 0 &&
                        roi_coordinate[3] == 0 &&
                        roi_coordinate[4] == 0)) {
                if (roi_coordinate[0] < -1000 || roi_coordinate[0] >  1000 ||
                        roi_coordinate[1] < -1000 || roi_coordinate[1] >  1000 ||
                        roi_coordinate[2] < -1000 || roi_coordinate[2] >  1000 ||
                        roi_coordinate[3] < -1000 || roi_coordinate[3] >  1000 ||
                        roi_coordinate[4] < 1 || roi_coordinate[4] >  1000  ||
                        roi_coordinate[0] >= roi_coordinate[2]||
                        roi_coordinate[1] >= roi_coordinate[3]) {
                    DBGT_EPILOG("Checking coordinates: Error");
                    return OMX_ErrorUnsupportedSetting;
                }
            }
            DBGT_PTRACE("Coordinates for %d ROI recieved from Application are:",i);
            DBGT_PTRACE("%d %d %d %d %d",
                    roi_coordinate[0], roi_coordinate[1], roi_coordinate[2],
                    roi_coordinate[3],roi_coordinate[4] );

            //Range of area specified by user varies from -1000 to +1000
            int hRange = 2000;
            int vRange = 2000;
            float hScale = ( float ) 99 / ( float ) hRange;
            float vScale = ( float ) 99 / ( float ) vRange;

            if (roi_coordinate[2] > roi_coordinate[0]) {
                roi_coordinate[2] -= roi_coordinate[0];
            } else {
                roi_coordinate[2] = roi_coordinate[0] - roi_coordinate[2];
            }
            if (roi_coordinate[3] > roi_coordinate[1]) {
                roi_coordinate[3] -= roi_coordinate[1];
            } else {
                roi_coordinate[3] = roi_coordinate[1] - roi_coordinate[3];
            }

            roi_coordinate[0] = (((float)( roi_coordinate[0] +
                            hRange / 2 ))* hScale) + 1;
            roi_coordinate[1] = (((float)( roi_coordinate[1] +
                            vRange / 2 )) * vScale) + 1;
            roi_coordinate[2] = (((float)( roi_coordinate[2])) * hScale) + 1;
            roi_coordinate[3] = (((float)( roi_coordinate[3])) * vScale) + 1;

            DBGT_PTRACE("Coordinates for %d ROI sent to OMX are:",i);
            DBGT_PTRACE("%d %d %d %d %d",
                    roi_coordinate[0], roi_coordinate[1], roi_coordinate[2],
                    roi_coordinate[3],roi_coordinate[4]);

            roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nX = roi_coordinate[0];
            roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nY = roi_coordinate[1];
            roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nWidth = roi_coordinate[2];
            roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nHeight = roi_coordinate[3];
            roi_type.ptr()->sROIs[i].nPriority = roi_coordinate[4];
            roi_type.ptr()->sROIs[i].eObjectType =
                (OMX_SYMBIAN_ROIOBJECTTYPE) OMX_STE_RoiObjectTypeTouch;

            err = OMX_SetConfig(mCam,
                    mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
                    roi_type.ptr());
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_SetConfig failed error - %d", (int)UNKNOWN_ERROR);
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
            char *tempStr = strstr(str, ",(");
            if (tempStr) {
                str = tempStr+1;
                maxAreasLessOne ++;
            } else {
                break;
            }
            if (maxAreasLessOne + 1 > MAX_METERING_AREAS) {
                DBGT_EPILOG("Return Error");
                return OMX_ErrorUnsupportedSetting;
            }
        }
    }

    DBGT_EPILOG("");
    return err;
}

#ifdef ENABLE_FACE_DETECTION
int STECamera::sendFaceROI(camera_frame_metadata_t aFaceMetaData) {

    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ROITYPE> roi_type;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE> Focus_Region;

    int numOfFocusAread = aFaceMetaData.number_of_faces;
    int leftX = 0, leftY = 0, width = 0, height = 0;
    int weight = 500;
    int hRange = 2000;
    int vRange = 2000;

    roi_type.ptr()->nNumberOfROIs = numOfFocusAread;
    roi_type.ptr()->nPortIndex = OMX_ALL;
    err = OMX_GetConfig(mCam,
            mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
            roi_type.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig failed error - %d", (int)UNKNOWN_ERROR);
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }


    Focus_Region.ptr()->nPortIndex = OMX_ALL;

    err = OMX_GetConfig(mCam,
            mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRegion),
            Focus_Region.ptr());
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetConfig Failed error - %d", (int)UNKNOWN_ERROR);
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }


    float hScale = ( float ) 100 / ( float ) hRange;
    float vScale = ( float ) 100 / ( float ) vRange;
    camera_face_t *face = aFaceMetaData.faces;
    for (int i=0 ;i<numOfFocusAread; i++) {

        leftX = face[i].rect[0];
        leftY = face[i].rect[1];
        width = face[i].rect[2];
        height = face[i].rect[3];


        /* replace this if else with abs functin */
        if (width > leftX) {
            width = width - leftX;
        } else {
            width = leftX - width;
        }

        /* replace this if else with abs functin */
        if (height > leftY) {
            height = height - leftY;
        } else {
            height = leftY - height;
        }

        leftX = ((float)(leftX + hRange / 2) * hScale ) + 1;
        leftY = ((float)(leftY + vRange / 2) * vScale ) + 1;
        width = ((float)width * hScale) + 1;
        height = ((float)height * vScale) + 1;

        DBGT_PTRACE("Left %d Top %d width %d height %d",
                leftX,leftY,width,height);

        roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nX = leftX;
        roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nY = leftY;
        roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nWidth = width;
        roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nHeight = height;
        roi_type.ptr()->sROIs[i].nPriority = weight;    //hardcoded
        roi_type.ptr()->sROIs[i].eObjectType =
            (OMX_SYMBIAN_ROIOBJECTTYPE) OMX_SYMBIAN_RoiObjectTypeFace;
    }

    err = OMX_SetConfig(mCam,
            mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
            roi_type.ptr());

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed error - %d", (int)UNKNOWN_ERROR);
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }

    Focus_Region.ptr()->eFocusRegionControl = OMX_SYMBIAN_FocusRegionFacePriority;
    err = OMX_SetConfig(mCam,
            mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRegion),
            Focus_Region.ptr());

    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig failed error - %d", (int)UNKNOWN_ERROR);
        DBGT_EPILOG("");
        return UNKNOWN_ERROR;
    }
    DBGT_EPILOG("");
    return err;
}
#endif

int STECamera::SetPropFocusArea(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ROITYPE> roi_type;

#ifdef ENABLE_FACE_DETECTION
    if (isFaceDetectionEnable()) {
        DBGT_PTRACE("Face detection enabled...set FocusArea is not allowed");
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }
#endif

    if (!(params.get(CameraParameters::KEY_FOCUS_AREAS))) {
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    int MAX_FOCUS_AREAS;

    int roi_coordinate[COORDINATES];
    if ( params.getInt(CameraParameters::KEY_MAX_NUM_FOCUS_AREAS)) {
        MAX_FOCUS_AREAS
            = atoi(params.get(CameraParameters::KEY_MAX_NUM_FOCUS_AREAS));
    } else {
        MAX_FOCUS_AREAS = 0;
    }

    roi_type.ptr()->nNumberOfROIs = MAX_FOCUS_AREAS;
    const char *str = params.get(CameraParameters::KEY_FOCUS_AREAS);
    if ((str != NULL)) {
        roi_type.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam,
                mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
                roi_type.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed");
            DBGT_EPILOG("");
            return (int)UNKNOWN_ERROR;
        }
        int maxAreasLessOne = 0;
        for (int i = 0; i < MAX_FOCUS_AREAS; i++) {
            sscanf(str, "(%d,%d,%d,%d,%d)",&roi_coordinate[0],&roi_coordinate[1],
                    &roi_coordinate[2],&roi_coordinate[3],&roi_coordinate[4] );

            DBGT_PTRACE("Coordinates for %d ROI recieved from Application are:",i);
            DBGT_PTRACE("%d %d %d %d %d",
                    roi_coordinate[0], roi_coordinate[1], roi_coordinate[2],
                    roi_coordinate[3],roi_coordinate[4] );
            if (!(roi_coordinate[0] == 0 &&
                        roi_coordinate[1] == 0 &&
                        roi_coordinate[2] == 0 &&
                        roi_coordinate[3] == 0 &&
                        roi_coordinate[4] == 0)) {
                if (roi_coordinate[0] < -1000 || roi_coordinate[0] >  1000 ||
                        roi_coordinate[1] < -1000 || roi_coordinate[1] >  1000 ||
                        roi_coordinate[2] < -1000 || roi_coordinate[2] >  1000 ||
                        roi_coordinate[3] < -1000 || roi_coordinate[3] >  1000 ||
                        roi_coordinate[4] < 1 || roi_coordinate[4] >  1000  ||
                        roi_coordinate[0] >= roi_coordinate[2]||
                        roi_coordinate[1] >= roi_coordinate[3]) {

                    DBGT_EPILOG("Return Error");
                    return OMX_ErrorUnsupportedSetting;
                }
            }
            //Range of area specified by user varies from -1000 to +1000
            int hRange = 2000;
            int vRange = 2000;
            float hScale = ( float ) 99 / ( float ) hRange;
            float vScale = ( float ) 99 / ( float ) vRange;

            if (roi_coordinate[2] > roi_coordinate[0]) {
                roi_coordinate[2] -= roi_coordinate[0];
            } else {
                roi_coordinate[2] = roi_coordinate[0] - roi_coordinate[2];
            }
            if (roi_coordinate[3] > roi_coordinate[1]) {
                roi_coordinate[3] -= roi_coordinate[1];
            } else {
                roi_coordinate[3] = roi_coordinate[1] - roi_coordinate[3];
            }
            roi_coordinate[0] = (((float)( roi_coordinate[0] + hRange / 2 ))*
                    hScale) + 1;
            roi_coordinate[1] = (((float)( roi_coordinate[1] + vRange / 2 ))*
                    vScale) + 1;
            roi_coordinate[2] = (((float)( roi_coordinate[2])) * hScale) + 1;
            roi_coordinate[3] = (((float)( roi_coordinate[3])) * vScale) + 1;

            DBGT_PTRACE("Coordinates for %d ROI sent to OMX are:",i);
            DBGT_PTRACE("%d %d %d %d %d",
                    roi_coordinate[0], roi_coordinate[1], roi_coordinate[2],
                    roi_coordinate[3],roi_coordinate[4] );
            roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nX = roi_coordinate[0];
            roi_type.ptr()->sROIs[i].sROI.sRect.sTopLeft.nY = roi_coordinate[1];
            roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nWidth = roi_coordinate[2];
            roi_type.ptr()->sROIs[i].sROI.sRect.sSize.nHeight = roi_coordinate[3];
            roi_type.ptr()->sROIs[i].nPriority = roi_coordinate[4];
            roi_type.ptr()->sROIs[i].eObjectType =
                (OMX_SYMBIAN_ROIOBJECTTYPE) OMX_STE_RoiObjectTypeTouch;
            err = OMX_SetConfig(mCam,
                    mOmxILExtIndex->getIndex(OmxILExtIndex::ERegionOI),
                    roi_type.ptr());
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("OMX_SetConfig failed");
                DBGT_EPILOG("");
                return (int)UNKNOWN_ERROR;
            }
            char *tempStr = strstr(str, ",(");
            if (tempStr) {
                str = tempStr+1;
                maxAreasLessOne ++;
            } else {
                break;
            }
        }
        if (maxAreasLessOne + 1 > MAX_FOCUS_AREAS) {
            DBGT_EPILOG("Return Error");
            return OMX_ErrorUnsupportedSetting;
        }
    }
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropColorEffect(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    if (!strcmp(mCurrentImageFilterMode,params.get(CameraParameters::KEY_EFFECT)))
    {
        // same mode as previous so no need to change
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    OMX_ERRORTYPE err = OMX_ErrorNone;
    imagefilterproperty g_imagefilterproperty[] = {
        {0, CameraParameters::EFFECT_NONE,       OMX_ImageFilterNone},
        {0, CameraParameters::EFFECT_MONO,       (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterGrayScale},
        {0, CameraParameters::EFFECT_NEGATIVE,    OMX_ImageFilterNegative},
        {0, CameraParameters::EFFECT_SOLARIZE,    OMX_ImageFilterSolarize},
        {0, CameraParameters::EFFECT_SEPIA,      (OMX_IMAGEFILTERTYPE)OMX_SYMBIAN_ImageFilterSepia},
        {0, CameraParameters::EFFECT_POSTERIZE,   (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterPosterize},
        {0, CameraParameters::EFFECT_WHITEBOARD,  (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterGrayscaleNegative},
        {0, CameraParameters::EFFECT_BLACKBOARD,  (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterFilm},
        {0, EFFECT_MONOTONE_NEGATIVE,  (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterGrayscaleNegative},
        {1, CameraParameters::EFFECT_AQUA,        (OMX_IMAGEFILTERTYPE)OMX_STE_ImageFilterWatercolor},
    };


    if (NULL != params.get(CameraParameters::KEY_EFFECT)) {
        int id = 0;
        int max_id = sizeof(g_imagefilterproperty) / sizeof(g_imagefilterproperty[0]);
        const char *imageFilter = params.get(CameraParameters::KEY_EFFECT);

        while ((id < max_id) &&
                 (strcmp(g_imagefilterproperty[id].imagefilter, imageFilter) != 0)) {
            id++;
        }
        if (id >= max_id) {
            DBGT_CRITICAL("Invalid Color Effect error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        OmxUtils::StructContainer<OMX_CONFIG_IMAGEFILTERTYPE> imageFilterConfig;

        imageFilterConfig.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonImageFilter), imageFilterConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        imageFilterConfig.ptr()->eImageFilter = g_imagefilterproperty[id].eImageFilter;
        DBGT_PTRACE("Set Colour effect %s",OmxUtils::name((OMX_IMAGEFILTERTYPE)imageFilterConfig.ptr()->eImageFilter));

        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonImageFilter), imageFilterConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        // If the previous colour effect is Posterize & now its changed then
        // the gamma setting needs to be set again corresponding to the current
        // scene mode.
        // Eg. if the current scene mode was Portrait, then the colour effect
        // was changed to Posterize. Since posterize effect modifies the gamma tuning
        // so when you change from posterize to some other effect, you need to retain
        // the gamma tuning corresponding to the present scene mode.

        if (!strcmp(mCurrentImageFilterMode,CameraParameters::EFFECT_POSTERIZE))
        {
            if (NULL != params.get(CameraParameters::KEY_SCENE_MODE)) {
                if (mSceneCtrl == NULL) {
                    mSceneCtrl = new SceneModeController(this);
                }
                if (mSceneCtrl->initialize() < 0) {
                    delete mSceneCtrl;
                    mSceneCtrl = NULL;
                    DBGT_EPILOG("OMX_ErrorBadParameter");
                    return OMX_ErrorBadParameter;
                }
                int iSceneMode = getSceneMode(params);
                if (iSceneMode < 0) {
                    DBGT_CRITICAL("Invalid Scene Mode iSceneMode = %d", iSceneMode);
                    DBGT_EPILOG("");
                    return iSceneMode;
                }

                err = mSceneCtrl->setGamma((SceneModeController::Mode)iSceneMode);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("Setting setGamma failed err = %d", err);
                    DBGT_EPILOG("");
                    return err;
                }
            }
        }

        //set current colour effect
        mParameters.set(CameraParameters::KEY_EFFECT,params.get(CameraParameters::KEY_EFFECT));
        strcpy(mCurrentImageFilterMode,params.get(CameraParameters::KEY_EFFECT));
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropAWBLock(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_LOCKTYPE> autoWhiteBalanceLockconfig;
    OMX_ERRORTYPE err = OMX_ErrorNone;
    autoWhiteBalanceLockconfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode

    if (!strcmp(params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK),"true")) {
        DBGT_PTRACE("Set up Auto White Balance Lock");
        autoWhiteBalanceLockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockImmediate;
    } else {
        DBGT_PTRACE("Removing White Balance Lock");
        autoWhiteBalanceLockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockOff;
    }

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_Symbian_IndexConfigWhiteBalanceLock, autoWhiteBalanceLockconfig.ptr() );
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig White Balance failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //set current white balance lock
    mParameters.set(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK,params.get(CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK));

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropAELock(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_LOCKTYPE> autoexposurelockconfig;
    OMX_ERRORTYPE err = OMX_ErrorNone;
    autoexposurelockconfig.ptr()->nPortIndex = OMX_ALL;  //only supported in video mode

    if (!strcmp(params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK),"true")) {
        DBGT_PTRACE("Set up Auto exposure Lock ");
        autoexposurelockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockImmediate;
    } else {
        DBGT_PTRACE("Removing Exposure Lock");
        autoexposurelockconfig.ptr()->eImageLock = OMX_SYMBIAN_LockOff;
    }

    err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureLock, autoexposurelockconfig.ptr() );
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_SetConfig Exposure Lock failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //set current exposure lock
    mParameters.set(CameraParameters::KEY_AUTO_EXPOSURE_LOCK,params.get(CameraParameters::KEY_AUTO_EXPOSURE_LOCK));

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doZoom(uint32_t aZoomKey)
{
    DBGT_PROLOG("aZoomKey: %d",aZoomKey);

    OMX_ERRORTYPE err = OMX_ErrorNone;
    // Zoom should be done from HAL in following use-cases,
    // 1. ZSL Still
    // 2. Video Recording supporting 2Mp Still During Record
    // rest the request should be sent to downstream
    // 1. Video Preview
    // 2. Video Recording not supporting 2Mp Still During record
    if (((EStill == mCamMode) && (!mIsStillZSL)) ||
        ((EVideo == mCamMode) &&
          (!mRecordRunning ||
          (mRecordRunning && !mEnableB2R2DuringRecord)))) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_ZOOMFACTORTYPE> zoomConfig;
        zoomConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EDigitalZoom), zoomConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        DBGT_PTRACE("do zoom for level =%d",aZoomKey);

        zoomConfig.ptr()->xZoomFactor.nValue = zoom_factor[aZoomKey];

#ifdef VIDEO_ZOOM_720P
        // In case of Video Mode 4x zoom is supported till 720p resolution only. So for higher resolutions set zoom
        // to 1x
        if (mRecordRunning) {
            if (mCamMode == EVideo && (mRecordInfo.getAlignedWidth() > 1280) && (mRecordInfo.getAlignedHeight() > 720)) {
                zoomConfig.ptr()->xZoomFactor.nValue = zoom_factor[0];
            }
        }
#endif // VIDEO_MODE_ZOOM
        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EDigitalZoom), zoomConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
    //save current zoom level
    mCurrentZoomLevel = aZoomKey;
    mParameters.set(CameraParameters::KEY_ZOOM,mCurrentZoomLevel);

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropZoom(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != params.get(CameraParameters::KEY_ZOOM)) {
        /* Returns if the application  request zoom > then
           max supported zoom */
        int zoomkey = params.getInt(CameraParameters::KEY_ZOOM);
        if (params.getInt(CameraParameters::KEY_MAX_ZOOM) < zoomkey) {
            DBGT_CRITICAL("not supported error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        if (mCurrentZoomLevel == zoomkey && (!mIsrecordsizechanged)) {
            // same mode as previous so no need to change
            DBGT_EPILOG("");
            return OMX_ErrorNone;
        }
        err = doZoom(zoomkey);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("doZoom failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::smoothZoomComplete()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    int32_t currentZoomLevel = mCurrentZoomLevel;
    DBGT_PTRACE("Requested smooth zoom level: %d Current zoom level: %d",
        mSmoothZoomLevel, currentZoomLevel);

    //Notify Application that smooth zoom is stopped.
    dispatchNotification(CAMERA_MSG_ZOOM, currentZoomLevel, true);

    // Reset the flag since smooth stop zoom is done.
    mSmoothZoomStopPending = false;

    // disable callback
    err = mOmxReqCbHandler.disable(OmxReqCbHandler::ESmoothZoomStatus);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Disable reqcbfailed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doSmoothZoomProcessing(void* aArg)
{
    DBGT_PROLOG("");
    Mutex::Autolock lock(mLock);
    OMX_ERRORTYPE err = OMX_ErrorNone;

    // Save current zoom level
    //mParameters.set(CameraParameters::KEY_ZOOM,mCurrentZoomLevel);

    //check callback was enabled
   // DBGT_ASSERT(mOmxReqCbHandler.isEnabled(OmxReqCbHandler::ESmoothZoomStatus),
   //     "Wrong event got, event: %d not enabled", OmxReqCbHandler::ESmoothZoomStatus);

    //stop not pending
    if (mSmoothZoomStopPending &&
            mOmxReqCbHandler.isEnabled(OmxReqCbHandler::ESmoothZoomStatus)) {
        int32_t currentZoomLevel = mCurrentZoomLevel;
        int32_t newZoomLevel = currentZoomLevel;

        //update current zoom level
        if (mSmoothZoomLevel > currentZoomLevel)
            newZoomLevel++;
        else if (mSmoothZoomLevel < currentZoomLevel)
            newZoomLevel--;

        //setup new zoom level
        if (currentZoomLevel != newZoomLevel) {
            err = doZoom(newZoomLevel);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("doZoom failed in doSmoothZoomProcessing err = %d", err);
                DBGT_EPILOG("");
                return err;
            }
        }

        //if expected zoom level reached
        if (currentZoomLevel == mSmoothZoomLevel) {
            err = smoothZoomComplete();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("smoothZoomComplete failed err = %d", err);
                DBGT_EPILOG("");
                return err;
            }
        }
        else
            dispatchNotification(CAMERA_MSG_ZOOM, currentZoomLevel, false);
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::startSmoothZoom(int32_t aZoomKey)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /* Returns if the application  request zoom > then
       max supported zoom */
    if (mParameters.getInt(CameraParameters::KEY_MAX_ZOOM) < aZoomKey) {
        DBGT_CRITICAL("not supported - OMX_ErrorBadParameter");
        DBGT_EPILOG("");
        return OMX_ErrorBadParameter;
    }

    int32_t currentZoomLevel = mCurrentZoomLevel;
    mSmoothZoomLevel = aZoomKey;

    DBGT_PTRACE("smoothzoom level =%d current zoom level =%d", mSmoothZoomLevel, currentZoomLevel);

    /* Returns if the application  request smooth zoom when smooth zoom
       is already in progress or equal to the current zoom level*/
    if ((currentZoomLevel == mSmoothZoomLevel && (!mIsrecordsizechanged))|| mSmoothZoomStopPending) {
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    err = updateSmoothZoom(OMX_TRUE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateSmoothZoom failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    // enable callback
    err = mOmxReqCbHandler.enable(OmxReqCbHandler::ESmoothZoomStatus);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Enable reqcbfailed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (mSmoothZoomLevel > currentZoomLevel) {
        currentZoomLevel++;
        err = doZoom(currentZoomLevel);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("doZoom failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    } else if (mSmoothZoomLevel < currentZoomLevel) {
        currentZoomLevel--;
        err = doZoom(currentZoomLevel);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("doZoom failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // Need to reset mSmoothZoomStopPending in case stop smooth zoom command
    // was called after start smooth zoom was complete.
    mSmoothZoomStopPending = true;
    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::stopSmoothZoom()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = updateSmoothZoom(OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateSmoothZoom failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (mSmoothZoomStopPending) {
        err = smoothZoomComplete();
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("smoothZoomComplete failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
    mSmoothZoomStopPending = false;

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::sendCommand(int32_t command, int32_t arg1,
                                int32_t arg2)
{
    DBGT_EPILOG("Command: %d Arg1: %d Arg2: %d", command, arg1, arg2);

    status_t status = NO_ERROR;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (command) {
#ifdef ENABLE_SMOOTH_ZOOM
        case CAMERA_CMD_START_SMOOTH_ZOOM:
            {
                DBGT_PTRACE("In SendCommand, got CAMERA_CMD_START_SMOOTH_ZOOM");

                Mutex::Autolock lock(mLock);

                err = startSmoothZoom(arg1);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("startSmoothZoom failed err - %d", OmxUtils::convertOmxErrorToStatus(err));
                    DBGT_EPILOG("");
                    return OmxUtils::convertOmxErrorToStatus(err);
                }
            }
            break;

        case CAMERA_CMD_STOP_SMOOTH_ZOOM:
            {
                DBGT_PTRACE("In SendCommand, got CAMERA_CMD_STOP_SMOOTH_ZOOM");

                Mutex::Autolock lock(mLock);

                err = stopSmoothZoom();
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("stopSmoothZoom failed err - %d", OmxUtils::convertOmxErrorToStatus(err));
                    DBGT_EPILOG("");
                    return OmxUtils::convertOmxErrorToStatus(err);
                }

            }
            break;

#endif //ENABLE_SMOOTH_ZOOM

#ifdef ENABLE_FACE_DETECTION
        case CAMERA_CMD_START_FACE_DETECTION:
            {
                DBGT_PTRACE("CAMERA_CMD_START_FACE_DETECTION received");
                if (arg1) {
                    DBGT_PTRACE("CAMERA_FACE_DETECTION_SW");
                } else {
                    DBGT_PTRACE("CAMERA_FACE_DETECTION_HW");
                }

                Mutex::Autolock lock(mLock);
                mFaceDetection = true;
            }
            break;

        case CAMERA_CMD_STOP_FACE_DETECTION:
            {
                DBGT_PTRACE("CAMERA_CMD_STOP_FACE_DETECTION received");
                Mutex::Autolock lock(mLock);
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

#ifdef ENABLE_CONTINUOUS_AUTOFOCUS
    case CAMERA_CMD_ENABLE_FOCUS_MOVE_MSG:
        if(arg1) {
           mMsgEnabled |= CAMERA_MSG_FOCUS_MOVE;
           mMsgEnabled &= ~CAMERA_MSG_FOCUS;
            err = mOmxReqCbHandler.enable(OmxReqCbHandler::EExtFocusStatus);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("Enable ExtFocusStatus reqcbfailed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
            mAutoFocusThreadData->mContinuousFocusState = ETrackingState;
        } else {
           mMsgEnabled &= ~CAMERA_MSG_FOCUS_MOVE;
            err = mOmxReqCbHandler.disable(OmxReqCbHandler::EExtFocusStatus);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("disable ExtFocusStatus reqcbfailed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
            mAutoFocusThreadData->mContinuousFocusState = EStopped;
        }
       break;
#endif
    default:
        status = BAD_VALUE;
        break;
    }

    DBGT_EPILOG("");
    return status;
}

int STECamera::SetPropWhiteBalance(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

#ifdef ENABLE_FACE_DETECTION
    if (isFaceDetectionEnable()) {
        DBGT_PTRACE("Face detection enabled...set WB is not allowed");
        DBGT_EPILOG("OMX_ErrorNone");
        return OMX_ErrorNone;
    }
#endif

    if (!strcmp(mCurrentWbMode,params.get(CameraParameters::KEY_WHITE_BALANCE)))
    {
        // same mode as previous so no need to change
        DBGT_EPILOG("OMX_ErrorNone");
        return OMX_ErrorNone;
    }

    wbproperty g_wbprop[] = {
        {0, CameraParameters::WHITE_BALANCE_AUTO, OMX_WhiteBalControlAuto},
        {0, CameraParameters::WHITE_BALANCE_INCANDESCENT, OMX_WhiteBalControlIncandescent},
        {0, CameraParameters::WHITE_BALANCE_FLUORESCENT, OMX_WhiteBalControlFluorescent},
        {0, CameraParameters::WHITE_BALANCE_DAYLIGHT, OMX_WhiteBalControlSunLight},
        {1, CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT, OMX_WhiteBalControlCloudy},
    };

    int i = 0, value;

    if (NULL != params.get(CameraParameters::KEY_WHITE_BALANCE)) {
        const char *wb = params.get(CameraParameters::KEY_WHITE_BALANCE);

        do {
            value = strcmp(g_wbprop[i].wb, wb);

            if (!value)
                break;
            else
                i++;
        } while (g_wbprop[i - 1].iMarkendofdata != 1);

        if (value != 0) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        }

        OmxUtils::StructContainer<OMX_CONFIG_WHITEBALCONTROLTYPE> wbConfig;
        wbConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        wbConfig.ptr()->eWhiteBalControl = g_wbprop[i].eWBType;
        DBGT_PTRACE("Set whitebalance %s",OmxUtils::name((OMX_WHITEBALCONTROLTYPE)wbConfig.ptr()->eWhiteBalControl));

        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

       //set current white balance mode
       mParameters.set(CameraParameters::KEY_WHITE_BALANCE,params.get(CameraParameters::KEY_WHITE_BALANCE));
       strcpy(mCurrentWbMode,params.get(CameraParameters::KEY_WHITE_BALANCE));
    }

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::SetPropFocus(const CameraParameters &params, OMX_HANDLETYPE mCam,  bool aSetControl /*=false*/)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    const char *focus = params.get(CameraParameters::KEY_FOCUS_MODE);

    if (NULL != focus) {
        int i = 0, value;

        if (!strcmp(mCurrentFocusMode, focus))
        {
            // same mode as previous so no need to change
            DBGT_EPILOG("");
            return OMX_ErrorNone;
        }

        /* Disable CAMERA_MSG_FOCUS_MOVE if it is enable
         * This message will be enable by application
         * via sendcommand
         */
        if (mMsgEnabled & CAMERA_MSG_FOCUS_MOVE) {
            mMsgEnabled &= ~CAMERA_MSG_FOCUS_MOVE;
            err = mOmxReqCbHandler.disable(OmxReqCbHandler::EExtFocusStatus);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("Enable ExtFocusStatus reqcbfailed");
                DBGT_EPILOG("");
                return UNKNOWN_ERROR;
            }
            /* This notification just in case application
             * waiting for callback for this message
             */
            dispatchNotification(CAMERA_MSG_FOCUS_MOVE, 1, 0);
        }

        if ((strcmp(focus, CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO) == 0)
                ||(strcmp(focus, CameraParameters::FOCUS_MODE_CONTINUOUS_PICTURE) == 0)) {
            mFocusHandler = mContinuousAutoFocusHandler;
        } else if (strcmp(focus, CameraParameters::FOCUS_MODE_FIXED) == 0) {
            mFocusHandler = mFixedFocusHandler;
        } else {
            mFocusHandler = mAutoFocusHandler;
        }

        err = mFocusHandler->setUpFocus(focus);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("setUpFocus failed error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        //set current focus mode
        mParameters.set(CameraParameters::KEY_FOCUS_MODE,params.get(CameraParameters::KEY_FOCUS_MODE));
        strcpy(mCurrentFocusMode, focus);
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropFlash(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    flashproperty g_flashprop[] = {
        {0, CameraParameters::FLASH_MODE_OFF, OMX_IMAGE_FlashControlOff},
        {0, CameraParameters::FLASH_MODE_ON, OMX_IMAGE_FlashControlOn},
        {0, CameraParameters::FLASH_MODE_RED_EYE, OMX_IMAGE_FlashControlRedEyeReduction},
        {0, CameraParameters::FLASH_MODE_TORCH, OMX_IMAGE_FlashControlTorch},
        {1, CameraParameters::FLASH_MODE_AUTO, OMX_IMAGE_FlashControlAuto},
    };

    if (NULL != params.get(CameraParameters::KEY_FLASH_MODE)) {
        int i = 0, value;
        const char *flash = params.get(CameraParameters::KEY_FLASH_MODE);

        do {
            value = strcmp(g_flashprop[i].flash, flash);

            if (!value)
                break;
            else
                i++;
        } while (g_flashprop[i - 1].iMarkendofdata != 1);

        if (value != 0) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE> flashConfig;
        flashConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl)/*OMX_IndexConfigFlashControl*/, flashConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        flashConfig.ptr()->eFlashControl = g_flashprop[i].eflashType;
        DBGT_PTRACE("Set Flash mode %s",OmxUtils::name((OMX_IMAGE_FLASHCONTROLTYPE)flashConfig.ptr()->eFlashControl));

        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl), &flashConfig);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current flash mode
        mParameters.set(CameraParameters::KEY_FLASH_MODE,params.get(CameraParameters::KEY_FLASH_MODE));
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::SetPropFlickerRemoval(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    flickerproperty g_flickerprop[] = {
        {0, CameraParameters::ANTIBANDING_OFF, OMX_SYMBIAN_FlickerRemovalOff},
        {0, CameraParameters::ANTIBANDING_AUTO, OMX_SYMBIAN_FlickerRemovalAuto},
        {0, CameraParameters::ANTIBANDING_50HZ, OMX_SYMBIAN_FlickerRemoval50},
        {1, CameraParameters::ANTIBANDING_60HZ, OMX_SYMBIAN_FlickerRemoval60},
    };

    if (NULL != params.get(CameraParameters::KEY_ANTIBANDING)) {
        int i = 0, value;
        const char *flicker = params.get(CameraParameters::KEY_ANTIBANDING);

        do {
            value = strcmp(g_flickerprop[i].flicker, flicker);

            if (!value)
                break;
            else
                i++;
        } while (g_flickerprop[i - 1].iMarkendofdata != 1);

        if (value != 0) {
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        }

        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE> flickerConfig;
        flickerConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFlickerRemoval), flickerConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        if ( 0 == strcmp(g_flickerprop[1].flicker, flicker)) {
            flickerConfig.ptr()->eFlickerRemoval = cameraGetLocation();
            DBGT_PTRACE("Auto flickering selected & flicker mode=%d\n",
                    (OMX_SYMBIAN_FLICKERREMOVALTYPE)(flickerConfig.ptr()->eFlickerRemoval));
        } else {
            flickerConfig.ptr()->eFlickerRemoval = g_flickerprop[i].eflickerType;
            DBGT_PTRACE("Manual flickering selected & flicker mode=%d\n",
                    (OMX_SYMBIAN_FLICKERREMOVALTYPE)(flickerConfig.ptr()->eFlickerRemoval));
        }

        DBGT_PTRACE("Set flicker mode %s",OmxUtils::name((OMX_SYMBIAN_FLICKERREMOVALTYPE)flickerConfig.ptr()->eFlickerRemoval));
        err = OMX_SetConfig(mCam, mOmxILExtIndex->getIndex(OmxILExtIndex::EFlickerRemoval), &flickerConfig);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        //set current flickering value
        mParameters.set(CameraParameters::KEY_ANTIBANDING,params.get(CameraParameters::KEY_ANTIBANDING));
    }

    DBGT_EPILOG("");
    return err;
}

int STECamera::getSceneMode(const CameraParameters &params)
{
    DBGT_PROLOG("");

    const char *scenemode = params.get(CameraParameters::KEY_SCENE_MODE);
    DBGT_PTRACE("Scene Mode selected =%s",scenemode);
    detailscenemodeproperty sceneModeProp[]= {
        {0, CameraParameters::SCENE_MODE_AUTO, SceneModeController::EAuto},
        //{0, CameraParameters::SCENE_MODE_BEACH, SceneModeController::EBeach},
        {0, CameraParameters::SCENE_MODE_SNOW, SceneModeController::ESnow},
        {0, CameraParameters::SCENE_MODE_PARTY, SceneModeController::EParty},
        {0, CameraParameters::SCENE_MODE_PORTRAIT, SceneModeController::EPortrait},
        {0, CameraParameters::SCENE_MODE_NIGHT_PORTRAIT, SceneModeController::ENightPortrait},
        {0, CameraParameters::SCENE_MODE_LANDSCAPE, SceneModeController::ELandscape},
        {0, CameraParameters::SCENE_MODE_NIGHT, SceneModeController::ENight},
        //{0, CameraParameters::SCENE_MODE_ACTION, SceneModeController::EAction},
        //{0, CameraParameters::SCENE_MODE_SUNSET, SceneModeController::ESunset},
        //{0, CameraParameters::SCENE_MODE_CANDLELIGHT, SceneModeController::ECandlelight},
        //{0, SCENE_MODE_BACKLIGHT, SceneModeController::EBacklight},
        //{0, SCENE_MODE_CLOSEUP, SceneModeController::ECloseup},
        {0, CameraParameters::SCENE_MODE_SPORTS, SceneModeController::ESports},
        {1, SCENE_MODE_DOCUMENT, SceneModeController::EDocument},
    };

    int i = 0,value = 0;
    do {
        value = strcmp(sceneModeProp[i].scenemode, scenemode);
        if (!value)
            break;
        else
            i++;
    } while (sceneModeProp[i - 1].iMarkendofdata != 1);

    DBGT_PTRACE("Scene Mode to be set =%s",sceneModeProp[i].scenemode);
    if (value != 0) {
        DBGT_CRITICAL("Invalid Scene Mode");
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");
    return sceneModeProp[i].iSceneMode;
}
int STECamera::SetPropSceneModes(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (!strcmp(mCurrentSceneMode,params.get(CameraParameters::KEY_SCENE_MODE)))
    {
        // same mode as previous so no need to change
        DBGT_EPILOG("OMX_ErrorNone");
        return OMX_ErrorNone;
    }

    if (NULL != params.get(CameraParameters::KEY_SCENE_MODE)) {
        if (mSceneCtrl == NULL) {
            mSceneCtrl = new SceneModeController(this);
        }
        if (mSceneCtrl->initialize() < 0) {
            delete mSceneCtrl;
            mSceneCtrl = NULL;
            DBGT_EPILOG("OMX_ErrorBadParameter");
            return OMX_ErrorBadParameter;
        }
        int iSceneMode = getSceneMode(params);
        if (iSceneMode < 0) {
            DBGT_CRITICAL("Invalid Scene Mode iSceneMode - %d", iSceneMode);
            DBGT_EPILOG("");
            return iSceneMode;
        }

        err = mSceneCtrl->setSceneMode((SceneModeController::Mode)iSceneMode);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Setting scenemode failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

    }
    //save current scene mode
    mParameters.set(CameraParameters::KEY_SCENE_MODE,params.get(CameraParameters::KEY_SCENE_MODE));
    strcpy(mCurrentSceneMode,params.get(CameraParameters::KEY_SCENE_MODE));
    DBGT_EPILOG("");
    return err;
}

int STECamera::SetExposureCompensation(const CameraParameters &params, OMX_HANDLETYPE mCam)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    // check that the keys should not be NULL first & then compare them with the current mode set
    if (!(((params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION)) &&
            strcmp(mCurrentEVCompMode,params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION))) ||
            ((params.get(KEY_METERING_EXPOSURE)) &&
            (strcmp(mCurrentMeteringMode,params.get(KEY_METERING_EXPOSURE)))) ||
            ((params.get(KEY_ISO)) &&
            (strcmp(mCurrentExpSensitivityMode,params.get(KEY_ISO)))))) {
        // same mode as previous so no need to change
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    }

    evcompensationproperty g_evprop[] = {
        {(float) - 2, -131072},
        {(float) - 5 / 3, -109226},
        {(float) - 4 / 3, -87381},
        { -1, -65536},
        {(float) - 2 / 3, -43690},
        {(float) - 1 / 3, -21845},
        {0, 0},
        {(float)1 / 3, 21845},
        {(float)2 / 3, 43690},
        {1, 65536},
        {(float)4 / 3, 87381},
        {(float)5 / 3, 109226},
        {2, 131072},
    };
    meteringProperty g_metering[] = {
        {0, METERING_EXPOSURE_AVERAGE,  OMX_MeteringModeAverage},
        {0, METERING_EXPOSURE_SPOT,     OMX_MeteringModeSpot},
        {0, METERING_EXPOSURE_MATRIX,   OMX_MeteringModeMatrix},
        {1, METERING_EXPOSURE_CENTRE_WEIGHTED,   (OMX_METERINGTYPE)OMX_STE_MeteringModeCenterWeighted},
    };

    //  if (NULL != params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION))
    {
        OmxUtils::StructContainer<OMX_CONFIG_EXPOSUREVALUETYPE> ExposureValue;
#ifdef PROP_EXPOSURE_COMP
        float value = 0;

        int keyExp = atoi(params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION));
        int keyExpMin = atoi(params.get(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION));
        int keyExpMax = atoi(params.get(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION));

        if (!((keyExpMin <= keyExp) && (keyExp <= keyExpMax))) {
            DBGT_CRITICAL("Invalid Exposure key");
            DBGT_EPILOG("UNKNOWN_ERROR");
            return UNKNOWN_ERROR;
        }

        float step = atof(params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP));
        value = step * keyExp;

        DBGT_PTRACE("Exposure Value: %f", value);

        ExposureValue.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), ExposureValue.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        uint32_t numExpValue = ARRAYCOUNT(g_evprop);
        DBGT_PTRACE("Number of numExpValue: %u", numExpValue);

        unsigned int i;
        for (i = 0; i < numExpValue; i++) {
            if (Comparef(value,g_evprop[i].stepindex)) {
                ExposureValue.ptr()->xEVCompensation = g_evprop[i].compensation_value;
                break;
            }
        }

        //Check if no match is found.
        if (i == numExpValue) {
            DBGT_CRITICAL("Exposure compensation value not found error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }

        ExposureValue.ptr()->bAutoAperture = OMX_TRUE;
        ExposureValue.ptr()->bAutoShutterSpeed = OMX_TRUE;

        //set current exposure compensation
        mParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION,
                params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION));
        strcpy(mCurrentEVCompMode,params.get(CameraParameters::KEY_EXPOSURE_COMPENSATION));
#else
        ExposureValue.ptr()->xEVCompensation = 0;

#endif //PROP_EXPOSURE_COMP

#if PROP_MET

        //Apply Metering mode only in case of Scene Mode Auto
        if (params.get(CameraParameters::KEY_SCENE_MODE) &&
                (!strcmp(CameraParameters::SCENE_MODE_AUTO,params.get(CameraParameters::KEY_SCENE_MODE)))) {
            if (NULL != params.get(KEY_METERING_EXPOSURE)) {
                int id = 0;
                int max_id = sizeof(g_metering) / sizeof(g_metering[0]);
                const char *metering = params.get(KEY_METERING_EXPOSURE);

                while (( id < max_id) && (strcmp(g_metering[id].metering, metering) != 0)) {
                    id++;
                }

                if (id < max_id) {
                    ExposureValue.ptr()->eMetering = g_metering[id].eMetering;
                    //set current metering exposure
                    mParameters.set(KEY_METERING_EXPOSURE,params.get(KEY_METERING_EXPOSURE));
                    strcpy(mCurrentMeteringMode,params.get(KEY_METERING_EXPOSURE));
                    DBGT_PTRACE("Set Metering %s",OmxUtils::name((OMX_METERINGTYPE)ExposureValue.ptr()->eMetering));

                } else {
                    //Do not Modify Metering in case of Wrong Metering.
                    DBGT_PTRACE("Wrong Metering Value %s ",params.get(KEY_METERING_EXPOSURE));
                }
            } else {
                ExposureValue.ptr()->eMetering = OMX_MeteringModeAverage;
                //set current metering exposure
                mParameters.set(KEY_METERING_EXPOSURE,params.get(KEY_METERING_EXPOSURE));
                strcpy(mCurrentMeteringMode,METERING_EXPOSURE_AVERAGE);
            }
        } else {
            //To prevent Call to SetExposureCompensation in case of non Auto Scene Mode
            if (params.get(KEY_METERING_EXPOSURE))
                strcpy(mCurrentMeteringMode,params.get(KEY_METERING_EXPOSURE));
        }

#endif //PROP_MET

#if PROP_ISO

        if (NULL != params.get(KEY_ISO)) {
            const char *iso = params.get(KEY_ISO);

            if ( strcmp(ISO_AUTO, iso) == 0 ) {
                ExposureValue.ptr()->bAutoSensitivity = (OMX_BOOL)true;
            } else {
                ExposureValue.ptr()->bAutoSensitivity = (OMX_BOOL)false;
                ExposureValue.ptr()->nSensitivity = (OMX_U32)params.getInt(KEY_ISO);
                DBGT_PTRACE("Sensitivity is %ld",ExposureValue.ptr()->nSensitivity);
            }
        } else {
            ExposureValue.ptr()->bAutoSensitivity = OMX_TRUE;
        }
        //set current ISO value
        mParameters.set(KEY_ISO,params.get(KEY_ISO));
        strcpy(mCurrentExpSensitivityMode,params.get(KEY_ISO));

#endif //PROP_ISO

#if PROP_EXPOSURE_COMP
        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), &ExposureValue);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
        OmxUtils::StructContainer<OMX_CONFIG_EXPOSURECONTROLTYPE> exposureControl;
        exposureControl.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), exposureControl.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
        exposureControl.ptr()->eExposureControl = OMX_ExposureControlAuto;
        err = OMX_SetConfig(mCam, (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), exposureControl.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
#endif //PROP_EXPOSURE_COMP

    }
    DBGT_EPILOG("");
    return err;
}

void STECamera::initPreviewOmxColorFormat(const char *const aPreviewFormat)
{
    DBGT_PROLOG("Preview format: %s", aPreviewFormat);

    DBGT_ASSERT(NULL != aPreviewFormat, "Preview format is NULL");

    uint32_t numFormats = ZARRAYCOUNT(g_CameraFormatInfo);

    DBGT_ASSERT(0 != numFormats, "No format supported");

    DBGT_PTRACE("Number of formats: %u", numFormats);

    uint32_t loop;

    for (loop = 0; loop < numFormats; loop ++) {

        //check format good
        DBGT_ASSERT(NULL != g_CameraFormatInfo[loop].mStr,
                   "Format is NULL at index: %u", loop);

        DBGT_PTRACE("Index: %u Format: %s", loop, g_CameraFormatInfo[loop].mStr);

        if (!strcmp(aPreviewFormat, g_CameraFormatInfo[loop].mStr)) {
            mCameraFormatInfoIndex = loop;

            //check for 0
            DBGT_ASSERT(0 != cameraFormatInfo().mPreviewStrideFactor, "mPreviewStrideFactor is zero");

            DBGT_PTRACE("Matching format found at Index: %u PreviewOmxColor: %s RecordOmxColor: %s Stride Factor: %u",
                 loop,
                 OmxUtils::name(cameraFormatInfo().mPreviewOmxColorFormat),
                 OmxUtils::name(cameraFormatInfo().mRecordOmxColorFormat),
                 cameraFormatInfo().mPreviewStrideFactor);
            break;
        }
    }

    DBGT_ASSERT(loop < numFormats,
               "No matching omx color format found for Preview format: %s",
               aPreviewFormat);

    DBGT_EPILOG("");
    return;
}

bool STECamera::checkPreviewFormat(const char *const aPreviewFormat)
{
    DBGT_PROLOG("Preview format: %s", aPreviewFormat);

    DBGT_ASSERT(NULL != aPreviewFormat, "Preview format is NULL");

    uint32_t numFormats = ZARRAYCOUNT(g_CameraFormatInfo);

    DBGT_ASSERT(0 != numFormats, "No format supported");

    DBGT_PTRACE("Number of formats: %u", numFormats);

    for (uint32_t loop = 0; loop < numFormats; loop ++) {

        //check format good
        DBGT_ASSERT(NULL != g_CameraFormatInfo[loop].mStr,
                   "Format is NULL at index: %u", loop);

        DBGT_PTRACE("Index: %u Format: %s", loop, g_CameraFormatInfo[loop].mStr);

        if (!strcmp(aPreviewFormat, g_CameraFormatInfo[loop].mStr)) {
            DBGT_PTRACE("Matching format found");
            DBGT_EPILOG("true");
            return true;
        }
    }

    DBGT_EPILOG("false");
    return false;
}

#define CAM_PROP_DELIMIT ","

void STECamera::Append_WhiteBalance(String8 &wb, String8 &wb_set)
{
    wb.append(CameraParameters::WHITE_BALANCE_AUTO);
    wb_set = wb;
    wb.append(CAM_PROP_DELIMIT);
    wb.append(CameraParameters::WHITE_BALANCE_INCANDESCENT);
    wb.append(CAM_PROP_DELIMIT);
    wb.append(CameraParameters::WHITE_BALANCE_FLUORESCENT);
    wb.append(CAM_PROP_DELIMIT);
    wb.append(CameraParameters::WHITE_BALANCE_DAYLIGHT);
    wb.append(CAM_PROP_DELIMIT);
    wb.append(CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT);
}

void STECamera::Append_SceneMode(String8 &scene_mode, String8 &scene_mode_set)
{
    scene_mode.append(CameraParameters::SCENE_MODE_AUTO);
    scene_mode_set = scene_mode;
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_PARTY);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_PORTRAIT);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_NIGHT_PORTRAIT);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_LANDSCAPE);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_NIGHT);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_SPORTS);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(CameraParameters::SCENE_MODE_BEACH);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(CameraParameters::SCENE_MODE_SNOW);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(CameraParameters::SCENE_MODE_ACTION);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(CameraParameters::SCENE_MODE_SUNSET);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(CameraParameters::SCENE_MODE_CANDLELIGHT);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(SCENE_MODE_BACKLIGHT);
    //scene_mode.append(CAM_PROP_DELIMIT);
    //scene_mode.append(SCENE_MODE_CLOSEUP);
    scene_mode.append(CAM_PROP_DELIMIT);
    scene_mode.append(SCENE_MODE_DOCUMENT);
}

void STECamera::Append_FlashMode(String8 &flash_mode, String8 &flash_mode_set)
{
    flash_mode.append(CameraParameters::FLASH_MODE_OFF);
    flash_mode_set = flash_mode;
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_ON);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_RED_EYE);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_TORCH);
    flash_mode.append(CAM_PROP_DELIMIT);
    flash_mode.append(CameraParameters::FLASH_MODE_AUTO);
}

void STECamera::Append_FocusMode(String8 &focus_mode, String8 &focus_mode_set)
{
    focus_mode.append(CameraParameters::FOCUS_MODE_AUTO);
    focus_mode_set = focus_mode;
    focus_mode.append(CAM_PROP_DELIMIT);
    focus_mode.append(CameraParameters::FOCUS_MODE_MACRO);
    focus_mode.append(CAM_PROP_DELIMIT);
    focus_mode.append(CameraParameters::FOCUS_MODE_INFINITY);
    focus_mode.append(CAM_PROP_DELIMIT);
    focus_mode.append(CameraParameters::FOCUS_MODE_FIXED);
    focus_mode.append(CAM_PROP_DELIMIT);
    focus_mode.append(CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO);
    focus_mode.append(CAM_PROP_DELIMIT);
    focus_mode.append(CameraParameters::FOCUS_MODE_CONTINUOUS_PICTURE);
}

void STECamera::Append_FlickerRemovalMode(String8 &flicker_mode, String8 &flicker_mode_set)
{
    flicker_mode.append(CameraParameters::ANTIBANDING_50HZ);
    flicker_mode_set = flicker_mode;
    flicker_mode.append(CAM_PROP_DELIMIT);
    flicker_mode.append(CameraParameters::ANTIBANDING_AUTO);
    flicker_mode.append(CAM_PROP_DELIMIT);
    flicker_mode.append(CameraParameters::ANTIBANDING_OFF);
    flicker_mode.append(CAM_PROP_DELIMIT);
    flicker_mode.append(CameraParameters::ANTIBANDING_60HZ);
}

void STECamera::Append_PreviewFormats(String8 &spf)
{
    DBGT_PROLOG("");
    uint32_t numFormats = ZARRAYCOUNT(g_CameraFormatInfo);

    DBGT_ASSERT(0 != numFormats, "No format supported");

    DBGT_PTRACE("Number of formats: %u", numFormats);

    for (uint32_t loop = 0; loop < numFormats; loop ++) {

        //check format good
        DBGT_ASSERT(NULL != g_CameraFormatInfo[loop].mStr,
                   "Format is NULL at index: %u", loop);

        DBGT_PTRACE("Index: %u Format: %s", loop, g_CameraFormatInfo[loop].mStr);

        spf.append(g_CameraFormatInfo[loop].mStr);

        //dont append on last index
        if (loop < (numFormats - 1)) {
            spf.append(CAM_PROP_DELIMIT);
        }
    }

    DBGT_PTRACE("Supported formats: %s", spf.string());
    DBGT_EPILOG("");
}

void STECamera::Append_ColorEffect(String8 &effect, String8 &effect_set)
{
    effect.append(CameraParameters::EFFECT_NONE );
    effect_set = effect;
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_MONO);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_NEGATIVE);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_SOLARIZE);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_SEPIA);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_POSTERIZE);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_WHITEBOARD);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_BLACKBOARD);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(CameraParameters::EFFECT_AQUA);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(EFFECT_MONOTONE_NEGATIVE);
}

void STECamera::Append_MeteringMode(String8 &effect, String8 &effect_set)
{
    effect.append(METERING_EXPOSURE_AVERAGE);
    effect_set = effect;
    effect.append(CAM_PROP_DELIMIT);
    effect.append(METERING_EXPOSURE_SPOT);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(METERING_EXPOSURE_MATRIX);
    effect.append(CAM_PROP_DELIMIT);
    effect.append(METERING_EXPOSURE_CENTRE_WEIGHTED);
}

void STECamera::Append_RegionControl(String8 &aRegion, String8 &aRegion_set)
{
    aRegion.append(REGION_CONTROL_OFF);
    aRegion_set = aRegion;
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_CENTER);
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_MULTI);
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_USER);
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_TOUCH);
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_OBJECT);
    aRegion.append(CAM_PROP_DELIMIT);
    aRegion.append(REGION_FACE);
}

void STECamera::Append_Val(const String8 &aKey,const String8 &val)
{
    String8 temp;
    const char* oldval = mParameters.get(aKey);
    temp.append(oldval);
    temp.append(CAM_PROP_DELIMIT);
    temp.append(val);
    mParameters.set(aKey,temp.string());
}

void STECamera::resetProperties()
{
    DBGT_PROLOG("");

    mCurrentZoomLevel = -1;
    mCurrentSceneMode[0] = '\0';
    mCurrentWbMode[0] = '\0';
    mCurrentMeteringMode[0] = '\0';
    mCurrentExpSensitivityMode[0] = '\0';
    mCurrentEVCompMode[0] = '\0';
    mCurrentImageFilterMode[0] = '\0';
    mCurrentSaturationMode[0] = '\0';
    mCurrentSharpnessMode[0] = '\0';
    mCurrentRegControl[0] = '\0';
    mCurrentRegCoordinate[0] = '\0';
    mCurrentRegCoordinateNum = 0;
    mCurrentFocusMode[0] = '\0';
    DBGT_EPILOG("");
}

status_t STECamera::setupRawSwConProcessing(const char* const aFormat, int aWidth, int aHeight)
{
     DBGT_PROLOG("Format: %s Width: %d Height: %d", aFormat, aWidth, aHeight);

    status_t rc = NO_ERROR;

#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
    //check if format requires any sw processing
    if ((strcmp(aFormat, CameraParameters::PIXEL_FORMAT_RGB565) == 0) ||
            (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV422I) == 0) ) {
        DBGT_EPILOG("SW Conversion not required rc - %d", rc);
        return rc;
    }

    //Possible HW formats are YUV422I and RGB565, both 16bpp
    uint32_t data16bpp = aWidth * aHeight * 2;
    uint32_t yuv420Data = (aWidth * aHeight * 3) / 2;

    swRoutines conv(NULL);

    //setup swconversion routine
#if SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565
    if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        conv = CamSwRoutines::RGB565_to_YUV420SP_NV21;
    else if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        conv = CamSwRoutines::RGB565_to_YUV420P;
    else
        DBGT_CRITICAL("Cant Happen");
#else //!OMX_COLOR_Format16bitRGB565
    if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        conv = CamSwRoutines::UYVY422Itld_to_YUV420SP_NV21;
    else if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        conv = CamSwRoutines::UYVY422Itld_to_YUV420P;
    else
        DBGT_CRITICAL("Cant Happen");
#endif //OMX_COLOR_Format16bitRGB565

    //alloc sw buffer for raw buffer only if HiResVF is not enabled
    if (!mHiResVFEnabled) {
        //get cuurent offset
        ssize_t offset;
        size_t size;
        mRawOmxBuffInfo.mCamMemoryBase->getMemory(&offset, &size);

        DBGT_PTRACE("Offset: %ld Size: %zu", offset, size);

        //new offset and size adjusted for yuv420 data
        rc = mRawOmxBuffInfo.mProcessingBuffer->realloc(
            mRawHeap,
            offset + (data16bpp - yuv420Data),
            yuv420Data);
        if (NO_ERROR != rc) {
            DBGT_CRITICAL("realloc failed rc = %d", rc);
            DBGT_EPILOG("");
            return rc;
        }

        //set conversion routine
        mRawOmxBuffInfo.mSwConversion = conv;
    }

#endif //ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
    DBGT_EPILOG("");
    return rc;
}

status_t STECamera::setupPreviewSwConProcessing(const char* const aFormat)
{
    DBGT_PROLOG("Format: %s", aFormat);

    status_t rc = NO_ERROR;

#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS

    //check if format requires any sw processing
    if ((strcmp(aFormat, CameraParameters::PIXEL_FORMAT_RGB565) == 0) ||
            (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV422I) == 0) ) {
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            //set conversion routine
            mPreviewOmxBuffInfo[i].mSwConversion = NULL;
        }
        DBGT_EPILOG("SW Conversion not required rc - %d", rc);
        return rc;
    }

    swRoutines conv(NULL);

    //setup swconversion routine
#if SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565
    if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        conv = CamSwRoutines::RGB565_to_YUV420SP_NV21;
    else if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        conv = CamSwRoutines::RGB565_to_YUV420P;
    else
        DBGT_CRITICAL("Cant Happen");
#else //!OMX_COLOR_Format16bitRGB565
    if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420SP) == 0)
        conv = CamSwRoutines::UYVY422Itld_to_YUV420SP_NV21;
    else if (strcmp(aFormat, CameraParameters::PIXEL_FORMAT_YUV420P) == 0)
        conv = CamSwRoutines::UYVY422Itld_to_YUV420P;
    else
        DBGT_CRITICAL("Cant Happen");
#endif //OMX_COLOR_Format16bitRGB565

    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
        //set conversion routine
        mPreviewOmxBuffInfo[i].mSwConversion = conv;
    }

#endif //ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS

    DBGT_EPILOG("");
    return rc;
}

status_t STECamera::setupPreviewSwBuffers(int aWidth, int aHeight)
{
    DBGT_PROLOG("Width: %d Height: %d", aWidth, aHeight);

    status_t rc = NO_ERROR;

#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
    //Possible HW formats are YUV422I and RGB565, both 16bpp
    uint32_t data16bpp = aWidth * aHeight * 2;
    uint32_t yuv420Data = (aWidth * aHeight * 3) / 2;

    /* Temp buffer required in two cases
     * 1) Overlays disabled
     * 2) Video rotation is in Landscape mode
     */
#if !defined(ENABLE_OVERLAYS) || (ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE)
    //clear previous allocated buffer
    mPreviewTempBuffer.clear();
    //allocate temp buffer for 16bpp data memcpy
    mPreviewTempBuffer = new MemoryHeapBase(data16bpp);

    //check memory allocated
    if (mPreviewTempBuffer == 0) {
        DBGT_EPILOG("NO_MEMORY");
        return NO_MEMORY;
    }
#endif //!ENABLE_OVERLAYS || (ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE)

    if (!mHiResVFEnabled) {
        //allocate sw buffers corresponding to all preview hw buffers
        for (int i = 0; i < kTotalPreviewBuffCount; i++) {
            //get cuurent offset
            ssize_t offset;
            size_t size;
            mPreviewOmxBuffInfo[i].mCamMemoryBase->getMemory(&offset, &size);

            DBGT_PTRACE("Offset: %ld Size: %zu", offset, size);

            //new offset and size adjusted for yuv420 data
            rc = mPreviewOmxBuffInfo[i].mProcessingBuffer->realloc(
                    mPreviewHeap[i],
                    offset + (data16bpp - yuv420Data),
                    yuv420Data);
            if (NO_ERROR != rc) {
                DBGT_CRITICAL("realloc failed rc = %d", rc);
                DBGT_EPILOG("");
                return rc;
            }
        }
    }

#endif //ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
    DBGT_EPILOG("");
    return rc;
}

void STECamera::setupPreviewSwRotProcessing(int aRotation)
{
    DBGT_PROLOG("Rotation: %d", aRotation);

#if (ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE)

    swRoutines conv(NULL);

    //check if rotation requires any sw processing
    if ((0 == aRotation) || (180 == aRotation)) {
        conv = NULL;
    } else if (aRotation == 90) {
#if (SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565)
        DBGT_CRITICAL("Cant Happen");
#else //!OMX_COLOR_Format16bitRGB565
        conv = CamSwRoutines::UYVY422Itld_270_rotation_inplace;
#endif //OMX_COLOR_Format16bitRGB565
    } else if (270 == aRotation) {
#if (SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565)
        DBGT_CRITICAL("Cant Happen");
#else //!OMX_COLOR_Format16bitRGB565
        conv = CamSwRoutines::UYVY422Itld_90_rotation_inplace;
#endif //OMX_COLOR_Format16bitRGB565
    } else
    DBGT_CRITICAL("Cant Happen");

    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
        mPreviewOmxBuffInfo[i].mSwRotation = conv;
    }

#endif //ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE

    DBGT_EPILOG("");
}

void STECamera::setupVideoSwRotProcessing(int aRotation)
{
    DBGT_PROLOG("Rotation: %d", aRotation);

    swRoutines conv(NULL);

    //check if rotation requires any sw processing
    if ((0 == aRotation) || (90 == aRotation) || (270 == aRotation))
        conv = NULL;
    else if (180 == aRotation)
        conv = CamSwRoutines::YUV420MB_180_rotation_inplace;
    else
        DBGT_CRITICAL("Cant Happen");

    for (int i = 0; i < kRecordBufferCount; i++) {
        mRecordOmxBuffInfo[i].mSwRotation = conv;
    }

    DBGT_EPILOG("");
}

sp<MemoryBase>& STECamera::doPreviewSwRotProcessing(
        const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
        /* MMHwBuffer *const aMMHwBuffer */
        int aPrevWidth, int aPrevHeight)
{
    DBGT_PROLOG("Buff Header: %p", aOmxBuffHdr);
    DBGT_ASSERT(NULL != aOmxBuffHdr, "aOmxBuffHdr is NULL");
    DBGT_ASSERT(NULL != aOmxBuffHdr->pBuffer, "aOmxBuffHdr->pBuffer is NULL");

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(aOmxBuffHdr->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
    DBGT_ASSERT(NULL != buffInfo->mCamMemoryBase.get(), "CamMemoryBase NULL");

    if (buffInfo->mSwRotation) {

#if 0
        //synch buffer
        synchCBData(MMHwBuffer::ESyncAfterWriteHwOperation, *aMMHwBuffer,
                    aOmxBuffHdr->pBuffer, aOmxBuffHdr->nAllocLen);
#endif //0

         //perform rotation
        buffInfo->mSwRotation(aOmxBuffHdr->pBuffer,
                              (unsigned char*)(mPreviewTempBuffer->base()),
                              paramPortVPB0.format.video.nFrameWidth,
                              paramPortVPB0.format.video.nFrameHeight);

#if 0
        //synch will be done during conversion
        if (!buffInfo->mSwConversion)
            synchCBData(MMHwBuffer::ESyncBeforeReadHwOperation, *aMMHwBuffer,
                        aOmxBuffHdr->pBuffer, aOmxBuffHdr->nAllocLen);
#endif //0

    }
    DBGT_EPILOG("");
    return *(reinterpret_cast<sp<MemoryBase>*>(&buffInfo->mCamMemoryBase));
}

sp<MemoryBase>& STECamera::doPreviewSwConProcessing(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
                                                 /* MMHwBuffer* const aMMHwBuffer */
                                                 int aPrevWidth, int aPrevHeight)
{
   // IN2("Buff Header: %p MMHwBuffer: %p", aOmxBuffHdr, aMMHwBuffer);

    DBGT_PROLOG("Buff Header: %p", aOmxBuffHdr);

    DBGT_ASSERT(NULL != aOmxBuffHdr, "aOmxBuffHdr is NULL");
    DBGT_ASSERT(NULL != aOmxBuffHdr->pBuffer, "aOmxBuffHdr->pBuffer is NULL");

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(aOmxBuffHdr->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
    DBGT_ASSERT(NULL != buffInfo->mCamMemoryBase.get(), "CamMemoryBase NULL");


#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS

    /* Check if sw processing is to be done */
    if (buffInfo->mSwConversion) {
        //check all buffers valid
        DBGT_ASSERT(NULL != aOmxBuffHdr->pBuffer, "OMX pBuffer NULL");

        uint32_t actualData = aPrevWidth * aPrevHeight * 2;
        DBGT_ASSERT(0 != actualData, "Preview Width: %d Height: %d not good",
                   aPrevWidth, aPrevHeight);
        DBGT_PTRACE("Actual Data: %u FilledLen: %lu", actualData, aOmxBuffHdr->nFilledLen);

        size_t newFrameSize = (aPrevWidth * aPrevHeight * 3) / 2;
        //perform conversion
        buffInfo->mSwConversion(aOmxBuffHdr->pBuffer + aOmxBuffHdr->nOffset,
                                (unsigned char *)buffInfo->mProcessingBuffer->base(),
                                aPrevWidth,
                                aPrevHeight);

        //copy extra data
#ifdef SEND_PREVIEW_EXTRADATA
        memcpy(aOmxBuffHdr->pBuffer + aOmxBuffHdr->nOffset + actualData,
               (uint8_t *)buffInfo->mProcessingBuffer->base() + newFrameSize,
               aOmxBuffHdr->nFilledLen - actualData);
#endif //SEND_PREVIEW_EXTRADATA
        DBGT_EPILOG("");
        return buffInfo->mProcessingBuffer->memoryBase();
    } else
#endif //ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
    {
        DBGT_EPILOG("");
        return *(reinterpret_cast<sp<MemoryBase>*>(&buffInfo->mCamMemoryBase));
    }
}

void STECamera::doPictureSwRotProcessing(const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
                                         MMHwBuffer *const aMMHwBuffer)
{
    DBGT_PROLOG("Buff Header: %p MMHwBuffer: %p", aOmxBuffHdr, aMMHwBuffer);

    DBGT_ASSERT(NULL != aOmxBuffHdr, "aOmxBuffHdr is NULL");
    DBGT_ASSERT(NULL != aOmxBuffHdr->pBuffer, "aOmxBuffHdr->pBuffer is NULL");

    //check correct buffer got
    OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(aOmxBuffHdr->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");
    DBGT_ASSERT(NULL != buffInfo->mCamMemoryBase.get(), "CamMemoryBase NULL");

    /* Check if sw processing is to be done */
    if (buffInfo->mSwRotation) {
        //synch buffer
        synchCBData(MMHwBuffer::ESyncAfterWriteHwOperation, *aMMHwBuffer,
                    aOmxBuffHdr->pBuffer, aOmxBuffHdr->nAllocLen);

        //perform conversion
        buffInfo->mSwRotation(aOmxBuffHdr->pBuffer,
                              NULL,
                              mPictureInfo.getAlignedWidth(),
                              mPictureInfo.getAlignedHeight());

        //synch buffer
        synchCBData(MMHwBuffer::ESyncBeforeReadHwOperation, *aMMHwBuffer,
                    aOmxBuffHdr->pBuffer, aOmxBuffHdr->nAllocLen);

    }

    DBGT_EPILOG("");
}


void STECamera::setupXP70Traces()
{
    DBGT_PROLOG("");

#ifdef DUMP_XP70_TRACES

    if (DynSetting::get(DynSetting::EXP70LogEnable) > 0) {
        OmxUtils::StructContainer<IFM_PARAM_FIRMWARETRACETYPE> xp70trace;

        xp70trace.ptr()->bEnabled = OMX_TRUE;
        xp70trace.ptr()->eTraceMechanism =
            static_cast<IFM_FIRMWARETRACE_MECHANISMTYPE>(DynSetting::get(DynSetting::EXP70LogMechanism));
        xp70trace.ptr()->nLogLevel = DynSetting::get(DynSetting::EXP70LogLevel);

        DBGT_PINFO("XP70 trace activated with loglevel: 0x%08x Mechanism: %d",
                    (unsigned int)xp70trace.ptr()->nLogLevel,
                    xp70trace.ptr()->eTraceMechanism);

        OMX_ERRORTYPE err = OMX_SetParameter(mCam, (OMX_INDEXTYPE)IFM_IndexParamFirmwareTrace,
                                             xp70trace.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Not able to set xp70 traces: %s", OmxUtils::name(err));
        }
    }

#endif //DUMP_XP70_TRACES
    DBGT_EPILOG("");
    return;
}

status_t STECamera::flushViewFinderBuffers(bool aFlushCamera /*= true */)
{
    DBGT_PROLOG("FlushCamera: %d", aFlushCamera);

    status_t err = NO_ERROR;
    int portIndex = CAM_VPB;

    if (mHiResVFEnabled) // If Hi Res VF is enabled flush buffers on port2
        portIndex = CAM_VPB + 2;

    if (aFlushCamera) {
        /* Flush all pending buffers*/
        OMX_ERRORTYPE omxerr = OmxUtils::flushPort(mCam, portIndex,NULL,&stateCam_sem);
        if ((OMX_ErrorNone != omxerr) && (OMX_ErrorSameState != omxerr)) {
            DBGT_CRITICAL("Flush failed error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    //flush all preview threads
    status_t status = mSwProcessingThread->get()->flush();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("SwProcessingThread Flush failed status = %d", status);
        DBGT_EPILOG("");
        return status;
    }

    status = mPreviewThread->get()->flush();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("PreviewThread Flush failed status = %d", status);
        DBGT_EPILOG("");
        return status;
    }

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::flushVideoBuffers(bool aFlushCamera /*= true */)
{
    DBGT_PROLOG("FlushCamera: %d", aFlushCamera);

    status_t err = NO_ERROR;

    if (aFlushCamera) {
        /* Flush all pending buffers*/
        OMX_ERRORTYPE omxerr = OmxUtils::flushPort(mCam, CAM_VPB+2,NULL, &stateCam_sem);
        if ((OMX_ErrorNone != omxerr) && (OMX_ErrorSameState != omxerr)) {
            DBGT_CRITICAL("Flush failed error - %d", (int)UNKNOWN_ERROR);
            DBGT_EPILOG("");
            return UNKNOWN_ERROR;
        }
    }

    //flush all preview threads
    status_t status = mRecordThread->get()->flush();
    if (NO_ERROR != status) {
        DBGT_CRITICAL("mRecordThread Flush failed status = %d", status);
        DBGT_EPILOG("");
        return status;
    }

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::doCallbackProcessing(ReqHandlerThreadData &aData)
{
    DBGT_PROLOG("Msg: %d(0x%08x) Info1: %d Info2: %d",
        aData.mMsg, aData.mMsg,
        aData.mInfo1, aData.mInfo2);

    status_t err = NO_ERROR;

    Mutex::Autolock lock(mLock);

    DBGT_PTRACE("MsgEnabled: %d(0x%08x)", mMsgEnabled, mMsgEnabled);

    if (mMsgEnabled & aData.mMsg) {

        /* Dispatch callback with out lock. This will allow
         * callback to call cameraHAL APIs. Otherwise there
         * would be a deadlock
         */
        mLock.unlock();
        mNotifyCb(aData.mMsg, aData.mInfo1, aData.mInfo2, mCallbackCookie);
        mLock.lock();
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doRawProcessing(void* aArg)
{
    DBGT_PROLOG("Arg: %p", aArg);

    bool mActualSnapDuringRecord = mSnapshotDuringRecord;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    camera_sem_wait(&isp_snap_sem, SEM_WAIT_TIMEOUT);

    // Disable Shake Detection Callback after ISPProc EOS Callback.
    err = mOmxReqCbHandler.disable(OmxReqCbHandler::EPreCaptureExposureTime);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("disable reqcbfailed hand err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = getThumbnailHandler()->handleBuffer(pISP1outBuffer->nFilledLen, pISP1outBuffer->nFlags);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("getThumbnailHandler()->handleBuffer() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    getThumbnailHandler()->handleEvent();

    if (mMsgEnabled & CAMERA_MSG_RAW_IMAGE) {
        //perforrm conversion
        sp<MemoryBase>& frame = doPreviewSwConProcessing(pISP1outBuffer,
                                                         /* , m_pHwBuffer_raw */
                                                         mConfigPreviewWidth,
                                                         mConfigPreviewHeight);

        if (mIsPictureCancelled) {
            DBGT_PTRACE("takePicture has been cancelled, no need to send raw data ");
            mDataCb(CAMERA_MSG_RAW_IMAGE, NULL, 0, NULL, mCallbackCookie);
        } else {
            mPerfManager.logAndDump(PerfManager::ERawImageCallback);
            camera_memory_t* callbackMem = ((CamServiceBuffer *)mRawOmxBuffInfo.mProcessingBuffer)->mCamHandle;
            if (callbackMem) {
#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
                if (mRawOmxBuffInfo.mSwConversion) {
                    //Nothing needs to be done
                } else
#endif
                {
                    size_t size = 0;
                    ssize_t offset = 0;
                    sp<IMemoryHeap> memHeap = frame->getMemory((ssize_t*)&offset,(size_t*)&size);
                    memcpy(callbackMem->data, (char*)memHeap->base()+ offset, size);
                }
                DBGT_PTRACE("Sending Raw Image Callback ");
                mDataCb(CAMERA_MSG_RAW_IMAGE, callbackMem, 0, NULL, mCallbackCookie);
            }
        }
    } else if (mMsgEnabled & CAMERA_MSG_RAW_IMAGE_NOTIFY) {
        mPerfManager.logAndDump(PerfManager::ERawImageCallback);
        mNotifyCb(CAMERA_MSG_RAW_IMAGE_NOTIFY, 0, 0, mCallbackCookie);
    }

    // Dont send the Snapshot callback when taking
    // Still during record
    if (!mActualSnapDuringRecord) {

        // TBD: We need to re-think on this use-case. We should not be calling
        // doStopPreview internally. Need to re-understand the issue and fix it
        // correctly.

        /* Call dostoppreview to stop preview after takepic
           this has to be taken care by CameraHal as APP
           does not call stoppreview explicitly
           Need not to check whether startPreview present or
           not as takePicture will get call only when startpreview
           called earlier.
        */
        Mutex::Autolock lock(mLock);
        err = doStopPreview();

        if (OMX_ErrorNone != err) {
            dispatchError(err);
        }

        int index = dequeueNativeBuffer();
        if (index < 0) {
            DBGT_CRITICAL("Error in dequeueNativeBuffer()");
            DBGT_EPILOG("");
            return OMX_ErrorUndefined;
        }

        status_t error = getLockForNativeBuffer(index);
        if (error != OK) {
            DBGT_CRITICAL("STENativeWindow::dequeueBuffer() failed buff err (%d)",error);
            err = OMX_ErrorUndefined;
            DBGT_EPILOG("");
            return err;
        }

        if (mPreviewInfo.getOverScannedWidth() != mPreviewInfo.getAlignedWidth()) {

            B2R2Utills::TImageData srcImage, intImage;

            srcImage.mWidth = mPreviewInfo.getAlignedWidth();
            srcImage.mHeight = mPreviewInfo.getAlignedHeight();
            srcImage.mBufSize = paramISPoutput1.nBufferSize;
            srcImage.mLogAddr = (void*)mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
            srcImage.mPhyAddr = (void*)mRawOmxBuffInfo.mMMHwBufferInfo.iPhyAddr;
            srcImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;

            intImage.mWidth = mPreviewInfo.getOverScannedWidth();
            intImage.mHeight = mPreviewInfo.getOverScannedHeight();
            intImage.mBufSize = paramPortVPB0.nBufferSize;
            intImage.mLogAddr = (void*)mPreviewOmxBuffInfo[index].mMMHwBufferInfo.iLogAddr;
            intImage.mPhyAddr = (void*)mPreviewOmxBuffInfo[index].mMMHwBufferInfo.iPhyAddr;
            intImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;

            OMX_ERRORTYPE err = (OMX_ERRORTYPE)B2R2Utills::resize(&srcImage, &intImage);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("B2R2Utills::resize() failed");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }

            mPreviewWindow->setCrop(0, 0, mConfigPreviewWidth - 1, mConfigPreviewHeight - 1);
        } else {
            // Copy the Preview BuffInfo for the Thumbnail of the Still
            memcpy(pISP1outBuffer->pBuffer,
                   (void*)g_lastPreviewOMXBuffInfo->mMMHwBufferInfo.iLogAddr,
                   paramISPoutput1.nBufferSize);
        }
        error = renderNativeBuffer(index);
        if (error != OK) {
            DBGT_CRITICAL("renderNativeBuffer FAILED");
            err = OMX_ErrorUndefined;
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doStopRecording()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    disableCallbacks(mRecordOmxBuffInfo, kRecordBufferCount);

    err = updateCapturing(CAM_VPB + 2, OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateCapturing failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //Keep pushing dummy Buffer till EOS is avaliable from OMXCamera
    uint32_t loop = 10, timeoutInMs = 100; //10*100ms = 1 second
    int val;
    do {
        //push one dummy buffer(not already with client) to get EOS
        for (int i = 0; i < kRecordBufferCount; i++) {
            OmxBuffInfo *buffInfo = static_cast<OmxBuffInfo *>(pRecBuffer[i]->pAppPrivate);
            DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");

            //If not with component, fill buffer
            if ((buffInfo->mFlags == OmxBuffInfo::ENone)
               || (buffInfo->mFlags == OmxBuffInfo::ECameraHal)) {
                err = OmxBuffInfo::fillBuffer(mCam, pRecBuffer[i]);
                if (OMX_ErrorNone != err) {
                    DBGT_CRITICAL("fillBUffer failed err = %d", err);
                    DBGT_EPILOG("");
                    return err;
                }
                break;
            }
        }

        struct timespec wait;
        DBGT_PTRACE("Before Wait for Record EOS");

        val = clock_gettime(CLOCK_REALTIME, &wait);
        if (0 != val ) {
            DBGT_CRITICAL("clock_gettime failed err - OMX_ErrorTimeout");
            DBGT_EPILOG("");
            return OMX_ErrorTimeout;
        }

        //wait for EOS with timeout
        appendTimeoutValue(timeoutInMs, wait);
        mLock.unlock();
        val = sem_timedwait(&record_eos_sem, &wait);
        mLock.lock();
    } while ((0 != val) && --loop);

    //check stop was ok
    if (0 != val ) {
        DBGT_CRITICAL("sem_timedwait failed err - OMX_ErrorTimeout");
        DBGT_EPILOG("");
        return OMX_ErrorTimeout;
    }

    //flush video buffers
    status_t status = flushVideoBuffers();
    if (NO_ERROR != status ) {
        DBGT_CRITICAL("flushVideoBuffers failed err - OMX_ErrorUndefined");
        DBGT_EPILOG("");
        return OMX_ErrorUndefined;
    }

    //Stop Dumping Frames
    mFrameDumper.stop(CFrameDump::ERecording);
    mFrameReplay.stop(CFrameReplay::ERecording);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::doStopPreview()
{
    DBGT_PROLOG("");

    OMX_STATETYPE state;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    disableCallbacks(mPreviewOmxBuffInfo, kTotalPreviewBuffCount);

    if (mHiResVFEnabled) {
        err = updateCapturing(CAM_VPB + 2, OMX_FALSE);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("updateCapturing failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    OMX_GetState(mCam, &state);
    DBGT_PTRACE("camera state = %s", OmxUtils::name(state));

    if ( OMX_StateExecuting == state) {
        /* Flush out the port 1 of camera component before
           Cam Component gets idle
        */

        if (EStill == mCamMode) {
            OMX_ERRORTYPE err = OmxUtils::flushPort(mCam, CAM_VPB + 1,NULL,&stateCam_sem);
            if (!((OMX_ErrorNone == err)||(OMX_ErrorSameState == err))) {
                DBGT_CRITICAL("Flush failed err = %d", err);
                DBGT_EPILOG("");
                return err;
            }
        }
        //ER 403945
        // In case of pressing reset switch while camera is streaming, stop preview is called
        // without setpreviewwindow called before. So check needs to be added of the
        // mPreviewWindow is initialized or not
        if (mPreviewWindow && !mIsStillZSL) {
            //flush pending buffers
            status_t status = flushViewFinderBuffers();
            if(NO_ERROR != status) {
                DBGT_CRITICAL("flushViewFinderBuffers failed err = OMX_ErrorUndefined");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
           }
        }

        //Stop duming frames
        mFrameDumper.stop(CFrameDump::EPreview);
        mFrameReplay.stop(CFrameReplay::EPreview);
        mPreviewRunning = false;
    }

    DBGT_EPILOG("");
    return err;
}

CFrameDumpReplay::TColorFmt STECamera::getDumpReplayColorFmt(OMX_COLOR_FORMATTYPE aOmxColorFmt)
{
    DBGT_PROLOG(" aOmxColorFmt: %#x", aOmxColorFmt);

    CFrameDumpReplay::TColorFmt colorFmt = CFrameDumpReplay::EYUV420MBPackedSemiPlanar;

    switch ((uint32_t)aOmxColorFmt) {
        case OMX_COLOR_FormatCbYCrY:
            colorFmt = CFrameDumpReplay::ECbYCrY;
            break;
        case OMX_COLOR_Format16bitRGB565:
            colorFmt = CFrameDumpReplay::E16bitRGB565;
            break;
        case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            colorFmt = CFrameDumpReplay::EYUV420MBPackedSemiPlanar;
            break;
        default:
            DBGT_CRITICAL("Cant Happen");
    }

    DBGT_EPILOG("colorFmt = %d", colorFmt);
    return colorFmt;
}

void STECamera::setPreviewFpsRange(int min, int max)
{
    DBGT_PROLOG("");

    char fpsrange[32];
    sprintf(fpsrange, "%d,%d", min, max);
    DBGT_PTRACE("Setting range to %s", fpsrange);
    mParameters.set(CameraParameters::KEY_PREVIEW_FPS_RANGE, fpsrange);

    DBGT_EPILOG("");
}

void STECamera::dispatchNotification(int32_t aMsg, int32_t aInfo1, int32_t aInfo2 /*= 0*/)
{
    DBGT_PROLOG("Msg: 0x%08x(%d) Info1: %d Info2: %d", aMsg, aMsg, aInfo1, aInfo2);

    ReqHandlerThreadData *req = findFreeReqSlot();

    //copy data
    req->mMsg = aMsg;
    req->mInfo1 = aInfo1;
    req->mInfo2 = aInfo2;
    req->mWaitForRequestToStart = false;

    //request
    mReqHandlerThread->get()->request(*req);

    DBGT_EPILOG("");
    return;
}

void STECamera::dispatchRequest(reqHandler aReqHandler, void *aArg /*= NULL */, bool aWaitForRequest /*= true*/)
{
    DBGT_PROLOG("Arg: %p WaitForRequest: %d", aArg, aWaitForRequest);

    ReqHandlerThreadData *req = findFreeReqSlot();

    //copy data
    req->mReqHandler = aReqHandler;
    req->mArg = aArg;
    req->mWaitForRequestToStart = aWaitForRequest;

    //request
    mReqHandlerThread->get()->request(*req);

    DBGT_EPILOG("");
    return;
}

ReqHandlerThreadData *STECamera::findFreeReqSlot()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mReqHandlerThreadData, "mReqHandlerThreadData is NULL");

    //find free slot
    for (uint32_t loop = 0; loop < kMaxRequests; loop++) {
        if (mReqHandlerThreadData[loop].updateIfFree()) {
            DBGT_EPILOG("%p", &mReqHandlerThreadData[loop]);
            return &mReqHandlerThreadData[loop];
        }
    }

    //none found
    DBGT_CRITICAL("Cant Happen");

    DBGT_EPILOG("");
    return (ReqHandlerThreadData *)NULL;
}

void STECamera::configureAndStartHiResVF()
{
    DBGT_PROLOG("");

    // set preview stride
    mParameters.set(KEY_PREVIEW_NSTRIDE,
                    cameraFormatInfo().mPreviewStrideFactor * mPreviewInfo.getAlignedWidth());
    OMX_ERRORTYPE stateChangeErr;
    if (g_IsPreviewConfig) {
        DBGT_PTRACE("Disabling Port VPB0");
        OMX_ERRORTYPE err  = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 0, NULL,NULL);

        if ((err == OMX_ErrorNone) || (OMX_ErrorSameState == err)) {
            DBGT_PTRACE("OMX_SendCommand OMX_CommandPortDisable Port 0 passed\n");
        }

        /*  Free preview buffers */
        if (OMX_ErrorSameState != err) {
            for (int i = 0; i < kTotalPreviewBuffCount; i++) {
                if ( NULL != pVFBuffer[i]) {
                    err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 0), pVFBuffer[i]);// to see the logic
                    if (err == OMX_ErrorNone) {
                        DBGT_PTRACE(" OMX_FreeBuffer for cam passed for buffer %d before reconfiguring \n", i);
                    } else {
                        DBGT_CRITICAL("OMX_FreeBuffer failed for VPB0");
                    }
                    mPreviewOmxBuffInfo[i].clear();
                }
            }
            camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
        }

        DBGT_PTRACE("Disabling Port VPB2");
        err  = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 2, NULL,NULL);
        if ((err == OMX_ErrorNone) || (OMX_ErrorSameState == err)) {
            DBGT_PTRACE("OMX_SendCommand OMX_CommandPortDisable Port 2 passed\n");
        }

        /*  Free recording buffers */
        if (OMX_ErrorSameState != err) {
            for (int i = 0; i < kRecordBufferCount; i++) {
                if (NULL != pRecBuffer[i]) {
                    err = OMX_FreeBuffer(mCam, (OMX_U32)(CAM_VPB + 2), pRecBuffer[i]);// to see the logic
                    if (err == OMX_ErrorNone)
                    {
                        DBGT_PTRACE("In Camera Destructor OMX_FreeBuffer for cam passed \n");
                    } else {
                        DBGT_CRITICAL("OMX_FreeBuffer failed for VPB2.   Error = 0x%x", err);
                    }
                    mRecordOmxBuffInfo[i].clear();
               }
            }
            camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
        }

        /*  Destroy the buffer heap*/
        DBGT_PTRACE("Destroying Buffer Heap for Preview and Record");

        STECamMMHwBuffer::freeHwBuffer(mPreviewOmxBuffInfo,true);
        STECamMMHwBuffer::freeHwBuffer(&mRawOmxBuffInfo);
        STECamMMHwBuffer::freeHwBuffer(mRecordOmxBuffInfo);
    }

    // Port 2 will be used for HiResVF Feature for sending High Resolution frames
    OmxUtils::initialize(paramPortVPB2, OMX_PortDomainVideo, CAM_VPB + 2);
    OMX_ERRORTYPE err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);
    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 2 passed \n");
    }

    paramPortVPB2.nBufferCountActual = kTotalPreviewBuffCount;     // the index of the input port. Should be modified.

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE *pt_video2 = &(paramPortVPB2.format.video);
    pt_video2->cMIMEType = (OMX_STRING)"";
    pt_video2->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video2->nFrameWidth = (OMX_U32) mPreviewInfo.getAlignedWidth();
    pt_video2->nFrameHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
    pt_video2->nStride = (OMX_U32)(2 * mPreviewInfo.getAlignedWidth());
    pt_video2->nSliceHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
    pt_video2->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_video2->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_video2->eColorFormat = cameraFormatInfo().mPreviewOmxColorFormat; // for video/still preview
    pt_video2->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_video2->xFramerate = mConfigFrameRate * (1 << 16); /*in Q16*/

    err = OMX_SetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);

    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 2 passed\n ");
    }

    err =OMX_GetParameter(mCam, OMX_IndexParamPortDefinition, &paramPortVPB2);

    if (err == OMX_ErrorNone) {
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 2 passed \n");
    }

    OMX_STATETYPE state;
    OMX_GetState(mCam, &state);
    DBGT_PTRACE("state before sending idle for camera= %s", OmxUtils::name(state));

    if (OMX_StateLoaded == state) {
        // If state is loaded, disable ports VPB0 and VPB1 and then move to IDLE state
        DBGT_PTRACE("Disabling Port VPB0");
        err  = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 0, NULL,&stateCam_sem);
        if ((OMX_ErrorNone == err) || (OMX_ErrorSameState == err)) {
            DBGT_PTRACE("OMX(uint32_t)_SendCommand OMX_CommandPortDisable Port 0 passed\n");
        }


        DBGT_PTRACE("Disabling Port VPB1");
        err  = OmxUtils::setPortState(mCam, OMX_CommandPortDisable,CAM_VPB + 1,NULL,&stateCam_sem);
        if ((OMX_ErrorNone == err) || (OMX_ErrorSameState == err)) {
            DBGT_PTRACE("OMX_SendCommand OMX_CommandPortDisable Port 1 passed\n");
        }

        stateChangeErr = OMX_SendCommand(mCam, OMX_CommandStateSet, OMX_StateIdle, NULL);
        if (stateChangeErr == OMX_ErrorNone) {
            DBGT_PTRACE("OMX_SendCommand OMX_StateIdle passed ");
        } else {
            DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed with error 0x%x ", stateChangeErr);
        }

    DBGT_PTRACE("Going to WAIT");
    camera_sem_wait(&stateCam_sem, SEM_WAIT_TIMEOUT);
    DBGT_PTRACE("WAIT Over");
}

    DBGT_EPILOG("");
    return;
}

void STECamera::setupExtraKeys()
{
    DBGT_PROLOG("");

    mParameters.set(KEY_PREVIEW_NSTRIDE,
                    cameraFormatInfo().mPreviewStrideFactor * mPreviewInfo.getAlignedWidth());

    DBGT_PTRACE("Preview Width: %d Stride factor: %d", mPreviewInfo.getAlignedWidth(), cameraFormatInfo().mPreviewStrideFactor);


    //setup video keys
    if (mRecordHeap != 0) {
        DBGT_PTRACE("STECam Calling setRecordHeap with RECORD_MEM_HANDLE as: %x REcord buffer Count %x, Frame size%x MemheapHandle %x",
             (unsigned int) mRecordOmxBuffInfo[0].m_pMMHwBuffer, (unsigned int)kRecordBufferCount, (unsigned int)mRecordFrameSize,
             (unsigned int)&mRecordHeap);
        mParameters.set(KEY_RECORD_MEM_HANDLE, (unsigned int) mRecordOmxBuffInfo[0].m_pMMHwBuffer);
        mParameters.set(KEY_RECORD_BUFFER_COUNT, (unsigned int) kRecordBufferCount);
        mParameters.set(KEY_RECORD_BUFFER_SIZE, (unsigned int) mRecordFrameSize);
        mParameters.set(KEY_RECORD_MEM_HEAP_HANDLE, (unsigned int)&mRecordHeap);
    }
    DBGT_EPILOG("");
}

OMX_ERRORTYPE STECamera::postOMXConfigStartViewfinder(bool aIsFirstVF /* = false */)
{
    DBGT_PROLOG("IsFirstVF: %d", aIsFirstVF);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mFrameDumper.start(CFrameDump::EPreview, paramPortVPB0.format.video.nFrameWidth,
                       paramPortVPB0.format.video.nFrameHeight,
                       getDumpReplayColorFmt(cameraFormatInfo().mPreviewOmxColorFormat),
                       this, pVFBuffer[0]->nAllocLen, mPreviewOmxBuffInfo[0].m_pMMHwBuffer);
    mFrameReplay.start(CFrameDump::EPreview, paramPortVPB0.format.video.nFrameWidth,
                       paramPortVPB0.format.video.nFrameHeight,
                       getDumpReplayColorFmt(cameraFormatInfo().mPreviewOmxColorFormat),
                       this, pVFBuffer[0]->nAllocLen, mPreviewOmxBuffInfo[0].m_pMMHwBuffer);

    //enable callbacks
    enableCallbacks(mPreviewOmxBuffInfo, kTotalPreviewBuffCount);

    //supply buffers

    err = supplyPreviewBuffers();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("supplyPreviewBuffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

#ifdef DUMP_FPS_FOR_VF
    mFrameRateDumperVF.start();
#endif //DUMP_FPS_FOR_VF

    if (aIsFirstVF) {
        //preview started
        mPreviewRunning = true;

        //perf trace
        mIsFirstViewFinderFrame = false;
    }

    mPerfManager.logAndDump(PerfManager::EViewFinderStarted);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::postOMXConfigStartVideoRecord()
{
    DBGT_PROLOG();

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mFrameDumper.start(CFrameDump::ERecording, paramPortVPB2.format.video.nFrameWidth,
                       paramPortVPB2.format.video.nFrameHeight,
                       getDumpReplayColorFmt(cameraFormatInfo().mRecordOmxColorFormat));
    mFrameReplay.start(CFrameDump::ERecording, paramPortVPB2.format.video.nFrameWidth,
                       paramPortVPB2.format.video.nFrameHeight,
                       getDumpReplayColorFmt(cameraFormatInfo().mRecordOmxColorFormat));

    //Reset B2R2 Semaphore
    if(mEnableB2R2DuringRecord == true){
        int sem_b2r2_val,sem_b2r2_reset_val,sem_crop_buffer_val,i;

        sem_getvalue(&sem_b2r2, &sem_b2r2_val);
        sem_getvalue(&sem_b2r2_reset, &sem_b2r2_reset_val);
        sem_getvalue(&sem_crop_buffer, &sem_crop_buffer_val);

        for (i=0;i<sem_b2r2_val;i++)sem_trywait(&sem_b2r2);
        for (i=0;i<sem_b2r2_reset_val;i++)sem_trywait(&sem_b2r2_reset);
        for (i=0;i<sem_crop_buffer_val;i++)sem_trywait(&sem_crop_buffer);

    }
    err = updateAutoPause(OMX_FALSE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateAutoPause failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = updateCapturing(CAM_VPB+2, OMX_TRUE);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("updateCapturing failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //enable callbacks
    enableCallbacks(mRecordOmxBuffInfo, kRecordBufferCount);

    //supply buffers
    err = supplyRecordBuffers();
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("supplyRecordBuffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mRecordRunning = true;

#ifdef DUMP_FPS_FOR_VIDEO
    mFrameRateDumperVideo.start();
#endif //DUMP_FPS_FOR_VIDEO

    //perf trace
    mIsFirstVideoRecordFrame = false;
    mPerfManager.logAndDump(PerfManager::EVideoStarted);

    DBGT_EPILOG("");
    return err;
}

status_t STECamera::initProcessingBuffers()
{
    DBGT_PROLOG("");

    status_t err = NO_ERROR;

    /* Processign Buffers are setup one time.
     * They're deleted in ~OmxBuffInfo and not in OmxBuffInfo::clear()
     */
    for (int i = 0; i < kTotalPreviewBuffCount; i++) {
        mPreviewOmxBuffInfo[i].mProcessingBuffer = new CamServiceBuffer(mRequestMemory,&mCallbackCookie);
        if (NULL == mPreviewOmxBuffInfo[i].mProcessingBuffer) {
            DBGT_CRITICAL("alloc failed NO_MEMORY");
            DBGT_EPILOG("");
            return NO_MEMORY;
        }
    }

    mRawOmxBuffInfo.mProcessingBuffer = new CamServiceBuffer(mRequestMemory,&mCallbackCookie);
    if (NULL == mRawOmxBuffInfo.mProcessingBuffer) {
        DBGT_CRITICAL("alloc failed NO_MEMORY");
        DBGT_EPILOG("");
        return NO_MEMORY;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE STECamera::setConfigVideoStab(OMX_BOOL aStabState /*= OMX_TRUE*/)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;
    OmxUtils::StructContainer<OMX_CONFIG_FRAMESTABTYPE> videoStab;

    err = OMX_GetConfig(mCam, OMX_IndexConfigCommonFrameStabilisation, &videoStab);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetConfig OMX_IndexConfigCommonFrameStabilisation for mCam failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    // Set Stab for VPB0
    videoStab.ptr()->nPortIndex = CAM_VPB + 0;
    videoStab.ptr()->bStab = aStabState;
    err = OMX_SetConfig(mCam, OMX_IndexConfigCommonFrameStabilisation, &videoStab);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetConfig OMX_IndexConfigCommonFrameStabilisation for mCam for VPB0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    // Set Stab for VPB2
    videoStab.ptr()->nPortIndex = CAM_VPB + 2;
    videoStab.ptr()->bStab = aStabState;
    err = OMX_SetConfig(mCam, OMX_IndexConfigCommonFrameStabilisation, &videoStab);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("SetConfig OMX_IndexConfigCommonFrameStabilisation for mCam for VPB2 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void STECamera::getCropVector(OMX_U8* apTmp, uint32_t& aLx,
                              uint32_t& aLy, uint32_t& aW, uint32_t& aH)
{
    DBGT_PROLOG("apTmp 0x%x", (unsigned int)apTmp);
    OMX_OTHER_EXTRADATATYPE* pExtra = (OMX_OTHER_EXTRADATATYPE*)((OMX_U32)apTmp);
    uint16_t aOverScannedWidth = mPreviewInfo.getOverScannedWidth();
    uint16_t aOverScannedHeight = mPreviewInfo.getOverScannedHeight();

    while (pExtra->eType != OMX_ExtraDataNone) {
        if (pExtra->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization) {
            if (pExtra->nDataSize != sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE)) {
                DBGT_PTRACE("DataSize of VideoStab Incorrect : ALERT");
            }
            OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pVideoStab
                = (OMX_SYMBIAN_DIGITALVIDEOSTABTYPE*)pExtra->data;
            // Get the state of VideoStabiliztion
            if (pVideoStab->bState == true) {
                DBGT_PTRACE("Video Stab is enabled, Get the Crop Vector");
                DBGT_PTRACE("Crop Vector: TopLeft(X) %u",
                        (unsigned int)pVideoStab->nTopLeftCropVectorX);
                DBGT_PTRACE("Crop Vector: TopLeft(Y) %u",
                        (unsigned int)pVideoStab->nTopLeftCropVectorY);
                DBGT_PTRACE("Crop Vector: nMaxOverscannedWidth %u",
                        (unsigned int)pVideoStab->nMaxOverscannedWidth);
                DBGT_PTRACE("Crop Vector: nMaxOverscannedHeight %u",
                        (unsigned int)pVideoStab->nMaxOverscannedHeight);
                aLx = pVideoStab->nTopLeftCropVectorX;
                aLy = pVideoStab->nTopLeftCropVectorY;
                aW = mConfigPreviewWidth;
                aH = mConfigPreviewHeight;

                if (mConfigPreviewWidth + aLx > aOverScannedWidth) {
                    aLx = aOverScannedWidth - mConfigPreviewWidth;
                }
                if (mConfigPreviewHeight + aLy > aOverScannedHeight) {
                    aLy = aOverScannedHeight - mConfigPreviewHeight;
                }

                break;
            }
        }
        pExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U8*)pExtra) + pExtra->nSize);
    }
    DBGT_EPILOG("");
}

status_t STECamera::UpdateVideoStabStatus()
{
    DBGT_PROLOG("");

    status_t status = NO_ERROR;

#ifdef ENABLE_VIDEO_STAB
    OMX_ERRORTYPE err = OMX_ErrorNone;
    bool videoStabStatusUpdated = true;

    bool previewPortEnabled = isPreviewPortEnabled();
    bool previewRunning = checkPreviewEnabled();
    if (previewPortEnabled) {
        if (previewRunning) {
            err = doStopPreview();
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("doStopPreview failed");
                DBGT_EPILOG("");
                return err;
            }
        }
        err = disablePreviewPort(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("disablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
    }

    // Update Video Stab Status
    if (mVideoStabEnabled) {
        // Video Stab already enabled, Disable it
        DBGT_PTRACE("Enable the video stab using SetConfig");
        err = setConfigVideoStab();
    } else {
        DBGT_PTRACE("Stab already enabled, Disable it using SetConfig");
        err = setConfigVideoStab(OMX_FALSE);

        // Reset the crop window set on Preview
        mPreviewWindow->setCrop(0, 0, mConfigPreviewWidth - 1, mConfigPreviewHeight - 1);
    }
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Video Stab Config for mCam failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (previewPortEnabled) {
        err = enablePreviewPort(true);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("enablePreviewPort failed");
            DBGT_EPILOG("");
            return err;
        }
        if (previewRunning) {
            status = doStartPreview();
            if (NO_ERROR != status) {
                DBGT_CRITICAL("doStartPreview failed");
                DBGT_EPILOG("");
                return OMX_ErrorUndefined;
            }
        }
    }

#endif //ENABLE_VIDEO_STAB

    DBGT_EPILOG("");
    return status;
}

OMX_ERRORTYPE STECamera::InitializePort(
        OMX_PARAM_PORTDEFINITIONTYPE& aPortDefType,
        OMX_PORTDOMAINTYPE aPortDomain,
        OMX_HANDLETYPE aOmxComponent,
        int aPortIndex,
        bool aStillMode)
{
    DBGT_PROLOG("Component %s, Port Index %d, Mode %s",
            OmxUtils::name(aOmxComponent), aPortIndex, aStillMode ? "Still" : "Video");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_VIDEO_PORTDEFINITIONTYPE* pt_video;
    OMX_IMAGE_PORTDEFINITIONTYPE* pt_image;

    // Initialize the port structure
    OmxUtils::initialize(aPortDefType, aPortDomain, aPortIndex);

    err = OMX_GetParameter(aOmxComponent, OMX_IndexParamPortDefinition, &aPortDefType);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    // Initialize Camera Ports
    if (aOmxComponent == mCam) {
        pt_video = &(aPortDefType.format.video);
        switch (aPortIndex) {
            // Initialize Camera Port 0
            case CAM_VPB + 0:
                DBGT_PTRACE("Initialize Cam Port VPB0");
                aPortDefType.nBufferCountActual = kTotalPreviewBuffCount;

                pt_video->cMIMEType = (OMX_STRING)"";
                pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->nFrameWidth = (OMX_U32) mPreviewInfo.getAlignedWidth();
                pt_video->nFrameHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
                pt_video->nStride = (OMX_U32)(2 * mPreviewInfo.getAlignedWidth());
                pt_video->nSliceHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
                pt_video->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
                pt_video->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
                pt_video->eColorFormat = cameraFormatInfo().mPreviewOmxColorFormat;// for video/still preview
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->xFramerate = mConfigFrameRate * (1 << 16); /*in Q16*/;
                break;

            // Initialize Camera Port 1
            case CAM_VPB + 1:
                DBGT_PTRACE("Initialize Cam Port VPB1");
                aPortDefType.nBufferCountActual = 1;
                if (mIsStillZSL) {
                    aPortDefType.nBufferCountActual = mStillZslBufferCount;
                }

                pt_video->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit;
                pt_video->xFramerate = mConfigFrameRate * (1 << 16); /*in Q16*/
                pt_video->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;

                if (mIsStillZSL) {
                    pt_video->eColorFormat =(OMX_COLOR_FORMATTYPE)mOmxRecordPixFmt;
                    pt_video->cMIMEType = (OMX_STRING)"";
                    pt_video->xFramerate= 22 * (1 << 16); /*in Q16*/;
                    pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                    pt_video->nFrameWidth = (OMX_U32)mPictureInfo.getAlignedWidth();
                    pt_video->nFrameHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
                    pt_video->nStride = (OMX_U32)(mPictureInfo.getAlignedWidth() * 3 / 2);
                    pt_video->nSliceHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
                    pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                    pt_video->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingUnused;
                }
                break;

            // Initialize Camera Port 2
            case CAM_VPB + 2:
                DBGT_PTRACE("Initialize Cam Port VPB2");
                aPortDefType.nBufferCountActual = kRecordBufferCount;

                pt_video->cMIMEType = (OMX_STRING)"";
                pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                Vector<Size> sizes;
                bool is2MpVideoSizeSupported = false;
                mParameters.getSupportedVideoSizes(sizes);
                for (unsigned int ss = 0; ss < sizes.size(); ss++) {
                    if (sizes[ss].width == OVERSCAN_STILL_WIDTH_2MP) {
                        is2MpVideoSizeSupported = true;
                        break;
                    }
                }
                if (mEnableB2R2DuringRecord && is2MpVideoSizeSupported) {
#ifdef ENABLE_VIDEO_STAB
                    // Video Stab is Enabled
                    // Back Camera will by itself give overscanned buffers ~ 2Mp
                    pt_video->nFrameWidth = STILL_WIDTH_2MP;
                    pt_video->nFrameHeight = STILL_HEIGHT_2MP;
                    pt_video->nStride = (OMX_U32)(1.5 * STILL_WIDTH_2MP);
                    pt_video->nSliceHeight = STILL_HEIGHT_2MP;
#else
                    // Back Camera should be set to provide 2Mp buffers
                    pt_video->nFrameWidth = OVERSCAN_STILL_WIDTH_2MP;
                    pt_video->nFrameHeight = OVERSCAN_STILL_HEIGHT_2MP;
                    pt_video->nStride = (OMX_U32)(1.5 * OVERSCAN_STILL_WIDTH_2MP);
                    pt_video->nSliceHeight = OVERSCAN_STILL_HEIGHT_2MP;
#endif
                } else {
                    pt_video->nFrameWidth = (OMX_U32)mRecordInfo.getAlignedWidth();
                    pt_video->nFrameHeight = (OMX_U32)mRecordInfo.getAlignedHeight();
                    pt_video->nStride = (OMX_U32)(1.5 * mRecordInfo.getAlignedWidth());
                    pt_video->nSliceHeight = (OMX_U32)mRecordInfo.getAlignedHeight();
                }
                if (mOmxRecordPixFmt == OMX_COLOR_FormatYUV420SemiPlanar) {
                    pt_video->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
                } else {
                    pt_video->eColorFormat = mOmxRecordPixFmt;
                }
                pt_video->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
                pt_video->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_VIDEO_CodingUnused;
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->xFramerate = mConfigFrameRate * (1 << 16); /*in Q16*/
                if (mSlowMotionFPS > mConfigFrameRate) {
                    pt_video->xFramerate = mSlowMotionFPS  * (1 << 16); /*in Q16*/
                }
                break;
        }
    } else if (aOmxComponent == mISPProc) {
        // Initialize ISP Proc Ports
        aPortDefType.nBufferCountActual = 1;
        pt_video = &(aPortDefType.format.video);

        switch (aPortIndex) {
            case 0:
                DBGT_PTRACE("Initialize ISPProc Port 0");
                pt_video->cMIMEType = (OMX_STRING)"";
                pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->nFrameWidth = paramPortVPB1.format.video.nFrameWidth;
                pt_video->nFrameHeight = paramPortVPB1.format.video.nFrameHeight;
                pt_video->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit;
                pt_video->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
                pt_video->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                break;
            case 1:
#ifdef HWISPPROC
                DBGT_PTRACE("Initialize ISPProc Port 1 for HWISP");
                pt_video->eColorFormat = cameraFormatInfo().mPreviewOmxColorFormat;
                pt_video->cMIMEType = (OMX_STRING)"";
                pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->nFrameWidth = (OMX_U32) mPreviewInfo.getAlignedWidth();
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->nFrameHeight = (OMX_U32)mPreviewInfo.getAlignedHeight();
                pt_video->nStride = (OMX_U32)((mPreviewInfo.getAlignedWidth() ) * 2);
                pt_video->nSliceHeight = (OMX_U32)mPreviewInfo.getAlignedHeight() ;
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->eCompressionFormat = OMX_VIDEO_CodingUnused;

                mRawImage.width = mPreviewInfo.getAlignedWidth() ;
                mRawImage.height = mPreviewInfo.getAlignedHeight();
                break;
#endif
            case 2:
#ifdef SWISPPROC
                DBGT_PTRACE("Initialize ISPProc Port 1 for SWISP");
#elif HWISPPROC
                DBGT_PTRACE("Initialize ISPProc Port 2 for HWISP");
#endif
                pt_video->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
                pt_video->cMIMEType = (OMX_STRING)"";
                pt_video->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->nFrameWidth = (OMX_U32)mPictureInfo.getAlignedWidth();
                pt_video->nFrameHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
                pt_video->nStride = (OMX_U32)(mPictureInfo.getAlignedWidth() * 3 / 2);
                pt_video->nSliceHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
                pt_video->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                pt_video->eCompressionFormat = OMX_VIDEO_CodingUnused;
                break;
        }
    } else if (aOmxComponent == mJpegEnc) {
        // Initialize Jpeg Enc Ports
        aPortDefType.nBufferCountActual = 1;
        if (!aStillMode) {
            aPortDefType.nBufferCountActual = kRecordBufferCount;
        }
        pt_image = &(aPortDefType.format.image);

        switch (aPortIndex) {
            case 0:
                DBGT_PTRACE("Initialize Jpeg Enc Port 0");
                if (mIsStillZSL) {
                    aPortDefType.nBufferCountActual = paramPortVPB1.nBufferCountActual;
                }
                pt_image->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
                pt_image->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
                break;
            case 1:
                DBGT_PTRACE("Initialize Jpeg Enc Port 1");
                pt_image->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingEXIF ;
                pt_image->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused ;
                break;
        }

        pt_image->cMIMEType = (OMX_STRING)"";
        pt_image->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
        if (aStillMode) {
            // Still Mode
            pt_image->nFrameWidth = (OMX_U32)mPictureInfo.getAlignedWidth();
            pt_image->nFrameHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
        } else {
            // Video Mode, Snapshot During Recording
            pt_image->nFrameWidth = mRecordInfo.getOverScannedWidth();
            pt_image->nFrameHeight = mRecordInfo.getOverScannedHeight();
        }
        pt_image->bFlagErrorConcealment  = (OMX_BOOL)OMX_FALSE;
        pt_image->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
    } else if (aOmxComponent == mExifMixer) {
        // Initialize Exif Mixed Ports
        aPortDefType.nBufferCountActual = 1;
        pt_image = &(aPortDefType.format.image);

        switch (aPortIndex) {
            case 1:
                if (!aStillMode) {
                    aPortDefType.nBufferCountActual = kRecordBufferCount;
                }
            case 2:
                DBGT_PTRACE("Initialize Exif Mixer Port %d", aPortIndex);
                pt_image->cMIMEType = (OMX_STRING)"";
                pt_image->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
                if (aStillMode) {
                    // Still Mode
                    pt_image->nFrameWidth = (OMX_U32)mPictureInfo.getAlignedWidth();
                    pt_image->nFrameHeight = (OMX_U32)mPictureInfo.getAlignedHeight();
                } else {
                    // Video Mode, Snapshot During Recording
                    pt_image->nFrameWidth = mRecordInfo.getOverScannedWidth();
                    pt_image->nFrameHeight = mRecordInfo.getOverScannedHeight();
                }
                pt_image->bFlagErrorConcealment  = (OMX_BOOL)OMX_FALSE;
                pt_image->eCompressionFormat  = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingEXIF;
                pt_image->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
                pt_image->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;
                break;
        }
    }

    err = OMX_SetParameter(aOmxComponent, OMX_IndexParamPortDefinition, &aPortDefType);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (aOmxComponent == mJpegEnc && aPortIndex == 1) {
        // Since Jpeg Output goes to EXIF, Jpeg Enc adds another 64Kb to its input buffer
        // for EXIF data from Camera.
        // To get the correct Jpeg Input Buffer Size query the component.
        err = OMX_GetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGinput);
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition on JpegEnc failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        if (aStillMode) {
            // Still Mode, use the picture info
            err = cropOutputSize(mJpegEnc, mPictureInfo.getWidth(), mPictureInfo.getHeight());
        } else {
            // Video Mode, use the record info
            // Jpeg in Video used for Sanpshot during Record
            int w = SNAPSHOT_WIDTH_2MP;
            int h = SNAPSHOT_HEIGHT_2MP;
#ifdef ENABLE_VIDEO_STAB
            // If Video Stab is not enabled, the buffers will be over-scanned
            // but the over-scanned data will not be valid
            // Therefore we will need to crop that invalid data
            if (!mVideoStabEnabled) {
                w = STILL_WIDTH_2MP;
                h = STILL_HEIGHT_2MP;
            }
#endif
            err = cropOutputSize(mJpegEnc, w, h);
        }
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("cropOutputSize for mJpegEnc failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    err = OMX_GetParameter(aOmxComponent, OMX_IndexParamPortDefinition, &aPortDefType);
    if (OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    if (aOmxComponent == mCam) {
        if (aPortIndex == (CAM_VPB + 0)) {
#ifdef ENABLE_VIDEO_STAB
            // set the overscanned width and height
            mPreviewInfo.setOverScannedParams((&(paramPortVPB0.format.video))->nStride/2,
                                              (&(paramPortVPB0.format.video))->nSliceHeight);
#endif
            mParameters.set(KEY_PREVIEW_NSTRIDE,
                            cameraFormatInfo().mPreviewStrideFactor * mPreviewInfo.getAlignedWidth());
        } else if (aPortIndex == (CAM_VPB + 2)) {
            if (mEnableB2R2DuringRecord) {
                int stride = 0;
                switch( (uint32_t)mOmxRecordPixFmt){
                    case OMX_COLOR_FormatCbYCrY:
                    case OMX_COLOR_Format16bitRGB565:
                        stride = mRecordInfo.getAlignedWidth()*2;
                        break;
                    case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
                    case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
                        stride = mRecordInfo.getAlignedWidth()*3/2;
                        break;
                    case OMX_COLOR_FormatYUV420SemiPlanar:
                        stride = mRecordInfo.getAlignedWidth();
                        break;
                    default:
                        stride = mRecordInfo.getAlignedWidth();
                        break;
                }

                mParameters.set(CameraParameters::KEY_RECORD_STRIDE, stride);
                DBGT_PTRACE("Set Record Stride %d, Slice %u", stride,
                      (unsigned int)(mRecordInfo.getHeight()));

                mParameters.set(CameraParameters::KEY_RECORD_SLICE_HEIGHT, mRecordInfo.getHeight());
            } else {
                DBGT_PTRACE("Set Record Stride %d, Slice %u",
                      (int)(&(paramPortVPB2.format.video))->nStride,
                      (unsigned int)(&(paramPortVPB2.format.video))->nSliceHeight);
                mParameters.set(CameraParameters::KEY_RECORD_STRIDE, (&(paramPortVPB2.format.video))->nStride);
                mParameters.set(CameraParameters::KEY_RECORD_SLICE_HEIGHT, (&(paramPortVPB2.format.video))->nSliceHeight);
            }

            // Set the Overscanned params to 2Mp if 2Mp still during record is enabled
            // In case video stab is enabled these values should be over-written.
            if (mEnableB2R2DuringRecord) {
                // set the overscanned width and height
                // Just check if 2mp Video Size is supported,
                // else use the desired record width and height
                Vector<Size> sizes;
                bool is2MpVideoSizeSupported = false;
                mParameters.getSupportedVideoSizes(sizes);
                for (unsigned int ss = 0; ss < sizes.size(); ss++) {
                    if (sizes[ss].width == OVERSCAN_STILL_WIDTH_2MP) {
                        is2MpVideoSizeSupported = true;
                        break;
                    }
                }
                if (is2MpVideoSizeSupported) {
                    mRecordInfo.setOverScannedParams(OVERSCAN_STILL_WIDTH_2MP,
                                                     OVERSCAN_STILL_HEIGHT_2MP);
                }
            }
#ifdef ENABLE_VIDEO_STAB
            // set the overscanned width and height
            mRecordInfo.setOverScannedParams((&(paramPortVPB2.format.video))->nStride*2/3,
                                             (&(paramPortVPB2.format.video))->nSliceHeight);
#endif
        }
    }

    DBGT_PTRACE("Dumping params for Component %s Port %d",
            OmxUtils::name(aOmxComponent), aPortIndex);
    OmxUtils::dump(aPortDefType);

    return err;
}

/* static */
void *STECamera::B2R2ThreadWrapper(void* me)
{
    DBGT_PROLOG("");
    STECamera* mySTECam = static_cast <STECamera*> (me);
    mySTECam->B2R2ThreadEntry();
    DBGT_EPILOG("");
    return NULL;
}

void STECamera::B2R2ThreadEntry()
{
    DBGT_PROLOG("");

    while (1) {
        // Wait for recording frame
        sem_wait(&sem_b2r2);

        // Exit when record stops running
        if (!mRecordRunning) {
            //Reset  b2r2 Semaphore so that CameraFillBuferDone Thread can proceed
            sem_post(&sem_b2r2_reset);
            break;
        }

        if (mEnableB2R2DuringRecord) {
            // Frame is available do the processing
            doB2R2Processing(mRecordIdx);
        }

        // Synchronise the snapshot with the blitter
        if (mSnapshotDuringRecord && (mSnapshotDuringRecIdx == -1)) {
            mSnapshotDuringRecIdx = mRecordIdx;
            // signal the record semaphore
            DBGT_PTRACE("Snapshot during Record enabled");
            sem_post(&still_during_record);

            B2R2Utills::TImageData srcImage, intImage;
            srcImage.mWidth = mPreviewInfo.getOverScannedWidth();
            srcImage.mHeight = mPreviewInfo.getOverScannedHeight();
            srcImage.mBufSize = paramPortVPB0.nBufferSize;
            srcImage.mLogAddr = (void*)g_lastPreviewOMXBuffInfo->mMMHwBufferInfo.iLogAddr;
            srcImage.mPhyAddr = (void*)g_lastPreviewOMXBuffInfo->mMMHwBufferInfo.iPhyAddr;
            srcImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;


            intImage.mWidth = mConfigPreviewWidth;
            intImage.mHeight = mConfigPreviewHeight;
            intImage.mBufSize = paramISPoutput1.nBufferSize;
            intImage.mLogAddr = (void*)mRawOmxBuffInfo.mMMHwBufferInfo.iLogAddr;
            intImage.mPhyAddr = (void*)mRawOmxBuffInfo.mMMHwBufferInfo.iPhyAddr;
            intImage.mColorFmt = paramPortVPB0.format.video.eColorFormat;

            OMX_ERRORTYPE err = (OMX_ERRORTYPE)B2R2Utills::resize(&srcImage, &intImage);
            if (OMX_ErrorNone != err) {
                DBGT_CRITICAL("B2R2Utills::resize() failed");
            }

            // Signal the semaphore for the thumbnail
            DBGT_PTRACE("Signal the thumbnail semaphore");
            sem_post(&isp_snap_sem);
        }

        if (mEnableB2R2DuringRecord) {
            sem_post(&sem_crop_buffer);
        }
    }
    DBGT_EPILOG("");
}

void STECamera::doB2R2Processing(int aSrcIndex)
{
    int offset = 0, top = 0, left = 0, width = 0, height = 0;
    // Do B2R2 Operations
    DBGT_PDEBUG("doB2R2Processing In Buffer %d", aSrcIndex);

    //Reset  b2r2 Semaphore so that CameraFillBuferDone Thread can proceed
    sem_post(&sem_b2r2_reset);

    offset = (int)(mRecordOmxBuffInfo[mRecordIdx].mMMHwBufferInfo.iLogAddr -
        mRecordOmxBuffInfo[0].mMMHwBufferInfo.iLogAddr );

#ifdef ENABLE_VIDEO_STAB
    // Get the record crop vectors if video stab is enabled
    float stride = 1.5; // For YUV420MB
    int allignedFrameSize = OVERSCAN_STILL_WIDTH_2MP * OVERSCAN_STILL_HEIGHT_2MP * stride;
    AlignPow2<int>::up(allignedFrameSize, 4);
    DBGT_PTRACE("allignedFrameSize %d", allignedFrameSize);
    OMX_U8* pTmp = (((OMX_U8*)(mRecordOmxBuffInfo[aSrcIndex].mMMHwBufferInfo.iLogAddr)) + allignedFrameSize);
    // Get the recording crop vectors
    getCropVector(pTmp,
                  (uint32_t&)left,
                  (uint32_t&)top,
                  (uint32_t&)width,
                  (uint32_t&)height);
    width = STILL_WIDTH_2MP;
    height = STILL_HEIGHT_2MP;
#endif

    if (mCurrentZoomLevel != 0) {
        getCropVectorForZoom((int&)left,
                             (int&)top,
                             (size_t&)width,
                             (size_t&)height,
                             false);

    // set the source cropping zone
    mRecordBlitter->setCropSrc(left, top, width, height);
    } else {
#ifdef ENABLE_VIDEO_STAB
        // set the source cropping zone
        mRecordBlitter->setCropSrc(left, top, width, height);
#endif
    }

    mRecordBlitter->transform(
        mRecordOmxBuffInfo[mRecordIdx].mMMHwChunkMetaData.nFd, offset,
        mRecordOmxBuffInfo[mRecordIdx].mMMHwChunkMetaData.nChunkSize,
        mRecordMMNativeBuf[mRecordIdx]->getHandle(), 0,
        mRecordMMNativeBuf[mRecordIdx]->getBufferSize() );
    return;
}

#undef _CNAME_
#define _CNAME_ SceneModeController

// STECamera::SceneModeController implementation
STECamera::SceneModeController::SceneModeController(STECamera *pCameraHwItf)
{
    mCameraHwItf = pCameraHwItf;
    mInitialized = false;
}

STECamera::SceneModeController::~SceneModeController()
{
}

STECamera::SceneModeController::TData STECamera::SceneModeController::sProperties[] =
{
    // ESceneMode, EWhiteBalance, EAEMetering, EExpControl,
    // EExpSensitivity, EExpEvComp, EFlashControl, EFocusRange,
    // ESaturation, EImageFilter, ESharpness

    // EAuto
    TData(
        OMX_SYMBIAN_SceneAuto,
        kInvalidValue,
        kInvalidValue,
        OMX_ExposureControlAuto,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EBeach
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ESnow
    TData(
        OMX_STE_Scene_Snow,
        kInvalidValue,
        OMX_STE_MeteringModeBeachAndSnow,
        OMX_ExposureControlAuto,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EParty
    TData(
        OMX_STE_Scene_Party,
        kInvalidValue,
        OMX_STE_MeteringModeCenterWeighted,
        OMX_ExposureControlAuto,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EPortrait
    TData(
        OMX_SYMBIAN_ScenePortrait,
        kInvalidValue,
        OMX_STE_MeteringModePortrait,
        OMX_ExposureControlLargeAperture,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ENightPortrait
    TData(
        OMX_SYMBIAN_SceneNightPortrait,
        kInvalidValue,
        OMX_STE_MeteringModePortrait,
        OMX_ExposureControlSmallApperture,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ELandscape
    TData(
        OMX_SYMBIAN_SceneLandscape,
        kInvalidValue,
        OMX_STE_MeteringModeScenery,
        OMX_ExposureControlAuto,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ENight
    TData(
        OMX_SYMBIAN_SceneNight,
        kInvalidValue,
        OMX_STE_MeteringModeCenterWeighted,
        OMX_ExposureControlNight,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EAction
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ESunset
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ECandlelight
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EBacklight
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ECloseup
    TData(
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // ESports
    TData(
        OMX_SYMBIAN_SceneSport,
        kInvalidValue,
        OMX_STE_MeteringModeCenterWeighted,
        OMX_ExposureControlSports,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        ),
    // EDocument
    TData(
        OMX_STE_Scene_Document,
        kInvalidValue,
        OMX_STE_MeteringModeCenterWeighted,
        OMX_ExposureControlAuto,
        kInvalidValue, // means don't change ISO settings
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue,
        kInvalidValue
        )
};

int STECamera::SceneModeController::initialize()
{
    if (mInitialized) {
        return 0;
    }

    // initialize the values of various configuration parameters at runtime
    // by loading them from some config file: TODO
    mInitialized = true;
    return 0;
}

OMX_ERRORTYPE STECamera::SceneModeController::setSceneMode(STECamera::SceneModeController::Mode aMode)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    DBGT_ASSERT(aMode >= 0 && aMode < EMaxMode, "Invalid mode %d specified", aMode);
    CAM_ASSERT_COMPILE(EMaxMode == ARRAYCOUNT(sProperties));

    // scene mode
    if (value(aMode, ESceneMode) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_SCENEMODETYPE> sceneConfig;
        sceneConfig.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        sceneConfig.ptr()->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)value(aMode, ESceneMode);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

    }

    // set white balance
    if (value(aMode, EWhiteBalance) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_CONFIG_WHITEBALCONTROLTYPE> wbConfig;
        wbConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        wbConfig.ptr()->eWhiteBalControl = (OMX_WHITEBALCONTROLTYPE)value(aMode, EWhiteBalance);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonWhiteBalance), wbConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set exposure control
    if (value(aMode, EExpControl) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_CONFIG_EXPOSURECONTROLTYPE> expConfig;
        expConfig.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), expConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        expConfig.ptr()->eExposureControl = (OMX_EXPOSURECONTROLTYPE)value(aMode, EExpControl);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposure), expConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set exposure value
    if (value(aMode, EAEMetering) != kInvalidValue) {

        OmxUtils::StructContainer<OMX_CONFIG_EXPOSUREVALUETYPE> expValue;
        expValue.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), expValue.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        expValue.ptr()->eMetering = (OMX_METERINGTYPE)value(aMode, EAEMetering);
        if (value(aMode, EExpSensitivity) == kAutoSensitivity) {
            expValue.ptr()->bAutoSensitivity = OMX_TRUE;
        } else if (value(aMode, EExpSensitivity) != kInvalidValue) {
            expValue.ptr()->bAutoSensitivity = OMX_FALSE;
            expValue.ptr()->nSensitivity = (OMX_U32)value(aMode, EExpSensitivity);
        }
        //expValue.ptr()->bAutoAperture = OMX_FALSE;
        /* ER 355933 */
        expValue.ptr()->bAutoShutterSpeed = OMX_TRUE;
        /* ER 355933 */
        if (value(aMode, EExpEvComp) != kInvalidValue) {
            expValue.ptr()->xEVCompensation = (OMX_S32)value(aMode, EExpEvComp);
        }

        err = OMX_SetConfig(mCameraHwItf->mCam,
                    (OMX_INDEXTYPE)(OMX_IndexConfigCommonExposureValue), expValue.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set flash control
    if (value(aMode, EFlashControl) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE> flConfig;
        flConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl), flConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        flConfig.ptr()->eFlashControl = (OMX_IMAGE_FLASHCONTROLTYPE)value(aMode, EFlashControl);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                    mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::EFlashControl), flConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set focus range
    if (value(aMode, EFocusRange) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE> frConfig;
        frConfig.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), frConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        frConfig.ptr()->eFocusRange = (OMX_SYMBIAN_FOCUSRANGETYPE)value(aMode, EFocusRange);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::EFocusRange), frConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set saturation
    if (value(aMode, ESaturation) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_CONFIG_SATURATIONTYPE> satConfig;
        satConfig.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCameraHwItf->mCam, OMX_IndexConfigCommonSaturation, satConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        satConfig.ptr()->nSaturation = value(aMode, ESaturation);

        err = OMX_SetConfig(mCameraHwItf->mCam, OMX_IndexConfigCommonSaturation, satConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set image filter
    if (value(aMode, EImageFilter) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_CONFIG_IMAGEFILTERTYPE> imgFilter;
        imgFilter.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam, OMX_IndexConfigCommonImageFilter, imgFilter.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        imgFilter.ptr()->eImageFilter = (OMX_IMAGEFILTERTYPE)value(aMode, EImageFilter);

        err = OMX_SetConfig(mCameraHwItf->mCam, OMX_IndexConfigCommonImageFilter, imgFilter.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    // set sharpness
    if (value(aMode, ESharpness) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_S32TYPE> sharp;
        sharp.ptr()->nPortIndex = OMX_ALL;

        err = OMX_GetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness), sharp.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        sharp.ptr()->nValue = (OMX_S32)value(aMode, ESharpness);

        err = OMX_SetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESharpness), sharp.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;

}
OMX_ERRORTYPE STECamera::SceneModeController::setGamma(STECamera::SceneModeController::Mode aMode)
{
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;
    DBGT_ASSERT(aMode >= 0 && aMode < EMaxMode, "Invalid mode %d specified", aMode);
    CAM_ASSERT_COMPILE(EMaxMode == ARRAYCOUNT(sProperties));

    //set Gamma
    if (value(aMode, ESceneMode) != kInvalidValue) {
        OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_SCENEMODETYPE> sceneConfig;
        sceneConfig.ptr()->nPortIndex = OMX_ALL;
        err = OMX_GetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_GetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        sceneConfig.ptr()->eSceneType = (OMX_SYMBIAN_SCENEMODETYPE)value(aMode, ESceneMode);
        DBGT_PTRACE("setGamma scenetype =%d",sceneConfig.ptr()->eSceneType);
        err = OMX_SetConfig(mCameraHwItf->mCam,
                mCameraHwItf->mOmxILExtIndex->getIndex(OmxILExtIndex::ESceneMode), sceneConfig.ptr());
        if (OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_SetConfig failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    DBGT_EPILOG("");
    return err;

}

}; // namespace android
