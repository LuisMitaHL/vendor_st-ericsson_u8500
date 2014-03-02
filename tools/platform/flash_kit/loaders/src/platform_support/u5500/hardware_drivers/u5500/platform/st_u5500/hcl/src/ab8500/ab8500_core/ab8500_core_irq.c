/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the ab8500(STw4500) CORE interrupt support
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"
#include "ab8500_core_irqp.h"
#include "ab8500_core_irq.h"
#include "ab8500_core.h"

extern t_ab8500_core_error SER_AB8500_CORE_Read(t_uint8 bank_addr, t_uint8 register_offset, t_uint32 count, t_uint8 *dummy_data, t_uint8 *data_out);
extern t_ab8500_core_error SER_AB8500_CORE_Write(t_uint8 bank_addr, t_uint8 register_offset, t_uint32 count, t_uint8 *data_in);

/**********************************************************************************************/
/* NAME:    t_irq_ab8500_core_error        AB8500_CORE_SPIWrite()                           */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to disable one or more interrupts                        */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address where data has to be written,   */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be written,      */
/*               t_uint8  *p_data             The value to be written                         */
/* OUT :        none                                                                          */
/*                                                                                            */
/* RETURN:        t_irq_ab8500_core_error :          TOUAREG error code                      */
/*                                                                                            */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                                  */
/**********************************************************************************************/
PRIVATE t_irq_ab8500_core_error AB8500_CORE_SPIWrite(t_uint8 block_add,t_uint8 reg_offset,t_uint8 count,t_uint8 *data_value)
{
    return((t_irq_ab8500_core_error)SER_AB8500_CORE_Write(block_add,reg_offset,count,data_value));
}

/**********************************************************************************************/
/* NAME:    t_irq_ab8500_core_error        AB8500_CORE_SPIRead()                              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to disable one or more interrupts                        */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address from where data has to be read, */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be read,         */
/*               t_uint8  *p_data_value_in  Dummy Data                                        */
/* OUT:          t_uint8  *p_data_value_out    Data read from the specified address           */
/*                                                                                            */
/* RETURN:        t_irq_ab8500_core_error :          TOUAREG error code                       */
/*                                                                                            */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                                  */
/**********************************************************************************************/
PRIVATE t_irq_ab8500_core_error AB8500_CORE_SPIRead
(
 IN t_uint8 block_add,
 IN t_uint8 reg_offset,
 IN t_uint8 count,
 IN t_uint8 *data_value_in,
 OUT t_uint8 *data_value_out
 )
{
    return((t_irq_ab8500_core_error)SER_AB8500_CORE_Read(block_add,reg_offset,count,data_value_in,data_value_out));
}

/**********************************************************************************************/
/* NAME:    t_irq_ab8500_core_error        AB8500_CORE_EnableIRQSrc()                         */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to enable one or more interrupts                         */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :         t_ab8500_core_mask_reg: Register to enable                                    */
/*              mask_bits             : Interrupt-bits to be enabled                          */
/* OUT :        none                                                                          */
/*                                                                                            */
/* RETURN:        t_irq_ab8500_core_error :          TOUAREG error code                       */
/*                                                                                            */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                                  */
/**********************************************************************************************/
PUBLIC t_irq_ab8500_core_error AB8500_CORE_EnableIRQSrc(t_ab8500_core_mask_reg reg_name, t_uint8 mask_bits)
{
    t_irq_ab8500_core_error error_ab8500 = IRQ_AB8500_CORE_OK;
    t_uint8 dummy_data = 0xAA, data_out=0x00;

    error_ab8500 = (t_irq_ab8500_core_error)AB8500_CORE_SPIRead(AB8500_CORE_INTERRUPT_BLOCK, (t_uint8)reg_name, 0x01, &dummy_data, &data_out); 
    if(IRQ_AB8500_CORE_OK != error_ab8500)
    {
        return(error_ab8500);
    }
        
    data_out = (data_out & ~mask_bits);               /* Write the mask register for the following set of interrupts */

    error_ab8500 = (t_irq_ab8500_core_error)AB8500_CORE_SPIWrite(AB8500_CORE_INTERRUPT_BLOCK, (t_uint8)reg_name, 0x01, &data_out);
    if(IRQ_AB8500_CORE_OK != error_ab8500)
    {
        return(error_ab8500);
    }

    return(error_ab8500);
}

/**********************************************************************************************/
/* NAME:    t_irq_ab8500_core_error        AB8500_CORE_DisableIRQSrc()                        */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to disable one or more interrupts                        */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :         t_ab8500_core_mask_reg: Register to disable                                   */
/*              mask_bits             : Interrupt-bits to be disabled                         */
/* OUT :        none                                                                          */
/*                                                                                            */
/* RETURN:        t_irq_ab8500_core_error :          TOUAREG error code                       */
/*                                                                                            */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                                  */
/**********************************************************************************************/
PUBLIC t_irq_ab8500_core_error AB8500_CORE_DisableIRQSrc(t_ab8500_core_mask_reg reg_name, t_uint8 mask_bits)
{
    t_irq_ab8500_core_error error_ab8500 = IRQ_AB8500_CORE_OK;
    t_uint8 dummy_data = 0xAA;
    t_uint8 data_out = 0x00;

    /* We have to unmask only the specific inerrupts given in the 8 bit value...
        Therefore first read the value present at that location and the unmask the specific interrupts */

    /* Read the mask register first */
    error_ab8500 = (t_irq_ab8500_core_error)AB8500_CORE_SPIRead(AB8500_CORE_INTERRUPT_BLOCK, (t_uint8)reg_name, 0x01, &dummy_data, &data_out); 
   
    if(IRQ_AB8500_CORE_OK != error_ab8500)
    {
        return(error_ab8500);
    }
        
    data_out = (data_out | mask_bits);
       
    /* Write the mask register for the following set of interrupts */
    error_ab8500 = (t_irq_ab8500_core_error)AB8500_CORE_SPIWrite(AB8500_CORE_INTERRUPT_BLOCK, (t_uint8)reg_name, 0x01, &data_out);
    if(IRQ_AB8500_CORE_OK != error_ab8500)
    {
        return(error_ab8500);
    }
    
    return(error_ab8500);
}

