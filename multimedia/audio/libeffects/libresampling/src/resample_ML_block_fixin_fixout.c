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
#ifndef M4_LL_MSEC
static int inline
resample_calc_nbIn_fixin_fixout(int               *nbIn,
                                unsigned int      nbOut,
                                ResampleDelayLine *delayLine,
                                ResampleFilter    *filterInfo);
static int inline
resample_calc_nbOut_fixin_fixout(unsigned int      nbIn,
                                 unsigned int      *nbOut,
                                 ResampleDelayLine *delayLine,
                                 ResampleFilter    *filterInfo);
static void
resample_update_flush(int *flush,int *nSamples,int nbOutSamples, ResampleContext *ctx);
static void
resample_update_flush_msec(int *flush,int *nSamples,int nbOutSamples, ResampleContext *ctx);


void samples2msec_8_8khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 3;
    *n_samples_out_1ms = n_samples_out >> 3;
}

void samples2msec_8_16khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 3;
    *n_samples_out_1ms = n_samples_out >> 4;
}

void samples2msec_8_48khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 3;
    *n_samples_out_1ms = n_samples_out / 48;
}

void samples2msec_16_8khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 4;
    *n_samples_out_1ms = n_samples_out >> 3;
}

void samples2msec_16_16khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 4;
    *n_samples_out_1ms = n_samples_out >> 4;
}

void samples2msec_16_48khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  >> 4;
    *n_samples_out_1ms = n_samples_out / 48;
}

void samples2msec_48_8khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  / 48;
    *n_samples_out_1ms = n_samples_out >> 3;
}

void samples2msec_48_16khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  / 48;
    *n_samples_out_1ms = n_samples_out >> 4;
}

void samples2msec_48_48khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms)
{
    *n_samples_in_1ms  = n_samples_in  / 48;
    *n_samples_out_1ms = n_samples_out / 48;
}


