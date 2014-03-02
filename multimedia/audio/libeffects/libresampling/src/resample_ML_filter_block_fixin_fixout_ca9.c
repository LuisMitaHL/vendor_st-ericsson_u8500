/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_ML_filter_block_fixin_fixout_ca9.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/libresampling.nmf>
#endif
#include "resample_local.h"
#ifndef M4_LL_MSEC
#ifdef NEON
#ifdef STM_ARM_NEON_MODELS
#include "stm-arm-neon.h"
#else
#include "arm_neon.h"
#endif
#endif
#if defined(ARM)||defined(NEON)
#define SATMAX32 0x7fffffff
#define SATMAX16 0x7fff
#define INCR_SHORT 2
#define INCR_INT 4
static void
resample_ML_conv_block_fixin_fixout_16_16 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
);
static void
resample_ML_conv_block_fixin_fixout_32_16 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
);
static void
resample_ML_conv_block_fixin_fixout_32_32 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
);
static void
resample_ML_convout_block_fixin_fixout_16_16 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
);
static void
resample_ML_convout_block_fixin_fixout_32_16 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
);
static void
resample_ML_convout_block_fixin_fixout_32_32 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
);
static void
resample_ML_filter_block_fixin_fixout_16_16  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
);
static void
resample_ML_filter_block_fixin_fixout_32_16  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
);
static void
resample_ML_filter_block_fixin_fixout_32_32  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
);
/********** COMMON CODE FOR UP AND DOWN SAMPLING ****************/

void
resample_ML_filter_block_fixin_fixout  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
)
{

	if (delayLine->ARM_Sample16_config)
		resample_ML_filter_block_fixin_fixout_16_16(in,istride,nbIn,out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam,squelch);
	else if (delayLine->ARM_Coef16)
		resample_ML_filter_block_fixin_fixout_32_16(in,istride,nbIn,out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam,squelch);
	else
		resample_ML_filter_block_fixin_fixout_32_32(in,istride,nbIn,out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam,squelch);
}
#pragma inline
static void
resample_ML_filter_block_fixin_fixout_16_16  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	int  val ;

#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	short *p_coef_s;
	short *p_coef_s_lsb;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	short *p_val_s ;
	short 	 *max_adr_s = (short*)baseAddress+(short)(delayLine->len);
	short 	 *min_adr_s = (short *)baseAddress;
	int flush_mask;
	int vector_len,vector_loop_shift;
	void *void_inptr,*void_outptr;
	int SATMAXCA9;

	p_val_s=(short *)baseAddress;
	p_val_s += (short)currIndex;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	void_outptr=out;
	if ((nbIn==0)||(squelch))
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	{
		if (offset>=M)
		{
			incr=(incr_offset_tab[offset]>>8)&0xff;
			offset=incr_offset_tab[offset]&0xff;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=(*(short*)void_inptr)&flush_mask;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_SHORT);
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
#if defined(NEON) && !defined(M4)
			sum_vec_s_lsb=vdupq_n_s32(0);
			sum_vec_s=vdupq_n_s32(0);
			if (!(delayLine->ARM_Coef16))
			{
				for (i=(actualLen)>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s);
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));
				Acc+=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=(actualLen)>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s);
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=(int)Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			for (i=0;i<actualLen>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if(!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
			}
			Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
			Acc<<=8;
			Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
			Acc = (Acc+0x400000)>>23;
			if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc;
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;
			*(short*)void_outptr=val;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_SHORT);
			for (i=incr;i>0;i--)
			{
				/* put in delay line */
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=(*(short*)void_inptr)&flush_mask;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_SHORT);
				*p_val_s=(short)val;
				*(p_val_s+delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
	} 
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
}
#pragma inline
static void
resample_ML_filter_block_fixin_fixout_32_16  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	int  val ;
#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	short *p_coef_s;
	short *p_coef_s_lsb;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	short *p_val_s ;
	short 	 *max_adr_s = (short*)baseAddress+(short)(delayLine->len);
	short 	 *min_adr_s = (short *)baseAddress;
	int flush_mask;
	int vector_len,vector_loop_shift;
	void *void_inptr,*void_outptr;
	int SATMAXCA9;

	p_val_s=(short *)baseAddress;
	p_val_s += (short)currIndex;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	void_outptr=out;
	if ((nbIn==0)||(squelch))
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	{
		if (offset>=M)
		{
			incr=(incr_offset_tab[offset]>>8)&0xff;
			offset=incr_offset_tab[offset]&0xff;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=((*(int*)void_inptr)&flush_mask)>>16;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
#if defined(NEON) && !defined(M4)
			sum_vec_s_lsb=vdupq_n_s32(0);
			sum_vec_s=vdupq_n_s32(0);
			if (!(delayLine->ARM_Coef16))
			{
				for (i=(actualLen)>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s);
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));
				Acc+=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=(actualLen)>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s);
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=(int)Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			for (i=0;i<actualLen>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if(!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
			}
			Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
			Acc<<=8;
			Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
			Acc = (Acc+0x400000)>>23;
			if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc;
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;
			*(int*)void_outptr=val<<16;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_INT);
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=((*(int*)void_inptr)&flush_mask)>>16;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
	}
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
}
#pragma inline
static void
resample_ML_filter_block_fixin_fixout_32_32  
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	int f_index;
	unsigned int offset = delayLine->offset;
	int  val ;
