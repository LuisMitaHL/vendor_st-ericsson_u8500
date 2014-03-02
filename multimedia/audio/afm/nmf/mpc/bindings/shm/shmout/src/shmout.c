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
#include <bindings/shm/shmout.nmf>
#include <archi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/component/include/Component.inl"


#define IN  0
#define OUT 1

#define MAX_NB_BUFFERS 4

////////////////////////////////////////////////////////////////////////
//					Global Variables
////////////////////////////////////////////////////////////////////////
static ShmConfig_t  mShmConfig;
static bool         mBufferSent;


static void *       mFifoIn[1];
static void *       mFifoOut[MAX_NB_BUFFERS];
static Port         mPorts[2];
static Component    mShmOut;

#ifdef _DEBUG_VARIABLE_
static t_sample_freq m_sample_freq;
static t_uint16 m_chans_nb;
static t_uint16 m_sample_size;
#endif

#define MAXITERCOUNT 512    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

////////////////////////////////////////////////////////////////////////

static void inline swap_bytes(int *buffer, int size) 
{
    if(size > 0)
    {
        int i, n;
        int tmp;
        
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp       = *buffer;                                            // 0x??BBAA
                tmp       = winsertu(tmp, 0x0810, tmp);                         // 0xAABBAA
                tmp       = wextractu(tmp, 0x1008);                             // 0x00AABB
                *buffer++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp       = *buffer;                                                // 0x??BBAA
            tmp       = winsertu(tmp, 0x0810, tmp);                             // 0xAABBAA
            tmp       = wextractu(tmp, 0x1008);                                 // 0x00AABB
            *buffer++ = tmp;
        }
    }
}

void ShmOut_Process(Component *this)
{
    if (Port_queuedBufferCount(&this->ports[IN]))
    {
        // Process Input buffer
        Buffer_p bufIn =  Port_dequeueBuffer(&this->ports[IN]);
        mBufferSent = false;

        if (mShmConfig.swap_bytes != NO_SWAP) {
            swap_bytes(bufIn->data, (int) bufIn->filledLen);
        }

        if(bufIn->flags & BUFFERFLAG_EOS){
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
        }
        Port_returnBuffer(&this->ports[OUT], bufIn);
    }


    if (Port_queuedBufferCount(&this->ports[OUT]) && mBufferSent == false)
    {
        // Process Output buffer
        Buffer_p bufOut =  Port_dequeueBuffer(&this->ports[OUT]);

        mBufferSent = true;
        Port_returnBuffer(&this->ports[IN], bufOut);
    }
}


void ShmOut_Reset(Component *this)
{
    mBufferSent = false; 
}

static void
ShmOut_disablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmOut_enablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmOut_flushPortIndication(t_uint32 portIdx) {
    (*mShmOut.reset)(&mShmOut);
}

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////


void METH(setParameter)(ShmConfig_t config ) {
    PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);

    mShmConfig = config;
}

void METH(fsmInit) (fsmInit_t initFsm) {
   	// trace init (mandatory before port init)
    FSM_traceInit(&mShmOut, initFsm.traceInfoAddr, initFsm.id1);

	Port_init(&mPorts[IN],  InputPort, true  , false, &mPorts[OUT], mFifoIn,  1, 
            &inputport, IN, (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mShmOut);

    Port_init(&mPorts[OUT], OutputPort,false , false, &mPorts[IN] , mFifoOut, mShmConfig.nb_buffer, 
            &outputport, OUT, (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mShmOut);

    mShmOut.process                 = ShmOut_Process;
    mShmOut.reset                   = ShmOut_Reset;
    mShmOut.disablePortIndication   = ShmOut_disablePortIndication;
    mShmOut.enablePortIndication    = ShmOut_enablePortIndication;
    mShmOut.flushPortIndication     = ShmOut_flushPortIndication;
    
    Component_init(&mShmOut,2, &mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mShmOut);
}

// From ARM port
void METH(fillThisBuffer)(Buffer_p buffer) {
    Component_deliverBuffer(&mShmOut, OUT, buffer);    
}

// From Coder/decoder
void METH(emptyThisBuffer)(Buffer_p buffer) {
    Component_deliverBuffer(&mShmOut, IN, buffer);
}


void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mShmOut, cmd, param);
}

void METH(newFormat)(
	t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {

#ifdef _DEBUG_VARIABLE_
    m_sample_freq = sample_freq;
    m_chans_nb    = chans_nb;
    m_sample_size = sample_size;
#endif
    if(outputsettings.newFormat != 0x0){
        outputsettings.newFormat(sample_freq, chans_nb, sample_size);
    }
}
