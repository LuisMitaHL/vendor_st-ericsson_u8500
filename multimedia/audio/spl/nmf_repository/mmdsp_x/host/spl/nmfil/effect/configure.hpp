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

/* 'spl.nmfil.effect.configure' interface */
#if !defined(spl_nmfil_effect_configure_IDL)
#define spl_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/spl/nmfil/effect/config.idt.h>


class spl_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(SPLimiterParams_t StaticParams) = 0;
    virtual void setConfig(SPLimiterConfig_t DynamicParams) = 0;
};

class Ispl_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Ispl_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(SPLimiterParams_t StaticParams) {
      ((spl_nmfil_effect_configureDescriptor*)reference)->setParameter(StaticParams);
    }
    void setConfig(SPLimiterConfig_t DynamicParams) {
      ((spl_nmfil_effect_configureDescriptor*)reference)->setConfig(DynamicParams);
    }
};

#endif
