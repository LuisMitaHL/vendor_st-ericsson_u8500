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
  \file mdrc5b_compressor.c
  \brief processing routines of the multi-band DRC
  \author Zong Wenbo
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev DEC 18, 2009
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
#include "mdrc5b_dsp.h"
#include "mdrc5b_rom.h"

static void mdrc5b_compressor_gain(MDRC5B_LOCAL_STRUCT_T *HeapPtr, MMlong *Rms);
static void mdrc5b_compute_filter_coef(int SampleFreq, int FreqCutoff, MMshort FILTER_COEF *FilterL, MMshort FILTER_COEF *FilterH);


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
    int NumBands;
    int SampleFreq;
    int FreqCutoff;
    int bidx;

    if(Heap->mdrc_filter_kind == MDRC_LEGACY_FILTER)
    {
        // compute internal variables according to settings
        NumBands   = Heap->NumBands;
        SampleFreq = Heap->SampleFreq;

        if(NumBands > 1)
        {
            // Freqcutoff is the lower boundary of the band
            for(bidx = 1; bidx < NumBands; bidx++)
            {
                FreqCutoff = Heap->BandCompressors[bidx].FreqCutoff;

                mdrc5b_compute_filter_coef(SampleFreq, FreqCutoff, Heap->p_filter_coef->LPFilt[bidx-1], Heap->p_filter_coef->HPFilt[bidx-1]);

#ifdef MDRC5B_DEBUG
                printf("FreqCutoff%d=%d\n", bidx, FreqCutoff);
                printf("lb%d=[%.10f %.10f %.10f]\n", bidx, (float)Heap->p_filter_coef->LPFilt[bidx-1][0]/8388608.0, (float)Heap->p_filter_coef->LPFilt[bidx-1][1]*(-2.0)/8388608.0, (float)Heap->p_filter_coef->LPFilt[bidx-1][2]/8388608.0);
                printf("la%d=[%.10f %.10f %.10f]\n", bidx, 1.0,                                                     (float)Heap->p_filter_coef->LPFilt[bidx-1][3]*(-2.0)/8388608.0, (float)Heap->p_filter_coef->LPFilt[bidx-1][4]/8388608.0);
                printf("hb%d=[%.10f %.10f %.10f]\n", bidx, (float)Heap->p_filter_coef->HPFilt[bidx-1][0]/8388608.0, (float)Heap->p_filter_coef->HPFilt[bidx-1][1]*(-2.0)/8388608.0, (float)Heap->p_filter_coef->HPFilt[bidx-1][2]/8388608.0);
                printf("ha%d=[%.10f %.10f %.10f]\n", bidx, 1.0,                                                     (float)Heap->p_filter_coef->HPFilt[bidx-1][3]*(-2.0)/8388608.0, (float)Heap->p_filter_coef->HPFilt[bidx-1][4]/8388608.0);
#endif
            }
        }
    }
}


