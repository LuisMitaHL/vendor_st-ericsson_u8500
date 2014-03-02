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

/* 'ext_grab.api.alert' interface */
#if !defined(ext_grab_api_alert_IDL)
#define ext_grab_api_alert_IDL

#include <cpp.hpp>
#include <ext_grab_types.idt.h>


class ext_grab_api_alertDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp) = 0;
    virtual void error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp) = 0;
    virtual void debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) = 0;
};

class Iext_grab_api_alert: public NMF::InterfaceReference {
  public:
    Iext_grab_api_alert(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp) {
      ((ext_grab_api_alertDescriptor*)reference)->info(info_id, buffer_id, timestamp);
    }
    void error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp) {
      ((ext_grab_api_alertDescriptor*)reference)->error(error_id, data, buffer_id, timestamp);
    }
    void debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) {
      ((ext_grab_api_alertDescriptor*)reference)->debug(debug_id, buffer_id, data1, data2, timestamp);
    }
};

#endif
