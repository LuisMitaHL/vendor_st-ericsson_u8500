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
 * \file   spl_M4.c
 * \brief
 * \author ST-Ericsson
 */
/**********************************************************************/
/**
  \file spl_M4.c
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
#ifdef M4
#include "audiolibs_common.h"
#include "spl_algo_M4.h"
#include "spl_util_M4.h"
#include "spl_dsp_M4.h"
#include "spl_rom_M4.h"
#ifdef SPL_DEBUG
long counter=0;
#endif

/************************* private functions *************************/
/* set default parameters */
void spl_default_param_settings(SPL_LOCAL_STRUCT_T * Heap) {
	/* common default parameters */
	Heap->Config.SPL_Mode         = SPL_LIMITATION;
	Heap->Config.Threshold        = SPL_THRESHOLD_DEFAULT;
	Heap->Config.SPL_UserGain     = SPL_USERGAIN_DEFAULT;
	Heap->Config.PeakL_Gain       = SPL_PEAKL_GAIN_DEFAULT;
	Heap->Config.Enable           = 0;
/*
	Heap->Enable_intern			  = 0;
	Heap->Enable_counter		  = 0;
	Heap->TargetThreshold		  = SPL_THRESHOLD_DEFAULT;
	Heap->CurrentThreshold		  = SPL_THRESHOLD_DEFAULT;
	Heap->UpdateThreshold		  = 1;
*/
	Heap->SampleFreq              = 48000;
	Heap->NumberChannel           = SPL_MAIN_CH_MAX;
	Heap->DataInOffset            = SPL_MAIN_CH_MAX;
	Heap->DataOutOffset           = SPL_MAIN_CH_MAX;
	if (Heap->Type != SPL_PEAK_LIMITER) {
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
	else {
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
void spl_derive_static_params(SPL_LOCAL_STRUCT_T * Heap) {
	int SampleFreq;
	int Tmp;
	long long Samples;
	SampleFreq = Heap->SampleFreq;
	// gain application attack coef
	Samples = (long long)Q23 * 1000000LL;
	Samples = Samples / SampleFreq;
	Samples = Samples / Heap->Tuning.GainAttackTime; // Q23
	Tmp = spl_pow10_coef(-(int)Samples);     // Q21
	Heap->GainAttackCoef = Tmp<<2;    // Q23
	// gain application release coef
	Samples = (long long)Q23 * 1000000LL;
	Samples = Samples / SampleFreq;
	Samples = Samples / Heap->Tuning.GainReleaseTime; // Q23
	Tmp = spl_pow10_coef(-(int)Samples);     // Q21
	Heap->GainReleaseCoef = Tmp<<2;    // Q23
	// threshold gain application attack coef
	Samples = (long long)Q23 * 1000000LL;
	Samples = Samples / SampleFreq;
	Samples = Samples / Heap->Tuning.ThreshAttackTime; // Q23
	Tmp = spl_pow10_coef(-(int)Samples);     // Q21
	Heap->ThreshAttackCoef = Tmp<<2;    // Q23
	// threshold gain application release coef
	Samples = (long long)Q23 * 1000000LL;
	Samples = Samples / SampleFreq;
	Samples = Samples / Heap->Tuning.ThreshReleaseTime; // Q23
	Tmp = spl_pow10_coef(-(int)Samples);     // Q21
	Heap->ThreshReleaseCoef = Tmp<<2;    // Q23
	// level estimation attack coef
	if (Heap->Tuning.PowerAttackTime != 0) {
		Samples = (long long)Q23 * 1000000LL;
		Samples = Samples / SampleFreq;
		Samples = Samples / Heap->Tuning.PowerAttackTime; // Q23
		Tmp = spl_pow10_coef(-(int)Samples);       // Q21
		Heap->PowerAttackCoef = Tmp<<2;  // Q23
	}
	else {
		Heap->PowerAttackCoef = 0;
	}
	// level estimation release coef
	if (Heap->Tuning.PowerReleaseTime != 0) {
		Samples = (long long)Q23 * 1000000LL;
		Samples = Samples / SampleFreq;
		Samples = Samples / Heap->Tuning.PowerReleaseTime; // Q23
		Tmp = spl_pow10_coef(-(int)Samples);     // Q21
		Heap->PowerReleaseCoef = Tmp<<2;  // Q23
	}
	else {
		Heap->PowerReleaseCoef = 0;
	}
	// hold samples
	Samples = (long long)SampleFreq * Heap->Tuning.GainHoldTime;
	Heap->GainHtSamp = Samples / 1000000;
	if (Heap->Type == SPL_PEAK_LIMITER) {
		Heap->PeakClipping = (int)Heap->Tuning.PeakClippingMode;
	}
	else {
		Heap->PeakClipping = SPL_NO_CLIPPING;
	}
	return;
}
/* return target gain in millibel */
int spl_get_gain(SPL_LOCAL_STRUCT_T * Heap) {
	int gain;
	/* in decimal mB */
	gain = (int)((((long long)Heap->AppliedGaindB ) * ((long long) 51200)) >> 23);
	return gain;
}
/* derive the internal variables used by the limiter */
/* according to the config settings */
void spl_derive_config_params(SPL_LOCAL_STRUCT_T * Heap) {
	int Tmp/*, Exp, Mantisse*/;
	if (Heap->Type == SPL_PEAK_LIMITER)
	{
		Tmp = Heap->Config.PeakL_Gain;
		// Heap->PeakL_GaindB = Tmp / 100; // convert millibel to dB
		Heap->PeakL_GaindB = (int)(((long long)0x800000+(2*(long long)Tmp*(long long)Q23_ONE_HUNDREDTH<<16))>>24); // result in Q16
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
	return;
}
void UpdateThresholds(SPL_LOCAL_STRUCT_T * Heap,int Threshold)
{
	int Tmp, Exp, Mantisse;
	Tmp = Threshold;
	Heap->ThresholddB = (int)(((long long)0x800000+(2*(long long)Tmp*(long long)Q23_ONE_HUNDREDTH<<16))>>24); // result in Q16
	if (Heap->Type == SPL_PEAK_LIMITER)
	{
		Tmp = (int)((((long long)Heap->ThresholddB ) * ((long long) Q23_ONE_TENTH)) >> 23); // divide by 10
		Tmp >>= 1;  // divide by 2
		spl_pow10(Tmp, &Exp, &Mantisse); // linear gain = 2^Exp * Mantisse
		Heap->ThresholdExp  = Exp;
		Heap->ThresholdMant = Mantisse;
		Tmp = (int)((((long long)(-Heap->ThresholddB) ) * ((long long) Q23_ONE_TENTH)) >> 23); //divide by 10
		Tmp >>= 1;  // divide by 2
		spl_pow10(Tmp, &Exp, &Mantisse); // linear gain = 2^Exp * Mantisse
		Heap->ThresholdInvExp  = Exp;
		Heap->ThresholdInvMant = Mantisse;
		if (Heap->ThresholdExp<0)
			Heap->Threshold = (Heap->ThresholdMant)>>(-Heap->ThresholdExp);
		else
			Heap->Threshold = (Heap->ThresholdMant)<<(Heap->ThresholdExp);
	}
}

/* set static parameter, internal function */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_static_param(SPL_LOCAL_STRUCT_T * Heap, SplParam_t *pParam) {
	int error = 0;
	Heap->memory_preset = pParam->MemoryPreset;
	error = spl_check_param_int (&Heap->Type, &pParam->Type,
			SPL_SOUND_PRESSURE_LIMITER, SPL_PEAK_LIMITER);

	if (Heap->Type == SPL_PEAK_LIMITER) {
		Heap->RmsMeasure = SPL_PEAK_MEASURE;
	}
	else {
		Heap->RmsMeasure = SPL_RMS_MEASURE;
	}
	return error;
}
/* set tuning parameter, internal function */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_tuning_param (SPL_LOCAL_STRUCT_T * Heap, SplTuning_t *pTuning) {
	int error = 0;
	error = spl_check_param_uint (&Heap->Tuning.PowerAttackTime, &pTuning->PowerAttackTime,
			SPL_POWER_ATTACK_TIME_MIN, SPL_POWER_ATTACK_TIME_MAX);
	error = spl_check_param_uint (&Heap->Tuning.PowerReleaseTime, &pTuning->PowerReleaseTime,
			SPL_POWER_RELEASE_TIME_MIN, SPL_POWER_RELEASE_TIME_MAX);
	error = spl_check_param_uint (&Heap->Tuning.GainAttackTime, &pTuning->GainAttackTime,
			SPL_GAIN_ATTACK_TIME_MIN, SPL_GAIN_ATTACK_TIME_MAX);
	error = spl_check_param_uint (&Heap->Tuning.GainReleaseTime, &pTuning->GainReleaseTime,
			SPL_GAIN_RELEASE_TIME_MIN, SPL_GAIN_RELEASE_TIME_MAX);
	error = spl_check_param_uint (&Heap->Tuning.GainHoldTime, &pTuning->GainHoldTime,
			SPL_GAIN_HOLD_TIME_MIN, SPL_GAIN_HOLD_TIME_MAX);
	if (Heap->Type == SPL_PEAK_LIMITER) {
		error = spl_check_param_uint (&Heap->Tuning.PeakClippingMode, &pTuning->PeakClippingMode,
				SPL_NO_CLIPPING, SPL_HARD_CLIPPING);
		error = spl_check_param_uint (&Heap->Tuning.ThreshAttackTime, &pTuning->ThreshAttackTime,
				SPL_GAIN_ATTACK_TIME_MIN, SPL_GAIN_ATTACK_TIME_MAX);
		error = spl_check_param_uint (&Heap->Tuning.ThreshReleaseTime, &pTuning->ThreshReleaseTime,
				SPL_GAIN_RELEASE_TIME_MIN, SPL_GAIN_RELEASE_TIME_MAX);
		error = spl_check_param_uint (&Heap->Tuning.ThreshHoldTime, &pTuning->ThreshHoldTime,
				SPL_GAIN_HOLD_TIME_MIN, SPL_GAIN_HOLD_TIME_MAX);
	}
	error = spl_check_param_uint (&Heap->Tuning.BlockSize, &pTuning->BlockSize,
			SPL_BLOCK_SIZE_MIN, SPL_BLOCK_SIZE_MAX);
	error = spl_check_param_uint (&Heap->Tuning.LookAheadSize, &pTuning->LookAheadSize,
			SPL_LOOKAHEAD_LEN_MIN, SPL_LOOKAHEAD_LEN_MAX);
	return error;
}
void spl_reset_io_buffers(SPL_LOCAL_STRUCT_T * Heap) {
	int i, k;
	// reset internal buffers
	for (i=0; i<SPL_MAIN_CH_MAX; i++) {
		for (k=0; k<SPL_BLOCK_SIZE_MAX; k++) {
			Heap->MainInBuf[i][k] = 0;
			Heap->MainOutBuf[i][k] = 0;
		}
		for (k=SPL_BLOCK_SIZE_MAX; k<SPL_BLOCK_SIZE_MAX*2; k++) {
			Heap->MainOutBuf[i][k] = 0;
		}
	}
	Heap->OutRdIdx = Heap->Tuning.BlockSize; // middle of buffer
	Heap->OutWtIdx = 0;
	Heap->InBufSamp = 0;
}
/* reset the internal variables/buffers */
void spl_reset_limiter(SPL_LOCAL_STRUCT_T * Heap) {
	int ch, k;
	for (ch=0; ch<Heap->NumberChannel; ch++) {
		Heap->PowerMeas[ch] = 0;
		for (k=0; k<Heap->Tuning.LookAheadSize; k++) {
			Heap->LimiterLABuf[ch][k] = 0; // buffer to subband lookahead
		}
	}
	Heap->LimiterLaIdx = 0;
	Heap->GainMant = Q23; // linear gain Q20
	Heap->GainExp = 0;
	Heap->GainHoldRem = 0;
	return;
}
inline int
apply_map16(const long long  tab[], const int diff_tab[],
		int diff_shift, int idx_width, int frac_width,
		const int x) {
	long long accu;
	int  vx;    /* signed but positive     */
	int      idx;
	int  ret, sgn;
	/* significant bit for x:
	   ----------------------
x:   1             bit of sign,
(idx_width+1) bits of index,
frac_width    bits of frac
*/
	vx  = abs(x);
	idx = vx>>frac_width;                   /* table index            */
	vx  = vx<<1;
	vx  = vx|1;                             /* inverse rounding       */
	vx  = vx<<idx_width;
	vx  = vx&0x7fffff;                     /* fractional index       */
	accu = (long long )tab[idx];                 /* deposit long in accu   */
	accu += (2*(long long)vx*(long long)diff_tab[idx]>>diff_shift);
	ret = (int)((accu+(long long)0x800000)>>24);
	sgn = x>>23;
	ret = ret^sgn;
	ret-=sgn;   /* copy sign and rounding */
	return ret;
}
/* final limiting stage, compare to limiting curve */
/* soft clipping, no sample above the threshold */
void spl_limiter_curve(int *MemOutPtr, int Size,
		SPL_LOCAL_STRUCT_T * HeapPtr) 
{
	int k;
	int InvExp=HeapPtr->ThresholdInvExp;
	int Exp=HeapPtr->ThresholdExp;
	/* take 2 bits of guard, and divide by threshold */
	/* multiply by 1/threshold = linear value of -ThresholddB */
	if ((Exp>=0)&&(InvExp>=0))
	{
		for (k = 0; k < Size; k++) 
		{
			// MemOutPtr[k] = wmsr(MemOutPtr[k], 1); /* divide by 2 */
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdInvMant)+0x400000)>>23)<<(InvExp);

			MemOutPtr[k] = apply_map16(PeakTab,PeakDiffTab,SPL_PEAK_DIFF_SHIFT,
					SPL_BASE_SIZE_PEAKTAB-1,
					23-SPL_BASE_SIZE_PEAKTAB,
					MemOutPtr[k]);
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdMant)+0x400000)>>23)<<(Exp);
		}
	}
	else if ((Exp>=0)&&(InvExp<0))
	{
		InvExp=-InvExp;
		for (k = 0; k < Size; k++) 
		{
			// MemOutPtr[k] = wmsr(MemOutPtr[k], 1); /* divide by 2 */
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdInvMant)+0x400000)>>23)>>(InvExp);

			MemOutPtr[k] = apply_map16(PeakTab,PeakDiffTab,SPL_PEAK_DIFF_SHIFT,
					SPL_BASE_SIZE_PEAKTAB-1,
					23-SPL_BASE_SIZE_PEAKTAB,
					MemOutPtr[k]);
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdMant)+0x400000)>>23)<<(Exp);
		}
	}
	else if ((Exp<0)&&(InvExp>=0))
	{
		Exp=-Exp;
		for (k = 0; k < Size; k++) 
		{
			// MemOutPtr[k] = wmsr(MemOutPtr[k], 1); /* divide by 2 */
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdInvMant)+0x400000)>>23)<<(InvExp);

			MemOutPtr[k] = apply_map16(PeakTab,PeakDiffTab,SPL_PEAK_DIFF_SHIFT,
					SPL_BASE_SIZE_PEAKTAB-1,
					23-SPL_BASE_SIZE_PEAKTAB,
					MemOutPtr[k]);
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdMant)+0x400000)>>23)>>(Exp);
		}
	}
	else
	{
		InvExp=-InvExp;
		Exp=-Exp;
		for (k = 0; k < Size; k++) 
		{
			// MemOutPtr[k] = wmsr(MemOutPtr[k], 1); /* divide by 2 */
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdInvMant)+0x400000)>>23)>>(InvExp);

			MemOutPtr[k] = apply_map16(PeakTab,PeakDiffTab,SPL_PEAK_DIFF_SHIFT,
					SPL_BASE_SIZE_PEAKTAB-1,
					23-SPL_BASE_SIZE_PEAKTAB,
					MemOutPtr[k]);
			MemOutPtr[k] = 
				(int)((((long long)MemOutPtr[k]*(long long)HeapPtr->ThresholdMant)+0x400000)>>23)>>(Exp);
		}
	}
}
/* main processing routine of the SPL. */
/* process exactly BlockSize samples  */
void spl_apply_limiter(SPL_LOCAL_STRUCT_T * HeapPtr)   
{
	int     LaIdx;
	int     NumberChannel;
	int     RmsMeasure;
	int     Samples;
	int     LookAheadSize;
	int     ch, k;
	int *Ptr;
	int *Ptr2;
	int *MemOutPtr;
	int  PeakdB;
	long long  Peak;
	long long  PeakMax;
	int GainAttackCoef;
	int GainReleaseCoef;
	int GainMant[SPL_BLOCK_SIZE_MAX + 1];
	int GainExp;
	int  TargetGaindB;
	int     GainHoldRem;
	int     GainHtSamp;
	int  Temp;
	int  MaxValuedB;
	long long  TempX, TempX2;
	int  Exp, TargetGain,j,tmp,len1;
	int Coef,coef[2],nomult;
	Samples = HeapPtr->Tuning.BlockSize;
	if(Samples == 0)
	{
		return;
	}
	LookAheadSize    = HeapPtr->Tuning.LookAheadSize;
	NumberChannel    = HeapPtr->NumberChannel;
	RmsMeasure       = HeapPtr->RmsMeasure;
	if ((HeapPtr->PowerAttackCoef==0)&&(HeapPtr->PowerReleaseCoef==0))
		nomult=1;
	else
	{
		nomult=0;
		coef[0]= HeapPtr->PowerAttackCoef;
		coef[1]= HeapPtr->PowerReleaseCoef;
	}
	// insert the new subband samples into the lookahead buffers
	LaIdx   = HeapPtr->LimiterLaIdx;
	PeakMax = 0;
	for (ch = 0; ch < NumberChannel; ch++)
	{
		Ptr = HeapPtr->MainInBuf[ch];
		Ptr2 = HeapPtr->LimiterLABuf[ch] + LaIdx;
		len1=(HeapPtr->LimiterLABuf[ch] + LookAheadSize)-Ptr2;
		len1=(len1>Samples ? Samples : len1);
		Peak = HeapPtr->PowerMeas[ch];
		// ()^2 and smooth
		if (RmsMeasure)
		{
			if (nomult==0)
			{
				for (k = 0; k < len1; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					TempX  = ((long long)Temp*(long long)Temp)<<1;
					TempX2 = Peak - TempX; 
					Coef=coef[1+(int)(TempX2>>63)]; //if TempX2 negative use PowerAttackCoef,else PowerReleaseCoef 
					//Peak = TempX+ spl_mul48x24_M4(TempX2, Coef);
					Peak=(TempX2>>23)*(long long)Coef;
					Peak+=((TempX2&0x7fffff)*(long long)Coef)>>23;
					Peak+=TempX;
					PeakMax = MAX(PeakMax, Peak);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					TempX  = ((long long)Temp*(long long)Temp)<<1;
					TempX2 = Peak - TempX;
					Coef=coef[1+(int)(TempX2>>63)]; //if TempX2 negative use PowerAttackCoef,else PowerReleaseCoef
					//Peak = TempX+ spl_mul48x24_M4(TempX2, Coef);
					Peak=(TempX2>>23)*(long long)Coef;
					Peak+=((TempX2&0x7fffff)*(long long)Coef)>>23;
					Peak+=TempX;
					PeakMax = MAX(PeakMax, Peak);
				}
			}
			else
			{
				for (k = 0; k < len1; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					Peak  = ((long long)Temp*(long long)Temp)<<1;
					PeakMax = MAX(PeakMax, Peak);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					Peak  = ((long long)Temp*(long long)Temp)<<1;
					PeakMax = MAX(PeakMax, Peak);
				}
			}
		}
		else // amplitude measure
		{
			if (nomult==0)
			{
				for (k = 0; k < len1; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					TempX  = ((long long)(abs(Temp)))<<24;
					TempX2 = Peak- TempX;
					Coef=coef[1+(int)(TempX2>>63)]; //if TempX2 negative use PowerAttackCoef,else PowerReleaseCoef
					//Peak = TempX+ spl_mul48x24_M4(TempX2, Coef);
					Peak=(TempX2>>23)*(long long)Coef;
					Peak+=((TempX2&0x7fffff)*(long long)Coef)>>23;
					Peak+=TempX;
					PeakMax = MAX(PeakMax, Peak);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					TempX  = ((long long)(abs(Temp)))<<24;
					TempX2 = Peak- TempX;
					Coef=coef[1+(int)(TempX2>>63)]; //if TempX2 negative use PowerAttackCoef,else PowerReleaseCoef
					//Peak = TempX+ spl_mul48x24_M4(TempX2, Coef);
					Peak=(TempX2>>23)*(long long)Coef;
					Peak+=((TempX2&0x7fffff)*(long long)Coef)>>23;
					Peak+=TempX;
					PeakMax = MAX(PeakMax, Peak);
				}
			}
			else
			{
				for (k = 0; k < len1; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					Peak  = ((long long)(abs(Temp)))<<24;
					PeakMax = MAX(PeakMax, Peak);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++)
				{
					Temp  = Ptr[k];
					*Ptr2++ = Temp;
					Peak  = ((long long)(abs(Temp)))<<24;
					PeakMax = MAX(PeakMax, Peak);
				}
			}
		}
		HeapPtr->PowerMeas[ch] = Peak;
	}
	// update index
	LaIdx                += Samples;
	HeapPtr->LimiterLaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);
	if (PeakMax < SPL_ALMOST_ZERO_THRESH)
	{
		PeakdB = SPL_POWER_DB_MINUS_INF<<16; // 8.16, [-128.0, 127.0] dB
	}
	else 
	{
		PeakdB = spl_log10(PeakMax); // 8.16, [-128.0, 127.0] dB
		if (RmsMeasure) 
		{
			PeakdB = (int)((((long long)PeakdB ) * ((long long) Q23_FiveOverEight)) >> 23); // x0.625
			PeakdB = PeakdB<<4; // x16
		}
		else 
		{
			PeakdB = (int)((((long long)PeakdB ) * ((long long) Q23_FiveOverEight)) >> 23); // x0.625
			PeakdB = PeakdB<< 5; // x32
		}
	}
	if (HeapPtr->Type == SPL_PEAK_LIMITER) 
	{
		TargetGaindB = HeapPtr->PeakL_GaindB;
		MaxValuedB= HeapPtr->PeakL_GaindB + PeakdB;
		if (MaxValuedB > HeapPtr->ThresholddB) 
		{
			TargetGaindB    = HeapPtr->ThresholddB-PeakdB;
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
		TargetGaindB        = HeapPtr->ThresholddB -  PeakdB; // 8.16, [-128.0, 127.0] dB
		TargetGaindB		= (TargetGaindB < 0 ? TargetGaindB : 0);
		GainAttackCoef      = HeapPtr->GainAttackCoef;
		GainReleaseCoef     = HeapPtr->GainReleaseCoef;
		GainHtSamp          = HeapPtr->GainHtSamp;
	}
	HeapPtr->AppliedGaindB  = TargetGaindB;
	// smooth gains
	TargetGaindB = (int)((((long long)TargetGaindB ) * ((long long) Q23_ONE_TENTH)) >> 23); // divide by 10
	TargetGaindB >>= 1;  // divide by 2
	spl_pow10(TargetGaindB, &Exp, &TargetGain); // linear gain = 2^Exp * TargetGain
	GainMant[0] = HeapPtr->GainMant;
	GainExp     = HeapPtr->GainExp;
	Temp        = GainExp - Exp;
	if (Temp >= 0) 
	{
		TargetGain = TargetGain>> Temp;
	}
	else 
	{
		GainMant[0] = GainMant[0]>>(-Temp);
		GainExp     = Exp;
	}
#ifdef SPL_DEBUG
	printf("%ld, peak %.3f, gain %.3f, exp %d, mant %.6f, %.6f\n", counter, (float)PeakdB/65536.0, (float)TargetGaindB/65536.0, GainExp, (float)GainMant[0]/8388608.0, (float)TargetGain/8388608.0);
#endif
	GainHoldRem = HeapPtr->GainHoldRem;
	Temp        = GainMant[0];
	if (TargetGain < Temp) 
	{// k refers to the previous sample, k+1 to the current sample
		// attack
		for (k = 0; k < Samples; k++) 
		{
			Temp = TargetGain + (int)(((long long)GainAttackCoef*(long long)(Temp-TargetGain)+0x400000)>>23);
			GainMant[k + 1] = Temp;
		}
		GainHoldRem = GainHtSamp; // init hold time
	}
	else 
	{
		// release
		if(GainHoldRem >= Samples)
		{
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
				for (k = 0; k < GainHoldRem; k++) 
				{
					GainMant[k + 1] = Temp;
				}
				GainHoldRem = 0;
			}
			for (k = GainHoldRem; k < Samples; k++) 
			{
				Temp = TargetGain + (int)(((long long)GainReleaseCoef*(long long)(Temp-TargetGain)+0x400000)>>23);
				GainMant[k + 1] = Temp;
			}
		}
	}
	//	k                    = wedge_M4(Temp);
	//	---------------------------------------
	k=0;
	tmp=abs(Temp);
	for(j=24;j>=2;j--)
	{
		if (tmp & 1) k=j;
		tmp=tmp>>1;
	}
	k-=2;
	//  -----------------------------------------
	HeapPtr->GainMant    = Temp<<k;
	HeapPtr->GainExp     = GainExp - k;
	HeapPtr->GainHoldRem = GainHoldRem;
	// output
	LaIdx = HeapPtr->LimiterLaIdx; // this is now the oldest sample
	for (ch = 0; ch < NumberChannel; ch++) 
	{
		MemOutPtr = HeapPtr->MainInBuf[ch];
		Ptr2 = HeapPtr->LimiterLABuf[ch] + LaIdx;
		len1=(HeapPtr->LimiterLABuf[ch] + LookAheadSize)-Ptr2;
		len1=(len1>Samples ? Samples : len1);
		if (HeapPtr->PeakClipping == SPL_SOFT_CLIPPING) 
		{
			Exp = GainExp - 1;  /* divide by 2 */
			if (Exp>=0)
			{
				for (k = 0; k < len1; k++) 
				{
					MemOutPtr[k] = (int)(((((long long)*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)<<Exp;
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++) 
				{
					MemOutPtr[k]=(int)(((((long long)*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)<<Exp;
				}
			}
			else
			{
				for (k = 0; k < len1; k++) 
				{
					MemOutPtr[k] = (int)(((((long long)*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)>>(-Exp);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++) 
				{
					MemOutPtr[k] = (int)(((((long long)*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)>>(-Exp);
				}
			}
		}
		else 
		{
			if (GainExp>=0) 
			{
				for (k = 0; k < len1; k++) 
				{
					MemOutPtr[k] = (int)((((long long)(*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)<<GainExp;
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++) 
				{
					MemOutPtr[k] = (int)((((long long)(*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)<<GainExp;
				}
			}
			else
			{
				for (k = 0; k < len1; k++) 
				{
					MemOutPtr[k] = (int)((((long long)(*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)>>(-GainExp);
				}
				Ptr2=HeapPtr->LimiterLABuf[ch];
				for (; k < Samples; k++) 
				{
					MemOutPtr[k] = (int)((((long long)(*Ptr2++)*(long long)GainMant[k+1])+0x400000)>>23)>>(-GainExp);
				}
			}
		}
		/* No sample above the threshold, compare to limiting curve */
		/* In peak limiter mode only (in SPL PeakClipping set to SPL_NO_CLIPPING */
		if (HeapPtr->PeakClipping == SPL_SOFT_CLIPPING) 
		{
			spl_limiter_curve(MemOutPtr, Samples, HeapPtr);
		}
		else if (HeapPtr->PeakClipping == SPL_HARD_CLIPPING) 
		{
			/* hard clipping */
			int ThresholdMin, ThresholdMax;
			ThresholdMax =  HeapPtr->Threshold;
			ThresholdMin = -ThresholdMax;
			for (k = 0; k < Samples; k++) 
			{
				Temp		= (ThresholdMax < MemOutPtr[k] ? ThresholdMax : MemOutPtr[k]);
				MemOutPtr[k] = (ThresholdMin > Temp ? ThresholdMin : Temp);
			}
		}
	}
}
/* update the look ahead buffer */
/* to be able to enable the limiter smoothly */
void spl_fill_lookahead_buffer(SPL_LOCAL_STRUCT_T * HeapPtr) {
	int LaIdx = 0;
	int NumberChannel;
	int Samples;
	int LookAheadSize;
	int ch, k;
	int *Ptr;
	int *Ptr2;
	int *MemOutPtr;
	int len1;
	Samples = HeapPtr->Tuning.BlockSize;
	LookAheadSize = HeapPtr->Tuning.LookAheadSize;
	NumberChannel = HeapPtr->NumberChannel;
	// insert the new subband samples into the lookahead buffers
	for (ch=0; ch<NumberChannel; ch++) {
		Ptr = HeapPtr->MainInBuf[ch];
		Ptr2 = HeapPtr->LimiterLABuf[ch];
		LaIdx = HeapPtr->LimiterLaIdx;
		len1=LookAheadSize-LaIdx;
		len1=(len1>Samples ? Samples : len1);
		for (k=0; k<len1; k++) 
		{
			Ptr2[LaIdx] = Ptr[k];
			LaIdx++;
		}
		LaIdx-=LookAheadSize;
		for (; k<Samples; k++) 
		{
			Ptr2[LaIdx] = Ptr[k];
			LaIdx++;
		}
	}
	// update index
	LaIdx = HeapPtr->LimiterLaIdx + Samples;
	HeapPtr->LimiterLaIdx = CIRC_ADD_H(LaIdx, LookAheadSize);
	// output
	for (ch=0; ch<NumberChannel; ch++) {
		MemOutPtr = HeapPtr->MainInBuf[ch];
		LaIdx = HeapPtr->LimiterLaIdx; // this is now the oldest sample
		len1=LookAheadSize-LaIdx;
		len1=(len1>Samples ? Samples : len1);
		for (k=0; k<len1; k++) 
		{
			MemOutPtr[k] = HeapPtr->LimiterLABuf[ch][LaIdx++];
		}
		LaIdx-=LookAheadSize;
		for (; k<Samples; k++) 
		{
			MemOutPtr[k] = HeapPtr->LimiterLABuf[ch][LaIdx++];
		}
	}
	return;
}
/************************* public functions **************************/
// set configuration parameters
// return values:
// 1:  to reset
// -1: wrong setting, but rounded, no need to exit
int spl_set_config(SPL_LOCAL_STRUCT_T * Heap, SplConfig_t *pConfig) {
	int error = 0;
	error = spl_check_param_int (&Heap->Config.Enable, &pConfig->Enable, 0, 1);
	error = spl_check_param_int (&Heap->Config.Threshold, &pConfig->Threshold,
			SPL_THRESHOLD_MIN, SPL_THRESHOLD_MAX);
	if (Heap->Type == SPL_SOUND_PRESSURE_LIMITER) {
		/* in this version only limitation mode is supported */
		error = spl_check_param_int (&Heap->Config.SPL_Mode, &pConfig->SPL_Mode,
				SPL_LIMITATION, SPL_LIMITATION);
		error = spl_check_param_int (&Heap->Config.SPL_UserGain, &pConfig->SPL_UserGain,
				SPL_USERGAIN_MIN, SPL_USERGAIN_MAX);
	}
	else {
		error = spl_check_param_int (&Heap->Config.PeakL_Gain, &pConfig->PeakL_Gain,
				SPL_PEAKL_GAIN_MIN, SPL_PEAKL_GAIN_MAX);
	}
	spl_derive_config_params(Heap);
	return error;
}
/* set stream parameter */
/* need for a reset after */
/* return -1 if one parameter is not between min, max value */
/* value is rounded, could keep on */
int spl_set_stream_param(SPL_LOCAL_STRUCT_T * Heap, SplStream_t *pStream) {
	int error = 0;
	error = spl_check_param_int (&Heap->SampleFreq, &pStream->SampleFreq,
			8000, 48000);
	error = spl_check_param_int (&Heap->NumberChannel, &pStream->NumberChannel,
			1, SPL_MAIN_CH_MAX);
	Heap->DataInOffset  = pStream->NumberChannel;
	Heap->DataOutOffset = pStream->NumberChannel;
	return error;
}
/* reset in case of stream property change, new stream etc. */
void spl_reset(SPL_LOCAL_STRUCT_T * Heap) {
	spl_reset_io_buffers(Heap);
	spl_reset_limiter(Heap);
	spl_derive_static_params(Heap);

	Heap->Enable_intern			  = 0;
	Heap->Enable_counter		  = 0;
	Heap->TargetThreshold		  = SPL_THRESHOLD_DEFAULT;
	Heap->CurrentThreshold		  = SPL_THRESHOLD_DEFAULT;
	Heap->UpdateThreshold		  = 1;

	spl_derive_config_params(Heap);
}
/* init:                                     */
/* if pParam  = NULL, use default parameters */
/* if pStream = NULL, use default parameters */
/* if pTuning = NULL, use default parameters */
/* return <0 if error in the parameters      */
/* but value are rounded, could keep on      */
int spl_init(SPL_LOCAL_STRUCT_T * Heap, SplParam_t *pParam,
		SplStream_t *pStream, SplTuning_t *pTuning) {
	int error = 0;
	Heap->Type = SPL_SOUND_PRESSURE_LIMITER;
	Heap->RmsMeasure = SPL_RMS_MEASURE; /* use RMS */
	Heap->memory_preset = MEM_PRESET_DEFAULT;
	if (pParam)
		error = spl_set_static_param(Heap, pParam);
	// note the order of calling sequence
	spl_default_param_settings(Heap);
	if (pTuning)
		error = spl_set_tuning_param(Heap, pTuning);
	if (pStream)
		error = spl_set_stream_param(Heap, pStream);
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
	if (error < 0)
		return INIT_FAILURE_CONFIGURE;
	return INIT_OK;
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
int spl_set_parameter(SPL_LOCAL_STRUCT_T * Heap, SplParam_t *pParam,
		SplStream_t *pStream, SplTuning_t *pTuning) {
	int error = 0;
	if (pParam)
		error = spl_set_static_param(Heap, pParam);
	if (pTuning)
		error = spl_set_tuning_param(Heap, pTuning);
	if (pStream)
		error = spl_set_stream_param(Heap, pStream);
	/* can be called after init, so need reset */
	spl_reset(Heap);
	return error;
}
/****************************************************************
 *                  processing entry routine
 ****************************************************************/
void spl_main(int *inbuf, int *outbuf, short size, SPL_LOCAL_STRUCT_T * Heap) {
	int ch;
	int PcminValidSamples;
	int Tmp;
	for (ch=0; ch<Heap->NumberChannel; ch++) {
		Heap->SysMainInPtr[ch] = &inbuf[ch];
		Heap->SysMainOutPtr[ch] = &outbuf[ch];
	}
	PcminValidSamples = size;
	while (PcminValidSamples > 0){
		Heap->ConsumedSamples = 0;
		Heap->ValidSamples = PcminValidSamples;
		// main processing steps
		spl_read(Heap);
		if (Heap->InBufSamp == Heap->Tuning.BlockSize) {
			if (Heap->Config.Enable)
			{
				Heap->Enable_intern=1;
				Heap->Enable_counter=SPL_ENABLE_COUNTER_MAX;
				Tmp = Heap->Config.PeakL_Gain;
				Heap->PeakL_GaindB = (int)(((long long)0x800000+(2*(long long)Tmp*(long long)Q23_ONE_HUNDREDTH<<16))>>24); // result in Q16


				Tmp=Heap->Config.Threshold;
				if (Heap->Type == SPL_SOUND_PRESSURE_LIMITER)
				{
					Tmp -= Heap->Config.SPL_UserGain;
					if (Tmp > 0)
						Tmp = 0;
				}
				Heap->TargetThreshold=Tmp;
			}
			else //if (Heap->Config.Enable)
			{
				if (Heap->Enable_counter==0)
				{
					Heap->Enable_intern=0;
				}
				else
				{
					(Heap->Enable_counter)--;
					Heap->PeakL_GaindB=0;
					Heap->TargetThreshold=0;
				}
			}
			if (Heap->CurrentThreshold>Heap->TargetThreshold)
			{
				Heap->UpdateThreshold=1;
				Heap->CurrentThreshold-=SPL_THRESHOLD_INCR;
				if (Heap->CurrentThreshold<Heap->TargetThreshold) 
				{
					Heap->CurrentThreshold=Heap->TargetThreshold;
				}
			}
			else if (Heap->CurrentThreshold<Heap->TargetThreshold)
			{
				Heap->UpdateThreshold=1;
				Heap->CurrentThreshold+=SPL_THRESHOLD_INCR;
				if (Heap->CurrentThreshold>Heap->TargetThreshold) 
				{
					Heap->CurrentThreshold=Heap->TargetThreshold;
				}
			}
			if (Heap->UpdateThreshold) UpdateThresholds(Heap,Heap->CurrentThreshold);
			if (Heap->CurrentThreshold==Heap->TargetThreshold)
			{
				Heap->UpdateThreshold=0;
			}
			//-------------------------------------------------------------------------------------------
			if (Heap->Enable_intern) {
				//-----------------------------------------------------------------
				spl_apply_limiter(Heap);
			}
			else {/* update the look ahead buffer */
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
		for (ch=0; ch<Heap->NumberChannel; ch++) {
			Heap->SysMainInPtr[ch]  += Heap->ConsumedSamples * Heap->DataInOffset;
			Heap->SysMainOutPtr[ch] += Heap->ConsumedSamples * Heap->DataOutOffset;
		}
	}
}
#endif //ifdef M4


