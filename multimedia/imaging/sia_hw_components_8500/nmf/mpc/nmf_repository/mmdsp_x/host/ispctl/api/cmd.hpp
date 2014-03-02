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

/* 'ispctl.api.cmd' interface */
#if !defined(ispctl_api_cmd_IDL)
#define ispctl_api_cmd_IDL

#include <cpp.hpp>
#include <host/ispctl_types.idt.h>
#include <host/trace.idt.h>


class ispctl_api_cmdDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void initISP(t_uint8 client_id) = 0;
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) = 0;
    virtual void readPageElement(t_uint16 addr, t_uint8 client_id) = 0;
    virtual void WaitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id) = 0;
    virtual void readListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) = 0;
    virtual void writePageElement(t_uint16 addr, t_uint32 value, t_uint8 client_id) = 0;
    virtual void writeListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) = 0;
    virtual void updateGridironTable(t_uint32 addr, t_uint16 size, t_uint8 client_id) = 0;
    virtual void subscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) = 0;
    virtual void unsubscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) = 0;
    virtual void allowSleep(void) = 0;
    virtual void preventSleep(void) = 0;
};

class Iispctl_api_cmd: public NMF::InterfaceReference {
  public:
    Iispctl_api_cmd(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void initISP(t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->initISP(client_id);
    }
    void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      ((ispctl_api_cmdDescriptor*)reference)->traceInit(trace_info, id);
    }
    void readPageElement(t_uint16 addr, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->readPageElement(addr, client_id);
    }
    void WaitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->WaitExpectedPeValue(addr, value_expected, polling_frequency, timeout, client_id);
    }
    void readListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->readListPageElement(tab_pe, number_of_pe, client_id);
    }
    void writePageElement(t_uint16 addr, t_uint32 value, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->writePageElement(addr, value, client_id);
    }
    void writeListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->writeListPageElement(tab_pe, number_of_pe, client_id);
    }
    void updateGridironTable(t_uint32 addr, t_uint16 size, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->updateGridironTable(addr, size, client_id);
    }
    void subscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->subscribeEvent(info_events, error_events, debug_events, client_id);
    }
    void unsubscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      ((ispctl_api_cmdDescriptor*)reference)->unsubscribeEvent(info_events, error_events, debug_events, client_id);
    }
    void allowSleep(void) {
      ((ispctl_api_cmdDescriptor*)reference)->allowSleep();
    }
    void preventSleep(void) {
      ((ispctl_api_cmdDescriptor*)reference)->preventSleep();
    }
};

#endif
