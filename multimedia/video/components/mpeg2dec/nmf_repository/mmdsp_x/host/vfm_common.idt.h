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

/* Generated vfm_common.idt defined type */
#if !defined(_VFM_COMMON_IDT_)
#define _VFM_COMMON_IDT_


typedef struct t_xyuv_ts_ddep_buffer_descriptor {
  t_uint32 nSize;
  t_uint32 nPhysicalAddress;
  t_uint32 nLogicalAddress;
  t_uint32 nMpcAddress;
} ts_ddep_buffer_descriptor;

typedef struct t_xyuv_ts_ddep_perf_param_out {
  t_uint32 control_algo_ticks[10];
  t_uint32 end_algo_ticks[10];
  t_uint32 acc_ticks;
  t_uint32 count;
} ts_ddep_perf_param_out;

#endif
