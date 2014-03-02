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

/* Generated C++ skeleton for 'vc1dec.mpc.ddep.api.end_codec' */
#include <host/vc1dec/mpc/ddep/api/end_codec.hpp>
static void sk_vc1dec_mpc_ddep_api_end_codec_endCodec(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  vc1dec_mpc_ddep_api_end_codecDescriptor *self = (vc1dec_mpc_ddep_api_end_codecDescriptor *)itfref;
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
  self->endCodec(status, info, durationInTicks);
}

static t_jump_method JT_vc1dec_mpc_ddep_api_end_codec[] = {
  (t_jump_method)sk_vc1dec_mpc_ddep_api_end_codec_endCodec,
};

static t_nmf_skel_function video_vc1dec_ddep_cpp_skel_functions[] = {
  {"vc1dec.mpc.ddep.api.end_codec", JT_vc1dec_mpc_ddep_api_end_codec},
};

/*const */t_nmf_skel_register video_vc1dec_ddep_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_vc1dec_ddep_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_vc1dec_ddep_cpp_skel_functions
};
