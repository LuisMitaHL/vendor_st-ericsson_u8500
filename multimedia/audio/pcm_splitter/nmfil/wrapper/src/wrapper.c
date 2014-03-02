/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <pcm_splitter/nmfil/wrapper.nmf>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include "fsm/component/include/Component.inl"

#ifndef ARCH

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pcm_splitter_nmfil_wrapper_src_wrapperTraces.h"
#endif

#else // ARCH

#include <stdio.h>
#define OstTraceFiltInst0(a, b) printf((b "\n"))
#define OstTraceFiltInst1(a, b, c) printf((b "\n"), (c))
#define OstTraceFiltInst2(a, b, c, d) printf((b "\n"), (c), (d))
#define OstTraceFiltInst3(a, b, c, d, e) printf((b "\n"), (c), (d), (e))
#define OstTraceFiltInst4(a, b, c, d, e, f) printf((b "\n"), (c), (d), (e), (f))

#endif // ARCH

#define MS_GRANULARITY  5

#define OUT 1
#define IN  0

#define ALL_OUTPUTS         0x6     // bitmask with all output port idxs set
#define SET_BIT(a, i)       ((a) |= (1 << (i)))
#define CLEAR_BIT(a, i)     ((a) &= ~(1 << (i)))
#define IS_BIT_SET(a, i)    ((a) & (1 << (i)))


//State machine variable
static void *       mFifoOut[NB_OUTPUT][1];
static void *       mFifoIn[1];
static Port         mPorts[NB_OUTPUT+1];
static Component    mPcmSplitter;

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

static Buffer_p             mSplitterOutputBuffer[NB_OUTPUT];
static Buffer_t             mSplitterOutputBufferHeader[NB_OUTPUT];
static Buffer_p             mSrcInputBuffer[NB_OUTPUT];
static ResampleContext      mResampleContext[NB_OUTPUT];


static t_uint16             mInputChannels;
static t_sample_freq        mInputFreq;
static t_uint16             mInputBlockSize;        

static t_sample_freq        mOutputFreq[NB_OUTPUT];
static t_uint16             mOutputChannels[NB_OUTPUT];
static t_uint32             mOutputChannelSwitchBoard[NB_OUTPUT][NMF_AUDIO_MAXCHANNELS];
static t_uint16             mOutputBlockSize[NB_OUTPUT];
static void                 (*mProcessOutputPort[NB_OUTPUT])(int idx); 

static int *                mHeap[NB_OUTPUT+1];
static int                  mHeapSize[NB_OUTPUT];

static int                  mInputBufferRefCount;

static int                  mDisabledPorts;
static int                  mPausedOutputs;

//pcmdump config
static PcmDump_t    mPcmDumpConfig[NB_OUTPUT+1];

////////////////////////////////////////////////////////////

#define DO_PROBE(port_idx, buf) pcmdump.probe((void*) buf->data, (t_uint24) buf->filledLen, &mPcmDumpConfig[port_idx])

static inline bool isPaused(int idx) { return IS_BIT_SET(mPausedOutputs, idx); }
static inline bool isDisabled(int idx) { return IS_BIT_SET(mDisabledPorts, idx); }

static inline bool 
allOutputsPausedOrDisabled() { return (((mDisabledPorts | mPausedOutputs) & ALL_OUTPUTS) == ALL_OUTPUTS); }


static inline bool 
isChannelSwitchingPort(int idx) {  
    int i;

    if (mOutputChannels[idx-OUT] != mInputChannels) {
        return true;
    }

    for (i = 0; i < mOutputChannels[idx-OUT]; i++) {
        if (mOutputChannelSwitchBoard[idx-OUT][i] != i) {
            return true;
        }
    }

    return false;
} 


static void 
doChannelSwitch(int idx, Buffer_p inbuf, Buffer_p outbuf)
{
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    t_uint16 i=0, j=0, k=0;

    

    for (i = 0, k = 0; i < inbuf->filledLen; i += mInputChannels) {
        for (j = 0; j < mOutputChannels[idx-OUT]; j++) {
            outbuf->data[k++] = inbuf->data[i + (mOutputChannelSwitchBoard[idx-OUT][j])];
        }
    }
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::applying ChannelSwitch on port %d filledLen=%d", idx,k);
    outbuf->filledLen       = k;
    outbuf->flags           = inbuf->flags;
    outbuf->byteInLastWord  = inbuf->byteInLastWord;
	outbuf->nTimeStamph     = inbuf->nTimeStamph;
	outbuf->nTimeStampl     = inbuf->nTimeStampl;
}