#if defined(NEON) && !defined(M4)
	int32x4_t h_vec;
	int32x4_t x_vec;
	int64x2_t sum_vec;
#else
	long long sum0,sum1,sum2,sum3;
#endif
	long long Acc=0;
	int *p_coef;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	int *p_val    ;
	int      *max_adr = &baseAddress[(delayLine->len)];
	int      *min_adr = baseAddress;
	int flush_mask;
	int vector_len,vector_loop_shift;
	void *void_inptr,*void_outptr;
	int SATMAXCA9;
	SATMAXCA9=SATMAX32;
	vector_len=(int)VECTOR_LEN_CA9_MUL32_64;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL32_64;
	void_inptr=in;
	void_outptr=out;
	if ((nbIn==0)||(squelch))
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	p_val = baseAddress;
	p_val += currIndex;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	if (offset>=M)
	{
		incr=(incr_offset_tab[offset]>>8)&0xff;
		offset=incr_offset_tab[offset]&0xff;
		{
			for (i=incr;i>0;i--)
			{
				if (p_val==min_adr)
					p_val=max_adr-1;
				else
					p_val--;
				val=(*(int*)void_inptr)&flush_mask;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				*p_val=val;
				*(p_val+delayLine->len)=val;
			}
		} 
		nbin_to_consume-=incr;
	}
	for (j=nbOutRequested;j>0;j--)
	{
		f_index = offset*(delta+delayLine->len);
		p_coef = &CoefRam[f_index];
#if defined(NEON) && !defined(M4)
		sum_vec=vdupq_n_s64(0);
		for (i=actualLen>>vector_loop_shift;i>0;i--)
		{
			h_vec=vld1q_s32(p_coef);
			x_vec=vld1q_s32(p_val);
			p_coef+=4;
			p_val+=4;
			sum_vec=vqdmlal_s32(sum_vec,vget_low_s32(x_vec), vget_low_s32(h_vec));
			sum_vec=vqdmlal_s32(sum_vec,vget_high_s32(x_vec), vget_high_s32(h_vec));
		}
		Acc=vget_lane_s64(vqadd_s64(vget_high_s64(sum_vec),vget_low_s64(sum_vec)),0);
		p_val-=actualLen;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#else	// ifdef NEON
		sum0=sum1=sum2=sum3=0;
		for (i=0;i<actualLen>>vector_loop_shift;i++)
		{
			sum0+=((long long)p_val[vector_len*i+0]*(long long)p_coef[vector_len*i+0])<<1;
			sum1+=((long long)p_val[vector_len*i+1]*(long long)p_coef[vector_len*i+1])<<1;
			sum2+=((long long)p_val[vector_len*i+2]*(long long)p_coef[vector_len*i+2])<<1;
			sum3+=((long long)p_val[vector_len*i+3]*(long long)p_coef[vector_len*i+3])<<1;
		}
		Acc=sum0+sum1+sum2+sum3;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=SATMAXCA9;
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#endif  // ifdef NEON
		incr=(incr_offset_tab[offset]>>8)&0xff;
		if (nbin_to_consume<incr)
			incr=nbin_to_consume;
		offset=incr_offset_tab[offset]&0xff;
		*(int*)void_outptr=val;
		void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_INT);
		for (i=incr;i>0;i--)
		{
			/* put in delay line */
			if (p_val==min_adr)
				p_val=max_adr-1;
			else
				p_val--;
			val=(*(int*)void_inptr)&flush_mask;
			void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
			*p_val=val;
			*(p_val+delayLine->len)=val;
		}
		nbin_to_consume-=incr;
	}
	delayLine->currIndex = p_val - baseAddress;
	*nbOutAvail = nbOutRequested;
}


