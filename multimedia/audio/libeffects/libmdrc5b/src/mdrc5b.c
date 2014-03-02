/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**********************************************************************/
/**
  \file mdrc5b.c
  \brief system interfacing routines
  \author Zong Wenbo
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev DEC 7, 2009
*/
/*####################################################################*/
/* Copyright  STMicroelectonics, HED/HVD/AA&VoIP @ Singapore          */
/* This program is property of HDE/HVD/Audio Algorithms & VOIP        */
/* Singapore from STMicroelectronics.                                 */
/* It should not be communicated outside STMicroelectronics           */
/* without authorization.                                             */
/*####################################################################*/

#ifdef _NMF_MPC_
#include "libeffects/libmdrc5b.nmf"
#endif

#include "mdrc5b.h"
#include "mdrc5b_util.h"
#include "mdrc5b_compressor.h"
#include "mdrc5b_limiter.h"


#ifdef MDRC5B_DEBUG
long counter = 0;
#endif


static void mdrc5b_compressor_limiter(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    mdrc5b_apply_compressor(Heap);
    
    mdrc5b_apply_limiter(Heap);

    mdrc5b_move(Heap); // move processed data to output buffer
}

static void mdrc5b_compressor(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    mdrc5b_apply_compressor(Heap);

    mdrc5b_move(Heap); // move processed data to output buffer
}

static void mdrc5b_limiter(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    mdrc5b_apply_limiter(Heap);

    mdrc5b_move(Heap); // move processed data to output buffer
}

int applyEffectConfig(MDRC5B_LOCAL_STRUCT_T *Heap, Mdrc5bAlgoConfig_t *pConfig)
{
    int compressor = FALSE;
    int limiter    = FALSE;
    int reset;
    int bidx;
    int bands;

    reset = FALSE;
    bands = pConfig->NumBands;

    switch(Heap->SampleFreq)
    {
        case 48000:
        case 44100:
        case 32000:
            Heap->BlockSize = MDRC5B_BLOCK_SIZE;
            break;

        case 24000:
        case 22050:
            Heap->BlockSize = MDRC5B_BLOCK_SIZE_MSF;
            break;

        case 16000:
        case 12000:
        case 11025:
        case 8000:
            Heap->BlockSize = MDRC5B_BLOCK_SIZE_LSF;
            break;

        default:
            return -1;
    }

    // two types of change
    // 1. change in compressor/limiter settings: only need to recompute smoothing coeffs, etc.
    // 2. change in structure (e.g. cutoff freq, number of subbands): do a complete reset

#ifdef MDRC5B_COMPRESSOR_ACTIVE
    if(pConfig->Mode & COMPRESSOR)
    {
        if(bands > MDRC5B_SUBBAND_MAX)
        {
            bands = MDRC5B_SUBBAND_MAX;
        }
        else if(bands < 0)
        {
            bands = 0;
        }

        if(Heap->NumBands != bands)
        {
            Heap->NumBands = bands;
            reset = TRUE;
        }

        for(bidx = 0; bidx < Heap->NumBands; bidx++)
        {
            Heap->BandCompressors[bidx] = pConfig->BandCompressors[bidx];
        }

        mdrc5b_derive_compressor_params(Heap);

        if(reset)
        {
            mdrc5b_compute_subband_filters(Heap);
        }
    }
    if(((Heap->Mode & COMPRESSOR) == 0) && ((pConfig->Mode & COMPRESSOR) == COMPRESSOR))
    {
        mdrc5b_reset_subband_data(Heap);
    }
    compressor = ((pConfig->Mode & COMPRESSOR) == COMPRESSOR) && Heap->CompressorEnable && (Heap->NumBands > 0);
#endif


#ifdef MDRC5B_LIMITER_ACTIVE
    if(pConfig->Mode & LIMITER)
    {
        Heap->Limiter = pConfig->Limiter;
        mdrc5b_derive_limiter_params(Heap);
    }
    if(((Heap->Mode & LIMITER) == 0) && ((pConfig->Mode & LIMITER) == LIMITER))
    {
        mdrc5b_reset_limiter(Heap);
    }
    limiter = ((pConfig->Mode & LIMITER) == LIMITER) && Heap->Limiter.Enable;
#endif

    if(compressor)
    {
        if(limiter)
        {
            Heap->processing = mdrc5b_compressor_limiter;
        }
        else
        {
            Heap->processing = mdrc5b_compressor;
        }
    }
    else
    {
        if(limiter)
        {
            Heap->processing = mdrc5b_limiter;
        }
        else
        {
            Heap->processing = mdrc5b_move;
        }
    }

    Heap->OutWtIdx = Heap->BlockSize; // middle of buffer

    // Note: update this param after updating the components
    Heap->Mode = pConfig->Mode;

    return 0;
}


void mdrc5b_reset_io_buffers(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    int i, k;


    // reset internal buffers

    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        for(k = 0; k < MDRC5B_BLOCK_SIZE; k++)
        {
            Heap->MainInBuf [i][k] = 0;
            Heap->MainOutBuf[i][k] = 0;
        }
        for(k = MDRC5B_BLOCK_SIZE; k < MDRC5B_BLOCK_SIZE * 2; k++)
        {
            Heap->MainOutBuf[i][k] = 0;
        }
    }


    Heap->OutRdIdx  = 0;
    Heap->OutWtIdx  = Heap->BlockSize; // middle of buffer
    Heap->InBufSamp = 0;
}



