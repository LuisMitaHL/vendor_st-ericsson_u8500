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

/* Generated drc/nmfil/host/effect/config.idt defined type */
#if !defined(_DRC_NMFIL_HOST_EFFECT_CONFIG_H_)
#define _DRC_NMFIL_HOST_EFFECT_CONFIG_H_

#include <samplefreq.idt.h>

#define NUMBER_OF_STATIC_CURVE_POINTS 8

typedef enum t_xyuv_t_acceptance_level {
  ACCEPTANCE_LEVEL_90_PERCENT=1,
  ACCEPTANCE_LEVEL_99_PERCENT,
  ACCEPTANCE_LEVEL_99_9_PERCENT} t_acceptance_level;

typedef struct t_xyuv_t_static_curve_point {
  t_sint16 x;
  t_sint16 y;
} t_static_curve_point;

typedef struct t_xyuv_t_static_curve {
  t_static_curve_point static_curve_point[NUMBER_OF_STATIC_CURVE_POINTS];
  t_sint16 input_gain;
  t_sint16 offset_gain;
} t_static_curve;

typedef struct t_xyuv_t_time_specifiers {
  t_uint16 level_detector_attack_time;
  t_uint16 level_detector_release_time;
  t_uint16 gain_processor_attack_time;
  t_uint16 gain_processor_release_time;
  t_acceptance_level acceptance_level;
} t_time_specifiers;

typedef struct t_xyuv_t_drc_config {
  t_bool enable;
  t_bool use_separate_channel_settings;
  t_bool use_peak_measure;
  t_bool use_peak_measure_channel2;
  t_uint16 look_ahead_time;
  t_static_curve static_curve;
  t_time_specifiers time_specifiers;
  t_static_curve static_curve_channel2;
  t_time_specifiers time_specifiers_channel2;
} t_drc_config;

#endif
