/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmin.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <bindings/shm/shmin.nmf>
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
#include "afm_nmf_mpc_bindings_shm_shmin_src_shminTraces.h"
#endif


#define IN  0
#define OUT 1

#define MAX_NB_BUFFERS 4

////////////////////////////////////////////////////////////////////////
//					Global Variables
////////////////////////////////////////////////////////////////////////
static Component    mShmIn;
static ShmConfig_t  mShmConfig;
static bool         mBufferSent;


static void *       mFifoIn[MAX_NB_BUFFERS];
static void *       mFifoOut[1];
static Port         mPorts[2];
static int          mSleepCnt;
static bool         mInputBufferAlreadyReturned; 

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

void ShmIn_Process(Component *this)
{
    if (Port_queuedBufferCount(&this->ports[OUT])) 
    {
        Buffer_p bufOut =  Port_dequeueBuffer(&this->ports[OUT]);
       
        mBufferSent = false;
		OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmin Process release input buffer");   
        preventSleep();
        mSleepCnt++;
        mInputBufferAlreadyReturned = true;
        Port_returnBuffer(&this->ports[IN], bufOut);
    }

    if (Port_queuedBufferCount(&this->ports[IN]) && mBufferSent == false)
    {
        Buffer_p bufIn =  Port_dequeueBuffer(&this->ports[IN]);

        if (mShmConfig.swap_bytes != NO_SWAP) {
            swap_bytes(bufIn->data, (int) bufIn->filledLen);
        }
        
        if (bufIn->flags & BUFFERFLAG_EOS){
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
        }

        mBufferSent = true;
		OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmin Process release output buffer");   
        Port_returnBuffer(&this->ports[OUT], bufIn);
    }
    else{
        if((Port_queuedBufferCount(&this->ports[IN]) == 0)  && mBufferSent == false){    
            // I'm starving please give me buffer faster !!
			OstTraceFiltInst0 (TRACE_WARNING, "AFM_MPC: shmin Process underflow !!");   
        }
    }
}

void ShmIn_Reset(Component *this)
{
    int i;
    if (mSleepCnt > 0) {
        for (i=0; i<mSleepCnt; i++){
            allowSleep();
        }
    } else {
        for (i=mSleepCnt; i<0; i++){
            preventSleep();
        }
    }
    mBufferSent = false; 
    mSleepCnt                = 0;
    mInputBufferAlreadyReturned = false;
}

static void
ShmIn_disablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmIn_enablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmIn_flushPortIndication(t_uint32 portIdx) {
	(*mShmIn.reset)(&mShmIn);
}

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////


void METH(setParameter)(ShmConfig_t config ) {
    int idx;
    
    PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);

    mShmConfig = config;
    mSleepCnt                = 0;
    mInputBufferAlreadyReturned = false;
}

void METH(fsmInit) (fsmInit_t initFsm) {
	// trace init (mandatory before port init)
    FSM_traceInit(&mShmIn, initFsm.traceInfoAddr, initFsm.id1);

    Port_init(&mPorts[IN], InputPort, false, false, &mPorts[OUT], mFifoIn, mShmConfig.nb_buffer, 
            &inputport, IN, (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mShmIn);

    Port_init(&mPorts[OUT], OutputPort, true , false,&mPorts[IN], mFifoOut, 1, 
            &outputport, OUT, (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mShmIn);

    mShmIn.process                  = ShmIn_Process;
    mShmIn.reset                    = ShmIn_Reset;
    mShmIn.disablePortIndication    = ShmIn_disablePortIndication;
    mShmIn.enablePortIndication     = ShmIn_enablePortIndication;
    mShmIn.flushPortIndication      = ShmIn_flushPortIndication;

    Component_init(&mShmIn,2, &mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mShmIn);
}

// From Coder/decoder
void METH(fillThisBuffer)(Buffer_p buffer) {
	TRACE_t * this = (TRACE_t *)&mShmIn;
	OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: shmin: fillThisBuffer 0x%x (nb queued buf %d)", (unsigned int)buffer, Port_queuedBufferCount(&mPorts[OUT]));	
    buffer->flags = 0; //Reset buffer flag (needed for EOS handing)
	Component_deliverBuffer(&mShmIn, OUT, buffer);    
}

// From ARM port
void METH(emptyThisBuffer)(Buffer_p buffer) {
    // I'm received a buffer from the host
	TRACE_t * this = (TRACE_t *)&mShmIn;
	OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: shmin: emptyThisBuffer 0x%x (nb queued buf %d)", (unsigned int)buffer, Port_queuedBufferCount(&mPorts[IN]));	
    // We start the allowSleep when a first returnInputBuffer has been done
    if (mInputBufferAlreadyReturned) {
        mSleepCnt--;
        allowSleep();
    }
    Component_deliverBuffer(&mShmIn, IN, buffer);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
   	Component_sendCommand(&mShmIn, cmd, param);
}

void METH(newFormat)(
	t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {

	outputsettings.newFormat(sample_freq, chans_nb, sample_size);
}
