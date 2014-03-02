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

/* 'vc1dec.mpc.ddep.api.start_codec' interface */
#if !defined(vc1dec_mpc_ddep_api_start_codec_IDL)
#define vc1dec_mpc_ddep_api_start_codec_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc.idt.h>


class vc1dec_mpc_ddep_api_start_codecDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) = 0;
    virtual void initCodec(t_uint32 param_in) = 0;
    virtual void voidFrame(void) = 0;
    virtual void allowSleep(void) = 0;
    virtual void preventSleep(void) = 0;
};

class Ivc1dec_mpc_ddep_api_start_codec: public NMF::InterfaceReference {
  public:
    Ivc1dec_mpc_ddep_api_start_codec(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) {
      ((vc1dec_mpc_ddep_api_start_codecDescriptor*)reference)->startCodec(addr_in_frame_buffer, addr_out_frame_buffer, addr_internal_buffer, addr_in_bitstream_buffer, addr_out_bitstream_buffer, addr_in_parameters, addr_out_parameters, addr_in_frame_parameters, addr_out_frame_parameters);
    }
    void initCodec(t_uint32 param_in) {
      ((vc1dec_mpc_ddep_api_start_codecDescriptor*)reference)->initCodec(param_in);
    }
    void voidFrame(void) {
      ((vc1dec_mpc_ddep_api_start_codecDescriptor*)reference)->voidFrame();
    }
    void allowSleep(void) {
      ((vc1dec_mpc_ddep_api_start_codecDescriptor*)reference)->allowSleep();
    }
    void preventSleep(void) {
      ((vc1dec_mpc_ddep_api_start_codecDescriptor*)reference)->preventSleep();
    }
};

#endif
