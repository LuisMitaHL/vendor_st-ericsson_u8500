/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _resample_h_
#define _resample_h_

#if defined(NEON) || defined(ARM) 
#define BUILD_FIXED
#endif
#include "audiolibs_common.h"
#include "resample_max_delay.h"

#ifdef NULL
#undef NULL
#define NULL 0
#endif

#define RESAMPLE_MAX_CHANNELS 8
#define RESAMPLE_MAX_CASCADE  4
#define RESAMPLE_MAX_UPSAMPLING           6 // 8->32, 12->48 use 4x then 7/5 -> 5.6 */
#define RESAMPLE_MAX_TEMPBUF_UPSAMPLING 1.1
#define RESAMPLE_BLOCK_SIZE   32
#define RESAMPLE_BLOCK_SIZE_MAX 80
#define RESAMPLE_MIN_SAMPLES_FOR_FLUSH 1500

//MUST be aligned with AFM_SRC_MODE in AFM_Types.h
#ifndef SRC_MODE_INTERNAL
typedef enum AFM_SRC_MODE {
    AFM_SRC_MODE_NORMAL,
    AFM_SRC_MODE_CUSTOM1,
    AFM_SRC_MODE_CUSTOM2,
    AFM_SRC_MODE_CUSTOM3,
    AFM_SRC_MODE_CUSTOM4,
    AFM_SRC_MODE_CUSTOM5,
    AFM_SRC_MODE_CUSTOM6,
    AFM_SRC_MODE_CUSTOM7,
    AFM_SRC_MODE_CUSTOM8
    
}AFM_SRC_MODE;
#endif

typedef enum{
    SRC_STANDARD_QUALITY=0,
    SRC_LOW_RIPPLE=1,
    SRC_LOW_LATENCY=2,
    SRC_LOW_LATENCY_IN_MSEC=6,
    SRC_HIFI=8
}SRC_QUALITY_t;



#ifndef __flexcc2__
#define RESAMPLE_MEM
#else
#ifdef __flexcc2__
#define RESAMPLE_MEM __EXTERN
#else
#define RESAMPLE_MEM YMEM
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef M4_LL_MSEC
typedef struct {
    int *baseAddress;
    unsigned int   len;
    unsigned int totalen;
    int   currIndex;
    int writeIndex;
    int nbremain;
    unsigned int offset;
	int ARM_Sample16_config;
	int ARM_Coef16;
} ResampleDelayLine;

typedef struct {
    int *baseAddress;
    int readptr;
    int writeptr;
    int nbremain;
} ResampleRemainbuf;
#else //ifndef M4_LL_MSEC
typedef struct {
    int *baseAddress;
    unsigned int   len;
    int   currIndex;
    unsigned int offset;
} ResampleDelayLine;
#endif //ifndef M4_LL_MSEC


typedef const struct {
    unsigned int M;    /* upsampling factor */
    unsigned int L;    /* downsampling factor */
    unsigned int decimno; /*multiplication factor to get to the real coef phase eg. M/L=3/2 x 4/4 = 12/8 so decimno=4*/
    const Float  YMEM *filter; /* pointer to lp filter */
    const int    RESAMPLE_MEM *incr_offset_tab; /* pointer to  table for update/convolution decision state machine */
    unsigned int ncfs;  /* nb of designed coefs */
    int          shift;/* compensation for normalization */
    Float        coef; /* compensation for normalization (downsampling only) */
    unsigned int nzcoefs; /* number of zero coefs case of non multiple lcd size tables*/
} ResampleFilter;

typedef ResampleFilter * const cascadedFilter;

typedef struct {
    ResampleDelayLine *delay[RESAMPLE_MAX_CASCADE];
#ifndef M4_LL_MSEC
    ResampleRemainbuf *Resampleremainbuf;
#endif
} ResampleContextChannel;

