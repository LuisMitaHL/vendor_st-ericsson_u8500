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

/* 'ext_ispctl.api.cmd' interface */
#if !defined(ext_ispctl_api_cmd_IDL)
#define ext_ispctl_api_cmd_IDL

#include <cpp.hpp>
#include <host/ext_ispctl_types.idt.h>


class ext_ispctl_api_cmdDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void readPageElement(t_uint16 addr, t_uint8 client_id) = 0;
    virtual void subscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) = 0;
    virtual void unsubscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) = 0;
};

class Iext_ispctl_api_cmd: public NMF::InterfaceReference {
  public:
    Iext_ispctl_api_cmd(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void readPageElement(t_uint16 addr, t_uint8 client_id) {
      ((ext_ispctl_api_cmdDescriptor*)reference)->readPageElement(addr, client_id);
    }
    void subscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      ((ext_ispctl_api_cmdDescriptor*)reference)->subscribeEvent(info_events, error_events, debug_events, client_id);
    }
    void unsubscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      ((ext_ispctl_api_cmdDescriptor*)reference)->unsubscribeEvent(info_events, error_events, debug_events, client_id);
    }
};

#endif
