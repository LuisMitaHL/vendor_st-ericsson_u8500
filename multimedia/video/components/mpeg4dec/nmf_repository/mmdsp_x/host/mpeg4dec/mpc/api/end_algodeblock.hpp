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

/* 'mpeg4dec.mpc.api.end_algodeblock' interface */
#if !defined(mpeg4dec_mpc_api_end_algodeblock_IDL)
#define mpeg4dec_mpc_api_end_algodeblock_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_mpeg4.idt.h>


class mpeg4dec_mpc_api_end_algodeblockDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void endAlgoDeblock(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks, t_uint32 bitstream_size) = 0;
};

class Impeg4dec_mpc_api_end_algodeblock: public NMF::InterfaceReference {
  public:
    Impeg4dec_mpc_api_end_algodeblock(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void endAlgoDeblock(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks, t_uint32 bitstream_size) {
      ((mpeg4dec_mpc_api_end_algodeblockDescriptor*)reference)->endAlgoDeblock(status, info, durationInTicks, bitstream_size);
    }
};

#endif
