/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmout.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/bindings/shmpcm/shmout.nmf>

#include <stdbool.h>
#include <armnmf_dbc.h>

#include "bindings_optim.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_bindings_shmpcm_shmout_src_shmoutTraces.h"
#endif

//#define SHMPCMOUT_TRACE_CONTROL

#ifdef SHMPCMOUT_TRACE_CONTROL
#include <stdio.h>
#endif

#define SAMPLE_SIZE_OUT (mShmConfig.bitsPerSampleOut>>3)

#define SAMPLE_SIZE_IN (mShmConfig.bitsPerSampleIn>>3)

#define FREQ_TAB_SIZE 17

static const unsigned int  Freq[FREQ_TAB_SIZE] = {0, 192000, 176400, 128000, 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7200};


hst_bindings_shmpcm_shmout::hst_bindings_shmpcm_shmout() {

  bInputBufJustArrived = true;
  mBufIn = NULL;
  mDataConsumed = 0;
  mInitialTS = 0;
  mNbSamplesForTSComputation = 0;
  bPropagateTS = false;

  mShmConfig.nb_buffer = 0;
  mShmConfig.swap_bytes = NO_SWAP;
  mShmConfig.bitsPerSampleIn = 0;
  mShmConfig.bitsPerSampleOut = 0;
  mShmConfig.channelsIn = 0;
  mShmConfig.channelsOut = 0;
  mShmConfig.sampleFreq = FREQ_UNKNOWN;

  bFsmInitialized = false;
  for (int i=0; i < MAX_NB_BUFFERS; i++)
  {
    mPendingBuffer[i] = 0;
  }
}

void hst_bindings_shmpcm_shmout::copy_in16b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out16b_swap_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in16b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out16b_noswap_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in16b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out32b_swapout_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in16b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in16b_out32b_noswap_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in32b_out16b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out16b_swapout_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in32b_out16b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out16b_noswap_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in32b_out32b_swap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out32b_swap_optim((void *) in, (void *) out, size);
}

void hst_bindings_shmpcm_shmout::copy_in32b_out32b_noswap(t_sint16 *in, t_sint16 *out, unsigned int size)
{
    copy_in32b_out32b_noswap_optim((void *) in, (void *) out, size);
}


void hst_bindings_shmpcm_shmout::ReturnInputBuffer()
{
	OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmout::ReturnInputBuffer release input buffer");
    mPorts[INPUT_PORT].dequeueAndReturnBuffer();
    mDataConsumed = 0;
	bInputBufJustArrived = true;
}

void hst_bindings_shmpcm_shmout::ReturnOutputBuffer(OMX_BUFFERHEADERTYPE * bufOut)
{
	// update TimeStamp
    if (bPropagateTS)
    {
        if ((mShmConfig.sampleFreq > 0) &&
            (mShmConfig.sampleFreq < FREQ_TAB_SIZE) &&
            (mShmConfig.channelsOut))
        {
            bufOut->nTimeStamp = (mNbSamplesForTSComputation * 10000)/(Freq[mShmConfig.sampleFreq] * mShmConfig.channelsOut) * 100;
            bufOut->nTimeStamp += mInitialTS;
        }
        else
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: Shmpcmout::ReturnOutputBuffer Can not compute TimeStamp Invalide sample freq %d or channel number %d",mShmConfig.sampleFreq, mShmConfig.channelsOut);
            bufOut->nTimeStamp = 0;
        }
	    mNbSamplesForTSComputation += bufOut->nFilledLen/SAMPLE_SIZE_OUT;
        OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: Shmpcmout::ReturnOutputBuffer latency control (host_shmpcmout, output) (timestamp = 0x%x 0x%x us)", (int)(bufOut->nTimeStamp >> 32), (unsigned int)(bufOut->nTimeStamp & 0xffffffffu));
    }

    // EOS event ?
    if(bufOut->nFlags & OMX_BUFFERFLAG_EOS){
        OstTraceFiltInst0(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::ReturnOutputBuffer Send EOS on OUTPUT_PORT");
        proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut->nFlags);
    }

	OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmpcmout::ReturnOutputBuffer release output buffer");

    mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
}