#ifndef M4_LL_MSEC
typedef struct _ResampleContext {
    /* non-channel dependent data */
    unsigned int         low_latency;
    unsigned int         fin_khz;
    unsigned int         fout_khz;
    unsigned int         upsampling;
    unsigned int         downsampling;
    unsigned int         flush_pending;
    unsigned int         flush_ack;
    unsigned int         flush_squelch;
    unsigned int         flush_pending_sav;
    unsigned int         flush_ack_sav;
    unsigned int         flush_squelch_sav;
    unsigned int         block_siz;
    unsigned int         block_size_by_nb_channel;
    unsigned int         maxCascade;
	unsigned int		 ARM_Sample16;
	unsigned int		 sample_size;
    int                  *tempbuf1;
    int                  *tempbuf2;
    Float                *CoefRam[RESAMPLE_MAX_CASCADE];
    cascadedFilter       *filterInfo;

    int                  (*processing) (struct _ResampleContext *ctx,
                                        int                     *in,
                                        int                     istride,
                                        int                     *nSamples,
                                        int                     *out,
                                        int                     *nbOutSamples,
                                        int                     *flush);
                                        
    void                (*samples2msec) (int n_samples_in, int n_samples_out, int *n_samples_in_1ms, int *n_samples_out_1ms);
                                        
    /* channel dependent data */
    ResampleContextChannel ctxChan[RESAMPLE_MAX_CHANNELS];
} ResampleContext;
#else //ifndef M4_LL_MSEC
typedef struct _ResampleContext {
    unsigned int         low_latency;
    unsigned int         fin_khz;
    unsigned int         fout_khz;
    unsigned int         block_siz;
    unsigned int         block_size_by_nb_channel;
    unsigned int         maxCascade;
    Float                *CoefRam[RESAMPLE_MAX_CASCADE];
    cascadedFilter       *filterInfo;
    int                  (*processing) (struct _ResampleContext *ctx,
                                        int                     *in,
                                        int                     istride,
                                        int                     *nSamples,
                                        int                     *out,
                                        int                     *nbOutSamples,
                                        int                     *flush);
    ResampleContextChannel ctxChan[1];
	unsigned int nSamples_coef;
	unsigned int nbOutReq_coef;
} ResampleContext;
#endif //ifndef M4_LL_MSEC

extern int
resample_calc_heap_size_fixin_fixout(int inrate, int outrate,int src_quality, int *heapsiz, unsigned int blocksiz, unsigned int channel_nb);

AUDIO_API_IMPORT extern int
resample_calc_max_heap_size_fixin_fixout(int Inrate,int Outrate,int src_quality,int *heapsiz, unsigned int blocksiz,unsigned int channel_nb,unsigned int conversion_type);

extern int
resample_calc_scratch_size_fixin_fixout(int inrate,int outrate,int src_quality,int *scratchsiz, unsigned int blocksiz);


AUDIO_API_IMPORT extern int
resample_x_init_ctx_low_mips_fixin_fixout(char *heap,int size_heap,
                                          ResampleContext *ctx, int inrate,int outrate,int src_quality,
                                          unsigned int channel_nb, unsigned int blocksiz);

AUDIO_API_IMPORT extern int
resample_x_init_ctx_low_mips_fixin_fixout_sample16(char *heap,int size_heap,
                                          ResampleContext *ctx, int inrate,int outrate,int src_quality,
                                          unsigned int channel_nb, unsigned int blocksiz);


int
resample_x_cpy_0_1_ctx_low_mips_fixin_fixout(ResampleContext *ctx, int Inrate,int Outrate,int src_quality,unsigned int blocksiz);

AUDIO_API_IMPORT extern int
resample_x_process_fixin_fixout(ResampleContext *ctx,
                                int *in,
                                int stride,
                                int *nbSamples,
                                int *output,
                                int *nbOut,
                                int *flush);



#ifdef __cplusplus
}
#endif // __cplusplus


/* generic heap size */
#define RESAMPLE_HEAP_SIZE (((RESAMPLE_MAX_DELAY * sizeof(int))+(sizeof(ResampleDelayLine)*RESAMPLE_MAX_CASCADE))* RESAMPLE_MAX_CHANNELS)

#endif /* Do not edit below this line */


