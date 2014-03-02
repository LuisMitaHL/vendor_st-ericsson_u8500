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

/* 'ext_ispctl.api.alert' interface */
#if !defined(ext_ispctl_api_alert_IDL)
#define ext_ispctl_api_alert_IDL

#include <cpp.hpp>
#include <host/ext_ispctl_types.idt.h>


class ext_ispctl_api_alertDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void info(enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp) = 0;
    virtual void error(enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp) = 0;
    virtual void debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) = 0;
};

class Iext_ispctl_api_alert: public NMF::InterfaceReference {
  public:
    Iext_ispctl_api_alert(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void info(enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp) {
      ((ext_ispctl_api_alertDescriptor*)reference)->info(info, value, timestamp);
    }
    void error(enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp) {
      ((ext_ispctl_api_alertDescriptor*)reference)->error(error_id, data, timestamp);
    }
    void debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) {
      ((ext_ispctl_api_alertDescriptor*)reference)->debug(debug_id, data1, data2, timestamp);
    }
};

#endif
