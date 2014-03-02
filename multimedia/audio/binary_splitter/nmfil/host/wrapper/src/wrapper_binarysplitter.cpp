/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <binary_splitter/nmfil/host/wrapper.nmf>
#include <string.h>
#include <armnmf_dbc.h>

//#define BINARYSPLITTER_TRACE_CONTROL

#ifdef BINARYSPLITTER_TRACE_CONTROL
#include <stdio.h>
#endif

binary_splitter_nmfil_host_wrapper::binary_splitter_nmfil_host_wrapper()
{
    t_uint16 idx;

    reset();

    mConfig.mode = PCMPROCESSING_MODE_COUNT;
    mConfig.block_size = 0;
    mConfig.out_freq = FREQ_UNKNOWN;
    mConfig.out_nof_channels = 0;
    mConfig.out_nof_bits_per_sample = 0;

    for (idx=0; idx<NB_OUTPUT; idx++)
    {
        mSampleFreq[idx] = FREQ_UNKNOWN;
        mOutputBuf[idx].bOutputRequired = false;
        mOutputBuf[idx].bIsSynchronized = false;
    }
    mNbChannels = 0;
    mSampleBitSize = 0;
}

binary_splitter_nmfil_host_wrapper::~binary_splitter_nmfil_host_wrapper()
{
}

t_nmf_error METH(construct)(void) {
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::construct\n");
#endif
    return NMF_OK;
}

void METH(destroy)(void) {
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::destroy\n");
#endif
}

void METH(start)(void) {
    t_uint16 idx;

    for (idx=0; idx<NB_OUTPUT; idx++)
    {
        mSampleFreq[idx] = FREQ_UNKNOWN;
    }
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::start\n");
#endif
}

void METH(stop)(void) {
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::stop\n");
#endif
}

void METH(process)()
{
    t_uint16    idx, nbEmptyOutput = 0;
    t_sint16	*mInputBuffer;
    t_sint16	*mOutputBuffer;

    if (!mPorts[INPUT_PORT_IDX].isEnabled()) {
        return;
    }

    if (!mPorts[INPUT_PORT_IDX].queuedBufferCount()) return;

    for (idx=0; idx<NB_OUTPUT ; idx++) {
        if (!mPorts[OUTPUT_PORT_IDX + idx].queuedBufferCount() &&
                mOutputBuf[idx].bIsSynchronized &&
                mPorts[OUTPUT_PORT_IDX + idx].isEnabled()) {
            return;
        }
        else if (mPorts[OUTPUT_PORT_IDX + idx].queuedBufferCount() &&
                 mPorts[OUTPUT_PORT_IDX + idx].isEnabled()) {
            mOutputBuf[idx].bOutputRequired = true;
        } else {
            nbEmptyOutput ++;
        }
    }

    if (nbEmptyOutput == NB_OUTPUT) {
        return;
    }
    else
    {
        mInputBufPtr = mPorts[INPUT_PORT_IDX].getBuffer(0);

        //Apply binary splitter
        for (idx=0;idx<NB_OUTPUT;idx++)
        {
            if (mOutputBuf[idx].bOutputRequired == true)
            {
                if(mPorts[OUTPUT_PORT_IDX + idx].isEnabled())
                {
                    mOutputBuf[idx].BufferPtr = mPorts[OUTPUT_PORT_IDX + idx].getBuffer(0);

                    ARMNMF_DBC_ASSERT(mInputBufPtr->nFilledLen <= mOutputBuf[idx].BufferPtr->nAllocLen);

                    mOutputBuffer = (t_sint16 *)((t_uint32)mOutputBuf[idx].BufferPtr->pBuffer + mOutputBuf[idx].BufferPtr->nOffset);
                    mInputBuffer= (t_sint16 *)((t_uint32)mInputBufPtr->pBuffer + mInputBufPtr->nOffset);

                    memcpy(mOutputBuffer ,mInputBuffer, mInputBufPtr->nFilledLen);

                    mOutputBuf[idx].BufferPtr->nFilledLen = mInputBufPtr->nFilledLen ;
                    mOutputBuf[idx].BufferPtr->nFlags     = mInputBufPtr->nFlags;
                    mOutputBuf[idx].BufferPtr->nTimeStamp = mInputBufPtr->nTimeStamp;

                    mPorts[OUTPUT_PORT_IDX + idx].dequeueAndReturnBuffer();
                    mOutputBuf[idx].bOutputRequired = false;
                }
            }
        }
    }

    //EOS handling
    for (idx=0; idx<NB_OUTPUT ; idx++) {
        // Avoid enable port and process calls not in synch
        if ((mPorts[OUTPUT_PORT_IDX + idx].isEnabled() == true) && (mOutputBuf[idx].BufferPtr != NULL)) {
            if(mOutputBuf[idx].BufferPtr->nFlags & OMX_BUFFERFLAG_EOS) {
                proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT_IDX+idx, mOutputBuf[idx].BufferPtr->nFlags);
                //  reset();
            }
        }
    }

    //return buffers
    mPorts[INPUT_PORT_IDX].dequeueAndReturnBuffer();

}

// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH
(reset)() {
    mInputBufPtr = NULL;
    for (int idx=0 ; idx<NB_OUTPUT ; idx++) {
        mOutputBuf[idx].BufferPtr = NULL;
        mOutputBuf[idx].bOutputRequired = false;
    }
}

