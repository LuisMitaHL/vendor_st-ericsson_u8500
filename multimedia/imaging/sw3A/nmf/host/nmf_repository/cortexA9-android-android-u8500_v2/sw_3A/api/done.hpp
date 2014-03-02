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

/* 'sw_3A.api.done' interface */
#if !defined(sw_3A_api_done_IDL)
#define sw_3A_api_done_IDL

#include <cpp.hpp>
#include <sw_3A_types.idt.h>


class sw_3A_api_doneDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void done(t_sw3A_PageElementList peList, t_sw3A_LoopState e_State, t_sw3A_metadata* p_metadata, t_sw3A_FlashDirective* p_flashDirective, t_sw3A_StillSynchro* p_stillSynchro) = 0;
    virtual void info(t_sw3A_Msg msg, t_sw3A_MsgData msgData) = 0;
};

class Isw_3A_api_done: public NMF::InterfaceReference {
  public:
    Isw_3A_api_done(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void done(t_sw3A_PageElementList peList, t_sw3A_LoopState e_State, t_sw3A_metadata* p_metadata, t_sw3A_FlashDirective* p_flashDirective, t_sw3A_StillSynchro* p_stillSynchro) {
      ((sw_3A_api_doneDescriptor*)reference)->done(peList, e_State, p_metadata, p_flashDirective, p_stillSynchro);
    }
    void info(t_sw3A_Msg msg, t_sw3A_MsgData msgData) {
      ((sw_3A_api_doneDescriptor*)reference)->info(msg, msgData);
    }
};

#endif
