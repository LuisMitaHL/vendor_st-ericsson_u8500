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

/* Generated t1xhv_post_process.idt defined type */
#if !defined(_t1xhv_post_process_idt)
#define _t1xhv_post_process_idt

#include <host/t1xhv_pp.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_pp_param_in {
  t_ushort_value source_frame_width;
  t_ushort_value source_frame_height;
  t_ushort_value input_format;
  t_ushort_value output_format;
  t_ushort_value filter_type;
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
} ts_t1xhv_pp_param_in;

#define VPP_MACROBLOCK_TILED 0

#define VPP_RASTER_PLANAR 1

#define VPP_420_MB_OUT 0

#define VPP_420_PLANAR_OUT 1

#define VPP_422_ITLVD_OUT 2

#define VPP_MPEG4_DEBLOCKING_FILTER 0

#define VPP_MPEG4_DERINGING_FILTER 1

#define VPP_MPEG4_DEBLOCKING_DERINGING_FILTER 2

#define VPP_DIVX6_DEBLOCKING_FILTER 3

#define VPP_DIVX6_DERINGING_FILTER 4

#define VPP_DIVX6_DEBLOCKING_DERINGING_FILTER 5

typedef ts_t1xhv_pp_param_in* tps_t1xhv_pp_param_in;

typedef struct t_xyuv_ts_t1xhv_pp_param_inout {
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_pp_param_inout;

typedef ts_t1xhv_pp_param_inout* tps_t1xhv_pp_param_inout;

typedef struct t_xyuv_ts_t1xhv_pp_param_out {
  t_ushort_value error_type;
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_pp_param_out;

typedef ts_t1xhv_pp_param_out* tps_t1xhv_pp_param_out;

#endif
