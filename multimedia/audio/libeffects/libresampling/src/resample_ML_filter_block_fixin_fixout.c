/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_ML_filter_block_fixin_fixout.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "resample_local.h"
#if !defined(MMDSP)  && !defined(ARM) && !defined(NEON)
#define SATMAXp   (double)( 2.147483e9)
#define SATMAXn   (double)(-2.147483e9)
/********** COMMON CODE FOR UP AND DOWN SAMPLING ****************/
void
resample_ML_filter_block_fixin_fixout
						(
						 int *in,
						 unsigned int    istride,
						 unsigned int    nbIn,
						 int *out,
						 unsigned int    ostride,
						 unsigned int    nbOutRequested,
						 unsigned int    *nbOutAvail,
						 ResampleDelayLine *delayLine,
						 ResampleFilter *filterInfo,
						 unsigned int loop_siz
 						 ,Float *CoefRam,
						 int squelch
						 )
{
	unsigned int i,iter;
	unsigned int j=0;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,odd,forwardlen;
	float val;
	Float coef;
	int dfindx=filterInfo->nzcoefs-delayLine->len*M*decimno+filterInfo->ncfs;
	int drindx=filterInfo->ncfs+2*filterInfo->nzcoefs;
	if (( filterInfo->coef != 0 )&&(nbIn>0))
	{
		/* this is a pure downsampling filter, compensate for normalization of
	   	the filter coefficients */
		j=0;
		for (i=0;i<nbIn;i++)
		{
			in[j]*=filterInfo->coef;
			j+=istride;
		}
		j=0;
	}
	odd=(delayLine->len&1);
	forwardlen=((delayLine->len)/2)+odd;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = dfindx+decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			f_index = drindx  -1-f_index;
			for(i=0; i<delayLine->len/2; i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index -= coefinc;
				/* mac */
				Acc += val*coef;
			} /* end second half */
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			*out = Acc;
			out = out + ostride;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->len-1;
			}
			delayLine->offset -= M;
			if (nbIn==0)
				val=0;
			else
				val = in[j*istride];
			j++;
			delayLine->baseAddress[currIndex]=val;
		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
}
/********** CODE FOR UP SAMPLING ONLY ***************************/
void
resample_ML_load_block_fixin_fixout(int *in,
						 unsigned int    istride,
						 unsigned int    nbIn,
						 ResampleDelayLine *delayLine,
						 ResampleFilter *filterInfo,int squelch/*,int low_latency*/)
{
	unsigned int i,j;
	int val;
	if (( filterInfo->coef != 0 )&&(nbIn>0))
	{
		/* this is a pure downsampling filter, compensate for normalization of
	   	the filter coefficients */
//		vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
		j=0;
		for (i=0;i<nbIn;i++)
		{
			in[j]*=filterInfo->coef;
			j+=istride;
		}
	}
	for (j=0;j<nbIn;j++)
	{
		delayLine->writeIndex--;
		if( delayLine->writeIndex < 0 )
		{
			delayLine->writeIndex = delayLine->totalen-1;
		}
		if (squelch!=0)
			val=0;
		else
			val = in[j*istride];
		delayLine->nbremain++;
		delayLine->baseAddress[delayLine->writeIndex] = val;
	}
}
void
resample_ML_conv_block_fixin_fixout
						(
						 int *out,
						 unsigned int    ostride,
						 unsigned int    nbOutRequested,
						 unsigned int    *nbOutAvail,
						 ResampleDelayLine *delayLine,
						 ResampleFilter *filterInfo,
						 unsigned int loop_siz
 						 ,Float *CoefRam
						 )
{
	unsigned int i,iter;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,odd,forwardlen;
	float val; 
	Float coef;
	int dfindx=filterInfo->nzcoefs-delayLine->len*M*decimno+filterInfo->ncfs;
	int drindx=filterInfo->ncfs+2*filterInfo->nzcoefs;
	odd=(delayLine->len&1);
	forwardlen=((delayLine->len)/2)+odd;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = dfindx+decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->totalen )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			f_index = drindx  -1-f_index;
			for(i=0; i<delayLine->len/2; i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->totalen )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index -= coefinc;
				/* mac */
				Acc += val*coef;
			} /* end second half */
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			*out = Acc;
			out = out + ostride;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->totalen-1;
			}
			delayLine->offset -= M;
			delayLine->nbremain--;

		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
}


/********** CODE FOR DOWN SAMPLING ONLY *****************/
void
resample_ML_convout_block_fixin_fixout
						(
						 int *in,
						 unsigned int    istride,
						 unsigned int    nbIn,
						 ResampleRemainbuf *resampleremainbuf,
						 unsigned int    nbOutRequested,
						 unsigned int    *nbOutAvail,
						 ResampleDelayLine *delayLine,
						 ResampleFilter *filterInfo,
						 unsigned int loop_siz
						 ,unsigned int block_siz
 						 ,Float *CoefRam
						 )
{
	unsigned int i,iter;
	unsigned int j=0;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,odd,forwardlen;
	float val; 
	Float coef;
	int dfindx=filterInfo->nzcoefs-delayLine->len*M*decimno+filterInfo->ncfs;
	int drindx=filterInfo->ncfs+2*filterInfo->nzcoefs;
	if (( filterInfo->coef != 0 )&&(nbIn>0))
	{
		/* this is a pure downsampling filter, compensate for normalization of
	   	the filter coefficients */
		j=0;
		for (i=0;i<nbIn;i++)
		{
			in[j]*=filterInfo->coef;
			j+=istride;
		}
		j=0;
	}
	odd=(delayLine->len&1);
	forwardlen=((delayLine->len)/2)+odd;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = dfindx+decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			f_index = drindx  -1-f_index;
			for(i=0; i<delayLine->len/2; i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index -= coefinc;
				/* mac */
				Acc += val*coef;
			} /* end second half */
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			resampleremainbuf->baseAddress[resampleremainbuf->writeptr]=Acc;
			if (resampleremainbuf->writeptr==(int)block_siz)
				resampleremainbuf->writeptr=0;
			else
				resampleremainbuf->writeptr++;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->len-1;
			}
			delayLine->offset -= M;
			if (nbIn==0)
				val=0;
			else
				val = in[j*istride];
			j++;
			delayLine->baseAddress[currIndex]=val;

		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
	resampleremainbuf->nbremain+=nbOutRequested;
}


