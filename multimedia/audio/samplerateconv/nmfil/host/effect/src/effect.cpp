/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   effect.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <samplerateconv/nmfil/host/effect.nmf>
#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>
#include "OMX_Core.h"

//#define SRC_TRACE_CONTROL

#ifdef SRC_TRACE_CONTROL
#include <stdio.h>
#endif

#define SATMAX_24BIT  8388607
#define SATMIN_24BIT -8388608


samplerateconv_nmfil_host_effect::samplerateconv_nmfil_host_effect() 
{
    mLowMipsToOut48 = 0;
    mHeap = NULL;
    mHeapSize = 0;
    mInBuf_p = NULL;
    mOutBuf_p = NULL;
    memset(&mResampleContext, 0 , sizeof(mResampleContext));
    memset(&m_config, 0 , sizeof(m_config));
}

samplerateconv_nmfil_host_effect::~samplerateconv_nmfil_host_effect()
{
}

t_bool METH(open)(const t_host_effect_config* config, t_effect_caps* caps)
{

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::open\n");
    printf("\tblock_size=%u\n", (unsigned int)config->block_size);
    printf("\tin.freq=%u\n", (unsigned int)config->infmt.freq);
    printf("\tin.#channels=%u\n", (unsigned int)config->infmt.nof_channels);
    printf("\tin.#bits/sample=%u\n", (unsigned int)config->infmt.nof_bits_per_sample);
    printf("\tout.freq=%u\n", (unsigned int)config->outfmt.freq);
    printf("\tout.#channels=%u\n", (unsigned int)config->outfmt.nof_channels);
    printf("\tout.#bits/sample=%u\n", (unsigned int)config->outfmt.nof_bits_per_sample);
#endif

    m_config = *config;

    ARMNMF_DBC_PRECONDITION(mHeap == NULL);
    ARMNMF_DBC_PRECONDITION(mHeapSize == 0);
    ARMNMF_DBC_PRECONDITION(mInBuf_p == NULL);
    ARMNMF_DBC_PRECONDITION(mOutBuf_p == NULL);
    ARMNMF_DBC_PRECONDITION(m_config.block_size > 0);
    ARMNMF_DBC_PRECONDITION((m_config.infmt.nof_channels > 0) && (m_config.infmt.nof_channels == m_config.outfmt.nof_channels));
    ARMNMF_DBC_PRECONDITION(((m_config.infmt.nof_bits_per_sample == 16) || (m_config.infmt.nof_bits_per_sample == 32)) && (m_config.infmt.nof_bits_per_sample == m_config.outfmt.nof_bits_per_sample));

   
    //---------- check assuming frequencies are unknown block_size not used for low latency with nsamples in msec -------------- 
    resample_calc_max_heap_size_fixin_fixout(0, 0,mLowMipsToOut48, 
											 &mHeapSize, 
											 m_config.block_size * m_config.infmt.nof_channels, 
											 m_config.infmt.nof_channels,0
											 );
    mHeap = new char[mHeapSize];

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::allocate HEAP %x\n",(unsigned int)mHeap);
#endif
		
    if (mHeap == 0) 
    {
        return false;
    }

    // Init src lib...
	// block_size not used for low latency with nsamples in msec
    int status = resample_x_init_ctx_low_mips_fixin_fixout(
        (char*)mHeap, mHeapSize, &mResampleContext, m_config.infmt.freq, 
        m_config.outfmt.freq, mLowMipsToOut48, m_config.infmt.nof_channels, m_config.block_size * m_config.infmt.nof_channels);

	if(status != 0)
	{
        delete [] mHeap;
        mHeap = NULL;
        return false;
	}
	
    caps->proc_type = EFFECT_PROCESS_TYPE_NOT_INPLACE;

	// 16 bits input/output: allocate temp buffers for 16/32 bits correction
	if (m_config.infmt.nof_bits_per_sample == 16)
	{
        unsigned int input_size, output_size;
	  
        if(mLowMipsToOut48==6)
        {
            // For low latency, block_size is in ms. 
            input_size  = m_config.block_size * m_config.infmt.nof_channels * mResampleContext.fin_khz;
            output_size = m_config.block_size * m_config.infmt.nof_channels * mResampleContext.fout_khz;
        }
        else
        {
            input_size  = m_config.block_size * m_config.infmt.nof_channels;
            output_size = m_config.block_size * m_config.infmt.nof_channels;
        }
	  
        mInBuf_p = new int[input_size];
        if (mInBuf_p == 0)
        {
            delete [] mHeap;
            mHeap = NULL;
            return false;
        }
        mOutBuf_p = new int[output_size];
        if (mOutBuf_p == 0)
        {
            delete [] mHeap;
            delete [] mInBuf_p;
            mInBuf_p = NULL;
            return false;
        }
	}
	
    return true;
}

