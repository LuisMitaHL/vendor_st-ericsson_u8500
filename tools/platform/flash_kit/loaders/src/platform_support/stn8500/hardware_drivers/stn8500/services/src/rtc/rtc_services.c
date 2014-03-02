/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  This module provides services for RTC initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"

#include "rtc.h"
#include "gic.h"

#include "services.h"
#include "rtc_services.h"

/*--------------------------------------------------------------------------*
 * Global Variables					                   						*
 *--------------------------------------------------------------------------*/
t_callback      g_callback_rtc = { 0, 0 };

t_ser_rtt_context   g_ser_rtt_context;

/*#if defined(SER_RTT)*/
t_callback_isr  RTC_InterruptHandler = 0;
/*#endif*/

/*--------------------------------------------------------------------------*
 * Constants and new types				                  					*
 *--------------------------------------------------------------------------*/

#define RTC_LINE    GIC_RTC_RTT_LINE

/*--------------------------------------------------------------------------*
 * Private functions					                    				*
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_RTC_InterruptHandler                                   	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for RTC			     	*/
/* 																            */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
void SER_RTC_InterruptHandler(unsigned int irq)
{
    t_rtc_IRQSrc    rtcIrqSrc = RTC_IRQ0;
    t_rtc_IRQStatus rtcIrqStatus;
    t_rtc_event     rtcEvent;
    t_rtc_alarm_id  rtcRecAlarms = {0};
    t_rtc_alarm_id  rtcNonRecAlarms = {0};
    t_ser_rtc_param rtc_param;

    RTC_GetIRQSrcStatus(rtcIrqSrc, &rtcIrqStatus);

    rtc_param.FilterIrqError = RTC_FilterProcessIRQSrc(&rtcIrqStatus, &rtcEvent, &rtcRecAlarms, &rtcNonRecAlarms);
    rtc_param.rtcIrqSrc = rtcIrqSrc;
    rtc_param.rtcIrqStatus = rtcIrqStatus;
    rtc_param.rtcEvent = rtcEvent;
    rtc_param.rtcRecAlarms = rtcRecAlarms;
    rtc_param.rtcNonRecAlarms = rtcNonRecAlarms;
    
    if (g_callback_rtc.fct != 0)
    {
        g_callback_rtc.fct(g_callback_rtc.param, &rtc_param);
    }
    

  }

/****************************************************************************/
/* NAME:    SER_RTC_InstallDefaultHandler                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine installs the default Interrupt Handler	        */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_RTC_InstallDefaultHandler(void)
{
    t_gic_error gic_error = GIC_OK;

/*#if !defined(SER_RTT)
    t_uint32    old_datum;
#endif*/

    gic_error = GIC_DisableItLine(RTC_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Line Disabling error - %d\n", gic_error);
        return;
    }

/* #if defined(SER_RTT) */
    RTC_InterruptHandler = SER_RTC_InterruptHandler;
/*#else
    gic_error = GIC_ChangeDatum(RTC_LINE, (t_uint32) SER_RTC_InterruptHandler, &old_datum);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
        return;
    }
#endif*/
    gic_error = GIC_EnableItLine(RTC_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Line Enabling error - %d\n", gic_error);
        return;
    }
}

/****************************************************************************/
/* NAME:    SER_RTC_InstallInterruptHandler                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine installs the User Defined Interrupt Handler	*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_RTC_InstallInterruptHandler(t_callback_isr fct)
{
    t_gic_error gic_error = GIC_OK;

/*#if !defined(SER_RTT)
    t_uint32    old_datum;
#endif*/

    gic_error = GIC_DisableItLine(RTC_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Line Disabling error - %d\n", gic_error);
        return;
    }

/*#if defined(SER_RTT) */
    RTC_InterruptHandler = fct;
/*#else
    gic_error = GIC_ChangeDatum(RTC_LINE, (t_uint32) fct, &old_datum);
    if (gic_error != GCI_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
        return;
    }
#endif*/
    gic_error = GIC_EnableItLine(RTC_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Line Enabling error - %d\n", gic_error);
        return;
    }
}

/****************************************************************************/
/* NAME:    SER_RTC_ReturnIntHandler                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: TBD                                                         */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC t_callback_isr SER_RTC_ReturnIntHandler(void)
{
    return(RTC_InterruptHandler);
}

/*--------------------------------------------------------------------------*
 * Private functions					                    				*
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_RTT_IntHandler                                      	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for RTT			     	*/
/* 																            */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void SER_RTT_InterruptHandler(unsigned int irq)
{
    t_rtt_irq_src_id    irq_src;

    t_ser_rtt_param     rtt_param;

    t_gic_error         gic_err = GIC_OK;

    /* Disable GIC RTC line */
    gic_err = GIC_DisableItLine(RTC_LINE);
    if (GIC_OK != gic_err)
    {
        PRINT("Error ser_rtc_InterruptHandler:GIC_DisableItLine() :%d", gic_err);
    }

    /* Acknowledge GIC RTC line 
    gic_err = GIC_AcknowledgeItLine(RTC_LINE);
    if (GIC_OK != gic_err)
    {
        PRINT("Error ser_rtc_InterruptHandler:GIC_AcknowledgeItLine() :%d", gic_err);
    }*/

    /* Retrieve the IRQ source */
    irq_src = RTT_GetIRQSrc(RTT_DEVICE_ID_0);

    /* Clear the IRQ source */
    RTT_ClearIRQSrc(irq_src);

    /* Disable the IRQ source */
    RTT_DisableIRQSrc(irq_src);

    /* Increment the number of interrupts */
    g_ser_rtt_context.interrupt_count++;

    rtt_param.irq_src = irq_src;

    /* Call back for extended processing */
    if (g_ser_rtt_context.g_callback_rtt.fct != 0)
    {
        g_ser_rtt_context.g_callback_rtt.fct(g_ser_rtt_context.g_callback_rtt.param, &rtt_param);
    }

    /* Enable RTT interrupt */
    RTT_EnableIRQSrc(irq_src);

    /* Enable GIC RTT line */
    gic_err = GIC_EnableItLine(RTC_LINE);
    if (GIC_OK != gic_err)
    {
        PRINT("Error ser_rtt_InterruptHandler:GIC_EnableItLine() :%d", gic_err);
    }
}

