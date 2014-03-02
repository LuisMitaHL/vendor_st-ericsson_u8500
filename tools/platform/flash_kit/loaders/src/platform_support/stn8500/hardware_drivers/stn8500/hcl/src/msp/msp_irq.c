/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "msp_irqp.h"

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_msp_register      *gp_msp_register[MSP_INSTANCES];

PRIVATE volatile t_uint32   dummyForWait;

/*--------------------------------------------------------------------------*
 * Global data for interrupt mode management                                *
 *--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*
* Public functions                                                         *
*-------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    MSP_SetBaseAddress                                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:																*/
/* Initializes the MSP register base address and global data variable.		*/
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2		*/
/* msp_base_address: MSP0/MSP1/MSP2 registers base address.                 */
/* OUT:        none                                                        	*/
/*                                                                          */
/* RETURN:  																*/
/* None                                                               		*/
/*                                                               			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:Non Re-entrant                                                */
/* REENTRANCY ISSUES: 														*/
/*	1)global variable p_MspReg[msp_device_id](register base address)		*/
/*                      is being modified                                   */

/****************************************************************************/
PUBLIC void MSP_SetBaseAddress(IN t_msp_device_id msp_device_id, IN t_logical_address msp_base_address)
{
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return;
    }

    gp_msp_register[msp_device_id] = (t_msp_register *) msp_base_address;

    return;
}

/****************************************************************************/
/* NAME:        MSP_GetDeviceId	                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Extacts Device Id corresponding to Msp Interrupt Source Id  */
/*                                                    						*/
/* PARAMETERS:                                                              */
/* IN:    msp_irq_src:            Interrupt source Id.					    */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:                                                                  */
/*              Device Id for MSP.                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_msp_device_id MSP_GetDeviceId(IN t_msp_irq_src msp_irq_src)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (msp_irq_src >> MSP_SHIFT_BY_TWELVE);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return(MSP_LAST_DEVICE_ID);
    }

    return(msp_device_id);
}

/****************************************************************************/
/* NAME:        MSP_EnableIRQSrc                                         	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Enables the interrupt source(s) supplied as argument        */
/*                						                                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            it_to_be_enabled: Interrupt(s) that will be enabled.      */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:                                                                  */
/*              None                                                        */
/* REMARK:                                                                  */
/*         This function do nothing and immediately returns if Invalid 		*/
/*			device id is found. 											*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC void MSP_EnableIRQSrc(IN t_msp_irq_src it_to_be_enabled)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (it_to_be_enabled >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = gp_msp_register[msp_device_id];

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return;
    }

    p_msp_register->msp_imsc |= (it_to_be_enabled & MSP_MASK_INTERRUPT_BITS);

    /*delay after enabling the interrupt
        -------------------------------------*/
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
    /* dummyForWait = dummyForWait; */    /* Ignore warning */
}

/****************************************************************************/
/* NAME:        MSP_DisableIRQSrc                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Disables the interrupt source(s) supplied as argument    */
/*                received from the MSP.                                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            it_to_be_disabled:Interrupt that will be disabled.        */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:                                                                  */
/*              None                                                        */
/* REMARK:                                                                  */
/*         This function do nothing and immediately returns if Invalid 		*/
/*			device id is found.  											*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC void MSP_DisableIRQSrc(IN t_msp_irq_src it_to_be_disabled)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (it_to_be_disabled >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = gp_msp_register[msp_device_id];

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return;
    }

    p_msp_register->msp_imsc = (p_msp_register->msp_imsc &~(it_to_be_disabled & MSP_MASK_INTERRUPT_BITS));

    /*delay after disabling the interrupt
    -------------------------------------*/
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
}

/****************************************************************************/
/* NAME:        MSP_GetIRQSrc                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Returns the IRQ source number(contents of MIS) registers */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:        msp_device_id:        Device Id for Identification		    */
/* OUT:       None 		                                                    */
/*                                                                          */
/* RETURN:                                                                  */
/*              Active IRQ sources                                          */
/*                                                                          */
/* REMARK: Unpredictable value is returned when invalid device id is found.	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_msp_irq_src MSP_GetIRQSrc(IN t_msp_device_id msp_device_id)
{
    t_msp_register  *p_msp_register = gp_msp_register[msp_device_id];

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return(((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
    }

    return(((t_msp_irq_src) p_msp_register->msp_mis) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
}

/****************************************************************************/
/* NAME:        MSP_ClearIRQSrc                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    clears the interrupt with supplied Id.                   */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            it_to_be_cleared: Interrupts to be cleared.               */
/* OUT:           None                                                      */
/*                                                                          */
/* RETURN:        None                                                      */
/*                                                                          */
/* REMARK:                                                                  */
/*         This function do nothing and immediately returns if Invalid 		*/
/*			device id is found.												*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC void MSP_ClearIRQSrc(IN t_msp_irq_src it_to_be_cleared)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (it_to_be_cleared >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = gp_msp_register[msp_device_id];

    if ((int) msp_device_id >= MSP_INSTANCES)
    {
        return;
    }

    /*Rx and Tx interrupts are cleared automatically
    ------------------------------------------------*/
    if ((it_to_be_cleared & MSP_MASK_INTERRUPT_BITS) != ((t_uint32) MSP0_IRQ_SRC_RX_SERVICE) && (it_to_be_cleared & MSP_MASK_INTERRUPT_BITS) != ((t_uint32) MSP0_IRQ_SRC_TX_SERVICE))
    {
        p_msp_register->msp_icr = (p_msp_register->msp_icr | (MSP_MASK_INTERRUPT_BITS & it_to_be_cleared));
    }
}

/****************************************************************************/
/* NAME:        MSP_IsPendingIRQSrc                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This function allows to know if the IRQSRc is active or not. */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:pending_its :Interrupt Id to be chceked for pending status.           */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:        TRUE/FALSE                                                */
/*                                                                          */
/* REMARK: FALSE is also returned in case invalid device id is found.  		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_bool MSP_IsPendingIRQSrc(IN t_msp_irq_src pending_its)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (pending_its >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = gp_msp_register[msp_device_id];

    if ((int) msp_device_id >= MSP_INSTANCES)
    {
        return(FALSE);
    }

    if ((p_msp_register->msp_mis | (MSP_MASK_INTERRUPT_BITS & pending_its)) != 0)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/* End of file - msp_irq.c*/