static void inline
resample_ML_block_fixin_fixout_copy_inline(ResampleContext *ctx,
                                           int             *in,
                                           int             istride,
                                           int             *nSamples,
                                           int             channel,
                                           int             *out,
                                           int             *nbOutSamples,
                                           int             *flush)
{
 	int j,k;
	int num;
    if ((*nSamples!=0)&&(*nbOutSamples!=0)&&(*nbOutSamples<=(int)ctx->block_siz))
    {
        ctx->block_siz=(int)*nbOutSamples;
    }
    
    if (*flush!=0)
    {
        *nSamples=(int)ctx->block_siz;
    }

    /*--------------------------- CASE OF FIN=FOUT -------------------------------------------------*/
    if (((*nSamples==0)||(*nbOutSamples==0)||(*nbOutSamples>(int)ctx->block_siz)))
    {
        *nbOutSamples=0;
        *nSamples=0;
    }
    else
    {
        k=0;
        num=(int)ctx->block_siz;
        for (j=0;j<num;j++)
        {
            out[k]=in[k];
            k+=istride;
        }
    }

    resample_update_flush(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_up_inline(ResampleContext *ctx,
                                         int             *in,
                                         int             istride,
                                         int             *nSamples,
                                         int             channel,
                                         int             *out,
                                         int             *nbOutSamples,
                                         int             *flush)
{
 	int nbIn=0,nbremain=0;
	unsigned int nbOut=0;
	unsigned int loop_siz[RESAMPLE_MAX_CASCADE];
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
 	unsigned int nbOutRequested[RESAMPLE_MAX_CASCADE];
 	int j,k;
 	int remaining_filter = ctx->maxCascade-1;
 	int old = 0;
 	int new;
 	int *tempOut[2];
 	int *tempOut1;  
 	int *tempOut2;
	int predicted_final_offset[RESAMPLE_MAX_CASCADE];
	void *in_savptr;
	in_savptr=in;
	tempOut1=ctx->tempbuf1;
	tempOut2=ctx->tempbuf2;
	tempOut[0] = tempOut1;
	tempOut[1] = tempOut2;
    
    if ((*nbOutSamples!=0)&&(*nbOutSamples<=(int)ctx->block_siz))
    {
        if (*nbOutSamples!=(int)ctx->block_siz)
        {
            ctx->block_siz=(int)*nbOutSamples;
        }
    }

    if (*flush!=0)
    {
        *nSamples=(int)ctx->block_siz;
    }

    /*------------------------- UPSAMPLING CASE ---------------------------------------*/
    if (((*nbOutSamples==0)||(*nbOutSamples>(int)ctx->block_siz)))
    {
        *nbOutSamples=0;
        *nSamples=0;
    }
    else
    {
        ctxChan->delay[0]->totalen=ctxChan->delay[0]->len+ctx->block_siz+1;
        *nbOutSamples=ctx->block_siz;
        nbremain=resample_ML_read_block_fixin_fixout(ctxChan->delay[0]);

        /*------------------- CASCADED STAGES ---------------------------------*/
        nbIn=ctx->block_siz;
        for (k=ctx->maxCascade-1;k>=0;k--)
        {
            nbOutRequested[k]=nbIn;
            predicted_final_offset[k]=resample_calc_nbIn_fixin_fixout(&nbIn,nbIn,ctxChan->delay[k],ctx->filterInfo[k]);
            loop_siz[k]=nbOutRequested[k]+nbIn;
        }
        if ((nbIn-nbremain)>(int)ctx->block_siz) //for block_siz=1, sometimes need 2 samples. so load 1 sample in dlyline & exit
        {
            if (*nSamples==(int)ctx->block_siz)
            {
                *nbOutSamples=0;
                *nSamples=ctx->block_siz;
                resample_ML_load_block_fixin_fixout(in,istride,*nSamples,ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);
				in_savptr=(void*)(((char*)in_savptr)+(*nSamples)*istride*(ctx->sample_size));

            }
            else
            {
                *nbOutSamples=0;
                *nSamples=0;
            }
        }
        else if ((*nSamples!=(int)ctx->block_siz)&&(nbremain<nbIn))
        {
            *nbOutSamples=0;
            *nSamples=0;
        }
        else if (ctx->low_latency==0)
        {
            if (nbremain>=nbIn)
            {
                /*----- 1ST STAGE DELAY LINE HAS ENOUGH SAMPLES TO CALCULATE 1ST STAGE OUTPUT WITHOUT UPDATING IT -----*/
                *nSamples=0;
                /*----------------- CALCULATE 1ST STAGE OUPUT TO TEMPOUT -----------------------*/
                resample_ML_conv_block_fixin_fixout(tempOut[old], 1, nbOutRequested[0],&nbOut,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],loop_siz[0]
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
            }
            else
            {
                /*--------------- UPDATE 1ST STAGE DELAY LINE -------------------------------------------------*/
                *nSamples=ctx->block_siz;
                resample_ML_load_block_fixin_fixout(in,istride,nbIn-nbremain,
                        ctxChan->delay[0],ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);
				in_savptr=(void*)(((char*)in_savptr)+(nbIn-nbremain)*istride*(ctx->sample_size));

                /*------------- CALCULATE 1ST STAGE OUTPUT TO TEMPOUT -----------------------------*/
                resample_ML_conv_block_fixin_fixout(tempOut[old],1,nbOutRequested[0],&nbOut,ctxChan->delay[0],
                        ctx->filterInfo[0],nbIn+nbOutRequested[0]
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                /*------------- UPDATE 1ST STAGE DELAY LINE WITH REST OF INPUT BLOCK --------------------------*/
                resample_ML_load_block_fixin_fixout(in_savptr,istride,ctx->block_siz-nbIn+nbremain,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);

                nbOut=nbOutRequested[0];
            }
            /*------------ CALCULATE INTERMEDIATE STAGES UPDATING AND OUTPUTING TO TEMPOUT --------*/
            for(j=1;j<remaining_filter;j++) 
            {
                new = !old; // toggle
                resample_ML_filter_block_fixin_fixout(tempOut[old],1, nbOut,
                        tempOut[new], 1, nbOutRequested[j],&nbOut,
                        ctxChan->delay[j],
                        ctx->filterInfo[j],loop_siz[j]
                        ,ctx->CoefRam[j],0
                        );
                ctxChan->delay[j]->offset=predicted_final_offset[j];
                old = new;
            }
            /*------------ CALCULATE OUTPUT AFTER UPDATING FINAL STAGE DELAY LINE -----------*/
            resample_ML_filter_block_fixin_fixout(tempOut[old],1, nbOut,
                    out, istride, nbOutRequested[remaining_filter],&nbOut,
                    ctxChan->delay[remaining_filter],
                    ctx->filterInfo[remaining_filter],loop_siz[remaining_filter]
                    ,ctx->CoefRam[remaining_filter],0
                    );
        }
        else
        {
            if (nbremain>=nbIn)
            {
                /*----- 1ST STAGE DELAY LINE HAS ENOUGH SAMPLES TO CALCULATE 1ST STAGE OUTPUT WITHOUT UPDATING IT -----*/
                *nSamples=0;
                /*----------------- CALCULATE 1ST STAGE OUPUT TO TEMPOUT -----------------------*/
#if defined(ARM)||defined(NEON)
                resample_ML_conv_block_fixin_fixout
#else
				resample_ML_conv_block_fixin_fixout_ll
#endif
					(
						tempOut[old], 1, nbOutRequested[0],&nbOut, //here
                        ctxChan->delay[0],
                        ctx->filterInfo[0],loop_siz[0]
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
            }
            else
            {
                /*--------------- UPDATE 1ST STAGE DELAY LINE -------------------------------------------------*/
                *nSamples=ctx->block_siz;
                resample_ML_load_block_fixin_fixout(in,istride,nbIn-nbremain,
                        ctxChan->delay[0],ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);
				in_savptr=(void*)(((char*)in_savptr)+(nbIn-nbremain)*istride*(ctx->sample_size));

                /*------------- CALCULATE 1ST STAGE OUTPUT TO TEMPOUT -----------------------------*/
#if defined(ARM)||defined(NEON)
				resample_ML_conv_block_fixin_fixout
#else
				resample_ML_conv_block_fixin_fixout_ll
#endif
					(tempOut[old],1,nbOutRequested[0],&nbOut,ctxChan->delay[0], //here
                        ctx->filterInfo[0],nbIn+nbOutRequested[0]
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                /*------------- UPDATE 1ST STAGE DELAY LINE WITH REST OF INPUT BLOCK --------------------------*/
				resample_ML_load_block_fixin_fixout(in_savptr,istride,ctx->block_siz-nbIn+nbremain,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);

                nbOut=nbOutRequested[0];
            }
            /*------------ CALCULATE INTERMEDIATE STAGES UPDATING AND OUTPUTING TO TEMPOUT --------*/
            for(j=1;j<remaining_filter;j++) 
            {
                new = !old; // toggle
#if defined(ARM)||defined(NEON)
                resample_ML_filter_block_fixin_fixout
#else
                resample_ML_filter_block_fixin_fixout_ll
#endif
					(tempOut[old],1, nbOut,  //here
                        tempOut[new], 1, nbOutRequested[j],&nbOut,
                        ctxChan->delay[j],
                        ctx->filterInfo[j],loop_siz[j]
                        ,ctx->CoefRam[j],
                        0
                        );
                ctxChan->delay[j]->offset=predicted_final_offset[j];
                old = new;
            }
            /*------------ CALCULATE OUTPUT AFTER UPDATING FINAL STAGE DELAY LINE -----------*/
#if defined(ARM)||defined(NEON)
            resample_ML_filter_block_fixin_fixout
#else
			resample_ML_filter_block_fixin_fixout_ll
#endif
				(tempOut[old],1, nbOut,  //here
                    out, istride, nbOutRequested[remaining_filter],&nbOut,
                    ctxChan->delay[remaining_filter],
                    ctx->filterInfo[remaining_filter],loop_siz[remaining_filter]
                    ,ctx->CoefRam[remaining_filter],
                    0
                    );
        }
        ctxChan->delay[remaining_filter]->offset=predicted_final_offset[remaining_filter];
    }

	resample_update_flush(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_up_maxCasc1_inline(ResampleContext *ctx,
                                                  int             *in,
                                                  int             istride,
                                                  int             *nSamples,
                                                  int             channel,
                                                  int             *out,
                                                  int             *nbOutSamples,
                                                  int             *flush)
{
 	int nbIn=0,nbremain=0;
	unsigned int nbOut=0;
	void *in_savptr;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
	int predicted_final_offset[RESAMPLE_MAX_CASCADE];
	in_savptr=in;
 
    if ((*nbOutSamples!=0)&&(*nbOutSamples<=(int)ctx->block_siz))
    {
        if (*nbOutSamples!=(int)ctx->block_siz)
        {
            ctx->block_siz=(int)*nbOutSamples;
        }
    }

    if (*flush!=0)
    {
        *nSamples=(int)ctx->block_siz;
    }

    /*------------------------- UPSAMPLING CASE ---------------------------------------*/
    if (((*nbOutSamples==0)||(*nbOutSamples>(int)ctx->block_siz)))
    {
        *nbOutSamples=0;
        *nSamples=0;
    }
    else
    {
        ctxChan->delay[0]->totalen=ctxChan->delay[0]->len+ctx->block_siz+1;
        *nbOutSamples=ctx->block_siz;
        nbremain=resample_ML_read_block_fixin_fixout(ctxChan->delay[0]);

        /*------------------- ONLY ONE STAGE ----------------------------------------*/
        predicted_final_offset[0]=resample_calc_nbIn_fixin_fixout(&nbIn,ctx->block_siz,ctxChan->delay[0],ctx->filterInfo[0]);
        if (ctx->low_latency==0)
        {
            if (nbremain>=nbIn)
            {
                /*----- DELAY LINE HAS ENOUGH SAMPLES TO CALCULATE OUTPUT BUFFER WITHOUT UPDATING IT -----*/
                /*----------------- CALCULATE OUPUT SAMPLES --------------------------------------------*/
                resample_ML_conv_block_fixin_fixout(out,istride,ctx->block_siz,&nbOut,ctxChan->delay[0],
                        ctx->filterInfo[0],nbIn+ctx->block_siz
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                *nSamples=0;
            }
            else
            {
                /*------- NEED TO UPDATE DELAY LINE BEFORE CALCULATING OUTPUT-------------------------*/
                if (*nSamples!=(int)ctx->block_siz)
                {
                    *nbOutSamples=0;
                    *nSamples=0;
                }
                else
                {
                    /*--------------- UPDATE DELAY LINE --------------------------------------------------------------*/
                    resample_ML_load_block_fixin_fixout(in,istride,nbIn-nbremain,ctxChan->delay[0],ctx->filterInfo[0],
							ctx->flush_squelch/*,ctx->low_latency*/);
					in_savptr=(void*)(((char*)in_savptr)+(nbIn-nbremain)*istride*(ctx->sample_size));
                    /*----------- CALCULATE OUTPUT SAMPLES -----------------------------------------------*/
                    resample_ML_conv_block_fixin_fixout(out,istride,ctx->block_siz,&nbOut,ctxChan->delay[0],
                            ctx->filterInfo[0],nbIn+ctx->block_siz
                            ,ctx->CoefRam[0]
                            );
                    ctxChan->delay[0]->offset=predicted_final_offset[0];
                    /*------------- UPDATE DELAY LINE WITH REST OF INPUT BLOCK --------------------------*/
					resample_ML_load_block_fixin_fixout(in_savptr,istride,
                            ctx->block_siz-nbIn+nbremain,ctxChan->delay[0],
                            ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);
                    *nSamples=ctx->block_siz;
                }
            }
        }
        else
        {
            if (nbremain>=nbIn)
            {
                /*----- DELAY LINE HAS ENOUGH SAMPLES TO CALCULATE OUTPUT BUFFER WITHOUT UPDATING IT -----*/
                /*----------------- CALCULATE OUPUT SAMPLES --------------------------------------------*/
#if defined(ARM)||defined(NEON)
                resample_ML_conv_block_fixin_fixout
#else
                resample_ML_conv_block_fixin_fixout_ll
#endif
					(out,istride,ctx->block_siz,&nbOut,ctxChan->delay[0],  //here
                        ctx->filterInfo[0],nbIn+ctx->block_siz
                        ,ctx->CoefRam[0]
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                *nSamples=0;
            }
            else
            {
                /*------- NEED TO UPDATE DELAY LINE BEFORE CALCULATING OUTPUT-------------------------*/
                if (*nSamples!=(int)ctx->block_siz)
                {
                    *nbOutSamples=0;
                    *nSamples=0;
                }
                else
                {
                    /*--------------- UPDATE DELAY LINE --------------------------------------------------------------*/
                    resample_ML_load_block_fixin_fixout(in,istride,nbIn-nbremain,ctxChan->delay[0],ctx->filterInfo[0],
							ctx->flush_squelch/*,ctx->low_latency*/);
					in_savptr=(void*)(((char*)in_savptr)+(nbIn-nbremain)*istride*(ctx->sample_size));

                    /*----------- CALCULATE OUTPUT SAMPLES -----------------------------------------------*/
#if defined(ARM)||defined(NEON)
                    resample_ML_conv_block_fixin_fixout
#else
                    resample_ML_conv_block_fixin_fixout_ll
#endif
						(out,istride,ctx->block_siz,&nbOut,ctxChan->delay[0],  //here
                            ctx->filterInfo[0],nbIn+ctx->block_siz
                            ,ctx->CoefRam[0]
                            );
                    ctxChan->delay[0]->offset=predicted_final_offset[0];
                    /*------------- UPDATE DELAY LINE WITH REST OF INPUT BLOCK --------------------------*/
                    resample_ML_load_block_fixin_fixout(in_savptr,istride,
                            ctx->block_siz-nbIn+nbremain,ctxChan->delay[0],
                            ctx->filterInfo[0],ctx->flush_squelch/*,ctx->low_latency*/);
                    *nSamples=ctx->block_siz;
                }
            }
        }
    } //*nbOutSamples!=(int)ctx->block_siz

	resample_update_flush(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_down_inline(ResampleContext *ctx,
                                           int             *in,
                                           int             istride,
                                           int             *nSamples,
                                           int             channel,
                                           int             *out,
                                           int             *nbOutSamples,
                                           int             *flush)
{
 	int nbIn=0,nbremain=0;
	unsigned int nbOut=0,nbtocopy;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
 	unsigned int nbOutRequested[RESAMPLE_MAX_CASCADE],NbIn;
 	int j,k;
 	int remaining_filter = ctx->maxCascade-1;
 	int old = 0;
 	int new;
 	int *tempOut[2];
 	int *tempOut1;  
 	int *tempOut2;
	void *out_savptr;
	int predicted_final_offset[RESAMPLE_MAX_CASCADE];
	tempOut1=ctx->tempbuf1;
	tempOut2=ctx->tempbuf2;
	tempOut[0] = tempOut1;
	tempOut[1] = tempOut2;
   	out_savptr=out; 
    if ((*nSamples!=0)&&(*nSamples<=(int)ctx->block_siz))
    {
        if (*nSamples!=(int)ctx->block_siz)
        {
            ctx->block_siz=(int)*nSamples;
        }
    }

    if (*flush!=0)
    {
        *nSamples=(int)ctx->block_siz;
    }

    /*-------------------- DOWNSAMPLING CASE -------------------------------*/
    if (  ((*nSamples==0)||(*nSamples>(int)ctx->block_siz)))
    {
        *nSamples=0;
        *nbOutSamples=0;
    }
    else
    {
        *nSamples=ctx->block_siz;

        /*-------------- CASCADED STAGES -----------------------------*/
        nbIn=ctx->block_siz;
        nbremain=ctxChan->Resampleremainbuf->nbremain;
        for (k=0;k<(int)ctx->maxCascade;k++)
        {
            predicted_final_offset[k]=resample_calc_nbOut_fixin_fixout(nbIn,&nbOutRequested[k],ctxChan->delay[k],ctx->filterInfo[k]);
            nbIn=nbOutRequested[k];
        }
        if (((nbOutRequested[ctx->maxCascade-1]+(unsigned int)nbremain)>=ctx->block_siz)&&(*nbOutSamples!=(int)ctx->block_siz))
        {
            *nbOutSamples=0;
            *nSamples=0;
        }
        else 
        {
            if ((nbOutRequested[ctx->maxCascade-1]+(unsigned int)nbremain)>=ctx->block_siz)
            {
                /*---------- REMAINBUF WILL HAVE ENOUGH SAMPLES TO OUTPUT 1 BLOCK -----*/
                /*---------- EMPTY REMAINBUF TO OUTPUT --------------------------------*/
                resample_ML_store_block_fixin_fixout(ctxChan->Resampleremainbuf,
                        out,istride,nbremain,ctx->block_siz,ctx->ARM_Sample16);
				out_savptr=(void*)(((char*)out_savptr)+nbremain*istride*(ctx->sample_size));

            }
            
            if (ctx->low_latency==0)
            {
                /*----------- UPDATE DELAY LINE AND OUTPUT SAMPLES TO TEMPOUT ----*/
                resample_ML_filter_block_fixin_fixout(in,istride, ctx->block_siz,
                        tempOut[old], 1, nbOutRequested[0],&nbOut,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->block_siz+nbOutRequested[0]
                        ,ctx->CoefRam[0],0
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                /*----------- UPDATE DELAY LINE AND OUTPUT SAMPLES TO TEMPOUT INTERMEDIATE STAGES ----*/
                for(j=1;j<remaining_filter;j++) 
                {
                    new = !old; // toggle
                    resample_ML_filter_block_fixin_fixout(tempOut[old],1, nbOut,
                            tempOut[new], 1, nbOutRequested[j],&nbOut,
                            ctxChan->delay[j],
                            ctx->filterInfo[j],nbOut+nbOutRequested[j]
                            ,ctx->CoefRam[j],0
                            );
                    ctxChan->delay[j]->offset=predicted_final_offset[j];
                    old = new;
                }
                if (ctx->flush_pending!=0)
                    NbIn=0;
                else
                    NbIn=nbOut;
                /*----------- UPDATE LAST STAGE DELAY LINE AND OUTPUT SAMPLES TO REMAINBUF ----*/
                resample_ML_convout_block_fixin_fixout(tempOut[old],1, NbIn,
                        ctxChan->Resampleremainbuf,
                        nbOutRequested[remaining_filter],&nbOut,
                        ctxChan->delay[remaining_filter],
                        ctx->filterInfo[remaining_filter],
                        nbOut+nbOutRequested[remaining_filter],ctx->block_siz
                        ,ctx->CoefRam[remaining_filter]
                        );
            }
            else
            {
                /*----------- UPDATE DELAY LINE AND OUTPUT SAMPLES TO TEMPOUT ----*/
#if defined(ARM)||defined(NEON)
                resample_ML_filter_block_fixin_fixout
#else
                resample_ML_filter_block_fixin_fixout_ll
#endif
					(in,istride, ctx->block_siz, //here
                        tempOut[old], 1, nbOutRequested[0],&nbOut,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->block_siz+nbOutRequested[0]
                        ,ctx->CoefRam[0],
                        0
                        );
                ctxChan->delay[0]->offset=predicted_final_offset[0];
                /*----------- UPDATE DELAY LINE AND OUTPUT SAMPLES TO TEMPOUT INTERMEDIATE STAGES ----*/
                for(j=1;j<remaining_filter;j++) 
                {
                    new = !old; // toggle
#if defined(ARM)||defined(NEON)
                    resample_ML_filter_block_fixin_fixout
#else
                    resample_ML_filter_block_fixin_fixout_ll
#endif
						(tempOut[old],1, nbOut, //here
                            tempOut[new], 1, nbOutRequested[j],&nbOut,
                            ctxChan->delay[j],
                            ctx->filterInfo[j],nbOut+nbOutRequested[j]
                            ,ctx->CoefRam[j],
                            0
                            );
                    ctxChan->delay[j]->offset=predicted_final_offset[j];
                    old = new;
                }
                if (ctx->flush_pending!=0)
                    NbIn=0;
                else
                    NbIn=nbOut;
                /*----------- UPDATE LAST STAGE DELAY LINE AND OUTPUT SAMPLES TO REMAINBUF ----*/
#if defined(ARM)||defined(NEON)
                resample_ML_convout_block_fixin_fixout
#else
				resample_ML_convout_block_fixin_fixout_ll
#endif
					(tempOut[old],1, NbIn,  //here
                        ctxChan->Resampleremainbuf,
                        nbOutRequested[remaining_filter],
                        &nbOut,
                        ctxChan->delay[remaining_filter],
                        ctx->filterInfo[remaining_filter],
                        nbOut+nbOutRequested[remaining_filter],ctx->block_siz
                        ,ctx->CoefRam[remaining_filter]
                        );
            }
            
            ctxChan->delay[remaining_filter]->offset=predicted_final_offset[remaining_filter];
            if ((nbOutRequested[ctx->maxCascade-1]+(unsigned int)nbremain)>=ctx->block_siz)
            {
                /*---------- REMAINBUF IF IT HAD ENUF SAMPLES FROM TEST ABOVE TO REST OF OUTPUT -----------*/
                nbtocopy=ctx->block_siz-nbremain;
                resample_ML_store_block_fixin_fixout(ctxChan->Resampleremainbuf,
                        (int*)out_savptr,istride,nbtocopy,ctx->block_siz,ctx->ARM_Sample16);
                *nbOutSamples=ctx->block_siz;
            }
            else
            {
                *nbOutSamples=0;
            }
        }//(((nout+(unsigned int)nbremain)>=ctx->block_siz)&&(*nbOutSamples!=(int)ctx->block_siz))
    }//*nSamples!=(int)ctx->block_siz

	resample_update_flush(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_down_maxCasc1_inline(ResampleContext *ctx,
                                                    int             *in,
                                                    int             istride,
                                                    int             *nSamples,
                                                    int             channel,
                                                    int             *out,
                                                    int             *nbOutSamples,
                                                    int             *flush)
{
 	int nbremain=0;
	unsigned int nbOut=0,nout,nbtocopy;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
 	unsigned int NbIn;
	void *out_savptr;
	int predicted_final_offset[RESAMPLE_MAX_CASCADE];
    
    if ((*nSamples!=0)&&(*nSamples<=(int)ctx->block_siz))
    {
        if (*nSamples!=(int)ctx->block_siz)
        {
            ctx->block_siz=(int)*nSamples;
        }
    }

    if (*flush!=0)
    {
        *nSamples=(int)ctx->block_siz;
    }
	out_savptr=out;
    /*-------------------- DOWNSAMPLING CASE -------------------------------*/
    if (  ((*nSamples==0)||(*nSamples>(int)ctx->block_siz)))
    {
        *nSamples=0;
        *nbOutSamples=0;
    }
    else
    {
        *nSamples=ctx->block_siz;
        
        /*-------------------- ONLY ONE STAGE -----------------------------*/
        nbremain=ctxChan->Resampleremainbuf->nbremain;
        predicted_final_offset[0]=resample_calc_nbOut_fixin_fixout(ctx->block_siz,&nout,ctxChan->delay[0],ctx->filterInfo[0]);
        if (((nout+(unsigned int)nbremain)>=ctx->block_siz)&&(*nbOutSamples!=(int)ctx->block_siz))
        {
            *nbOutSamples=0;
            *nSamples=0;
        }
        else
        {
            if ((nout+(unsigned int)nbremain)>=ctx->block_siz)
            {
                /*---------- REMAINBUF WILL HAVE ENOUGH SAMPLES TO OUTPUT 1 BLOCK -----*/
                /*---------- EMPTY REMAINBUF TO OUTPUT --------------------------------*/
                resample_ML_store_block_fixin_fixout(ctxChan->Resampleremainbuf,
                        out,istride,nbremain,ctx->block_siz,ctx->ARM_Sample16);
				out_savptr=(void*)(((char*)out_savptr)+nbremain*istride*(ctx->sample_size));
				

            }
            if (ctx->flush_pending!=0)
                NbIn=0;
            else
                NbIn=ctx->block_siz;
            /*----------- UPDATE DELAY LINE AND OUTPUT SAMPLES TO REMAINBUF ----*/
            if (ctx->low_latency==0)
            {
                resample_ML_convout_block_fixin_fixout(in,istride,NbIn,
                        ctxChan->Resampleremainbuf,
                        nout,&nbOut,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->block_siz+nout,ctx->block_siz
                        ,ctx->CoefRam[0]
                        );
            }
            else
            {
#if defined(ARM)||defined(NEON)
                resample_ML_convout_block_fixin_fixout
#else
                resample_ML_convout_block_fixin_fixout_ll
#endif
					(in,istride,NbIn, //here
                        ctxChan->Resampleremainbuf,
                        nout,&nbOut,
                        ctxChan->delay[0],
                        ctx->filterInfo[0],ctx->block_siz+nout,ctx->block_siz
                        ,ctx->CoefRam[0]
                        );
            }
            ctxChan->delay[0]->offset=predicted_final_offset[0];
            if ((nout+(unsigned int)nbremain)>=ctx->block_siz)
            {
                /*---------- REMAINBUF TO REST OF OUTPUT IF ENUF SAMPLES AS FROM TEST ABOVE ------*/
                nbtocopy=ctx->block_siz-nbremain;
                resample_ML_store_block_fixin_fixout(ctxChan->Resampleremainbuf,
                   	(int*)out_savptr,istride,nbtocopy,ctx->block_siz,ctx->ARM_Sample16);
                	*nbOutSamples=ctx->block_siz;
					
            }
            else
            {
                *nbOutSamples=0;
            }
        }
    }//*nSamples!=(int)ctx->block_siz

	resample_update_flush(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_msec_copy_inline(ResampleContext *ctx,
                                                int             *in,
                                                int             istride,
                                                int             *nSamples,
                                                int             channel,
                                                int             *out,
                                                int             *nbOutSamples,
                                                int             *flush)
{
 	int j,k;
	int num;
    
    /*--------------------------- CASE OF FIN=FOUT -------------------------------------------------*/
    k=0;
    num=*nSamples;
    for (j=0;j<num;j++)
    {
        out[k]=in[k];
        k+=istride;
    }

    resample_update_flush_msec(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_msec_up_inline(ResampleContext *ctx,
                                              int             *in,
                                              int             istride,
                                              int             *nSamples,
                                              int             channel,
                                              int             *out,
                                              int             *nbOutSamples,
                                              int             *flush)
{
 	int nbIn=0;
	unsigned int nbOut=0;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
 	int k;
	int  noutsmpls_generated,total_noutsmpls_generated;
	int squelch;
	void *void_inptr,*void_outptr,*void_ptrsav;

    /*------------------------- UPSAMPLING CASE ---------------------------------------*/
    ctxChan->delay[0]->totalen=ctxChan->delay[0]->len;

    /*------------------- CASCADED STAGES ---------------------------------*/
    nbIn=ctx->block_siz;
    if (*flush!=0) 
        *nSamples=ctx->block_siz;
    else
        ctx->block_siz=*nSamples;
    nbIn=*nSamples;
    total_noutsmpls_generated=*nSamples;
    for (k=0;k<(int)ctx->maxCascade;k++)//calculate total number of output samples 
        total_noutsmpls_generated*=ctx->filterInfo[k]->M/ctx->filterInfo[k]->L;
	void_inptr=in; //1st stage input from exterior

    for (k=0;k<(int)ctx->maxCascade;k++)
    {
        squelch=0;
        if (k==0) squelch=*flush; 
        noutsmpls_generated=*nSamples*ctx->filterInfo[k]->M/ctx->filterInfo[k]->L; //noutsmpls will be generated this stage
        //-------- intermediate stages put samples to output buffer as the no of samples is always increasing
		void_outptr=(void*)(((char*)out)+(istride*total_noutsmpls_generated - istride*noutsmpls_generated)*(ctx->sample_size));
		void_ptrsav=void_outptr;
        if (k==((int)ctx->maxCascade-1)) /*outptr=out*/void_outptr=out; //last stage presents samples to the exterior

#if defined(ARM)||defined(NEON)
        resample_ML_filter_block_fixin_fixout
#else
        resample_ML_filter_block_fixin_fixout_ll
#endif
			(  //here
				void_inptr,
                istride, 
                *nSamples, //nbIn
				void_outptr,
                istride, //ostride
                noutsmpls_generated, //nbOutRequested
                &nbOut, //*nbOutAvail
                ctxChan->delay[k], //*delayLine
                ctx->filterInfo[k], //*filterInfo
                noutsmpls_generated+*nSamples, //loop_siz
                ctx->CoefRam[k],
                squelch
                );
        *nSamples=nbOut;
		void_inptr=void_ptrsav; //next stage takes input from current stage at the correct address
    }
    *nbOutSamples=nbOut;
    *nSamples=nbIn;
	
    resample_update_flush_msec(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_msec_up_maxCasc1_inline(ResampleContext *ctx,
                                                       int             *in,
                                                       int             istride,
                                                       int             *nSamples,
                                                       int             channel,
                                                       int             *out,
                                                       int             *nbOutSamples,
                                                       int             *flush)
{
	unsigned int nbOut=0;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
    
    /*------------------------- UPSAMPLING CASE ---------------------------------------*/
    ctxChan->delay[0]->totalen=ctxChan->delay[0]->len;

    /*------------------- ONLY ONE STAGE ----------------------------------------*/
    if (*flush!=0) 
        *nSamples=ctx->block_siz;
    else
        ctx->block_siz=*nSamples;
#if defined(ARM)||defined(NEON)
    resample_ML_filter_block_fixin_fixout
#else
    resample_ML_filter_block_fixin_fixout_ll
#endif
		( //here
            in,
            istride, 
            *nSamples, //nbIn
            out, 
            istride, //ostride
            *nSamples*ctx->filterInfo[0]->M/ctx->filterInfo[0]->L, //nbOutRequested
            &nbOut, //*nbOutAvail
            ctxChan->delay[0], //*delayLine
            ctx->filterInfo[0], //*filterInfo
            *nSamples*ctx->filterInfo[0]->M/ctx->filterInfo[0]->L+*nSamples, //loop_siz
            ctx->CoefRam[0],
            *flush
            );
    *nbOutSamples=nbOut;
	
    resample_update_flush_msec(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_msec_down_inline(ResampleContext *ctx,
                                                int             *in,
                                                int             istride,
                                                int             *nSamples,
                                                int             channel,
                                                int             *out,
                                                int             *nbOutSamples,
                                                int             *flush)
{
 	int nbIn=0;
	unsigned int nbOut=0;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
 	int k;
	int *inptr,*outptr,noutsmpls_generated;
	int squelch;
    
    /*-------------- CASCADED STAGES -----------------------------*/
    nbIn=ctx->block_siz;
    if (*flush!=0) 
        *nSamples=ctx->block_siz;
    else
        ctx->block_siz=*nSamples;
    nbIn=*nSamples;
    inptr=in; 
    outptr=in; //intermediate stages work on in buffer as the nsamples always decreases
    for (k=0;k<(int)ctx->maxCascade;k++) 
    {
        squelch=0;
        if (k==0) squelch=*flush;
        if (k==((int)ctx->maxCascade-1)) outptr=out; //last stage put samples in output
        noutsmpls_generated=*nSamples*ctx->filterInfo[k]->M/ctx->filterInfo[k]->L; //noutsmpls will be generated this stage
#if defined(ARM)||defined(NEON)
        resample_ML_filter_block_fixin_fixout
#else
        resample_ML_filter_block_fixin_fixout_ll
#endif
			( //here
            inptr,
            istride, 
            *nSamples, //nbIn
            outptr, 
            istride, //ostride
            noutsmpls_generated, //nbOutRequested
            &nbOut, //*nbOutAvail
            ctxChan->delay[k], //*delayLine
            ctx->filterInfo[k], //*filterInfo
            noutsmpls_generated+*nSamples, //loop_siz
            ctx->CoefRam[k],
            squelch
            );
        *nSamples=nbOut;
    }
    *nbOutSamples=nbOut;
    *nSamples=nbIn;

	resample_update_flush_msec(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */


static void inline
resample_ML_block_fixin_fixout_msec_down_maxCasc1_inline(ResampleContext *ctx,
                                                         int             *in,
                                                         int             istride,
                                                         int             *nSamples,
                                                         int             channel,
                                                         int             *out,
                                                         int             *nbOutSamples,
                                                         int             *flush)
{
	unsigned int nbOut=0;
	ResampleContextChannel *ctxChan = &ctx->ctxChan[channel];
    
    /*-------------------- ONLY ONE STAGE -----------------------------*/
    if (*flush!=0) 
        *nSamples=ctx->block_siz;
    else
        ctx->block_siz=*nSamples;
#if defined(ARM)||defined(NEON)
    resample_ML_filter_block_fixin_fixout
#else
    resample_ML_filter_block_fixin_fixout_ll
#endif
		(  //here
            in,
            istride, 
            *nSamples, //nbIn
            out, 
            istride, //ostride
            *nSamples*ctx->filterInfo[0]->M/ctx->filterInfo[0]->L, //nbOutRequested
            &nbOut, //*nbOutAvail
            ctxChan->delay[0], //*delayLine
            ctx->filterInfo[0], //*filterInfo
            *nSamples*ctx->filterInfo[0]->M/ctx->filterInfo[0]->L+*nSamples, //loop_siz
            ctx->CoefRam[0],
            *flush
            );
    *nbOutSamples=nbOut;

	resample_update_flush_msec(flush,nSamples,*nbOutSamples,ctx);
} /* end resample_BLK() */



int
resample_ML_block_fixin_fixout_copy(ResampleContext *ctx,
                                    int             *in,
                                    int             nb_channels,
                                    int             *nSamples,
                                    int             *out,
                                    int             *nbOutSamples,
                                    int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    
    flush_in               = *flush;
    nbin_in                = *nSamples;
    nbout_in               = *nbOutSamples;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;
    
    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            *nSamples          = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            *nbOutSamples = nbout_in;
            *flush        = flush_in;
            resample_ML_block_fixin_fixout_copy_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       nbOutSamples, 
                                                       flush);
            if(*nSamples > nbSamples_out)
                nbSamples_out = *nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(*nbOutSamples != 0)
            {
                *nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_up(ResampleContext *ctx,
                                  int             *in,
                                  int             nb_channels,
                                  int             *nSamples,
                                  int             *out,
                                  int             *nbOutSamples,
                                  int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    
    flush_in               = *flush;
    nbin_in                = *nSamples;
    nbout_in               = *nbOutSamples;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;
    
    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            *nSamples          = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            *nbOutSamples = nbout_in;
            *flush        = flush_in;
            resample_ML_block_fixin_fixout_up_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       nbOutSamples, 
                                                       flush);
            if(*nSamples > nbSamples_out)
                nbSamples_out = *nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(*nbOutSamples != 0)
            {
                *nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_up_maxCasc1(ResampleContext *ctx,
                                           int             *in,
                                           int             nb_channels,
                                           int             *nSamples,
                                           int             *out,
                                           int             *nbOutSamples,
                                           int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    
    flush_in               = *flush;
    nbin_in                = *nSamples;
    nbout_in               = *nbOutSamples;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;
    
    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            *nSamples          = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            *nbOutSamples = nbout_in;
            *flush        = flush_in;
            resample_ML_block_fixin_fixout_up_maxCasc1_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       nbOutSamples, 
                                                       flush);
            if(*nSamples > nbSamples_out)
                nbSamples_out = *nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(*nbOutSamples != 0)
            {
                *nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_down(ResampleContext *ctx,
                                    int             *in,
                                    int             nb_channels,
                                    int             *nSamples,
                                    int             *out,
                                    int             *nbOutSamples,
                                    int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    
    flush_in               = *flush;
    nbin_in                = *nSamples;
    nbout_in               = *nbOutSamples;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;
    
    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            *nSamples          = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            *nbOutSamples = nbout_in;
            *flush        = flush_in;
            resample_ML_block_fixin_fixout_down_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       nbOutSamples, 
                                                       flush);
            if(*nSamples > nbSamples_out)
                nbSamples_out = *nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(*nbOutSamples != 0)
            {
                *nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_down_maxCasc1(ResampleContext *ctx,
                                             int             *in,
                                             int             nb_channels,
                                             int             *nSamples,
                                             int             *out,
                                             int             *nbOutSamples,
                                             int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    
    flush_in               = *flush;
    nbin_in                = *nSamples;
    nbout_in               = *nbOutSamples;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;
    
    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            *nSamples          = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            *nbOutSamples = nbout_in;
            *flush        = flush_in;
            	resample_ML_block_fixin_fixout_down_maxCasc1_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       nbOutSamples, 
                                                       flush);
            if(*nSamples > nbSamples_out)
                nbSamples_out = *nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(*nbOutSamples != 0)
            {
                *nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_msec_copy(ResampleContext *ctx,
                                         int             *in,
                                         int             nb_channels,
                                         int             *nSamples_1ms,
                                         int             *out,
                                         int             *nbOutSamples_1ms,
                                         int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    int nSamples, nbOutSamples;
    
    // convert ms to nsamples in this mode
    nSamples     = nbin_in  = *nSamples_1ms     * ctx->fin_khz;
    nbOutSamples = nbout_in = *nbOutSamples_1ms * ctx->fout_khz;
    
    flush_in               = *flush;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;

    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            nSamples           = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            nbOutSamples = nbout_in;
            *flush       = flush_in;
            resample_ML_block_fixin_fixout_msec_copy_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       &nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       &nbOutSamples, 
                                                       flush);
            if(nSamples > nbSamples_out)
                nbSamples_out = nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(nbOutSamples != 0)
            {
                nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    //convert nsamples to ms in this mode
    //*nSamples_1ms     = nSamples     / ctx->fin_khz;
    //*nbOutSamples_1ms = nbOutSamples / ctx->fout_khz;
    (*ctx->samples2msec) (nSamples, nbOutSamples, nSamples_1ms, nbOutSamples_1ms);
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_msec_up(ResampleContext *ctx,
                                       int             *in,
                                       int             nb_channels,
                                       int             *nSamples_1ms,
                                       int             *out,
                                       int             *nbOutSamples_1ms,
                                       int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    int nSamples, nbOutSamples;
    
    // convert ms to nsamples in this mode
    nSamples     = nbin_in  = *nSamples_1ms     * ctx->fin_khz;
    nbOutSamples = nbout_in = *nbOutSamples_1ms * ctx->fout_khz;
    
    flush_in               = *flush;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;

    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            nSamples           = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            nbOutSamples = nbout_in;
            *flush       = flush_in;
            resample_ML_block_fixin_fixout_msec_up_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       &nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       &nbOutSamples, 
                                                       flush);
            if(nSamples > nbSamples_out)
                nbSamples_out = nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(nbOutSamples != 0)
            {
                nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    //convert nsamples to ms in this mode
    //*nSamples_1ms     = nSamples     / ctx->fin_khz;
    //*nbOutSamples_1ms = nbOutSamples / ctx->fout_khz;
    (*ctx->samples2msec) (nSamples, nbOutSamples, nSamples_1ms, nbOutSamples_1ms);
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_msec_up_maxCasc1(ResampleContext *ctx,
                                                int             *in,
                                                int             nb_channels,
                                                int             *nSamples_1ms,
                                                int             *out,
                                                int             *nbOutSamples_1ms,
                                                int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    int nSamples, nbOutSamples;
    
    // convert ms to nsamples in this mode
    nSamples     = nbin_in  = *nSamples_1ms     * ctx->fin_khz;
    nbOutSamples = nbout_in = *nbOutSamples_1ms * ctx->fout_khz;
    
    flush_in               = *flush;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;

    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            nSamples           = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            nbOutSamples = nbout_in;
            *flush       = flush_in;
            	resample_ML_block_fixin_fixout_msec_up_maxCasc1_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       &nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       &nbOutSamples, 
                                                       flush);
            if(nSamples > nbSamples_out)
                nbSamples_out = nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(nbOutSamples != 0)
            {
                nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    //convert nsamples to ms in this mode
    //*nSamples_1ms     = nSamples     / ctx->fin_khz;
    //*nbOutSamples_1ms = nbOutSamples / ctx->fout_khz;
    (*ctx->samples2msec) (nSamples, nbOutSamples, nSamples_1ms, nbOutSamples_1ms);
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_msec_down(ResampleContext *ctx,
                                         int             *in,
                                         int             nb_channels,
                                         int             *nSamples_1ms,
                                         int             *out,
                                         int             *nbOutSamples_1ms,
                                         int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    int nSamples, nbOutSamples;
    
    // convert ms to nsamples in this mode
    nSamples     = nbin_in  = *nSamples_1ms     * ctx->fin_khz;
    nbOutSamples = nbout_in = *nbOutSamples_1ms * ctx->fout_khz;
    
    flush_in               = *flush;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;

    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            nSamples           = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            nbOutSamples = nbout_in;
            *flush       = flush_in;
            resample_ML_block_fixin_fixout_msec_down_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       &nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       &nbOutSamples, 
                                                       flush);
            if(nSamples > nbSamples_out)
                nbSamples_out = nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(nbOutSamples != 0)
            {
                nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    //convert nsamples to ms in this mode
    //*nSamples_1ms     = nSamples     / ctx->fin_khz;
    //*nbOutSamples_1ms = nbOutSamples / ctx->fout_khz;
    (*ctx->samples2msec) (nSamples, nbOutSamples, nSamples_1ms, nbOutSamples_1ms);
    
    return 0;
} /* end resample_BLK() */


int
resample_ML_block_fixin_fixout_msec_down_maxCasc1(ResampleContext *ctx,
                                                  int             *in,
                                                  int             nb_channels,
                                                  int             *nSamples_1ms,
                                                  int             *out,
                                                  int             *nbOutSamples_1ms,
                                                  int             *flush)
{
    int nbin_in, nbout_in, flush_in, nbSamples_out, channel;
    int nSamples, nbOutSamples;
    
    // convert ms to nsamples in this mode
    nSamples     = nbin_in  = *nSamples_1ms     * ctx->fin_khz;
    nbOutSamples = nbout_in = *nbOutSamples_1ms * ctx->fout_khz;
    
    flush_in               = *flush;
    ctx->flush_pending_sav = ctx->flush_pending;
    ctx->flush_squelch_sav = ctx->flush_squelch;
    ctx->flush_ack_sav     = ctx->flush_ack;

    for(channel = 0; channel < nb_channels; channel++)
    {
        if(channel > 0)
        {
            ctx->flush_pending = ctx->flush_pending_sav;
            ctx->flush_squelch = ctx->flush_squelch_sav;
            ctx->flush_ack     = ctx->flush_ack_sav;
            nSamples           = nbin_in;
        }
        nbSamples_out = 0;
        
        while(1)
        {
            nbOutSamples = nbout_in;
            *flush       = flush_in;
            	resample_ML_block_fixin_fixout_msec_down_maxCasc1_inline(ctx,
													   (void*)(((char*)in)+channel*(ctx->sample_size)),
                                                       nb_channels, 
                                                       &nSamples, 
                                                       channel, 
                                                       (void*)(((char*)out)+channel*(ctx->sample_size)), 
                                                       &nbOutSamples, 
                                                       flush);
            if(nSamples > nbSamples_out)
                nbSamples_out = nSamples;
            if(flush_in == 0) break;
            if(nbout_in == 0) break;
            if(ctx->flush_pending == 0) break;
            if(nbOutSamples != 0)
            {
                nSamples = nbSamples_out;
                break;
            }
            if(ctx->flush_ack != 0) break;
        }
    }
    
    //convert nsamples to ms in this mode
    //*nSamples_1ms     = nSamples     / ctx->fin_khz;
    //*nbOutSamples_1ms = nbOutSamples / ctx->fout_khz;
    (*ctx->samples2msec) (nSamples, nbOutSamples, nSamples_1ms, nbOutSamples_1ms);
    
    return 0;
} /* end resample_BLK() */


static int inline
resample_calc_nbIn_fixin_fixout(int               *nbIn,
                                unsigned int      nbOut,
                                ResampleDelayLine *delayLine,
                                ResampleFilter    *filterInfo)
{
    int sum;
    int predicted_final_offset;
    
    sum = delayLine->offset+(nbOut*filterInfo->L)-filterInfo->M-filterInfo->L;
    if(sum < 0)
    {
        *nbIn=0;
    }
    else
    {
        *nbIn=1+(sum/filterInfo->M);
    }
    predicted_final_offset=delayLine->offset+(nbOut*filterInfo->L)-(*nbIn*filterInfo->M);
    return predicted_final_offset;
}


static int inline
resample_calc_nbOut_fixin_fixout(unsigned int    nbIn,
                                 unsigned int    *nbOut,
                                 ResampleDelayLine *delayLine,
                                 ResampleFilter *filterInfo)
{
    unsigned int sum;
    int predicted_final_offset;
    
    *nbOut=0;
    if(nbIn*filterInfo->M > delayLine->offset)
    {
        sum=(nbIn*filterInfo->M)-delayLine->offset-1;
        *nbOut=(sum/filterInfo->L)+1;
    }
    predicted_final_offset=delayLine->offset+(*nbOut*filterInfo->L)-(nbIn*filterInfo->M);
    return predicted_final_offset;
}

void
resample_update_flush(int *flush,int *nSamples,int nbOutSamples, ResampleContext *ctx)
{
	int nsamp_flush=RESAMPLE_MIN_SAMPLES_FOR_FLUSH;
	if (*flush!=0)
	{
		if (ctx->flush_pending!=0)
		{
			if ((nbOutSamples!=0)||(*nSamples!=0)) // count only if filtering was done
			{
				if (ctx->flush_pending<nsamp_flush)
					ctx->flush_pending+=(int)ctx->block_siz;
				else
				{
					if (nbOutSamples!=0)
						ctx->flush_ack=1;
				}
			}
			*nSamples=0;
		}
		else
		{
			if (*nSamples!=0)
			{
				ctx->flush_pending+=(int)ctx->block_siz;
				ctx->flush_squelch=1;
			}
		}
	}
	else
	{
		ctx->flush_ack=0;
		ctx->flush_pending=0;
		ctx->flush_squelch=0;
	}
	*flush=ctx->flush_ack;
}

void
resample_update_flush_msec(int *flush,int *nSamples,int nbOutSamples, ResampleContext *ctx)
{
	int nsamp_flush;
    if (ctx->upsampling==0)
        nsamp_flush=200;
    else
        nsamp_flush=50;
	if (*flush!=0)
	{
		if (ctx->flush_pending!=0)
		{
			if ((nbOutSamples!=0)||(*nSamples!=0)) // count only if filtering was done
			{
				if (ctx->flush_pending<nsamp_flush)
					ctx->flush_pending+=(int)ctx->block_siz;
				else
				{
					if (nbOutSamples!=0)
						ctx->flush_ack=1;
				}
			}
			*nSamples=0;
		}
		else
		{
			if (*nSamples!=0)
			{
				ctx->flush_pending+=(int)ctx->block_siz;
				ctx->flush_squelch=1;
			}
		}
	}
	else
	{
		ctx->flush_ack=0;
		ctx->flush_pending=0;
		ctx->flush_squelch=0;
	}
	*flush=ctx->flush_ack;
}
#endif //ifndef M4_LL_MSEC
