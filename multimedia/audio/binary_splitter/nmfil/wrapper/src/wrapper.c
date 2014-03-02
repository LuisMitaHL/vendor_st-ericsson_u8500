/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <binary_splitter/nmfil/wrapper.nmf>
#include <dbc.h>
#include "fsm/component/include/Component.inl"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "binary_splitter_nmfil_wrapper_src_wrapperTraces.h"
#endif

#define NB_OUTPUT 2
#define NB_INPUT 1

#define NMF_AUDIO_MAXCHANNELS 16

#define OUT 1
#define IN  0

//State machine variable
static void *       mFifoOut[NB_OUTPUT][1];
static void *       mFifoIn[NB_INPUT];
static Port         mPorts[NB_OUTPUT+NB_INPUT];
static Component    mSplitter;

// AV synchronisation
static t_sample_freq        mSampleFreq[NB_OUTPUT];
static AVSynchro_t          mTimeStampInfo;
static bool                 mTimeStampUpdateFlag;
static t_uint16	            mNbChannels[NB_OUTPUT+NB_INPUT];
static t_uint16             mSampleBitSize;

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

typedef struct
{
    Buffer_p    BufferPtr;
    t_uint32    mOutputChannel[NMF_AUDIO_MAXCHANNELS];
    bool        mChannelSwitchFlag;
}OutputBuffer;

static OutputBuffer     mOutputBuf[NB_OUTPUT];
Buffer_p                mInputBufPtr;

////////////////////////////////////////////////////////////
void
BinarySplitter_process(Component *this)
{
    t_uint16 idx, i, j, k, disabledports;
    int * inbuf = NULL;
    int * outbuf = NULL;

    if (!Port_isEnabled(&this->ports[IN])) {
        return;
    }

    if (!Port_queuedBufferCount(&this->ports[IN])) return;

    disabledports = 0;

    for (idx=0; idx<NB_OUTPUT ; idx++) {
        if (!Port_queuedBufferCount(&this->ports[OUT+idx]) &&
                (Port_isEnabled(&this->ports[OUT+idx]))) {
            return;
        }
    }

    // check that atleast one output port is enabled
    for (idx=0; idx<NB_OUTPUT ; idx++) {
        if (!Port_isEnabled(&this->ports[OUT+idx])) {
            disabledports++;
        }
        if (disabledports == NB_OUTPUT) {
            return;
        }
    }

    OstTraceFiltInst0(TRACE_FLOW,"BinarySplitterMpc::process");

    mInputBufPtr = Port_getBuffer(&this->ports[IN], 0);

    //apply splitter
    for (idx=0;idx<NB_OUTPUT;idx++)
    {
        if (Port_isEnabled(&this->ports[OUT+idx]))
        {
            mOutputBuf[idx].BufferPtr = Port_getBuffer(&this->ports[OUT+idx], 0);

#ifdef ENABLE_AVSYNC
            // AV synchronisation
            if ( mInputBufPtr->flags & BUFFERFLAG_STARTTIME)
            {	// then save initial TimeStamp value
                TSinit(&mTimeStampInfo, mInputBufPtr->nTimeStamph, mInputBufPtr->nTimeStampl);
                mTimeStampUpdateFlag = true;
                // and propagate StartTime Flag
                mOutputBuf[idx].BufferPtr->flags  |= mInputBufPtr->flags & BUFFERFLAG_STARTTIME;
            }
#endif //ENABLE_AVSYNC
            inbuf = (int *)mInputBufPtr->data;
            outbuf = (int *)mOutputBuf[idx].BufferPtr->data;
            if (mOutputBuf[idx].mChannelSwitchFlag == true)
            {
                k = 0;
                for (i = 0; i < mInputBufPtr->filledLen; i+=mNbChannels[IN]) {
                    for (j = 0; j < mNbChannels[OUT+idx]; j++)
                    {
                        outbuf[k++] = inbuf[i + (mOutputBuf[idx].mOutputChannel[j])];
                    }
                }

                mOutputBuf[idx].BufferPtr->filledLen        = k;
                mOutputBuf[idx].BufferPtr->byteInLastWord   = mInputBufPtr->byteInLastWord;
            }
            else
            {
                for (i = 0; i < mInputBufPtr->filledLen; i++) {
                    outbuf[i] = inbuf[i];
                }

                mOutputBuf[idx].BufferPtr->filledLen        = mInputBufPtr->filledLen ;
                mOutputBuf[idx].BufferPtr->byteInLastWord   = mInputBufPtr->byteInLastWord;
            }

#ifdef ENABLE_AVSYNC
            // Av Synchronisation update timestamp
            if (mTimeStampUpdateFlag)
            {
                TScomputation(&mTimeStampInfo, mOutputBuf[idx].BufferPtr, mOutputBuf[idx].BufferPtr->filledLen, mSampleFreq[idx], mNbChannels[IN]);
            }
#endif // ENABLE_AVSYNC

            Port_dequeueAndReturnBuffer(&this->ports[OUT+idx]);
        }
    }

    //EOS handling
    if (mInputBufPtr->flags & BUFFERFLAG_EOS)
    {
        for (idx=0; idx<NB_OUTPUT ; idx++) {
            if (Port_isEnabled(&this->ports[OUT+idx]))
            {
                mOutputBuf[idx].BufferPtr->flags |= BUFFERFLAG_EOS;
                proxy.eventHandler(OMX_EventBufferFlag, OUT+idx,
                                   mOutputBuf[idx].BufferPtr->flags);
                OstTraceFiltInst1(TRACE_FLOW,"BinarySplitterMpc::process EOS sent on output port %d", OUT+idx);
            }
        }
    }

    //return buffers
    Port_dequeueAndReturnBuffer(&this->ports[IN]);
}

