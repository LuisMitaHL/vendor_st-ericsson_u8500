/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides CRYP M0 routines for CRYP HCL
*  Specification release related to this implementation: A_V2.2
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "cryp_irqp.h"

PRIVATE t_cryp_register  *gp_cryp0_register; 
PRIVATE t_cryp_register  *gp_cryp1_register; 

/*--------------------------------------------------------------------------*/
/*		 NAME :	CRYP_SetBaseAddress()										*/	    	
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine only initialize the global pointer.			*/
/* PARAMETERS :																*/
/*         IN : cryp_device_id                                              */
/*         IN : cryp_base_address:CRYP registers base addres				*/
/*		  OUT : None														*/
/* 																			*/
/*	   RETURN : None														*/
/*  			  															*/
/*  REENTRANCY: Non Re-entrant												*/
/*--------------------------------------------------------------------------*/

PUBLIC  void CRYP_SetBaseAddress (IN t_cryp_device_id cryp_device_id, IN t_logical_address cryp_base_address)
{  	
	if(CRYP_DEVICE_ID_0 == cryp_device_id)
	{
		gp_cryp0_register = (t_cryp_register*)cryp_base_address;	
	}
	else if(CRYP_DEVICE_ID_1 == cryp_device_id)
	{
		gp_cryp1_register = (t_cryp_register*)cryp_base_address;	
	}
	
}

/****************************************************************************/
/* NAME:	 CRYP_EnableIRQSrc() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine enables single/ multiple interrupt sources	*/
/* PARAMETERS    :															*/
/* IN            : cryp_device_id                                           */
/* IN            : irq_src -- irqsource identification						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void 												    */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/	
/****************************************************************************/
PUBLIC void CRYP_EnableIRQSrc(IN t_cryp_device_id cryp_device_id, IN t_cryp_irq_src irq_src)
{
	if(CRYP_DEVICE_ID_0 == cryp_device_id)
	{
		/*Set the bits*/
	    gp_cryp0_register->imsc |= irq_src;	
	}
	else if(CRYP_DEVICE_ID_1 == cryp_device_id)
	{
		/*Set the bits*/
	    gp_cryp1_register->imsc |= irq_src;	
	}
}

/****************************************************************************/
/* NAME:	 CRYP_DisableIRQSrc() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine clears the interrupt mask for the CRYP loigc*/
/* PARAMETERS    :															*/
/* IN            : cryp_device_id                                           */
/* IN            : irq_src: The interrupt disable mask value.               */
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void 													*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/	
/****************************************************************************/
PUBLIC void CRYP_DisableIRQSrc(IN t_cryp_device_id cryp_device_id, t_cryp_irq_src irq_src)
{	
    if(CRYP_DEVICE_ID_0 == cryp_device_id)
    {
    	/*Clear the bits*/
	    gp_cryp0_register->imsc &= (~irq_src);	
    }
    else if(CRYP_DEVICE_ID_1 == cryp_device_id)
    {
    	/*Clear the bits*/
	    gp_cryp1_register->imsc &= (~irq_src);	
    }
    
}

/****************************************************************************/
/* NAME:	 CRYP_GetIRQSrc() 												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine returns the interrupt source.               */
/* PARAMETERS    :															*/
/* IN            : cryp_device_id          									*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_cryp_irq_src											*/	
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/	
/****************************************************************************/
PUBLIC t_cryp_irq_src CRYP_GetIRQSrc(IN t_cryp_device_id cryp_device_id)
{																			
	t_cryp_irq_src  interrupt_status=0;
	
	if(CRYP_DEVICE_ID_0 == cryp_device_id)
	{
		/*Return the register content*/
	    interrupt_status = gp_cryp0_register->mis;
	}
	else if(CRYP_DEVICE_ID_1 == cryp_device_id)
	{
		/*Return the register content*/
	    interrupt_status = gp_cryp1_register->mis;
	}
    
    return interrupt_status;
}             
/****************************************************************************/
/* NAME:	 CRYP_IsPendingIRQSrc() 										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine checks the status of the IRQ signal.        */
/* PARAMETERS    :															*/
/* IN            : cryp_device_id          									*/
/* IN            : irq_src -- irqsource identification						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : TRUE    :   Returned when the requested interrupt        */
/*                              source is still pending                     */
/*                  FALSE   :   Returned when the requested interrupt       */
/*                              source is not pending                       */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/	
/****************************************************************************/
PUBLIC t_bool CRYP_IsPendingIRQSrc(IN t_cryp_device_id cryp_device_id, IN t_cryp_irq_src irq_src)
{
	t_bool it_status=FALSE;
	
	if(CRYP_DEVICE_ID_0 == cryp_device_id)
	{
		it_status = (t_bool)((gp_cryp0_register->mis & irq_src) ? TRUE : FALSE);
	}
	else if(CRYP_DEVICE_ID_1 == cryp_device_id)
	{
		it_status = (t_bool)((gp_cryp1_register->mis & irq_src) ? TRUE : FALSE);
	}
	
	return (it_status);
}
/*End of file*/