/********** CODE FOR UP SAMPLING ONLY ***************************/
void
resample_ML_load_block_fixin_fixout(int *in,
		unsigned int    istride,
		unsigned int    nbIn,
		ResampleDelayLine *delayLine,
		ResampleFilter *filterInfo,
		int squelch)
{
	int   *baseAddress = &delayLine->baseAddress[0];
	int *p_val    ;
	short *p_val_s;
	int j;
	void *void_inptr;
	int nbremain = delayLine->nbremain;
	int      *max_adr = &baseAddress[delayLine->totalen];
	int      *min_adr = baseAddress;
	short    *min_adr_s = (short*)baseAddress;
	short    *max_adr_s = min_adr_s+(short)(delayLine->totalen);

	void_inptr=in;
	p_val_s=(short*)baseAddress;
	p_val_s+=(short)(delayLine->writeIndex);
	p_val = baseAddress;
	p_val += delayLine->writeIndex;
	if ((delayLine->ARM_Sample16_config)||(delayLine->ARM_Coef16))
	{
		if (squelch) 
		{
			for (j=nbIn;j>0;j--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				*p_val_s = 0;
				*(p_val_s+(short)(delayLine->totalen))=*p_val_s;
				nbremain++;
			}
		}
		else 
		{
			if (!delayLine->ARM_Sample16_config)
			{
				for (j=nbIn;j>0;j--)
				{
					if (p_val_s==min_adr_s)
						p_val_s=max_adr_s-1;
					else
						p_val_s--;
					*p_val_s=(*(int*)void_inptr)>>16;
					*(p_val_s+(short)(delayLine->totalen))=*p_val_s;
					void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
					nbremain++;
				}
			}
			else
			{
				for (j=nbIn;j>0;j--)
				{
					if (p_val_s==min_adr_s)
						p_val_s=max_adr_s-1;
					else
						p_val_s--;
					*p_val_s=*(short*)void_inptr;
					*(p_val_s+(short)(delayLine->totalen))=*p_val_s;
					void_inptr=(void*)(((char*)void_inptr)+istride*INCR_SHORT);
					nbremain++;
				}
			}
		}
		delayLine->writeIndex = p_val_s - (short*)baseAddress;
	}
	else
	{
		if(squelch)
		{
			for (j=nbIn;j>0;j--)
			{
				if (p_val==min_adr)
					p_val=max_adr-1;
				else
					p_val--;
				*p_val = 0;
				*(p_val+(delayLine->totalen))=*p_val_s;
				nbremain++;
			}
		}
		else
		{
			for (j=nbIn;j>0;j--)
			{
				if (p_val==min_adr)
					p_val=max_adr-1;
				else
					p_val--;
				*p_val=*(int*)void_inptr;
				*(p_val+delayLine->totalen)=*p_val;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				nbremain++;
			}
		}
		delayLine->writeIndex = p_val - baseAddress;
	} 
	delayLine->nbremain = nbremain;
}
void
resample_ML_conv_block_fixin_fixout 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
)
{
	if (delayLine->ARM_Sample16_config)
		resample_ML_conv_block_fixin_fixout_16_16(out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam);
	else if (delayLine->ARM_Coef16)
		resample_ML_conv_block_fixin_fixout_32_16(out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam);
	else
		resample_ML_conv_block_fixin_fixout_32_32(out,ostride,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,CoefRam);
}
#pragma inline
static void
resample_ML_conv_block_fixin_fixout_16_16 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	int nbremain = delayLine->nbremain;
	int  val=0 ;
#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	int vector_len,vector_loop_shift;
	short *p_coef_s;
	short *p_coef_s_lsb;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	void *void_outptr;
	short	 *max_adr_s = (short*)baseAddress+(short)(delayLine->totalen);
	short    *min_adr_s = (short*)baseAddress;
	short *p_val_s;
	int SATMAXCA9;
	void_outptr=out;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	p_val_s = (short*)baseAddress;
	p_val_s += (short)currIndex;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	if (offset>=M)
	{
		incr=(incr_offset_tab[offset]>>8)&0xff;
		offset=incr_offset_tab[offset]&0xff;
		for (i=incr;i>0;i--)
		{
			if (p_val_s==min_adr_s)
				p_val_s=max_adr_s-1;
			else
				p_val_s--;
			nbremain--;
		}
		nbin_to_consume-=incr;
	}
	{
		for (j=nbOutRequested;j>0;j--)
		{
#if defined(NEON) && !defined(M4)
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
			sum_vec_s_lsb=vdupq_n_s32(0);
			sum_vec_s=    vdupq_n_s32(0);
			if(!(delayLine->ARM_Coef16))
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));
				Acc+=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			for (i=0;i<(delayLine->len+delta)>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if (!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
				Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
				Acc<<=8;
				Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
				Acc = (Acc+0x400000)>>23;
				if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
				if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
				val=Acc;
			}
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;
			*(short*)void_outptr=(short)val;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_SHORT);
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				nbremain--;
			}
			nbin_to_consume-=incr;
		}
	} 
	delayLine->nbremain = nbremain;
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
}
#pragma inline
static void
resample_ML_conv_block_fixin_fixout_32_16 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	int nbremain = delayLine->nbremain;
	int  val=0 ;
