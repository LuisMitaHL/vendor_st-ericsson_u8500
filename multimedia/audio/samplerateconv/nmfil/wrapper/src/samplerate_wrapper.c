/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   samplerate_wrapper.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <samplerateconv/nmfil/wrapper.nmf>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include <verbose.h>
#include "libeffects/libresampling/include/resample.h"
#include "fsm/component/include/Component.inl"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "samplerateconv_nmfil_wrapper_src_samplerate_wrapperTraces.h"
#endif


#define OUT 1
#define IN  0
#define fmax_in_kHz_low_latency 48

//State machine variable
static void *       mFifoIn[1];
static void *       mFifoOut[1];
static Port         mPorts[2];
static Component    mSRC;

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////
static ResampleContext  mResampleContext;

static t_uint16         mChannels = 0;
static t_uint16         mPrevChannels = 0;
static t_sample_freq    mInputFreq  = FREQ_UNKNOWN;
static t_sample_freq    mOutputFreq = FREQ_UNKNOWN;
static t_sample_freq    mPrevInputFreq  = FREQ_UNKNOWN;
static t_sample_freq    mPrevOutputFreq = FREQ_UNKNOWN;
static t_uint16         mLowMipsToOut48;
static t_uint16         mLowMipsToOut48OnMemoryRequest;
static void *           mHeap;
static t_uint16         mHeapSize;
static t_uint16         mBlockSize;

// AV synchronisation
static AVSynchro_t      mTimeStampInfo;
static bool             mTimeStampUpdateFlag;


////////////////////////////////////////////////////////////
t_uint24 SRC_Freq2kHz(t_sample_freq freq)
{
  switch(freq)
  {
    case FREQ_192KHZ:	return 192;
	case FREQ_176_4KHZ:	return 176;
	case FREQ_128KHZ:	return 128;
	case FREQ_96KHZ:	return 96;
	case FREQ_88_2KHZ:	return 88;
	case FREQ_64KHZ:	return 64;
	case FREQ_48KHZ:	return 48;
	case FREQ_44_1KHZ:	return 44;
	case FREQ_32KHZ:	return 32;
	case FREQ_24KHZ:	return 24;
	case FREQ_22_05KHZ:	return 22;
	case FREQ_16KHZ:	return 16;
	case FREQ_12KHZ:	return 12;
	case FREQ_11_025KHZ:return 11;
	case FREQ_8KHZ:		return 8;
	case FREQ_7_2KHZ:	return 7;
	default :			return 0;
  }
}

void
SRC_reset(Component *this) {
    int status;

    TSreset(&mTimeStampInfo);
    mTimeStampUpdateFlag = false;

    if (mLowMipsToOut48==6) {
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                mHeap, mHeapSize, &mResampleContext, mInputFreq, 
                mOutputFreq, mLowMipsToOut48, mChannels, 0);
    } else {
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                mHeap, mHeapSize, &mResampleContext, mInputFreq, 
                mOutputFreq, mLowMipsToOut48, mChannels, mBlockSize * mChannels);
    }
	mPrevOutputFreq = mOutputFreq;
	mPrevInputFreq  = mInputFreq;
	mPrevChannels   = mChannels;
    ASSERT(status == 0);
}

void
SRC_disablePortIndication(t_uint32 portIdx) {
    
}


void
SRC_reconfigure()
{
  if((mPrevInputFreq != mInputFreq) ||
	 (mPrevOutputFreq != mOutputFreq) ||
	 (mChannels != mPrevChannels))
  {
	// sample freq (or nb_channels) changes since previous init,
	// reset just in case mode == 6 because in this case we are sure we have enough heap allocated
	// To be cleaned.
	if((mLowMipsToOut48==6) &&
	   (mLowMipsToOut48 == mLowMipsToOut48OnMemoryRequest) &&
	   (mHeapSize) && 
	   (mPorts[OUT].disabledState == OMX_StateInvalid) &&
	   (mPorts[IN].disabledState  == OMX_StateInvalid)) 
	  SRC_reset(&mSRC);
  }
}


void
SRC_enablePortIndication(t_uint32 portIdx) {
  SRC_reconfigure();
}


void
SRC_flushPortIndication(t_uint32 portIdx) {
    
}

static void
copy_input_to_output(
        Buffer_p inputBuf, 
        Buffer_p outputBuf) 
{
    int i;

    for (i = 0; i < inputBuf->filledLen; i++) {
        outputBuf->data[i] = inputBuf->data[i];
    }

    outputBuf->filledLen        = inputBuf->filledLen ;
    outputBuf->byteInLastWord   = inputBuf->byteInLastWord;
    outputBuf->flags           |= (inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_STARTTIME));
}

