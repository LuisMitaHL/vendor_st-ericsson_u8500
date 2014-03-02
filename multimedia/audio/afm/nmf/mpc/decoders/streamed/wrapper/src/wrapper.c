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
#include <decoders/streamed/wrapper.nmf>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include <archi.h>
#include "fsm/component/include/Component.inl"

#include "common_interface.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_decoders_streamed_wrapper_src_wrapperTraces.h"
#endif

#define SIZE_SYNC_BUFF      24
#define INPUT_BITS_PER_WORD  16
#define BITSTREAM_WORD_SIZE  24

#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

// interface with decoder algo
static CODEC_INTERFACE_T    mInterfaceFE;
static int                  mSampleFreq;
static int                  mNbChannel;
static int                  mSampleSize;

static void *               mFifoOut[1];
static void *               mFifoIn[1];
static Port                 mPorts[2];
static Component            mWrapper;

////////////////////////////////////////
static  BS_STRUCT_T         mBSWrite;
static  BS_STRUCT_T         mBSRead;
static  BS_STRUCT_T         mBSInput;

// AV synchronisation
// take care when resetting this buffer 
// as it is used to store next timestamp value 
static  Buffer_t            mBuffer_out;

static  unsigned int        mAvailableRoom;
static  unsigned int        mAvailableBits;
static  unsigned int        mInputBufBits;
static  unsigned int        mFrameSizeBit;
static  int                 mFrameNb;
static  bool                mFrameReady;
static  bool                mEosReceived;
static  bool                bInputBufJustArrived;
static  bool                bOutputBufferFilled;
static  bool                bStopEmittingBuffers;

static  unsigned int        mPayloadDataBitsRead;
static  unsigned int        mPayloadDataBitsLeft;
static  unsigned int        mPayloadHeaderBitsRead;
static  unsigned int        mPartialHeaderArray[40];
static  unsigned int        mPartialHeaderByteCount;
static  bool                bPartialAsfHeaderPresent;
static  unsigned int        mPayLoadPresent;
// AV synchronisation
// Note: this simple implementation assumes:
// - timestamp values are continue
// - there is no data outputed before starttime is received (no "decode only" frame as it is not yet implemented)
static  bool                mTimeStampUpdateFlag;
static AVSynchro_t          mTimeStampInfo;

static bool                 bEndOfFrame;
////////////////////////////////////////////////////////////
static void
framebuffer_reset(void) {
    bs_reset(&mBSWrite);
    bs_reset(&mBSRead);
    bs_reset(&mBSInput);

    mEosReceived    = false;
    mAvailableRoom  = mBSWrite.buf_size * BITSTREAM_WORD_SIZE;
    mAvailableBits  = 0;
    mFrameReady     = false;
    bInputBufJustArrived = true;
    bOutputBufferFilled = false;   
    mInputBufBits = 0;

    mPayloadDataBitsRead = 0;
    mPayloadDataBitsLeft = 0;
    mPayloadHeaderBitsRead = 0;
    mPartialHeaderByteCount = 0;
    bPartialAsfHeaderPresent = false;

	TSreset(&mTimeStampInfo);
	mTimeStampUpdateFlag = false;
    bEndOfFrame = false;
}

static int
read_array_and_bitstream(int num1, int num2, int arrayIndex)
{
    unsigned int byte1, byte2, byte3, byte4;
    unsigned int value = 0;
    unsigned int byteCount;
    
    for(byteCount = 0; byteCount < num1; byteCount++){
        
        value = value | mPartialHeaderArray[arrayIndex+byteCount] << 8* byteCount;
    }
    
    for(byteCount = 0; byteCount < num2; byteCount++){
        byte1 = bs_read_byte(&mBSInput);
        value = value | byte1 << 8* byteCount;
    }
    return value;
}

