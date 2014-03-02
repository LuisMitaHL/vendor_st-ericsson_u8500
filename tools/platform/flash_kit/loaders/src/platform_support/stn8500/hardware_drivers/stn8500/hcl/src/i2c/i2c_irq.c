/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides interrupt routines for the I2C Controller
* \author  ST-Ericsson
*/
/*****************************************************************************/


/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "i2c_irq.h"
#include "i2c_irqp.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/

#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    /* 8500 ED has 4 I2C controllers */
    PRIVATE t_i2c_registers *gp_i2c_registers[4];
#else 
    /* 8500 V1 has 5 I2C controllers */
    PRIVATE t_i2c_registers *gp_i2c_registers[5];
#endif

/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_SetBaseAddress										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes I2C register base address.     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	id          : I2C controller id                         */
/*                    i2c_base_address	:	I2C registers base address		*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/
/*			1)		Global variable gp_registers (register base address)	*/
/*					is being modified										*/

/****************************************************************************/
PUBLIC void I2C_SetBaseAddress(t_i2c_device_id id, t_logical_address address)
{
    /* Initializing the I2C controller base address */
    gp_i2c_registers[id] = (t_i2c_registers *) address;
}

/****************************************************************************/
/* NAME			:	I2C_EnableIRQSrc										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Enable the given I2C controller to generate interrupts. */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:  t_i2c_irq_src_id id	: the IRQ source to be enabled.		*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC void I2C_EnableIRQSrc(t_i2c_irq_src_id irq_id)
{
	t_i2c_device_id id;
	id = GETDEVICE((t_uint32)irq_id);
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		return;
	}
    
    gp_i2c_registers[id]->imscr |= ((t_uint32)I2C_IRQ_SRC_ALL & (t_uint32)irq_id);

}
/****************************************************************************/
/* NAME			:	I2C_DisableIRQSrc										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Disable the given I2C controller to generate interrupts. */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id id	: the IRQ source to be disabled.	*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC void I2C_DisableIRQSrc(t_i2c_irq_src_id irq_id)
{
	t_i2c_device_id id;
	id = GETDEVICE((t_uint32)irq_id);
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		return;
	}
    
	gp_i2c_registers[id]->imscr &= ~((t_uint32)I2C_IRQ_SRC_ALL & (t_uint32)irq_id);

}

/****************************************************************************/
/* NAME			:	I2C_GetIRQSrc									    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: Get the interrupt source relative to the given identifier */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id id	: get the IRQ source.       		*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	t_i2c_irq_src_id								    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC t_i2c_irq_src_id I2C_GetIRQSrc(t_i2c_device_id irq_id)
{

    return((t_i2c_irq_src_id)((gp_i2c_registers[(t_uint32)irq_id]->misr) |((t_uint32)irq_id << I2CID_SHIFT)));

}

/****************************************************************************/
/* NAME			:	I2C_ClearIRQSrc									    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Clear the interrupt source relative to the given identifier*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id id	: clear the IRQ source.             */
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC void I2C_ClearIRQSrc(t_i2c_irq_src_id irq_id)
{
	t_i2c_device_id id;
	id = GETDEVICE((t_uint32)irq_id);
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		return;
	}
	
    gp_i2c_registers[id]->icr |= ((t_uint32)I2C_IRQ_SRC_ALL & (t_uint32)irq_id);

}

/****************************************************************************/
/* NAME			:	I2C_IsPendingIRQSrc										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to check the status of an             */
/*               interrupt source                                           */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id id	: the IRQ source to be checked		*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC t_bool I2C_IsPendingIRQSrc(t_i2c_irq_src_id irq_id)
{
	
	t_i2c_device_id id;
	id = GETDEVICE((t_uint32)irq_id);
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		return(FALSE);
	}

    if (gp_i2c_registers[id]->misr & ((t_uint32)I2C_IRQ_SRC_ALL & (t_uint32)irq_id))
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }

}

/****************************************************************************/
/* NAME			:	I2C_GetDeviceID									    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Get the device id corresponding to an interrupt id.        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id id	: the IRQ source id.           		*/
/* 		OUT 	:	None                                                	*/
/*                                                                          */
/* RETURN		:	None											    	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    :	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/

/****************************************************************************/
PUBLIC t_i2c_device_id I2C_GetDeviceID(t_i2c_irq_src_id irq_id)
{

    return(GETDEVICE((t_uint32)irq_id));

}

