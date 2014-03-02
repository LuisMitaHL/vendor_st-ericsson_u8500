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

/* 'grab.api.cmd' interface */
#if !defined(grab_api_cmd_IDL)
#define grab_api_cmd_IDL

#include <cpp.hpp>
#include <host/grab_types.idt.h>
#include <host/trace.idt.h>


class grab_api_cmdDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) = 0;
    virtual void abort(enum e_grabPipeID pipe, t_uint16 client_id) = 0;
    virtual void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) = 0;
    virtual void subscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) = 0;
    virtual void unsubscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) = 0;
    virtual void Overscan_Supported(t_uint16 Adaptive_Overscan_legacy) = 0;
    virtual void disableDspFineLowPower(void) = 0;
    virtual void setBMSAdd(t_uint16 bmsHadd, t_uint16 bmsLadd) = 0;
    virtual void setHiddenBMS(t_uint16 hiddenBMS) = 0;
};

class Igrab_api_cmd: public NMF::InterfaceReference {
  public:
    Igrab_api_cmd(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      ((grab_api_cmdDescriptor*)reference)->traceInit(trace_info, id);
    }
    void abort(enum e_grabPipeID pipe, t_uint16 client_id) {
      ((grab_api_cmdDescriptor*)reference)->abort(pipe, client_id);
    }
    void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) {
      ((grab_api_cmdDescriptor*)reference)->execute(pipe, params, client_id);
    }
    void subscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      ((grab_api_cmdDescriptor*)reference)->subscribeEvents(info_events, error_events, debug_events, client_id);
    }
    void unsubscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      ((grab_api_cmdDescriptor*)reference)->unsubscribeEvents(info_events, error_events, debug_events, client_id);
    }
    void Overscan_Supported(t_uint16 Adaptive_Overscan_legacy) {
      ((grab_api_cmdDescriptor*)reference)->Overscan_Supported(Adaptive_Overscan_legacy);
    }
    void disableDspFineLowPower(void) {
      ((grab_api_cmdDescriptor*)reference)->disableDspFineLowPower();
    }
    void setBMSAdd(t_uint16 bmsHadd, t_uint16 bmsLadd) {
      ((grab_api_cmdDescriptor*)reference)->setBMSAdd(bmsHadd, bmsLadd);
    }
    void setHiddenBMS(t_uint16 hiddenBMS) {
      ((grab_api_cmdDescriptor*)reference)->setHiddenBMS(hiddenBMS);
    }
};

#endif
