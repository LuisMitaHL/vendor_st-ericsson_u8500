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

/* 'malloc.setheap' interface */
#if !defined(malloc_setheap_IDL)
#define malloc_setheap_IDL

#include <cpp.hpp>
#include <host/memorybank.idt.h>


class malloc_setheapDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setHeap(t_memory_bank memory_bank, void* buf, t_uint32 size) = 0;
};

class Imalloc_setheap: public NMF::InterfaceReference {
  public:
    Imalloc_setheap(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setHeap(t_memory_bank memory_bank, void* buf, t_uint32 size) {
      ((malloc_setheapDescriptor*)reference)->setHeap(memory_bank, buf, size);
    }
};

#endif
