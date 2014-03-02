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

/* 'sw_3A.api.control' interface */
#if !defined(sw_3A_api_control_IDL)
#define sw_3A_api_control_IDL

#include <cpp.hpp>
#include <sw_3A_types.idt.h>


class sw_3A_api_controlDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_uint32 nIndex, void* p) = 0;
    virtual void setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats) = 0;
    virtual void setFrameRate(t_uint32 framerate_x100, t_uint32 framerate_min_x100, t_sint32 bFixedFramerate) = 0;
    virtual void open(t_uint32 peListMaxSize, t_sw3A_ISPSharedBuffer sharedBuf, t_sw3A_FlashModeSet flashModesSet) = 0;
    virtual void close(void) = 0;
    virtual void startLoop(t_sint32 bNotifyCompletion, t_sw3A_PageElementList* fwState) = 0;
    virtual void stopLoop(t_sint32 bNotifyCompletion) = 0;
    virtual void process(t_sw3A_StatEvent statEvent) = 0;
};

class Isw_3A_api_control: public NMF::InterfaceReference {
  public:
    Isw_3A_api_control(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_uint32 nIndex, void* p) {
      ((sw_3A_api_controlDescriptor*)reference)->setConfig(nIndex, p);
    }
    void setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats) {
      ((sw_3A_api_controlDescriptor*)reference)->setMode(loopState, numberOfStats);
    }
    void setFrameRate(t_uint32 framerate_x100, t_uint32 framerate_min_x100, t_sint32 bFixedFramerate) {
      ((sw_3A_api_controlDescriptor*)reference)->setFrameRate(framerate_x100, framerate_min_x100, bFixedFramerate);
    }
    void open(t_uint32 peListMaxSize, t_sw3A_ISPSharedBuffer sharedBuf, t_sw3A_FlashModeSet flashModesSet) {
      ((sw_3A_api_controlDescriptor*)reference)->open(peListMaxSize, sharedBuf, flashModesSet);
    }
    void close(void) {
      ((sw_3A_api_controlDescriptor*)reference)->close();
    }
    void startLoop(t_sint32 bNotifyCompletion, t_sw3A_PageElementList* fwState) {
      ((sw_3A_api_controlDescriptor*)reference)->startLoop(bNotifyCompletion, fwState);
    }
    void stopLoop(t_sint32 bNotifyCompletion) {
      ((sw_3A_api_controlDescriptor*)reference)->stopLoop(bNotifyCompletion);
    }
    void process(t_sw3A_StatEvent statEvent) {
      ((sw_3A_api_controlDescriptor*)reference)->process(statEvent);
    }
};

#endif