#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	int vector_len,vector_loop_shift;
	short *p_coef_s;
	short *p_coef_s_lsb;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	void *void_outptr;
	short	 *max_adr_s = (short*)baseAddress+(short)(delayLine->totalen);
	short    *min_adr_s = (short*)baseAddress;
	short *p_val_s;
	int SATMAXCA9;
	void_outptr=out;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	p_val_s = (short*)baseAddress;
	p_val_s += (short)currIndex;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	if (offset>=M)
	{
		incr=(incr_offset_tab[offset]>>8)&0xff;
		offset=incr_offset_tab[offset]&0xff;
		for (i=incr;i>0;i--)
		{
			if (p_val_s==min_adr_s)
				p_val_s=max_adr_s-1;
			else
				p_val_s--;
			nbremain--;
		}
		nbin_to_consume-=incr;
	}
	{
		for (j=nbOutRequested;j>0;j--)
		{
#if defined(NEON) && !defined(M4)
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
			sum_vec_s_lsb=vdupq_n_s32(0);
			sum_vec_s=    vdupq_n_s32(0);
			if(!(delayLine->ARM_Coef16))
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));
				Acc+=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0);
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			for (i=0;i<(delayLine->len+delta)>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if (!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
				Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
				Acc<<=8;
				Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
				Acc = (Acc+0x400000)>>23;
				if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
				if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
				val=Acc;
			}
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;
			*(int*)void_outptr=val<<16;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_INT);
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				nbremain--;
			}
			nbin_to_consume-=incr;
		}
	}
	delayLine->nbremain = nbremain;
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
}
#pragma inline
static void
resample_ML_conv_block_fixin_fixout_32_32 
(
 int *out,
 unsigned int ostride,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	int f_index;
	unsigned int offset = delayLine->offset;
	int nbremain = delayLine->nbremain;
	int  val ;
#if defined(NEON) && !defined(M4)
	int32x4_t h_vec;
	int32x4_t x_vec;
	int64x2_t sum_vec;
#else
	long long sum0,sum1,sum2,sum3;
#endif
	long long Acc=0;
	int vector_len,vector_loop_shift;
	int *p_coef;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	int *p_val    ;
	void *void_outptr;
	int		 *max_adr = &baseAddress[delayLine->totalen];
	int      *min_adr = baseAddress;
	int SATMAXCA9;
	void_outptr=out;
	SATMAXCA9=SATMAX32;
	vector_len=(int)VECTOR_LEN_CA9_MUL32_64;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL32_64;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len and the last ones are set to 0
	p_val = baseAddress;
	p_val += currIndex;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	if (offset>=M)
	{
		incr=(incr_offset_tab[offset]>>8)&0xff;
		offset=incr_offset_tab[offset]&0xff;
		for (i=incr;i>0;i--)
		{
			if (p_val==min_adr)
				p_val=max_adr-1;
			else
				p_val--;
			nbremain--;
		}
		nbin_to_consume-=incr;
	}
	for (j=nbOutRequested;j>0;j--)
	{
		f_index = offset*(delta+delayLine->len);
#if defined(NEON) && !defined(M4)
		p_coef = &CoefRam[f_index];
		sum_vec=vdupq_n_s64(0);
		for (i=actualLen>>vector_loop_shift;i>0;i--)
		{
			h_vec=vld1q_s32(p_coef);
			x_vec=vld1q_s32(p_val);
			p_coef+=vector_len;
			p_val+=vector_len;
			sum_vec=vqdmlal_s32(sum_vec,vget_low_s32(x_vec), vget_low_s32(h_vec));
			sum_vec=vqdmlal_s32(sum_vec,vget_high_s32(x_vec), vget_high_s32(h_vec));
		}
		Acc=vget_lane_s64(vqadd_s64(vget_high_s64(sum_vec),vget_low_s64(sum_vec)),0);
		p_val-=actualLen;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#else	// ifdef NEON
		p_coef = &CoefRam[f_index];
		sum0=sum1=sum2=sum3=0;
		for (i=0;i<actualLen>>vector_loop_shift;i++)
		{
			sum0+=((long long)p_val[4*i+0]*(long long)p_coef[4*i+0])<<1;
			sum1+=((long long)p_val[4*i+1]*(long long)p_coef[4*i+1])<<1;
			sum2+=((long long)p_val[4*i+2]*(long long)p_coef[4*i+2])<<1;
			sum3+=((long long)p_val[4*i+3]*(long long)p_coef[4*i+3])<<1;
		}
		Acc=sum0+sum1+sum2+sum3;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#endif  // ifdef NEON
		incr=(incr_offset_tab[offset]>>8)&0xff;
		if (nbin_to_consume<incr)
			incr=nbin_to_consume;
		offset=incr_offset_tab[offset]&0xff;
		*(int*)void_outptr=val;
		void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_INT);
		for (i=incr;i>0;i--)
		{
			if (p_val==min_adr)
				p_val=max_adr-1;
			else
				p_val--;
			nbremain--;
		}
		nbin_to_consume-=incr;
	}
	delayLine->nbremain = nbremain;
	delayLine->currIndex = p_val - baseAddress;
	*nbOutAvail = nbOutRequested;
}

