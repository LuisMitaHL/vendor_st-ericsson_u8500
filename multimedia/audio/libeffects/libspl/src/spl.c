/*####################################################################*/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 *
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl.c
 * \brief
 * \author ST-Ericsson
 */
/**********************************************************************/
/**
  \file spl.c
  \brief system interfacing routines
  \author Zong Wenbo, Matthieu Durnerin
  \email wenbo.zong@st.com, matthieu.durnerin@stericsson.com
  \date DEC 1,  2009
  \last rev Matthieu Durnerin March 26, 2010
*/

#ifdef SPL_DEBUG
#include <stdio.h>
#include <verbose.h>
#endif
#include "spl_defines.h"
#ifndef M4
#include "audiolibs_common.h"
#include "spl_algo.h"
#include "spl_util.h"
#include "spl_dsp.h"
#include "spl_rom.h"

#ifdef SPL_DEBUG
long counter = 0;
#endif


/************************* private functions *************************/
/* set default parameters */
void spl_default_param_settings(SPL_LOCAL_STRUCT_T *Heap)
{
   /* common default parameters */
   Heap->Config.SPL_Mode         = SPL_LIMITATION;
   Heap->Config.Threshold        = SPL_THRESHOLD_DEFAULT;
   Heap->Config.SPL_UserGain     = SPL_USERGAIN_DEFAULT;
   Heap->Config.PeakL_Gain       = SPL_PEAKL_GAIN_DEFAULT;
   Heap->Config.Enable           = FALSE;
/*
    Heap->Enable_intern          = 0;
    Heap->Enable_counter         = 0;
    Heap->TargetThreshold        = SPL_THRESHOLD_DEFAULT;
    Heap->CurrentThreshold       = SPL_THRESHOLD_DEFAULT;
    Heap->UpdateThreshold        = 1;
*/

   Heap->SampleFreq              = 48000;
   Heap->NumberChannel           = SPL_MAIN_CH_MAX;
   Heap->DataInOffset            = SPL_MAIN_CH_MAX;
   Heap->DataOutOffset           = SPL_MAIN_CH_MAX;

   if(Heap->Type != SPL_PEAK_LIMITER)
   {
      Heap->Tuning.PeakClippingMode  = SPL_NO_CLIPPING;
      Heap->Tuning.PowerAttackTime   = SPL_POWER_ATTACK_TIME_DEFAULT;
      Heap->Tuning.PowerReleaseTime  = SPL_POWER_RELEASE_TIME_DEFAULT;
      Heap->Tuning.GainAttackTime    = SPL_GAIN_ATTACK_TIME_DEFAULT;
      Heap->Tuning.GainReleaseTime   = SPL_GAIN_RELEASE_TIME_DEFAULT;
      Heap->Tuning.GainHoldTime      = SPL_GAIN_HOLD_TIME_DEFAULT;
      Heap->Tuning.ThreshAttackTime  = SPL_GAIN_ATTACK_TIME_DEFAULT;
      Heap->Tuning.ThreshReleaseTime = SPL_GAIN_RELEASE_TIME_DEFAULT;
      Heap->Tuning.ThreshHoldTime    = SPL_GAIN_HOLD_TIME_DEFAULT;
      Heap->Tuning.BlockSize         = SPL_BLOCK_SIZE_DEFAULT;
      Heap->Tuning.LookAheadSize     = SPL_LOOKAHEAD_LEN_DEFAULT;
   }
   else
   {
      Heap->Tuning.PeakClippingMode  = SPL_SOFT_CLIPPING;
      Heap->Tuning.PowerAttackTime   = SPL_POWER_ATTACK_TIME_PEAK_DEFAULT;
      Heap->Tuning.PowerReleaseTime  = SPL_POWER_RELEASE_TIME_PEAK_DEFAULT;
      Heap->Tuning.GainAttackTime    = SPL_GAIN_ATTACK_TIME_PEAK_DEFAULT;
      Heap->Tuning.GainReleaseTime   = SPL_GAIN_RELEASE_TIME_PEAK_DEFAULT;
      Heap->Tuning.GainHoldTime      = SPL_GAIN_HOLD_TIME_PEAK_DEFAULT;
      Heap->Tuning.ThreshAttackTime  = SPL_THRESH_ATTACK_TIME_PEAK_DEFAULT;
      Heap->Tuning.ThreshReleaseTime = SPL_THRESH_RELEASE_TIME_PEAK_DEFAULT;
      Heap->Tuning.ThreshHoldTime    = SPL_THRESH_HOLD_TIME_PEAK_DEFAULT;
      Heap->Tuning.BlockSize         = SPL_BLOCK_SIZE_PEAK_DEFAULT;
      Heap->Tuning.LookAheadSize     = SPL_LOOKAHEAD_LEN_PEAK_DEFAULT;
   }
}


