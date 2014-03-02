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

#ifndef _HCL_RTC_HWP_H_
#define _HCL_RTC_HWP_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "rtc_irq.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

#define RTCPERIPHID0    0x31
#define RTCPERIPHID1    0x00 
#define RTCPERIPHID2    0x28 
#define RTCPERIPHID3    0x00
#define RTCPCELLID0     0x0D
#define RTCPCELLID1     0xF0
#define RTCPCELLID2     0x05
#define RTCPCELLID3     0xB1


/* Control register bitfields  */
#define RTC_CONTROL_ENABLEwidth 1

#define RTC_CONTROL_ENABLEshift 26

/* Interrupt mask register  */
#define RTC_IMASK_MASKwidth 1
#define RTC_IMASK_MASKshift 0

/* Raw interrupt status register  */
#define RTC_RAWINT_RISwidth 1
#define RTC_RAWINT_RISshift 0

/* Masked interrupt status register */
#define RTC_MASKINT_MISwidth    1
#define RTC_MASKINT_MISshift    0

/* Interrupt clear register */
#define RTC_INTCLR_ICRwidth 1
#define RTC_INTCLR_ICRshift 0

/* Trim Control register mask */
#define MASK_CLKDIV MASK_HALFWORD0
#define MASK_CLKDEL (0x3FFUL << 16)
#define MASK_TRIM   (0x3FFFFFFUL)

#define MASK_CWMONTH (0xFUL << 25)
#define MASK_CWDAYM  (0x1FUL << 20)
#define MASK_CWDAYW  (0x7UL << 17)
#define MASK_CWHOUR  (0x1FUL << 12)
#define MASK_WMIN    (0x3FUL << 6)
#define MASK_CWSEC   (0x3FUL)
#define MASK_CWYEAR  (0x3FFFUL)

