/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/decoders/streamed/wrapper.nmf>
#include <armnmf_dbc.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#include "audiotables.h"
};

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_decoders_streamed_wrapper_src_wrapperTraces.h"
#endif

#undef OST_TRACE_COMPILER_IN_USE


#define SIZE_SYNC_BUFF_IN_BITS       24
#define INPUT_BITS_PER_WORD          32
#define BITSTREAM_WORD_SIZE_IN_BITS  32
#define BITSTREAM_WORD_SIZE_IN_BYTES (BITSTREAM_WORD_SIZE_IN_BITS/8)
// TODO: make this a static constant
//#define SAMPLE_SIZE_IN_BYTES          2

#define ONE_MILI_SEC 1000      // this is 1000 micro second



hst_decoders_streamed_wrapper::hst_decoders_streamed_wrapper (void)
{
    memset(&mInterfaceFE, 0, sizeof(CODEC_INTERFACE_T));

    memset(&mBSWrite, 0, sizeof(BS_STRUCT_T));
    memset(&mBSRead, 0, sizeof(BS_STRUCT_T));
    memset(&mBSInput, 0, sizeof(BS_STRUCT_T));

    memset(&mBuffer_out, 0, sizeof(OMX_BUFFERHEADERTYPE));

    bInputBufJustArrived        = true;
    bOutputBufferFilled         = false;
    bPartialAsfHeaderPresent    = false;
    bStopEmittingBuffers        = false;
    mEosReceived                = false;
    mAvailableRoom              = 0;
    mAvailableBits              = 0;
    mFrameReady                 = false;
    mFrameNb                    = 0;
    mFrameSizeBit               = 0;
    mInputBufBits               = 0;
    mNbChannel                  = 0;
    mPartialHeaderByteCount     = 0;
    mPayloadDataBitsRead        = 0;
    mPayloadDataBitsLeft        = 0;
    mPayloadHeaderBitsRead      = 0;
    mSampleFreq                 = FREQ_UNKNOWN;
    mSampleSize                 = 0;
    mPayLoadPresent             = 0 ;

    mPropagateTS                = true;
    for (unsigned int i = 0; i < 40; i++) {
        mPartialHeaderArray[i] = 0;
    }

}


void METH(stop)(void) {
    // Close the decoder...
    decoder.close(&mInterfaceFE);
}

void METH(framebuffer_reset)(void) {
    bs_reset(&mBSWrite);
    bs_reset(&mBSRead);
    bs_reset(&mBSInput);

    mEosReceived    = false;
    mAvailableRoom  = mBSWrite.buf_size * BITSTREAM_WORD_SIZE_IN_BITS;
    mAvailableBits  = 0;
    mFrameReady     = false;
    bInputBufJustArrived = true;
    bOutputBufferFilled = false;
    bStopEmittingBuffers = false;
    mInputBufBits = 0;
	mPropagateTS    = true;

    mPayloadDataBitsRead = 0;
    mPayloadDataBitsLeft = 0;
    mPayloadHeaderBitsRead = 0;
    mPartialHeaderByteCount = 0;
    bPartialAsfHeaderPresent = false;
}

int METH(read_array_and_bitstream)(unsigned int num1, unsigned int num2, unsigned int arrayIndex)
{
    unsigned int byte1;
    unsigned int value = 0;
    unsigned int byteCount;
    
    for(byteCount = 0; byteCount < num1; byteCount++){
        
        value = value | mPartialHeaderArray[arrayIndex+byteCount] << 8* byteCount;
    }
    
    for(byteCount = 0; byteCount < num2; byteCount++){
        byte1 =  bs_read_bit_arm(&mBSInput, 8);
        value = value | byte1 << 8* byteCount;
    }
    return value;
}

