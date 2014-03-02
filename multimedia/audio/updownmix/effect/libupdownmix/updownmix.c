/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ctx->c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "updownmix.h"
#ifdef USE_TABLE
void getpathinfo(UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout);
#endif
void init_updownmix(
#ifdef USE_TABLE
		int nchin,int nchout,UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout,float *gain_table
#else
		int nchin,int nchout,UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout
#endif
)
{
//--------------- current availabilities --------------
//		nchin->nchout
//			1->1
//			1->2
//			1->6
//			2->6
//			2->1
//			6->1
//			6->2
//			6->6
//-----------------------------------------------------
#ifdef USE_TABLE
	ctx->nchin=nchin;
	ctx->nchout=nchout;
	ctx->gain_table=gain_table;
	getpathinfo(ctx,channelmapin,channelmapout);
#else
	if ((nchin==1)&&(nchout==2))
	{
		//---------------------- 1->2 ------------
    	ctx->inpath[0]=channelmapin[ChannelCF];
		ctx->outpath[0]=channelmapout[ChannelLF];
		ctx->gain[0]=1.0;

		ctx->inpath[1]=channelmapin[ChannelCF];
		ctx->outpath[1]=channelmapout[ChannelRF];
		ctx->gain[1]=1.0;

		ctx->numpath=2;
		ctx->nchin=1;
		ctx->nchout=2;
	}
	else if ((nchin==2)&&(nchout==1))
	{
		//---------------------- 2->1 ------------
		ctx->inpath[0]=channelmapin[ChannelLF];
		ctx->outpath[0]=channelmapout[ChannelCF];
    	ctx->gain[0]=.5;

		ctx->inpath[1]=channelmapin[ChannelRF];
		ctx->outpath[1]=channelmapout[ChannelCF];
		ctx->gain[1]=.5;

		ctx->numpath=2;
		ctx->nchin=2;
		ctx->nchout=1;
	}
	else if ((nchin==1)&&(nchout==6))
	{
		//---------------------- 1->6 ------------
		ctx->inpath[0]=channelmapin[ChannelCF];
		ctx->outpath[0]=channelmapout[ChannelLF];
    	ctx->gain[0]=1.0;

		ctx->inpath[1]=channelmapin[ChannelCF];
		ctx->outpath[1]=channelmapout[ChannelLS];
    	ctx->gain[1]=1.0;

		ctx->inpath[2]=channelmapin[ChannelCF];
		ctx->outpath[2]=channelmapout[ChannelCF];
    	ctx->gain[2]=1.0;

		ctx->inpath[3]=channelmapin[ChannelCF];
		ctx->outpath[3]=channelmapout[ChannelRS];
    	ctx->gain[3]=1.0;

		ctx->inpath[4]=channelmapin[ChannelCF];
		ctx->outpath[4]=channelmapout[ChannelRF];
    	ctx->gain[4]=1.0;

		ctx->inpath[5]=channelmapin[ChannelCF];
		ctx->outpath[5]=channelmapout[ChannelLFE];
    	ctx->gain[5]=1.0;
	
		ctx->numpath=6;
		ctx->nchin=1;
		ctx->nchout=6;
	}
	else if ((nchin==6)&&(nchout==1))
	{
		//---------------------- 6->1 ------------
		ctx->inpath[0]=channelmapin[ChannelLF];
		ctx->outpath[0]=channelmapout[ChannelCF];
    	ctx->gain[0]=.5/3;

		ctx->inpath[1]=channelmapin[ChannelLS];
		ctx->outpath[1]=channelmapout[ChannelCF];
    	ctx->gain[1]=.680838869/3;

		ctx->inpath[2]=channelmapin[ChannelCF];
		ctx->outpath[2]=channelmapout[ChannelCF];
    	ctx->gain[2]=.707106781/3;

		ctx->inpath[3]=channelmapin[ChannelRS];
		ctx->outpath[3]=channelmapout[ChannelCF];
    	ctx->gain[3]=.680838869/3;

		ctx->inpath[4]=channelmapin[ChannelRF];
		ctx->outpath[4]=channelmapout[ChannelCF];
    	ctx->gain[4]=.5/3;

		ctx->inpath[5]=channelmapin[ChannelLFE];
		ctx->outpath[5]=channelmapout[ChannelCF];
    	ctx->gain[5]=.25/3;
	
		ctx->numpath=6;
		ctx->nchin=6;
		ctx->nchout=1;
	}
	else if ((nchin==2)&&(nchout==6))
	{

		//---------- 2->6 Dolby Prologic II -------
		ctx->inpath[0]=channelmapin[ChannelLF];
		ctx->outpath[0]=channelmapout[ChannelLF];
		ctx->gain[0]=.5;

		ctx->inpath[1]=channelmapin[ChannelLF];
		ctx->outpath[1]=channelmapout[ChannelLS];
		ctx->gain[1]=.87177979/2;

		ctx->inpath[2]=channelmapin[ChannelLF];
		ctx->outpath[2]=channelmapout[ChannelCF];
		ctx->gain[2]=.70710678/2;

		ctx->inpath[3]=channelmapin[ChannelLF];
		ctx->outpath[3]=channelmapout[ChannelRS];
		ctx->gain[3]=.48989795/2;

		ctx->inpath[4]=channelmapin[ChannelRF];
		ctx->outpath[4]=channelmapout[ChannelLS];
		ctx->gain[4]=.48989795/2;

		ctx->inpath[5]=channelmapin[ChannelRF];
		ctx->outpath[5]=channelmapout[ChannelCF];
		ctx->gain[5]=.70710678/2;

		ctx->inpath[6]=channelmapin[ChannelRF];
		ctx->outpath[6]=channelmapout[ChannelRS];
		ctx->gain[6]=.87177979/2;

		ctx->inpath[7]=channelmapin[ChannelRF];
		ctx->outpath[7]=channelmapout[ChannelRF];
		ctx->gain[7]=.5;

		ctx->inpath[8]=channelmapin[ChannelRF];
		ctx->outpath[8]=channelmapout[ChannelLFE];
		ctx->gain[8]=0.0;


		ctx->numpath=9;
		ctx->nchin=2;
		ctx->nchout=6;
	}
	else if ((nchin==6)&&(nchout==2))
	{
		//---------- 6->2 -------
		ctx->inpath[0]=channelmapin[ChannelLF];
		ctx->outpath[0]=channelmapout[ChannelLF];
		ctx->gain[0]=1.0/3;

		ctx->inpath[1]=channelmapin[ChannelLS];
		ctx->outpath[1]=channelmapout[ChannelLF];
		ctx->gain[1]=.87177978900/3;

		ctx->inpath[2]=channelmapin[ChannelCF];
		ctx->outpath[2]=channelmapout[ChannelLF];
		ctx->gain[2]=.707106781/3;

		ctx->inpath[3]=channelmapin[ChannelRS];
		ctx->outpath[3]=channelmapout[ChannelLF];
		ctx->gain[3]=.489897949/3;

		ctx->inpath[4]=channelmapin[ChannelLS];
		ctx->outpath[4]=channelmapout[ChannelRF];
		ctx->gain[4]=.489897947/3;

		ctx->inpath[5]=channelmapin[ChannelCF];
		ctx->outpath[5]=channelmapout[ChannelRF];
		ctx->gain[5]=.707106781/3;

		ctx->inpath[6]=channelmapin[ChannelRS];
		ctx->outpath[6]=channelmapout[ChannelRF];
		ctx->gain[6]=.87177989/3;

		ctx->inpath[7]=channelmapin[ChannelRF];
		ctx->outpath[7]=channelmapout[ChannelRF];
		ctx->gain[7]=1.0/3;

		ctx->inpath[8]=channelmapin[ChannelLFE];
		ctx->outpath[8]=channelmapout[ChannelLF];
		ctx->gain[8]=.25/3;

		ctx->inpath[9]=channelmapin[ChannelLFE];
		ctx->outpath[9]=channelmapout[ChannelRF];
		ctx->gain[9]=.25/3;


		ctx->numpath=10;
		ctx->nchin=6;
		ctx->nchout=2;
	}
	else if ((nchin==6)&&(nchout==6))
	{
		//---------- 6->6 -------
		ctx->inpath[0]=channelmapin[ChannelLF];
		ctx->outpath[0]=channelmapout[ChannelLF];
		ctx->gain[0]=1.0;

		ctx->inpath[1]=channelmapin[ChannelRF];
		ctx->outpath[1]=channelmapout[ChannelRF];
		ctx->gain[1]=1.0;

		ctx->inpath[2]=channelmapin[ChannelCF];
		ctx->outpath[2]=channelmapout[ChannelCF];
		ctx->gain[2]=1.0;

		ctx->inpath[3]=channelmapin[ChannelLS];
		ctx->outpath[3]=channelmapout[ChannelLS];
		ctx->gain[3]=1.0;

		ctx->inpath[4]=channelmapin[ChannelRS];
		ctx->outpath[4]=channelmapout[ChannelRS];
		ctx->gain[4]=1.0;

		ctx->inpath[5]=channelmapin[ChannelLFE];
		ctx->outpath[5]=channelmapout[ChannelLFE];
		ctx->gain[5]=1.0;

		ctx->numpath=6;
		ctx->nchin=6;
		ctx->nchout=6;
	}
	else
	{
		//----------- default 1->1 out=in ---------
		ctx->inpath[0]=channelmapin[ChannelCF];
		ctx->outpath[0]=channelmapout[ChannelCF];
		ctx->gain[0]=1.0;

		ctx->numpath=1;
		ctx->nchin=1;
		ctx->nchout=1;
	}
#endif //ifdef USE_TABLE
}


