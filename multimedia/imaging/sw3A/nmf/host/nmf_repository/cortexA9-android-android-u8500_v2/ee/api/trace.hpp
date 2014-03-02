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

/* 'ee.api.trace' interface */
#if !defined(ee_api_trace_IDL)
#define ee_api_trace_IDL

#include <cpp.hpp>
#include <ee/api/panic.idt.h>
#include <inc/nmf-tracedescription.h>


class ee_api_traceDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void nmfTraceInit(void) = 0;
    virtual void nmfTraceDone(void) = 0;
    virtual void nmfTraceSetMode(t_bool onoff) = 0;
    virtual void nmfRegisterComposite(void* _this, void* traceDumper) = 0;
    virtual void nmfUnregisterComposite(void* _this) = 0;
    virtual void nmfTraceReset(void) = 0;
    virtual void nmfTraceComponent(t_nmfTraceComponentCommandDescription command, void* _this, const char* _name, const char* _templateName) = 0;
    virtual void nmfTraceBind(t_nmfTraceBindCommandDescription command, void* _thisfrom, const char* _namefrom, void* _thisto, const char* _nameto) = 0;
    virtual void nmfTraceInterfaceMethod(void* _this, const char* _name, unsigned int _itfmethidx) = 0;
    virtual void nmfTraceActivity(t_nmfTraceActivityCommandDescription command, void* _this, unsigned int _itfmethidx) = 0;
    virtual void nmfTracePanic(t_panic_reason reason, void* faultingComponent, t_uint32 info1, t_uint32 info2) = 0;
};

class Iee_api_trace: public NMF::InterfaceReference {
  public:
    Iee_api_trace(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void nmfTraceInit(void) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceInit();
    }
    void nmfTraceDone(void) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceDone();
    }
    void nmfTraceSetMode(t_bool onoff) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceSetMode(onoff);
    }
    void nmfRegisterComposite(void* _this, void* traceDumper) {
      ((ee_api_traceDescriptor*)reference)->nmfRegisterComposite(_this, traceDumper);
    }
    void nmfUnregisterComposite(void* _this) {
      ((ee_api_traceDescriptor*)reference)->nmfUnregisterComposite(_this);
    }
    void nmfTraceReset(void) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceReset();
    }
    void nmfTraceComponent(t_nmfTraceComponentCommandDescription command, void* _this, const char* _name, const char* _templateName) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceComponent(command, _this, _name, _templateName);
    }
    void nmfTraceBind(t_nmfTraceBindCommandDescription command, void* _thisfrom, const char* _namefrom, void* _thisto, const char* _nameto) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceBind(command, _thisfrom, _namefrom, _thisto, _nameto);
    }
    void nmfTraceInterfaceMethod(void* _this, const char* _name, unsigned int _itfmethidx) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceInterfaceMethod(_this, _name, _itfmethidx);
    }
    void nmfTraceActivity(t_nmfTraceActivityCommandDescription command, void* _this, unsigned int _itfmethidx) {
      ((ee_api_traceDescriptor*)reference)->nmfTraceActivity(command, _this, _itfmethidx);
    }
    void nmfTracePanic(t_panic_reason reason, void* faultingComponent, t_uint32 info1, t_uint32 info2) {
      ((ee_api_traceDescriptor*)reference)->nmfTracePanic(reason, faultingComponent, info1, info2);
    }
};

#endif
