/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmin.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include <hst/bindings/shmpcm/shmin.nmf>
#include <stdbool.h>
#include <armnmf_dbc.h>

#include "bindings_optim.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_bindings_shmpcm_shmin_src_shminTraces.h"
#endif

//#define SHMPCMIN_TRACE_CONTROL
#ifdef SHMPCMIN_TRACE_CONTROL
#include <stdio.h>
#endif
//FIXME !!
#ifndef IMPORT_C
#define IMPORT_C
#endif

#define SAMPLE_SIZE_IN (mShmConfig.bitsPerSampleIn>>3)
#define SAMPLE_SIZE_OUT (mShmConfig.bitsPerSampleOut>>3)

#define FREQ_TAB_SIZE 17

static const unsigned int  Freq[FREQ_TAB_SIZE] = {0, 192000, 176400, 128000, 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7200};

hst_bindings_shmpcm_shmin::hst_bindings_shmpcm_shmin()
{
    bFsmInitialized = false;
    for (int i=0; i < MAX_NB_BUFFERS; i++)
    {
        mPendingBuffer[i] = 0;
    }
}

void hst_bindings_shmpcm_shmin::fillBufWithZeros(OMX_BUFFERHEADERTYPE*  buf)
{
    int i;
    for (i = buf->nFilledLen; i < buf->nAllocLen; i++) {
        buf->pBuffer[i] = 0;
    }
    buf->nFilledLen = buf->nAllocLen;
}


void hst_bindings_shmpcm_shmin::updateTimestamp(OMX_BUFFERHEADERTYPE* bufOut, ShmPcmConfig_t ShmConfig, OMX_S64 initial, OMX_S64 * nbSamples )
{
    if (bPropagateTS)
    {
        // update TimeStamp
        if ((ShmConfig.sampleFreq > 0) &&
            (ShmConfig.sampleFreq < FREQ_TAB_SIZE) &&
            (ShmConfig.channelsOut))
        {
            bufOut->nTimeStamp = (*nbSamples * 10000)/(Freq[ShmConfig.sampleFreq] * ShmConfig.channelsOut) * 100;
            bufOut->nTimeStamp += initial;
        }
        else
        {
            bufOut->nTimeStamp = 0;
        }
        *nbSamples += bufOut->nFilledLen/SAMPLE_SIZE_OUT;
        OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: Shmpcmin::updateTimestamp latency control (host_shmpcmin, output) (timestamp = 0x%x 0x%x us)", (int)(bufOut->nTimeStamp >> 32), (unsigned int)(bufOut->nTimeStamp & 0xffffffffu));
    }
}
int hst_bindings_shmpcm_shmin::copy_downmix_in16b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in16b_out16b_swap_optim((void *) in, (void *) out, size / 2);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in16b_out16b_noswap_optim((void *) in, (void *) out, size / 2);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in16b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in16b_out32b_swapin_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in16b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in32b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in32b_out16b_swapin_optim((void *) in, (void *) out, size / 2);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in32b_out16b_noswap_optim((void *) in, (void *) out, size / 2);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in32b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in32b_out32b_swap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_downmix_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_downmix_in32b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in16b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in16b_out16b_swap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in16b_out16b_noswap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in16b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in16b_out32b_swapin_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in16b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in32b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in32b_out16b_swapin_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in32b_out16b_noswap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in32b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in32b_out32b_swap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_upmix_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_upmix_in32b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size * 2;
}

int hst_bindings_shmpcm_shmin::copy_in16b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out16b_swap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out16b_noswap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in16b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out32b_swapin_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in32b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out16b_swapin_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out16b_noswap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in32b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out32b_swap_optim((void *) in, (void *) out, size);
    return size;
}

int hst_bindings_shmpcm_shmin::copy_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out32b_noswap_optim((void *) in, (void *) out, size);
    return size;
}