void process_updownmix( int *input,int *output, int nsamples_per_channel,UPDOWNMIX_LOCAL_STRUCT_T *ctx)
{
#ifndef INPLACE
	//--------------------- Not In Place -------
	int i,indxin,indxout,pathno;
	float accu;
	for (i=0;i<nsamples_per_channel*(ctx->nchout);i++)
		output[i]=0;
	for (pathno=0;pathno<ctx->numpath;pathno++)
	{
		indxin=ctx->inpath[pathno];
		indxout=ctx->outpath[pathno];
		for (i=0;i<nsamples_per_channel;i++)
		{
			accu=output[indxout];
			accu+=ctx->gain[pathno]*(float)input[indxin];
			output[indxout]=accu;
			indxin+=ctx->nchin;
			indxout+=ctx->nchout;
		}
	}
#else
	//-------------------- In Place ------------------
	int i,indxin,indxout,pathno,scratch[MAXCH];
	float accu,outpath_touched[MAXCH];
	if (ctx->nchin<=ctx->nchout)  //upmix, work down, scratch buffer is on input
	{
		indxin=(nsamples_per_channel*ctx->nchin)-ctx->nchin;
		indxout=(nsamples_per_channel*ctx->nchout)-ctx->nchout;
		for (i=0;i<nsamples_per_channel;i++)
		{
			for (pathno=ctx->numpath-1;pathno>=0;pathno--)
			{
				scratch[ctx->inpath[pathno]]=input[indxin+ctx->inpath[pathno]];
				outpath_touched[ctx->outpath[pathno]]=0;
			}
			for (pathno=ctx->numpath-1;pathno>=0;pathno--)
			{
				accu=outpath_touched[ctx->outpath[pathno]]*output[indxout+ctx->outpath[pathno]];
				accu+=ctx->gain[pathno]*(float)scratch[ctx->inpath[pathno]];
				outpath_touched[ctx->outpath[pathno]]=1.0;
				output[indxout+ctx->outpath[pathno]]=accu;
			}
			indxin-=ctx->nchin;
			indxout-=ctx->nchout;
		}
	}
	else  //downmix, work up, scratch buffer is on output
	{
		indxin=0;
		indxout=0;
		for (i=0;i<nsamples_per_channel;i++)
		{
			for (pathno=0;pathno<ctx->numpath;pathno++)
				scratch[ctx->outpath[pathno]]=0.0;
			for (pathno=0;pathno<ctx->numpath;pathno++)
			{
				accu=scratch[ctx->outpath[pathno]];
				accu+=ctx->gain[pathno]*(float)input[indxin+ctx->inpath[pathno]];
				scratch[ctx->outpath[pathno]]=accu;
			}
			for (pathno=0;pathno<ctx->numpath;pathno++)
				output[indxout+ctx->outpath[pathno]]=scratch[ctx->outpath[pathno]];
			indxin+=ctx->nchin;
			indxout+=ctx->nchout;
		}
	}
#endif
}


