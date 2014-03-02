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

/* 'pcmprocessings.mips.configure' interface */
#if !defined(pcmprocessings_mips_configure_IDL)
#define pcmprocessings_mips_configure_IDL

#include <cpp.hpp>
#include <host/pcmprocessings/mips/mips.idt.h>


class pcmprocessings_mips_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(Mips_configure_t Mips_configure) = 0;
};

class Ipcmprocessings_mips_configure: public NMF::InterfaceReference {
  public:
    Ipcmprocessings_mips_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(Mips_configure_t Mips_configure) {
      ((pcmprocessings_mips_configureDescriptor*)reference)->setConfig(Mips_configure);
    }
};

#endif
