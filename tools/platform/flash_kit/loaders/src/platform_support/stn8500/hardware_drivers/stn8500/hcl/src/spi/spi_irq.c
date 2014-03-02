/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PL022 (SPI)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "spi_irqp.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE t_spi_register  *gp_spi_register[NUM_SPI_INSTANCES];

/****************************************************************************/
/* NAME:	void SPI_SetBaseAddress ()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the SPI registers.					*/
/* PARAMETERS:																*/
/* IN : t_logical_address spi_base_address:	SPI registers base address 	    */
/* OUT: none 																*/
/* RETURN: void 															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC void SPI_SetBaseAddress(IN t_spi_device_id spi_device_id, IN t_logical_address spi_base_address)
{
    /* Initialize the SPI registers structure */
    gp_spi_register[spi_device_id] = (t_spi_register *) spi_base_address;

    return;
}

/**********************************************************************************************/
/* NAME:	void		SPI_EnableIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to enable one or more interrupts						  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_irq   irq_src:	represent the enum of the interrupt to be enabled	  */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		void																		  */
/*                                                                                            */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant																	  */

/**********************************************************************************************/
PUBLIC void SPI_EnableIRQSrc(IN t_spi_device_id spi_device_id, IN t_spi_irq_src irq_src)
{
    t_uint32    flag = 0;

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_TRANSMIT)
    {
        flag |= SPI_IMSC_MASK_TXIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE)
    {
        flag |= SPI_IMSC_MASK_RXIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SPI_IMSC_MASK_RTIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SPI_IMSC_MASK_RORIM;
    }

    gp_spi_register[spi_device_id]->spi_imsc |= flag;
}

/**********************************************************************************************/
/* NAME:	void		SPI_DisableIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables one or more interrupts								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_irq_src    irq_src:	represent the enum of the interrupt to be disabled	  */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		void											 		           	          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC void SPI_DisableIRQSrc(IN t_spi_device_id spi_device_id, IN t_spi_irq_src irq_src)
{
    t_uint32    flag = 0x0;

    /* Change the current contents */
    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_TRANSMIT)
    {
        flag |= SPI_IMSC_MASK_TXIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE)
    {
        flag |= SPI_IMSC_MASK_RXIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SPI_IMSC_MASK_RTIM;
    }

    if (irq_src & (t_spi_irq_src) SPI_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SPI_IMSC_MASK_RORIM;
    }

    gp_spi_register[spi_device_id]->spi_imsc &= ~flag;
}

/**********************************************************************************************/
/* NAME:	t_spi_irq_src	SPI_GetIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the value of various interrupt enums					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 			t_spi_device_id device													  */
/* OUT : 			NONE																	  */
/* 																			                  */
/* RETURN:			t_spi_irq_src :    	 Active interrupt source.							  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC t_spi_irq_src SPI_GetIRQSrc(IN t_spi_device_id spi_device_id)
{
    t_uint32    value = 0;

    value = gp_spi_register[spi_device_id]->spi_mis;

    return(t_spi_irq_src) value;
}

/**********************************************************************************************/
/* NAME:	void		SPI_ClearIRQSrc()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clear the receive time out interrupt							  */
/*				& receive over run raw interrupt											  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_irq_src    irq_src:	represent the enum of the interrupt to be cleared.*/
/* OUT :        none 																		  */
/*              																			  */
/* 																			                  */
/* RETURN:		void 																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC void SPI_ClearIRQSrc(IN t_spi_device_id spi_device_id, IN t_spi_irq_src irq_src)
{
    t_uint32    flag = 0;

    if (irq_src & (t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag |= SPI_ICR_MASK_RTIC;
    }

    if (irq_src & (t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag |= SPI_ICR_MASK_RORIC;
    }

    gp_spi_register[spi_device_id]->spi_icr |= flag;
}

/**********************************************************************************************/
/* NAME:	t_bool		SPI_IsPendingIRQSrc()											      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to check whether this interrupt has been asserted.		  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_irq_src_id irq_src_id  	interrupt source, whose id is to check.       */
/* OUT :        none 																		  */
/* 																			                  */
/* RETURN:		t_bool : TRUE, if asserted.  										  		  */
/*                                                                                            */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_bool SPI_IsPendingIRQSrc(IN t_spi_device_id spi_device_id, IN t_spi_irq_src_id irq_src_id)
{
    t_bool      is_pending = FALSE;
    t_uint32    irq_status, flag = 0;

    if ((t_uint32) irq_src_id & (t_uint32) SPI_IRQ_SRC_TRANSMIT)
    {
        flag = SPI_IMSC_MASK_TXIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SPI_IRQ_SRC_RECEIVE)
    {
        flag = SPI_IMSC_MASK_RXIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT)
    {
        flag = SPI_IMSC_MASK_RTIM;
    }
    else if ((t_uint32) irq_src_id & (t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN)
    {
        flag = SPI_IMSC_MASK_RORIM;
    }

    irq_status = gp_spi_register[spi_device_id]->spi_mis;

    is_pending = (t_bool) ((irq_status & flag) ? TRUE : FALSE);

    return(is_pending);
}


