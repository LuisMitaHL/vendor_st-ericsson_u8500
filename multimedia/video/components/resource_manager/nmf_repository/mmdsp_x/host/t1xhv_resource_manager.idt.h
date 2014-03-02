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

/* Generated t1xhv_resource_manager.idt defined type */
#if !defined(_t1xhv_resource_manager_idt)
#define _t1xhv_resource_manager_idt


typedef enum t_xyuv_t_t1xhv_buffer_status {
  BUFFER_BOW,
  BUFFER_EOW} t_t1xhv_buffer_status;

typedef enum t_xyuv_t_codec_type {
  ID_DECODER,
  ID_ENCODER} t_codec_type;

typedef enum t_xyuv_t_t1xhv_resource {
  RESOURCE_MMDSP,
  RESOURCE_MTF,
  RESOURCE_VPP,
  RESOURCE_CABAC,
  RESOURCE_MAX} t_t1xhv_resource;

typedef enum t_xyuv_t_t1xhv_resource_status {
  STA_RESOURCE_FREE,
  STA_PARAM_ERROR,
  STA_ERROR_ALLOC_FULL,
  STA_RESOURCE_LOST} t_t1xhv_resource_status;

#endif
