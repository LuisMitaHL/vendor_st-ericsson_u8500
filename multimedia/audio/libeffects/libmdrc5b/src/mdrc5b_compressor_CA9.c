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
  \file mdrc5b_compressor_CA9.c
  \brief processing routines of the multi-band DRC
  \authors Zong Wenbo, Champsaur Ludovic
  \email wenbo.zong@st.com, ludovic.champsaur@stericsson.com
  \date DEC 1,  2009
  \last rev DEC 18, 2009
  \last rev NEON DEC 2, 2011
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

#include "mdrc5b_options.h"

#ifdef MDRC5B_COMPRESSOR_ACTIVE

#include "mdrc5b_compressor.h"


static void mdrc5b_compressor_gain(MDRC5B_LOCAL_STRUCT_T *HeapPtr, MMlong *Rms);


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    compute the subband filters according to the cut-off freqs
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_compute_subband_filters(MDRC5B_LOCAL_STRUCT_T * Heap)
{
    // no legacy filter for CA9
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    derive the internal variables used by the limiter
            according to the user settings
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_derive_compressor_params(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    int bidx;
    int NumBands;
    int SampleFreq;
    MMshort Tmp, Tmp1, Tmp2, Tmp3;
    int KneePoints;
    int p;


    NumBands   = Heap->NumBands;
    SampleFreq = Heap->SampleFreq;

    Heap->CompressorEnable = FALSE; // clear the flag
    Heap->MdrcLALen        = (int) ((SampleFreq * MDRC5B_LOOKAHEAD_DURATION) / 1000); // msec

    for(bidx = 0; bidx < NumBands; bidx++)
    {
        Heap->CompressorEnable |= Heap->BandCompressors[bidx].Enable;

        // translate compression curve to internal representation
        Tmp = Heap->BandCompressors[bidx].PostGain;
        Tmp = (Tmp > MDRC5B_POSTGAIN_MAX ? MDRC5B_POSTGAIN_MAX : Tmp);
        Tmp = (Tmp < MDRC5B_POSTGAIN_MIN ? MDRC5B_POSTGAIN_MIN : Tmp);
        //Heap->ComPostGaindB[bidx] = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16
        Heap->ComPostGaindB[bidx] = (Tmp << 16) / 100; // result in Q16 : since abs(Tmp) <= 4500, (Tmp << 16) doesn't saturate in 32 bits

        //Heap->ComThreshdBOrig[bidx][0][0] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[0][0], Q23_ONE_HUNDREDTH), 16)); // result in Q16
        //Heap->ComThreshdBOrig[bidx][0][1] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[0][1], Q23_ONE_HUNDREDTH), 16)); // result in Q16
        Heap->ComThreshdBOrig[bidx][0][0] = (Heap->BandCompressors[bidx].DynamicResponse[0][0] << 16) / 100; // result in Q16 : since abs(Heap->BandCompressors[bidx].DynamicResponse[0][0]) <= 9000, (Heap->BandCompressors[bidx].DynamicResponse[0][0] << 16) doesn't saturate in 32 bits
        Heap->ComThreshdBOrig[bidx][0][1] = (Heap->BandCompressors[bidx].DynamicResponse[0][1] << 16) / 100; // result in Q16 : since abs(Heap->BandCompressors[bidx].DynamicResponse[0][1]) <= 9000, (Heap->BandCompressors[bidx].DynamicResponse[0][1] << 16) doesn't saturate in 32 bits

        Heap->ComThreshdB[bidx][0][0] = Heap->ComThreshdBOrig[bidx][0][0];
        Heap->ComThreshdB[bidx][0][1] = Heap->ComThreshdBOrig[bidx][0][1];

        /*
        //Heap->ComSlope[bidx][0] = (Heap->BandCompressors[bidx].DynamicResponse[1][1]/100.0 + 138.5)  \
        //                        / (Heap->BandCompressors[bidx].DynamicResponse[1][0]/100.0 + 138.5); // -138.5 dB for 24 bits
        */
        Tmp1 = Heap->BandCompressors[bidx].DynamicResponse[1][1];
        Tmp1 = (Tmp1 < MDRC5B_INPUT_MIN_MB ? MDRC5B_INPUT_MIN_MB : Tmp1);
        Tmp1 = (Tmp1 > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp1);
        Tmp  = Heap->BandCompressors[bidx].DynamicResponse[1][0];
        Tmp  = (Tmp  < MDRC5B_INPUT_MIN_MB ? MDRC5B_INPUT_MIN_MB : Tmp);
        Tmp  = (Tmp  > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp);
        //Heap->ComSlope[bidx][0] = wround_L(wL_msl(wL_msl((Tmp1 + 18660), 24)/(Tmp + 18660), 16)); // Q16
        Heap->ComSlope[bidx][0] = ((Tmp1 + 18660) << 16) / (Tmp + 18660);

#ifdef MDRC5B_DEBUG
        printf("comp %d, p 0, (%.2f, %.2f), slope=%.3f\n", bidx, (float)Heap->ComThreshdBOrig[bidx][0][0]/65536.0, (float)Heap->ComThreshdBOrig[bidx][0][1]/65536.0, (float)Heap->ComSlope[bidx][0]/65536.0);
#endif
        KneePoints = Heap->BandCompressors[bidx].KneePoints;
        for(p = KneePoints - 2; p >= 1; p--)
        {
            //Heap->ComThreshdBOrig[bidx][p][0] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[p][0], Q23_ONE_HUNDREDTH), 16)); // result in Q16
            //Heap->ComThreshdBOrig[bidx][p][1] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[p][1], Q23_ONE_HUNDREDTH), 16)); // result in Q16
            Heap->ComThreshdBOrig[bidx][p][0] = (Heap->BandCompressors[bidx].DynamicResponse[p][0] << 16) / 100; // result in Q16 : since abs(Heap->BandCompressors[bidx].DynamicResponse[p][0]) <= 9000, (Heap->BandCompressors[bidx].DynamicResponse[p][0] << 16) doesn't saturate in 32 bits
            Heap->ComThreshdBOrig[bidx][p][1] = (Heap->BandCompressors[bidx].DynamicResponse[p][1] << 16) / 100; // result in Q16 : since abs(Heap->BandCompressors[bidx].DynamicResponse[p][1]) <= 9000, (Heap->BandCompressors[bidx].DynamicResponse[p][1] << 16) doesn't saturate in 32 bits

            Heap->ComThreshdB[bidx][p][0] = Heap->ComThreshdBOrig[bidx][p][0];
            Heap->ComThreshdB[bidx][p][1] = Heap->ComThreshdBOrig[bidx][p][1];

            /*
            //Heap->ComSlope[bidx][p] = (Heap->BandCompressors[bidx].DynamicResponse[p+1][1]/100.0 - Heap->BandCompressors[bidx].DynamicResponse[p][1]/100.0) \
            //                        / MAX(1.0, Heap->BandCompressors[bidx].DynamicResponse[p+1][0]/100.0 - Heap->BandCompressors[bidx].DynamicResponse[p][0]/100.0);
            */
            Tmp3 = Heap->BandCompressors[bidx].DynamicResponse[p][1];
            Tmp3 = (Tmp3 < MDRC5B_INPUT_MIN_MB ? MDRC5B_INPUT_MIN_MB : Tmp3);
            Tmp3 = (Tmp3 > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp3);
            Tmp2 = Heap->BandCompressors[bidx].DynamicResponse[p][0];
            Tmp2 = (Tmp2 < MDRC5B_INPUT_MIN_MB ? MDRC5B_INPUT_MIN_MB : Tmp2);
            Tmp2 = (Tmp2 > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp2);
            Tmp1 = Heap->BandCompressors[bidx].DynamicResponse[p + 1][1];
            Tmp1 = (Tmp1 < MDRC5B_INPUT_MIN_MB ? MDRC5B_INPUT_MIN_MB : Tmp1);
            Tmp1 = (Tmp1 > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp1);
            Tmp  = Heap->BandCompressors[bidx].DynamicResponse[p + 1][0];
            Tmp  = (Tmp  < Tmp2 + 1 ? Tmp2 + 1 : Tmp); // to avoid divide-by-0
            Tmp  = (Tmp  > MDRC5B_INPUT_MAX_MB ? MDRC5B_INPUT_MAX_MB : Tmp);

            //Heap->ComSlope[bidx][p] = wround_L(wL_msl(wL_msl((Tmp1 - Tmp3), 24)/(Tmp - Tmp2), 16)); // Q16
            Heap->ComSlope[bidx][p] = ((Tmp1 - Tmp3) << 16) / (Tmp - Tmp2); // Q16
#ifdef MDRC5B_DEBUG
            printf("comp %d, p %d, (%.2f, %.2f), slope=%.3f\n", bidx, p, (float)Heap->ComThreshdBOrig[bidx][p][0]/65536.0, (float)Heap->ComThreshdBOrig[bidx][p][1]/65536.0, (float)Heap->ComSlope[bidx][p]/65536.0);
#endif
        }


        // translate time constants to filter coeffs
        Tmp = Heap->BandCompressors[bidx].AttackTime;
        Tmp = (Tmp > MDRC5B_COM_AT_TIME_MAX ? MDRC5B_COM_AT_TIME_MAX : Tmp);
        Tmp = (Tmp < MDRC5B_COM_AT_TIME_MIN ? MDRC5B_COM_AT_TIME_MIN : Tmp);

        // this is the implementation is floating-point
        //Samples = (float)SampleFreq * Tmp / 1000000.0; // Attack time in sampling periods (floating)
        //Heap->ComAtCoef[bidx] = pow(0.1, 1.0/Samples);
        Heap->ComAtCoef[bidx] = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) Tmp)) * 2147483648.0); // Q31

        // release coef
        Tmp = Heap->BandCompressors[bidx].ReleaseTime;
        Tmp = (Tmp > MDRC5B_COM_RE_TIME_MAX ? MDRC5B_COM_RE_TIME_MAX : Tmp);
        Tmp = (Tmp < MDRC5B_COM_RE_TIME_MIN ? MDRC5B_COM_RE_TIME_MIN : Tmp);
        Heap->ComReCoef[bidx] = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) Tmp)) * 2147483648.0); // Q31


        // hold samples
        Heap->HtSamp[bidx] = (int) (((MMlong) SampleFreq) * MDRC5B_HOLD_TIME / 1000000);     // hold time in samples


        // RMS estimation coef
        Heap->RmsAtCoef[bidx] = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_RMS_ATTACK_TIME)) * 2147483648.0); // Q31

        // release coef
        Heap->RmsReCoef[bidx] = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_RMS_RELEASE_TIME)) * 2147483648.0); // Q31

#ifdef MDRC5B_DEBUG
        printf("comp %d, ComAtCoef=%.6f, ComReCoef=%.6f, HtSamp=%d, RmsAtCoef=%.6f, RmsReCoef=%.6f\n",
               bidx,
               (float)Heap->ComAtCoef[bidx]/2147483648.0,
               (float)Heap->ComReCoef[bidx]/2147483648.0,
               Heap->HtSamp[bidx],
               (float)Heap->RmsAtCoef[bidx]/2147483648.0,
               (float)Heap->RmsReCoef[bidx]/2147483648.0);
