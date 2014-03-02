/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_TIME_UTILITIES_H_
#define _T_TIME_UTILITIES_H_
/**
 * @addtogroup ldr_time_utilities
 * @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** MAX defined timers */
#define MAX_TIMERS    100

/** defined type of function pointer to the handle function */
typedef void (*HandleFunction_t)(void *Param_p, void *Timer_p, void *Data_p);

/**
 * Structure of timer data
 */
typedef struct {
    uint32           Time;             /**< Requested for timer time.*/
    uint32           PeriodicalTime;   /**< Requested periodical time for timer.*/
    HandleFunction_t HandleFunction_p; /**< Callback function*/
    void            *Data_p;           /**< Pointer to data. */
    void            *Param_p;          /**< Extra parameters. */
    uint32           Set_Time;         /**< Used for debugging purposes only. */
    uint32           Set_System_Time;  /**< Used for debugging purposes only. */
} Timer_t;

/**
 * Header for buffer of timers
 */
typedef struct {
    uint32   MaxTimers;
    uint32   ActiveTimers;
    Timer_t *Timers_p;
} TimerHeader_t;

/** @} */
#endif /*_T_TIME_UTILITIES_H_*/