static void
doVolume(int idx, Buffer_p inbuf, Buffer_p outbuf) {
    
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst4(TRACE_FLOW,"PcmSplitterMpc::applying volume on port %d inbuf->filledLen=%d inbuf->data=%x  outbuf->data=%x", idx,inbuf->filledLen,inbuf->data,outbuf->data);
    
    volctrl[idx-OUT].process(inbuf->data, outbuf->data, (t_uint16)inbuf->filledLen);

    outbuf->filledLen       = inbuf->filledLen;
    outbuf->flags           = inbuf->flags;
    outbuf->byteInLastWord  = inbuf->byteInLastWord;
	outbuf->nTimeStamph     = inbuf->nTimeStamph;
	outbuf->nTimeStampl     = inbuf->nTimeStampl;
}

static void
returnOutputBuffer(int idx, Buffer_p outbuf) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    if (outbuf->flags & BUFFERFLAG_EOS) {
        proxy.eventHandler(OMX_EventBufferFlag, idx, outbuf->flags);
        OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::returnOutputBuffer EOS sent on output port %d", idx);
    }
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::returnOutputBuffer on port %d filledLen=%d", idx,outbuf->filledLen);
    Port_dequeueAndReturnBuffer(&mPorts[idx]);
}

static void
releaseInputBuffer(int idx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    

    CLEAR_BIT(mInputBufferRefCount, idx);
    if (mInputBufferRefCount == 0) {
        OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::releaseInputBuffer on output port %d", idx);
        Port_dequeueAndReturnBuffer(&mPorts[IN]);
    }
}

static void
applyVolume(int idx) {
    Buffer_p inbuf, outbuf;

    if (!IS_BIT_SET(mInputBufferRefCount, idx)
            || Port_queuedBufferCount(&mPorts[idx]) == 0) {
        return;
    }

    inbuf   = Port_getBuffer(&mPorts[IN], 0);
    outbuf  = Port_getBuffer(&mPorts[idx], 0);

    doVolume(idx, inbuf, outbuf);

    // probe output port
    DO_PROBE(idx, outbuf);

    releaseInputBuffer(idx);
    returnOutputBuffer(idx, outbuf);
}

static void
applySrcAndVolume(int idx) {
    Buffer_p inbuf,tmp_buf,outbuf;
    bool needInputBuf, filledOutputBuf;
    int i;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    if (Port_queuedBufferCount(&mPorts[idx]) == 0
            || !IS_BIT_SET(mInputBufferRefCount, idx)) {
        return;
    }

    inbuf   = Port_getBuffer(&mPorts[IN], 0);
    outbuf  = Port_getBuffer(&mPorts[idx], 0);

    //We need to use a temporary buffer for the input because the SRC will modify it during the processing
    tmp_buf = mSplitterOutputBuffer[idx-OUT];
    for(i=0;i<inbuf->allocLen;i++){
        tmp_buf->data[i] = inbuf->data[i];
    }
    tmp_buf->filledLen       = inbuf->filledLen;
    tmp_buf->flags           = inbuf->flags;
    tmp_buf->byteInLastWord  = inbuf->byteInLastWord;
    tmp_buf->nTimeStamph     = inbuf->nTimeStamph;
    tmp_buf->nTimeStampl     = inbuf->nTimeStampl;

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::applying SRC on port %d", idx);
   
    resample_process(&mResampleContext[idx-OUT], tmp_buf, outbuf, mOutputChannels[idx-OUT], &needInputBuf, &filledOutputBuf);
       
    if (needInputBuf) {
        releaseInputBuffer(idx);
    }

    if (!filledOutputBuf) {
        return;
    }

    doVolume(idx, outbuf, outbuf);

    // probe output port
    DO_PROBE(idx, outbuf);

    returnOutputBuffer(idx, outbuf);
}

