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

/* 'msp_dma_ctrl_ab8500.bitclock' interface */
#if !defined(msp_dma_ctrl_ab8500_bitclock_IDL)
#define msp_dma_ctrl_ab8500_bitclock_IDL

#include <cpp.hpp>


class msp_dma_ctrl_ab8500_bitclockDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void enable(t_bool on_off) = 0;
};

class Imsp_dma_ctrl_ab8500_bitclock: public NMF::InterfaceReference {
  public:
    Imsp_dma_ctrl_ab8500_bitclock(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void enable(t_bool on_off) {
      ((msp_dma_ctrl_ab8500_bitclockDescriptor*)reference)->enable(on_off);
    }
};

#endif
