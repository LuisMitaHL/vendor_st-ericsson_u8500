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

/* 'decoders.streamed.configure' interface */
#if !defined(decoders_streamed_configure_IDL)
#define decoders_streamed_configure_IDL

#include <cpp.hpp>


class decoders_streamed_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(void* buffer_in, t_uint16 size_in, void* buffer_out, t_uint16 size_out, t_uint16 payLoad) = 0;
};

class Idecoders_streamed_configure: public NMF::InterfaceReference {
  public:
    Idecoders_streamed_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(void* buffer_in, t_uint16 size_in, void* buffer_out, t_uint16 size_out, t_uint16 payLoad) {
      ((decoders_streamed_configureDescriptor*)reference)->setParameter(buffer_in, size_in, buffer_out, size_out, payLoad);
    }
};

#endif
