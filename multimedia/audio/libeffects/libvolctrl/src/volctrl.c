/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/libvolctrl.nmf>
#endif //#ifdef _NMF_MPC_
#include "volctrl_include.h"
#include "volctrl.h"
#ifndef __flexcc2__
#define restrict __restrict__
#endif
#ifndef MMDSP

extern float volctrl_get_dB_from_gain(float gain_lin);
int min(int a,int b)
{
    return a<b? a:b;
}

int max(int a, int b)
{
    return a>b? a:b;
}

void out_sat(float a, int *out)
{
    if (a > 2147483647.0)
    {
        *out = 0x7fffffff;
    }
    else if (a < -2147483648.0)
    {
        *out = 0x80000000;
    }
    else
        *out = (int)a;
}

void out_sat16(float a, short *out)
{
	int temp;
    if (a > 2147483647.0)
    {
        temp = 0x7fffffff;
    }
    else if (a < -2147483648.0)
    {
        temp = 0x80000000;
    }
    else
        temp = (int)a;
    if (temp < 0x7fff0000) temp += (1 << 15); //round to 16 bits

	*out=(short)(temp>>16);
}


float volctrl_smooth_one_gain(VOLCTRL_LOCAL_STRUCT_T * restrict volctrl, int gain_idx)
{
    double alpha = volctrl->alpha[gain_idx];
    double gain = volctrl->gain_smoothed[gain_idx];
	volctrl->current_duration[gain_idx]=(unsigned int)((100000.*(double)volctrl->nb_smooth_iter[gain_idx])/(double)volctrl->sample_freq);
//	printf("\n current_duration: %d gain_idx %d \n",volctrl->current_duration[gain_idx],gain_idx);
	volctrl->current_gaindB[gain_idx]=(int)(256.*(volctrl_get_dB_from_gain((float)(volctrl->gain_smoothed[gain_idx]))));
//	printf("\n smooth current_gaindB: %d \n",volctrl->current_gaindB[gain_idx]);
    if (volctrl->nb_smooth_iter[gain_idx]) {
		if (volctrl->terminate[gain_idx]==0)
		{
        	if (volctrl->db_ramp) gain *= alpha;
        	else gain += alpha;
		
        	volctrl->nb_smooth_iter[gain_idx] --;
		}
        if (!volctrl->nb_smooth_iter[gain_idx]) {
            float gain_asked = volctrl->gain[gain_idx];
            volctrl->gain_smoothed[gain_idx] = (double)gain_asked;
            volctrl->alpha[gain_idx] = (double)0;

            if (gain_asked <= VOLCTRL_MINIMUM_GAIN_LIN)
            {
                volctrl->gain_mask &= ~(1<<gain_idx);
                volctrl->gain_smoothed[gain_idx] = (double)0;
            }
            return gain_asked;
        }
        else
        {
            volctrl->gain_smoothed[gain_idx] = gain;
            return (float)gain;
        }
    }
    else
    {
        return (float)gain;
    }   
}

void process_one_channel_no_cross_gain( int *chan, int *out, int gain_idx,
		int nb_sample, int offset,
		VOLCTRL_LOCAL_STRUCT_T *ctx)
{
	int i;
	int end_smooth;
	float gain = (float)(ctx->gain_smoothed[gain_idx]);
	float tmp;

	//to gain MIPS check for gain of 0dB and return 
	if ((ctx->gain_cmd[1][gain_idx]!=0)||(ctx->gain_cmd[0][gain_idx]!=0))
	{
		if (offset == 2) nb_sample >>= 1;
		end_smooth = min(ctx->nb_smooth_iter[gain_idx], nb_sample);
		for (i=0; i<end_smooth;i++)
		{
			tmp = gain*((float)(*chan));
			out_sat(tmp, out);

			chan+=offset;
			out+=offset;

			gain = volctrl_smooth_one_gain(ctx, gain_idx);
		}
		for (;i<nb_sample;i++)
		{
			tmp = gain*((float)(*chan));
			out_sat(tmp, out);

			chan += offset;
			out += offset;
		}
	} 
        else if (out != chan) {

	    if (offset == 2) nb_sample >>= 1;

            for (i=0; i < nb_sample; i++) {
                *out    = *chan;
                chan    += offset;
                out     += offset;
            }
        }
}

