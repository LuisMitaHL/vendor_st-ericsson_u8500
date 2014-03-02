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
  \file mdrc5b_limiter_CA9.c
  \brief processing routines of the limiter
  \author Zong Wenbo, Champsaur Ludovic
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
#include "mdrc/effect/libmdrc5b.nmf"
#endif

#include "mdrc5b_options.h"

#ifdef MDRC5B_LIMITER_ACTIVE

#include "mdrc5b_limiter.h"


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
void mdrc5b_derive_limiter_params(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    int     SampleFreq;
    MMshort  Tmp;
    MMlong  Samples;


    SampleFreq            = Heap->SampleFreq;
    Heap->LimiterLALen    = (int) ((SampleFreq * MDRC5B_LIMITER_LOOKAHEAD_DURATION) / 1000); // msec

    Tmp                   = Heap->Limiter.Thresh;
    Tmp                   = (Tmp > MDRC5B_LIMITER_THRESH_MB_MAX? MDRC5B_LIMITER_THRESH_MB_MAX: Tmp);
    Tmp                   = (Tmp < MDRC5B_LIMITER_THRESH_MB_MIN? MDRC5B_LIMITER_THRESH_MB_MIN: Tmp);
//    Heap->LimiterThreshdB = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16
    Heap->LimiterThreshdB = (Tmp << 16) / 100;


    // gain application attack coef
    Heap->LimiterAtCoef     = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_LIMITER_ATTACK_TIME_DEFAULT)) * 2147483648.0); // Q31

    // gain application release coef
    Heap->LimiterReCoef     = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_LIMITER_RELEASE_TIME_DEFAULT)) * 2147483648.0); // Q31


    // level estimation attack coef
    Heap->LimiterPeakAtCoef = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_LIMITER_EST_ATTACK_TIME_DEFAULT)) * 2147483648.0); // Q31

    // level estimation release coef
    Heap->LimiterPeakReCoef = (MMshort) (pow(0.1, 1000000.0 / ((double) SampleFreq * (double) MDRC5B_LIMITER_EST_RELEASE_TIME_DEFAULT)) * 2147483648.0); // Q31


    // hold samples
    Samples = (MMlong) SampleFreq * MDRC5B_LIMITER_HOLD_TIME;
    Heap->LimiterHtSamp = Samples / 1000000;

#ifdef MDRC5B_DEBUG
    printf("Limiter : LALen=%d, ThreshdB=%.6f, AtCoef=%.6f, ReCoef=%.6f, PeakAtCoef=%.6f, PeakReCoef=%.6f, HtSamp=%d\n",
           Heap->LimiterLALen,
           (float)Heap->LimiterThreshdB/65536.0,
           (float)Heap->LimiterAtCoef/2147483648.0,
           (float)Heap->LimiterReCoef/2147483648.0,
           (float)Heap->LimiterPeakAtCoef/2147483648.0,
           (float)Heap->LimiterPeakReCoef/2147483648.0,
           Heap->LimiterHtSamp);
