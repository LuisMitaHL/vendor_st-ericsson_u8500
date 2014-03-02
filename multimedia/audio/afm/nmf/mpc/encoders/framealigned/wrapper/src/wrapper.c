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
#include <encoders/framealigned/wrapper.nmf>
#include <stdio.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include <archi.h>
#include "fsm/component/include/Component.inl"
#include "common_interface.h"


#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

// interface with encoder algo
static CODEC_INTERFACE_T            mInterfaceFE;
static int                          mFrameNb;
static int                          mSampleBufSize;
static void *                       mFifoOut[1];
static void *                       mFifoIn[1];
static Port                         mPorts[2];
static Component                    mWrapper;



////////////////////////////////////////////////////////////
static void
framebuffer_reset(void) {

    bs_reset(&mInterfaceFE.stream_struct.bits_struct);
}


static void
count_frame(void)
{
    VERBOSE(("+ frame %d\r", ++mFrameNb));
}

static void
wrapper_reset(void) {
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;

    mFrameNb        = 0;

    encoder.reset(&mInterfaceFE);
    mSampleBufSize = mInterfaceFE.sample_struct.block_len
                     * mInterfaceFE.sample_struct.chans_nb;
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
    if(portIdx == IN) {
        wrapper_reset();
    }
}

void
encode_frame(Component *this, Buffer_p bufIn, Buffer_p bufOut)
{
    t_uint16    framesize_bits;
    t_uint16    framesize_bytes;
    BS_STRUCT_T saved_bs_struct;
    int         status;

    count_frame();

    saved_bs_struct = mInterfaceFE.stream_struct.bits_struct;

    if ((bufIn->flags & BUFFERFLAG_EOS) && (bufIn->filledLen < mSampleBufSize))
    {
        int *p = bufIn->data + bufIn->filledLen;
        while (p < bufIn->data + mSampleBufSize)
        {
            *p++ = 0;
        }
    }

    status = encoder.encodeFrame(&mInterfaceFE);

    if (status != RETURN_STATUS_OK) {
        proxy.eventHandler(OMX_EventError, OMX_DSP_ErrorStreamCorrupt, 0);
        return;
    }


    framesize_bits = mInterfaceFE.stream_struct.real_size_frame_in_bit;
    POSTCONDITION((framesize_bits % 8) == 0);

    framesize_bytes = framesize_bits/8;
    bufOut->filledLen = framesize_bytes/2 + framesize_bytes%2;
    bufOut->byteInLastWord = (framesize_bytes %2 == 0) ? 2: 1;

    bs_fast_move_bit(&saved_bs_struct, framesize_bits);
    mInterfaceFE.stream_struct.bits_struct = saved_bs_struct;

    if  (bufIn->flags & BUFFERFLAG_EOS) {
        bufOut->flags |= BUFFERFLAG_EOS;
        proxy.eventHandler(OMX_EventBufferFlag, OUT, bufOut->flags);
        wrapper_reset();
    }

    Port_dequeueAndReturnBuffer(&this->ports[OUT]);

    Port_dequeueAndReturnBuffer(&this->ports[IN]);
}

static void
process(Component *this) {
    Buffer_p bufIn, bufOut;

    if (Port_queuedBufferCount(&this->ports[IN]) == 0
            || Port_queuedBufferCount(&this->ports[OUT]) == 0) {
        return;
    }

    bufIn   = Port_getBuffer(&this->ports[IN], 0);
    bufOut  = Port_getBuffer(&this->ports[OUT], 0);

    bs_init(&mInterfaceFE.stream_struct.bits_struct,
            bufOut->data, (unsigned int)bufOut->allocLen, 16);

    mInterfaceFE.sample_struct.buf_add = bufIn->data;
    if (bufIn->filledLen == 0 && bufIn->flags & BUFFERFLAG_EOS) {

        bufOut->filledLen = 0;
        bufOut->flags |= BUFFERFLAG_EOS;

        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        Port_dequeueAndReturnBuffer(&this->ports[IN]);

        proxy.eventHandler(OMX_EventBufferFlag, OUT, bufOut->flags);

        wrapper_reset();
    }
    else {
        encode_frame(this, bufIn, bufOut);
    }

}

void METH(fsmInit) (fsmInit_t initFsm) {
	// trace init (mandatory before port init)
    FSM_traceInit(&mWrapper, initFsm.traceInfoAddr, initFsm.id1);

    encoder.open(&mInterfaceFE);

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

void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb,
                     t_uint16 sample_bitsize)
{
    //PRECONDITION(sample_freq == mInterfaceFe.sample_struct.sample_freq);
    PRECONDITION(chans_nb == mInterfaceFE.sample_struct.chans_nb);
    PRECONDITION(sample_bitsize == encoder.getSampleBitSize());
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
