/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the RTC Real Time Clock module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HCL_RTC_H_
#define _HCL_RTC_H_

#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#ifndef __INC_DBG_H
#include "debug.h"
#endif

/* Maximum number of alarms that can be set simultaneously */
#define RTC_MAX_REC_ALARMS      31
#define RTC_MAX_NON_REC_ALARMS  255

/*
 * Definition of the HCL RTC driver Version numbers
 */
#define RTC_HCL_VERSION_ID    2
#define RTC_HCL_MAJOR_ID      0   /*major modifications*/
#define RTC_HCL_MINOR_ID      6   /*minor modifications*/

/*******************************************************************************
 *						                                                       *
 * RTC Clockwatch Structures,Macros and Enums                                  *
 *                                                                             * 
 *******************************************************************************/

/* Counter register type */
typedef t_uint32    t_rtc_counter;

typedef struct
{
	t_uint32 rtt_counter1;
	t_uint32 rtt_counter2;
} t_rtt_counter;

typedef struct
{
	t_uint32 num_useful_pattern;
	t_uint32 pattern_val1;
	t_uint32 pattern_val2;
	t_uint32 pattern_val3;
	t_uint32 pattern_val4;
	
}t_rtt_pattern;


typedef enum
{
    RTC_NON_RECURSIVE_ALARM = 0,
    RTC_RECURSIVE_ALARM     = 1
} t_rtc_alarm_type;

typedef struct
{
    t_rtc_alarm_type    type;
    t_uint8             res[3];
    t_uint32            id;             /* alarm ID */
} t_rtc_alarm_id;

typedef struct
{
    t_uint8     second;                 /* second (from 0 to 59)           							*/
    t_uint8     minute;                 /* minute (from 0 to 59)           							*/
    t_uint8     hour;                   /* hour in day (from 0 to 23)      							*/
    t_uint8     day_of_week;
    t_uint16    day;                    /* day in month (from 1 to 31) (extended for recursive alarms) */
    t_uint8     month;                  /* month (from 1 to 12)            							*/
    t_uint16     year;                   /* year from 2000 (from 01 to 1999) 							*/
    /* Modified to 2000 */
} t_rtc_date;

typedef enum
{
    RTC_RECURSIVE_ALARM0    = 0x00000001,
    RTC_RECURSIVE_ALARM1    = 0x00000002,
    RTC_RECURSIVE_ALARM2    = 0x00000004,
    RTC_RECURSIVE_ALARM3    = 0x00000008,
    RTC_RECURSIVE_ALARM4    = 0x00000010,
    RTC_RECURSIVE_ALARM5    = 0x00000020,
    RTC_RECURSIVE_ALARM6    = 0x00000040,
    RTC_RECURSIVE_ALARM7    = 0x00000080,
    RTC_RECURSIVE_ALARM8    = 0x00000100,
    RTC_RECURSIVE_ALARM9    = 0x00000200,
    RTC_RECURSIVE_ALARM10   = 0x00000400,
    RTC_RECURSIVE_ALARM11   = 0x00000800,
    RTC_RECURSIVE_ALARM12   = 0x00001000,
    RTC_RECURSIVE_ALARM13   = 0x00002000,
    RTC_RECURSIVE_ALARM14   = 0x00004000,
    RTC_RECURSIVE_ALARM15   = 0x00008000,
    RTC_RECURSIVE_ALARM16   = 0x00010000,
    RTC_RECURSIVE_ALARM17   = 0x00020000,
    RTC_RECURSIVE_ALARM18   = 0x00040000,
    RTC_RECURSIVE_ALARM19   = 0x00080000,
    RTC_RECURSIVE_ALARM20   = 0x00100000,
    RTC_RECURSIVE_ALARM21   = 0x00200000,
    RTC_RECURSIVE_ALARM22   = 0x00400000,
    RTC_RECURSIVE_ALARM23   = 0x00800000,
    RTC_RECURSIVE_ALARM24   = 0x01000000,
    RTC_RECURSIVE_ALARM25   = 0x02000000,
    RTC_RECURSIVE_ALARM26   = 0x04000000,
    RTC_RECURSIVE_ALARM27   = 0x08000000,
    RTC_RECURSIVE_ALARM28   = 0x10000000,
    RTC_RECURSIVE_ALARM29   = 0x20000000,
    RTC_RECURSIVE_ALARM30   = 0x40000000
} t_rtc_recursive_alarm_id;

typedef enum
{
    RTC_NO_PENDING_EVENT_ERROR                  = HCL_NO_PENDING_EVENT_ERROR,
    RTC_NO_MORE_FILTER_PENDING_EVENT            = HCL_NO_MORE_FILTER_PENDING_EVENT,
    RTC_NO_MORE_PENDING_EVENT                   = HCL_NO_MORE_PENDING_EVENT,
    RTC_REMAINING_FILTER_PENDING_EVENTS         = HCL_REMAINING_FILTER_PENDING_EVENTS,
    RTC_REMAINING_PENDING_EVENTS                = HCL_REMAINING_PENDING_EVENTS,
    RTC_INTERNAL_EVENT                          = HCL_INTERNAL_EVENT,
    RTC_OK                                      = HCL_OK,
        
    RTC_UNSUPPORTED_HW							= HCL_UNSUPPORTED_HW, 
    RTC_UNSUPPORTED_FEATURE						= HCL_UNSUPPORTED_FEATURE,
    RTC_INVALID_PARAMETER						= HCL_INVALID_PARAMETER,
    RTC_REQUEST_NOT_APPLICABLE					= HCL_REQUEST_NOT_APPLICABLE,
    RTC_REQUEST_PENDING							= HCL_REQUEST_PENDING,
    RTC_NOT_CONFIGURED      					= HCL_NOT_CONFIGURED,
    RTC_INTERNAL_ERROR							= HCL_INTERNAL_ERROR,
    
    RTC_ALARM_IN_USE                            = -9,	
    RTC_ERROR_NO_MORE_ALARM                     = -10,	
    RTC_ERROR_INVALID_DATE                      = -11,	
    RTC_ERROR_INVALID_ALARM                     = -12,	
    RTC_ERROR_FATAL                             = -13,	
    RTC_WARNING_PENDING_ALARM_REMOVED           = -14,	
    RTC_ERROR_PERIPHERAL_IDENTIFICATION_FAILED  = -15,	
    RTC_ERROR_WRITE_PROTECTED                   = -16	   
} t_rtc_error;