void hst_bindings_shmpcm_shmout::process()
{
    OMX_BUFFERHEADERTYPE*    bufIn, *bufOut;
    t_sint32   offset, input_size, output_size;
    t_sint16 * inptr, * outptr;
    int nb_samples_out, nb_samples_in;;

#ifdef SHMPCMOUT_TRACE_CONTROL
    printf("hst_shmpcm_shmout::process\n");
#endif

    if(mPorts[INPUT_PORT].queuedBufferCount() == 0
       || mPorts[OUTPUT_PORT].queuedBufferCount() == 0) {
        return;
    }

    ARMNMF_DBC_ASSERT(mShmConfig.channelsIn == mShmConfig.channelsOut);

    bufIn  =  (OMX_BUFFERHEADERTYPE*) mPorts[INPUT_PORT].getBuffer(0);
    bufOut =  (OMX_BUFFERHEADERTYPE*) mPorts[OUTPUT_PORT].getBuffer(0);

    offset = (t_sint32)bufOut->nFilledLen;

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

    if (nb_samples_in == nb_samples_out) {

        //copy(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmout::Shmout_copy_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += nb_samples_in * SAMPLE_SIZE_OUT;
        bufOut->nFlags     |= bufIn->nFlags;

        ReturnInputBuffer();
        ReturnOutputBuffer(bufOut);
    }
    else if (nb_samples_in < nb_samples_out) {

        //copy(inptr, outptr, nb_samples_in);
        (*hst_bindings_shmpcm_shmout::Shmout_copy_routine) (inptr, outptr, nb_samples_in);

        bufOut->nFilledLen += nb_samples_in * SAMPLE_SIZE_OUT;

        if(bufIn->nFlags & OMX_BUFFERFLAG_EOS){
            bufOut->nFlags     = bufIn->nFlags;
            ReturnOutputBuffer(bufOut);
        }

        bufOut->nFlags |= (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME);

        ReturnInputBuffer();
    }
    else {

      //copy(inptr, outptr, nb_samples_out);
      (*hst_bindings_shmpcm_shmout::Shmout_copy_routine) (inptr, outptr, nb_samples_out);

      mDataConsumed += nb_samples_out * SAMPLE_SIZE_IN;
      bufOut->nFilledLen += nb_samples_out * SAMPLE_SIZE_OUT;

	  bufOut->nFlags |= (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME);
      if (bufIn->nFlags & OMX_BUFFERFLAG_STARTTIME) {
        bufIn->nFlags ^= OMX_BUFFERFLAG_STARTTIME; // reset STARTTIME flag
      }
      ReturnOutputBuffer(bufOut);

      if(mPorts[OUTPUT_PORT].queuedBufferCount() != 0){
	scheduleProcessEvent();
      }
    }
}

void hst_bindings_shmpcm_shmout::reset()
{
//  mBufIn->nFilledLen       = 0;
//   mBufIn->pBuffer          = NULL;
//   mBufIn->nFlags           = 0;
  bInputBufJustArrived      = true;
  mDataConsumed            = 0;
  mNbSamplesForTSComputation               = 0;
  bPropagateTS = false;
}

void hst_bindings_shmpcm_shmout::disablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_shmpcm_shmout::disablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}
void hst_bindings_shmpcm_shmout::enablePortIndication(t_uint32 portIdx) {
    OstTraceFiltInst0(TRACE_ERROR,"AFM_HOST: hst_bindings_shmpcm_shmout::enablePortIndication Forbidden !!!");
    ARMNMF_DBC_ASSERT(0);
}

void hst_bindings_shmpcm_shmout::flushPortIndication(t_uint32 portIdx) {

    OMX_BUFFERHEADERTYPE* bufIn, *bufOut;
    if(mPorts[INPUT_PORT].queuedBufferCount() != 0){
        bufIn  =  (OMX_BUFFERHEADERTYPE*) mPorts[INPUT_PORT].getBuffer(0);
        bufIn->nFilledLen = 0;
    }
    if(mPorts[OUTPUT_PORT].queuedBufferCount() != 0){
        bufOut  =  (OMX_BUFFERHEADERTYPE*) mPorts[OUTPUT_PORT].getBuffer(0);
        bufOut->nFilledLen = 0;
    }
    reset();

} ;

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
#ifdef SHMPCMOUT_TRACE_CONTROL
    printf("hst_shmpcm_shmout::fsminit\n");
