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

/* 'interruptdfc' interface */
#if !defined(interruptdfc_IDL)
#define interruptdfc_IDL

#include <cpp.hpp>


class interruptdfcDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void interruptDFC(void) = 0;
};

class Iinterruptdfc: public NMF::InterfaceReference {
  public:
    Iinterruptdfc(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void interruptDFC(void) {
      ((interruptdfcDescriptor*)reference)->interruptDFC();
    }
};

#endif