static void
applyChannelSwitchAndVolume(int idx) {
    Buffer_p inbuf, outbuf;

    if (Port_queuedBufferCount(&mPorts[idx]) == 0
            || !IS_BIT_SET(mInputBufferRefCount, idx)) {
        return;
    }
    
    inbuf   = Port_getBuffer(&mPorts[IN], 0);
    outbuf  = Port_getBuffer(&mPorts[idx], 0);

    doChannelSwitch(idx, inbuf, outbuf);

    doVolume(idx, outbuf, outbuf);

    // probe output port
    DO_PROBE(idx, outbuf);

    releaseInputBuffer(idx);
    returnOutputBuffer(idx, outbuf);
}

static void
applyChannelSwitchAndSrcAndVolume(int idx) {
    Buffer_p    inbuf, outbuf;
    bool        needInputBuf, filledOutputBuf;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    if (Port_queuedBufferCount(&mPorts[idx]) == 0) {
        return;
    }

    if (mSrcInputBuffer[idx-OUT] == 0) {
        if (!IS_BIT_SET(mInputBufferRefCount, idx)) {
            return;
        }
        inbuf   = Port_getBuffer(&mPorts[IN], 0);
        doChannelSwitch(idx, inbuf, mSplitterOutputBuffer[idx-OUT]);
        mSrcInputBuffer[idx-OUT] = mSplitterOutputBuffer[idx-OUT];
    }

    outbuf  = Port_getBuffer(&mPorts[idx], 0);

    OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterMpc::applying SRC on port %d mSrcInputBuffer->filledLen=%d outbuf->allocLen=%s", idx,mSrcInputBuffer[idx-OUT]->filledLen,outbuf->allocLen);

    resample_process(&mResampleContext[idx-OUT], mSrcInputBuffer[idx-OUT], outbuf, mOutputChannels[idx-OUT], &needInputBuf, &filledOutputBuf);

    if (needInputBuf) {
        mSrcInputBuffer[idx-OUT] = 0;
        releaseInputBuffer(idx);
    }

    if (!filledOutputBuf) {
        return;
    }

    doVolume(idx, outbuf, outbuf);

    // probe output port
    DO_PROBE(idx, outbuf);

    returnOutputBuffer(idx, outbuf);
}

static void
Splitter_process(Component *this)
{
    t_uint16 idx;

    if (mInputBufferRefCount == 0 && Port_queuedBufferCount(&mPorts[IN])) {
        mInputBufferRefCount = ALL_OUTPUTS;  
    }

    if (allOutputsPausedOrDisabled()) {
        OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterMpc::Splitter_process all outputs paused or disabled");
        return;
    }

    // probe input port
    if (Port_queuedBufferCount(&mPorts[IN]) != 0) {
        Buffer_p inbuf = Port_getBuffer(&mPorts[IN], 0);
        // pcmdump!!!
        DO_PROBE(IN, inbuf);
    }

    for (idx = 1; idx <= NB_OUTPUT ; idx++) {
        if (isDisabled(idx) || isPaused(idx)) {
            if (IS_BIT_SET(mInputBufferRefCount, idx)) {
                releaseInputBuffer(idx);
            }
            continue;
        }
        (*mProcessOutputPort[idx-OUT])(idx);
    }
}

static void
initOutputVolctrl(int idx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::initOutputVolctrl idx=%d mOutputChannels=%d", idx,mOutputChannels[idx-OUT]);
    volctrl[idx-OUT].init(mOutputChannels[idx-OUT], mOutputChannels[idx-OUT], mOutputFreq[idx-OUT]);
}

static void *
portMalloc(int idx, int size) {
    void * p;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::allocating %d words on port %d", size, idx);
    ASSERT(size <= mHeapSize[idx-OUT]);
    p = mHeap[idx-OUT];
    mHeap[idx-OUT] += size;
    mHeapSize[idx-OUT] -= size;
    return p;
}

static bool
isLowLatencySRC(int idx) {
    if ((mOutputFreq[idx-OUT] == FREQ_8KHZ || mOutputFreq[idx-OUT] == FREQ_16KHZ || mOutputFreq[idx-OUT] == FREQ_48KHZ)
            && (mInputFreq == FREQ_8KHZ || mInputFreq == FREQ_16KHZ || mInputFreq == FREQ_48KHZ)) {
        return true;
    } else {
        return false;
    }
}

