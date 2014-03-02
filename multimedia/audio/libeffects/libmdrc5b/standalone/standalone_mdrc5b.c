/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/****************************************************************/
/* FILE: main.c                                                 */
/* AUTHOR: ZONG Wenbo                                           */
/* EMAIL: wenbo.zong@st.com                                     */
/* DIV: HED/AA&VoIP                                             */
/* DATE: 2009 May 20                                            */
/* LAST REV: 2009 May 23                                        */
/****************************************************************/
/* GOAL: defines the main function for testing mdrc5b in unix   */
/****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if defined(__arm__) && !defined(__arm)
// arm-linux only
#include <ctype.h>
#include <math.h>
#endif // __arm__ && !__arm
#include "get_param.h"
#include "mdrc5b.h"


#ifdef __arm__
//#define ARM_RVDEBUG_TEST1
//#define ARM_RVDEBUG_TEST2
//#define ARM_RVDEBUG_TEST3
//#define ARM_RVDEBUG_TEST4
//#define ARM_RVDEBUG_TEST5
//#define ARM_RVDEBUG_TEST6
//#define ARM_RVDEBUG_TEST7
//#define ARM_RVDEBUG_TEST8
//#define ARM_RVDEBUG_TEST9
//#define ARM_RVDEBUG_TEST10
//#define ARM_RVDEBUG_TEST11
//#define ARM_RVDEBUG_TEST12
//#define ARM_RVDEBUG_TEST13
//#define ARM_RVDEBUG_TEST14
//#define ARM_RVDEBUG_TEST15
//#define ARM_RVDEBUG_TEST16
//#define ARM_RVDEBUG_TEST17
//#define ARM_RVDEBUG_TEST18
//#define ARM_RVDEBUG_TEST19
//#define ARM_RVDEBUG_TEST20
//#define ARM_RVDEBUG_TEST21
//#define ARM_RVDEBUG_TEST22

#if defined(ARM_RVDEBUG_TEST1) || defined(ARM_RVDEBUG_TEST2) || defined(ARM_RVDEBUG_TEST3) || defined(ARM_RVDEBUG_TEST4)|| defined(ARM_RVDEBUG_TEST5)||\
           defined(ARM_RVDEBUG_TEST6)||defined(ARM_RVDEBUG_TEST7)||defined(ARM_RVDEBUG_TEST8)||defined(ARM_RVDEBUG_TEST9)||defined(ARM_RVDEBUG_TEST10)||\
          defined(ARM_RVDEBUG_TEST11)||defined(ARM_RVDEBUG_TEST12)||defined(ARM_RVDEBUG_TEST13)||defined(ARM_RVDEBUG_TEST14)||defined(ARM_RVDEBUG_TEST15)||\
          defined(ARM_RVDEBUG_TEST16)|| defined(ARM_RVDEBUG_TEST17)|| defined(ARM_RVDEBUG_TEST18)|| defined(ARM_RVDEBUG_TEST19) ||defined(ARM_RVDEBUG_TEST20)||\
          defined(ARM_RVDEBUG_TEST21)|| defined(ARM_RVDEBUG_TEST22)
#define ARM_RVDEBUG
#endif // ARM_RVDEBUG_TEST1 || ARM_RVDEBUG_TEST2 || ARM_RVDEBUG_TEST3 || ARM_RVDEBUG_TEST4|| ARM_RVDEBUG_TEST5...||ARM_RVDEBUG_TEST22

#include "../../../tools/ARM/perf/include/tperf_api.h"
#ifdef __arm
extern void core_init(void);            // RVCT only
#else /* __arm */
#include "Profile.h"                    // arm-linux only
#endif /* __arm */

#ifdef ARM_CYCLES
int PMU_index      [NB_PMU_MEASURES];
int cpt_PMU_measure[NB_PMU_MEASURES];

static const struct
{
    char          name[50];
    t_PMU_measure index;
} PMU_measure_name[NB_PMU_MEASURES] =
{
    {"MRDC5B_MAIN",                           PMU_MEASURE_MRDC5B_MAIN                          },
    {"MRDC5B_READ",                           PMU_MEASURE_MRDC5B_READ                          },
    {"MRDC5B_WRITE",                          PMU_MEASURE_MRDC5B_WRITE                         },
    {"MRDC5B_MOVE",                           PMU_MEASURE_MRDC5B_MOVE                          },
    {"MRDC5B_APPLY_COMPRESSOR",               PMU_MEASURE_MRDC5B_APPLY_COMPRESSOR              },
    {"MRDC5B_LEGACY_FILTER",                  PMU_MEASURE_MRDC5B_LEGACY_FILTER                 },
    {"MRDC5B_BIQUAD_FILTER",                  PMU_MEASURE_MRDC5B_BIQUAD_FILTER                 },
    {"MRDC5B_FIR_FILTER",                     PMU_MEASURE_MRDC5B_FIR_FILTER                    },
    {"MRDC5B_COMPRESSOR_INSERT_NEW_SUBBAND",  PMU_MEASURE_MRDC5B_COMPRESSOR_INSERT_NEW_SUBBAND },
    {"MRDC5B_COMPRESSOR_COMPUTE_POWER",       PMU_MEASURE_MRDC5B_COMPRESSOR_COMPUTE_POWER      },
    {"MRDC5B_COMPRESSOR_SMOOTH_AND_MAX",      PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_AND_MAX     },
    {"MRDC5B_COMPRESSOR_GAIN",                PMU_MEASURE_MRDC5B_COMPRESSOR_GAIN               },
    {"MRDC5B_COMPRESSOR_SMOOTH_GAIN",         PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_GAIN        },
    {"MRDC5B_COMPRESSOR_OUTPUT",              PMU_MEASURE_MRDC5B_COMPRESSOR_OUTPUT             },
    {"MRDC5B_APPLY_LIMITER",                  PMU_MEASURE_MRDC5B_APPLY_LIMITER                 },
    {"MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT", PMU_MEASURE_MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT},
    {"MRDC5B_LIMITER_INSERT_NEW_SUBBAND",     PMU_MEASURE_MRDC5B_LIMITER_INSERT_NEW_SUBBAND    },
    {"MRDC5B_LIMITER_SMOOTH_GAIN",            PMU_MEASURE_MRDC5B_LIMITER_SMOOTH_GAIN           },
    {"MRDC5B_LIMITER_APPLY_GAIN",             PMU_MEASURE_MRDC5B_LIMITER_APPLY_GAIN            },
};
#endif // ARM_CYCLES
#endif // __arm__

#ifndef __flexcc2__
#define winit_circ_ptr(a,b,c)      (a)
#define winit_circ_ymem_ptr(a,b,c) (a)
#endif // !__flexcc2__

//#define ENABLE_PROFILING
//#define SECOND_TEST

#ifdef ENABLE_PROFILING
static int count = 0, a_cycles = 0, a_samples = 0, frm_highest_mips, frame_count=0;
static int diff_buff_fill = 0;
#endif

static Mdrc5bAlgoConfig_t    EXTMEM mMdrc5bAlgoConfig;
static MdrcConfig_t          EXTMEM mMdrcConfig;


static MDRC5B_LOCAL_STRUCT_T EXTMEM mdrc5b_local_struct;
static t_filter_buffer       EXTMEM filter_buffer;
static t_mdrc_filters        EXTMEM mdrc_filters;

// use union for X memory because memory needs depend on parameters and all fields are not needed at the same time
static union
{
    t_filter_mem                    filter_mem;
    MMshort                         biquad_mem          [2][MDRC_BANDS_MAX - 1][2][2 * MDRC_BIQUAD_ORDER_MAX + 2];
    MMshort                         FIR_coef               [MDRC_BANDS_MAX]       [MDRC_FIR_ORDER_MAX];
} union_X_mem;

