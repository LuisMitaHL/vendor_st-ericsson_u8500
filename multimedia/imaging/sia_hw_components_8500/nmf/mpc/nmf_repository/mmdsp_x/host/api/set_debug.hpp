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

/* 'api.set_debug' interface */
#if !defined(api_set_debug_IDL)
#define api_set_debug_IDL

#include <cpp.hpp>


class api_set_debugDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setDebug(t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) = 0;
    virtual void readRegister(t_uint16 reg_adress) = 0;
};

class Iapi_set_debug: public NMF::InterfaceReference {
  public:
    Iapi_set_debug(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setDebug(t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) {
      ((api_set_debugDescriptor*)reference)->setDebug(debug_mode, param1, param2);
    }
    void readRegister(t_uint16 reg_adress) {
      ((api_set_debugDescriptor*)reference)->readRegister(reg_adress);
    }
};

#endif
