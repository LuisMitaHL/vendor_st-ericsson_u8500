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
#include "vector.h"
#define div sizeof(cascadedFilter)
#ifndef M4_LL_MSEC
#define COEF16_ON 0
//#define COEF16_ON 1
#define COEF16_OFF 0
const RESAMPLE_MEM int dummy[1]={0};
static int resample_init_core(char *heap,int size_heap,
        ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
        unsigned int channel_nb, unsigned int blocksiz, int vector_len);
static int resample_ck_downsampling(int Inrate,int Outrate,ResampleContext *ctx);
static int get_freq(int Inrate, int Outrate,int src_quality,int *inRate, int *outRate);
static int get_coef_ram_size(int Inrate,int Outrate,int src_quality,int stage);
static int get_config(int src_quality,int inrate,int outrate,cascadedFilter **filterInfo,unsigned int *maxCascade,int **Delayptr);


enum
{
    f96_infreq,     //  ESAA_FREQ_96KHZ,
    f88_infreq,     //  ESAA_FREQ_88_2KHZ,
    f64_infreq,     //  ESAA_FREQ_64KHZ,
    f48_infreq,     //  ESAA_FREQ_48KHZ,
    f44_infreq,     //  ESAA_FREQ_44_1KHZ,
    f32_infreq,     //  ESAA_FREQ_32KHZ,
    f24_infreq,     //  ESAA_FREQ_24KHZ,
    f22_infreq,     //  ESAA_FREQ_22_05KHZ,
    f16_infreq,     //  ESAA_FREQ_16KHZ,
    f12_infreq,     //  ESAA_FREQ_12KHZ,
    f11_infreq,     //  ESAA_FREQ_11_025KHZ,
    f8_infreq,      //  ESAA_FREQ_8KHZ,
    f7_infreq,      //  ESAA_FREQ_7_2KHZ,
    num_infreq
};
enum
{
    f48_outfreq,    //  ESAA_FREQ_48KHZ,
    f44_outfreq,    //  ESAA_FREQ_44_1KHZ,
    f32_outfreq,    //  ESAA_FREQ_32KHZ,
    f24_outfreq,    //  ESAA_FREQ_24KHZ,
    f22_outfreq,    //  ESAA_FREQ_22_05KHZ,
    f16_outfreq,    //  ESAA_FREQ_16KHZ,
    f12_outfreq,    //  ESAA_FREQ_12KHZ,
    f11_outfreq,    //  ESAA_FREQ_11_025KHZ,
    f8_outfreq,     //  ESAA_FREQ_8KHZ,
    f7_outfreq,     //  ESAA_FREQ_7_2KHZ,
    num_outfreq
};
const RESAMPLE_MEM int infreq[ESAA_FREQ_LAST_IN_LIST]={
    num_infreq,     //  ESAA_FREQ_UNKNOWNKHZ
    num_infreq,     //  ESAA_FREQ_192KHZ
    num_infreq,     //  ESAA_FREQ_176_4KHZ
    num_infreq,     //  ESAA_FREQ_128KHZ
    f96_infreq,     //  ESAA_FREQ_96KHZ
    f88_infreq,     //  ESAA_FREQ_88_2KHZ
    f64_infreq,     //  ESAA_FREQ_64KHZ
    f48_infreq,     //  ESAA_FREQ_48KHZ
    f44_infreq,     //  ESAA_FREQ_44_1KHZ
    f32_infreq,     //  ESAA_FREQ_32KHZ
    f24_infreq,     //  ESAA_FREQ_24KHZ
    f22_infreq,     //  ESAA_FREQ_22_05KHZ
    f16_infreq,     //  ESAA_FREQ_16KHZ
    f12_infreq,     //  ESAA_FREQ_12KHZ
    f11_infreq,     //  ESAA_FREQ_11_025KHZ
    f8_infreq,      //  ESAA_FREQ_8KHZ
    f7_infreq,      //  ESAA_FREQ_7_2KHZ
};
const RESAMPLE_MEM int outfreq[ESAA_FREQ_LAST_IN_LIST]={
    num_outfreq,        //  ESAA_FREQ_UNKNOWNKHZ
    num_outfreq,        //  ESAA_FREQ_192KHZ
    num_outfreq,        //  ESAA_FREQ_176_4KHZ
    num_outfreq,        //  ESAA_FREQ_128KHZ
    num_outfreq,        //  ESAA_FREQ_96KHZ
    num_outfreq,        //  ESAA_FREQ_88_2KHZ
    num_outfreq,        //  ESAA_FREQ_64KHZ
    f48_outfreq,        //  ESAA_FREQ_48KHZ
    f44_outfreq,        //  ESAA_FREQ_44_1KHZ
    f32_outfreq,        //  ESAA_FREQ_32KHZ
    f24_outfreq,        //  ESAA_FREQ_24KHZ
    f22_outfreq,        //  ESAA_FREQ_22_05KHZ
    f16_outfreq,        //  ESAA_FREQ_16KHZ
    f12_outfreq,        //  ESAA_FREQ_12KHZ
    f11_outfreq,        //  ESAA_FREQ_11_025KHZ
    f8_outfreq,     //  ESAA_FREQ_8KHZ
    f7_outfreq,     //  ESAA_FREQ_7_2KHZ
};
const RESAMPLE_MEM int freq_lookup[ESAA_FREQ_LAST_IN_LIST] = {
    ESAA_FREQ_UNKNOWNKHZ,  //0
    ESAA_FREQ_UNKNOWNKHZ,  //1
    ESAA_FREQ_UNKNOWNKHZ,  //2
    ESAA_FREQ_UNKNOWNKHZ,  //3
    96,     //4
    88,     //5
    64,     //6
    48,     //7
    44,     //8
    32,     //9
    24,     //10
    22,     //11
    16,     //12
    12,     //13
    11,     //14
    8,      //15
    7       //16
};