void METH(asf_read_payload_header)(void)
{
    unsigned int NumberOfPayloadInfoByte;      //4 bytes
    //unsigned int TimeStamp;                    //varies  
    //unsigned int TimeStampRef;                 //varies
    unsigned int NumberofPayloadDataBytes;     //4 bytes
    unsigned int payloadTemp;
    unsigned int headerByteRead = 0;
    unsigned int partialHeaderByteLeft = 0;

    //TimeStamp = 0;
    //TimeStampRef = 0;
    NumberOfPayloadInfoByte = 0;
    if(bPartialAsfHeaderPresent == false) {
        //read 4 bytes for NumberOfPayloadInfoByte
        NumberOfPayloadInfoByte = read_array_and_bitstream(0,4,0);
        headerByteRead += 4;
        
        payloadTemp = NumberOfPayloadInfoByte/2;
        //read payloadTemp bytes for TimeStamp
        //TimeStamp = read_array_and_bitstream(0,payloadTemp,0);
        headerByteRead += payloadTemp;
        
        //read payloadTemp bytes for TimeStampRef        
        //TimeStampRef = read_array_and_bitstream(0,payloadTemp,0);  
        headerByteRead += payloadTemp;
        
        //read 4 bytes for NumberofPayloadDataBytes
        NumberofPayloadDataBytes = read_array_and_bitstream(0,4,0);   
        headerByteRead += 4;
        
        mPayloadHeaderBitsRead = headerByteRead*8;
    }
    else { //partial header information present
        partialHeaderByteLeft = mPartialHeaderByteCount;
        
        //read 4 bytes for NumberOfPayloadInfoByte
        if(mPartialHeaderByteCount >= 4){            
            NumberOfPayloadInfoByte = read_array_and_bitstream(4,0,0); 
            partialHeaderByteLeft -= 4;   
        }
        else {
            NumberOfPayloadInfoByte = read_array_and_bitstream(mPartialHeaderByteCount,4-mPartialHeaderByteCount,0);
            partialHeaderByteLeft = 0;
        }
        headerByteRead += 4;
        
        payloadTemp = NumberOfPayloadInfoByte/2;
        //read payloadTemp bytes for TimeStamp
        if(partialHeaderByteLeft >= payloadTemp) {
            //TimeStamp = read_array_and_bitstream(payloadTemp,0,headerByteRead);
            partialHeaderByteLeft -= payloadTemp;                  
        }
        else {
            //TimeStamp = read_array_and_bitstream(partialHeaderByteLeft,payloadTemp-partialHeaderByteLeft,headerByteRead);
            partialHeaderByteLeft = 0;
        }
        headerByteRead += payloadTemp;
        
        //read payloadTemp bytes for TimeStampRef
        if(partialHeaderByteLeft >= payloadTemp) {
            //TimeStampRef = read_array_and_bitstream(payloadTemp,0,headerByteRead);
            partialHeaderByteLeft -= payloadTemp;                  
        }
        else {
            //TimeStampRef = read_array_and_bitstream(partialHeaderByteLeft,payloadTemp-partialHeaderByteLeft,headerByteRead);
            partialHeaderByteLeft = 0;
        }
        headerByteRead += payloadTemp;
        
        //read 4 bytes for NumberofPayloadDataBytes
        if(partialHeaderByteLeft == 4) {
            NumberofPayloadDataBytes = read_array_and_bitstream(4,0,headerByteRead);
            partialHeaderByteLeft -= 4;
        }
        else {
            NumberofPayloadDataBytes = read_array_and_bitstream(partialHeaderByteLeft,4-partialHeaderByteLeft,headerByteRead);
            partialHeaderByteLeft = 0;
        }
        bPartialAsfHeaderPresent = false;
        headerByteRead += payloadTemp;
        mPayloadHeaderBitsRead = (headerByteRead - partialHeaderByteLeft)*8;
    }
    
    mPayloadDataBitsLeft = NumberofPayloadDataBytes * 8;
    //mPayloadHeaderBitsRead = headerByteRead*8;
    
}

