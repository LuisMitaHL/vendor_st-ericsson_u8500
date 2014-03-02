/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file for TIMER (MTU) 
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TMR_P_H_
#define _TMR_P_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "tmr_irqp.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define TMR_HCL_VERSION_ID          1
#define TMR_MAJOR_VERSION           0
#define TMR_MINOR_VERSION           7

#define TMR_TOTAL_NUMBER_OF_TIMERS  (TMR_INSTANCES_OF_TMR_UNITS * TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT)

#define TMR_PERIPHID0   0x04
#define TMR_PERIPHID1   0x08
#define TMR_PERIPHID2   0x08
#define TMR_PERIPHID3   0x04
#define TMR_PCELLID0    0x0D
#define TMR_PCELLID1    0xF0
#define TMR_PCELLID2    0x05
#define TMR_PCELLID3    0xB1

#define TMR_SPACE_FOR_SAVING_DEVICE_CONTEXT 34

/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/
/*definition relative to the use of MTU */
typedef enum
{
    TMR_STATE_BUSY          = 0,
    TMR_STATE_FREE          = 1,
    TMR_STATE_INITIALIZED   = 2,
    TMR_STATE_RUNNING       = 3
} t_tmr_state;

typedef struct
{
    t_tmr_register          *p_tmr_register[TMR_INSTANCES_OF_TMR_UNITS];
    t_tmr_register_subset   *tmr_temp_ptr[TMR_TOTAL_NUMBER_OF_TIMERS];  /*timer ptrs */

    t_uint8                 tmr_initialized[TMR_INSTANCES_OF_TMR_UNITS];

    t_tmr_state             tmr_state[TMR_TOTAL_NUMBER_OF_TIMERS];      /*state of the timer BUSY/FREE*/
    t_tmr_id                tmr_mask[TMR_TOTAL_NUMBER_OF_TIMERS];       /*mask for a specific timer*/
    t_tmr_mode              tmr_mode[TMR_TOTAL_NUMBER_OF_TIMERS];       /*mode for a specific timer*/

    t_uint32                number_of_total_timers;                     /*number of timers for the TU in use*/
    t_tmr_event             tmr_event;

    t_uint32                tmr_device_context[TMR_SPACE_FOR_SAVING_DEVICE_CONTEXT];
} t_tmr_system_context;

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/* HW access macros to value register*/
#define TMR_SET_LOAD_REG(m)     (p_tmr_register_subset->tmr_load = (m))
#define TMR_GET_VALUE_REG(m)    ((m) = p_tmr_register_subset->tmr_value)

/* HW access macros to  background load register*/
#define TMR_SET_BGLOAD_REG(m)   (p_tmr_register_subset->tmr_bgload = (m))

/*********************control register  access macros********************/
#define TMR_TIMER_ENABLE    TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, TIMER_ENABLE, 1)

/*
 * timer enable on control register
 */
#define TIMER_ENABLEwidth   1
#define TIMER_ENABLEshift   7

/*
 * timer disaable on control register
 */
#define TMR_TIMER_DISABLE   TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, TIMER_DISABLE, 0)

/*
 * timer DISABLE on control register
 */
#define TIMER_DISABLEwidth  1
#define TIMER_DISABLEshift  7

/*
 * periodic timer enable on control register
 */
#define TMR_PERIODIC_TIMER_ENABLE   TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, PERIODIC_TIMER_ENABLE, 1)

/*
 *  periodic PERIODIC_TIMER enable on control register
 */
#define PERIODIC_TIMER_ENABLEwidth  1
#define PERIODIC_TIMER_ENABLEshift  6

/*
 * periodic timer DISABLE on control register
 */
#define TMR_PERIODIC_TIMER_DISABLE  TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, PERIODIC_TIMER_DISABLE, 0)

/*
 *  periodic PERIODIC_TIMER DISABLE on control register
 */
#define PERIODIC_TIMER_DISABLEwidth 1
#define PERIODIC_TIMER_DISABLEshift 6

/*
 * prescaler set on control register
 */
#define TMR_PRESCALER_SET(x)    TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, PRESCALER_SET, x)
#define PRESCALER_SETwidth      2
#define PRESCALER_SETshift      2


/*
 *  timer size on control register
 */
#define TIMER_SIZE_SETwidth 1
#define TIMER_SIZE_SETshift 1

#define TIMER_SIZE_16       0
#define TIMER_SIZE_32       1

/*#define MTU_TIMER_SIZE_SET(x)        TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control,  TIMER_SIZE_SET, x)*/
#define MTU_TIMER_SIZE_SET_0(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[0]->tmr_control1, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_1(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[0]->tmr_control2, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_2(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[0]->tmr_control3, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_3(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[0]->tmr_control4, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_4(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[1]->tmr_control1, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_5(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[1]->tmr_control2, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_6(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[1]->tmr_control3, TIMER_SIZE_SET, x)
#define MTU_TIMER_SIZE_SET_7(x) TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[1]->tmr_control4, TIMER_SIZE_SET, x)

/*
 * timer one shot on control register
 */
#define TMR_TIMER_ONE_SHOT_SET(x)   TMR_HCL_FIELDSET(p_tmr_register_subset->tmr_control, TIMER_ONE_SHOT_SET, x)

/*
 *  timer one shot on control register
 */
#define TIMER_ONE_SHOT_SETwidth 1
#define TIMER_ONE_SHOT_SETshift 0

#define TIMER_WRAPPING          0
#define TIMER_ONE_SHOT          1

/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
#ifdef __DEBUG
t_uint8         GetnAlarmsInserted(void);
void            PrintAlarmList(void);
#endif
PRIVATE void    tmr_EnterCriticalSection(t_uint32 *);
PRIVATE void    tmr_ExitCriticalSection(t_uint32);
#endif /* _HCL_TMRP_H_ */

/* End of file - tmr_p.h*/

