/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  Private Header file of RTC Real Time Clock module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HCL_RTCP_H_
#define _HCL_RTCP_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#ifndef _HCL_RTC_H_
#include "rtc.h"
#endif
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#include "rtc_irqp.h"

/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/
/* Alarm Id type */
typedef t_uint32        t_rtc_id;

typedef volatile struct struct_t_rtc_rec_alarm
{
    t_bool          inUse;
    t_uint8         reserved[3];
    t_rtc_counter   expTime;
    t_rtc_counter   recTime;
    t_rtc_date      expDate; // added 
} t_rtc_rec_alarm;

typedef volatile struct struct_t_rtc_non_rec_alarm
{
    t_bool          inUse;
    t_uint8         reserved[3];
    t_rtc_counter   expTime;
    t_rtc_date      expDate; //added
} t_rtc_non_rec_alarm;

typedef enum
{
    NO_LEAP,
    LEAP
} IS_LEAP;

typedef struct
{
    /* Pointer to RTC registers structure */
    t_rtc_register     *p_rtc_reg;

    /* Alarm management */
    t_rtc_rec_alarm     rtc_rec_alarm_list[RTC_MAX_REC_ALARMS];
    t_rtc_non_rec_alarm rtc_non_rec_alarm_list[RTC_MAX_NON_REC_ALARMS];
    t_uint8             rtc_nrec_alarms_inserted;
    t_uint8             rtc_nnon_rec_alarms_inserted;
    t_rtc_event         rtc_active_events;
} t_rtc_system_context;

/*------------------------------------------------------------------------
 * Defines for RTC Clockwatch
 *----------------------------------------------------------------------*/
/* Date related defines */
#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * HOUR_PER_DAY)
#define SECS_PER_MONTH  (SECS_PER_DAY * MAX_DAY)
#define SECS_PER_MIN    60
#define HOUR_PER_DAY    24
#define MIN_PER_HOUR    60

#define BASE_YEAR       2000 

#define BASIC_DAYS_YEAR 365

#define MIN_MIN         0
#define MAX_MIN         59

#define MIN_SEC         0
#define MAX_SEC         59

#define MIN_HOUR        0
#define MAX_HOUR        23

#define MIN_DAY         1
#define MAX_DAY         31

#define MIN_MONTH       1
#define MAX_MONTH       12

#define MIN_YEAR        1

#define MAX_YEAR        1999

#define FIRSTDAY        1

#define MAX_VALID_SEC   0xffcd8bff  /* 31/dec/2134 23h 59min 59sec */

/*------------------------------------------------------------------------
 * Defines for RTC Timer
 *----------------------------------------------------------------------*/
#define RTT_CLK32   0x1F7AUL

/* HW access macros */
#define GET_RTT_COUNTER(c)      ((c) = g_rtc_system_context.p_rtc_reg->RTTDR)
#define SET_RTT_MODE_ONESHOT    HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT0)
#define SET_RTT_MODE_PERIODIC   HCL_CLEAR_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT0)
#define ENABLE_RTT              HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT1)
#define DISABLE_RTT             HCL_CLEAR_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT1)
#define MASK_RTT_INTS           HCL_CLEAR_BITS(g_rtc_system_context.p_rtc_reg->RTCIMSC, MASK_BIT1)
#define UNMASK_RTT_INTS         HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTCIMSC, MASK_BIT1)
#define CLEAR_RTT_INTS          HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT1)
#define GET_RTT_INT_STATUS(m)   ((m) = g_rtc_system_context.p_rtc_reg->RTCMIS >> 1)
#define READ_RTT_ENABLE_BIT(f)  ((f) = HCL_READ_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT1) >> 1)
#define WRITE_RTT_ENABLE_BIT	 HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT1);
#define DISABLE_RTT_SELF_START   HCL_CLEAR_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, MASK_BIT2);






/* NULL COUNTER VALUE */
#define RESET   0


/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
PRIVATE void            rtcAlarm_Init(void);
PRIVATE t_rtc_error     rtcAlarm_Insert(t_rtc_counter alarm_time, t_rtc_counter rec_time, t_rtc_date ,t_rtc_alarm_id *rtc_alarm_id);
PRIVATE t_rtc_error     rtcAlarm_Delete(t_rtc_alarm_id rtc_alarm_id);

PRIVATE t_rtc_error     rtcAlarm_ChangeDate(t_rtc_counter new_time ,t_rtc_date date);

PRIVATE void            safeMatchUpdate(t_rtc_counter new_match_value);
PRIVATE void            safeMatchUpdateCond(t_rtc_counter new_match_value, t_uint32 alarms_before);
PRIVATE t_rtc_counter   getNewMatch(void);
PRIVATE void            disableRtcInts(void);

PRIVATE t_rtc_error     date_to_sec(t_rtc_date rtc_dDate, t_rtc_counter *seconds);
PRIVATE t_rtc_error     sec_to_date(t_rtc_date *rtc_date, t_rtc_counter seconds);
PRIVATE IS_LEAP         isleap(t_uint32 year);
PRIVATE void            month_and_day(t_uint32 days_in_year, IS_LEAP leap, t_uint32 *month, t_uint32 *day);
PRIVATE t_rtc_error     day_to_sec(t_rtc_date rtc_date, t_rtc_counter *seconds);

PRIVATE t_rtc_error RTC_GetMatchDateAndTime(OUT t_rtc_date *alarm_date);
PRIVATE t_rtc_error RTC_GetCountDateAndTime(OUT t_rtc_date *alarm_date);
/*PRIVATE void  RTC_LoadDateAndTime(IN t_rtc_date);*/
PRIVATE void RTC_LoadMatchDateAndTime(IN t_rtc_date);

#endif /* _HCL_RTCP_H_ */