static void mdrc5b_compute_filter_coef(int SampleFreq, int FreqCutoff, MMshort FILTER_COEF *FilterL, MMshort FILTER_COEF *FilterH)
{
    MMshort fc;
    MMshort fc_man, fc_exp;
    MMshort a_man, a_exp;
    MMshort t_man, t_exp;
    MMlong gl_man;
    MMshort gl_exp;
    MMlong gh_man;
    MMshort gh_exp;
    MMlong a1_man;
    MMshort a1_exp;
    MMlong a2_man;
    MMshort a2_exp;
    MMshort pi_man, pi_exp;
    MMlong den_man;
    MMshort den_exp;
    MMshort asq_man, asq_exp;
    MMshort tsq_man, tsq_exp;
    MMshort term1_man, term1_exp;
    MMshort term2_man, term2_exp;
    MMshort Sqrt2_man, Sqrt2_exp;
    MMlong term1_ll_man, term2_ll_man;
    MMshort term1_ll_exp, term2_ll_exp;
    MMshort ltemp;
    MMlong lltemp1, lltemp2;
    MMshort exp;


    exp   = wedge((MMshort) SampleFreq);
    t_exp = -(23 + (23 - exp) - 1);
    t_man = wL_msl(1, -t_exp) / SampleFreq;

    // tsq (1/SampleFreq^2)
    lltemp1 = wL_fmul(t_man, t_man);
    exp     = wedge_X(lltemp1);
    tsq_man = wround_L(wL_msl(lltemp1, exp));
    tsq_exp = 2 * t_exp + 23 - exp;

    // PI = 3.141592653589793236
    pi_man = 0x6487ED;
    pi_exp = -21;

    // TWOSQRT = 1.4142135623730950488016887242097
    Sqrt2_man = 0x5A8279;
    Sqrt2_exp = -22;

    fc = FreqCutoff; //4780; //230; //FreqCutoff;

    fc_exp = wedge(fc);
    fc_man = fc << fc_exp; //wmsl(fc,fc_exp);
    fc_exp = -fc_exp;

    // a = 2.0*PI*FreqCutOff
    lltemp1 = (MMlong)pi_man * (MMlong)fc_man;
    a_man = lltemp1 >> 23;
    a_exp = pi_exp + fc_exp + 23 + 1;

    // asq
    asq_man = ((MMlong)a_man * (MMlong)a_man) >> 23;
    asq_exp = a_exp * 2 + 23;

    // term1 = asq*tsq
    term1_ll_man = ((MMlong)asq_man * (MMlong)tsq_man);
    term1_ll_exp = asq_exp + tsq_exp;
    term1_man = term1_ll_man >> 23;
    term1_exp = term1_ll_exp + 23;

    // term2 = 2.0*TWOSQRT*a*t
    term2_man = ((MMlong)a_man * (MMlong)t_man) >> 23;
    term2_exp = a_exp + t_exp + 23;
    term2_ll_man = ((MMlong)Sqrt2_man * (MMlong)term2_man);
    term2_ll_exp = Sqrt2_exp + term2_exp + 1;
    term2_man = term2_ll_man >> 23;
    term2_exp = term2_ll_exp + 23;

    if(term1_exp<-20 && term2_exp<-20)
    {
        den_man = 0x400000 + ((term1_man)>>(-20-term1_exp)) + ((term2_man)>>(-20-term2_exp));
        den_exp = -20;
    }
    else
    {
        den_exp = (term1_exp>=term2_exp)?term1_exp:term2_exp;
        den_man = (0x4LL<<(-den_exp)) + ((term1_man)>>(den_exp-term1_exp)) + ((term2_man)>>(den_exp-term2_exp));
    }

    lltemp1 = wL_msr(term1_ll_man,(-28-term1_ll_exp)); //((term1_ll_man)>>(-28-term1_ll_exp));
    lltemp2 = wL_msr(term2_ll_man,(-28-term2_ll_exp)); //((term2_ll_man)>>(-28-term2_ll_exp));

    gl_man = term1_ll_man / (MMlong)den_man;
    gl_exp = term1_ll_exp - den_exp;

    ltemp = (MMshort)wL_msr(gl_man,(-23-gl_exp)); //(gl_man>>(-23-gl_exp));
    FilterL[0] = FilterL[2] = ltemp; //+1;
    FilterL[1] = -FilterL[0];

    lltemp1 = ((MMlong)0x200000000000LL); // 4.0 in Q43
    lltemp1 = (wL_msr(term1_ll_man,(-43-term1_ll_exp))/*(term1_ll_man>>(-43-term1_ll_exp))*/ - lltemp1);
    a1_man = lltemp1 / (MMlong)den_man;
    a1_exp = (-43+1) - (den_exp) - 1;

    ltemp = (MMshort)-wL_msr(a1_man,(-23-a1_exp)); //-(a1_man>>(-0x17-a1_exp));
    FilterL[3] = FilterH[3] = ltemp; //-2;

    lltemp1 = ((MMlong)0x100000000000LL); // 4.0 in Q42
    lltemp1 = lltemp1 - wL_msr(term2_ll_man,(-42-term2_ll_exp)); //(term2_ll_man>>(-42-term2_ll_exp));
    lltemp1 = lltemp1 + wL_msr(term1_ll_man,(-42-term1_ll_exp)); //(term1_ll_man>>(-42-term1_ll_exp));
    a2_man = lltemp1 / (MMlong)den_man;
    a2_exp = -42 - den_exp;

    ltemp = (MMshort)wL_msl(a2_man,(a2_exp-(-23))); //(a2_man<<(a2_exp-(-23)));
    FilterL[4] = FilterH[4] = ltemp; //-2;

    lltemp1 = ((MMlong)0x400000000000LL); // 4.0 in Q44
    gh_man = lltemp1 / (MMlong)den_man;
    gh_exp = -44 - den_exp;

    ltemp = (MMshort)wL_msr(gh_man,(-23-gh_exp)); //(gh_man>>(-23-gh_exp));
    FilterH[0] = FilterH[1] = FilterH[2] = ltemp; //-2;
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
    MMlong Samples, SamplingPeriod_us_Q23;


    NumBands   = Heap->NumBands;
    SampleFreq = Heap->SampleFreq;

    Heap->CompressorEnable = FALSE; // clear the flag
    Heap->MdrcLALen        = (int) ((SampleFreq * MDRC5B_LOOKAHEAD_DURATION) / 1000); // msec

    for(bidx = 0; bidx < NumBands; bidx++)
    {
        Heap->CompressorEnable |= Heap->BandCompressors[bidx].Enable;

        // translate compression curve to internal representation
        Tmp = Heap->BandCompressors[bidx].PostGain;
        Tmp = wmin(Tmp, MDRC5B_POSTGAIN_MAX);
        Tmp = wmax(Tmp, MDRC5B_POSTGAIN_MIN);
        Heap->ComPostGaindB[bidx] = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16


        Heap->ComThreshdBOrig[bidx][0][0] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[0][0], Q23_ONE_HUNDREDTH), 16)); // result in Q16

        Heap->ComThreshdBOrig[bidx][0][1] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[0][1], Q23_ONE_HUNDREDTH), 16)); // result in Q16

        Heap->ComThreshdB[bidx][0][0] = Heap->ComThreshdBOrig[bidx][0][0];
        Heap->ComThreshdB[bidx][0][1] = Heap->ComThreshdBOrig[bidx][0][1];

        /*
        //Heap->ComSlope[bidx][0] = (Heap->BandCompressors[bidx].DynamicResponse[1][1]/100.0 + 138.5)  \
        //                          / (Heap->BandCompressors[bidx].DynamicResponse[1][0]/100.0 + 138.5); // -138.5 dB for 24 bits
        */
        Tmp1 = Heap->BandCompressors[bidx].DynamicResponse[1][1];
        Tmp1 = wmax(Tmp1, MDRC5B_INPUT_MIN_MB);
        Tmp1 = wmin(Tmp1, MDRC5B_INPUT_MAX_MB);
        Tmp = Heap->BandCompressors[bidx].DynamicResponse[1][0];
        Tmp = wmax(Tmp, MDRC5B_INPUT_MIN_MB);
        Tmp = wmin(Tmp, MDRC5B_INPUT_MAX_MB);
