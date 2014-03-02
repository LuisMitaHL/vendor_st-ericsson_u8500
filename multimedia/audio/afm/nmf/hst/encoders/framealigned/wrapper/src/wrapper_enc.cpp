/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper_enc.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/encoders/framealigned/wrapper.nmf>
#include <armnmf_dbc.h>
#include <verbose.h>
#include <string.h>

// size of the bitstream word (configured through bs-init)
// TODO FIXME: Set to 8 bits when the libbitstream supports it!
#define BS_INIT_SIZE_IN_BITS 32
#define BS_WORD_SIZE_IN_BYTES (BS_INIT_SIZE_IN_BITS/8)

#define IN  0
#define OUT 1

hst_encoders_framealigned_wrapper::hst_encoders_framealigned_wrapper (void)
{
    memset(&mInterfaceFE, 0, sizeof(CODEC_INTERFACE_T));

    mCodecInitialized   = false;
    mByteBufSize        = 0;
    mFrameNb            = 0;
}

void METH(stop)(void) {
    // Close the encoder...
    encoder.close(&mInterfaceFE);
}

// Interface for fsm
void hst_encoders_framealigned_wrapper::reset()
{
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;

    mFrameNb = 0;

    if(!encoder.IsNullInterface())
    {
        encoder.reset(&mInterfaceFE);
        mByteBufSize = mInterfaceFE.sample_struct.block_len * mInterfaceFE.sample_struct.chans_nb * 2;
    }    
}

void METH(disablePortIndication)(t_uint32 portIdx) {
    if (portIdx == IN) {
        reset();
    }
}

void METH(enablePortIndication)(t_uint32 portIdx) {}

void METH(flushPortIndication)(t_uint32 portIdx) {
    if (portIdx == IN) {
        reset();
    }
}

void hst_encoders_framealigned_wrapper::encode_frame(OMX_BUFFERHEADERTYPE * bufIn, OMX_BUFFERHEADERTYPE * bufOut)
{
    t_uint16    framesize_bits;
    t_uint16    framesize_bytes;
   // BS_STRUCT_T saved_bs_struct;
    int         status;

    VERBOSE(("+ frame %d\r", ++mFrameNb));

   // saved_bs_struct = mInterfaceFE.stream_struct.bits_struct;

    if((bufIn->nFlags & OMX_BUFFERFLAG_EOS) && (bufIn->nFilledLen < (unsigned int)mByteBufSize))
    {
        OMX_U8 *p = bufIn->pBuffer + bufIn->nFilledLen;
        while (p < (bufIn->pBuffer + mByteBufSize))  
        { 
            *p++ = 0;
        }
        bufIn->nFilledLen = mByteBufSize;
    }  
    // Changes for handling single frame case with ENDOFFRAME Flag 
    /* else if(bufIn->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)
    {
        mInterfaceFE.sample_struct.block_len=bufIn->nFilledLen/(encoder.getSampleBitSize()>>3);
    }
    */
    status = encoder.encodeFrame(&mInterfaceFE);

    if (status != RETURN_STATUS_OK) {
        proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorStreamCorrupt, 0);
        return;
    }


    framesize_bits = mInterfaceFE.stream_struct.real_size_frame_in_bit;
    ARMNMF_DBC_POSTCONDITION((framesize_bits % 8) == 0);

    framesize_bytes = framesize_bits/8;
    bufOut->nFilledLen = framesize_bytes;

    if((mInterfaceFE.codec_state.first_time  == true) &&(mInterfaceFE.codec_state.mode == AAC_RAW))
    { 
        bufOut->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
        bufOut->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
        bufOut->nFlags |= OMX_BUFFERFLAG_STARTTIME;

        mInterfaceFE.codec_state.first_time  = false;
    }

    // bs_fast_move_bit_arm(&saved_bs_struct, framesize_bits);
    // mInterfaceFE.stream_struct.bits_struct = saved_bs_struct;

    if  (bufIn->nFlags & OMX_BUFFERFLAG_EOS) {
        bufOut->nFlags |= OMX_BUFFERFLAG_EOS;
        proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);
        reset();
    }


    mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    mPorts[INPUT_PORT].dequeueAndReturnBuffer();
}

void hst_encoders_framealigned_wrapper::process() 
{
    OMX_BUFFERHEADERTYPE * bufIn, * bufOut;

    if (mPorts[INPUT_PORT].queuedBufferCount() == 0 ||
            mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
        return;
    }

    if(encoder.IsNullInterface() || !mCodecInitialized)
    {
        mCodecInitialized = false;
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        return;
    }

    bufIn   = mPorts[INPUT_PORT].getBuffer(0);
    bufOut  = mPorts[OUTPUT_PORT].getBuffer(0);

	bufOut->nTimeStamp = bufIn->nTimeStamp;

    ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_local_struct != 0);

    // check that the buffer is a 32-bit multiple (since bs_init is 32bits)
    ARMNMF_DBC_ASSERT_MSG((bufOut->nAllocLen % BS_WORD_SIZE_IN_BYTES) == 0, "Buffer size not compliant with bs_init\n");

    bs_init(&mInterfaceFE.stream_struct.bits_struct,
            (unsigned int*)bufOut->pBuffer, (bufOut->nAllocLen/BS_WORD_SIZE_IN_BYTES) + 1, BS_INIT_SIZE_IN_BITS);

    mInterfaceFE.sample_struct.buf_add = (Float *) bufIn->pBuffer;
    if (bufIn->nFilledLen == 0 && bufIn->nFlags & OMX_BUFFERFLAG_EOS) {

        bufOut->nFilledLen = 0;
        bufOut->nFlags |= OMX_BUFFERFLAG_EOS;

        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();

        proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);

        reset();
    }
    else {
        encode_frame(bufIn, bufOut);
    }

}



////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    mPorts[INPUT_PORT].init (InputPort, false, false, 0, 0, 1, &inputport,   INPUT_PORT, 
            (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);
    mPorts[OUTPUT_PORT].init(OutputPort,false ,false, 0, 0, 1, &outputport, OUTPUT_PORT, 
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

    init(2, mPorts, &proxy, &me, false);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(setParameter)()
{
    memset(&mInterfaceFE, 0, sizeof(CODEC_INTERFACE_T));
    encoder.open(&mInterfaceFE);
    mCodecInitialized = true;

    ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
    ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);

    reset();

}

void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb,
        t_uint16 sample_bitsize)
{
    //PRECONDITION(sample_freq == mInterfaceFe.sample_struct.sample_freq);
    //  ARMNMF_DBC_PRECONDITION(chans_nb == mInterfaceFE.sample_struct.chans_nb);
    //  ARMNMF_DBC_PRECONDITION(sample_bitsize == encoder.getSampleBitSize());
}