void hst_bindings_shmpcm_shmin::process()
{
#ifdef SHMPCMIN_TRACE_CONTROL
    printf("hst_shmpcm_shmin::process\n");
#endif

    OMX_BUFFERHEADERTYPE *bufIn, *bufOut;
    t_sint16 *inptr, *outptr;
    int input_size, output_size, copied_size, offset;
    int nb_samples_out, nb_samples_in;;

    if ( (mShmConfig.isSynchronized == false)
         &&  mInputBufAlreadyReceived
         &&  mPorts[INPUT_PORT].queuedBufferCount() == 0
         &&  mPorts[OUTPUT_PORT].queuedBufferCount() != 0)
    {
        bufOut = (OMX_BUFFERHEADERTYPE*)mPorts[OUTPUT_PORT].getBuffer(0);

        OstTraceFiltInst1(TRACE_WARNING, "AFM_HOST: Shmpcmin::Underflow, output buffer filled with %d bytes of zeros", bufOut->nFilledLen);

        for (unsigned int i =0; i < bufOut->nAllocLen; i++) {
            bufOut->pBuffer[i] = 0;
        }

        bufOut->nFlags = 0;
        bufOut->nFilledLen = bufOut->nAllocLen;
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        return;
    }

    if (mShmConfig.channelsIn == 2 && mShmConfig.channelsOut == 1)
    {
        processDownMix();
        return;
    }
    else if (mShmConfig.channelsIn == 1 && mShmConfig.channelsOut == 2) {
        processUpMix();
        return;
    }

    if (mPorts[INPUT_PORT].queuedBufferCount() == 0
            || mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
        return;
    }

    mInputBufAlreadyReceived = true;

    bufIn  = (OMX_BUFFERHEADERTYPE*)mPorts[INPUT_PORT].getBuffer(0);
    bufOut = (OMX_BUFFERHEADERTYPE*)mPorts[OUTPUT_PORT].getBuffer(0);

    offset = bufOut->nFilledLen;

    if ((bInputBufJustArrived == true) && (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
        bInputBufJustArrived = false;
        mInitialTS = bufIn->nTimeStamp;
        mNbSamplesForTSComputation = - (offset / (t_sint32)SAMPLE_SIZE_OUT);
        bPropagateTS = true;
    }

    inptr = (t_sint16 *)(bufIn->pBuffer + mDataConsumed);
    outptr = (t_sint16 *)(bufOut->pBuffer + offset);

    input_size = bufIn->nFilledLen - mDataConsumed;
    output_size = bufOut->nAllocLen - offset;

    nb_samples_in = input_size / SAMPLE_SIZE_IN;
    nb_samples_out = output_size / SAMPLE_SIZE_OUT;

    if (nb_samples_in == nb_samples_out)
    {
        //copied_size = copy(inptr, outptr, nb_samples_in);
        copied_size = (*hst_bindings_shmpcm_shmin::Shmin_copy_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += output_size;
        bufOut->nFlags     = bufIn->nFlags;
        bufOut->nOffset    = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::process release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
        mDataConsumed = 0;

        if (bufOut->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
        }

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::process release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();

    }
    else if (nb_samples_in < nb_samples_out) {

        //copied_size = copy(inptr, outptr, nb_samples_in);
        copied_size = (*hst_bindings_shmpcm_shmin::Shmin_copy_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen   += copied_size*SAMPLE_SIZE_OUT;
        bufOut->nFlags       = bufIn->nFlags;
        bInputBufJustArrived = true;
        bufOut->nOffset      = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::process release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        mDataConsumed = 0;

        if (bufOut->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::process eos found, release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
            fillBufWithZeros(bufOut);

            updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
        // Changes for handling endofframe flag for single frame case
        /*else if (bufOut->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)
        {
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::process ENDOFFRAME found, release output buffer for framealigned case (nFilledLen=%d)",bufOut->nFilledLen);
            updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }*/
        else if (mPorts[INPUT_PORT].queuedBufferCount() != 0) {
            scheduleProcessEvent();
        } else if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_WARNING,"AFM_HOST: Shmpcmin::process Underflow, output buffer filled with zeros (%d/%d)", bufOut->nFilledLen, bufOut->nAllocLen);

            fillBufWithZeros(bufOut);

            bufOut->nFlags = 0;
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
    }
    else  {

        //copied_size = copy(inptr, outptr, nb_samples_out);
        copied_size = (*hst_bindings_shmpcm_shmin::Shmin_copy_routine) (inptr, outptr, nb_samples_out);

        mDataConsumed += nb_samples_out * SAMPLE_SIZE_IN;
        bufOut->nFilledLen += copied_size * SAMPLE_SIZE_OUT;
        bufOut->nOffset    = 0;

        if (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)
        {
          bufOut->nFlags |= (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME);
          bufIn->nFlags  ^= OMX_BUFFERFLAG_STARTTIME;
        }

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::process release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }
}

void hst_bindings_shmpcm_shmin::processDownMix()
{
    OMX_BUFFERHEADERTYPE*    bufIn, *bufOut;
    int         offset;
    t_sint16 * inptr, *outptr;
    int input_size, output_size;
    int nb_samples_out, nb_samples_in;;

    if (mPorts[INPUT_PORT].queuedBufferCount() == 0
            || mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
        return;
    }

    mInputBufAlreadyReceived = true;

    bufIn  = (OMX_BUFFERHEADERTYPE*)mPorts[INPUT_PORT].getBuffer(0);
    bufOut = (OMX_BUFFERHEADERTYPE*)mPorts[OUTPUT_PORT].getBuffer(0);

    offset = bufOut->nFilledLen;

    if ((bInputBufJustArrived == true) && (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
        bInputBufJustArrived = false;
        mInitialTS = bufIn->nTimeStamp;
        mNbSamplesForTSComputation = - (offset / (t_sint32)SAMPLE_SIZE_OUT);
        bPropagateTS = true;
    }

    inptr = (t_sint16 *)(bufIn->pBuffer + mDataConsumed);

    input_size = bufIn->nFilledLen - mDataConsumed;
    output_size = bufOut->nAllocLen - offset;

    nb_samples_in  = input_size  / SAMPLE_SIZE_IN;
    nb_samples_out = output_size / SAMPLE_SIZE_OUT;

    outptr = (t_sint16 *)(bufOut->pBuffer + offset);

    if (nb_samples_in == (nb_samples_out*2))
    {
        //copied_size = copy_downmix(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_downmix_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += (nb_samples_in/2) * SAMPLE_SIZE_OUT;
        bufOut->nFlags     = bufIn->nFlags;
        bufOut->nOffset    = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processDownMix release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
        mDataConsumed = 0;

        if (bufOut->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
        }

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processDownMix release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }
    else if (nb_samples_in < (nb_samples_out*2)) {

        //copied_size = copy_downmix(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_downmix_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += (nb_samples_in/2) * SAMPLE_SIZE_OUT;
        bufOut->nFlags     = bufIn->nFlags;
        bufOut->nOffset    = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processDownMix release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
        mDataConsumed = 0;

        if (bufOut->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::processDownMix eos found, release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
            fillBufWithZeros(bufOut);

            updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
        else if (mPorts[INPUT_PORT].queuedBufferCount() != 0) {
            scheduleProcessEvent();
        } else if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_WARNING,"AFM_HOST: Shmpcmin::processDownMix Underflow, output buffer filled with zeros (%d/%d)", bufOut->nFilledLen, bufOut->nAllocLen);

            fillBufWithZeros(bufOut);
            bufOut->nFlags = 0;
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
    }
    else {
      // case where the input size is bigger than the output size
      // so copy the out size only, and keep the consummed data count
        //copied_size = copy_downmix(inptr, outptr, nb_samples_out * 2);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_downmix_routine) (inptr, outptr, nb_samples_out * 2);

        mDataConsumed += (nb_samples_out * 2) * SAMPLE_SIZE_IN;
        bufOut->nFilledLen += nb_samples_out * SAMPLE_SIZE_OUT;
        bufOut->nOffset    = 0;
        if (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)
        {
          bufOut->nFlags |= (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME);
          bufIn->nFlags  ^= OMX_BUFFERFLAG_STARTTIME;
        }
        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processDownMix release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }
}

// FIXME: processUpMix is certainly bugged, need to fix it when either standalone test or real available usecase is ready
void hst_bindings_shmpcm_shmin::processUpMix()
{
    OMX_BUFFERHEADERTYPE*    bufIn, *bufOut;
    int         offset;
    t_sint16 * inptr, *outptr;
    int input_size, output_size;
    int nb_samples_out, nb_samples_in;

    if (mPorts[INPUT_PORT].queuedBufferCount() == 0
            || mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
        return;
    }

    mInputBufAlreadyReceived= true;

    bufIn  = (OMX_BUFFERHEADERTYPE*)mPorts[INPUT_PORT].getBuffer(0);
    bufOut = (OMX_BUFFERHEADERTYPE*)mPorts[OUTPUT_PORT].getBuffer(0);

    offset = bufOut->nFilledLen;

    if ((bInputBufJustArrived == true) && (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
      bInputBufJustArrived = false;
      mInitialTS = bufIn->nTimeStamp;
      mNbSamplesForTSComputation = - (offset / (t_sint32)SAMPLE_SIZE_OUT);
      bPropagateTS = true;
    }

    inptr = (t_sint16 *)(bufIn->pBuffer + mDataConsumed);
    outptr = (t_sint16 *)(bufOut->pBuffer + offset);

    input_size = bufIn->nFilledLen - mDataConsumed;
    output_size = bufOut->nAllocLen - offset;

    nb_samples_in = input_size / SAMPLE_SIZE_IN;
    nb_samples_out = output_size / SAMPLE_SIZE_OUT;

    if ((nb_samples_in*2) == nb_samples_out)
    {
        //copied_size = copy_upmix(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_upmix_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += (nb_samples_in * 2) * SAMPLE_SIZE_OUT;
        bufOut->nFlags     = bufIn->nFlags;
        bufOut->nOffset    = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processUpMix release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
        mDataConsumed = 0;

        if (bufIn->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
        }

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processUpMix release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }
    else if ((nb_samples_in*2) < nb_samples_out) {

        //copied_size = copy_upmix(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_upmix_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += (nb_samples_in * 2) * SAMPLE_SIZE_OUT;
        bufOut->nFlags     = bufIn->nFlags;
        bufOut->nOffset    = 0;

        OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processUpMix release input buffer");
        mPorts[INPUT_PORT].dequeueAndReturnBuffer();
        bInputBufJustArrived = true;
        mDataConsumed = 0;

        if (bufOut->nFlags & OMX_BUFFERFLAG_EOS) {
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::processUpMix eos found, release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
            fillBufWithZeros(bufOut);
            updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
        else if (mPorts[INPUT_PORT].queuedBufferCount() != 0) {
            scheduleProcessEvent();
        } else if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_WARNING,"AFM_HOST: Shmpcmin::processUpMix Underflow, output buffer filled with zeros (%d/%d)", bufOut->nFilledLen, bufOut->nAllocLen);

            fillBufWithZeros(bufOut);
            bufOut->nFlags = 0;
            mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
        }
    }
    else {

        //copied_size = copy_upmix(inptr, outptr, nb_samples_out/2);
        (*hst_bindings_shmpcm_shmin::Shmin_copy_upmix_routine) (inptr, outptr, nb_samples_out / 2);

        mDataConsumed += (nb_samples_out / 2) * SAMPLE_SIZE_IN;
        bufOut->nFilledLen += nb_samples_out * SAMPLE_SIZE_OUT;
        bufOut->nOffset     = 0;
        if (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME)
        {
          bufOut->nFlags |= (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME);
          bufIn->nFlags  ^= OMX_BUFFERFLAG_STARTTIME;
        }
        OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::processUpMix release output buffer (nFilledLen=%d)",bufOut->nFilledLen);
        updateTimestamp(bufOut, mShmConfig, mInitialTS, &mNbSamplesForTSComputation);
        mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    }
}

void hst_bindings_shmpcm_shmin::reset()
{
    mDataConsumed   = 0;
    mSendNewFormat  = true;
    mInitialTS = 0;
    mNbSamplesForTSComputation = 0;
    bInputBufJustArrived = true;
    bPropagateTS = false;
    mInputBufAlreadyReceived = false;
}

void hst_bindings_shmpcm_shmin::disablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_shmpcm_shmin::disablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}
void hst_bindings_shmpcm_shmin::enablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_shmpcm_shmin::enablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}
void hst_bindings_shmpcm_shmin::flushPortIndication(t_uint32 portIdx) { reset(); } ;

////////////////////////////////////////////////////////////////////////
//                    Provided Interfaces
////////////////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    mPorts[INPUT_PORT].init(InputPort,  false, false, 0,        0, mShmConfig.nb_buffer,  &inputport,  INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);
    mPorts[OUTPUT_PORT].init(OutputPort, true, false, 0, &mBufOut, 1, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

    init(2, mPorts, &proxy, &me, false);
    // queue emptythisbuffer received before fsminit (if any)
    bFsmInitialized = true;
    for(int i=0;i<MAX_NB_BUFFERS;i++)
    {
        if(mPendingBuffer[i] != (OMX_BUFFERHEADERTYPE_p)0)
        {
            mPorts[INPUT_PORT].queueBuffer(mPendingBuffer[i]);
            mPendingBuffer[i] = (OMX_BUFFERHEADERTYPE_p)0;
        }
    }
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    OstTraceInt0(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::setTunnelStatus should not be called : treatment done in controller only");
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(setParameter)(ShmPcmConfig_t config, void *buffer, t_uint16 output_blocksize) {
#ifdef SHMPCMIN_TRACE_CONTROL
    printf("hst_shmpcm_shmin::setParameter\n");
    printf("\tnb_buffer=%u\n", (unsigned int)config.nb_buffer);
    printf("\tswap_bytes=%u\n", (unsigned int)config.swap_bytes);
    printf("\tbitsPerSampleIn=%u\n", (unsigned int)config.bitsPerSampleIn);
    printf("\tbitsPerSampleOut=%u\n", (unsigned int)config.bitsPerSampleOut);
    printf("\tchannelsIn =%u\n", (unsigned int)config.channelsIn);
    printf("\tchannelsOut =%u\n", (unsigned int)config.channelsOut);
    printf("\tsampleFreq =%d\n", (int)config.sampleFreq );
    printf("\toutput blocksize =%u\n", (unsigned int)output_blocksize);
#endif
    OstTraceInt3(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::setParameter nb_buffer=%d buffer=0x%x, swap_bytes=%d", (unsigned int)config.nb_buffer,(unsigned int)buffer,(unsigned int)config.swap_bytes);
    OstTraceInt1(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::setParameter blocksize=%d", (unsigned int)output_blocksize);
    OstTraceInt2(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::setParameter bitsPerSampleIn=%d, bitsPerSampleOut=%d", (unsigned int)config.bitsPerSampleIn, (unsigned int)config.bitsPerSampleOut);
    OstTraceInt2(TRACE_ALWAYS, "AFM_HOST: Shmpcmin::setParameter channelsIn=%d, channelsOut=%d", (unsigned int)config.channelsIn, (unsigned int)config.channelsOut);

    ARMNMF_DBC_PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);

    mShmConfig = config;
    mBufOut =  (OMX_BUFFERHEADERTYPE*) buffer;

    ARMNMF_DBC_ASSERT((mShmConfig.bitsPerSampleIn  == 16) || (mShmConfig.bitsPerSampleIn  == 32));
    ARMNMF_DBC_ASSERT((mShmConfig.bitsPerSampleOut == 16) || (mShmConfig.bitsPerSampleOut == 32));
    if(mShmConfig.bitsPerSampleIn == 16)
    {
        if(mShmConfig.bitsPerSampleOut == 16)
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in16b_out16b_swap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in16b_out16b_swap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in16b_out16b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in16b_out16b_noswap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in16b_out16b_noswap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in16b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in16b_out32b_swap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in16b_out32b_swap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in16b_out32b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in16b_out32b_noswap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in16b_out32b_noswap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in16b_out32b_noswap;
            }
        }
    }
    else
    {
        if(mShmConfig.bitsPerSampleOut == 16)
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in32b_out16b_swap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in32b_out16b_swap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in32b_out16b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in32b_out16b_noswap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in32b_out16b_noswap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in32b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in32b_out32b_swap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in32b_out32b_swap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in32b_out32b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = &hst_bindings_shmpcm_shmin::copy_downmix_in32b_out32b_noswap;
                Shmin_copy_upmix_routine   = &hst_bindings_shmpcm_shmin::copy_upmix_in32b_out32b_noswap;
                Shmin_copy_routine         = &hst_bindings_shmpcm_shmin::copy_in32b_out32b_noswap;
            }
        }
    }
}


void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    if (mSendNewFormat
            && cmd == OMX_CommandStateSet && ((OMX_STATETYPE)param) == OMX_StateExecuting){
        outputsettings.newFormat(
                mShmConfig.sampleFreq,
                mShmConfig.channelsOut,
                mShmConfig.bitsPerSampleOut);
        mSendNewFormat = false;
    }

    Component::sendCommand(cmd, param) ;
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer){

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmin::emptyThisBuffer receive input buffer (nFilledLen=%d)",buffer->nFilledLen);
    OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: Shmpcmin::emptyThisBuffer latency control (host_shmpcmin, input) (timestamp = 0x%x 0x%x us)", (int)(buffer->nTimeStamp >> 32), (unsigned int)(buffer->nTimeStamp & 0xffffffffu));

    // check if fsmInit already done otherwise save this buffer pointer to queue it later
    if(bFsmInitialized)
    {
        Component::deliverBuffer(INPUT_PORT, buffer);
    }
    else
    {
        for (int i=0;i<MAX_NB_BUFFERS;i++)
        {
            if(mPendingBuffer[i] == (OMX_BUFFERHEADERTYPE_p)0)
            {
                mPendingBuffer[i] = buffer;
                break;
            }
        }
    }

}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer){
    buffer->nFlags = 0; //Reset buffer flag (needed for EOS handing)
    Component::deliverBuffer(OUTPUT_PORT, buffer);
}

