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

/* 'pcmdump.complete' interface */
#if !defined(pcmdump_complete_IDL)
#define pcmdump_complete_IDL

#include <cpp.hpp>
#include <host/pcmdump/complete.idt.h>


class pcmdump_completeDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void pcmdump_complete(PcmDumpCompleteInfo_t completeInfo) = 0;
};

class Ipcmdump_complete: public NMF::InterfaceReference {
  public:
    Ipcmdump_complete(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void pcmdump_complete(PcmDumpCompleteInfo_t completeInfo) {
      ((pcmdump_completeDescriptor*)reference)->pcmdump_complete(completeInfo);
    }
};

#endif
