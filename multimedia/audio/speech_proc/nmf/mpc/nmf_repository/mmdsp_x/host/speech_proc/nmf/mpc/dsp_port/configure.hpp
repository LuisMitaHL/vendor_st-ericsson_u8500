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

/* 'speech_proc.nmf.mpc.dsp_port.configure' interface */
#if !defined(speech_proc_nmf_mpc_dsp_port_configure_IDL)
#define speech_proc_nmf_mpc_dsp_port_configure_IDL

#include <cpp.hpp>


class speech_proc_nmf_mpc_dsp_port_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_uint24 shared_buffer_header) = 0;
};

class Ispeech_proc_nmf_mpc_dsp_port_configure: public NMF::InterfaceReference {
  public:
    Ispeech_proc_nmf_mpc_dsp_port_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_uint24 shared_buffer_header) {
      ((speech_proc_nmf_mpc_dsp_port_configureDescriptor*)reference)->setParameter(shared_buffer_header);
    }
};

#endif
