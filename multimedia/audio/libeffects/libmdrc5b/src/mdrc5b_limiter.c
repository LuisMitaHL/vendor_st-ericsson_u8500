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
  \file mdrc5b_limiter.c
  \brief processing routines of the limiter
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

#ifdef MDRC5B_LIMITER_ACTIVE

#include "mdrc5b_limiter.h"
#include "mdrc5b_dsp.h"
#include "mdrc5b_rom.h"


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
    Word48  Samples;
    MMshort coef[4];


    SampleFreq            = Heap->SampleFreq;
    Heap->LimiterLALen    = (int) ((SampleFreq * MDRC5B_LIMITER_LOOKAHEAD_DURATION) / 1000); // msec

    Tmp                   = Heap->Limiter.Thresh;
    Tmp                   = MIN(Tmp, MDRC5B_LIMITER_THRESH_MB_MAX);
    Tmp                   = MAX(Tmp, MDRC5B_LIMITER_THRESH_MB_MIN);
    Heap->LimiterThreshdB = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16

//--------------------------------------------------
/*

    //for 48KHz only
    Heap->LimiterAtCoef     = MDRC5B_LIMITER_ATTACK_COEF;
    Heap->LimiterReCoef     = MDRC5B_LIMITER_RELEASE_COEF;
    Heap->LimiterPeakAtCoef = MDRC5B_LIMITER_PEAK_ATTACK_COEF;
    Heap->LimiterPeakReCoef = MDRC5B_LIMITER_PEAK_RELEASE_COEF;
*/

    switch(SampleFreq)
    {
        case 8000:
            coef[0] = 6290568;
            coef[1] = 8364498;
            coef[2] = 6290;
            coef[3] = 8364498;
            break;

        case 11025:
            coef[0] = 6807489;
            coef[1] = 8371106;
            coef[2] = 45301;
            coef[3] = 8371106;
            break;

        case 12000:
            coef[0] = 6923992;
            coef[1] = 8372527;
            coef[2] = 69239;
            coef[3] = 8372527;
            break;

        case 16000:
            coef[0] = 7264235;
            coef[1] = 8376544;
            coef[2] = 229715;
            coef[3] = 8376544;
            break;

        case 22050:
            coef[0] = 7556808;
            coef[1] = 8379852;
            coef[2] = 616457;
            coef[3] = 8379852;
            break;

        case 24000:
            coef[0] = 7621197;
            coef[1] = 8380563;
            coef[2] = 762119;
            coef[3] = 8380563;
            break;

        case 32000:
            coef[0] = 7806204;
            coef[1] = 8382574;
            coef[2] = 1388161;
            coef[3] = 8382574;
            break;

        case 44100:
            coef[0] = 7961853;
            coef[1] = 8384229;
            coef[2] = 2274031;
            coef[3] = 8384229;
            break;

        case 48000:
            coef[0] = 7995701;
            coef[1] = 8384584;
            coef[2] = 2528462;
            coef[3] = 8384584;
            break;

        default:
            // gain application attack coef
            Samples = (Word48) Q23 * 1000000LL;
            Samples = Samples / SampleFreq;
            Samples = Samples / MDRC5B_LIMITER_ATTACK_TIME_DEFAULT; // Q23
            Tmp     = mdrc5b_pow10_coef(-((MMshort) Samples));     // Q21
            coef[0] = wmsl(Tmp, 2);    // Q23

            // gain application release coef
            Samples = (Word48)Q23 * 1000000LL;
            Samples = Samples / SampleFreq;
            Samples = Samples / MDRC5B_LIMITER_RELEASE_TIME_DEFAULT; // Q23
            Tmp     = mdrc5b_pow10_coef(-((MMshort) Samples));     // Q21
            coef[1] = wmsl(Tmp, 2);    // Q23


            // level estimation attack coef
            Samples = (Word48)Q23 * 1000000LL;
            Samples = Samples / SampleFreq;
            Samples = Samples / MDRC5B_LIMITER_EST_ATTACK_TIME_DEFAULT; // Q23
            Tmp     = mdrc5b_pow10_coef(-((MMshort) Samples));       // Q21
            coef[2] = wmsl(Tmp, 2);  // Q23

            // level estimation release coef
            Samples = (Word48)Q23 * 1000000LL;
            Samples = Samples / SampleFreq;
            Samples = Samples / MDRC5B_LIMITER_EST_RELEASE_TIME_DEFAULT; // Q23
            Tmp     = mdrc5b_pow10_coef(-((MMshort) Samples));     // Q21
            coef[3] = wmsl(Tmp, 2);  // Q23
            break;
    }

    Heap->LimiterAtCoef     = coef[0];
    Heap->LimiterReCoef     = coef[1];
    Heap->LimiterPeakAtCoef = coef[2];
    Heap->LimiterPeakReCoef = coef[3];