/****************************************************************************/
/* NAME:    SER_RTC_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine provides initialization services for RTC		*/
/*                                                                          */
/* PARAMETERS:			                                                    */
/*                                                                          */
/* RETURN: void                                                             */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_RTC_Init(t_uint8 dummy)
{
    /* should be done after gic_init */
    t_gic_error gic_error;
    t_gic_func_ptr   old_datum;
    t_rtc_error rtc_err;
    
   /* RTC_Reset(); */
    
/*#if defined(SER_RTT)*/
     gic_error = GIC_ChangeDatum(GIC_RTC_RTT_LINE, (t_gic_func_ptr)SER_RTT_RTC_DispachIrq, &old_datum);

    if (gic_error != GIC_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
        return;
    }

    g_ser_rtt_context.p_rtt_interrupt_handler = SER_RTT_InterruptHandler;
    
    RTC_InterruptHandler = SER_RTC_InterruptHandler;
/*#else
    gic_error = GIC_ChangeDatum(RTC_LINE, (t_uint32) SER_RTC_InterruptHandler, &old_datum);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
        return;
    }
#endif*/
    gic_error = GIC_EnableItLine(RTC_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Line enabling error - %d\n", gic_error);
        return;
    }

    RTC_SetBaseAddress(RTC_REG_BASE_ADDR);
    
    rtc_err = RTC_Init(RTC_REG_BASE_ADDR);
    if (RTC_OK != rtc_err)
    {
        PRINT("Error SER_RTC_Init:RTC_Init() :%d", rtc_err);
    }

    /* Initialize global context */
    g_ser_rtt_context.interrupt_count = 0;

    g_ser_rtt_context.g_callback_rtt.fct = 0;

    g_ser_rtt_context.g_callback_rtt.param = 0;    
    
}

/************************************************************************************/
/* NAME:    SER_RTC_RegisterCallback                                          		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                          		*/
/* PARAMETERS:  t_callback_fct fct : Address of the call back                 		*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                   		*/

/************************************************************************************/
PUBLIC void SER_RTC_RegisterCallback(t_callback_fct fct, void *param)
{
    g_callback_rtc.fct = fct;
    g_callback_rtc.param = param;
}

/****************************************************************************/
/* NAME:    SER_RTC_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops services for RTC							*/
/* WARNING : NEED TO BE AVOIDED . CAN BE DONE FOR TESTING					*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_RTC_Close(void)
{
    /* TBD */
}


/****************************************************************************/
/* NAME:    SER_RTT_ReturnIntHandler                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns the pointer to RTT interrupt handler to be used in  */
/*				Dispatch IRQ Handler for RTC-RTT interrupt	                */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: t_callback_isr                                                   */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC t_callback_isr SER_RTT_ReturnIntHandler(void)
{
    return(g_ser_rtt_context.p_rtt_interrupt_handler);
}

/************************************************************************************/
/* NAME:    SER_RTT_WaitOnInt	                                            		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine polls on the passed interrupt source					*/
/*                                                                          		*/
/* PARAMETERS: t_rtt_irq_src_id: Interrupt Sources ored                            	*/
/*                                                                          		*/
/* RETURN: void													             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           		*/

/************************************************************************************/
PUBLIC void SER_RTT_WaitOnInt(IN t_rtt_irq_src_id irq_src, IN t_uint32 count)
{
    if (irq_src == RTT_IRQ_SRC_ID_0)
    {
        while (g_ser_rtt_context.interrupt_count < count)
        {
            ;
        }

        g_ser_rtt_context.interrupt_count = 0;
    }
}

/****************************************************************************/
/* NAME:    SER_RTT_RTC_DispachIrq                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine dispacthes the RTC/RTT ISR	                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/*                                                                          */
/* RETURN: NONE                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC void SER_RTT_RTC_DispachIrq(unsigned int irq)
{
    /* Higher priority is given to RTC Interrupt */
    t_rtc_IRQSrc        rtc_irq_src;
    t_rtt_irq_src_id    rtt_irq_src;

    t_callback_isr      isr = 0;

    rtc_irq_src = RTC_GetIRQSrc(RTC_DEVICE0);
    if (RTC_IRQ0 == rtc_irq_src)
    {
        /* RTC Int. Handler */
        isr = SER_RTC_ReturnIntHandler();
        isr(irq);
    }

    rtt_irq_src = RTT_GetIRQSrc(RTT_DEVICE_ID_0);
    if (RTT_IRQ_SRC_ID_0 == rtt_irq_src)
    {
        /* RTT Int. Handler */
        isr = SER_RTT_ReturnIntHandler();
        isr(irq);
    }

}


