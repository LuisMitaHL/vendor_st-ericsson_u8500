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
#include <encoders/streamed/wrapper.nmf>
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
static void *                       mFifoOut[1];
static void *                       mFifoIn[1];
static Port                         mPorts[2];
static Component                    mWrapper;
static Buffer_p                     mBufOut;

// output FIFO management
static BS_STRUCT_T                  mBSRead;
static unsigned int                 mOutputBufBits;
static int                          mAvailableBits;
static int                          mAvailableRoom;
static bool                         mEosReceived;

////////////////////////////////////////////////////////////
static void
framebuffer_reset(void) {

    bs_reset(&mInterfaceFE.stream_struct.bits_struct);
    bs_reset(&mBSRead);

    mAvailableRoom  = mBSRead.buf_size * 16;
    mAvailableBits  = 0;
    mEosReceived    = FALSE;
}


static void
count_frame(void) {
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
    mBufOut         = 0;
    mEosReceived    = false;

    framebuffer_reset();
    encoder.reset(&mInterfaceFE);
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
    if (portIdx == IN) {
        wrapper_reset();
    }
}

void
encode_frame(Component *this) {
    t_uint16    framesize_bits;
    BS_STRUCT_T saved_bs_struct;
    int         status;
    Buffer_p    bufIn;

    count_frame();

    bufIn = Port_dequeueBuffer(&this->ports[IN]);

    if (bufIn->filledLen == 0) {
        ASSERT(bufIn->flags & BUFFERFLAG_EOS);
        mEosReceived = true;
        Port_returnBuffer(&this->ports[IN], bufIn);
        return;
    }

    mInterfaceFE.sample_struct.buf_add = bufIn->data;
    saved_bs_struct = mInterfaceFE.stream_struct.bits_struct;

    if (bufIn->flags & BUFFERFLAG_EOS) {
        // zero fill input buf up to blocksize
        int i;
        for (i = bufIn->filledLen; i < bufIn->allocLen; i++) {
            bufIn->data[i] = 0;
        }
    }

    status = encoder.encodeFrame(&mInterfaceFE);

    if (status != RETURN_STATUS_OK) {
        proxy.eventHandler(OMX_EventError, OMX_DSP_ErrorStreamCorrupt, 0);
        return;
    }

    framesize_bits = mInterfaceFE.stream_struct.real_size_frame_in_bit;
    POSTCONDITION((framesize_bits % 8) == 0);

    bs_fast_move_bit(&saved_bs_struct, framesize_bits);
    mInterfaceFE.stream_struct.bits_struct = saved_bs_struct;

    mAvailableRoom -= framesize_bits;
    mAvailableBits += framesize_bits;

    if (bufIn->flags & BUFFERFLAG_EOS) {
        mEosReceived = true;
    }

    Port_returnBuffer(&this->ports[IN], bufIn);
}

static void
fillOutputBuffer(Component *this) {
    int     i, j, bitstowrite;

    bitstowrite = (mAvailableBits < mOutputBufBits) ? mAvailableBits : mOutputBufBits;

    if (!mEosReceived && bitstowrite % 16 != 0) {
        // remaining bits will be copied when next frame is available
        bitstowrite -= (bitstowrite % 16);
    }

    mAvailableBits -= bitstowrite;
    mAvailableRoom += bitstowrite;
    mOutputBufBits -= bitstowrite;

    for (i = mBufOut->filledLen, j = 0; bitstowrite != 0; i++, j++) {
        if (bitstowrite >= 16) {
            mBufOut->data[i] = bs_read_bit(&mBSRead, 16);
            bitstowrite -= 16;
        }
        else if (bitstowrite > 0) {
            ASSERT(mEosReceived);
            mBufOut->data[i] = bs_read_bit(&mBSRead, bitstowrite) << (16 - bitstowrite);
            if (bitstowrite < 8) {
                mBufOut->byteInLastWord = 1;
            }
            bitstowrite = 0;
        } 
    }

    mBufOut->filledLen += j;

    if (mAvailableBits == 0 && mEosReceived) {
        mBufOut->flags |= BUFFERFLAG_EOS;
        proxy.eventHandler(OMX_EventBufferFlag, OUT, mBufOut->flags);
        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        wrapper_reset();
    } 
    else if (mOutputBufBits == 0) {
        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        mBufOut = 0;
    } 

}

static void
process(Component *this) {

    if (mBufOut == 0 && Port_queuedBufferCount(&this->ports[OUT])) {
        mBufOut                     = Port_getBuffer(&this->ports[OUT], 0);
        mOutputBufBits              = mBufOut->allocLen * 16;
        mBufOut->filledLen          = 0;
        mBufOut->byteInLastWord     = 2;
    }
    
    // first try to copy from FrameBuffer to output buf
    // as this may give enough room to encode a new frame
    if (mBufOut) {
        fillOutputBuffer(this);
    }

    // encode a frame if possible
    if (Port_queuedBufferCount(&this->ports[IN])
            && mAvailableRoom >= encoder.getMaxFrameSize()
            && ! mEosReceived) {

        encode_frame(this);

        // new data in FB so try again to fill output buf
        if (mBufOut) {
            fillOutputBuffer(this);
        }
    }
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

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

    mWrapper.reset                  = wrapper_reset;
    mWrapper.process                = process;
    mWrapper.disablePortIndication  = disablePortIndication;
    mWrapper.enablePortIndication   = enablePortIndication;
    mWrapper.flushPortIndication    = flushPortIndication;

    Component_init(&mWrapper, 2, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
}

void METH(setParameter)(void *buffer_in, t_uint16 size_in, void *buffer_out, t_uint16 size_out) {
    t_uint16 framebuffer_size;
    t_uint16 framebuffer_size_bits;

    framebuffer_size_bits   = encoder.getMaxFrameSize();

    framebuffer_size = framebuffer_size_bits / 16
                       + (framebuffer_size_bits % 16 ? 1 : 0) + 1;

    if (size_out < framebuffer_size) {
        ASSERT(0);
    }

    bs_init(&mInterfaceFE.stream_struct.bits_struct, buffer_out, size_out, 16);
    bs_init(&mBSRead, buffer_out, size_out, 16);
}

void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb,
                     t_uint16 sample_bitsize)
{
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

