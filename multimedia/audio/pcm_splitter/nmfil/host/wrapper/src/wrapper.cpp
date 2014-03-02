/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <pcm_splitter/nmfil/host/wrapper.nmf>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <armnmf_dbc.h>
#include <string.h>
#include <verbose.h>

#ifndef ARCH

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pcm_splitter_nmfil_host_wrapper_src_wrapperTraces.h"
#endif

#else // ARCH

#include <stdio.h>
#define OstTraceFiltInst0(a, b) printf((b "\n"))
#define OstTraceFiltInst1(a, b, c) printf((b "\n"), (c))
#define OstTraceFiltInst2(a, b, c, d) printf((b "\n"), (c), (d))
#define OstTraceFiltInst3(a, b, c, d, e) printf((b "\n"), (c), (d), (e))
#define OstTraceFiltInst4(a, b, c, d, e, f) printf((b "\n"), (c), (d), (e), (f))

#endif // ARCH

#define PCMSPLITTER_SAMPLE_2_BYTES 2
#define PCMSPLITTER_SAMPLE_4_BYTES 4

// For traces purpose
#ifdef ALLOW_PRINTF
#include <stdio.h>
#define OstTraceFiltInst0(a, b) printf((b "\n"))
#define OstTraceFiltInst1(a, b, c) printf((b "\n"), (c))
#define OstTraceFiltInst2(a, b, c, d) printf((b "\n"), (c), (d))
#define OstTraceFiltInst3(a, b, c, d, e) printf((b "\n"), (c), (d), (e))
#define OstTraceFiltInst4(a, b, c, d, e, f) printf((b "\n"), (c), (d), (e), (f))
#endif //ALLOW_PRINTF


METH(pcm_splitter_nmfil_host_wrapper)() {
    int i;
    mFsmInitDone = OMX_FALSE;
    mInputBufferRefCount=0;
    for(i=0 ; i<NB_OUTPUT ; i++){
        mSplitterOutputBuffer[i]=NULL;
        mSplitterOutputBufferHeader[i].pBuffer=NULL;
        mSplitterOutputBufferHeader[i].nAllocLen=0;
        mSrcInputBuffer[i]=NULL;
        mHeap[i]=NULL;
        mHeapSize[i]=0;

        for (int j=0 ; j<NMF_AUDIO_MAXCHANNELS ; j++) {
            mOutputChannelSwitchBoard[i][j] = 0;
        }
    }
}

METH(~pcm_splitter_nmfil_host_wrapper)() {
    for(int i=1 ; i<=NB_OUTPUT ; i++){
        deinitOutputSrc(i);
    }
}


void pcm_splitter_nmfil_host_wrapper::doChannelSwitch(int idx, OMX_BUFFERHEADERTYPE_p inbuf, OMX_BUFFERHEADERTYPE_p outbuf)
{
    t_uint16 i=0, j=0, k=0;

 
    if (mInputBytesPerSample == PCMSPLITTER_SAMPLE_4_BYTES) {
        OMX_U32 * inBufPtr = (OMX_U32 *)inbuf->pBuffer;
        OMX_U32 * outBufPtr = (OMX_U32 *)outbuf->pBuffer;

        for (i = 0, k = 0; i < (inbuf->nFilledLen)/mInputBytesPerSample; i += mInputChannels) {
            for (j = 0; j < mOutputChannels[idx-OUT]; j++) {
                outBufPtr[k++] = inBufPtr[i + (mOutputChannelSwitchBoard[idx-OUT][j])];
            }
        }
    } else {
        t_sint16 * inBufPtr = (t_sint16 *)inbuf->pBuffer;
        t_sint16 * outBufPtr = (t_sint16 *)outbuf->pBuffer;

        for (i = 0, k = 0; i < (inbuf->nFilledLen)/mInputBytesPerSample; i += mInputChannels) {
            for (j = 0; j < mOutputChannels[idx-OUT]; j++) {
                outBufPtr[k++] = inBufPtr[i + (mOutputChannelSwitchBoard[idx-OUT][j])];
            }
        }
    }

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::applying ChannelSwitch on port %d :  outbuf->nFilledLen=%d", idx,k*mInputBytesPerSample);
    for (j = 0; j < mOutputChannels[idx-OUT]; j++) {
        OstTraceFiltInst4(TRACE_DEBUG,"PcmSplitterNmfHost::applying ChannelSwitch on port %d mOutputChannelSwitchBoard[%d][%d]=%d", idx,idx-OUT,j,mOutputChannelSwitchBoard[idx-OUT][j]);
    }

    outbuf->nFilledLen       = k*mInputBytesPerSample;
    outbuf->nFlags           = inbuf->nFlags;
	outbuf->nTimeStamp       = inbuf->nTimeStamp;
}

