/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  Public header file for the RTC Real Time Clock module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_RTC_IRQ_H
#define __INC_RTC_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "rtc.h"

/* Maximum number of alarms that can be set simultaneously */
typedef t_uint8 t_rtc_IRQStatus;

/*------------------------------------------------------------------------
 * Structures and enums for RTC Clockwatch 
 *----------------------------------------------------------------------*/
/* Interrupt related enum */
/* Enum to check which interrupt is asserted */
typedef enum
{
    RTC_IRQ0    = 0x01,
    RTC_IRQ_NONE= 0x0
} t_rtc_IRQSrc;

typedef enum
{
    RTC_DEVICE0
} t_rtc_device;

/*------------------------------------------------------------------------
 * Type definations for RTC Timer
 *----------------------------------------------------------------------*/
typedef enum
{
    RTT_IRQ_SRC_NONE= 0x0,
    RTT_IRQ_SRC_ID_0= 0x1
} t_rtt_irq_src_id;

typedef enum
{
    RTT_DEVICE_ID_0
} t_rtt_device_id;


/*------------------------------------------------------------------------
 * Functions declaration for RTT Clockwatch
 *----------------------------------------------------------------------*/
PUBLIC void            RTC_SetBaseAddress(t_logical_address rtc_base_address);
PUBLIC void            RTC_EnableIRQSrc(t_rtc_IRQSrc irq_src);
PUBLIC void            RTC_DisableIRQSrc(t_rtc_IRQSrc irq_src);
PUBLIC t_rtc_IRQSrc    RTC_GetIRQSrc(t_rtc_device device);
PUBLIC void            RTC_ClearIRQSrc(t_rtc_IRQSrc irq_src);
PUBLIC t_bool          RTC_IsPendingIRQSrc(t_rtc_IRQSrc irq_src);
PUBLIC void            RTC_GetIRQSrcStatus(t_rtc_IRQSrc irq_src, t_rtc_IRQStatus *p_irq_status);
PUBLIC t_rtc_error	   RTC_FilterProcessIRQSrc
							(
								IN t_rtc_IRQStatus  *p_irq_status,
								IN t_rtc_event      *p_event,
								IN t_rtc_alarm_id   *p_rec_alarm,
							    IN t_rtc_alarm_id   *p_non_rec_alarm
							);

/*------------------------------------------------------------------------
 * Functions declaration for RTC Timer
 *----------------------------------------------------------------------*/

PUBLIC void                RTT_EnableIRQSrc(IN t_rtt_irq_src_id irqsrc);
PUBLIC void                RTT_DisableIRQSrc(IN t_rtt_irq_src_id irqsrc);
PUBLIC t_rtt_irq_src_id    RTT_GetIRQSrc(IN t_rtt_device_id device);
PUBLIC void                RTT_ClearIRQSrc(IN t_rtt_irq_src_id irqsrc);
PUBLIC t_bool              RTT_IsPendingIRQSrc(IN t_rtt_irq_src_id irqsrc);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_RTC_IRQ_H */

/* End of file - rtc_irq.h */