//reformat wma_v9 payload
void METH(asf_reformatted_payload)(unsigned int nbits_to_write)
{
    unsigned int byteCount = 0;
    unsigned int partialDataBits = 0;
    //reset 
    mPayloadDataBitsRead = 0;
    
    while(mPayloadDataBitsRead < nbits_to_write)
    {
      if(mPayloadDataBitsLeft == 0)
      {
        //read header
        asf_read_payload_header();
        mPayloadDataBitsRead += mPayloadHeaderBitsRead; 
      }
      
      if((nbits_to_write - mPayloadDataBitsRead) >= mPayloadDataBitsLeft){
         bs_copy_bit_arm(&mBSInput, &mBSWrite, mPayloadDataBitsLeft);  
         mPayloadDataBitsRead  += mPayloadDataBitsLeft; 
         
         mAvailableRoom -= mPayloadDataBitsLeft;
         mAvailableBits += mPayloadDataBitsLeft;
         
         mPayloadDataBitsLeft = 0;
         //read next header
         if((nbits_to_write - mPayloadDataBitsRead) > mPayloadHeaderBitsRead)  {   
             asf_read_payload_header();
             mPayloadDataBitsRead += mPayloadHeaderBitsRead; 
                      
         } else if (nbits_to_write == mPayloadDataBitsRead) {
             //done
             break;
         }else {
           //incomplete header
           //save it to mPartialHeaderArray
           mPartialHeaderByteCount = (nbits_to_write - mPayloadDataBitsRead)/8;
           for (byteCount = 0; byteCount < mPartialHeaderByteCount; byteCount++)
           {
	            mPartialHeaderArray[byteCount] = bs_read_bit_arm(&mBSInput, 8);
           }
           bPartialAsfHeaderPresent = true; 
           mPayloadDataBitsRead += mPartialHeaderByteCount*8;
         }
         
      }else { 
         //incomplete packet data
         partialDataBits = nbits_to_write - mPayloadDataBitsRead;
         bs_copy_bit_arm(&mBSInput, &mBSWrite, partialDataBits);
         mAvailableRoom -= partialDataBits;
         mAvailableBits += partialDataBits;  
         
         mPayloadDataBitsRead += partialDataBits;
         mPayloadDataBitsLeft -= partialDataBits;
      }
    }
    mInputBufBits -= mPayloadDataBitsRead;
}

void METH(framebuffer_write)(OMX_BUFFERHEADERTYPE_p buffer) {

    int nbits_to_write = (mAvailableRoom < mInputBufBits) ? mAvailableRoom : mInputBufBits ;
    if ((mPayLoadPresent ==1) && (decoder.getHeaderType() == HEADER_PACKET)) {
        asf_reformatted_payload(nbits_to_write);
    } 
    else {
        // copy nbits_to_write from inputbuf to FB
        bs_copy_bit_arm(&mBSInput, &mBSWrite, nbits_to_write);

        mInputBufBits  -= nbits_to_write;
        mAvailableRoom -= nbits_to_write;
        mAvailableBits += nbits_to_write;
    }
    
 if ((mInputBufBits == 0)&&(!bStopEmittingBuffers)) {
      if (((OMX_BUFFERHEADERTYPE*)buffer)->nFlags & OMX_BUFFERFLAG_EOS) {
            mEosReceived = true;
        }
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
    }
}


void METH(framebuffer_consume)(unsigned int nbits) {

    if (nbits <= mAvailableBits) {
        mAvailableRoom += nbits;
        mAvailableBits -= nbits;
    } else {
        // XXX this should not happen in theory but it does in practice
        mAvailableRoom += mAvailableBits;
        mAvailableBits = 0;
    }

    bs_fast_move_bit_arm(&mBSRead, nbits);
}

