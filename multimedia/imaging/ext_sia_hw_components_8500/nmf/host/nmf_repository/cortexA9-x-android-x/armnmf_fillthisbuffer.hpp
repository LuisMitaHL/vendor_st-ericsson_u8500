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

/* 'armnmf_fillthisbuffer' interface */
#if !defined(armnmf_fillthisbuffer_IDL)
#define armnmf_fillthisbuffer_IDL

#include <cpp.hpp>
#include <armnmf_buffer.idt.h>


class armnmf_fillthisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) = 0;
};

class Iarmnmf_fillthisbuffer: public NMF::InterfaceReference {
  public:
    Iarmnmf_fillthisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) {
      ((armnmf_fillthisbufferDescriptor*)reference)->fillThisBuffer(buffer);
    }
};

#endif
