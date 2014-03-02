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

/* 'debug.mpc.api.msg' interface */
#if !defined(debug_mpc_api_msg_IDL)
#define debug_mpc_api_msg_IDL

#include <cpp.hpp>


class debug_mpc_api_msgDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void msg(char tab[80]) = 0;
};

class Idebug_mpc_api_msg: public NMF::InterfaceReference {
  public:
    Idebug_mpc_api_msg(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void msg(char tab[80]) {
      ((debug_mpc_api_msgDescriptor*)reference)->msg(tab);
    }
};

#endif