/********** CODE FOR DOWN SAMPLING ONLY *****************/
void
resample_ML_convout_block_fixin_fixout 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
)
{
{
	if (delayLine->ARM_Sample16_config)
		resample_ML_convout_block_fixin_fixout_16_16(in,istride,nbIn,resampleremainbuf,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,block_siz,CoefRam);
	else if (delayLine->ARM_Coef16)
		resample_ML_convout_block_fixin_fixout_32_16(in,istride,nbIn,resampleremainbuf,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,block_siz,CoefRam);
	else
		resample_ML_convout_block_fixin_fixout_32_32(in,istride,nbIn,resampleremainbuf,nbOutRequested,nbOutAvail,delayLine,filterInfo,loop_siz,block_siz,CoefRam);
}

}
#pragma inline
static void
resample_ML_convout_block_fixin_fixout_16_16 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	short *p_coef_s=0;
	short *p_coef_s_lsb;
	int  val ;
#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	int   *baseAddress_remain = resampleremainbuf->baseAddress;
	short *p_val_s;
	int *p_remain;
	short *max_adr_s = (short*)baseAddress+(short)(delayLine->len);
	short *min_adr_s = (short*)baseAddress;
	int		 *max_adr_remain = &baseAddress_remain[block_siz+1];
	int		*min_adr_remain  = &baseAddress_remain[0];
	int flush_mask;
	int SATMAXCA9,vector_len,vector_loop_shift;
	void *void_inptr;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	if (nbIn==0)
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of 4 and the last ones are set to 0
	p_val_s = (short*)baseAddress;
	p_val_s += (short)currIndex;
	p_remain = baseAddress_remain;
	p_remain += resampleremainbuf->writeptr;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	{
		if (offset>=M) 
		{
			incr=(incr_offset_tab[offset]>>8)&0xff;
			offset=incr_offset_tab[offset]&0xff;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=(*(short*)void_inptr)&flush_mask;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_SHORT);
				*p_val_s=(short)val;
				*(p_val_s+delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
#if defined(NEON) && !defined(M4)
			sum_vec_s=vdupq_n_s32(0);
			sum_vec_s_lsb=vdupq_n_s32(0);
			if (!(delayLine->ARM_Coef16))
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));		
				Acc+=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=(int)Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			for (i=0;i<actualLen>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if (!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
			}
			Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
			Acc<<=8;
			Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
			Acc = (Acc+0x400000)>>23;
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc; 
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;

			*p_remain=val;
			for (i=incr;i>0;i--)
			{
				/* put in delay line */
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=(*(short*)void_inptr)&flush_mask;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_SHORT);
				*p_val_s=(short)val;
				*(p_val_s+delayLine->len)=(short)val;
			}
			if (p_remain==max_adr_remain-1)
				p_remain=min_adr_remain;
			else
				p_remain++;
			nbin_to_consume-=incr;
		}
	} 
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
	resampleremainbuf->nbremain+=nbOutRequested;
	resampleremainbuf->writeptr=p_remain-baseAddress_remain;
}
#pragma inline
static void
resample_ML_convout_block_fixin_fixout_32_16 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	unsigned int offset = delayLine->offset;
	short *p_coef_s=0;
	short *p_coef_s_lsb;
	int  val ;