#endif // MDRC5B_DEBUG
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    reset the internal variables/buffers
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_reset_limiter(MDRC5B_LOCAL_STRUCT_T *Heap)
{
    int ch, k;

    for(ch = 0; ch < MDRC5B_MAIN_CH_MAX; ch++)
    {
        Heap->LimiterPeak[ch] = 0;


        for(k = 0; k < MDRC5B_LIMITER_LOOKAHEAD_LEN; k++)
        {
            Heap->LimiterLABuf[ch][k] = 0; // buffer to subband lookahead
        }

    }

    Heap->LimiterLaIdx = 0;

    Heap->LimiterGainMant = Q31;
    Heap->LimiterGainExp = 0;

    Heap->LimiterHoldRem = 0;
    Heap->PrevShiftBits = 0;
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    main processing routine of the limiter.
            process exactly MDRC5B_BLOCK_SIZE samples
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_apply_limiter(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    unsigned int LaIdx;
    unsigned int NumMainCh;
    unsigned int Samples;
    unsigned int ch, k, n;
    MMlong       *Ptr;
    MMlong       *Ptr2;

    MMlong       *MemOutPtr;
    MMshort      PeakdB;
    MMlong       PeakMax;
    int          RmsMeasure;
    MMshort      LimiterAtCoef;
    MMshort      LimiterReCoef;
    MMshort      LimiterGainMant[MDRC5B_BLOCK_SIZE + 1];
    MMshort      LimiterGainExp;
    MMshort      LimiterTargetGaindB;
    unsigned int LimiterHoldRem;
    unsigned int LimiterHtSamp;
    MMshort      Exp, TargetGain;
    MMshort      MaxShiftBits;
    unsigned int lookahead_len = (unsigned int) HeapPtr->LimiterLALen;
    unsigned int cpt1, cpt2;
    uint32x2x2_t Temp_u32x2x2;
    uint32x2_t   Ldbits_u32x2, Ldbits2_u32x2;
    uint32x2_t   bsl_u32x2;
    int32x2_t    LimGainMant_32x2;
    int64x2_t    TempX_64x2, MemOut_64x2;
    int64x2_t    Tmp_64x2;
    int64x2_t    LimiterGainExp_64x2, sample_64x2;
    int64x1_t    TempX_64x1, sample_64x1;
    int32_t      *LimiterGainMant_ptr;
    int32x2_t    Tmp_32x2, Ldbits_32x2, n_32x2;
    int32x2_t    TempX_low_32x2, TempX_high_32x2;
    int32x2x2_t  Tmp_32x2x2;
    int64x1_t    Peak_64x1, PeakMax_64x1, Tmp_64x1, diffX_64x1;
    int64x1_t    Peak_scale_pow_64x1, Peak_scale_64x1, Zero_s64x1;
    int64x1_t    MaxShiftBits_neg_64x1, MaxShiftBits_hd_64x1;
    int64x2_t    diffX_64x2;
    uint64x1_t   bsl_u64x1;
    int32x2_t    LimiterPeakCoef_32x2, diffX_low_32x2, diffX_high_32x2;
    int32x2_t    TargetGain_32x2;
    uint32x2x2_t Peak_u32x2x2;
    uint32x2_t   Peak_exp_u32x2, Peak_exp2_u32x2, Peak_mant_u32x2;
    int32x2_t    x_32x2, xn_32x2, PeakdB_32x2, Peak_exp_32x2;
    int32x2_t    LimiterTargetGaindB_32x2, Exp_32x2, LimiterCoef_32x2;
    int32x4_t    Tmp_32x4;


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_APPLY_LIMITER)

    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT)

    Samples   = (unsigned int) HeapPtr->BlockSize;
    NumMainCh = (unsigned int) HeapPtr->NumMainCh;

    TempX_64x2 = vdupq_n_s64(0);
    for(ch = 0; ch < NumMainCh; ch++)
    {
        Ptr = HeapPtr->MainInBuf[ch];
        // compute the number of bits needs to be shifted to avoid overflow
        for(k = (Samples >> 1); k > 0; k--)
        {
            sample_64x2 = vld1q_s64(Ptr);
            Ptr        +=2;
            sample_64x2 = veorq_s64(sample_64x2, vshrq_n_s64(sample_64x2, 63));
            TempX_64x2  = vorrq_s64(TempX_64x2, sample_64x2);
        }
        if(Samples & 1)
        {
            sample_64x1 = vld1_s64(Ptr);
            sample_64x1 = veor_s64(sample_64x1, vshr_n_s64(sample_64x1, 63));
            TempX_64x2  = vorrq_s64(TempX_64x2, vcombine_s64(sample_64x1, sample_64x1));
        }
    }
    TempX_64x1    = vorr_s64(vget_low_s64(TempX_64x2), vget_high_s64(TempX_64x2));
    Temp_u32x2x2  = vuzp_u32(vreinterpret_u32_s64(TempX_64x1), vreinterpret_u32_s64(TempX_64x1));
    bsl_u32x2     = vceq_u32(Temp_u32x2x2.val[1], vdup_n_u32(0));                  // MSB == 0 ?
    // use clz instead of cls because we are sure that input value is positive
    // and because cls(LSB) could be wrong (if MSB is equal to 0 and bit 31 of LSL is 1)
    // thus clz result will be 1 more than cls result (that's why you may see (Ldbits - 1)
    // instead of Ldbits below)
    Ldbits_u32x2  = vadd_u32(vclz_u32(Temp_u32x2x2.val[0]), vdup_n_u32(32));       // clz(LSB)+32
    Ldbits2_u32x2 = vclz_u32(Temp_u32x2x2.val[1]);                                 // clz(MSB)
    Ldbits_u32x2  = vbsl_u32(bsl_u32x2, Ldbits_u32x2, Ldbits2_u32x2);              // MSB == 0 ? clz(LSB)+32 : clz(MSB)
    bsl_u32x2     = vceq_u32(Ldbits_u32x2, vdup_n_u32(64));                        // Ldbits == 64 ? (i.e. TempX == 0 ?)
    // the aim of MaxShiftBits is that sample will be shifted so that it occupies
    // 24 significant bits for 24 bits samples or 32 significant bits for 32 bits samples
    // but we are in 64 bits architecture on CA9/NEON
    // so we must right shift of ((64 - 24) - (Ldbits - 1)) bits for 24 bits samples
    // or of ((64 - 32) - (Ldbits - 1)) bits for 32 bits samples
    // and we add 1 because it was done this way on MMDSP (I don't know why !)
#ifdef SAMPLES_24_BITS
    // MaxShiftBits = ((64 - 24) - (Ldbits - 1)) + 1
    //              = 42 - Ldbits
    Ldbits_32x2     = vsub_s32(vdup_n_s32(42), vreinterpret_s32_u32(Ldbits_u32x2));
#else // SAMPLES_24_BITS
    // MaxShiftBits = ((64 - 32) - (Ldbits - 1)) + 1
    //              = 34 - Ldbits
    Ldbits_32x2     = vsub_s32(vdup_n_s32(34), vreinterpret_s32_u32(Ldbits_u32x2));
#endif // SAMPLES_24_BITS
    Ldbits_32x2     = vmax_s32(vdup_n_s32(1), Ldbits_32x2);
    Ldbits_32x2     = vbsl_s32(bsl_u32x2, vdup_n_s32(1), Ldbits_32x2);              // if(TempX == 0) Ldbits = 1
    MaxShiftBits    = vget_lane_s32(Ldbits_32x2, 0);

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT)
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT\n");
        sprintf(string, "MaxShiftBits=%d\n", MaxShiftBits);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_INSERT_NEW_SUBBAND)

    // insert the new subband samples into the lookahead buffers
    RmsMeasure = HeapPtr->Limiter.RmsMeasure;

    LaIdx = (unsigned int) HeapPtr->LimiterLaIdx;
    if(LaIdx + Samples >= lookahead_len)
    {
        cpt1                  = lookahead_len - LaIdx;
        cpt2                  = Samples - cpt1;
        // update index
        HeapPtr->LimiterLaIdx = (int) cpt2;
    }
    else
    {
        cpt1                  = Samples;
        cpt2                  = 0;
        // update index
        HeapPtr->LimiterLaIdx = (int) (LaIdx + Samples);
    }

    LimiterPeakCoef_32x2  = vdup_n_s32(HeapPtr->LimiterPeakAtCoef);                               // LimiterPeakAtCoef, LimiterPeakAtCoef
    LimiterPeakCoef_32x2  = vset_lane_s32(HeapPtr->LimiterPeakReCoef, LimiterPeakCoef_32x2, 1);   // LimiterPeakAtCoef, LimiterPeakReCoef
    Peak_scale_64x1       = vdup_n_s64(HeapPtr->PrevShiftBits - MaxShiftBits);
    Peak_scale_pow_64x1   = vshl_n_s64(Peak_scale_64x1, 1);
    MaxShiftBits_neg_64x1 = vdup_n_s64(-MaxShiftBits);
#ifdef SAMPLES_24_BITS
    MaxShiftBits_hd_64x1  = vdup_n_s64(24 - MaxShiftBits);