void
pcm_splitter_nmfil_host_wrapper::doVolume(int idx, OMX_BUFFERHEADERTYPE_p inbuf, OMX_BUFFERHEADERTYPE_p outbuf) {
    

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::applying volume on port %d nFilledLen/mInputBytesPerSample=%d", idx, (t_uint16)((inbuf->nFilledLen)/mInputBytesPerSample));
    
    if (mInputBytesPerSample == PCMSPLITTER_SAMPLE_4_BYTES) {
        int *inputBuffer,*outputBuffer;
        inputBuffer = (int*)((t_uint32)inbuf->pBuffer + inbuf->nOffset);
        outputBuffer = (int*)((t_uint32)outbuf->pBuffer + outbuf->nOffset);
        volctrl[idx-OUT].process(inputBuffer, outputBuffer, (t_uint16)((inbuf->nFilledLen)/mInputBytesPerSample));
    } else {
        t_sint16 *inputBuffer,*outputBuffer;
        inputBuffer = (t_sint16*)((int)inbuf->pBuffer + inbuf->nOffset);
        outputBuffer = (t_sint16*)((int)outbuf->pBuffer + outbuf->nOffset);
        volctrl[idx-OUT].process_16bits(inputBuffer, outputBuffer, (t_uint16)((inbuf->nFilledLen)/mInputBytesPerSample));
    }

    outbuf->nFilledLen       = inbuf->nFilledLen;
    outbuf->nFlags           = inbuf->nFlags;
	outbuf->nTimeStamp       = inbuf->nTimeStamp;
}

void
pcm_splitter_nmfil_host_wrapper::returnOutputBuffer(int idx, OMX_BUFFERHEADERTYPE_p outbuf) {

    if (outbuf->nFlags & OMX_BUFFERFLAG_EOS) {
        proxy.eventHandler(OMX_EventBufferFlag, idx, outbuf->nFlags);
        OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::returnOutputBuffer EOS sent on output port %d", idx);
    }

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::returnOutputBuffer on output port %d outbuf->nFilledLen=%d", idx,outbuf->nFilledLen);
    mPorts[idx].dequeueAndReturnBuffer();
}

void
pcm_splitter_nmfil_host_wrapper::releaseInputBuffer(int idx) {
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::releaseInputBuffer Clear bit for output port %d", idx);

    CLEAR_BIT(mInputBufferRefCount, idx);
    if (mInputBufferRefCount == 0) {
        OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterNmfHost::releaseInputBuffer return input buffer");
        mPorts[IN].dequeueAndReturnBuffer();
    }
}

