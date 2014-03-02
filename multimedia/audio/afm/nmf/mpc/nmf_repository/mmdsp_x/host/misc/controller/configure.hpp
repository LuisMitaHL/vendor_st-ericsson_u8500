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

/* 'misc.controller.configure' interface */
#if !defined(misc_controller_configure_IDL)
#define misc_controller_configure_IDL

#include <cpp.hpp>


class misc_controller_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_uint16 portDirections) = 0;
};

class Imisc_controller_configure: public NMF::InterfaceReference {
  public:
    Imisc_controller_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_uint16 portDirections) {
      ((misc_controller_configureDescriptor*)reference)->setParameter(portDirections);
    }
};

#endif
