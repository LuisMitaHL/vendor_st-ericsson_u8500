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

/* 'timer.api.timer' interface */
#if !defined(timer_api_timer_IDL)
#define timer_api_timer_IDL

#include <cpp.hpp>


class timer_api_timerDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual t_uint32 startTimer(t_uint32 fisrtAlarm, t_uint32 period) = 0;
    virtual t_uint32 startHighPrecisionTimer(t_uint32 fisrtAlarm, t_uint32 period) = 0;
    virtual void stopTimer(void) = 0;
};

class Itimer_api_timer: public NMF::InterfaceReference {
  public:
    Itimer_api_timer(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    t_uint32 startTimer(t_uint32 fisrtAlarm, t_uint32 period) {
      return ((timer_api_timerDescriptor*)reference)->startTimer(fisrtAlarm, period);
    }
    t_uint32 startHighPrecisionTimer(t_uint32 fisrtAlarm, t_uint32 period) {
      return ((timer_api_timerDescriptor*)reference)->startHighPrecisionTimer(fisrtAlarm, period);
    }
    void stopTimer(void) {
      ((timer_api_timerDescriptor*)reference)->stopTimer();
    }
};

#endif