void
pcm_splitter_nmfil_host_wrapper::applyVolume(int idx) {
    OMX_BUFFERHEADERTYPE_p inbuf, outbuf;

    if (!IS_BIT_SET(mInputBufferRefCount, idx)
            || mPorts[idx].queuedBufferCount() == 0) {
        return;
    }

    inbuf   = mPorts[IN].getBuffer(0);
    outbuf  = mPorts[idx].getBuffer(0);

    doVolume(idx, inbuf, outbuf);

    if(inbuf->nFlags & OMX_BUFFERFLAG_EXTRADATA) {
        //Propagate extradatas for telephony usecases (will be propagated only when inputfreq=outputfreq and no channel switch)
        OMX_U8 * pExtraDataOut  =  (OMX_U8 *)(outbuf->pBuffer + outbuf->nOffset + outbuf->nFilledLen);
        OMX_U8 * pExtraDataIn  =  (OMX_U8 *)(inbuf->pBuffer + inbuf->nOffset + inbuf->nFilledLen);
        OMX_OTHER_EXTRADATATYPE* pOmxBufExtraData = (OMX_OTHER_EXTRADATATYPE*) pExtraDataIn;
        OMX_U32 extradataSize = pOmxBufExtraData->nSize;
        if((outbuf->nFilledLen + pOmxBufExtraData->nSize) <= outbuf->nAllocLen) {
            //Check that we can copy extradatas in outputbuffer
            OstTraceFiltInst1(TRACE_DEBUG,"PcmSplitterNmfHost::propagate extradatas (size=%d)", pOmxBufExtraData->nSize);
            memcpy ( pExtraDataOut, pExtraDataIn, extradataSize);
        } else {
            //Don't propagate OMX_BUFFERFLAG_EXTRADATA flag.
            outbuf->nFlags &= ~OMX_BUFFERFLAG_EXTRADATA;
        }
    }

    releaseInputBuffer(idx);
    returnOutputBuffer(idx, outbuf);
}

void
pcm_splitter_nmfil_host_wrapper::applySrcAndVolume(int idx) {
    OMX_BUFFERHEADERTYPE_p inbuf, outbuf;
    bool needInputBuf=false, filledOutputBuf=true;

    if (mPorts[idx].queuedBufferCount() == 0
            || !IS_BIT_SET(mInputBufferRefCount, idx)) {
        return;
    }

    inbuf   = mPorts[IN].getBuffer(0);
    outbuf  = mPorts[idx].getBuffer(0);

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::applying SRC on port %d", idx);

    memcpy(mSplitterOutputBuffer[idx-OUT]->pBuffer,inbuf->pBuffer,inbuf->nFilledLen);
    mSplitterOutputBuffer[idx-OUT]->nFilledLen       = inbuf->nFilledLen;
    mSplitterOutputBuffer[idx-OUT]->nFlags           = inbuf->nFlags;

    resample_process(&mResampleContext[idx-OUT], mSplitterOutputBuffer[idx-OUT], outbuf, mOutputChannels[idx-OUT], &needInputBuf, &filledOutputBuf);

    if (needInputBuf) {
        releaseInputBuffer(idx);
    }

    if (!filledOutputBuf) {
        return;
    }

    doVolume(idx, outbuf, outbuf);

    returnOutputBuffer(idx, outbuf);
}

void
pcm_splitter_nmfil_host_wrapper::applyChannelSwitchAndVolume(int idx) {
    OMX_BUFFERHEADERTYPE_p inbuf, outbuf;

    if (mPorts[idx].queuedBufferCount() == 0
            || !IS_BIT_SET(mInputBufferRefCount, idx)) {
        return;
    }
    
    inbuf   = mPorts[IN].getBuffer(0);
    outbuf  = mPorts[idx].getBuffer(0);

    doChannelSwitch(idx, inbuf, outbuf);

    doVolume(idx, outbuf, outbuf);

    releaseInputBuffer(idx);
    returnOutputBuffer(idx, outbuf);
}

void
pcm_splitter_nmfil_host_wrapper::applyChannelSwitchAndSrcAndVolume(int idx) {
    OMX_BUFFERHEADERTYPE_p    inbuf, outbuf;
    bool        needInputBuf, filledOutputBuf;

    if (mPorts[idx].queuedBufferCount() == 0) {
        return;
    }

    if (mSrcInputBuffer[idx-OUT] == 0) {
        if (!IS_BIT_SET(mInputBufferRefCount, idx)) {
            return;
        }
        inbuf   = mPorts[IN].getBuffer(0);
        doChannelSwitch(idx, inbuf, mSplitterOutputBuffer[idx-OUT]);
        mSrcInputBuffer[idx-OUT] = mSplitterOutputBuffer[idx-OUT];
    }

    outbuf  = mPorts[idx].getBuffer(0);

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::applying SRC on port %d", idx);

    resample_process(&mResampleContext[idx-OUT], mSrcInputBuffer[idx-OUT], outbuf, mOutputChannels[idx-OUT], &needInputBuf, &filledOutputBuf);

    if (needInputBuf) {
        mSrcInputBuffer[idx-OUT] = 0;
        releaseInputBuffer(idx);
    }

    if (!filledOutputBuf) {
        return;
    }

    doVolume(idx, outbuf, outbuf);

    returnOutputBuffer(idx, outbuf);
}

