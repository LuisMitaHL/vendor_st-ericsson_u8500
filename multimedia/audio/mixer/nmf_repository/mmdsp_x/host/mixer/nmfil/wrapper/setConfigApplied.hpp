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

/* 'mixer.nmfil.wrapper.setConfigApplied' interface */
#if !defined(mixer_nmfil_wrapper_setConfigApplied_IDL)
#define mixer_nmfil_wrapper_setConfigApplied_IDL

#include <cpp.hpp>


class mixer_nmfil_wrapper_setConfigAppliedDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void newConfigApplied(t_uint32 configARMAddress) = 0;
};

class Imixer_nmfil_wrapper_setConfigApplied: public NMF::InterfaceReference {
  public:
    Imixer_nmfil_wrapper_setConfigApplied(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void newConfigApplied(t_uint32 configARMAddress) {
      ((mixer_nmfil_wrapper_setConfigAppliedDescriptor*)reference)->newConfigApplied(configARMAddress);
    }
};

#endif
