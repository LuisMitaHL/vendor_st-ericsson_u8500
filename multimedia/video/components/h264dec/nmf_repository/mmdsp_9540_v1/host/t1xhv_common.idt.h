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

/* Generated t1xhv_common.idt defined type */
#if !defined(_t1xhv_common_idt)
#define _t1xhv_common_idt

#include <host/t1xhv_retarget.idt.h>
#include <host/t1xhv_post_process.idt.h>

typedef struct t_xyuv_ts_t1xhv_bitstream_buf_pos {
  t_ahb_address addr_bitstream_buf_struct;
  t_ahb_address addr_bitstream_start;
  t_ulong_value bitstream_offset;
  t_ulong_value reserved_1;
} ts_t1xhv_bitstream_buf_pos;

typedef ts_t1xhv_bitstream_buf_pos* tps_t1xhv_bitstream_buf_pos;

typedef struct t_xyuv_ts_t1xhv_bitstream_buf {
  t_ahb_address addr_buffer_start;
  t_ahb_address addr_buffer_end;
  t_ahb_address addr_window_start;
  t_ahb_address addr_window_end;
} ts_t1xhv_bitstream_buf;

typedef ts_t1xhv_bitstream_buf* tps_t1xhv_bitstream_buf;

typedef struct t_xyuv_ts_t1xhv_bitstream_buf_link {
  t_ahb_address addr_next_buf_link;
  t_ahb_address addr_prev_buf_link;
  t_ahb_address addr_buffer_start;
  t_ahb_address addr_buffer_end;
} ts_t1xhv_bitstream_buf_link;

typedef ts_t1xhv_bitstream_buf_link* tps_t1xhv_bitstream_buf_link;

typedef enum t_xyuv_t_t1xhv_command {
  CMD_RESET,
  CMD_ABORT,
  CMD_START,
  CMD_UPDATE_BUFFER,
  CMD_STOP_SLICE} t_t1xhv_command;

typedef enum t_xyuv_t_t1xhv_status {
  STATUS_JOB_COMPLETE,
  STATUS_JOB_ABORTED,
  STATUS_JOB_UNKNOWN,
  STATUS_BUFFER_NEEDED,
  STATUS_PAYLOAD_BUFFER_NEEDED} t_t1xhv_status;

typedef enum t_xyuv_t_t1xhv_debug_mode {
  DBG_MODE_NORMAL,
  DBG_MODE_NO_HW,
  DBG_MODE_PATTERN} t_t1xhv_debug_mode;

typedef enum t_xyuv_t_t1xhv_algo_id {
  ID_VDC_MPEG4,
  ID_VDC_H263,
  ID_SDC_JPEG,
  ID_VDC_H264,
  ID_VDC_VC1,
  ID_VDC_MPEG4_RASTER_OUTPUT,
  ID_VDC_H263_RASTER_OUTPUT,
  ID_VDC_H264_RASTER_OUTPUT,
  ID_SDC_JPEG_NOSLICE,
  ID_VDC_MPEG2,
  ID_VDC_LAST,
  ID_VEC_MPEG4,
  ID_SEC_JPEG,
  ID_SEC_JPEG_THUMBNAIL,
  ID_SEC_JPEG_RASTER_INPUT,
  ID_VEC_H264,
  ID_VEC_LAST,
  ID_POST_PROCESS,
  ID_PP_LAST,
  ID_ALGO_ID_MAX,
  ID_ALGO_ID_ANY,
  ID_ALGO_ID_NONE} t_t1xhv_algo_id;

#endif