void process_one_of_nch_no_cross_gain( int *chan, int *out, int chno,
                                        int nb_sample_per_ch,VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i;
    int end_smooth;
	int gain_idx=START_IDX_MCH+chno;
    float gain = (float)(ctx->gain_smoothed[gain_idx]);
    float tmp;

    end_smooth = min(ctx->nb_smooth_iter[gain_idx], nb_sample_per_ch);

    for (i=0; i<end_smooth;i++)
    {
        tmp = gain*((float)(*(chan+chno)));
        out_sat(tmp, out+chno);

        chan+=ctx->input_mode;
        out+=ctx->input_mode;

        gain = volctrl_smooth_one_gain(ctx, gain_idx);
    }

    for (;i<nb_sample_per_ch;i++)
    {
        tmp = gain*((float)(*(chan+chno)));
        out_sat(tmp, out+chno);

        chan += ctx->input_mode;
        out += ctx->input_mode;
    }
}

void process_one_channel_with_cross_gain(
        int *direct_chan, int *cross_chan,
        int *out, int strideout, int nb_sample,
        int direct_gain_idx, int cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i;
    int end_smooth;
    float direct_gain = (float)(ctx->gain_smoothed[direct_gain_idx]);
    float cross_gain  = (float)(ctx->gain_smoothed[cross_gain_idx]);
    float tmp;

    nb_sample >>= 1;

    end_smooth = max(ctx->nb_smooth_iter[direct_gain_idx] ,ctx->nb_smooth_iter[cross_gain_idx]);
    end_smooth = min(end_smooth, nb_sample);

    for (i=0;i<end_smooth;i++)
    {
        tmp = direct_gain*((float)*direct_chan) + cross_gain*((float)*cross_chan);
        out_sat(tmp, out);

        direct_chan+=2;
        cross_chan+=2;
        out+=strideout;

        direct_gain = volctrl_smooth_one_gain(ctx,direct_gain_idx);
        cross_gain  = volctrl_smooth_one_gain(ctx,cross_gain_idx);
    }

    for (;i<nb_sample;i++)
    {
        tmp = direct_gain*((float)*direct_chan) + cross_gain*((float)*cross_chan);
        out_sat(tmp, out);

        direct_chan+=2;
        cross_chan+=2;
        out+=strideout;
    }
}


void process_two_channel_with_cross_gain(   int *chan, int *out, int nb_sample,
                                            VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i, end_smooth;
    float right, left;
    float gll = (float)ctx->gain_smoothed[GLL];
    float glr = (float)ctx->gain_smoothed[GLR];
    float grl = (float)ctx->gain_smoothed[GRL];
    float grr = (float)ctx->gain_smoothed[GRR];
    float tmp;

    nb_sample >>= 1;

    end_smooth = max(ctx->nb_smooth_iter[GLL],ctx->nb_smooth_iter[GLR]);
    end_smooth = max(end_smooth ,ctx->nb_smooth_iter[GRL]);
    end_smooth = max(end_smooth ,ctx->nb_smooth_iter[GRR]);
    end_smooth = min(end_smooth , nb_sample);

    for(i=0;i<end_smooth;i++)
    {
        left = (float)(*chan++);
        right = (float)(*chan++);

        tmp = left * gll + right * grl;
        out_sat(tmp, out++);

        tmp = left * glr + right * grr;
        out_sat(tmp, out++);

        gll  = volctrl_smooth_one_gain(ctx,GLL);
        grl  = volctrl_smooth_one_gain(ctx,GRL);
        glr  = volctrl_smooth_one_gain(ctx,GLR);
        grr  = volctrl_smooth_one_gain(ctx,GRR);
    }

    for(;i<nb_sample;i++)
    {
        left = (float)(*chan++);
        right = (float)(*chan++);

        tmp = left * gll + right * grl;
        out_sat(tmp, out++);

        tmp = left * glr + right * grr;
        out_sat(tmp, out++);
    }
}
//------------------------------- functions needed for sample16---------------
void process_one_channel_no_cross_gain_sample16( short *chan, short *out, int gain_idx,
		int nb_sample, int offset,
		VOLCTRL_LOCAL_STRUCT_T *ctx)
{
	int i;
	int end_smooth;
	float gain = (float)(ctx->gain_smoothed[gain_idx]);
	float tmp;

	//to gain MIPS check for gain of 0dB and return 
	if ((ctx->gain_cmd[1][gain_idx]!=0)||(ctx->gain_cmd[0][gain_idx]!=0))
	{
		if (offset == 2) nb_sample >>= 1;
		end_smooth = min(ctx->nb_smooth_iter[gain_idx], nb_sample);
		for (i=0; i<end_smooth;i++)
		{
			tmp = gain*((float)(((int)(*chan))<<16));
			out_sat16(tmp, out);

			chan+=offset;
			out+=offset;

			gain = volctrl_smooth_one_gain(ctx, gain_idx);
		}
		for (;i<nb_sample;i++)
		{
			tmp = gain*((float)(((int)(*chan))<<16));
			out_sat16(tmp, out);

			chan += offset;
			out += offset;
		}
	} 
        else if (out != chan) {

	    if (offset == 2) nb_sample >>= 1;

            for (i=0; i < nb_sample; i++) {
                *out    = *chan;
                chan    += offset;
                out     += offset;
            }
        }
}

