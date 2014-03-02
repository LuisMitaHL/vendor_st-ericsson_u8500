/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper_dec.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/decoders/framealigned/wrapper.nmf>

#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>

// size of the bitstream word (configured through bs-init)
// TODO FIXME: Set to 8 bits when the libbitstream supports it!
#define BS_INIT_SIZE_IN_BITS 32
#define BS_WORD_SIZE_IN_BYTES (BS_INIT_SIZE_IN_BITS/8)

#define SIZE_SYNC_BUFF  24

hst_decoders_framealigned_wrapper::hst_decoders_framealigned_wrapper (void)
{
    memset(&mInterfaceFE, 0, sizeof(CODEC_INTERFACE_T));

    memset(&mBSRead, 0, sizeof(BS_STRUCT_T));

    mCodecInitialized   = false;
    mSampleFreq         = FREQ_UNKNOWN;
    mNbChannel          = 0;
    mSampleSize         = 0;
    mFrameNb            = 0;
}

void METH(stop)(void) {
    // Close the decoder...
    decoder.close(&mInterfaceFE);
}

inline void hst_decoders_framealigned_wrapper::count_frame(void) 
{ 
  VERBOSE(("+ frame %d\r", ++mFrameNb));
}

void hst_decoders_framealigned_wrapper::check_synchro(OMX_BUFFERHEADERTYPE_p bufIn)
{
    int     dummy;
    unsigned int syncbuff_h, syncbuff_l;
    BS_STRUCT_T mybs;
    AUDIO_MODE_T    algo = UNKNOWN;

    mybs = mBSRead;

    syncbuff_h  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF);
    syncbuff_l  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF);

    algo = (AUDIO_MODE_T) decoder.checkSynchro(syncbuff_h, syncbuff_l, (t_sint32 *)&dummy);

    if( algo != UNKNOWN ) {
        mInterfaceFE.codec_state.mode = algo;
    }

    if (algo == UNKNOWN || ((OMX_BUFFERHEADERTYPE * )bufIn)->nFlags & OMX_BUFFERFLAG_DATACORRUPT) {
        mInterfaceFE.codec_state.bfi  = true;
    }

    mInterfaceFE.stream_struct.bits_struct  = mBSRead;
}

void hst_decoders_framealigned_wrapper::check_output_format(void)
{
    if (mInterfaceFE.sample_struct.sample_freq != mSampleFreq
            || mInterfaceFE.sample_struct.chans_nb != mNbChannel
            || decoder.getSampleBitSize() != mSampleSize) {

        mSampleFreq     = mInterfaceFE.sample_struct.sample_freq;
        mNbChannel      = mInterfaceFE.sample_struct.chans_nb;
        mSampleSize     = decoder.getSampleBitSize();

	if (!IS_NULL_INTERFACE(outputsettings, newFormat))
	{
	  outputsettings.newFormat((t_sample_freq)mSampleFreq, mNbChannel, mSampleSize);
	}
	//        afminform.newFormat(mSampleFreq, mNbChannel, mSampleSize);
    }
}

// Interface for fsm
void hst_decoders_framealigned_wrapper::reset()
{
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;
    
    mSampleFreq     = FREQ_UNKNOWN;
    mNbChannel      = 0;
    mSampleSize     = 0;
    mFrameNb        = 0;

    if(!decoder.IsNullInterface()) decoder.reset(&mInterfaceFE);
}

void METH(disablePortIndication)(t_uint32 portIdx) {} ;
void METH(enablePortIndication)(t_uint32 portIdx) {} ;
void METH(flushPortIndication)(t_uint32 portIdx) {} ;

void hst_decoders_framealigned_wrapper::decode_frame(OMX_BUFFERHEADERTYPE_p bufIn, OMX_BUFFERHEADERTYPE_p bufOut)
{
    int status;
    OMX_BUFFERHEADERTYPE * omxBufIn = (OMX_BUFFERHEADERTYPE *)bufIn, * omxBufOut = (OMX_BUFFERHEADERTYPE *)bufOut;

    count_frame();

    status = decoder.decodeFrame(&mInterfaceFE);

    if (status != RETURN_STATUS_OK) {
      proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorStreamCorrupt, 0);
      return;
    }

    check_output_format(); 

    
    if (mInterfaceFE.codec_state.output_enable) {

        omxBufOut->nFilledLen       = mInterfaceFE.sample_struct.block_len * mNbChannel * (mSampleSize / 8) ;
        omxBufOut->pBuffer          = (OMX_U8*) mInterfaceFE.sample_struct.buf_add;
		//        bufOut->byteInLastWord  = 3; 

        if  (omxBufIn->nFlags & OMX_BUFFERFLAG_EOS 
	     && mInterfaceFE.codec_state.remaining_blocks == 0) 
	{
	  omxBufOut->nFlags |= OMX_BUFFERFLAG_EOS;
	  proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, omxBufOut->nFlags);
	  reset();
        }

        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }

    if (mInterfaceFE.codec_state.bfi) {
        //TODO: What to do with this BFI ?
        mInterfaceFE.codec_state.bfi = 0;
    }

    if (mInterfaceFE.codec_state.remaining_blocks == 0)      {
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
    }
}


void hst_decoders_framealigned_wrapper::process(void)
{
    OMX_BUFFERHEADERTYPE * bufIn, * bufOut;
    
    if(mPorts[INPUT_PORT].queuedBufferCount() == 0 ||
       mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
      return;
    }

    if(decoder.IsNullInterface() || !mCodecInitialized)
    {
      mCodecInitialized = false;
      mPorts[INPUT_PORT].dequeueAndReturnBuffer();
      return;
    }
    
    bufIn   = mPorts[INPUT_PORT].getBuffer(0);
    bufOut  = mPorts[OUTPUT_PORT].getBuffer(0);

    // check that the buffer is a 32-bit multiple (since bs_init is 32bits)
    ARMNMF_DBC_ASSERT_MSG((bufOut->nAllocLen % BS_WORD_SIZE_IN_BYTES) == 0, "Buffer size not compliant with bs_init\n");

    // the target registers on ARM are 32 bits...
    bs_init(&mBSRead, (unsigned int*)bufIn->pBuffer, (bufIn->nAllocLen/BS_WORD_SIZE_IN_BYTES), BS_INIT_SIZE_IN_BITS);

    mInterfaceFE.sample_struct.buf_add = (Float*)bufOut->pBuffer;
    decoder.setOutputBuf(&mInterfaceFE, bufOut->pBuffer);

    if ((bufIn->nFilledLen == 0) && (bufIn->nFlags & OMX_BUFFERFLAG_EOS)) {

        bufOut->nFilledLen = 0;
        bufOut->nFlags |= OMX_BUFFERFLAG_EOS;

        proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);
        
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
	
	reset();
    }
    else {
      if (mInterfaceFE.codec_state.remaining_blocks == 0) {
	check_synchro(bufIn);
      }
      
      decode_frame(bufIn, bufOut);
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

  mPorts[INPUT_PORT].init(InputPort,  false, false, 0, 0, 1, &inputport,   INPUT_PORT, 
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
  decoder.open(&mInterfaceFE);
  mCodecInitialized = true;
  
  ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
  ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);
  
  reset();
}
