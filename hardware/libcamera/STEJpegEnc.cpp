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
#define DBGT_LAYER 2
#define DBGT_PREFIX "JpgEnc"

#include "STECamTrace.h"

//Internal includes
#include "STECamOmxUtils.h"
#include "STECamFrameRateDumper.h"
#include "STECamPerfManager.h"
#include "STECamOmxILExtIndex.h"
#include "STEExtIspCamera.h"
#include "STEJpegEnc.h"
#include "STECamUtils.h"

namespace android {

OMX_ERRORTYPE omxJpegEncEventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
                                    OMX_EVENTTYPE eEvent,OMX_U32 nData1,
                                    OMX_U32 nData2, OMX_PTR pEventData);

OMX_ERRORTYPE omxJpegEncFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                    OMX_IN OMX_PTR pAppData,
                                    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

OMX_ERRORTYPE omxJpegEncEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                         OMX_IN OMX_PTR pAppData,
                                         OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

#undef _CNAME_
#define _CNAME_ SteJpegEnc

SteJpegEnc::SteJpegEnc( int width,
                        int height,
                        int quality,
                        int inBufferSize,
                        OMX_IMAGE_CODINGTYPE codingType,
                        OMX_COLOR_FORMATTYPE format ) {

    timestamp_begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

    DBGT_PROLOG("");
    char swjpegenc[] = "OMX.ST.VFM.SWJPEGEnc";
    char jpegenc[] = "OMX.ST.VFM.JPEGEnc";
    OMX_ERRORTYPE err = OMX_ErrorNone;

    //initialize omxutils
    err = mOmxUtils.init();
    if(err != OMX_ErrorNone){
        DBGT_CRITICAL("can't initialize omx utils");
        return;
    }

    err = (mOmxUtils.interface()->GetpOMX_Init())();
    if(err != OMX_ErrorNone){
        DBGT_CRITICAL("can't initialize omx utils interface");
        return;
    }

    mJpegCallback.EmptyBufferDone = omxJpegEncEmptyBufferDone;
    mJpegCallback.FillBufferDone = omxJpegEncFillBufferDone;
    mJpegCallback.EventHandler = omxJpegEncEventHandler;

    // use hardware jpeg encoder for yuv420MB frame, use sw jpeg encoder for other formats
    switch ((uint32_t)format){
        case (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mJpegEnc,jpegenc, this, &mJpegCallback);
        break;
        default:
            err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mJpegEnc,swjpegenc, this, &mJpegCallback);
        break;
    }

    if(err == OMX_ErrorNone &&  mJpegEnc != 0){
        DBGT_PTRACE("JpegEnc GetHandle Done for JPEGENC ");
    }else{
        DBGT_CRITICAL("JpegEnc GetHandle failed err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
    }

    sem_init(&mStateSem , 0 , 0);
    sem_init(&mSetPortCamSem , 0 , 0);
    sem_init(&mJpegEncSem , 0 , 0);

    mWidth = width;
    mHeight = height;
    mformat = format;
    if(codingType == OMX_IMAGE_CodingEXIF)
        mIsExifInJpeg = true;
    else
        mIsExifInJpeg = false;

    // align to 16 the width & height
    AlignPow2<int>::up(mWidth, 16);
    AlignPow2<int>::up(mHeight, 16);

    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramJPEGinput);
    OmxUtils::StructWrapper<OMX_PARAM_PORTDEFINITIONTYPE>::init(paramJPEGoutput);

    // set input & output ports
    OmxUtils::initialize(paramJPEGinput, OMX_PortDomainImage, 0);

    // the index of the input port. Should be modified.
    err =OMX_GetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGinput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 JPEGENC passed");
    }else{
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 JPEGENC failed");
    }

    // Here, the min number of buffers to be used is retrieved
    paramJPEGinput.nBufferCountActual = 1;
    OMX_IMAGE_PORTDEFINITIONTYPE *pt_jpeginput = &(paramJPEGinput.format.image);
    pt_jpeginput->cMIMEType = (OMX_STRING)"";
    pt_jpeginput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_jpeginput->nFrameWidth = (OMX_U32) mWidth;
    pt_jpeginput->nFrameHeight = (OMX_U32) mHeight;
    pt_jpeginput->nStride = (OMX_U32) 1;
    pt_jpeginput->nSliceHeight = 16;
    pt_jpeginput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
    pt_jpeginput->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
    pt_jpeginput->eColorFormat = (OMX_COLOR_FORMATTYPE)format;
    pt_jpeginput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err =OMX_SetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGinput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 0 JPEG ENC passed ");
    }else{
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 0 JPEG ENC failed ");
    }

    err =OMX_GetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGinput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 JPEG ENC passed");
    }else{
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 JPEG ENC failed");
    }

    OmxUtils::initialize(paramJPEGoutput, OMX_PortDomainImage, 1);
    // the index of the output port. Should be modified.
    err =OMX_GetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGoutput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 JPEGENC passed");
    }else{
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 JPEGENC failed");
    }

    paramJPEGoutput.nBufferCountActual = 1;
    // Here, the min number of buffers to be used is retrieved
    OMX_IMAGE_PORTDEFINITIONTYPE *pt_jpegoutput = &(paramJPEGoutput.format.image);
    pt_jpegoutput->cMIMEType = (OMX_STRING)"";
    pt_jpegoutput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
    pt_jpegoutput->nFrameWidth = (OMX_U32) mWidth;
    pt_jpegoutput->nFrameHeight = (OMX_U32) mHeight;
    pt_jpegoutput->nStride = (OMX_U32) mWidth;
    pt_jpegoutput->nSliceHeight = 16;
    pt_jpegoutput->bFlagErrorConcealment= (OMX_BOOL)OMX_FALSE;
    pt_jpegoutput->eCompressionFormat = (OMX_IMAGE_CODINGTYPE)codingType;
    pt_jpegoutput->eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused ;
    pt_jpegoutput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

    err =OMX_SetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGoutput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 1 JPEG ENC passed ");
    }else{
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 JPEG ENC failed ");
    }

    // Set the cropping zone
    OmxUtils::StructContainer<OMX_CONFIG_RECTTYPE> qrect;

    err = OMX_GetConfig(mJpegEnc, OMX_IndexConfigCommonOutputCrop, qrect.ptr());
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Get cropping zone for mJpegEnc passed");
    }else{
        DBGT_CRITICAL("Get cropping zone for mJpegEnc failed %x",err);
    }

    qrect.ptr()->nPortIndex = 1;
    qrect.ptr()->nLeft = 0;
    qrect.ptr()->nTop = 0;
    qrect.ptr()->nWidth = width;
    qrect.ptr()->nHeight = height;

    err = OMX_SetConfig(mJpegEnc, OMX_IndexConfigCommonOutputCrop, qrect.ptr());
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Set cropping zone for mJpegEnc passed");
    }else{
        DBGT_CRITICAL("Get cropping zone for mJpegEnc failed %x",err);
    }

    err =OMX_GetParameter(mJpegEnc, OMX_IndexParamPortDefinition, &paramJPEGoutput);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 JPEG ENC passed ");
    } else {
        DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 JPEG ENC failed ");
    }

    //setup jpeg quality
    OmxUtils::StructContainer<OMX_IMAGE_PARAM_QFACTORTYPE> qfactor;
    qfactor.ptr()->nPortIndex = 1;
    qfactor.ptr()->nQFactor = quality;

    err = OMX_SetParameter(mJpegEnc, OMX_IndexParamQFactor, qfactor.ptr());
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_SetParameter OMX_IndexParamQFactor port 1 JPEG ENC passed ");
    }else{
        DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamQFactor port 1 JPEG ENC failed ");
    }

    err =OMX_SendCommand(mJpegEnc,OMX_CommandStateSet,OMX_StateIdle,NULL);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for JPEGENC");
    }else{
        DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed for JPEGENC 0x%x",err);
    }

    // get frame length
    OMX_U32 nAllocLen;
    switch ((uint32_t)format){
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
        case OMX_COLOR_FormatYUV420PackedPlanar:
            if(mIsExifInJpeg)
                nAllocLen =inBufferSize ;
            else
                nAllocLen = (mWidth*mHeight*3)/2;
        break;
        default:
            nAllocLen = mWidth*mHeight*2;
       break;
    }
    err = OMX_AllocateBuffer(mJpegEnc, &pJpegInBuffer, 0,NULL , (OMX_U32)nAllocLen);
    if(err == OMX_ErrorNone &&  pJpegInBuffer->pBuffer != 0){
        DBGT_PTRACE("Allocate Buffer Done for JPEGENC size:%lu",pJpegInBuffer->nAllocLen);
    }else{
        DBGT_CRITICAL("Allocate Buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
    }

    pJpegInBuffer->nFilledLen = pJpegInBuffer->nAllocLen;
    err = OMX_AllocateBuffer(mJpegEnc, &pJpegOutBuffer, 1,NULL , (OMX_U32)nAllocLen);
    if(err == OMX_ErrorNone &&  pJpegOutBuffer->pBuffer != 0){
        DBGT_PTRACE("Allocate Buffer Done output:%lu",pJpegOutBuffer->nAllocLen);
    }else{
        DBGT_CRITICAL("Allocate Buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
    }

    camera_sem_wait(&mStateSem, SEM_WAIT_TIMEOUT);

    err =OMX_SendCommand(mJpegEnc,OMX_CommandStateSet,OMX_StateExecuting,NULL);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Sending WAIT_STATE_EXECUTING passed for JPEGENC");
    }else{
        DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for JPEGENC 0x%x",err);
    }
    camera_sem_wait(&mStateSem, SEM_WAIT_TIMEOUT);
    timestamp_end = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
    timestamp_init = (timestamp_end-timestamp_begin)/*/1000000*/;

    DBGT_PDEBUG("Init time: %llu ms",timestamp_init/1000000);
    DBGT_EPILOG();
}

