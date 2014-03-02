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

/* 'ispctl.api.cfg' interface */
#if !defined(ispctl_api_cfg_IDL)
#define ispctl_api_cfg_IDL

#include <cpp.hpp>
#include <host/ispctl_types.idt.h>


class ispctl_api_cfgDescriptor: public NMF::InterfaceDescriptor {
  public:
};

class Iispctl_api_cfg: public NMF::InterfaceReference {
  public:
    Iispctl_api_cfg(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

};

#endif
