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

/* 'fsminit' interface */
#if !defined(fsminit_IDL)
#define fsminit_IDL

#include <cpp.hpp>
#include <host/fsmInit.idt.h>


class fsminitDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void fsmInit(fsmInit_t init) = 0;
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) = 0;
};

class Ifsminit: public NMF::InterfaceReference {
  public:
    Ifsminit(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void fsmInit(fsmInit_t init) {
      ((fsminitDescriptor*)reference)->fsmInit(init);
    }
    void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) {
      ((fsminitDescriptor*)reference)->setTunnelStatus(portIdx, isTunneled);
    }
};

#endif
