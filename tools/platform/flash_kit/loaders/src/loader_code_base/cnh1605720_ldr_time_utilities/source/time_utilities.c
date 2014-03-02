/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 * @addtogroup ldr_time_utilities
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include  "r_basicdefinitions.h"
#include  "r_time_utilities.h"
#include  "error_codes.h"
#include  "r_debug.h"
#include  "r_debug_macro.h"

#ifdef WIN32
#define DECLARE_TIMER_HANDLES
#include  "t_emul_threads.h"
#endif //WIN32

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/


/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
/* System  variable for controlling Timer handler */
TimerHeader_t  SystemTimers;
volatile uint32 SystemTime = 0;


/*******************************************************************************
 * local function declaration
 ******************************************************************************/
static  uint32  Timer_FindFreeTimer(void);
Timer_t *Timer_GetActiveTimer(uint32 TimerKey);

#ifdef WIN32//Dummy functions for Windows build

static void R_Do_HL1TIMER_EnableInterrupt(void);
static void R_Do_HL1TIMER_DisableInterrupt(void);

#endif
/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initialization of the Timers handler.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] Timers     Numbers of defined timers.
 *
 * @retval  E_SUCCESS After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If no timer s for initialization.
 */
ErrorCode_e Do_Timer_TimersInit(void *Object_p, uint32 Timers)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32 TimerArraySize;

    /* Check input parameters. */
    VERIFY(0 != Timers, E_INVALID_INPUT_PARAMETERS);

    TimerArraySize = Timers * sizeof(Timer_t);
    /* Allocate memory space for timers */
    SystemTimers.Timers_p = (Timer_t *)malloc(TimerArraySize);
    ASSERT(NULL != SystemTimers.Timers_p);

    /* Clear Timers buffer */
    memset(SystemTimers.Timers_p, 0x00, TimerArraySize);

    /* Inicialize numbers of defined and activated timers */
    SystemTimers.MaxTimers = Timers;
    SystemTimers.ActiveTimers = 0;

#ifndef WIN32

    ReturnValue = TimerInit(1000);

#else

    ReturnValue = E_SUCCESS;

#endif

ErrorExit:
    return ReturnValue;
}

/*
 * Stops the Timers.
 */
void Do_Timer_TimersStop(void)
{
    TimerStop();
}

/*
 * Reserve new timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] Timer_p    pointer to the timer data.
 *
 * @return Index of reserved timer.
 * @return Index 0 if no free timers.
 */
uint32 Do_Timer_TimerGet(void *Object_p, Timer_t *Timer_p)
{
    uint32  TimerKey = 0;
    Timer_t  *TimerData_p = SystemTimers.Timers_p;

    /* Disable Timer interrupt */
    R_Do_HL1TIMER_DisableInterrupt();

    /* Find free timer */
    TimerKey = Timer_FindFreeTimer();

    if (0 == TimerKey) {
        /* no free timers */
        A_(printf("time_utilities.c (%d): No free timers!\n", __LINE__);)
        goto  ErrorExit;
    }

    /* Check input parameters */
    if ((Timer_p->Time == 0) || (Timer_p->HandleFunction_p == NULL)) {
        A_(printf("time_utilities.c (%d): ** Invalid input parameters! **\n", __LINE__);)
        TimerKey = 0;
        goto  ErrorExit;
    }

    /* setup reserved timer */
    if (Timer_p->Time <= 2) {
        TimerData_p[TimerKey - 1].Time = 2;
    } else {
        TimerData_p[TimerKey - 1].Time = Timer_p->Time;
    }

    TimerData_p[TimerKey - 1].HandleFunction_p = Timer_p->HandleFunction_p;
    TimerData_p[TimerKey - 1].PeriodicalTime = Timer_p->PeriodicalTime;
    TimerData_p[TimerKey - 1].Data_p = Timer_p->Data_p;
    TimerData_p[TimerKey - 1].Param_p = Timer_p->Param_p;

    TimerData_p[TimerKey - 1].Set_Time = Timer_p->Time;
    TimerData_p[TimerKey - 1].Set_System_Time = SystemTime;

    /* count the new reserved timer */
    SystemTimers.ActiveTimers++;

    if (SystemTimers.ActiveTimers > SystemTimers.MaxTimers) {
        SystemTimers.ActiveTimers--;
        A_(printf("time_utilities.c (%d): No free timers!\n", __LINE__);)
        TimerKey = 0;
        goto  ErrorExit;
    }

ErrorExit:
    /* Enable Timer interrupt */
    R_Do_HL1TIMER_EnableInterrupt();

    return TimerKey;
}


