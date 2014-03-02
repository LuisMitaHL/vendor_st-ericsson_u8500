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

/* Generated t1xhv_vec.idt defined type */
#if !defined(_t1xhv_vec_idt)
#define _t1xhv_vec_idt

#include <host/t1xhv_retarget.idt.h>

typedef enum t_xyuv_t_t1xhv_encoder_info {
  VEC_ERT_NONE,
  VEC_VOID,
  VEC_INFO_SKIP,
  VEC_ERT,
  VEC_ERT_NOT_SUPPORTED,
  VEC_ERT_BAD_PARAMETER,
  VEC_ERT_FATAL_ERROR,
  VEC_ERT_ADDR_SOURCE_BUFFER,
  VEC_ERT_ADDR_FWD_REF_BUFFER,
  VEC_ERT_ADDR_GRAB_REF_BUFFER,
  VEC_ERT_ADDR_DEST_BUFFER,
  VEC_ERT_ADDR_DEBLOCKING_PARAM_BUFFER,
  VEC_ERT_ADDR_MOTION_VECTOR_BUFFER,
  VEC_ERT_ADDR_INTRA_REFRESH_BUFFER,
  VEC_ERT_ADDR_SEARCH_WINDOW_BUFFER,
  VEC_ERT_ADDR_SEARCH_WINDOW_END,
  VEC_ERT_ADDR_JPEG_RUN_LEVEL_BUFFER,
  VEC_ERT_ADDR_HEADER_BUFFER,
  VEC_ERT_ADDR_BITSTREAM_START,
  VEC_ERT_ADDR_BUFFER_START,
  VEC_ERT_ADDR_BUFFER_END,
  VEC_ERT_ADDR_WINDOW_START,
  VEC_ERT_ADDR_WINDOW_END,
  VEC_ERT_FRAME_WIDTH,
  VEC_ERT_FRAME_HEIGHT,
  VEC_ERT_WINDOW_WIDTH,
  VEC_ERT_WINDOW_HEIGHT,
  VEC_ERT_WINDOW_HORIZONTAL_OFFSET,
  VEC_ERT_WINDOW_VERTICAL_OFFSET,
  VEC_ERT_QUANT,
  VEC_ERT_VOP_FCODE_FORWARD,
  VEC_ERT_QUANT_TABLE,
  VEC_ERT_DATA_PARTITIONED,
  ERR_ENCODE_NOT_FOUND} t_t1xhv_encoder_info;

typedef struct t_xyuv_ts_t1xhv_vec_frame_buf_in {
  t_ahb_address addr_source_buffer;
  t_ahb_address addr_fwd_ref_buffer;
  t_ahb_address addr_grab_ref_buffer;
  t_ahb_address addr_intra_refresh_buffer;
} ts_t1xhv_vec_frame_buf_in;

typedef ts_t1xhv_vec_frame_buf_in* tps_t1xhv_vec_frame_buf_in;

typedef struct t_xyuv_ts_t1xhv_vec_frame_buf_out {
  t_ahb_address addr_dest_buffer;
  t_ahb_address addr_deblocking_param_buffer;
  t_ahb_address addr_motion_vector_buffer;
  t_ahb_address addr_intra_refresh_buffer;
  t_ahb_address addr_ime_mv_field_buffer;
  t_ahb_address addr_ime_mv_field_buffer_end;
  t_ahb_address addr_debug_mtf_buffer;
  t_ahb_address reserved_3;
} ts_t1xhv_vec_frame_buf_out;

typedef ts_t1xhv_vec_frame_buf_out* tps_t1xhv_vec_frame_buf_out;

typedef struct t_xyuv_ts_t1xhv_vec_internal_buf {
  t_ahb_address addr_search_window_buffer;
  t_ahb_address addr_search_window_end;
  t_ahb_address addr_jpeg_run_level_buffer;
  t_ahb_address addr_h264e_H4D_buffer;
  t_ahb_address addr_h264e_rec_local;
  t_ahb_address addr_h264e_metrics;
  t_ahb_address addr_h264e_cup_context;
  t_ahb_address reserved_1;
} ts_t1xhv_vec_internal_buf;

typedef ts_t1xhv_vec_internal_buf* tps_t1xhv_vec_internal_buf;

typedef struct t_xyuv_ts_t1xhv_vec_header_buf {
  t_ahb_address addr_header_buffer;
  t_ulong_value header_size;
  t_ahb_address reserved_1;
  t_ahb_address reserved_2;
} ts_t1xhv_vec_header_buf;

typedef ts_t1xhv_vec_header_buf* tps_t1xhv_vec_header_buf;

#endif
