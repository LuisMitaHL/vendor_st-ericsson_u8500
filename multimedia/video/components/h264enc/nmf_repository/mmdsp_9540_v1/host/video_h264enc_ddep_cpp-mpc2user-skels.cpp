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

/* Generated C++ skeleton for 'h264enc.mpc.api.mpc_trace_init_ack' */
#include <host/h264enc/mpc/api/mpc_trace_init_ack.hpp>
static void sk_h264enc_mpc_api_mpc_trace_init_ack_traceInitAck(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  h264enc_mpc_api_mpc_trace_init_ackDescriptor *self = (h264enc_mpc_api_mpc_trace_init_ackDescriptor *)itfref;
  /* Server calling */
  self->traceInitAck();
}

static t_jump_method JT_h264enc_mpc_api_mpc_trace_init_ack[] = {
  (t_jump_method)sk_h264enc_mpc_api_mpc_trace_init_ack_traceInitAck,
};

/* Generated C++ skeleton for 'h264enc.mpc.ddep.api.end_codec' */
#include <host/h264enc/mpc/ddep/api/end_codec.hpp>
static void sk_h264enc_mpc_ddep_api_end_codec_endCodec(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  h264enc_mpc_ddep_api_end_codecDescriptor *self = (h264enc_mpc_ddep_api_end_codecDescriptor *)itfref;
  t_uint32 status;
  t_uint32 info;
  t_uint32 durationInTicks;
    /* status <t_uint32> marshalling */
  status = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
    /* info <t_uint32> marshalling */
  info = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* durationInTicks <t_uint32> marshalling */
  durationInTicks = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->endCodec(status, info, durationInTicks);
}

static t_jump_method JT_h264enc_mpc_ddep_api_end_codec[] = {
  (t_jump_method)sk_h264enc_mpc_ddep_api_end_codec_endCodec,
};

static t_nmf_skel_function video_h264enc_ddep_cpp_skel_functions[] = {
  {"h264enc.mpc.api.mpc_trace_init_ack", JT_h264enc_mpc_api_mpc_trace_init_ack},
  {"h264enc.mpc.ddep.api.end_codec", JT_h264enc_mpc_ddep_api_end_codec},
};

/*const */t_nmf_skel_register video_h264enc_ddep_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_h264enc_ddep_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_h264enc_ddep_cpp_skel_functions
};
