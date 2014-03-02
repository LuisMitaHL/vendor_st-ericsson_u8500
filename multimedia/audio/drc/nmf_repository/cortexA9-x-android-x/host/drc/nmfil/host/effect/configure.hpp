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

/* 'drc.nmfil.host.effect.configure' interface */
#if !defined(drc_nmfil_host_effect_configure_IDL)
#define drc_nmfil_host_effect_configure_IDL

#include <cpp.hpp>
#include <drc/nmfil/host/effect/config.idt.h>


class drc_nmfil_host_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_drc_config drc_config) = 0;
};

class Idrc_nmfil_host_effect_configure: public NMF::InterfaceReference {
  public:
    Idrc_nmfil_host_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_drc_config drc_config) {
      ((drc_nmfil_host_effect_configureDescriptor*)reference)->setConfig(drc_config);
    }
};

#endif
