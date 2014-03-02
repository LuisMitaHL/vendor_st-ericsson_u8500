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

/* 'pcmdump.configure' interface */
#if !defined(pcmdump_configure_IDL)
#define pcmdump_configure_IDL

#include <cpp.hpp>
#include <host/pcmdump/configure.idt.h>


class pcmdump_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void configure_pcmprobe(PcmDumpConfigure_t sConfigureInfo) = 0;
};

class Ipcmdump_configure: public NMF::InterfaceReference {
  public:
    Ipcmdump_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void configure_pcmprobe(PcmDumpConfigure_t sConfigureInfo) {
      ((pcmdump_configureDescriptor*)reference)->configure_pcmprobe(sConfigureInfo);
    }
};

#endif