static void
asf_read_payload_header(void)
{
    unsigned int NumberOfPayloadInfoByte;      //4 bytes
    unsigned int TimeStamp;                    //varies  
    unsigned int TimeStampRef;                 //varies
    unsigned int NumberofPayloadDataBytes;     //4 bytes
    unsigned int payloadTemp;
    unsigned int headerByteRead = 0;
    unsigned int partialHeaderByteLeft = 0;

    TimeStamp = 0;
    TimeStampRef = 0;
    NumberOfPayloadInfoByte = 0;
    if(bPartialAsfHeaderPresent == false) {
        //read 4 bytes for NumberOfPayloadInfoByte
        NumberOfPayloadInfoByte = read_array_and_bitstream(0,4,0);
        headerByteRead += 4;
        
        payloadTemp = NumberOfPayloadInfoByte/2;
        //read payloadTemp bytes for TimeStamp
        TimeStamp = read_array_and_bitstream(0,payloadTemp,0);
        headerByteRead += payloadTemp;
        
        //read payloadTemp bytes for TimeStampRef        
        TimeStampRef = read_array_and_bitstream(0,payloadTemp,0);  
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
            TimeStamp = read_array_and_bitstream(payloadTemp,0,headerByteRead);
            partialHeaderByteLeft -= payloadTemp;                  
        }
        else {
            TimeStamp = read_array_and_bitstream(partialHeaderByteLeft,payloadTemp-partialHeaderByteLeft,headerByteRead);
            partialHeaderByteLeft = 0;
        }
        headerByteRead += payloadTemp;
        
        //read payloadTemp bytes for TimeStampRef
        if(partialHeaderByteLeft >= payloadTemp) {
            TimeStampRef = read_array_and_bitstream(payloadTemp,0,headerByteRead);
            partialHeaderByteLeft -= payloadTemp;                  
        }
        else {
            TimeStampRef = read_array_and_bitstream(partialHeaderByteLeft,payloadTemp-partialHeaderByteLeft,headerByteRead);
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
static void 
asf_reformatted_payload(int nbits_to_write)
{
    int byteCount = 0;
    int partialDataBits = 0;
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
         bs_copy_bit(&mBSInput, &mBSWrite, mPayloadDataBitsLeft);  
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
              mPartialHeaderArray[byteCount] = bs_read_byte(&mBSInput);
           }
           bPartialAsfHeaderPresent = true; 
           mPayloadDataBitsRead += mPartialHeaderByteCount*8;
         }
         
      }else { 
         //incomplete packet data
         partialDataBits = nbits_to_write - mPayloadDataBitsRead;
         bs_copy_bit(&mBSInput, &mBSWrite, partialDataBits);
         mAvailableRoom -= partialDataBits;
         mAvailableBits += partialDataBits;  
         
         mPayloadDataBitsRead += partialDataBits;
         mPayloadDataBitsLeft -= partialDataBits;
      }
    }
    mInputBufBits -= mPayloadDataBitsRead;
}

static void
framebuffer_write(Component *this, Buffer_p buffer) {

    int i, nbits;
    int nbits_to_write = (mAvailableRoom < mInputBufBits) ? mAvailableRoom : mInputBufBits ;
    
    if ((mPayLoadPresent ==1) && (decoder.getHeaderType() == HEADER_PACKET)) {
        asf_reformatted_payload(nbits_to_write);
    } 
    else {

        // copy nbits_to_write from inputbuf to FB
        bs_copy_bit(&mBSInput, &mBSWrite, nbits_to_write);

        mInputBufBits  -= nbits_to_write;
        mAvailableRoom -= nbits_to_write;
        mAvailableBits += nbits_to_write;
    }
    
    if (mInputBufBits == 0) {
        if (buffer->flags & BUFFERFLAG_EOS) {
            mEosReceived = true;
        }
        bEndOfFrame = buffer->flags & BUFFERFLAG_ENDOFRAME;
        Port_dequeueAndReturnBuffer(&this->ports[IN]);
        bInputBufJustArrived = true;
    }
}


static void
framebuffer_consume(int nbits) {

    if (nbits <= mAvailableBits) {
        mAvailableRoom += nbits;
        mAvailableBits -= nbits;
    } else {
        // XXX this should not happen in theory but it does in practice
        mAvailableRoom += mAvailableBits;
        mAvailableBits = 0;
    }
    bEndOfFrame = false;

    bs_fast_move_bit(&mBSRead, nbits);
}

