/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_local.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _resample_local_h_
#define _resample_local_h_
#include "resample.h"
#if defined(ARM) || defined(NEON)
#define nb_buf 2 //since there is no circular mode need to double the delay line length
#define MAX_VECTOR_LEN 8
#define VECTOR_LEN_CA9_MUL32_64 4
#define VECTOR_LEN_CA9_MUL16_32 8
#define VECTOR_LOOP_CA9_SHIFT_MUL32_64 2
#define VECTOR_LOOP_CA9_SHIFT_MUL16_32 3
#define VECTOR_LEN_M4 2
#define VECTOR_LOOP_M4 1
#else
#define nb_buf 1
#define MAX_VECTOR_LEN 1
#endif

typedef struct {
    char **ptr;
    int *max_memory;
    int *total_char;
} resample_alloc_parameters;

extern void resample_init_alloc(char *base_adr, int size_mem, resample_alloc_parameters *param);
extern void *resample_alloc(int num, int size, resample_alloc_parameters *param);
extern int  resample_choose_processing_routine(ResampleContext *ctx);

#ifndef M4_LL_MSEC
extern void samples2msec_8_8khz  (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_8_16khz (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_8_48khz (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_16_8khz (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_16_16khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_16_48khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_48_8khz (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_48_16khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
extern void samples2msec_48_48khz(int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);

extern void
resample_ML_block_fixin_varout                   (int               *in, 
                                                  int               nb_channels, 
                                                  int               nSamples,
                                                  int               *out, 
                                                  int               *nbOutSamples,
                                                  ResampleContext   *ctx,
                                                  int               channel);

extern int
resample_ML_block_fixin_fixout_copy              (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_up                (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_up_maxCasc1       (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_down              (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_down_maxCasc1     (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
                                                 
extern int                                      
resample_ML_block_fixin_fixout_msec_copy         (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_msec_up           (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_msec_up_maxCasc1  (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int                                      
resample_ML_block_fixin_fixout_msec_down         (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);
extern int
resample_ML_block_fixin_fixout_msec_down_maxCasc1(ResampleContext   *ctx,
                                                  int               *in,
                                                  int               nb_channels,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);

extern void
resample_ML_block_varin_fixout                   (ResampleContext   *ctx,
                                                  int               *in,
                                                  int               istride,
                                                  int               *nSamples,
                                                  int               *out,
                                                  int               *nbOutSamples,
                                                  int               *flush);

void
resample_ML_filter_block_fixin_varout            (int               *in,
                                                  unsigned int      istride,
                                                  unsigned int      nbIn,
                                                  int               *out,
                                                  unsigned int      ostride,
                                                  unsigned int      *nbOut,
                                                  ResampleDelayLine *delayLine,
                                                  ResampleFilter    *filterInfo);
void                                             
resample_ML_filter_block_fixin_fixout            (int               *in,
                                                  unsigned int      istride,
                                                  unsigned int      nbIn,
                                                  int               *out,
                                                  unsigned int      ostride,
                                                  unsigned int      nbOutRequested,
                                                  unsigned int      *nbOutAvail,
                                                  ResampleDelayLine *delayLine,
                                                  ResampleFilter    *filterInfo,
                                                  unsigned int      loop_siz,
                                                  Float             *CoefRam,
												  int				squelch);
void
resample_ML_filter_block_fixin_fixout_ll         (int               *in,
                                                  unsigned int      istride,
                                                  unsigned int      nbIn,
                                                  int               *out,
                                                  unsigned int      ostride,
                                                  unsigned int      nbOutRequested,
                                                  unsigned int      *nbOutAvail,
                                                  ResampleDelayLine *delayLine,
                                                  ResampleFilter    *filterInfo,
                                                  unsigned int      loop_siz,
                                                  Float             *CoefRam,
                                                  int               squelch);
int
resample_ML_read_block_fixin_fixout(ResampleDelayLine *delayLine);

void
resample_ML_load_block_fixin_fixout(int *in,
                         unsigned int    istride,
                         unsigned int    nbIn,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,int squelch/*,int low_latency*/);
void
resample_ML_conv_block_fixin_fixout(int *out,
                         unsigned int    ostride,
                         unsigned int    nbOutRequested,
                         unsigned int    *nbOutAvail,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,
                         unsigned int loop_siz
                        ,Float *CoefRam
                         );
void
resample_ML_conv_block_fixin_fixout_ll(int *out,
                         unsigned int    ostride,
                         unsigned int    nbOutRequested,
                         unsigned int    *nbOutAvail,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,
                         unsigned int loop_siz
                        ,Float *CoefRam
                         );
void
resample_ML_store_block_fixin_fixout(   ResampleRemainbuf *resampleremainbuf,
                                int *out,
                                unsigned int    ostride,
                                unsigned int    nbOutRequested,
                                unsigned int    block_siz,
								unsigned int	ARM_Sample16);
void
resample_ML_convout_block_fixin_fixout(int *in,
                         unsigned int    istride,
                         unsigned int    nbIn,
                         ResampleRemainbuf *resampleremainbuf,
                         unsigned int    nbOutRequested,
                         unsigned int    *nbOutAvail,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,
                         unsigned int loop_siz,
                         unsigned int block_siz
                        ,Float *CoefRam
                         );
void
resample_ML_convout_block_fixin_fixout_ll(int *in,
                         unsigned int    istride,
                         unsigned int    nbIn,
                         ResampleRemainbuf *resampleremainbuf,
                         unsigned int    nbOutRequested,
                         unsigned int    *nbOutAvail,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,
                         unsigned int loop_siz,
                         unsigned int block_siz
                        ,Float *CoefRam
                         );

void
resample_ML_filter_block_varin_fixout(int *in,
                         unsigned int    istride,
                         unsigned int    nbIn,
                         int *out,
                         unsigned int    ostride,
                         unsigned int    nbOutRequested,
                         unsigned int    *nbOutAvail,
                         ResampleDelayLine *delayLine,
                         ResampleFilter *filterInfo,
                         unsigned int flush);

#include "fidelity.h"
#include "table2xa.h"
#include "table2d.h"
#include "table4xa.h"
#include "table4d.h"
#include "table3xa.h"
#include "table3d.h"
#include "table6xa.h"
#include "table6d.h"
#include "table12d.h"
#include "table2_3.h"
#include "table4_3.h"
#include "table3_2.h"
#include "table3_4.h"
#include "table48_44_fasta.h"
#include "table44_48_fast.h"
#include "table44_48LR.h"
#include "table12_9.h"
#include "table10_12.h"
#include "table12_10.h"
#include "table9_12.h"
#include "table48_16_low_latency.h"
#include "table16_8_low_latency.h"
#include "table8_16_low_latency.h"
#include "table16_48_low_latency.h"
#if defined(hifi) || defined(hifi_locoefs)
#ifdef hifi_locoefs
#include "table7_5H.h"
#include "table5_7H.h"
#include "table7_9H.h"
#include "table9_7H.h"
#else
#include "table44_48_fastH.h"
#include "table48_44_fastaH.h"
#endif
#include "table3xaH.h"
#include "table2dH.h"
#include "table3dH.h"
#include "table4dH.h"
#include "table6dH.h"
#include "table2_3H.h"
#include "table3_2H.h"
#include "table3_4H.h"
#include "table4_3H.h"
#include "table12dH.h"
#include "table2xaH.h"
#include "table4xaH.h"
#include "table6xaH.h"
#endif
#else //ifndef M4_LL_MSEC
extern int
resample_ML_block_M4(ResampleContext *ctx,
		int             *in,
		int             nb_channels,
		int             *nSamples_inmsec,
		int             *out,
		int             *nbOutSamples_inmsec,
		int             *flush);
#include "table48_16_low_latency.h"
#include "table16_48_low_latency.h"
#endif //ifndef M4_LL_MSEC
// define for test mode
#define RESAMPLE_MAX_FLOATING_POINT_DIFF  (0.000001)
#define RESAMPLE_MAX_FIXED_POINT_DIFF     (256)

#define ZERO_SCALE (0)

#endif /* Do not edit below this line */



