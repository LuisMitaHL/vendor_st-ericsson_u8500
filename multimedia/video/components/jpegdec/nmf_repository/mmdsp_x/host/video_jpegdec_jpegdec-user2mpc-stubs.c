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

#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated host stub for 'jpegdec.mpc.api.algo' */
#include <host/jpegdec/mpc/api/algo.h>

static t_cm_error st_jpegdec_mpc_api_algo_controlAlgo(void* THIS, t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[4];
    /* command <t_t1xhv_command> marshalling */
  _xyuv_data[0] = (t_uint16)((unsigned int)command & 0xFFFFU);
  _xyuv_data[0+1] = (t_uint16)((unsigned int)command >> 16);
    /* channelNumber <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)channelNumber;
    /* param <t_uint16> marshalling */
  _xyuv_data[3] = (t_uint16)param;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 0);
}

static t_cm_error st_jpegdec_mpc_api_algo_setDebug(void* THIS, t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[6];
    /* mode <t_t1xhv_debug_mode> marshalling */
  _xyuv_data[0] = (t_uint16)((unsigned int)mode & 0xFFFFU);
  _xyuv_data[0+1] = (t_uint16)((unsigned int)mode >> 16);
    /* param1 <t_uint32> marshalling */
  _xyuv_data[2] = (t_uint16)param1;
  _xyuv_data[2+1] = (t_uint16)(param1 >> 16);
    /* param2 <t_uint32> marshalling */
  _xyuv_data[4] = (t_uint16)param2;
  _xyuv_data[4+1] = (t_uint16)(param2 >> 16);
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 1);
}

static t_cm_bf_host2mpc_handle* INITstub_jpegdec_mpc_api_algo(t_nmf_interface_desc* itf) {
  ((Ijpegdec_mpc_api_algo *)itf)->controlAlgo = st_jpegdec_mpc_api_algo_controlAlgo;
  ((Ijpegdec_mpc_api_algo *)itf)->setDebug = st_jpegdec_mpc_api_algo_setDebug;
  return (t_cm_bf_host2mpc_handle*)&((Ijpegdec_mpc_api_algo *)itf)->THIS;
}

static t_nmf_stub_function video_jpegdec_jpegdec_stub_functions[] = {
  {"jpegdec.mpc.api.algo", &INITstub_jpegdec_mpc_api_algo},
};

/*const */t_nmf_stub_register video_jpegdec_jpegdec_stub_register = {
    NMF_BC_IN_C,
    sizeof(video_jpegdec_jpegdec_stub_functions)/sizeof(t_nmf_stub_function),
    video_jpegdec_jpegdec_stub_functions
};