//      Heap->ComSlope[bidx][0] = wround_L(wL_msl(wL_msl((Tmp1 + 13847), 24)/(Tmp + 13847), 16)); // Q16
        Heap->ComSlope[bidx][0] = wround_L(wL_msl(wL_msl((Tmp1 + 18660), 24)/(Tmp + 18660), 16)); // Q16

#ifdef MDRC5B_DEBUG
        printf("comp %d, p 0, (%.2f, %.2f), slope=%.3f\n", bidx, (float)Heap->ComThreshdBOrig[bidx][0][0]/65536.0, (float)Heap->ComThreshdBOrig[bidx][0][1]/65536.0, (float)Heap->ComSlope[bidx][0]/65536.0);
#endif
        KneePoints = Heap->BandCompressors[bidx].KneePoints;
        for(p = KneePoints - 2; p >= 1; p--)
        {
            Heap->ComThreshdBOrig[bidx][p][0] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[p][0], Q23_ONE_HUNDREDTH), 16)); // result in Q16
            Heap->ComThreshdBOrig[bidx][p][1] = wround_L(wL_msl(wL_fmul(Heap->BandCompressors[bidx].DynamicResponse[p][1], Q23_ONE_HUNDREDTH), 16)); // result in Q16

            Heap->ComThreshdB[bidx][p][0] = Heap->ComThreshdBOrig[bidx][p][0];
            Heap->ComThreshdB[bidx][p][1] = Heap->ComThreshdBOrig[bidx][p][1];

            /*
            //Heap->ComSlope[bidx][p] = (Heap->BandCompressors[bidx].DynamicResponse[p+1][1]/100.0 - Heap->BandCompressors[bidx].DynamicResponse[p][1]/100.0) \
            //                        / MAX(1.0, Heap->BandCompressors[bidx].DynamicResponse[p+1][0]/100.0 - Heap->BandCompressors[bidx].DynamicResponse[p][0]/100.0);
            */
            Tmp3 = Heap->BandCompressors[bidx].DynamicResponse[p][1];
            Tmp3 = wmax(Tmp3, MDRC5B_INPUT_MIN_MB);
            Tmp3 = wmin(Tmp3, MDRC5B_INPUT_MAX_MB);
            Tmp2 = Heap->BandCompressors[bidx].DynamicResponse[p][0];
            Tmp2 = wmax(Tmp2, MDRC5B_INPUT_MIN_MB);
            Tmp2 = wmin(Tmp2, MDRC5B_INPUT_MAX_MB);
            Tmp1 = Heap->BandCompressors[bidx].DynamicResponse[p+1][1];
            Tmp1 = wmax(Tmp1, MDRC5B_INPUT_MIN_MB);
            Tmp1 = wmin(Tmp1, MDRC5B_INPUT_MAX_MB);
            Tmp = Heap->BandCompressors[bidx].DynamicResponse[p+1][0];
            Tmp = wmax(Tmp, Tmp2+1); // to avoid divide-by-0
            Tmp = wmin(Tmp, MDRC5B_INPUT_MAX_MB);

            Heap->ComSlope[bidx][p] = wround_L(wL_msl(wL_msl((Tmp1 - Tmp3), 24)/(Tmp - Tmp2), 16)); // Q16
