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

/* Generated t1xhv_pp.idt defined type */
#if !defined(_t1xhv_pp_idt)
#define _t1xhv_pp_idt

#include <host/t1xhv_retarget.idt.h>

typedef enum t_xyuv_t_t1xhv_post_process_info {
  PP_ERT_NONE,
  PP_ERT,
  PP_ERT_NOT_SUPPORTED,
  PP_ERT_BAD_PARAMETER,
  PP_ERT_BAD_ADD_SOURCE_BUFFER,
  PP_ERT_BAD_ADD_DEBLOCKING_PARAM_BUFFER,
  PP_ERT_BAD_ADD_DEST_BUFFER,
  PP_ERT_BAD_FRAME_WIDTH,
  PP_ERT_BAD_FRAME_HEIGHT,
  PP_ERT_END_OF_FRAME_REACHED} t_t1xhv_post_process_info;

typedef struct t_xyuv_ts_t1xhv_pp_frame_buf_in {
  t_ahb_address addr_source_buffer;
  t_ahb_address addr_deblocking_param_buffer;
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
} ts_t1xhv_pp_frame_buf_in;

typedef ts_t1xhv_pp_frame_buf_in* tps_t1xhv_pp_frame_buf_in;

typedef struct t_xyuv_ts_t1xhv_pp_frame_buf_out {
  t_ahb_address addr_dest_buffer;
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
} ts_t1xhv_pp_frame_buf_out;

typedef ts_t1xhv_pp_frame_buf_out* tps_t1xhv_pp_frame_buf_out;

typedef struct t_xyuv_ts_t1xhv_pp_internal_buf {
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_pp_internal_buf;

typedef ts_t1xhv_pp_internal_buf* tps_t1xhv_pp_internal_buf;

#endif