#endif
    }
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    derive the internal variables/buffers
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_reset_subband_data(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    int i, j, k;


    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        switch(Heap->NumMainCh)
        {
            case 1:
                Heap->ChWeight[i] = 2147483647;    // Q31: 1
                break;
            case 2:
                Heap->ChWeight[i] = 1073741824;    // Q31: 0.5
                break;
            case 3:
                Heap->ChWeight[i] = 715827883;     // Q31: 1/3
                break;
            default:
                Heap->ChWeight[i] = 2147483647;    // Q31
                break;
        }
    }

    if(Heap->mdrc_filter_kind == MDRC_LEGACY_FILTER)
    {
        for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
        {
            for(j = 0; j < MDRC5B_SUBBAND_MAX - 1; j++)
            {
                for(k = 0; k < 8; k++)
                {
                    Heap->p_filter_mem->LPFiltHist[i][j][k] = 0;
                    Heap->p_filter_mem->HPFiltHist[i][j][k] = 0;
                }
            }
        }
    }
    else
    {
        reset_mdrc_filter(Heap->p_mdrc_filters);
    }

    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        for(j = 0; j < MDRC5B_SUBBAND_MAX; j++)
        {
            for(k = 0; k < MDRC5B_BLOCK_SIZE; k++)
            {
                Heap->p_filter_buffer->MainSubBuf[i][j][k] = 0;
            }
        }
    }
    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        for(j = 0; j < MDRC5B_SUBBAND_MAX; j++)
        {
            for (k=0; k<MDRC5B_LOOKAHEAD_LEN; k++)
            {
                Heap->MainSubLABuf[i][j][k] = 0;
            }
        }
    }


    Heap->LaIdx = 0;


    for(j = 0; j < MDRC5B_SUBBAND_MAX; j++)
    {
        Heap->ComGainMant[j]     = Q31; // linear gain
        Heap->ComGainExp[j]      = 0;
        Heap->MainSubRms[j]      = 0; // silent
        Heap->ComTargetGaindB[j] = 0;
        Heap->ComHoldRem[j]      = 0;
    }

    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        Heap->p_band_input[i] = (MMshort *) Heap->MainInBuf[i];

        for(j = 0; j < MDRC5B_SUBBAND_MAX; j++)
        {
            Heap->p_band_output[i][j] = Heap->p_filter_buffer->MainSubBuf[i][j];
        }
    }
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    main processing routine of the multi-band DRC.
            process exactly MDRC5B_BLOCK_SIZE samples
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_apply_compressor(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    unsigned int k, n, ch, bidx, Samples, LaIdx, NumBands, NumMainCh;
    MMshort      *MemInPtr;
    MMlong       *MemOutPtr;
    MMshort      *Ptr;

    MMshort      *Ptr2;
#ifndef IVC_LOUDER_CH_PRIORITY
    MMshort      Weight;
#endif // !IVC_LOUDER_CH_PRIORITY
    MMlong       MainSubSq[MDRC5B_SUBBAND_MAX][MDRC5B_BLOCK_SIZE];
    MMlong       *PtrX;
    MMlong       MainSubRmsMax[MDRC5B_SUBBAND_MAX];
    int          Exp, Exp_table[MDRC5B_SUBBAND_MAX];
    MMshort      TargetGain, TargetGain_table[MDRC5B_SUBBAND_MAX], GainThreshold;
    unsigned int HtSamp;
    unsigned int ComHoldRem;
    MMshort      ComGainMant[MDRC5B_SUBBAND_MAX][MDRC5B_BLOCK_SIZE + 1];
    int          ComGainExp[MDRC5B_SUBBAND_MAX], GainExp;
    MMshort      ComAtCoef;
    MMshort      ComReCoef;

    unsigned int cpt1, cpt2;
    int32x4x2_t  sample_32x4x2;
    int32x4_t    sample_32x4;
#ifdef IVC_LOUDER_CH_PRIORITY
    uint64x2_t   bsl_u64x2;
    int32x2_t    sample_32x2;
#else // IVC_LOUDER_CH_PRIORITY
    int32x4_t    Tmp_32x4;
#endif // IVC_LOUDER_CH_PRIORITY
    int64x2_t    MainSubSq1_64x2, MainSubSq2_64x2, Tmp_64x2;
    int64x1_t    MainSubSq_64x1;
    int32x2_t    RmsCoef_32x2;
    int64x1_t    diffX_64x1;
    int64x2_t    diffX_64x2;
    int32x2_t    diffX_low_32x2, diffX_high_32x2;
    int64x1_t    MainSubRms_64x1, MainSubRmsMax_64x1;
    uint64x1_t   bsl_u64x1;
    int32_t      *p_ComGainMant;
    int32x4_t    ComGainMant_32x4;
    int32x2_t    ComGainMant_32x2, TargetGain_32x2;
    int32x2_t    ComAtCoef_32x2, ComReCoef_32x2;
    int64x2_t    sample1_64x2, sample2_64x2, GainExp_64x2;
    int64x1_t    sample_64x1;
    int32x2_t    Tmp_32x2;


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_APPLY_COMPRESSOR)

    Samples   = (unsigned int) HeapPtr->BlockSize;
    NumBands  = (unsigned int) HeapPtr->NumBands;
    NumMainCh = (unsigned int) HeapPtr->NumMainCh;

    if(NumBands <= 1)
    {
        // no need to filter
        for(ch = 0; ch < NumMainCh; ch++)
        {
            // MainInBuf is stored in MMlong but at this stage of algorithm,
            // only 32 least-significant bit are significant
            // (see mdrc5b_read() routine which fills MainInBuf).
            // Thus we can access to this data thru a MMshort pointer (with increment of 2).
            Ptr      = HeapPtr->p_filter_buffer->MainSubBuf[ch][0];
            MemInPtr = (MMshort *) HeapPtr->MainInBuf[ch];
            for(k = (Samples >> 2); k > 0; k--)
            {
                sample_32x4x2 = vld2q_s32(MemInPtr);
                MemInPtr     += 8;
                vst1q_s32(Ptr, sample_32x4x2.val[0]);
                Ptr          += 4;
            }
            k = (Samples & 3);
            if(k > 0)
            {
                sample_32x4x2 = vld2q_s32(MemInPtr);
                switch(k)
                {
                    case 3:
                        vst1q_lane_s32(Ptr + 2, sample_32x4x2.val[0], 2);
                    case 2:
                        vst1_s32(Ptr, vget_low_s32(sample_32x4x2.val[0]));
                        break;
                    case 1:
                        vst1q_lane_s32(Ptr, sample_32x4x2.val[0], 0);
                    default:
                        break;
                }
            }
        }
    }
    else if(HeapPtr->mdrc_filter_kind == MDRC_LEGACY_FILTER)
    {
        START_PMU_MEASURE(PMU_MEASURE_MRDC5B_LEGACY_FILTER)

        // no legacy filter for CA9
        assert(0);

        STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_LEGACY_FILTER)
    }
    else
    {
        START_PMU_MEASURE(HeapPtr->mdrc_filter_kind == MDRC_BIQUAD_FILTER ? PMU_MEASURE_MRDC5B_BIQUAD_FILTER : PMU_MEASURE_MRDC5B_FIR_FILTER)

        (*HeapPtr->p_mdrc_filters->p_sub_bands_filtering) (HeapPtr->p_mdrc_filters,
                                                           HeapPtr->p_band_input,
                                                           HeapPtr->p_band_output,
                                                           Samples);

        STOP_PMU_MEASURE(HeapPtr->mdrc_filter_kind == MDRC_BIQUAD_FILTER ? PMU_MEASURE_MRDC5B_BIQUAD_FILTER : PMU_MEASURE_MRDC5B_FIR_FILTER)
    }

#ifdef DEBUG_FILTERS_OUTPUT
    // Debug sub-band filters output
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        debug_write_string("MRDC5B_FILTER\n");
        for(k = 0; k < Samples; k++)
        {
            for(ch = 0; ch < NumMainCh; ch++)
            {
                debug_write_string("| ");
                for(bidx = 0; bidx < NumBands; bidx++)
                {
                    debug_write_filter_output(ch, bidx, HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx][k]);
                }
            }
            debug_write_string("|\n");
        }
    }
#endif // DEBUG_FILTERS_OUTPUT

    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_INSERT_NEW_SUBBAND)

    // insert the new subband samples into the lookahead buffers
    LaIdx = HeapPtr->LaIdx;
    if(LaIdx + Samples >= HeapPtr->MdrcLALen)
    {
        cpt1           = HeapPtr->MdrcLALen - LaIdx;
        cpt2           = Samples - cpt1;
        // update index
        HeapPtr->LaIdx = cpt2;
    }
    else
    {
        cpt1           = Samples;
        cpt2           = 0;
        // update index
        HeapPtr->LaIdx = LaIdx + Samples;
    }
    for(ch = 0; ch < NumMainCh; ch++)
    {
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            Ptr  =  HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx];
            Ptr2 = &HeapPtr->MainSubLABuf[ch][bidx][LaIdx];  // go to the first valid sample
            for(k = (cpt1 >> 2); k > 0; k--)
            {
                sample_32x4 = vld1q_s32(Ptr);
                Ptr        += 4;
                vst1q_s32(Ptr2, sample_32x4);
                Ptr2       += 4;
            }
            k = (cpt1 & 3);
            if(k > 0)
            {
                sample_32x4 = vld1q_s32(Ptr);
                switch(k)
                {
                    case 3:
                        vst1q_lane_s32(Ptr2 + 2, sample_32x4, 2);
                    case 2:
                        vst1_s32(Ptr2, vget_low_s32(sample_32x4));
                        break;
                    case 1:
                        vst1q_lane_s32(Ptr2, sample_32x4, 0);
                    default:
                        break;
                }
                Ptr += k;
            }
            if(cpt2 > 0)
            {
                Ptr2 = HeapPtr->MainSubLABuf[ch][bidx];
                for(k = (cpt2 >> 2); k > 0; k--)
                {
                    sample_32x4 = vld1q_s32(Ptr);
                    Ptr        += 4;
                    vst1q_s32(Ptr2, sample_32x4);
                    Ptr2       += 4;
                }
                k = (cpt2 & 3);
                if(k > 0)
                {
                    sample_32x4 = vld1q_s32(Ptr);
                    switch(k)
                    {
                        case 3:
                            vst1q_lane_s32(Ptr2 + 2, sample_32x4, 2);
                        case 2:
                            vst1_s32(Ptr2, vget_low_s32(sample_32x4));
                            break;
                        case 1:
                            vst1q_lane_s32(Ptr2, sample_32x4, 0);
                        default:
                            break;
                    }
                }
            }
        }
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_INSERT_NEW_SUBBAND)


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_COMPUTE_POWER)

    // compute power
#ifndef IVC_LOUDER_CH_PRIORITY
    Weight = HeapPtr->ChWeight[0];