static void
sampleRateConversion(
        Buffer_p inputBuf, 
        Buffer_p outputBuf, 
        bool *needInputBuf, 
        bool *filledOutputBuf) 
{
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    int i, nbsmp, fill_length;

    if (mInputFreq == mOutputFreq) {
        copy_input_to_output(inputBuf, outputBuf);
        *needInputBuf    = true;
        *filledOutputBuf = true;
        return;
    }

    // perform SRC on each channel
    if (mLowMipsToOut48 == 6) {
        consumed = mBlockSize;
        produced = 0;
    } else {
        produced = consumed = mBlockSize;
    }

    flush = flush_in = (inputBuf->flags & BUFFERFLAG_EOS) ? true : false;
    if (flush_in) {
        if (mLowMipsToOut48 == 6) {
            fill_length = mBlockSize * mChannels * mResampleContext.fin_khz;
        } else {
            fill_length = mBlockSize * mChannels;
        }
        // SRC lib will read blocksize samples, 
        // so we must fill the buffer with zeros
        for (i = inputBuf->filledLen; i < fill_length; i++) {
            inputBuf->data[i] = 0;
        }
    }

    if((*mResampleContext.processing) (&mResampleContext, 
                                       inputBuf->data, 
                                       mChannels, 
                                       &consumed, 
                                       outputBuf->data, 
                                       &produced, 
                                       &flush) != 0)
    {
        ASSERT(FALSE);
    }

    if (mLowMipsToOut48 == 6) {
        if (!flush_in)
        {
            POSTCONDITION(consumed == mBlockSize);
            POSTCONDITION(produced == mBlockSize);
        }
        consumed *= mResampleContext.fin_khz;
        produced *= mResampleContext.fout_khz;
        nbsmp     = consumed;
    }
    else {
        POSTCONDITION((consumed == mBlockSize) || (consumed == 0));
        POSTCONDITION((produced == mBlockSize) || (produced == 0));
        nbsmp = mBlockSize;
    }
    
    if (flush_in && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else
    {
        if ((consumed == nbsmp) || flush) 
        {
            *needInputBuf = true;
        }
    }

    if (mLowMipsToOut48 == 6) 
    {
        nbsmp = produced;
    }
    if ((produced == nbsmp) || flush) {
        outputBuf->byteInLastWord = 2;
        outputBuf->filledLen = produced * mChannels;
        if (flush) {
            outputBuf->flags |= BUFFERFLAG_EOS;
        }
        *filledOutputBuf = true;
    } 

    if (inputBuf->flags & BUFFERFLAG_STARTTIME)
    {	// propagate only once starttime flag
        outputBuf->flags |= BUFFERFLAG_STARTTIME;
        inputBuf->flags  ^= BUFFERFLAG_STARTTIME;
    }
}

void
SRC_process(Component *this) {
    int i;
    OstTraceFiltInst0(TRACE_FLOW,"SRC::process  called");
    if (Port_queuedBufferCount(&this->ports[IN]) 
            &&  Port_queuedBufferCount(&this->ports[OUT])) 
    {
        bool needInputBuf = false, filledOutputBuf = false;
        Buffer_p inputBuf   = Port_getBuffer(&this->ports[IN], 0);
        Buffer_p outputBuf  = Port_getBuffer(&this->ports[OUT], 0);

        OstTraceFiltInst0(TRACE_FLOW,"SRC::process  called got all buffers");
        
        // store starting timestamp value
        if (inputBuf->flags & BUFFERFLAG_STARTTIME) {
            TSinit(&mTimeStampInfo, inputBuf->nTimeStamph, inputBuf->nTimeStampl);
            mTimeStampUpdateFlag = true;
        }

        if (mLowMipsToOut48!=6) {
            PRECONDITION(inputBuf->allocLen >= mBlockSize * mChannels);
            PRECONDITION(outputBuf->allocLen >= mBlockSize * mChannels);
        } else {
		  PRECONDITION(inputBuf->allocLen >= mBlockSize * mChannels * SRC_Freq2kHz(mInputFreq));
		  PRECONDITION(outputBuf->allocLen >= mBlockSize * mChannels * SRC_Freq2kHz(mOutputFreq));
        }

        sampleRateConversion(inputBuf, outputBuf, &needInputBuf, &filledOutputBuf);

        if (needInputBuf) {
            OstTraceFiltInst0(TRACE_FLOW,"SRC::process  release IN buffer");
            Port_dequeueAndReturnBuffer(&this->ports[IN]);
        }

        if (filledOutputBuf) {
            if(outputBuf->flags & BUFFERFLAG_EOS) {    
                OstTraceFiltInst1(TRACE_FLOW,"SRC::process  output BufferFlag %x",outputBuf->flags);
                proxy.eventHandler(OMX_EventBufferFlag, 1, outputBuf->flags);
                SRC_reset(this);
            }

            // Av sycnhronisation, compute timestamp
            if (mTimeStampUpdateFlag) {
                TScomputation(&mTimeStampInfo, outputBuf, outputBuf->filledLen, mOutputFreq, mChannels);
            }
            OstTraceFiltInst0(TRACE_FLOW,"SRC::process  release OUT buffer");
            Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        }
    }
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setParameter)(
        t_sample_freq   input_freq, 
        t_sample_freq   output_freq,  
        t_uint16        lowmips_to_out48,
        t_uint16        conv_type, 
        t_uint16        blocksize, /*SamplesInMs for arguments of resample_x_process_fixin_fixout (mLowMipsToOut48=6)*/ 
        t_uint16        channels,
		t_uint16        force_reconfiguration
        ) {


    PRECONDITION(input_freq != FREQ_UNKNOWN && output_freq != FREQ_UNKNOWN); 
    PRECONDITION(output_freq >= FREQ_48KHZ && output_freq <= FREQ_7_2KHZ);
    PRECONDITION(input_freq >= FREQ_96KHZ && input_freq <= FREQ_7_2KHZ);
    //PRECONDITION(!lowmips_to_out48 || output_freq == FREQ_48KHZ);

	mPrevInputFreq      = mInputFreq;
	mPrevOutputFreq     = mOutputFreq;
	mPrevChannels       = mChannels;
    mInputFreq          = input_freq;
    mOutputFreq         = output_freq;
    mLowMipsToOut48     = lowmips_to_out48;
    mBlockSize          = blocksize;
    mChannels           = channels;

    if (mLowMipsToOut48 == 6) {
	    resample_calc_max_heap_size_fixin_fixout(0,0,6,&mHeapSize,0,2,0);
		if(force_reconfiguration)
		{
		  SRC_reconfigure();
		}
    } else {
        resample_calc_max_heap_size_fixin_fixout(mInputFreq,mOutputFreq,mLowMipsToOut48,&mHeapSize,mBlockSize*mChannels,mChannels,conv_type);
    }
}


void METH(setMemory)(void *buf, t_uint16 size) {

    int    status;

    ASSERT(size == mHeapSize);
    mHeap = buf;

    Component_init(&mSRC, 2, mPorts, &proxy);

    if (mLowMipsToOut48 == 6) {
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                mHeap, mHeapSize, &mResampleContext, mInputFreq, 
                mOutputFreq, mLowMipsToOut48, mChannels,0);
    } else {
        status = resample_x_init_ctx_low_mips_fixin_fixout(
                mHeap, mHeapSize, &mResampleContext, mInputFreq, 
                mOutputFreq, mLowMipsToOut48, mChannels, mBlockSize * mChannels);
    }
    ASSERT(status == 0);
}

void METH(fsmInit) (fsmInit_t initFsm) {
    // trace init (mandatory before port init)
    FSM_traceInit(&mSRC, initFsm.traceInfoAddr, initFsm.id1);

    Port_init(
            &mPorts[IN], InputPort, false, false, 0, &mFifoIn,  1, &inputport, IN, 
            (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mSRC);

    Port_init(
            &mPorts[OUT], OutputPort, false, false, 0, &mFifoOut, 1, &outputport, OUT, 
            (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mSRC);

    mSRC.reset                  = SRC_reset;
    mSRC.process                = SRC_process;
    mSRC.disablePortIndication  = SRC_disablePortIndication;
    mSRC.enablePortIndication   = SRC_enablePortIndication;
    mSRC.flushPortIndication    = SRC_flushPortIndication;

    mLowMipsToOut48OnMemoryRequest = mLowMipsToOut48;
    afm.requestMemory(mHeapSize);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) 
{
    ASSERT(freq == mInputFreq);
    ASSERT(chans_nb == mChannels);
    ASSERT(sample_bitsize == 24);

}

void METH(processEvent)() {
    Component_processEvent(&mSRC);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mSRC, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mSRC, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mSRC, cmd, param);
}


