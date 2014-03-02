/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated libeffects/libtransducer_equalizer/include/config.idt defined type */
#if !defined(_TRANSDUCER_EQUALIZER_CONFIG_H_)
#define _TRANSDUCER_EQUALIZER_CONFIG_H_


#define NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS 40

#define NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS 400

#define TEQ_FIFO_CONFIG_SIZE 3

typedef struct t_xyuv_TransducerEqualizerParams_t {
  t_uint16 iMemoryPreset;
  t_uint16 istereo;
  t_uint16 inb_alloc_biquad_cells_per_channel;
  t_uint16 inb_alloc_FIR_coefs_per_channel;
  t_uint16 ialert_on;
} TransducerEqualizerParams_t;

typedef struct t_xyuv_t_transducer_equalizer_biquad_cell_params {
  t_uint16 b_exp;
  t_uint16 b0_low;
  t_uint16 b0_high;
  t_uint16 b1_low;
  t_uint16 b1_high;
  t_uint16 b2_low;
  t_uint16 b2_high;
  t_uint16 a1_low;
  t_uint16 a1_high;
  t_uint16 a2_low;
  t_uint16 a2_high;
} t_transducer_equalizer_biquad_cell_params;

typedef struct t_xyuv_t_transducer_equalizer_FIR_coef_params {
  t_uint16 coef_low;
  t_uint16 coef_high;
} t_transducer_equalizer_FIR_coef_params;

typedef struct t_xyuv_TransducerEqualizerConfig_t {
  t_uint16 iEnable;
  t_uint16 iautomatic_transition_gain;
  t_uint16 iaverage_gain_exp_l;
  t_uint16 iaverage_gain_mant_l_low;
  t_uint16 iaverage_gain_mant_l_high;
  t_uint16 iaverage_gain_exp_r;
  t_uint16 iaverage_gain_mant_r_low;
  t_uint16 iaverage_gain_mant_r_high;
  t_uint16 itransition_gain_exp_l;
  t_uint16 itransition_gain_mant_l_low;
  t_uint16 itransition_gain_mant_l_high;
  t_uint16 itransition_gain_exp_r;
  t_uint16 itransition_gain_mant_r_low;
  t_uint16 itransition_gain_mant_r_high;
  t_uint16 ismoothing_delay_low;
  t_uint16 ismoothing_delay_high;
  t_uint16 iFIR_smoothing_delay_granularity;
  t_uint16 inb_biquad_cells_per_channel;
  t_uint16 inb_FIR_coefs_per_channel;
  t_uint16 isame_biquad_l_r;
  t_uint16 isame_FIR_l_r;
  t_uint16 ibiquad_first;
  t_uint16 ibiquad_gain_exp_l;
  t_uint16 ibiquad_gain_mant_l_low;
  t_uint16 ibiquad_gain_mant_l_high;
  t_uint16 ibiquad_gain_exp_r;
  t_uint16 ibiquad_gain_mant_r_low;
  t_uint16 ibiquad_gain_mant_r_high;
  t_uint16 iFIR_gain_exp_l;
  t_uint16 iFIR_gain_mant_l_low;
  t_uint16 iFIR_gain_mant_l_high;
  t_uint16 iFIR_gain_exp_r;
  t_uint16 iFIR_gain_mant_r_low;
  t_uint16 iFIR_gain_mant_r_high;
  t_transducer_equalizer_biquad_cell_params ibiquad_cells[NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];
  t_transducer_equalizer_FIR_coef_params iFIR_coefs[NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];
} TransducerEqualizerConfig_t;

typedef struct t_xyuv_TransducerEqualizerConfigStructFifo_t {
  t_uint16 write_index;
  t_uint16 read_index;
  TransducerEqualizerConfig_t config[TEQ_FIFO_CONFIG_SIZE];
} TransducerEqualizerConfigStructFifo_t;

#endif