#else // SAMPLES_24_BITS
    MaxShiftBits_hd_64x1  = vdup_n_s64(32 - MaxShiftBits);
#endif // SAMPLES_24_BITS
    PeakMax_64x1          = vdup_n_s64(0);

    for(ch = 0; ch < NumMainCh; ch++)
    {
        Ptr  = HeapPtr->MainInBuf[ch];
        Ptr2 = HeapPtr->LimiterLABuf[ch] + LaIdx;  // go to the first valid sample

        Peak_64x1 = vdup_n_s64(HeapPtr->LimiterPeak[ch]);
        if(RmsMeasure)
        {
            // compensate Peak according to the previous shift bits
            Peak_64x1 = vqrshl_s64(Peak_64x1, Peak_scale_pow_64x1);                                 // neg value => shift right rounding

            // rms measure
            for(k = cpt1; k > 0; k--)
            {
                Tmp_64x1        = vld1_s64(Ptr);
                Ptr++;
                vst1_s64(Ptr2, Tmp_64x1);
                Ptr2++;
                Tmp_64x1        = vqrshl_s64(Tmp_64x1, MaxShiftBits_neg_64x1);
                Tmp_64x2        = vcombine_s64(Tmp_64x1, Tmp_64x1);
                Tmp_32x2x2      = vuzp_s32(vget_low_s32(vreinterpretq_s32_s64(Tmp_64x2)), vget_high_s32(vreinterpretq_s32_s64(Tmp_64x2)));
                Tmp_32x2        = Tmp_32x2x2.val[0];                                                // LSB of Tmp_64x2 (MSB is dummy)
                TempX_64x2      = vqdmull_s32(Tmp_32x2, Tmp_32x2);
                TempX_64x1      = vget_low_s64(TempX_64x2);
                diffX_64x1      = vqsub_s64(Peak_64x1, TempX_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 63));                 // sign(diffX)
                diffX_64x2      = vcombine_s64(diffX_64x1, diffX_64x1);
                diffX_low_32x2  = vshrn_n_s64(vshlq_n_s64(diffX_64x2, 32), 32);                     // wextract_l(diffX), wextract_l(diffX)
                diffX_high_32x2 = vrshrn_n_s64(diffX_64x2, 32);                                     // wround_L(diffX), wround_L(diffX)
                Tmp_64x2        = vmovl_s32(vqrdmulh_s32(LimiterPeakCoef_32x2, diffX_low_32x2));    // (MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), (MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef)
                Tmp_64x2        = vqdmlal_s32(Tmp_64x2, LimiterPeakCoef_32x2, diffX_high_32x2);     // wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), wL_fmul(wround_L(diffX), LimiterPeakAtCoef)), wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef), wL_fmul(wround_L(diffX), LimiterPeakReCoef))
                Tmp_64x2        = vqaddq_s64(TempX_64x2, Tmp_64x2);
                Peak_64x1       = vbsl_s64(bsl_u64x1, vget_low_s64(Tmp_64x2), vget_high_s64(Tmp_64x2));
                Tmp_64x1        = vqsub_s64(Peak_64x1, PeakMax_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Peak_64x1 - PeakMax_64x1)
                PeakMax_64x1    = vbsl_s64(bsl_u64x1, PeakMax_64x1, Peak_64x1);
            }
            Ptr2 = HeapPtr->LimiterLABuf[ch];
            for(k = cpt2; k > 0; k--)
            {
                Tmp_64x1        = vld1_s64(Ptr);
                Ptr++;
                vst1_s64(Ptr2, Tmp_64x1);
                Ptr2++;
                Tmp_64x1        = vqrshl_s64(Tmp_64x1, MaxShiftBits_neg_64x1);
                Tmp_64x2        = vcombine_s64(Tmp_64x1, Tmp_64x1);
                Tmp_32x2x2      = vuzp_s32(vget_low_s32(vreinterpretq_s32_s64(Tmp_64x2)), vget_high_s32(vreinterpretq_s32_s64(Tmp_64x2)));
                Tmp_32x2        = Tmp_32x2x2.val[0];                                                // LSB of Tmp_64x2 (MSB is dummy)
                TempX_64x2      = vqdmull_s32(Tmp_32x2, Tmp_32x2);
                TempX_64x1      = vget_low_s64(TempX_64x2);
                diffX_64x1      = vqsub_s64(Peak_64x1, TempX_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 63));                 // sign(diffX)
                diffX_64x2      = vcombine_s64(diffX_64x1, diffX_64x1);
                diffX_low_32x2  = vshrn_n_s64(vshlq_n_s64(diffX_64x2, 32), 32);                     // wextract_l(diffX), wextract_l(diffX)
                diffX_high_32x2 = vrshrn_n_s64(diffX_64x2, 32);                                     // wround_L(diffX), wround_L(diffX)
                Tmp_64x2        = vmovl_s32(vqrdmulh_s32(LimiterPeakCoef_32x2, diffX_low_32x2));    // (MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), (MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef)
                Tmp_64x2        = vqdmlal_s32(Tmp_64x2, LimiterPeakCoef_32x2, diffX_high_32x2);     // wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), wL_fmul(wround_L(diffX), LimiterPeakAtCoef)), wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef), wL_fmul(wround_L(diffX), LimiterPeakReCoef))
                Tmp_64x2        = vqaddq_s64(TempX_64x2, Tmp_64x2);
                Peak_64x1       = vbsl_s64(bsl_u64x1, vget_low_s64(Tmp_64x2), vget_high_s64(Tmp_64x2));
                Tmp_64x1        = vqsub_s64(Peak_64x1, PeakMax_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Peak_64x1 - PeakMax_64x1)
                PeakMax_64x1    = vbsl_s64(bsl_u64x1, PeakMax_64x1, Peak_64x1);
            }
        }
        else
        {
            // compensate Peak according to the previous shift bits
            Peak_64x1  = vqrshl_s64(Peak_64x1, Peak_scale_64x1);

            // amplitude measure
            Zero_s64x1 = vdup_n_s64(0);
            for(k = cpt1; k > 0; k--)
            {
                Tmp_64x1        = vld1_s64(Ptr);
                Ptr++;
                vst1_s64(Ptr2, Tmp_64x1);
                Ptr2++;
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Tmp_64x1)
                TempX_64x1      = vqsub_s64(Zero_s64x1, Tmp_64x1);                                  // -Tmp_64x1
                TempX_64x1      = vbsl_s64(bsl_u64x1, TempX_64x1, Tmp_64x1);
                TempX_64x1      = vqrshl_s64(TempX_64x1, MaxShiftBits_hd_64x1);
                TempX_64x2      = vcombine_s64(TempX_64x1, TempX_64x1);
                diffX_64x1      = vqsub_s64(Peak_64x1, TempX_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 63));                 // sign(diffX)
                diffX_64x2      = vcombine_s64(diffX_64x1, diffX_64x1);
                diffX_low_32x2  = vshrn_n_s64(vshlq_n_s64(diffX_64x2, 32), 32);                     // wextract_l(diffX), wextract_l(diffX)
                diffX_high_32x2 = vrshrn_n_s64(diffX_64x2, 32);                                     // wround_L(diffX), wround_L(diffX)
                Tmp_64x2        = vmovl_s32(vqrdmulh_s32(LimiterPeakCoef_32x2, diffX_low_32x2));    // (MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), (MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef)
                Tmp_64x2        = vqdmlal_s32(Tmp_64x2, LimiterPeakCoef_32x2, diffX_high_32x2);     // wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), wL_fmul(wround_L(diffX), LimiterPeakAtCoef)), wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef), wL_fmul(wround_L(diffX), LimiterPeakReCoef))
                Tmp_64x2        = vqaddq_s64(TempX_64x2, Tmp_64x2);
                Peak_64x1       = vbsl_s64(bsl_u64x1, vget_low_s64(Tmp_64x2), vget_high_s64(Tmp_64x2));
                Tmp_64x1        = vqsub_s64(Peak_64x1, PeakMax_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Peak_64x1 - PeakMax_64x1)
                PeakMax_64x1    = vbsl_s64(bsl_u64x1, PeakMax_64x1, Peak_64x1);
            }
            Ptr2 = HeapPtr->LimiterLABuf[ch];
            for(k = cpt2; k > 0; k--)
            {
                Tmp_64x1        = vld1_s64(Ptr);
                Ptr++;
                vst1_s64(Ptr2, Tmp_64x1);
                Ptr2++;
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Tmp_64x1)
                TempX_64x1      = vqsub_s64(Zero_s64x1, Tmp_64x1);                                  // -Tmp_64x1
                TempX_64x1      = vbsl_s64(bsl_u64x1, TempX_64x1, Tmp_64x1);
                TempX_64x1      = vqrshl_s64(TempX_64x1, MaxShiftBits_hd_64x1);
                TempX_64x2      = vcombine_s64(TempX_64x1, TempX_64x1);
                diffX_64x1      = vqsub_s64(Peak_64x1, TempX_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(diffX_64x1, 63));                 // sign(diffX)
                diffX_64x2      = vcombine_s64(diffX_64x1, diffX_64x1);
                diffX_low_32x2  = vshrn_n_s64(vshlq_n_s64(diffX_64x2, 32), 32);                     // wextract_l(diffX), wextract_l(diffX)
                diffX_high_32x2 = vrshrn_n_s64(diffX_64x2, 32);                                     // wround_L(diffX), wround_L(diffX)
                Tmp_64x2        = vmovl_s32(vqrdmulh_s32(LimiterPeakCoef_32x2, diffX_low_32x2));    // (MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), (MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef)
                Tmp_64x2        = vqdmlal_s32(Tmp_64x2, LimiterPeakCoef_32x2, diffX_high_32x2);     // wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakAtCoef), wL_fmul(wround_L(diffX), LimiterPeakAtCoef)), wL_addsat((MMlong) wfmulr(wextract_l(diffX), LimiterPeakReCoef), wL_fmul(wround_L(diffX), LimiterPeakReCoef))
                Tmp_64x2        = vqaddq_s64(TempX_64x2, Tmp_64x2);
                Peak_64x1       = vbsl_s64(bsl_u64x1, vget_low_s64(Tmp_64x2), vget_high_s64(Tmp_64x2));
                Tmp_64x1        = vqsub_s64(Peak_64x1, PeakMax_64x1);
                bsl_u64x1       = vreinterpret_u64_s64(vshr_n_s64(Tmp_64x1, 63));                   // sign(Peak_64x1 - PeakMax_64x1)
                PeakMax_64x1    = vbsl_s64(bsl_u64x1, PeakMax_64x1, Peak_64x1);
            }
        }

        HeapPtr->LimiterPeak[ch] = vget_lane_s64(Peak_64x1, 0);                                     // save history
    }  // for(ch = 0...)
    PeakMax                = vget_lane_s64(PeakMax_64x1, 0);
    HeapPtr->PrevShiftBits = MaxShiftBits;

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_INSERT_NEW_SUBBAND)


    if(PeakMax < MDRC5B_ALMOST_ZERO_THRESH)
    {
        PeakdB = (MDRC5B_POWER_DB_MINUS_INF << 16); // 8.16, [-128.0, 127.0] dB
    }
    else
    {
        Peak_u32x2x2    = vuzp_u32(vreinterpret_u32_s64(PeakMax_64x1), vreinterpret_u32_s64(PeakMax_64x1));
        bsl_u32x2       = vceq_u32(Peak_u32x2x2.val[1], vdup_n_u32(0));
        Peak_exp_u32x2  = vadd_u32(vclz_u32(Peak_u32x2x2.val[0]), vdup_n_u32(32));
        Peak_exp2_u32x2 = vclz_u32(Peak_u32x2x2.val[1]);
        Peak_exp_u32x2  = vbsl_u32(bsl_u32x2, Peak_exp_u32x2, Peak_exp2_u32x2);
        Peak_mant_u32x2 = vrshrn_n_u64(vshlq_u64(vreinterpretq_u64_s64(vcombine_s64(PeakMax_64x1, PeakMax_64x1)), vreinterpretq_s64_u64(vmovl_u32(Peak_exp_u32x2))), 32);

        // if(Peak_mant >= sqrt(0.5))
        // {
        //     Peak_exp--;
        //     Peak_mant >>= 1;
        // }
        bsl_u32x2       = vcge_u32(Peak_mant_u32x2, vdup_n_u32(0xB504F334));
        Peak_exp_u32x2  = vbsl_u32(bsl_u32x2, vsub_u32(Peak_exp_u32x2, vdup_n_u32(1)), Peak_exp_u32x2);
        Peak_mant_u32x2 = vbsl_u32(bsl_u32x2, vrshr_n_u32(Peak_mant_u32x2, 1), Peak_mant_u32x2);

        Peak_exp_32x2 = vreinterpret_s32_u32(Peak_exp_u32x2);
#ifdef SAMPLES_24_BITS
        // correction of 16 bits if input samples are 24 bits
        Peak_exp_32x2 = vsub_s32(Peak_exp_32x2, vdup_n_s32(16));
#endif // SAMPLES_24_BITS

        // at this point : sqrt(0.5)/2 <= Peak_mant < sqrt(0.5)
        //
        // ln(1+x) = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9 - x^10/10 ...    accuracy OK if |x| < 0.5
        // sqrt(0.5)/2 <= Peak_mant < sqrt(0.5)  =>  sqrt(0.5)-1 <= 2*Peak_mant-1 < 2*sqrt(0.5)-1
        //                                       =>  ln(Peak_mant) = ln(1+x)-ln(2) with x=2*Peak_mant-1, i.e. |x| < 0.414214...

        // x=2*PeakMax_mant-1 in Q31
        // => sqrt(0.5)-1 <= x < 2*sqrt(0.5)-1
        x_32x2      = vreinterpret_s32_u32(vsub_u32(Peak_mant_u32x2, vdup_n_u32(0x80000000)));

        PeakdB_32x2 = x_32x2;                                                                     // PeakdB = x

        xn_32x2     = vqrdmulh_s32(x_32x2, x_32x2);                                               // xn = x^2
        PeakdB_32x2 = vqsub_s32(PeakdB_32x2, vrshr_n_s32(xn_32x2, 1));                            // PeakdB = x - x^2/2

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^3
        PeakdB_32x2 = vqadd_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x2AAAAAAB)));      // PeakdB = x - x^2/2 + x^3/3

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^4
        PeakdB_32x2 = vqsub_s32(PeakdB_32x2, vrshr_n_s32(xn_32x2, 2));                            // PeakdB = x - x^2/2 + x^3/3 - x^4/4

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^5
        PeakdB_32x2 = vqadd_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x1999999A)));      // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^6
        PeakdB_32x2 = vqsub_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x15555555)));      // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^7
        PeakdB_32x2 = vqadd_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x12492492)));      // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^8
        PeakdB_32x2 = vqsub_s32(PeakdB_32x2, vrshr_n_s32(xn_32x2, 3));                            // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^9
        PeakdB_32x2 = vqadd_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x0E38E38E)));      // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9

        xn_32x2     = vqrdmulh_s32(xn_32x2, x_32x2);                                              // xn = x^10
        PeakdB_32x2 = vqsub_s32(PeakdB_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x0CCCCCCD)));      // PeakdB = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - x^6/6 + x^7/7 - x^8/8 + x^9/9 - x^10/10

        // at this point : PeakMaxdB contains ln(1+x) in Q31

        if(RmsMeasure)
        {
            // dB(power) = 10*log10(power)

            // PeakMaxdB = 10*log10(PeakMax)+20*log10(2)*(HEADROOM+MaxShiftBits)
            //           = 10*ln(PeakMax)/ln(10)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 10/ln(10)*ln(PeakMax_mant*2^(-PeakMax_exp))+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 10/ln(10)*(ln(PeakMax_mant)-PeakMax_exp*ln(2))+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 10/ln(10)*ln(PeakMax_mant)-PeakMax_exp*10*ln(2)/ln(10)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 10/ln(10)*ln(PeakMax_mant)+10*ln(2)/ln(10)*(2*(HEADROOM+MaxShiftBits)-PeakMax_exp)
            //
            // => RmsdB = 10/ln(10)*ln(1+x)+10*ln(2)/ln(10)*(2*(HEADROOM+MaxShiftBits)-PeakMax_exp)
            // => RmsdB (Q16) = 0x457CB*ln(1+x)+0x302A3*(2*(HEADROOM+MaxShiftBits)-PeakMax_exp)

            // fractional mutiply 0x457CB*ln(1+x) in Q16
            PeakdB_32x2   = vqrdmulh_s32(PeakdB_32x2, vdup_n_s32(0x457CB));

            // PeakdB_exp = 2*(HEADROOM+MaxShiftBits)-PeakdB_exp
            Peak_exp_32x2 = vsub_s32(vdup_n_s32(2 * (HEADROOM + MaxShiftBits)), Peak_exp_32x2);

            // PeakMaxdB final value (integer mac 0x302A3*PeakdB_exp)
            PeakdB_32x2   = vmla_s32(PeakdB_32x2, Peak_exp_32x2, vdup_n_s32(0x302A3));
        }
        else
        {
            // dB(power) = 20*log10(abs)

            // PeakMaxdB = 20*log10(PeakMax)+20*log10(2)*(HEADROOM+MaxShiftBits)
            //           = 20*ln(PeakMax)/ln(10)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 20/ln(10)*ln(PeakMax_mant*2^(-PeakMax_exp))+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 20/ln(10)*(ln(PeakMax_mant)-PeakMax_exp*ln(2))+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 20/ln(10)*ln(PeakMax_mant)-PeakMax_exp*20*ln(2)/ln(10)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits)
            //           = 20/ln(10)*ln(PeakMax_mant)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits-PeakMax_exp)
            //
            // => RmsdB = 20/ln(10)*ln(1+x)+20*ln(2)/ln(10)*(HEADROOM+MaxShiftBits-PeakMax_exp)
            // => RmsdB (Q16) = 0x8AF96*ln(1+x)+0x60546*(HEADROOM+MaxShiftBits-PeakMax_exp)

            // fractional mutiply 0x8AF96*ln(1+x) in Q16
            PeakdB_32x2     = vqrdmulh_s32(PeakdB_32x2, vdup_n_s32(0x8AF96));

            // PeakdB_exp = HEADROOM+MaxShiftBits-PeakdB_exp
            Peak_exp_32x2 = vsub_s32(vdup_n_s32(HEADROOM + MaxShiftBits), Peak_exp_32x2);

            // PeakMaxdB final value (integer mac 0x60546*PeakdB_exp)
            PeakdB_32x2     = vmla_s32(PeakdB_32x2, Peak_exp_32x2, vdup_n_s32(0x60546));
        }
        PeakdB = vget_lane_s32(PeakdB_32x2, 0);
    }
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_PEAKMAX_PEAKDB\n");
        sprintf(string, "PeakMax=0x%012llX, HEADROOM+MaxShiftBits=%d => PeakdB=0x%06X\n",
#ifdef SAMPLES_24_BITS
                        PeakMax & 0xFFFFFFFFFFFFLL,
#else // SAMPLES_24_BITS
                        (PeakMax >> 16) & 0xFFFFFFFFFFFFLL,
#endif // SAMPLES_24_BITS
                        HEADROOM + MaxShiftBits,
                        PeakdB & 0xFFFFFF);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT


    // smooth gains
    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_SMOOTH_GAIN)

    LimiterTargetGaindB = HeapPtr->LimiterThreshdB - PeakdB; // 8.16, [-128.0, 127.0] dB
    if(LimiterTargetGaindB < 0)
    {
        // linear_gain = 10^(gain_dB/20)
        //             = 2^gain_pow2
        //
        // with gain_pow2 = log2(linear_gain)
        //                = log10(linear_gain)/log10(2)
        //                = gain_dB/20/log10(2)
        //                = FinalGainQ16/2^16/20/log10(2)
        LimiterTargetGaindB_32x2 = vqrdmulh_s32(vdup_n_s32(LimiterTargetGaindB), vdup_n_s32(0x1542A5A1));
        Exp_32x2                 = vshr_n_s32(LimiterTargetGaindB_32x2, 16);
        Exp_32x2                 = vadd_s32(Exp_32x2, vdup_n_s32(1));
        LimiterTargetGaindB_32x2 = vsub_s32(LimiterTargetGaindB_32x2, vshl_n_s32(Exp_32x2, 16));

        // -0.5 < LimiterTargetGaindB < 0 in Q16
        // 2^LimiterTargetGaindB = exp(LimiterTargetGaindB*ln(2))
        // exp(x) = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040 + ...    accuracy OK if |x| < 0.1
        // -0.5*ln(2) < LimiterTargetGaindB*ln(2) < 0 => |LimiterTargetGaindB*ln(2)/4| < 0.1
        // exp(x) = (exp(x/4))^4 => 2^LimiterTargetGaindB = (1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040)^4
        // with x=LimiterTargetGaindB*ln(2)/4 with good accuracy

        x_32x2          = vqrdmulh_s32(LimiterTargetGaindB_32x2, vdup_n_s32(0x58B90BFC));               // LimiterTargetGaindB*ln(2) in Q16, i.e. LimiterTargetGaindB*ln(2)/4 in Q18

        x_32x2          = vshl_n_s32(x_32x2, 13);                                                       // x = LimiterTargetGaindB*ln(2)/4 in Q31

        TargetGain_32x2 = vdup_n_s32(Q31);                                                              // TargetGain = 1

        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, x_32x2);                                           // TargetGain = 1 + x

        xn_32x2         = vqrdmulh_s32(x_32x2, x_32x2);                                                 // xn = x^2
        TargetGain_32x2 = vrsra_n_s32(TargetGain_32x2, xn_32x2, 1);                                     // TargetGain = 1 + x + x^2/2

        xn_32x2         = vqrdmulh_s32(xn_32x2, x_32x2);                                                // xn = x^3
        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x15555555)));    // TargetGain = 1 + x + x^2/2 + x^3/6

        xn_32x2         = vqrdmulh_s32(xn_32x2, x_32x2);                                                // xn = x^4
        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x05555555)));    // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24

        xn_32x2         = vqrdmulh_s32(xn_32x2, x_32x2);                                                // xn = x^5
        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x01111111)));    // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120

        xn_32x2         = vqrdmulh_s32(xn_32x2, x_32x2);                                                // xn = x^6
        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x002D82D8)));    // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720

        xn_32x2         = vqrdmulh_s32(xn_32x2, x_32x2);                                                // xn = x^7
        TargetGain_32x2 = vqadd_s32(TargetGain_32x2, vqrdmulh_s32(xn_32x2, vdup_n_s32(0x00068068)));    // TargetGain = 1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040

        TargetGain_32x2 = vqrdmulh_s32(TargetGain_32x2, TargetGain_32x2);                               // TargetGain = (1 + x + x^2/2 + x^3/6 + x^4/24 + x^5/120 + x^6/720 + x^7/5040)^4
        TargetGain_32x2 = vqrdmulh_s32(TargetGain_32x2, TargetGain_32x2);                               //

        Exp             = vget_lane_s32(Exp_32x2, 0);
        TargetGain      = vget_lane_s32(TargetGain_32x2, 0);
    }
    else
    {
        LimiterTargetGaindB = 0;
        TargetGain          = (1 << 30);
        Exp                 = 1;
    }

    LimiterGainMant[0] = HeapPtr->LimiterGainMant;
    LimiterGainExp     = HeapPtr->LimiterGainExp;
    if(LimiterGainExp >= Exp)
    {
        TargetGain >>= (LimiterGainExp - Exp);
    }
    else
    {
        LimiterGainMant[0] >>= (Exp - LimiterGainExp);
        LimiterGainExp = Exp;
    }
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_TARGETGAIN_EXP\n");
        sprintf(string, "TargetGain=0x%06X, Exp=%d\n", (TargetGain >> 8), Exp);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT

#ifdef MDRC5B_DEBUG
    printf("%ld, peak %.3f, gain %.3f, exp %d, mant %.6f, %.6f\n", counter, (float)PeakdB/65536.0, (float)LimiterTargetGaindB/65536.0, LimiterGainExp, (float)LimiterGainMant[0]/8388608.0, (float)TargetGain/8388608.0);
#endif // MDRC5B_DEBUG

    LimiterAtCoef  = HeapPtr->LimiterAtCoef;
    LimiterReCoef  = HeapPtr->LimiterReCoef;
    LimiterHoldRem = HeapPtr->LimiterHoldRem;
    LimiterHtSamp  = HeapPtr->LimiterHtSamp;

    if(TargetGain < LimiterGainMant[0]) // k refers to the previous sample, k+1 to the current sample
    {
        // attack
        Tmp_32x2            = vdup_n_s32(LimiterGainMant[0]);
        TargetGain_32x2     = vdup_n_s32(TargetGain);
        LimiterCoef_32x2    = vdup_n_s32(LimiterAtCoef);
        LimiterGainMant_ptr = &LimiterGainMant[1];
        for(k = Samples; k > 0 ; k--)
        {
            Tmp_32x2 = vqsub_s32(Tmp_32x2, TargetGain_32x2);
            Tmp_32x2 = vqrdmulh_s32(LimiterCoef_32x2, Tmp_32x2);
            Tmp_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);
            vst1_lane_s32(LimiterGainMant_ptr, Tmp_32x2, 0);
            LimiterGainMant_ptr++;
        }
        LimiterHoldRem = LimiterHtSamp; // init hold time
    }
    else if(TargetGain > LimiterGainMant[0])
    {
        // release
        Tmp_32x4            = vdupq_n_s32(LimiterGainMant[0]);
        n                   = (LimiterHoldRem < Samples ? LimiterHoldRem : Samples);
        LimiterHoldRem     -= n;
        LimiterGainMant_ptr = &LimiterGainMant[1];
        for(k = (n >> 2); k > 0; k--)
        {
            vst1q_s32(LimiterGainMant_ptr, Tmp_32x4);
            LimiterGainMant_ptr += 4;
        }
        k = (n & 3);
        switch(k)
        {
            case 3:
                vst1q_lane_s32(LimiterGainMant_ptr + 2, Tmp_32x4, 2);
            case 2:
                vst1_s32(LimiterGainMant_ptr, vget_low_s32(Tmp_32x4));
                break;
            case 1:
                vst1q_lane_s32(LimiterGainMant_ptr, Tmp_32x4, 0);
            default:
                break;
        }
        LimiterGainMant_ptr += k;

        Tmp_32x2             = vget_low_s32(Tmp_32x4);
        TargetGain_32x2      = vdup_n_s32(TargetGain);
        LimiterCoef_32x2     = vdup_n_s32(LimiterReCoef);
        for(k = Samples - n; k > 0; k--)
        {
            Tmp_32x2 = vqsub_s32(Tmp_32x2, TargetGain_32x2);
            Tmp_32x2 = vqrdmulh_s32(LimiterCoef_32x2, Tmp_32x2);
            Tmp_32x2 = vqadd_s32(TargetGain_32x2, Tmp_32x2);
            vst1_lane_s32(LimiterGainMant_ptr, Tmp_32x2, 0);
            LimiterGainMant_ptr++;
        }
    }
    else
    {
        // TargetGain == LimiterGainMant[0] => no need to smooth
        Tmp_32x4            = vdupq_n_s32(TargetGain);
        LimiterGainMant_ptr = &LimiterGainMant[1];
        for(k = (Samples >> 2); k > 0; k--)
        {
            vst1q_s32(LimiterGainMant_ptr, Tmp_32x4);
            LimiterGainMant_ptr += 4;
        }
        k = (Samples & 3);
        switch(k)
        {
            case 3:
                vst1q_lane_s32(LimiterGainMant_ptr + 2, Tmp_32x4, 2);
            case 2:
                vst1_s32(LimiterGainMant_ptr, vget_low_s32(Tmp_32x4));
                break;
            case 1:
                vst1q_lane_s32(LimiterGainMant_ptr, Tmp_32x4, 0);
            default:
                break;
        }
        Tmp_32x2             = vget_low_s32(Tmp_32x4);
    }
    n_32x2                   = vcls_s32(Tmp_32x2);        // dummy vect to use vcls
    Tmp_32x2                 = vshl_s32(Tmp_32x2, n_32x2);
    n                        = vget_lane_s32(n_32x2, 0);
    HeapPtr->LimiterGainMant = vget_lane_s32(Tmp_32x2, 0);
    HeapPtr->LimiterGainExp  = LimiterGainExp - n;
    HeapPtr->LimiterHoldRem  = LimiterHoldRem;

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_SMOOTH_GAIN)
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_SMOOTH_GAIN\n");
        sprintf(string, "TargetGain         = %d\n", (TargetGain         >> 8));
        debug_write_string(string);
        sprintf(string, "LimiterGainMant[0] = %d\n", (LimiterGainMant[0] >> 8));
        debug_write_string(string);
        sprintf(string, "LimiterAtCoef      = %d\n", (LimiterAtCoef      >> 8));
        debug_write_string(string);
        sprintf(string, "LimiterReCoef      = %d\n", (LimiterReCoef      >> 8));
        debug_write_string(string);
        sprintf(string, "LimiterHoldRem     = %d\n", (LimiterHoldRem     >> 8));
        debug_write_string(string);
        sprintf(string, "LimiterHtSamp      = %d\n", (LimiterHtSamp      >> 8));
        for(k = 0; k < Samples; k++)
        {
            sprintf(string, "0x%06X ", (LimiterGainMant[k + 1] >> 8));
            debug_write_string(string);
        }
        debug_write_string("\n");
    }