void mdrc5b_default_settings(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    Heap->SampleFreq = 48000;
    Heap->BlockSize  = MDRC5B_BLOCK_SIZE;

    Heap->Mode       = 0;

    Heap->processing = mdrc5b_move;

#ifdef MDRC5B_COMPRESSOR_ACTIVE
    // default config of compressors and limiter
    Heap->CompressorEnable = FALSE; // clear the flag
    Heap->NumBands         = 1;
    Heap->MdrcLALen        = MDRC5B_LOOKAHEAD_LEN;

    // band 1
    Heap->BandCompressors[0].FreqCutoff            = 0;      // not used
    Heap->BandCompressors[0].PostGain              = 0;      // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].KneePoints            = 5;
    Heap->BandCompressors[0].DynamicResponse[0][0] = -9000;  // not used
    Heap->BandCompressors[0].DynamicResponse[0][1] = -9000;  // not used
    Heap->BandCompressors[0].DynamicResponse[1][0] = -7000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[1][1] = -8000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[2][0] = -6000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[2][1] = -6000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[3][0] = -2000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[3][1] = -2000;  // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[4][0] = 0;      // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].DynamicResponse[4][1] = -10;    // in millibel, i.e. 1/100 dB
    Heap->BandCompressors[0].AttackTime            = 4000;   // in microseconds
    Heap->BandCompressors[0].ReleaseTime           = 200000; // in microseconds
#endif

#ifdef MDRC5B_LIMITER_ACTIVE
    Heap->Limiter.Enable     = TRUE;
    Heap->LimiterLALen       = MDRC5B_LIMITER_LOOKAHEAD_LEN;
    Heap->Limiter.RmsMeasure = 0; // use amplitude by default
    Heap->Limiter.Thresh     = MDRC5B_LIMITER_THRESH_MB_DEFAULT; // in millibel, i.e. 1/100 dB
#endif
}

void mdrc5b_reset(MDRC5B_LOCAL_STRUCT_T * Heap)
{
    mdrc5b_init(Heap);
}

void mdrc5b_init(MDRC5B_LOCAL_STRUCT_T * Heap)
{
    // note the order of calling sequence

    mdrc5b_default_settings(Heap);

    mdrc5b_reset_io_buffers(Heap);

#ifdef MDRC5B_COMPRESSOR_ACTIVE
    mdrc5b_reset_subband_data(Heap);
    mdrc5b_compute_subband_filters(Heap);
    mdrc5b_derive_compressor_params(Heap);
#endif

#ifdef MDRC5B_LIMITER_ACTIVE
    mdrc5b_reset_limiter(Heap);
    mdrc5b_derive_limiter_params(Heap);
#endif
}


/****************************************************************
 *                  processing entry routine
 ****************************************************************/
void mdrc5b_main(MDRC5B_LOCAL_STRUCT_T *Heap, void *inbuf, void *outbuf, int size)
{
    int PcminValidSamples;
#ifndef MMDSP
    int ByteOffsetIn, ByteOffsetOut;
#endif // !MMDSP
    

#ifdef __arm__
    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_MAIN)
#endif // __arm__

    Heap->SysMainInPtr  = inbuf;
    Heap->SysMainOutPtr = outbuf;
#ifdef MMDSP
#ifndef __flexcc2__
    size /= sizeof(MMshort);        // useless for MMDSP : sizeof(MMshort)=1
#endif // !__flexcc2__
    PcminValidSamples = (size >> (Heap->NumMainCh - 1)) ;           // size/Heap->NumMainCh
#else // MMDSP
    if(Heap->sample_bit_size == 32)
    {
        PcminValidSamples = (size >> (Heap->NumMainCh - 1 + 2));    // size/Heap->NumMainCh/4 (4 bytes/sample)
        ByteOffsetIn      = (Heap->DataInOffset  << 2);
        ByteOffsetOut     = (Heap->DataOutOffset << 2);
    }
    else
    {
        PcminValidSamples = (size >> (Heap->NumMainCh - 1 + 1));    // size/Heap->NumMainCh/2 (2 bytes/sample)
        ByteOffsetIn      = (Heap->DataInOffset  << 1);
        ByteOffsetOut     = (Heap->DataOutOffset << 1);
    }
#endif // MMDSP

    while(PcminValidSamples > 0)
    {
        Heap->ConsumedSamples = 0;
        Heap->ValidSamples    = PcminValidSamples;

        // main processing steps
        mdrc5b_read(Heap);

        if(Heap->InBufSamp == Heap->BlockSize)
        {
#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)
            char string[200];

            sprintf(string, "\n*************** samples %d to %d ***************\n", debug_cpt_samples, debug_cpt_samples + Heap->BlockSize - 1);
            debug_write_string(string);
#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT

            (*Heap->processing)(Heap);

#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)
            debug_cpt_samples += Heap->BlockSize;
#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT
#ifdef MDRC5B_DEBUG
            counter += Heap->BlockSize;
#endif // MDRC5B_DEBUG
        }

        mdrc5b_write(Heap);

        // Update the sample counters
        PcminValidSamples -= Heap->ConsumedSamples;

        // Update system input and output pointers
#ifdef MMDSP
        Heap->SysMainInPtr  = (void *) (((MMshort *) Heap->SysMainInPtr)  + Heap->ConsumedSamples * Heap->DataInOffset);
        Heap->SysMainOutPtr = (void *) (((MMshort *) Heap->SysMainOutPtr) + Heap->ConsumedSamples * Heap->DataOutOffset);
#else // MMDSP
        Heap->SysMainInPtr  = (void *) (((char *) Heap->SysMainInPtr)  + Heap->ConsumedSamples * ByteOffsetIn);
        Heap->SysMainOutPtr = (void *) (((char *) Heap->SysMainOutPtr) + Heap->ConsumedSamples * ByteOffsetOut);
#endif // MMDSP
    }

#ifdef __arm__
    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_MAIN)
#endif // __arm__
}
