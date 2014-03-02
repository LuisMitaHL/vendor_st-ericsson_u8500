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

/* 'bindings.shm.configure' interface */
#if !defined(bindings_shm_configure_IDL)
#define bindings_shm_configure_IDL

#include <cpp.hpp>
#include <host/common/bindings/shmconfig.idt.h>


class bindings_shm_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(ShmConfig_t config) = 0;
};

class Ibindings_shm_configure: public NMF::InterfaceReference {
  public:
    Ibindings_shm_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(ShmConfig_t config) {
      ((bindings_shm_configureDescriptor*)reference)->setParameter(config);
    }
};

#endif
