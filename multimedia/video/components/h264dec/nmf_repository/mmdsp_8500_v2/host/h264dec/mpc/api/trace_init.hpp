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

/* 'h264dec.mpc.api.trace_init' interface */
#if !defined(h264dec_mpc_api_trace_init_IDL)
#define h264dec_mpc_api_trace_init_IDL

#include <cpp.hpp>
#include <host/trace.idt.h>


class h264dec_mpc_api_trace_initDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) = 0;
};

class Ih264dec_mpc_api_trace_init: public NMF::InterfaceReference {
  public:
    Ih264dec_mpc_api_trace_init(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      ((h264dec_mpc_api_trace_initDescriptor*)reference)->traceInit(trace_info, id);
    }
};

#endif