// use union for Y memory because memory needs depend on parameters and all fields are not needed at the same time
static union
{
    t_filter_coef                   filter_coef;
    t_biquad_cell                   biquad_cell         [2][MDRC_BANDS_MAX - 1]   [MDRC_BIQUAD_ORDER_MAX];
    struct
    {
        MMshort                     FIR_common_mem      [2][MDRC_FIR_ORDER_MAX + MDRC5B_BLOCK_SIZE];
        MMshort                     FIR_common_mem_optim   [(MDRC_FIR_ORDER_MAX + 1) / 2];
    } FIR_common_mem;
} YMEM union_Y_mem;

static MMshort               EXTMEM *inbuf;
static MMshort               EXTMEM *outbuf;
static short                 EXTMEM sizebuf     = 48;
static int                   EXTMEM sample_freq = 0;
static int                   EXTMEM in_offset, out_offset;
static int                   EXTMEM in_chan;
static t_mdrc_filter_kind    EXTMEM mdrc_filter_kind;
static t_biquad_kind         EXTMEM biquad_kind;
static t_FIR_kind            EXTMEM FIR_kind;


/* this function prepare all internal params depending on static input parameters */
void setParameter(void)
{
    // set the parameters that remain unchanged till the effect is destroyed
    mdrc5b_local_struct.mdrc_filter_kind = mdrc_filter_kind;
    mdrc5b_local_struct.biquad_kind      = biquad_kind;
    mdrc5b_local_struct.FIR_kind         = FIR_kind;
    mdrc5b_local_struct.SampleFreq       = sample_freq;
    mdrc5b_local_struct.NumMainCh        = in_chan;
    mdrc5b_local_struct.DataInOffset     = in_offset;
    mdrc5b_local_struct.DataOutOffset    = out_offset;
}

void my_open(void)
{
    /* setParameter() must be called before this function */
    /* call init function in library */
    int i, j;

    mdrc5b_local_struct.p_filter_buffer = &filter_buffer;
    switch(mdrc5b_local_struct.mdrc_filter_kind)
    {
        case MDRC_LEGACY_FILTER:
            mdrc5b_local_struct.p_filter_coef  = &union_Y_mem.filter_coef;
            mdrc5b_local_struct.p_filter_mem   = &union_X_mem.filter_mem;
            mdrc5b_local_struct.p_mdrc_filters = NULL;
            break;

        case MDRC_BIQUAD_FILTER:
            mdrc5b_local_struct.p_filter_coef  = NULL;
            mdrc5b_local_struct.p_filter_mem   = NULL;
            mdrc5b_local_struct.p_mdrc_filters = &mdrc_filters;

            mdrc5b_local_struct.p_mdrc_filters->mdrc_filter_kind = mdrc5b_local_struct.mdrc_filter_kind;
            mdrc5b_local_struct.p_mdrc_filters->nb_channels      = mdrc5b_local_struct.NumMainCh;

            for(i = 0; i < MDRC_BANDS_MAX; i++)
            {
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = NULL;
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[0]    = NULL;
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[1]    = NULL;
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[0]    = NULL;
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[1]    = NULL;
            }

            for(i = 0; i < MDRC_BANDS_MAX; i++)
            {
                if(i < MDRC_BANDS_MAX - 1)
                {
                    mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = union_Y_mem.biquad_cell[0][i];
                }
                if(i > 0)
                {
                    mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = union_Y_mem.biquad_cell[1][i - 1];
                }

                for(j = 0; j < mdrc5b_local_struct.p_mdrc_filters->nb_channels; j++)
                {
                    if(i < MDRC_BANDS_MAX - 1)
                    {
                        mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j] = union_X_mem.biquad_mem[0][i][j];
                    }
                    if(i > 0)
                    {
                        mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j] = union_X_mem.biquad_mem[1][i - 1][j];
                    }
                }
            }
            break;

        case MDRC_FIR_FILTER:
            mdrc5b_local_struct.p_filter_coef  = NULL;
            mdrc5b_local_struct.p_filter_mem   = NULL;
            mdrc5b_local_struct.p_mdrc_filters = &mdrc_filters;

            mdrc5b_local_struct.p_mdrc_filters->mdrc_filter_kind = mdrc5b_local_struct.mdrc_filter_kind;
            mdrc5b_local_struct.p_mdrc_filters->nb_channels      = mdrc5b_local_struct.NumMainCh;

            for(i = 0; i < MDRC_BANDS_MAX; i++)
            {
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef = NULL;
            }
            mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0]    = winit_circ_ymem_ptr(NULL, NULL, NULL);
            mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[1]    = winit_circ_ymem_ptr(NULL, NULL, NULL);
            mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = NULL;

            for(i = 0; i < MDRC_BANDS_MAX; i++)
            {
                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef = union_X_mem.FIR_coef[i];
            }
            for(j = 0; j < mdrc5b_local_struct.p_mdrc_filters->nb_channels; j++)
            {
                MMshort YMEM *p_FIR_mem = union_Y_mem.FIR_common_mem.FIR_common_mem[j];

                mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j] = winit_circ_ymem_ptr(p_FIR_mem, p_FIR_mem, p_FIR_mem + MDRC_FIR_ORDER_MAX + MDRC5B_BLOCK_SIZE);
            }
            mdrc5b_local_struct.p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = union_Y_mem.FIR_common_mem.FIR_common_mem_optim;
            break;

        default:
            break;
    }

    mdrc5b_init(&mdrc5b_local_struct);

    if(mMdrc5bAlgoConfig.Mode & COMPRESSOR)
    {
        switch(mdrc_filter_kind)
        {
            case MDRC_BIQUAD_FILTER:
            case MDRC_FIR_FILTER:
                if(get_mdrc_filter_config(mdrc5b_local_struct.p_mdrc_filters,
                                          &mMdrcConfig.MdrcFiltersConfig,
                                          mdrc5b_local_struct.mdrc_filter_kind,
                                          mdrc5b_local_struct.biquad_kind,
                                          mdrc5b_local_struct.FIR_kind) != 0)
                {
                    assert(FALSE);
                }
                break;

            default:
                break;
        }
    }
}

void my_reset(void)
{
    mdrc5b_reset(&mdrc5b_local_struct);
}