bool METH(find_first_sync_word)(void)
{
    unsigned int syncbuff_h, syncbuff_l;
    BS_STRUCT_T mybs;
    AUDIO_MODE_T    algo = UNKNOWN;
    int             skipped_bits = 0;

    if (mAvailableBits < 2 * SIZE_SYNC_BUFF_IN_BITS) {
        return false;
    }

    mybs = mBSRead;

    syncbuff_h  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF_IN_BITS);
    syncbuff_l  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF_IN_BITS);

    algo = (AUDIO_MODE_T) decoder.checkSynchro((t_uint24)syncbuff_h, (t_uint24)syncbuff_l, (t_sint32 *)&mFrameSizeBit);

    if (mFrameSizeBit > decoder.getMaxFrameSize()) {
        algo = UNKNOWN;
    }

    while (algo == UNKNOWN
            && mAvailableBits - skipped_bits >= 2 * SIZE_SYNC_BUFF_IN_BITS + 1) {

        syncbuff_h <<= 1;
        syncbuff_h |= (syncbuff_l >> 23);
        syncbuff_l <<= 1;
        syncbuff_l |= bs_read_bit_arm(&mybs, 1);

        algo = (AUDIO_MODE_T) decoder.checkSynchro(syncbuff_h, syncbuff_l, (t_sint32 *) &mFrameSizeBit);

        if (mFrameSizeBit >= decoder.getMaxFrameSize()) {
            algo = UNKNOWN;
        }

        skipped_bits++;
    }
    if (skipped_bits) {
        framebuffer_consume(skipped_bits);
    }

    if (algo == UNKNOWN) {
        return false;
    } else {
        mInterfaceFE.codec_state.mode = algo;
        return true;
    }
}

bool METH(check_next_sync_word)(bool *not_enough_data)
{
    AUDIO_MODE_T    algo = UNKNOWN;
    t_sint32        dummy;
    unsigned int    syncbuff_h;
    unsigned int    syncbuff_l;

    if (mAvailableBits >= (unsigned int)(mFrameSizeBit + 2 * SIZE_SYNC_BUFF_IN_BITS)) {
        BS_STRUCT_T next_sync_struct = mBSRead;

        bs_fast_move_bit_arm(&next_sync_struct, mFrameSizeBit);

        syncbuff_h = bs_read_bit_arm(&next_sync_struct, SIZE_SYNC_BUFF_IN_BITS);
        syncbuff_l = bs_read_bit_arm(&next_sync_struct, SIZE_SYNC_BUFF_IN_BITS);

        algo = (AUDIO_MODE_T)decoder.checkSynchro(syncbuff_h, syncbuff_l, &dummy);

        if (algo == mInterfaceFE.codec_state.mode) {
            mInterfaceFE.stream_struct.bits_struct  = mBSRead;
            return true;
        }
    } else {
        *not_enough_data = true;
    }

    return false;
}

bool METH(parse_header_first_frame)(void)
{
  if (mAvailableBits >= (unsigned int) decoder.getMaxFrameSize()) {

        mInterfaceFE.stream_struct.bits_struct  = mBSRead;

        if (mFrameNb == 0) {
            return find_first_sync_word();
        }
        return true;
    }
    return false;
}

bool METH(parse_header_at_each_frame)(void)
{
    bool not_enough_data = false;

    while (1) {
        if (! find_first_sync_word()) {
            return false;
        }
        if (check_next_sync_word(&not_enough_data)) {
            return true;
        }
        if (not_enough_data) {
            return false;
        }

        // synchro not found so skip 1 bit
        framebuffer_consume(1);
    }
}

bool METH(parse_no_header_varying_size)(void)
{
  if (mAvailableBits >= (unsigned int) decoder.getMaxFrameSize()) {

        mInterfaceFE.stream_struct.bits_struct  = mBSRead;

        if (mFrameNb == 0) {
            t_sint32 dummy = 0;
            mInterfaceFE.codec_state.mode
	      = (AUDIO_MODE_T) decoder.checkSynchro(0, 0, &dummy);
        }
        return true;

    }
    else if (mEosReceived && mAvailableBits) {
        mInterfaceFE.stream_struct.bits_struct  = mBSRead;
        mInterfaceFE.last_input_data = 1;

        if (mFrameNb == 0) {
            t_sint32 dummy = 0;
            mInterfaceFE.codec_state.mode 
	      = (AUDIO_MODE_T) decoder.checkSynchro(0, 0, &dummy);
        }

        return true;
    }

    return false;
}

