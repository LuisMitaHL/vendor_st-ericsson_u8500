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

/* 'ext.api.set_debug' interface */
#if !defined(ext_api_set_debug_IDL)
#define ext_api_set_debug_IDL

#include <cpp.hpp>


class ext_api_set_debugDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setDebug(t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) = 0;
    virtual void readRegister(t_uint16 reg_adress) = 0;
};

class Iext_api_set_debug: public NMF::InterfaceReference {
  public:
    Iext_api_set_debug(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setDebug(t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) {
      ((ext_api_set_debugDescriptor*)reference)->setDebug(debug_mode, param1, param2);
    }
    void readRegister(t_uint16 reg_adress) {
      ((ext_api_set_debugDescriptor*)reference)->readRegister(reg_adress);
    }
};

#endif