//----------------------------------------------

    // hold samples
    Samples = (Word48) SampleFreq * MDRC5B_LIMITER_HOLD_TIME;
    Heap->LimiterHtSamp = Samples / 1000000;

#ifdef MDRC5B_DEBUG
    printf("Limiter : LALen=%d, ThreshdB=%.6f, AtCoef=%.6f, ReCoef=%.6f, PeakAtCoef=%.6f, PeakReCoef=%.6f, HtSamp=%d\n",
           Heap->LimiterLALen,
           (float)Heap->LimiterThreshdB/65536.0,
           (float)Heap->LimiterAtCoef/8388608.0,
           (float)Heap->LimiterReCoef/8388608.0,
           (float)Heap->LimiterPeakAtCoef/8388608.0,
           (float)Heap->LimiterPeakReCoef/8388608.0,
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

    Heap->LimiterGainMant = Q23; // linear gain Q20
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
    int     LaIdx;
    int     NumMainCh;
    int     RmsMeasure;
    int     Samples;
    int     ch, k, n;
    MMlong  *Ptr;
#ifdef __flexcc2__
    MMlong  __circ * Ptr2;
#else
    MMlong  *Ptr2;
#endif
    MMlong  *MemOutPtr;
    MMshort PeakdB;
    MMlong  Peak;
    MMlong  PeakMax;
    MMshort LimiterPeakAtCoef;
    MMshort LimiterPeakReCoef;
    MMshort LimiterAtCoef;
    MMshort LimiterReCoef;
    MMshort LimiterGainMant[MDRC5B_BLOCK_SIZE + 1];
    MMshort LimiterGainExp;
    MMshort  LimiterTargetGaindB;
    int     LimiterHoldRem;
    int     LimiterHtSamp;
    MMshort Temp;
    MMlong  TempX;
    MMshort Exp, TargetGain;
    MMshort MaxShiftBits = 0;
    int     lookahead_len = HeapPtr->LimiterLALen;


    Samples   = HeapPtr->BlockSize;
    NumMainCh = HeapPtr->NumMainCh;

    TempX = 0;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif // __flexcc2__
    for(ch = 0; ch < NumMainCh; ch++)
    {
        Ptr = HeapPtr->MainInBuf[ch];
        // compute the number of bits needs to be shifted to avoid overflow
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif // __flexcc2__
        for(k = 0; k < Samples; k++)
        {
            TempX |= Ptr[k] ^ (Ptr[k] >> 47);
        }
    }
    if(TempX)
    {
        MaxShiftBits = wmax(0, 24 - wedge_X(TempX));
    }
    MaxShiftBits += 1;
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT\n");
        sprintf(string, "MaxShiftBits=%d\n", MaxShiftBits);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT

    // insert the new subband samples into the lookahead buffers
    RmsMeasure        = HeapPtr->Limiter.RmsMeasure;
    LimiterPeakAtCoef = HeapPtr->LimiterPeakAtCoef;
    LimiterPeakReCoef = HeapPtr->LimiterPeakReCoef;
    PeakMax           = 0;

#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
        Ptr   = HeapPtr->MainInBuf[ch];
        LaIdx = HeapPtr->LimiterLaIdx;
#ifdef __flexcc2__
        Ptr2 = winit_circ_ptr(&HeapPtr->LimiterLABuf[ch][LaIdx],
                              HeapPtr->LimiterLABuf[ch],
                              &HeapPtr->LimiterLABuf[ch][lookahead_len]);
#else
        Ptr2 = &HeapPtr->LimiterLABuf[ch][LaIdx];  // go to the first valid sample
#endif

        Peak = HeapPtr->LimiterPeak[ch];

        // abs() and smooth
        if(RmsMeasure)
        {
            // compensate Peak according to the previous shift bits
            Peak = wX_msr(Peak, (MaxShiftBits - HeapPtr->PrevShiftBits) * 2);

#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
                MMlong diffX;

                Temp    = wL_msr(Ptr[k], MaxShiftBits);
                *Ptr2++ = Ptr[k];
#ifndef __flexcc2__
                if(Ptr2 == &HeapPtr->LimiterLABuf[ch][lookahead_len])
                {
                    Ptr2 = HeapPtr->LimiterLABuf[ch];
                }
#endif // !__flexcc2__

                TempX = wL_fmul(Temp, Temp); // Q47
                diffX = wL_subsat(Peak, TempX);
                if(diffX < 0)
                {
                    Peak = wL_addsat(TempX, ivc_fmul48x24(diffX, LimiterPeakAtCoef));
                }
                else
                {
                    Peak = wL_addsat(TempX, ivc_fmul48x24(diffX, LimiterPeakReCoef));
                }
                PeakMax = MAX(PeakMax, Peak);
            }
        }
        else
        {
            // compensate Peak according to the previous shift bits
            Peak = wX_msr(Peak, MaxShiftBits - HeapPtr->PrevShiftBits);

            // amplitude measure
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
            for(k = 0; k < Samples; k++)
            {
                MMlong diffX;

                TempX   = Ptr[k];
                *Ptr2++ = Ptr[k];
#ifndef __flexcc2__
                if(Ptr2 == &(HeapPtr->LimiterLABuf[ch][lookahead_len]))
                {
                    Ptr2 = HeapPtr->LimiterLABuf[ch];
                }
#endif // !__flexcc2__

                TempX = wL_msl(wL_abssat(TempX), 24 - MaxShiftBits);      // Q47
                diffX = wL_subsat(Peak, TempX);
                if(diffX < 0)
                {
                    Peak = wL_addsat(TempX, ivc_fmul48x24(diffX, LimiterPeakAtCoef));
                }
                else
                {
                    Peak = wL_addsat(TempX, ivc_fmul48x24(diffX, LimiterPeakReCoef));
                }

                PeakMax = MAX(PeakMax, Peak);
            }
        }

        HeapPtr->LimiterPeak[ch] = Peak;
    }
    // update index
    LaIdx                 = HeapPtr->LimiterLaIdx + Samples;
    HeapPtr->LimiterLaIdx = CIRC_ADD_H(LaIdx, lookahead_len);

    HeapPtr->PrevShiftBits = MaxShiftBits;


    if(PeakMax < MDRC5B_ALMOST_ZERO_THRESH)
    {
        PeakdB = wmsl(MDRC5B_POWER_DB_MINUS_INF, 16); // 8.16, [-128.0, 127.0] dB
    }
    else
    {
        Word48 PeakdBX = mdrc5b_log10_hd(PeakMax);

        if(RmsMeasure)
        {
            PeakdBX = wL_addsat(PeakdBX, wL_imul((HEADROOM + MaxShiftBits) * 2, 2525223));
            PeakdB  = wround_L(wL_addsat(wL_msl(PeakdBX, 20), wL_msl(PeakdBX, 18))); // x(2^20+2^18)/2^24 i.e. x0.625 >> 3
        }
        else
        {
            PeakdBX = wL_addsat(PeakdBX, wL_imul((HEADROOM + MaxShiftBits), 2525223));
            PeakdB  = wround_L(wL_addsat(wL_msl(PeakdBX, 21), wL_msl(PeakdBX, 19))); // x(2^21+2^19)/2^24 i.e. x0.625 >> 2
        }
    }
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_PEAKMAX_PEAKDB\n");
        sprintf(string, "PeakMax=0x%012llX, HEADROOM+MaxShiftBits=%d => PeakdB=0x%06X\n",
                        PeakMax & 0xFFFFFFFFFFFFLL,
                        HEADROOM + MaxShiftBits,
                        PeakdB & 0xFFFFFF);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT

    // smooth gains
    LimiterTargetGaindB = wsubsat(HeapPtr->LimiterThreshdB, PeakdB); // 8.16, [-128.0, 127.0] dB
    if(LimiterTargetGaindB < 0)
    {
        Temp = wfmul(LimiterTargetGaindB, Q23_ONE_TENTH); // divide by 10
        Temp >>= 1;  // divide by 2
        mdrc5b_pow10(Temp, &Exp, &TargetGain); // linear gain = 2^Exp * TargetGain
    }
    else
    {
        LimiterTargetGaindB = 0;
        TargetGain          = (1 << 22);
        Exp                 = 1;
    }

    LimiterGainMant[0] = HeapPtr->LimiterGainMant;
    LimiterGainExp     = HeapPtr->LimiterGainExp;
    if(LimiterGainExp >= Exp)
    {
        TargetGain = wmsr(TargetGain, LimiterGainExp - Exp);
    }
    else
    {
        LimiterGainMant[0] = wmsr(LimiterGainMant[0], Exp - LimiterGainExp);
        LimiterGainExp     = Exp;
    }
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_TARGETGAIN_EXP\n");
        sprintf(string, "TargetGain=0x%06X, Exp=%d\n", TargetGain, Exp);
        debug_write_string(string);
    }