bool METH(parse_header_packet)(void)
{
    if (mFrameNb == 0) {
      t_sint32 dummy = 0;
        mInterfaceFE.codec_state.mode = (AUDIO_MODE_T) decoder.checkSynchro(0, 0, &dummy);
        
        mFrameSizeBit = decoder.getBlockSize() * 8;
    }

    if (mEosReceived && mInterfaceFE.codec_state.eof == 0) {
        mInterfaceFE.last_input_data = 1;
    
    }
    else if (mAvailableBits < (unsigned int)(decoder.getBlockSize() * 8)) 
    {    
        return false; 
    } 
        
    mInterfaceFE.stream_struct.bits_struct  = mBSRead;
    
    return true;
}


bool METH(parse_one_frame)(void)
{
    switch (decoder.getHeaderType()) {
    case HEADER_AT_EACH_FRAME:
        return parse_header_at_each_frame();

    case NO_HEADER_VARYING_SIZE:
        return parse_no_header_varying_size();

    case HEADER_PACKET:
        return parse_header_packet();

    case HEADER_ONCE:
        return parse_header_first_frame();

    default:
      ARMNMF_DBC_ASSERT_MSG(0, "Decoder header type unknown\n");
        return false;
    }
}


/*
	Support for the Frame Aligned input data, in OpenMax Buffers, to streaming decoder wrapper
    This will work only if OMX_BUFFERFLAG_ENDOFFRAME is Set
*/
bool METH(parse_framealigned_data)(void)
{
   bool  status = false;

   mInterfaceFE.stream_struct.bits_struct  = mBSRead;

    switch (decoder.getHeaderType()) {
    case HEADER_AT_EACH_FRAME:

		status = find_first_sync_word();
        if (mAvailableBits < (unsigned int)mFrameSizeBit)
        {
            return false;
        }

        return status;

    case NO_HEADER_VARYING_SIZE:

		if (mFrameNb == 0) {
            t_sint32 dummy = 0;
            mInterfaceFE.codec_state.mode  = (AUDIO_MODE_T) decoder.checkSynchro(0, 0, &dummy);
        }

		if (mEosReceived) {
            if ( mAvailableBits)
            {
			    mInterfaceFE.last_input_data = 1;
            }
            else
            {
                // We have received the last OMX buffer with no data.
                // We don't want this buffer to go to decoder, rather it should merely raise EOS event in process() func
                return false;
            }
		}

        return true;

    case HEADER_PACKET:
        return parse_header_packet();

    case HEADER_ONCE:
        mInterfaceFE.stream_struct.bits_struct  = mBSRead;

        if (mFrameNb == 0) {
            return find_first_sync_word();
        }
        return true;

    default:
        ARMNMF_DBC_ASSERT_MSG(0, "Decoder header type unknown\n");
        return false;
    }
}

void METH(count_frame)(void)
{
    mFrameNb++;
}

void METH(check_output_format)(void)
{
    if (mInterfaceFE.sample_struct.sample_freq != mSampleFreq
            || mInterfaceFE.sample_struct.chans_nb != mNbChannel
            || decoder.getSampleBitSize() != mSampleSize) {

        mSampleFreq     = (t_sample_freq) mInterfaceFE.sample_struct.sample_freq;
        mNbChannel      = mInterfaceFE.sample_struct.chans_nb;
        mSampleSize     = decoder.getSampleBitSize();

        outputsettings.newFormat((t_sample_freq)mSampleFreq, mNbChannel, mSampleSize);
    }
}

