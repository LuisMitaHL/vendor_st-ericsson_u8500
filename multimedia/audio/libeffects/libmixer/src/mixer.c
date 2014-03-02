/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/libmixer.nmf>
#endif
#include "mixer_include.h"
#ifdef MMDSP

void mixer_stereo(MMshort **input_list, int nb_input, MMshort *output,
		int nb_sample, MIXER_LOCAL_STRUCT_T *ctx)
{
	MMshort i,j;
	MMlong accu_left, accu_right;
	MMshort env_l,env_r;
	MMshort *out_ptr = output;
	MMshort gain;

	for(i = 0 ; i < nb_sample; i+=2)
	{
		accu_left  = input_list[0][i];
		accu_right = input_list[0][i+1];
		for(j = 1 ; j < nb_input ; j++)
		{
			accu_left  += input_list[j][i];
			accu_right += input_list[j][i+1];
		}

		/* envelop detection */
		env_l = mixer_envelop_detection(accu_left,  ctx, LEFT_CHANNEL );
		env_r = mixer_envelop_detection(accu_right, ctx, RIGHT_CHANNEL);

		if(wmax(env_r,env_l) > ctx->threshold)
		{
			/* attenuation computation */
			if(env_l > env_r)
				gain = mixer_compute_gain(ctx,LEFT_CHANNEL);
			else
				gain = mixer_compute_gain(ctx,RIGHT_CHANNEL);

			/* apply gain */
			accu_left  = mixer_short_long_mult(gain,accu_left);
			accu_right = mixer_short_long_mult(gain,accu_right);

			/* write output buffer */
			*out_ptr++ = wextract_h(wL_msl(accu_left,24));
			*out_ptr++ = wextract_h(wL_msl(accu_right,24));
		}
		else /* no attenaution */
		{
			*out_ptr++ = wextract_h(wL_msl(accu_left,24));
			*out_ptr++ = wextract_h(wL_msl(accu_right,24));
		}
	}
}


void mixer_mono(MMshort **input_list, int nb_input, MMshort *output,
		int nb_sample, int offset, MIXER_LOCAL_STRUCT_T *ctx)
{
	MMshort i,j;
	MMlong accu;
	MMshort env;
	MMshort *out_ptr = output;
	MMshort gain;

	for(i = 0 ; i < nb_sample; i+=offset)
	{
		accu = input_list[0][i];
		for(j = 1 ; j < nb_input ; j++)
		{
			accu += input_list[j][i];
		}

		/* envelop detection */
		env = mixer_envelop_detection(accu, ctx, LEFT_CHANNEL );

		if(env > ctx->threshold)
		{
			/* attenuation computation */
			gain = mixer_compute_gain(ctx,LEFT_CHANNEL);

			/* apply gain */
			accu  = mixer_short_long_mult(gain,accu);

			/* write output buffer */
			*out_ptr = wextract_h(wL_msl(accu,24));
			out_ptr += offset;
		}
		else /* no attenaution */
		{
			*out_ptr = wextract_h(wL_msl(accu,24));
			out_ptr += offset;
		}
	}
}


#ifndef _NMF_MPC_
void mixer_nocompression(MMshort **input_list, int nb_input, MMshort *output,
		int offset, int nb_sample)
{
	MMshort i,j;
	MMshort sample;

	for(i = 0 ; i < nb_sample; i+=offset)
	{
		sample = input_list[0][i];
		for(j = 1 ; j < nb_input; j++)
		{
			sample = waddsat(sample, input_list[j][i]);
		}
		output[i] = sample;
	}

}

void mixer_stereo_to_dual_mono(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i,tmp1,tmp2,gain;

	if(normalize)
	{
		gain = invSQROOT2;
	}
	else
	{
		gain = FORMAT_FLOAT(0.5,MAXVAL);
	}

	for(i=0;i<(size>>1);i++)
	{
		tmp1 = wfmulr(*ptr, gain);
		tmp2 = wfmulr(*(ptr+1), gain);
		*ptr = waddsat(tmp1,tmp2);
		ptr += 2;
	}
}



