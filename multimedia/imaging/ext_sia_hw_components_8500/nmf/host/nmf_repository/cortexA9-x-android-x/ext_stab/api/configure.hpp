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

/* 'ext_stab.api.configure' interface */
#if !defined(ext_stab_api_configure_IDL)
#define ext_stab_api_configure_IDL

#include <cpp.hpp>
#include <ext_grabctl_types.idt.h>
#include <ext_grab_types.idt.h>


class ext_stab_api_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void informBuffers(t_uint32 hCurvePhysical, t_sint32* hCurveLogical, t_uint32 vCurvePhysical, t_sint32* vCurveLogical, t_uint32 x_size, t_uint32 y_size) = 0;
    virtual void activateStab(t_bool enable) = 0;
    virtual void setConfig(enum e_grabctlPortID port_idx, t_uint8 capture_started) = 0;
    virtual void setParams(struct s_grabParams GrabParams, t_uint32 input_port) = 0;
};

class Iext_stab_api_configure: public NMF::InterfaceReference {
  public:
    Iext_stab_api_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void informBuffers(t_uint32 hCurvePhysical, t_sint32* hCurveLogical, t_uint32 vCurvePhysical, t_sint32* vCurveLogical, t_uint32 x_size, t_uint32 y_size) {
      ((ext_stab_api_configureDescriptor*)reference)->informBuffers(hCurvePhysical, hCurveLogical, vCurvePhysical, vCurveLogical, x_size, y_size);
    }
    void activateStab(t_bool enable) {
      ((ext_stab_api_configureDescriptor*)reference)->activateStab(enable);
    }
    void setConfig(enum e_grabctlPortID port_idx, t_uint8 capture_started) {
      ((ext_stab_api_configureDescriptor*)reference)->setConfig(port_idx, capture_started);
    }
    void setParams(struct s_grabParams GrabParams, t_uint32 input_port) {
      ((ext_stab_api_configureDescriptor*)reference)->setParams(GrabParams, input_port);
    }
};

#endif
