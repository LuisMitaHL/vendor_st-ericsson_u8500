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

/* 'mdrc.nmfil.effect.configure' interface */
#if !defined(mdrc_nmfil_effect_configure_IDL)
#define mdrc_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/libeffects/libmdrc5b/include/config.idt.h>


class mdrc_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void getMemorySizes(MdrcParams_t params, t_uint32 MemorySizesDspAddress) = 0;
    virtual void setParameter(MdrcParams_t params) = 0;
    virtual void setConfig(t_uint32 DynamicParamsFifoDspAddress) = 0;
};

class Imdrc_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Imdrc_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void getMemorySizes(MdrcParams_t params, t_uint32 MemorySizesDspAddress) {
      ((mdrc_nmfil_effect_configureDescriptor*)reference)->getMemorySizes(params, MemorySizesDspAddress);
    }
    void setParameter(MdrcParams_t params) {
      ((mdrc_nmfil_effect_configureDescriptor*)reference)->setParameter(params);
    }
    void setConfig(t_uint32 DynamicParamsFifoDspAddress) {
      ((mdrc_nmfil_effect_configureDescriptor*)reference)->setConfig(DynamicParamsFifoDspAddress);
    }
};

#endif
