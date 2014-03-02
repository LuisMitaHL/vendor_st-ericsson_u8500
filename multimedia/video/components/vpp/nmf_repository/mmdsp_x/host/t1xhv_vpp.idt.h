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

/* Generated t1xhv_vpp.idt defined type */
#if !defined(_t1xhv_vpp_idt)
#define _t1xhv_vpp_idt

#include <host/t1xhv_retarget.idt.h>

typedef enum t_xyuv_t_t1xhv_vpp_info {
  VPP_ERT_NONE,
  VPP_VOID,
  VPP_ERT,
  VPP_ERT_BAD_FORMAT_CONV_TYPE,
  VPP_ERT_BAD_ADD_SRC_BUFFER,
  VPP_ERT_BAD_ADD_DEST_BUFFER,
  VPP_ERT_BAD_ADD_DBLK_PARAM_BUFFER,
  VPP_ERT_BAD_TASK_TYPE,
  VPP_ERT_BAD_OFFSET_X,
  VPP_ERT_BAD_OFFSET_Y,
  VPP_ERT_BAD_INPUT_WIDTH,
  VPP_ERT_BAD_INPUT_HEIGHT,
  VPP_ERT_BAD_OUTPUT_WIDTH,
  VPP_ERT_BAD_OUTPUT_HEIGHT,
  VPP_ERT_NOT_SUPPORTED} t_t1xhv_vpp_info;

typedef enum t_xyuv_t_t1xhv_format_conv_type {
  YUV420RasterPlaner_YUV420MB,
  YUV420MB_YUV420MB} t_t1xhv_format_conv_type;

typedef enum t_xyuv_t_task_type {
  TASK_CONV_FROM_420=0,
  TASK_CONV_FROM_422=1,
  TASK_H263_INLOOP_SW=2,
  TASK_H264_INLOOP=3,
  TASK_VC1_INLOOP=4,
  TASK_MPEG4_POST_FLT_HW=5,
  TASK_DIVX6_POST_FLT_HW=6,
  TASK_H264RCDO_INLOOP=7,
  TASK_MPEG4_POST_DRG_HW=8,
  TASK_MPEG4_POST_FLT_DRG_HW=9,
  TASK_DIVX_POST_FLT_DRG_SW=10,
  TASK_DIVX_POST_FLT_DRG_HW=11,
  TASK_H263_INLOOP_HW=12,
  TASK_MPEG4_POST_FLT_DRG_SW=13} t_task_type;

#endif
