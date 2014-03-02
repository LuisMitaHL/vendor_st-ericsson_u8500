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

/* 'grabctl.api.configure' interface */
#if !defined(grabctl_api_configure_IDL)
#define grabctl_api_configure_IDL

#include <cpp.hpp>
#include <host/grab_types.idt.h>
#include <host/grabctl_types.idt.h>


class grabctl_api_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) = 0;
    virtual void setConfig(enum e_grabctlPortID port_idx, enum e_grabctlCfgIdx param_idx, enum e_capturing capture_mode, t_uint8 param_data) = 0;
    virtual void setConfigBMS(enum e_grabctlPortID port_idx, t_uint16 bufferAddressesH[6], t_uint16 bufferAddressesL[6], t_uint16 bufferCount, t_uint16 xsize, t_uint16 ysize, t_uint16 linelen, t_uint16 pixel_order) = 0;
    virtual void setClientID(t_uint8 client_id) = 0;
    virtual void setPort2PipeMapping(enum e_grabPipeID pipe4port0, enum e_grabPipeID pipe4port1, enum e_grabPipeID pipe4port2) = 0;
    virtual void setClockWaitingForStart(void) = 0;
    virtual void setClockRunning(void) = 0;
    virtual void setHiddenBMS(t_uint16 hidden_bms, t_uint16 nFramesBefore) = 0;
    virtual void endAllCaptures(void) = 0;
};

class Igrabctl_api_configure: public NMF::InterfaceReference {
  public:
    Igrabctl_api_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) {
      ((grabctl_api_configureDescriptor*)reference)->setParams(GrabParams, port_idx, fifoOutsize);
    }
    void setConfig(enum e_grabctlPortID port_idx, enum e_grabctlCfgIdx param_idx, enum e_capturing capture_mode, t_uint8 param_data) {
      ((grabctl_api_configureDescriptor*)reference)->setConfig(port_idx, param_idx, capture_mode, param_data);
    }
    void setConfigBMS(enum e_grabctlPortID port_idx, t_uint16 bufferAddressesH[6], t_uint16 bufferAddressesL[6], t_uint16 bufferCount, t_uint16 xsize, t_uint16 ysize, t_uint16 linelen, t_uint16 pixel_order) {
      ((grabctl_api_configureDescriptor*)reference)->setConfigBMS(port_idx, bufferAddressesH, bufferAddressesL, bufferCount, xsize, ysize, linelen, pixel_order);
    }
    void setClientID(t_uint8 client_id) {
      ((grabctl_api_configureDescriptor*)reference)->setClientID(client_id);
    }
    void setPort2PipeMapping(enum e_grabPipeID pipe4port0, enum e_grabPipeID pipe4port1, enum e_grabPipeID pipe4port2) {
      ((grabctl_api_configureDescriptor*)reference)->setPort2PipeMapping(pipe4port0, pipe4port1, pipe4port2);
    }
    void setClockWaitingForStart(void) {
      ((grabctl_api_configureDescriptor*)reference)->setClockWaitingForStart();
    }
    void setClockRunning(void) {
      ((grabctl_api_configureDescriptor*)reference)->setClockRunning();
    }
    void setHiddenBMS(t_uint16 hidden_bms, t_uint16 nFramesBefore) {
      ((grabctl_api_configureDescriptor*)reference)->setHiddenBMS(hidden_bms, nFramesBefore);
    }
    void endAllCaptures(void) {
      ((grabctl_api_configureDescriptor*)reference)->endAllCaptures();
    }
};

#endif
