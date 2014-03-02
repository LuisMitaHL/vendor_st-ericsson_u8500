/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains Interrupt Support Routines for UART (U5500)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "uart_irqp.h"

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_uart_register *gp_uart0_register;
PRIVATE t_uart_register *gp_uart1_register;
PRIVATE t_uart_register *gp_uart2_register;
PRIVATE t_uart_register *gp_uart3_register;

/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME           :    UART_SetBaseAddress                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :   This routine initializes UART register base address   */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :   uart_base_address    :    UART registers base address */
/*                :   uart_device_id       :   Idetify the UART             */
/*         OUT    :   None                                                  */
/*                                                                          */
/*      RETURN    :   None                                                  */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Non Re-entrant                                     */
/* REENTRANCY ISSUES:                                                       */
/*            1)     Global variable gp_uart_register(register base address)*/
/*                   is being modified                                      */
/****************************************************************************/
PUBLIC void UART_SetBaseAddress(IN t_uart_device_id uart_device_id, IN t_logical_address uart_base_address)
{
    /* Initializing the registers structure *
     *--------------------------------------*/
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            gp_uart0_register = (t_uart_register *) uart_base_address;
            break;

        case UART_DEVICE_ID_1:
            gp_uart1_register = (t_uart_register *) uart_base_address;
            break;

        case UART_DEVICE_ID_2:
            gp_uart2_register = (t_uart_register *) uart_base_address;
            break;
        case UART_DEVICE_ID_3:
            gp_uart3_register = (t_uart_register *) uart_base_address;
            break;

	case UART_DEVICE_ID_INVALID:
	default:
	    break;    
    }
    return;
}

/****************************************************************************/
/* NAME           :    UART_EnableIRQSrc                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :   This function enables requested interrupt source(s)   */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :   enable_irq    :    Interrupt source(s) to be enabled. */
/*                                                                          */
/*         OUT    :   None                                                  */
/*                                                                          */
/*      RETURN    :   None                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Non-Reentrant                                      */
/* REENTRANCY ISSUES:                                                       */
/*                1)     Global variable gp_uart_register(Interrupt mask    */
/*                       Set/Clear) is being modified                       */
/****************************************************************************/
PUBLIC void UART_EnableIRQSrc(IN t_uart_irq_src enable_irq)
{
    t_uart_register *lp_uart_register;

    switch (UART_GETDEVICE(enable_irq))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = gp_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = gp_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = gp_uart2_register;
            break;
        case UART_DEVICE_ID_3:
            lp_uart_register = gp_uart3_register;
            break;

        case UART_DEVICE_ID_INVALID:
        default:
            return;
    }

    lp_uart_register->uartx_imsc |= (enable_irq & UART_IRQ_SRC_ALL_IT);
    lp_uart_register->uartx_abimsc |= ((enable_irq >> UART_AUTOBAUD_INTR_SHIFT) & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE));
}

/****************************************************************************/
/* NAME           :   UART_DisableIRQSrc                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :   This function disables requested interrupt source(s)  */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :   disable_irq: interrupt source(s) to be disabled.      */
/*                                                                          */
/*         OUT    :   None                                                  */
/*                                                                          */
/*      RETURN    :   None                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Non-Reentrant                                      */
/* REENTRANCY ISSUES:                                                       */
/*                1)    Global variable gp_uart_register(Interrupt mask     */
/*                      Set/Clear) is being modified                        */
/****************************************************************************/
PUBLIC void UART_DisableIRQSrc(IN t_uart_irq_src disable_irq)
{
    t_uart_register *lp_uart_register;

    switch (UART_GETDEVICE(disable_irq))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = gp_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = gp_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = gp_uart2_register;
            break;
        case UART_DEVICE_ID_3:
            lp_uart_register = gp_uart3_register;
            break;

        case UART_DEVICE_ID_INVALID:
        default:
            return;
    }

    lp_uart_register->uartx_imsc &= ~(disable_irq & UART_IRQ_SRC_ALL_IT);
    lp_uart_register->uartx_abimsc &= ~((disable_irq >> UART_AUTOBAUD_INTR_SHIFT) & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE));
}