// The config function contains mdrc5b-specific processing, and should be
// faithfully reproduced when using mdrc5b library in any framework.
void setConfig(FILE *file_param)
{
    int  num_band, i;
    long mode, bands, Enable, FreqCutoff, KneePoints, AttackTime, ReleaseTime, PostGain, DynamicResponseX, DynamicResponseY, RmsMeasure, Thresh, filter_kind;
    char str[64];


    mode = -1;
    get_param(file_param, (char *) "mode", 'd', &mode);
    printf("mode: %ld\n", mode);
    if((mode < 0) || (mode > 3))
    {
        fprintf(stderr, "\n mode is incorrect\n");
        exit(2);
    }
    mMdrc5bAlgoConfig.Mode = mode;


#ifdef MDRC5B_COMPRESSOR_ACTIVE
    if(mode & COMPRESSOR)
    {
        bands = -1;
        get_param(file_param, (char *) "bands", 'd', &bands);
        printf("bands: %ld\n", bands);
        if((bands > 5) || (bands < 1))
        {
            fprintf(stderr, "\n Number of subbands is incorrect. Minimum is 1, maximum is 5\n");
            exit(5);
        }
        mMdrc5bAlgoConfig.NumBands = bands;

        for(num_band = 0; num_band < (int) bands; num_band++)
        {
            Enable      = -1;
            FreqCutoff  = -1;
            KneePoints  = -1;
            AttackTime  = -1;
            ReleaseTime = -1;
            PostGain    = -1;

            sprintf(str, "Enable%d", num_band + 1);
            get_param(file_param, str, 'd', &Enable);
            printf("%s: %ld\n", str, Enable);
            sprintf(str, "FreqCutoff%d", num_band + 1);
            get_param(file_param, str, 'd', &FreqCutoff);
            printf("%s: %ld\n", str, FreqCutoff);

            sprintf(str, "KneePoints%d", num_band + 1);
            get_param(file_param, str, 'd', &KneePoints);
            printf("%s: %ld\n", str, KneePoints);
            for(i = 0; i < (int) KneePoints; i++)
            {
                DynamicResponseX = -1;
                DynamicResponseY = -1;

                sprintf(str, "DynamicResponse%dX%d", num_band + 1, i + 1);
                get_param(file_param, str, 'd', &DynamicResponseX);
                printf("%s: %ld\n", str, DynamicResponseX);
                sprintf(str, "DynamicResponse%dY%d", num_band + 1, i + 1);
                get_param(file_param, str, 'd', &DynamicResponseY);
                printf("%s: %ld\n", str, DynamicResponseY);

                mMdrc5bAlgoConfig.BandCompressors[num_band].DynamicResponse[i][0] = DynamicResponseX;
                mMdrc5bAlgoConfig.BandCompressors[num_band].DynamicResponse[i][1] = DynamicResponseY;
            }

            sprintf(str, "AttackTime%d", num_band + 1);
            get_param(file_param, str, 'd', &AttackTime);
            printf("%s: %ld\n", str, AttackTime);
            sprintf(str, "ReleaseTime%d", num_band + 1);
            get_param(file_param, str, 'd', &ReleaseTime);
            printf("%s: %ld\n", str, ReleaseTime);
            sprintf(str, "PostGain%d", num_band + 1);
            get_param(file_param, str, 'd', &PostGain);
            printf("%s: %ld\n", str, PostGain);

            mMdrc5bAlgoConfig.BandCompressors[num_band].Enable      = Enable;
            mMdrc5bAlgoConfig.BandCompressors[num_band].FreqCutoff  = FreqCutoff;
            mMdrc5bAlgoConfig.BandCompressors[num_band].KneePoints  = KneePoints;
            mMdrc5bAlgoConfig.BandCompressors[num_band].AttackTime  = AttackTime;
            mMdrc5bAlgoConfig.BandCompressors[num_band].ReleaseTime = ReleaseTime;
            mMdrc5bAlgoConfig.BandCompressors[num_band].PostGain    = PostGain;
        }
        for(; num_band < 5; num_band++)
        {
            mMdrc5bAlgoConfig.BandCompressors[num_band].Enable      = FALSE;
        }

        filter_kind = (long) MDRC_LEGACY_FILTER;
        get_param(file_param, (char *) "MdrcFilterKind", 'd', &filter_kind);
        printf("MdrcFilterKind:       %11ld", filter_kind);
        mdrc_filter_kind = (t_mdrc_filter_kind) filter_kind;
        switch(mdrc_filter_kind)
        {
            case MDRC_LEGACY_FILTER:
                printf(" (MDRC_LEGACY_FILTER)\n");
                break;
            case MDRC_BIQUAD_FILTER:
                printf(" (MDRC_BIQUAD_FILTER)\n");
                break;
            case MDRC_FIR_FILTER:
                printf(" (MDRC_FIR_FILTER)\n");
                break;
            default :
                printf("\n");
                break;
        }

        biquad_kind                            = BUTTERWORTH_BIQUAD; // default value
        FIR_kind                               = SYMMETRIC_FIR;      // default value
        mMdrcConfig.MdrcFiltersConfig.nb_bands = (t_uint16) bands;
        switch(mdrc_filter_kind)
        {
            case MDRC_BIQUAD_FILTER:
                for(num_band = 0; num_band < (int) (bands - 1); num_band++)
                {
                    long local_biquad_kind, biquad_order, b_exp, b0, b1, b2, a1, a2;

                    local_biquad_kind = -1;
                    sprintf(str, "MdrcBiquadLP%dKind", num_band);
                    get_param(file_param, str, 'd', &local_biquad_kind);
                    printf("%s:    %11ld", str, local_biquad_kind);
                    switch((t_biquad_kind) local_biquad_kind)
                    {
                        case BUTTERWORTH_BIQUAD:
                            printf(" (BUTTERWORTH_BIQUAD)\n");
                            break;
                        case OTHER_BIQUAD:
                            biquad_kind = OTHER_BIQUAD;
                            printf(" (OTHER_BIQUAD)\n");
                            break;
                        default :
                            printf("\n");
                            break;
                    }
                    mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_kind = (t_uint16) local_biquad_kind;

                    biquad_order = -1;
                    sprintf(str, "MdrcBiquadLP%dOrder", num_band);
                    get_param(file_param, str, 'd', &biquad_order);
                    printf("%s:   %11ld\n", str, biquad_order);
                    mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_order = (t_uint16) biquad_order;

                    for(i = 0; i < (int) biquad_order; i++)
                    {
                        b_exp = b0 = b1 = b2 = a1 = a2 = -1;

                        sprintf(str, "MdrcBiquadLP%d_%db_exp", num_band, i);
                        get_param(file_param, str, 'd', &b_exp);
                        printf("%s: %11ld\n", str, b_exp);

                        sprintf(str, "MdrcBiquadLP%d_%db0", num_band, i);
                        get_param(file_param, str, 'd', &b0);
                        printf("%s:    %11ld\n", str, b0);

                        sprintf(str, "MdrcBiquadLP%d_%db1", num_band, i);
                        get_param(file_param, str, 'd', &b1);
                        printf("%s:    %11ld\n", str, b1);

                        sprintf(str, "MdrcBiquadLP%d_%db2", num_band, i);
                        get_param(file_param, str, 'd', &b2);
                        printf("%s:    %11ld\n", str, b2);

                        sprintf(str, "MdrcBiquadLP%d_%da1", num_band, i);
                        get_param(file_param, str, 'd', &a1);
                        printf("%s:    %11ld\n", str, a1);

                        sprintf(str, "MdrcBiquadLP%d_%da2", num_band, i);
                        get_param(file_param, str, 'd', &a2);
                        printf("%s:    %11ld\n", str, a2);

                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b_exp   = (t_uint16)   b_exp;
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b0_low  = (t_uint16)  (b0        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b0_high = (t_uint16) ((b0 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b1_low  = (t_uint16)  (b1        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b1_high = (t_uint16) ((b1 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b2_low  = (t_uint16)  (b2        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].b2_high = (t_uint16) ((b2 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].a1_low  = (t_uint16)  (a1        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].a1_high = (t_uint16) ((a1 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].a2_low  = (t_uint16)  (a2        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadLP.biquad_cell_int16[i].a2_high = (t_uint16) ((a2 >> 16) & 0xFFFF);
                    }

                    local_biquad_kind = -1;
                    sprintf(str, "MdrcBiquadHP%dKind", num_band);
                    get_param(file_param, str, 'd', &local_biquad_kind);
                    printf("%s:    %11ld", str, local_biquad_kind);
                    switch((t_biquad_kind) local_biquad_kind)
                    {
                        case BUTTERWORTH_BIQUAD:
                            printf(" (BUTTERWORTH_BIQUAD)\n");
                            break;
                        case OTHER_BIQUAD:
                            biquad_kind = OTHER_BIQUAD;
                            printf(" (OTHER_BIQUAD)\n");
                            break;
                        default :
                            printf("\n");
                            break;
                    }
                    mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_kind = (t_uint16) local_biquad_kind;

                    biquad_order = -1;
                    sprintf(str, "MdrcBiquadHP%dOrder", num_band);
                    get_param(file_param, str, 'd', &biquad_order);
                    printf("%s:   %11ld\n", str, biquad_order);
                    mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_order = (t_uint16) biquad_order;

                    for(i = 0; i < (int) biquad_order; i++)
                    {
                        b_exp = b0 = b1 = b2 = a1 = a2 = -1;

                        sprintf(str, "MdrcBiquadHP%d_%db_exp", num_band, i);
                        get_param(file_param, str, 'd', &b_exp);
                        printf("%s: %11ld\n", str, b_exp);

                        sprintf(str, "MdrcBiquadHP%d_%db0", num_band, i);
                        get_param(file_param, str, 'd', &b0);
                        printf("%s:    %11ld\n", str, b0);

                        sprintf(str, "MdrcBiquadHP%d_%db1", num_band, i);
                        get_param(file_param, str, 'd', &b1);
                        printf("%s:    %11ld\n", str, b1);

                        sprintf(str, "MdrcBiquadHP%d_%db2", num_band, i);
                        get_param(file_param, str, 'd', &b2);
                        printf("%s:    %11ld\n", str, b2);

                        sprintf(str, "MdrcBiquadHP%d_%da1", num_band, i);
                        get_param(file_param, str, 'd', &a1);
                        printf("%s:    %11ld\n", str, a1);

                        sprintf(str, "MdrcBiquadHP%d_%da2", num_band, i);
                        get_param(file_param, str, 'd', &a2);
                        printf("%s:    %11ld\n", str, a2);

                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b_exp   = (t_uint16)   b_exp;
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b0_low  = (t_uint16)  (b0        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b0_high = (t_uint16) ((b0 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b1_low  = (t_uint16)  (b1        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b1_high = (t_uint16) ((b1 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b2_low  = (t_uint16)  (b2        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].b2_high = (t_uint16) ((b2 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].a1_low  = (t_uint16)  (a1        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].a1_high = (t_uint16) ((a1 >> 16) & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].a2_low  = (t_uint16)  (a2        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_biquad[num_band].biquadHP.biquad_cell_int16[i].a2_high = (t_uint16) ((a2 >> 16) & 0xFFFF);
                    }
                }
                break;

            case MDRC_FIR_FILTER:
                for(num_band = 0; num_band < (int) bands; num_band++)
                {
                    long local_FIR_kind, order, gain_exp, coef;

                    local_FIR_kind = -1;
                    sprintf(str, "MdrcFIR%dKind", num_band);
                    get_param(file_param, str, 'd', &local_FIR_kind);
                    printf("%s:         %11ld", str, local_FIR_kind);
                    switch((t_FIR_kind) local_FIR_kind)
                    {
                        case SYMMETRIC_FIR:
                            printf(" (SYMMETRIC_FIR)\n");
                            break;
                        case OTHER_FIR:
                            FIR_kind = OTHER_FIR;
                            printf(" (OTHER_FIR)\n");
                            break;
                        default :
                            printf("\n");
                            break;
                    }
                    mMdrcConfig.MdrcFiltersConfig.mdrc_FIR[num_band].FIR_kind = (t_uint16) local_FIR_kind;

                    order = -1;
                    sprintf(str, "MdrcFIR%dOrder", num_band);
                    get_param(file_param, str, 'd', &order);
                    printf("%s:        %11ld\n", str, order);
                    mMdrcConfig.MdrcFiltersConfig.mdrc_FIR[num_band].FIR.order = (t_uint16) order;

                    gain_exp = -1;
                    sprintf(str, "MdrcFIR%dGainExp", num_band);
                    get_param(file_param, str, 'd', &gain_exp);
                    printf("%s:      %11ld\n", str, gain_exp);
                    mMdrcConfig.MdrcFiltersConfig.mdrc_FIR[num_band].FIR.gain_exp = (t_uint16) gain_exp;

                    for(i = 0; i < (int) order; i++)
                    {
                        coef = -1;
                        sprintf(str, "MdrcFIR%dCoef%d", num_band, i);
                        get_param(file_param, str, 'd', &coef);
                        if(i < 10)
                        {
                            printf("%s:        %11ld\n", str, coef);
                        }
                        else
                        {
                            printf("%s:       %11ld\n", str, coef);
                        }
                        mMdrcConfig.MdrcFiltersConfig.mdrc_FIR[num_band].FIR.coef[i].low  = (t_uint16)  (coef        & 0xFFFF);
                        mMdrcConfig.MdrcFiltersConfig.mdrc_FIR[num_band].FIR.coef[i].high = (t_uint16) ((coef >> 16) & 0xFFFF);
                    }
                }
                break;

            default:
                break;
        }
    }
#endif // #ifdef MDRC5B_COMPRESSOR_ACTIVE

#ifdef MDRC5B_LIMITER_ACTIVE
    if(mode & LIMITER)
    {
        // limiter is active
        Enable     = -1;
        RmsMeasure = -1;
        Thresh     = -1;

        get_param(file_param, (char *) "LimiterEnable", 'd', &Enable);
        printf("LimiterEnable: %ld\n", Enable);
        get_param(file_param, (char *) "LimiterRmsMeasure", 'd', &RmsMeasure);
        printf("RmsMeasure: %ld\n", RmsMeasure);
        get_param(file_param, (char *) "LimiterThresh", 'd', &Thresh);
        printf("LimiterThresh: %ld\n", Thresh);

        mMdrc5bAlgoConfig.Limiter.Enable     = Enable;
        mMdrc5bAlgoConfig.Limiter.RmsMeasure = RmsMeasure;
        mMdrc5bAlgoConfig.Limiter.Thresh     = Thresh;
    }
#endif
}

void processBuffer(MMshort *in, MMshort *out, int size)
{
    mdrc5b_main(&mdrc5b_local_struct, (void *) in, (void *) out, (int) (size * sizeof(MMshort)));
}



int main(int argc, char **argv)
{
    FILE  *file_mainin = NULL, *file_mainout = NULL, *file_param = NULL;
    char  filename_mainout[200];
    long  niter = 0;
    long  i;
    int   n, j;
    long  cpt = 0;
    int   last_percent = -1, percent;
    int   frmCnt = 0;
    int   little_endian = 1;
    int   low, high;
    int   sample;
#ifdef SECOND_TEST
    char  name[256];
#endif // SECOND_TEST

#ifdef __flexcc2__
    long  max_cycle_count = 0,  max_cycle_frame_nb = 0;
    long  start_count, stop_count;   // For locating worst case frame
#endif //__flexcc2__

#ifdef ENABLE_PROFILING
    FILE  *fp = fopen("Profiling.txt" , "a");
    float max_mips=0;
    long  Frame_Number = 0;
    int   prev_buff_fill, after_buff_fill, diff;
#endif

#ifdef __arm__
#ifdef ARM_CYCLES
    int index_PMU_measures;
#endif // ARM_CYCLES


#ifdef __arm
    core_init();            // RVCT only
#else /* __arm */
    FILE *pcache_evc_file;
    char pcache_evc_start[1024];
    char pcache_evc_end  [1024];
    typedef struct
    {
        char      name[50];
        int       cpt, index_min, index_max;
        int       min, max;
        long long sum, sum2;
    } t_stat_evc;
    t_stat_evc evc_stat[7];
    char evc_list[6][3] = {"10", "60", "61", "68", "74", "8B"};
    /*
    const char *l1_events_string[] =
    {
        "Soft Increment",//0x0,
        "Instr Cache Miss",//0x1,
        "Instr TLB Miss",//0x2,
        "Data Cache Miss",//0x3,
        "Data Cache Access",//0x4,
        "Data TLB Miss",//0x5,
        "Data Read Exec",//0x6,
        "Data Write Exec",//0x7,
        "Exception Taken",//0x9,
        "Exception return Exec",//0xA,
        "Context ID",//0xB,
        "Soft PC change",//0xC,
        "Immediate Branch Exec",//0xD,
        "Unaligned Access Exec",//0xF,
        "Branch miss/not predicted",//0x10,
        "Cycle Count",//0x11,
        "Branch predicted",//0x12,
        "Java BC Executed",//0x40,
        "Soft Java BC Executed",//0x41,
        "Jazelle BB Executed",//0x42,
        "Coherent Line Miss",//0x50,
        "Coherent Line Hit",//0x51,
        "Instr Cache Stall",//0x60,
        "Data Cache Stall",//0x61,
        "Main TLB Miss Stall",//0x62,
        "STREX passed",//0x63,
        "STREX failed",//0x64,
        "Data Eviction",//0x65,
        "Inst not dispatch",//0x66,
        "Issue Empty",//0x67,
        "Instr Executed Approx",//0x68,
        "Function Return",//0x6E,
        "Main Inst Exec",//0x70,
        "Second Inst Exec",//0x71,
        "LD/ST Inst Exec",//0x72,
        "FPU Inst Exec",//0x73,
        "Neon Inst Exec",//0x74,
        "PLD Stalls",//0x80,
        "Write Buffer Stalls",//0x81,
        "Inst Main TLB Miss Stalls",//0x82,
        "Data Main TLB Miss Stalls",//0x83,
        "Inst Micro TLB Miss Stalls",//0x84,
        "Data Micro TLB Miss Stalls",//0x85,
        "DMB Stalls",//0x86,
        "Integer Core Cycle Count",//0x8A,
        "Data Engine Cycle Count",//0x8B,
        "ISB Inst",//0x90,
        "DSB Inst",//0x91,
        "DMB Inst",//0x92,
        "External Interrupts",//0x93,
        "PLE Cache Line Request Done",//0xA0,
        "PLE Cache Line Request Skipped",//0xA1,
        "PLE Fifo Flush",//0xA2,
        "PLE Request Done",//0xA3,
        "PLE Fifo Overflow",//0xA4,
        "PLE Request programmed",//0xA5,
        "Unsupported" // Extra for unknown/unsupported value
    };
    */
#endif /* __arm */
#endif /* __arm__ */


//    {
//        signed char      x_char       = 1;
//        signed short     x_short      = 1;
//        signed int       x_int        = 1;
//        signed long      x_long       = 1;
//        signed long long x_longlong   = 1;
//        MMshort          x_MMshort    = 1;
//        MMlong           x_MMlong     = 1;
//        MMlonglong       x_MMlonglong = 1;
//        int              nb_bits;
//
//        printf("\n############################\n");
//        printf("TYPES SIZES :\n");
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_char < 0)
//            {
//                printf("char is %d bits\n", nb_bits);
//                break;
//            }
//            x_char <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_short < 0)
//            {
//                printf("short is %d bits\n", nb_bits);
//                break;
//            }
//            x_short <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_int < 0)
//            {
//                printf("int is %d bits\n", nb_bits);
//                break;
//            }
//            x_int <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_long < 0)
//            {
//                printf("long is %d bits\n", nb_bits);
//                break;
//            }
//            x_long <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_longlong < 0)
//            {
//                printf("long long is %d bits\n", nb_bits);
//                break;
//            }
//            x_longlong <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_MMshort < 0)
//            {
//                printf("MMshort is %d bits\n", nb_bits);
//                break;
//            }
//            x_MMshort <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_MMlong < 0)
//            {
//                printf("MMlong is %d bits\n", nb_bits);
//                break;
//            }
//            x_MMlong <<= 1;
//        }
//        for(nb_bits = 1; nb_bits < 100; nb_bits++)
//        {
//            if(x_MMlonglong < 0)
//            {
//                printf("MMlonglong is %d bits\n", nb_bits);
//                break;
//            }
//            x_MMlonglong <<= 1;
//        }
//        printf("############################\n\n");
//    }



#ifdef ARM_RVDEBUG


#ifdef ARM_RVDEBUG_TEST1
    strcpy(filename_mainout, "FIR_stereo_48k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/stereo_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST1

#ifdef ARM_RVDEBUG_TEST2
    strcpy(filename_mainout, "IIR_stereo_48k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_IIR_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/stereo_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST2

#ifdef ARM_RVDEBUG_TEST3
    strcpy(filename_mainout, "FIR_stereo_48k_rms_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_stereo_48k_rms",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/stereo_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST3

#ifdef ARM_RVDEBUG_TEST4
    strcpy(filename_mainout, "FIR_freq_sweep_48k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST4

#ifdef ARM_RVDEBUG_TEST5
    strcpy(filename_mainout, "FIR_freq_sweep_44k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_44k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_44k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST5

#ifdef ARM_RVDEBUG_TEST6
    strcpy(filename_mainout, "FIR_freq_sweep_32k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_32k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_32k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST6

#ifdef ARM_RVDEBUG_TEST7
    strcpy(filename_mainout, "FIR_freq_sweep_24k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_24k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_24k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST7

#ifdef ARM_RVDEBUG_TEST8
    strcpy(filename_mainout, "FIR_freq_sweep_22k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_22k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_22k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST8

#ifdef ARM_RVDEBUG_TEST9
    strcpy(filename_mainout, "FIR_freq_sweep_16k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_16k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_16k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST9

#ifdef ARM_RVDEBUG_TEST10
    strcpy(filename_mainout, "FIR_freq_sweep_12k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_12k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_12k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST10

#ifdef ARM_RVDEBUG_TEST11
    strcpy(filename_mainout, "FIR_freq_sweep_11k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_11k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_11k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST11

#ifdef ARM_RVDEBUG_TEST12
    strcpy(filename_mainout, "FIR_freq_sweep_8k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_freq_sweep_8k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/freq_sweep_8k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST12

#ifdef ARM_RVDEBUG_TEST13
    strcpy(filename_mainout, "IIR_sin1k_stereo_48k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_IIR_sin1k_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/sin1k_48k_stereo.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST13

#ifdef ARM_RVDEBUG_TEST14
    strcpy(filename_mainout, "IIR_sin1k_mono_48k_CA9.pcm");
       file_param   = fopen("../../../streams/mdrc/streams/params_IIR_sin1k_mono_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/sin1k_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST14

#ifdef ARM_RVDEBUG_TEST15
    strcpy(filename_mainout, "FIR_sin1k_stereo_48k_CA9.pcm");  
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_sin1k_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/sin1k_48k_stereo.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST15

#ifdef ARM_RVDEBUG_TEST16
    strcpy(filename_mainout, "FIR_sin1k_mono_48k_CA9.pcm");  
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_sin1k_mono_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/sin1k_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST16

#ifdef ARM_RVDEBUG_TEST17
    strcpy(filename_mainout, "FIR_mozart_mono_48k_CA9.pcm");  
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_mozart_mono_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/mozart_48k_mono.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST17

#ifdef ARM_RVDEBUG_TEST18
    strcpy(filename_mainout, "FIR_mozart_stereo_48k_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_mozart_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/mozart_48k_stereo.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST18

#ifdef ARM_RVDEBUG_TEST19
    strcpy(filename_mainout, "IIR_mozart_mono_48k_CA9.pcm"); 
    file_param   = fopen("../../../streams/mdrc/streams/params_IIR_mozart_mono_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/mozart_48k_mono.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST19

#ifdef ARM_RVDEBUG_TEST20
    strcpy(filename_mainout, "IIR_mozart_stereo_48k_CA9.pcm");    
    file_param   = fopen("../../../streams/mdrc/streams/params_IIR_mozart_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/mozart_48k_stereo.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST20

#ifdef ARM_RVDEBUG_TEST21
    strcpy(filename_mainout, "IIR_mult_ampl_sweep_stereo_48k_CA9.pcm");   
    file_param   = fopen("../../../streams/mdrc/streams/params_IIR_mult_ampl_sweep_stereo_48k",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/multitone_ampl_sweep_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST21

#ifdef ARM_RVDEBUG_TEST22
    strcpy(filename_mainout, "FIR_stereo_48k_1band_CA9.pcm");
    file_param   = fopen("../../../streams/mdrc/streams/params_FIR_stereo_48k_1band",    "rb");
    file_mainin  = fopen("../../../streams/mdrc/streams/stereo_48k.pcm", "rb");
    file_mainout = fopen(filename_mainout,                           "wb");
#endif // ARM_RVDEBUG_TEST22

    if(file_param == NULL)
    {
        fprintf(stderr, "can't open file_param !\n");
        return -1;
    }
    if(file_mainin == NULL)
    {
        fprintf(stderr, "can't open file_mainin !\n");
        return -1;
    }
    if(file_mainout == NULL)
    {
        fprintf(stderr, "can't open file_mainout !\n");
        return -1;
    }

    in_offset  = in_chan;
    out_offset = in_offset;

    fseek(file_mainin, 0, SEEK_END);
    niter = ftell(file_mainin) / 2 / in_chan;
    rewind(file_mainin);

    setConfig(file_param);

    /* call setParameter function */
    setParameter();

    /* initialization */
    my_open();

#else // ARM_RVDEBUG

    if ((argc != 7) && (argc != 8))
    {
        printf("\n Usage :");
        printf("\n mdrc5b <param_file> <main_input_file> <main_output_file> <ch_no> <fs> <block_size> [endianness]\n");
        printf("     <param_file>       - Text file containing the configuration parameters (see the sample file) \n");
        printf("     <main_input_file>  - PCM file for main channels \n");
        printf("     <main_ouput_file>  - PCM file, channel mode same as input\n");
        printf("     <ch_no>            - Number of main channels in main_input_file, must be {1,2}\n");
        printf("     <fs>               - sampling rate \n");
        printf("     <block_size>       - Number of samples per frame \n");
        printf("     [endianness]       - L for little-endian (default), B for big-endian\n\n");
        exit(1);
    }

    in_chan = atoi(argv[4]);
    if ((in_chan > 2) || (in_chan < 1))
    {
        fprintf(stderr, "\n Number of main channels is incorrect. Minimum is 1, maximum is 2\n");
        exit(3);
    }
    in_offset  = in_chan;
    out_offset = in_offset;

    sample_freq = atoi(argv[5]);
    printf("sample_freq = %d \n", sample_freq);
    switch(sample_freq)
    {
        case 48000:
        case 44100:
        case 32000:
        case 24000:
        case 22050:
        case 16000:
        case 12000:
        case 11025:
        case 8000:
            break;
        default:
            fprintf(stderr, "\n Sampling frequency not supported\n");
            exit(3);
    }

    sizebuf = atoi(argv[6]);

    if (argc == 8)
    {
        if ((argv[7][0] == 'B') || (argv[7][0] == 'b'))
        {
            little_endian = 0;
            printf("Big endian input\n");
        }
    }

    /* read params into the config structure */
    file_param = fopen(argv[1], "rb");
    if (file_param == NULL)
    {
        fprintf(stderr, "\n Can't open parameter file <%s>\n", argv[1]);
        exit(1);
    }
    else
    {
        fprintf(stdout, "\n parameter file opened :<%s>\n", argv[1]);
    }

    setConfig(file_param);

    /* call setParameter function */
    setParameter();

    /* initialization */
    my_open();


    // open input files
    file_mainin = fopen(argv[2], "rb");
    if (file_mainin == NULL)
    {
        printf("\n Can't open input file <%s>\n", argv[2]);
        exit(1);
    }
    else
    {
        printf("\n input file opened :<%s>\n", argv[2]);

        fseek(file_mainin, 0, SEEK_END);
        niter = ftell(file_mainin) / 2 / in_chan;
        rewind(file_mainin);
    }


    if (niter < sizebuf)
    {
        printf("all files must contains at least %d samples \n", sizebuf);
        exit(1);
    }


    // open output file
    strcpy(filename_mainout, argv[3]);
    file_mainout = fopen(argv[3], "wb");
    if (file_mainout == NULL)
    {
        printf("\n Can't open output file <%s>\n", argv[3]);
        exit(0);
    }
    else
    {
        printf("\n output file opened :<%s>\n", argv[3]);
    }

#endif // ARM_RVDEBUG


    inbuf  = (MMshort *) malloc(sizeof(MMshort) * sizebuf * in_offset);
    outbuf = (MMshort *) malloc(sizeof(MMshort) * sizebuf * out_offset); // make sure to have enough space for stereo


    /* reset of internal params is needed */
    my_reset();

#ifdef __arm__
    mdrc5b_local_struct.sample_bit_size = 32;
#else // __arm__
    mdrc5b_local_struct.sample_bit_size = 24;
#endif // __arm__
    mdrc5b_local_struct.SampleFreq = sample_freq;
    //printf("SL:mdrc5b_local_struct.SampleFreq = %d\n", sample_freq);

    if(applyEffectConfig(&mdrc5b_local_struct, &mMdrc5bAlgoConfig) != 0)
    {
        fprintf(stderr, "error in applyEffectConfig !\n");
        exit(-1);
    }

#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)
    {
        char *p_string_error = debug_samples_open(in_chan, mMdrc5bAlgoConfig.NumBands, filename_mainout);

        if(p_string_error != NULL)
        {
            fprintf(stderr, "%s\n", p_string_error);
            exit(-1);
        }
    }
   // niter = 48000;
#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT

    printf("\n\nSTART for %d loops, niter = %ld, block_size = %d, chs=%d\n",  (int) ((niter + sizebuf - 1) / sizebuf), niter, sizebuf, in_chan);

#ifdef __arm__
#ifdef ARM_CYCLES
    for(index_PMU_measures = 0; index_PMU_measures < NB_PMU_MEASURES; index_PMU_measures++)
    {
        cpt_PMU_measure[index_PMU_measures] = 0;
    }
#ifdef __arm
    startperf();    // RVCT only
#endif // __arm
#endif // ARM_CYCLES
#ifndef __arm
    // arm-linux only
    init_perf(1);
    pcache_evc_file = fopen("/proc/pcache/l1evc", "w");
    if(pcache_evc_file == NULL)
    {
        fprintf(stderr, "can't open /proc/pcache/l1evc output file!\n");
    }
    else
    {
        char pcache_evc_string[100];

        for(j = 0; j < 7; j++)
        {
            char *p_char;

            if(j < 6)
            {
                for(p_char = evc_list[j]; *p_char != 0; p_char++)
                {
                    *p_char = toupper(*p_char);
                }
            }
            *evc_stat[j].name     = 0;
            evc_stat[j].cpt       = 0;
            evc_stat[j].index_min = 0;
            evc_stat[j].index_max = 0;
            evc_stat[j].min       = 0;
            evc_stat[j].max       = 0;
            evc_stat[j].sum       = 0;
            evc_stat[j].sum2      = 0;
        }
        sprintf(pcache_evc_string, "%s %s %s %s %s %s", evc_list[0], evc_list[1], evc_list[2], evc_list[3], evc_list[4], evc_list[5]);
        printf("\n*** PCACHE EVC list : %s ***\n\n", pcache_evc_string);
        fprintf(pcache_evc_file, "%s\n", pcache_evc_string);
        fclose(pcache_evc_file);
    }
#endif // !__arm
#endif // __arm__
    for(i = 0 ; i < niter; i += (long) sizebuf)
    {
        if(niter - i > (long) sizebuf)
        {
            n = sizebuf;
        }
        else
        {
            n = (int) (niter - i);
        }
        for(j = 0 ; j < n * in_offset; j++)
        {
            if(little_endian)
            {
                low  = getc(file_mainin);
                high = getc(file_mainin);
            }
            else
            {
                high = getc(file_mainin);
                low  = getc(file_mainin);
            }
#ifdef __flexcc2__
            sample   = (high << 16) | ((low & 0xFF) << 8);
#else //__flexcc2__
            sample   = (high << 24) | ((low & 0xFF) << 16);
#ifndef __arm__
            // for x86_mmdsp target (24 bits samples)
            sample >>= 8;
#endif // !__arm__
#endif //__flexcc2__
            inbuf[j] = sample;
        }
        for(; j < sizebuf * in_offset; j++)
        {
            inbuf[j] = 0;
        }


#ifdef __flexcc2__
        start_count = get_cycle_count();
#endif //__flexcc2__

#ifdef ENABLE_PROFILING
        prev_buff_fill = get_cycle_count();
#endif

        // call the processing routine
#if defined(__arm__) && !defined(__arm)
        // arm-linux only
        if(pcache_evc_file != NULL)
        {
            pcache_evc_file = fopen("/proc/pcache/l1evc", "r");
            if(pcache_evc_file == NULL)
            {
                fprintf(stderr, "can't open /proc/pcache/l1evc input file !\n");
                return -1;
            }
            fgets(pcache_evc_start, sizeof(pcache_evc_start), pcache_evc_file);
            fclose(pcache_evc_file);
        }
        start_perf(0);
#endif // __arm__ && !__arm
        processBuffer(inbuf, outbuf, sizebuf * in_offset);
#if defined(__arm__) && !defined(__arm)
        // arm-linux only
        end_perf(0);
        if(pcache_evc_file != NULL)
        {
            char pcache_evc_string[1024], evc_string[100], *p_char1, *p_char2;
            int  evc_val;

            pcache_evc_file = fopen("/proc/pcache/l1evc", "r");
            if(pcache_evc_file == NULL)
            {
                fprintf(stderr, "can't open /proc/pcache/l1evc input file !\n");
                return -1;
            }
            fgets(pcache_evc_end, sizeof(pcache_evc_end), pcache_evc_file);
            fclose(pcache_evc_file);
            for(p_char1 = pcache_evc_end; *p_char1 != 0; p_char1++)
            {
                *p_char1 = toupper(*p_char1);
            }
            for(j = 0; j < 7; j++)
            {
                strcpy(pcache_evc_string, pcache_evc_end);
                if(j == 6)
                {
                    strcpy(evc_string, "CCNT =");
                    p_char1 = strstr(pcache_evc_string, evc_string);
                    if(p_char1 == NULL)
                    {
                        fprintf(stderr, "can't find \"%s\" into \"%s\" !\n", evc_string, pcache_evc_string);
                        return -1;
                    }
                    p_char2 = strchr(p_char1, '=') + 1;
                    strcpy(evc_string, "CCNT");
                }
                else
                {
                    sprintf(evc_string, "[%s]", evc_list[j]);
                    p_char1 = strstr(pcache_evc_string, evc_string);
                    if(p_char1 == NULL)
                    {
                        fprintf(stderr, "can't find \"%s\" into \"%s\" !\n", evc_string, pcache_evc_string);
                        return -1;
                    }
                    p_char1 = strchr(p_char1, ']') + 2;
                    p_char2 = strchr(p_char1, ':') + 1;
                    *(p_char2 - 2) = 0;
                    strcpy(evc_string, p_char1);
                }
                p_char1 = strchr(p_char2, ';');
                *p_char1 = 0;
                sscanf(p_char2, "%d", &evc_val);
                if(evc_stat[j].cpt == 0)
                {
                    strcpy(evc_stat[j].name, evc_string);
                    evc_stat[j].index_min = 0;
                    evc_stat[j].index_max = 0;
                    evc_stat[j].min       = evc_val;
                    evc_stat[j].max       = evc_val;
                }
                else
                {
                    if(evc_val < evc_stat[j].min)
                    {
                        evc_stat[j].index_min = evc_stat[j].cpt;
                        evc_stat[j].min       = evc_val;
                    }
                    if(evc_val > evc_stat[j].max)
                    {
                        evc_stat[j].index_max = evc_stat[j].cpt;
                        evc_stat[j].max       = evc_val;
                    }
                }
                evc_stat[j].sum  += ((long long) evc_val);
                evc_stat[j].sum2 += ((long long) evc_val) * ((long long) evc_val);
                evc_stat[j].cpt++;
            }
        }
#endif // __arm__ && !__arm

#ifdef ENABLE_PROFILING
        after_buff_fill = get_cycle_count();
        diff            = after_buff_fill - prev_buff_fill;
        diff_buff_fill += diff;

        Frame_Number++;

        //print MIPS of first 5 frames
        if(Frame_Number <= 5)
        {
            frame_count = Frame_Number;
            max_mips = ((float)diff_buff_fill/((float)(sizebuf * in_offset )*1000000)*sample_freq);
            fprintf (fp,"Mips = %f  In FRAME # %d\n\n",max_mips,frame_count);
        }

        if(diff_buff_fill > a_cycles)
        {
            a_cycles = diff_buff_fill;
            a_samples = sizebuf * in_offset;
            frm_highest_mips = Frame_Number;
        }
        diff_buff_fill = 0;
#endif

#ifdef __flexcc2__
        stop_count = get_cycle_count();

        if((stop_count - start_count) > max_cycle_count)
        {
            max_cycle_count = stop_count - start_count;
            max_cycle_frame_nb = frmCnt;
        }
#endif //__flexcc2__
        frmCnt++;


        for(j = 0; j < n * out_offset; j++)
        {
#ifdef __arm__
            // 32 bits samples
            sample = (outbuf[j] + (outbuf[j] < 0x7FFF8000 ? (1 << 15) : 0)) >> 16;
#else //__arm__
            // 24 bits samples
            sample = (outbuf[j] + (outbuf[j] < 0x7FFF80 ? (1 << 7) : 0)) >> 8;
#endif // __arm__
            low    = (sample & 0xFF);
            high   = ((sample >> 8) & 0xFF);

            if(little_endian)
            {
                putc(low,  file_mainout);
                putc(high, file_mainout);
            }
            else
            {
                putc(high, file_mainout);
                putc(low,  file_mainout);
            }
        }


#ifndef __flexcc2__
        cpt += (long) n;
        percent = 100 * cpt / niter;
        if(percent > last_percent)
        {
            last_percent = percent;
            fprintf(stderr, " %d %% \r", percent);
        }
#endif // !defined(__flexcc2__)
    }

#ifdef __arm__
#ifdef ARM_CYCLES
    for(index_PMU_measures = 0; index_PMU_measures < NB_PMU_MEASURES; index_PMU_measures++)
    {
        printf("\n\n****** PMU measure %d : ", index_PMU_measures);
        for(j = 0; j < NB_PMU_MEASURES; j++)
        {
            if(PMU_measure_name[j].index == index_PMU_measures)
            {
                printf("%s", PMU_measure_name[j].name);
                break;
            }
        }
        if(j == NB_PMU_MEASURES)
        {
            printf("???");
        }
        if(cpt_PMU_measure[index_PMU_measures])
        {
            printf(" (%d measures) ******\n", cpt_PMU_measure[index_PMU_measures]);
            PMU_unquiet(PMU_index[index_PMU_measures]);
        }
        else
        {
            printf(" (no measure) ******\n");
        }
    }
#ifdef __arm
    endperf();          // RVCT only
#endif /* __arm */
#endif // ARM_CYCLES
#ifndef __arm
    // arm-linux only
    {
        long long total_times     = get_total_times();
        double    CPU_duration    = ((double) total_times) / 1000000.0;
        double    stream_duration = ((double) i) / ((double) sample_freq);
        double    CPU_freq_kHz, CPU_freq_MHz;
        FILE      *p_CPU_freq_file;

        p_CPU_freq_file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
        if(p_CPU_freq_file == NULL)
        {
            fprintf(stderr, "Can't open /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq file\n");
            CPU_freq_MHz = 800.0;
            fprintf(stderr, "assume CPU frequency = %1.2lf MHz\n", CPU_freq_MHz);
        }
        else
        {
            fscanf(p_CPU_freq_file, "%lf", &CPU_freq_kHz);
            fclose(p_CPU_freq_file);
            CPU_freq_MHz = CPU_freq_kHz / 1000.0;
        }

        printf("CPU duration    = %1.3lf seconds\n", CPU_duration);
        printf("stream duration = %1.3lf seconds\n", stream_duration);
        printf("=> %1.2lf MCPS at %1.2lf MHz\n", CPU_duration / stream_duration * CPU_freq_MHz, CPU_freq_MHz);

        if(pcache_evc_file != NULL)
        {
            printf("\n****************************************\nPCACHE EVC values :\n");
            for(j = 0; j < 7; j++)
            {
                printf("\n%s (%d measures)\n", evc_stat[j].name, evc_stat[j].cpt);
                printf("min         = %8d (measure %4d)\n", evc_stat[j].min, evc_stat[j].index_min);
                printf("max         = %8d (measure %4d)\n", evc_stat[j].max, evc_stat[j].index_max);
                //printf("sum         = %18lld\n", evc_stat[j].sum);
                //printf("sum2        = %18lld \n", evc_stat[j].sum2);
                if(evc_stat[j].cpt > 0)
                {
                    double avg     = ((double) evc_stat[j].sum)  / ((double) evc_stat[j].cpt);
                    double var     = ((double) evc_stat[j].sum2) / ((double) evc_stat[j].cpt) - avg * avg;
                    double std_dev = sqrt(var);

                    printf("avg         = %8.0lf\n", avg);
                    //printf("var         = %16.0lf\n", var);
                    //printf("std_dev     = %8.0lf\n", std_dev);
                    printf("std_dev/avg = %5.2lf %%\n", std_dev / avg * 100.0);
                }
            }
            printf("****************************************\n\n");
        }
    }
#endif // !__arm
#endif // __arm__

#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)
    debug_samples_close(in_chan, mMdrc5bAlgoConfig.NumBands);
#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT

#ifdef ENABLE_PROFILING
    max_mips = ((float)a_cycles/((float)a_samples*1000000)*sample_freq);
    fprintf(fp,"Peak Mips = %f  In FRAME # %d\n\n",max_mips,frm_highest_mips);
    fprintf(fp," Test ends \n\n");
    if(fp)
    {
        fclose (fp);
        fp = NULL;
    }
#endif

    //--------------------- 2nd test --------------------//

#ifdef SECOND_TEST
    fclose(file_mainout);
    rewind(file_mainin);

    sprintf(name, "%s_2", argv[3]);
    file_mainout = fopen(name, "wb");


    /* reset of internal params is needed */
    my_reset();
    mdrc5b_local_struct.SampleFreq = sample_freq;


    if(applyEffectConfig(&mdrc5b_local_struct, &mMdrc5bAlgoConfig) != 0)
    {
        printf("error in applyEffectConfig !\n");
        exit(-1);
    }

    for(i = 0 ; i < niter; i += (long) sizebuf)
    {
        if(niter - i > (long) sizebuf)
        {
            n = sizebuf;
        }
        else
        {
            n = (int) (niter - i);
        }
        for(j = 0 ; j < n * in_offset; j++)
        {
            if(little_endian)
            {
                low  = getc(file_mainin);
                high = getc(file_mainin);
            }
            else
            {
                high = getc(file_mainin);
                low  = getc(file_mainin);
            }
#ifdef __flexcc2__
            sample   = (high << 16) | ((low & 0xFF) << 8);
#else //__flexcc2__
            sample   = (high << 24) | ((low & 0xFF) << 16);
#ifndef __arm__
            // for x86_mmdsp target (24 bits samples)
            sample >>= 8;
#endif // !__arm__
#endif //__flexcc2__
            inbuf[j] = (MMshort) (sample << 8);
        }
        for(; j < sizebuf * in_offset; j++)
        {
            inbuf[j] = 0;
        }


#ifdef __flexcc2__
        start_count = get_cycle_count();
#endif //__flexcc2__


        // call the processing routine
        processBuffer(inbuf, outbuf, sizebuf * in_offset);

#ifdef __flexcc2__
        stop_count = get_cycle_count();

        if ((stop_count - start_count) > max_cycle_count)
        {
            max_cycle_count = stop_count - start_count;
            max_cycle_frame_nb = frmCnt;
        }
#endif //__flexcc2__
        frmCnt++;


        for(j = 0; j < n * out_offset; j++)
        {
#ifdef __arm__
            // 32 bits samples
            sample = (outbuf[j] + (outbuf[j] < 0x7FFF8000 ? (1 << 15) : 0)) >> 16;
#else //__arm__
            // 24 bits samples
            sample = (outbuf[j] + (outbuf[j] < 0x7FFF80 ? (1 << 7) : 0)) >> 8;
#endif // __arm__
            low    = (sample & 0xFF);
            high   = ((sample >> 8) & 0xFF);

            if(little_endian)
            {
                putc(low,  file_mainout);
                putc(high, file_mainout);
            }
            else
            {
                putc(high, file_mainout);
                putc(low,  file_mainout);
            }
        }

#ifndef __flexcc2__
        cpt += (long) n;
        percent = 100 * cpt / niter;
        if(percent > last_percent)
        {
            last_percent = percent;
            fprintf(stderr, " %d %% \r", percent);
        }
#endif // !defined(__flexcc2__)
    }
#endif // SECOND_TEST

    //--------------------- 2nd test done --------------------//

#ifndef __flexcc2__
    fprintf(stderr, "\n");
#endif // !defined(__flexcc2__)

    free(inbuf);
    free(outbuf);

#ifdef __flexcc2__
    printf("Worst case MHz is %.1f, occurs in frame no. %ld \n", max_cycle_count*48.0/128000, max_cycle_frame_nb);
#endif //__flexcc2__

    fclose(file_mainin);
    fclose(file_mainout);
    fclose(file_param);

    return 0;
}
