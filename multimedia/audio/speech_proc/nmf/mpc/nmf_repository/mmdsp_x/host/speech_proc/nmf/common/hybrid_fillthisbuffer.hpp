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

/* 'speech_proc.nmf.common.hybrid_fillthisbuffer' interface */
#if !defined(speech_proc_nmf_common_hybrid_fillthisbuffer_IDL)
#define speech_proc_nmf_common_hybrid_fillthisbuffer_IDL

#include <cpp.hpp>


class speech_proc_nmf_common_hybrid_fillthisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void fillThisBuffer(t_uint32 buffer) = 0;
};

class Ispeech_proc_nmf_common_hybrid_fillthisbuffer: public NMF::InterfaceReference {
  public:
    Ispeech_proc_nmf_common_hybrid_fillthisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void fillThisBuffer(t_uint32 buffer) {
      ((speech_proc_nmf_common_hybrid_fillthisbufferDescriptor*)reference)->fillThisBuffer(buffer);
    }
};

#endif
