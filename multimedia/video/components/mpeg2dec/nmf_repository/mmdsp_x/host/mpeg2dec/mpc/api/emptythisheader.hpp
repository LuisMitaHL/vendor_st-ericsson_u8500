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

/* 'mpeg2dec.mpc.api.emptythisheader' interface */
#if !defined(mpeg2dec_mpc_api_emptythisheader_IDL)
#define mpeg2dec_mpc_api_emptythisheader_IDL

#include <cpp.hpp>
#include <host/buffer.idt.h>


class mpeg2dec_mpc_api_emptythisheaderDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void emptyThisHeader(Buffer_p buffer) = 0;
};

class Impeg2dec_mpc_api_emptythisheader: public NMF::InterfaceReference {
  public:
    Impeg2dec_mpc_api_emptythisheader(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void emptyThisHeader(Buffer_p buffer) {
      ((mpeg2dec_mpc_api_emptythisheaderDescriptor*)reference)->emptyThisHeader(buffer);
    }
};

#endif
