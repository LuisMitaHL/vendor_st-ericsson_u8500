/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmout.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <bindings/shmpcm/shmout.nmf>
#include <archi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/component/include/Component.inl"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_bindings_shmpcm_shmout_src_shmoutTraces.h"
#endif


#define IN  0
#define OUT 1

#define MAX_NB_BUFFERS 4

////////////////////////////////////////////////////////////////////////
//					Global Variables
////////////////////////////////////////////////////////////////////////
static ShmPcmConfig_t  mShmConfig;

static void (*Shmout_copy_routine) (int *in, int *out, int size);

static Buffer_t     mBufIn;

static void *       mFifoIn[1];
static void *       mFifoOut[MAX_NB_BUFFERS];
static Port         mPorts[2];
static Component    mShmout;

static t_uint16     mDataConsumed;
static int          mShiftSizeOut = 0;

// AV synchronisation
static bool         mTimeStampDetected;

// store any fillthisbuffer received before fsminit
// This can happen in some specific case
static bool             mFsmInitialized = false;
static Buffer_p         mPendingBuffer[MAX_NB_BUFFERS] = {0,0,0,0};

#define MAXITERCOUNT 512    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

////////////////////////////////////////////////////////////////////////


static void copy_in16b_out16b_swap(int *in, int *out, int size)
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0x??AABB
                tmp    = winsertu (tmp, 0x0810, tmp);                           // 0xBBAABB
                tmp    = wextractu(tmp, 0x1008);                                // 0x00BBAA
                *out++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0x??AABB
            tmp    = winsertu (tmp, 0x0810, tmp);                               // 0xBBAABB
            tmp    = wextractu(tmp, 0x1008);                                    // 0x00BBAA
            *out++ = tmp;
        }
    }
}

static void copy_in16b_out16b_noswap(int *in, int *out, int size)
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out++ = *in++;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out++ = *in++;
        }
    }
}


static void copy_in16b_out32b_swap(int *in, int *out, int size)
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0x??AABB
                tmp    = winsertu (tmp, 0x0810, tmp);                           // 0xBBAABB
                tmp    = wextractu(tmp, 0x1008);                                // 0x00BBAA
                *out++ = tmp;
                *out++ = 0;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0x??AABB
            tmp    = winsertu (tmp, 0x0810, tmp);                               // 0xBBAABB
            tmp    = wextractu(tmp, 0x1008);                                    // 0x00BBAA
            *out++ = tmp;
            *out++ = 0;
        }
    }
}


static void copy_in16b_out32b_noswap(int *in, int *out, int size)
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out++ = 0;
                *out++ = *in++;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out++ = 0;
            *out++ = *in++;
        }
    }
}


static void copy_in24b_out16b_swap(int *in, int *out, int size)
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = (waddsat(*in++, 0x80) >> 8);                           // 0xssAABB
                tmp    = winsertu (tmp, 0x0810, tmp);                           // 0xBBAABB
                tmp    = wextractu(tmp, 0x1008);                                // 0x00BBAA
                *out++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = (waddsat(*in++, 0x80) >> 8);                               // 0xssAABB
            tmp    = winsertu (tmp, 0x0810, tmp);                               // 0xBBAABB
            tmp    = wextractu(tmp, 0x1008);                                    // 0x00BBAA
            *out++ = tmp;
        }
    }
}


static void copy_in24b_out16b_noswap(int *in, int *out, int size)
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABBCC
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = (waddsat(*in++, 0x80) >> 8);
                *out++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = (waddsat(*in++, 0x80) >> 8);
            *out++ = tmp;
        }
    }
}


static void copy_in24b_out32b_swap(int *in, int *out, int size)
{
    int i, n;
    int tmp, tmp_h, tmp_l;
    
    if(size > 0)
    {
        // input sample = 0xAABBCC
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0xAABBCC
                tmp_h  = winsertu(wextractu(tmp, 0x0810), 0x0800, tmp);         // 0xAABBAA
                tmp_h  = wextractu(tmp_h, 0x1000);                              // 0x00BBAA
                tmp_l  = wextractu(tmp, 0x0800);                                // 0x0000CC
                *out++ = tmp_h;
                *out++ = tmp_l;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0xAABBCC
            tmp_h  = winsertu(wextractu(tmp, 0x0810), 0x0800, tmp);             // 0xAABBAA
            tmp_h  = wextractu(tmp_h, 0x1000);                                  // 0x00BBAA
            tmp_l  = wextractu(tmp, 0x0800);                                    // 0x0000CC
            *out++ = tmp_h;
            *out++ = tmp_l;
        }
    }
}


