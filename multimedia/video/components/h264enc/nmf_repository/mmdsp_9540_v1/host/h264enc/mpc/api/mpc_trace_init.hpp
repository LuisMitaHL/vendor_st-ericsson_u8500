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

/* 'h264enc.mpc.api.mpc_trace_init' interface */
#if !defined(h264enc_mpc_api_mpc_trace_init_IDL)
#define h264enc_mpc_api_mpc_trace_init_IDL

#include <cpp.hpp>
#include <host/trace.idt.h>


class h264enc_mpc_api_mpc_trace_initDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void traceInit(t_uint16 addrH, t_uint16 addrL, t_uint16 id) = 0;
};

class Ih264enc_mpc_api_mpc_trace_init: public NMF::InterfaceReference {
  public:
    Ih264enc_mpc_api_mpc_trace_init(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void traceInit(t_uint16 addrH, t_uint16 addrL, t_uint16 id) {
      ((h264enc_mpc_api_mpc_trace_initDescriptor*)reference)->traceInit(addrH, addrL, id);
    }
};

#endif
