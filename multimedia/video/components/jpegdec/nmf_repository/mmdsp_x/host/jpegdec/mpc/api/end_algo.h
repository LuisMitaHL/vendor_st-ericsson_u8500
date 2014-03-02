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

/* 'jpegdec.mpc.api.end_algo' interface */
#if !defined(jpegdec_mpc_api_end_algo_IDL)
#define jpegdec_mpc_api_end_algo_IDL

#include <cm/inc/cm_type.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_jpeg.idt.h>


typedef struct sIjpegdec_mpc_api_end_algo {
  void* THIS;
  t_cm_error (*endAlgo)(void* THIS, t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks);
} Ijpegdec_mpc_api_end_algo;

typedef struct {
  void* THIS;
  void (*endAlgo)(void* THIS, t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks);
} CBjpegdec_mpc_api_end_algo;

#endif