static void copy_in24b_out32b_noswap(int *in, int *out, int size)
{
    int i, n;
    int tmp, tmp_h, tmp_l;
    
    if(size > 0)
    {
        // input sample = 0xAABBCC
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0xAABBCC
                tmp_h  =  wextractu(tmp, 0x1008);                               // 0x00AABB
                tmp_l  = (wextractu(tmp, 0x0800) << 8);                         // 0x00CC00
                *out++ = tmp_l;
                *out++ = tmp_h;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0xAABBCC
            tmp_h  =  wextractu(tmp, 0x1008);                                   // 0x00AABB
            tmp_l  = (wextractu(tmp, 0x0800) << 8);                             // 0x00CC00
            *out++ = tmp_l;
            *out++ = tmp_h;
        }
    }
}


static void ReturnInputBuffer(Component *this)
{
    Port_dequeueAndReturnBuffer(&this->ports[IN]);
    mDataConsumed = 0;
}

static void ReturnOutputBuffer(Component *this, Buffer_p bufOut)
{
    if(bufOut->flags & BUFFERFLAG_EOS){
        proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut->flags);
    }

    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmout: ReturnOutputBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)bufOut, (unsigned int)bufOut->filledLen, Port_queuedBufferCount(&mPorts[OUT]));   
    // telephony latency control
    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmout: latency control (mpc_shmpcmout, output) (timestamp = 0x%x 0x%x 0x%x us)", bufOut->nTimeStamph, (unsigned int)(bufOut->nTimeStampl >> 24), (unsigned int)(bufOut->nTimeStampl & 0xffffffu));
    Port_dequeueAndReturnBuffer(&this->ports[OUT]);
}


void ShmPcmOut_Process(Component *this)
{
    Buffer_p    bufIn, bufOut;
    int         out_offset, available_out_size, remaining_in_size;

    OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmout Process");

    if(Port_queuedBufferCount(&this->ports[IN]) == 0 
            || Port_queuedBufferCount(&this->ports[OUT]) == 0) {
        if(Port_queuedBufferCount(&this->ports[OUT])){
            OstTraceFiltInst0 (TRACE_WARNING, "AFM_MPC: shmpcmout Process underflow !!");   
        }
        return;
    }

    bufIn  =  Port_getBuffer(&this->ports[IN] ,0);
    bufOut =  Port_getBuffer(&this->ports[OUT],0);

    out_offset         =   (int) bufOut->filledLen;
    available_out_size = (((int) bufOut->allocLen) - out_offset) >> mShiftSizeOut;
    remaining_in_size  = (((int) bufIn->filledLen) - mDataConsumed);

    if (bufIn->flags & BUFFERFLAG_STARTTIME)
    {
        mTimeStampDetected = true;
    }

    // propagate timestamp if needed
    if (mTimeStampDetected)
    {
        TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsIn);
    }


    if(remaining_in_size == available_out_size)
    {
        (*Shmout_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);

        bufOut->filledLen += remaining_in_size << mShiftSizeOut;
        bufOut->flags     |= bufIn->flags;

        ReturnInputBuffer(this);
        ReturnOutputBuffer(this, bufOut);
    }
    else if(remaining_in_size < available_out_size)
    {
        (*Shmout_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);

        bufOut->filledLen += remaining_in_size << mShiftSizeOut;
        bufOut->flags |= bufIn->flags;

        if(bufIn->flags & BUFFERFLAG_EOS){
            ReturnOutputBuffer(this, bufOut);
        }

        ReturnInputBuffer(this);
    }
    else if(remaining_in_size > available_out_size)
    {
        (*Shmout_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, available_out_size);

        mDataConsumed     += available_out_size;
        bufOut->filledLen += available_out_size << mShiftSizeOut;

        if(bufIn->flags & BUFFERFLAG_STARTTIME){
            bufOut->flags |=  BUFFERFLAG_STARTTIME;		
            bufIn->flags  ^=  BUFFERFLAG_STARTTIME;
        }

        ReturnOutputBuffer(this, bufOut);

        if(Port_queuedBufferCount(&this->ports[OUT]) != 0){
            ShmPcmOut_Process(this);
        }
    }
}


void ShmPcmOut_Reset(Component *this)
{
    mBufIn.filledLen        = 0;
    mBufIn.address          = NULL;
    mBufIn.byteInLastWord   = 0;
    mBufIn.flags            = 0;
    mDataConsumed           = 0;

    mTimeStampDetected      = false;
}

static void
ShmPcmOut_disablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmPcmOut_enablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmPcmOut_flushPortIndication(t_uint32 portIdx) {
    (*mShmout.reset)(&mShmout);
}

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////