#if defined(NEON) && !defined(M4)
	int16x8_t h_vec_s;
	int16x8_t x_vec_s;
	int32x4_t sum_vec_s;
	int32x2_t temp_s;
	int32x4_t sum_vec_s_lsb;
#else
	int sum0_s,sum1_s,sum2_s,sum3_s,sum4_s,sum5_s,sum6_s,sum7_s;
	int sum0_s_lsb,sum1_s_lsb,sum2_s_lsb,sum3_s_lsb,sum4_s_lsb,sum5_s_lsb,sum6_s_lsb,sum7_s_lsb;
#endif
	long long Acc=0;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	int   *baseAddress_remain = resampleremainbuf->baseAddress;
	short *p_val_s;
	int *p_remain;
	short *max_adr_s = (short*)baseAddress+(short)(delayLine->len);
	short *min_adr_s = (short*)baseAddress;
	int		 *max_adr_remain = &baseAddress_remain[block_siz+1];
	int		*min_adr_remain  = &baseAddress_remain[0];
	int flush_mask;
	int SATMAXCA9,vector_len,vector_loop_shift;
	void *void_inptr;
	SATMAXCA9=SATMAX16;
#ifdef M4
	vector_len=(int)VECTOR_LEN_M4;
	vector_loop_shift=VECTOR_LOOP_M4;
#else
	vector_len=(int)VECTOR_LEN_CA9_MUL16_32;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL16_32;
#endif
	if (nbIn==0)
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of 4 and the last ones are set to 0
	p_val_s = (short*)baseAddress;
	p_val_s += (short)currIndex;
	p_remain = baseAddress_remain;
	p_remain += resampleremainbuf->writeptr;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	{
		if (offset>=M) 
		{
			incr=(incr_offset_tab[offset]>>8)&0xff;
			offset=incr_offset_tab[offset]&0xff;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=((*(int*)void_inptr)&flush_mask)>>16;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s=(short*)CoefRam+2*offset*(actualLen);
			p_coef_s_lsb=(short*)CoefRam+(2*offset+1)*(actualLen);
#if defined(NEON) && !defined(M4)
			sum_vec_s=vdupq_n_s32(0);
			sum_vec_s_lsb=vdupq_n_s32(0);
			if (!(delayLine->ARM_Coef16))
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					h_vec_s=vld1q_s16(p_coef_s_lsb);
					p_coef_s_lsb+=vector_len;
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s_lsb=vqdmlal_s16(sum_vec_s_lsb,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc<<=12;
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s_lsb),vget_low_s32(sum_vec_s_lsb));		
				Acc+=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x800000)>>24;
			}
			else
			{
				for (i=actualLen>>vector_loop_shift;i>0;i--)
				{
					h_vec_s=vld1q_s16(p_coef_s); 
					x_vec_s=vld1q_s16(p_val_s); 
					p_coef_s+=vector_len;
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_low_s16(x_vec_s), vget_low_s16(h_vec_s)); 
					sum_vec_s=vqdmlal_s16(sum_vec_s,vget_high_s16(x_vec_s), vget_high_s16(h_vec_s));
					p_val_s+=vector_len;
				}
				temp_s=vqadd_s32(vget_high_s32(sum_vec_s),vget_low_s32(sum_vec_s));
				Acc=(long long)vget_lane_s32(temp_s,0); 
				Acc+=(long long)vget_lane_s32(temp_s,1);
				Acc = (Acc+0x8000)>>16;
			}
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=(int)Acc;
			p_val_s-=actualLen;
#else	// ifdef NEON
			sum0_s=sum1_s=sum2_s=sum3_s=sum4_s=sum5_s=sum6_s=sum7_s=0;
			sum0_s_lsb=sum1_s_lsb=sum2_s_lsb=sum3_s_lsb=sum4_s_lsb=sum5_s_lsb=sum6_s_lsb=sum7_s_lsb=0;
			for (i=0;i<actualLen>>vector_loop_shift;i++)
			{
				sum0_s+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s[vector_len*i+0])<<0;
				sum1_s+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s[vector_len*i+1])<<0;
#ifndef M4
				sum2_s+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s[vector_len*i+2])<<0;
				sum3_s+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s[vector_len*i+3])<<0;
				sum4_s+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s[vector_len*i+4])<<0;
				sum5_s+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s[vector_len*i+5])<<0;
				sum6_s+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s[vector_len*i+6])<<0;
				sum7_s+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s[vector_len*i+7])<<0;
