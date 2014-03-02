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

/* 'memgrabctl.api.configure' interface */
#if !defined(memgrabctl_api_configure_IDL)
#define memgrabctl_api_configure_IDL

#include <cpp.hpp>
#include <host/grab_types.idt.h>
#include <host/grabctl_types.idt.h>


class memgrabctl_api_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) = 0;
    virtual void setConfig(enum e_grabctlPortID main_port_idx, enum e_grabctlPortID second_port_idx, enum e_grabctlPortID third_port_idx, t_uint32 nbPorts, enum e_grabctlCfgIdx param_idx, t_uint8 param_data) = 0;
    virtual void setClientID(t_uint8 client_id) = 0;
};

class Imemgrabctl_api_configure: public NMF::InterfaceReference {
  public:
    Imemgrabctl_api_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) {
      ((memgrabctl_api_configureDescriptor*)reference)->setParams(GrabParams, port_idx, fifoOutsize);
    }
    void setConfig(enum e_grabctlPortID main_port_idx, enum e_grabctlPortID second_port_idx, enum e_grabctlPortID third_port_idx, t_uint32 nbPorts, enum e_grabctlCfgIdx param_idx, t_uint8 param_data) {
      ((memgrabctl_api_configureDescriptor*)reference)->setConfig(main_port_idx, second_port_idx, third_port_idx, nbPorts, param_idx, param_data);
    }
    void setClientID(t_uint8 client_id) {
      ((memgrabctl_api_configureDescriptor*)reference)->setClientID(client_id);
    }
};

#endif