#endif
    // trace init (mandatory before port init)
    if (initFsm.traceInfoAddr){
        setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }

    mPorts[INPUT_PORT].init(InputPort, true, false, 0, &mBufIn, 1,  &inputport,  INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);
    mPorts[OUTPUT_PORT].init(OutputPort, false, false, 0, 0, mShmConfig.nb_buffer, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

    init(2, mPorts, &proxy, &me, false);

    // queue fillthisbuffer received before fsminit (if any)
    bFsmInitialized = true;
    for(int i=0;i<MAX_NB_BUFFERS;i++)
    {
        if(mPendingBuffer[i] != (OMX_BUFFERHEADERTYPE_p)0)
        {
            mPorts[OUTPUT_PORT].queueBuffer(mPendingBuffer[i]);
            mPendingBuffer[i] = (OMX_BUFFERHEADERTYPE_p)0;
        }
    }
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    OstTraceInt0(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::setTunnelStatus should not be called : treatment done in controller only");
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(setParameter)(
        ShmPcmConfig_t config ,
        void *buffer,
        t_uint16 input_blocksize)
{
#ifdef SHMPCMOUT_TRACE_CONTROL
    printf("hst_shmpcm_shmout::setParameter\n");
    printf("\tnb_buffer=%u\n", (unsigned int)config.nb_buffer);
    printf("\tswap_bytes=%u\n", (unsigned int)config.swap_bytes);
    printf("\tbitsPerSampleIn=%u\n", (unsigned int)config.bitsPerSampleIn);
    printf("\tbitsPerSampleOut=%u\n", (unsigned int)config.bitsPerSampleOut);
    printf("\tchannelsIn =%u\n", (unsigned int)config.channelsIn);
    printf("\tchannelsOut =%u\n", (unsigned int)config.channelsOut);
    printf("\tsampleFreq =%d\n", (int)config.sampleFreq);
    printf("\tinput blocksize =%u\n", (unsigned int)input_blocksize);
#endif

    OstTraceInt3(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::setParameter nb_buffer=%d buffer=0x%x,, swap_bytes=%d", (unsigned int)config.nb_buffer,(unsigned int)buffer,(unsigned int)config.swap_bytes);
    OstTraceInt1(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::setParameter blocksize=%d", (unsigned int)input_blocksize);
    OstTraceInt2(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::setParameter bitsPerSampleIn=%d, bitsPerSampleOut=%d", (unsigned int)config.bitsPerSampleIn,(unsigned int)config.bitsPerSampleOut);
    OstTraceInt2(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::setParameter channelsIn=%d, channelsOut=%d", (unsigned int)config.channelsIn,(unsigned int)config.channelsOut);

    ARMNMF_DBC_PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);
    ARMNMF_DBC_PRECONDITION(config.channelsIn == config.channelsOut);

    mShmConfig = config;
    mBufIn = (OMX_BUFFERHEADERTYPE*) buffer;

    ARMNMF_DBC_ASSERT((mShmConfig.bitsPerSampleIn  == 16) || (mShmConfig.bitsPerSampleIn  == 32));
    ARMNMF_DBC_ASSERT((mShmConfig.bitsPerSampleOut == 16) || (mShmConfig.bitsPerSampleOut == 32));
    if(mShmConfig.bitsPerSampleIn == 16)
    {
        if(mShmConfig.bitsPerSampleOut == 16)
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in16b_out16b_swap;
            }
            else
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in16b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in16b_out32b_swap;
            }
            else
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in16b_out32b_noswap;
            }
        }
    }
    else
    {
        if(mShmConfig.bitsPerSampleOut == 16)
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in32b_out16b_swap;
            }
            else
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in32b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP)
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in32b_out32b_swap;
            }
            else
            {
                Shmout_copy_routine = &hst_bindings_shmpcm_shmout::copy_in32b_out32b_noswap;
            }
        }
    }
}


void METH(newFormat)(
	t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {

#ifdef SHMPCMOUT_TRACE_CONTROL
    printf("hst_shmpcm_shmout::newFormat\n");
    printf("\tfreq=%d\n", (int)sample_freq);
    printf("\t#channels=%u\n", (unsigned int)chans_nb);
    printf("\t#bits/sample=%u\n", (unsigned int)sample_size);
#endif

    OstTraceInt2(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::newFormat freq=%d, channels=%d", (unsigned int)sample_freq,(unsigned int)chans_nb);
    OstTraceInt1(TRACE_ALWAYS, "AFM_HOST: Shmpcmout::newFormat bits/sample=%d", (unsigned int)sample_size);


    ARMNMF_DBC_PRECONDITION(mShmConfig.sampleFreq == sample_freq);
    ARMNMF_DBC_PRECONDITION(mShmConfig.channelsIn == chans_nb);

    ARMNMF_DBC_PRECONDITION(mShmConfig.bitsPerSampleIn == sample_size);
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer){

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_HOST: Shmpcmout::emptyThisBuffer receive input buffer (nFilledLen=%d)",buffer->nFilledLen);
    OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: Shmpcmout::emptyThisBuffer latency control (host_shmpcmout, input) (timestamp = 0x%x 0x%x us)", (int)(buffer->nTimeStamp >> 32), (unsigned int)(buffer->nTimeStamp & 0xffffffffu));

    Component::deliverBuffer(INPUT_PORT, buffer);
}


void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)  {
    if(bFsmInitialized)
    {
        Component::deliverBuffer(OUTPUT_PORT, buffer);
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