void METH(setParameter)(
        ShmPcmConfig_t config ,
        void *buffer, 
        t_uint16 input_blocksize) 
{
    PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);
    PRECONDITION(config.channelsIn == config.channelsOut);

    mShmConfig = config;

    ASSERT((mShmConfig.bitsPerSampleIn  == 16) || (mShmConfig.bitsPerSampleIn  == 24));
    ASSERT((mShmConfig.bitsPerSampleOut == 16) || (mShmConfig.bitsPerSampleOut == 32));
    if(mShmConfig.bitsPerSampleIn == 16) 
    {
        if(mShmConfig.bitsPerSampleOut == 16) 
        {
            mShiftSizeOut = 0;
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = copy_in16b_out16b_swap;
            }
            else
            {
                Shmout_copy_routine = copy_in16b_out16b_noswap;
            }
        }
        else
        {
            mShiftSizeOut = 1;
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = copy_in16b_out32b_swap;
            }
            else
            {
                Shmout_copy_routine = copy_in16b_out32b_noswap;
            }
        }
    }
    else
    {
        if(mShmConfig.bitsPerSampleOut == 16) 
        {
            mShiftSizeOut = 0;
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = copy_in24b_out16b_swap;
            }
            else
            {
                Shmout_copy_routine = copy_in24b_out16b_noswap;
            }
        }
        else
        {
            mShiftSizeOut = 1;
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = copy_in24b_out32b_swap;
            }
            else
            {
                Shmout_copy_routine = copy_in24b_out32b_noswap;
            }
        }
    }

    mBufIn.data     = buffer;
    mBufIn.allocLen = input_blocksize;
}

void METH(fsmInit) (fsmInit_t initFsm) {
    int i;    
    // trace init (mandatory before port init)
    FSM_traceInit(&mShmout, initFsm.traceInfoAddr, initFsm.id1);

    mFifoIn[0] = &mBufIn;

    Port_init(&mPorts[IN], InputPort, true,  false, 0, mFifoIn,  1, 
            &inputport, IN, (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mShmout);
    Port_init(&mPorts[OUT],OutputPort,false ,false, 0, mFifoOut, mShmConfig.nb_buffer, 
            &outputport, OUT, (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mShmout);

    mShmout.process                 = ShmPcmOut_Process;
    mShmout.reset                   = ShmPcmOut_Reset;
    mShmout.disablePortIndication   = ShmPcmOut_disablePortIndication;
    mShmout.enablePortIndication    = ShmPcmOut_enablePortIndication;
    mShmout.flushPortIndication     = ShmPcmOut_flushPortIndication;

    Component_init(&mShmout,2, &mPorts, &proxy);

    mFsmInitialized = true;
    // check if buffers have been received before then handle them
    for(i=0;i<MAX_NB_BUFFERS;i++)
    {
      if(mPendingBuffer[i] != (Buffer_p)0)
      {
        Port_queueBuffer(&mPorts[OUT], mPendingBuffer[i]);
        mPendingBuffer[i] = (Buffer_p)0;
      }
    }
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mShmout);
}

// From ARM port
void METH(fillThisBuffer)(Buffer_p buffer) {
    TRACE_t * this = (TRACE_t *)&mShmout;
    OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: shmpcmout: fillThisBuffer 0x%x (nb queued buf %d)", (unsigned int)buffer, Port_queuedBufferCount(&mPorts[OUT]));	
    
    if(mFsmInitialized)
    {
      Component_deliverBuffer(&mShmout, OUT, buffer);
    }
    else
    {
      int i;
      for (i=0;i<MAX_NB_BUFFERS;i++)
      {
        if(mPendingBuffer[i] == (Buffer_p)0)
        {
          mPendingBuffer[i] = buffer;
          break;
        }
      }
    }
}

// From Coder/decoder
void METH(emptyThisBuffer)(Buffer_p buffer) {
    TRACE_t * this = (TRACE_t *)&mShmout;

    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmout: emptyThisBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)buffer,  (unsigned int)buffer->filledLen, Port_queuedBufferCount(&mPorts[IN]) );	

    // telephony latency control
    OstTraceFiltInst4(TRACE_DEBUG, "AFM_MPC: shmpcmout: latency control (mpc_shmpcmout, input) (timestamp = 0x%x 0x%x 0x%x us) (flags %d)", buffer->nTimeStamph, (unsigned int)(buffer->nTimeStampl >> 24), (unsigned int)(buffer->nTimeStampl & 0xffffffu), (unsigned int)buffer->flags);	

    Component_deliverBuffer(&mShmout, IN, buffer);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mShmout, cmd, param);
}

void METH(newFormat)(
        t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {
    PRECONDITION(mShmConfig.sampleFreq == sample_freq);
    PRECONDITION(mShmConfig.channelsIn == chans_nb);

    PRECONDITION(mShmConfig.bitsPerSampleIn == sample_size);
}