void
METH(process)()
{
    t_uint16 idx;

    if (mInputBufferRefCount == 0 && mPorts[IN].queuedBufferCount()>0) {
        mInputBufferRefCount = ALL_OUTPUTS;  
    }

    if (allOutputsPausedOrDisabled()) {
        OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterNmfHost::Splitter_process all outputs paused or disabled");
        return;
    }
            
    for (idx = 1; idx <= NB_OUTPUT ; idx++) {
        if (isDisabled(idx) || isPaused(idx)) {
            if (IS_BIT_SET(mInputBufferRefCount, idx)) {
                releaseInputBuffer(idx);
            }
            continue;
        }
        (this->*mProcessOutputPort[idx-OUT])(idx);
    }
}

void
pcm_splitter_nmfil_host_wrapper::initOutputVolctrl(int idx) {
    OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterNmfHost::initOutputVolctrl idx=%d inChannelNummber=%d outChannelNummber=%d ", idx,mOutputChannels[idx-OUT],mOutputChannels[idx-OUT]);
    volctrl[idx-OUT].init(mOutputChannels[idx-OUT],mOutputChannels[idx-OUT], mOutputFreq[idx-OUT],mChannelTypeOut,mChannelTypeOut);
}

void *
pcm_splitter_nmfil_host_wrapper::portMalloc(int idx, int size) {
    void * p;
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::allocating %d bytes on port %d",  size, idx);
    p = malloc(size);
    
    ARMNMF_DBC_ASSERT(p != NULL);
    mHeap[idx-1] = p;
    mHeapSize[idx-1] = size;
    return p;
}

bool
pcm_splitter_nmfil_host_wrapper::isLowLatencySRC(int idx) {
    if ((mOutputFreq[idx-OUT] == FREQ_8KHZ || mOutputFreq[idx-OUT] == FREQ_16KHZ || mOutputFreq[idx-OUT] == FREQ_48KHZ)
            && (mInputFreq == FREQ_8KHZ || mInputFreq == FREQ_16KHZ || mInputFreq == FREQ_48KHZ)) {
        return true;
    } else {
        return false;
    }
}

