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

/* 'virtual_surround.nmfil.host.effect.configure' interface */
#if !defined(virtual_surround_nmfil_host_effect_configure_IDL)
#define virtual_surround_nmfil_host_effect_configure_IDL

#include <cpp.hpp>
#include <virtual_surround/nmfil/host/effect/config.idt.h>


class virtual_surround_nmfil_host_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_virtual_surround_config vs_config) = 0;
};

class Ivirtual_surround_nmfil_host_effect_configure: public NMF::InterfaceReference {
  public:
    Ivirtual_surround_nmfil_host_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_virtual_surround_config vs_config) {
      ((virtual_surround_nmfil_host_effect_configureDescriptor*)reference)->setConfig(vs_config);
    }
};

#endif
