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

/* 'dmaout.low_power' interface */
#if !defined(dmaout_low_power_IDL)
#define dmaout_low_power_IDL

#include <cpp.hpp>


class dmaout_low_powerDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void startLowPowerTransition(void* buffer_ring, t_uint16 buffer_ring_size) = 0;
    virtual void startNormalModeTransition(void) = 0;
    virtual void mutePort(t_uint16 port_idx) = 0;
};

class Idmaout_low_power: public NMF::InterfaceReference {
  public:
    Idmaout_low_power(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void startLowPowerTransition(void* buffer_ring, t_uint16 buffer_ring_size) {
      ((dmaout_low_powerDescriptor*)reference)->startLowPowerTransition(buffer_ring, buffer_ring_size);
    }
    void startNormalModeTransition(void) {
      ((dmaout_low_powerDescriptor*)reference)->startNormalModeTransition();
    }
    void mutePort(t_uint16 port_idx) {
      ((dmaout_low_powerDescriptor*)reference)->mutePort(port_idx);
    }
};

#endif