/****************************************************************************/
/* NAME           :    UART_GetIRQSrc                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine returns the interrupt status of all     */
/*                     the interrupts.                                      */
/*  PARAMETERS    :                                                         */
/*          IN    :    uart_device_id:  The device for which the interrupt  */
/*                                      source is queried                   */
/*                                                                          */
/*         OUT    :    None                                                 */
/*                                                                          */
/*      RETURN    :    t_uart_irq_src: ORed value of all the active         */
/*                                     interrupt sources                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_uart_irq_src UART_GetIRQSrc(IN t_uart_device_id uart_device_id)
{
    t_uart_irq_src  irq_src = MASK_NULL32;
    t_uart_register *lp_uart_register;

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = gp_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = gp_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = gp_uart2_register;
            break;
        
        case UART_DEVICE_ID_3:
            lp_uart_register = gp_uart3_register;
            break;

        case UART_DEVICE_ID_INVALID:
        default:
            return((t_uart_irq_src) UART_IRQ_SRC_NO_IT);
    }

    irq_src = (lp_uart_register->uartx_mis) & (t_uart_irq_src) (UART_IRQ_SRC_ALL_IT);
    irq_src |=
            (
                (lp_uart_register->uartx_abmis & (t_uart_irq_src) (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE)) <<
                UART_AUTOBAUD_INTR_SHIFT
            );

    irq_src |= (((t_uint32) uart_device_id) << UART_ID_SHIFT);

    return(irq_src);
}

/****************************************************************************/
/* NAME           :    UART_ClearIRQSrc                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This function clears/acknowledges the requested      */
/*                     interrupt source(s).                                 */
/*  PARAMETERS    :                                                         */
/*          IN    :    clear_irq: Interrupt(s) to be cleared/acknowledged   */
/*         OUT    :    none                                                 */
/*                                                                          */
/*      RETURN    :    void                                                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :     Non-Re-Entrant                                    */
/* REENTRANCY ISSUES:                                                       */
/*            1)          Global variable gp_uart_register Interrupt Clear  */
/*                        register is being modified                        */
/****************************************************************************/
PUBLIC void UART_ClearIRQSrc(IN t_uart_irq_src clear_irq)
{
    t_uart_register *lp_uart_register;

    switch (UART_GETDEVICE(clear_irq))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = gp_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = gp_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = gp_uart2_register;
            break;
        
        case UART_DEVICE_ID_3:
            lp_uart_register = gp_uart3_register;
            break;

        case UART_DEVICE_ID_INVALID:
        default:
            return;
    }

    lp_uart_register->uartx_icr |= (clear_irq & UART_IRQ_SRC_ALL_IT);
    lp_uart_register->uartx_abicr |= ((clear_irq >> UART_AUTOBAUD_INTR_SHIFT) & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE));
}

/****************************************************************************/
/* NAME          :    UART_IsPendingIRQSrc                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :    This routine tests if specific interrupt source(s)    */
/*                    is/are active or not.                                 */
/*  PARAMETERS   :                                                          */
/*          IN   :    irq_src :   interrupt source(s) to be checked whether */
/*                                it has been asserted.                     */
/*         OUT   :    none                                                  */
/*                                                                          */
/*      RETURN   :    TRUE    :   Returned when the requested interrupt     */
/*                                source is still pending                   */
/*               :    FALSE   :   Returned when the requested interrupt     */
/*                                source is not pending                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-Entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_bool UART_IsPendingIRQSrc(IN t_uart_irq_src irq_src)
{
    t_uint32        irq_mask;
    t_uart_register *lp_uart_register;

    switch (UART_GETDEVICE(irq_src))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = gp_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = gp_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = gp_uart2_register;
            break;
        
        case UART_DEVICE_ID_3:
            lp_uart_register = gp_uart3_register;
            break;

        case UART_DEVICE_ID_INVALID:
        default:
            return(FALSE);
    }

    irq_mask = lp_uart_register->uartx_mis & UART_IRQ_SRC_ALL_IT;
    if (0 != (irq_mask & irq_src))
    {
        return(TRUE);
    }

    irq_mask = lp_uart_register->uartx_abmis;
    if (0 != (((irq_src >> UART_AUTOBAUD_INTR_SHIFT) & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE)) & irq_mask))
    {
        return(TRUE);
    }

    return(FALSE);
}

/****************************************************************************/
/* NAME         :   UART_GetDeviceID                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine returns the Device Id corresponding to the */
/*                  interrupt source(s)                                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src  :  Identify the interrupt source               */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   UART number (UART_DEVICE_ID_0, UART_DEVICE_ID_1,        */
/*                  UART_DEVICE_ID_2, UART_DEVICE_ID_3)                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-Entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_uart_device_id UART_GetDeviceID(IN t_uart_irq_src irq_src)
{
    return(UART_GETDEVICE(irq_src));
}

/* End of file - uart_irq.c */

