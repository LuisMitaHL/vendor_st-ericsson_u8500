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

/* Generated t1xhv_vdc_vc1.idt defined type */
#if !defined(_t1xhv_vdc_vc1_idt)
#define _t1xhv_vdc_vc1_idt

#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vdc_vc1_param_in {
  t_ulong_value frame_size;
  t_ushort_value max_picture_width;
  t_ushort_value max_picture_height;
  t_ushort_value profile;
  t_ushort_value quantizer;
  t_ushort_value dquant;
  t_ushort_value max_b_frames;
  t_ushort_value multires_coding_enabled;
  t_ushort_value extended_mv_enabled;
  t_ushort_value overlap_transform_enabled;
  t_ushort_value syncmarker_enabled;
  t_ushort_value rangered_enabled;
  t_ushort_value frame_interpolation_enabled;
  t_ushort_value variable_size_transform_enabled;
  t_ushort_value loop_filter_enabled;
  t_ushort_value fast_uvmc_enabled;
  t_ushort_value is_smpte_conformant;
  t_ulong_value padding2;
  t_ulong_value padding3;
  t_ulong_value padding4;
} ts_t1xhv_vdc_vc1_param_in;

typedef ts_t1xhv_vdc_vc1_param_in* tps_t1xhv_vdc_vc1_param_in;

typedef struct t_xyuv_ts_t1xhv_vdc_vc1_param_inout {
  t_ushort_value intensity_compensate_enabled;
  t_ushort_value last_ref_rangered_enabled;
  t_ushort_value previous_last_ref_rangered_enabled;
  t_ushort_value last_ref_interpolation_hint_enabled;
  t_ushort_value last_ref_buffer_fullness;
  t_ushort_value luma_scale;
  t_ushort_value luma_shift;
  t_ushort_value rnd_ctrl;
  t_ushort_value reference_resolution;
  t_ushort_value padding1;
  t_ulong_value padding2;
  t_ulong_value padding3;
  t_ulong_value padding4;
} ts_t1xhv_vdc_vc1_param_inout;

typedef ts_t1xhv_vdc_vc1_param_inout* tps_t1xhv_vdc_vc1_param_inout;

typedef struct t_xyuv_ts_t1xhv_vdc_vc1_param_out {
  t_ushort_value error_type;
  t_ushort_value frame_interpolation_hint_enabled;
  t_ushort_value range_reduction_frame_enabled;
  t_ushort_value b_fraction_numerator;
  t_ushort_value b_fraction_denominator;
  t_ushort_value buffer_fullness;
  t_ushort_value picture_res;
  t_ushort_value max_picture_width;
  t_ushort_value max_picture_height;
  t_ushort_value picture_width;
  t_ushort_value picture_height;
  t_ushort_value picture_type;
  t_ulong_value padding1;
  t_ulong_value padding2;
} ts_t1xhv_vdc_vc1_param_out;

typedef ts_t1xhv_vdc_vc1_param_out* tps_t1xhv_vdc_vc1_param_out;

typedef struct t_xyuv_ts_t1xhv_vdc_vc1_param_in_common {
  t_ushort_value max_picture_width;
  t_ushort_value max_picture_height;
  t_ushort_value profile;
  t_ushort_value level;
  t_ushort_value quantizer;
  t_ushort_value dquant;
  t_ushort_value max_b_frames;
  t_ushort_value q_framerate_for_postproc;
  t_ushort_value q_bitrate_for_postproc;
  t_ushort_value loop_filter_enabled;
  t_ushort_value multires_coding_enabled;
  t_ushort_value fast_uvmc_enabled;
  t_ushort_value extended_mv_enabled;
  t_ushort_value variable_size_transform_enabled;
  t_ushort_value overlap_transform_enabled;
  t_ushort_value syncmarker_enabled;
  t_ushort_value rangered_enabled;
  t_ushort_value frame_interpolation_enabled;
  t_ulong_value frame_size;
  t_ulong_value padding3;
  t_ulong_value padding4;
} ts_t1xhv_vdc_vc1_param_in_common;

typedef ts_t1xhv_vdc_vc1_param_in_common* tps_t1xhv_vdc_vc1_param_in_common;

#endif