#endif // !IVC_LOUDER_CH_PRIORITY
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        Ptr  = HeapPtr->p_filter_buffer->MainSubBuf[0][bidx];
        PtrX = MainSubSq[bidx];

        // square and weight
        for(k = (Samples >> 2); k > 0; k--)
        {
            sample_32x4     = vld1q_s32(Ptr);
            Ptr            += 4;
#ifdef IVC_LOUDER_CH_PRIORITY
            MainSubSq1_64x2 = vqdmull_s32(vget_low_s32 (sample_32x4), vget_low_s32 (sample_32x4));
            MainSubSq2_64x2 = vqdmull_s32(vget_high_s32(sample_32x4), vget_high_s32(sample_32x4));
#else // IVC_LOUDER_CH_PRIORITY
            Tmp_32x4        = vqrdmulhq_n_s32(sample_32x4, Weight);
            MainSubSq1_64x2 = vqdmull_s32(vget_low_s32 (sample_32x4), vget_low_s32 (Tmp_32x4));
            MainSubSq2_64x2 = vqdmull_s32(vget_high_s32(sample_32x4), vget_high_s32(Tmp_32x4));
#endif // IVC_LOUDER_CH_PRIORITY
            vst1q_s64(PtrX, MainSubSq1_64x2);
            PtrX += 2;
            vst1q_s64(PtrX, MainSubSq2_64x2);
            PtrX += 2;
        }

        k = (Samples & 3);
        if(k > 0)
        {
            sample_32x4     = vld1q_s32(Ptr);
#ifdef IVC_LOUDER_CH_PRIORITY
            MainSubSq1_64x2 = vqdmull_s32(vget_low_s32(sample_32x4), vget_low_s32(sample_32x4));
#else // IVC_LOUDER_CH_PRIORITY
            Tmp_32x4        = vqrdmulhq_n_s32(sample_32x4, Weight);
            MainSubSq1_64x2 = vqdmull_s32(vget_low_s32(sample_32x4), vget_low_s32(Tmp_32x4));
#endif // IVC_LOUDER_CH_PRIORITY
            switch(k)
            {
                case 3:
#ifdef IVC_LOUDER_CH_PRIORITY
                    MainSubSq2_64x2 = vqdmull_s32(vget_high_s32(sample_32x4), vget_high_s32(sample_32x4));
#else // IVC_LOUDER_CH_PRIORITY
                    MainSubSq2_64x2 = vqdmull_s32(vget_high_s32(sample_32x4), vget_high_s32(Tmp_32x4));
#endif // IVC_LOUDER_CH_PRIORITY
                    vst1q_lane_s64(PtrX + 2, MainSubSq2_64x2, 0);
                case 2:
                    vst1q_s64(PtrX, MainSubSq1_64x2);
                    break;
                case 1:
                    vst1q_lane_s64(PtrX, MainSubSq1_64x2, 0);
                default:
                    break;
            }
        }
    }

    for(ch = 1; ch < NumMainCh; ch++)
    {
#ifndef IVC_LOUDER_CH_PRIORITY
        Weight = HeapPtr->ChWeight[ch];
#endif // !IVC_LOUDER_CH_PRIORITY
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            Ptr  = HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx];
            PtrX = MainSubSq[bidx];

            // square and weight
            for(k = (Samples >> 1); k > 0; k--)
            {
#ifdef IVC_LOUDER_CH_PRIORITY
                MainSubSq1_64x2 = vld1q_s64(PtrX);
                sample_32x2     = vld1_s32(Ptr);
                Ptr            += 2;
                MainSubSq2_64x2 = vqdmull_s32(sample_32x2, sample_32x2);
                diffX_64x2      = vqsubq_s64(MainSubSq1_64x2, MainSubSq2_64x2);
                bsl_u64x2       = vreinterpretq_u64_s64(vshrq_n_s64(diffX_64x2, 64));       // sign(MainSubSq1_64x2 - MainSubSq2_64x2)
                MainSubSq1_64x2 = vbslq_s64(bsl_u64x2, MainSubSq2_64x2, MainSubSq1_64x2);   // max(MainSubSq1_64x2, MainSubSq2_64x2)
                vst1q_s64(PtrX, MainSubSq1_64x2);
                PtrX += 2;
#else // IVC_LOUDER_CH_PRIORITY
                sample_32x2     = vld1_s32(Ptr);
                Ptr            += 2;
                MainSubSq1_64x2 = vld1q_s64(PtrX);
                Tmp_32x2        = vqrdmulh_n_s32(sample_32x2, Weight);
                MainSubSq1_64x2 = vqdmlal_s32(MainSubSq1_64x2, sample_32x2, Tmp_32x2);
                vst1q_s64(PtrX, MainSubSq1_64x2);
                PtrX += 2;
#endif // IVC_LOUDER_CH_PRIORITY
            }

            if(Samples & 1)
            {
#ifdef IVC_LOUDER_CH_PRIORITY
                MainSubSq_64x1  = vld1_s64(PtrX);
                sample_32x2     = vld1_s32(Ptr);
                MainSubSq2_64x2 = vqdmull_s32(sample_32x2, sample_32x2);
                diffX_64x1      = vqsub_s64(MainSubSq_64x1, vget_low_s64(MainSubSq2_64x2));
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 64));                     // sign(MainSubSq_64x1 - vget_low_s64(MainSubSq2_64x2))
                MainSubSq_64x1  = vbsl_s64(bsl_u64x1, vget_low_s64(MainSubSq2_64x2), MainSubSq_64x1);   // max(MainSubSq_64x1, vget_low_s64(MainSubSq2_64x2))
                vst1_lane_s64(PtrX, MainSubSq_64x1, 0);
#else // IVC_LOUDER_CH_PRIORITY
                sample_32x2     = vld1_s32(Ptr);
                MainSubSq1_64x2 = vld1q_s64(PtrX);
                Tmp_32x2        = vqrdmulh_n_s32(sample_32x2, Weight);
                MainSubSq1_64x2 = vqdmlal_s32(MainSubSq1_64x2, sample_32x2, Tmp_32x2);
                vst1q_lane_s64(PtrX, MainSubSq1_64x2, 0);
#endif // IVC_LOUDER_CH_PRIORITY
            }
        }
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_COMPUTE_POWER)
#ifdef DEBUG_COMPRESSOR_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_COMPRESSOR_COMPUTE_POWER\n");
        for(k = 0; k < Samples; k++)
        {
            for(bidx = 0; bidx < NumBands; bidx++)
            {
#ifdef SAMPLES_24_BITS
                sprintf(string, "0x%012llX ", MainSubSq[bidx][k] & 0xFFFFFFFFFFFFLL);
#else // SAMPLES_24_BITS
                sprintf(string, "0x%012llX ", (MainSubSq[bidx][k] >> 16) & 0xFFFFFFFFFFFFLL);
#endif // SAMPLES_24_BITS
                debug_write_string(string);
            }
            debug_write_string("\n");
        }
    }
#endif // DEBUG_COMPRESSOR_OUTPUT


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_AND_MAX)

    // smooth and max()
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        PtrX               = MainSubSq[bidx];
        MainSubRms_64x1    = vdup_n_s64(HeapPtr->MainSubRms[bidx]);
        RmsCoef_32x2       = vdup_n_s32(HeapPtr->RmsAtCoef[bidx]);                                      // RmsAtCoef, RmsAtCoef
        RmsCoef_32x2       = vset_lane_s32(HeapPtr->RmsReCoef[bidx], RmsCoef_32x2, 1);                  // RmsAtCoef, RmsReCoef
        MainSubRmsMax_64x1 = vdup_n_s64(0);

        for(k = Samples; k > 0; k--)
        {
            MainSubSq_64x1     = vld1_s64(PtrX);
            PtrX++;
            diffX_64x1         = vqsub_s64(MainSubRms_64x1, MainSubSq_64x1);
            diffX_64x2         = vcombine_s64(diffX_64x1, diffX_64x1);
            diffX_low_32x2     = vshrn_n_s64(vshlq_n_s64(diffX_64x2, 32), 32);                          // wextract_l(diffX), wextract_l(diffX)
            diffX_high_32x2    = vrshrn_n_s64(diffX_64x2, 32);                                          // wround_L(diffX), wround_L(diffX)
            Tmp_64x2           = vmovl_s32(vqrdmulh_s32(diffX_low_32x2, RmsCoef_32x2));                 // (MMlong) wfmulr(wextract_l(diffX), RmsAtCoef), (MMlong) wfmulr(wextract_l(diffX), RmsReCoef)
            Tmp_64x2           = vqdmlal_s32(Tmp_64x2, diffX_high_32x2, RmsCoef_32x2);                  // wL_addsat((MMlong) wfmulr(wextract_l(diffX), RmsAtCoef), wL_fmul(wround_L(diffX), RmsAtCoef)), wL_addsat((MMlong) wfmulr(wextract_l(diffX), RmsReCoef), wL_fmul(wround_L(diffX), RmsReCoef))
            bsl_u64x1          = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 64));                      // bsl_u64x1 = sign(diffX_64x1)
            MainSubRms_64x1    = vbsl_s64(bsl_u64x1, vget_low_s64(Tmp_64x2), vget_high_s64(Tmp_64x2));  // diffX_64x1 < 0 ? wL_addsat((MMlong) wfmulr(wextract_l(diffX), RmsAtCoef), wL_fmul(wround_L(diffX), RmsAtCoef)) : wL_addsat((MMlong) wfmulr(wextract_l(diffX), RmsReCoef), wL_fmul(wround_L(diffX), RmsReCoef))
            MainSubRms_64x1    = vqadd_s64(MainSubSq_64x1, MainSubRms_64x1);
            diffX_64x1         = vqsub_s64(MainSubRmsMax_64x1, MainSubRms_64x1);
            bsl_u64x1          = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 64));                      // bsl_u64x1 = sign(MainSubRmsMax_64x1 - MainSubRms_64x1)
            MainSubRmsMax_64x1 = vbsl_s64(bsl_u64x1, MainSubRms_64x1, MainSubRmsMax_64x1);              // MainSubRmsMax_64x1 = MAX(MainSubRms_64x1, MainSubRmsMax_64x1)
        }
        MainSubRmsMax[bidx]       = vget_lane_s64(MainSubRmsMax_64x1, 0);
        HeapPtr->MainSubRms[bidx] = vget_lane_s64(MainSubRms_64x1,    0);                               // save history
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_AND_MAX)
#ifdef DEBUG_COMPRESSOR_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_COMPRESSOR_SMOOTH_AND_MAX\n");
        for(bidx = 0; bidx < NumBands; bidx++)
        {
#ifdef SAMPLES_24_BITS
            sprintf(string, "0x%012llX ", HeapPtr->MainSubRms[bidx] & 0xFFFFFFFFFFFFLL);
#else // SAMPLES_24_BITS
            sprintf(string, "0x%012llX ", (HeapPtr->MainSubRms[bidx] >> 16) & 0xFFFFFFFFFFFFLL);
#endif // SAMPLES_24_BITS
            debug_write_string(string);
        }
        debug_write_string("\n");
        for(bidx = 0; bidx < NumBands; bidx++)
        {
#ifdef SAMPLES_24_BITS
            sprintf(string, "0x%012llX ", MainSubRmsMax[bidx] & 0xFFFFFFFFFFFFLL);
#else // SAMPLES_24_BITS
            sprintf(string, "0x%012llX ", (MainSubRmsMax[bidx] >> 16) & 0xFFFFFFFFFFFFLL);
#endif // SAMPLES_24_BITS
            debug_write_string(string);
        }
        debug_write_string("\n");
    }
#endif // DEBUG_COMPRESSOR_OUTPUT


    // compressor

    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_GAIN)

    mdrc5b_compressor_gain(HeapPtr, MainSubRmsMax);

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_GAIN)
#ifdef DEBUG_COMPRESSOR_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_COMPRESSOR_GAIN\n");
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            sprintf(string, "0x%06X ", HeapPtr->ComTargetGaindB[bidx] & 0xFFFFFF);
            debug_write_string(string);
        }
        debug_write_string("\n");
    }
