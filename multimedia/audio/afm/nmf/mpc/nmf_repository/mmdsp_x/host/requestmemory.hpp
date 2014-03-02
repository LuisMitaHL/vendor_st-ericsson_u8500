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

/* 'requestmemory' interface */
#if !defined(requestmemory_IDL)
#define requestmemory_IDL

#include <cpp.hpp>


class requestmemoryDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void requestMemory(t_uint16 size) = 0;
};

class Irequestmemory: public NMF::InterfaceReference {
  public:
    Irequestmemory(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void requestMemory(t_uint16 size) {
      ((requestmemoryDescriptor*)reference)->requestMemory(size);
    }
};

#endif