void process_one_of_nch_no_cross_gain_sample16( short *chan, short *out, int chno,
                                        int nb_sample_per_ch,VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i;
    int end_smooth;
	int gain_idx=START_IDX_MCH+chno;
    float gain = (float)(ctx->gain_smoothed[gain_idx]);
    float tmp;

    end_smooth = min(ctx->nb_smooth_iter[gain_idx], nb_sample_per_ch);

    for (i=0; i<end_smooth;i++)
    {
        tmp = gain*((float)((((int)(*(chan+chno)))<<16)));
        out_sat16(tmp, out+chno);

        chan+=ctx->input_mode;
        out+=ctx->input_mode;

        gain = volctrl_smooth_one_gain(ctx, gain_idx);
    }

    for (;i<nb_sample_per_ch;i++)
    {
        tmp = gain*((float)((((int)(*(chan+chno)))<<16)));
        out_sat16(tmp, out+chno);

        chan += ctx->input_mode;
        out += ctx->input_mode;
    }
}

void process_one_channel_with_cross_gain_sample16(
        short *direct_chan, short *cross_chan,
        short *out, int strideout, int nb_sample,
        int direct_gain_idx, int cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i;
    int end_smooth;
    float direct_gain = (float)(ctx->gain_smoothed[direct_gain_idx]);
    float cross_gain  = (float)(ctx->gain_smoothed[cross_gain_idx]);
    float tmp;

    nb_sample >>= 1;

    end_smooth = max(ctx->nb_smooth_iter[direct_gain_idx] ,ctx->nb_smooth_iter[cross_gain_idx]);
    end_smooth = min(end_smooth, nb_sample);

    for (i=0;i<end_smooth;i++)
    {
        tmp = direct_gain*((float)(((int)(*direct_chan))<<16)) + cross_gain*((float)(((int)(*cross_chan))<<16));
        out_sat16(tmp, out);

        direct_chan+=2;
        cross_chan+=2;
        out+=strideout;

        direct_gain = volctrl_smooth_one_gain(ctx,direct_gain_idx);
        cross_gain  = volctrl_smooth_one_gain(ctx,cross_gain_idx);
    }

    for (;i<nb_sample;i++)
    {
        tmp = direct_gain*((float)(((int)(*direct_chan))<<16)) + cross_gain*((float)(((int)(*cross_chan))<<16));
        out_sat16(tmp, out);

        direct_chan+=2;
        cross_chan+=2;
        out+=strideout;
    }
}


