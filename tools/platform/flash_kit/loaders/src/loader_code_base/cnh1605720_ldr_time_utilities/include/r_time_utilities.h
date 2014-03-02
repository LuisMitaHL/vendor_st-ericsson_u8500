/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_TIME_UTILITIES_H_
#define _R_TIME_UTILITIES_H_
/**
 * @addtogroup ldr_time_utilities Timer utilities
 * Can be created and handled 255 timers. Every timer can be the periodical or
 * non-periodical. The maximum time that can be set is 0xFFFFFFFF seconds.
 * Timer is count-down counter and when time is 0 the timer will finish and can
 * be called callback function if is defined, when timer is registered.
 *
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include  "t_time_utilities.h"
#include  "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Initialization of the Timers handler.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] Timers     Numbers of defined timers.
 *
 * @retval  E_SUCCESS   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If no timer s for initialization.
 */
ErrorCode_e Do_Timer_TimersInit(void *Object_p, uint32 Timers);

/*
 * Stops the Timers.
 */
void Do_Timer_TimersStop(void);

/**
 * Reserve new timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] Timer_p    pointer to the timer data.
 *
 * @return Index of reserved timer.
 * @return Index 0 if no free timers.
 */
uint32 Do_Timer_TimerGet(void *Object_p, Timer_t *Timer_p);

/**
 * Release reserved timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @retval E_SUCCESS    After successful execution.
 * @retval E_INVALID_INPUT_PARAMETERS If one of the input.
 * @retval E_NONEXIST_TIMER non exist timer.
 */
ErrorCode_e Do_Timer_TimerRelease(void *Object_p, uint32 TimerKey);

/**
 * Read the current time of timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @return current time.
 */
uint32 Do_Timer_ReadTime(void *Object_p, uint32 TimerKey);

/**
 * Get current system time.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 *
 * @return System Time.
 */
uint32 Do_Timer_GetSystemTime(void *Object_p);

/**
 * Busy waiting implementation of a sleep function
 *
 * @param [in] Interval   Interval in ms.
 *
 */
void Sleep(uint32 Interval);

/*
 * Timer interrupt setting.

 * @param [in] Time_Period Time for timer Interrupt.
 *
 * @retval E_SUCCESS              If successfully set.
 * @retval E_GENERAL_FATAL_ERROR  If failed.
 */
ErrorCode_e TimerInit(uint32 Time_Period);

/*
 * Stops the Timer.
 */
void TimerStop(void);

void R_Do_HL1TIMER_EnableInterrupt(void);
void R_Do_HL1TIMER_DisableInterrupt(void);

/*
 * Get active timer.
 *
 * @param [in] TimerKey Timer index of requested timer
 *
 * @retval Pointer Pointer to timer meta data if requested timer exist.
 * @retval NULL    if requested timer is not founded.
 */
Timer_t *Timer_GetActiveTimer(uint32 TimerKey);

/** @} */
#endif /*_R_TIME_UTILITIES_H_*/
