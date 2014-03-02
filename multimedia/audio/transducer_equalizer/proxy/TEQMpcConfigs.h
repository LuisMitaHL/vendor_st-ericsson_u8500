/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQMpcConfigs.h
 * \brief  Redefine OMX structures at 24-bits DSP format
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TEQ_MPC_CONFIGS_H_
#define _TEQ_MPC_CONFIGS_H_

#include <host/libeffects/libtransducer_equalizer/include/config.idt.h>

//AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE
typedef struct {
    t_uint32 iMemoryPreset;
    t_uint32 istereo;                  /* STATIC, 0 : mono, 1 : stereo, default 0 */
    t_uint32 inb_alloc_biquad_cells_per_channel;
    t_uint32 inb_alloc_FIR_coefs_per_channel;
    t_uint32 ialert_on;
} ARMParamTeq_t;

//AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE
typedef struct
{
    t_uint32 b_exp;
    t_uint32 b0_low;    // 16 LSB
    t_uint32 b0_high;   // 16 MSB
    t_uint32 b1_low;    // 16 LSB
    t_uint32 b1_high;   // 16 MSB
    t_uint32 b2_low;    // 16 LSB
    t_uint32 b2_high;   // 16 MSB
    t_uint32 a1_low;    // 16 LSB
    t_uint32 a1_high;   // 16 MSB
    t_uint32 a2_low;    // 16 LSB
    t_uint32 a2_high;   // 16 MSB
} ARMtransducer_equalizer_biquad_cell_params_t;


typedef struct
{
    t_uint32 coef_low;  // 16 LSB
    t_uint32 coef_high; // 16 MSB
} ARMtransducer_equalizer_FIR_coef_params_t;

typedef struct
{
    t_uint32                                        iEnable;
    t_uint32                                        iautomatic_transition_gain;
    t_uint32                                        iaverage_gain_exp_l;
    t_uint32                                        iaverage_gain_mant_l_low;                 // 16 LSB
    t_uint32                                        iaverage_gain_mant_l_high;                // 16 MSB
    t_uint32                                        iaverage_gain_exp_r;
    t_uint32                                        iaverage_gain_mant_r_low;                 // 16 LSB
    t_uint32                                        iaverage_gain_mant_r_high;                // 16 MSB
    t_uint32                                        itransition_gain_exp_l;
    t_uint32                                        itransition_gain_mant_l_low;              // 16 LSB
    t_uint32                                        itransition_gain_mant_l_high;             // 16 MSB
    t_uint32                                        itransition_gain_exp_r;
    t_uint32                                        itransition_gain_mant_r_low;              // 16 LSB
    t_uint32                                        itransition_gain_mant_r_high;             // 16 MSB
    t_uint32                                        ismoothing_delay_low;                     // 16 LSB
    t_uint32                                        ismoothing_delay_high;                    // 16 MSB
    t_uint32                                        iFIR_smoothing_delay_granularity;
    t_uint32                                        inb_biquad_cells_per_channel;
    t_uint32                                        inb_FIR_coefs_per_channel;
    t_uint32                                        isame_biquad_l_r;
    t_uint32                                        isame_FIR_l_r;
    t_uint32                                        ibiquad_first;
    t_uint32                                        ibiquad_gain_exp_l;
    t_uint32                                        ibiquad_gain_mant_l_low;                  // 16 LSB
    t_uint32                                        ibiquad_gain_mant_l_high;                 // 16 MSB
    t_uint32                                        ibiquad_gain_exp_r;
    t_uint32                                        ibiquad_gain_mant_r_low;                  // 16 LSB
    t_uint32                                        ibiquad_gain_mant_r_high;                 // 16 MSB
    t_uint32                                        iFIR_gain_exp_l;
    t_uint32                                        iFIR_gain_mant_l_low;                     // 16 LSB
    t_uint32                                        iFIR_gain_mant_l_high;                    // 16 MSB
    t_uint32                                        iFIR_gain_exp_r;
    t_uint32                                        iFIR_gain_mant_r_low;                     // 16 LSB
    t_uint32                                        iFIR_gain_mant_r_high;                    // 16 MSB
    ARMtransducer_equalizer_biquad_cell_params_t    ibiquad_cells[NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];
    ARMtransducer_equalizer_FIR_coef_params_t     iFIR_coefs   [NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];
} ARMTransducerEqualizerConfig_t;

#endif //_TEQ_MPC_CONFIGS_H_