void
BinarySplitter_reset(void)
{
    t_uint16 idx, ch;

    mInputBufPtr = NULL;

    for (idx=0 ; idx<NB_OUTPUT ; idx++) {
        mOutputBuf[idx].BufferPtr = NULL;
        mSampleFreq[idx] = FREQ_UNKNOWN;
    }

#ifdef ENABLE_AVSYNC
    TSreset(&mTimeStampInfo);
    mTimeStampUpdateFlag = false;
#endif //ENABLE_AVSYNC
}

// Interface for fsm
static void
reset(Component *this) {
    BinarySplitter_reset();
}

void
BinarySplitter_disablePortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"BinarySplitterMpc::BinarySplitter_disablePortIndication  on port %d", (t_uint16)portIdx);

}

void
BinarySplitter_enablePortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"BinarySplitterMpc::BinarySplitter_enablePortIndication  on port %d", (t_uint16)portIdx);

    if (portIdx == IN) {
        BinarySplitter_reset();
    }

    if (portIdx != IN) {
        //call newFormat only if wrapper is in executing state
        if (mSampleFreq[portIdx-1] != FREQ_UNKNOWN) {
            if (!IS_NULL_INTERFACE(outputsettings[portIdx-1], newFormat))
            {
                outputsettings[portIdx-1].newFormat(mSampleFreq[portIdx-1],mNbChannels[portIdx],16);
            }
        }
    }
}

void
BinarySplitter_flushPortIndication(t_uint32 portIdx) {
    TRACE_t * this = (TRACE_t *)&mSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"BinarySplitterMpc::BinarySplitter_flushPortIndication  on port %d", (t_uint16)portIdx);

    BinarySplitter_reset();
}



