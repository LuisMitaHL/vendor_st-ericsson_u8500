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

/* 'setmemory' interface */
#if !defined(setmemory_IDL)
#define setmemory_IDL

#include <cpp.hpp>


class setmemoryDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setMemory(void* buf, t_uint16 size) = 0;
};

class Isetmemory: public NMF::InterfaceReference {
  public:
    Isetmemory(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setMemory(void* buf, t_uint16 size) {
      ((setmemoryDescriptor*)reference)->setMemory(buf, size);
    }
};

#endif
