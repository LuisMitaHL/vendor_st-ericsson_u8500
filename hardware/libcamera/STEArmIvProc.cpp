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
#define DBGT_PREFIX "SwConv"

#include "STECamTrace.h"

//Internal includes
#include "STECamOmxUtils.h"
#include "STECamFrameRateDumper.h"
#include "STECamPerfManager.h"
#include "STECamOmxILExtIndex.h"
#include "STEExtIspCamera.h"
#include "STEArmIvProc.h"
#include "STECamUtils.h"

namespace android {

    OMX_ERRORTYPE omxArmIvProcEventHandler(
            OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
            OMX_EVENTTYPE eEvent,OMX_U32 nData1,
            OMX_U32 nData2, OMX_PTR pEventData);

    OMX_ERRORTYPE omxArmIvProcFillBufferDone(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData,
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

    OMX_ERRORTYPE omxArmIvProcEmptyBufferDone(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData,
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

#undef _CNAME_
#define _CNAME_ SteArmIvProc

    SteArmIvProc::SteArmIvProc(
            int inputWidth,
            int inputHeight,
            OMX_COLOR_FORMATTYPE inputColorFormat,
            int outputWidth,
            int outputHeight,
            OMX_COLOR_FORMATTYPE outputColorFormat)
    {
        timestamp_begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

        DBGT_PROLOG("");
        char ArmIvProc[] = "OMX.ST.ArmIVProc";
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

        mArmIvProcCallback.EmptyBufferDone = omxArmIvProcEmptyBufferDone;
        mArmIvProcCallback.FillBufferDone  = omxArmIvProcFillBufferDone;
        mArmIvProcCallback.EventHandler    = omxArmIvProcEventHandler;

        err=(mOmxUtils.interface()->GetpOMX_GetHandle())(&mArmIvProc,ArmIvProc, this, &mArmIvProcCallback);
        if(err == OMX_ErrorNone &&  mArmIvProc != 0){
            DBGT_PTRACE("ArmIvProc GetHandle Done for ARMIVPROC ");
        }else{
            DBGT_CRITICAL("ArmIvProc GetHandle failed err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
        }

        sem_init(&mStateSem      , 0 , 0);
        sem_init(&mSetPortCamSem , 0 , 0);
        sem_init(&mArmIvProcSem  , 0 , 0);

        mInputWidth        = inputWidth;
        mInputHeight       = inputHeight;
        mInputColorFormat  = inputColorFormat;
        mOutputWidth       = outputWidth;
        mOutputHeight      = outputHeight;
        mOutputColorFormat = outputColorFormat;

        // set input port
        OmxUtils::initialize(paramArmIvProcInput, OMX_PortDomainImage, 0);

        // the index of the input port. Should be modified.
        err =OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcInput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC passed");
        }else{
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC failed err = 0x%x", err);
        }

        // Here, the min number of buffers to be used is retrieved
        paramArmIvProcInput.nBufferCountActual = 1;
        OMX_VIDEO_PORTDEFINITIONTYPE *pt_ArmIvProcInput = &(paramArmIvProcInput.format.video);
        pt_ArmIvProcInput->cMIMEType = (OMX_STRING)"";
        pt_ArmIvProcInput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
        pt_ArmIvProcInput->nFrameWidth = (OMX_U32) mInputWidth;
        pt_ArmIvProcInput->nFrameHeight = (OMX_U32) mInputHeight;
        pt_ArmIvProcInput->nStride = 0;
        pt_ArmIvProcInput->nSliceHeight = 0;
        pt_ArmIvProcInput->bFlagErrorConcealment = (OMX_BOOL)OMX_FALSE;
        pt_ArmIvProcInput->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
        pt_ArmIvProcInput->eColorFormat = inputColorFormat;
        pt_ArmIvProcInput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

        err =OMX_SetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcInput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC passed ");
        }else{
            DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC failed err = 0x%x", err);
        }

        err =OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcInput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC passed");
        }else{
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 0 ARMIVPROC failed err = 0x%x", err);
        }

        // set output port
        OmxUtils::initialize(paramArmIvProcOutput, OMX_PortDomainImage, 1);

        // the index of the output port. Should be modified.
        err =OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcOutput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC passed");
        }else{
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC failed err = 0x%x", err);
        }

