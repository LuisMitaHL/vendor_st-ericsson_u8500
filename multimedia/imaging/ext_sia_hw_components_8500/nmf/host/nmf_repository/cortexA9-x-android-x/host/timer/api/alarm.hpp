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

/* 'timer.api.alarm' interface */
#if !defined(timer_api_alarm_IDL)
#define timer_api_alarm_IDL

#include <cpp.hpp>


class timer_api_alarmDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void signal(void) = 0;
};

class Itimer_api_alarm: public NMF::InterfaceReference {
  public:
    Itimer_api_alarm(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void signal(void) {
      ((timer_api_alarmDescriptor*)reference)->signal();
    }
};

#endif
