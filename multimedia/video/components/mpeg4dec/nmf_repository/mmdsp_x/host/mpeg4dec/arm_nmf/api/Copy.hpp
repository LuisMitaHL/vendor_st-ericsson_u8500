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

/* 'mpeg4dec.arm_nmf.api.Copy' interface */
#if !defined(mpeg4dec_arm_nmf_api_Copy_IDL)
#define mpeg4dec_arm_nmf_api_Copy_IDL

#include <cpp.hpp>
#include <host/vfm_common.idt.h>


class mpeg4dec_arm_nmf_api_CopyDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void InternalCopy(t_uint16 InBuffCnt, t_uint32 offset) = 0;
};

class Impeg4dec_arm_nmf_api_Copy: public NMF::InterfaceReference {
  public:
    Impeg4dec_arm_nmf_api_Copy(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void InternalCopy(t_uint16 InBuffCnt, t_uint32 offset) {
      ((mpeg4dec_arm_nmf_api_CopyDescriptor*)reference)->InternalCopy(InBuffCnt, offset);
    }
};

#endif
