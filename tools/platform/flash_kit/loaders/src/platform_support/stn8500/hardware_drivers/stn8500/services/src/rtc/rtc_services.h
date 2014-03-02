/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  Header file for RTC services
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _SERVICES_RTC_
#define _SERVICES_RTC_

#include "rtc.h"
#include "rtc_irq.h"
#include "services.h"

typedef void (*t_callback_isr) (unsigned int);

typedef struct
{
    t_rtc_IRQSrc    rtcIrqSrc;
    t_rtc_IRQStatus rtcIrqStatus;
    t_rtc_event     rtcEvent;
    t_rtc_alarm_id  rtcRecAlarms;
    t_rtc_alarm_id  rtcNonRecAlarms;
    t_rtc_error     FilterIrqError;
}t_ser_rtc_param, *t_p_ser_rtc_param;

typedef struct
{
    volatile t_uint32   interrupt_count;
    t_callback          g_callback_rtt;
    t_callback_isr      p_rtt_interrupt_handler;
} t_ser_rtt_context;


typedef struct
{
    t_rtt_irq_src_id    irq_src;
} t_ser_rtt_param, *t_p_ser_rtt_param;


PUBLIC void SER_RTC_Init(t_uint8);
PUBLIC void SER_RTC_Close(void);
PUBLIC void SER_RTC_RegisterCallback(t_callback_fct, void *);
PUBLIC void SER_RTC_InstallDefaultHandler(void);

PUBLIC t_callback_isr SER_RTT_ReturnIntHandler(void);
PUBLIC t_callback_isr SER_RTC_ReturnIntHandler(void);
PUBLIC void SER_RTT_RTC_DispachIrq(unsigned int);

PUBLIC void SER_RTC_InstallInterruptHandler(t_callback_isr);

PUBLIC void SER_RTT_WaitOnInt(t_rtt_irq_src_id, t_uint32);
#endif /* END OF FILE*/

