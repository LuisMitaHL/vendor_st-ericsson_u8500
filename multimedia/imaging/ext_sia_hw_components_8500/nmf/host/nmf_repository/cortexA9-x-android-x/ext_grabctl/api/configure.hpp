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

/* 'ext_grabctl.api.configure' interface */
#if !defined(ext_grabctl_api_configure_IDL)
#define ext_grabctl_api_configure_IDL

#include <cpp.hpp>
#include <ext_grab_types.idt.h>
#include <ext_grabctl_types.idt.h>


class ext_grabctl_api_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) = 0;
    virtual void setConfig(enum e_grabctlPortID port_idx, enum e_grabctlCfgIdx param_idx, t_uint8 param_data) = 0;
    virtual void setClientID(t_uint8 client_id) = 0;
    virtual void setPort2PipeMapping(enum e_grabPipeID pipe4port0, enum e_grabPipeID pipe4port1, enum e_grabPipeID pipe4port2) = 0;
    virtual void setClockWaitingForStart(void) = 0;
    virtual void setClockRunning(void) = 0;
};

class Iext_grabctl_api_configure: public NMF::InterfaceReference {
  public:
    Iext_grabctl_api_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setParams(GrabParams, port_idx, fifoOutsize);
    }
    void setConfig(enum e_grabctlPortID port_idx, enum e_grabctlCfgIdx param_idx, t_uint8 param_data) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setConfig(port_idx, param_idx, param_data);
    }
    void setClientID(t_uint8 client_id) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setClientID(client_id);
    }
    void setPort2PipeMapping(enum e_grabPipeID pipe4port0, enum e_grabPipeID pipe4port1, enum e_grabPipeID pipe4port2) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setPort2PipeMapping(pipe4port0, pipe4port1, pipe4port2);
    }
    void setClockWaitingForStart(void) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setClockWaitingForStart();
    }
    void setClockRunning(void) {
      ((ext_grabctl_api_configureDescriptor*)reference)->setClockRunning();
    }
};

#endif
