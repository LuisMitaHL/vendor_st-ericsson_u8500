/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   pcmadapter.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <bindings/pcmadapter.nmf>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/component/include/Component.inl"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_bindings_pcmadapter_src_pcmadapterTraces.h"
#endif


#define MAX(a, b)       ((a) > (b) ? (a) : (b)) 

#define OUT 1
#define IN  0

#define MAX_OST_SIZE 48
#define MAX_COMMON_TRACE 8
#define HALF_MAX_COMMON_TRACE (MAX_COMMON_TRACE >> 1)

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

static Buffer_t     mBufIn, mBufOut;
static void *       mFifoIn[1];
static void *       mFifoOut[1];
static Port         mPorts[2];
static Component    mPcmAdapter;

static int  *       mBuffer;
static t_uint16     mBufferSize;

static Buffer_p     mInputBuf, mOutputBuf;
static int          *mReadPtr, *mWritePtr;

static t_uint16     mInputSize;
static t_uint16     mChannelsIn;
static t_uint16     mChannelsOut;
static t_uint16     mBitPerSampleIn;
static t_uint16     mBitPerSampleOut;

// To handle TimeStamp
static bool mTimeStampDetected;
struct TimeStamp { 
    int           *mTimeStampPos;
    int			  mFlag;
    int           mTimeStampMsp;
    unsigned long mTimeStampLsp;
};
typedef struct TimeStamp TimeStamp_t;
#define MAX_TIMESTAMP 2
static TimeStamp_t   mTimeStampTab[MAX_TIMESTAMP];
static int mTimeStampWrIndex;
static int mTimeStampRdIndex;
static t_sample_freq mSampleRate;
static t_uint32 mSizeSentToOutput;

#define MAXITERCOUNT 512    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

////////////////////////////////////////////////////////////

static inline void stereo2mono(Buffer_p buf) {
    int i, n;
    t_sword *p1, *p2;

    ASSERT((buf->filledLen % 2) == 0);

    buf->filledLen /= 2;
    p1 = p2 = buf->data;
    if (mBitPerSampleOut == 16) {
#ifdef __flexcc2__
        #pragma loop maxitercount(MAXITERCOUNT)
#endif
        for(n = (int) buf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for (i = 0; i < MAXITERCOUNT; i++) {
                *p2++ = waddsat(wsgn16(*p1) >> 1, wsgn16(*(p1 + 1)) >> 1);
                p1 += 2;
            }
        }
#ifdef __flexcc2__
        #pragma loop maxitercount(MAXITERCOUNT)
#endif
        for (i = 0; i < n; i++) {
            *p2++ = waddsat(wsgn16(*p1) >> 1, wsgn16(*(p1 + 1)) >> 1);
            p1 += 2;
        }
    }
    else if (mBitPerSampleOut == 24) {
#ifdef __flexcc2__
        #pragma loop maxitercount(MAXITERCOUNT)
#endif
        for(n = (int) buf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for (i = 0; i < MAXITERCOUNT; i++) {
                *p2++ = waddsat(*p1 >> 1, *(p1 + 1) >> 1);
                p1 += 2;
            }
        }
#ifdef __flexcc2__
        #pragma loop maxitercount(MAXITERCOUNT)
#endif
        for (i = 0; i < n; i++) {
            *p2++ = waddsat(*p1 >> 1, *(p1 + 1) >> 1);
            p1 += 2;
        }
    }
    else { ASSERT(0); }
}

static inline void mono2stereo(Buffer_p buf) {
    int i, n;
    t_sword *p1, *p2, smpl;

    p1 = buf->data +     buf->filledLen - 1;
    p2 = buf->data + 2 * buf->filledLen - 1;
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) buf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            smpl  = *p1--;
            *p2-- = smpl;
            *p2-- = smpl;
        }
    }
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        smpl  = *p1--;
        *p2-- = smpl;
        *p2-- = smpl;
    }

    buf->filledLen *= 2;
}

static inline void c16to24(Buffer_p buf) {
    int i, n;
    t_sword *p;

    p = buf->data;
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) buf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            *p <<= 8;
            p++;
        }
    }
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        *p <<= 8;
        p++;
    }
}

static void inline c24to16(Buffer_p buf) {
    int i, n;
    t_sword *p;

    p = buf->data;
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) buf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            *p = wmsr(waddsat(*p, 0x80), 8);
            p++;
        }
    }
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        *p = wmsr(waddsat(*p, 0x80), 8);
        p++;
    }
}