void process_two_channel_with_cross_gain_sample16(   short *chan, short *out, int nb_sample,
                                            VOLCTRL_LOCAL_STRUCT_T *ctx)
{
    int i, end_smooth;
    float right, left;
    float gll = (float)ctx->gain_smoothed[GLL];
    float glr = (float)ctx->gain_smoothed[GLR];
    float grl = (float)ctx->gain_smoothed[GRL];
    float grr = (float)ctx->gain_smoothed[GRR];
    float tmp;

    nb_sample >>= 1;

    end_smooth = max(ctx->nb_smooth_iter[GLL],ctx->nb_smooth_iter[GLR]);
    end_smooth = max(end_smooth ,ctx->nb_smooth_iter[GRL]);
    end_smooth = max(end_smooth ,ctx->nb_smooth_iter[GRR]);
    end_smooth = min(end_smooth , nb_sample);

    for(i=0;i<end_smooth;i++)
    {
        left = (float)(((int)(*chan++))<<16);
        right =(float)(((int)(*chan++))<<16);

        tmp = left * gll + right * grl;
        out_sat16(tmp, out++);

        tmp = left * glr + right * grr;
        out_sat16(tmp, out++);

        gll  = volctrl_smooth_one_gain(ctx,GLL);
        grl  = volctrl_smooth_one_gain(ctx,GRL);
        glr  = volctrl_smooth_one_gain(ctx,GLR);
        grr  = volctrl_smooth_one_gain(ctx,GRR);
    }

    for(;i<nb_sample;i++)
    {
        left = (float)(((int)(*chan++))<<16);
        right =(float)(((int)(*chan++))<<16);

        tmp = left * gll + right * grl;
        out_sat16(tmp, out++);

        tmp = left * glr + right * grr;
        out_sat16(tmp, out++);
    }
}
#else
#pragma inline 
MMshort volctrl_smooth_one_gain(VOLCTRL_LOCAL_STRUCT_T * restrict volctrl, int gain_idx)
{
    MMshort alpha       = volctrl->alpha[gain_idx];
    MMshort alpha_shift = volctrl->alpha_shift[gain_idx];
    MMlong  gain        = volctrl->gain_smoothed[gain_idx];
    MMlong  delta;
	//duration=(nb_iter/fs)*100000  =  nb_iter*(6250/fs)*16  = nb_iter*sample_freqinv*16
	volctrl->current_duration[gain_idx]=wextract_h(wL_msl(wL_imuluu(volctrl->nb_smooth_iter[gain_idx],volctrl->sample_freqinv),5));
//	printf("\n current_duration: %d \n",volctrl->current_duration[gain_idx]);
	volctrl->current_gaindB[gain_idx]=get_dB_from_gain(wextract_h(volctrl->gain_smoothed[gain_idx]), GUARD_BIT);
//	printf("\n smooth current_gaindB: %d \n",volctrl->current_gaindB[gain_idx]);
    if(volctrl->nb_smooth_iter[gain_idx])
    {
        if (volctrl->db_ramp) {
            delta  = wL_add(wL_imul(alpha,
                                    wextract_h(gain)),
                                    wextract_h(wL_imulsu(alpha,
                                    wextract_l(gain)))); // do not use fmul because need unsigned mult (imulsu)
            delta = wL_msl(delta, alpha_shift); // alpha_shift has been incremented by 1 in volctrl_set_one_gain  because we did not use fmul
        }
        else {
            delta = wL_msl(alpha, alpha_shift+23);
        }
		if (volctrl->terminate[gain_idx]==0)
		{
        	gain  = wL_add(gain,delta);
        
        	volctrl->nb_smooth_iter[gain_idx] --;
		}
        else
        {
            volctrl->nb_smooth_iter[gain_idx] =0;
            volctrl->gain[gain_idx] = gain;
        }
        if(volctrl->nb_smooth_iter[gain_idx] == 0)
        {
            MMlong gain_asked = volctrl->gain[gain_idx];
            volctrl->gain_smoothed[gain_idx] = gain_asked;
            volctrl->alpha[gain_idx] = 0;
            
            /* add test to disactivate cross gain if not necessary */
            if((gain_idx == GLR) && (gain_asked == 0) && (volctrl->gain_mask & GRR_MASK))
            {
                volctrl->gain_mask &= ~GLR_MASK;
            }
            else if((gain_idx == GRL) && (gain_asked == 0) && (volctrl->gain_mask & GLL_MASK))
            {
                volctrl->gain_mask &= ~GRL_MASK;
            }
            
            return  wextract_h(gain_asked);    
        }
        else
        {
            volctrl->gain_smoothed[gain_idx] = gain;
            return wextract_h(gain);
        }
    }
    else
    {
        return  wextract_h(gain);
    }
}



void process_one_channel_no_cross_gain( MMshort *chan, MMshort *out, MMshort gain_idx,
		MMshort nb_sample, MMshort offset,
		VOLCTRL_LOCAL_STRUCT_T *ctx )
{
	MMshort i;
	MMshort end_smooth;
	MMshort gain   = wextract_h(ctx->gain_smoothed[gain_idx]);
	MMlong  tmp;
	//to gain MIPS check for gain of 0dB and return 
	if ((ctx->gain_cmd[1][gain_idx]!=0)||(ctx->gain_cmd[0][gain_idx]!=0))
        {

            if(offset == 2) {
                nb_sample >>= 1;
            }

            end_smooth = wmin(ctx->nb_smooth_iter[gain_idx], nb_sample);

            if (end_smooth < 0) {
                end_smooth = 0;
            }

            if (end_smooth > 1023) {
                end_smooth = 1023;
            }

#pragma loop force_hwloop
            for(i=0;i<end_smooth;i++)
            {
                tmp = wL_fmul(*chan,gain);
                *out = wextract_h(wL_msl(tmp,GUARD_BIT));
                chan+=offset;
                out+=offset;

                gain = volctrl_smooth_one_gain(ctx,gain_idx);
            }

#pragma loop force_hwloop
            for(;i<nb_sample;i++)
            {
                tmp = wL_fmul(*chan,gain);
                *out = wextract_h(wL_msl(tmp,GUARD_BIT));
                chan+=offset;
                out+=offset;
            }
        }
        else if (out != chan) {

            if(offset == 2) {
                nb_sample >>= 1;
            }

#pragma loop force_hwloop
            for (i = 0; i < nb_sample; i++) {
                *out    = *chan;
                chan    += offset;
                out     += offset;
            }
        }
            
}