typedef enum
{
    RTC_NO_EVENT                                = 0,
    RTC_NON_RECURSIVE_ALARM_EVENT,
    RTC_RECURSIVE_ALARM_EVENT,
    RTC_MULTIPLE_ALARM_EVENT
} t_rtc_event;


/*******************************************************************************
 *						                                                       *
 * RTC Timer Structures,Macros and Enums                                       *
 *                                                                             * 
 *******************************************************************************/
 typedef t_uint32    t_rtt_count;
 
 typedef enum
{
    RTT_MODE_PERIODIC   = 0,
    RTT_MODE_ONE_SHOT   = 1
} t_rtt_mode;

typedef enum
{
    RTT_STATE_RUNNING   = 0,
    RTT_STATE_STOPPED
} t_rtt_timer_state;

typedef enum
{
    RTT_NO_PENDING_EVENT_ERROR          = HCL_NO_PENDING_EVENT_ERROR,
    RTT_NO_MORE_FILTER_PENDING_EVENT    = HCL_NO_MORE_FILTER_PENDING_EVENT,
    RTT_NO_MORE_PENDING_EVENT           = HCL_NO_MORE_PENDING_EVENT,
    RTT_REMAINING_FILTER_PENDING_EVENTS = HCL_REMAINING_FILTER_PENDING_EVENTS,
    RTT_REMAINING_PENDING_EVENTS        = HCL_REMAINING_PENDING_EVENTS,
    RTT_INTERNAL_EVENT                  = HCL_INTERNAL_EVENT,
    RTT_OK                              = HCL_OK,
    RTT_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,
    RTT_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,
    RTT_UNSUPPORTED_FEATURE				= HCL_UNSUPPORTED_FEATURE, 
    RTT_REQUEST_NOT_APPLICABLE			= HCL_REQUEST_NOT_APPLICABLE,
    RTT_REQUEST_PENDING     			= HCL_REQUEST_PENDING, 
    RTT_NOT_CONFIGURED      			= HCL_NOT_CONFIGURED, 
    RTT_INTERNAL_ERROR      			= HCL_INTERNAL_ERROR, 
    RTT_ERROR_TIMER_ALREADY_STOPPED     = -9,
    RTT_ERROR_TIMER_ALREADY_STARTED     = -10
} t_rtt_error;
 
/*------------------------------------------------------------------------
 * RTC Clockwatch Functions
 *----------------------------------------------------------------------*/
PUBLIC t_rtc_error RTC_Init(IN t_logical_address rtc_base_address);
PUBLIC t_rtc_error RTC_SetTrim(IN t_uint32 rtc_frequency_integer,IN t_uint32 rtc_frequency_fraction,IN t_uint8 rtc_decimal_point);
PUBLIC t_rtc_error RTC_GetTrim(OUT t_uint32 *p_rtc_frequency_integer, OUT t_uint32 *p_rtc_frequency_fraction);
PUBLIC t_rtc_error RTC_SetSecureMode(IN t_bool rtc_secure);
PUBLIC t_rtc_error RTC_GetVersion(OUT t_version *p_version);
PUBLIC t_rtc_error RTC_SetDbgLevel(IN t_dbg_level debug_level);
PUBLIC t_rtc_error RTC_GetDbgLevel(OUT t_dbg_level *debug_level);
PUBLIC t_rtc_error RTC_SetPermanentDateAndTime(IN t_rtc_date rtc_date);
PUBLIC t_rtc_error RTC_GetPermanentDateAndTime(OUT t_rtc_date *rtc_date);
PUBLIC t_rtc_error RTC_SetAlarm(IN t_rtc_date rtc_date,IN t_rtc_date rtc_recurse_period, OUT t_rtc_alarm_id *rtc_alarm_id);
PUBLIC t_rtc_error RTC_ClearAlarm(IN t_rtc_alarm_id rtc_alarm_id);
PUBLIC t_bool      RTC_IsEventActive(IN t_rtc_event *p_event);
PUBLIC void        RTC_AcknowledgeEvent(IN t_rtc_event *p_event);
PUBLIC t_rtc_error RTC_Reset(void);

/*------------------------------------------------------------------------
 * RTC Timer Functions
 *----------------------------------------------------------------------*/
/* PUBLIC t_rtt_error RTT_StartTimer(IN t_rtt_mode rtt_mode, IN t_rtt_count rtt_counter_value); */
PUBLIC t_rtt_error RTT_StartTimer(IN t_rtt_mode rtt_mode, const IN t_rtt_counter rtt_counter, const IN t_rtt_pattern rtt_pattern);
PUBLIC t_rtt_error RTT_RestartTimer(void);
PUBLIC t_rtt_error RTT_GetCounterValue(OUT t_rtt_count *p_rtt_counter_value);
PUBLIC t_rtt_error RTT_StopTimer(void);
PUBLIC t_rtt_error RTT_CheckTimerState(OUT t_rtt_timer_state *p_rtt_state);

#endif /* #ifndef _HCL_RTC_H_ */