static void move_chunk() {
    int i, n, offset;
    t_sword *p1, *p2;

    if (mTimeStampDetected)
    {
        offset = (mReadPtr - mBuffer);
        for (i = 0; i < MAX_TIMESTAMP; i++)
        {
            if (mTimeStampTab[i].mTimeStampPos)
            {
                mTimeStampTab[i].mTimeStampPos -= offset;
            }
        }
    }

    p1     = mReadPtr;
    p2     = mBuffer;
    offset = (mWritePtr - mReadPtr);
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = offset; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            *p2++ = *p1++;
        }
    }
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        *p2++ = *p1++;
    }
    mReadPtr  = mBuffer;
    mWritePtr = mBuffer + offset;
}

static void
propagate_timestamp(){
    // an output TimeStamp is computed for each output buffer 
    // formula is OutputTimeStamp = InputTimeStamp + ((ReadPtr - TimeStampPos)/(CnannelNB*SampleFreq))
    long Offset;
    TimeStamp_t * timestamp_p;

    timestamp_p = &mTimeStampTab[mTimeStampRdIndex];

    if (timestamp_p->mTimeStampPos)
    {
        Offset = mReadPtr - timestamp_p->mTimeStampPos;

        mOutputBuf->nTimeStampl = timestamp_p->mTimeStampLsp;
        mOutputBuf->nTimeStamph = timestamp_p->mTimeStampMsp;
        TSupdate(mOutputBuf, Offset, mSampleRate, mChannelsOut);
    }

    // propagate STARTTIME flag ?
    if (timestamp_p->mFlag & BUFFERFLAG_STARTTIME)
    {
        mOutputBuf->flags |= BUFFERFLAG_STARTTIME;
        timestamp_p->mFlag ^= BUFFERFLAG_STARTTIME;
    }
    // mOutputBuf->flags is reset by ENS otherwise it should be reset here
}

static void
handleNewInputBuffer(Component *this) {
    TimeStamp_t * timestamp_p = &mTimeStampTab[mTimeStampWrIndex];

    // check that we got the PCM format before getting some buffers
    PRECONDITION(mChannelsIn != 0);

    mInputBuf = Port_getBuffer(&this->ports[IN], 0);

    ASSERT(mInputBuf->filledLen <= mInputSize);

    // get new timestamp
    if (mInputBuf->flags & BUFFERFLAG_STARTTIME)
    {
        mTimeStampDetected = true;
        // toggle Write Index not to overwrite first TimeStamp
        mTimeStampWrIndex = 1 - mTimeStampWrIndex; 
    }
    if (mTimeStampDetected)
    {
        timestamp_p->mTimeStampPos = mWritePtr;
        timestamp_p->mFlag         = mInputBuf->flags;
        timestamp_p->mTimeStampMsp = mInputBuf->nTimeStamph;
        timestamp_p->mTimeStampLsp = mInputBuf->nTimeStampl;
    }

    if (mBitPerSampleIn == 16 && mBitPerSampleOut == 24) {
        c16to24(mInputBuf);
    }
    else if (mBitPerSampleIn == 24 && mBitPerSampleOut == 16) {
        c24to16(mInputBuf);
    }

    if (mChannelsOut == 2 && mChannelsIn == 1) {
        mono2stereo(mInputBuf);
    } 
    else if (mChannelsOut == 1 && mChannelsIn == 2) {
        stereo2mono(mInputBuf);
    }

    mWritePtr = mInputBuf->data + mInputBuf->filledLen;
}

static void
handleNewOutputBuffer(Component *this) {
    int NextReadIndex;

    mOutputBuf = Port_getBuffer(&this->ports[OUT], 0);
    // We must increment the read ptr by the size that has been sent to the output.
    // mSizeSentToOutput is equal to the output buffer's filled length (set before returning the output buffer)
    mReadPtr = mOutputBuf->data + mSizeSentToOutput;

    // TimeStamp has to be removed if Read pointer is over next TimeStamp position
    NextReadIndex = 1 - mTimeStampRdIndex;

    if ((mTimeStampTab[NextReadIndex].mTimeStampPos) &&
            (mReadPtr >= mTimeStampTab[NextReadIndex].mTimeStampPos))
    {
        mTimeStampTab[mTimeStampRdIndex].mTimeStampPos = 0;						
        mTimeStampRdIndex = 1 - mTimeStampRdIndex;
        mTimeStampWrIndex = 1 - mTimeStampWrIndex;  // toggle also WrIndex no to overwrite next TimeStamp to read
    }
}

