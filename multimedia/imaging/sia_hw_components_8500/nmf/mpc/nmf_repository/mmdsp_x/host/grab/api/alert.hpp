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

/* 'grab.api.alert' interface */
#if !defined(grab_api_alert_IDL)
#define grab_api_alert_IDL

#include <cpp.hpp>
#include <host/grab_types.idt.h>


class grab_api_alertDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp) = 0;
    virtual void error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp) = 0;
    virtual void debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) = 0;
};

class Igrab_api_alert: public NMF::InterfaceReference {
  public:
    Igrab_api_alert(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp) {
      ((grab_api_alertDescriptor*)reference)->info(info_id, buffer_id, frame_id, timestamp);
    }
    void error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp) {
      ((grab_api_alertDescriptor*)reference)->error(error_id, data, buffer_id, timestamp);
    }
    void debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp) {
      ((grab_api_alertDescriptor*)reference)->debug(debug_id, buffer_id, data1, data2, timestamp);
    }
};

#endif