void
METH(initOutputSrc)(int idx) {
    int status, heapsize;
    void *buf;
    ARMNMF_DBC_PRECONDITION(idx > IN && idx < (NB_OUTPUT +1));

    if (mInputFreq == mOutputFreq[idx-OUT]) {
        return;
    }

    if (isLowLatencySRC(idx)) {

        OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::initOutputSrc LOW LATENCY for port %d",idx);
        resample_calc_max_heap_size_fixin_fixout(0,0,SRC_LOW_LATENCY_IN_MSEC,&heapsize,mInputBlockSize,mOutputChannels[idx-OUT],0);
        buf = portMalloc(idx, heapsize);
        if (mInputBytesPerSample == PCMSPLITTER_SAMPLE_4_BYTES) {
            status = resample_x_init_ctx_low_mips_fixin_fixout(
                    (char*)buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                    mOutputFreq[idx-OUT], SRC_LOW_LATENCY_IN_MSEC, mOutputChannels[idx-OUT],mInputBlockSize);
        } else {
            status = resample_x_init_ctx_low_mips_fixin_fixout_sample16(
                    (char*)buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                    mOutputFreq[idx-OUT], SRC_LOW_LATENCY_IN_MSEC, mOutputChannels[idx-OUT],mInputBlockSize);
        }
    } 
    else {
        int size = mInputBlockSize*mOutputChannels[idx-OUT];
                
        OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterNmfHost::initOutputSrc NO LOW LATENCY for port %d blocksize=%d mOutputChannels=%d",idx,mInputBlockSize*mOutputChannels[idx-OUT],mOutputChannels[idx-OUT]);
        resample_calc_max_heap_size_fixin_fixout(mInputFreq,mOutputFreq[idx-OUT],SRC_STANDARD_QUALITY,&heapsize,size,mOutputChannels[idx-OUT],0);
        buf = portMalloc(idx, heapsize); 

        if (mInputBytesPerSample == PCMSPLITTER_SAMPLE_4_BYTES) {
            status = resample_x_init_ctx_low_mips_fixin_fixout(
                    (char*)buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                    mOutputFreq[idx-OUT], SRC_STANDARD_QUALITY, mOutputChannels[idx-OUT], size);
        } else {
            status = resample_x_init_ctx_low_mips_fixin_fixout_sample16(
                    (char*)buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                    mOutputFreq[idx-OUT], SRC_STANDARD_QUALITY, mOutputChannels[idx-OUT], size);
        }

    }
    ARMNMF_DBC_ASSERT(status == 0);

    buf = malloc(mInputBlockSize * mOutputChannels[idx-OUT]*mInputBytesPerSample);

    ARMNMF_DBC_ASSERT(buf != NULL);
    mSplitterOutputBufferHeader[idx-OUT].pBuffer = (OMX_U8*)buf;
    mSplitterOutputBufferHeader[idx-OUT].nOffset = 0;
    mSplitterOutputBufferHeader[idx-OUT].nAllocLen = mInputBlockSize * mOutputChannels[idx-OUT]*mInputBytesPerSample;

}

void
pcm_splitter_nmfil_host_wrapper::deinitOutputSrc(int idx) {
    if(mHeap[idx-OUT] != NULL){
        free(mHeap[idx-OUT]);
        mHeap[idx-OUT] = NULL;
    }
    if(mSplitterOutputBufferHeader[idx-OUT].pBuffer != NULL){
        free(mSplitterOutputBufferHeader[idx-OUT].pBuffer);
        mSplitterOutputBufferHeader[idx-OUT].pBuffer = NULL;
    }
}

void
pcm_splitter_nmfil_host_wrapper::initOutputPortProcessFunction(int idx) {

    if (!isChannelSwitchingPort(idx)) {
        if (mInputFreq == mOutputFreq[idx-OUT]) {
            mProcessOutputPort[idx-OUT] = &pcm_splitter_nmfil_host_wrapper::applyVolume;
#ifdef ALLOW_PRINTF
            printf("initOutputPortProcessFunction : applyVolume for port %d\n",idx);
#endif
        } else {
            mProcessOutputPort[idx-OUT] = &pcm_splitter_nmfil_host_wrapper::applySrcAndVolume;
#ifdef ALLOW_PRINTF
            printf("initOutputPortProcessFunction : applySrcAndVolume for port %d\n",idx);
#endif
        }
    }
    else {
        if (mInputFreq == mOutputFreq[idx-OUT]) {
            mProcessOutputPort[idx-OUT] = &pcm_splitter_nmfil_host_wrapper::applyChannelSwitchAndVolume;
#ifdef ALLOW_PRINTF
            printf("initOutputPortProcessFunction : applyChannelSwitchAndVolume for port %d\n",idx);
#endif
        } else {
            mProcessOutputPort[idx-OUT] = &pcm_splitter_nmfil_host_wrapper::applyChannelSwitchAndSrcAndVolume;
#ifdef ALLOW_PRINTF
            printf("initOutputPortProcessFunction : applyChannelSwitchAndSrcAndVolume for port %d\n",idx);
#endif
        }
    }
}

void
pcm_splitter_nmfil_host_wrapper::initEnabledOutputPort(int idx) {
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::initEnabledOutputPort %d", idx);

    initOutputVolctrl(idx);
    initOutputSrc(idx);
    initOutputPortProcessFunction(idx);
}

