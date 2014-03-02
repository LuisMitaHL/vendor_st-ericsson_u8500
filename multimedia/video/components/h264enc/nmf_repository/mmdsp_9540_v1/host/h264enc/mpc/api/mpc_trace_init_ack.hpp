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

/* 'h264enc.mpc.api.mpc_trace_init_ack' interface */
#if !defined(h264enc_mpc_api_mpc_trace_init_ack_IDL)
#define h264enc_mpc_api_mpc_trace_init_ack_IDL

#include <cpp.hpp>
#include <host/trace.idt.h>


class h264enc_mpc_api_mpc_trace_init_ackDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void traceInitAck(void) = 0;
};

class Ih264enc_mpc_api_mpc_trace_init_ack: public NMF::InterfaceReference {
  public:
    Ih264enc_mpc_api_mpc_trace_init_ack(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void traceInitAck(void) {
      ((h264enc_mpc_api_mpc_trace_init_ackDescriptor*)reference)->traceInitAck();
    }
};

#endif