void mixer_dual_mono_to_stereo(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i,tmp;

	if(normalize)
	{
		for(i=0;i<(size>>1);i++)
		{
			tmp = *ptr;
			tmp = wfmulr(tmp,invSQROOT2);
			*ptr++ = tmp;
			*ptr++ = tmp;
		}
	}
	else
	{
		for(i=0;i<(size>>1);i++)
		{
			tmp = *ptr++;
			*ptr++ = tmp;
		}
	}
}
#endif //ifndef _NMF_MPC_



#else //ifdef MMDSP

void mixer_stereo(MMshort **input_list, int nb_input, MMshort *output,
		int nb_sample, MIXER_LOCAL_STRUCT_T *ctx)
{
	MMshort i,j;
	MMlong  accu_left, accu_right, gain,env_l,env_r,maxval;
	MMshort *out_ptr = output;
	MMshort Nb_sample=nb_sample&DATA_BLOCK_SIZE_MASK;

	//-------------- modif for Sample16 ----------
	short **in_list_s, *out_ptr_s;
	in_list_s=(short **)input_list;
	out_ptr_s=(short *)output;
	//--------------------------------------------

	for(i = 0 ; i < Nb_sample; i+=2)
	{
		//-------------- modif for Sample16 -------------
		if ((ctx->data_block_size)&SAMPLE16)
		{
			accu_left  = (MMlong)((int)(in_list_s[0][i]) << 16);
			accu_right = (MMlong)((int)(in_list_s[0][i+1]) <<16) ;
			for(j = 1 ; j < nb_input ; j++)
			{
				accu_left  += (MMlong)((int)(in_list_s[j][i]) <<16);
				accu_right += (MMlong)((int)(in_list_s[j][i+1]) <<16);
			}
		}
		else
		//------------------------------------------------
		{
			accu_left  = input_list[0][i];
			accu_right = input_list[0][i+1];
			for(j = 1 ; j < nb_input ; j++)
			{
				accu_left  += input_list[j][i];
				accu_right += input_list[j][i+1];
			}
		}
		// envelop detection
		env_l = mixer_envelop_detection(accu_left,  ctx, LEFT_CHANNEL );
		env_r = mixer_envelop_detection(accu_right, ctx, RIGHT_CHANNEL);
		maxval=env_r;
		if (env_l>maxval) maxval=env_l;
		if (maxval > ctx->threshold)
		{
			// attenuation computation
			if(env_l > env_r)
			{
				gain = mixer_compute_gain(ctx,LEFT_CHANNEL);
			}
			else
			{
				gain = mixer_compute_gain(ctx,RIGHT_CHANNEL);
			}
			if (gain>1.0) gain=1.0;

			accu_left=(MMlong)gain*accu_left;
			accu_right=(MMlong)gain*accu_right;
		}
		if (accu_left>MAXVAL32) accu_left=MAXVAL32;
		if (accu_left<MINVAL32) accu_left=MINVAL32;
		if (accu_right>MAXVAL32) accu_right=MAXVAL32;
		if (accu_right<MINVAL32) accu_right=MINVAL32;
		//----------------- modif for Sample16 ----------
		if ((ctx->data_block_size)&SAMPLE16)
		{
			*out_ptr_s++ = (int)accu_left >> 16;
			*out_ptr_s++ = (int)accu_right >> 16;
		}
		else
		//---------------------------------------------
		{
			*out_ptr++ = (MMshort)accu_left;
			*out_ptr++ = (MMshort)accu_right;
		}
	}
}

