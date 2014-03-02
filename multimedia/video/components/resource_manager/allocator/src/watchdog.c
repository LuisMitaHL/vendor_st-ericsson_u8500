/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <resource_manager/allocator.nmf>
#include "itc_api.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
typedef struct {
	t_uint32                                      AbsTimeout;
	Iresource_manager_api_inform_resource_status *  callback;
} ts_watchdog_desc;

EXTMEM ts_watchdog_desc         s_watchdogDesc[RESOURCE_MAX];
EXTMEM t_uint16                 is_timer_armed;
EXTMEM t_uint32                 timer_scheduled;

#define TIMER_THRESHOLD      2
#define TIMER_D2             0x7FFFFFFFUL
#define TIMER_D4             0x3FFFFFFFUL

/*------------------------------------------------------------------------
 * Private functions 
 *----------------------------------------------------------------------*/
 
#pragma inline
static void MEMO_INTERRUPT_H(t_uint16 mask)
{
    t_uint16 mask_cmp = ITREMAP_interf_itmemoh;                            
    ITREMAP_interf_itmemoh = mask_cmp | mask; 
}

#pragma inline
static void UNMEMO_INTERRUPT_H(t_uint16 mask)
{
    t_uint16 mask_cmp = ITREMAP_interf_itmemoh;                            
    ITREMAP_interf_itmemoh = mask_cmp & ~(t_uint16)(mask); 
}


void init_watchdog(void)
{
  t_uint16 i;
  is_timer_armed  = 0;
  timer_scheduled = 0;
  UNMEMO_INTERRUPT_H(0x4000);
  for(i=0;i<RESOURCE_MAX;i++)
  {
   	s_watchdogDesc[i].AbsTimeout = 0;
	  s_watchdogDesc[i].callback   = (Iresource_manager_api_inform_resource_status*)NULL;
  }
}

void deinit_watchdog(void)
{
  is_timer_armed = 0;  
  timer_scheduled = 0;
  MASK_INTERRUPT_H(0x4000);
  MEMO_INTERRUPT_H(0x4000);  
}

/* simply a>b, but with handling of timer rollback */
static t_uint16 a_greater_b(t_uint32 a, t_uint32 b)
{
  t_uint32 diff_time;

  /* Use if to avoid signed */
  if (a >= b)
     diff_time = (t_uint32)((t_uint32)a - (t_uint32)b);
  else
     diff_time = (t_uint32)((t_uint32)b - (t_uint32)a);

  if(((a > b) && (diff_time < TIMER_D2))
    ||((a  < b) && (diff_time > TIMER_D2)))  
      return 1;
    else
      return 0;
}


static void program_timer(t_uint32 scheduled_time)
{  
  is_timer_armed = 1;
  timer_scheduled = scheduled_time;
  iInterrupt.subscribeInterrupt(30, &iTimeout);
  
  /* Mask cmp_it, set timer, unmask */
  MASK_INTERRUPT_H(0x4000);
  ITC_SET_REG_32(ITC_CMP3_L, ITC_CMP3_H, scheduled_time); 
  UNMASK_INTERRUPT_H(0x4000);
  MEMO_INTERRUPT_H(0x4000);
}


/*------------------------------------------------------------------------
 * Methods of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  setTimeout 
 *
 * Define a timeout for the specified resource
 * 
 * \param resource              the resource being monitored
 * \param timeout               the timeout expressed in 90 KHz ticks 
 * \param cb                    callback to be called in case of timeout
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(setTimeout)(t_t1xhv_resource                               resource, 
                      Iresource_manager_api_inform_resource_status   *cb, 
                      t_uint32                                       timeout)
{
  t_uint32 current_time, scheduled_time, diff_time;
  t_uint16 i;
  
  /* no need to handle timer rollback, thanks to 32-bits unsigned arithmetic !! */
  current_time = ITC_GET_TIMER_32() + TIMER_THRESHOLD; 
  scheduled_time = current_time + timeout;

  s_watchdogDesc[resource].AbsTimeout = scheduled_time;
  s_watchdogDesc[resource].callback   = cb;
             
  if(((is_timer_armed)&&(a_greater_b(timer_scheduled,scheduled_time)))
     || (!is_timer_armed))
  {
      program_timer(scheduled_time);
  }  
}/* end of setTimeout() function */

/*****************************************************************************/
/**
 * \brief  cancelTimeout 
 *
 * Cancel timeout for the specified resource
 * 
 * \param resource              the resource being monitored
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(cancelTimeout)(t_t1xhv_resource resource)
{
  t_uint32 min_sched;
  t_sint16 found = -1;
  t_uint16 i;

  is_timer_armed = 0;
  timer_scheduled = 0;
	iInterrupt.unsubscribeInterrupt(30);
  /* mask cmp3 it and disable memorization of interrupts */
  MASK_INTERRUPT_H(0x4000);
  UNMEMO_INTERRUPT_H(0x4000);

	s_watchdogDesc[resource].AbsTimeout = 0;
	s_watchdogDesc[resource].callback   = (Iresource_manager_api_inform_resource_status*)NULL;
 
  // need to initialize to a big value but not too big 
  // otherwise it will be taken as a rollback...
  min_sched = ITC_GET_TIMER_32() + TIMER_D4; 

  /* if there were other timeouts scheduled, take the smallest one and arm the timer */
  for(i=0;i<RESOURCE_MAX;i++)
  {
    if((s_watchdogDesc[i].AbsTimeout!=0)&&(a_greater_b(min_sched,s_watchdogDesc[i].AbsTimeout)))
    {
      found     = i;
      min_sched = s_watchdogDesc[i].AbsTimeout;
    }
  }
  
  if(found!=-1)
  {
    program_timer(min_sched);
  }
} /* end of cancelTimeout() function */


#pragma interface
Iresource_manager_api_inform_resource_status cb3;

/*****************************************************************************/
/**
 * \brief  signalInterrupt 
 * Function called when timeout expires
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(signalInterrupt)(t_uint16 interruptSource)
{
  t_sint16 found = -1;
  t_uint16 i=0;

  while(i<RESOURCE_MAX)
  {
    if(s_watchdogDesc[i].AbsTimeout==timer_scheduled)
    {
      // call back of the interface delegation
    	cb3=*(s_watchdogDesc[i].callback);
	    cb3.informResourceStatus(STA_RESOURCE_LOST, i);	
    }
    i++;
  }
}

void METH(signalBuffer)(t_t1xhv_buffer_status bufferStatus) {}
