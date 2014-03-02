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

/* Generated host skeleton for 'h264dec.mpc.api.end_cabac' */
#include <host/h264dec/mpc/api/end_cabac.h>
static void sk_h264dec_mpc_api_end_cabac_endCabac(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Ih264dec_mpc_api_end_cabac *self = (Ih264dec_mpc_api_end_cabac *)itfref;
  t_t1xhv_status status;
  t_t1xhv_decoder_info info;
  t_uint32 durationInTicks;
    /* status <t_t1xhv_status> marshalling */
  status = (t_t1xhv_status)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* info <t_t1xhv_decoder_info> marshalling */
  info = (t_t1xhv_decoder_info)((t_uint16)_xyuv_data[2] | ((t_uint16)_xyuv_data[2+1] << 16));
    /* durationInTicks <t_uint32> marshalling */
  durationInTicks = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->endCabac(self->THIS, status, info, durationInTicks);
}

static t_jump_method JT_h264dec_mpc_api_end_cabac[] = {
  (t_jump_method)sk_h264dec_mpc_api_end_cabac_endCabac,
};

static t_nmf_skel_function video_h264dec_cabac_skel_functions[] = {
  {"h264dec.mpc.api.end_cabac", JT_h264dec_mpc_api_end_cabac},
};

/*const */t_nmf_skel_register video_h264dec_cabac_skel_register = {
    NMF_BC_IN_C,
    sizeof(video_h264dec_cabac_skel_functions)/sizeof(t_nmf_skel_function),
    video_h264dec_cabac_skel_functions
};
