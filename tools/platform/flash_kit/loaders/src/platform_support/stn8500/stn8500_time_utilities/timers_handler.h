/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

#ifndef TIMERS_HANDLER_H_
#define TIMERS_HANDLER_H_
/**
 * @addtogroup ldr_time_utilities Timer utilities
 *
 * @{
 */

/*************************************************************************
* Includes
*************************************************************************/

#ifdef CFG_ENABLE_LOADER_TYPE
#include  "tmr_irq.h"
#endif


/**
 * On every 10mS, timer interrurpt handler will execute this timer handler.
 *
 * @return none.
 */
#ifdef CFG_ENABLE_LOADER_TYPE
void Do_Timer_TimersHandler(t_tmr_device_id tmr_device_id);
#else
void Do_Timer_TimersHandler(void *Object_p);
#endif

/** @} */
#endif /* TIMERS_HANDLER_H_ */