////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(start)(void) {
    t_uint16 idx, ch;

    for (idx=0 ; idx<NB_OUTPUT+NB_INPUT ; idx++) {
        mNbChannels[idx] = 0;
    }
    for (idx=0; idx<NB_OUTPUT; idx++)
    {
        mSampleFreq[idx] = FREQ_UNKNOWN;
        for (ch=0; ch<NMF_AUDIO_MAXCHANNELS; ch++)
        {
            mOutputBuf[idx].mOutputChannel[ch] = 0;
            mOutputBuf[idx].mChannelSwitchFlag = false;
        }
    }

}

void METH(setParameter)(
    t_sample_freq freq,
    t_channelConfig chConfig,
    t_uint16 blocksize) {

    t_uint16 idx;

    for (idx=0 ; idx<NB_INPUT ; idx++) {
        mNbChannels[IN] = chConfig.nChans[0];
    }

    for (idx=0 ; idx<NB_OUTPUT ; idx++) {
        mSampleFreq[idx]     = freq;
        mNbChannels[idx+1]   = chConfig.nChans[idx+1];
    }

}

void METH(fsmInit) (fsmInit_t initFsm) {
    t_uint16 idx = 0;

    // trace init (mandatory before port init)
    FSM_traceInit(&mSplitter, initFsm.traceInfoAddr, initFsm.id1);

    mSplitter.process = BinarySplitter_process;
    Port_init(&mPorts[IN], InputPort, false, false,
              0, &mFifoIn, 1, &inputport, IN,
              (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mSplitter);
    for (idx=0 ; idx<NB_OUTPUT ; idx++) {
        Port_init(&mPorts[OUT+idx], OutputPort, false,
                  false, 0, &mFifoOut[idx],  1, &outputport[idx],
                  OUT+idx, (initFsm.portsDisabled & (1 << (OUT+idx))), (initFsm.portsTunneled & (1 << (OUT+idx))), &mSplitter);
    }

    mSplitter.reset                   = reset;
    mSplitter.disablePortIndication   = BinarySplitter_disablePortIndication;
    mSplitter.enablePortIndication    = BinarySplitter_enablePortIndication;
    mSplitter.flushPortIndication     = BinarySplitter_flushPortIndication;

    Component_init(&mSplitter, NB_OUTPUT + 1, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx)
{
    t_uint16 i = 0;
    PRECONDITION(chans_nb == mNbChannels[IN]);
    PRECONDITION(sample_bitsize == 16);
    for (i=0; i<NB_OUTPUT; i++)
    {
        //PRECONDITION(freq == mSampleFreq[i]);
        if (!IS_NULL_INTERFACE(outputsettings[i], newFormat))
        {
            outputsettings[i].newFormat(freq,mNbChannels[i+OUT],sample_bitsize);
        }
    }
}

void METH(setOutputChannelConfig)(t_uint16 portIdx, t_OutChannelConfig OutChannelConfig)
{
    t_uint16 ch;
    TRACE_t * this = (TRACE_t *)&mSplitter;
    OstTraceFiltInst1(TRACE_FLOW,"BinarySplitterMpc::setOutputChannelConfig portIdx=%d", portIdx);

    PRECONDITION(mNbChannels[portIdx] != 0);

    for (ch=0; ch<mNbChannels[portIdx]; ch++)
    {
        mOutputBuf[portIdx-OUT].mOutputChannel[ch] = OutChannelConfig.nOutPutChannel[ch];
    }
    mOutputBuf[portIdx-OUT].mChannelSwitchFlag = true;
}

void METH(processEvent)()
{
    Component_processEvent(&mSplitter);
}

void METH(emptyThisBuffer)(Buffer_p buf, t_uint8 idx)
{
    Component_deliverBufferCheck(&mSplitter, IN+idx, buf);
}

void METH(fillThisBuffer)(Buffer_p buf, t_uint8 idx)
{
    Component_deliverBufferCheck(&mSplitter, OUT+idx, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
    Component_sendCommand(&mSplitter, cmd, param);
}

void METH(setTrace)(t_uint32 addr){
    mSplitter.fsm.traceObject.mTraceInfoPtr = (TraceInfo_t *)addr;
}