#ifdef MDRC5B_DEBUG
            printf("comp %d, p %d, (%.2f, %.2f), slope=%.3f\n", bidx, p, (float)Heap->ComThreshdBOrig[bidx][p][0]/65536.0, (float)Heap->ComThreshdBOrig[bidx][p][1]/65536.0, (float)Heap->ComSlope[bidx][p]/65536.0);
#endif
        }


        SamplingPeriod_us_Q23 = (MMlong)Q23 * 1000000LL;
        SamplingPeriod_us_Q23 = SamplingPeriod_us_Q23 / SampleFreq;  // sampling period in micro-seconds (Q23)

        // translate time constants to filter coeffs
        Tmp = Heap->BandCompressors[bidx].AttackTime;
        Tmp = MIN(Tmp, MDRC5B_COM_AT_TIME_MAX);
        Tmp = MAX(Tmp, MDRC5B_COM_AT_TIME_MIN);

        // this is the implementation is floating-point
        //Samples = (float)SampleFreq * Tmp / 1000000.0; // Attack time in sampling periods (floating)
        //Heap->ComAtCoef[bidx] = pow(0.1, 1.0/Samples);

        // in fixed-point
        // 0.1^(1/N) = 10^(-1/N)
        // 1/N : Q23 / (F*Time/1000000) = Q23 / F * 1000000 / Time  (time in microseconds)
        Tmp = SamplingPeriod_us_Q23 / Tmp;    // 1/(attack time in sampling periods) (Q23)
        Tmp = mdrc5b_pow10_coef(-Tmp);        // Q21
        Heap->ComAtCoef[bidx] = wmsl(Tmp, 2); // Q23

        // release coef
        Tmp = Heap->BandCompressors[bidx].ReleaseTime;
        Tmp = MIN(Tmp, MDRC5B_COM_RE_TIME_MAX);
        Tmp = MAX(Tmp, MDRC5B_COM_RE_TIME_MIN);
        Tmp = SamplingPeriod_us_Q23 / Tmp;     // 1/(release time in sampling periods) (Q23)
        Tmp = mdrc5b_pow10_coef(-Tmp);         // Q21
        Heap->ComReCoef[bidx] = wmsl(Tmp, 2);  // Q23


        // hold samples
        Samples = (MMlong)SampleFreq * MDRC5B_HOLD_TIME;
        Heap->HtSamp[bidx] = Samples / 1000000;     // hold time in samples


        // RMS estimation coef
        Tmp = SamplingPeriod_us_Q23 / MDRC5B_RMS_ATTACK_TIME; // 1/(RMS attack time in sampling periods) (Q23)
        Tmp = mdrc5b_pow10_coef(-Tmp);         // Q21
        Heap->RmsAtCoef[bidx] = wmsl(Tmp, 2);  // Q23

        // release coef
        Tmp = SamplingPeriod_us_Q23 / MDRC5B_RMS_RELEASE_TIME; // 1/(RMS release time in sampling periods) (Q23)
        Tmp = mdrc5b_pow10_coef(-Tmp);         // Q21
        Heap->RmsReCoef[bidx] = wmsl(Tmp, 2);  // Q23