static bool
find_first_sync_word(void)
{
    unsigned int syncbuff_h, syncbuff_l;
    BS_STRUCT_T mybs;
    AUDIO_MODE_T    algo = UNKNOWN;
    int             skipped_bits = 0;

    if (mAvailableBits < 2 * SIZE_SYNC_BUFF) {
        return false;
    }

    mybs = mBSRead;

    syncbuff_h  = bs_read_bit(&mybs, SIZE_SYNC_BUFF);
    syncbuff_l  = bs_read_bit(&mybs, SIZE_SYNC_BUFF);

    mFrameSizeBit = 0;
    algo = decoder.checkSynchro(syncbuff_h, syncbuff_l, &mFrameSizeBit);

    if (mFrameSizeBit > decoder.getMaxFrameSize()) {
        algo = UNKNOWN;
    }

    while (algo == UNKNOWN
            && mAvailableBits - skipped_bits >= 2 * SIZE_SYNC_BUFF + 1) {

        syncbuff_h <<= 1;
        syncbuff_h |= (syncbuff_l >> 23);
        syncbuff_l <<= 1;
        syncbuff_l |= bs_read_bit(&mybs, 1);

        mFrameSizeBit = 0;
        algo = decoder.checkSynchro(syncbuff_h, syncbuff_l, &mFrameSizeBit);

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

static bool
check_next_sync_word(bool *not_enough_data)
{
    AUDIO_MODE_T    algo = UNKNOWN;
    unsigned int    dummy = 0;
    unsigned int    syncbuff_h;
    unsigned int    syncbuff_l;

    if (mAvailableBits >= (mFrameSizeBit + 2 * SIZE_SYNC_BUFF)) {
        BS_STRUCT_T next_sync_struct = mBSRead;

        bs_fast_move_bit(&next_sync_struct, mFrameSizeBit);

        syncbuff_h = bs_read_bit(&next_sync_struct, SIZE_SYNC_BUFF);
        syncbuff_l = bs_read_bit(&next_sync_struct, SIZE_SYNC_BUFF);

        algo = decoder.checkSynchro(syncbuff_h, syncbuff_l, &dummy);

        if (algo == mInterfaceFE.codec_state.mode) {
            mInterfaceFE.stream_struct.bits_struct  = mBSRead;
            return true;
        }
    } else {
        *not_enough_data = true;
    }

    return false;
}

static bool
parse_header_first_frame(void)
{
    if ((mAvailableBits >= decoder.getMaxFrameSize()) ||
        (mAvailableBits && bEndOfFrame)    ) {

        mInterfaceFE.stream_struct.bits_struct  = mBSRead;

        if (mFrameNb == 0) {
            return find_first_sync_word();
        }
        return true;
    }
    return false;
}

static bool
parse_header_at_each_frame(void)
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

static bool
parse_no_header_varying_size(void)
{
    if ((mAvailableBits >= decoder.getMaxFrameSize()) ||
        (mAvailableBits && bEndOfFrame)){

        mInterfaceFE.stream_struct.bits_struct  = mBSRead;

        if (mFrameNb == 0) {
            int dummy = 0;
            mInterfaceFE.codec_state.mode
            = decoder.checkSynchro(dummy, dummy, &dummy);
        }
        return true;

    }
    else if (mEosReceived && mAvailableBits) {
        mInterfaceFE.stream_struct.bits_struct  = mBSRead;
        mInterfaceFE.last_input_data = 1;

        if (mFrameNb == 0) {
            int dummy = 0;
            mInterfaceFE.codec_state.mode
            = decoder.checkSynchro(dummy, dummy, &dummy);
        }

        return true;
    }

    return false;
}

static bool
parse_header_packet(void)
{
    if (mFrameNb == 0) {
        int dummy = 0;
        mInterfaceFE.codec_state.mode = decoder.checkSynchro(dummy, dummy, &dummy);
        
        mFrameSizeBit = decoder.getBlockSize() * 8;
    }

    if (mEosReceived && mInterfaceFE.codec_state.eof == 0) {
        mInterfaceFE.last_input_data = 1;
    
    }
    else if (mAvailableBits < decoder.getBlockSize() * 8) 
    {    
        return false; 
    } 
        
    mInterfaceFE.stream_struct.bits_struct  = mBSRead;
    
    return true;
}


static bool
parse_one_frame(void)
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
        ASSERT(0);
        return false;
    }
}

static void
count_frame(void)
{
    mFrameNb++;

    VERBOSE(("+ frame %d\r", mFrameNb));
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
    }
}

static void
wrapper_reset(void) {
    mInterfaceFE.codec_state.mode                   = UNKNOWN;
    mInterfaceFE.codec_state.output_enable          = false;
    mInterfaceFE.codec_state.emit_buffers           = true;
    mInterfaceFE.codec_state.bfi                    = false;
    mInterfaceFE.codec_state.first_time             = true;
    mInterfaceFE.sample_struct.samples_interleaved  = true;

    mSampleFreq     = FREQ_UNKNOWN;
    mNbChannel      = 0;
    mSampleSize     = 0;
    mFrameNb        = 0;

    //reset input and output buffer
    framebuffer_reset();
    decoder.reset(&mInterfaceFE);
}

// Interface for fsm
static void
reset(Component *this) {
    bStopEmittingBuffers = false;
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
    if(portIdx == OUT) {
        //call newFormat only if wrapper is in executing state
        if (mSampleFreq != FREQ_UNKNOWN) {
            outputsettings.newFormat(mSampleFreq, mNbChannel, mSampleSize);
        }
        Component_scheduleProcessEvent(&mWrapper);
        bStopEmittingBuffers = false;
        return;
    }
}