static void
initOutputSrc(int idx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    int status, heapsize;
    void *buf;
    

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initOutputSrc idx=%d", idx);
    if (mOutputFreq[idx-OUT] == mInputFreq) {
        return;
    }

    
    
    if (isLowLatencySRC(idx)) {
        OstTraceFiltInst4(TRACE_FLOW,"PcmSplitterMpc::initOutputSrc Low Latency mInputFreq=%d mOutputFreq=%d mInputBlockSize=%d mOutputChannels=%d", mInputFreq,mOutputFreq[idx-OUT],mInputBlockSize,mOutputChannels[idx-OUT]);

        resample_calc_max_heap_size_fixin_fixout(0, 0,SRC_LOW_LATENCY_IN_MSEC,&heapsize,mInputBlockSize,mOutputChannels[idx-OUT],0);

        buf = portMalloc(idx, heapsize); 
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                mOutputFreq[idx-OUT], SRC_LOW_LATENCY_IN_MSEC, mOutputChannels[idx-OUT],mInputBlockSize);
    } 
    else {
        OstTraceFiltInst4(TRACE_FLOW,"PcmSplitterMpc::initOutputSrc mInputFreq=%d mOutputFreq=%d mInputBlockSize=%d mOutputChannels=%d", mInputFreq,mOutputFreq[idx-OUT],mInputBlockSize*mOutputChannels[idx-OUT],mOutputChannels[idx-OUT]);
        resample_calc_max_heap_size_fixin_fixout(
                mInputFreq, mOutputFreq[idx-OUT], SRC_STANDARD_QUALITY, &heapsize,
                mInputBlockSize*mOutputChannels[idx-OUT], mOutputChannels[idx-OUT],0);
        buf = portMalloc(idx, heapsize); 
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                buf, heapsize, &mResampleContext[idx-OUT], mInputFreq, 
                mOutputFreq[idx-OUT], SRC_STANDARD_QUALITY, mOutputChannels[idx-OUT], mInputBlockSize*mOutputChannels[idx-OUT]);

    }
    ASSERT(status == 0);
    
    buf = portMalloc(idx, mInputBlockSize * mOutputChannels[idx-OUT]);
    mSplitterOutputBufferHeader[idx-OUT].data = buf;
}

static void
initOutputPortProcessFunction(int idx) {
    TRACE_t * this = (TRACE_t *) &mPcmSplitter;

    if (!isChannelSwitchingPort(idx)) {
        if (mInputFreq == mOutputFreq[idx-OUT]) {
            OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initOutputPortProcessFunction port %d => applyVolume", idx);
            mProcessOutputPort[idx-OUT] = applyVolume;
        } else {
            OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initOutputPortProcessFunction port %d => applySrcAndVolume", idx);
            mProcessOutputPort[idx-OUT] = applySrcAndVolume;
        }
    }
    else {
        if (mInputFreq == mOutputFreq[idx-OUT]) {
            OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initOutputPortProcessFunction port %d => applyChannelSwitchAndVolume", idx);
            mProcessOutputPort[idx-OUT] = applyChannelSwitchAndVolume;
        } else {
            OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initOutputPortProcessFunction port %d => applyChannelSwitchAndSrcAndVolume", idx);
            mProcessOutputPort[idx-OUT] = applyChannelSwitchAndSrcAndVolume;
        }
    }
}

static void
initEnabledOutputPort(int idx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::initEnabledOutputPort idx=%d", idx);

    initOutputVolctrl(idx);
    initOutputSrc(idx);
    initOutputPortProcessFunction(idx);
}

static void
initAllEnabledOutputPorts() {
    int i;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    for (i = 1 ; i <= NB_OUTPUT; i++) {
        if (isDisabled(i)) continue;
        initEnabledOutputPort(i);
    }
}


void resetPort(t_uint16 idx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::resetPort idx=%d", idx);

    if (idx == IN) {
        mInputBufferRefCount = 0;
    }
    else {
        mSrcInputBuffer[idx-OUT]          = 0; 
        mSplitterOutputBuffer[idx-OUT]    = &mSplitterOutputBufferHeader[idx-OUT];
    }
}

static void
Splitter_reset(Component *this)
{
    t_uint16 idx;

    OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterMpc::Splitter_reset");

    for (idx = 0 ; idx <= NB_OUTPUT ; idx++) {
        resetPort(idx);
    }
}