void METH(getSampleSize)(t_uint32 *sampleIn,t_uint32 *sampleOut)
{
    *sampleIn = 32;
    *sampleOut = 32;
}

void METH(reset)(t_effect_reset_reason reason)
{

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::reset\n");
    printf("\treason=%d\n", (int)reason);
#endif

    // TODO: reset src lib, how?
    // TODO: Shall this function be called for all reset conditions or only open?
    /*int status = resample_x_init_ctx_low_mips_fixin_fixout(
      (char*)mHeap, mHeapSize, &mResampleContext, m_config.infmt.freq, 
      mOutputFreq, mLowMipsToOut48, mChannels, mBlockSize * mChannels);
      ARMNMF_DBC_ASSERT(status == 0);*/
}

void METH(process)(t_effect_process_params* params)
{
    t_effect_process_not_inplace_params* nip_params = (t_effect_process_not_inplace_params*)params;
    OMX_BUFFERHEADERTYPE* pInputBuf  = (OMX_BUFFERHEADERTYPE*)nip_params->inbuf_hdr;
    OMX_BUFFERHEADERTYPE* pOutputBuf = (OMX_BUFFERHEADERTYPE*)nip_params->outbuf_hdr;

    ARMNMF_DBC_PRECONDITION(pInputBuf->nOffset == 0);

    int mSamplesInMsec,mBlockSize,numtofill;
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    unsigned int i;

    int * inptr, *outptr;
    int itemp,j;

    mBlockSize=m_config.block_size;
    mSamplesInMsec=m_config.block_size;

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::process\n");
    printf("\tInput FilledLen=%d\n", (int)pInputBuf->nFilledLen);
    printf("\tOutput FilledLen=%d\n", (int)pOutputBuf->nFilledLen);
#endif

    if (m_config.infmt.freq == m_config.outfmt.freq) {
        // If equal frequencies then no SRC, just copy it assuming that buffers have equal size...
        memcpy(pOutputBuf->pBuffer, pInputBuf->pBuffer, pInputBuf->nFilledLen);
        // Forward applicable parts of buffer header...
        pOutputBuf->nFilledLen = pInputBuf->nFilledLen;
        pOutputBuf->nOffset = pInputBuf->nOffset;
        pOutputBuf->nTimeStamp = pInputBuf->nTimeStamp;
        pOutputBuf->nFlags = pInputBuf->nFlags;
        nip_params->inbuf_emptied = true;
        nip_params->outbuf_filled = true;
        return;
    }

    unsigned int nof_bytes_per_sample = (m_config.infmt.nof_bits_per_sample / 8);

    if (pInputBuf->nFlags & OMX_BUFFERFLAG_EOS) 
    {
        // SRC lib will read blocksize samples, 
        // so we must pad the buffer tail with zeros
        if (mLowMipsToOut48==6)
            numtofill=mSamplesInMsec*mResampleContext.fin_khz*m_config.infmt.nof_channels * nof_bytes_per_sample;
        else
            numtofill=mBlockSize*m_config.infmt.nof_channels * nof_bytes_per_sample;
        for (i = pInputBuf->nFilledLen; i < (unsigned int)numtofill; i++) 
        {
            pInputBuf->pBuffer[i] = 0;
        }
    }
    else 
    {
        if (mLowMipsToOut48==6)
        {
            ARMNMF_DBC_ASSERT(pInputBuf->nFilledLen == (mSamplesInMsec*mResampleContext.fin_khz*m_config.infmt.nof_channels*nof_bytes_per_sample));
        }
        else
        {
            ARMNMF_DBC_ASSERT(pInputBuf->nFilledLen == (mBlockSize*m_config.infmt.nof_channels*nof_bytes_per_sample));
        }
    }

    if (m_config.infmt.nof_bits_per_sample == 16) {
        inptr = mInBuf_p;
        outptr = mOutBuf_p;

        // feed the input buffer
        for (i = 0, j = 0 ; i < pInputBuf->nFilledLen; i += nof_bytes_per_sample, j++)
        {
            itemp=((int)(pInputBuf->pBuffer[i+1]))<<24;
            itemp|=((int)(pInputBuf->pBuffer[i]))<<16;
            itemp>>=8;
            mInBuf_p[j] = itemp;
        }
    } else {
        inptr = (int*)pInputBuf->pBuffer;
        outptr = (int*)pOutputBuf->pBuffer;
    }
    

    // perform SRC on each channel
    if (mLowMipsToOut48 == 6) {
        consumed = mBlockSize;
        produced = 0;
    } else {
        produced = consumed = mBlockSize;
    }

    flush = flush_in = (pInputBuf->nFlags & OMX_BUFFERFLAG_EOS) ? true : false;

#ifdef SRC_TRACE_CONTROL
    printf("Output buffer to algo is %x\n",(unsigned int)pOutputBuf->pBuffer);
#endif
    if((*mResampleContext.processing) (&mResampleContext, 
                                       inptr, 
                                       m_config.infmt.nof_channels, 
                                       &consumed, 
                                       outptr, 
                                       &produced, 
                                       &flush) != 0)
    {
        ARMNMF_DBC_ASSERT(FALSE);
    }

    if (mLowMipsToOut48 == 6)
    {
        if (!flush_in)
        {
            ARMNMF_DBC_POSTCONDITION(consumed == mSamplesInMsec);
            ARMNMF_DBC_POSTCONDITION(produced == mSamplesInMsec);
        }
        consumed *= mResampleContext.fin_khz;
        produced *= mResampleContext.fout_khz;
    }
    else
    {
        ARMNMF_DBC_POSTCONDITION((consumed == (int)mBlockSize) || (consumed == 0));
        ARMNMF_DBC_POSTCONDITION((produced == (int)mBlockSize) || (produced == 0));
    }
    
    if (flush_in && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        nip_params->inbuf_emptied = false;
    }
    else
    {
        if (mLowMipsToOut48==6)
        {
            if ((consumed == (int)mSamplesInMsec*(int)mResampleContext.fin_khz))
                nip_params->inbuf_emptied = true;
        }
        else
        {
            if ((consumed == (int)mBlockSize)|| flush) 
                nip_params->inbuf_emptied = true;
        }
    }

    if ((produced == (int)mBlockSize) || flush || (mLowMipsToOut48 == 6)) 
    {
        if (m_config.outfmt.nof_bits_per_sample == 16) {

            unsigned int noOfProducedSamples = produced*m_config.infmt.nof_channels;
            for (i = 0; i < noOfProducedSamples; i++)
            {
	            // deliver 16 bits output (Q23 -> Q15 samples), check saturation for 24-bit samples
                itemp=mOutBuf_p[i];
                itemp+=0x00000080;
                if (itemp > SATMAX_24BIT) itemp= SATMAX_24BIT;
                if (itemp < SATMIN_24BIT) itemp= SATMIN_24BIT;
                pOutputBuf->pBuffer[2*i+1]=(char)(itemp>>16);
                pOutputBuf->pBuffer[2*i]=(char)(itemp>>8);
            }
        }

        pOutputBuf->nFilledLen = produced * m_config.infmt.nof_channels * nof_bytes_per_sample;
        pOutputBuf->nOffset = pInputBuf->nOffset;
        pOutputBuf->nTimeStamp = pInputBuf->nTimeStamp;

        if (flush)
        {
            pOutputBuf->nFlags |= OMX_BUFFERFLAG_EOS;
        }

        nip_params->outbuf_filled = true;
    } 
}

void METH(close)(void)
{

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::close\n");
#endif

    if (mHeap) 
    {
#ifdef SRC_TRACE_CONTROL
        printf("samplerateconv_nmfil_host_effect::delete HEAP %x\n",(unsigned int)mHeap);
#endif
        delete [] mHeap; 
        mHeap = NULL;
    }

    mHeapSize = 0;

    if (mInBuf_p)  delete [] mInBuf_p; mInBuf_p = NULL;
    if (mOutBuf_p) delete [] mOutBuf_p; mOutBuf_p = NULL;

}

void METH(setParameter)(t_uint16 lowmips_to_out48)
{

#ifdef SRC_TRACE_CONTROL
    printf("samplerateconv_nmfil_host_effect::setParameter\n");
#endif

    mLowMipsToOut48     = lowmips_to_out48;
}
