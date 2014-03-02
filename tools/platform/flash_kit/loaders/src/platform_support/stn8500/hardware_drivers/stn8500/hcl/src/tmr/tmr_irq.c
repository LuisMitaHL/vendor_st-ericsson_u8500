/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the TIMER (MTU)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "tmr_irq.h"
#include "tmr_irqp.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
PRIVATE t_tmr_register  *gp_tmr_register[TMR_INSTANCES_OF_TMR_UNITS];

/***************************************************************************
PRIVATE APIs Declarations 
****************************************************************************/
/*PUBLIC API FUNCTIONS*/
/********************************************************************************/
/* NAME:	 TMR_SetBaseAddress()												*/
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This routine initializes the TMR units. This function is     */
/*                 called twice, once for each timer unit.                      */
/* PARAMETERS    :											                    */
/* IN            : t_logical_address                                            */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void												            */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: NA														        */

/********************************************************************************/
PUBLIC void TMR_SetBaseAddress(t_tmr_device_id tmr_device_id, t_logical_address tmr_base_address)
{
    if (NULL != tmr_base_address)
    {
        switch (tmr_device_id)
        {
            case TMR_DEVICE_ID_0:
                gp_tmr_register[0] = (t_tmr_register *) tmr_base_address;
                break;

            case TMR_DEVICE_ID_1:
                gp_tmr_register[1] = (t_tmr_register *) tmr_base_address;
                break;


            case TMR_DEVICE_ID_INVALID:
            default:
                return;
        }
    }
}

/********************************************************************************/
/* NAME:	 TMR_GetIRQSrc()                                                    */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This function returns the IRQ source numbers of the available*/
/*				  interrupts.                                                   */
/* PARAMETERS    :											                    */
/* IN            : tmr_device_id -- (MTU1,MTU2,MTU3 or MTU4)	            */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : t_tmr_irq_src												*/
/* TYPE          : Public                                                       */
/*                             8     7                  0                       */
/*                           ------  ----- ------- ------ -----                 */
/*                          |      |      |       |      |      |               */
/*                          |Timer8|Timer7|       |Timer1|Timer0|               */
/*                           ------  ----- ------- ------ -----                 */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_tmr_irq_src TMR_GetIRQSrc(t_tmr_device_id tmr_device_id)
{
    t_tmr_irq_src   tmr_irq_src = 0;

    switch (tmr_device_id)
    {
        case TMR_DEVICE_ID_0:
            tmr_irq_src = gp_tmr_register[0]->tmr_mis;
            break;

        case TMR_DEVICE_ID_1:
            tmr_irq_src = gp_tmr_register[1]->tmr_mis << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT;
            break;

        case TMR_DEVICE_ID_INVALID:
        default:
            break;
    }

    return(tmr_irq_src);
}

/********************************************************************************/
/* NAME:	 TMR_EnableIRQSrc()                                                 */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This routine enables single/ multiple interrupt sources	    */
/* PARAMETERS    :											                    */
/* IN            : tmr_irq_src -- irqsource identification                      */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void 												        */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC void TMR_EnableIRQSrc(t_tmr_irq_src tmr_irq_src)
{
    if (NULL != gp_tmr_register[0])
    {
        gp_tmr_register[0]->tmr_imsc |= tmr_irq_src & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1);
    }

    if (NULL != gp_tmr_register[1])
    {
        gp_tmr_register[1]->tmr_imsc |= (tmr_irq_src >> TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1);
    }


    /*tmr_MTUEnableIRQSrc(tmr_irq_src);*/
}

/********************************************************************************/
/* NAME:	 TMR_DisableIRQSrc()                                                */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This routine disables single/ multiple interrupt sources	    */
/* PARAMETERS    :											                    */
/* IN            : tmr_irq_src -- irqsource identification                      */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void 												        */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC void TMR_DisableIRQSrc(t_tmr_irq_src tmr_irq_src)
{
    if (NULL != gp_tmr_register[0])
    {
        gp_tmr_register[0]->tmr_imsc &= ~(tmr_irq_src & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1));
    }

    if (NULL != gp_tmr_register[1])
    {
        gp_tmr_register[1]->tmr_imsc &= ~((tmr_irq_src >> TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1));
    }

}

/********************************************************************************/
/* NAME:	 TMR_ClearIRQSrc()                                                  */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : Clears the interrupt source in the Interrupt Clear Register .*/
/* PARAMETERS    :											                    */
/* IN            : tmr_irq_src (TIMER_x)                                        */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void                                                         */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC void TMR_ClearIRQSrc(t_tmr_irq_src tmr_irq_src)
{
    if (NULL != gp_tmr_register[0])
    {
        gp_tmr_register[0]->tmr_icr |= tmr_irq_src & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1);
    }

    if (NULL != gp_tmr_register[1])
    {
        gp_tmr_register[1]->tmr_icr |= (tmr_irq_src >> TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) & ((1 << TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT) - 1);
    }

}

/********************************************************************************/
/* NAME:	 TMR_GetDeviceID()												    */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This function provides the Device ID number associated       */
/*             	  with the IRQ Source ID number.                                */
/* PARAMETERS    :											                    */
/* IN            : t_tmr_irq_src id                                             */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : t_tmr_device_id 												*/
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_tmr_device_id TMR_GetDeviceID(t_tmr_irq_src tmr_irq_src)
{
    t_tmr_device_id tmr_device_id = TMR_DEVICE_ID_0;

    if (TMR_TIMER_IDS_IN_TMR_UNIT1 & tmr_irq_src)
    {
        tmr_device_id = TMR_DEVICE_ID_1;
    }


    if ((TMR_TIMER_IDS_IN_TMR_UNIT0 & tmr_irq_src) && (TMR_TIMER_IDS_IN_TMR_UNIT1 & tmr_irq_src))
    {
        tmr_device_id = TMR_DEVICE_ID_INVALID;
    }

    return(tmr_device_id);
}

/********************************************************************************/
/* NAME:	 TMR_IsPendingIRQSrc()                                              */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This function allows to know if the IRQ Source is            */
/*				  active or not.t_tmr_irq_src is a single interrupt.            */
/* PARAMETERS    :											                    */
/* IN            : tmr_irq_src                                                  */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : t_bool												        */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_bool TMR_IsPendingIRQSrc(t_tmr_irq_src tmr_irq_src)
{
    if
    (
        (
            (
                TMR_GetIRQSrc(TMR_DEVICE_ID_0) |
                TMR_GetIRQSrc(TMR_DEVICE_ID_1) 
            ) & tmr_irq_src
        )
    )
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/* End of file - tmr_irq.c*/

