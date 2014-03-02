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

/* 'mdrc.nmfil.host.effect.configure' interface */
#if !defined(mdrc_nmfil_host_effect_configure_IDL)
#define mdrc_nmfil_host_effect_configure_IDL

#include <cpp.hpp>
#include <libeffects/libmdrc5b/include/config.idt.h>


class mdrc_nmfil_host_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(MdrcParams_t& StaticParams) = 0;
    virtual void setConfig(MdrcConfig_t& DynamicParams) = 0;
};

class Imdrc_nmfil_host_effect_configure: public NMF::InterfaceReference {
  public:
    Imdrc_nmfil_host_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(MdrcParams_t& StaticParams) {
      ((mdrc_nmfil_host_effect_configureDescriptor*)reference)->setParameter(StaticParams);
    }
    void setConfig(MdrcConfig_t& DynamicParams) {
      ((mdrc_nmfil_host_effect_configureDescriptor*)reference)->setConfig(DynamicParams);
    }
};

#endif
