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

/* 'shared_fillthisbuffer' interface */
#if !defined(shared_fillthisbuffer_IDL)
#define shared_fillthisbuffer_IDL

#include <cpp.hpp>


class shared_fillthisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void fillThisBuffer(t_uint32 buffer) = 0;
};

class Ishared_fillthisbuffer: public NMF::InterfaceReference {
  public:
    Ishared_fillthisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void fillThisBuffer(t_uint32 buffer) {
      ((shared_fillthisbufferDescriptor*)reference)->fillThisBuffer(buffer);
    }
};

#endif
