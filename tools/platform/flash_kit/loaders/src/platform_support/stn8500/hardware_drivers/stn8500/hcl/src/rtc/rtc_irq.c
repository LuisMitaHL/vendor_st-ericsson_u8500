/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  This module provides some support routines for the RTC
* \author  ST-Ericsson
*/
/*****************************************************************************/
 
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "rtc_irq.h"
#include "rtc_irqp.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE t_rtc_register  *gp_rtc_reg;

/*For debug HCL */
#ifdef __DEBUG
PRIVATE t_dbg_level     MY_DEBUG_LEVEL_VAR_NAME = (t_dbg_level) 0xFFFFFFFF;
PRIVATE t_dbg_id        MY_DEBUG_ID = RTC_HCL_DBG_ID;
#endif

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_SetBaseAddress(t_logical_address rtc_base_address) */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the RTC registers                   */
/* PARAMETERS:                                                               */
/* IN : t_logical_address RtcBaseAddress: RTC registers base address         */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error                  : RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/

/* WARNING: modify the ID mechanism, basing it only on the part number
 * and in case of mismatch, do not return an error
 */
PUBLIC void RTC_SetBaseAddress(t_logical_address rtc_base_address)
{
    gp_rtc_reg = (t_rtc_register *) rtc_base_address;
}

/*RTC Clockwatch APIs*/
/*****************************************************************************/
/* NAME:      void RTC_EnableIRQSrc(t_rtc_IRQSrc irq_src) 		    	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to enable an interrupt source            */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_IRQSrc irq_src   : to enable the interrupt source	         */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC void RTC_EnableIRQSrc(t_rtc_IRQSrc irq_src)
{
    DBGENTER1("%d", irq_src);
    HCL_FIELDSET(gp_rtc_reg->RTCIMSC, RTC_IMASK_MASK, 1);
    DBGEXIT(RTC_OK);
}

/*****************************************************************************/
/* NAME:      void RTC_DisableIRQSrc(t_rtc_IRQSrc irq_src) 		    	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to disable an interrupt source           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_IRQSrc irq_src   : to disable the interrupt source	         */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC void RTC_DisableIRQSrc(t_rtc_IRQSrc irq_src)
{
    DBGENTER1("%d", irq_src);
    HCL_FIELDSET(gp_rtc_reg->RTCIMSC, RTC_IMASK_MASK, 0);
    DBGEXIT(RTC_OK);
}

/*****************************************************************************/
/* NAME:      t_rtc_IRQSrc RTC_GetIRQSrc(t_rtc_device device)		   	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the interrupting source of the device      */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_device device: The device for which the interrupt source    */
/*		   						is queried 							         */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_IRQSrc :Source for the interrupt	                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtc_IRQSrc RTC_GetIRQSrc(t_rtc_device device)
{
    /* Only to be compliant with HCL interrupt handling directives */
    if (RTC_DEVICE0 == device)
    {
        t_uint32    irq_status;

        /* Read masked interrupt status */
        irq_status = gp_rtc_reg->RTCMIS;

        if (TRUE == (t_bool) irq_status)
        {
            return(RTC_IRQ0);
        }
    }

    return(RTC_IRQ_NONE);
}

/*****************************************************************************/
/* NAME:      void RTC_ClearIRQSrc(t_rtc_IRQSrc irq_src) 		    	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to clear the  interrupt source           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_IRQSrc irq_src   : The interrupt source	to be cleared        */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC void RTC_ClearIRQSrc(t_rtc_IRQSrc irq_src)
{
    DBGENTER1("%d", irq_src);
    HCL_FIELDSET(gp_rtc_reg->RTCICR, RTC_INTCLR_ICR, 1);
    DBGEXIT(RTC_OK);
}

