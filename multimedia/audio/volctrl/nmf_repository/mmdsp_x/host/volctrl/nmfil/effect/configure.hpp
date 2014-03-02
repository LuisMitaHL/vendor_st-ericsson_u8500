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

/* 'volctrl.nmfil.effect.configure' interface */
#if !defined(volctrl_nmfil_effect_configure_IDL)
#define volctrl_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/volctrl/nmfil/effect/config.idt.h>


class volctrl_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(VolctrlConfig_t config) = 0;
    virtual void setRampConfig(VolctrlRampConfig_t config) = 0;
};

class Ivolctrl_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Ivolctrl_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(VolctrlConfig_t config) {
      ((volctrl_nmfil_effect_configureDescriptor*)reference)->setConfig(config);
    }
    void setRampConfig(VolctrlRampConfig_t config) {
      ((volctrl_nmfil_effect_configureDescriptor*)reference)->setRampConfig(config);
    }
};

#endif
