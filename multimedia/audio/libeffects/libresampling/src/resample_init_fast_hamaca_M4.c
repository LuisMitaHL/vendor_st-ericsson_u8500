/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_init_fast_hamaca.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/libresampling.nmf>
#endif
#include "resample_local.h"
#ifdef M4_LL_MSEC
#define ESAA_FREQ_48 7
#define ESAA_FREQ_16 12
static int resample_init_core(char *heap,int size_heap,
        ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
        unsigned int channel_nb, unsigned int blocksiz);
static int get_coef_ram_size(int Inrate,int Outrate,int src_quality,int stage);
static int find_index(int fin,int fout,int *index,int *finEQfout);
static RESAMPLE_MEM ResampleFilter fast_48_16_low_latency = {
    M_48_16_low_latency,
    L_48_16_low_latency,
    decimno_48_16_low_latency,
    resample_48_16_low_latency,
    src_incr_offset_1_3,
    N_48_16_low_latency,
    SHIFT_48_16_low_latency,
    SCALE_48_16_low_latency,
    NZCOEFS_48_16_low_latency
};
static RESAMPLE_MEM ResampleFilter fast_16_48_low_latency = {
    M_16_48_low_latency,
    L_16_48_low_latency,
    decimno_16_48_low_latency,
    resample_16_48_low_latency,
    src_incr_offset_3_1,
    N_16_48_low_latency,
    SHIFT_16_48_low_latency,
    SCALE_16_48_low_latency,
    NZCOEFS_16_48_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_48_16_low_latency[1] = {
    (ResampleFilter *)&fast_48_16_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_16_48_low_latency[1] = {
    (ResampleFilter *)&fast_16_48_low_latency
};
const RESAMPLE_MEM int delay_48_16_low_latency[1] = {DELAY_48_16_low_latency};
const RESAMPLE_MEM int delay_16_48_low_latency[1] = {DELAY_16_48_low_latency};
static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtrM4[4]={
	delay_16_48_low_latency,	//16->16
    delay_16_48_low_latency,	//16->48
    delay_48_16_low_latency,   	//48->16
    delay_48_16_low_latency		//48->48
};
static cascadedFilter * const RESAMPLE_MEM CascadedPtrM4[4]={
	Fast_16_48_low_latency,		//16->16
    Fast_16_48_low_latency,		//16->48
    Fast_48_16_low_latency,		//48->16
    Fast_48_16_low_latency		//48->48
};
static const RESAMPLE_MEM int MaxCascadedPtrM4[4]={
    1,  //16->16
    1,  //16->48
    1,  //48->16
    1   //48->48
};
static const RESAMPLE_MEM int finkhzPtrM4[4]={
	16,	//16->16
	16, //16->48
	48, //48->16
	48  //48->48
};
static const RESAMPLE_MEM int foutkhzPtrM4[4]={
	16,	//16->16
	48, //16->48
	16, //48->16
	48  //48->48
};


int
resample_calc_heap_size_fixin_fixout(int Inrate,int Outrate,int src_quality,int *heapsiz, unsigned int blocksiz,unsigned int channel_nb)
{
	int *delay_ptr=0;
    int maxCascade=1,indx=0;
    int finEQfout,index,i,dlylen=0;
	unsigned int Channel_nb=1;
	*heapsiz=0;
    if  (src_quality!=SRC_LOW_LATENCY_IN_MSEC) return 1;
	if (find_index(Inrate,Outrate,&index,&finEQfout)!=0) return 1;
	if (finEQfout) return 0;
	delay_ptr=(int*)DelayPtrM4[index];
	maxCascade=MaxCascadedPtrM4[index];
	dlylen=0;
	for (i=0;i<maxCascade;i++)
	{
		dlylen+=delay_ptr[i]+VECTOR_LEN_M4;
	}
    *heapsiz=(sizeof(ResampleDelayLine)*Channel_nb*maxCascade)+     //xxx (delay line structure)*(no of channels)*(no of stages)
        (sizeof(short)*nb_buf*dlylen*Channel_nb);                     //xxx (accumeulated delay line lenth for all stages)*(no of channels)
    for (indx=0;indx<maxCascade;indx++)
    {
        *heapsiz+=get_coef_ram_size(Inrate,Outrate,src_quality,indx);
                                                                    //xxx (ncoefs copied to ram for CA9)
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------------------
//  Inrate=ESAA_FREQ_xxKHZ  (ESAA_FREQ_UNKNOWNKHZ=0)
//  Outrate=ESAA_FREQ_xxKHZ
//  src_quality 0=high quality 1=low ripple 2=low_latency 6=low_latency with fixed I/O nsamples in msec
//  channel_nb 1=mono 2=stereo
//  conversion_type 0=UNKNOWN 1=UPSAMPLING 2=DOWNSAMPLING
//-------------------------------------------------------------------------------------------------------------
AUDIO_API_EXPORT int
resample_calc_max_heap_size_fixin_fixout(int Inrate,int Outrate,int src_quality,int *heapsiz, unsigned int blocksiz,unsigned int channel_nb,unsigned int conversion_type)
{
	unsigned int Channel_nb=1;
    if  (src_quality!=SRC_LOW_LATENCY_IN_MSEC) return 1;
	resample_calc_heap_size_fixin_fixout(ESAA_FREQ_48,ESAA_FREQ_16,src_quality,heapsiz,blocksiz,Channel_nb);
    if (*heapsiz!=0)
        return 0;
    else
        return 1;
}
AUDIO_API_EXPORT int
resample_x_init_ctx_low_mips_fixin_fixout_sample16(char *heap,int size_heap,
		ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
		unsigned int channel_nb, unsigned int blocksiz)
{
	int retval;
	unsigned int Channel_nb=1;
	retval=resample_init_core(heap,size_heap,
			ctx, Inrate, Outrate, src_quality,
			Channel_nb, blocksiz);
	return retval;
}
AUDIO_API_EXPORT int
resample_x_init_ctx_low_mips_fixin_fixout(char *heap,int size_heap,
        ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
        unsigned int channel_nb, unsigned int blocksiz)
{
	int retval;
	unsigned int Channel_nb=1;
	retval=resample_init_core(heap,size_heap,
			ctx, Inrate, Outrate, src_quality,
			Channel_nb, blocksiz);
	return retval;
}
int resample_init_core(char *heap,int size_heap,
		ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
		unsigned int channel_nb, unsigned int blocksiz)
{
	unsigned int i,j;
	int indx;
	int *dlyline_data=0,*delay_ptr=0;
	char *heap_ptr;
	int heap_max_memory;
	int heap_total_char;
	int dlylen,delta;
	int index,finEQfout,inptr;
	int dfindx;
	long long  longtemp;
	short temp_s;
	int temp;
	short *CoefRam_s[RESAMPLE_MAX_CASCADE];
	resample_alloc_parameters alloc_params;
	if  (src_quality!=SRC_LOW_LATENCY_IN_MSEC) return 1;
	if (find_index(Inrate,Outrate,&index,&finEQfout)!=0) return 1;
	ctx->processing = resample_x_process_fixin_fixout;
	ctx->low_latency=SRC_LOW_LATENCY_IN_MSEC;
	ctx->block_siz=blocksiz;
	ctx->block_size_by_nb_channel=blocksiz;
	ctx->maxCascade=MaxCascadedPtrM4[index];
	ctx->fin_khz=finkhzPtrM4[index];
	ctx->fout_khz=foutkhzPtrM4[index];
	ctx->filterInfo=CascadedPtrM4[index];
	if (finEQfout) return 0;
	alloc_params.ptr=&heap_ptr;
	alloc_params.max_memory=&heap_max_memory;
	alloc_params.total_char=&heap_total_char;
	resample_init_alloc(heap,size_heap,&alloc_params);
	delay_ptr=(int*)DelayPtrM4[index];
	for(j=0;j<ctx->maxCascade;j++)
	{
		/*----------- initialize delay lines -----*/
		for(i=0;i<channel_nb;i++)
		{
			ctx->ctxChan[i].delay[j]=resample_alloc(1,sizeof(ResampleDelayLine),&alloc_params); //xxx (dly line struct size)*(nb chan)*(no. stages)
			if (ctx->ctxChan[i].delay[j]==NULL) return 1;
			ctx->ctxChan[i].delay[j]->currIndex = 0;
			ctx->ctxChan[i].delay[j]->offset = ctx->filterInfo[j]->M;
		}
		for(i=0;i<channel_nb;i++)
		{
			dlyline_data=resample_alloc(nb_buf*delay_ptr[j]+VECTOR_LEN_M4-1,sizeof(short),&alloc_params);
			if (dlyline_data==NULL) return 1;
			ctx->ctxChan[i].delay[j]->baseAddress=dlyline_data;
			ctx->ctxChan[i].delay[j]->len=delay_ptr[j];
		}
		ctx->CoefRam[j]=resample_alloc(get_coef_ram_size(Inrate,Outrate,src_quality,j),1,&alloc_params); //xxx cpy coefs to ram for CA9
		CoefRam_s[j]=(short*)ctx->CoefRam[j];
	}
	/*---------------------- Copy each stage's Coefs into contiguous Ram by phase pages -----------------------*/
	for (j=0;j<ctx->maxCascade;j++)
	{
		dlylen=ctx->ctxChan[0].delay[j]->len;
		delta=dlylen&(VECTOR_LEN_M4-1);
		if (delta!=0)
			delta=VECTOR_LEN_M4-delta; //number of additional coefs needed to be a multiple of vector_len so the last ones can be set to 0
		dfindx=ctx->filterInfo[j]->nzcoefs-
			dlylen*ctx->filterInfo[j]->M*ctx->filterInfo[j]->decimno+
			ctx->filterInfo[j]->ncfs;
		for (indx=0;indx<ctx->filterInfo[j]->M;indx++)
		{
			for (i=0;i<dlylen;i++)
			{

				// eg. for M/L = 3/1 x 4/4 = 12/4 and len=60
				// phase0 CoefRam[0,1,.......59] = CoefConst[0,12,24 .. 708]
				// phase1 CoefRam[60,61,....119] = CoefConst[4,16,28 .. 712]
				// phase2 CoefRam[120,121,..179] = CoefConst[8,20,32 .. 716]
				//        CoefRam[indx*len+i]    = CoefConst[decimno*indx+decimno*i*M]
				inptr=dfindx+ctx->filterInfo[j]->decimno*indx+ctx->filterInfo[j]->decimno*i*ctx->filterInfo[j]->M;
				longtemp=((long long)(ctx->filterInfo[j]->filter[inptr])*(long long)(ctx->filterInfo[j]->coef))<<1;
				if ((ctx->filterInfo[j]->shift)<0)
					longtemp<<=-(ctx->filterInfo[j]->shift);
				else
					longtemp>>=ctx->filterInfo[j]->shift;
				temp=((longtemp+0x800000)>>24);
				temp_s=(short)((temp+0x80)>>8); // store 16-bit MSB of coef
				CoefRam_s[j][indx*(delta+dlylen)+i]=temp_s;
			}
			for (i=dlylen;i<dlylen+delta;i++) CoefRam_s[j][indx*(delta+dlylen)+i]=0;
		}
	}
	return 0;
}
int
resample_x_cpy_0_1_ctx_low_mips_fixin_fixout(ResampleContext *ctx, int Inrate,int Outrate,int src_quality,unsigned int blocksiz)
{
    return 0;
}
void resample_init_alloc(char *base_adr, int size_mem, resample_alloc_parameters *param)
{
    if (base_adr != NULL)
    {
        *param->ptr=base_adr;
        *param->max_memory=size_mem;
        *param->total_char=0;
    }
}
void *resample_alloc(int num, int size, resample_alloc_parameters *param )
{
    char *base;
    int size_n = size*num;
    int i;
    while ((unsigned int) *param->total_char % sizeof(int) != 0)
    {
        (*param->ptr)++;
        (*param->total_char)++;
    }
    (*param->total_char)+=size_n;
    if (*param->total_char>*param->max_memory)
    {
        base=NULL;
    }
    else
    {
        base=*param->ptr;
        for (i=0;i<size_n;i++)
            base[i]=0;
    }
    (*param->ptr)+=size_n;
    return base;
}
int get_coef_ram_size(int Inrate,int Outrate,int src_quality,int stage)
{
    int siz=sizeof(Float);
	int *delay_ptr;
    int dlylen,M,delta,index,finEQfout;
    cascadedFilter *tempfilt;
	find_index(Inrate,Outrate,&index,&finEQfout);
	delay_ptr=(int*)DelayPtrM4[index];
	tempfilt=CascadedPtrM4[index]; 
    siz=0;
    M=tempfilt[stage]->M;
	dlylen=delay_ptr[stage];
	delta=dlylen&(VECTOR_LEN_M4-1);
    if (delta!=0)
        dlylen=dlylen+VECTOR_LEN_M4-delta; //number of coefs needs to be a multiple of vector_len so the last ones may be set to 0
	siz+=M*dlylen*sizeof(short);
    return siz;
}
int resample_choose_processing_routine(ResampleContext *ctx)
{
	ctx->processing = resample_ML_block_M4;
	if (ctx->fin_khz==16)
	{
		ctx->nSamples_coef=0x4001; 	// x16 
		ctx->nbOutReq_coef=0xc01; 	// x3

	}
	else
	{
		ctx->nSamples_coef=0xc001; 	// x48 
		ctx->nbOutReq_coef=0x156; 	// 1/3
	}
    return 0;
}
static int find_index(int fin,int fout,int *index,int *finEQfout)
{
	*finEQfout=0;
	switch(fin)
	{
		case ESAA_FREQ_16:
			switch(fout)
			{
				case ESAA_FREQ_16:
					*finEQfout=1;
					*index=0;
					break;
				case ESAA_FREQ_48:
					*finEQfout=0;
					*index=1;
					break;
				default:
					return 1;
			}
			break;
		case ESAA_FREQ_48:
			switch(fout)
			{
				case ESAA_FREQ_16:
					*finEQfout=0;
					*index=2;
					break;
				case ESAA_FREQ_48:
					*finEQfout=1;
					*index=3;
					break;
				default:
					return 1;
			}
			break;
		default:
			return 1; 
	}
	return 0;
}
#endif //ifdef M4_LL_MSEC

