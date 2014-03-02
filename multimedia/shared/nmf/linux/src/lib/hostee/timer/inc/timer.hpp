/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <time.h>
#include <signal.h>

class timer: public timerTemplate {
protected:
  timer_t timerid;

public:
  bool isPrioritySet;

  timer();
  ~timer();
  virtual t_nmf_error construct(void);
  virtual void destroy(void);
  virtual t_uint32 startTimer(t_uint32 fisrtAlarm, t_uint32 period);
  virtual t_uint32 startHighPrecisionTimer(t_uint32 fisrtAlarm, t_uint32 period);
  virtual void stopTimer(void);
};

#endif /* TIMER_HPP */