#ifdef MDRC5B_DEBUG
        printf("comp %d, ComAtCoef=%.6f, ComReCoef=%.6f, HtSamp=%d, RmsAtCoef=%.6f, RmsReCoef=%.6f\n",
               bidx,
               (float)Heap->ComAtCoef[bidx]/8388608.0,
               (float)Heap->ComReCoef[bidx]/8388608.0,
               Heap->HtSamp[bidx],
               (float)Heap->RmsAtCoef[bidx]/8388608.0,
               (float)Heap->RmsReCoef[bidx]/8388608.0);
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
                Heap->ChWeight[i] = 8388607;    // Q23: 1
                break;
            case 2:
                Heap->ChWeight[i] = 4194304;    // Q23: 0.5
                break;
            case 3:
                Heap->ChWeight[i] = 2796203;    // Q23: 1/3
                break;
            default:
                Heap->ChWeight[i] = 8388607;    // Q23
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
        Heap->ComGainMant[j] = Q23; // linear gain
        Heap->ComGainExp[j] = 0;
        Heap->MainSubRms[j] = 0; // silent
        Heap->ComTargetGaindB[j] = 0;
        Heap->ComHoldRem[j] = 0;
    }

    for(i = 0; i < MDRC5B_MAIN_CH_MAX; i++)
    {
        Heap->p_band_input[i] = (MMshort *) Heap->MainInBuf[i];
#ifdef __flexcc2__
        Heap->p_band_input[i]++; // because MMDSP is big-endian
#endif
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
    int     k, n, ch;
    int     Samples;
    int     LaIdx;
    int     NumBands;
    int     NumMainCh;
    MMshort  *MemInPtr;
    MMlong  *MemOutPtr;
    MMshort *Ptr;
#ifdef __flexcc2__
    MMshort __circ *Ptr2;
#else // __flexcc2__
    MMshort *Ptr2;
#endif // __flexcc2__
    int bidx;
#ifndef IVC_LOUDER_CH_PRIORITY
    MMshort Weight;
#endif // !IVC_LOUDER_CH_PRIORITY
    MMlong  MainSubSq[MDRC5B_SUBBAND_MAX][MDRC5B_BLOCK_SIZE];
    MMlong  *PtrX;
    MMlong  MainSubRmsMax[MDRC5B_SUBBAND_MAX];
    MMshort RmsAtCoef;
    MMshort RmsReCoef;
    MMlong  MainSubRms;
    MMshort  FinalGain;
    int     Exp;
    MMshort TargetGain;
    int     HtSamp;
    int     ComHoldRem;
    MMshort ComGainMant[MDRC5B_SUBBAND_MAX][MDRC5B_BLOCK_SIZE + 1];
    int     ComGainExp[MDRC5B_SUBBAND_MAX], GainExp;
    MMshort ComAtCoef;
    MMshort ComReCoef;
    MMshort Tmp;


    Samples   = HeapPtr->BlockSize;
    NumBands  = HeapPtr->NumBands;
    NumMainCh = HeapPtr->NumMainCh;

    if(NumBands <= 1)
    {
        // no need to filter
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
        for(ch = 0; ch < NumMainCh; ch++)
        {
            // MainInBuf is stored in MMlong but at this stage of algorithm,
            // only 24 least-significant bit are significant
            // (see mdrc5b_read() routine which fills MainInBuf).
            // Thus we can access to this data thru a MMshort pointer (with increment of 2).
            Ptr      = HeapPtr->p_filter_buffer->MainSubBuf[ch][0];
            MemInPtr = (MMshort *) HeapPtr->MainInBuf[ch];
#ifdef __flexcc2__
            MemInPtr++; // because MMDSP is big-endian
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
                *Ptr++    = (MMshort) *MemInPtr;
                MemInPtr += 2;
            }
        }
    }
    else if(HeapPtr->mdrc_filter_kind == MDRC_LEGACY_FILTER)
    {
        // subband filtering
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
        for(ch = 0; ch < NumMainCh; ch++)
        {
            // MainInBuf is stored in MMlong but at this stage of algorithm,
            // only 24 least-significant bit are significant
            // (see mdrc5b_read() routine which fills MainInBuf).
            // Thus we can access to this data thru a MMshort pointer (with increment of 2).
            MemInPtr = (MMshort *) HeapPtr->MainInBuf[ch];
#ifdef __flexcc2__
            MemInPtr++; // because MMDSP is big-endian
#endif

            // 1st band
            Ptr = HeapPtr->p_filter_buffer->MainSubBuf[ch][0];
            mdrc5b_biquad2(MemInPtr, 2, Ptr, Samples, HeapPtr->p_filter_coef->LPFilt[0], HeapPtr->p_filter_mem->LPFiltHist[ch][0]);

            // 2nd band to 2nd last band
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
            for(bidx = 1; bidx < NumBands - 1; bidx++)
            {
                Ptr = HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx];
                mdrc5b_biquad2(MemInPtr, 2, Ptr, Samples, HeapPtr->p_filter_coef->LPFilt[bidx],     HeapPtr->p_filter_mem->LPFiltHist[ch][bidx]);
                mdrc5b_biquad2(Ptr,      1, Ptr, Samples, HeapPtr->p_filter_coef->HPFilt[bidx - 1], HeapPtr->p_filter_mem->HPFiltHist[ch][bidx - 1]);
            }

            // last band
            Ptr = HeapPtr->p_filter_buffer->MainSubBuf[ch][NumBands - 1];
            mdrc5b_biquad2(MemInPtr, 2, Ptr, Samples, HeapPtr->p_filter_coef->HPFilt[NumBands - 2], HeapPtr->p_filter_mem->HPFiltHist[ch][NumBands - 2]);
        }
    }
    else
    {
        (*HeapPtr->p_mdrc_filters->p_sub_bands_filtering) (HeapPtr->p_mdrc_filters,
                                                           HeapPtr->p_band_input,
                                                           HeapPtr->p_band_output,
                                                           Samples);
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

    // insert the new subband samples into the lookahead buffers
    LaIdx = HeapPtr->LaIdx;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            Ptr   = HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx];
