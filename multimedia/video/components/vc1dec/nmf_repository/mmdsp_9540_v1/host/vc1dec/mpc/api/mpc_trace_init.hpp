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

/* 'vc1dec.mpc.api.mpc_trace_init' interface */
#if !defined(vc1dec_mpc_api_mpc_trace_init_IDL)
#define vc1dec_mpc_api_mpc_trace_init_IDL

#include <cpp.hpp>
#include <host/trace.idt.h>


class vc1dec_mpc_api_mpc_trace_initDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) = 0;
};

class Ivc1dec_mpc_api_mpc_trace_init: public NMF::InterfaceReference {
  public:
    Ivc1dec_mpc_api_mpc_trace_init(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      ((vc1dec_mpc_api_mpc_trace_initDescriptor*)reference)->traceInit(trace_info, id);
    }
};

#endif