void
pcm_splitter_nmfil_host_wrapper::initAllEnabledOutputPorts() {
    int i;

    for (i = 1 ; i <= NB_OUTPUT; i++) {
        if (isDisabled(i)) continue;
        initEnabledOutputPort(i);
    }
}


void pcm_splitter_nmfil_host_wrapper::resetPort(t_uint16 idx) {

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::resetPort %d", idx);

    if (idx == IN) {
        mInputBufferRefCount = 0;
    } else {
        mSrcInputBuffer[idx-OUT]        = 0; 
        mSplitterOutputBuffer[idx-OUT]  = &mSplitterOutputBufferHeader[idx-OUT];
    }
}

void METH(reset)()
{
    t_uint16 idx;

    OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterNmfHost::Splitter_reset");

    for (idx = 0 ; idx <= NB_OUTPUT ; idx++) {
        resetPort(idx);
    }
}

void
METH(disablePortIndication)(t_uint32 portIdx) {
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::Splitter_disablePortIndication  on port %d", portIdx);

    SET_BIT(mDisabledPorts, portIdx);

    resetPort(portIdx);

    if(portIdx != IN)
    {  
        deinitOutputSrc(portIdx);  
    } else {
        for (int i = 1 ; i <= NB_OUTPUT; i++) {
            deinitOutputSrc(i);
        }
    }

    if (mPorts[IN].queuedBufferCount() > 0) {
        scheduleProcessEvent();
    }
}

void
METH(enablePortIndication)(t_uint32 portIdx) {
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::Splitter_enablePortIndication  on port %d", portIdx);

    CLEAR_BIT(mDisabledPorts, portIdx);

    if (portIdx != IN) {
        if (!isDisabled(IN)) {
            initEnabledOutputPort(portIdx);
        }
    } else {
        initAllEnabledOutputPorts();
    }
}

void
METH(flushPortIndication)(t_uint32 portIdx) {
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::Splitter_flushPortIndication  on port %d", portIdx);
    resetPort(portIdx);
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setMuteOutputPort)(t_uint16 portIdx, BOOL bMute) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setMute)) {
        volctrl[portIdx-OUT].setMute(bMute);
    }
}

void METH(setBalanceOutputPort)(t_uint16 portIdx, t_sint16 nBalance) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setBalance)) {
        volctrl[portIdx-OUT].setBalance(nBalance);
    }
}

void METH(setVolumeOutputPort)(t_uint16 portIdx, t_sint16 nVolume) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setVolume)) {
        volctrl[portIdx-OUT].setVolume(nVolume);
    }
}

void METH(setVolumeRampOutputPort)(
        t_uint16 portIdx, 
        t_sint16 nStartVolume, 
        t_sint16 nEndVolume, 
        t_uint16 nChannels, 
        t_uint24 nDuration, 
        BOOL bTerminate) 
{
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setVolumeRamp)) {
        volctrl[portIdx-OUT].setVolumeRamp(nStartVolume, nEndVolume, nChannels, nDuration, bTerminate);
    }
}

void METH(setChannelSwitchBoardOutputPort)(t_uint16 portIdx, t_uint16 channelSwitchBoard[]) {
    int i;

    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

    for (i = 0; i < mOutputChannels[portIdx-OUT]; i++) {
        mOutputChannelSwitchBoard[portIdx-OUT][i] = channelSwitchBoard[i];
    }
}

void METH(setPausedOutputPort)(t_uint16 portIdx, BOOL bIsPaused) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= 1);

#ifdef ALLOW_PRINTF
    printf("PcmSplitterNmfHost::setPausedOutputPort portIdx=%d  bIsPaused=%d\n", portIdx, bIsPaused);
#endif

    if (bIsPaused) {
        SET_BIT(mPausedOutputs, portIdx);
    } else {
        CLEAR_BIT(mPausedOutputs, portIdx);
    }

    if ((mFsmInitDone == OMX_TRUE) && mPorts[IN].queuedBufferCount()) {
        scheduleProcessEvent();
    }
}