#endif // DEBUG_COMPRESSOR_OUTPUT


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_GAIN)

    // smooth gains
    {
        // WARNING : this optimization works only for MDRC5B_SUBBAND_MAX = 5
        // if MDRC5B_SUBBAND_MAX is modified, optimization must be modified accordlingly

        // linear_gain = 10^(gain_dB/20)
        //             = 2^gain_pow2
        //
        // with gain_pow2 = log2(linear_gain)
        //                = log10(linear_gain)/log10(2)
        //                = gain_dB/(20*log10(2))
        int32x4_t  ComTargetGaindB_32x4, ComPostGaindB_32x4, Gain_dB_32x4, TargetGain_32x4, Exp_32x4, Coef_32x4, One_32x4, Zero_32x4, Q31_32x4, x_32x4, xn_32x4;
        int32x2_t  ComTargetGaindB_32x2, ComPostGaindB_32x2, Gain_dB_32x2, TargetGain_32x2, Exp_32x2, Coef_32x2, One_32x2, Zero_32x2, Q31_32x2, x_32x2, xn_32x2;
        int32_t    *p_ComTargetGaindB, *p_ComPostGaindB;
        uint32x4_t bsl_u32x4;
        uint32x2_t bsl_u32x2;

        Coef_32x4            = vdupq_n_s32(0x1542A5A1); // 1.0/(20*log10(2)) in Q31
        One_32x4             = vdupq_n_s32(1);
        Coef_32x2            = vget_low_s32(Coef_32x4);
        One_32x2             = vget_low_s32(One_32x4);

        p_ComTargetGaindB    = HeapPtr->ComTargetGaindB;
        p_ComPostGaindB      = HeapPtr->ComPostGaindB;

        ComTargetGaindB_32x4 = vld1q_s32(p_ComTargetGaindB);
        p_ComTargetGaindB   += 4;
        ComPostGaindB_32x4   = vld1q_s32(p_ComPostGaindB);
        p_ComPostGaindB     += 4;
        Gain_dB_32x4         = vqaddq_s32(ComTargetGaindB_32x4, ComPostGaindB_32x4);
        Gain_dB_32x4         = vqrdmulhq_s32(Gain_dB_32x4, Coef_32x4);
        Exp_32x4             = vshrq_n_s32(Gain_dB_32x4, 16);
        Exp_32x4             = vaddq_s32(Exp_32x4, One_32x4);
        Gain_dB_32x4         = vsubq_s32(Gain_dB_32x4, vshlq_n_s32(Exp_32x4, 16));

        ComTargetGaindB_32x2 = vld1_s32(p_ComTargetGaindB);
        ComPostGaindB_32x2   = vld1_s32(p_ComPostGaindB);
        Gain_dB_32x2         = vqadd_s32(ComTargetGaindB_32x2, ComPostGaindB_32x2);
        Gain_dB_32x2         = vqrdmulh_s32(Gain_dB_32x2, Coef_32x2);
        Exp_32x2             = vshr_n_s32(Gain_dB_32x2, 16);
        Exp_32x2             = vadd_s32(Exp_32x2, One_32x2);
        Gain_dB_32x2         = vsub_s32(Gain_dB_32x2, vshl_n_s32(Exp_32x2, 16));

        // -0.5 < Gain_dB < 0 in Q16
        // 2^Gain_dB = exp(Gain_dB*ln(2))
        // exp(x) = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040 + ...    accuracy OK if |x| < 0.1
        // -0.5*ln(2) < Gain_dB*ln(2) < 0 => |Gain_dB*ln(2)/4| < 0.1
        // exp(x) = (exp(x/4))^4 => 2^Gain_dB = (1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040)^4 with x=Gain_dB*ln(2)/4 with good accuracy

        Coef_32x4            = vdupq_n_s32(0x58B90BFC);                                         // ln(2) in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        x_32x4               = vqrdmulhq_s32(Gain_dB_32x4, Coef_32x4);                          // Gain_dB*ln(2) in Q16, i.e. Gain_dB*ln(2)/4 in Q18
        x_32x2               = vqrdmulh_s32 (Gain_dB_32x2, Coef_32x2);                          //

        x_32x4               = vshlq_n_s32(x_32x4, 13);                                         // x = Gain_dB*ln(2)/4 in Q31
        x_32x2               = vshl_n_s32 (x_32x2, 13);                                         //

        TargetGain_32x4      = vdupq_n_s32(Q31);                                                // TargetGain = 1
        TargetGain_32x2      = vget_low_s32(TargetGain_32x4);                                   //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, x_32x4);                             // TargetGain = 1 + x
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, x_32x2);                             //

        xn_32x4              = vqrdmulhq_s32(x_32x4, x_32x4);                                   // xn = x^2
        xn_32x2              = vqrdmulh_s32 (x_32x2, x_32x2);                                   //

        TargetGain_32x4      = vrsraq_n_s32(TargetGain_32x4, xn_32x4, 1);                       // TargetGain = 1 + x + x^2/2
        TargetGain_32x2      = vrsra_n_s32 (TargetGain_32x2, xn_32x2, 1);                       //

        xn_32x4              = vqrdmulhq_s32(xn_32x4, x_32x4);                                  // xn = x^3
        xn_32x2              = vqrdmulh_s32 (xn_32x2, x_32x2);                                  //

        Coef_32x4            = vdupq_n_s32(0x15555555);                                         // 1/6 in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, vqrdmulhq_s32(xn_32x4, Coef_32x4));  // TargetGain = 1 + x + x^2/2 + x^3/6
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, vqrdmulh_s32 (xn_32x2, Coef_32x2));  //

        xn_32x4              = vqrdmulhq_s32(xn_32x4, x_32x4);                                  // xn = x^4
        xn_32x2              = vqrdmulh_s32 (xn_32x2, x_32x2);                                  //

        Coef_32x4            = vdupq_n_s32(0x05555555);                                         // 1/24 in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, vqrdmulhq_s32(xn_32x4, Coef_32x4));  // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, vqrdmulh_s32 (xn_32x2, Coef_32x2));  //

        xn_32x4              = vqrdmulhq_s32(xn_32x4, x_32x4);                                  // xn = x^5
        xn_32x2              = vqrdmulh_s32 (xn_32x2, x_32x2);                                  //

        Coef_32x4            = vdupq_n_s32(0x01111111);                                         // 1/120 in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, vqrdmulhq_s32(xn_32x4, Coef_32x4));  // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, vqrdmulh_s32 (xn_32x2, Coef_32x2));  //

        xn_32x4              = vqrdmulhq_s32(xn_32x4, x_32x4);                                  // xn = x^6
        xn_32x2              = vqrdmulh_s32 (xn_32x2, x_32x2);                                  //

        Coef_32x4            = vdupq_n_s32(0x002D82D8);                                         // 1/720 in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, vqrdmulhq_s32(xn_32x4, Coef_32x4));  // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, vqrdmulh_s32 (xn_32x2, Coef_32x2));  //

        xn_32x4              = vqrdmulhq_s32(xn_32x4, x_32x4);                                  // xn = x^7
        xn_32x2              = vqrdmulh_s32 (xn_32x2, x_32x2);                                  //

        Coef_32x4            = vdupq_n_s32(0x00068068);                                         // 1/5040 in Q31
        Coef_32x2            = vget_low_s32(Coef_32x4);                                         //

        TargetGain_32x4      = vqaddq_s32(TargetGain_32x4, vqrdmulhq_s32(xn_32x4, Coef_32x4));  // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040
        TargetGain_32x2      = vqadd_s32 (TargetGain_32x2, vqrdmulh_s32 (xn_32x2, Coef_32x2));  //

        TargetGain_32x4      = vqrdmulhq_s32(TargetGain_32x4, TargetGain_32x4);                 // TargetGain = (1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040)^4
        TargetGain_32x2      = vqrdmulh_s32 (TargetGain_32x2, TargetGain_32x2);                 //
        TargetGain_32x4      = vqrdmulhq_s32(TargetGain_32x4, TargetGain_32x4);                 //
        TargetGain_32x2      = vqrdmulh_s32 (TargetGain_32x2, TargetGain_32x2);                 //

        bsl_u32x4            = vdupq_n_u32   (HeapPtr->BandCompressors[0].Enable);
        bsl_u32x4            = vsetq_lane_u32(HeapPtr->BandCompressors[1].Enable, bsl_u32x4, 1);
        bsl_u32x4            = vsetq_lane_u32(HeapPtr->BandCompressors[2].Enable, bsl_u32x4, 2);
        bsl_u32x4            = vsetq_lane_u32(HeapPtr->BandCompressors[3].Enable, bsl_u32x4, 3);
        bsl_u32x2            = vdup_n_u32    (HeapPtr->BandCompressors[4].Enable);
        Zero_32x4            = vdupq_n_s32(0);
        Q31_32x4             = vdupq_n_s32(Q31);
        Zero_32x2            = vget_low_s32(Zero_32x4);
        Q31_32x2             = vget_low_s32(Q31_32x4);
        bsl_u32x4            = vceqq_u32(bsl_u32x4, vreinterpretq_u32_s32(Zero_32x4));
        bsl_u32x2            = vceq_u32 (bsl_u32x2, vreinterpret_u32_s32 (Zero_32x2));
        Exp_32x4             = vbslq_s32(bsl_u32x4, Zero_32x4, Exp_32x4);
        Exp_32x2             = vbsl_s32 (bsl_u32x2, Zero_32x2, Exp_32x2);
        TargetGain_32x4      = vbslq_s32(bsl_u32x4, Q31_32x4,  TargetGain_32x4);
        TargetGain_32x2      = vbsl_s32 (bsl_u32x2, Q31_32x2,  TargetGain_32x2);
        vst1q_s32    (Exp_table,            Exp_32x4);
        vst1_lane_s32(Exp_table        + 4, Exp_32x2,        0);
        vst1q_s32    (TargetGain_table,     TargetGain_32x4);
        vst1_lane_s32(TargetGain_table + 4, TargetGain_32x2, 0);
    }
    for(bidx = 0; bidx < NumBands; bidx++)
    {
//        if(HeapPtr->BandCompressors[bidx].Enable)
//        {
//            // linear_gain = 10^(gain_dB/20)
//            //             = 2^gain_pow2
//            //
//            // with gain_pow2 = log2(linear_gain)
//            //                = log10(linear_gain)/log10(2)
//            //                = gain_dB/(20*log10(2))
//            double gain_pow2, exp_double, mant_double;
//
//            gain_pow2   = (((double) HeapPtr->ComTargetGaindB[bidx]) + ((double) HeapPtr->ComPostGaindB[bidx])) / 65536.0 / 20.0 / LOG10_OF_2;
//            exp_double  = ceil(gain_pow2);
//            mant_double = pow(2.0, gain_pow2 - exp_double);
//            Exp         = (MMshort) exp_double;
//            TargetGain  = (MMshort) floor(((double) Q31) * mant_double);
//        }
//        else
//        {
//            Exp        = 0;
//            TargetGain = Q31;
//        }
        Exp         = Exp_table       [bidx];
        TargetGain  = TargetGain_table[bidx];

#ifdef MDRC5B_DEBUG
        printf("%ld, band %d, gain %.3f, exp %d, mant %.6f\n", counter, bidx, (float)HeapPtr->ComTargetGaindB[bidx]/65536.0, HeapPtr->ComGainExp[bidx], (float)HeapPtr->ComGainMant[bidx]/2147483648.0);
#endif // MDRC5B_DEBUG

        ComGainMant[bidx][0] = HeapPtr->ComGainMant[bidx];
        ComGainExp[bidx]     = HeapPtr->ComGainExp[bidx];
        if(ComGainExp[bidx] > Exp)
        {
            TargetGain >>= (ComGainExp[bidx] - Exp);
            Exp          = ComGainExp[bidx];
        }
        else
        {
            ComGainMant[bidx][0] >>= (Exp - ComGainExp[bidx]);
            ComGainExp[bidx]       = Exp;
        }


        ComAtCoef  = HeapPtr->ComAtCoef[bidx];
        ComReCoef  = HeapPtr->ComReCoef[bidx];
        ComHoldRem = HeapPtr->ComHoldRem[bidx];
        HtSamp     = HeapPtr->HtSamp[bidx];

        // produce gains for the whole block of samples
        GainThreshold = (Exp < 0 ? (GAIN_MINUS_60DB_Q31 << (-Exp)) : (GAIN_MINUS_60DB_Q31 >> Exp));
        if(((TargetGain - GainThreshold) < ComGainMant[bidx][0])) // k refers to the previous sample, k+1 to the current sample
        {
            // attack
            TargetGain_32x2  = vdup_n_s32(TargetGain);
            ComAtCoef_32x2   = vdup_n_s32(ComAtCoef);
            ComGainMant_32x2 = vdup_n_s32(ComGainMant[bidx][0]);
            p_ComGainMant    = &ComGainMant[bidx][1];
            for(k = (Samples >> 1); k > 0; k--)
            {
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComAtCoef_32x2, Tmp_32x2);          // ComAtCoef*(ComGainMant(n)-TargetGain), dummy
                ComGainMant_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // ComGainMant(n+1)=TargetGain+ComAtCoef*(ComGainMant(n)-TargetGain), dummy
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n+1)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComAtCoef_32x2, Tmp_32x2);          // ComAtCoef*(ComGainMant(n+1)-TargetGain), dummy
                Tmp_32x2         = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // ComGainMant(n+2)=TargetGain+ComAtCoef*(ComGainMant(n+1)-TargetGain), dummy
                ComGainMant_32x2 = vrev64_s32(ComGainMant_32x2);                    // dummy, ComGainMant(n+1)
                ComGainMant_32x2 = vext_s32(ComGainMant_32x2, Tmp_32x2, 1);         // ComGainMant(n+1), ComGainMant(n+2)
                vst1_s32(p_ComGainMant, ComGainMant_32x2);
                p_ComGainMant   += 2;
                ComGainMant_32x2 = Tmp_32x2;                                        // last ComGainMant, dummy
            }
            if(Samples & 1)
            {
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComAtCoef_32x2, Tmp_32x2);          // ComAtCoef*(ComGainMant(n)-TargetGain), dummy
                ComGainMant_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // last ComGainMant=TargetGain+ComAtCoef*(ComGainMant(n)-TargetGain), dummy
                vst1_lane_s32(p_ComGainMant, ComGainMant_32x2, 0);
            }
            ComHoldRem = HtSamp; // init hold time
        }
        else
        {
            // release
            n                = (ComHoldRem < Samples ? ComHoldRem : Samples);
            ComHoldRem      -= n;
            ComGainMant_32x4 = vdupq_n_s32(ComGainMant[bidx][0]);               // last ComGainMant, last ComGainMant, last ComGainMant, last ComGainMant
            p_ComGainMant    = &ComGainMant[bidx][1];
            for(k = (n >> 2); k > 0; k--)
            {
                vst1q_s32(p_ComGainMant, ComGainMant_32x4);
                p_ComGainMant+= 4;
            }
            k = (n & 3);
            switch(k)
            {
                case 3:
                    vst1q_lane_s32(p_ComGainMant + 2, ComGainMant_32x4, 2);
                case 2:
                    vst1_s32(p_ComGainMant, vget_low_s32(ComGainMant_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_ComGainMant, ComGainMant_32x4, 0);
                default:
                    break;
            }
            p_ComGainMant += k;

            n                = Samples - n;
            ComGainMant_32x2 = vget_low_s32(ComGainMant_32x4);                      // last ComGainMant, last ComGainMant
            TargetGain_32x2  = vdup_n_s32(TargetGain);
            ComReCoef_32x2   = vdup_n_s32(ComReCoef);
            for(k = (n >> 1); k > 0; k--)
            {
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComReCoef_32x2, Tmp_32x2);          // ComReCoef*(ComGainMant(n)-TargetGain), dummy
                ComGainMant_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // ComGainMant(n+1)=TargetGain+ComReCoef*(ComGainMant(n)-TargetGain), dummy
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n+1)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComReCoef_32x2, Tmp_32x2);          // ComReCoef*(ComGainMant(n+1)-TargetGain), dummy
                Tmp_32x2         = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // ComGainMant(n+2)=TargetGain+ComReCoef*(ComGainMant(n+1)-TargetGain), dummy
                ComGainMant_32x2 = vrev64_s32(ComGainMant_32x2);                    // dummy, ComGainMant(n+1)
                ComGainMant_32x2 = vext_s32(ComGainMant_32x2, Tmp_32x2, 1);         // ComGainMant(n+1), ComGainMant(n+2)
                vst1_s32(p_ComGainMant, ComGainMant_32x2);
                p_ComGainMant   += 2;
                ComGainMant_32x2 = Tmp_32x2;                                        // last ComGainMant, dummy
            }
            if(n & 1)
            {
                Tmp_32x2         = vqsub_s32(ComGainMant_32x2, TargetGain_32x2);    // ComGainMant(n)-TargetGain, dummy
                Tmp_32x2         = vqrdmulh_s32(ComReCoef_32x2, Tmp_32x2);          // ComReCoef*(ComGainMant(n)-TargetGain), dummy
                ComGainMant_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);            // last ComGainMant=TargetGain+ComReCoef*(ComGainMant(n)-TargetGain), dummy
                vst1_lane_s32(p_ComGainMant, ComGainMant_32x2, 0);
            }
        }

        // save history
        // in any case ComGainMant_32x2 contains last ComGainMant in its lower part (see above)
        Tmp_32x2                   = vcls_s32(ComGainMant_32x2);
        ComGainMant_32x2           = vshl_s32(ComGainMant_32x2, Tmp_32x2);
        HeapPtr->ComGainMant[bidx] = vget_lane_s32(ComGainMant_32x2, 0);
        HeapPtr->ComGainExp[bidx]  = ComGainExp[bidx] - vget_lane_s32(Tmp_32x2, 0);
        HeapPtr->ComHoldRem[bidx]  = ComHoldRem;
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_GAIN)


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_OUTPUT)

    // output
    LaIdx = HeapPtr->LaIdx; // this is now the oldest sample
    if(LaIdx + Samples >= HeapPtr->MdrcLALen)
    {
        cpt1 = HeapPtr->MdrcLALen - LaIdx;
        cpt2 = Samples - cpt1;
    }
    else
    {
        cpt1 = Samples;
        cpt2 = 0;
    }
    for(ch = 0; ch < NumMainCh; ch++)
    {
        MemOutPtr =  HeapPtr->MainInBuf[ch];
        Ptr2      = &HeapPtr->MainSubLABuf[ch][0][LaIdx];  // go to the first valid sample

        // first band
        GainExp       = ComGainExp[0];
        p_ComGainMant = &ComGainMant[0][1];
        GainExp_64x2  = vdupq_n_s64((int64_t) GainExp);
        for(k = (cpt1 >> 2); k > 0; k--)
        {
            sample_32x4      = vld1q_s32(Ptr2);
            Ptr2            += 4;
            ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
            p_ComGainMant   += 4;
            sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
            sample1_64x2     = vqshlq_s64(vmovl_s32(vget_low_s32 (sample_32x4)), GainExp_64x2);
            sample2_64x2     = vqshlq_s64(vmovl_s32(vget_high_s32(sample_32x4)), GainExp_64x2);
            vst1q_s64(MemOutPtr, sample1_64x2);
            MemOutPtr       += 2;
            vst1q_s64(MemOutPtr, sample2_64x2);
            MemOutPtr       += 2;
        }
        k = (cpt1 & 3);
        if(k > 0)
        {
            sample_32x4      = vld1q_s32(Ptr2);
            ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
            sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
            sample1_64x2     = vqshlq_s64(vmovl_s32(vget_low_s32(sample_32x4)), GainExp_64x2);
            switch(k)
            {
                case 3:
                    sample_64x1 = vqshl_s64(vget_low_s64(vmovl_s32(vget_high_s32(sample_32x4))), vget_low_s64(GainExp_64x2));
                    vst1_s64(MemOutPtr + 2, sample_64x1);
                case 2:
                    vst1q_s64(MemOutPtr, sample1_64x2);
                    break;
                case 1:
                    vst1q_lane_s64(MemOutPtr, sample1_64x2, 0);
                default:
                    break;
            }
            p_ComGainMant += k;
            MemOutPtr     += k;
        }
        if(cpt2 > 0)
        {
            Ptr2 = HeapPtr->MainSubLABuf[ch][0];
            for(k = (cpt2 >> 2); k > 0; k--)
            {
                sample_32x4      = vld1q_s32(Ptr2);
                Ptr2            += 4;
                ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                p_ComGainMant   += 4;
                sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                sample1_64x2     = vqshlq_s64(vmovl_s32(vget_low_s32 (sample_32x4)), GainExp_64x2);
                sample2_64x2     = vqshlq_s64(vmovl_s32(vget_high_s32(sample_32x4)), GainExp_64x2);
                vst1q_s64(MemOutPtr, sample1_64x2);
                MemOutPtr       += 2;
                vst1q_s64(MemOutPtr, sample2_64x2);
                MemOutPtr       += 2;
            }
            k = (cpt2 & 3);
            if(k > 0)
            {
                sample_32x4      = vld1q_s32(Ptr2);
                ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                sample1_64x2     = vqshlq_s64(vmovl_s32(vget_low_s32(sample_32x4)), GainExp_64x2);
                switch(k)
                {
                    case 3:
                        sample_64x1 = vqshl_s64(vget_low_s64(vmovl_s32(vget_high_s32(sample_32x4))), vget_low_s64(GainExp_64x2));
                        vst1_s64(MemOutPtr + 2, sample_64x1);
                    case 2:
                        vst1q_s64(MemOutPtr, sample1_64x2);
                        break;
                    case 1:
                        vst1q_lane_s64(MemOutPtr, sample1_64x2, 0);
                    default:
                        break;
                }
            }
        }

        // higher bands
        for(bidx = 1; bidx < NumBands; bidx++)
        {
            Ptr2          = &HeapPtr->MainSubLABuf[ch][bidx][LaIdx];  // go to the first valid sample
            GainExp       = ComGainExp[bidx];
            MemOutPtr     = HeapPtr->MainInBuf[ch];
            p_ComGainMant = &ComGainMant[bidx][1];
            GainExp_64x2  = vdupq_n_s64((int64_t) GainExp);
            for(k = (cpt1 >> 2); k > 0; k--)
            {
                sample_32x4      = vld1q_s32(Ptr2);
                Ptr2            += 4;
                ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                p_ComGainMant   += 4;
                sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                sample1_64x2     = vld1q_s64(MemOutPtr);
                sample1_64x2     = vqaddq_s64(sample1_64x2, vqshlq_s64(vmovl_s32(vget_low_s32 (sample_32x4)), GainExp_64x2));
                vst1q_s64(MemOutPtr, sample1_64x2);
                MemOutPtr       += 2;
                sample2_64x2     = vld1q_s64(MemOutPtr);
                sample2_64x2     = vqaddq_s64(sample2_64x2, vqshlq_s64(vmovl_s32(vget_high_s32(sample_32x4)), GainExp_64x2));
                vst1q_s64(MemOutPtr, sample2_64x2);
                MemOutPtr       += 2;
            }
            k = (cpt1 & 3);
            if(k > 0)
            {
                sample_32x4      = vld1q_s32(Ptr2);
                ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                sample1_64x2     = vld1q_s64(MemOutPtr);
                sample1_64x2     = vqaddq_s64(sample1_64x2, vqshlq_s64(vmovl_s32(vget_low_s32(sample_32x4)), GainExp_64x2));
                switch(k)
                {
                    case 3:
                        sample_64x1 = vld1_s64(MemOutPtr + 2);
                        sample_64x1 = vqadd_s64(sample_64x1, vqshl_s64(vget_low_s64(vmovl_s32(vget_high_s32(sample_32x4))), vget_low_s64(GainExp_64x2)));
                        vst1_s64(MemOutPtr + 2, sample_64x1);
                    case 2:
                        vst1q_s64(MemOutPtr, sample1_64x2);
                        break;
                    case 1:
                        vst1q_lane_s64(MemOutPtr, sample1_64x2, 0);
                    default:
                        break;
                }
                p_ComGainMant += k;
                MemOutPtr     += k;
            }
            if(cpt2 > 0)
            {
                Ptr2 = HeapPtr->MainSubLABuf[ch][bidx];
                for(k = (cpt2 >> 2); k > 0; k--)
                {
                    sample_32x4      = vld1q_s32(Ptr2);
                    Ptr2            += 4;
                    ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                    p_ComGainMant   += 4;
                    sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                    sample1_64x2     = vld1q_s64(MemOutPtr);
                    sample1_64x2     = vqaddq_s64(sample1_64x2, vqshlq_s64(vmovl_s32(vget_low_s32 (sample_32x4)), GainExp_64x2));
                    vst1q_s64(MemOutPtr, sample1_64x2);
                    MemOutPtr       += 2;
                    sample2_64x2     = vld1q_s64(MemOutPtr);
                    sample2_64x2     = vqaddq_s64(sample2_64x2, vqshlq_s64(vmovl_s32(vget_high_s32(sample_32x4)), GainExp_64x2));
                    vst1q_s64(MemOutPtr, sample2_64x2);
                    MemOutPtr       += 2;
                }
                k = (cpt2 & 3);
                if(k > 0)
                {
                    sample_32x4      = vld1q_s32(Ptr2);
                    ComGainMant_32x4 = vld1q_s32(p_ComGainMant);
                    sample_32x4      = vqrdmulhq_s32(sample_32x4, ComGainMant_32x4);
                    sample1_64x2     = vld1q_s64(MemOutPtr);
                    sample1_64x2     = vqaddq_s64(sample1_64x2, vqshlq_s64(vmovl_s32(vget_low_s32(sample_32x4)), GainExp_64x2));
                    switch(k)
                    {
                        case 3:
                            sample_64x1 = vld1_s64(MemOutPtr + 2);
                            sample_64x1 = vqadd_s64(sample_64x1, vqshl_s64(vget_low_s64(vmovl_s32(vget_high_s32(sample_32x4))), vget_low_s64(GainExp_64x2)));
                            vst1_s64(MemOutPtr + 2, sample_64x1);
                        case 2:
                            vst1q_s64(MemOutPtr, sample1_64x2);
                            break;
                        case 1:
                            vst1q_lane_s64(MemOutPtr, sample1_64x2, 0);
                        default:
                            break;
                    }
                }
            }
        }
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_COMPRESSOR_OUTPUT)

