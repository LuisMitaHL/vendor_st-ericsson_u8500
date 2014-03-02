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

/* Generated pcmdump/complete.idt defined type */
#if !defined(__COMPLETE_H_)
#define __COMPLETE_H_


typedef enum t_xyuv_t_complete_type {
  REGULAR_COMPLETE_CALL=-1} t_complete_type;

typedef struct t_xyuv_PcmDumpCompleteInfo_t {
  t_uint8 omx_port_idx;
  t_uint8 effect_position_idx;
  t_uint8 bufferIdx;
  t_sint24 offset;
} PcmDumpCompleteInfo_t;

#endif