/* derive the internal variables used by the limiter */
/* according to the static settings, tuning param and sample freq */
void spl_derive_static_params(SPL_LOCAL_STRUCT_T *Heap)
{
    int SampleFreq;
    Word24 Tmp;
    Word48 Samples;

    SampleFreq = Heap->SampleFreq;

    // gain application attack coef
    Samples = (Word48)Q23 * 1000000LL;
    Samples = Samples / SampleFreq;
    Samples = Samples / Heap->Tuning.GainAttackTime; // Q23
    Tmp = spl_pow10_coef(-(Word24)Samples);     // Q21
    Heap->GainAttackCoef = wmsl(Tmp, 2);    // Q23

    // gain application release coef
    Samples = (Word48)Q23 * 1000000LL;
    Samples = Samples / SampleFreq;
    Samples = Samples / Heap->Tuning.GainReleaseTime; // Q23
    Tmp = spl_pow10_coef(-(Word24)Samples);     // Q21
    Heap->GainReleaseCoef = wmsl(Tmp, 2);    // Q23

    // threshold gain application attack coef
    Samples = (Word48)Q23 * 1000000LL;
    Samples = Samples / SampleFreq;
    Samples = Samples / Heap->Tuning.ThreshAttackTime; // Q23
    Tmp = spl_pow10_coef(-(Word24)Samples);     // Q21
    Heap->ThreshAttackCoef = wmsl(Tmp, 2);    // Q23

    // threshold gain application release coef
    Samples = (Word48)Q23 * 1000000LL;
    Samples = Samples / SampleFreq;
    Samples = Samples / Heap->Tuning.ThreshReleaseTime; // Q23
    Tmp = spl_pow10_coef(-(Word24)Samples);     // Q21
    Heap->ThreshReleaseCoef = wmsl(Tmp, 2);    // Q23

    // level estimation attack coef
    if(Heap->Tuning.PowerAttackTime != 0)
    {
        Samples = (Word48)Q23 * 1000000LL;
        Samples = Samples / SampleFreq;
        Samples = Samples / Heap->Tuning.PowerAttackTime; // Q23
        Tmp = spl_pow10_coef(-(Word24)Samples);       // Q21
        Heap->PowerAttackCoef = wmsl(Tmp, 2);  // Q23
    }
    else
    {
        Heap->PowerAttackCoef = 0;
    }

    // level estimation release coef
    if(Heap->Tuning.PowerReleaseTime != 0)
    {
        Samples = (Word48)Q23 * 1000000LL;
        Samples = Samples / SampleFreq;
        Samples = Samples / Heap->Tuning.PowerReleaseTime; // Q23
        Tmp = spl_pow10_coef(-(Word24)Samples);     // Q21
        Heap->PowerReleaseCoef = wmsl(Tmp, 2);  // Q23
    }
    else
    {
        Heap->PowerReleaseCoef = 0;
    }

    // hold samples
    Samples = (Word48)SampleFreq * Heap->Tuning.GainHoldTime;
    Heap->GainHtSamp = Samples / 1000000;

    if(Heap->Type == SPL_PEAK_LIMITER)
    {
        Heap->PeakClipping = (int)Heap->Tuning.PeakClippingMode;
    }
    else
    {
        Heap->PeakClipping = SPL_NO_CLIPPING;
    }
}

/* derive the internal variables used by the limiter */
/* according to the config settings */
void spl_derive_config_params(SPL_LOCAL_STRUCT_T *Heap)
{
    Word24 Tmp;


    if(Heap->Type == SPL_PEAK_LIMITER)
    {
        Tmp = Heap->Config.PeakL_Gain;
        // Heap->PeakL_GaindB = Tmp / 100; // convert millibel to dB
        Heap->PeakL_GaindB = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16
    }

    /*
    VERBOSE(("SPL params\n"));
    VERBOSE(("SamplingFreq \t= %d\n",Heap->SampleFreq));
    VERBOSE(("NumberChannel \t= %d\n",Heap->NumberChannel));
    VERBOSE(("Type \t= %d\n",Heap->Type));
    VERBOSE(("PeakClippingMode \t= %d\n",Heap->Tuning.PeakClippingMode));
    VERBOSE(("PowerAttackTime \t= %d\n",Heap->Tuning.PowerAttackTime));
    VERBOSE(("PowerReleaseTime \t= %d\n",Heap->Tuning.PowerReleaseTime));
    VERBOSE(("GainAttackTime \t= %d\n",Heap->Tuning.GainAttackTime));
    VERBOSE(("GainReleaseTime \t= %d\n",Heap->Tuning.GainReleaseTime));
    VERBOSE(("GainHoldTime \t= %d\n",Heap->Tuning.GainHoldTime));
    VERBOSE(("ThreshAttackTime \t= %d\n",Heap->Tuning.ThreshAttackTime));
    VERBOSE(("ThreshReleaseTime \t= %d\n",Heap->Tuning.ThreshReleaseTime));
    VERBOSE(("ThreshHoldTime \t= %d\n",Heap->Tuning.ThreshHoldTime));
    VERBOSE(("BlockSize \t= %d\n",Heap->Tuning.BlockSize));
    VERBOSE(("LookAheadSize \t= %d\n",Heap->Tuning.LookAheadSize));

    VERBOSE(("SPL coef\n"));
    VERBOSE(("PowerAttackCoef \t= %d\n",Heap->PowerAttackCoef));
    VERBOSE(("PowerReleaseCoef \t= %d\n",Heap->PowerReleaseCoef));
    VERBOSE(("GainAttackCoef \t= %d\n",Heap->GainAttackCoef));
    VERBOSE(("GainReleaseCoef \t= %d\n",Heap->GainReleaseCoef));
    VERBOSE(("ThreshAttackCoef \t= %d\n",Heap->ThreshAttackCoef));
    VERBOSE(("ThreshReleaseCoef \t= %d\n",Heap->ThreshReleaseCoef));
    VERBOSE(("ThresholddB \t= %d\n",Heap->ThresholddB));
    */
}


