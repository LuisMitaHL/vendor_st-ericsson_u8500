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

/* 'shared_emptythisbuffer' interface */
#if !defined(shared_emptythisbuffer_IDL)
#define shared_emptythisbuffer_IDL

#include <cpp.hpp>


class shared_emptythisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void emptyThisBuffer(t_uint32 buffer) = 0;
};

class Ishared_emptythisbuffer: public NMF::InterfaceReference {
  public:
    Ishared_emptythisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void emptyThisBuffer(t_uint32 buffer) {
      ((shared_emptythisbufferDescriptor*)reference)->emptyThisBuffer(buffer);
    }
};

#endif
