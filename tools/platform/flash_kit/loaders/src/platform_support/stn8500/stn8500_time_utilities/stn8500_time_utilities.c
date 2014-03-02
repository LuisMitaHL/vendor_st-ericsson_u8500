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
#include  "error_codes.h"
#include  "r_debug.h"
#include  "r_debug_macro.h"
#include  "r_time_utilities.h"

#ifdef WIN32
#define DECLARE_TIMER_HANDLES
#include  "t_emul_threads.h"
#else
#include "memory_mapping.h"
#include "rtc.h"
#include "rtc_irq.h"
#include "rtc_services.h"
#endif

/***********************************************************************
 * File scope types, constants and variables
 **********************************************************************/
/* System  variable for controlling Timer handler */
extern TimerHeader_t  SystemTimers;
extern uint32 SystemTime;

#define RTT_ONE_SEC  32768uL
#define PATTERN_VALUE  0xAAAAAAAA
#define USEFUL_PATTERN 127
extern t_ser_rtt_context g_ser_rtt_context;

/***********************************************************************
 * Definition of external functions
 **********************************************************************/

#ifdef CFG_ENABLE_LOADER_TYPE
void Do_Timer_TimersHandler(void *param1, void *param2)
{
    void *Object_p = NULL;
#else
void Do_Timer_TimersHandler(void *Object_p)
{
#endif

    uint32  SearchTimerKey = 0;
    uint32  TmpActiveTimers = SystemTimers.ActiveTimers;
    Timer_t  *TimerData_p = NULL;
    HandleFunction_t TmpHandleFunction_p = NULL;
    void *TmpData_p = NULL;
    void *TmpParam_p = NULL;

#ifdef TIMER_DEBUG
    static int ii = 0;

    ii++;

    if (ii % 10000 == 0) {
        A_(printf("ii %d\n", ii);)
        ii = 0;
    }

#endif

    /* first check number of active timers */
    if (SystemTimers.ActiveTimers != 0) {
        do {
            /* get Timer(SearchTimerKey) data */
            TimerData_p = Timer_GetActiveTimer(SearchTimerKey + 1);

            if (TimerData_p != NULL) {
                /* check for elapsed time */
                if (0 == TimerData_p->Time) {
                    /* check function handle pointer */
                    if (TimerData_p->HandleFunction_p != NULL) {
                        /* save temporary pointer to handle function */
                        TmpHandleFunction_p = TimerData_p->HandleFunction_p;
                        TmpData_p = TimerData_p->Data_p;
                        TmpParam_p = TimerData_p->Param_p;

                        /* Check for periodicals of timer */
                        if (TimerData_p->PeriodicalTime != 0) {
                            /* Set again Timer with periodical time */
                            TimerData_p->Time = TimerData_p->PeriodicalTime;
                        } else {
                            /* relase current timer */
                            (void)Do_Timer_TimerRelease(Object_p, SearchTimerKey + 1);

                            /* count for handled timer */
                            TmpActiveTimers--;
                        }

                        /* execute handle function */
                        /* @TODO: Check the order of the parameters XVSZOAN */
                        //            TmpHandleFunction_p(TmpData_p, TimerData_p, TmpParam_p);

                        B_(printf("timer callback %d ms!\n", TimerData_p->Set_Time);)
                        B_(printf("set system time %d ms!\n", TimerData_p->Set_System_Time);)
                        B_(printf("system time now %d ms!\n", SystemTime);)
                        TmpHandleFunction_p(TmpParam_p, TimerData_p, TmpData_p);
                    }
                } else {
                    /* decrement Time */
                    TimerData_p->Time--;

                    /* count for handled timer */
                    TmpActiveTimers--;
                }
            }

            /* next timer for check */
            SearchTimerKey++;
        } while ((TmpActiveTimers > 0) && (SearchTimerKey < SystemTimers.MaxTimers));
    }

    /* Increment system time every 1 mS */
    SystemTime ++;
}

#ifndef WIN32

void R_Do_HL1TIMER_EnableInterrupt(void)
{
    RTT_EnableIRQSrc(RTT_IRQ_SRC_ID_0);
}

void R_Do_HL1TIMER_DisableInterrupt(void)
{
    RTT_DisableIRQSrc(RTT_IRQ_SRC_ID_0);
}
#endif //WIN32


/*
 * Timer interrupt setting.

 * @param [in] Time_Period Time for timer Interrupt.
 *
 * @retval E_SUCCESS              If successfully set.
 * @retval E_GENERAL_FATAL_ERROR  If failed.
 */
ErrorCode_e TimerInit(uint32 Time_Period)
{
    ErrorCode_e ReturnValue = E_TIMER_INIT_FAILED;

    /* RTT initialization */
    t_rtt_pattern   rtt_pattern;
    t_rtt_counter   rtt_count;
    t_rtt_error     rtt_err;
    uint32          rtt_counter;

    rtt_counter = (RTT_ONE_SEC * Time_Period) / 1000000;

    if (0 == rtt_counter) {
        goto ErrorExit;
    }

    /* Variable Initialization */
    (rtt_count.rtt_counter1) = rtt_counter;
    (rtt_count.rtt_counter2) = rtt_counter;

    rtt_pattern.num_useful_pattern = USEFUL_PATTERN;
    rtt_pattern.pattern_val1 = PATTERN_VALUE;
    rtt_pattern.pattern_val2 = PATTERN_VALUE;
    rtt_pattern.pattern_val3 = PATTERN_VALUE;
    rtt_pattern.pattern_val4 = PATTERN_VALUE;

    /* Register Timer handler as callback function */
    g_ser_rtt_context.g_callback_rtt.fct = Do_Timer_TimersHandler;

    RTT_EnableIRQSrc(RTT_IRQ_SRC_ID_0);

    /* Start timer */
    rtt_err = RTT_StartTimer(RTT_MODE_PERIODIC, rtt_count, rtt_pattern);

    if (RTT_OK != rtt_err) {
        goto ErrorExit;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * Stops the Timer.
 */
void TimerStop(void)
{
    (void)RTT_StopTimer();
}
/** @} */