void
Splitter_disablePortIndication(t_uint32 portIdx) {
    t_uint16 idx;

    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::Splitter_disablePortIndication  on port %d", (t_uint24)portIdx);

    SET_BIT(mDisabledPorts, portIdx);

    resetPort((t_uint16)portIdx);

    if (Port_queuedBufferCount(&mPcmSplitter.ports[IN])) {
        Component_scheduleProcessEvent(&mPcmSplitter);
    }
}

void
Splitter_enablePortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::Splitter_enablePortIndication  on port %d", (t_uint24)portIdx);

    CLEAR_BIT(mDisabledPorts, portIdx);

    if (portIdx != IN) {
        if (!isDisabled(IN)) {
		  initEnabledOutputPort((int)portIdx);
        }
    } else {
        initAllEnabledOutputPorts();
    }
}

void
Splitter_flushPortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::Splitter_flushPortIndication  on port %d", (t_uint24)portIdx);
    resetPort((t_uint16)portIdx);
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setMuteOutputPort)(t_uint16 portIdx, BOOL bMute) {
    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setMute)) {
        volctrl[portIdx-OUT].setMute(bMute);
    }
}

void METH(setBalanceOutputPort)(t_uint16 portIdx, t_sint16 nBalance) {
    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setBalance)) {
        volctrl[portIdx-OUT].setBalance(nBalance);
    }
}

void METH(setVolumeOutputPort)(t_uint16 portIdx, t_sint16 nVolume) {
    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

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
    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

    if (!IS_NULL_INTERFACE(volctrl[portIdx-OUT], setVolumeRamp)) {
        volctrl[portIdx-OUT].setVolumeRamp(nStartVolume, nEndVolume, nChannels, nDuration, bTerminate);
    }
}

void METH(setChannelSwitchBoardOutputPort)(t_uint16 portIdx, t_uint16 channelSwitchBoard[]) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    int i;

    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

    for (i = 0; i < mOutputChannels[portIdx-OUT]; i++) {
        mOutputChannelSwitchBoard[portIdx-OUT][i] = channelSwitchBoard[i]; 
        OstTraceFiltInst4(TRACE_FLOW,"PcmSplitterMpc::setChannelSwitchBoardOutputPort portIdx=%d  mOutputChannelSwitchBoard[%d][%d]=%d", portIdx, portIdx-OUT,i,mOutputChannelSwitchBoard[portIdx-OUT][i]);
    }

    initOutputPortProcessFunction(portIdx);
}

void METH(setPausedOutputPort)(t_uint16 portIdx, BOOL bIsPaused) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    PRECONDITION(portIdx <= NB_OUTPUT && portIdx >= OUT);

    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::setPausedOutputPort portIdx=%d  bIsPaused=%d", portIdx, bIsPaused);

    if (bIsPaused) {
        SET_BIT(mPausedOutputs, portIdx);
    } else {
        CLEAR_BIT(mPausedOutputs, portIdx);
    }

    if (mPcmSplitter.reset && Port_queuedBufferCount(&mPcmSplitter.ports[IN])) {
        Component_scheduleProcessEvent(&mPcmSplitter);
    }
}

void METH(setInputPortParameter)(InputPortParam_t inputPortParam) {
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    mInputFreq         = inputPortParam.nSamplingRate;
    mInputChannels     = inputPortParam.nChannels;
    mInputBlockSize    = inputPortParam.nBlockSize;

    OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterMpc::setInputPortParameter Freq=%d Channels=%d Blocksize=%d", mInputFreq, mInputChannels, mInputBlockSize);
}