#endif  // DEBUG_LIMITER_OUTPUT


#ifdef MDRC5B_DEBUG
printf("%d, peak %.3f, gain %.3f, exp %d, mant %.6f, %.6f\n", counter, (float)PeakdB/65536.0, (float)LimiterTargetGaindB/65536.0, LimiterGainExp, (float)LimiterGainMant[0]/8388608.0, (float)TargetGain/8388608.0);
#endif

    LimiterAtCoef  = HeapPtr->LimiterAtCoef;
    LimiterReCoef  = HeapPtr->LimiterReCoef;
    LimiterHoldRem = HeapPtr->LimiterHoldRem;
    LimiterHtSamp  = HeapPtr->LimiterHtSamp;

    if(TargetGain < LimiterGainMant[0]) // k refers to the previous sample, k+1 to the current sample
    {
        // attack
        Temp = LimiterGainMant[0];
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
            Temp                   = waddsat(TargetGain, wfmulr(LimiterAtCoef, wsubsat(Temp, TargetGain)));
            LimiterGainMant[k + 1] = Temp;
        }
        LimiterHoldRem = LimiterHtSamp; // init hold time
    }
    else if(TargetGain > LimiterGainMant[0])
    {
        // release
        Temp            = LimiterGainMant[0];
        n               = wmin(LimiterHoldRem, Samples);
        LimiterHoldRem -= n;
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < n; k++)
        {
            LimiterGainMant[k + 1] = Temp;
        }
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(; k < Samples; k++)
        {
            Temp                   = waddsat(TargetGain, wfmulr(LimiterReCoef, wsubsat(Temp, TargetGain)));
            LimiterGainMant[k + 1] = Temp;
        }
    }
    else
    {
        // TargetGain == LimiterGainMant[0] => no need to smooth
        Temp = TargetGain;
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
            LimiterGainMant[k + 1] = Temp;
        }
    }
    n                        = wedge(Temp);
    HeapPtr->LimiterGainMant = wmsl(Temp, n);
    HeapPtr->LimiterGainExp  = LimiterGainExp - n;
    HeapPtr->LimiterHoldRem  = LimiterHoldRem;