/*
 * Release reserved timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @retval E_SUCCESS  After successful execution.
 * @retval E_INVALID_INPUT_PARAMETERS If one of the input.
 * @retval E_NONEXIST_TIMER non exist timer.
 */
ErrorCode_e Do_Timer_TimerRelease(void *Object_p, uint32 TimerKey)
{
    Timer_t  *TimerData_p = NULL;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    /* Disable Timer interrupt */
    R_Do_HL1TIMER_DisableInterrupt();

    /* check input parameters */
    VERIFY(0 != TimerKey, E_INVALID_INPUT_PARAMETERS);

    TimerData_p = Timer_GetActiveTimer(TimerKey);

    if (NULL != TimerData_p) {
        /* clear timer data */
        TimerData_p->Time = 0;
        TimerData_p->HandleFunction_p = NULL;

        /* first check number of active timers */
        if (SystemTimers.ActiveTimers > 0) {
            /* remove timer from activ timer counter */
            SystemTimers.ActiveTimers--;
        }

        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

    ReturnValue = E_NONEXIST_TIMER;
ErrorExit:
    /* Enable Timer interrupt */
    R_Do_HL1TIMER_EnableInterrupt();

    return ReturnValue;
}


/*
 * Read the current time of timer.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 * @param [in] TimerKey   Index of reserved timer.
 *
 * @return current time.
 */
uint32 Do_Timer_ReadTime(void *Object_p, uint32 TimerKey)
{
    Timer_t  *TimerData_p = NULL;
    uint32  Time = 0;

    TimerData_p = Timer_GetActiveTimer(TimerKey);

    if (TimerData_p != NULL) {
        /* get timer data */
        Time = TimerData_p->Time;
    }

    return  Time;
}

/*
 * Get current system time.
 *
 * @param [in] Object_p   pointer to LCM instance context.
 *
 * @return System Time.
 */
uint32 Do_Timer_GetSystemTime(void *Object_p)
{
    return  SystemTime;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * Find free timer.
 *
 * @retval Timer index if free timer exist.
 * @retval 0 if no free timer.
 */
static uint32 Timer_FindFreeTimer(void)
{
    uint32  TimerKey = 0;
    Timer_t  *TmpTimers_p = SystemTimers.Timers_p;

    if (SystemTimers.MaxTimers == SystemTimers.ActiveTimers) {
        return  TimerKey;
    }

    for (TimerKey = 0; TimerKey < SystemTimers.MaxTimers; TimerKey++) {
        if (TmpTimers_p[TimerKey].HandleFunction_p == NULL) {
            return (TimerKey + 1);
        }
    }

    TimerKey = 0;    /* No free timer and return "0" */
    return  TimerKey;
}


/*
 * Get active timer.
 *
 * @param [in] TimerKey Timer index of requested timer
 *
 * @retval Pointer Pointer to timer meta data if requested timer exist.
 * @retval NULL    if requested timer is not founded.
 */
Timer_t *Timer_GetActiveTimer(uint32 TimerKey)
{
    Timer_t  *TmpTimers_p = NULL;

    /* check input parameters */
    if ((0 == TimerKey) || (TimerKey > SystemTimers.MaxTimers)) {
        return  TmpTimers_p;
    }

    /* get pointer to the timer */
    TmpTimers_p = (Timer_t *)&SystemTimers.Timers_p[TimerKey - 1];

    /* check if timer is active */
    if (TmpTimers_p->HandleFunction_p == NULL) {
        TmpTimers_p = NULL;
    }

    return  TmpTimers_p;
}

void Sleep(uint32 Interval)
{
    uint32 Start = Do_Timer_GetSystemTime(NULL);

    /* wait for time Interval to pass */
    while (SystemTime < (Start + Interval)) {
        ;
    }
}

#ifdef WIN32

/*
 * Timer interrupt setting.
 *
 * @param [in] Time_Period Time for timer Interrupt.
 *
 * @retval E_SUCCESS            If successfuly is set.
 * @retval E_TIMER_INIT_FAILED  If Initialization failed.
 * @retval E_TIMER_IRQ_CONF_FAILED If interrupt settings failed.
 */

//Dummy functions for Windows build

static void R_Do_HL1TIMER_EnableInterrupt(void)
{
    return;
}

static void R_Do_HL1TIMER_DisableInterrupt(void)
{
    return;
}

#endif

/** @} */
