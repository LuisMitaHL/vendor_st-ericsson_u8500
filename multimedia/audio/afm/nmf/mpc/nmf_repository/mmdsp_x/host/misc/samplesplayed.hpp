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

/* 'misc.samplesplayed' interface */
#if !defined(misc_samplesplayed_IDL)
#define misc_samplesplayed_IDL

#include <cpp.hpp>
#include <host/misc/samplesplayed.idt.h>


class misc_samplesplayedDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void SetSamplesPlayedAddr(void* Addr) = 0;
};

class Imisc_samplesplayed: public NMF::InterfaceReference {
  public:
    Imisc_samplesplayed(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void SetSamplesPlayedAddr(void* Addr) {
      ((misc_samplesplayedDescriptor*)reference)->SetSamplesPlayedAddr(Addr);
    }
};

#endif