/*****************************************************************************/
/* NAME:      void RTC_IsPendingIRQSrc(t_rtc_IRQSrc irq_src) 	    	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine checks the status of a particular interrupt     */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_IRQSrc irq_src   : the interrupt source				     */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_bool : Status of the interrupt source	                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_bool RTC_IsPendingIRQSrc(t_rtc_IRQSrc irq_src)
{
    t_uint32    irq_status;
    DBGENTER1("%d", irq_src);
    irq_status = gp_rtc_reg->RTCMIS;
    DBGEXIT(RTC_OK);
    return((t_bool) irq_status);
}

/*****************************************************************************/
/* NAME:      void RTC_GetIRQSrcStatus(										 */
/*									  t_rtc_IRQSrc irq_src, 				 */
/*									  t_rtc_IRQStatus *p_irq_status			 */
/*									  )		   						 	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to fill the p_irq_status structure 	 */
/*				according to the irq status							         */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_IRQSrc irq_src   : to disable the interrupt source	         */
/*		   t_rtc_IRQStatus *p_irq_status : Pointer to the interrupt status   */
/*										  structure     					 */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC void RTC_GetIRQSrcStatus(t_rtc_IRQSrc irq_src, t_rtc_IRQStatus *p_irq_status)
{
    /* Only to be compliant with HCL interrupt handling directives. */
    *p_irq_status = (t_uint8) irq_src;
}

/* RTC Timer APIs*/
/*****************************************************************************/
/* NAME:      void RTT_EnableIRQSrc(t_rtt_irq_src_id irqsrc)        	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to enable an interrupt source            */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_irq_src_id irqsrc   : to enable the interrupt source	     */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC void RTT_EnableIRQSrc(IN t_rtt_irq_src_id irqsrc)
{
    /* Unmask the timer interrupt */
    HCL_SET_BITS(gp_rtc_reg->RTCIMSC, MASK_BIT1);
}

/*****************************************************************************/
/* NAME:      void RTT_DisableIRQSrc(t_rtt_irq_src_id irqsrc) 		    	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to disable an interrupt source           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_irq_src_id irqsrc   : to disable the interrupt source	     */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC void RTT_DisableIRQSrc(IN t_rtt_irq_src_id irqsrc)
{
    /* Mask the timer interrupt */
    HCL_CLEAR_BITS(gp_rtc_reg->RTCIMSC, MASK_BIT1);
}

/*****************************************************************************/
/* NAME:      t_rtt_irq_src_id RTT_GetIRQSrc(t_rtt_device_id device)		 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the interrupting source of the device      */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_device device: The device for which the interrupt source    */
/*		   						is queried 							         */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: t_rtt_irq_src_id :Source for the interrupt	                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtt_irq_src_id RTT_GetIRQSrc(IN t_rtt_device_id device)
{
    t_uint32    irq_status;

    /* Only to be compliant with HCL interrupt handling directives */
    if (RTT_DEVICE_ID_0 == device)
    {
        /* Read masked interrupt status */
        irq_status = gp_rtc_reg->RTCMIS >> 1;

        if (TRUE == (t_bool) irq_status)
        {
            return(RTT_IRQ_SRC_ID_0);
        }
    }

    return(RTT_IRQ_SRC_NONE);
}

/*****************************************************************************/
/* NAME:      void RTT_ClearIRQSrc(t_rtt_irq_src_id irqsrc) 		    	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to clear the  interrupt source           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_irq_src_id irqsrc   : The interrupt source	to be cleared    */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC void RTT_ClearIRQSrc(IN t_rtt_irq_src_id irqsrc)
{
    /* Clear the timer interrupt source */
    HCL_SET_BITS(gp_rtc_reg->RTCICR, MASK_BIT1);
}

/*****************************************************************************/
/* NAME:      void RTT_IsPendingIRQSrc(t_rtt_irq_src_id irqsrc) 	    	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine checks the status of a particular interrupt     */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_irq_src_id irqsrc   : the interrupt source					 */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: t_bool : Status of the interrupt source	                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_bool RTT_IsPendingIRQSrc(IN t_rtt_irq_src_id irqsrc)
{
    t_uint32    irqstatus;

    irqstatus = gp_rtc_reg->RTCMIS >> 1;

    return((t_bool) irqstatus);
}