void mixer_nch(MMshort **input_list, int nb_input, MMshort *output,
		int nb_sample_per_channel, int nch,int nocompression, MIXER_LOCAL_STRUCT_T *ctx)
{
	MMshort i,j,k,maxindx,nb_sample;
	MMlong  accu[MAXCH], gain,env,maxval;
	MMshort *out_ptr = output;
	MMshort Nb_sample_per_channel=nb_sample_per_channel&DATA_BLOCK_SIZE_MASK;
	short **in_list_s, *out_ptr_s;

	nb_sample=Nb_sample_per_channel*nch;
	//-------------- modif for Sample16 ----------
	in_list_s=(short **)input_list;
	out_ptr_s=(short *)output;
	//--------------------------------------------
	for(i = 0 ; i < nb_sample; i+=nch)
	{
		maxval=0;
		maxindx=0;
		for (k=0;k<nch;k++)
		{
			accu[k]=0;
			//-------------- modif for Sample16 ----------
			if ((ctx->data_block_size)&SAMPLE16)
			{
				for(j = 0 ; j < nb_input ; j++)
				{
					accu[k]  += (MMlong)((int)(in_list_s[j][i+k]) <<16);
				}
			}
			else
			//---------------------------------------------
			{
				for(j = 0 ; j < nb_input ; j++)
				{
					accu[k]  += input_list[j][i+k];
				}
			}
			// envelop detection
			if (nocompression==0) {
				env = mixer_envelop_detection(accu[k],ctx,k);
				if (env>maxval)
				{
					maxval=env;
					maxindx=k;
				}
			}
		}
		gain=1.0;
		if ((maxval > ctx->threshold)&&(nocompression==0))
		{
			// attenuation computation
			gain = mixer_compute_gain(ctx,maxindx);
			if (gain>1.0) gain=1.0;
		}
		for (k=0;k<nch;k++)
		{
			accu[k]=(MMlong)gain*accu[k];
			if (accu[k]>MAXVAL32) accu[k]=MAXVAL32;
			if (accu[k]<MINVAL32) accu[k]=MINVAL32;
			//-------------- modif for Sample16 ----------
			if ((ctx->data_block_size)&SAMPLE16)
				*out_ptr_s++ = (int)accu[k] >> 16;
			else
			//--------------------------------------------
				*out_ptr++ = (MMshort)accu[k];
		}
	}
}

void mixer_mono(MMshort **input_list, int nb_input, MMshort *output,
		int nb_sample, int offset, MIXER_LOCAL_STRUCT_T *ctx)
{
	MMshort i,j;
	MMlong accu;
	MMlong env;
	MMshort *out_ptr = output;
	MMlong gain;
	MMshort Nb_sample=nb_sample&DATA_BLOCK_SIZE_MASK;
	//-------------- modif for Sample16 ----------
	short **in_list_s, *out_ptr_s;
	in_list_s=(short **)input_list;
	out_ptr_s=(short *)output;
	//--------------------------------------------
	for(i = 0 ; i < Nb_sample; i+=offset)
	{
		//-------------- modif for Sample16 ----------
		if ((ctx->data_block_size)&SAMPLE16)
		{
			accu  = (MMlong)((int)(in_list_s[0][i])<<16);
			for(j = 1 ; j < nb_input ; j++)
			{
				accu  += (MMlong)((int)(in_list_s[j][i])<<16);
			}
		}
		else
		//------------------------------------------
		{
			accu = input_list[0][i];
			for(j = 1 ; j < nb_input ; j++)
			{
				accu += input_list[j][i];
			}
		}
		//envelop detection
		env = mixer_envelop_detection(accu, ctx, LEFT_CHANNEL );

		if(env > ctx->threshold)
		{
			// attenuation computation
			gain = mixer_compute_gain(ctx,LEFT_CHANNEL);

			// apply gain
			if (gain>1.0) gain=1.0;
			accu  *= gain;
		}
		if (accu>MAXVAL32) accu=MAXVAL32;
		if (accu<MINVAL32) accu=MINVAL32;
		//-------------- modif for Sample16 ----------
		if ((ctx->data_block_size)&SAMPLE16)
		{
			*out_ptr_s= (int)accu >> 16;
			out_ptr_s += offset;
		}
		else
		//--------------------------------------------
		{
			*out_ptr = (MMshort)accu;
			out_ptr += offset;
		}
	}
}


void mixer_nocompression(MMshort **input_list, int nb_input, MMshort *output,
		int offset, int nb_sample)
{
	MMshort i,j;
	MMlong sample;
	MMshort Nb_sample=nb_sample&DATA_BLOCK_SIZE_MASK;
	for(i = 0 ; i < Nb_sample; i+=offset)
	{
		sample = (input_list[0][i]);
		for(j = 1 ; j < nb_input; j++)
		{
			sample = sample+(input_list[j][i]);
		}
		if (sample>MAXVAL32) sample=MAXVAL32;
		if (sample<MINVAL32) sample=MINVAL32;
		output[i] = (MMshort)sample;
	}
}