void UpdateThresholds(SPL_LOCAL_STRUCT_T * Heap,int Threshold)
{
    Word24 Tmp, Exp, Mantisse; 
    
    Tmp = Threshold;
    Heap->ThresholddB = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16
    if (Heap->Type == SPL_PEAK_LIMITER)
    {
        Tmp = wfmul(Heap->ThresholddB, Q23_ONE_TENTH); // divide by 10
        Tmp >>= 1;  // divide by 2
        spl_pow10(Tmp, &Exp, &Mantisse); // linear gain = 2^Exp * Mantisse
        Heap->ThresholdExp  = Exp;
        Heap->ThresholdMant = Mantisse;
        Tmp = wfmul(-Heap->ThresholddB, Q23_ONE_TENTH); // divide by 10
        Tmp >>= 1;  // divide by 2
        spl_pow10(Tmp, &Exp, &Mantisse); // linear gain = 2^Exp * Mantisse
        Heap->ThresholdInvExp  = Exp;
        Heap->ThresholdInvMant = Mantisse;Heap->Threshold = wmsl(Heap->ThresholdMant, Heap->ThresholdExp);
    }
}


/* set static parameter, internal function */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_static_param(SPL_LOCAL_STRUCT_T *Heap, SplParam_t *pParam)
{
    int error = 0;

    error |= spl_check_param_int(&Heap->Type, &pParam->Type, SPL_SOUND_PRESSURE_LIMITER, SPL_PEAK_LIMITER);

    if(Heap->Type == SPL_PEAK_LIMITER)
    {
        Heap->RmsMeasure = SPL_PEAK_MEASURE;
    }
    else
    {
        Heap->RmsMeasure = SPL_RMS_MEASURE;
    }

   return error;
}

/* set tuning parameter, internal function */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_tuning_param (SPL_LOCAL_STRUCT_T *Heap, SplTuning_t *pTuning)
{
    int error = 0;

    error |= spl_check_param_uint(&Heap->Tuning.PowerAttackTime, &pTuning->PowerAttackTime,
                                  SPL_POWER_ATTACK_TIME_MIN, SPL_POWER_ATTACK_TIME_MAX);
    error |= spl_check_param_uint(&Heap->Tuning.PowerReleaseTime, &pTuning->PowerReleaseTime,
                                  SPL_POWER_RELEASE_TIME_MIN, SPL_POWER_RELEASE_TIME_MAX);
    error |= spl_check_param_uint(&Heap->Tuning.GainAttackTime, &pTuning->GainAttackTime,
                                  SPL_GAIN_ATTACK_TIME_MIN, SPL_GAIN_ATTACK_TIME_MAX);
    error |= spl_check_param_uint(&Heap->Tuning.GainReleaseTime, &pTuning->GainReleaseTime,
                                  SPL_GAIN_RELEASE_TIME_MIN, SPL_GAIN_RELEASE_TIME_MAX);
    error |= spl_check_param_uint(&Heap->Tuning.GainHoldTime, &pTuning->GainHoldTime,
                                  SPL_GAIN_HOLD_TIME_MIN, SPL_GAIN_HOLD_TIME_MAX);
    if(Heap->Type == SPL_PEAK_LIMITER) 
    {
        error |= spl_check_param_uint(&Heap->Tuning.PeakClippingMode, &pTuning->PeakClippingMode,
                                      SPL_NO_CLIPPING, SPL_HARD_CLIPPING);
        error |= spl_check_param_uint(&Heap->Tuning.ThreshAttackTime, &pTuning->ThreshAttackTime,
                                      SPL_GAIN_ATTACK_TIME_MIN, SPL_GAIN_ATTACK_TIME_MAX);
        error |= spl_check_param_uint(&Heap->Tuning.ThreshReleaseTime, &pTuning->ThreshReleaseTime,
                                      SPL_GAIN_RELEASE_TIME_MIN, SPL_GAIN_RELEASE_TIME_MAX);
        error |= spl_check_param_uint(&Heap->Tuning.ThreshHoldTime, &pTuning->ThreshHoldTime,
                                      SPL_GAIN_HOLD_TIME_MIN, SPL_GAIN_HOLD_TIME_MAX);
    }
    error |= spl_check_param_uint(&Heap->Tuning.BlockSize, &pTuning->BlockSize,
                                  SPL_BLOCK_SIZE_MIN, SPL_BLOCK_SIZE_MAX);
    error |= spl_check_param_uint(&Heap->Tuning.LookAheadSize, &pTuning->LookAheadSize,
                                  SPL_LOOKAHEAD_LEN_MIN, SPL_LOOKAHEAD_LEN_MAX);

    return error;
}


