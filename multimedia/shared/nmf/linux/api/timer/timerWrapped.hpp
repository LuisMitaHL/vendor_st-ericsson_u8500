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

/* 'timerWrapped' header library */
#if !defined(timerWrapped_NMF)
#define timerWrapped_NMF

#include <inc/type.h>
#include <cpp.hpp>
#if defined(__STN_8500) || defined(__STN_9540)
#include <cm/inc/cm.hpp>
#endif
#include <timer/api/timer.hpp>
#include <timer/api/alarm.hpp>
#include <ee/api/trace.hpp>

class timerWrapped: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~timerWrapped() {} // Protected, use timerWrapped() instead !!!
};

/*
 * Component Factory
 */
class timer;
IMPORT_NMF_COMPONENT timerWrapped* timerWrappedCreate(timer *pComp=0x0);
IMPORT_NMF_COMPONENT void timerWrappedDestroy(timerWrapped*& instance);

#endif
