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

/* 'speech_proc.nmf.common.hybrid_emptythisbuffer' interface */
#if !defined(speech_proc_nmf_common_hybrid_emptythisbuffer_IDL)
#define speech_proc_nmf_common_hybrid_emptythisbuffer_IDL

#include <cpp.hpp>


class speech_proc_nmf_common_hybrid_emptythisbufferDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void emptyThisBuffer(t_uint32 buffer) = 0;
};

class Ispeech_proc_nmf_common_hybrid_emptythisbuffer: public NMF::InterfaceReference {
  public:
    Ispeech_proc_nmf_common_hybrid_emptythisbuffer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void emptyThisBuffer(t_uint32 buffer) {
      ((speech_proc_nmf_common_hybrid_emptythisbufferDescriptor*)reference)->emptyThisBuffer(buffer);
    }
};

#endif
