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

/* Generated t1xhv_vdc_mpeg4.idt defined type */
#if !defined(_t1xhv_vdc_mpeg4_idt)
#define _t1xhv_vdc_mpeg4_idt

#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg4_param_in {
  t_ushort_value picture_coding_type;
  t_ushort_value quant;
  t_ushort_value quant_type;
  t_ushort_value intra_quant_mat[64];
  t_ushort_value nonintra_quant_mat[64];
  t_ushort_value low_delay;
  t_ushort_value interlaced;
  t_ushort_value rounding_type;
  t_ushort_value intra_dc_vlc_thr;
  t_ushort_value vop_fcode_forward;
  t_ushort_value vop_fcode_backward;
  t_ushort_value frame_width;
  t_ushort_value frame_height;
  t_ushort_value flag_short_header;
  t_ushort_value modulo_time_base;
  t_ushort_value vop_time_increment;
  t_ushort_value vop_time_increment_resolution;
  t_ushort_value resync_marker_disable;
  t_ushort_value data_partitioned;
  t_ushort_value reversible_vlc;
  t_ushort_value error_concealment_config;
  t_ushort_value flag_sorenson;
  t_ulong_value flv_version;
  t_ulong_value quarter_sample;
  t_ushort_value source_frame_width;
  t_ushort_value source_frame_height;
  t_ushort_value output_format;
  t_ushort_value deblock_flag;
  t_ulong_value decode_flag;
  t_ushort_value sprite_enable;
  t_ushort_value no_of_sprite_warping_points;
  t_ushort_value sprite_warping_accuracy;
  t_ushort_value sprite_brightness_change;
  t_short_value difftraj_x;
  t_short_value difftraj_y;
  t_ushort_value advanced_intra;
  t_ushort_value deblocking_filter;
  t_ushort_value slice_structure;
  t_ushort_value modified_quantizer;
} ts_t1xhv_vdc_mpeg4_param_in;

typedef ts_t1xhv_vdc_mpeg4_param_in* tps_t1xhv_vdc_mpeg4_param_in;

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg4_param_inout {
  t_ushort_value future_reference_vop;
  t_ushort_value reserved_1;
  t_long_value forward_time;
  t_long_value backward_time;
  t_ulong_value reserved_2;
} ts_t1xhv_vdc_mpeg4_param_inout;

typedef ts_t1xhv_vdc_mpeg4_param_inout* tps_t1xhv_vdc_mpeg4_param_inout;

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg4_param_out {
  t_ushort_value error_type;
  t_ushort_value picture_loss;
  t_ushort_value slice_loss_first_mb[8];
  t_ushort_value slice_loss_mb_num[8];
  t_ushort_value concealed_mb_num;
  t_ushort_value concealed_vp_num;
  t_ushort_value decoded_vp_num;
  t_ushort_value error_map[225];
  t_ulong_value reserved_2;
} ts_t1xhv_vdc_mpeg4_param_out;

typedef ts_t1xhv_vdc_mpeg4_param_out* tps_t1xhv_vdc_mpeg4_param_out;

#endif