void spl_reset_io_buffers(SPL_LOCAL_STRUCT_T *Heap)
{
    int i, k;

    // reset internal buffers
    for(i = 0; i < SPL_MAIN_CH_MAX; i++)
    {
        for(k = 0; k < SPL_BLOCK_SIZE_MAX; k++)
        {
            Heap->MainInBuf [i][k] = 0;
            Heap->MainOutBuf[i][k] = 0;
        }
        for(k = SPL_BLOCK_SIZE_MAX; k < SPL_BLOCK_SIZE_MAX * 2; k++)
        {
            Heap->MainOutBuf[i][k] = 0;
        }
    }
 
    Heap->OutRdIdx  = Heap->Tuning.BlockSize; // middle of buffer
    Heap->OutWtIdx  = 0;
    Heap->InBufSamp = 0;
}


/* reset the internal variables/buffers */
void spl_reset_limiter(SPL_LOCAL_STRUCT_T *Heap)
{
    int ch, k;

    for(ch = 0; ch < Heap->NumberChannel; ch++)
    {
       Heap->PowerMeas[ch] = 0;

       for(k = 0; k < Heap->Tuning.LookAheadSize; k++)
       {
           Heap->LimiterLABuf[ch][k] = 0; // buffer to subband lookahead
       }
    }

    Heap->LimiterLaIdx = 0;

    Heap->GainMant = Q23; // linear gain Q20
    Heap->GainExp = 0;

    Heap->GainHoldRem = 0;
}


/*
  function: apply_map16()

  input x:  value in [-1,1)
  output y: value in [-1,1)   with    y = f(x)

  tab[]: 17 values tabulating f():
     tab[0]  = f(0)                   diff_tab[0]  = f(1/16)  - f(0)
     tab[1]  = f(1/16)                diff_tab[1]  = f(2/16)  - f(1/16)
     ...
     tab[15] = f(15/16)               diff_tab[15] = f(16/16) - f(15/16)
*/
inline MMshort apply_map16(const MMlong  tab[], const MMshort diff_tab[],
                           MMshort diff_shift, MMshort idx_width, MMshort frac_width,
                           const MMshort x)
{
    MMlonglong accu;
    MMshort  vx;    /* signed but positive     */
    int      idx;
    MMshort  ret, sgn;

    /* significant bit for x:
       ----------------------
       x:   1             bit of sign,
            (idx_width+1) bits of index,
            frac_width    bits of frac
    */
    vx  = wabssat(x);
    idx = wasr(vx,frac_width);                   /* table index            */
    vx  = wasl(vx,1);
    vx  = wor(vx,1);                             /* inverse rounding       */
    vx  = wasl(vx,idx_width);
    vx  = wand(vx,0x7fffff);                     /* fractional index       */
    accu = (MMlonglong)tab[idx];                 /* deposit long in accu   */
    accu = wX_add(accu, wX_asr(wX_fmul(vx, diff_tab[idx]), diff_shift));
                                                 /* linear interpolation   */
    ret = wround_X(accu);
    sgn = wasr(x,23);
    ret = wxor(ret, sgn);
    ret = wsub(ret, sgn);                        /* copy sign and rounding */

    return ret;
}


/* final limiting stage, compare to limiting curve */
/* soft clipping, no sample above the threshold */
void spl_limiter_curve(MMshort *MemOutPtr, int Size,
                       SPL_LOCAL_STRUCT_T *HeapPtr)
{
    int k;

    /* take 2 bits of guard, and divide by threshold */
    /* multiply by 1/threshold = linear value of -ThresholddB */
    #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
    for(k = 0; k < Size; k++)
    {
        // MemOutPtr[k] = wmsr(MemOutPtr[k], 1); /* divide by 2 */
        MemOutPtr[k] = wmsl(wfmulr(MemOutPtr[k], HeapPtr->ThresholdInvMant),
                            HeapPtr->ThresholdInvExp);

        MemOutPtr[k] = apply_map16(PeakTab,PeakDiffTab,SPL_PEAK_DIFF_SHIFT,
                                           SPL_BASE_SIZE_PEAKTAB-1,
                                           23-SPL_BASE_SIZE_PEAKTAB,
                                           MemOutPtr[k]);
        MemOutPtr[k] = wmsl(wfmulr(MemOutPtr[k], HeapPtr->ThresholdMant),
                            HeapPtr->ThresholdExp);
    }
}


