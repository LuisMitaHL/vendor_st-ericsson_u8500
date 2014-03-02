 /************************************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PL022 (SSP) 
* \author  ST-Ericsson
*/
/************************************************************************************************/
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ssp_irqp.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE t_ssp_register  *gp_ssp_register[NUM_SSP_INSTANCES];

/****************************************************************************/
/* NAME:	void SSP_SetBaseAddress ()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the SSP registers.					*/
/* PARAMETERS:																*/
/* IN : t_logical_address ssp_base_address:	SSP registers base address 	    */
/* OUT: none 																*/
/* RETURN: void 															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC void SSP_SetBaseAddress(IN t_ssp_device_id ssp_device_id, IN t_logical_address ssp_base_address)
{
    /* Initialize the SSP registers structure */
    gp_ssp_register[ssp_device_id] = (t_ssp_register *) ssp_base_address;

    return;
}

/**********************************************************************************************/
/* NAME:	void		SSP_EnableIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to enable one or more interrupts						  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_irq   irq_src:	represent the enum of the interrupt to be enabled	  */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		void																		  */
/*                                                                                            */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant																	  */

/**********************************************************************************************/
PUBLIC void SSP_EnableIRQSrc(IN t_ssp_device_id ssp_device_id, IN t_ssp_irq_src irq_src)
{
    t_uint32    flag = 0;

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_TRANSMIT)
    {
        flag |= SSP_IMSC_MASK_TXIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE)
    {
        flag |= SSP_IMSC_MASK_RXIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SSP_IMSC_MASK_RTIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SSP_IMSC_MASK_RORIM;
    }

    gp_ssp_register[ssp_device_id]->ssp_imsc |= flag;
}

/**********************************************************************************************/
/* NAME:	void		SSP_DisableIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables one or more interrupts								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_irq_src    irq_src:	represent the enum of the interrupt to be disabled	  */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		void											 		           	          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC void SSP_DisableIRQSrc(IN t_ssp_device_id ssp_device_id, IN t_ssp_irq_src irq_src)
{
    t_uint32    flag = 0x0;

    /* Change the current contents */
    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_TRANSMIT)
    {
        flag |= SSP_IMSC_MASK_TXIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE)
    {
        flag |= SSP_IMSC_MASK_RXIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SSP_IMSC_MASK_RTIM;
    }

    if (irq_src & (t_ssp_irq_src) SSP_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SSP_IMSC_MASK_RORIM;
    }

    gp_ssp_register[ssp_device_id]->ssp_imsc &= ~flag;
}

/**********************************************************************************************/
/* NAME:	t_ssp_irq_src	SSP_GetIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the value of various interrupt enums					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 			t_ssp_device_id device													  */
/* OUT : 			NONE																	  */
/* 																			                  */
/* RETURN:			t_ssp_irq_src :    	 Active interrupt source.							  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC t_ssp_irq_src SSP_GetIRQSrc(IN t_ssp_device_id ssp_device_id)
{
    t_uint32    value = 0;

    value = gp_ssp_register[ssp_device_id]->ssp_mis;

    return(t_ssp_irq_src) value;
}

/**********************************************************************************************/
/* NAME:	void		SSP_ClearIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clear the receive time out interrupt							  */
/*				& receive over run raw interrupt											  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_irq_src    irq_src:	represent the enum of the interrupt to be cleared.*/
/* OUT :        none 																		  */
/*              																			  */
/* 																			                  */
/* RETURN:		void 																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC void SSP_ClearIRQSrc(IN t_ssp_device_id ssp_device_id, IN t_ssp_irq_src irq_src)
{
    t_uint32    flag = 0;

    if (irq_src & (t_uint32) SSP_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SSP_ICR_MASK_RTIC;
    }

    if (irq_src & (t_uint32) SSP_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SSP_ICR_MASK_RORIC;
    }

    gp_ssp_register[ssp_device_id]->ssp_icr |= flag;
}

/**********************************************************************************************/
/* NAME:	t_bool		SSP_IsPendingIRQSrc()											      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to check whether this interrupt has been asserted.		  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_irq_src_id irq_src_id  	interrupt source, whose id is to check.       */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		t_bool : TRUE, if asserted.  										  		  */
/*                                                                                            */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_bool SSP_IsPendingIRQSrc(IN t_ssp_device_id ssp_device_id, IN t_ssp_irq_src_id irq_src_id)
{
    t_bool      is_pending = FALSE;
    t_uint32    irq_status, flag = 0;

    if ((t_uint32) irq_src_id & (t_uint32) SSP_IRQ_SRC_TRANSMIT)
    {
        flag = SSP_IMSC_MASK_TXIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SSP_IRQ_SRC_RECEIVE)
    {
        flag = SSP_IMSC_MASK_RXIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SSP_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag = SSP_IMSC_MASK_RTIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SSP_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag = SSP_IMSC_MASK_RORIM;
    }

    irq_status = gp_ssp_register[ssp_device_id]->ssp_mis;

    is_pending = (t_bool) ((irq_status & flag) ? TRUE : FALSE);

    return(is_pending);
}