SteJpegEnc::~SteJpegEnc() {
    DBGT_PROLOG("");
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err =OMX_SendCommand(mJpegEnc,OMX_CommandStateSet,OMX_StateIdle,NULL);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for JPEGENC");
    }else{
        DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed for JPEGENC 0x%x",err);
    }
    camera_sem_wait(&mStateSem, SEM_WAIT_TIMEOUT);

    err =OMX_SendCommand(mJpegEnc,OMX_CommandStateSet,OMX_StateLoaded,NULL);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("Sending WAIT_STATE_LOADED passed for JPEGENC");
    }else{
        DBGT_CRITICAL("OMX_SendCommand OMX_StateLoad failed for JPEGENC 0x%x",err);
    }

    err =OMX_FreeBuffer(mJpegEnc,  (OMX_U32)0, pJpegInBuffer);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_FreeBuffer passed for Jpeg");
    }

    err =OMX_FreeBuffer(mJpegEnc,  (OMX_U32)1, pJpegOutBuffer);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("OMX_FreeBuffer passed for Jpeg");
    }

    camera_sem_wait(&mStateSem, SEM_WAIT_TIMEOUT);

    DBGT_PTRACE("Free handle jpeg enc");
    err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mJpegEnc);
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("In Jpeg encoder destructor GetpOMX_FreeHandle camera passed ");
    }else{
        DBGT_CRITICAL("In Jpeg encoder destructor GetpOMX_FreeHandle camera failed ");
    }

    err = (mOmxUtils.interface()->GetpOMX_Deinit())();
    if(err == OMX_ErrorNone){
        DBGT_PTRACE("In Jpeg encoder destructor GetpOMX_Deinit  passed ");
    }else{
        DBGT_CRITICAL("In Jpeg encoder destructor GetpOMX_Deinit failed ");
    }
    sem_destroy(&mStateSem);
    sem_destroy(&mSetPortCamSem);
    sem_destroy(&mJpegEncSem);

    DBGT_EPILOG("");
}