#ifdef DEBUG_LIMITER_OUTPUT
    if((debug_cpt_samples >= DEBUG_CPT_MIN) && (debug_cpt_samples <= DEBUG_CPT_MAX))
    {
        char string[100];

        debug_write_string("MRDC5B_LIMITER_SMOOTH_GAIN\n");
        sprintf(string, "TargetGain         = %d\n", TargetGain);
        debug_write_string(string);
        sprintf(string, "LimiterGainMant[0] = %d\n", LimiterGainMant[0]);
        debug_write_string(string);
        sprintf(string, "LimiterAtCoef      = %d\n", LimiterAtCoef);
        debug_write_string(string);
        sprintf(string, "LimiterReCoef      = %d\n", LimiterReCoef);
        debug_write_string(string);
        sprintf(string, "LimiterHoldRem     = %d\n", LimiterHoldRem);
        debug_write_string(string);
        sprintf(string, "LimiterHtSamp      = %d\n", LimiterHtSamp);
        for(k = 0; k < Samples; k++)
        {
            sprintf(string, "0x%06X ", LimiterGainMant[k + 1]);
            debug_write_string(string);
        }
        debug_write_string("\n");
    }
#endif  // DEBUG_LIMITER_OUTPUT

    // output
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
        MemOutPtr = HeapPtr->MainInBuf[ch];
        LaIdx     = HeapPtr->LimiterLaIdx; // this is now the oldest sample

#ifdef __flexcc2__
        Ptr2 = winit_circ_ptr(&HeapPtr->LimiterLABuf[ch][LaIdx],
                              HeapPtr->LimiterLABuf[ch],
                              &HeapPtr->LimiterLABuf[ch][lookahead_len]);
#else
        Ptr2 = &HeapPtr->LimiterLABuf[ch][LaIdx];  // go to the first valid sample
#endif

#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(k = 0; k < Samples; k++)
        {
            TempX = *Ptr2++;
#ifndef __flexcc2__
            if(Ptr2 == &(HeapPtr->LimiterLABuf[ch][lookahead_len]))
            {
                Ptr2 = HeapPtr->LimiterLABuf[ch];
            }
#endif // !__flexcc2__
            MemOutPtr[k] = wL_msl(ivc_fmul48x24(TempX, LimiterGainMant[k + 1]), LimiterGainExp);
        }
    }

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
