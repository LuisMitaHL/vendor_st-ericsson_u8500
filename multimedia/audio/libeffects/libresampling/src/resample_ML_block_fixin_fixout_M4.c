/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_ML_block_fixin_fixout.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "resample_local.h"

#ifdef M4_LL_MSEC
#define SATMAX 0x7fff

int
resample_ML_block_M4(ResampleContext *ctx,
		int             *in,
		int             nb_channels,
		int             *nSamples_inmsec,
		int             *out,
		int             *nbOutSamples_inmsec,
		int             *flush)
{
#ifdef OPT_M4    
	int *CoefRam;
	ResampleContextChannel *ctxChan;
	ResampleDelayLine *delayLine;
	ResampleFilter *filterInfo;
	// convert ms to nsamples using Q10 format 
	int nSamples=(*nSamples_inmsec*ctx->nSamples_coef)>>10;
	// find nbOutRequested according to M/L ration in Q10 format
	unsigned int nbOutRequested;
	unsigned int    M;
	const EXTERN int *incr_offset_tab;
	unsigned int actualLen;
	int currIndex;
	unsigned int offset;
	short  val ;	
    short *p_val_s ;
    int Acc=0;
   	int *p_coef_s;
    int *p_val ;

	unsigned int j,k,delayLine_len;

	int i,incr;
	int nbin_to_consume;
	int   *baseAddress;	
	short 	 *max_adr_s;
	short 	 *min_adr_s;
	short *inptr,*outptr;
	*flush=0;
	// convert nbout to msec
	*nbOutSamples_inmsec=*nSamples_inmsec;
	if (ctx->fin_khz==ctx->fout_khz)
	{
		for (j=(nSamples-1);j>0;j--) 
            out[j]=in[j];

        out[j]=in[j];
		return 0;
	}

	nbOutRequested=(nSamples*ctx->nbOutReq_coef)>>10;
	CoefRam=ctx->CoefRam[0];
	filterInfo=ctx->filterInfo[0];
	incr_offset_tab=filterInfo->incr_offset_tab;
	M = filterInfo->M;
	ctxChan = &ctx->ctxChan[0];
	delayLine=ctxChan->delay[0];
	currIndex = delayLine->currIndex;
	offset = delayLine->offset;
	baseAddress = &delayLine->baseAddress[0];
    delayLine_len = delayLine->len ;
	max_adr_s = (short*)baseAddress+(short)(delayLine_len);
	min_adr_s = (short *)baseAddress;
	p_val_s=(short *)baseAddress;
	p_val_s += (short)currIndex;
	outptr=(short*)out;

	actualLen = delayLine_len+((delayLine_len)&1); //number of additional coefs needed to be a multiple of 2 and the last ones are set to 0
	nbin_to_consume=nSamples;
	inptr=(short*)in;

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
				val=*inptr;
				inptr++;
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine_len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s = (int *)((short*)CoefRam+offset*(actualLen));
            p_val    = (int *)p_val_s;			
            
            Acc = __smuad( p_val[0],p_coef_s[0]);

            for (k=((actualLen>>1)-1);k>0;k--)
                Acc = __smlad( p_val[k],p_coef_s[k],Acc );		          			
            
            Acc = (Acc+0x4000)>>15;             
			Acc=__ssat(Acc,16);

            val = Acc ;            
            *outptr=val;          
			outptr++;
			
            incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;

			offset=incr_offset_tab[offset]&0xff;           

			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;

				val=*inptr;
				inptr++;
				
                *p_val_s=(short)val;
				*(p_val_s+delayLine_len)=(short)val;
			}

			nbin_to_consume-=incr;
		}
	} 
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);   
	return 0;
#else //OPT_M4

    int *CoefRam;
	ResampleContextChannel *ctxChan;
	ResampleDelayLine *delayLine;
	ResampleFilter *filterInfo;
	// convert ms to nsamples using Q10 format 
	int nSamples=(*nSamples_inmsec*ctx->nSamples_coef)>>10;
	// find nbOutRequested according to M/L ration in Q10 format
	unsigned int nbOutRequested;
	unsigned int    M;
	const EXTERN int *incr_offset_tab;
	unsigned int actualLen;
	int currIndex;
	unsigned int offset;
	short  val ;
	int sum0_s,sum1_s;
	long long Acc=0;
	short *p_coef_s;
	int j;
	int i,incr;
	int nbin_to_consume;
	int   *baseAddress;
	short *p_val_s ;
	short 	 *max_adr_s;
	short 	 *min_adr_s;
	short *inptr,*outptr;
	*flush=0;
	// convert nbout to msec
	*nbOutSamples_inmsec=*nSamples_inmsec;
	if (ctx->fin_khz==ctx->fout_khz)
	{
		for (j=0;j<nSamples;j++) out[j]=in[j];
		return 0;
	}
	nbOutRequested=(nSamples*ctx->nbOutReq_coef)>>10;
	CoefRam=ctx->CoefRam[0];
	filterInfo=ctx->filterInfo[0];
	incr_offset_tab=filterInfo->incr_offset_tab;
	M = filterInfo->M;
	ctxChan = &ctx->ctxChan[0];
	delayLine=ctxChan->delay[0];
	currIndex = delayLine->currIndex;
	offset = delayLine->offset;
	baseAddress = &delayLine->baseAddress[0];
	max_adr_s = (short*)baseAddress+(short)(delayLine->len);
	min_adr_s = (short *)baseAddress;
	p_val_s=(short *)baseAddress;
	p_val_s += (short)currIndex;
	outptr=(short*)out;
	actualLen = delayLine->len+((delayLine->len)&1); //number of additional coefs needed to be a multiple of 2 and the last ones are set to 0
	nbin_to_consume=nSamples;
	inptr=(short*)in;
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
				val=*inptr;
				inptr++;
				*p_val_s=(short)val;
				*(p_val_s+(short)delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
		for (j=nbOutRequested;j>0;j--)
		{
			p_coef_s=(short*)CoefRam+offset*(actualLen);
			sum0_s=sum1_s=0;
			for (i=0;i<actualLen>>1;i++)
			{
				sum0_s+=(int)p_val_s[2*i+0]*(int)p_coef_s[2*i+0];
				sum1_s+=(int)p_val_s[2*i+1]*(int)p_coef_s[2*i+1];
			}
			Acc=sum0_s+sum1_s;
			Acc = (Acc+0x4000)>>15;
			if (Acc>(SATMAX)) Acc=SATMAX;
			if (Acc<-(SATMAX)) Acc=-(SATMAX);
			val=Acc;
			incr=(incr_offset_tab[offset]>>8)&0xff;
			if (nbin_to_consume<incr)
				incr=nbin_to_consume;
			offset=incr_offset_tab[offset]&0xff;
			*outptr=val;
			outptr++;
			for (i=incr;i>0;i--)
			{
				if (p_val_s==min_adr_s)
					p_val_s=max_adr_s-1;
				else
					p_val_s--;
				val=*inptr;
				inptr++;
				*p_val_s=(short)val;
				*(p_val_s+delayLine->len)=(short)val;
			}
			nbin_to_consume-=incr;
		}
	} 
	delayLine->currIndex = (int)(p_val_s - (short*)baseAddress);
//-----------------------------------------------------------------
	return 0;
#endif //OPT_M4   
} 
#endif //ifdef M4_LL_MSEC
