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

/* 'comfortnoise.nmfil.effect.configure' interface */
#if !defined(comfortnoise_nmfil_effect_configure_IDL)
#define comfortnoise_nmfil_effect_configure_IDL

#include <cpp.hpp>
#include <host/comfortnoise/nmfil/effect/config.idt.h>


class comfortnoise_nmfil_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_cng_config config) = 0;
};

class Icomfortnoise_nmfil_effect_configure: public NMF::InterfaceReference {
  public:
    Icomfortnoise_nmfil_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_cng_config config) {
      ((comfortnoise_nmfil_effect_configureDescriptor*)reference)->setConfig(config);
    }
};

#endif
