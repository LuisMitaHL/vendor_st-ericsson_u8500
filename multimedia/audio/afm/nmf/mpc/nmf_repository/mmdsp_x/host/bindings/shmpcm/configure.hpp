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

/* 'bindings.shmpcm.configure' interface */
#if !defined(bindings_shmpcm_configure_IDL)
#define bindings_shmpcm_configure_IDL

#include <cpp.hpp>
#include <host/common/bindings/shmpcmconfig.idt.h>


class bindings_shmpcm_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(ShmPcmConfig_t config, void* buffer, t_uint16 output_blocksize) = 0;
};

class Ibindings_shmpcm_configure: public NMF::InterfaceReference {
  public:
    Ibindings_shmpcm_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(ShmPcmConfig_t config, void* buffer, t_uint16 output_blocksize) {
      ((bindings_shmpcm_configureDescriptor*)reference)->setParameter(config, buffer, output_blocksize);
    }
};

#endif
