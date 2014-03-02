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

/* 'eventhandler' interface */
#if !defined(eventhandler_IDL)
#define eventhandler_IDL

#include <cpp.hpp>
#include <host/omxevent.idt.h>
#include <host/omxeventext.idt.h>
#include <host/omxerror.idt.h>


class eventhandlerDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) = 0;
};

class Ieventhandler: public NMF::InterfaceReference {
  public:
    Ieventhandler(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) {
      ((eventhandlerDescriptor*)reference)->eventHandler(ev, data1, data2);
    }
};

#endif