static void sendOutputBuffer(Component *this, Buffer_p bufOut)
{
    unsigned short id;
    unsigned int i;
    unsigned int size = bufOut->filledLen;

    if (mTimeStampDetected){
        propagate_timestamp();
    }

    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: pcmadapter: sendOutputBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)bufOut, (unsigned int)bufOut->filledLen, Port_queuedBufferCount(&mPorts[OUT]));   
    
    // telephony latency control
    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: pcmadapter: latency control (mpc_pcmadapter, output) (timestamp = 0x%x 0x%x 0x%x us)", bufOut->nTimeStamph, (unsigned int)(bufOut->nTimeStampl >> 24), (unsigned int)(bufOut->nTimeStampl & 0xffffffu));	

    // pcm dump depending on port activation via component TRACE_GROUP
    id = ((TRACE_t *)this)->mId1; // to get back port id associated to this pcmadapter
    if (id <= MAX_COMMON_TRACE) {
        if (id > HALF_MAX_COMMON_TRACE) {
            id -= HALF_MAX_COMMON_TRACE;
        }
        // we are able to trace port 0, port 1/5, port 2/6, port 3/7 and port 4/8
        if ((unsigned int)((1 << id) << MAX_COMMON_TRACE) & (((TRACE_t *)this)->mTraceInfoPtr->traceEnable)){
            OstTraceFiltInst1(TRACE_OMX_BUFFER, "AFM_MPC: pcmadapter:: ReturnOutputBuffer nFilledLen %d", (unsigned int)(bufOut->filledLen*4));
            // size is limited to MAX_OST_SIZE bytes
            for (i = 0; i < size; i+=MAX_OST_SIZE)
            {
                OstTraceFiltInstData(TRACE_OMX_BUFFER, "AFM_MPC: pcmadapter::ReturnOutputBuffer data = %{int8[]}", (unsigned char *)&bufOut->data[i], (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
            }
        }
    }    
    // end pcm dump

    mSizeSentToOutput = bufOut->filledLen;
    Port_dequeueAndReturnBuffer(&this->ports[OUT]);
    mOutputBuf = 0;
}

void
PcmAdapter_process(Component *this) {

    if (mInputBuf == 0 && Port_queuedBufferCount(&this->ports[IN])) {
        handleNewInputBuffer(this);
    }
    if (mOutputBuf == 0 &&  Port_queuedBufferCount(&this->ports[OUT])){
        handleNewOutputBuffer(this);
    }

    if (!mOutputBuf) return;

    if (mWritePtr - mReadPtr >= (signed)mBufOut.allocLen) {
        // we have an output buffer and enough data to fill it
        // so just send it downstream

        mOutputBuf->data            = mReadPtr;
        mOutputBuf->filledLen       = mOutputBuf->allocLen;
        mOutputBuf->byteInLastWord  = 3;

        // propagate Drain ID
        if (mWritePtr - mReadPtr == (signed)mBufOut.allocLen) {
            mOutputBuf->flags      |= (mInputBuf->flags & BUFFERFLAG_MASK_FOR_DRAIN);
            mInputBuf->flags       &= ~BUFFERFLAG_MASK_FOR_DRAIN;
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: pcmadapter:process propagate DRAIN ID %x (send output)", mOutputBuf->flags);
        }

        sendOutputBuffer(this, mOutputBuf);
    }
    else if (mInputBuf && mInputBuf->flags & BUFFERFLAG_EOS) {
        // we have an output buf and not enough data to fill it
        // but input buffer has EOS so we will not get more data
        // so just send the data we have downstream with EOS flag

        // we must ensure that the output buffer has the right allocLen
        // the easy way is to call move_chunk
        move_chunk();

        mOutputBuf->data            = mReadPtr;
        mOutputBuf->filledLen       = mWritePtr - mReadPtr;
        mOutputBuf->flags          |= BUFFERFLAG_EOS + (mInputBuf->flags & BUFFERFLAG_MASK_FOR_DRAIN);
        mOutputBuf->byteInLastWord  = 3;

        proxy.eventHandler(OMX_EventBufferFlag, 1, mOutputBuf->flags);
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_MPC: pcmadapter:process EventBufferFlag %x ", mOutputBuf->flags);   

        sendOutputBuffer(this, mOutputBuf);

        Port_dequeueAndReturnBuffer(&this->ports[IN]);
        mInputBuf = 0;
    }
    else if (mInputBuf) {
        // we have an output buffer but not enough data to fill 
        // it so we need to get some more from upstream

        if (mBuffer + mBufferSize - mWritePtr < mInputSize) {
            // not enough room to append an input buffer
            move_chunk();
        }

        mInputBuf->data         = mWritePtr;
        // propagate DRAIN ID
        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: pcmadapter:process propagate DRAIN ID %x", mInputBuf->flags);   
        mOutputBuf->flags      |= (mInputBuf->flags & BUFFERFLAG_MASK_FOR_DRAIN);
        Port_dequeueAndReturnBuffer(&this->ports[IN]);
        mInputBuf= 0;
    }
}

static inline int gcd(int a, int b) {
    int tmp;

    while (b != 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;
}

int 
computeBufferSize(PcmAdapterParam_t params) {
    int buffer_size;

    buffer_size = (params.nInputBlockSize + params.nOutputBlockSize 
        - gcd(params.nInputBlockSize, params.nOutputBlockSize)) * params.nChansOut;

    if (params.nChansIn > params.nChansOut) {
        buffer_size += 
            params.nInputBlockSize * (params.nChansIn - params.nChansOut);
    }

    return buffer_size;
}

void
PcmAdapter_reset(Component *this) {
    int i;

    mBufIn.data     = mBuffer;
    mBufOut.data    = mBuffer;

    mReadPtr        = mBuffer;
    mWritePtr       = mBuffer;

    mInputBuf       = 0;
    mOutputBuf      = &mBufOut;

    mSizeSentToOutput = 0;

    mTimeStampWrIndex   = 0;
    mTimeStampRdIndex   = 0;
    mTimeStampDetected  = false;

    mTimeStampTab[0].mTimeStampPos = 0;
    mTimeStampTab[1].mTimeStampPos = 0;

}

void
PcmAdapter_disablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

void
PcmAdapter_enablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

void
PcmAdapter_flushPortIndication(t_uint32 portIdx) {
    PcmAdapter_reset(&mPcmAdapter);
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setParameter)(PcmAdapterParam_t params) {

    ASSERT(params.nBufferSize == computeBufferSize(params));

    mBuffer             = params.pBuffer;
    mBufferSize         = params.nBufferSize;

    mBufIn.allocLen     = params.nInputBlockSize * params.nChansIn;
    // for input buffers, we need enough room to perform mono 2 stereo conversion
    mInputSize          = params.nInputBlockSize 
        * MAX(params.nChansIn, params.nChansOut);
    mBufOut.allocLen    = params.nOutputBlockSize * params.nChansOut;
    mChannelsIn         = params.nChansIn;
    mChannelsOut        = params.nChansOut;

    mBitPerSampleIn     = params.nBitPerSampleIn;
    mBitPerSampleOut    = params.nBitPerSampleOut;

    mSampleRate         = params.nSampleRate;
}

void METH(fsmInit) (fsmInit_t initFsm) {
    // trace init (mandatory before port init)
    FSM_traceInit(&mPcmAdapter, initFsm.traceInfoAddr, initFsm.id1);

    mFifoIn[0]  = &mBufIn;
    mFifoOut[0] = &mBufOut;

    Port_init(&mPorts[IN], InputPort, true, false, 0, &mFifoIn, 1, &inputport, IN, 
            (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mPcmAdapter);
    Port_init(&mPorts[OUT], OutputPort, true, false, 0, &mFifoOut, 1, &outputport, OUT, 
            (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mPcmAdapter);

    mPcmAdapter.reset                   = PcmAdapter_reset;
    mPcmAdapter.process                 = PcmAdapter_process;
    mPcmAdapter.disablePortIndication   = PcmAdapter_disablePortIndication;
    mPcmAdapter.enablePortIndication    = PcmAdapter_enablePortIndication;
    mPcmAdapter.flushPortIndication     = PcmAdapter_flushPortIndication;

    Component_init(&mPcmAdapter, 2, mPorts, &proxy);

}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(newFormat)(
        t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
    ASSERT(chans_nb == mChannelsIn);
    ASSERT(sample_bitsize == mBitPerSampleIn);
    ASSERT(freq == mSampleRate);
    outputsettings.newFormat(freq, mChannelsOut, mBitPerSampleOut);
}

void METH(processEvent)() {
    Component_processEvent(&mPcmAdapter);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    TRACE_t * this = (TRACE_t *)&mPcmAdapter;

    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: pcmadapter: emptyThisBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)buf, (unsigned int)buf->filledLen, Port_queuedBufferCount(&mPorts[IN]) );	

    // telephony latency control
    OstTraceFiltInst4(TRACE_DEBUG, "AFM_MPC: pcmadapter: latency control (mpc_pcmadapter, input) (timestamp = 0x%x 0x%x 0x%x us) (flags %d)", buf->nTimeStamph, (unsigned int)(buf->nTimeStampl >> 24), (unsigned int)(buf->nTimeStampl & 0xffffffu), (unsigned int)buf->flags);	

    // check that we do not have any condition that can lead to memory corruption...
    if ((buf->filledLen != 0) && (buf->filledLen != buf->allocLen)) {
      ASSERT(0);
    }

    Component_deliverBuffer(&mPcmAdapter, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    TRACE_t * this = (TRACE_t *)&mPcmAdapter;
    OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: pcmadapter: fillThisBuffer 0x%x (nb queued buf %d)", (unsigned int)buf, Port_queuedBufferCount(&mPorts[OUT]));	
    Component_deliverBuffer(&mPcmAdapter, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mPcmAdapter, cmd, param);
}