static RESAMPLE_MEM ResampleFilter filter_fast_up_2x = {
    M_2xa,
    L_2xa,
    decimno_2xa,
    resample_12x,
    src_incr_offset_2_1,
    N_2xa,
    SHIFT_2xa,
    SCALE_2x,
    NZCOEFS_2xa
};
static  RESAMPLE_MEM ResampleFilter filter_fast_up_3x = {
    M_3xa,
    L_3xa,
    decimno_3xa,
    resample_12x,
    src_incr_offset_3_1,
    N_3xa,
    SHIFT_3xa,
    SCALE_3x,
    NZCOEFS_3xa
};
static  RESAMPLE_MEM ResampleFilter filter_fast_up_4x = {
    M_4xa,
    L_4xa,
    decimno_4xa,
    resample_12x,
    src_incr_offset_4_1,
    N_4xa,
    SHIFT_4xa,
    SCALE_4x,
    NZCOEFS_4xa
};
static RESAMPLE_MEM ResampleFilter filter_fast_up_6x = {
    M_6xa,
    L_6xa,
    decimno_6xa,
    resample_12x,
    src_incr_offset_6_1,
    N_6xa,
    SHIFT_6xa,
    SCALE_6x,
    NZCOEFS_6xa
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_2x = {
    M_2d,
    L_2d,
    decimno_2d,
    resample_12x,
    src_incr_offset_1_2,
    N_2d,
    SHIFT_2d,
    SCALE_2d,
    NZCOEFS_2d
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_3x = {
    M_3d,
    L_3d,
    decimno_3d,
    resample_12x,
    src_incr_offset_1_3,
    N_3d,
    SHIFT_3d,
    SCALE_3d,
    NZCOEFS_3d
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_4x = {
    M_4d,
    L_4d,
    decimno_4d,
    resample_12x,
    src_incr_offset_1_4,
    N_4d,
    SHIFT_4d,
    SCALE_4d,
    NZCOEFS_4d
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_6x = {
    M_6d,
    L_6d,
    decimno_6d,
    resample_12x,
    src_incr_offset_1_6,
    N_6d,
    SHIFT_6d,
    SCALE_6d,
    NZCOEFS_6d
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_12x = {
    M_12d,
    L_12d,
    decimno_12d,
    resample_12x,
    src_incr_offset_1_12,
    N_12d,
    SHIFT_12d,
    SCALE_12d,
    NZCOEFS_12d
};
static RESAMPLE_MEM ResampleFilter fast_48_44a = {
    M_48_44_FASTa,
    L_48_44_FASTa,
    decimno_48_44_FASTa,
    resample_48_44,
    src_incr_offset_34_37,
    N_48_44_FASTa,
    SHIFT_48_44_FASTa,
    SCALE_48_44_FAST,
    NZCOEFS_48_44_FASTa
};
static RESAMPLE_MEM ResampleFilter fast_44_48 = {
    M_44_48_FAST,
    L_44_48_FAST,
    decimno_44_48_FAST,
#ifdef common_table
    resample_48_44,
#else
    resample_44_48,
#endif
    src_incr_offset_37_34,
    N_44_48_FAST,
    SHIFT_44_48_FAST,
    SCALE_44_48_FAST,
    NZCOEFS_44_48_FAST
};
static RESAMPLE_MEM ResampleFilter fast_44_48LR = {
    M_44_48LR,
    L_44_48LR,
    decimno_44_48LR,
    resample_44_48LR,
    src_incr_offset_37_34,
    N_44_48LR,
    SHIFT_44_48LR,
    SCALE_44_48LR,
    NZCOEFS_44_48LR
};
static RESAMPLE_MEM ResampleFilter fast_12_9 = {
    M_12_9,
    L_12_9,
    decimno_12_9,
    resample_12x,
    src_incr_offset_4_3,
    N_12_9,
    SHIFT_12_9,
    SCALE_12_9,
    NZCOEFS_12_9
};
static RESAMPLE_MEM ResampleFilter fast_10_12 = {
    M_10_12,
    L_10_12,
    decimno_10_12,
    resample_12x,
    src_incr_offset_5_6,
    N_10_12,
    SHIFT_10_12,
    SCALE_10_12,
    NZCOEFS_10_12

};
static RESAMPLE_MEM ResampleFilter fast_12_10 = {
    M_12_10,
    L_12_10,
    decimno_12_10,
    resample_12x,
    src_incr_offset_6_5,
    N_12_10,
    SHIFT_12_10,
    SCALE_12_10,
    NZCOEFS_12_10
};
static RESAMPLE_MEM ResampleFilter fast_9_12 = {
    M_9_12,
    L_9_12,
    decimno_9_12,
    resample_12x,
    src_incr_offset_3_4,
    N_9_12,
    SHIFT_9_12,
    SCALE_9_12,
    NZCOEFS_9_12
};
static RESAMPLE_MEM ResampleFilter fast_2_3 = {
    M_2_3,
    L_2_3,
    decimno_2_3,
    resample_12x,
    src_incr_offset_2_3,
    N_2_3,
    SHIFT_2_3,
    SCALE_2_3,
    NZCOEFS_2_3
};
static RESAMPLE_MEM ResampleFilter fast_3_4 = {
    M_3_4,
    L_3_4,
    decimno_3_4,
    resample_12x,
    src_incr_offset_3_4,
    N_3_4,
    SHIFT_3_4,
    SCALE_3_4,
    NZCOEFS_3_4
};
static RESAMPLE_MEM ResampleFilter fast_4_3 = {
    M_4_3,
    L_4_3,
    decimno_4_3,
    resample_12x,
    src_incr_offset_4_3,
    N_4_3,
    SHIFT_4_3,
    SCALE_4_3,
    NZCOEFS_4_3
};
static RESAMPLE_MEM ResampleFilter fast_3_2 = {
    M_3_2,
    L_3_2,
    decimno_3_2,
    resample_12x,
    src_incr_offset_3_2,
    N_3_2,
    SHIFT_3_2,
    SCALE_3_2,
    NZCOEFS_3_2
};
static RESAMPLE_MEM ResampleFilter filter_none = {
    0,
    0,
    1,
    0,
    dummy,
    0,
    0,
    0,
    0
};
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
static RESAMPLE_MEM ResampleFilter fast_16_8_low_latency = {
    M_16_8_low_latency,
    L_16_8_low_latency,
    decimno_16_8_low_latency,
    resample_16_8_low_latency,
    src_incr_offset_1_2,
    N_16_8_low_latency,
    SHIFT_16_8_low_latency,
    SCALE_16_8_low_latency,
    NZCOEFS_16_8_low_latency

};
static RESAMPLE_MEM ResampleFilter fast_8_16_low_latency = {
    M_8_16_low_latency,
    L_8_16_low_latency,
    decimno_8_16_low_latency,
    resample_8_16_low_latency,
    src_incr_offset_2_1,
    N_8_16_low_latency,
    SHIFT_8_16_low_latency,
    SCALE_8_16_low_latency,
    NZCOEFS_8_16_low_latency
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
#if defined(hifi) || defined(hifi_locoefs)
static RESAMPLE_MEM ResampleFilter filter_fast_up_2xH = {
    M_2xaH,
    L_2xaH,
    decimno_2xaH,
    resample_12xH,
    src_incr_offset_2_1,
    N_2xaH,
    SHIFT_2xaH,
    SCALE_2xH,
    NZCOEFS_2xaH
};
static  RESAMPLE_MEM ResampleFilter filter_fast_up_3xH = {
    M_3xaH,
    L_3xaH,
    decimno_3xaH,
    resample_12xH,
    src_incr_offset_3_1,
    N_3xaH,
    SHIFT_3xaH,
    SCALE_3xH,
    NZCOEFS_3xaH
};
static  RESAMPLE_MEM ResampleFilter filter_fast_up_4xH = {
    M_4xaH,
    L_4xaH,
    decimno_4xaH,
    resample_12xH,
    src_incr_offset_4_1,
    N_4xaH,
    SHIFT_4xaH,
    SCALE_4xH,
    NZCOEFS_4xaH
};
static RESAMPLE_MEM ResampleFilter filter_fast_up_6xH = {
    M_6xaH,
    L_6xaH,
    decimno_6xaH,
    resample_12xH,
    src_incr_offset_6_1,
    N_6xaH,
    SHIFT_6xaH,
    SCALE_6xH,
    NZCOEFS_6xaH
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_2xH = {
    M_2dH,
    L_2dH,
    decimno_2dH,
    resample_12xH,
    src_incr_offset_1_2,
    N_2dH,
    SHIFT_2dH,
    SCALE_2dH,
    NZCOEFS_2dH
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_3xH = {
    M_3dH,
    L_3dH,
    decimno_3dH,
    resample_12xH,
    src_incr_offset_1_3,
    N_3dH,
    SHIFT_3dH,
    SCALE_3dH,
    NZCOEFS_3dH
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_4xH = {
    M_4dH,
    L_4dH,
    decimno_4dH,
    resample_12xH,
    src_incr_offset_1_4,
    N_4dH,
    SHIFT_4dH,
    SCALE_4dH,
    NZCOEFS_4dH
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_6xH = {
    M_6dH,
    L_6dH,
    decimno_6dH,
    resample_12xH,
    src_incr_offset_1_6,
    N_6dH,
    SHIFT_6dH,
    SCALE_6dH,
    NZCOEFS_6dH
};
static RESAMPLE_MEM ResampleFilter filter_fast_dwn_12xH = {
    M_12dH,
    L_12dH,
    decimno_12dH,
    resample_12xH,
    src_incr_offset_1_12,
    N_12dH,
    SHIFT_12dH,
    SCALE_12dH,
    NZCOEFS_12dH
};
#ifndef hifi_locoefs
static RESAMPLE_MEM ResampleFilter fast_48_44aH = {
    M_48_44_FASTaH,
    L_48_44_FASTaH,
    decimno_48_44_FASTaH,
    resample_48_44H,
    src_incr_offset_34_37,
    N_48_44_FASTaH,
    SHIFT_48_44_FASTaH,
    SCALE_48_44_FASTH,
    NZCOEFS_48_44_FASTaH
};
static RESAMPLE_MEM ResampleFilter fast_44_48H = {
    M_44_48_FASTH,
    L_44_48_FASTH,
    decimno_44_48_FASTH,
#ifdef common_table
    resample_48_44H,
#else
    resample_44_48H,
#endif
    src_incr_offset_37_34,
    N_44_48_FASTH,
    SHIFT_44_48_FASTH,
    SCALE_44_48_FASTH,
    NZCOEFS_44_48_FASTH
};
#endif
static RESAMPLE_MEM ResampleFilter fast_12_9H = {
    M_12_9,
    L_12_9,
    decimno_12_9,
    resample_12x,
    src_incr_offset_4_3,
    N_12_9,
    SHIFT_12_9,
    SCALE_12_9,
    NZCOEFS_12_9
};
static RESAMPLE_MEM ResampleFilter fast_10_12H = {
    M_10_12,
    L_10_12,
    decimno_10_12,
    resample_12x,
    src_incr_offset_5_6,
    N_10_12,
    SHIFT_10_12,
    SCALE_10_12,
    NZCOEFS_10_12
};
static RESAMPLE_MEM ResampleFilter fast_12_10H = {
    M_12_10,
    L_12_10,
    decimno_12_10,
    resample_12x,
    src_incr_offset_6_5,
    N_12_10,
    SHIFT_12_10,
    SCALE_12_10,
    NZCOEFS_12_10
};
static RESAMPLE_MEM ResampleFilter fast_9_12H = {
    M_9_12,
    L_9_12,
    decimno_9_12,
    resample_12x,
    src_incr_offset_3_4,
    N_9_12,
    SHIFT_9_12,
    SCALE_9_12,
    NZCOEFS_9_12
};
static RESAMPLE_MEM ResampleFilter fast_2_3H = {
    M_2_3H,
    L_2_3H,
    decimno_2_3H,
    resample_12xH,
    src_incr_offset_2_3,
    N_2_3H,
    SHIFT_2_3H,
    SCALE_2_3H,
    NZCOEFS_2_3H
};
static RESAMPLE_MEM ResampleFilter fast_3_4H = {
    M_3_4H,
    L_3_4H,
    decimno_3_4H,
    resample_12xH,
    src_incr_offset_3_4,
    N_3_4H,
    SHIFT_3_4H,
    SCALE_3_4H,
    NZCOEFS_3_4H
};
static RESAMPLE_MEM ResampleFilter fast_4_3H = {
    M_4_3H,
    L_4_3H,
    decimno_4_3H,
    resample_12xH,
    src_incr_offset_4_3,
    N_4_3H,
    SHIFT_4_3H,
    SCALE_4_3H,
    NZCOEFS_4_3H
};
static RESAMPLE_MEM ResampleFilter fast_3_2H = {
    M_3_2H,
    L_3_2H,
    decimno_3_2H,
    resample_12xH,
    src_incr_offset_3_2,
    N_3_2H,
    SHIFT_3_2H,
    SCALE_3_2H,
    NZCOEFS_3_2H
};
#ifdef hifi_locoefs
static RESAMPLE_MEM ResampleFilter fast_7_5H = {
    M_7_5H,
    L_7_5H,
    decimno_7_5H,
    resample_7_5H,
    src_incr_offset_7_5,
    N_7_5H,
    SHIFT_7_5H,
    SCALE_7_5H,
    NZCOEFS_7_5H
};
static RESAMPLE_MEM ResampleFilter fast_7_9H = {
    M_7_9H,
    L_7_9H,
    decimno_7_9H,
    resample_7_9H,
    src_incr_offset_7_9,
    N_7_9H,
    SHIFT_7_9H,
    SCALE_7_9H,
    NZCOEFS_7_9H
};
static RESAMPLE_MEM ResampleFilter fast_9_7H = {
    M_9_7H,
    L_9_7H,
    decimno_9_7H,
    resample_7_9H,
    src_incr_offset_9_7,
    N_9_7H,
    SHIFT_9_7H,
    SCALE_9_7H,
    NZCOEFS_9_7H
};
static RESAMPLE_MEM ResampleFilter fast_5_7H = {
    M_5_7H,
    L_5_7H,
    decimno_5_7H,
    resample_7_5H,
    src_incr_offset_5_7,
    N_5_7H,
    SHIFT_5_7H,
    SCALE_5_7H,
    NZCOEFS_5_7H
};
#endif
#endif
static RESAMPLE_MEM cascadedFilter fast_up_2x[1] = {
    (ResampleFilter *)&filter_fast_up_2x            /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_up_3x[1] = {
    (ResampleFilter *)&filter_fast_up_3x            /* 3x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_4x[1] = {
    (ResampleFilter *)&filter_fast_up_4x            /* 4x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_6x[1] = {
    (ResampleFilter *)&filter_fast_up_6x            /* 6x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_2x[1] = {
    (ResampleFilter *)&filter_fast_dwn_2x           /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_3x[1] = {
    (ResampleFilter *)&filter_fast_dwn_3x           /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_4x[1] = {
    (ResampleFilter *)&filter_fast_dwn_4x           /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_6x[1] = {
    (ResampleFilter *)&filter_fast_dwn_6x           /* 1/6 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_12x[1] = {
    (ResampleFilter *)&filter_fast_dwn_12x      /* 1/12 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_48_44[1] = {
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_32_44a[2] = {
    (ResampleFilter *)&fast_3_2,                /* 3/2 ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_24_44[2] = {
    (ResampleFilter *)&filter_fast_up_2x,       /* 2x ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_16_44a[2] = {
    (ResampleFilter *)&filter_fast_up_3x,       /* 3x ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_44[2] = {
    (ResampleFilter *)&filter_fast_up_4x,       /* 4x ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_8_44a[2] = {
    (ResampleFilter *)&filter_fast_up_6x,       /* 6x ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_8[1] = {
    (ResampleFilter *)&fast_2_3                 /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_48[1] = {
    (ResampleFilter *)&fast_3_4                 /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_24[2] = {
    (ResampleFilter *)&fast_3_4,                    /* 3/4 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_16[1] = {
    (ResampleFilter *)&fast_4_3             /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_32[2] = {
    (ResampleFilter *)&fast_4_3,                /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2x        /* 2x ratio */
};
static RESAMPLE_MEM  cascadedFilter Fast_32_48[1] = {
    (ResampleFilter *)&fast_3_2                 /* 3/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_48[1] = {
    (ResampleFilter *)&fast_44_48                   /* 37/34 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_48LR[1] = {
    (ResampleFilter *)&fast_44_48LR                   /* 37/34 ratio */
};

static RESAMPLE_MEM cascadedFilter Fast_72_80[2] = {
    (ResampleFilter *)&fast_12_9,
    (ResampleFilter *)&fast_10_12                   /* 10/9 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_80_72[2] = {
    (ResampleFilter *)&fast_12_10,
    (ResampleFilter *)&fast_9_12                    /* 9/10 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_16[2] = {
    (ResampleFilter *)&fast_44_48,                  /* 37/34 ratio */
    (ResampleFilter *)&fast_4_3                     /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_32[3] = {
    (ResampleFilter *)&fast_44_48,                  /* 37/34 ratio */
    (ResampleFilter *)&fast_4_3,                    /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2x            /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_16a[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&fast_2_3                 /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_48[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_up_2x        /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_48[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_up_4x        /* 4x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_44[2] = {
    (ResampleFilter *)&fast_3_4,                /* 3/4 ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_11[3] = {
    (ResampleFilter *)&filter_fast_dwn_4x,      /* 1/4 ratio */
    (ResampleFilter *)&fast_48_44a,             /* 34/37 ratio */
    (ResampleFilter *)&fast_3_4                 /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_12[2] = {
    (ResampleFilter *)&filter_fast_dwn_4x,      /* 1/4 ratio */
    (ResampleFilter *)&fast_3_4                 /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_12[3] = {
    (ResampleFilter *)&filter_fast_dwn_4x,      /* 1/4 ratio */
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_11[3] = {
    (ResampleFilter *)&filter_fast_dwn_4x,      /* 1/4 ratio */
    (ResampleFilter *)&fast_48_44a,             /* 34/37 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_22[3] = {
    (ResampleFilter *)&filter_fast_dwn_2x,      /* 1/2 ratio */
    (ResampleFilter *)&fast_48_44a,             /* 34/37 ratio */
    (ResampleFilter *)&fast_3_4             /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_8a[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_3x       /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_8a[3] = {
    (ResampleFilter *)&filter_fast_dwn_3x,      /* 1/3 ratio */
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_8[3] = {
    (ResampleFilter *)&filter_fast_dwn_6x,      /* 1/6 ratio */
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_8[2] = {
    (ResampleFilter *)&filter_fast_dwn_2x,      /* 1/2 ratio */
    (ResampleFilter *)&filter_fast_dwn_4x       /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_24[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_4x       /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_48[2] = {
    (ResampleFilter *)&fast_44_48,              /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2x       /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_44[2] = {
    (ResampleFilter *)&filter_fast_dwn_2x,      /* 1/2 ratio */
    (ResampleFilter *)&fast_48_44a              /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_22[2] = {
    (ResampleFilter *)&fast_48_44a,             /* 34/37 ratio */
    (ResampleFilter *)&filter_fast_dwn_4x       /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Filter_none[1] = {
    (ResampleFilter *)&filter_none                  /* none */
};
static RESAMPLE_MEM cascadedFilter Fast_48_16_low_latency[1] = {
    (ResampleFilter *)&fast_48_16_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_16_8_low_latency[1] = {
    (ResampleFilter *)&fast_16_8_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_48_8_low_latency[2] = {
    (ResampleFilter *)&fast_48_16_low_latency,
    (ResampleFilter *)&fast_16_8_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_8_16_low_latency[1] = {
    (ResampleFilter *)&fast_8_16_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_16_48_low_latency[1] = {
    (ResampleFilter *)&fast_16_48_low_latency
};
static RESAMPLE_MEM cascadedFilter Fast_8_48_low_latency[2] = {
    (ResampleFilter *)&fast_8_16_low_latency,
    (ResampleFilter *)&fast_16_48_low_latency
};
#if defined(hifi) || defined(hifi_locoefs)
#ifdef hifi_locoefs
static RESAMPLE_MEM cascadedFilter fast_up_2xH_lc[1] = {
    (ResampleFilter *)&filter_fast_up_2xH               /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_up_3xH_lc[1] = {
    (ResampleFilter *)&filter_fast_up_3xH               /* 3x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_4xH_lc[1] = {
    (ResampleFilter *)&filter_fast_up_4xH               /* 4x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_6xH_lc[1] = {
    (ResampleFilter *)&filter_fast_up_6xH               /* 6x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_2xH_lc[1] = {
    (ResampleFilter *)&filter_fast_dwn_2xH              /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_3xH_lc[1] = {
    (ResampleFilter *)&filter_fast_dwn_3xH              /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_4xH_lc[1] = {
    (ResampleFilter *)&filter_fast_dwn_4xH              /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_6xH_lc[1] = {
    (ResampleFilter *)&filter_fast_dwn_6xH              /* 1/6 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_12xH_lc[1] = {
    (ResampleFilter *)&filter_fast_dwn_12xH             /* 1/12 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_48_44H_lc[2] = {
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_32_44aH_lc[3] = {
    (ResampleFilter *)&fast_3_2H,                   /* 3/2 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_24_44H_lc[3] = {
    (ResampleFilter *)&filter_fast_up_2xH,          /* 2x ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_16_44aH_lc[3] = {
    (ResampleFilter *)&filter_fast_up_3xH,          /* 3x ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_44H_lc[3] = {
    (ResampleFilter *)&filter_fast_up_4xH,          /* 4x ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_8_44aH_lc[3] = {
    (ResampleFilter *)&filter_fast_up_6xH,          /* 6x ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_8H_lc[1] = {
    (ResampleFilter *)&fast_2_3H                    /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_48H_lc[1] = {
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_24H_lc[2] = {
    (ResampleFilter *)&fast_3_4H,                   /* 3/4 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_16H_lc[1] = {
    (ResampleFilter *)&fast_4_3H                    /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_32H_lc[2] = {
    (ResampleFilter *)&fast_4_3H,                   /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2xH           /* 2x ratio */
};
static RESAMPLE_MEM  cascadedFilter Fast_32_48H_lc[1] = {
    (ResampleFilter *)&fast_3_2H                    /* 3/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_48H_lc[2] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H                    /* 7/9 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_72_80H_lc[2] = {
    (ResampleFilter *)&fast_12_9H,
    (ResampleFilter *)&fast_10_12H                  /* 10/9 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_80_72H_lc[2] = {
    (ResampleFilter *)&fast_12_10H,
    (ResampleFilter *)&fast_9_12H                   /* 9/10 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_16H_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&fast_4_3H                    /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_32H_lc[4] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&fast_4_3H,                   /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2xH           /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_16aH_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&fast_2_3H                    /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_48H_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_up_2xH           /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_48H_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_up_4xH           /* 4x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_44H_lc[3] = {
    (ResampleFilter *)&fast_3_4H,                   /* 3/4 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_11H_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,         /* 1/4 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H,                   /* 5/7 ratio */
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_12H_lc[2] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,         /* 1/4 ratio */
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_12H_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,         /* 1/4 ratio */
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_11H_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,         /* 1/4 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H,                   /* 5/7 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_22H_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,         /* 1/2 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H,                   /* 5/7 ratio */
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_8aH_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_3xH          /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_8aH_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_3xH,         /* 1/3 ratio */
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_8H_lc[4] = {
    (ResampleFilter *)&filter_fast_dwn_6xH,         /* 1/6 ratio */
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_8H_lc[2] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,         /* 1/2 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH          /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_24H_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH          /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_48H_lc[3] = {
    (ResampleFilter *)&fast_7_5H,                   /* 7/5 ratio */
    (ResampleFilter *)&fast_7_9H,                   /* 7/9 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_44H_lc[3] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,         /* 1/2 ratio */
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H                    /* 5/7 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_22H_lc[3] = {
    (ResampleFilter *)&fast_9_7H,                   /* 9/7 ratio */
    (ResampleFilter *)&fast_5_7H,                   /* 5/7 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH          /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Filter_noneH_lc[1] = {
    (ResampleFilter *)&filter_none                  /* none */
};
#else
static RESAMPLE_MEM cascadedFilter fast_up_2xH[1] = {
    (ResampleFilter *)&filter_fast_up_2xH           /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_up_3xH[1] = {
    (ResampleFilter *)&filter_fast_up_3xH           /* 3x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_4xH[1] = {
    (ResampleFilter *)&filter_fast_up_4xH           /* 4x ratio */
};
static  RESAMPLE_MEM cascadedFilter fast_up_6xH[1] = {
    (ResampleFilter *)&filter_fast_up_6xH           /* 6x ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_2xH[1] = {
    (ResampleFilter *)&filter_fast_dwn_2xH          /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_3xH[1] = {
    (ResampleFilter *)&filter_fast_dwn_3xH          /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_4xH[1] = {
    (ResampleFilter *)&filter_fast_dwn_4xH          /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_6xH[1] = {
    (ResampleFilter *)&filter_fast_dwn_6xH          /* 1/6 ratio */
};
static RESAMPLE_MEM cascadedFilter fast_dwn_12xH[1] = {
    (ResampleFilter *)&filter_fast_dwn_12xH     /* 1/12 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_48_44H[1] = {
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_32_44aH[2] = {
    (ResampleFilter *)&fast_3_2H,               /* 3/2 ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_24_44H[2] = {
    (ResampleFilter *)&filter_fast_up_2xH,      /* 2x ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_16_44aH[2] = {
    (ResampleFilter *)&filter_fast_up_3xH,      /* 3x ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_44H[2] = {
    (ResampleFilter *)&filter_fast_up_4xH,      /* 4x ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_8_44aH[2] = {
    (ResampleFilter *)&filter_fast_up_6xH,      /* 6x ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_8H[1] = {
    (ResampleFilter *)&fast_2_3H                    /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_48H[1] = {
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_24H[2] = {
    (ResampleFilter *)&fast_3_4H,                   /* 3/4 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_16H[1] = {
    (ResampleFilter *)&fast_4_3H                /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_12_32H[2] = {
    (ResampleFilter *)&fast_4_3H,               /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2xH       /* 2x ratio */
};
static RESAMPLE_MEM  cascadedFilter Fast_32_48H[1] = {
    (ResampleFilter *)&fast_3_2H                    /* 3/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_48H[1] = {
    (ResampleFilter *)&fast_44_48H                  /* 37/34 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_72_80H[2] = {
    (ResampleFilter *)&fast_12_9H,
    (ResampleFilter *)&fast_10_12H                  /* 10/9 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_80_72H[2] = {
    (ResampleFilter *)&fast_12_10H,
    (ResampleFilter *)&fast_9_12H                   /* 9/10 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_16H[2] = {
    (ResampleFilter *)&fast_44_48H,                 /* 37/34 ratio */
    (ResampleFilter *)&fast_4_3H                        /* 4/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_32H[3] = {
    (ResampleFilter *)&fast_44_48H,                 /* 37/34 ratio */
    (ResampleFilter *)&fast_4_3H,                   /* 4/3 ratio */
    (ResampleFilter *)&filter_fast_up_2xH           /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_16aH[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&fast_2_3H                    /* 2/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_48H[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_up_2xH       /* 2x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_11_48H[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_up_4xH       /* 4x ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_44H[2] = {
    (ResampleFilter *)&fast_3_4H,               /* 3/4 ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_11H[3] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,     /* 1/4 ratio */
    (ResampleFilter *)&fast_48_44aH,                /* 34/37 ratio */
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_12H[2] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,     /* 1/4 ratio */
    (ResampleFilter *)&fast_3_4H                    /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_12H[3] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,     /* 1/4 ratio */
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_11H[3] = {
    (ResampleFilter *)&filter_fast_dwn_4xH,     /* 1/4 ratio */
    (ResampleFilter *)&fast_48_44aH,                /* 34/37 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_22H[3] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,     /* 1/2 ratio */
    (ResampleFilter *)&fast_48_44aH,                /* 34/37 ratio */
    (ResampleFilter *)&fast_3_4H                /* 3/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_22_8aH[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_3xH      /* 1/3 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_44_8aH[3] = {
    (ResampleFilter *)&filter_fast_dwn_3xH,     /* 1/3 ratio */
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_8H[3] = {
    (ResampleFilter *)&filter_fast_dwn_6xH,     /* 1/6 ratio */
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_64_8H[2] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,     /* 1/2 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH      /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_24H[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH      /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_88_48H[2] = {
    (ResampleFilter *)&fast_44_48H,             /* 37/34 ratio */
    (ResampleFilter *)&filter_fast_dwn_2xH      /* 1/2 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_44H[2] = {
    (ResampleFilter *)&filter_fast_dwn_2xH,     /* 1/2 ratio */
    (ResampleFilter *)&fast_48_44aH             /* 34/37 ratio */
};
static RESAMPLE_MEM cascadedFilter Fast_96_22H[2] = {
    (ResampleFilter *)&fast_48_44aH,                /* 34/37 ratio */
    (ResampleFilter *)&filter_fast_dwn_4xH      /* 1/4 ratio */
};
static RESAMPLE_MEM cascadedFilter Filter_noneH[1] = {
    (ResampleFilter *)&filter_none                  /* none */
};
#endif
#endif

//cascadedDelay
const RESAMPLE_MEM int delay_up_2x[1] = {DELAY_2xa};
const RESAMPLE_MEM int delay_up_3x[1] = {DELAY_3xa};
const RESAMPLE_MEM int delay_up_4x[1] = {DELAY_4xa};
const RESAMPLE_MEM int delay_up_6x[1] = {DELAY_6xa};
const RESAMPLE_MEM int delay_down_2x[1] = {DELAY_2d};
const RESAMPLE_MEM int delay_down_3x[1] = {DELAY_3d};
const RESAMPLE_MEM int delay_down_4x[1] = {DELAY_4d};
const RESAMPLE_MEM int delay_down_6x[1] = {DELAY_6d};
const RESAMPLE_MEM int delay_down_12x[1] = {DELAY_12d};
const RESAMPLE_MEM int delay48_44[1] = {DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay32_44a[2] = {DELAY_3_2,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay24_44[2] = {DELAY_2xa,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay16_44a[2] = {DELAY_3xa,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay12_44[2] = {DELAY_4xa,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay8_44a[2] = {DELAY_6xa,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay12_8[1] = {DELAY_2_3};
const RESAMPLE_MEM int delay64_48[1] = {DELAY_3_4};
const RESAMPLE_MEM int delay64_24[2] = {DELAY_3_4,DELAY_2d};
const RESAMPLE_MEM int delay12_16[1] = {DELAY_4_3};
const RESAMPLE_MEM int delay12_32[2] = {DELAY_4_3,DELAY_2xa};
const RESAMPLE_MEM int delay32_48[1] = {DELAY_3_2};
const RESAMPLE_MEM int delay44_48[1] = {DELAY_44_48_FAST};
const RESAMPLE_MEM int delay44_48LR[1] = {DELAY_44_48LR};
const RESAMPLE_MEM int delay72_80[2] = {DELAY_12_9,DELAY_10_12};
const RESAMPLE_MEM int delay80_72[2] = {DELAY_12_10,DELAY_9_12};
const RESAMPLE_MEM int delay11_16[2] = {DELAY_44_48_FAST,DELAY_4_3};
const RESAMPLE_MEM int delay11_32[3] = {DELAY_44_48_FAST,DELAY_4_3,DELAY_2xa};
const RESAMPLE_MEM int delay22_16a[2] = {DELAY_44_48_FAST,DELAY_2_3};
const RESAMPLE_MEM int delay22_48[2] = {DELAY_44_48_FAST,DELAY_2xa};
const RESAMPLE_MEM int delay11_48[2] = {DELAY_44_48_FAST,DELAY_4xa};
const RESAMPLE_MEM int delay64_44[2] = {DELAY_3_4,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay64_11[3] = {DELAY_4d,DELAY_48_44_FASTa,DELAY_3_4};
const RESAMPLE_MEM int delay64_12[2] = {DELAY_4d,DELAY_3_4};
const RESAMPLE_MEM int delay88_12[3] = {DELAY_4d,DELAY_44_48_FAST,DELAY_2d};
const RESAMPLE_MEM int delay96_11[3] = {DELAY_4d,DELAY_48_44_FASTa,DELAY_2d};
const RESAMPLE_MEM int delay64_22[3] = {DELAY_2d,DELAY_48_44_FASTa,DELAY_3_4};
const RESAMPLE_MEM int delay22_8a[2] = {DELAY_44_48_FAST,DELAY_3d};
const RESAMPLE_MEM int delay44_8a[3] = {DELAY_3d,DELAY_44_48_FAST,DELAY_2d};
const RESAMPLE_MEM int delay88_8[3] = {DELAY_6d,DELAY_44_48_FAST,DELAY_2d};
const RESAMPLE_MEM int delay64_8[2] = {DELAY_2d,DELAY_4d};
const RESAMPLE_MEM int delay88_24[2] = {DELAY_44_48_FAST,DELAY_4d};
const RESAMPLE_MEM int delay88_48[2] = {DELAY_44_48_FAST,DELAY_2d};
const RESAMPLE_MEM int delay96_44[2] = {DELAY_2d,DELAY_48_44_FASTa};
const RESAMPLE_MEM int delay96_22[2] = {DELAY_48_44_FASTa,DELAY_4d};
const RESAMPLE_MEM int delay_none[1] = {0};
const RESAMPLE_MEM int delay_48_16_low_latency[1] = {DELAY_48_16_low_latency};
const RESAMPLE_MEM int delay_16_8_low_latency[1] = {DELAY_16_8_low_latency};
const RESAMPLE_MEM int delay_48_8_low_latency[2] = {DELAY_48_16_low_latency,DELAY_16_8_low_latency};
const RESAMPLE_MEM int delay_8_16_low_latency[1] = {DELAY_8_16_low_latency};
const RESAMPLE_MEM int delay_16_48_low_latency[1] = {DELAY_16_48_low_latency};
const RESAMPLE_MEM int delay_8_48_low_latency[2] = {DELAY_8_16_low_latency,DELAY_16_48_low_latency};
#if defined(hifi) || defined(hifi_locoefs)
//************************* hifi COEFS ****************
#ifdef hifi_locoefs
const RESAMPLE_MEM int delay_up_2xH_lc[1] = {DELAY_2xaH};
const RESAMPLE_MEM int delay_up_3xH_lc[1] = {DELAY_3xaH};
const RESAMPLE_MEM int delay_up_4xH_lc[1] = {DELAY_4xaH};
const RESAMPLE_MEM int delay_up_6xH_lc[1] = {DELAY_6xaH};
const RESAMPLE_MEM int delay_down_2xH_lc[1] = {DELAY_2dH};
const RESAMPLE_MEM int delay_down_3xH_lc[1] = {DELAY_3dH};
const RESAMPLE_MEM int delay_down_4xH_lc[1] = {DELAY_4dH};
const RESAMPLE_MEM int delay_down_6xH_lc[1] = {DELAY_6dH};
const RESAMPLE_MEM int delay_down_12xH_lc[1] = {DELAY_12dH};
const RESAMPLE_MEM int delay48_44H_lc[2] = {DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay32_44aH_lc[3] = {DELAY_3_2H,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay24_44H_lc[3] = {DELAY_2xaH,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay16_44aH_lc[3] = {DELAY_3xaH,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay12_44H_lc[3] = {DELAY_4xaH,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay8_44aH_lc[3] = {DELAY_6xaH,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay12_8H_lc[1] = {DELAY_2_3H};
const RESAMPLE_MEM int delay64_48H_lc[1] = {DELAY_3_4H};
const RESAMPLE_MEM int delay64_24H_lc[2] = {DELAY_3_4H,DELAY_2dH};
const RESAMPLE_MEM int delay12_16H_lc[1] = {DELAY_4_3H};
const RESAMPLE_MEM int delay12_32H_lc[2] = {DELAY_4_3H,DELAY_2xaH};
const RESAMPLE_MEM int delay32_48H_lc[1] = {DELAY_3_2H};
const RESAMPLE_MEM int delay44_48H_lc[2] = {DELAY_7_5H,DELAY_7_9H};
const RESAMPLE_MEM int delay72_80H_lc[2] = {DELAY_12_9,DELAY_10_12};
const RESAMPLE_MEM int delay80_72H_lc[2] = {DELAY_12_10,DELAY_9_12};
const RESAMPLE_MEM int delay11_16H_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_4_3H};
const RESAMPLE_MEM int delay11_32H_lc[4] = {DELAY_7_5H,DELAY_7_9H,DELAY_4_3H,DELAY_2xaH};
const RESAMPLE_MEM int delay22_16aH_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_2_3H};
const RESAMPLE_MEM int delay22_48H_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_2xaH};
const RESAMPLE_MEM int delay11_48H_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_4xaH};
const RESAMPLE_MEM int delay64_44H_lc[3] = {DELAY_3_4H,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay64_11H_lc[4] = {DELAY_4dH,DELAY_9_7H,DELAY_5_7H,DELAY_3_4H};
const RESAMPLE_MEM int delay64_12H_lc[2] = {DELAY_4dH,DELAY_3_4H};
const RESAMPLE_MEM int delay88_12H_lc[4] = {DELAY_4dH,DELAY_7_5H,DELAY_7_9H,DELAY_2dH};
const RESAMPLE_MEM int delay96_11H_lc[4] = {DELAY_4dH,DELAY_9_7H,DELAY_5_7H,DELAY_2dH};
const RESAMPLE_MEM int delay64_22H_lc[4] = {DELAY_2dH,DELAY_9_7H,DELAY_5_7H,DELAY_3_4H};
const RESAMPLE_MEM int delay22_8aH_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_3dH};
const RESAMPLE_MEM int delay44_8aH_lc[4] = {DELAY_3dH,DELAY_7_5H,DELAY_7_9H,DELAY_2dH};
const RESAMPLE_MEM int delay88_8H_lc[4] = {DELAY_6dH,DELAY_7_5H,DELAY_7_9H,DELAY_2dH};
const RESAMPLE_MEM int delay64_8H_lc[2] = {DELAY_2dH,DELAY_4dH};
const RESAMPLE_MEM int delay88_24H_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_4dH};
const RESAMPLE_MEM int delay88_48H_lc[3] = {DELAY_7_5H,DELAY_7_9H,DELAY_2dH};
const RESAMPLE_MEM int delay96_44H_lc[3] = {DELAY_2dH,DELAY_9_7H,DELAY_5_7H};
const RESAMPLE_MEM int delay96_22H_lc[3] = {DELAY_9_7H,DELAY_5_7H,DELAY_4dH};
const RESAMPLE_MEM int delay_noneH_lc[1] = {0};
#else
const RESAMPLE_MEM int delay_up_2xH[1] = {DELAY_2xaH};
const RESAMPLE_MEM int delay_up_3xH[1] = {DELAY_3xaH};
const RESAMPLE_MEM int delay_up_4xH[1] = {DELAY_4xaH};
const RESAMPLE_MEM int delay_up_6xH[1] = {DELAY_6xaH};
const RESAMPLE_MEM int delay_down_2xH[1] = {DELAY_2dH};
const RESAMPLE_MEM int delay_down_3xH[1] = {DELAY_3dH};
const RESAMPLE_MEM int delay_down_4xH[1] = {DELAY_4dH};
const RESAMPLE_MEM int delay_down_6xH[1] = {DELAY_6dH};
const RESAMPLE_MEM int delay_down_12xH[1] = {DELAY_12dH};
const RESAMPLE_MEM int delay48_44H[1] = {DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay32_44aH[2] = {DELAY_3_2H,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay24_44H[2] = {DELAY_2xaH,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay16_44aH[2] = {DELAY_3xaH,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay12_44H[2] = {DELAY_4xaH,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay8_44aH[2] = {DELAY_6xaH,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay12_8H[1] = {DELAY_2_3H};
const RESAMPLE_MEM int delay64_48H[1] = {DELAY_3_4H};
const RESAMPLE_MEM int delay64_24H[2] = {DELAY_3_4H,DELAY_2dH};
const RESAMPLE_MEM int delay12_16H[1] = {DELAY_4_3H};
const RESAMPLE_MEM int delay12_32H[2] = {DELAY_4_3H,DELAY_2xaH};
const RESAMPLE_MEM int delay32_48H[1] = {DELAY_3_2H};
const RESAMPLE_MEM int delay44_48H[1] = {DELAY_44_48_FASTH};
const RESAMPLE_MEM int delay72_80H[2] = {DELAY_12_9,DELAY_10_12};
const RESAMPLE_MEM int delay80_72H[2] = {DELAY_12_10,DELAY_9_12};
const RESAMPLE_MEM int delay11_16H[2] = {DELAY_44_48_FASTH,DELAY_4_3H};
const RESAMPLE_MEM int delay11_32H[3] = {DELAY_44_48_FASTH,DELAY_4_3H,DELAY_2xaH};
const RESAMPLE_MEM int delay22_16aH[2] = {DELAY_44_48_FASTH,DELAY_2_3H};
const RESAMPLE_MEM int delay22_48H[2] = {DELAY_44_48_FASTH,DELAY_2xaH};
const RESAMPLE_MEM int delay11_48H[2] = {DELAY_44_48_FASTH,DELAY_4xaH};
const RESAMPLE_MEM int delay64_44H[2] = {DELAY_3_4H,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay64_11H[3] = {DELAY_4dH,DELAY_48_44_FASTaH,DELAY_3_4H};
const RESAMPLE_MEM int delay64_12H[2] = {DELAY_4dH,DELAY_3_4H};
const RESAMPLE_MEM int delay88_12H[3] = {DELAY_4dH,DELAY_44_48_FASTH,DELAY_2dH};
const RESAMPLE_MEM int delay96_11H[3] = {DELAY_4dH,DELAY_48_44_FASTaH,DELAY_2dH};
const RESAMPLE_MEM int delay64_22H[3] = {DELAY_2dH,DELAY_48_44_FASTaH,DELAY_3_4H};
const RESAMPLE_MEM int delay22_8aH[2] = {DELAY_44_48_FASTH,DELAY_3dH};
const RESAMPLE_MEM int delay44_8aH[3] = {DELAY_3dH,DELAY_44_48_FASTH,DELAY_2dH};
const RESAMPLE_MEM int delay88_8H[3] = {DELAY_6dH,DELAY_44_48_FASTH,DELAY_2dH};
const RESAMPLE_MEM int delay64_8H[2] = {DELAY_2dH,DELAY_4dH};
const RESAMPLE_MEM int delay88_24H[2] = {DELAY_44_48_FASTH,DELAY_4dH};
const RESAMPLE_MEM int delay88_48H[2] = {DELAY_44_48_FASTH,DELAY_2dH};
const RESAMPLE_MEM int delay96_44H[2] = {DELAY_2dH,DELAY_48_44_FASTaH};
const RESAMPLE_MEM int delay96_22H[2] = {DELAY_48_44_FASTaH,DELAY_4dH};
const RESAMPLE_MEM int delay_noneH[1] = {0};
#endif
#endif


static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtr[num_infreq][num_outfreq]={
    {delay_down_2x,delay96_44,delay_down_3x,delay_down_4x,delay96_22,delay_down_6x,delay64_8,delay96_11,delay_down_12x,delay_none}, //96
    {delay88_48,delay_down_2x,delay22_8a,delay88_24,delay_down_4x,delay44_8a,delay88_12,delay64_8,delay88_8,delay_none},            //88
    {delay64_48,delay64_44,delay_down_2x,delay64_24,delay64_22,delay_down_4x,delay64_12,delay64_11,delay64_8,delay_none},           //64
    {delay_none,delay48_44,delay12_8,delay_down_2x,delay96_44,delay_down_3x,delay_down_4x,delay96_22,delay_down_6x,delay_none},     //48
    {delay44_48,delay_none,delay22_16a,delay88_48,delay_down_2x,delay22_8a,delay88_24,delay_down_4x,delay44_8a,delay_none},         //44
    {delay32_48,delay32_44a,delay_none,delay64_48,delay64_44,delay_down_2x,delay64_24,delay64_22,delay_down_4x,delay_none},         //32
    {delay_up_2x,delay24_44,delay12_16,delay_none,delay48_44,delay12_8,delay_down_2x,delay96_44,delay_down_3x,delay_none},          //24
    {delay22_48,delay_up_2x,delay11_16,delay44_48,delay_none,delay22_16a,delay88_48,delay_down_2x,delay22_8a,delay_none},           //22
    {delay_up_3x,delay16_44a,delay_up_2x,delay32_48,delay32_44a,delay_none,delay64_48,delay64_44,delay_down_2x,delay_none},         //16
    {delay_up_4x,delay12_44,delay12_32,delay_up_2x,delay24_44,delay12_16,delay_none,delay48_44,delay12_8,delay_none},               //12
    {delay11_48,delay_up_4x,delay11_32,delay22_48,delay_up_2x,delay11_16,delay44_48,delay_none,delay22_16a,delay_none},             //11
    {delay_up_6x,delay8_44a,delay_up_4x,delay_up_3x,delay16_44a,delay_up_2x,delay32_48,delay32_44a,delay_none,delay80_72},          //8
    {delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay72_80,delay_none}                 //7
};
static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtrLR[num_infreq][num_outfreq]={
    {delay_down_2x,delay96_44,delay_down_3x,delay_down_4x,delay96_22,delay_down_6x,delay64_8,delay96_11,delay_down_12x,delay_none}, //96
    {delay88_48,delay_down_2x,delay22_8a,delay88_24,delay_down_4x,delay44_8a,delay88_12,delay64_8,delay88_8,delay_none},            //88
    {delay64_48,delay64_44,delay_down_2x,delay64_24,delay64_22,delay_down_4x,delay64_12,delay64_11,delay64_8,delay_none},           //64
    {delay_none,delay48_44,delay12_8,delay_down_2x,delay96_44,delay_down_3x,delay_down_4x,delay96_22,delay_down_6x,delay_none},     //48
    {delay44_48LR,delay_none,delay22_16a,delay88_48,delay_down_2x,delay22_8a,delay88_24,delay_down_4x,delay44_8a,delay_none},         //44
    {delay32_48,delay32_44a,delay_none,delay64_48,delay64_44,delay_down_2x,delay64_24,delay64_22,delay_down_4x,delay_none},         //32
    {delay_up_2x,delay24_44,delay12_16,delay_none,delay48_44,delay12_8,delay_down_2x,delay96_44,delay_down_3x,delay_none},          //24
    {delay22_48,delay_up_2x,delay11_16,delay44_48,delay_none,delay22_16a,delay88_48,delay_down_2x,delay22_8a,delay_none},           //22
    {delay_up_3x,delay16_44a,delay_up_2x,delay32_48,delay32_44a,delay_none,delay64_48,delay64_44,delay_down_2x,delay_none},         //16
    {delay_up_4x,delay12_44,delay12_32,delay_up_2x,delay24_44,delay12_16,delay_none,delay48_44,delay12_8,delay_none},               //12
    {delay11_48,delay_up_4x,delay11_32,delay22_48,delay_up_2x,delay11_16,delay44_48,delay_none,delay22_16a,delay_none},             //11
    {delay_up_6x,delay8_44a,delay_up_4x,delay_up_3x,delay16_44a,delay_up_2x,delay32_48,delay32_44a,delay_none,delay80_72},          //8
    {delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay_none,delay72_80,delay_none}                 //7
};

#if defined(hifi) || defined(hifi_locoefs)
#ifdef hifi_locoefs
static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtrH[num_infreq][num_outfreq]={
    {delay_down_2xH_lc,delay96_44H_lc,delay_down_3xH_lc,delay_down_4xH_lc,delay96_22H_lc,delay_down_6xH_lc,delay64_8H_lc,delay96_11H_lc,delay_down_12xH_lc,delay_noneH_lc}, //96
    {delay88_48H_lc,delay_down_2xH_lc,delay22_8aH_lc,delay88_24H_lc,delay_down_4xH_lc,delay44_8aH_lc,delay88_12H_lc,delay64_8H_lc,delay88_8H_lc,delay_noneH_lc},            //88
    {delay64_48H_lc,delay64_44H_lc,delay_down_2xH_lc,delay64_24H_lc,delay64_22H_lc,delay_down_4xH_lc,delay64_12H_lc,delay64_11H_lc,delay64_8H_lc,delay_noneH_lc},           //64
    {delay_noneH_lc,delay48_44H_lc,delay12_8H_lc,delay_down_2xH_lc,delay96_44H_lc,delay_down_3xH_lc,delay_down_4xH_lc,delay96_22H_lc,delay_down_6xH_lc,delay_noneH_lc},     //48
    {delay44_48H_lc,delay_noneH_lc,delay22_16aH_lc,delay88_48H_lc,delay_down_2xH_lc,delay22_8aH_lc,delay88_24H_lc,delay_down_4xH_lc,delay44_8aH_lc,delay_noneH_lc},         //44
    {delay32_48H_lc,delay32_44aH_lc,delay_noneH_lc,delay64_48H_lc,delay64_44H_lc,delay_down_2xH_lc,delay64_24H_lc,delay64_22H_lc,delay_down_4xH_lc,delay_noneH_lc},         //32
    {delay_up_2xH_lc,delay24_44H_lc,delay12_16H_lc,delay_noneH_lc,delay48_44H_lc,delay12_8H_lc,delay_down_2xH_lc,delay96_44H_lc,delay_down_3xH_lc,delay_noneH_lc},          //24
    {delay22_48H_lc,delay_up_2xH_lc,delay11_16H_lc,delay44_48H_lc,delay_noneH_lc,delay22_16aH_lc,delay88_48H_lc,delay_down_2xH_lc,delay22_8aH_lc,delay_noneH_lc},           //22
    {delay_up_3xH_lc,delay16_44aH_lc,delay_up_2xH_lc,delay32_48H_lc,delay32_44aH_lc,delay_noneH_lc,delay64_48H_lc,delay64_44H_lc,delay_down_2xH_lc,delay_noneH_lc},         //16
    {delay_up_4xH_lc,delay12_44H_lc,delay12_32H_lc,delay_up_2xH_lc,delay24_44H_lc,delay12_16H_lc,delay_noneH_lc,delay48_44H_lc,delay12_8H_lc,delay_noneH_lc},               //12
    {delay11_48H_lc,delay_up_4xH_lc,delay11_32H_lc,delay22_48H_lc,delay_up_2xH_lc,delay11_16H_lc,delay44_48H_lc,delay_noneH_lc,delay22_16aH_lc,delay_noneH_lc},             //11
    {delay_up_6xH_lc,delay8_44aH_lc,delay_up_4xH_lc,delay_up_3xH_lc,delay16_44aH_lc,delay_up_2xH_lc,delay32_48H_lc,delay32_44aH_lc,delay_noneH_lc,delay80_72H_lc},          //8
    {delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay_noneH_lc,delay72_80H_lc,delay_noneH_lc}                 //7
};
#else
static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtrH[num_infreq][num_outfreq]={
    {delay_down_2xH,delay96_44H,delay_down_3xH,delay_down_4xH,delay96_22H,delay_down_6xH,delay64_8H,delay96_11H,delay_down_12xH,delay_noneH},   //96
    {delay88_48H,delay_down_2xH,delay22_8aH,delay88_24H,delay_down_4xH,delay44_8aH,delay88_12H,delay64_8H,delay88_8H,delay_noneH},          //88
    {delay64_48H,delay64_44H,delay_down_2xH,delay64_24H,delay64_22H,delay_down_4xH,delay64_12H,delay64_11H,delay64_8H,delay_noneH},         //64
    {delay_noneH,delay48_44H,delay12_8H,delay_down_2xH,delay96_44H,delay_down_3xH,delay_down_4xH,delay96_22H,delay_down_6xH,delay_noneH},       //48
    {delay44_48H,delay_noneH,delay22_16aH,delay88_48H,delay_down_2xH,delay22_8aH,delay88_24H,delay_down_4xH,delay44_8aH,delay_noneH},           //44
    {delay32_48H,delay32_44aH,delay_noneH,delay64_48H,delay64_44H,delay_down_2xH,delay64_24H,delay64_22H,delay_down_4xH,delay_noneH},           //32
    {delay_up_2xH,delay24_44H,delay12_16H,delay_noneH,delay48_44H,delay12_8H,delay_down_2xH,delay96_44H,delay_down_3xH,delay_noneH},            //24
    {delay22_48H,delay_up_2xH,delay11_16H,delay44_48H,delay_noneH,delay22_16aH,delay88_48H,delay_down_2xH,delay22_8aH,delay_noneH},         //22
    {delay_up_3xH,delay16_44aH,delay_up_2xH,delay32_48H,delay32_44aH,delay_noneH,delay64_48H,delay64_44H,delay_down_2xH,delay_noneH},           //16
    {delay_up_4xH,delay12_44H,delay12_32H,delay_up_2xH,delay24_44H,delay12_16H,delay_noneH,delay48_44H,delay12_8H,delay_noneH},             //12
    {delay11_48H,delay_up_4xH,delay11_32H,delay22_48H,delay_up_2xH,delay11_16H,delay44_48H,delay_noneH,delay22_16aH,delay_noneH},               //11
    {delay_up_6xH,delay8_44aH,delay_up_4xH,delay_up_3xH,delay16_44aH,delay_up_2xH,delay32_48H,delay32_44aH,delay_noneH,delay80_72H},            //8
    {delay_noneH,delay_noneH,delay_noneH,delay_noneH,delay_noneH,delay_noneH,delay_noneH,delay_noneH,delay72_80H,delay_noneH}                   //7
};
#endif
#endif
static cascadedFilter * const RESAMPLE_MEM CascadedPtr[num_infreq][num_outfreq]={
    {fast_dwn_2x,Fast_96_44,fast_dwn_3x,fast_dwn_4x,Fast_96_22,fast_dwn_6x,Fast_64_8,Fast_96_11,fast_dwn_12x,Filter_none},  //96
    {Fast_88_48,fast_dwn_2x,Fast_22_8a,Fast_88_24,fast_dwn_4x,Fast_44_8a,Fast_88_12,Fast_64_8,Fast_88_8,Filter_none},           //88
    {Fast_64_48,Fast_64_44,fast_dwn_2x,Fast_64_24,Fast_64_22,fast_dwn_4x,Fast_64_12,Fast_64_11,Fast_64_8,Filter_none},      //64
    {Filter_none,Fast_48_44,Fast_12_8,fast_dwn_2x,Fast_96_44,fast_dwn_3x,fast_dwn_4x,Fast_96_22,fast_dwn_6x,Filter_none},       //48
    {Fast_44_48,Filter_none,Fast_22_16a,Fast_88_48,fast_dwn_2x,Fast_22_8a,Fast_88_24,fast_dwn_4x,Fast_44_8a,Filter_none},       //44
    {Fast_32_48,Fast_32_44a,Filter_none,Fast_64_48,Fast_64_44,fast_dwn_2x,Fast_64_24,Fast_64_22,fast_dwn_4x,Filter_none},       //32
    {fast_up_2x,Fast_24_44,Fast_12_16,Filter_none,Fast_48_44,Fast_12_8,fast_dwn_2x,Fast_96_44,fast_dwn_3x,Filter_none},         //24
    {Fast_22_48,fast_up_2x,Fast_11_16,Fast_44_48,Filter_none,Fast_22_16a,Fast_88_48,fast_dwn_2x,Fast_22_8a,Filter_none},        //22
    {fast_up_3x,Fast_16_44a,fast_up_2x,Fast_32_48,Fast_32_44a,Filter_none,Fast_64_48,Fast_64_44,fast_dwn_2x,Filter_none},       //16
    {fast_up_4x,Fast_12_44,Fast_12_32,fast_up_2x,Fast_24_44,Fast_12_16,Filter_none,Fast_48_44,Fast_12_8,Filter_none},           //12
    {Fast_11_48,fast_up_4x,Fast_11_32,Fast_22_48,fast_up_2x,Fast_11_16,Fast_44_48,Filter_none,Fast_22_16a,Filter_none},     //11
    {fast_up_6x,Fast_8_44a,fast_up_4x,fast_up_3x,Fast_16_44a,fast_up_2x,Fast_32_48,Fast_32_44a,Filter_none,Fast_80_72},         //8
    {Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Fast_72_80,Filter_none}    //7
};
static cascadedFilter * const RESAMPLE_MEM CascadedPtrLR[num_infreq][num_outfreq]={
    {fast_dwn_2x,Fast_96_44,fast_dwn_3x,fast_dwn_4x,Fast_96_22,fast_dwn_6x,Fast_64_8,Fast_96_11,fast_dwn_12x,Filter_none},  //96
    {Fast_88_48,fast_dwn_2x,Fast_22_8a,Fast_88_24,fast_dwn_4x,Fast_44_8a,Fast_88_12,Fast_64_8,Fast_88_8,Filter_none},           //88
    {Fast_64_48,Fast_64_44,fast_dwn_2x,Fast_64_24,Fast_64_22,fast_dwn_4x,Fast_64_12,Fast_64_11,Fast_64_8,Filter_none},      //64
    {Filter_none,Fast_48_44,Fast_12_8,fast_dwn_2x,Fast_96_44,fast_dwn_3x,fast_dwn_4x,Fast_96_22,fast_dwn_6x,Filter_none},       //48
    {Fast_44_48LR,Filter_none,Fast_22_16a,Fast_88_48,fast_dwn_2x,Fast_22_8a,Fast_88_24,fast_dwn_4x,Fast_44_8a,Filter_none},       //44
    {Fast_32_48,Fast_32_44a,Filter_none,Fast_64_48,Fast_64_44,fast_dwn_2x,Fast_64_24,Fast_64_22,fast_dwn_4x,Filter_none},       //32
    {fast_up_2x,Fast_24_44,Fast_12_16,Filter_none,Fast_48_44,Fast_12_8,fast_dwn_2x,Fast_96_44,fast_dwn_3x,Filter_none},         //24
    {Fast_22_48,fast_up_2x,Fast_11_16,Fast_44_48,Filter_none,Fast_22_16a,Fast_88_48,fast_dwn_2x,Fast_22_8a,Filter_none},        //22
    {fast_up_3x,Fast_16_44a,fast_up_2x,Fast_32_48,Fast_32_44a,Filter_none,Fast_64_48,Fast_64_44,fast_dwn_2x,Filter_none},       //16
    {fast_up_4x,Fast_12_44,Fast_12_32,fast_up_2x,Fast_24_44,Fast_12_16,Filter_none,Fast_48_44,Fast_12_8,Filter_none},           //12
    {Fast_11_48,fast_up_4x,Fast_11_32,Fast_22_48,fast_up_2x,Fast_11_16,Fast_44_48,Filter_none,Fast_22_16a,Filter_none},     //11
    {fast_up_6x,Fast_8_44a,fast_up_4x,fast_up_3x,Fast_16_44a,fast_up_2x,Fast_32_48,Fast_32_44a,Filter_none,Fast_80_72},         //8
    {Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Filter_none,Fast_72_80,Filter_none}    //7
};

#if defined(hifi) || defined(hifi_locoefs)
#ifdef hifi_locoefs
static cascadedFilter * const RESAMPLE_MEM CascadedPtrH[num_infreq][num_outfreq]={
    {fast_dwn_2xH_lc,Fast_96_44H_lc,fast_dwn_3xH_lc,fast_dwn_4xH_lc,Fast_96_22H_lc,fast_dwn_6xH_lc,Fast_64_8H_lc,Fast_96_11H_lc,fast_dwn_12xH_lc,Filter_noneH_lc},  //96
    {Fast_88_48H_lc,fast_dwn_2xH_lc,Fast_22_8aH_lc,Fast_88_24H_lc,fast_dwn_4xH_lc,Fast_44_8aH_lc,Fast_88_12H_lc,Fast_64_8H_lc,Fast_88_8H_lc,Filter_noneH_lc},           //88
    {Fast_64_48H_lc,Fast_64_44H_lc,fast_dwn_2xH_lc,Fast_64_24H_lc,Fast_64_22H_lc,fast_dwn_4xH_lc,Fast_64_12H_lc,Fast_64_11H_lc,Fast_64_8H_lc,Filter_noneH_lc},      //64
    {Filter_noneH_lc,Fast_48_44H_lc,Fast_12_8H_lc,fast_dwn_2xH_lc,Fast_96_44H_lc,fast_dwn_3xH_lc,fast_dwn_4xH_lc,Fast_96_22H_lc,fast_dwn_6xH_lc,Filter_noneH_lc},       //48
    {Fast_44_48H_lc,Filter_noneH_lc,Fast_22_16aH_lc,Fast_88_48H_lc,fast_dwn_2xH_lc,Fast_22_8aH_lc,Fast_88_24H_lc,fast_dwn_4xH_lc,Fast_44_8aH_lc,Filter_noneH_lc},       //44
    {Fast_32_48H_lc,Fast_32_44aH_lc,Filter_noneH_lc,Fast_64_48H_lc,Fast_64_44H_lc,fast_dwn_2xH_lc,Fast_64_24H_lc,Fast_64_22H_lc,fast_dwn_4xH_lc,Filter_noneH_lc},       //32
    {fast_up_2xH_lc,Fast_24_44H_lc,Fast_12_16H_lc,Filter_noneH_lc,Fast_48_44H_lc,Fast_12_8H_lc,fast_dwn_2xH_lc,Fast_96_44H_lc,fast_dwn_3xH_lc,Filter_noneH_lc},         //24
    {Fast_22_48H_lc,fast_up_2xH_lc,Fast_11_16H_lc,Fast_44_48H_lc,Filter_noneH_lc,Fast_22_16aH_lc,Fast_88_48H_lc,fast_dwn_2xH_lc,Fast_22_8aH_lc,Filter_noneH_lc},        //22
    {fast_up_3xH_lc,Fast_16_44aH_lc,fast_up_2xH_lc,Fast_32_48H_lc,Fast_32_44aH_lc,Filter_noneH_lc,Fast_64_48H_lc,Fast_64_44H_lc,fast_dwn_2xH_lc,Filter_noneH_lc},       //16
    {fast_up_4xH_lc,Fast_12_44H_lc,Fast_12_32H_lc,fast_up_2xH_lc,Fast_24_44H_lc,Fast_12_16H_lc,Filter_noneH_lc,Fast_48_44H_lc,Fast_12_8H_lc,Filter_noneH_lc},           //12
    {Fast_11_48H_lc,fast_up_4xH_lc,Fast_11_32H_lc,Fast_22_48H_lc,fast_up_2xH_lc,Fast_11_16H_lc,Fast_44_48H_lc,Filter_noneH_lc,Fast_22_16aH_lc,Filter_noneH_lc},     //11
    {fast_up_6xH_lc,Fast_8_44aH_lc,fast_up_4xH_lc,fast_up_3xH_lc,Fast_16_44aH_lc,fast_up_2xH_lc,Fast_32_48H_lc,Fast_32_44aH_lc,Filter_noneH_lc,Fast_80_72H_lc},         //8
    {Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Filter_noneH_lc,Fast_72_80H_lc,Filter_noneH_lc}    //7
};
#else
static cascadedFilter * const RESAMPLE_MEM CascadedPtrH[num_infreq][num_outfreq]={
    {fast_dwn_2xH,Fast_96_44H,fast_dwn_3xH,fast_dwn_4xH,Fast_96_22H,fast_dwn_6xH,Fast_64_8H,Fast_96_11H,fast_dwn_12xH,Filter_noneH},    //96
    {Fast_88_48H,fast_dwn_2xH,Fast_22_8aH,Fast_88_24H,fast_dwn_4xH,Fast_44_8aH,Fast_88_12H,Fast_64_8H,Fast_88_8H,Filter_noneH},         //88
    {Fast_64_48H,Fast_64_44H,fast_dwn_2xH,Fast_64_24H,Fast_64_22H,fast_dwn_4xH,Fast_64_12H,Fast_64_11H,Fast_64_8H,Filter_noneH},        //64
    {Filter_noneH,Fast_48_44H,Fast_12_8H,fast_dwn_2xH,Fast_96_44H,fast_dwn_3xH,fast_dwn_4xH,Fast_96_22H,fast_dwn_6xH,Filter_noneH},     //48
    {Fast_44_48H,Filter_noneH,Fast_22_16aH,Fast_88_48H,fast_dwn_2xH,Fast_22_8aH,Fast_88_24H,fast_dwn_4xH,Fast_44_8aH,Filter_noneH},     //44
    {Fast_32_48H,Fast_32_44aH,Filter_noneH,Fast_64_48H,Fast_64_44H,fast_dwn_2xH,Fast_64_24H,Fast_64_22H,fast_dwn_4xH,Filter_noneH},     //32
    {fast_up_2xH,Fast_24_44H,Fast_12_16H,Filter_noneH,Fast_48_44H,Fast_12_8H,fast_dwn_2xH,Fast_96_44H,fast_dwn_3xH,Filter_noneH},           //24
    {Fast_22_48H,fast_up_2xH,Fast_11_16H,Fast_44_48H,Filter_noneH,Fast_22_16aH,Fast_88_48H,fast_dwn_2xH,Fast_22_8aH,Filter_noneH},      //22
    {fast_up_3xH,Fast_16_44aH,fast_up_2xH,Fast_32_48H,Fast_32_44aH,Filter_noneH,Fast_64_48H,Fast_64_44H,fast_dwn_2xH,Filter_noneH},     //16
    {fast_up_4xH,Fast_12_44H,Fast_12_32H,fast_up_2xH,Fast_24_44H,Fast_12_16H,Filter_noneH,Fast_48_44H,Fast_12_8H,Filter_noneH},         //12
    {Fast_11_48H,fast_up_4xH,Fast_11_32H,Fast_22_48H,fast_up_2xH,Fast_11_16H,Fast_44_48H,Filter_noneH,Fast_22_16aH,Filter_noneH},       //11
    {fast_up_6xH,Fast_8_44aH,fast_up_4xH,fast_up_3xH,Fast_16_44aH,fast_up_2xH,Fast_32_48H,Fast_32_44aH,Filter_noneH,Fast_80_72H},           //8
    {Filter_noneH,Filter_noneH,Filter_noneH,Filter_noneH,Filter_noneH,Filter_noneH,Filter_noneH,Filter_noneH,Fast_72_80H,Filter_noneH}  //7
};
#endif
#endif

#ifdef hifi_locoefs
static const RESAMPLE_MEM int MaxCascadedPtrH[num_infreq][num_outfreq]={
    {sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_96_44H_lc)/div,sizeof(fast_dwn_3xH_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Fast_96_22H_lc)/div,sizeof(fast_dwn_6xH_lc)/div,sizeof(Fast_64_8H_lc)/div,sizeof(Fast_96_11H_lc)/div,sizeof(fast_dwn_12xH_lc)/div,sizeof(Filter_noneH_lc)/div},  //96

    {sizeof(Fast_88_48H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_22_8aH_lc)/div,sizeof(Fast_88_24H_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Fast_44_8aH_lc)/div,sizeof(Fast_88_12H_lc)/div,sizeof(Fast_64_8H_lc)/div,sizeof(Fast_88_8H_lc)/div,sizeof(Filter_noneH_lc)/div},           //88

    {sizeof(Fast_64_48H_lc)/div,sizeof(Fast_64_44H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_64_24H_lc)/div,sizeof(Fast_64_22H_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Fast_64_12H_lc)/div,sizeof(Fast_64_11H_lc)/div,sizeof(Fast_64_8H_lc)/div,sizeof(Filter_noneH_lc)/div},      //64

    {sizeof(Filter_noneH_lc)/div,sizeof(Fast_48_44H_lc)/div,sizeof(Fast_12_8H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_96_44H_lc)/div,sizeof(fast_dwn_3xH_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Fast_96_22H_lc)/div,sizeof(fast_dwn_6xH_lc)/div,sizeof(Filter_noneH_lc)/div},       //48

    {sizeof(Fast_44_48H_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_22_16aH_lc)/div,sizeof(Fast_88_48H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_22_8aH_lc)/div,sizeof(Fast_88_24H_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Fast_44_8aH_lc)/div,sizeof(Filter_noneH_lc)/div},       //44

    {sizeof(Fast_32_48H_lc)/div,sizeof(Fast_32_44aH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_64_48H_lc)/div,sizeof(Fast_64_44H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_64_24H_lc)/div,sizeof(Fast_64_22H_lc)/div,sizeof(fast_dwn_4xH_lc)/div,sizeof(Filter_noneH_lc)/div},       //32

    {sizeof(fast_up_2xH_lc)/div,sizeof(Fast_24_44H_lc)/div,sizeof(Fast_12_16H_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_48_44H_lc)/div,sizeof(Fast_12_8H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_96_44H_lc)/div,sizeof(fast_dwn_3xH_lc)/div,sizeof(Filter_noneH_lc)/div},         //24

    {sizeof(Fast_22_48H_lc)/div,sizeof(fast_up_2xH_lc)/div,sizeof(Fast_11_16H_lc)/div,sizeof(Fast_44_48H_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_22_16aH_lc)/div,sizeof(Fast_88_48H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Fast_22_8aH_lc)/div,sizeof(Filter_noneH_lc)/div},        //22

    {sizeof(fast_up_3xH_lc)/div,sizeof(Fast_16_44aH_lc)/div,sizeof(fast_up_2xH_lc)/div,sizeof(Fast_32_48H_lc)/div,sizeof(Fast_32_44aH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_64_48H_lc)/div,sizeof(Fast_64_44H_lc)/div,sizeof(fast_dwn_2xH_lc)/div,sizeof(Filter_noneH_lc)/div},       //16

    {sizeof(fast_up_4xH_lc)/div,sizeof(Fast_12_44H_lc)/div,sizeof(Fast_12_32H_lc)/div,sizeof(fast_up_2xH_lc)/div,sizeof(Fast_24_44H_lc)/div,sizeof(Fast_12_16H_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_48_44H_lc)/div,sizeof(Fast_12_8H_lc)/div,sizeof(Filter_noneH_lc)/div},           //12

    {sizeof(Fast_11_48H_lc)/div,sizeof(fast_up_4xH_lc)/div,sizeof(Fast_11_32H_lc)/div,sizeof(Fast_22_48H_lc)/div,sizeof(fast_up_2xH_lc)/div,sizeof(Fast_11_16H_lc)/div,sizeof(Fast_44_48H_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_22_16aH_lc)/div,sizeof(Filter_noneH_lc)/div},     //11

    {sizeof(fast_up_6xH_lc)/div,sizeof(Fast_8_44aH_lc)/div,sizeof(fast_up_4xH_lc)/div,sizeof(fast_up_3xH_lc)/div,sizeof(Fast_16_44aH_lc)/div,sizeof(fast_up_2xH_lc)/div,sizeof(Fast_32_48H_lc)/div,sizeof(Fast_32_44aH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_80_72H_lc)/div},         //8

    {sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Filter_noneH_lc)/div,sizeof(Fast_72_80H_lc)/div,sizeof(Filter_noneH_lc)/div}    //7
};
#endif






static const RESAMPLE_MEM int MaxCascadedPtr[num_infreq][num_outfreq]={
    {sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_96_22)/div,sizeof(fast_dwn_6x)/div,sizeof(Fast_64_8)/div,sizeof(Fast_96_11)/div,sizeof(fast_dwn_12x)/div,sizeof(Filter_none)/div},  //96

    {sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Fast_88_24)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_44_8a)/div,sizeof(Fast_88_12)/div,sizeof(Fast_64_8)/div,sizeof(Fast_88_8)/div,sizeof(Filter_none)/div},           //88

    {sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_64_24)/div,sizeof(Fast_64_22)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_64_12)/div,sizeof(Fast_64_11)/div,sizeof(Fast_64_8)/div,sizeof(Filter_none)/div},      //64

    {sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_96_22)/div,sizeof(fast_dwn_6x)/div,sizeof(Filter_none)/div},       //48

    {sizeof(Fast_44_48)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Fast_88_24)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_44_8a)/div,sizeof(Filter_none)/div},       //44

    {sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_64_24)/div,sizeof(Fast_64_22)/div,sizeof(fast_dwn_4x)/div,sizeof(Filter_none)/div},       //32

    {sizeof(fast_up_2x)/div,sizeof(Fast_24_44)/div,sizeof(Fast_12_16)/div,sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(Filter_none)/div},         //24

    {sizeof(Fast_22_48)/div,sizeof(fast_up_2x)/div,sizeof(Fast_11_16)/div,sizeof(Fast_44_48)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Filter_none)/div},        //22

    {sizeof(fast_up_3x)/div,sizeof(Fast_16_44a)/div,sizeof(fast_up_2x)/div,sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Filter_none)/div},       //16

    {sizeof(fast_up_4x)/div,sizeof(Fast_12_44)/div,sizeof(Fast_12_32)/div,sizeof(fast_up_2x)/div,sizeof(Fast_24_44)/div,sizeof(Fast_12_16)/div,sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(Filter_none)/div},           //12

    {sizeof(Fast_11_48)/div,sizeof(fast_up_4x)/div,sizeof(Fast_11_32)/div,sizeof(Fast_22_48)/div,sizeof(fast_up_2x)/div,sizeof(Fast_11_16)/div,sizeof(Fast_44_48)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Filter_none)/div},     //11

    {sizeof(fast_up_6x)/div,sizeof(Fast_8_44a)/div,sizeof(fast_up_4x)/div,sizeof(fast_up_3x)/div,sizeof(Fast_16_44a)/div,sizeof(fast_up_2x)/div,sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_80_72)/div},         //8

    {sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Fast_72_80)/div,sizeof(Filter_none)/div}    //7
};


static const RESAMPLE_MEM int MaxCascadedPtrLR[num_infreq][num_outfreq]={
    {sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_96_22)/div,sizeof(fast_dwn_6x)/div,sizeof(Fast_64_8)/div,sizeof(Fast_96_11)/div,sizeof(fast_dwn_12x)/div,sizeof(Filter_none)/div},  //96

    {sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Fast_88_24)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_44_8a)/div,sizeof(Fast_88_12)/div,sizeof(Fast_64_8)/div,sizeof(Fast_88_8)/div,sizeof(Filter_none)/div},           //88

    {sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_64_24)/div,sizeof(Fast_64_22)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_64_12)/div,sizeof(Fast_64_11)/div,sizeof(Fast_64_8)/div,sizeof(Filter_none)/div},      //64

    {sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_96_22)/div,sizeof(fast_dwn_6x)/div,sizeof(Filter_none)/div},       //48

    {sizeof(Fast_44_48LR)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Fast_88_24)/div,sizeof(fast_dwn_4x)/div,sizeof(Fast_44_8a)/div,sizeof(Filter_none)/div},       //44

    {sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_64_24)/div,sizeof(Fast_64_22)/div,sizeof(fast_dwn_4x)/div,sizeof(Filter_none)/div},       //32

    {sizeof(fast_up_2x)/div,sizeof(Fast_24_44)/div,sizeof(Fast_12_16)/div,sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_96_44)/div,sizeof(fast_dwn_3x)/div,sizeof(Filter_none)/div},         //24

    {sizeof(Fast_22_48)/div,sizeof(fast_up_2x)/div,sizeof(Fast_11_16)/div,sizeof(Fast_44_48)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Fast_88_48)/div,sizeof(fast_dwn_2x)/div,sizeof(Fast_22_8a)/div,sizeof(Filter_none)/div},        //22

    {sizeof(fast_up_3x)/div,sizeof(Fast_16_44a)/div,sizeof(fast_up_2x)/div,sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_64_48)/div,sizeof(Fast_64_44)/div,sizeof(fast_dwn_2x)/div,sizeof(Filter_none)/div},       //16

    {sizeof(fast_up_4x)/div,sizeof(Fast_12_44)/div,sizeof(Fast_12_32)/div,sizeof(fast_up_2x)/div,sizeof(Fast_24_44)/div,sizeof(Fast_12_16)/div,sizeof(Filter_none)/div,sizeof(Fast_48_44)/div,sizeof(Fast_12_8)/div,sizeof(Filter_none)/div},           //12

    {sizeof(Fast_11_48)/div,sizeof(fast_up_4x)/div,sizeof(Fast_11_32)/div,sizeof(Fast_22_48)/div,sizeof(fast_up_2x)/div,sizeof(Fast_11_16)/div,sizeof(Fast_44_48)/div,sizeof(Filter_none)/div,sizeof(Fast_22_16a)/div,sizeof(Filter_none)/div},     //11

    {sizeof(fast_up_6x)/div,sizeof(Fast_8_44a)/div,sizeof(fast_up_4x)/div,sizeof(fast_up_3x)/div,sizeof(Fast_16_44a)/div,sizeof(fast_up_2x)/div,sizeof(Fast_32_48)/div,sizeof(Fast_32_44a)/div,sizeof(Filter_none)/div,sizeof(Fast_80_72)/div},         //8

    {sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Filter_none)/div,sizeof(Fast_72_80)/div,sizeof(Filter_none)/div}    //7
};


static const RESAMPLE_MEM int * const RESAMPLE_MEM DelayPtrll[6]={
    delay_48_16_low_latency,
    delay_16_8_low_latency,
    delay_48_8_low_latency,
    delay_8_16_low_latency,
    delay_16_48_low_latency,
    delay_8_48_low_latency
};
static cascadedFilter * const RESAMPLE_MEM CascadedPtrll[6]={
    Fast_48_16_low_latency,
    Fast_16_8_low_latency,
    Fast_48_8_low_latency,
    Fast_8_16_low_latency,
    Fast_16_48_low_latency,
    Fast_8_48_low_latency
};
static const RESAMPLE_MEM int MaxCascadedPtrll[6]={
    sizeof(Fast_48_16_low_latency)/div,
    sizeof(Fast_16_8_low_latency)/div,
    sizeof(Fast_48_8_low_latency)/div,
    sizeof(Fast_8_16_low_latency)/div,
    sizeof(Fast_16_48_low_latency)/div,
    sizeof(Fast_8_48_low_latency)/div
};
int ckfor_lo_latency(int inrate, int outrate, int src_quality, int *indx)
{
    int temp=src_quality;
    if ((src_quality==SRC_LOW_LATENCY)||(src_quality==SRC_LOW_LATENCY_IN_MSEC))
    {
        if ((inrate==f48_infreq)&&((outrate==f16_outfreq)||(outrate==f48_outfreq)))
        {
            *indx=0;
            return temp;
        }
        if ((inrate==f16_infreq)&&((outrate==f8_outfreq)||(outrate==f16_outfreq)))
        {
            *indx=1;
            return temp;
        }
        if ((inrate==f48_infreq)&&((outrate==f8_outfreq)||(outrate==f48_outfreq)))
        {
            *indx=2;
            return temp;
        }
        if ((inrate==f8_infreq)&&((outrate==f16_outfreq)||(outrate==f8_outfreq)))
        {
            *indx=3;
            return temp;
        }
        if ((inrate==f16_infreq)&&((outrate==f48_outfreq)||(outrate==f16_outfreq)))
        {
            *indx=4;
            return temp;
        }
        if ((inrate==f8_infreq)&&((outrate==f48_outfreq)||(outrate==f8_outfreq)))
        {
            *indx=5;
            return temp;
        }
    }
    return 0;
}
int
resample_calc_heap_size_fixin_fixout(int Inrate,int Outrate,int src_quality,int *heapsiz, unsigned int blocksiz,unsigned int channel_nb)
{
	int *delay_ptr;
	cascadedFilter       *filterInfo;
    int maxCascade,inrate,outrate,indx,lolatency=0;
	unsigned int maxCascade_u;
    int i,dlylen=0,remainbuf_siz=sizeof(ResampleRemainbuf);
    if  (
              (src_quality!=SRC_STANDARD_QUALITY)
            &&(src_quality!=SRC_LOW_RIPPLE)
            &&(src_quality!=SRC_LOW_LATENCY)
            &&(src_quality!=SRC_LOW_LATENCY_IN_MSEC)
#if defined(hifi) || defined(hifi_locoefs)
            &&(src_quality!=SRC_HIFI)
#endif
        ) return 1;
	if (get_freq(Inrate,Outrate,src_quality,&inrate,&outrate)!=0)
		return 1;
	if (get_config(src_quality,inrate,outrate,&filterInfo,&maxCascade_u,&delay_ptr)==1) return 1;
	maxCascade=(int)maxCascade_u;
	for (i=0;i<maxCascade;i++)
	{
		dlylen+=delay_ptr[i]+MAX_VECTOR_LEN;
	}
    lolatency=ckfor_lo_latency(inrate,outrate,src_quality,&indx);
    if (lolatency!=0)
    {
        dlylen=0;
        maxCascade=MaxCascadedPtrll[indx];
        for (i=0;i<maxCascade;i++)
        {
            dlylen+=DelayPtrll[indx][i]+MAX_VECTOR_LEN;
        }
        if (lolatency==SRC_LOW_LATENCY_IN_MSEC) remainbuf_siz=0; //don't need remainbuf structure if nb in/out smpls is in time
    }
    *heapsiz=(sizeof(ResampleDelayLine)*channel_nb*maxCascade)+     //xxx (delay line structure)*(no of channels)*(no of stages)
        (remainbuf_siz*channel_nb)+                                 //xxx (remainbuf structure)*(no of channels)
        (sizeof(int)*nb_buf*dlylen*channel_nb);                     //xxx (accumeulated delay line lenth for all stages)*(no of channels)
    if (Inrate==Outrate)
        return 0;
    if (get_freq(Inrate,Outrate,src_quality,&inrate,&outrate)!=0)
        return 1;
    lolatency=ckfor_lo_latency(inrate,outrate,src_quality,&indx);
    if (lolatency!=SRC_LOW_LATENCY_IN_MSEC)  //dont need remainbuf if nb in/out samples is in time
    {
        if (inrate>=outrate)
            *heapsiz+=sizeof(int)*nb_buf*(blocksiz+channel_nb); //remainbuf in delay line
        else
            *heapsiz+=sizeof(int)*(blocksiz+channel_nb); //remainbuf at output
                                                                    //xxx (blocksiz) for remainbuf length
    }
//	maxCascade=maxcascaded[inrate][outrate];
	maxCascade=(int)maxCascade_u;
    if (lolatency!=0) maxCascade=MaxCascadedPtrll[indx];
    if (maxCascade>1)
    {
        if (lolatency!=SRC_LOW_LATENCY_IN_MSEC) //dont need tempbuf for low latency with fixed no of in/out samples in time
            *heapsiz+=sizeof(int)*(blocksiz+(blocksiz>>2)+(blocksiz>>3)+1); //tempbuf for multiple stages ratio of 1.375
                                                                    //xxx (1.375*blocksiz) for intermediate stage output samples
    }
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
    int max_heapsiz=0,actual_heapsiz,inrate,fin,fout,startfreq,stopfreq,temp;
    if  (
              (src_quality!=SRC_STANDARD_QUALITY)
            &&(src_quality!=SRC_LOW_RIPPLE)
            &&(src_quality!=SRC_LOW_LATENCY)
            &&(src_quality!=SRC_LOW_LATENCY_IN_MSEC)
#if defined(hifi) || defined(hifi_locoefs)
            &&(src_quality!=SRC_HIFI)
#endif
        ) return 1;

    if (Outrate!=0)
    {
        if (Inrate!=0) // If not 0, the Input frequency is known
        {
            startfreq=Inrate;
            stopfreq=Inrate;
        }
        else
        {
            if (conversion_type==0) //SRC_UNKNOWN
            {
                startfreq=ESAA_FREQ_96KHZ;
                stopfreq=ESAA_FREQ_7_2KHZ;
            }
            else
            {
                if (conversion_type==1) //SRC_UPSAMPLING
                {
                    startfreq=Outrate;
                    stopfreq=ESAA_FREQ_7_2KHZ;
                }
                else    //SRC_DOWNSAMPLING
                {
                    startfreq=ESAA_FREQ_96KHZ;
                    stopfreq=Outrate;
                }
            }
        }
        for (inrate=startfreq;inrate<=stopfreq;inrate++)
        {
            // if in lowlatency mode, verify that the frequencies are valid (8,16,48kHz) before checking
            get_freq(inrate,Outrate,src_quality,&fin,&fout);
            if (((src_quality!=SRC_LOW_LATENCY)&&(src_quality!=SRC_LOW_LATENCY_IN_MSEC))||ckfor_lo_latency(fin,fout,src_quality,&temp))
            {
                if (resample_calc_heap_size_fixin_fixout(inrate,Outrate,src_quality,&actual_heapsiz,blocksiz,channel_nb)==0)
                {
                    if (actual_heapsiz>max_heapsiz)
                        max_heapsiz=actual_heapsiz;
                }
            }
        }
    }
    else
    {
        if (Inrate!=0)
        {
            startfreq=Inrate;
            stopfreq=Inrate;
        }
        else
        {
            startfreq=ESAA_FREQ_96KHZ;
            stopfreq=ESAA_FREQ_7_2KHZ;
        }
        for (inrate=startfreq;inrate<=stopfreq;inrate++)
        {
            for (Outrate=ESAA_FREQ_48KHZ;Outrate<=ESAA_FREQ_7_2KHZ;Outrate++)
            {
                //------- heap size calculation conditions ----------------
                //1. UNKNOWN
                //2. UPSAMPLING&&(infreq<=outfreq)
                //3. DOWNSAMPLING&&(infreq>=outfreq)
                //---------------------------------------------------------
                if ((conversion_type==0)||((conversion_type==1)&&(inrate>=Outrate))||((conversion_type==2)&&(inrate<=Outrate)))
                {
                    // if in lowlatency mode, verify that the frequencies are valid (8,16,48kHz) before checking
                    get_freq(inrate,Outrate,src_quality,&fin,&fout);
                    if (((src_quality!=SRC_LOW_LATENCY)&&(src_quality!=SRC_LOW_LATENCY_IN_MSEC))||ckfor_lo_latency(fin,fout,src_quality,&temp))
                    {
                        if (resample_calc_heap_size_fixin_fixout(inrate,Outrate,src_quality,&actual_heapsiz,blocksiz,channel_nb)==0)
                        {
                            if (actual_heapsiz>max_heapsiz)
                                max_heapsiz=actual_heapsiz;
                        }
                    }
                }
            }
        }
    }
    *heapsiz=max_heapsiz;
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
	int i,j,temp,retval,vector_len=1;
#if !defined(MMDSP) || defined(ARM) || defined(NEON)
    int sz=(int)sizeof(ResampleContext);
    char *tmp=(char*)ctx;
    for (i=0;i<sz;i++)
        tmp[i]=0;
#else
    buffer_reset((char*)ctx,0,(int)sizeof(ResampleContext));
#endif
#if defined(ARM) || defined(NEON)
#ifdef M4
		vector_len=VECTOR_LEN_M4;
#else
		vector_len=VECTOR_LEN_CA9_MUL16_32;
#endif
#endif
	ctx->ARM_Sample16=1;
	ctx->sample_size=sizeof(short);
	retval=resample_init_core(heap,size_heap,
			ctx, Inrate, Outrate, src_quality,
			channel_nb, blocksiz,vector_len);
	if (retval==0)
	{
		for(j=0;j<(ctx->maxCascade);j++)
		{
			temp=1;
/*
			if (ctx->downsampling)
			{
				temp=RESAMPLE_SAMPLE16_SHORTI_INTO;
				if (j>0) temp=RESAMPLE_SAMPLE16_INTI_INTO;
			}
			else
			{
				temp=RESAMPLE_SAMPLE16_INTI_INTO;
				if (j==ctx->maxCascade-1) temp=RESAMPLE_SAMPLE16_INTI_SHORTO;
			}
			if (src_quality==SRC_LOW_LATENCY_IN_MSEC) 
				temp=RESAMPLE_SAMPLE16_SHORTI_SHORTO;
*/
			for(i=0;i<channel_nb;i++)
			{
				ctx->ctxChan[i].delay[j]->ARM_Sample16_config=temp; 
			}
		}
	}
	return retval;
}


AUDIO_API_EXPORT int
resample_x_init_ctx_low_mips_fixin_fixout(char *heap,int size_heap,
        ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
        unsigned int channel_nb, unsigned int blocksiz)
{
	int i,j,retval,vector_len=1,inrate,outrate,indx,Coef16;
#if !defined(MMDSP) || defined(ARM) || defined(NEON)
    int sz=(int)sizeof(ResampleContext);
    char *tmp=(char*)ctx;
    for (i=0;i<sz;i++)
        tmp[i]=0;
#else
    buffer_reset((char*)ctx,0,(int)sizeof(ResampleContext));
#endif
    if (get_freq(Inrate,Outrate,src_quality,&inrate,&outrate)!=0) return 1;
	if (ckfor_lo_latency(inrate,outrate,src_quality,&indx))
		Coef16=COEF16_ON;
	else
		Coef16=COEF16_OFF;
#if defined(ARM) || defined(NEON)
	if(Coef16) 
#ifdef M4
		vector_len=VECTOR_LEN_M4;
#else
		vector_len=VECTOR_LEN_CA9_MUL16_32;
#endif
	else
		vector_len=VECTOR_LEN_CA9_MUL32_64;
#endif
	ctx->sample_size=(int)(sizeof(int));
	ctx->ARM_Sample16=0;

	retval=resample_init_core(heap,size_heap,
			ctx, Inrate, Outrate, src_quality,
			channel_nb, blocksiz,vector_len);
	if (retval==0)
	{
		for(i=0;i<channel_nb;i++)
		{
    		for(j=0;j<(ctx->maxCascade);j++)
    		{
				ctx->ctxChan[i].delay[j]->ARM_Sample16_config=0;
			}
		}
	}
	return retval;
}


int resample_init_core(char *heap,int size_heap,
        ResampleContext *ctx, int Inrate,int Outrate,int src_quality,
        unsigned int channel_nb, unsigned int blocksiz, int vector_len)
{
    unsigned int i,j;
    int inrate,outrate;
    int indx,inbufsiz;
    int *data;
    int *dlyline_data=0,*delay_ptr;
    char *heap_ptr;
    int heap_max_memory;
    int heap_total_char;
#if defined(ARM) || defined(NEON)
    int dlylen,delta;
    int inptr,filtercoeflen;
    int lolatency;
    int dfindx;
    int drindx;
	long long  longtemp;
	short temp_s;
	int temp;
#endif
	short *CoefRam_s[RESAMPLE_MAX_CASCADE];
    resample_alloc_parameters alloc_params;
    if  (
              (src_quality!=SRC_STANDARD_QUALITY)
            &&(src_quality!=SRC_LOW_RIPPLE)
            &&(src_quality!=SRC_LOW_LATENCY)
            &&(src_quality!=SRC_LOW_LATENCY_IN_MSEC)
#if defined(hifi) || defined(hifi_locoefs)
            &&(src_quality!=SRC_HIFI)
#endif
        ) return 1;

    alloc_params.ptr=&heap_ptr;
    alloc_params.max_memory=&heap_max_memory;
    alloc_params.total_char=&heap_total_char;
    resample_init_alloc(heap,size_heap,&alloc_params);
    /* set context */
    ctx->processing = resample_x_process_fixin_fixout;
    if (get_freq(Inrate,Outrate,src_quality,&inrate,&outrate)!=0) return 1;
	if (get_config(src_quality,inrate,outrate,&ctx->filterInfo,&ctx->maxCascade,&delay_ptr)==1) return 1;
    ctx->low_latency=ckfor_lo_latency(inrate,outrate,src_quality,&indx);
    if (ctx->low_latency!=0)
    {
        ctx->filterInfo=CascadedPtrll[indx];
        ctx->maxCascade=MaxCascadedPtrll[indx];
    }
    ctx->block_siz=blocksiz;
    ctx->block_size_by_nb_channel=blocksiz;
    if(resample_ck_downsampling(Inrate,Outrate,ctx) != 0) return 1;
    ctx->flush_ack=0;
    ctx->flush_pending=0;
    ctx->flush_squelch=0;
    ctx->flush_ack_sav=0;
    ctx->flush_pending_sav=0;
    ctx->flush_squelch_sav=0;
    for(j=0;j<ctx->maxCascade;j++)
    {
        /*----------- initialize delay lines -----*/
        for(i=0;i<channel_nb;i++)
        {
            ctx->ctxChan[i].delay[j]=resample_alloc(1,sizeof(ResampleDelayLine),&alloc_params); //xxx (dly line struct size)*(nb chan)*(no. stages)
            if (ctx->ctxChan[i].delay[j]==NULL) return 1;
            ctx->ctxChan[i].delay[j]->currIndex = 0;
            ctx->ctxChan[i].delay[j]->writeIndex = 0;
            ctx->ctxChan[i].delay[j]->nbremain = 0;
            ctx->ctxChan[i].delay[j]->offset = ctx->filterInfo[j]->M;
        }
        for(i=0;i<channel_nb;i++)
        {
            if ((j==0)&&(ctx->upsampling!=0)&&((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC)) //dont need remainbuf if nb samples in/out is in time
                /* remainbuf is at the filter input in the first delayline in upsampling case */
                inbufsiz=nb_buf*(((int)blocksiz/channel_nb)+1);
            else
                inbufsiz=0;
            if (ckfor_lo_latency(inrate,outrate,src_quality,&indx)!=0)
			{
                dlyline_data=resample_alloc(nb_buf*DelayPtrll[indx][j]+inbufsiz+vector_len-1,sizeof(int),&alloc_params);
			}
            else
			{
				dlyline_data=resample_alloc(nb_buf*  delay_ptr[j]     +inbufsiz+vector_len-1,sizeof(int),&alloc_params);
			}
                                                            //xxx (remain buf in dly line=blocksiz) +(dly line length)*(nb chan)*(no. stages)
            if (dlyline_data==NULL) return 1;
			ctx->ctxChan[i].delay[j]->baseAddress=dlyline_data;
			ctx->ctxChan[i].delay[j]->len=delay_ptr[j];
			ctx->ctxChan[i].delay[j]->totalen=delay_ptr[j]+inbufsiz;
            if (ckfor_lo_latency(inrate,outrate,src_quality,&indx)!=0)
            {
                ctx->ctxChan[i].delay[j]->len=DelayPtrll[indx][j];
                ctx->ctxChan[i].delay[j]->totalen=DelayPtrll[indx][j]+inbufsiz;
				ctx->ctxChan[i].delay[j]->ARM_Coef16=COEF16_ON;
            }
			else
			{
				ctx->ctxChan[i].delay[j]->ARM_Coef16=COEF16_OFF;
			}
        }
        if (Inrate!=Outrate)
        {
            ctx->CoefRam[j]=resample_alloc(get_coef_ram_size(Inrate,Outrate,src_quality,j),1,&alloc_params); //xxx cpy coefs to ram for CA9
			CoefRam_s[j]=(short*)ctx->CoefRam[j];
        }
    }
    if ((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC) //dont need remainbuf for case of in/out sample length is in time
    {
        for(i=0;i<channel_nb;i++)
        {
            ctx->ctxChan[i].Resampleremainbuf = resample_alloc(1,sizeof(ResampleRemainbuf),&alloc_params); //xxx (nb channel)*(remainbuf struct size)
            if(ctx->ctxChan[i].Resampleremainbuf==NULL) return 1;
        }
        if (ctx->downsampling!=0)
        {
            for(i=0;i<channel_nb;i++)
            {
                /* remainbuf is at the filter output in downsampling case*/
                data = resample_alloc((blocksiz/channel_nb)+1,sizeof(int),&alloc_params); //xxx (blocksiz) for remainbuf if on output
                if( data == NULL ) return 1;
                ctx->ctxChan[i].Resampleremainbuf->baseAddress = data;
                ctx->ctxChan[i].Resampleremainbuf->writeptr=0;
                ctx->ctxChan[i].Resampleremainbuf->readptr=0;
                ctx->ctxChan[i].Resampleremainbuf->nbremain=0;
            }
        }
    }
    if (ctx->maxCascade>1)
    {
        if ((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC) //dont need tempbuf for low latency with fixed no of in/out samples in time
        {
            /* provide for temporary storage of samples at first output stage with max ratio of 1.375*/
            ctx->tempbuf1=resample_alloc(blocksiz+(blocksiz>>2)+(blocksiz>>3)+1,sizeof(int),&alloc_params); //xxx (1.375*blocksiz) forcascaded samples
            if (ctx->tempbuf1 == NULL) return 1;
        }
    }
    if (ctx->maxCascade>2)
    {
        if ((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC) //dont need tempbuf for low latency with fixed no of in/out samples in time
            /* provide for temporary toggle buffer storage of samples at additional output stage(s)*/
            ctx->tempbuf2=ctx->tempbuf1+(blocksiz/2)+1;
    }
#if defined(ARM) || defined(NEON)
    /*---------------------- Copy each stage's Coefs into contiguous Ram by phase pages -----------------------*/
    if (Inrate!=Outrate)
    {
        lolatency=ckfor_lo_latency(inrate,outrate,src_quality,&indx);
        for (j=0;j<ctx->maxCascade;j++)
        {
            dlylen=ctx->ctxChan[0].delay[j]->len;
            delta=dlylen&(vector_len-1);
            if (delta!=0)
                delta=vector_len-delta; //number of additional coefs needed to be a multiple of vector_len so the last ones can be set to 0
            filtercoeflen=ctx->filterInfo[j]->M*ctx->filterInfo[j]->decimno*dlylen;

            dfindx=ctx->filterInfo[j]->nzcoefs-
                dlylen*ctx->filterInfo[j]->M*ctx->filterInfo[j]->decimno+
                    ctx->filterInfo[j]->ncfs;
            drindx=ctx->filterInfo[j]->ncfs+2*ctx->filterInfo[j]->nzcoefs;
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
                    if (lolatency==0)
                    {
                        /*------------ normally, only half the coefs are available in YMEM -------*/
                        if (inptr>=filtercoeflen/2)
                        {
                            inptr=drindx-1-inptr;
                        }
					}
					longtemp=((long long)(ctx->filterInfo[j]->filter[inptr])*(long long)(ctx->filterInfo[j]->coef))<<1;
					if ((ctx->filterInfo[j]->shift)<0)
						longtemp<<=-(ctx->filterInfo[j]->shift);
					else
						longtemp>>=ctx->filterInfo[j]->shift;
					if ((ctx->ARM_Sample16)||(ctx->ctxChan[0].delay[j]->ARM_Coef16))
					{
/*
						longtemp=((long long)(ctx->filterInfo[j]->filter[inptr])*(long long)(ctx->filterInfo[j]->coef))<<1;
						if ((ctx->filterInfo[j]->shift)<0)
							longtemp<<=-(ctx->filterInfo[j]->shift);
						else
							longtemp>>=ctx->filterInfo[j]->shift;
*/
//#ifndef NEW
//						temp_s=(short)((longtemp+  0x80000000  )>>32); //store only the 16-bit LSB of the coefs
//						CoefRam_s[j][indx*(delta+dlylen)+i]=temp_s;
//#else
						temp=((longtemp+0x800000)>>24);
						if (ctx->ctxChan[0].delay[j]->ARM_Coef16)
						{
							temp_s=(short)((temp+0x80)>>8); // store 16-bit MSB of coef
							CoefRam_s[j][2*indx*(delta+dlylen)+i]=temp_s;
							temp_s=0; //store 8-bit LSB of coef
							CoefRam_s[j][(2*indx+1)*(delta+dlylen)+i]=temp_s;
						}
						else
						{
							temp_s=(short)(temp>>12); // store 12-bit MSB of coef
							CoefRam_s[j][2*indx*(delta+dlylen)+i]=temp_s;
							temp_s=(short)(temp&0xfff); //store 12-bit LSB of coef
							CoefRam_s[j][(2*indx+1)*(delta+dlylen)+i]=temp_s;
						}
//#endif
					}
					else
					{
						temp=((longtemp+0x800000)>>24);
						ctx->CoefRam[j][indx*(delta+dlylen)+i]=temp;
//                    	ctx->CoefRam[j][indx*(delta+dlylen)+i]=ctx->filterInfo[j]->filter[inptr];
					}
                }
				if ((ctx->ARM_Sample16==0)&&(ctx->ctxChan[0].delay[j]->ARM_Coef16==0))
				{
					for (i=dlylen;i<dlylen+delta;i++) ctx->CoefRam[j][indx*(delta+dlylen)+i]=0;
				}
				else
				{
//#ifdef NEW
                	for (i=dlylen;i<dlylen+delta;i++) CoefRam_s[j][2*indx*(delta+dlylen)+i]=0;
                	for (i=dlylen;i<dlylen+delta;i++) CoefRam_s[j][(2*indx+1)*(delta+dlylen)+i]=0;
//#else
//                	for (i=dlylen;i<dlylen+delta;i++) CoefRam_s[j][indx*(delta+dlylen)+i]=0;
//#endif
				}
            }
        }
    }
#endif
    return 0;
}
int
resample_x_cpy_0_1_ctx_low_mips_fixin_fixout(ResampleContext *ctx, int Inrate,int Outrate,int src_quality,unsigned int blocksiz)
{
    unsigned int j;
    int inrate,outrate;
    int inbufsiz,SIZ,k,indx;
	unsigned int maxCascade_u;
	cascadedFilter       *filterInfo;
	int *delay_ptr;
    if  (
              (src_quality!=SRC_STANDARD_QUALITY)
            &&(src_quality!=SRC_LOW_RIPPLE)
            &&(src_quality!=SRC_LOW_LATENCY)
            &&(src_quality!=SRC_LOW_LATENCY_IN_MSEC)
#if defined(hifi) || defined(hifi_locoefs)
            &&(src_quality!=SRC_HIFI)
#endif
        ) return 1;
    if (get_freq(Inrate,Outrate,src_quality,&inrate,&outrate)!=0)
        return 1;
	if (get_config(src_quality,inrate,outrate,&filterInfo,&maxCascade_u,&delay_ptr)==1) return 1;
    for(j=0;j<ctx->maxCascade;j++)
    {
        ctx->ctxChan[1].delay[j]->currIndex = ctx->ctxChan[0].delay[j]->currIndex;
        ctx->ctxChan[1].delay[j]->writeIndex = ctx->ctxChan[0].delay[j]->writeIndex;
        ctx->ctxChan[1].delay[j]->nbremain = ctx->ctxChan[0].delay[j]->nbremain;
        ctx->ctxChan[1].delay[j]->offset = ctx->ctxChan[0].delay[j]->offset;
        if ((j==0)&&(ctx->upsampling!=0)&&((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC)) //dont need remainbuf if nb samples in/out is in
            //remainbuf is at the filter input in the first delayline in upsampling case
            inbufsiz=nb_buf*(((int)blocksiz/2)+1);
        else
            inbufsiz=0;
		SIZ=(nb_buf*delay_ptr[j]+inbufsiz);
        if (ckfor_lo_latency(inrate,outrate,src_quality,&indx)!=0)
            SIZ=(nb_buf*DelayPtrll[indx][j]+inbufsiz);
        for (k=0;k<SIZ;k++)
        {
            (ctx->ctxChan[1].delay[j]->baseAddress)[k]=(ctx->ctxChan[0].delay[j]->baseAddress)[k];
        }
    }
    if ((ctx->low_latency)!=SRC_LOW_LATENCY_IN_MSEC) //no need for remainbuf if nb in/out samples is in time
    {
        if (ctx->downsampling!=0)
        {
            SIZ=(blocksiz/2)+1;
            for (k=0;k<SIZ;k++)
            {
                (ctx->ctxChan[1].Resampleremainbuf->baseAddress)[k]=(ctx->ctxChan[0].Resampleremainbuf->baseAddress)[k];
            }
            ctx->ctxChan[1].Resampleremainbuf->writeptr=ctx->ctxChan[0].Resampleremainbuf->writeptr;
            ctx->ctxChan[1].Resampleremainbuf->readptr=ctx->ctxChan[0].Resampleremainbuf->readptr;
            ctx->ctxChan[1].Resampleremainbuf->nbremain=ctx->ctxChan[0].Resampleremainbuf->nbremain;
        }
    }
    return 0;
}
int resample_ck_downsampling(int Inrate,int Outrate,ResampleContext *ctx)
{
    int inrate,outrate;
    if (Inrate>=ESAA_FREQ_LAST_IN_LIST) return 1;
    inrate=infreq[Inrate];
    if (inrate>=num_infreq) return 1;
    if (Outrate>=ESAA_FREQ_LAST_IN_LIST) return 1;
    outrate=outfreq[Outrate];
    if (outrate>=num_outfreq) return 1;
    inrate=freq_lookup[Inrate];
    if (inrate==ESAA_FREQ_UNKNOWNKHZ) return 1;
    outrate=freq_lookup[Outrate];
    if (outrate==ESAA_FREQ_UNKNOWNKHZ) return 1;
    if (inrate>outrate)
    {
        ctx->upsampling=0;
        ctx->downsampling=1;
    }
    else
    {
        if (outrate>inrate)
        {
            ctx->upsampling=1;
            ctx->downsampling=0;
        }
        else
        {
            ctx->upsampling=0;
            ctx->downsampling=0;
        }
    }
    ctx->fin_khz=inrate;
    ctx->fout_khz=outrate;
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
#if !defined(MMDSP) || defined(ARM) || defined(NEON)
    int i;
#endif
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
#if !defined(MMDSP) || defined(ARM) || defined(NEON)
        for (i=0;i<size_n;i++)
            base[i]=0;
#else
        buffer_reset((char *) base, 0,size_n);
#endif
    }
    (*param->ptr)+=size_n;
    return base;
}
int get_freq(int Inrate, int Outrate,int src_quality, int *inRate, int *outRate)
{
	unsigned int maxCascade_u;
	cascadedFilter       *filterInfo;
	int *delay_ptr;
    if (Inrate>=ESAA_FREQ_LAST_IN_LIST) return 1;
    *inRate=infreq[Inrate];
    if (*inRate>=num_infreq) return 1;
    if (Outrate>=ESAA_FREQ_LAST_IN_LIST) return 1;
    *outRate=outfreq[Outrate];
    if (*outRate>=num_outfreq) return 1;
	if (get_config(src_quality,*inRate,*outRate,&filterInfo,&maxCascade_u,&delay_ptr)==1) return 1;
    /*------------ check for undefined input and output frequency combinations -------*/
	if ((Inrate!= Outrate)&&(delay_ptr==delay_none)) return 1;
    if ((Inrate!= Outrate)&&(filterInfo==Filter_none)) return 1;
    return 0;
}
int get_coef_ram_size(int Inrate,int Outrate,int src_quality,int stage)
{
    int siz=sizeof(Float);
#if defined(ARM) || defined(NEON)
	int *delay_ptr;
	unsigned int maxCascade_u;
    int indx,inrate,outrate,dlylen,M,delta;
    cascadedFilter *tempfilt;
    if(get_freq(Inrate,Outrate,src_quality,&inrate,&outrate) != 0){
        return 1;
    }
	if (get_config(src_quality,inrate,outrate,&tempfilt,&maxCascade_u,&delay_ptr)==1) return 1;	
    if (ckfor_lo_latency(inrate,outrate,src_quality,&indx)!=0)
    {
        tempfilt=CascadedPtrll[indx];
    }
    siz=0;
    M=tempfilt[stage]->M;
	dlylen=delay_ptr[stage];
	if (ckfor_lo_latency(inrate,outrate,src_quality,&indx)!=0)
	{
		dlylen=DelayPtrll[indx][stage];
	}
    delta=dlylen&(MAX_VECTOR_LEN-1);
    if (delta!=0)
        dlylen=dlylen+MAX_VECTOR_LEN-delta; //number of coefs needs to be a multiple of vector_len so the last ones may be set to 0
    siz+=M*dlylen*sizeof(Float);
#endif // ARM or NEON
    return siz;
}


int resample_choose_processing_routine(ResampleContext *ctx)
{
    if ((ctx->low_latency)==SRC_LOW_LATENCY_IN_MSEC)
    {
        if(ctx->maxCascade == 1)
        {
            if(ctx->upsampling)
                ctx->processing = resample_ML_block_fixin_fixout_msec_up_maxCasc1;
            else if(ctx->downsampling)
                ctx->processing = resample_ML_block_fixin_fixout_msec_down_maxCasc1;
            else
                ctx->processing = resample_ML_block_fixin_fixout_msec_copy;
        }
        else
        {
            if(ctx->upsampling)
                ctx->processing = resample_ML_block_fixin_fixout_msec_up;
            else if(ctx->downsampling)
                ctx->processing = resample_ML_block_fixin_fixout_msec_down;
            else
                ctx->processing = resample_ML_block_fixin_fixout_msec_copy;
        }
        switch(ctx->fin_khz)
        {
            case 8:
                switch(ctx->fout_khz)
                {
                    case 8:
                        ctx->samples2msec = samples2msec_8_8khz;
                        break;
                    case 16:
                        ctx->samples2msec = samples2msec_8_16khz;
                        break;
                    case 48:
                        ctx->samples2msec = samples2msec_8_48khz;
                        break;
                    default:
                        return 1;
                }
                break;
            case 16:
                switch(ctx->fout_khz)
                {
                    case 8:
                        ctx->samples2msec = samples2msec_16_8khz;
                        break;
                    case 16:
                        ctx->samples2msec = samples2msec_16_16khz;
                        break;
                    case 48:
                        ctx->samples2msec = samples2msec_16_48khz;
                        break;
                    default:
                        return 1;
                }
                break;
            case 48:
                switch(ctx->fout_khz)
                {
                    case 8:
                        ctx->samples2msec = samples2msec_48_8khz;
                        break;
                    case 16:
                        ctx->samples2msec = samples2msec_48_16khz;
                        break;
                    case 48:
                        ctx->samples2msec = samples2msec_48_48khz;
                        break;
                    default:
                        return 1;
                }
                break;
            default:
                return 1;
        }
    }
    else
    {
        if(ctx->maxCascade == 1)
        {
            if(ctx->upsampling)
                ctx->processing = resample_ML_block_fixin_fixout_up_maxCasc1;
            else if(ctx->downsampling)
                ctx->processing = resample_ML_block_fixin_fixout_down_maxCasc1;
            else
                ctx->processing = resample_ML_block_fixin_fixout_copy;
        }
        else
        {
            if(ctx->upsampling)
                ctx->processing = resample_ML_block_fixin_fixout_up;
            else if(ctx->downsampling)
                ctx->processing = resample_ML_block_fixin_fixout_down;
            else
                ctx->processing = resample_ML_block_fixin_fixout_copy;
        }
    }
    
    return 0;
}
static int get_config(
		int src_quality,
		int inrate,
		int outrate,
		cascadedFilter **filterInfo,
		unsigned int *maxCascade,
		int **Delayptr
		)
{
#if defined(hifi) || defined(hifi_locoefs)
	if (src_quality==SRC_HIFI)
	{
#ifdef hifi_locoefs
		*filterInfo=CascadedPtrH[inrate][outrate];
		*maxCascade=MaxCascadedPtrH[inrate][outrate];
		*Delayptr=(int*)DelayPtrH[inrate][outrate];
		if (*Delayptr==NULL) return(1);
#else
		*filterInfo=CascadedPtrH[inrate][outrate];
		*maxCascade=MaxCascadedPtr[inrate][outrate];
		*Delayptr=(int*)DelayPtrH[inrate][outrate];
		if (*Delayptr==NULL) return(1);
#endif
	}
	else
#endif
		if (src_quality!=SRC_LOW_RIPPLE)
		{
			*filterInfo=CascadedPtr[inrate][outrate];
			*maxCascade=MaxCascadedPtr[inrate][outrate];
			*Delayptr=(int*)DelayPtr[inrate][outrate];
			if (*Delayptr==NULL) return(1);
		}
		else
		{
			*filterInfo=CascadedPtrLR[inrate][outrate];
			*maxCascade=MaxCascadedPtrLR[inrate][outrate];
			*Delayptr=(int*)DelayPtrLR[inrate][outrate];
			if (*Delayptr==NULL) return(1);
		}
	return 0;
}
#endif //ifndef M4_LL_MSEC