#ifdef DEBUG_COMPRESSOR_OUTPUT
    // Debug sub-band filters output
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        debug_write_string("MRDC5B_COMPRESSOR_OUTPUT\n");
        for(k = 0; k < Samples; k++)
        {
            for(ch = 0; ch < NumMainCh; ch++)
            {
                debug_write_string("| ");
                debug_write_compressor_output(ch, (int) (HeapPtr->MainInBuf[ch][k] >> 1));
            }
            debug_write_string("|\n");
        }
    }
#endif // DEBUG_COMPRESSOR_OUTPUT

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_APPLY_COMPRESSOR)
}


//#define DEBUG_LN

/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    compute the compression gain for each band.
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
static void mdrc5b_compressor_gain(MDRC5B_LOCAL_STRUCT_T *HeapPtr, MMlong *Rms)
{
    unsigned int bidx, NumBands, KneePoints, p;
    MMshort      RmsdB_table[MDRC5B_SUBBAND_MAX];
    int32_t      *p_ComSlope;
    int32_t      *p_ComThreshdB;
    int32x2_t    ComThreshdB_32x2, Delta_32x2, ComSlope_32x2;
    int32x4_t    ComThreshdB_32x4;
    unsigned int remaining_KneePoints;
    uint64_t     *p_Rms;
    uint64x2_t   Rms1_u64x2, Rms2_u64x2, Rms3_u64x2, Thres_u64x2;
    uint32x4x2_t Rms_u32x4x2;
    uint32x2x2_t Rms_u32x2x2;
    uint32x4_t   Const_u32x4, Rms_mant_u32x4, Rms_exp_u32x4, Rms_exp2_u32x4;
    uint32x2_t   Const_u32x2, Rms_mant_u32x2, Rms_exp_u32x2, Rms_exp2_u32x2, Tmp1_u32x2, Tmp2_u32x2;
    uint64x2_t   bsl_u64x2;
    uint32x4_t   bsl_u32x4;
    uint32x2_t   bsl_u32x2, bsl2_u32x2;
    int32x4_t    Const_32x4, x_32x4, xn_32x4, Rms_exp_32x4, RmsdBMin_32x4, RmsdB_32x4;
    int32x2_t    Const_32x2, x_32x2, xn_32x2, Rms_exp_32x2, RmsdBMin_32x2, RmsdB_32x2;
#ifdef DEBUG_LN
    double       x[MDRC5B_SUBBAND_MAX], xn[MDRC5B_SUBBAND_MAX], lnx[MDRC5B_SUBBAND_MAX];
#endif //DEBUG_LN


    // WARNING : this optimization works only for MDRC5B_SUBBAND_MAX = 5
    // if MDRC5B_SUBBAND_MAX is modified, optimization must be modified accordlingly
    p_Rms          = (uint64_t *) Rms;
    Rms1_u64x2     = vld1q_u64(p_Rms);
    p_Rms         += 2;
    Rms2_u64x2     = vld1q_u64(p_Rms);
    p_Rms         += 2;
    Rms3_u64x2     = vld1q_u64(p_Rms);
    Const_u32x4    = vdupq_n_u32(0);
    Const_u32x2    = vget_low_u32(Const_u32x4);
    Rms_u32x4x2    = vuzpq_u32(vreinterpretq_u32_u64(Rms1_u64x2), vreinterpretq_u32_u64(Rms2_u64x2));
    Rms_u32x2x2    = vuzp_u32 (vget_low_u32(vreinterpretq_u32_u64(Rms3_u64x2)), vget_high_u32(vreinterpretq_u32_u64(Rms3_u64x2)));
    bsl_u32x4      = vceqq_u32(Rms_u32x4x2.val[1], Const_u32x4);
    bsl_u32x2      = vceq_u32 (Rms_u32x2x2.val[1], Const_u32x2);
    Const_u32x4    = vdupq_n_u32(32);
    Const_u32x2    = vget_low_u32(Const_u32x4);
    Rms_exp_u32x4  = vaddq_u32(vclzq_u32(Rms_u32x4x2.val[0]), Const_u32x4);
    Rms_exp_u32x2  = vadd_u32 (vclz_u32 (Rms_u32x2x2.val[0]), Const_u32x2);
    Rms_exp2_u32x4 = vclzq_u32(Rms_u32x4x2.val[1]);
    Rms_exp2_u32x2 = vclz_u32 (Rms_u32x2x2.val[1]);
    Rms_exp_u32x4  = vbslq_u32(bsl_u32x4, Rms_exp_u32x4, Rms_exp2_u32x4);
    Rms_exp_u32x2  = vbsl_u32 (bsl_u32x2, Rms_exp_u32x2, Rms_exp2_u32x2);
    Tmp1_u32x2     = vrshrn_n_u64(vshlq_u64(Rms1_u64x2, vreinterpretq_s64_u64(vmovl_u32(vget_low_u32 (Rms_exp_u32x4)))), 32);
    Tmp2_u32x2     = vrshrn_n_u64(vshlq_u64(Rms2_u64x2, vreinterpretq_s64_u64(vmovl_u32(vget_high_u32(Rms_exp_u32x4)))), 32);
    Rms_mant_u32x4 = vcombine_u32(Tmp1_u32x2, Tmp2_u32x2);
    Rms_mant_u32x2 = vrshrn_n_u64(vshlq_u64(Rms3_u64x2, vreinterpretq_s64_u64(vmovl_u32(Rms_exp_u32x2))), 32);

#ifdef DEBUG_LN
    printf("\nmdrc5b_compressor_gain\n");
    printf("Rms[0]=0x%016llX => Rms_mant[0] = 0x%08X, Rms_exp[0] = %2u => Rms[0]=0x%016llX\n", vgetq_lane_u64(Rms1_u64x2, 0), vgetq_lane_u32(Rms_mant_u32x4, 0), vgetq_lane_u32(Rms_exp_u32x4, 0), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 0)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 0));
    printf("Rms[1]=0x%016llX => Rms_mant[1] = 0x%08X, Rms_exp[1] = %2u => Rms[1]=0x%016llX\n", vgetq_lane_u64(Rms1_u64x2, 1), vgetq_lane_u32(Rms_mant_u32x4, 1), vgetq_lane_u32(Rms_exp_u32x4, 1), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 1)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 1));
    printf("Rms[2]=0x%016llX => Rms_mant[2] = 0x%08X, Rms_exp[2] = %2u => Rms[2]=0x%016llX\n", vgetq_lane_u64(Rms2_u64x2, 0), vgetq_lane_u32(Rms_mant_u32x4, 2), vgetq_lane_u32(Rms_exp_u32x4, 2), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 2)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 2));
    printf("Rms[3]=0x%016llX => Rms_mant[3] = 0x%08X, Rms_exp[3] = %2u => Rms[3]=0x%016llX\n", vgetq_lane_u64(Rms2_u64x2, 1), vgetq_lane_u32(Rms_mant_u32x4, 3), vgetq_lane_u32(Rms_exp_u32x4, 3), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 3)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 3));
    printf("Rms[4]=0x%016llX => Rms_mant[4] = 0x%08X, Rms_exp[4] = %2u => Rms[4]=0x%016llX\n", vgetq_lane_u64(Rms3_u64x2, 0), vget_lane_u32 (Rms_mant_u32x2, 0), vget_lane_u32 (Rms_exp_u32x2, 0), (((unsigned long long) vget_lane_u32 (Rms_mant_u32x2, 0)) << 32) >> vget_lane_u32 (Rms_exp_u32x2, 0));
