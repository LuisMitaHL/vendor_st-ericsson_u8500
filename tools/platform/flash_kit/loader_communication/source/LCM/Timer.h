/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "error_codes.h"
#include "t_time_utilities.h"
#include "Logger.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif
#include <ctime>

class Timer
{
public:
    Timer();
    /*
     * Initialization of the Timers handler.
     *
     * @param [in] Timers   Numbers of defined timers.
     *
     * @retval   E_SUCCESS After successful execution.
     * @retval   E_INVALID_INPUT_PARAMETERS If no timer s for initialization.
     */
    ErrorCode_e Init(uint32 Timers);

    /*
     * Reserve new timer.
     *
     * @param [in] Timer_p   pointer to the timer data.
     *
     * @return   Index of reserved timer.
     * @return   Index 0 if no free timers.
     */
    uint32 Get(Timer_t *Timer_p);

    /*
     * Release reserved timer.
     *
     * @param [in] TimerKey   Index of reserved timer.
     *
     * @retval   E_SUCCESS  After successful execution.
     * @retval   E_INVALID_INPUT_PARAMETERS If one of the input.
     * @retval   E_NONEXIST_TIMER non exist timer.
     */
    ErrorCode_e Release(uint32 TimerKey);

    /*
     * Read the current time of timer.
     *
     * @param [in] TimerKey   Index of reserved timer.
     *
     * @return   Current time of the timer.
     */
    uint32 ReadTime(uint32 TimerKey);

    /**
     * Get current system time.
     *
     * @return System Time.
     */
    uint32 GetSystemTime(void);

    /*
     * The timer handler method which is polled in the main execution thread.
     * Checks if there is a timer which time has elapsed and calls its callback,
     * handle the timers time decrementation.
     *
     * @param [in] IsTimerOn   Determinates if the timers are active to be handled.
     *
     * @return   Function doesn't return value.
     */
    void DoTimerHandler(bool IsTimerOn);

public:
    ~Timer(void);
    void SetLogger(Logger *logger) {
        logger_ = logger;
    }

private:
    TimerHeader_t timers_;
    time_t systemTime_;

    Logger *logger_;

    uint32 GetIncrement();
    uint32 FindFreeTimer();
};

#endif // _TIMER_H_
