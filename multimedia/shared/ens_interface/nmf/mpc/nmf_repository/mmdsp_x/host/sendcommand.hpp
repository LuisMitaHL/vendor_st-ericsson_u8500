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

/* 'sendcommand' interface */
#if !defined(sendcommand_IDL)
#define sendcommand_IDL

#include <cpp.hpp>
#include <host/omxcommand.idt.h>
#include <host/omxstate.idt.h>


class sendcommandDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) = 0;
};

class Isendcommand: public NMF::InterfaceReference {
  public:
    Isendcommand(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
      ((sendcommandDescriptor*)reference)->sendCommand(cmd, param);
    }
};

#endif