#endif //DEBUG_LN

#ifdef SAMPLES_24_BITS
    // correction of 16 bits (2*8 bits because of squared samples) if input samples are 24 bits
    Const_u32x4    = vdupq_n_u32(16);
    Const_u32x2    = vget_low_u32(Const_u32x4);
    Rms_exp_u32x4  = vsubq_u32(Rms_exp_u32x4, Const_u32x4);
    Rms_exp_u32x2  = vsub_u32 (Rms_exp_u32x2, Const_u32x2);
#endif // SAMPLES_24_BITS

    // at this point : Rms_mant is left shifted unsigned value of Rms (then Rms_mant is Q32) and Rms_exp is its exponet
    // i.e. Rms = Rms_mant*2^(-Rms_exp-64) with 0.5 <= Rms_mant < 1

    Const_u32x4    = vdupq_n_u32(0xB504F334);                                                       // sqrt(0.5) in Q32
    Const_u32x2    = vget_low_u32(Const_u32x4);                                                     //

    bsl_u32x4      = vcgeq_u32(Rms_mant_u32x4, Const_u32x4);                                        // bsl = (Rms_mant >= sqrt(0.5))
    bsl_u32x2      = vcge_u32 (Rms_mant_u32x2, Const_u32x2);                                        //

    Const_u32x4    = vdupq_n_u32(1);                                                                // if(Rms_mant >= sqrt(0.5))
    Const_u32x2    = vget_low_u32(Const_u32x4);                                                     // {
    Rms_exp_u32x4  = vbslq_u32(bsl_u32x4, vsubq_u32(Rms_exp_u32x4, Const_u32x4), Rms_exp_u32x4);    //     Rms_exp--;
    Rms_exp_u32x2  = vbsl_u32 (bsl_u32x2, vsub_u32 (Rms_exp_u32x2, Const_u32x2), Rms_exp_u32x2);    //     Rms_mant >>= 1;
    Rms_mant_u32x4 = vbslq_u32(bsl_u32x4, vrshrq_n_u32(Rms_mant_u32x4, 1),       Rms_mant_u32x4);   // }
    Rms_mant_u32x2 = vbsl_u32 (bsl_u32x2, vrshr_n_u32 (Rms_mant_u32x2, 1),       Rms_mant_u32x2);   //

