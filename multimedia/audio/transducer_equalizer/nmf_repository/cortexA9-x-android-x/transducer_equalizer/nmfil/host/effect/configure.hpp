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

/* 'transducer_equalizer.nmfil.host.effect.configure' interface */
#if !defined(transducer_equalizer_nmfil_host_effect_configure_IDL)
#define transducer_equalizer_nmfil_host_effect_configure_IDL

#include <cpp.hpp>
#include <libeffects/libtransducer_equalizer/include/config.idt.h>


class transducer_equalizer_nmfil_host_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(TransducerEqualizerParams_t& StaticParams, t_uint16 instance_uint16) = 0;
    virtual void setConfig(TransducerEqualizerConfig_t& DynamicParams) = 0;
};

class Itransducer_equalizer_nmfil_host_effect_configure: public NMF::InterfaceReference {
  public:
    Itransducer_equalizer_nmfil_host_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(TransducerEqualizerParams_t& StaticParams, t_uint16 instance_uint16) {
      ((transducer_equalizer_nmfil_host_effect_configureDescriptor*)reference)->setParameter(StaticParams, instance_uint16);
    }
    void setConfig(TransducerEqualizerConfig_t& DynamicParams) {
      ((transducer_equalizer_nmfil_host_effect_configureDescriptor*)reference)->setConfig(DynamicParams);
    }
};

#endif
