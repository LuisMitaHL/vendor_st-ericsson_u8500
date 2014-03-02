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

/* 'h264dec.mpc.ddep.api.end_codec' interface */
#if !defined(h264dec_mpc_ddep_api_end_codec_IDL)
#define h264dec_mpc_ddep_api_end_codec_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_h264.idt.h>


class h264dec_mpc_ddep_api_end_codecDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void endCodec(t_uint32 status, t_uint32 info, t_uint32 durationInTicks) = 0;
};

class Ih264dec_mpc_ddep_api_end_codec: public NMF::InterfaceReference {
  public:
    Ih264dec_mpc_ddep_api_end_codec(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void endCodec(t_uint32 status, t_uint32 info, t_uint32 durationInTicks) {
      ((h264dec_mpc_ddep_api_end_codecDescriptor*)reference)->endCodec(status, info, durationInTicks);
    }
};

#endif
