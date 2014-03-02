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

/* 'dummy' interface */
#if !defined(dummy_IDL)
#define dummy_IDL

#include <cpp.hpp>
#include <host/omxclock.idt.h>


class dummyDescriptor: public NMF::InterfaceDescriptor {
  public:
};

class Idummy: public NMF::InterfaceReference {
  public:
    Idummy(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

};

#endif
