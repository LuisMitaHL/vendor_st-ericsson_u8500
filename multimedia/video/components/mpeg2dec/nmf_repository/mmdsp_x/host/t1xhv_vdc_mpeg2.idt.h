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

/* Generated t1xhv_vdc_mpeg2.idt defined type */
#if !defined(_t1xhv_vdc_mpeg2_idt)
#define _t1xhv_vdc_mpeg2_idt

#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg2_param_in {
  t_ushort_value vertical_size;
  t_ushort_value mb_width;
  t_ushort_value mb_height;
  t_ushort_value intra_quantizer_matrix[64];
  t_ushort_value non_intra_quantizer_matrix[64];
  t_ushort_value picture_coding_type;
  t_ushort_value full_pel_forward_vector;
  t_ushort_value forward_f_code;
  t_ushort_value full_pel_backward_vector;
  t_ushort_value backward_f_code;
  t_ushort_value f_code_0_0;
  t_ushort_value f_code_0_1;
  t_ushort_value f_code_1_0;
  t_ushort_value f_code_1_1;
  t_ushort_value intra_dc_precision;
  t_ushort_value picture_structure;
  t_ushort_value top_field_first;
  t_ushort_value frame_pred_frame_dct;
  t_ushort_value concealment_motion_vectors;
  t_ushort_value q_scale_type;
  t_ushort_value intra_vlc_format;
  t_ushort_value alternate_scan;
  t_ushort_value scalable_mode;
  t_ushort_value MPEG2_Flag;
  t_ulong_value reserved1;
} ts_t1xhv_vdc_mpeg2_param_in;

typedef ts_t1xhv_vdc_mpeg2_param_in* tps_t1xhv_vdc_mpeg2_param_in;

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg2_param_inout {
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_vdc_mpeg2_param_inout;

typedef ts_t1xhv_vdc_mpeg2_param_inout* tps_t1xhv_vdc_mpeg2_param_inout;

typedef struct t_xyuv_ts_t1xhv_vdc_mpeg2_param_out {
  t_ushort_value error_map[225];
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_vdc_mpeg2_param_out;

typedef ts_t1xhv_vdc_mpeg2_param_out* tps_t1xhv_vdc_mpeg2_param_out;

#endif
