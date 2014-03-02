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

/* 'ext_grabctl.api.error' interface */
#if !defined(ext_grabctl_api_error_IDL)
#define ext_grabctl_api_error_IDL

#include <cpp.hpp>
#include <ext_grab_types.idt.h>


class ext_grabctl_api_errorDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) = 0;
};

class Iext_grabctl_api_error: public NMF::InterfaceReference {
  public:
    Iext_grabctl_api_error(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) {
      ((ext_grabctl_api_errorDescriptor*)reference)->error(error_id, data, pipe_id);
    }
};

#endif