#ifdef DEBUG_LN
    printf("\nsqrt(0.5)/2 <= Rms_mant < sqrt(0.5)\n");
    printf("Rms_mant[0] = 0x%08X, Rms_exp[0] = %2u => Rms[0]=0x%016llX\n", vgetq_lane_u32(Rms_mant_u32x4, 0), vgetq_lane_u32(Rms_exp_u32x4, 0), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 0)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 0));
    printf("Rms_mant[1] = 0x%08X, Rms_exp[1] = %2u => Rms[1]=0x%016llX\n", vgetq_lane_u32(Rms_mant_u32x4, 1), vgetq_lane_u32(Rms_exp_u32x4, 1), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 1)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 1));
    printf("Rms_mant[2] = 0x%08X, Rms_exp[2] = %2u => Rms[2]=0x%016llX\n", vgetq_lane_u32(Rms_mant_u32x4, 2), vgetq_lane_u32(Rms_exp_u32x4, 2), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 2)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 2));
    printf("Rms_mant[3] = 0x%08X, Rms_exp[3] = %2u => Rms[3]=0x%016llX\n", vgetq_lane_u32(Rms_mant_u32x4, 3), vgetq_lane_u32(Rms_exp_u32x4, 3), (((unsigned long long) vgetq_lane_u32(Rms_mant_u32x4, 3)) << 32) >> vgetq_lane_u32(Rms_exp_u32x4, 3));
    printf("Rms_mant[4] = 0x%08X, Rms_exp[4] = %2u => Rms[4]=0x%016llX\n", vget_lane_u32 (Rms_mant_u32x2, 0), vget_lane_u32 (Rms_exp_u32x2, 0), (((unsigned long long) vget_lane_u32 (Rms_mant_u32x2, 0)) << 32) >> vget_lane_u32 (Rms_exp_u32x2, 0));
#endif //DEBUG_LN

    // at this point : sqrt(0.5)/2 <= Rms_mant < sqrt(0.5)
    //
    // RmsdB = 10*log10(Rms)+20*log10(2)*HEADROOM
    //       = 10*ln(Rms)/ln(10)+20*ln(2)/ln(10)*HEADROOM
    //       = 10/ln(10)*ln(Rms_mant*2^(-Rms_exp))+20*ln(2)/ln(10)*HEADROOM
    //       = 10/ln(10)*(ln(Rms_mant)-Rms_exp*ln(2))+20*ln(2)/ln(10)*HEADROOM
    //       = 10/ln(10)*ln(Rms_mant)-Rms_exp*10*ln(2)/ln(10)+20*ln(2)/ln(10)*HEADROOM
    //       = 10/ln(10)*ln(Rms_mant)+10*ln(2)/ln(10)*(2*HEADROOM-Rms_exp)
    //
    // ln(1+x) = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9 - x^10/10 ...    accuracy OK if |x| < 0.5
    // sqrt(0.5)/2 <= Rms_mant < sqrt(0.5)  =>  sqrt(0.5)-1 <= 2*Rms_mant-1 < 2*sqrt(0.5)-1
    //                                      =>  ln(Rms_mant) = ln(1+x)-ln(2) with x=2*Rms_mant-1, i.e. |x| < 0.414214...
    //
    // => RmsdB = 10/ln(10)*ln(1+x)+10*ln(2)/ln(10)*(2*HEADROOM-Rms_exp)

    Const_u32x4    = vdupq_n_u32(0x80000000);                                                       // 0.5 in Q32
    Const_u32x2    = vget_low_u32(Const_u32x4);                                                     //

    x_32x4         = vreinterpretq_s32_u32(vsubq_u32(Rms_mant_u32x4, Const_u32x4));                 // x=2*Rms_mant-1 in Q31
    x_32x2         = vreinterpret_s32_u32 (vsub_u32 (Rms_mant_u32x2, Const_u32x2));                 // => sqrt(0.5)-1 <= x < 2*sqrt(0.5)-1

    RmsdB_32x4     = x_32x4;                                                                        // RmsdB = x
    RmsdB_32x2     = x_32x2;                                                                        //

#ifdef DEBUG_LN
    lnx[0] = xn[0] = x[0] = ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0;
    lnx[1] = xn[1] = x[1] = ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0;
    lnx[2] = xn[2] = x[2] = ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0;
    lnx[3] = xn[3] = x[3] = ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0;
    lnx[4] = xn[4] = x[4] = ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0;
    printf("\nln(1+x), x\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(x_32x4, x_32x4);                                                 // xn = x^2
    xn_32x2        = vqrdmulh_s32 (x_32x2, x_32x2);                                                 //

    RmsdB_32x4     = vqsubq_s32(RmsdB_32x4, vrshrq_n_s32(xn_32x4, 1));                              // RmsdB = x - x^2/2
    RmsdB_32x2     = vqsub_s32 (RmsdB_32x2, vrshr_n_s32 (xn_32x2, 1));                              //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] -= xn[0] / 2.0;
    lnx[1] -= xn[1] / 2.0;
    lnx[2] -= xn[2] / 2.0;
    lnx[3] -= xn[3] / 2.0;
    lnx[4] -= xn[4] / 2.0;
    printf("\nln(1+x), x - x^2/2\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^3
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x2AAAAAAB);                                                       // 1/3 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqaddq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3
    RmsdB_32x2     = vqadd_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] += xn[0] / 3.0;
    lnx[1] += xn[1] / 3.0;
    lnx[2] += xn[2] / 3.0;
    lnx[3] += xn[3] / 3.0;
    lnx[4] += xn[4] / 3.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^4
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    RmsdB_32x4     = vqsubq_s32(RmsdB_32x4, vrshrq_n_s32(xn_32x4, 2));                              // RmsdB = x - x^2/2 + x^3/3 - x^4/4
    RmsdB_32x2     = vqsub_s32 (RmsdB_32x2, vrshr_n_s32 (xn_32x2, 2));                              //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] -= xn[0] / 4.0;
    lnx[1] -= xn[1] / 4.0;
    lnx[2] -= xn[2] / 4.0;
    lnx[3] -= xn[3] / 4.0;
    lnx[4] -= xn[4] / 4.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^5
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x1999999A);                                                       // 1/5 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqaddq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5
    RmsdB_32x2     = vqadd_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] += xn[0] / 5.0;
    lnx[1] += xn[1] / 5.0;
    lnx[2] += xn[2] / 5.0;
    lnx[3] += xn[3] / 5.0;
    lnx[4] += xn[4] / 5.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^6
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x15555555);                                                       // 1/6 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqsubq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6
    RmsdB_32x2     = vqsub_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] -= xn[0] / 6.0;
    lnx[1] -= xn[1] / 6.0;
    lnx[2] -= xn[2] / 6.0;
    lnx[3] -= xn[3] / 6.0;
    lnx[4] -= xn[4] / 6.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^7
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x12492492);                                                       // 1/7 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqaddq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7
    RmsdB_32x2     = vqadd_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] += xn[0] / 7.0;
    lnx[1] += xn[1] / 7.0;
    lnx[2] += xn[2] / 7.0;
    lnx[3] += xn[3] / 7.0;
    lnx[4] += xn[4] / 7.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^8
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    RmsdB_32x4     = vqsubq_s32(RmsdB_32x4, vrshrq_n_s32(xn_32x4, 3));                              // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8
    RmsdB_32x2     = vqsub_s32 (RmsdB_32x2, vrshr_n_s32 (xn_32x2, 3));                              //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] -= xn[0] / 8.0;
    lnx[1] -= xn[1] / 8.0;
    lnx[2] -= xn[2] / 8.0;
    lnx[3] -= xn[3] / 8.0;
    lnx[4] -= xn[4] / 8.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^9
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x0E38E38E);                                                       // 1/9 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqaddq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9
    RmsdB_32x2     = vqadd_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] += xn[0] / 9.0;
    lnx[1] += xn[1] / 9.0;
    lnx[2] += xn[2] / 9.0;
    lnx[3] += xn[3] / 9.0;
    lnx[4] += xn[4] / 9.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    xn_32x4        = vqrdmulhq_s32(xn_32x4, x_32x4);                                                // xn = x^10
    xn_32x2        = vqrdmulh_s32 (xn_32x2, x_32x2);                                                //

    Const_32x4     = vdupq_n_s32(0x0CCCCCCD);                                                       // 1/10 in Q31
    Const_32x2     = vget_low_s32(Const_32x4);                                                      //

    RmsdB_32x4     = vqsubq_s32(RmsdB_32x4, vqrdmulhq_s32(xn_32x4, Const_32x4));                    // RmsdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9 - x^10/10
    RmsdB_32x2     = vqsub_s32 (RmsdB_32x2, vqrdmulh_s32 (xn_32x2, Const_32x2));                    //