        paramArmIvProcOutput.nBufferCountActual = 1;
        // Here, the min number of buffers to be used is retrieved
        OMX_VIDEO_PORTDEFINITIONTYPE *pt_ArmIvProcOutput = &(paramArmIvProcOutput.format.video);
        pt_ArmIvProcOutput->cMIMEType = (OMX_STRING)"";
        pt_ArmIvProcOutput->pNativeRender = (OMX_NATIVE_DEVICETYPE)NULL;
        pt_ArmIvProcOutput->nFrameWidth = (OMX_U32) mOutputWidth;
        pt_ArmIvProcOutput->nFrameHeight = (OMX_U32) mOutputHeight;
        pt_ArmIvProcOutput->nStride = 0;
        pt_ArmIvProcOutput->nSliceHeight = 0;
        pt_ArmIvProcOutput->bFlagErrorConcealment= (OMX_BOOL)OMX_FALSE;
        pt_ArmIvProcOutput->eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
        pt_ArmIvProcOutput->eColorFormat = outputColorFormat;
        pt_ArmIvProcOutput->pNativeWindow = (OMX_NATIVE_DEVICETYPE)NULL;

        err =OMX_SetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcOutput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_SetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC passed ");
        }else{
            DBGT_CRITICAL("OMX_SetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC failed err = 0x%x", err);
        }

        err =OMX_GetParameter(mArmIvProc, OMX_IndexParamPortDefinition, &paramArmIvProcOutput);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_GetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC passed ");
        } else {
            DBGT_CRITICAL("OMX_GetParameter OMX_IndexParamPortDefinition port 1 ARMIVPROC failed err = 0x%x", err);
        }

        err =OMX_SendCommand(mArmIvProc,OMX_CommandStateSet,OMX_StateIdle,NULL);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for ARMIVPROC");
        }else{
            DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed for ARMIVPROC 0x%x",err);
        }

        // Allocate input buffer
        mInputBufSize = paramArmIvProcInput.nBufferSize;
        err = OMX_AllocateBuffer(mArmIvProc, &pArmIvProcInBuffer, 0,NULL , (OMX_U32)mInputBufSize);

        if(err == OMX_ErrorNone &&  pArmIvProcInBuffer->pBuffer != 0){
            DBGT_PTRACE("Allocate input buffer done for ARMIVPROC size:%lu",pArmIvProcInBuffer->nAllocLen);
        }else{
            DBGT_CRITICAL("Allocate input buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
        }
        pArmIvProcInBuffer->nFilledLen = pArmIvProcInBuffer->nAllocLen;

        // Allocate input buffer
        mOutputBufSize = paramArmIvProcOutput.nBufferSize;
        err = OMX_AllocateBuffer(mArmIvProc, &pArmIvProcOutBuffer, 1,NULL , (OMX_U32)mOutputBufSize);

        if(err == OMX_ErrorNone &&  pArmIvProcOutBuffer->pBuffer != 0){
            DBGT_PTRACE("Allocate out buffer done for ARMIVPROC size:%lu",pArmIvProcOutBuffer->nAllocLen);

        }else{
            DBGT_CRITICAL("Allocate output buffer err= %d (0x%x) ", (OMX_ERRORTYPE) err,(OMX_ERRORTYPE) err );
        }
        pArmIvProcOutBuffer->nFilledLen = pArmIvProcOutBuffer->nAllocLen;


        camera_sem_wait(& mStateSem, SEM_WAIT_TIMEOUT);

        err =OMX_SendCommand(mArmIvProc,OMX_CommandStateSet,OMX_StateExecuting,NULL);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("Sending WAIT_STATE_EXECUTING passed for ARMIVPROC");
        }else{
            DBGT_CRITICAL("OMX_SendCommand OMX_StateExecuting failed for ARMIVPROC 0x%x",err);
        }
        camera_sem_wait(& mStateSem, SEM_WAIT_TIMEOUT);

        timestamp_end = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        timestamp_init = (timestamp_end-timestamp_begin)/*/1000000*/;
        DBGT_EPILOG("");
    }

    SteArmIvProc::~SteArmIvProc()
    {
        DBGT_PROLOG("");
        OMX_ERRORTYPE err = OMX_ErrorNone;

        err =OMX_SendCommand(mArmIvProc,OMX_CommandStateSet,OMX_StateIdle,NULL);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("Sending WAIT_STATE_IDLE passed for ARMIVPROC");
        }else{
            DBGT_CRITICAL("OMX_SendCommand OMX_StateIdle failed for ARMIVPROC 0x%x",err);
        }
        camera_sem_wait(& mStateSem, SEM_WAIT_TIMEOUT);

        err =OMX_SendCommand(mArmIvProc,OMX_CommandStateSet,OMX_StateLoaded,NULL);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("Sending WAIT_STATE_LOADED passed for ARMIVPROC");
        }else{
            DBGT_CRITICAL("OMX_SendCommand OMX_StateLoad failed for ARMIVPROC 0x%x",err);
        }

        err =OMX_FreeBuffer(mArmIvProc,  (OMX_U32)0, pArmIvProcInBuffer);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_FreeBuffer passed for Jpeg");
        }

        err =OMX_FreeBuffer(mArmIvProc,  (OMX_U32)1, pArmIvProcOutBuffer);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("OMX_FreeBuffer passed for Jpeg");
        }

        camera_sem_wait(&mStateSem, SEM_WAIT_TIMEOUT);

        sem_destroy(&mStateSem);
        sem_destroy(&mSetPortCamSem);
        sem_destroy(&mArmIvProcSem);

        DBGT_PTRACE("Free handle jpeg enc");
        err = mOmxUtils.interface()->GetpOMX_FreeHandle()(mArmIvProc);
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("In ArmIvProc destructor GetpOMX_FreeHandle camera passed ");
        }else{
            DBGT_CRITICAL("In ArmIvProc destructor GetpOMX_FreeHandle camera failed ");
        }

        err = (mOmxUtils.interface()->GetpOMX_Deinit())();
        if(err == OMX_ErrorNone){
            DBGT_PTRACE("In ArmIvProc destructor GetpOMX_Deinit  passed ");
        }else{
            DBGT_CRITICAL("In ArmIvProc destructor GetpOMX_Deinit failed ");
        }
        DBGT_EPILOG("");
    }

    int SteArmIvProc::transform(
            char* inBuffer,
            char* &outBuffer)
    {
        DBGT_PROLOG("");

        DBGT_PTRACE("InputWidth %d OutputHeight %d",mInputWidth, mInputHeight);
        DBGT_PTRACE("Input color format 0x%x",(int)mInputColorFormat);
        DBGT_PTRACE("OutputWidth %d OutputHeight %d",mOutputWidth, mOutputHeight);
        DBGT_PTRACE("Input color format 0x%x",(int)mOutputColorFormat);

        OMX_ERRORTYPE err = OMX_ErrorNone;

        timestamp_begin = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);

        // recopy frame to encode to the input buffer
        int nAllocLen = 0;
        switch ((uint32_t)mInputColorFormat){
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
            nAllocLen =mInputWidth*mInputHeight*3/2;
            break;
        case OMX_COLOR_FormatCbYCrY:
            nAllocLen =mInputWidth*mInputHeight*2;
            break;
        default:
            nAllocLen =mInputWidth*mInputHeight*3/2;
            break;
        }

        memcpy(pArmIvProcInBuffer->pBuffer, inBuffer, nAllocLen );

        err =OMX_FillThisBuffer(mArmIvProc,pArmIvProcOutBuffer);
        if(err ==OMX_ErrorNone ){
            DBGT_PTRACE("OMX_FillThisBuffer Done 0x%x  ", (unsigned int) pArmIvProcOutBuffer->pBuffer);
        } else {
            DBGT_CRITICAL("OMX_FillThisBuffer error: %s", OmxUtils::name(err));
        }

        err =OMX_EmptyThisBuffer(mArmIvProc,pArmIvProcInBuffer);
        if(err ==OMX_ErrorNone ){
            DBGT_PTRACE("OMX_EmptyThisBuffer Done 0x%x  ", (unsigned int) pArmIvProcInBuffer->pBuffer);
        } else {
            DBGT_CRITICAL("OMX_EmptyThisBuffer error: %s", OmxUtils::name(err));
        }

        camera_sem_wait(& mArmIvProcSem, SEM_WAIT_TIMEOUT);

        outBuffer = (char*)pArmIvProcOutBuffer->pBuffer;

        timestamp_end = (nsecs_t) systemTime(SYSTEM_TIME_MONOTONIC);
        timestamp_encode = (timestamp_end-timestamp_begin)/*/1000000*/;

        DBGT_EPILOG("");
        return 0;
    }

    OMX_ERRORTYPE omxArmIvProcEventHandler(
            OMX_HANDLETYPE hComponent,OMX_PTR pAppData,
            OMX_EVENTTYPE eEvent,OMX_U32 nData1,
            OMX_U32 nData2, OMX_PTR pEventData)
    {
        SteArmIvProc* pArmIvProc = (SteArmIvProc*) pAppData;

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
                    sem_post(&pArmIvProc->mStateSem);
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

    OMX_ERRORTYPE omxArmIvProcEmptyBufferDone(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData,
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
    {
        SteArmIvProc* pArmIvProc = (SteArmIvProc*) pAppData;

        DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);

        DBGT_EPILOG("");
        return OMX_ErrorNone;
    };

    OMX_ERRORTYPE omxArmIvProcFillBufferDone(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_IN OMX_PTR pAppData,
            OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
    {
        SteArmIvProc* pArmIvProc = (SteArmIvProc*) pAppData;

        DBGT_PROLOG("Handle: %p, AppData: %p, BuffHeader: %p", hComponent, pAppData, pBuffer);
        sem_post(&pArmIvProc->mArmIvProcSem);
        DBGT_EPILOG("");
        return OMX_ErrorNone;
    };

}; //namespace android