void METH(setOutputPortParameter)(t_uint16 idx, OutputPortParam_t outputPortParam) {

    TRACE_t * this = (TRACE_t *) &mPcmSplitter;
    int i;

    PRECONDITION(idx <= NB_OUTPUT && idx >= OUT);

    // Params
    mOutputChannels[idx-OUT]      = outputPortParam.nChannels;
    mOutputFreq[idx-OUT]          = outputPortParam.nSamplingRate; 
    mOutputBlockSize[idx-OUT]     = outputPortParam.nBlockSize;
    mHeap[idx-OUT]                = outputPortParam.pHeap;
    mHeapSize[idx-OUT]            = outputPortParam.nHeapSize;

    OstTraceFiltInst4(TRACE_FLOW,"PcmSplitterMpc::setOutputPortParameter idx=%d Freq=%d Channels=%d Blocksize=%d", idx, mOutputFreq[idx-OUT], mOutputChannels[idx-OUT], mOutputBlockSize[idx-OUT]);

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

void METH(setParameter)(SplitterParam_t splitterParam) {
    int i;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    mPcmSplitter.fsm.traceObject.mTraceInfoPtr = (TraceInfo_t *)splitterParam.traceAddr;

    OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterMpc::setParameter");

    METH(setInputPortParameter)(splitterParam.inputPortParam);

    for (i = 1; i <= NB_OUTPUT; i++) {
       METH(setOutputPortParameter)(i, splitterParam.outputPortParam[i-OUT]); 
    }

}

void METH(fsmInit) (fsmInit_t initFsm)
{
    t_uint16 idx;
    TRACE_t * this = (TRACE_t *)&mPcmSplitter;

    // trace init (mandatory before port init)
    FSM_traceInit(&mPcmSplitter, initFsm.traceInfoAddr, initFsm.id1);

    OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterMpc::fsmInit portsDisabled=%d", initFsm.portsDisabled);

    Port_init(&mPorts[IN], InputPort, false,
              false, 0, &mFifoIn, 1, &inputport,
              IN, (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << IN)), &mPcmSplitter);

    for (idx = 1 ; idx <= NB_OUTPUT ; idx++) {
        Port_init(&mPorts[idx], OutputPort, false,
                  false, 0, &mFifoOut[idx-OUT],  1, &outputport[idx-OUT],
                  idx, (initFsm.portsDisabled & (1 << idx)), (initFsm.portsTunneled & (1 << idx)), &mPcmSplitter);
    }

    mDisabledPorts = initFsm.portsDisabled;

    mPcmSplitter.reset                  = Splitter_reset;
    mPcmSplitter.process                = Splitter_process;
    mPcmSplitter.disablePortIndication  = Splitter_disablePortIndication;
    mPcmSplitter.enablePortIndication   = Splitter_enablePortIndication;
    mPcmSplitter.flushPortIndication    = Splitter_flushPortIndication;

    if (!isDisabled(IN)) {
        initAllEnabledOutputPorts();
    }

    Component_init(&mPcmSplitter, NB_OUTPUT + 1, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) {
}

void METH(processEvent)() {
    Component_processEvent(&mPcmSplitter);
}

void METH(emptyThisBuffer)(Buffer_p buf) {

    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::emptyThisBuffer filledLen=%d flags=%d", (t_uint24)buf->filledLen, (t_uint24)buf->flags);
    
    Component_deliverBuffer(&mPcmSplitter, IN, buf);
}

// events from volctrl 
void METH(eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 idx) {
    ASSERT(event == OMX_DSP_EventIndexSettingChanged);
    proxy.eventHandler(event, idx+OUT, data2);
}

void METH(fillThisBuffer)(Buffer_p buf, t_uint8 idx) {

    TRACE_t * this = (TRACE_t *)&mPcmSplitter;
    OstTraceFiltInst2(TRACE_FLOW,"PcmSplitterMpc::fillThisBuffer on port %d allocLen=%d", OUT+idx, (t_uint24)buf->allocLen);

	buf->nTimeStamph = 0;
	buf->nTimeStampl = 0;
    Component_deliverBuffer(&mPcmSplitter, OUT+idx, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mPcmSplitter, cmd, param);
}

void METH(configure_pcmprobe)(PcmDumpConfigure_t sConfigure)
{
    t_sint24 tab_idx;
    TRACE_t * this = (TRACE_t*)&mPcmSplitter;
    ASSERT(sConfigure.nmf_port_idx < (NB_OUTPUT + 1));

    // configure the pcmdump singleton
    OstTraceFiltInst3(TRACE_DEBUG, "PcmSplitterMpc::configure_pcmprobe: nmf port idx: %d, enable: %d, omx port idx: %d", sConfigure.nmf_port_idx, sConfigure.enable, sConfigure.omx_port_idx);
    OstTraceFiltInst2(TRACE_DEBUG, "PcmSplitterMpc::configure_pcmprobe: buffer: 0x%x, buffer size: %d", (t_uint24)sConfigure.buffer, sConfigure.buffer_size);

    pcmdump.configure(sConfigure, this, &complete, &mPcmDumpConfig[sConfigure.nmf_port_idx]);
}