/* main processing routine of the SPL. */
/* process exactly BlockSize samples  */
void spl_apply_limiter(SPL_LOCAL_STRUCT_T *HeapPtr) 
{
    int     LaIdx;
    int     NumberChannel;
    int     RmsMeasure;
    int     Samples;
    int     LookAheadSize;
    int     ch, k;
    MMshort *Ptr;
#ifdef __flexcc2__
    MMshort __circ *Ptr2;
#else // __flexcc2__
    MMshort *Ptr2;
#endif // __flexcc2__
    MMshort *MemOutPtr;
    Word24  PeakdB;
    Word48  Peak;
    Word48  PeakMax;
    MMshort PowerAttackCoef;
    MMshort PowerReleaseCoef;
    MMshort GainAttackCoef;
    MMshort GainReleaseCoef;
    MMshort GainMant[SPL_BLOCK_SIZE_MAX + 1];
    MMshort GainExp;
    Word24  TargetGaindB;
    int     GainHoldRem;
    int     GainHtSamp;
    Word24  Temp;
    Word24  MaxValuedB;
    Word48  TempX, TempX2;
    Word24  Exp, TargetGain;


    Samples = HeapPtr->Tuning.BlockSize;
    if(Samples == 0)
    {
        return;
    }
    LookAheadSize    = HeapPtr->Tuning.LookAheadSize;
    NumberChannel    = HeapPtr->NumberChannel;
    RmsMeasure       = HeapPtr->RmsMeasure;
    PowerAttackCoef  = HeapPtr->PowerAttackCoef;
    PowerReleaseCoef = HeapPtr->PowerReleaseCoef;

    // insert the new subband samples into the lookahead buffers
    LaIdx   = HeapPtr->LimiterLaIdx;
    PeakMax = 0;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(SPL_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumberChannel; ch++)
    {
        Ptr = HeapPtr->MainInBuf[ch];
#ifdef __flexcc2__
        Ptr2 = winit_circ_ptr(HeapPtr->LimiterLABuf[ch] + LaIdx,
                              HeapPtr->LimiterLABuf[ch],
                              HeapPtr->LimiterLABuf[ch] + LookAheadSize);
#else // __flexcc2__
        Ptr2 = HeapPtr->LimiterLABuf[ch] + LaIdx;
#endif // __flexcc2__

        Peak = HeapPtr->PowerMeas[ch];
        // ()^2 and smooth
        if(RmsMeasure)
        {
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < Samples; k++)
            {
                Temp  = Ptr[k];
                *Ptr2 = Temp;
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               HeapPtr->LimiterLABuf[ch] + LookAheadSize,
                               HeapPtr->LimiterLABuf[ch]);
#endif // __flexcc2__

                TempX  = wL_fmul(Temp, Temp); // Q47
                TempX2 = wL_subsat(Peak, TempX);
                if(TempX2 < 0)
                {
                    Peak = wL_addsat(TempX, spl_mul48x24(TempX2, PowerAttackCoef));
                }
                else
                {
                    Peak = wL_addsat(TempX, spl_mul48x24(TempX2, PowerReleaseCoef));
                }
                PeakMax = MAX(PeakMax, Peak);
            }
        }
        else // amplitude measure
        {
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < Samples; k++)
            {
                Temp  = Ptr[k];
                *Ptr2 = Temp;
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               HeapPtr->LimiterLABuf[ch] + LookAheadSize,
                               HeapPtr->LimiterLABuf[ch]);
#endif // __flexcc2__

                TempX  = wX_depmsp(wabssat(Temp));      // Q47
                TempX2 = wL_subsat(Peak, TempX);
                if(TempX2 < 0)
                {
                    Peak = wL_addsat(TempX, spl_mul48x24(TempX2, PowerAttackCoef));
                }
                else
                {
                    Peak = wL_addsat(TempX, spl_mul48x24(TempX2, PowerReleaseCoef));
                }
                PeakMax = MAX(PeakMax, Peak);
            }
        }
        HeapPtr->PowerMeas[ch] = Peak;
    }

    // update index
    LaIdx                += Samples;
    HeapPtr->LimiterLaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);

    if(PeakMax < SPL_ALMOST_ZERO_THRESH)
    {
        PeakdB = wmsl(SPL_POWER_DB_MINUS_INF, 16); // 8.16, [-128.0, 127.0] dB
    }
    else 
    {
        PeakdB = spl_log10(PeakMax); // 8.16, [-128.0, 127.0] dB
        if(RmsMeasure) 
        {
            PeakdB = wfmul(PeakdB, Q23_FiveOverEight); // x0.625
            PeakdB = wmsl(PeakdB, 4); // x16
        }
        else 
        {
            PeakdB = wfmul(PeakdB, Q23_FiveOverEight); // x0.625
            PeakdB = wmsl(PeakdB, 5); // x32
        }
    }
    if(HeapPtr->Type == SPL_PEAK_LIMITER) 
    {
        TargetGaindB = HeapPtr->PeakL_GaindB;
        MaxValuedB   = waddsat(HeapPtr->PeakL_GaindB, PeakdB);
        if (MaxValuedB > HeapPtr->ThresholddB) 
        {
            TargetGaindB    = wsubsat(HeapPtr->ThresholddB, PeakdB);
            GainAttackCoef  = HeapPtr->ThreshAttackCoef;
            GainReleaseCoef = HeapPtr->ThreshReleaseCoef;
            GainHtSamp      = HeapPtr->ThreshHtSamp;
        }
        else 
        {
            GainAttackCoef  = HeapPtr->GainAttackCoef;
            GainReleaseCoef = HeapPtr->GainReleaseCoef;
            GainHtSamp      = HeapPtr->GainHtSamp;
        }
    }
    else 
    {
        TargetGaindB        = wsubsat(HeapPtr->ThresholddB,  PeakdB); // 8.16, [-128.0, 127.0] dB
        TargetGaindB        = wmin(TargetGaindB, 0);
        GainAttackCoef      = HeapPtr->GainAttackCoef;
        GainReleaseCoef     = HeapPtr->GainReleaseCoef;
        GainHtSamp          = HeapPtr->GainHtSamp;
    }

    HeapPtr->AppliedGaindB  = TargetGaindB;
    // smooth gains
    TargetGaindB   = wfmul(TargetGaindB, Q23_ONE_TENTH); // divide by 10
    TargetGaindB >>= 1;  // divide by 2
    spl_pow10(TargetGaindB, &Exp, &TargetGain); // linear gain = 2^Exp * TargetGain
    
    GainMant[0] = HeapPtr->GainMant;
    GainExp     = HeapPtr->GainExp;
    Temp        = GainExp - Exp;
    if(Temp >= 0) 
    {
        TargetGain = wmsr(TargetGain, Temp);
    }
    else 
    {
        GainMant[0] = wmsr(GainMant[0], -Temp);
        GainExp     = Exp;
    }

