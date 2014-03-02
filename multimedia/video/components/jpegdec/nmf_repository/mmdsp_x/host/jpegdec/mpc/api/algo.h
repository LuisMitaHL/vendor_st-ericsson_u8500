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

/* 'jpegdec.mpc.api.algo' interface */
#if !defined(jpegdec_mpc_api_algo_IDL)
#define jpegdec_mpc_api_algo_IDL

#include <cm/inc/cm_type.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_jpeg.idt.h>


typedef struct sIjpegdec_mpc_api_algo {
  void* THIS;
  t_cm_error (*controlAlgo)(void* THIS, t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param);
  t_cm_error (*setDebug)(void* THIS, t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2);
} Ijpegdec_mpc_api_algo;

typedef struct {
  void* THIS;
  void (*controlAlgo)(void* THIS, t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param);
  void (*setDebug)(void* THIS, t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2);
} CBjpegdec_mpc_api_algo;

#endif
