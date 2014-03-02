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

/* 'h264dec.mpc.ddep.api.start_codec' interface */
#if !defined(h264dec_mpc_ddep_api_start_codec_IDL)
#define h264dec_mpc_ddep_api_start_codec_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_h264.idt.h>


class h264dec_mpc_ddep_api_start_codecDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void startCodecHed(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters, t_uint32 enable_preproc, t_uint32 bitstream_start, t_uint32 bitstream_end, t_uint32 ib_start, t_uint32 hed_cfg, t_uint32 hed_picwidth, t_uint32 hed_codelength) = 0;
    virtual void flush(void) = 0;
    virtual void voidFrameHed(void) = 0;
    virtual void allowSleep(void) = 0;
    virtual void preventSleep(void) = 0;
};

class Ih264dec_mpc_ddep_api_start_codec: public NMF::InterfaceReference {
  public:
    Ih264dec_mpc_ddep_api_start_codec(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void startCodecHed(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters, t_uint32 enable_preproc, t_uint32 bitstream_start, t_uint32 bitstream_end, t_uint32 ib_start, t_uint32 hed_cfg, t_uint32 hed_picwidth, t_uint32 hed_codelength) {
      ((h264dec_mpc_ddep_api_start_codecDescriptor*)reference)->startCodecHed(addr_in_frame_buffer, addr_out_frame_buffer, addr_internal_buffer, addr_in_bitstream_buffer, addr_out_bitstream_buffer, addr_in_parameters, addr_out_parameters, addr_in_frame_parameters, addr_out_frame_parameters, enable_preproc, bitstream_start, bitstream_end, ib_start, hed_cfg, hed_picwidth, hed_codelength);
    }
    void flush(void) {
      ((h264dec_mpc_ddep_api_start_codecDescriptor*)reference)->flush();
    }
    void voidFrameHed(void) {
      ((h264dec_mpc_ddep_api_start_codecDescriptor*)reference)->voidFrameHed();
    }
    void allowSleep(void) {
      ((h264dec_mpc_ddep_api_start_codecDescriptor*)reference)->allowSleep();
    }
    void preventSleep(void) {
      ((h264dec_mpc_ddep_api_start_codecDescriptor*)reference)->preventSleep();
    }
};

#endif