int SteJpegEnc::encode( char* inBuffer, char* &outBuffer, int &outSize ){

    DBGT_PROLOG("width %d height %d",mWidth, mHeight);
    OMX_ERRORTYPE err = OMX_ErrorNone;

    timestamp_begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

    // recopy frame to encode to the input buffer
    memcpy(pJpegInBuffer->pBuffer, inBuffer, pJpegInBuffer->nAllocLen );

    if(mIsExifInJpeg){
        pJpegInBuffer->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
        switch ((uint32_t)mformat){
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
        case OMX_COLOR_FormatYUV420PackedPlanar:
            pJpegInBuffer->nFilledLen = (mWidth*mHeight*3)/2;
            break;
        default:
            pJpegInBuffer->nFilledLen = mWidth*mHeight*2;
            break;
        }
        pJpegInBuffer->nOffset = 0;
    }else{
        pJpegInBuffer->nFilledLen = pJpegInBuffer->nAllocLen;
    }

    err =OMX_FillThisBuffer(mJpegEnc,pJpegOutBuffer);
    if(err ==OMX_ErrorNone ){
        DBGT_PTRACE("OMX_FillThisBuffer Done 0x%x  ", (unsigned int) pJpegOutBuffer->pBuffer);
    } else {
        DBGT_CRITICAL("OMX_FillThisBuffer error: %s", OmxUtils::name(err));
    }

    err =OMX_EmptyThisBuffer(mJpegEnc,pJpegInBuffer);
    if(err ==OMX_ErrorNone ){
        DBGT_PTRACE("OMX_EmptyThisBuffer Done 0x%x  ", (unsigned int) pJpegInBuffer->pBuffer);
    } else {
        DBGT_CRITICAL("OMX_EmptyThisBuffer error: %s", OmxUtils::name(err));
    }

    camera_sem_wait(&mJpegEncSem, SEM_WAIT_TIMEOUT);
    outBuffer = (char*)pJpegOutBuffer->pBuffer;
    outSize = pJpegOutBuffer->nFilledLen;

    DBGT_PTRACE("Jpeg encode done size:%d",outSize);

    timestamp_end = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
    timestamp_encode = (timestamp_end-timestamp_begin)/*/1000000*/;

    DBGT_PDEBUG("Encode time: %llu ms",timestamp_encode/1000000);
    DBGT_EPILOG();
    return 0;
}

