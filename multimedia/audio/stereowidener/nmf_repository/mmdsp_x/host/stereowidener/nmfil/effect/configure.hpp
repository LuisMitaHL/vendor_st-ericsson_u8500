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

/* 'stereowidener.nmfil.effect.configure' interface */
#if !defined(stereowidener_nmfil_effect_configure_IDL)
#define stereowidener_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/stereowidener/nmfil/effect/config.idt.h>


class stereowidener_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(StereowidenerParams_t params) = 0;
    virtual void setConfig(StereowidenerConfig_t config) = 0;
};

class Istereowidener_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Istereowidener_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(StereowidenerParams_t params) {
      ((stereowidener_nmfil_effect_configureDescriptor*)reference)->setParameter(params);
    }
    void setConfig(StereowidenerConfig_t config) {
      ((stereowidener_nmfil_effect_configureDescriptor*)reference)->setConfig(config);
    }
};

#endif
