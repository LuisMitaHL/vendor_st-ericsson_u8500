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

/* Generated t1xhv_vec_jpeg.idt defined type */
#if !defined(_t1xhv_vec_jpeg_idt)
#define _t1xhv_vec_jpeg_idt

#include <host/t1xhv_vec.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vec_jpeg_param_in {
  t_ushort_value frame_width;
  t_ushort_value frame_height;
  t_ushort_value window_width;
  t_ushort_value window_height;
  t_ushort_value window_horizontal_offset;
  t_ushort_value window_vertical_offset;
  t_ushort_value sampling_mode;
  t_ushort_value restart_interval;
  t_ushort_value quant_luma[64];
  t_ushort_value quant_chroma[64];
  t_ushort_value last_slice;
  t_ushort_value enable_optimized_quant;
  t_ushort_value target_bpp;
  t_ushort_value enable_optimized_huffman;
  t_ushort_value rotation;
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
} ts_t1xhv_vec_jpeg_param_in;

typedef ts_t1xhv_vec_jpeg_param_in* tps_t1xhv_vec_jpeg_param_in;

typedef struct t_xyuv_ts_t1xhv_vec_jpeg_param_inout {
  t_ushort_value restart_mcu_count;
  t_ushort_value dc_predictor_y;
  t_ushort_value dc_predictor_cb;
  t_ushort_value dc_predictor_cr;
  t_ushort_value restart_marker_id;
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
} ts_t1xhv_vec_jpeg_param_inout;

typedef ts_t1xhv_vec_jpeg_param_inout* tps_t1xhv_vec_jpeg_param_inout;

typedef struct t_xyuv_ts_t1xhv_vec_jpeg_param_out {
  t_ushort_value error_type;
  t_ushort_value reserved;
  t_ulong_value bitstream_size;
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
} ts_t1xhv_vec_jpeg_param_out;

typedef ts_t1xhv_vec_jpeg_param_out* tps_t1xhv_vec_jpeg_param_out;

#endif