#ifdef __flexcc2__
            Ptr2 = winit_circ_ptr(&HeapPtr->MainSubLABuf[ch][bidx][LaIdx],
                                  HeapPtr->MainSubLABuf[ch][bidx],
                                  &HeapPtr->MainSubLABuf[ch][bidx][ HeapPtr->MdrcLALen]);
#else // __flexcc2__
            Ptr2 = &HeapPtr->MainSubLABuf[ch][bidx][LaIdx];  // go to the first valid sample
#endif // __flexcc2__

#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif // __flexcc2__
            for(k = 0; k < Samples; k++)
            {
                *Ptr2 = Ptr[k];
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               &HeapPtr->MainSubLABuf[ch][bidx][HeapPtr->MdrcLALen],
                               HeapPtr->MainSubLABuf[ch][bidx]);
#endif // __flexcc2__
            }
        }
    }
    // update index
    LaIdx         += Samples;
    HeapPtr->LaIdx = CIRC_ADD_H(LaIdx, HeapPtr->MdrcLALen);


    // compute power
#ifndef IVC_LOUDER_CH_PRIORITY
    Weight = HeapPtr->ChWeight[0];
#endif // !IVC_LOUDER_CH_PRIORITY
#ifdef __flexcc2__
    #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        Ptr  = HeapPtr->p_filter_buffer->MainSubBuf[0][bidx];
        PtrX = MainSubSq[bidx];

        // square and weight
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
#ifdef IVC_LOUDER_CH_PRIORITY
            PtrX[k] = wL_fmul(Ptr[k], Ptr[k]);                      // Q47
#else // IVC_LOUDER_CH_PRIORITY
            Tmp     = wfmul(Ptr[k], Weight);                        // Q23
            PtrX[k] = wL_fmul(Ptr[k], Tmp);                         // Q47
#endif // IVC_LOUDER_CH_PRIORITY
        }
    }

#ifdef __flexcc2__
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 1; ch < NumMainCh; ch++)
    {
#ifndef IVC_LOUDER_CH_PRIORITY
        Weight = HeapPtr->ChWeight[ch];
#endif // !IVC_LOUDER_CH_PRIORITY
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            Ptr  = HeapPtr->p_filter_buffer->MainSubBuf[ch][bidx];
            PtrX = MainSubSq[bidx];

            // square and weight
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
#ifdef IVC_LOUDER_CH_PRIORITY
                PtrX[k] = MAX(PtrX[k], wL_fmul(Ptr[k], Ptr[k]));    // Q47
#else // IVC_LOUDER_CH_PRIORITY
                Tmp     = wfmul(Ptr[k], Weight);                    // Q23
                PtrX[k] = wL_addsat(PtrX[k], wL_fmul(Ptr[k], Tmp)); // Q47
#endif // IVC_LOUDER_CH_PRIORITY
            }
        }
    }
#ifdef DEBUG_COMPRESSOR_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_COMPRESSOR_COMPUTE_POWER\n");
        for(k = 0; k < Samples; k++)
        {
            for(bidx = 0; bidx < NumBands; bidx++)
            {
                sprintf(string, "0x%012llX ", MainSubSq[bidx][k] & 0xFFFFFFFFFFFFLL);
                debug_write_string(string);
            }
            debug_write_string("\n");
        }
    }
#endif // DEBUG_COMPRESSOR_OUTPUT


    // smooth and max()
#ifdef __flexcc2__
    #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        PtrX                = MainSubSq[bidx];
        MainSubRms          = HeapPtr->MainSubRms[bidx];
        RmsAtCoef           = HeapPtr->RmsAtCoef[bidx];
        RmsReCoef           = HeapPtr->RmsReCoef[bidx];
        MainSubRmsMax[bidx] = 0;

#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
            MMlong diffX = wL_subsat(MainSubRms, PtrX[k]);

            if(diffX < 0)
            {
                MainSubRms = wL_addsat(PtrX[k], ivc_fmul48x24(diffX, RmsAtCoef));
            }
            else
            {
                MainSubRms = wL_addsat(PtrX[k], ivc_fmul48x24(diffX, RmsReCoef));
            }
            MainSubRmsMax[bidx] = MAX(MainSubRms, MainSubRmsMax[bidx]);
        }
        HeapPtr->MainSubRms[bidx] = MainSubRms;    // save history
    }