#endif
				if (!(delayLine->ARM_Coef16))
				{
					sum0_s_lsb+=((int)p_val_s[vector_len*i+0]*(int)p_coef_s_lsb[vector_len*i+0])<<0;
					sum1_s_lsb+=((int)p_val_s[vector_len*i+1]*(int)p_coef_s_lsb[vector_len*i+1])<<0;
#ifndef M4
					sum2_s_lsb+=((int)p_val_s[vector_len*i+2]*(int)p_coef_s_lsb[vector_len*i+2])<<0;
					sum3_s_lsb+=((int)p_val_s[vector_len*i+3]*(int)p_coef_s_lsb[vector_len*i+3])<<0;
					sum4_s_lsb+=((int)p_val_s[vector_len*i+4]*(int)p_coef_s_lsb[vector_len*i+4])<<0;
					sum5_s_lsb+=((int)p_val_s[vector_len*i+5]*(int)p_coef_s_lsb[vector_len*i+5])<<0;
					sum6_s_lsb+=((int)p_val_s[vector_len*i+6]*(int)p_coef_s_lsb[vector_len*i+6])<<0;
					sum7_s_lsb+=((int)p_val_s[vector_len*i+7]*(int)p_coef_s_lsb[vector_len*i+7])<<0;
#endif
				}
			}
			Acc=sum0_s+sum1_s+sum2_s+sum3_s+sum4_s+sum5_s+sum6_s+sum7_s;
			Acc<<=8;
			Acc+=sum0_s_lsb+sum1_s_lsb+sum2_s_lsb+sum3_s_lsb+sum4_s_lsb+sum5_s_lsb+sum6_s_lsb+sum7_s_lsb;
			Acc = (Acc+0x400000)>>23;
			if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
			if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
			val=Acc; 
