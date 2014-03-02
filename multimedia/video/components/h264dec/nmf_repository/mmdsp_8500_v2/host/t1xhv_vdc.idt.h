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

/* Generated t1xhv_vdc.idt defined type */
#if !defined(_t1xhv_vdc_idt)
#define _t1xhv_vdc_idt

#include <host/t1xhv_retarget.idt.h>

typedef enum t_xyuv_t_t1xhv_decoder_info {
  VDC_ERT_NONE,
  VDC_VOID,
  VDC_ERT,
  VDC_ERT_END_OF_BITSREAM_REACHED,
  VDC_ERT_BEGINNING_OF_BITSREAM_REACHED,
  VDC_ERT_NOT_SUPPORTED,
  VDC_ERT_BAD_PARAMETER,
  VDC_ERT_BITSTREAM_ERR,
  VDC_ERT_BAD_ADD_FWD_REF_BUFFER,
  VDC_ERT_BAD_ADD_DEST_BUFFER,
  VDC_ERT_BAD_ADD_DEBLOCKING_PARAM_BUFFER,
  VDC_ERT_BAD_ADD_BITSTREAM_START,
  VDC_ERT_BAD_ADD_BUFFER_START,
  VDC_ERT_BAD_ADD_BUFFER_END,
  VDC_ERT_BAD_ADD_WINDOW_START,
  VDC_ERT_BAD_ADD_WINDOW_END,
  VDC_ERT_BAD_FRAME_WIDTH,
  VDC_ERT_BAD_FRAME_HEIGHT,
  VDC_ERT_BAD_QUANT,
  VDC_ERT_BAD_VOP_FCODE_FORWARD,
  VDC_ERT_BAD_TIME_INCREMENT_RESOLUTION,
  VDC_ERT_EAD_DIFF_EAJ,
  VDC_ERT_EAF_DIFF_EAJ,
  VDC_ERT_DATA_PARTITIONED_WITH_FS_GT_CIF} t_t1xhv_decoder_info;

typedef struct t_xyuv_ts_t1xhv_vdc_frame_buf_in {
  t_ahb_address addr_fwd_ref_buffer;
  t_ahb_address addr_bwd_ref_buffer;
  t_ahb_address addr_source_buffer;
  t_ahb_address addr_deblocking_param_buffer_in;
} ts_t1xhv_vdc_frame_buf_in;

typedef ts_t1xhv_vdc_frame_buf_in* tps_t1xhv_vdc_frame_buf_in;

typedef struct t_xyuv_ts_t1xhv_vdc_frame_buf_out {
  t_ahb_address addr_dest_buffer;
  t_ahb_address addr_deblocking_param_buffer;
  t_ahb_address addr_motion_vector_buffer;
  t_ahb_address addr_jpeg_coef_buffer;
  t_ahb_address addr_jpeg_line_buffer;
  t_ahb_address addr_dest_local_rec_buffer;
  t_ahb_address addr_dest_buffer_deblock;
  t_ahb_address addr_debug_mtf_buffer;
} ts_t1xhv_vdc_frame_buf_out;

typedef ts_t1xhv_vdc_frame_buf_out* tps_t1xhv_vdc_frame_buf_out;

typedef struct t_xyuv_ts_t1xhv_vdc_internal_buf {
  t_ahb_address addr_vpp_dummy_buffer;
  t_ahb_address addr_h264d_block_info;
  t_ahb_address addr_h264d_mb_slice_map;
  t_ahb_address addr_mv_history_buffer;
  t_ahb_address addr_mv_type_buffer;
  t_ahb_address addr_mb_not_coded;
  t_ahb_address addr_x_err_res_buffer;
  t_ahb_address addr_y_err_res_buffer;
} ts_t1xhv_vdc_internal_buf;

typedef ts_t1xhv_vdc_internal_buf* tps_t1xhv_vdc_internal_buf;

#endif