#ifdef DEBUG_COMPRESSOR_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_COMPRESSOR_SMOOTH_AND_MAX\n");
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            sprintf(string, "0x%012llX ", HeapPtr->MainSubRms[bidx] & 0xFFFFFFFFFFFFLL);
            debug_write_string(string);
        }
        debug_write_string("\n");
        for(bidx = 0; bidx < NumBands; bidx++)
        {
            sprintf(string, "0x%012llX ", MainSubRmsMax[bidx] & 0xFFFFFFFFFFFFLL);
            debug_write_string(string);
        }
        debug_write_string("\n");
    }
#endif // DEBUG_COMPRESSOR_OUTPUT


    // compressor
    mdrc5b_compressor_gain(HeapPtr, MainSubRmsMax);
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


    // smooth gains
#ifdef __flexcc2__
    #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        if(HeapPtr->BandCompressors[bidx].Enable)
        {
            FinalGain = waddsat(HeapPtr->ComTargetGaindB[bidx], HeapPtr->ComPostGaindB[bidx]); // Q16, [-128.0, 127.0] dB
            FinalGain = wfmul(FinalGain, Q23_ONE_TENTH); // divide by 10
            FinalGain >>= 1;                             // divide by 2

            mdrc5b_pow10(FinalGain, &Exp, &TargetGain); // linear gain = 2^Exp * TargetGain
        }
        else
        {
            Exp        = 0;
            TargetGain = Q23;
        }

#ifdef MDRC5B_DEBUG
        printf("%ld, band %d, gain %.3f, exp %d, mant %.6f\n", counter, bidx, (float)HeapPtr->ComTargetGaindB[bidx]/65536.0, HeapPtr->ComGainExp[bidx], (float)HeapPtr->ComGainMant[bidx]/8388608.0);
#endif

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
//      if (TargetGain < ComGainMant[bidx][0]) // k refers to the previous sample, k+1 to the current sample
        if(TargetGain - wL_msr(GAIN_MINUS_60DB_Q23, Exp) < ComGainMant[bidx][0]) // k refers to the previous sample, k+1 to the current sample
        {
            // attack
            Tmp = ComGainMant[bidx][0];
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
                Tmp                      = waddsat(TargetGain, wfmulr(ComAtCoef, wsubsat(Tmp, TargetGain))); // Q20
                ComGainMant[bidx][k + 1] = Tmp;
            }
            ComHoldRem = HtSamp; // init hold time
        }
        else
        {
            // release
            Tmp         = ComGainMant[bidx][0];
            n           = wmin(ComHoldRem, Samples);
            ComHoldRem -= n;
#ifdef __flexcc2__
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < n; k++)
            {
                ComGainMant[bidx][k + 1] = Tmp;
            }
#ifdef __flexcc2__
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(; k < Samples; k++)
            {
                Tmp                      = waddsat(TargetGain, wfmulr(ComReCoef, wsubsat(Tmp, TargetGain)));
                ComGainMant[bidx][k + 1] = Tmp;
            }
        }

        // save history
        n                          = wedge(Tmp);
        HeapPtr->ComGainMant[bidx] = wmsl(Tmp, n);
        HeapPtr->ComGainExp[bidx]  = ComGainExp[bidx] - n;

        HeapPtr->ComHoldRem[bidx]  = ComHoldRem;
    }


    // output
    LaIdx = HeapPtr->LaIdx; // this is now the oldest sample
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
        MemOutPtr = HeapPtr->MainInBuf[ch];

#ifdef __flexcc2__
        Ptr2 = winit_circ_ptr(&HeapPtr->MainSubLABuf[ch][0][LaIdx],
                              HeapPtr->MainSubLABuf[ch][0],
                              &HeapPtr->MainSubLABuf[ch][0][HeapPtr->MdrcLALen]);
#else // __flexcc2__
        Ptr2 = &HeapPtr->MainSubLABuf[ch][0][LaIdx];  // go to the first valid sample
#endif // __flexcc2__

        // first band
        GainExp = ComGainExp[0];
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
            MemOutPtr[k] = wL_msl((MMlong) wfmulr(*Ptr2, ComGainMant[0][k + 1]), GainExp);
#ifdef __flexcc2__
            Ptr2++;
#else // __flexcc2__
            Ptr2 = mod_add(Ptr2,
                           1,
                           &HeapPtr->MainSubLABuf[ch][0][HeapPtr->MdrcLALen],
                           HeapPtr->MainSubLABuf[ch][0]);