#endif  // ifdef NEON
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;

			*p_remain=val<<16;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=((*(int*)void_inptr)&flush_mask)>>16;
				void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			if (p_remain==max_adr_remain-1)
				p_remain=min_adr_remain;
			else
				p_remain++;
			nbin_to_consume-=incr;
		}
	}
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
	*nbOutAvail = nbOutRequested;
	resampleremainbuf->nbremain+=nbOutRequested;
	resampleremainbuf->writeptr=p_remain-baseAddress_remain;
}
#pragma inline
static void
resample_ML_convout_block_fixin_fixout_32_32 
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int nbOutRequested, 
 unsigned int   *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
)
{
	unsigned int    M = filterInfo->M;
	const EXTERN int *incr_offset_tab=filterInfo->incr_offset_tab;
	unsigned int actualLen;
	int currIndex = delayLine->currIndex;
	int f_index;
	unsigned int offset = delayLine->offset;
	int  val ;
#if defined(NEON) && !defined(M4)
	int32x4_t h_vec;
	int32x4_t x_vec;
	int64x2_t sum_vec;
#else
	long long sum0,sum1,sum2,sum3;
#endif
	long long Acc=0;
	Float *p_coef;
	int j;
	int i,incr,delta;
	int nbin_to_consume;
	int   *baseAddress = &delayLine->baseAddress[0];
	int   *baseAddress_remain = resampleremainbuf->baseAddress;
	int *p_val    ;
	int *p_remain;
	int      *max_adr = &baseAddress[(delayLine->len)];
	int      *min_adr = baseAddress;
	int		 *max_adr_remain = &baseAddress_remain[block_siz+1];
	int		*min_adr_remain  = &baseAddress_remain[0];
	int flush_mask;
	int SATMAXCA9,vector_len,vector_loop_shift;
	void *void_inptr;
	SATMAXCA9=SATMAX32;
	vector_len=(int)VECTOR_LEN_CA9_MUL32_64;
	vector_loop_shift=VECTOR_LOOP_CA9_SHIFT_MUL32_64;
	if (nbIn==0)
		flush_mask=0;
	else
		flush_mask=-1;
	delta=(delayLine->len)&(vector_len-1);
	if (delta!=0)
		delta=vector_len-delta; //number of additional coefs needed to be a multiple of 4 and the last ones are set to 0
	p_val = baseAddress;
	p_val += currIndex;
	p_remain = baseAddress_remain;
	p_remain += resampleremainbuf->writeptr;
	actualLen = delayLine->len+delta;
	nbin_to_consume=loop_siz-nbOutRequested;
	void_inptr=in;
	if (offset>=M) 
	{
		incr=(incr_offset_tab[offset]>>8)&0xff;
		offset=incr_offset_tab[offset]&0xff;
		for (i=incr;i>0;i--)
		{
			if (p_val==min_adr)
				p_val=max_adr-1;
			else
				p_val--;
			val=(*(int*)void_inptr)&flush_mask;
			void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
			*p_val=val;
			*(p_val+delayLine->len)=val;
		}
		nbin_to_consume-=incr;
	}
	for (j=nbOutRequested;j>0;j--)
	{
		f_index = offset*(delta+delayLine->len);
		p_coef = &CoefRam[f_index];
#if defined(NEON) && !defined(M4)
		sum_vec=vdupq_n_s64(0);
		for (i=actualLen>>vector_loop_shift;i>0;i--)
		{
			h_vec=vld1q_s32(p_coef);
			x_vec=vld1q_s32(p_val);
			p_coef+=vector_len;
			p_val+=vector_len;
			sum_vec=vqdmlal_s32(sum_vec,vget_low_s32(x_vec), vget_low_s32(h_vec));
			sum_vec=vqdmlal_s32(sum_vec,vget_high_s32(x_vec), vget_high_s32(h_vec));
		}
		Acc=vget_lane_s64(vqadd_s64(vget_high_s64(sum_vec),vget_low_s64(sum_vec)),0);
		p_val-=actualLen;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#else	// ifdef NEON
		sum0=sum1=sum2=sum3=0;
		for (i=0;i<actualLen>>vector_loop_shift;i++)
		{
			sum0+=((long long)p_val[4*i+0]*(long long)p_coef[4*i+0])<<1;
			sum1+=((long long)p_val[4*i+1]*(long long)p_coef[4*i+1])<<1;
			sum2+=((long long)p_val[4*i+2]*(long long)p_coef[4*i+2])<<1;
			sum3+=((long long)p_val[4*i+3]*(long long)p_coef[4*i+3])<<1;
		}
		Acc=sum0+sum1+sum2+sum3;
		Acc = (Acc+0x800000)>>24;
		if (Acc>(SATMAXCA9)) Acc=(SATMAXCA9);
		if (Acc<-(SATMAXCA9)) Acc=-(SATMAXCA9);
		val=Acc;
#endif  // ifdef NEON
		incr=(incr_offset_tab[offset]>>8)&0xff;
		if (nbin_to_consume<incr)
			incr=nbin_to_consume;
		offset=incr_offset_tab[offset]&0xff;
		*p_remain=val;
		if (p_remain==max_adr_remain-1)
			p_remain=min_adr_remain;
		else
			p_remain++;
		for (i=incr;i>0;i--)
		{
			/* put in delay line */
			if (p_val==min_adr)
				p_val=max_adr-1;
			else
				p_val--;
			val=(*(int*)void_inptr)&flush_mask;
			void_inptr=(void*)(((char*)void_inptr)+istride*INCR_INT);
			*p_val=val;
			*(p_val+delayLine->len)=val;
		}
		nbin_to_consume-=incr;
	}
	delayLine->currIndex = p_val - baseAddress;
	*nbOutAvail = nbOutRequested;
	resampleremainbuf->nbremain+=nbOutRequested;
	resampleremainbuf->writeptr=p_remain-baseAddress_remain;
}


void
resample_ML_store_block_fixin_fixout(	ResampleRemainbuf *resampleremainbuf,
		int *out,
		unsigned int    ostride,
		unsigned int    nbOutRequested,
		unsigned int    block_siz,
		unsigned int	ARM_Sample16
		)
{
	unsigned int iter;
	int *p_val;
	int *baseAddress = resampleremainbuf->baseAddress;
	void *void_outptr;
	int      *max_adr = &baseAddress[block_siz+1];
	int      *min_adr = &baseAddress[0];
	p_val=baseAddress;
	p_val+=resampleremainbuf->readptr;
	void_outptr=out;
	if (ARM_Sample16)
	{
		for (iter=nbOutRequested;iter>0;iter--)
		{
			*(short*)void_outptr=*p_val;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_SHORT);
			if (p_val==max_adr-1)
				p_val=min_adr;
			else
				p_val++;
		}
	}
	else
	{
		for (iter=nbOutRequested;iter>0;iter--)
		{
			*(int*)void_outptr=*p_val;
			void_outptr=(void*)(((char*)void_outptr)+ostride*INCR_INT);
			if (p_val==max_adr-1)
				p_val=min_adr;
			else
				p_val++;
		}
	}
	resampleremainbuf->readptr=p_val-baseAddress;
	resampleremainbuf->nbremain-=nbOutRequested;
}
#endif // #if defined(ARM)||defined(NEON)
#endif //ifndef M4_LL_MSEC