#ifdef DEBUG_LN
    xn[0]  *= x[0];
    xn[1]  *= x[1];
    xn[2]  *= x[2];
    xn[3]  *= x[3];
    xn[4]  *= x[4];
    lnx[0] -= xn[0] / 10.0;
    lnx[1] -= xn[1] / 10.0;
    lnx[2] -= xn[2] / 10.0;
    lnx[3] -= xn[3] / 10.0;
    lnx[4] -= xn[4] / 10.0;
    printf("\nln(1+x), x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9 - x^10/10\n");
    printf("0 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[0]), lnx[0], ((double) vgetq_lane_s32(RmsdB_32x4, 0)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 0));
    printf("1 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[1]), lnx[1], ((double) vgetq_lane_s32(RmsdB_32x4, 1)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 1));
    printf("2 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[2]), lnx[2], ((double) vgetq_lane_s32(RmsdB_32x4, 2)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 2));
    printf("3 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[3]), lnx[3], ((double) vgetq_lane_s32(RmsdB_32x4, 3)) / 2147483648.0, vgetq_lane_s32(RmsdB_32x4, 3));
    printf("4 : %15.12lf - %15.12lf / %15.12lf / 0x%08X\n", log(1.0 + x[4]), lnx[4], ((double) vget_lane_s32 (RmsdB_32x2, 0)) / 2147483648.0, vget_lane_s32 (RmsdB_32x2, 0));
#endif //DEBUG_LN

    // at this point : RmsdB contains ln(1+x) in Q31
    // but RmsdB = 10/ln(10)*ln(1+x)+10*ln(2)/ln(10)*(2*HEADROOM-Rms_exp)
    // => RmsdB (Q16) = 0x457CB*ln(1+x)+0x302A3*(2*HEADROOM-Rms_exp)

    Const_32x4     = vdupq_n_s32(0x457CB);                                                          // fractional mutiply 0x457CB*ln(1+x)
    Const_32x2     = vget_low_s32(Const_32x4);                                                      // in Q16
    RmsdB_32x4     = vqrdmulhq_s32(RmsdB_32x4, Const_32x4);                                         //
    RmsdB_32x2     = vqrdmulh_s32 (RmsdB_32x2, Const_32x2);                                         //

    Const_32x4     = vdupq_n_s32(2*HEADROOM);                                                       // Rms_exp = 2*HEADROOM+1-Rms_exp
    Const_32x2     = vget_low_s32(Const_32x4);                                                      // in Q16
    Rms_exp_32x4   = vsubq_s32(Const_32x4, vreinterpretq_s32_u32(Rms_exp_u32x4));                   //
    Rms_exp_32x2   = vsub_s32 (Const_32x2, vreinterpret_s32_u32 (Rms_exp_u32x2));                   //

    Const_32x4     = vdupq_n_s32(0x302A3);                                                          // RmsdB final value
    Const_32x2     = vget_low_s32(Const_32x4);                                                      // (integer mac 0x302A3*Rms_exp)
    RmsdB_32x4     = vmlaq_s32(RmsdB_32x4, Rms_exp_32x4, Const_32x4);                               //
    RmsdB_32x2     = vmla_s32 (RmsdB_32x2, Rms_exp_32x2, Const_32x2);                               //

    Thres_u64x2    = vdupq_n_u64(MDRC5B_ALMOST_ZERO_THRESH);
    RmsdBMin_32x4  = vdupq_n_s32((MDRC5B_POWER_DB_MINUS_INF) << 16);
    RmsdBMin_32x2  = vget_low_s32(RmsdBMin_32x4);
    bsl_u64x2      = vreinterpretq_u64_s64(vshrq_n_s64(vqsubq_s64(vreinterpretq_s64_u64(Rms1_u64x2), vreinterpretq_s64_u64(Thres_u64x2)), 64));  // sign(Rms1_64x2 - Thres_64x2)
    bsl_u32x2      = vshrn_n_u64(bsl_u64x2, 32);
    bsl_u64x2      = vreinterpretq_u64_s64(vshrq_n_s64(vqsubq_s64(vreinterpretq_s64_u64(Rms2_u64x2), vreinterpretq_s64_u64(Thres_u64x2)), 64));  // sign(Rms2_64x2 - Thres_64x2)
    bsl2_u32x2     = vshrn_n_u64(bsl_u64x2, 32);
    bsl_u32x4      = vcombine_u32(bsl_u32x2, bsl2_u32x2);
    bsl_u64x2      = vreinterpretq_u64_s64(vshrq_n_s64(vqsubq_s64(vreinterpretq_s64_u64(Rms3_u64x2), vreinterpretq_s64_u64(Thres_u64x2)), 64));  // sign(Rms1_64x2 - Thres_64x2)
    bsl_u32x2      = vshrn_n_u64(bsl_u64x2, 32);
    RmsdB_32x4     = vbslq_s32(bsl_u32x4, RmsdBMin_32x4, RmsdB_32x4);
    RmsdB_32x2     = vbsl_s32 (bsl_u32x2, RmsdBMin_32x2, RmsdB_32x2);

    vst1q_s32    (RmsdB_table,     RmsdB_32x4);
    vst1_lane_s32(RmsdB_table + 4, RmsdB_32x2, 0);


    NumBands = (unsigned int) HeapPtr->NumBands;
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        if(!HeapPtr->BandCompressors[bidx].Enable)
        {
            // subband disabled, no gain to apply
            HeapPtr->ComTargetGaindB[bidx] = 0;
        }
        else
        {
            KneePoints = (unsigned int) HeapPtr->BandCompressors[bidx].KneePoints;
//            if(Rms[bidx] < MDRC5B_ALMOST_ZERO_THRESH)
//            {
//                RmsdB = ((MDRC5B_POWER_DB_MINUS_INF) << 16); // Q16, [-128.0, 127.0] dB
//            }
//            else
//            {
//                double RmsdB_double;
//
//                // dB(power) = 10*log10(power)
//#ifdef SAMPLES_24_BITS
//                RmsdB_double  = 10.0 * log10(((double) Rms[bidx]) / 140737488355328.0);     // 2^47
//#else // SAMPLES_24_BITS
//                RmsdB_double  = 10.0 * log10(((double) Rms[bidx]) / 9223372036854775808.0); // 2^63
//#endif // SAMPLES_24_BITS
//                RmsdB_double += 20.0 * LOG10_OF_2 * HEADROOM;
//                RmsdB         = (MMshort) floor(RmsdB_double * 65536.0 + 0.5);              // Q16
//            }
//            RmsdB_32x2 = vdup_n_s32(RmsdB);
            RmsdB_32x2 = vdup_n_s32(RmsdB_table[bidx]);

            p_ComThreshdB    = HeapPtr->ComThreshdB[bidx][1];           // 1st knee point is at -inf, so compare with 2nd kneepoint
            ComThreshdB_32x2 = vld1_s32(p_ComThreshdB);
            Delta_32x2       = vsub_s32(RmsdB_32x2, ComThreshdB_32x2);  // Q16
            if(vget_lane_s32(Delta_32x2, 0) <= 0)
            {
                // below the 2nd knee point
                // HeapPtr->ComTargetGaindB[bidx] = wround_L(wL_msl(wL_fmul(Delta, ComSlope[0]), 7)) - (RmsdB - ComThreshdB[1]);
                p_ComSlope                     = HeapPtr->ComSlope[bidx];
                ComSlope_32x2                  = vld1_s32(p_ComSlope);
                HeapPtr->ComTargetGaindB[bidx] = vget_lane_s32(vrshrn_n_s64(vmull_s32(Delta_32x2, ComSlope_32x2), 16), 0)
                                               - vget_lane_s32(Delta_32x2, 1);
#ifdef MDRC5B_DEBUG
                printf("band %d, in db %.3f, gain %.3f, slope %.3f\n",
                       bidx,
                       (float) vget_lane_s32(RmsdB_32x2, 0)    / 65536.0,
                       (float) HeapPtr->ComTargetGaindB[bidx]  / 65536.0,
                       (float) vget_lane_s32(ComSlope_32x2, 0) / 65536.0);
#endif // MDRC5B_DEBUG
            }
            else
            {
                p_ComSlope           = &HeapPtr->ComSlope   [bidx][KneePoints - 2 - 1];    // -1 because they will be loaded 2 by 2
                p_ComThreshdB        =  HeapPtr->ComThreshdB[bidx][KneePoints - 2 - 1];    // -1 because they will be loaded 2 by 2
                remaining_KneePoints = ((KneePoints - 2) & 1);
                for(p = (KneePoints - 2) >> 1; p > 0; p--)
                {
                    ComThreshdB_32x4 = vld1q_s32(p_ComThreshdB);
                    p_ComThreshdB   -= 4;
                    ComSlope_32x2    = vld1_s32(p_ComSlope);
                    p_ComSlope      -= 2;
                    Delta_32x2       = vsub_s32(RmsdB_32x2, vget_high_s32(ComThreshdB_32x4));  // Q16
                    if(vget_lane_s32(Delta_32x2, 0) >= 0)
                    {
                        //HeapPtr->ComTargetGaindB[bidx] = wround_L(wL_msl(wL_fmul(Delta, ComSlope[p]), 7)) - (RmsdB - ComThreshdB[1]);
                        ComSlope_32x2                  = vrev64_s32(ComSlope_32x2);
                        HeapPtr->ComTargetGaindB[bidx] = vget_lane_s32(vrshrn_n_s64(vmull_s32(Delta_32x2, ComSlope_32x2), 16), 0)
                                                       - vget_lane_s32(Delta_32x2, 1);
#ifdef MDRC5B_DEBUG
                        printf("band %d, in db %.3f, gain %.3f, slope %.3f, th %.3f\n",
                               bidx,
                               (float) vget_lane_s32(RmsdB_32x2, 0)        / 65536.0,
                               (float) HeapPtr->ComTargetGaindB[bidx]      / 65536.0,
                               (float) vget_lane_s32 (ComSlope_32x2,    0) / 65536.0,
                               (float) vgetq_lane_s32(ComThreshdB_32x4, 2) / 65536.0);
#endif // MDRC5B_DEBUG
                        remaining_KneePoints = 0;
                        break;
                    }
                    Delta_32x2       = vsub_s32(RmsdB_32x2, vget_low_s32(ComThreshdB_32x4));  // Q16
                    if(vget_lane_s32(Delta_32x2, 0) >= 0)
                    {
                        //HeapPtr->ComTargetGaindB[bidx] = wround_L(wL_msl(wL_fmul(Delta, ComSlope[p]), 7)) - (RmsdB - ComThreshdB[1]);
                        HeapPtr->ComTargetGaindB[bidx] = vget_lane_s32(vrshrn_n_s64(vmull_s32(Delta_32x2, ComSlope_32x2), 16), 0)
                                                       - vget_lane_s32(Delta_32x2, 1);
#ifdef MDRC5B_DEBUG
                        printf("band %d, in db %.3f, gain %.3f, slope %.3f, th %.3f\n",
                               bidx,
                               (float) vget_lane_s32(RmsdB_32x2, 0)        / 65536.0,
                               (float) HeapPtr->ComTargetGaindB[bidx]      / 65536.0,
                               (float) vget_lane_s32 (ComSlope_32x2,    0) / 65536.0,
                               (float) vgetq_lane_s32(ComThreshdB_32x4, 0) / 65536.0);
#endif // MDRC5B_DEBUG
                        remaining_KneePoints = 0;
                        break;
                    }
                }
                if(remaining_KneePoints)
                {
                    p_ComThreshdB   += 2;
                    ComThreshdB_32x2 = vld1_s32(p_ComThreshdB);
                    Delta_32x2       = vsub_s32(RmsdB_32x2, ComThreshdB_32x2);  // Q16
                    if(vget_lane_s32(Delta_32x2, 0) <= 0)
                    {
                        //HeapPtr->ComTargetGaindB[bidx] = wround_L(wL_msl(wL_fmul(Delta, ComSlope[p]), 7)) - (RmsdB - ComThreshdB[1]);
                        ComSlope_32x2                  = vld1_s32(p_ComSlope);
                        ComSlope_32x2                  = vrev64_s32(ComSlope_32x2);
                        HeapPtr->ComTargetGaindB[bidx] = vget_lane_s32(vrshrn_n_s64(vmull_s32(Delta_32x2, ComSlope_32x2), 16), 0)
                                                       - vget_lane_s32(Delta_32x2, 1);
#ifdef MDRC5B_DEBUG
                        printf("band %d, in db %.3f, gain %.3f, slope %.3f, th %.3f\n",
                               bidx,
                               (float) vget_lane_s32(RmsdB_32x2, 0)       / 65536.0,
                               (float) HeapPtr->ComTargetGaindB[bidx]     / 65536.0,
                               (float) vget_lane_s32(ComSlope_32x2,    0) / 65536.0,
                               (float) vget_lane_s32(ComThreshdB_32x2, 0) / 65536.0);
#endif // MDRC5B_DEBUG
                    }
                }
            }
        }
    }
}


#endif // #ifdef MDRC5B_COMPRESSOR_ACTIVE