#endif // __flexcc2__
        }

        // higher bands
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
        for(bidx = 1; bidx < NumBands; bidx++)
        {
#ifdef __flexcc2__
            Ptr2 = winit_circ_ptr(&HeapPtr->MainSubLABuf[ch][bidx][LaIdx],
                                  HeapPtr->MainSubLABuf[ch][bidx],
                                  &HeapPtr->MainSubLABuf[ch][bidx][HeapPtr->MdrcLALen]);
#else // __flexcc2__
            Ptr2 = &HeapPtr->MainSubLABuf[ch][bidx][LaIdx];  // go to the first valid sample
#endif // __flexcc2__
            GainExp = ComGainExp[bidx];

#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
                MemOutPtr[k] = wL_addsat(MemOutPtr[k], wL_msl((MMlong) wfmulr(*Ptr2, ComGainMant[bidx][k + 1]), GainExp));
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               &HeapPtr->MainSubLABuf[ch][bidx][HeapPtr->MdrcLALen],
                               HeapPtr->MainSubLABuf[ch][bidx]);
#endif // __flexcc2__
            }
        }
    }

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
}


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
    int    bidx;
    int    NumBands;
    MMshort RmsdB;
    MMshort Delta;
    MMshort *ComThreshdB;
    MMshort *ComSlope;
    int    KneePoints;
    int    p;


    NumBands = HeapPtr->NumBands;

#ifdef __flexcc2__
    #pragma loop maxitercount(MDRC5B_SUBBAND_MAX)
#endif
    for(bidx = 0; bidx < NumBands; bidx++)
    {
        if(!HeapPtr->BandCompressors[bidx].Enable)
        {
            // subband disabled, no gain to apply
            HeapPtr->ComTargetGaindB[bidx] = 0;
        }
        else
        {
            ComSlope   = HeapPtr->ComSlope[bidx];
            KneePoints = HeapPtr->BandCompressors[bidx].KneePoints;
            if(Rms[bidx] < MDRC5B_ALMOST_ZERO_THRESH)
            {
                RmsdB = wmsl(MDRC5B_POWER_DB_MINUS_INF, 16); // Q16, [-128.0, 127.0] dB
            }
            else
            {
                MMlong RmsdBX;

                RmsdBX = mdrc5b_log10_hd(Rms[bidx]);
                RmsdBX = wL_addsat(RmsdBX, wL_imul(HEADROOM * 2, 2525223));
                RmsdB  = wround_L(wL_addsat(wL_msl(RmsdBX, 20), wL_msl(RmsdBX, 18))); // x(2^20+2^18)/2^24 i.e. x0.625 >> 3
            }


            ComThreshdB = HeapPtr->ComThreshdB[bidx][1];    // 1st knee point is at -inf, so compare with 2nd kneepoint
            Delta       = RmsdB - ComThreshdB[0];           // Q16
            if(Delta <= 0)
            {
                // below the 2nd knee point
                HeapPtr->ComTargetGaindB[bidx] = ComThreshdB[1] - RmsdB + wround_L(wL_msl(wL_fmul(Delta, ComSlope[0]), 7));
#ifdef MDRC5B_DEBUG
                printf("band %d, in db %.3f, gain %.3f, slope %.3f\n", bidx, (float)RmsdB/65536.0, (float)HeapPtr->ComTargetGaindB[bidx]/65536.0, (float)ComSlope[0]/65536.0);
#endif
            }
            else
            {
#ifdef __flexcc2__
                #pragma loop maxitercount(MDRC5B_KNEEPOINTS_MAX)
#endif
                for(p = KneePoints - 2; p >= 1; p--)
                {
                    ComThreshdB = HeapPtr->ComThreshdB[bidx][p];
                    Delta       = RmsdB - ComThreshdB[0];             // Q16
                    if(Delta >= 0)
                    {
                        HeapPtr->ComTargetGaindB[bidx] = ComThreshdB[1] - RmsdB + wround_L(wL_msl(wL_fmul(Delta, ComSlope[p]), 7));
#ifdef MDRC5B_DEBUG
                        printf("band %d, in db %.3f, gain %.3f, slope %.3f, th %.3f\n", bidx, (float)RmsdB/65536.0, (float)HeapPtr->ComTargetGaindB[bidx]/65536.0, (float)ComSlope[p]/65536.0, (float)ComThreshdB[0]/65536.0);
#endif
                        break;
                    }
                }
            }
        }
    }
}

#endif // #ifdef MDRC5B_COMPRESSOR_ACTIVE
