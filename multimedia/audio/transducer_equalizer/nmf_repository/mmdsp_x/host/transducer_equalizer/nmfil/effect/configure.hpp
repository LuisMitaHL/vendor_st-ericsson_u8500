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

/* 'transducer_equalizer.nmfil.effect.configure' interface */
#if !defined(transducer_equalizer_nmfil_effect_configure_IDL)
#define transducer_equalizer_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/libeffects/libtransducer_equalizer/include/config.idt.h>


class transducer_equalizer_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(TransducerEqualizerParams_t StaticParams) = 0;
    virtual void setConfig(t_uint32 DynamicParamsFifoDspAddress) = 0;
};

class Itransducer_equalizer_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Itransducer_equalizer_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(TransducerEqualizerParams_t StaticParams) {
      ((transducer_equalizer_nmfil_effect_configureDescriptor*)reference)->setParameter(StaticParams);
    }
    void setConfig(t_uint32 DynamicParamsFifoDspAddress) {
      ((transducer_equalizer_nmfil_effect_configureDescriptor*)reference)->setConfig(DynamicParamsFifoDspAddress);
    }
};

#endif
