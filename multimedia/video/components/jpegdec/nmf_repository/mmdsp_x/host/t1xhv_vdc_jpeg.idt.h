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

/* Generated t1xhv_vdc_jpeg.idt defined type */
#if !defined(_t1xhv_vdc_jpeg_idt)
#define _t1xhv_vdc_jpeg_idt

#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vdc_hw_table {
  t_ushort_value huffman_y_code_dc[12];
  t_ushort_value huffman_y_size_dc[12];
  t_ushort_value huffman_y_code_ac[256];
  t_ushort_value huffman_y_size_ac[256];
  t_ushort_value huffman_cb_code_dc[12];
  t_ushort_value huffman_cb_size_dc[12];
  t_ushort_value huffman_cb_code_ac[256];
  t_ushort_value huffman_cb_size_ac[256];
  t_ushort_value huffman_cr_code_dc[12];
  t_ushort_value huffman_cr_size_dc[12];
  t_ushort_value huffman_cr_code_ac[256];
  t_ushort_value huffman_cr_size_ac[256];
} ts_t1xhv_vdc_hw_table;

typedef ts_t1xhv_vdc_hw_table* tps_t1xhv_vdc_hw_table;

typedef struct t_xyuv_ts_t1xhv_vdc_sw_dc_table {
  t_ushort_value huffman_bits_dc[16];
  t_ushort_value huffman_val_dc[12];
  t_long_value maxcode[18];
  t_long_value valoffset[17];
  t_short_value look_nbits[256];
  t_ushort_value look_sym[256];
} ts_t1xhv_vdc_sw_dc_table;

typedef struct t_xyuv_ts_t1xhv_vdc_sw_ac_table {
  t_ushort_value huffman_bits_ac[16];
  t_ushort_value huffman_val_ac[256];
  t_long_value maxcode[18];
  t_long_value valoffset[17];
  t_short_value look_nbits[256];
  t_ushort_value look_sym[256];
} ts_t1xhv_vdc_sw_ac_table;

typedef struct t_xyuv_ts_t1xhv_vdc_sw_table {
  ts_t1xhv_vdc_sw_dc_table huffman_y_dc_table;
  ts_t1xhv_vdc_sw_ac_table huffman_y_ac_table;
  ts_t1xhv_vdc_sw_dc_table huffman_cb_dc_table;
  ts_t1xhv_vdc_sw_ac_table huffman_cb_ac_table;
  ts_t1xhv_vdc_sw_dc_table huffman_cr_dc_table;
  ts_t1xhv_vdc_sw_ac_table huffman_cr_ac_table;
} ts_t1xhv_vdc_sw_table;

typedef ts_t1xhv_vdc_sw_table* tps_t1xhv_vdc_sw_table;

typedef struct t_xyuv_ts_t1xhv_vdc_huff_table {
  ts_t1xhv_vdc_hw_table hw_huff_table;
  t_ulong_value sw_huff_table;
  t_long_value reserved1;
  t_ulong_value reserved2;
  t_long_value reserved3;
} ts_t1xhv_vdc_huff_table;

typedef ts_t1xhv_vdc_huff_table* tps_t1xhv_vdc_huff_table;

typedef struct t_xyuv_ts_t1xhv_vdc_jpeg_param_in {
  t_ushort_value frame_width;
  t_ushort_value frame_height;
  t_ushort_value nb_components;
  t_ushort_value h_sampling_factor_y;
  t_ushort_value v_sampling_factor_y;
  t_ushort_value h_sampling_factor_cb;
  t_ushort_value v_sampling_factor_cb;
  t_ushort_value h_sampling_factor_cr;
  t_ushort_value v_sampling_factor_cr;
  t_ushort_value downsampling_factor;
  t_ushort_value restart_interval;
  t_ushort_value progressive_mode;
  t_ushort_value nb_scan_components;
  t_ushort_value component_selector_y;
  t_ushort_value component_selector_cb;
  t_ushort_value component_selector_cr;
  t_ushort_value start_spectral_selection;
  t_ushort_value end_spectral_selection;
  t_ushort_value successive_approx_position;
  t_ushort_value ace_enable;
  t_ushort_value ace_strength;
  t_ushort_value noslice_enable;
  t_ulong_value buffer_size;
  t_ushort_value quant_y[64];
  t_ushort_value quant_cb[64];
  t_ushort_value quant_cr[64];
  ts_t1xhv_vdc_huff_table huff_table;
  t_ushort_value window_width;
  t_ushort_value window_height;
  t_ushort_value window_horizontal_offset;
  t_ushort_value window_vertical_offset;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_vdc_jpeg_param_in;

typedef ts_t1xhv_vdc_jpeg_param_in* tps_t1xhv_vdc_jpeg_param_in;

typedef struct t_xyuv_ts_t1xhv_vdc_jpeg_param_inout {
  t_ulong_value mcu_index;
  t_ulong_value end_of_band_run;
  t_ushort_value dc_predictor_y;
  t_ushort_value dc_predictor_cb;
  t_ushort_value dc_predictor_cr;
  t_ushort_value slice_pos;
  t_ulong_value ace_count0;
  t_ulong_value ace_count1;
  t_ulong_value ace_count2;
  t_ulong_value ace_count3;
  t_ulong_value crop_mcu_index;
  t_ulong_value crop_mcu_index_in_row;
  t_ushort_value mcu_index_rst;
  t_ushort_value rst_value;
  t_ulong_value reserved_1;
} ts_t1xhv_vdc_jpeg_param_inout;

typedef ts_t1xhv_vdc_jpeg_param_inout* tps_t1xhv_vdc_jpeg_param_inout;

typedef struct t_xyuv_ts_t1xhv_vdc_jpeg_param_out {
  t_ushort_value error_type;
  t_ushort_value last_slice;
  t_ushort_value ace_offset0;
  t_ushort_value ace_offset1;
  t_ushort_value ace_offset2;
  t_ushort_value ace_offset3;
  t_ulong_value out_buff_size;
} ts_t1xhv_vdc_jpeg_param_out;

typedef ts_t1xhv_vdc_jpeg_param_out* tps_t1xhv_vdc_jpeg_param_out;

#endif
