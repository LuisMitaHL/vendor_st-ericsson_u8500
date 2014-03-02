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

/* 'dmaout.low_signal' interface */
#if !defined(dmaout_low_signal_IDL)
#define dmaout_low_signal_IDL

#include <cpp.hpp>


class dmaout_low_signalDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void lowPowerMode(t_uint16 framecount) = 0;
    virtual void normalMode(void) = 0;
    virtual void acknowledgeRmForLowPowerMode(void) = 0;
    virtual void lowPowerModeSetup(t_uint16 port_idx) = 0;
    virtual void fifoEmptied(void) = 0;
    virtual void digitalMute(t_uint16 mute) = 0;
    virtual void portMuted(t_uint16 port_idx) = 0;
};

class Idmaout_low_signal: public NMF::InterfaceReference {
  public:
    Idmaout_low_signal(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void lowPowerMode(t_uint16 framecount) {
      ((dmaout_low_signalDescriptor*)reference)->lowPowerMode(framecount);
    }
    void normalMode(void) {
      ((dmaout_low_signalDescriptor*)reference)->normalMode();
    }
    void acknowledgeRmForLowPowerMode(void) {
      ((dmaout_low_signalDescriptor*)reference)->acknowledgeRmForLowPowerMode();
    }
    void lowPowerModeSetup(t_uint16 port_idx) {
      ((dmaout_low_signalDescriptor*)reference)->lowPowerModeSetup(port_idx);
    }
    void fifoEmptied(void) {
      ((dmaout_low_signalDescriptor*)reference)->fifoEmptied();
    }
    void digitalMute(t_uint16 mute) {
      ((dmaout_low_signalDescriptor*)reference)->digitalMute(mute);
    }
    void portMuted(t_uint16 port_idx) {
      ((dmaout_low_signalDescriptor*)reference)->portMuted(port_idx);
    }
};

#endif