void METH(setInputPortParameter)(HostInputPortParam_t inputPortParam) {
    mInputFreq         = inputPortParam.nSamplingRate;
    mInputChannels     = inputPortParam.nChannels;
    mInputBlockSize    = inputPortParam.nBlockSize;
    mInputBytesPerSample = inputPortParam.nBitsPerSample / 8;
}

void METH(setOutputPortParameter)(t_uint16 idx, HostOutputPortParam_t outputPortParam) {
    ARMNMF_DBC_PRECONDITION(idx <= NB_OUTPUT && idx >= 1);

    // Params
    mOutputChannels[idx-OUT]      = outputPortParam.nChannels;
    mOutputFreq[idx-OUT]          = outputPortParam.nSamplingRate; 
    mOutputBlockSize[idx-OUT]     = outputPortParam.nBlockSize;
    mHeap[idx-OUT]                  = NULL;//(int *)outputPortParam.pHeap;
    mHeapSize[idx-OUT]              = 0;//outputPortParam.nHeapSize;
    mChannelTypeOut[idx-OUT]      = CHANNEL_NONE;

    // Configs
    METH(setChannelSwitchBoardOutputPort)(idx, outputPortParam.nChannelSwitchBoard);
    METH(setPausedOutputPort)(idx, outputPortParam.bPaused);
    METH(setMuteOutputPort)(idx, outputPortParam.bMute);
    METH(setBalanceOutputPort)(idx, outputPortParam.nBalance);
    METH(setVolumeOutputPort)(idx, outputPortParam.nVolume);
    METH(setVolumeRampOutputPort)(
            idx, outputPortParam.nVolume, outputPortParam.nRampEndVolume, outputPortParam.nRampChannels, 
            outputPortParam.nRampDuration, outputPortParam.bRampTerminate);
}

void METH(setParameter)(HostSplitterParam_t splitterParam) {
    int i;

    METH(setInputPortParameter)(splitterParam.inputPortParam);

    for (i = 1; i <= NB_OUTPUT; i++) {
       METH(setOutputPortParameter)(i, splitterParam.outputPortParam[i-OUT]); 
       initOutputPortProcessFunction(i);
    }

}

void METH(fsmInit) (fsmInit_t initFsm)
{
    t_uint16 idx;

    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::fsmInit portsDisabled=%d", initFsm.portsDisabled);

    mPorts[IN].init(InputPort, false,
              false, 0, mFifoIn, 1, &inputport,
              IN, (initFsm.portsDisabled & (1 << (IN)))!=0, (initFsm.portsTunneled & (1 << IN))!=0, this);

    for (idx = 1 ; idx <= NB_OUTPUT ; idx++) {
        mPorts[idx].init(OutputPort, false,
                  false, 0, mFifoOut[idx-OUT],  1, &outputport[idx-OUT],
                  idx, (initFsm.portsDisabled & (1 << idx))!=0, (initFsm.portsTunneled & (1 << idx))!=0, this);
    }

    mDisabledPorts = initFsm.portsDisabled;

    if (!isDisabled(IN)) {
        initAllEnabledOutputPorts();
    }

    init(NB_OUTPUT + 1, mPorts, &proxy, &me, false);

    mFsmInitDone = OMX_TRUE;
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) {
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buf) {

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::emptyThisBuffer nFilledLen=%d nFlags=%d", buf->nFilledLen, buf->nFlags);
    
    Component::deliverBuffer(IN, buf);
}

// events from volctrl 
void METH(eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 idx) {
    ARMNMF_DBC_ASSERT(event == ((OMX_EVENTTYPE)OMX_DSP_EventIndexSettingChanged));
    proxy.eventHandler(event, idx+OUT, data2);
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buf, t_uint8 idx) {

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterNmfHost::fillThisBuffer on port %d allocLen=%d", OUT+idx, buf->nAllocLen);

    Component::deliverBuffer(OUT+idx, buf);
}


