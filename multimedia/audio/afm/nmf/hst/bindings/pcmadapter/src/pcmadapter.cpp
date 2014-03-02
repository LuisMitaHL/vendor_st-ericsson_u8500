/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   pcmadapter.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/bindings/pcmadapter.nmf>
#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <armnmf_dbc.h>
#include <string.h>

#include "bindings_optim.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_bindings_pcmadapter_src_pcmadapterTraces.h"
#endif

#define PCMADAPTER_TRACE_CONTROL

#define MAX(a, b)       ((a) > (b) ? (a) : (b))

#define MAX_OST_SIZE 56
#define MAX_COMMON_TRACE 8
#define HALF_MAX_COMMON_TRACE (MAX_COMMON_TRACE >> 1)


OMX_S64 hst_bindings_pcmadapter::computeTimeStamp(t_uint16 nbChannels, OMX_S64 *nbSamples, t_uint16 nbNewSampleSize)
{
    OMX_S64 timestamp;

    if ((mSampleFreq > 0) &&
            (nbChannels > 0))
    {
        timestamp = ((*nbSamples * 10000)/(mSampleFreq * nbChannels)) * 100;
        timestamp += mInitialTS;
    }
    else
    {
        timestamp = 0;
    }
    *nbSamples += nbNewSampleSize;

    return timestamp;
}


void hst_bindings_pcmadapter::initTimeStampComputation()
{
    if (mInputBuf->nFlags & OMX_BUFFERFLAG_STARTTIME) {
        mInputBuf->nFlags ^= OMX_BUFFERFLAG_STARTTIME;
        mInitialTS = mInputBuf->nTimeStamp;
        mNbSamplesForTSComputationInput = 0;
        if (mOutputBuf)
        {
            mSetStartTime = false;
            mOutputBuf->nFlags |= OMX_BUFFERFLAG_STARTTIME;
            mNbSamplesForTSComputationOutput = - (mOutputBuf->nOffset*8)/mBitPerSampleOut;
        }
        else
        {
            mSetStartTime = true;
            mNbSamplesForTSComputationOutput = 0;
        }
        bPropagateTS = true;
    }
}

/*********************************************************/
/* Function name : stereo2mono_16                        */
/*    *C = (short) (((long)Left +  (long)Right) >> 1)    */
/*********************************************************/
void hst_bindings_pcmadapter::stereo2mono_16(OMX_BUFFERHEADERTYPE *buf)
{
    ARMNMF_DBC_ASSERT((buf->nFilledLen % 2) == 0);
    unsigned int size = (unsigned int) (buf->nFilledLen / 2);

    stereo2mono_16_optim((void *) buf->pBuffer, size);

    buf->nFilledLen /= 2;
}


/******************************************************************/
/* Function name : stereo2mono_32                                 */
/*    *C = (long) (((long long)Left +  (long long)Right) >> 1)    */
/******************************************************************/
void hst_bindings_pcmadapter::stereo2mono_32(OMX_BUFFERHEADERTYPE *buf)
{
    ARMNMF_DBC_ASSERT((buf->nFilledLen % 4) == 0);
    unsigned int size = (unsigned int) (buf->nFilledLen / 4);

    stereo2mono_32_optim((void *) buf->pBuffer, size);

    buf->nFilledLen /= 2;
}


/**********************************************************/
/* Function name : mono2stereo_16                         */
/*    Left = Right = A                                    */
/**********************************************************/
void hst_bindings_pcmadapter::mono2stereo_16(OMX_BUFFERHEADERTYPE *buf)
{
    ARMNMF_DBC_ASSERT((buf->nFilledLen % 2) == 0);
    unsigned int size = (unsigned int) (buf->nFilledLen / 2);

    mono2stereo_16_optim((void *) buf->pBuffer, size);

    buf->nFilledLen *= 2;
    if((buf->pBuffer + buf->nFilledLen) > (mBuffer + mBufferSize))
    {
        ARMNMF_DBC_ASSERT(0);
    }
}

/*********************************************************/
/* Function name : mono2stereo_32                        */
/*    Left = Right = A                                   */
/*********************************************************/
void hst_bindings_pcmadapter::mono2stereo_32(OMX_BUFFERHEADERTYPE *buf)
{
    ARMNMF_DBC_ASSERT((buf->nFilledLen % 4) == 0);
    unsigned int size = (unsigned int) (buf->nFilledLen / 4);

    mono2stereo_32_optim((void *) buf->pBuffer, size);

    buf->nFilledLen *= 2;
    if((buf->pBuffer + buf->nFilledLen) > (mBuffer + mBufferSize))
    {
        ARMNMF_DBC_ASSERT(0);
    }
}


