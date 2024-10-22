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

/* Generated libeffects/libmdrc5b/include/config.idt defined type */
#if !defined(_MDRC_CONFIG_H_)
#define _MDRC_CONFIG_H_


#define MDRC_FIFO_CONFIG_SIZE 3

#define MDRC_CHANNELS_MAX 2

#define MDRC_BANDS_MAX 5

#define MDRC_BIQUAD_ORDER_MIN 1

#define MDRC_BIQUAD_ORDER_MAX 4

#define MDRC_FIR_ORDER_MIN 21

#define MDRC_FIR_ORDER_MAX 81

#define MDRC_NB_BIQUAD_LPHP_MAX 4

typedef enum t_xyuv_t_mdrc_filter_kind {
  MDRC_LEGACY_FILTER,
  MDRC_BIQUAD_FILTER,
  MDRC_FIR_FILTER} t_mdrc_filter_kind;

typedef enum t_xyuv_t_biquad_kind {
  BUTTERWORTH_BIQUAD,
  OTHER_BIQUAD} t_biquad_kind;

typedef enum t_xyuv_t_FIR_kind {
  SYMMETRIC_FIR,
  OTHER_FIR} t_FIR_kind;

typedef struct t_xyuv_t_biquad_cell_int16 {
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
} t_biquad_cell_int16;

typedef struct t_xyuv_t_FIR_coef_int16 {
  t_uint16 low;
  t_uint16 high;
} t_FIR_coef_int16;

typedef struct t_xyuv_t_FIR_int16 {
  t_uint16 order;
  t_uint16 gain_exp;
  t_FIR_coef_int16 coef[MDRC_FIR_ORDER_MAX];
} t_FIR_int16;

typedef struct t_xyuv_t_biquadLP_HP_int16 {
  t_uint16 biquad_kind;
  t_uint16 biquad_order;
  t_biquad_cell_int16 biquad_cell_int16[MDRC_BIQUAD_ORDER_MAX];
} t_biquadLP_HP_int16;

typedef struct t_xyuv_t_mdrc_biquad_int16 {
  t_biquadLP_HP_int16 biquadLP;
  t_biquadLP_HP_int16 biquadHP;
} t_mdrc_biquad_int16;

typedef struct t_xyuv_t_mdrc_FIR_int16 {
  t_uint16 FIR_kind;
  t_FIR_int16 FIR;
} t_mdrc_FIR_int16;

typedef struct t_xyuv_MdrcFiltersConfig_t {
  t_uint16 nb_bands;
  t_mdrc_biquad_int16 mdrc_biquad[MDRC_NB_BIQUAD_LPHP_MAX];
  t_mdrc_FIR_int16 mdrc_FIR[MDRC_BANDS_MAX];
} MdrcFiltersConfig_t;

typedef struct t_xyuv_MdrcBandConfig_t {
  t_uint16 bEnable;
  t_uint16 mFreqCutoff_low;
  t_uint16 mFreqCutoff_high;
  t_uint16 mPostGain_low;
  t_uint16 mPostGain_high;
  t_uint16 mKneePoints;
  t_uint16 mDynamicResponse[10][2];
  t_uint16 mAttackTime_low;
  t_uint16 mAttackTime_high;
  t_uint16 mReleaseTime_low;
  t_uint16 mReleaseTime_high;
} MdrcBandConfig_t;

typedef struct t_xyuv_MdrcLimiterConfig_t {
  t_uint16 bEnable;
  t_uint16 mRmsMeasure;
  t_uint16 mThresh_low;
  t_uint16 mThresh_high;
} MdrcLimiterConfig_t;

typedef struct t_xyuv_MdrcConfig_t {
  t_uint16 mMode;
  t_uint16 mNumBands;
  MdrcBandConfig_t mBandCompressors[MDRC_BANDS_MAX];
  MdrcLimiterConfig_t mLimiter;
  MdrcFiltersConfig_t MdrcFiltersConfig;
} MdrcConfig_t;

typedef struct t_xyuv_MdrcConfigStructFifo_t {
  t_uint16 write_index;
  t_uint16 read_index;
  MdrcConfig_t config[MDRC_FIFO_CONFIG_SIZE];
} MdrcConfigStructFifo_t;

typedef struct t_xyuv_MdrcParams_t {
  t_uint16 mChannels;
  t_uint16 mSamplingFreq;
  t_uint16 mFilterKind;
  t_uint16 mBiquadKind;
  t_uint16 mFirKind;
} MdrcParams_t;

#endif
