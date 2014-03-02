/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */
#define _CNAME_ DurationTimerPerfTimer

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamPerfTimer.h"

namespace android {

void DurationTimerPerfTimer::init()
    {
    DBGT_PROLOG("");

    //set to 0
    mTotalTimeFromInit = 0;

    //start timer
    mTimer.start();

    DBGT_EPILOG("");
    }

MPerfTimer::Time DurationTimerPerfTimer::elapsedTime()
    {
    DBGT_PROLOG("");
    
    //stop timer
    mTimer.stop();

    //get elapsed time
    int64_t time = mTimer.durationUsecs();

    //add elapsed time
    mTotalTimeFromInit += static_cast<Time>(time);

    DBGT_PTRACE("Duration: %lld", mTotalTimeFromInit);

    //start timer
    mTimer.start();

    DBGT_EPILOG("");
    return mTotalTimeFromInit;
    }

#undef _CNAME_
#define _CNAME_ SystemTimePerfTimer

void SystemTimePerfTimer::init()
    {
    DBGT_PROLOG("");

    //store current time
    mInitTime = systemTime(SYSTEM_TIME_REALTIME);

    DBGT_EPILOG("");
    }

MPerfTimer::Time SystemTimePerfTimer::elapsedTime()
    {
    DBGT_PROLOG("");
    
    //get current time
    nsecs_t currentTick = systemTime(SYSTEM_TIME_REALTIME);

    DBGT_PTRACE("Current tick: %lld", currentTick);

    Time time = (currentTick-mInitTime)/1000;

    DBGT_EPILOG("");
    return time;
    }

} //namespace android 

/* End of STECamPerfTimer.cpp */
