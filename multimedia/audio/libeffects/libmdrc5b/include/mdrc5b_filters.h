/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: local struct definition
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdrc5b_filters_h_
#define _mdrc5b_filters_h_

#include "audiolibs_common.h"
#include "config.idt"


#ifdef __flexcc2__
#define MMshort    short
#define MMlong     long
#define MMlonglong long long
#else
#define MMshort    int
#define MMlong     long long
#define MMlonglong long long
#endif



typedef struct
{
    MMshort b_exp;
    MMshort b0, b1, b2, a1, a2;
} t_biquad_cell;

typedef struct
{
    MMshort b_exp;
    MMshort b0, a1, a2;
} t_biquad_cell_Butter;

typedef struct
{
    MMshort order;
    MMshort gain_exp;
    MMshort *coef;
} t_FIR;

typedef struct
{
    int       biquad_order;
    void YMEM *p_biquad_cell_void;
    MMshort   *p_biquad_mem[MDRC_CHANNELS_MAX];
} t_biquad_processing_ctx;

#define DUMMY_BIQUAD_LP       (0 * 3)
#define BUTTERWORTH_BIQUAD_LP (1 * 3)
#define OTHER_BIQUAD_LP       (2 * 3)
#define DUMMY_BIQUAD_HP       (0 * 1)
#define BUTTERWORTH_BIQUAD_HP (1 * 1)
#define OTHER_BIQUAD_HP       (2 * 1)
typedef enum
{
    DUMMY_BIQUAD_LP_DUMMY_BIQUAD_HP             = (DUMMY_BIQUAD_LP       + DUMMY_BIQUAD_HP),
    DUMMY_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP       = (DUMMY_BIQUAD_LP       + BUTTERWORTH_BIQUAD_HP),
    DUMMY_BIQUAD_LP_OTHER_BIQUAD_HP             = (DUMMY_BIQUAD_LP       + OTHER_BIQUAD_HP),
    BUTTERWORTH_BIQUAD_LP_DUMMY_BIQUAD_HP       = (BUTTERWORTH_BIQUAD_LP + DUMMY_BIQUAD_HP),
    BUTTERWORTH_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP = (BUTTERWORTH_BIQUAD_LP + BUTTERWORTH_BIQUAD_HP),
    BUTTERWORTH_BIQUAD_LP_OTHER_BIQUAD_HP       = (BUTTERWORTH_BIQUAD_LP + OTHER_BIQUAD_HP),
    OTHER_BIQUAD_LP_DUMMY_BIQUAD_HP             = (OTHER_BIQUAD_LP       + DUMMY_BIQUAD_HP),
    OTHER_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP       = (OTHER_BIQUAD_LP       + BUTTERWORTH_BIQUAD_HP),
    OTHER_BIQUAD_LP_OTHER_BIQUAD_HP             = (OTHER_BIQUAD_LP       + OTHER_BIQUAD_HP)
} t_biquad_LP_HP_combination;

typedef struct
{
    t_biquad_LP_HP_combination biquad_LP_HP_combination;
    t_biquad_processing_ctx    biquadLP_processing_ctx;
    t_biquad_processing_ctx    biquadHP_processing_ctx;
} t_mdrc_biquads;

typedef struct
{
    t_FIR_kind FIR_kind;
    t_FIR      FIR;
} t_mdrc_FIR;

typedef struct mdrc_filters_struct
{
    t_mdrc_filter_kind    mdrc_filter_kind;
    void                  (*p_sub_bands_filtering) (struct mdrc_filters_struct *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples);
    int                   nb_channels;
    int                   nb_bands;
    union
    {
        t_mdrc_biquads mdrc_biquads[MDRC_BANDS_MAX];
        struct
        {
            t_mdrc_FIR          mdrc_FIR[MDRC_BANDS_MAX];
            MMshort YMEM __circ *p_FIR_common_mem[MDRC_CHANNELS_MAX];
            // optimized part
            MMshort             order_optim;
            MMshort             *p_coef_optim [MDRC_BANDS_MAX];
            MMshort             gain_exp_optim[MDRC_BANDS_MAX];
            MMshort YMEM        *p_FIR_common_mem_optim;
        } mdrc_FIRs;
    } mdrc_filters;
} t_mdrc_filters;



extern void mdrc_filter_default_config(t_mdrc_filters *p_mdrc_filters);

extern int  get_mdrc_filter_config    (t_mdrc_filters      *p_mdrc_filters,
                                       MdrcFiltersConfig_t *p_mdrc_filters_config,
                                       t_mdrc_filter_kind  param_mdrc_filter_kind,
                                       t_biquad_kind       param_biquad_kind,
                                       t_FIR_kind          param_FIR_kind);

extern void reset_mdrc_filter         (t_mdrc_filters *p_mdrc_filters);

extern void free_mdrc_filter          (t_mdrc_filters *p_mdrc_filters);


#endif // _mdrc5b_filters_h_