void process_updownmix_sample16( short *input,short *output, int nsamples_per_channel,UPDOWNMIX_LOCAL_STRUCT_T *ctx)
{
#ifndef INPLACE
	//--------------------- Not In Place -------
	int i,indxin,indxout,pathno;
	float accu;
	for (i=0;i<nsamples_per_channel*(ctx->nchout);i++)
		output[i]=0;
	for (pathno=0;pathno<ctx->numpath;pathno++)
	{
		indxin=ctx->inpath[pathno];
		indxout=ctx->outpath[pathno];
		for (i=0;i<nsamples_per_channel;i++)
		{
			accu=(float)(((int)output[indxout])<<16);
			accu+=ctx->gain[pathno]*(float)(((int)input[indxin])<<16) ;
			output[indxout]=(short)(((int)accu)>>16);
			indxin+=ctx->nchin;
			indxout+=ctx->nchout;
		}
	}
#else
	//-------------------- In Place ------------------
	int i,indxin,indxout,pathno;
	int scratch[MAXCH];
	float accu,outpath_touched[MAXCH];
	if (ctx->nchin<=ctx->nchout)  //upmix, work down, scratch buffer is on input
	{
		indxin=(nsamples_per_channel*ctx->nchin)-ctx->nchin;
		indxout=(nsamples_per_channel*ctx->nchout)-ctx->nchout;
		for (i=0;i<nsamples_per_channel;i++)
		{
			for (pathno=ctx->numpath-1;pathno>=0;pathno--)
			{
				scratch[ctx->inpath[pathno]]=((int)(input[indxin+ctx->inpath[pathno]]))<<16;
				outpath_touched[ctx->outpath[pathno]]=0;
			}
			for (pathno=ctx->numpath-1;pathno>=0;pathno--)
			{
				accu=outpath_touched[ctx->outpath[pathno]]*((float)(((int)(output[indxout+ctx->outpath[pathno]]))<<16));
				accu+=ctx->gain[pathno]*(float)scratch[ctx->inpath[pathno]];
				outpath_touched[ctx->outpath[pathno]]=1.0;
				output[indxout+ctx->outpath[pathno]]=(short)(((int)accu)>>16);
			}
			indxin-=ctx->nchin;
			indxout-=ctx->nchout;
		}
	}
	else  //downmix, work up, scratch buffer is on output
	{
		indxin=0;
		indxout=0;
		for (i=0;i<nsamples_per_channel;i++)
		{
			for (pathno=0;pathno<ctx->numpath;pathno++)
				scratch[ctx->outpath[pathno]]=0.0;
			for (pathno=0;pathno<ctx->numpath;pathno++)
			{
				accu=scratch[ctx->outpath[pathno]];
				accu+=ctx->gain[pathno]*(float)(((int)input[indxin+ctx->inpath[pathno]])<<16);
				scratch[ctx->outpath[pathno]]=accu;
			}
			for (pathno=0;pathno<ctx->numpath;pathno++)
				output[indxout+ctx->outpath[pathno]]=(short)((scratch[ctx->outpath[pathno]])>>16);
			indxin+=ctx->nchin;
			indxout+=ctx->nchout;
		}
	}
#endif
}
#ifdef USE_TABLE
void getpathinfo(UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout)
{
//	---------------------------------------------
//  info structure of gainxy[MAXCH*MAXCH]
//  x=input channel number
//  y=output channel number
//  ---------------------------------------------
//	gain11	input channel 1 to output channel 1
//	gain12	input channel 1 to output channel 2
//		.			.			.
//		.			.			.
//	gain19			.			.
//	gain21	input channel 2 to output channel 1
//	gain22			.			.
//		.			.			.
//		.			.			.
//	gain99	input channel 9 to output channel 9
//	---------------------------------------------
	int indxin,indxout,infoindx,pathindx=0;
	float gain;
	for (indxin=0;indxin<MAXCH;indxin++) //sweep each line
	{
		for (indxout=0;indxout<MAXCH;indxout++) //sweep each column
		{
			infoindx=MAXCH*indxin+indxout;
			gain=ctx->gain_table[infoindx];
			if (gain!=0.0)
			{
				ctx->inpath[pathindx]=channelmapin[indxin+1]; //1st indx is channelNone
				ctx->outpath[pathindx]=channelmapout[indxout+1];
				ctx->gain[pathindx]=gain;
				pathindx++;
			}
		}
	}
	ctx->numpath=pathindx;
}
#endif //ifdef USE_TABLE