/* Generic macros to access bit fields */
#define HCL_FIELDMASK(fieldId) \
        ((t_uint32) ((t_uint32) ((t_uint32) (~0) >> (32 - (fieldId##width))) << (t_uint32) (fieldId##shift)))
#define HCL_FIELDSET(datum, fieldId, val) \
    (datum) = ((t_uint32) (datum) &~HCL_FIELDMASK(fieldId)) | ((t_uint32) ((t_uint32) (val) << (t_uint32) (fieldId##shift)) & (HCL_FIELDMASK(fieldId)))
#define HCL_FIELDGET(datum, fieldId)    ((t_uint32) (((datum) & ((t_uint32) HCL_FIELDMASK(fieldId))) >> (fieldId##shift)))


/* HW access macros */
#define GET_RTC_COUNTER(c)      ((c) = (g_rtc_system_context.p_rtc_reg)->RTCDR)
#define RTC_ENABLE              HCL_FIELDSET((g_rtc_system_context.p_rtc_reg)->RTCCR, RTC_CONTROL_ENABLE, 1)
#define GET_RTC_MATCH(m)        ((m) = (g_rtc_system_context.p_rtc_reg)->RTCMR)
#define SET_RTC_MATCH(m)        ((g_rtc_system_context.p_rtc_reg)->RTCMR = (m))
#define SET_RTC_DATE(d)         ((g_rtc_system_context.p_rtc_reg)->RTCLR = (d))
#define MASK_RTC_INTS           HCL_FIELDSET((g_rtc_system_context.p_rtc_reg)->RTCIMSC, RTC_IMASK_MASK, 0)
#define UNMASK_RTC_INTS         HCL_FIELDSET((g_rtc_system_context.p_rtc_reg)->RTCIMSC, RTC_IMASK_MASK, 1)
#define RESTORE_RTC_INTS(m)     HCL_FIELDSET((g_rtc_system_context.p_rtc_reg)->RTCIMSC, RTC_IMASK_MASK, (m))
#define CLEAR_RTC_INTS          HCL_FIELDSET((g_rtc_system_context.p_rtc_reg)->RTCICR, RTC_INTCLR_ICR, 1)
#define RTC_RAW_INT_STATUS      (HCL_FIELDGET((g_rtc_system_context.p_rtc_reg)->RTCRIS, RTC_RAWINT_RIS) == 1 ? TRUE : FALSE)
#define GET_RTC_INT_STATUS(m)   ((m) = (g_rtc_system_context.p_rtc_reg)->RTCMIS)
#define GET_RTC_IMSC_STATUS(m)  ((m) = (g_rtc_system_context.p_rtc_reg)->RTCIMSC)
#define WRITE_TCR_CLKDIV(m)     HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, m, MASK_CLKDIV)
#define READ_TCR_CLKDIV(m)      (m = HCL_READ_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, MASK_CLKDIV))
#define WRITE_TCR_CLKDEL(m)     (HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, ((m) << 16), MASK_CLKDEL))
#define READ_TCR_CLKDEL(m)      (m = (HCL_READ_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, MASK_CLKDEL) >> 16))
#define SET_TCR_SEC             HCL_SET_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, MASK_BIT27)
#define CLEAR_TCR_SEC           HCL_CLEAR_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, MASK_BIT27)
#define READ_TCR_SEC(m)         (m = HCL_READ_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, MASK_BIT27) >> 27)
#define WRITE_TCR_TRIM(i, f, m) \
    (m) = ((f << 16) | i); \
    (HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCCR, m, MASK_TRIM))
    
#define bcdtoi(bcd)        ( (bcd&0xF) + ((bcd>>4)&0xF)*10 + ((bcd>>8)&0xF)*100 + ((bcd>>12)&0xF)*1000 )
/* bcdtoi is 0x2010 -> 2010 */
#define itobcd(chr)       ( (((chr%10000)/1000)<<12) + (((chr%1000)/100)<<8) + (((chr%100)/10)<<4) + (chr%10) )
/* itobcd is 2010 -> 0x2010 */

/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
#define SAVE_AND_DISABLE_INTERRUPTS(contents) \
    GET_RTC_IMSC_STATUS(contents); \
    MASK_RTC_INTS;

#define RESTORE_INTERRUPTS(contents)    RESTORE_RTC_INTS(contents);


/* RTC registers */

typedef volatile struct
{
    t_uint32    RTCDR;          /* @0x00 */
    t_uint32    RTCMR;          /* @0x04 */
    t_uint32    RTCLR;          /* @0x08 */
    t_uint32    RTCCR;          /* @0x0C */
    t_uint32    RTCIMSC;        /* @0x10 */
    t_uint32    RTCRIS;         /* @0x14 */
    t_uint32    RTCMIS;         /* @0x18 */
    t_uint32    RTCICR;         /* @0x1C */
    
    t_uint32    RTTDR;          /* @0x20 */
    t_uint32    RTTLR1;         /* @0x24 */
    t_uint32    RTTCR;          /* @0x28 */
	
	t_uint32    RTTLR2;         /* @0x2C */

	
    /*t_uint32    unused_0[(0x30 - 0x2C) >> 2]; */
    t_uint32    RTCYR;          /* @0x30 */
    t_uint32    RTCYMR;         /* @0x34 */
    t_uint32    RTCYLR;         /* @0x38 */
    
    /*t_uint32    unused_1[(0x80 - 0x3C) >> 2]; */
    
    t_uint32    RTTPR1;         /* @0x3C */
    t_uint32    RTTPR2;         /* @0x40 */
    t_uint32    RTTPR3;         /* @0x44 */
    t_uint32    RTTPR4;         /* @0x48 */

    t_uint32    RTTIN;          /* @0x4C */
    t_uint32    RTTWK;          /* @0x50 */
    t_uint32    RTTSMIN;        /* @0x54 */
    
   t_uint32    unused_1[(0x80 - 0x58) >> 2];
    
    t_uint32    RTCITCR;        /* @0x80 */
    t_uint32    RTCITIP;        /* @0x84 */
    t_uint32    RTCITOP;        /* @0x88 */
    
    t_uint32    unused_2[(0xFE0 - 0x08C) >> 2];

    t_uint32    RTCPeriphID0;   /* @4064 */
    t_uint32    RTCPeriphID1;   /* @4068 */
    t_uint32    RTCPeriphID2;   /* @4072 */
    t_uint32    RTCPeriphID3;   /* @4076 */
    t_uint32    RTCPCellID0;    /* @4080 */
    t_uint32    RTCPCellID1;    /* @4084 */
    t_uint32    RTCPCellID2;    /* @4088 */
    t_uint32    RTCPCellID3;    /* @4092 */
} t_rtc_register;

#endif /* _HCL_RTC_HWP_H_ */