void
resample_ML_convout_block_fixin_fixout_ll
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 ResampleRemainbuf *resampleremainbuf,
 unsigned int    nbOutRequested,
 unsigned int    *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,unsigned int block_siz
 ,Float *CoefRam
 )
{
	unsigned int i,iter;
	unsigned int j=0;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,forwardlen;
	Float coef;
	float val;

	if (( filterInfo->coef != 0 )&&(nbIn>0))
	{
		/* this is a pure downsampling filter, compensate for normalization of
		   the filter coefficients */
//		vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
		j=0;
		for (i=0;i<nbIn;i++)
		{
			in[j]*=filterInfo->coef;
			j+=istride;
		}
		j=0;
	}

	forwardlen=delayLine->len;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			resampleremainbuf->baseAddress[resampleremainbuf->writeptr]=Acc;
			if (resampleremainbuf->writeptr==(int)block_siz)
				resampleremainbuf->writeptr=0;
			else
				resampleremainbuf->writeptr++;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->len-1;
			}
			delayLine->offset -= M;
			if (nbIn==0)
				val=0;
			else
				val = in[j*istride];
			j++;
			delayLine->baseAddress[currIndex]=val;

		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
	resampleremainbuf->nbremain+=nbOutRequested;
} 


void
resample_ML_conv_block_fixin_fixout_ll
(
 int *out,
 unsigned int    ostride,
 unsigned int    nbOutRequested,
 unsigned int    *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam
 )
{
	unsigned int i,iter;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,forwardlen;
	Float coef;
	float val;

	forwardlen=delayLine->len;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->totalen )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			*out = Acc;
			out = out + ostride;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->totalen-1;
			}
			delayLine->offset -= M;
			delayLine->nbremain--;

		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
}


void
resample_ML_filter_block_fixin_fixout_ll
(
 int *in,
 unsigned int    istride,
 unsigned int    nbIn,
 int *out,
 unsigned int    ostride,
 unsigned int    nbOutRequested,
 unsigned int    *nbOutAvail,
 ResampleDelayLine *delayLine,
 ResampleFilter *filterInfo,
 unsigned int loop_siz
 ,Float *CoefRam,
 int squelch
 )
{
	unsigned int i,iter;
	unsigned int j=0;
	unsigned int    M = filterInfo->M;
	unsigned int    L = filterInfo->L;
	int decimno = filterInfo->decimno;
	int		coefinc=decimno*M;
	const YMEM Float *filter = filterInfo->filter;
	int    shift = filterInfo->shift;
	double Acc;
	unsigned int index;
	int currIndex = delayLine->currIndex;
	int f_index,forwardlen;
	Float coef;
	float val;

	if (( filterInfo->coef != 0 )&&(nbIn>0))
	{
		/* this is a pure downsampling filter, compensate for normalization of
		   the filter coefficients */
//		vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
		j=0;
		for (i=0;i<nbIn;i++)
		{
			in[j]*=filterInfo->coef;
			j+=istride;
		}
		j=0;
	}

	forwardlen=delayLine->len;
	for (iter=0;iter<loop_siz;iter++)	
	{
		if( delayLine->offset < M) 
		{
			f_index = decimno*delayLine->offset;
			index = currIndex;
			Acc = 0;
			/* Do first half */
			for(i=0;i<forwardlen;i++)
			{
				/* get data */
				val = delayLine->baseAddress[index];
				index += 1;
				if( index == delayLine->len )
					index = 0;
				/* get coeff */
				coef = filter[f_index];
				f_index += coefinc;
				/* mac */
				Acc += val*coef;
			} /* end first half */
			/* compensate for normalization */
			if( shift >= 0 )
			{
				Acc = Acc / (1<<shift);
			}
			else
			{
				Acc = Acc * (1<<(-shift));
			}
			if (Acc>SATMAXp) Acc=SATMAXp;
			if (Acc<SATMAXn) Acc=SATMAXn;
//			assert( Acc < 1.0);
//			assert( Acc >= -1.0);
			*out = Acc;
			out = out + ostride;
			delayLine->offset += L;
		} /* end if part */
		else
		{
			currIndex--;
			if (currIndex<0)
			{
				currIndex = delayLine->len-1;
			}
			delayLine->offset -= M;
			if ((nbIn==0)||(squelch!=0))
				val=0;
			else
				val = in[j*istride];
			j++;
			delayLine->baseAddress[currIndex]=val;
		} /* end else part*/
	} /* end while */
	delayLine->currIndex = currIndex;
	*nbOutAvail=nbOutRequested;
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
	for (iter=0;iter<nbOutRequested;iter++)
	{
		*out=resampleremainbuf->baseAddress[resampleremainbuf->readptr];
		if (resampleremainbuf->readptr==(int)block_siz)
			resampleremainbuf->readptr=0;
		else
			resampleremainbuf->readptr++;
		out = out + ostride;
	}
	resampleremainbuf->nbremain-=nbOutRequested;
}

#endif //MMDSP

#ifndef M4_LL_MSEC
int 
resample_ML_read_block_fixin_fixout(ResampleDelayLine *delayLine)
{
	return delayLine->nbremain;
}
#endif