void process_one_channel_with_cross_gain( 
        MMshort *direct_chan, MMshort *cross_chan,
        MMshort *out, MMshort strideout, MMshort nb_sample,
        MMshort direct_gain_idx, MMshort cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx )
{
    MMshort i;
    MMshort end_smooth;
    MMshort direct_gain   = wextract_h(ctx->gain_smoothed[direct_gain_idx]);
    MMshort cross_gain    = wextract_h(ctx->gain_smoothed[cross_gain_idx]);
    MMlong  tmp1,tmp2;

    nb_sample >>= 1;

    end_smooth = wmax(ctx->nb_smooth_iter[direct_gain_idx] ,ctx->nb_smooth_iter[cross_gain_idx]);
    end_smooth = wmin(end_smooth, nb_sample);

#pragma loop force_hwloop
    for(i=0;i<end_smooth;i++)
    {
        tmp1 = wL_fmul(*direct_chan, direct_gain);
        direct_chan+=2;
        
        tmp2 = wL_fmul(*cross_chan, cross_gain);
        cross_chan+=2;
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out+=strideout;
        
        direct_gain = volctrl_smooth_one_gain(ctx,direct_gain_idx);
        cross_gain  = volctrl_smooth_one_gain(ctx,cross_gain_idx);
    }

#pragma loop force_hwloop
    for(;i<nb_sample;i++)
    {
        tmp1 = wL_fmul(*direct_chan, direct_gain);
        direct_chan+=2;
        
        tmp2 = wL_fmul(*cross_chan, cross_gain);
        cross_chan+=2;
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out+=strideout;
    }
}

void process_two_channel_with_cross_gain( MMshort *chan, MMshort *out, MMshort nb_sample,
										  VOLCTRL_LOCAL_STRUCT_T *ctx )
{
    MMshort i,right,left;
    MMshort end_smooth;
    MMshort gll = wextract_h(ctx->gain_smoothed[GLL]);
    MMshort glr = wextract_h(ctx->gain_smoothed[GLR]);
    MMshort grl = wextract_h(ctx->gain_smoothed[GRL]);
    MMshort grr = wextract_h(ctx->gain_smoothed[GRR]);
    MMlong  tmp1,tmp2;
    nb_sample >>= 1;
    
    end_smooth = wmax(ctx->nb_smooth_iter[GLL],ctx->nb_smooth_iter[GLR]);
    end_smooth = wmax(end_smooth ,ctx->nb_smooth_iter[GRL]);
    end_smooth = wmax(end_smooth ,ctx->nb_smooth_iter[GRR]);
    end_smooth = wmin(end_smooth , nb_sample);

#pragma loop force_hwloop
    for(i=0;i<end_smooth;i++)
    {
        left  = *chan++;
        right = *chan++;
        
        tmp1 = wL_fmul(left,  gll);
        tmp2 = wL_fmul(right, grl);
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out++;
        
        tmp1 = wL_fmul(left,  glr);
        tmp2 = wL_fmul(right, grr);
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out++;

        gll  = volctrl_smooth_one_gain(ctx,GLL);
        grl  = volctrl_smooth_one_gain(ctx,GRL);
        glr  = volctrl_smooth_one_gain(ctx,GLR);
        grr  = volctrl_smooth_one_gain(ctx,GRR);
    }

#pragma loop force_hwloop
    for(;i<nb_sample;i++)
    {
        left  = *chan++;
        right = *chan++;
        
        tmp1 = wL_fmul(left,  gll);
        tmp2 = wL_fmul(right, grl);
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out++;
        
        tmp1 = wL_fmul(left,  glr);
        tmp2 = wL_fmul(right, grr);
        
        tmp1 = wL_addsat(tmp1,tmp2);
        *out = wextract_h(wL_msl(tmp1,GUARD_BIT));
        out++;
    }
}
#endif
