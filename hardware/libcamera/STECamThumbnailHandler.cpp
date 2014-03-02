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
//Multimedia Includes
#include <IFM_Types.h>

//Internal Includes
#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 1
#define DBGT_PREFIX "Thumb "

#include "STECamTrace.h"
#include "STECamera.h"
#include "STECamOmxUtils.h"
#include "STECamThumbnailHandler.h"

namespace android {

char componentNameArmIv[] = "OMX.STE.VISUALPROCESSOR.2D-OPERATIONS.SW";
char componentNameSwJpegEnc[] = "OMX.ST.VFM.SWJPEGEnc";

OMX_ERRORTYPE ArmIvEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
                                OMX_U32 nData2, OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);

    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p", OmxUtils::name(eEvent), eEvent, nData1, nData2, pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

    EnableDataFlow *parent = static_cast<EnableDataFlow *>(pAppData);

    switch (eEvent) {
        case OMX_EventCmdComplete: {
            OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
            DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

            if (cmd == OMX_CommandStateSet) {
                OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                switch (newState) {
                    case OMX_StateLoaded:
                        parent->mStateArmIvSem.signal();
                        break;

                    case OMX_StateIdle:
                        parent->mStateArmIvSem.signal();
                        break;

                    case OMX_StateExecuting:
                        parent->mStateArmIvSem.signal();
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
            } else if (OMX_CommandPortEnable == cmd)
                parent->mStateArmIvSem.signal();
            else if (OMX_CommandPortDisable == cmd)
                parent->mStateArmIvSem.signal();

            break;
        }
        case OMX_EventError:
            DBGT_PTRACE("Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ", (OMX_ERRORTYPE) nData1, (OMX_ERRORTYPE) nData1, (OMX_ERRORTYPE) nData2, (OMX_ERRORTYPE) nData2);
            break;
        case OMX_EventBufferFlag: {
            if ((nData1 ==  1 ) &&  (nData2 & OMX_BUFFERFLAG_EOS)) {
                parent->mArmIvEosSem.signal();
                parent->mPerfManager.logAndDump(PerfManager::EArmIVEOS);
            }
            break;
        }
        default:
            DBGT_PTRACE("Unknown event: %d", eEvent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE SWJpegEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1,
                                 OMX_U32 nData2, OMX_PTR pEventData)
{
    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);
    DBGT_PTRACE("Event: %s(%d) Data1: %lu Data2: %lu EventData: %p", OmxUtils::name(eEvent), eEvent, nData1, nData2, pEventData);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

    EnableDataFlow *parent = static_cast<EnableDataFlow *>(pAppData);

    switch (eEvent) {
        case OMX_EventCmdComplete: {
            OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
            DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

            if (cmd == OMX_CommandStateSet) {
                OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);

                switch (newState) {
                    case OMX_StateLoaded:
                        parent->mStateSWJpegSem.signal();
                        break;

                    case OMX_StateIdle:
                        parent->mStateSWJpegSem.signal();
                        break;

                    case OMX_StateExecuting:
                        parent->mStateSWJpegSem.signal();
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
            } else if (OMX_CommandPortDisable == cmd)
                parent->mStateSWJpegSem.signal();
            else if (OMX_CommandPortEnable == cmd)
                parent->mStateSWJpegSem.signal();

            break;
        }
        case OMX_EventError:
            DBGT_CRITICAL("Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ", (OMX_ERRORTYPE) nData1, (OMX_ERRORTYPE) nData1, (OMX_ERRORTYPE) nData2, (OMX_ERRORTYPE) nData2);
            break;
        case OMX_EventBufferFlag: {
            if ((nData1 == 1 ) ) {
                parent->mSwJpegEosSem.signal();
                parent->mPerfManager.logAndDump(PerfManager::ESwJpegEOS);
            }

            break;
        }
        default:
            DBGT_PTRACE("Unknown event: %d", eEvent);
    }
    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ArmIVEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_PTR pAppData,
                                   OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    //NULL checking
    DBGT_ASSERT(NULL != hComponent, "Invalid handle");
    DBGT_ASSERT(NULL != pBuffer, "Invalid buffer");

    DBGT_PTRACE("ArmIVEmptyBufferDone empty buffer done \n");

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

//forward class declarations
#undef _CNAME_
#define _CNAME_ ThumbnailHandler

ThumbnailHandler::~ThumbnailHandler()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}


//forward class declarations
#undef _CNAME_
#define _CNAME_ EnableDataFlow

EnableDataFlow::EnableDataFlow (OMX_HANDLETYPE &aExifMixer,
                                OmxBuffInfo &aArmIvOmxBuffInfo,
                                OMX_OSI_CONFIG_SHARED_CHUNK_METADATA &aChunkData_ArmIvProc,
                                OmxUtils &aOmxUtils,
                                const OMX_COLOR_FORMATTYPE &aPreviewOmxColorFormat,
                                PerfManager &aPerfManager,
                                sem_t &aStateExifSem) :

                                mExifMixer(aExifMixer),
                                mArmIvOmxBuffInfo(aArmIvOmxBuffInfo),
                                mChunkData_ArmIvProc(aChunkData_ArmIvProc),
                                mOmxUtils(aOmxUtils),
                                mPreviewOmxColorFormat(aPreviewOmxColorFormat),
                                mPerfManager(aPerfManager),
                                mStateExifSem(aStateExifSem)
{
    DBGT_PROLOG("");

    mArmIvProc = NULL;
    mSwJpegEnc = NULL;
    mArmIvInBuffer = NULL;
    mPortsEnabled = true;

    DBGT_EPILOG("");
}

EnableDataFlow::~EnableDataFlow()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_ERRORTYPE EnableDataFlow::init(const ImageInfo &aThumbnailInfo,
                                   const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

     //Initialize Port definitions structures
    OmxUtils::initialize(mParamArmIvInput, OMX_PortDomainVideo, 0);
    OmxUtils::initialize(mParamArmIvOutput, OMX_PortDomainVideo, 1);
    OmxUtils::initialize(mParamSwJpegInput, OMX_PortDomainImage, 0);
    OmxUtils::initialize(mParamSwJpegOutput, OMX_PortDomainImage, 1);
    OmxUtils::initialize(mParamExifInput, OMX_PortDomainImage, 0);

    //Initialize Semaphores
    status_t err1 = mStateArmIvSem.init(1);
    if(-1 == err1) {
        DBGT_CRITICAL("mStateArmIvSem init failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = mStateSWJpegSem.init(1);
    if(-1 == err1) {
        DBGT_CRITICAL("mStateSWJpegSem init failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = mArmIvEosSem.init(1);
    if(-1 == err1) {
        DBGT_CRITICAL("mArmIvEosSem init failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    err1 = mSwJpegEosSem.init(1);
    if(-1 == err1) {
        DBGT_CRITICAL("mSwJpegEosSem init failed - OMX_ErrorInsufficientResources");
        DBGT_EPILOG("");
        return OMX_ErrorInsufficientResources;
    }

    //Initialize ArmIv & SwJpeg Enc Callbacks & Get Handle
    armivcallback.EmptyBufferDone = ArmIVEmptyBufferDone;
    armivcallback.FillBufferDone = 0;
    armivcallback.EventHandler = ArmIvEventHandler;

    err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mArmIvProc, componentNameArmIv, this, &armivcallback);
    if(OMX_ErrorNone != err ||  NULL == mArmIvProc) {
        DBGT_CRITICAL("ivproc GetHandle failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    swjpegcallback.EmptyBufferDone = 0;
    swjpegcallback.FillBufferDone = 0;
    swjpegcallback.EventHandler = SWJpegEventHandler;

    err = (mOmxUtils.interface()->GetpOMX_GetHandle())(&mSwJpegEnc, componentNameSwJpegEnc, this, &swjpegcallback);
    if(OMX_ErrorNone != err ||  NULL == mSwJpegEnc) {
        DBGT_CRITICAL("swjpegenc GetHandle failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = configPorts(aThumbnailInfo, aPreviewInfo);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("configPorts() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //Setup Tunneling between armiv-swjpeg-exifmixer
    err = setupTunneling();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("configPorts() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = disablePorts();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("configPorts() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //Send armiv & swjpeg enc to executing state with all the ports disabled
    err = sendComponentToExecuting();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("sendComponentToExecuting failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::freeHandles()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_ASSERT(NULL != mArmIvProc, "mArmIvProc is NULL");
    DBGT_ASSERT(NULL != mSwJpegEnc, "mSwJpegEnc is NULL");

    DBGT_PTRACE("Sending ArmIv to unloaded State");
    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mArmIvProc);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle ArmIVProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mArmIvProc = NULL ;

    DBGT_PTRACE("Sending SwJpegEnc to unloaded State");
    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mSwJpegEnc);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("GetpOMX_FreeHandle SwJpegenc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mSwJpegEnc = NULL ;

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::resetComponents()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("Sending Command StateIdle to SwJpegEnc");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for SwJpegEnc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command StateIdle to ArmIv");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateIdle failed for ArmIVProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();
    mStateSWJpegSem.wait();

    DBGT_PTRACE("Sending Command StateLoaded to ArmIv");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for ArmIVProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command StateLoaded to SwJpegEnc");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Sending OMX_StateLoaded failed for SwJpegEnc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = freeBuffers();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Free buffers err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateSWJpegSem.wait();
    mStateArmIvSem.wait();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::freeBuffers()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (NULL != mArmIvInBuffer) {
        err = OMX_FreeBuffer(mArmIvProc, (OMX_U32)( 0 ), mArmIvInBuffer);
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("OMX_FreeBuffer failed for ArmIVProc input port err = %d", err);
            DBGT_EPILOG("");
            return err;
        }

        mArmIvInBuffer = NULL;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::configure(const ImageInfo &aThumbnailInfo,
                                        const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /* check whether mArmIvProc and mSwJpegEnc are in executing state or not,
     * if not, send them to executing first.
     * This being the case when without doing setParameters(), startPreview()
     * is called. As thumbnailhandler is setup for disableDataFlow, when a
     * setParameters() is done with some valid thumbnail size, enableDataFlow's
     * sendComponentToExecuting() is not called leading to a deadlock.
     */
    OMX_STATETYPE armIvProcState, swJpegEncState;
    OMX_GetState(mArmIvProc, &armIvProcState);
    OMX_GetState(mSwJpegEnc, &swJpegEncState);

    if(OMX_StateExecuting != armIvProcState || OMX_StateExecuting != swJpegEncState){
        err = sendComponentToExecuting();
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("sendComponentToExecuting failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    err = configPorts(aThumbnailInfo, aPreviewInfo);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("configPorts() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = enablePorts();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("enablePorts failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::updateSnapShot(const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mArmIvProc Port 0");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortDisable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 0 mArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = freeBuffers();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("freeBuffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();

    /****************************************************************************/
    //   Initialize ARM IV input
    /****************************************************************************/
    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port0  failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamArmIvInput.nBufferCountActual = 1;     // the index of the input port. Should be modified.

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE *pt_armivinput = &(mParamArmIvInput.format.video);

    pt_armivinput->cMIMEType = (OMX_STRING)"";
    pt_armivinput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_armivinput->nFrameWidth = (OMX_U32)aPreviewInfo.getAlignedWidth();
    pt_armivinput->nFrameHeight = (OMX_U32)aPreviewInfo.getAlignedHeight();
    pt_armivinput->nStride = (OMX_U32)(2 * aPreviewInfo.getAlignedWidth());
    pt_armivinput->nSliceHeight = (OMX_U32)aPreviewInfo.getAlignedHeight();
    pt_armivinput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_armivinput->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_armivinput->eColorFormat = mPreviewOmxColorFormat;
    pt_armivinput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition for mArmIvProc port0  failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port0  failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //dump
    DBGT_PTRACE("Dumping params for ARM IV input");
    OmxUtils::dump(mParamArmIvInput);

    DBGT_PTRACE("Sending Command OMX_CommandPortEnable for mArmIvProc Port 0");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortEnable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Enable 0 mArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = useBuffer();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Use buffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::release()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = disablePorts();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("disablePorts() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::handleBuffer(OMX_U32 aFilledLen, OMX_U32 aFlags)
{
    DBGT_PROLOG("aFilledLen: 0x%08x aFlags: 0x%08x", (unsigned int)aFilledLen, (unsigned int)aFlags);

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mArmIvInBuffer->nFilledLen = aFilledLen;
    mArmIvInBuffer->nFlags = aFlags;

    err = OMX_EmptyThisBuffer(mArmIvProc, mArmIvInBuffer);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_EmptyThisBuffer failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void EnableDataFlow::handleEvent()
{
    DBGT_PROLOG("");

    DBGT_PTRACE("Waiting for ArmIv EOS");
    mArmIvEosSem.wait();

    DBGT_PTRACE("Waiting for SwJpegEnc EOS");
    mSwJpegEosSem.wait();

    DBGT_EPILOG("");
}

OMX_ERRORTYPE EnableDataFlow::setAutoRotate(OMX_BOOL aEnaleAutoRotate)
{
    DBGT_PROLOG("aEnaleAutoRotate: %d", aEnaleAutoRotate);

    DBGT_ASSERT(NULL != mArmIvProc, "mArmIvProc is NULL");
    DBGT_ASSERT(1 == mParamArmIvOutput.nPortIndex, "mParamArmIvOutput.nPortIndex is invalid");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    OMX_INDEXTYPE index;

    OmxUtils::StructContainer<OMX_SYMBIAN_CONFIG_BOOLEANTYPE> autoRotate;

    err = OMX_GetExtensionIndex(mArmIvProc, (char *)OMX_SYMBIAN_INDEX_CONFIG_COMMON_AUTOROTATIONCONTROL_NAME, &index);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Still Auto Rotation OMX_GetExtensionIndex failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    autoRotate.ptr()->nPortIndex = mParamArmIvOutput.nPortIndex;

    err = OMX_GetConfig(mArmIvProc, index, autoRotate.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Auto Rotation OMX_GetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    autoRotate.ptr()->bEnabled = aEnaleAutoRotate;

    err = OMX_SetConfig(mArmIvProc, index, autoRotate.ptr());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Auto Rotation OMX_SetConfig failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::sendComponentToExecuting()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("Sending Command StateIdle to ArmIv");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mArmIvProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command StateIdle to SWJpeg");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet IDLE failed for mSwJpegEnc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();
    mStateSWJpegSem.wait();

    DBGT_PTRACE("Sending Command StateExecuting to ArmIv");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandStateSet, OMX_StateExecuting, NULL);
     if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mArmIvProc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command StateExecuting to SWJpeg");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandStateSet, OMX_StateExecuting, NULL);
     if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_CommandStateSet EXECUTING failed for mSwJpegEnc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }


    mStateArmIvSem.wait();
    mStateSWJpegSem.wait();

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::setupTunneling()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //make tunnel between output port1 of ARMIV Proc and input port of SW JPEG Enc
    DBGT_PTRACE("ARM IV  port: %lu SW JPEG ENcport: %lu", mParamArmIvOutput.nPortIndex, mParamSwJpegInput.nPortIndex);
    err = (mOmxUtils.interface()->GetpOMX_SetupTunnel())(mArmIvProc, mParamArmIvOutput.nPortIndex,
            mSwJpegEnc, mParamSwJpegInput.nPortIndex);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL(" OMX_SetupTunnel failed between ARMIV Proc and SW JPEGEnc err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //make tunnel between Output port of  SWJPEGENC with input port 0 of EXIF MIxer
    DBGT_PTRACE("SW JPEG ENcport: %lu EXIF Mixer POrt: %lu", mParamSwJpegOutput.nPortIndex, mParamExifInput.nPortIndex);
    err = (mOmxUtils.interface()->GetpOMX_SetupTunnel())(mSwJpegEnc, mParamSwJpegOutput.nPortIndex,
            mExifMixer, mParamExifInput.nPortIndex);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL(" OMX_SetupTunnel failed between SWJPEGENC and EXIFMIXER err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::configPorts(const ImageInfo &aThumbnailInfo,
                                          const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("Thumbnail Width: %d Height: %d \nPreview Width: %d Heidht: %d\n",
         aThumbnailInfo.getWidth(), aThumbnailInfo.getHeight(),
         aPreviewInfo.getAlignedWidth(), aPreviewInfo.getAlignedHeight());

    DBGT_ASSERT(0 != aThumbnailInfo.getWidth(), "invalid width");
    DBGT_ASSERT(0 != aThumbnailInfo.getHeight(), "invalid height");
    DBGT_ASSERT(0 != aPreviewInfo.getAlignedWidth(), "invalid Preview Width");
    DBGT_ASSERT(0 != aPreviewInfo.getAlignedHeight(), "invalid Preview Height");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    /****************************************************************************/
    //   Initialize ARM IV input
    /****************************************************************************/
    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port0  failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamArmIvInput.nBufferCountActual = 1;     // the index of the input port. Should be modified.

    //Here, the min number of buffers to be used is retrieved
    OMX_VIDEO_PORTDEFINITIONTYPE *pt_armivinput = &(mParamArmIvInput.format.video);

    pt_armivinput->cMIMEType = (OMX_STRING)"";
    pt_armivinput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;

    pt_armivinput->nFrameWidth = (OMX_U32)aPreviewInfo.getAlignedWidth();
    pt_armivinput->nFrameHeight = (OMX_U32)aPreviewInfo.getAlignedHeight();
    pt_armivinput->nStride = (OMX_U32)(2 * aPreviewInfo.getAlignedWidth());
    pt_armivinput->nSliceHeight = (OMX_U32)aPreviewInfo.getAlignedHeight();
    pt_armivinput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_armivinput->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_armivinput->eColorFormat = mPreviewOmxColorFormat;
    pt_armivinput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition for mArmIvProc port0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //dump
    DBGT_PTRACE("Dumping params for ARM IV input");
    OmxUtils::dump(mParamArmIvInput);
    //***********Set Rotation value in ARMIVPRoc
    if((aThumbnailInfo.getRotation() == 90)||(aThumbnailInfo.getRotation() == 270)){
        OmxUtils::StructContainer<OMX_CONFIG_ROTATIONTYPE> rfactor;
        rfactor.ptr()->nPortIndex = 1;
        rfactor.ptr()->nRotation = (aThumbnailInfo.getRotation()+180)%360;
        err = OMX_SetConfig(mArmIvProc, OMX_IndexConfigCommonRotate, rfactor.ptr());
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("SetConfig for mArmIvProc port1 failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }
    //***********
    /****************************************************************************/
    //   Initialize ARM IV output
    /****************************************************************************/
    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamArmIvOutput.nBufferCountActual = 1;

    OMX_VIDEO_PORTDEFINITIONTYPE    *pt_armivoutput = &(mParamArmIvOutput.format.video);

    pt_armivoutput->cMIMEType = (OMX_STRING)"";
    pt_armivoutput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_armivoutput->nFrameWidth = (OMX_U32) aThumbnailInfo.getAlignedWidth();
    pt_armivoutput->nFrameHeight = (OMX_U32)aThumbnailInfo.getAlignedHeight();
    pt_armivoutput->nSliceHeight = (OMX_U32)aThumbnailInfo.getAlignedHeight();
    pt_armivoutput->nStride = 0;
    pt_armivoutput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_armivoutput->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_armivoutput->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY;
    pt_armivoutput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition for mArmIvProc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &mParamArmIvOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mArmIvProc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //dump
    DBGT_PTRACE("Dumping params for ARM IV output");
    OmxUtils::dump(mParamArmIvOutput);

    /****************************************************************************/
    //   Initialize SW JPEG Encoder Input for DCF thumbnail
    /****************************************************************************/
    err = OMX_GetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mSwJpegEnc port0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamSwJpegInput.nBufferCountActual = 1;     // the index of the input port. Should be modified.

    OMX_IMAGE_PORTDEFINITIONTYPE *pt_swjpeginput = &(mParamSwJpegInput.format.image);

    pt_swjpeginput->cMIMEType       = (OMX_STRING)"";
    pt_swjpeginput->pNativeRender    = (OMX_NATIVE_DEVICETYPE)NULL;


    pt_swjpeginput->nFrameWidth = (OMX_U32) aThumbnailInfo.getAlignedWidth();
    pt_swjpeginput->nFrameHeight = (OMX_U32)aThumbnailInfo.getAlignedHeight();
    pt_swjpeginput->nStride = (OMX_U32) (aThumbnailInfo.getAlignedWidth() * 3 / 2);
    pt_swjpeginput->nSliceHeight = (OMX_U32)aThumbnailInfo.getAlignedHeight();
    pt_swjpeginput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_swjpeginput->eCompressionFormat    = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_swjpeginput->eColorFormat          = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY;
    pt_swjpeginput->pNativeWindow         = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition for mSwJpegEnc port0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mSwJpegEnc port0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    //dump
    DBGT_PTRACE("Dumping params for mParamSwJpegInput");
    OmxUtils::dump(mParamSwJpegInput);

    /****************************************************************************/
    //   Initialize SW JPEG Encoder Ouput for DCF thumbnail
    /****************************************************************************/
    if (aThumbnailInfo.invalidQuality() != aThumbnailInfo.getQuality()) {
        //setup thumbnail quality
        OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
        qfactor.ptr()->nPortIndex = 1;
        qfactor.ptr()->nQFactor = aThumbnailInfo.getQuality();

        DBGT_PTRACE("Setting JPEG Thumbnail Quality: %lu ", qfactor.ptr()->nQFactor);

        err = OMX_SetConfig(mSwJpegEnc, OMX_IndexParamQFactor, qfactor.ptr());
        if(OMX_ErrorNone != err) {
            DBGT_CRITICAL("SetConfig mSwJpegEnc port1 failed err = %d", err);
            DBGT_EPILOG("");
            return err;
        }
    }

    err = OMX_GetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mSwJpegEnc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamSwJpegOutput.nBufferCountActual = 1;     // the index of the input port. Should be modified.
    OMX_IMAGE_PORTDEFINITIONTYPE *pt_swjpegoutput = &(mParamSwJpegOutput.format.image);

    pt_swjpegoutput->cMIMEType = (OMX_STRING)"";
    pt_swjpegoutput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;

    pt_swjpegoutput->nFrameWidth = (OMX_U32)aThumbnailInfo.getWidth();
    pt_swjpegoutput->nFrameHeight = (OMX_U32)aThumbnailInfo.getHeight();

    pt_swjpegoutput->nStride = (OMX_U32)(aThumbnailInfo.getWidth() * 3 / 2);
    pt_swjpegoutput->nSliceHeight = (OMX_U32)aThumbnailInfo.getHeight();
    pt_swjpegoutput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_swjpegoutput->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingJPEG;
    pt_swjpegoutput->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
    pt_swjpegoutput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter OMX_IndexParamPortDefinition for mSwJpegEnc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = STECamera::cropOutputSize(mSwJpegEnc, aThumbnailInfo.getWidth(), aThumbnailInfo.getHeight());
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("cropOutputSize for mSwJpegEnc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = OMX_GetParameter(mSwJpegEnc, OMX_IndexParamPortDefinition, &mParamSwJpegOutput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter OMX_IndexParamPortDefinition for mSwJpegEnc port1 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Dumping params for mParamSwJpegOutput");
    OmxUtils::dump(mParamSwJpegOutput);

    /****************************************************************************/
    // Reconfigure ExifMixer port 0
    /****************************************************************************/
    err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &mParamExifInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter mExifMixer port 0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mParamExifInput.nBufferCountActual = 1;     // the index of the input port. Should be modified.

    OMX_IMAGE_PORTDEFINITIONTYPE *pt_exifinput1 = &(mParamExifInput.format.image);

    pt_exifinput1->cMIMEType = (OMX_STRING)"";
    pt_exifinput1->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_exifinput1->nFrameWidth = (OMX_U32)aThumbnailInfo.getWidth();
    pt_exifinput1->nFrameHeight = (OMX_U32)aThumbnailInfo.getHeight();
    pt_exifinput1->nStride = (OMX_U32)(aThumbnailInfo.getWidth() * 3 / 2);
    pt_exifinput1->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_exifinput1->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingJPEG;
    pt_exifinput1->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
    pt_exifinput1->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err = OMX_SetParameter(mExifMixer, OMX_IndexParamPortDefinition, &mParamExifInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Setparameter mExifMixer port 0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }
    //dump

    err = OMX_GetParameter(mExifMixer, OMX_IndexParamPortDefinition, &mParamExifInput);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Getparameter mExifMixer port 0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Dumping params for mParamExifInput");
    OmxUtils::dump(mParamExifInput);

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::disablePorts()
{
    DBGT_PROLOG("");

    if (!mPortsEnabled) {
        return OMX_ErrorNone;
   }


    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mArmIvProc Port 0");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortDisable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 0 mArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = freeBuffers();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("freeBuffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mSwJpegEnc Port 0");

    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandPortDisable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 0 mSwJpegEnc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mArmIvProc Port 1");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortDisable, 1, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 1 mArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();
    mStateSWJpegSem.wait();

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mSwJpegEnc Port 1");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandPortDisable, 1, NULL);

    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 1 mSwJpegEnc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command OMX_CommandPortDisable for mExifMixer Port 0");
    err = OMX_SendCommand(mExifMixer, OMX_CommandPortDisable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Disable 1 mExifMixer failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    camera_sem_wait(&mStateExifSem, SEM_WAIT_TIMEOUT);
    mStateSWJpegSem.wait();

    mPortsEnabled = false;

    DBGT_EPILOG("");
    return err;
}


OMX_ERRORTYPE EnableDataFlow::enablePorts()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("Sending Command Port Enable for mExifMixer Port 0");
    err = OMX_SendCommand(mExifMixer, OMX_CommandPortEnable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 1 mExifMixer failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command Port Enable for SwJpegEnc Port 1");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandPortEnable, 1, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 1 SwJpegEnc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateSWJpegSem.wait();
    camera_sem_wait(&mStateExifSem, SEM_WAIT_TIMEOUT);

    DBGT_PTRACE("Sending Command Port Enable for ArmIvProc Port 1");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortEnable, 1, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 1 ArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_PTRACE("Sending Command Port Enabled for SwJpegEnc Port 0");
    err = OMX_SendCommand(mSwJpegEnc, OMX_CommandPortEnable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 0 SwJpegEnc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();
    mStateSWJpegSem.wait();

    DBGT_PTRACE("Sending Command OMX_CommandPortEnable for mArmIvProc Port 0");
    err = OMX_SendCommand(mArmIvProc, OMX_CommandPortEnable, 0, NULL);
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Port Enable 0 mArmIvProc failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    err = useBuffer();
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("Use buffers failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mStateArmIvSem.wait();

   mPortsEnabled = true;

    DBGT_EPILOG("");
    return err;
}

OMX_ERRORTYPE EnableDataFlow::useBuffer()
{
    DBGT_PROLOG("");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    DBGT_PTRACE("\n Calling OMX_SetConfig before OMX_UseBuffer ArmIV  Chunk\n");
    err = OMX_SetConfig(mArmIvProc, (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, &mChunkData_ArmIvProc);

    if (err != OMX_ErrorNone) {
        DBGT_PTRACE("OMX_SetConfig Failed  \n");
    }

    mArmIvInBuffer = NULL;
    DBGT_PTRACE("usebuffer mArmIvProc ");

    err = OMX_UseBuffer(mArmIvProc,    // hComponent
                        &mArmIvInBuffer,    // address where ptr to buffer header will be stored
                        0,          // port index (for port for which buffer is provided)
                        (void *)(&mArmIvOmxBuffInfo),    // App. private data = pointer to beginning of allocated data
                        // to have a context when component returns with a callback (i.e. to know
                        // what to free etc.
                        (OMX_U32)(mParamArmIvInput.nBufferSize),        // buffer size
                        (OMX_U8 *)(mArmIvOmxBuffInfo.mMMHwBufferInfo.iLogAddr) // buffer data ptr
                       );

    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_UseBuffer failed for mArmIvProc port 0 failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_ASSERT(NULL != mArmIvInBuffer->pBuffer, "Buffer is NULL");

    mArmIvOmxBuffInfo.mCamMemoryBase.get()->setOmxBufferHeaderType(mArmIvInBuffer);

    DBGT_EPILOG("");
    return err;
}

//forward class declarations
#undef _CNAME_
#define _CNAME_ DisableDataFlow

DisableDataFlow::~DisableDataFlow()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

OMX_ERRORTYPE DisableDataFlow::configure(const ImageInfo &aThumbnailInfo,
                                         const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::updateSnapShot(const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::release()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::handleBuffer(OMX_U32 aFilledLen, OMX_U32 aFlags)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

void DisableDataFlow::handleEvent()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
}

OMX_ERRORTYPE DisableDataFlow::init(const ImageInfo &aThumbnailInfo,
                   const ImageInfo &aPreviewInfo)
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::resetComponents()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::sendComponentToExecuting()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE DisableDataFlow::freeBuffers()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
    return OMX_ErrorNone;
}

} //namespace android