void METH(wrapper_reset)(void) {
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.emit_buffers           = true;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;
    mInterfaceFE.last_input_data = 0;
    mSampleFreq     = FREQ_UNKNOWN;
    mNbChannel      = 0;
    mSampleSize     = 0;
    mFrameNb        = 0;

	mPropagateTS    = false;
    //reset input and output buffer
    framebuffer_reset();
    decoder.reset(&mInterfaceFE);
}

// Interface for fsm
void METH(reset)() {
    wrapper_reset();
}

void METH(disablePortIndication)(t_uint32 portIdx) {
    if(portIdx == INPUT_PORT) {
        wrapper_reset();
    }
}

void METH(enablePortIndication)(t_uint32 portIdx) {

    if(portIdx == OUTPUT_PORT) {
        if (mSampleFreq != FREQ_UNKNOWN) {
            outputsettings.newFormat((t_sample_freq)mSampleFreq, mNbChannel, mSampleSize) ;
        }
        scheduleProcessEvent();
        bStopEmittingBuffers = false ;
        return;
    }
}

void METH(flushPortIndication)(t_uint32 portIdx) {
    if(portIdx == INPUT_PORT) {
        wrapper_reset();
    }
}

void METH(decode_frame)()
{
    int status;


    count_frame();

    if (mInterfaceFE.codec_state.remaining_blocks == 0)      {
        mInterfaceFE.stream_struct.real_size_frame_in_bit = mAvailableBits;
    }

    status = decoder.decodeFrame(&mInterfaceFE);

    if(status == RETURN_STATUS_WARNING) {
        OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: frame %d -> decoder Warning: %d", mFrameNb, mInterfaceFE.codec_state.status);
    }
    if (status > RETURN_STATUS_WARNING) {
        OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: frame %d -> decoder Error: %d", mFrameNb, mInterfaceFE.codec_state.status);
        proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorStreamCorrupt, 0);
        bStopEmittingBuffers = true;
        return;
    }

    mBuffer_out.nFilledLen = mInterfaceFE.sample_struct.block_len * mInterfaceFE.sample_struct.chans_nb *  ((mInterfaceFE.sample_struct.sample_size)/8);
    bOutputBufferFilled = mInterfaceFE.codec_state.output_enable;
    bStopEmittingBuffers = (bool)!mInterfaceFE.codec_state.emit_buffers;

    if(decoder.getHeaderType() == HEADER_PACKET){
        mFrameSizeBit =
                bs_delta_bit(&mInterfaceFE.stream_struct.bits_struct, &mBSRead);
        framebuffer_consume(mFrameSizeBit);
        mFrameReady     = false;
    }
    else if (mInterfaceFE.codec_state.remaining_blocks == 0)      {
        if ((decoder.getHeaderType() == HEADER_ONCE)
                || (decoder.getHeaderType() == NO_HEADER_VARYING_SIZE)) {
            mFrameSizeBit =
                bs_delta_bit(&mInterfaceFE.stream_struct.bits_struct, &mBSRead);
        }

        framebuffer_consume(mFrameSizeBit);
        mFrameReady     = false;

    }

    if (mInterfaceFE.codec_state.bfi) {
        //TODO: What to do with this BFI ?
        mInterfaceFE.codec_state.bfi = 0;
    }
}

void METH(copy_output_buffer_to_buffer)(OMX_BUFFERHEADERTYPE_p targetBuf)
{
    unsigned int i;
    OMX_BUFFERHEADERTYPE * pTargetBuf = (OMX_BUFFERHEADERTYPE *)targetBuf;

	OstTraceFiltInst1 (TRACE_DEBUG, "AFM_HOST: decoder wrapper copy_output_buffer_to_buffer targetBuf 0x%x", (unsigned int)targetBuf);

    check_output_format();

    pTargetBuf->nFilledLen = mBuffer_out.nFilledLen;

    for (i=0; i<pTargetBuf->nFilledLen; i++) {
        pTargetBuf->pBuffer[i] = mBuffer_out.pBuffer[i];
    }

    pTargetBuf->nFlags = mBuffer_out.nFlags;
    pTargetBuf->nTimeStamp = mBuffer_out.nTimeStamp;

	// reset old flag and TimeStamp
	mBuffer_out.nFlags = 0;
	mBuffer_out.nTimeStamp = 0;
}