#ifdef SPL_DEBUG
    printf("%ld, peak %.3f, gain %.3f, exp %d, mant %.6f, %.6f\n", counter, (float)PeakdB/65536.0, (float)TargetGaindB/65536.0, GainExp, (float)GainMant[0]/8388608.0, (float)TargetGain/8388608.0);
#endif

    GainHoldRem = HeapPtr->GainHoldRem;
    Temp        = GainMant[0];
    if(TargetGain < Temp) 
    {// k refers to the previous sample, k+1 to the current sample
        // attack
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
        for (k = 0; k < Samples; k++) 
        {
            Temp            = waddsat(TargetGain, wfmulr(GainAttackCoef, wsubsat(Temp, TargetGain)));
            GainMant[k + 1] = Temp;
        }
        GainHoldRem = GainHtSamp; // init hold time
    }
    else 
    {
        // release
        if(GainHoldRem >= Samples)
        {
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for (k = 0; k < Samples; k++) 
            {
                GainMant[k + 1] = Temp;
            }
            GainHoldRem -= Samples;
        }
        else
        {
            if(GainHoldRem > 0)
            {
#ifdef __flexcc2__
                #pragma loop minitercount(1)
                #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
                for (k = 0; k < GainHoldRem; k++) 
                {
                    GainMant[k + 1] = Temp;
                }
                GainHoldRem = 0;
            }
            
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = GainHoldRem; k < Samples; k++) 
            {
               Temp            = waddsat(TargetGain, wfmulr(GainReleaseCoef, wsubsat(Temp, TargetGain)));
               GainMant[k + 1] = Temp;
            }
        }
    }
    k                    = wedge(Temp);
    HeapPtr->GainMant    = wmsl(Temp, k);
    HeapPtr->GainExp     = GainExp - k;
    HeapPtr->GainHoldRem = GainHoldRem;

    
    // output
    LaIdx = HeapPtr->LimiterLaIdx; // this is now the oldest sample
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(SPL_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumberChannel; ch++) 
    {
        MemOutPtr = HeapPtr->MainInBuf[ch];

#ifdef __flexcc2__
        Ptr2 = winit_circ_ptr(HeapPtr->LimiterLABuf[ch] + LaIdx,
                              HeapPtr->LimiterLABuf[ch],
                              HeapPtr->LimiterLABuf[ch] + LookAheadSize);
#else // __flexcc2__
        Ptr2 = HeapPtr->LimiterLABuf[ch] + LaIdx;
#endif // __flexcc2__

        if(HeapPtr->PeakClipping == SPL_SOFT_CLIPPING) 
        {
            Exp = GainExp - 1;  /* divide by 2 */
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < Samples; k++) 
            {
                MemOutPtr[k] = wmsl(wfmulr(*Ptr2, GainMant[k + 1]), Exp);
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               HeapPtr->LimiterLABuf[ch] + LookAheadSize,
                               HeapPtr->LimiterLABuf[ch]);
#endif // __flexcc2__
            }
        }
        else 
        {
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < Samples; k++) 
            {
                MemOutPtr[k] = wmsl(wfmulr(*Ptr2, GainMant[k + 1]), GainExp);
#ifdef __flexcc2__
                Ptr2++;
#else // __flexcc2__
                Ptr2 = mod_add(Ptr2,
                               1,
                               HeapPtr->LimiterLABuf[ch] + LookAheadSize,
                               HeapPtr->LimiterLABuf[ch]);
#endif // __flexcc2__
            }
        }
        
        /* No sample above the threshold, compare to limiting curve */
        /* In peak limiter mode only (in SPL PeakClipping set to SPL_NO_CLIPPING */
        if(HeapPtr->PeakClipping == SPL_SOFT_CLIPPING) 
        {
            spl_limiter_curve(MemOutPtr, Samples, HeapPtr);
        }
        else if(HeapPtr->PeakClipping == SPL_HARD_CLIPPING) 
        {
            /* hard clipping */
            MMshort ThresholdMin, ThresholdMax;
            
            ThresholdMax =  HeapPtr->Threshold;
            ThresholdMin = -ThresholdMax;
#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < Samples; k++) 
            {
                Temp         = wmin(ThresholdMax, MemOutPtr[k]);
                MemOutPtr[k] = wmax(ThresholdMin, Temp);
            }
        }
    }
}