void METH
(disablePortIndication)(t_uint32 portIdx) {
    t_uint16 idx;
    for (idx=0 ; idx<NB_OUTPUT ; idx++){
        if (mPorts[OUTPUT_PORT_IDX+idx].queuedBufferCount()) {
            scheduleProcessEvent();
            break;
        }
    }


#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper:: disablePortIndication\n");
    printf("\tportIdx=%u\n",portIdx);
#endif
}

void METH
(enablePortIndication)(t_uint32 portIdx) {
    int i;
    //call newFormat only if wrapper is in executing state
    if (portIdx == INPUT_PORT_IDX) {
        for (i=0; i<NB_OUTPUT; i++)
        {
            mSampleFreq[i] = FREQ_UNKNOWN;
        }
    }
    else if (mSampleFreq[portIdx-1] != FREQ_UNKNOWN) {
        scheduleProcessEvent();
    }
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper:: enablePortIndication\n");
    printf("\tportIdx=%u\n",portIdx);
#endif
}

void METH
(flushPortIndication)(t_uint32 portIdx) {

#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper:: flushPortIndication\n");
    printf("\tportIdx=%u\n",portIdx);
#endif
    mInputBufPtr = NULL;
    for (int idx=0 ; idx<NB_OUTPUT ; idx++) {
        mOutputBuf[idx].BufferPtr = NULL;
        mOutputBuf[idx].bOutputRequired = false;
    }
}



////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setParameter)(t_pcmprocessing_config_host config)
{
    t_uint16 idx = 0;
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::setParameter\n");
    printf("\tmode=%u\n", (unsigned int)config.mode);
    printf("\tblock_size=%u\n", (unsigned int)config.block_size);
    printf("\tout_freq=%u\n", (unsigned int)config.out_freq);
    printf("\tout_nof_channels=%u\n", (unsigned int)config.out_nof_channels);
    printf("\tout_nof_bits_per_sample=%u\n", (unsigned int)config.out_nof_bits_per_sample);
#endif
    ARMNMF_DBC_PRECONDITION(config.block_size > 0);
    // Store PCM proc config for later use...
    mConfig = config;
    //ARMNMF_DBC_PRECONDITION(config.out_nof_channels == 1 || config.out_nof_channels == 2);

    mNbChannels     = config.out_nof_channels;
    mSampleBitSize  = config.out_nof_bits_per_sample;

    for (idx=0; idx<NB_OUTPUT; idx++)
    {
        mSampleFreq[idx]     = config.out_freq;
    }
}

void METH(fsmInit) (fsmInit_t initFsm)
{
    t_uint16 idx = 0;

#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::fsmInit \n");
#endif
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    mPorts[INPUT_PORT_IDX].init(InputPort, false, false, 0, 0, 1, &inputport[idx], INPUT_PORT_IDX, (initFsm.portsDisabled & (1 << (INPUT_PORT_IDX))), (initFsm.portsTunneled & (1 << (INPUT_PORT_IDX))), this);
    for (idx=0 ; idx<NB_OUTPUT ; idx++) {
        mPorts[OUTPUT_PORT_IDX + idx].init(OutputPort, false, false, 0, 0, 1, &outputport[idx], OUTPUT_PORT_IDX + idx, (initFsm.portsDisabled & (1 << (OUTPUT_PORT_IDX + idx))), (initFsm.portsTunneled & (1 << (OUTPUT_PORT_IDX + idx))), this);
    }

    // Init this component...
    init(NB_OUTPUT + 1, mPorts, &proxy, &me, false);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx) {
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::newFormat\n");
    printf("\tfreq=%u\n", (unsigned int)freq);
    printf("\tchannels=%u\n", (unsigned int)chans_nb);
    printf("\tbits/sample=%u\n", (unsigned int)sample_bitsize);
#endif
    ARMNMF_DBC_PRECONDITION(chans_nb == mNbChannels);
}

void METH(setSynchronizedOutputPort)(t_uint16 portIdx, BOOL bIsSynchronized) {
    mOutputBuf[portIdx].bIsSynchronized = bIsSynchronized;
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx)
{
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::emptyThisBuffer \n");
    printf("\tbuffer->nAllocLen=%u\n",buffer->nAllocLen);
    printf("\tbuffer->nFilledLen=%u\n",buffer->nFilledLen);
#endif
    if (mPorts[INPUT_PORT_IDX].isEnabled() == false) {
        // temporary fix: as the connected pcmadapter might be running,
        // it might still be delivering bffer on the input port
        // but as it is disabled, we should not get any, so
        // give it back to the pcmadapter
        inputport[idx].fillThisBuffer(buffer);
        return;
    }
    Component::deliverBuffer(INPUT_PORT_IDX + idx, buffer);
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer , t_uint8 idx)
{
    OMX_BUFFERHEADERTYPE* omxBuffer = (OMX_BUFFERHEADERTYPE*)buffer;
    omxBuffer->nFilledLen = 0;
#ifdef BINARYSPLITTER_TRACE_CONTROL
    printf("binary_splitter_nmfil_host_wrapper::fillThisBuffer \n");
    printf("\tbuffer->nAllocLen=%u\n",buffer->nAllocLen);
    printf("\tbuffer->nFilledLen=%u\n",buffer->nFilledLen);
#endif
    Component::deliverBuffer(OUTPUT_PORT_IDX + idx, buffer);
}