void
pcm_splitter_nmfil_host_wrapper::fillBufferWithZeroes(OMX_BUFFERHEADERTYPE_p buf) 
{
    unsigned int i;

    for (i = buf->nFilledLen; i < buf->nAllocLen; i++) {
        buf->pBuffer[i] = 0;
    }
    buf->nFilledLen = buf->nAllocLen;
}

void
pcm_splitter_nmfil_host_wrapper::resample_process_low_latency(
        ResampleContext *resampleContext,
        OMX_BUFFERHEADERTYPE_p inputBuf,
        OMX_BUFFERHEADERTYPE_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    int framing = resampleContext->block_siz / resampleContext->fin_khz;

    consumed    = framing;
    produced    = 0;

    if (inputBuf->nFlags & OMX_BUFFERFLAG_EOS) {
        fillBufferWithZeroes(inputBuf);
        flush = true;
    }

    flush_in    = flush;

    if ((*resampleContext->processing) (
                resampleContext,
                (int *)inputBuf->pBuffer, 
                channels, 
                &consumed, 
                (int *)outputBuf->pBuffer, 
                &produced, 
                &flush) != 0)
    {
        ARMNMF_DBC_ASSERT(FALSE);
    }

    if (flush_in == 0) {
        ARMNMF_DBC_POSTCONDITION(consumed == framing);
        ARMNMF_DBC_POSTCONDITION(produced == framing);
    }

    if ((inputBuf->nFlags & OMX_BUFFERFLAG_EOS) && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else {
        *needInputBuf = true;
    }

    outputBuf->nFilledLen = produced * resampleContext->fout_khz * channels * mInputBytesPerSample;

    if (flush) {
        outputBuf->nFlags |= OMX_BUFFERFLAG_EOS;
    }

    *filledOutputBuf = true;
}

void
pcm_splitter_nmfil_host_wrapper::resample_process_std(
        ResampleContext *resampleContext,
        OMX_BUFFERHEADERTYPE_p inputBuf,
        OMX_BUFFERHEADERTYPE_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    int consumed = 0, produced = 0, flush = 0;
    int blocksize;


  
    blocksize = resampleContext->block_size_by_nb_channel/channels;
    produced = consumed = blocksize;

    if (inputBuf->nFlags & OMX_BUFFERFLAG_EOS) {
        fillBufferWithZeroes(inputBuf);
        flush = true;
    }

    if ((*resampleContext->processing) (
                resampleContext,
                (int*)inputBuf->pBuffer, 
                channels, 
                &consumed, 
                (int*)outputBuf->pBuffer, 
                &produced, 
                &flush) != 0)
    {
        ARMNMF_DBC_ASSERT(FALSE);
    }

    ARMNMF_DBC_POSTCONDITION(consumed == blocksize || consumed == 0);
    ARMNMF_DBC_POSTCONDITION(produced == blocksize || produced == 0);

    if ((inputBuf->nFlags & OMX_BUFFERFLAG_EOS) && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else
    {
        if (consumed == blocksize || flush) {
            *needInputBuf = true;
        } else {
            *needInputBuf = false;
        }
    }

    if (produced == blocksize || flush) {
        outputBuf->nFilledLen = produced * channels * mInputBytesPerSample;
        if (flush) {
            outputBuf->nFlags |= OMX_BUFFERFLAG_EOS;
        }
        *filledOutputBuf = true;
    } else {
        *filledOutputBuf = false;
    }
}

void
pcm_splitter_nmfil_host_wrapper::resample_process(
        ResampleContext *resampleContext,
        OMX_BUFFERHEADERTYPE_p inputBuf,
        OMX_BUFFERHEADERTYPE_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    if (resampleContext->low_latency == SRC_LOW_LATENCY_IN_MSEC) {
        resample_process_low_latency(resampleContext, inputBuf, outputBuf, channels, needInputBuf, filledOutputBuf);
    }
    else {
        resample_process_std(resampleContext, inputBuf, outputBuf, channels, needInputBuf, filledOutputBuf);
    }
}