/* update the look ahead buffer */
/* to be able to enable the limiter smoothly */
void spl_fill_lookahead_buffer(SPL_LOCAL_STRUCT_T *HeapPtr) 
{
    int LaIdx = 0;
    int NumberChannel;
    int Samples;
    int LookAheadSize;
    int ch, k;
    MMshort *Ptr;
    MMshort *Ptr2;
    MMshort *MemOutPtr;

    Samples = HeapPtr->Tuning.BlockSize;
    LookAheadSize = HeapPtr->Tuning.LookAheadSize;

    NumberChannel = HeapPtr->NumberChannel;

    // insert the new subband samples into the lookahead buffers
    for(ch = 0; ch < NumberChannel; ch++)
    {
        Ptr = HeapPtr->MainInBuf[ch];
        Ptr2 = HeapPtr->LimiterLABuf[ch];
        LaIdx = HeapPtr->LimiterLaIdx;

        for(k = 0; k < Samples; k++)
        {
            Ptr2[LaIdx] = Ptr[k];
            LaIdx++;
            LaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);
        }
    }
    // update index
    LaIdx = HeapPtr->LimiterLaIdx + Samples;
    HeapPtr->LimiterLaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);

    // output
    for(ch = 0; ch < NumberChannel; ch++)
    {
        MemOutPtr = HeapPtr->MainInBuf[ch];
        LaIdx = HeapPtr->LimiterLaIdx; // this is now the oldest sample

#ifdef __flexcc2__
        #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
        for(k = 0; k < Samples; k++)
        {
            MemOutPtr[k] = HeapPtr->LimiterLABuf[ch][LaIdx];
            LaIdx++;
            LaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);
        }
    }
}


/************************* public functions **************************/
// set configuration parameters
// return values:
// 1:  to reset
// -1: wrong setting, but rounded, no need to exit
int spl_set_config(SPL_LOCAL_STRUCT_T *Heap, SplConfig_t *pConfig)
{
    int error = 0;

    error |= spl_check_param_int(&Heap->Config.Enable, &pConfig->Enable, 0, 1);
    error |= spl_check_param_int(&Heap->Config.Threshold, &pConfig->Threshold,
                                 SPL_THRESHOLD_MIN, SPL_THRESHOLD_MAX);
    if(Heap->Type == SPL_SOUND_PRESSURE_LIMITER)
    {
        /* in this version only limitation mode is supported */
        error |= spl_check_param_int(&Heap->Config.SPL_Mode, &pConfig->SPL_Mode,
                                     SPL_LIMITATION, SPL_LIMITATION);
        error |= spl_check_param_int(&Heap->Config.SPL_UserGain, &pConfig->SPL_UserGain,
                                     SPL_USERGAIN_MIN, SPL_USERGAIN_MAX);
    }
    else
    {
        error |= spl_check_param_int(&Heap->Config.PeakL_Gain, &pConfig->PeakL_Gain,
                                     SPL_PEAKL_GAIN_MIN, SPL_PEAKL_GAIN_MAX);
    }
    spl_derive_config_params(Heap);

    return error;
}


/* set stream parameter */
/* need for a reset after */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_stream_param(SPL_LOCAL_STRUCT_T *Heap, SplStream_t *pStream)
{
    int error = 0;

    error |= spl_check_param_int(&Heap->SampleFreq, &pStream->SampleFreq,
                                 8000, 48000);
    error |= spl_check_param_int(&Heap->NumberChannel, &pStream->NumberChannel,
                                 1, SPL_MAIN_CH_MAX);
    Heap->DataInOffset  = pStream->NumberChannel;
    Heap->DataOutOffset = pStream->NumberChannel;

    return error;
}


/* reset in case of stream property change, new stream etc. */
void spl_reset(SPL_LOCAL_STRUCT_T *Heap)
{
    spl_reset_io_buffers(Heap);
    spl_reset_limiter(Heap);
    spl_derive_static_params(Heap);
    
    Heap->Enable_intern    = 0;
    Heap->Enable_counter   = 0;
    Heap->TargetThreshold  = SPL_THRESHOLD_DEFAULT;
    Heap->CurrentThreshold = SPL_THRESHOLD_DEFAULT;
    Heap->UpdateThreshold  = 1;

    spl_derive_config_params(Heap);
}


/* init:                                     */
/* if pParam  = NULL, use default parameters */
/* if pStream = NULL, use default parameters */
/* if pTuning = NULL, use default parameters */
/* return <0 if error in the parameters      */
/* but value are rounded, could keep on      */
int spl_init(SPL_LOCAL_STRUCT_T *Heap, SplParam_t *pParam,
             SplStream_t *pStream, SplTuning_t *pTuning)
{
    int error = 0;

    Heap->Type = SPL_SOUND_PRESSURE_LIMITER;
    Heap->RmsMeasure = SPL_RMS_MEASURE; /* use RMS */

    if(pParam)
    {
        error |= spl_set_static_param(Heap, pParam);
    }

    // note the order of calling sequence
    spl_default_param_settings(Heap);

    if(pTuning)
    {
        error |= spl_set_tuning_param(Heap, pTuning);
    }

    if(pStream)
    {
        error |= spl_set_stream_param(Heap, pStream);
    }

    spl_reset(Heap);
    /*
      printf("Type %d\n", Heap->Type);
      printf("SampleFreq %d\n", Heap->SampleFreq);
      printf("NumberChannel %d\n", Heap->NumberChannel);
      printf("PowerAttackCoef %d\n", Heap->PowerAttackCoef);
      printf("PowerReleaseCoef %d\n", Heap->PowerReleaseCoef);
      printf("GainAttackCoef %d\n", Heap->GainAttackCoef);
      printf("GainReleaseCoef %d\n", Heap->GainReleaseCoef);
      printf("GainHtSamp %d\n", Heap->GainHtSamp);
      printf("ThresholddB %d\n", Heap->ThresholddB);
      printf("PeakL_GaindB %d\n", Heap->PeakL_GaindB);
    */
#ifdef UNIX
    printf("Init done...\n");
#endif

    return error;
}