void mixer_nocompression_sample16(MMshort **input_list, int nb_input, MMshort *output,
		int offset, int nb_sample)
{
	MMshort i,j;
	MMlong sample;
	short **in_list_s, *out_ptr_s;
	MMshort Nb_sample=nb_sample&DATA_BLOCK_SIZE_MASK;
	in_list_s=(short **)input_list;
	out_ptr_s=(short *)output;
	for(i = 0 ; i < Nb_sample; i+=offset)
	{
		sample = (MMlong)((int)(in_list_s[0][i]) << 16);
		for(j = 1 ; j < nb_input; j++)
		{
			sample = sample+(MMlong)((int)(in_list_s[j][i]) << 16);
		}
		if (sample>MAXVAL32) sample=MAXVAL32;
		if (sample<MINVAL32) sample=MINVAL32;
		*out_ptr_s = (int)sample >> 16;
		out_ptr_s+=offset;
	}
}


void mixer_stereo_to_dual_mono(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i;
	MMlong gain,resf;
	MMshort Size=size&DATA_BLOCK_SIZE_MASK;

	if(normalize)
		gain = invSQROOT2;
	else
		gain = 0.5;
	for(i=0;i<(Size>>1);i++)
	{
		resf=((MMlong)(*ptr))*gain;
		resf+=((MMlong)(*(ptr+1)))*gain;
		if (resf>MAXVAL32) resf=MAXVAL32;
		if (resf<MINVAL32) resf=MINVAL32;
		*ptr=(MMshort)resf;
		ptr+=2;
	}
}

void mixer_stereo_to_dual_mono_sample16(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i;
	MMlong gain,resf;
	MMlong inval;
	short *ptr_s;
	MMshort Size=size&DATA_BLOCK_SIZE_MASK;
	ptr_s=(short *)ptr;
	if(normalize)
		gain = invSQROOT2;
	else
		gain = 0.5;

	for(i=0;i<(Size>>1);i++)
	{
		inval=(MMlong)(((int)(*ptr_s))<<16);
		ptr_s++;
		resf=inval*gain;

		inval=(MMlong)(((int)(*ptr_s))<<16);
		ptr_s--;
		resf+=inval*gain;

		if (resf>MAXVAL32) resf=MAXVAL32;
		if (resf<MINVAL32) resf=MINVAL32;
		*ptr_s=((int)resf)>>16;
		ptr_s+=2;
	}
}


void mixer_dual_mono_to_stereo(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i;
	MMlong resf;
	MMshort Size=size&DATA_BLOCK_SIZE_MASK;
	if(normalize)
	{
		for(i=0;i<(Size>>1);i++)
		{
			resf=((MMlong)(*ptr))*(invSQROOT2);
			if (resf>MAXVAL32) resf=MAXVAL32;
			if (resf<MINVAL32) resf=MINVAL32;
			*ptr++=(MMshort)resf;
			*ptr++=(MMshort)resf;
		}
	}
	else
	{
		for(i=0;i<(size>>1);i++)
		{
			resf=(MMlong)*ptr++;
			if (resf>MAXVAL32) resf=MAXVAL32;
			if (resf<MINVAL32) resf=MINVAL32;
			*ptr++=(MMshort)resf;
		}
	}
}
void mixer_dual_mono_to_stereo_sample16(MMshort *ptr, MMshort size, MMshort normalize)
{
	MMshort i;
	MMlong resf;
	MMlong inval;
	short *ptr_s;
	MMshort Size=size&DATA_BLOCK_SIZE_MASK;
	ptr_s=(short *)ptr;
	if(normalize)
	{
		for(i=0;i<(Size>>1);i++)
		{
			inval=(MMlong)(((int)(*ptr_s))<<16);
			resf=inval*invSQROOT2;
			if (resf>MAXVAL32) resf=MAXVAL32;
			if (resf<MINVAL32) resf=MINVAL32;
			*ptr_s++=((int)resf)>>16;
			*ptr_s++=((int)resf)>>16;
		}
	}
	else
	{
		for(i=0;i<(Size>>1);i++)
		{
			resf=(MMlong)(((int)(*ptr_s))<<16);
			if (resf>MAXVAL32) resf=MAXVAL32;
			if (resf<MINVAL32) resf=MINVAL32;
			*ptr_s++=((int)resf)>>16;
			*ptr_s++=((int)resf)>>16;
		}
	}
}

#endif //ifdef MMDSP

