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

/* 'sw_3A.api.sync' interface */
#if !defined(sw_3A_api_sync_IDL)
#define sw_3A_api_sync_IDL

#include <cpp.hpp>
#include <sw_3A_types.idt.h>


class sw_3A_api_syncDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual t_sint32 isConfigAvailable(t_uint32 pIndex, void* pp) = 0;
    virtual t_sint32 getConfig(t_uint32 nIndex, void* p) = 0;
    virtual t_sint32 getMakenotesAllocSize(void) = 0;
    virtual t_sint32 getExtradataAllocSize(void) = 0;
    virtual t_sint16 getTorchPower(void) = 0;
};

class Isw_3A_api_sync: public NMF::InterfaceReference {
  public:
    Isw_3A_api_sync(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    t_sint32 isConfigAvailable(t_uint32 pIndex, void* pp) {
      return ((sw_3A_api_syncDescriptor*)reference)->isConfigAvailable(pIndex, pp);
    }
    t_sint32 getConfig(t_uint32 nIndex, void* p) {
      return ((sw_3A_api_syncDescriptor*)reference)->getConfig(nIndex, p);
    }
    t_sint32 getMakenotesAllocSize(void) {
      return ((sw_3A_api_syncDescriptor*)reference)->getMakenotesAllocSize();
    }
    t_sint32 getExtradataAllocSize(void) {
      return ((sw_3A_api_syncDescriptor*)reference)->getExtradataAllocSize();
    }
    t_sint16 getTorchPower(void) {
      return ((sw_3A_api_syncDescriptor*)reference)->getTorchPower();
    }
};

#endif
