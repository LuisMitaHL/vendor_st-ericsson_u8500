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

/* 'armnmf_emptythisbuffer' interface */
#if !defined(armnmf_emptythisbuffer_IDL)
#define armnmf_emptythisbuffer_IDL

#include <cpp.hpp>
#include <armnmf_buffer.idt.h>


class armnmf_emptythisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) = 0;
};

class Iarmnmf_emptythisbuffer: public NMF::InterfaceReference {
  public:
    Iarmnmf_emptythisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) {
      ((armnmf_emptythisbufferDescriptor*)reference)->emptyThisBuffer(buffer);
    }
};

#endif
