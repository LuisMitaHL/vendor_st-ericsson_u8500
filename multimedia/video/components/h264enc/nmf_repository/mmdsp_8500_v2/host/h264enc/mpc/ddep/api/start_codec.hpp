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

/* 'h264enc.mpc.ddep.api.start_codec' interface */
#if !defined(h264enc_mpc_ddep_api_start_codec_IDL)
#define h264enc_mpc_ddep_api_start_codec_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vec_h264.idt.h>


class h264enc_mpc_ddep_api_start_codecDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_header_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) = 0;
    virtual void flush(void) = 0;
};

class Ih264enc_mpc_ddep_api_start_codec: public NMF::InterfaceReference {
  public:
    Ih264enc_mpc_ddep_api_start_codec(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_header_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) {
      ((h264enc_mpc_ddep_api_start_codecDescriptor*)reference)->startCodec(addr_in_frame_buffer, addr_out_frame_buffer, addr_internal_buffer, addr_in_header_buffer, addr_in_bitstream_buffer, addr_out_bitstream_buffer, addr_in_parameters, addr_out_parameters, addr_in_frame_parameters, addr_out_frame_parameters);
    }
    void flush(void) {
      ((h264enc_mpc_ddep_api_start_codecDescriptor*)reference)->flush();
    }
};

#endif
