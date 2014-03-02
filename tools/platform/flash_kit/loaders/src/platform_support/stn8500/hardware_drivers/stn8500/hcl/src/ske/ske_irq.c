/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides the Hardware Abstraction/Control API definitions for
*  Elementary Interrupt Management(M0 functions) in SKE(Scroll Key and Keypad Encoder)
*  Reference : Software Code Development, C Coding Rules, Guidelines 
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "ske_irq.h"
#include "ske_hwp.h"


//PRIVATE t_ske_register  *gp_ske_register;
PUBLIC t_ske_register  *gp_ske_register;


/*******************************************************************************************/
/* NAME:  SKE_SetBaseAddress                                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function initialises the SKE Hardware base address for IRQ Management */
/* PARAMETERS:                                                                             */
/* IN:    ske_base_address : Base address of SKE                                           */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void SKE_SetBaseAddress(IN t_logical_address ske_base_address)
{
    gp_ske_register = (t_ske_register *)ske_base_address;
}


/*******************************************************************************************/
/* NAME:  SKE_EnableIRQSrc                                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables the Interrupts for Scroll Key 0,1 & Keypad encoders  */
/* PARAMETERS:                                                                             */
/* IN:    ske_irq_src :Interrupt sources to be enabled(can be individual or combined)      */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void SKE_EnableIRQSrc(IN t_ske_irq_src ske_irq_src)
{
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_0))
    {
        HCL_SET_BITS(gp_ske_register->ske_imsc,SKE_SKEIM0_MASK);
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_1))
    {
        HCL_SET_BITS(gp_ske_register->ske_imsc,SKE_SKEIM1_MASK);
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_AUTOSCAN))
    {
        HCL_SET_BITS(gp_ske_register->ske_imsc,SKE_KPIMA_MASK);
    }    
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_SOFTSCAN))
    {
        HCL_SET_BITS(gp_ske_register->ske_imsc,SKE_KPIMS_MASK);
    }  

}


/*******************************************************************************************/
/* NAME:  SKE_DisableIRQSrc                                                                */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function disables the Interrupts for Scroll Key 0,1 & Keypad encoders */
/* PARAMETERS:                                                                             */
/* IN:    ske_irq_src :Interrupt sources to be disabled(can be individual or combined)     */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void SKE_DisableIRQSrc(IN t_ske_irq_src ske_irq_src)
{
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_0))
    {
        HCL_CLEAR_BITS(gp_ske_register->ske_imsc,SKE_SKEIM0_MASK);
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_1))
    {
        HCL_CLEAR_BITS(gp_ske_register->ske_imsc,SKE_SKEIM1_MASK);
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_AUTOSCAN))
    {
        HCL_CLEAR_BITS(gp_ske_register->ske_imsc,SKE_KPIMA_MASK);
    }    
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_SOFTSCAN))
    {
        HCL_CLEAR_BITS(gp_ske_register->ske_imsc,SKE_KPIMS_MASK);
    }    

}

/*******************************************************************************************/
/* NAME:  SKE_GetIRQSrc                                                                    */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets masked interrupt status for Scroll Key 0,1 &            */
/*              Keypad encoders                                                            */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: Interrupt sources(combined) which has occurred                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC t_ske_irq_src SKE_GetIRQSrc(void)
{
	t_uint32 irq_status;
    irq_status = HCL_READ_BITS(gp_ske_register->ske_mis,SKE_COMBINED_IRQ_MASK);
    return ((t_ske_irq_src)irq_status);
}

/*******************************************************************************************/
/* NAME:  SKE_ClearIRQSrc                                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function clears the Interrupts for Scroll Key 0,1 & Keypad encoders   */
/* PARAMETERS:                                                                             */
/* IN:    ske_irq_src :Interrupt sources to be cleared(can be individual or combined)      */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void SKE_ClearIRQSrc(IN t_ske_irq_src ske_irq_src)
{
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_0))
    {
        HCL_SET_BITS(gp_ske_register->ske_icr,SKE_SKEIC0_MASK);
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_1))
    {
        HCL_SET_BITS(gp_ske_register->ske_icr,SKE_SKEIC1_MASK);
    }   
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_AUTOSCAN))
    {
        HCL_SET_BITS(gp_ske_register->ske_icr,SKE_KPICA_MASK);
    }    
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_SOFTSCAN))
    {
        HCL_SET_BITS(gp_ske_register->ske_icr,SKE_KPICS_MASK);
    }          

}

/*******************************************************************************************/
/* NAME:  SKE_IsPendingIRQSrc                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function tells whether required the Interrupt(s) are active or not    */
/*              for Scroll Key 0,1 & Keypad encoders                                       */
/* PARAMETERS:                                                                             */
/* IN:    ske_irq_src :Interrupt sources(can be individual or combined)                    */
/*                     whose status needs to be known                                      */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: Status of Interrupt                                                             */
/*           TRUE - if the required IRQ(any of IRQs, for combined input) is/are active     */
/*           FALSE - otherwise(i.e.if not active)                                          */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC t_bool SKE_IsPendingIRQSrc(IN t_ske_irq_src ske_irq_src)
{
	t_uint32 irq_status = 0;
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_0))
    {        
    	irq_status |= ((t_uint32)(HCL_READ_BITS(gp_ske_register->ske_mis,SKE_SKEMIS0_MASK)));
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_SCROLL_DEVICE_1))
    {
        irq_status |= ((t_uint32)(HCL_READ_BITS(gp_ske_register->ske_mis,SKE_SKEMIS1_MASK)));
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_AUTOSCAN))
    {
        irq_status |= ((t_uint32)(HCL_READ_BITS(gp_ske_register->ske_mis,SKE_KPMISA_MASK)));
    }
    if(ske_irq_src & ((t_uint32)SKE_IRQ_KEYPAD_SOFTSCAN))
    {
        irq_status |= ((t_uint32)(HCL_READ_BITS(gp_ske_register->ske_mis,SKE_KPMISS_MASK)));
    }
    if(irq_status)
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }    
}

