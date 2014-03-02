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

/* 'debug.mpc.api.printf' interface */
#if !defined(debug_mpc_api_printf_IDL)
#define debug_mpc_api_printf_IDL

#include <cpp.hpp>
#include <stdarg.h>


class debug_mpc_api_printfDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void printf(char* format, va_list ap) = 0;
};

class Idebug_mpc_api_printf: public NMF::InterfaceReference {
  public:
    Idebug_mpc_api_printf(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void printf(char* format, va_list ap) {
      ((debug_mpc_api_printfDescriptor*)reference)->printf(format, ap);
    }
};

#endif