void METH(process_input_buffer)(OMX_BUFFERHEADERTYPE_p pBufIn)
{

    OMX_BUFFERHEADERTYPE * bufIn  = (OMX_BUFFERHEADERTYPE*) pBufIn;
	OMX_BUFFERHEADERTYPE * bufOut = NULL;

    unsigned int  nFlags = (unsigned int ) bufIn->nFlags;

    if (bInputBufJustArrived == true)
    {
		if ((mPropagateTS && !(bufIn->nFlags & OMX_BUFFERFLAG_DECODEONLY))||
		    (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
            mBuffer_out.nTimeStamp = bufIn->nTimeStamp;
            mBuffer_out.nFlags |= OMX_BUFFERFLAG_STARTTIME;
            mPropagateTS = false;
		}

        OstTraceFiltInst1 (TRACE_DEBUG, "AFM_HOST: decoder wrapper process_input_buffer inBuf 0x%x", (unsigned int)pBufIn);

        mInputBufBits   = bufIn->nFilledLen * 8;

        // TODO: check what this means when it comes to Host...
        // if (bufIn->byteInLastWord == 1){
        // mInputBufBits -= 8;
        // }
        bs_init(&mBSInput, (unsigned int*)bufIn->pBuffer, bufIn->nAllocLen/(INPUT_BITS_PER_WORD/8), INPUT_BITS_PER_WORD);
        bInputBufJustArrived = false;

		if (nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
            int dummy = 0 ;
            int CodecConfigDataSize = mInputBufBits;

            nFlags &= (~(unsigned int)(OMX_BUFFERFLAG_ENDOFFRAME));
            decoder.checkSynchro(dummy, dummy, (t_sint32 *)&CodecConfigDataSize);
        }
    }

    framebuffer_write(bufIn);

	if (nFlags & OMX_BUFFERFLAG_ENDOFFRAME) {
        mFrameReady = parse_framealigned_data();
	}

	if (nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        mFrameReady = false;
	mPropagateTS = true;
	}
}


void METH(process)()
{

    OMX_BUFFERHEADERTYPE * bufIn  = NULL;
    OMX_BUFFERHEADERTYPE * bufOut = NULL;

	OstTraceFiltInst2 (TRACE_FLOW, "AFM_HOST: decoder wrapper: process (bOutputBufferFilled = %d) and (bStopEmittingBuffers = %d)", bOutputBufferFilled,bStopEmittingBuffers);

	if (!bOutputBufferFilled && mPorts[INPUT_PORT].queuedBufferCount() != 0 && !mEosReceived) {
        bufIn   = mPorts[INPUT_PORT].getBuffer(0);
        process_input_buffer(bufIn);
    }

    while (1)
    {
        if (bStopEmittingBuffers){
            scheduleProcessEvent();
            return;
        }

        if (mPorts[OUTPUT_PORT].queuedBufferCount() != 0) {
            bufOut   = mPorts[OUTPUT_PORT].getBuffer(0);
            if (bOutputBufferFilled) {
                copy_output_buffer_to_buffer(bufOut);
                mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
		//		printf("release outptubuffer: %d\n", mOutBuf++);
                bOutputBufferFilled = false;
                bufOut = NULL;
            } else {
                OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: decoder wrapper: process No buffer queued at output port of decoder");
            }
        }
        if (bOutputBufferFilled) return; // A frame has been decoded, but no output buffer has been provided yet : no need to parse nor decode
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: decoder wrapper: process before parsing (mFrameReady = %d))", mFrameReady);

        if (!mFrameReady) {
            mFrameReady = parse_one_frame();

            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: decoder wrapper: process after parsing (mFrameReady_after_parsing = %d))", mFrameReady);

            if (!mFrameReady) {
                if (mInputBufBits != 0 || mPorts[INPUT_PORT].queuedBufferCount() != 0 ) {
                    scheduleProcessEvent();
                }
                else if ((mEosReceived) && (bufOut != NULL)) {
                    bufOut->nFilledLen = 0;
                    bufOut->nFlags |= OMX_BUFFERFLAG_EOS;
                    proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);

                    mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
                    wrapper_reset();
                    if (mPorts[INPUT_PORT].queuedBufferCount() != 0) {
                        scheduleProcessEvent();
                    }
                }
                return;
            }
        }
        decode_frame();
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: decoder wrapper:  (Frame No. = %d))", mFrameNb);

    }
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(fsmInit) (fsmInit_t initFsm) {
    //decoder init
    decoder.open(&mInterfaceFE);

    ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
    ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);

    decoder.setOutputBuf(&mInterfaceFE, mBuffer_out.pBuffer);
    // TODO; check about this float.......
    mInterfaceFE.sample_struct.buf_add = (Float*)mBuffer_out.pBuffer;

	// trace init (mandatory before port init)
	if (initFsm.traceInfoAddr){
	    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
	}

	OstTraceFiltInst0 (TRACE_ALWAYS, "AFM_HOST: this component stands for hst_st_dec_wrapper");


    mPorts[INPUT_PORT].init(InputPort, false, false, 0, 0, 1, &inputport, INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT))), (initFsm.portsTunneled & (1 << (INPUT_PORT))), this);
    mPorts[OUTPUT_PORT].init(OutputPort, false, false, 0, 0, 1, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT))), (initFsm.portsTunneled & (1 << (OUTPUT_PORT))), this);

    init(2, mPorts, &proxy, &me, false);
}


