/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <libeffects/libresampling.nmf>
#include <dbc.h>
#include <buffer.idt>
#include <stdbool.h>
#include "resample_local.h"

static void
fillBufferWithZeroes(Buffer_p buf) 
{
    int i;

    for (i = buf->filledLen; i < buf->allocLen; i++) {
        buf->data[i] = 0;
    }
}


// convert number of samples in timestamp unit (i.e us)
inline t_uint24
sample2Timestamp(t_uint24 freq, int nbSample)
{
  switch(freq)
  {
	case 96: return (wfmulr(nbSample, 0x42aaaa) *  20); // 1000000 / 96000 = 50000/96000 * 20
	case 88: return (wfmulr(nbSample, 0x48ba2e) *  20); // 1000000 / 88000 = 50000/88000 * 20
	case 64: return (wfmulr(nbSample, 0x640000) *  20); // 1000000 / 64000 = 50000/64000 * 20
	case 48: return (wfmulr(nbSample, 0x6aaaaa) *  25); // 1000000 / 48000 = 40000/48000 * 25
	case 44: return (wfmulr(nbSample, 0x74198a) *  25); // 1000000 / 44100 = 40000/44100 * 25
	case 32: return (wfmulr(nbSample, 0x500000) *  50); // 1000000 / 32000 = 20000/32000 * 50
	case 24: return (wfmulr(nbSample, 0x6aaaaa) *  50); // 1000000 / 24000 = 20000/24000 * 50
	case 22: return (wfmulr(nbSample, 0x74198a) *  50); // 1000000 / 22050 = 20000/22050 * 50
	case 16: return (wfmulr(nbSample, 0x500000) * 100); // 1000000 / 16000 = 10000/16000 * 100
	case 12: return (wfmulr(nbSample, 0x6aaaaa) * 100); // 1000000 / 12000 = 10000/12000 * 100
	case 11: return (wfmulr(nbSample, 0x74198a) * 100); // 1000000 / 11025 = 10000/11025 * 100 
	case 8:  return (nbSample * 125);                   // 1000000 /  8000 = 125
	case 7:  return (wfmulr(nbSample, 0x58e38e) * 200); // 1000000 /  7200 = 5000/7200  * 200
	default : return 0;
  }
}


static void
updateOutputFlagAndTimeStamp( Buffer_p inputBuf,
							  Buffer_p outputBuf,
							  t_uint24 outFreq,
							  int outSampleSize,
							  int lowLatency)
{
  outputBuf->flags |= (inputBuf->flags & ~(BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN)); // EOS flag and Drain ID are already managed
  inputBuf->flags &= ~BUFFERFLAG_STARTTIME; // reset input buffer STARTTIME flag
  
  if(lowLatency)
  {
	// no timestamp conversion needed for low latency
	outputBuf->nTimeStamph = inputBuf->nTimeStamph;              
	outputBuf->nTimeStampl = inputBuf->nTimeStampl;
  }
  else
  {
	if(outputBuf->nTimeStampl || outputBuf->nTimeStamph)
	{
	  // timestamp already updated, because we used more that one input buffer to create an output buffer
	  // nothing to do 
	}
	else
	{
	  // output timestamp = input timestamp
	  outputBuf->nTimeStamph = inputBuf->nTimeStamph;              
	  outputBuf->nTimeStampl = inputBuf->nTimeStampl;
	  
	  // and increase input timestamp by the duration of one ouput buffer, so that if we regenerate one buffer
	  // with this input buffer, the timestamp of next output buffer will be correct
	  inputBuf->nTimeStampl += sample2Timestamp(outFreq, outSampleSize);
	}
  }
}


static void
resample_process_low_latency(
        ResampleContext *resampleContext,
        Buffer_p inputBuf,
        Buffer_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    int framing = resampleContext->block_siz / resampleContext->fin_khz;

    consumed    = framing;
    produced    = 0;

    if (inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN)) {
        fillBufferWithZeroes(inputBuf);
        flush = true;
    }

    flush_in    = flush;

    if ((*resampleContext->processing) (
                resampleContext,
                inputBuf->data, 
                channels, 
                &consumed, 
                outputBuf->data, 
                &produced, 
                &flush) != 0)
    {
        ASSERT(FALSE);
    }

    if (flush_in == 0) {
        POSTCONDITION(consumed == framing);
        POSTCONDITION(produced == framing);
    }

    if ((inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN)) && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else {
        *needInputBuf = true;
    }

    outputBuf->filledLen = produced * resampleContext->fout_khz * channels;

    if (flush) {
        outputBuf->flags |= (inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN));
    }

	updateOutputFlagAndTimeStamp(inputBuf,outputBuf,0,0,1);
	
    *filledOutputBuf = true;
}

static void
resample_process_std(
        ResampleContext *resampleContext,
        Buffer_p inputBuf,
        Buffer_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    int i,nbsmp,fill_length, blocksize;


    blocksize = resampleContext->block_size_by_nb_channel/channels;

    produced = consumed = blocksize;

    if (inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN)){
        fillBufferWithZeroes(inputBuf);
        flush = true;
    }

    flush_in    = flush;

    if ((*resampleContext->processing) (
                resampleContext,
                inputBuf->data, 
                channels, 
                &consumed, 
                outputBuf->data, 
                &produced, 
                &flush) != 0)
    {
        ASSERT(FALSE);
    }

    POSTCONDITION(consumed == blocksize || consumed == 0);
    POSTCONDITION(produced == blocksize || produced == 0);

    if ((inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN)) && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else
    {
        if (consumed == blocksize || flush) {
            *needInputBuf = true;
        } else {
            *needInputBuf = false;
        }
    }

    if (produced == blocksize || flush) {
        outputBuf->byteInLastWord = 2;	 
        outputBuf->filledLen = produced * channels;
        if (flush) {
            outputBuf->flags |= (inputBuf->flags & (BUFFERFLAG_EOS | BUFFERFLAG_MASK_FOR_DRAIN));
        }
        *filledOutputBuf = true;
    } else {
        *filledOutputBuf = false;
    }

	updateOutputFlagAndTimeStamp(inputBuf, outputBuf, resampleContext->fout_khz, blocksize, 0);
}

void
resample_process(
        ResampleContext *resampleContext,
        Buffer_p inputBuf,
        Buffer_p outputBuf,
        int channels,
        bool *needInputBuf,
        bool *filledOutputBuf)
{
    if (resampleContext->low_latency == SRC_LOW_LATENCY_IN_MSEC) {
        resample_process_low_latency(resampleContext, inputBuf, outputBuf, channels, needInputBuf, filledOutputBuf);
    }
    else {
        resample_process_std(resampleContext, inputBuf, outputBuf, channels, needInputBuf, filledOutputBuf);
    }
}