#endif  // DEBUG_LIMITER_OUTPUT


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_APPLY_GAIN)
    // output
    LaIdx = (unsigned int) HeapPtr->LimiterLaIdx;         // this is now the oldest sample
    if(LaIdx + Samples >= lookahead_len)
    {
        cpt1 = lookahead_len - LaIdx;
        cpt2 = Samples - cpt1;
    }
    else
    {
        cpt1 = Samples;
        cpt2 = 0;
    }
    LimiterGainExp_64x2 = vdupq_n_s64(LimiterGainExp);
    for(ch = 0; ch < NumMainCh; ch++)
    {
        MemOutPtr           = HeapPtr->MainInBuf[ch];
        Ptr2                = HeapPtr->LimiterLABuf[ch] + LaIdx;  // go to the first valid sample
        LimiterGainMant_ptr = &LimiterGainMant[1];
        for(k = (cpt1 >> 1); k > 0; k--)
        {
            LimGainMant_32x2     = vld1_s32(LimiterGainMant_ptr);
            LimiterGainMant_ptr += 2;
            TempX_64x2           = vld1q_s64(Ptr2);
            Ptr2                += 2;
            TempX_low_32x2       = vshrn_n_s64(vshlq_n_s64(TempX_64x2, 32), 32);                     // wextract_l(TempX), wextract_l(TempX)
            TempX_high_32x2      = vrshrn_n_s64(TempX_64x2, 32);                                     // wround_L(TempX), wround_L(TempX)
            Tmp_64x2             = vmovl_s32(vqrdmulh_s32(TempX_low_32x2, LimGainMant_32x2));        // (MMlong) wfmulr(wextract_l(TempX), LimGainMant), (MMlong) wfmulr(wextract_l(TempX), LimGainMant)
            MemOut_64x2          = vqdmlal_s32(Tmp_64x2, TempX_high_32x2, LimGainMant_32x2);         // wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant)), wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant))
            MemOut_64x2          = vqshlq_s64(MemOut_64x2, LimiterGainExp_64x2);
            vst1q_s64(MemOutPtr, MemOut_64x2);
            MemOutPtr           += 2;
        }
        if(cpt1 & 1)
        {
            LimGainMant_32x2     = vld1_s32(LimiterGainMant_ptr);
            TempX_64x2           = vld1q_s64(Ptr2);
            TempX_low_32x2       = vshrn_n_s64(vshlq_n_s64(TempX_64x2, 32), 32);                     // wextract_l(TempX), wextract_l(TempX)
            TempX_high_32x2      = vrshrn_n_s64(TempX_64x2, 32);                                     // wround_L(TempX), wround_L(TempX)
            Tmp_64x2             = vmovl_s32(vqrdmulh_s32(TempX_low_32x2, LimGainMant_32x2));        // (MMlong) wfmulr(wextract_l(TempX), LimGainMant), (MMlong) wfmulr(wextract_l(TempX), LimGainMant)
            MemOut_64x2          = vqdmlal_s32(Tmp_64x2, TempX_high_32x2, LimGainMant_32x2);         // wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant)), wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant))
            MemOut_64x2          = vqshlq_s64(MemOut_64x2, LimiterGainExp_64x2);
            vst1q_lane_s64(MemOutPtr, MemOut_64x2, 0);
            MemOutPtr++;
        }
        Ptr2 = HeapPtr->LimiterLABuf[ch];
        for(k = (cpt2 >> 1); k > 0; k--)
        {
            LimGainMant_32x2     = vld1_s32(LimiterGainMant_ptr);
            LimiterGainMant_ptr += 2;
            TempX_64x2           = vld1q_s64(Ptr2);
            Ptr2                += 2;
            TempX_low_32x2       = vshrn_n_s64(vshlq_n_s64(TempX_64x2, 32), 32);                     // wextract_l(TempX), wextract_l(TempX)
            TempX_high_32x2      = vrshrn_n_s64(TempX_64x2, 32);                                     // wround_L(TempX), wround_L(TempX)
            Tmp_64x2             = vmovl_s32(vqrdmulh_s32(TempX_low_32x2, LimGainMant_32x2));        // (MMlong) wfmulr(wextract_l(TempX), LimGainMant), (MMlong) wfmulr(wextract_l(TempX), LimGainMant)
            MemOut_64x2          = vqdmlal_s32(Tmp_64x2, TempX_high_32x2, LimGainMant_32x2);         // wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant)), wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant))
            MemOut_64x2          = vqshlq_s64(MemOut_64x2, LimiterGainExp_64x2);
            vst1q_s64(MemOutPtr, MemOut_64x2);
            MemOutPtr           += 2;
        }
        if(cpt2 & 1)
        {
            LimGainMant_32x2     = vld1_s32(LimiterGainMant_ptr);
            TempX_64x2           = vld1q_s64(Ptr2);
            TempX_low_32x2       = vshrn_n_s64(vshlq_n_s64(TempX_64x2, 32), 32);                     // wextract_l(TempX), wextract_l(TempX)
            TempX_high_32x2      = vrshrn_n_s64(TempX_64x2, 32);                                     // wround_L(TempX), wround_L(TempX)
            Tmp_64x2             = vmovl_s32(vqrdmulh_s32(TempX_low_32x2, LimGainMant_32x2));        // (MMlong) wfmulr(wextract_l(TempX), LimGainMant), (MMlong) wfmulr(wextract_l(TempX), LimGainMant)
            MemOut_64x2          = vqdmlal_s32(Tmp_64x2, TempX_high_32x2, LimGainMant_32x2);         // wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant)), wL_addsat((MMlong) wfmulr(wextract_l(TempX), LimGainMant), wL_fmul(wround_L(TempX), LimGainMant))
            MemOut_64x2          = vqshlq_s64(MemOut_64x2, LimiterGainExp_64x2);
            vst1q_lane_s64(MemOutPtr, MemOut_64x2, 0);
        }
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_LIMITER_APPLY_GAIN)

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_APPLY_LIMITER)

#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        debug_write_string("MRDC5B_LIMITER_APPLY_GAIN\n");
        for(k = 0; k < Samples; k++)
        {
            debug_write_string("| ");
            for(ch = 0; ch < NumMainCh; ch++)
            {
                debug_write_limiter_output(ch, (int) HeapPtr->MainInBuf[ch][k]);
            }
            debug_write_string("|\n");
        }
    }
#endif  // DEBUG_LIMITER_OUTPUT
}

#endif // #ifdef MDRC5B_LIMITER_ACTIVE