OMX_ERRORTYPE omxJpegEncEventHandler(     OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
        OMX_EVENTTYPE eEvent,OMX_U32 nData1,
        OMX_U32 nData2, OMX_PTR pEventData){
    SteJpegEnc* pEncoder = (SteJpegEnc*) pAppData;

    DBGT_PROLOG("Handle: %p AppData: %p", hComponent, pAppData);
    DBGT_PTRACE(    "Event: %s(%d) Data1: %lu Data2: %lu EventData: %p",
            OmxUtils::name(eEvent), eEvent, nData1, nData2, pEventData);

    DBGT_ASSERT(NULL != hComponent, "Invalid handle" );

    switch (eEvent) {
    case OMX_EventCmdComplete:
        {
            OMX_COMMANDTYPE cmd = (OMX_COMMANDTYPE) nData1;
            DBGT_PTRACE("Cmd: %s(%d)", OmxUtils::name(cmd), cmd);

            if (cmd == OMX_CommandStateSet) {
                OMX_STATETYPE newState = (OMX_STATETYPE) nData2;
                DBGT_PTRACE("State: %s(%d)", OmxUtils::name(newState), newState);
                sem_post(&pEncoder->mStateSem);
            }
            break;
        }
    case OMX_EventError:
        {
            DBGT_CRITICAL(    "Error detected by OMX event handler : %d (0x%x) - %d (0x%x) ",
                    (OMX_ERRORTYPE) nData1,(OMX_ERRORTYPE) nData1,
                    (OMX_ERRORTYPE) nData2,(OMX_ERRORTYPE) nData2);
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
        DBGT_PTRACE("Unknown event: %d", eEvent);
    }

    DBGT_EPILOG("");
    return OMX_ErrorNone;

};

OMX_ERRORTYPE omxJpegEncEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer){
    SteJpegEnc* pEncoder = (SteJpegEnc*) pAppData;

    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

    DBGT_EPILOG("");
    return OMX_ErrorNone;
};

OMX_ERRORTYPE omxJpegEncFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer){
    SteJpegEnc* pEncoder = (SteJpegEnc*) pAppData;

    DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);
    sem_post(&pEncoder->mJpegEncSem);
    DBGT_EPILOG("");
    return OMX_ErrorNone;
};

}; //namespace android

