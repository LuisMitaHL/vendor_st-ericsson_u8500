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

/* 'amr.nmfil.encoder.configure' interface */
#if !defined(amr_nmfil_encoder_configure_IDL)
#define amr_nmfil_encoder_configure_IDL

#include <cpp.hpp>
#include <host/amr/nmfil/encoder/config.idt.h>


class amr_nmfil_encoder_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(AmrEncParam_t params) = 0;
    virtual void setConfig(AmrEncConfig_t config) = 0;
};

class Iamr_nmfil_encoder_configure: public NMF::InterfaceReference {
  public:
    Iamr_nmfil_encoder_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(AmrEncParam_t params) {
      ((amr_nmfil_encoder_configureDescriptor*)reference)->setParameter(params);
    }
    void setConfig(AmrEncConfig_t config) {
      ((amr_nmfil_encoder_configureDescriptor*)reference)->setConfig(config);
    }
};

#endif