/* set parameter:                                     */
/* if pParam  = NULL, do not set param parameters */
/* if pStream = NULL, do not set stream parameters */
/* if pTuning = NULL, do not set tuning parameters */
/* return <0 if error in the parameters      */
/* but value are rounded, could keep on      */
/* spl_set_parameter should be used if static parameters have to be set after */
/* the init. If applied before the spl_init function, be sure to use the same */
/* parameters in the init function to avoid reinitializing the parameters */
int spl_set_parameter(SPL_LOCAL_STRUCT_T *Heap, SplParam_t *pParam,
                      SplStream_t *pStream, SplTuning_t *pTuning)
{
    int error = 0;

    if(pParam)
    {
        error |= spl_set_static_param(Heap, pParam);
    }

    if(pTuning)
    {
        error |= spl_set_tuning_param(Heap, pTuning);
    }

    if(pStream)
    {
        error |= spl_set_stream_param(Heap, pStream);
    }

    /* can be called after init, so need reset */
    spl_reset(Heap);

    return error;
}


/****************************************************************
 *                  processing entry routine
 ****************************************************************/
void spl_main(MMshort *inbuf, MMshort *outbuf, short size, SPL_LOCAL_STRUCT_T *Heap)
{
    int ch;
    int PcminValidSamples;
    int Tmp;

    for(ch = 0; ch < Heap->NumberChannel; ch++)
    {
       Heap->SysMainInPtr [ch] = &inbuf[ch];
       Heap->SysMainOutPtr[ch] = &outbuf[ch];
    }

    PcminValidSamples = size;

    while(PcminValidSamples > 0)
    {
        Heap->ConsumedSamples = 0;
        Heap->ValidSamples = PcminValidSamples;

        // main processing steps
        spl_read(Heap);

        if(Heap->InBufSamp == Heap->Tuning.BlockSize)
        {
            if(Heap->Config.Enable)
            {
                Heap->Enable_intern = 1;
                Heap->Enable_counter = SPL_ENABLE_COUNTER_MAX;
                Tmp = Heap->Config.PeakL_Gain;
                Heap->PeakL_GaindB = wround_L(wL_msl(wL_fmul(Tmp, Q23_ONE_HUNDREDTH), 16)); // result in Q16
                Tmp=Heap->Config.Threshold;
                if(Heap->Type == SPL_SOUND_PRESSURE_LIMITER)
                {
                    Tmp -= Heap->Config.SPL_UserGain;
                    if(Tmp > 0)
                    {
                        Tmp = 0;
                    }
                }
                Heap->TargetThreshold = Tmp;
            }
            else //if (Heap->Config.Enable)
            {
                if(Heap->Enable_counter == 0)
                {
                    Heap->Enable_intern = 0;
                }
                else
                {
                    Heap->Enable_counter--;
                    Heap->PeakL_GaindB = 0;
                    Heap->TargetThreshold = 0;
                }
            }
            if(Heap->CurrentThreshold > Heap->TargetThreshold)
            {
                Heap->UpdateThreshold = 1;
                Heap->CurrentThreshold -= SPL_THRESHOLD_INCR;
                if (Heap->CurrentThreshold < Heap->TargetThreshold) 
                {
                    Heap->CurrentThreshold = Heap->TargetThreshold;
                }
            }
            else if(Heap->CurrentThreshold < Heap->TargetThreshold)
            {
                Heap->UpdateThreshold = 1;
                Heap->CurrentThreshold += SPL_THRESHOLD_INCR;
                if(Heap->CurrentThreshold > Heap->TargetThreshold) 
                {
                    Heap->CurrentThreshold = Heap->TargetThreshold;
                }
            }
            if(Heap->UpdateThreshold)
            {
                UpdateThresholds(Heap, Heap->CurrentThreshold);
            }
            if(Heap->CurrentThreshold == Heap->TargetThreshold)
            {
                Heap->UpdateThreshold = 0;
            }
            //-------------------------------------------------------------------------------------------
            if(Heap->Enable_intern)
            {
                spl_apply_limiter(Heap);
            }
            else
            {
                /* update the look ahead buffer */
                /* to be able to enable the limiter smoothly */
                spl_fill_lookahead_buffer(Heap);
            }

            spl_move(Heap); // move processed data to output buffer

#ifdef SPL_DEBUG
            counter+=Heap->Tuning.BlockSize;
#endif
        }

        spl_write(Heap);

        // Update the sample counters
        PcminValidSamples -= Heap->ConsumedSamples;

        // Update system input and output pointers
        for(ch = 0; ch < Heap->NumberChannel; ch++)
        {
            Heap->SysMainInPtr [ch] += Heap->ConsumedSamples * Heap->DataInOffset;
            Heap->SysMainOutPtr[ch] += Heap->ConsumedSamples * Heap->DataOutOffset;
        }
    }
}

#endif // ifndef M4