/*********************************************************/
/* Function name : c16to32                               */
/* long B = ((long) A) << 16                             */
/*********************************************************/
void hst_bindings_pcmadapter::c16to32(OMX_BUFFERHEADERTYPE *buf)
{
    ARMNMF_DBC_ASSERT((buf->nFilledLen % 2) == 0);
    unsigned int size = (unsigned int) (buf->nFilledLen / 2);

    c16to32_optim((void *) buf->pBuffer, size);

    buf->nFilledLen *= 2;
    if((buf->pBuffer + buf->nFilledLen) > (mBuffer + mBufferSize))
    {
        ARMNMF_DBC_ASSERT(0);
    }
}


/*********************************************************/
/* Function name : c32to16                              */
/*    long long C = (long long)A + 0x8000;              */
/*    if (C > 0x7FFFFFFF)                               */
/*        C = 0x7FFFFFFF;                               */
/*    B = (short) (C >> 16);                            */
/*********************************************************/
void hst_bindings_pcmadapter::c32to16(OMX_BUFFERHEADERTYPE *buf)
{
    //ARMNMF_DBC_ASSERT((buf->nFilledLen % 4) == 0);
    int size = (int) (buf->nFilledLen / 4);

    c32to16_optim((void *) buf->pBuffer, size);
    buf->nFilledLen /= 2;
}


