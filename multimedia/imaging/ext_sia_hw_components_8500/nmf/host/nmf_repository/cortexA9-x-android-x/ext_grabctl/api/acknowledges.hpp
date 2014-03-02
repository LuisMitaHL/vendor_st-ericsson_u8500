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

/* 'ext_grabctl.api.acknowledges' interface */
#if !defined(ext_grabctl_api_acknowledges_IDL)
#define ext_grabctl_api_acknowledges_IDL

#include <cpp.hpp>


class ext_grabctl_api_acknowledgesDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void configured(t_uint16 port_idx) = 0;
    virtual void clockwaitingforstart(t_uint16 port_idx) = 0;
    virtual void clockrunning(t_uint16 port_idx) = 0;
};

class Iext_grabctl_api_acknowledges: public NMF::InterfaceReference {
  public:
    Iext_grabctl_api_acknowledges(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void configured(t_uint16 port_idx) {
      ((ext_grabctl_api_acknowledgesDescriptor*)reference)->configured(port_idx);
    }
    void clockwaitingforstart(t_uint16 port_idx) {
      ((ext_grabctl_api_acknowledgesDescriptor*)reference)->clockwaitingforstart(port_idx);
    }
    void clockrunning(t_uint16 port_idx) {
      ((ext_grabctl_api_acknowledgesDescriptor*)reference)->clockrunning(port_idx);
    }
};

#endif
