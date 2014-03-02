/***********************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 **********************************************************************/

/*************************************************************************
* Includes
*************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "Timer.h"
#include "LCDriverMethods.h"


/***********************************************************************
 * Declaration of file local functions
 **********************************************************************/
/* Constructor */
Timer::Timer(void):
    systemTime_(0),
    logger_(0)
{
    timers_.Timers_p = NULL;
    timers_.ActiveTimers = 0;
    timers_.MaxTimers = 0;
}

/* Destructor */
Timer::~Timer(void)
{
    delete[] timers_.Timers_p;
}
/***********************************************************************
 * Definition of external functions
 **********************************************************************/

/*
 * Initialization of the Timers handler.
 *
 * @param [in] Timers   Numbers of defined timers.
 *
 * @retval   E_SUCCESS After successful execution.
 * @retval   E_INVALID_INPUT_PARAMETERS If no timer s for initialization.
 */
ErrorCode_e Timer::Init(uint32 Timers)
{
    /* Check input parameters. */
    if (0 == Timers) {
        /* Invalid input parameters! */
        return E_INVALID_INPUT_PARAMETERS;
    }

    /* Allocate memory space for timers */
    timers_.Timers_p = new Timer_t[Timers];

    if (timers_.Timers_p == NULL) {
        /* Error allocation Timers buffer! */
        return E_ALLOCATE_FAILED;
    }

    /* Clear Timers buffer */
    memset(timers_.Timers_p, 0x00, Timers * sizeof(Timer_t));

    /* Inicialize numbers of defined and activated timers */
    timers_.MaxTimers = Timers;
    timers_.ActiveTimers = 0;

    return E_SUCCESS;
}

/*
 * Reserve new timer.
 *
 * @param [in] Timer_p   pointer to the timer data.
 *
 * @return   Index of reserved timer.
 * @return   Index 0 if no free timers.
 */
uint32 Timer::Get(Timer_t *Timer_p)
{
    uint32 TimerKey = 0;

    /* Check input parameters */
    if (0 == Timer_p->Time || NULL == Timer_p->HandleFunction_p) {
        goto ErrorExit;
    }

    /* Find free timer */
    TimerKey = FindFreeTimer();

    if (0 == TimerKey) {
        goto ErrorExit; // no free timers
    }

    timers_.ActiveTimers++;

    timers_.Timers_p[TimerKey - 1] = *Timer_p;

#ifdef _TIMERDEBUG
    logger_->log("Timer: Get - Timer 0x%p", &timers_.Timers_p[TimerKey - 1]);
#endif

ErrorExit:
    return TimerKey;
}

/*
 * Release reserved timer.
 *
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @retval   E_SUCCESS  After successful execution.
 * @retval   E_INVALID_INPUT_PARAMETERS If one of the input.
 * @retval   E_NONEXIST_TIMER non exist timer.
 */
ErrorCode_e Timer::Release(uint32 TimerKey)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    Timer_t *TimerData_p = NULL;

    /* check input parameters */
    if ((0 == TimerKey) || (timers_.MaxTimers < TimerKey)) {
        goto ErrorExit;
    }

    TimerData_p = &timers_.Timers_p[TimerKey - 1];

    if (NULL != TimerData_p->HandleFunction_p) {
        /* clear timer data */
        TimerData_p->Time = 0;
        TimerData_p->HandleFunction_p = NULL;

        /* first check number of active timers */
        if (0 < timers_.ActiveTimers) {
            /* remove timer from active timer counter */
            timers_.ActiveTimers--;
        }

        ReturnValue = E_SUCCESS;

#ifdef _TIMERDEBUG
        logger_->log("Timer: Release - Timer 0x%p", (void *)TimerData_p);
#endif
    } else {
        ReturnValue = E_NONEXIST_TIMER;
    }

ErrorExit:
    return ReturnValue;
}

/*
 * Read the current time of timer.
 *
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @return   Current time of the timer.
 */
uint32 Timer::ReadTime(uint32 TimerKey)
{
    uint32 Time = 0;

    if (TimerKey == 0 || TimerKey > timers_.MaxTimers) {
        return Time;
    }

    Timer_t *TimerData_p = &timers_.Timers_p[TimerKey - 1];

    if (NULL != TimerData_p->HandleFunction_p) {
        /* get timer data */
        Time = TimerData_p->Time;
    }

    return Time;
}

/*
 * Get current system time.
 *
 * @return   System Time.
 */
uint32 Timer::GetSystemTime()
{
    return static_cast<uint32>(systemTime_);
}


/*
 * The timer handler method which is polled in the main execution thread.
 * Checks if there is a timer which time has elapsed and calls its callback,
 * handle the timers time decrementation.
 *
 * @param [in] IsTimerOn   Determinates if the timers are active to be handled.
 *
 * @return   Function doesn't return value.
 */
void Timer::DoTimerHandler(bool IsTimerOn)
{
    uint32 SearchTimerKey = 0;
    uint32 TimeIncrement = GetIncrement();

    while (timers_.ActiveTimers > 0 && SearchTimerKey < timers_.MaxTimers) {
        /* get Timer(SearchTimerKey) data */
        Timer_t *TimerData_p = &timers_.Timers_p[SearchTimerKey];

        if (NULL != TimerData_p->HandleFunction_p) {
            /* decrement Time */
            if (IsTimerOn) {
                TimerData_p->Time -= min(TimerData_p->Time, TimeIncrement);
            }

            /* check for elapsed time */
            if (TimerData_p->Time == 0) {
#ifdef _TIMERDEBUG
                logger_->log("Timer: DoTimerHandler - callback handle function Timer 0x%p", TimerData_p);
#endif
                TimerData_p->HandleFunction_p(TimerData_p->Param_p, TimerData_p, TimerData_p->Data_p);

                /* Check for periodicals of timer */
                if (TimerData_p->PeriodicalTime != 0) {
                    /* Set again Timer with periodical time */
                    TimerData_p->Time = TimerData_p->PeriodicalTime;
                } else {
                    /* release current timer */
                    Release(SearchTimerKey + 1);
                }
            }
        }

        /* next timer for check */
        SearchTimerKey++;
    }
}

/*
 * Get the difference in time from the last call.
 *
 * @return increment   the duration of time which should be
 *                     substracted from the timers.
 */
uint32 Timer::GetIncrement()
{
    time_t now = OS::GetSystemTimeInMs();
    time_t increment = now - systemTime_;
    systemTime_ = now;
    return static_cast<uint32>(increment);
}

/*
 * Find free timer.
 *
 * @retval   Timer index if free timer exist.
 * @retval   0 if no free timer.
 */
uint32 Timer::FindFreeTimer()
{
    uint32 TimerKey = 0;

    for (size_t i = 0; i != timers_.MaxTimers; ++i) {
        if (timers_.Timers_p[i].HandleFunction_p == NULL) {
            TimerKey = i + 1;
            break;
        }
    }

    return TimerKey;
}
