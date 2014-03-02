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

/* 'msp_dma_ctrl_ab8500.configure' interface */
#if !defined(msp_dma_ctrl_ab8500_configure_IDL)
#define msp_dma_ctrl_ab8500_configure_IDL

#include <cpp.hpp>


class msp_dma_ctrl_ab8500_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(void* bufferTx, t_uint16 bufferSizeInSample, t_uint16 nbMspChannelEnabled, t_uint16 target) = 0;
};

class Imsp_dma_ctrl_ab8500_configure: public NMF::InterfaceReference {
  public:
    Imsp_dma_ctrl_ab8500_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(void* bufferTx, t_uint16 bufferSizeInSample, t_uint16 nbMspChannelEnabled, t_uint16 target) {
      ((msp_dma_ctrl_ab8500_configureDescriptor*)reference)->setParameter(bufferTx, bufferSizeInSample, nbMspChannelEnabled, target);
    }
};

#endif