void hst_bindings_pcmadapter::move_chunk()
{
    int size = (int) (mWritePtr - mReadPtr);

    move_chunk_optim((void *) mReadPtr, (void *) mBuffer, size);

    mReadPtr  = mBuffer;
    mWritePtr = mBuffer + size;
    OstTraceFiltInst2(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter: move_chunk : mReadPtr = 0x%x,mWritePtr = 0x%x",(unsigned int)mReadPtr,(unsigned int)mWritePtr);
}

void hst_bindings_pcmadapter::handleNewInputBuffer() {
    // check that we got the PCM format before getting some buffers
    ARMNMF_DBC_PRECONDITION(mChannelsIn != 0);

    mInputBuf = mPorts[INPUT_PORT].getBuffer(0);
    OstTraceFiltInst2(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : handleNewInputBuffer : mInputBuf->pBuffer = 0x%x | mInputBuf->nFilledLen = %d",(unsigned int)mInputBuf->pBuffer,mInputBuf->nFilledLen);

    if (mInputBuf->nFlags & OMX_BUFFERFLAG_EOS) {
        OstTraceFiltInst0(TRACE_ALWAYS, "AFM_HOST: hst_bindings_pcmadapter::handleNewInputBuffer EOS received !");
    }

    ARMNMF_DBC_ASSERT(mInputBuf->nFilledLen <= mInputSize);

    //Change the nb of bit per sample
    if (mBitPerSampleIn == 16 && mBitPerSampleOut == 32) {
        c16to32(mInputBuf);
    }
    else if (mBitPerSampleIn == 32 && mBitPerSampleOut == 16) {
        c32to16(mInputBuf);
    }


    // Change the nb of channel
    if (mChannelsIn == 1 && mChannelsOut == 2) {
        if (mBitPerSampleOut == 16)
            mono2stereo_16(mInputBuf);
        else
            mono2stereo_32(mInputBuf);
    }
    else if (mChannelsIn == 2 && mChannelsOut == 1) {
        if (mBitPerSampleOut == 16)
            stereo2mono_16(mInputBuf);
        else
            stereo2mono_32(mInputBuf);
    }

    mWritePtr = mInputBuf->pBuffer + mInputBuf->nFilledLen;
    OstTraceFiltInst1(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : handleNewInputBuffer : mWritePtr = 0x%x\n",(unsigned int)mWritePtr);

    initTimeStampComputation();

    if (bPropagateTS)
    {
        OMX_S64 expectedTimeStamp = computeTimeStamp(mChannelsOut, &mNbSamplesForTSComputationInput, (mInputBuf->nFilledLen*8/mBitPerSampleOut)); // use mChannels/mBitPerSampleOut and not In since nFilledLen has been updated during conversion
        if (mInputBuf->nTimeStamp != expectedTimeStamp)
        {
            mInputBuf->nFlags |= OMX_BUFFERFLAG_STARTTIME;
            initTimeStampComputation();
			mNbSamplesForTSComputationInput = (mInputBuf->nFilledLen*8/mBitPerSampleOut);
            OstTraceFiltInst4(TRACE_WARNING,"Input Timestamp mismatch !! (received 0x%x 0x%x) (expected 0x%x 0x%x)", (int)(mInputBuf->nTimeStamp >> 32), (unsigned int)(mInputBuf->nTimeStamp & 0xffffffffu), (int)(expectedTimeStamp >> 32), (unsigned int)(expectedTimeStamp & 0xffffffffu));
        }
    }


}

void hst_bindings_pcmadapter::handleNewOutputBuffer() {

    mOutputBuf = (OMX_BUFFERHEADERTYPE *)mPorts[OUTPUT_PORT].getBuffer(0);
    // We must increment the read ptr by the size that has been sent to the output.
    // mSizeSentToOutput is equal to the output buffer's filled length (set before returning the output buffer)
    mReadPtr = mOutputBuf->pBuffer + mSizeSentToOutput;
    OstTraceFiltInst3(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : handleNewOutputBuffer : mOutputBuf->pBuffer = 0x%x | mOutputBuf->nAllocLen = %d mReadPtr = 0x%x",(unsigned int)mOutputBuf->pBuffer,mOutputBuf->nAllocLen,(unsigned int)mReadPtr);

    if (mSetStartTime)
    {
        mSetStartTime = false;
        mOutputBuf->nFlags |= OMX_BUFFERFLAG_STARTTIME;
    }
}

void hst_bindings_pcmadapter::returnOutputBuffer() {

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: hst_bindings_pcmadapter::emptyThisBuffer receive input buffer (nFilledLen=%d)",mOutputBuf->nFilledLen);

    if (bPropagateTS)
    {
        mOutputBuf->nTimeStamp= computeTimeStamp(mChannelsOut, &mNbSamplesForTSComputationOutput, (mOutputBuf->nFilledLen*8)/mBitPerSampleOut);
        OstTraceFiltInst3(TRACE_DEBUG, "AFM_HOST: hst_bindings_pcmadapter::process latency control (host_pcmadapter, output) (timestamp = 0x%x 0x%x us) flags = 0x%x", (int)(mOutputBuf->nTimeStamp >> 32), (unsigned int)(mOutputBuf->nTimeStamp & 0xffffffffu), (int)(mOutputBuf->nFlags));
    }

    if (mOutputBuf->nFlags & OMX_BUFFERFLAG_EOS) {
        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: hst_bindings_pcmadapter::returnOutputBuffer EOS set in output buffer");
    }

    // pcm dump

    t_sint16 id = getId1(); // to get back port id associated to this pcmadapter
    if (id <= MAX_COMMON_TRACE) {
        if (id > HALF_MAX_COMMON_TRACE) {
            id -= HALF_MAX_COMMON_TRACE;
        }
        // we are able to trace port 0, port 1/5, port 2/6, port 3/7 and port 4/8
        if ((t_uint16)((1 << id) << MAX_COMMON_TRACE) & getTraceEnable()){
            OstTraceFiltInst1(TRACE_OMX_BUFFER, "AFM_HOST: hst_bindings_pcmadapter:: ReturnOutputBuffer nFilledLen %d", (unsigned int)(mOutputBuf->nFilledLen));
            // size is limited to MAX_OST_SIZE bytes
            OMX_U32 size = mOutputBuf->nFilledLen;
            for (OMX_U32 i = 0; i < size; i+=MAX_OST_SIZE)
            {
                OstTraceFiltInstData(TRACE_OMX_BUFFER, "AFM_HOST: hst_bindings_pcmadapter::ReturnOutputBuffer data = %{int8[]}", (OMX_U8 *)((OMX_U8 *)(mOutputBuf->pBuffer) + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
            }
        }
    }

    // end pcm dump
    mSizeSentToOutput = mOutputBuf->nFilledLen;
    mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    mOutputBuf = 0;
}


void hst_bindings_pcmadapter::process() {

    OstTraceFiltInst0(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : process");
    if (mInputBuf == 0
            && mPorts[INPUT_PORT].queuedBufferCount()) {
        OstTraceFiltInst0(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : process :call to handleNewInputBuffer");
        handleNewInputBuffer();
    }
    if (mOutputBuf == 0
            &&  mPorts[OUTPUT_PORT].queuedBufferCount()){
        OstTraceFiltInst0(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : process :call to handleNewOutputBuffer");
        handleNewOutputBuffer();
    }

    if (!mOutputBuf) return;

    if (mWritePtr - mReadPtr >= (int)mBufOut.nAllocLen) {
        OstTraceFiltInst0(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : process : output buffer and enough data to fill it: send it downstream");
        // we have an output buffer and enough data to fill it
        // so just send it downstream

        mOutputBuf->pBuffer            = mReadPtr;
        mOutputBuf->nFilledLen       = mOutputBuf->nAllocLen;
        //mOutputBuf->byteInLastWord  = 3;

        returnOutputBuffer();
    }
    else if (mInputBuf && mInputBuf->nFlags & OMX_BUFFERFLAG_EOS) {
        OstTraceFiltInst0(TRACE_ALWAYS,"AFM_HOST: hst_bindings_pcmadapter : process : EOS in input");
        // we have an output buf and not enough data to fill it
        // but input buffer has EOS so we will not get more data
        // so just send the data we have downstream with EOS flag

        // we must ensure that the output buffer has the right allocLen
        // the easy way is to call move_chunk
        move_chunk();

        mOutputBuf->pBuffer         = mReadPtr;
        mOutputBuf->nFilledLen      = mWritePtr - mReadPtr;
        mOutputBuf->nFlags          |= OMX_BUFFERFLAG_EOS;
        //mOutputBuf->byteInLastWord  = 3;


        proxy.eventHandler(OMX_EventBufferFlag, 1, mOutputBuf->nFlags);

        returnOutputBuffer();
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        mInputBuf = 0;

    }
    else if (mInputBuf) {
        // we have an output buffer but not enough data to fill
        // it so we need to get some more from upstream

        OstTraceFiltInst0(TRACE_DEBUG,"AFM_HOST: hst_bindings_pcmadapter : process : output buf but not enough data to fill it : get more from upstream");
        if (mBuffer + mBufferSize - mWritePtr < mInputSize) {
            // not enough room to append an input buffer
            move_chunk();
        }

        mInputBuf->pBuffer         = mWritePtr;
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        mInputBuf= 0;
    }
}

int hst_bindings_pcmadapter::gcd(int a, int b) {
    int tmp;

    while (b != 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;
}


t_uint16 hst_bindings_pcmadapter::computeBufferSize(HostPcmAdapterParam_t params) {
    t_sint16 buffer_size;

    int producedSize = MAX(params.nInputBlockSize * params.nChansIn * (params.nBitPerSampleIn/8),
                            params.nInputBlockSize * params.nChansOut * (params.nBitPerSampleOut/8));

    // We first do the conversion for updating the number of bits per sample => Multiply by 2 in order to have enough
    // room to make the conversion from 16 to 32 bits per sample
    if ((params.nBitPerSampleOut > params.nBitPerSampleIn) && (params.nChansOut < params.nChansIn)) {
        producedSize = producedSize*2;
    }

    int consumedSize = params.nOutputBlockSize * params.nBitPerSampleOut/8 * params.nChansOut;

    if ((params.nOutputBlockSize % params.nInputBlockSize) == 0) {
             buffer_size = producedSize + consumedSize - (params.nInputBlockSize * params.nChansOut * (params.nBitPerSampleOut/8));
    } else {
        buffer_size = producedSize + consumedSize -gcd(producedSize,consumedSize);

        if (params.nChansIn > params.nChansOut) {
            // Here we take the maximum number of bytes per sample as we do the conversion of byte per sample first
            buffer_size +=
                params.nInputBlockSize * MAX(params.nBitPerSampleIn/8,params.nBitPerSampleOut/8) * (params.nChansIn - params.nChansOut);
        } else if (params.nBitPerSampleIn > params.nBitPerSampleOut) {
            buffer_size +=
                params.nInputBlockSize * params.nChansIn * 2;
        }
    }


    return buffer_size;
}


void hst_bindings_pcmadapter::reset() {
    mBufIn.pBuffer     = mBuffer;
    mBufOut.pBuffer    = mBuffer;

    mReadPtr        = mBuffer;
    mWritePtr       = mBuffer;

    mInputBuf       = 0;
    mOutputBuf      = &mBufOut;

    mSizeSentToOutput = 0;

    mSetStartTime = false;
    mInitialTS = 0;
    mNbSamplesForTSComputationInput = 0;
    mNbSamplesForTSComputationOutput = 0;
    bPropagateTS = false;

}

void hst_bindings_pcmadapter::disablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_pcmadapter::disablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}

void hst_bindings_pcmadapter::enablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_pcmadapter::enablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}

void hst_bindings_pcmadapter::flushPortIndication(t_uint32 portIdx) {
    reset();
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setParameter)(HostPcmAdapterParam_t params) {

    ARMNMF_DBC_ASSERT(params.nBufferSize == computeBufferSize(params));

    // Reset mBufIn and mBufOut buffer headers
    memset(&mBufIn, 0, sizeof(OMX_BUFFERHEADERTYPE));
    memset(&mBufOut, 0, sizeof(OMX_BUFFERHEADERTYPE));

    mBuffer             = (OMX_U8 *)params.pBuffer;
    mBufferSize         = params.nBufferSize;

    mBufIn.nAllocLen    = params.nInputBlockSize * params.nChansIn * (params.nBitPerSampleIn/8);

    // for input buffers, we need enough room to perform mono 2 stereo conversion + conversion 16 to 32 bits per sample
    mInputSize          = MAX(mBufIn.nAllocLen, params.nInputBlockSize * params.nChansOut * (params.nBitPerSampleOut/8));
    // We first do the conversion for updating the number of bits per sample => Multiply by 2 in order to have enough
    // room to make the conversion from 16 to 32 bits per sample
    if ((params.nBitPerSampleOut > params.nBitPerSampleIn) && (params.nChansOut < params.nChansIn)) {
        mInputSize = mInputSize*2;
    }

    mBufOut.nAllocLen   = params.nOutputBlockSize * params.nChansOut * (params.nBitPerSampleOut/8);

    mChannelsIn         = params.nChansIn;
    mChannelsOut        = params.nChansOut;

    mBitPerSampleIn     = params.nBitPerSampleIn;
    mBitPerSampleOut    = params.nBitPerSampleOut;

    mSampleFreq         = params.nSampleFreq;
}

void METH(fsmInit) (fsmInit_t initFsm) {
    OMX_BUFFERHEADERTYPE     *fifoIn, *fifoOut;

    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    fifoIn = &mBufIn;
    fifoOut = &mBufOut;

    mPorts[INPUT_PORT].init(InputPort, true, false, 0, &fifoIn, 1, &inputport, INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0,this);
    mPorts[OUTPUT_PORT].init(OutputPort, true, false, 0, &fifoOut, 1, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0,this);

    init(2, mPorts, &proxy, &me, false);

    OstTraceFiltInst2(TRACE_ALWAYS,"AFM_HOST: hst_bindings_pcmadapter::fsminit: mBufIn.nAllocLen = %d | mBufOut.nAllocLen = %d",(int) mBufIn.nAllocLen,(int) mBufOut.nAllocLen);
    OstTraceFiltInst3(TRACE_ALWAYS,"AFM_HOST: hst_bindings_pcmadapter::fsminit: setParameter : mBuffer = 0x%x | mBufferSize = %d | mInputSize = %d",(unsigned int)mBuffer,mBufferSize,mInputSize);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    OstTraceFiltInst0(TRACE_ERROR, "AFM_HOST: hst_bindings_pcmadapter::setTunnelStatus Error should not be called");
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(newFormat)(
        t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
    ARMNMF_DBC_ASSERT(chans_nb == mChannelsIn);
    ARMNMF_DBC_ASSERT(sample_bitsize == mBitPerSampleIn);

    mChannelsIn   = chans_nb;

    outputsettings.newFormat(freq, mChannelsOut, mBitPerSampleOut);
}


void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer){

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: hst_bindings_pcmadapter::emptyThisBuffer receive input buffer (nFilledLen=%d)",buffer->nFilledLen);
    OstTraceFiltInst3(TRACE_DEBUG, "AFM_HOST: hst_bindings_pcmadapter::emptyThisBuffer latency control (host_pcmadapter, input) (timestamp = 0x%x 0x%x us), flag = 0x%x", (int)(buffer->nTimeStamp >> 32), (unsigned int)(buffer->nTimeStamp & 0xffffffffu), buffer->nFlags);

    // check that we do not have any condition that can lead to memory corruption...
    if ((buffer->nFilledLen != 0) && (buffer->nFilledLen != buffer->nAllocLen)) {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_bindings_pcmadapter::emptyThisBuffer Error potential memory corruption (nFilledLen=%d) (nAllocLen=%d)",buffer->nFilledLen,buffer->nAllocLen);
        ARMNMF_DBC_ASSERT(0);
    }

    Component::deliverBuffer(INPUT_PORT, buffer);
}