void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}


void METH(setParameter)(void *buffer_in, t_uint32 size_in, void *buffer_out, t_uint32 size_out, t_uint32 payLoad) {
    t_uint32 max_consumed_bits;
    t_uint16 blocksize, maxchans;

    t_uint32 min_size_in;
    t_uint32 min_size_out;

    if (decoder.getHeaderType() == HEADER_AT_EACH_FRAME ||
            decoder.getHeaderType() == HEADER_UNKNOWN) {
        max_consumed_bits = decoder.getMaxFrameSize() + 2 * SIZE_SYNC_BUFF_IN_BITS;
    } else {
        max_consumed_bits = decoder.getMaxFrameSize();
    }

    // if the framesize buffer == current framesize then
    // the bs_delta_bit() doesn't see the consumed bit (pos_ptr seems to not change)
    // so we need to add 1 to the size
    min_size_in = max_consumed_bits / BITSTREAM_WORD_SIZE_IN_BITS
                  + (max_consumed_bits % BITSTREAM_WORD_SIZE_IN_BITS ? 1 : 0) + 1 ;

    decoder.getOutputGrain(&blocksize, &maxchans);
    min_size_out =  (t_uint32)((decoder.getSampleBitSize() /8) * blocksize * maxchans);

    ARMNMF_DBC_ASSERT_MSG((size_in >= min_size_in) && (size_out >= min_size_out), "streamed wrapper: Set parameter sizes not consistant\n");

    bs_init(&mBSWrite, (unsigned int *)buffer_in, size_in/BITSTREAM_WORD_SIZE_IN_BYTES, BITSTREAM_WORD_SIZE_IN_BITS);
    bs_init(&mBSRead, (unsigned int *)buffer_in, size_in/BITSTREAM_WORD_SIZE_IN_BYTES, BITSTREAM_WORD_SIZE_IN_BITS);

    mBuffer_out.pBuffer = (OMX_U8 *)buffer_out;
    mBuffer_out.nAllocLen = (t_uint32)size_out;
    mBuffer_out.nFilledLen = 0;
    mBuffer_out.nFlags = 0;
    mBuffer_out.nTimeStamp = 0;
    mPayLoadPresent =  payLoad;
}
