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

/* 'ext_grab.api.cmd' interface */
#if !defined(ext_grab_api_cmd_IDL)
#define ext_grab_api_cmd_IDL

#include <cpp.hpp>
#include <ext_grab_types.idt.h>


class ext_grab_api_cmdDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void abort(enum e_grabPipeID pipe, t_uint16 client_id) = 0;
    virtual void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) = 0;
    virtual void subscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) = 0;
    virtual void unsubscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) = 0;
    virtual void set_frame_skip(t_uint16 frameSkip) = 0;
};

class Iext_grab_api_cmd: public NMF::InterfaceReference {
  public:
    Iext_grab_api_cmd(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void abort(enum e_grabPipeID pipe, t_uint16 client_id) {
      ((ext_grab_api_cmdDescriptor*)reference)->abort(pipe, client_id);
    }
    void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) {
      ((ext_grab_api_cmdDescriptor*)reference)->execute(pipe, params, client_id);
    }
    void subscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      ((ext_grab_api_cmdDescriptor*)reference)->subscribeEvents(info_events, error_events, debug_events, client_id);
    }
    void unsubscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      ((ext_grab_api_cmdDescriptor*)reference)->unsubscribeEvents(info_events, error_events, debug_events, client_id);
    }
    void set_frame_skip(t_uint16 frameSkip) {
      ((ext_grab_api_cmdDescriptor*)reference)->set_frame_skip(frameSkip);
    }
};

#endif
