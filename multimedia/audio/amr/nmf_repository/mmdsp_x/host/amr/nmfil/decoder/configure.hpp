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

/* 'amr.nmfil.decoder.configure' interface */
#if !defined(amr_nmfil_decoder_configure_IDL)
#define amr_nmfil_decoder_configure_IDL

#include <cpp.hpp>
#include <host/amr/nmfil/decoder/config.idt.h>


class amr_nmfil_decoder_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(AmrDecParams_t params) = 0;
    virtual void setConfig(AmrDecConfig_t params) = 0;
};

class Iamr_nmfil_decoder_configure: public NMF::InterfaceReference {
  public:
    Iamr_nmfil_decoder_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(AmrDecParams_t params) {
      ((amr_nmfil_decoder_configureDescriptor*)reference)->setParameter(params);
    }
    void setConfig(AmrDecConfig_t params) {
      ((amr_nmfil_decoder_configureDescriptor*)reference)->setConfig(params);
    }
};

#endif
