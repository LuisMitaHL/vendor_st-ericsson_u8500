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

/* 'common.configure' interface */
#if !defined(common_configure_IDL)
#define common_configure_IDL

#include <cpp.hpp>
#include <host/common/config.idt.h>
#include <host/misc/samplesplayed.idt.h>


class common_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(DmaConfig_t config, t_uint16 target) = 0;
};

class Icommon_configure: public NMF::InterfaceReference {
  public:
    Icommon_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(DmaConfig_t config, t_uint16 target) {
      ((common_configureDescriptor*)reference)->setParameter(config, target);
    }
};

#endif
