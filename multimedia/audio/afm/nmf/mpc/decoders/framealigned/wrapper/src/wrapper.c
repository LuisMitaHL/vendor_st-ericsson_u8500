/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <decoders/framealigned/wrapper.nmf>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include <archi.h>
#include "fsm/component/include/Component.inl"

#include "common_interface.h"


#define SIZE_SYNC_BUFF  24

#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

BS_STRUCT_T                 mBSRead;

// interface with decoder algo
static CODEC_INTERFACE_T    mInterfaceFE;
static int                  mSampleFreq;
static int                  mNbChannel;
static int                  mSampleSize;
static int                  mFrameNb;


static void *               mFifoOut[1];
static void *               mFifoIn[1];
static Port                 mPorts[2];
static Component            mWrapper;

static int                  mTimeStamp_Read;

////////////////////////////////////////////////////////////

static void
check_synchro(Buffer_p bufIn)
{
    int     dummy = 0;
    bool    bfi;
    unsigned int syncbuff_h, syncbuff_l;
    BS_STRUCT_T mybs;
    AUDIO_MODE_T    algo = UNKNOWN;

    mybs = mBSRead;

    syncbuff_h  = bs_read_bit(&mybs, SIZE_SYNC_BUFF);
    syncbuff_l  = bs_read_bit(&mybs, SIZE_SYNC_BUFF);

    algo = decoder.checkSynchro(syncbuff_h, syncbuff_l, &dummy);

    if ( algo != UNKNOWN ) {
        mInterfaceFE.codec_state.mode = algo;
    }

    if (algo == UNKNOWN || bufIn->flags & BUFFERFLAG_DATACORRUPT) {
        mInterfaceFE.codec_state.bfi  = true;
    }

    mInterfaceFE.stream_struct.bits_struct  = mBSRead;
}

static void
count_frame(void)
{
    VERBOSE(("+ frame %d\r", ++mFrameNb));
}

static void
check_output_format(void)
{
    if (mInterfaceFE.sample_struct.sample_freq != mSampleFreq
            || mInterfaceFE.sample_struct.chans_nb != mNbChannel
            || decoder.getSampleBitSize() != mSampleSize) {

        mSampleFreq     = mInterfaceFE.sample_struct.sample_freq;
        mNbChannel      = mInterfaceFE.sample_struct.chans_nb;
        mSampleSize     = decoder.getSampleBitSize();

        outputsettings.newFormat(mSampleFreq, mNbChannel, mSampleSize);
        //afminform.newFormat(mSampleFreq, mNbChannel, mSampleSize);
    }
}

static void
wrapper_reset(void) {
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;

    mSampleFreq     = 0;
    mNbChannel      = 0;
    mSampleSize     = 0;
    mFrameNb        = 0;

    mTimeStamp_Read = 0;

    decoder.reset(&mInterfaceFE);
}

// Interface for fsm
static void
reset(Component *this) {
    wrapper_reset();
}

static void
disablePortIndication(t_uint32 portIdx) {
    if(portIdx == IN) {
        wrapper_reset();
    }
}

static void
enablePortIndication(t_uint32 portIdx) {
}

static void
flushPortIndication(t_uint32 portIdx) {
}

static void
decode_frame(Component *this, Buffer_p bufIn, Buffer_p bufOut)
{
    int status;

    count_frame();

    status = decoder.decodeFrame(&mInterfaceFE);

    if (status != RETURN_STATUS_OK) {
        proxy.eventHandler(OMX_EventError, OMX_DSP_ErrorStreamCorrupt, 0);
        return;
    }

    if (mInterfaceFE.codec_state.output_enable) {

        check_output_format();
        bufOut->filledLen       = mInterfaceFE.sample_struct.block_len
                                  * mInterfaceFE.sample_struct.chans_nb ;
        bufOut->data            = mInterfaceFE.sample_struct.buf_add;
        bufOut->byteInLastWord  = 3;

        bufOut->nTimeStamph     = bufIn->nTimeStamph;
        bufOut->nTimeStampl     = bufIn->nTimeStampl;
        if  (bufIn->flags & BUFFERFLAG_STARTTIME)
        {
            bufOut->flags |= BUFFERFLAG_STARTTIME;
        }

        if  (bufIn->flags & BUFFERFLAG_EOS
                && mInterfaceFE.codec_state.remaining_blocks == 0) {
            bufOut->flags |= BUFFERFLAG_EOS;
            proxy.eventHandler(OMX_EventBufferFlag, OUT, bufOut->flags);
            wrapper_reset();
        }

        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
    }

    if (mInterfaceFE.codec_state.bfi) {
        //TODO: What to do with this BFI ?
        mInterfaceFE.codec_state.bfi = 0;
    }

    if (mInterfaceFE.codec_state.remaining_blocks == 0)      {
        Port_dequeueAndReturnBuffer(&this->ports[IN]);
        mTimeStamp_Read = 0;
    }
}


static void
process(Component *this) {
    Buffer_p bufIn, bufOut;

    if (Port_queuedBufferCount(&this->ports[OUT]) == 0){
        return;
    }

    if (Port_queuedBufferCount(&this->ports[IN]) == 0) {
        return;
    }

    bufIn   = Port_getBuffer(&this->ports[IN], 0);
    bufOut  = Port_getBuffer(&this->ports[OUT], 0);

    bs_init(&mBSRead, bufIn->data, (unsigned int)bufIn->allocLen , 16);

    mInterfaceFE.sample_struct.buf_add = bufOut->data;
    decoder.setOutputBuf(&mInterfaceFE, bufOut->data);

    if (bufIn->filledLen == 0 && bufIn->flags & BUFFERFLAG_EOS) {

        bufOut->filledLen = 0;
        bufOut->flags |= BUFFERFLAG_EOS;

        proxy.eventHandler(OMX_EventBufferFlag, OUT, bufOut->flags);

        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        Port_dequeueAndReturnBuffer(&this->ports[IN]);

        wrapper_reset();
    }
    else {
        if (mInterfaceFE.codec_state.remaining_blocks == 0) {
            check_synchro(bufIn);
        }

        decode_frame(this, bufIn, bufOut);
    }
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(fsmInit) (fsmInit_t initFsm) {
	// trace init (mandatory before port init)
    FSM_traceInit(&mWrapper, initFsm.traceInfoAddr, initFsm.id1);

	decoder.open(&mInterfaceFE);

    POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
    POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);

    Port_init(&mPorts[IN], InputPort, false, false, 0, &mFifoIn, 1, &inputport, IN, 
            (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mWrapper);
    Port_init(&mPorts[OUT], OutputPort, false, false, 0, &mFifoOut, 1, &outputport, OUT, 
            (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mWrapper);

    mWrapper.reset                  = reset;
    mWrapper.process                = process;
    mWrapper.disablePortIndication  = disablePortIndication;
    mWrapper.enablePortIndication   = enablePortIndication;
    mWrapper.flushPortIndication    = flushPortIndication;

    Component_init(&mWrapper, 2, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mWrapper);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mWrapper, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mWrapper, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mWrapper, cmd, param);
}