static void
flushPortIndication(t_uint32 portIdx) {
    if(portIdx == IN) {
        wrapper_reset();
    }
}

static void
decode_frame(Component *this)
{
    int status;

	OstTraceFiltInst0 (TRACE_API, "AFM_MPC: decode_frame function");

    count_frame();

    mInterfaceFE.stream_struct.real_size_frame_in_bit = mAvailableBits;

    status = decoder.decodeFrame(&mInterfaceFE);

    if(status == RETURN_STATUS_WARNING) {
        OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: frame %d -> decoder Warning: %d", mFrameNb, mInterfaceFE.codec_state.status);
    }
    if (status > RETURN_STATUS_WARNING) {
        OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: frame %d -> decoder Error: %d", mFrameNb, mInterfaceFE.codec_state.status);        
        proxy.eventHandler(OMX_EventError, OMX_DSP_ErrorStreamCorrupt, 0);
        bStopEmittingBuffers = true;
        return;
    }

    mBuffer_out.filledLen = mInterfaceFE.sample_struct.block_len * mInterfaceFE.sample_struct.chans_nb ;
    bStopEmittingBuffers = (bool)!mInterfaceFE.codec_state.emit_buffers;
    bOutputBufferFilled  = mInterfaceFE.codec_state.output_enable;
    
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


static void
copy_output_buffer_to_buffer(Buffer_p targetBuf) {
    int i;
    check_output_format();

    targetBuf->filledLen = mBuffer_out.filledLen;

    for (i=0; i<targetBuf->filledLen; i++) {
        targetBuf->data[i] = mBuffer_out.data[i];
    }

    targetBuf->byteInLastWord  = 3;
    targetBuf->flags |= mBuffer_out.flags;

	// if StartTime Flag then reset it as it has been copied to real output buffer
	if (mBuffer_out.flags & BUFFERFLAG_STARTTIME)
	{
		mBuffer_out.flags ^= BUFFERFLAG_STARTTIME;		
	}

	if (mTimeStampUpdateFlag)
	{
		TScomputation(&mTimeStampInfo, targetBuf, mBuffer_out.filledLen, mSampleFreq, mNbChannel);
	}
}

static void
process_input_buffer(Component *this, Buffer_p bufIn) {
    if ((bInputBufJustArrived == true) &&
		!mTimeStampUpdateFlag &&
		!((bufIn->flags) & BUFFERFLAG_DECODEONLY))
	{	
		// if first time input buffer is read and StartTime Flag
		// then save StartTime and initial TimeStamp value
		mBuffer_out.flags |= BUFFERFLAG_STARTTIME;
        TSinit(&mTimeStampInfo, bufIn->nTimeStamph, bufIn->nTimeStampl);
		mTimeStampUpdateFlag = true;		
	}

	if (bInputBufJustArrived == true) {
        mInputBufBits   = bufIn->filledLen * INPUT_BITS_PER_WORD;
        if (bufIn->byteInLastWord == 1){
            mInputBufBits -= 8;
        }
        bs_init(&mBSInput, bufIn->data, (unsigned int)bufIn->allocLen, INPUT_BITS_PER_WORD);
        bInputBufJustArrived = false;

        /* check ASC frame */
        /* 0x80 = BUFFERFLAG_CODECCONFIG to be defined into ens/nmf/mpc/common/buffer.idt */
        if(bufIn->flags & 0x80 ) {
            int dummy = 0;
            decoder.checkSynchro(dummy, dummy, &mInputBufBits);
            /* clearing End of Frame flag in case of ASC detection */
            bufIn->flags &= (~((unsigned int)BUFFERFLAG_ENDOFRAME)); 
        }
    }
    framebuffer_write(this, bufIn);
}


static void
process(Component *this) {

    Buffer_p bufIn;
    Buffer_p bufOut = NULL;

	OstTraceFiltInst1 (TRACE_ALWAYS, "AFM_MPC: decoder wrapper: process (bOutputBufferFilled = %d)", bOutputBufferFilled);

    if (!bOutputBufferFilled && Port_queuedBufferCount(&this->ports[IN]) != 0 && !mEosReceived) {
        bufIn   = Port_getBuffer(&this->ports[IN], 0);
        process_input_buffer(this, bufIn);
    }

    while (1)
    {
        if (bStopEmittingBuffers){
            return;
        }

        if (Port_queuedBufferCount(&this->ports[OUT]) != 0) {
            bufOut   = Port_getBuffer(&this->ports[OUT], 0);
            if (bOutputBufferFilled) {
                copy_output_buffer_to_buffer(bufOut);
                Port_dequeueAndReturnBuffer(&this->ports[OUT]);
                bOutputBufferFilled = false;
                bufOut = NULL;
            } else {
				OstTraceFiltInst0(TRACE_DEBUG, "AFM_MPC: decoder wrapper: underflow");
            }
        }
        if (bOutputBufferFilled) return; // A frame has been decoded, but no output buffer has been provided yet : no need to parse nor decode

        if (!mFrameReady) {
            mFrameReady = parse_one_frame();
            if (!mFrameReady) {
                if (mInputBufBits != 0 || Port_queuedBufferCount(&this->ports[IN]) != 0 ) {
                    Component_scheduleProcessEvent(this);
                }
                else if ((mEosReceived) && (bufOut != NULL)) {
                    bufOut->filledLen = 0;
                    bufOut->flags |= BUFFERFLAG_EOS;
                    proxy.eventHandler(OMX_EventBufferFlag, OUT, bufOut->flags);

					TScomputation(&mTimeStampInfo, bufOut, 0, mSampleFreq, mNbChannel);
                    Port_dequeueAndReturnBuffer(&this->ports[OUT]);

                    wrapper_reset();
                    if (Port_queuedBufferCount(&this->ports[IN]) != 0) {
                        Component_scheduleProcessEvent(this);
                    }
                } 
                return;
            }
        }
        decode_frame(this);
    }
}

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(fsmInit) (fsmInit_t initFsm) {
	TRACE_t * this = (TRACE_t *)&mWrapper;
	
	// trace init (mandatory before port init)
    FSM_traceInit(&mWrapper, initFsm.traceInfoAddr, initFsm.id1);

	OstTraceFiltInst0(TRACE_ALWAYS, "AFM_MPC: this component stands for mpc_st_dec_wrapper");
	
	//decoder init
    decoder.open(&mInterfaceFE);

    POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
    POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);

    decoder.setOutputBuf(&mInterfaceFE, mBuffer_out.data);
    mInterfaceFE.sample_struct.buf_add = mBuffer_out.data;

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

void METH(setParameter)(void *buffer_in, t_uint16 size_in, void *buffer_out, t_uint16 size_out,  t_uint16 payLoad) {
    t_uint16 framebuffer_size;
    t_uint16 max_consumed_bits;
    t_uint16 blocksize, maxchans;

    t_uint16 min_size_in;
    t_uint16 min_size_out;

    if (decoder.getHeaderType() == HEADER_AT_EACH_FRAME ||
            decoder.getHeaderType() == HEADER_UNKNOWN) {
        max_consumed_bits = decoder.getMaxFrameSize() + 2 * SIZE_SYNC_BUFF;
    } else {
        max_consumed_bits = decoder.getMaxFrameSize();
    }

    // if the framesize buffer == current framesize then
    // the bs_delta_bit() doesn't see the consumed bit (pos_ptr seems to not change)
    // so we need to add 1 to the size
    min_size_in = max_consumed_bits / BITSTREAM_WORD_SIZE
                  + (max_consumed_bits % BITSTREAM_WORD_SIZE ? 1 : 0) + 1 ;

    decoder.getOutputGrain(&blocksize, &maxchans);
    min_size_out = sizeof(t_sword)*blocksize*maxchans;
    if ((size_in < min_size_in) || (size_out < min_size_out)) {
        ASSERT(0);
    }

    bs_init(&mBSWrite, buffer_in, size_in, 24);
    bs_init(&mBSRead, buffer_in, size_in, 24);

    mBuffer_out.data = (t_sword *)buffer_out;
    mBuffer_out.allocLen = (t_uint32)size_out;
    mBuffer_out.byteInLastWord = 0;
    mBuffer_out.filledLen = 0;
    mBuffer_out.flags = 0;
    mBuffer_out.address = (t_uint32)buffer_out;

    mPayLoadPresent = payLoad;
}

void METH(processEvent)() {
    Component_processEvent(&mWrapper);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
	TRACE_t * this = (TRACE_t *)&mWrapper;
	OstTraceFiltInst1 (TRACE_DEBUG, "AFM_MPC: decoder wrapper: emptyThisBuffer 0x%x", (unsigned int)buf);
	
    Component_deliverBuffer(&mWrapper, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
	TRACE_t * this = (TRACE_t *)&mWrapper;
	OstTraceFiltInst1 (TRACE_DEBUG, "AFM_MPC: deocder wrapper: fillThisBuffer 0x%x", (unsigned int)buf);	

    Component_deliverBuffer(&mWrapper, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mWrapper, cmd, param);
}

