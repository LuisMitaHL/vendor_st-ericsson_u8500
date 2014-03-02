/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
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
#include "sbag_irqp.h"

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_sbag_register *gp_sbag_register;

/*--------------------------------------------------------------------------*
 * Global data for interrupt mode management                                *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
* Public functions                                                         	*
*---------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:  	SBAG_SetBaseAddress		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes SBAG register base address.	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	sbag_base_address     :  SBAG registers base address    */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	none													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/
/*			1)		Global variable p_sbag_register(register base address)	*/
/*					is being modified										*/

/****************************************************************************/
PUBLIC void
SBAG_SetBaseAddress(IN t_logical_address sbag_base_address)
{
    gp_sbag_register = (t_sbag_register *)sbag_base_address;
    return;
}

/****************************************************************************/
/* NAME			:  	SBAG_GetDeviceId		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the interrupt source.				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_device     :  The internal device causing   		*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	Device of SBAG causing the interrupt					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC t_sbag_device
SBAG_GetDeviceId(IN t_sbag_irq_src sbag_irq_src)
{
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_irq_src >> SBAG_SHIFT_BY_SIXTEEN);
    if (sbag_device > SBAG_TMSAT)
    {
        return (SBAG_TMSAT);
    }

    return (sbag_device);
}

/****************************************************************************/
/* NAME			:  	SBAG_EnableIRQSrc		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the interrupt source.				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_irq_src     :  Interrupts that will be enabled	*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	none													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC void
SBAG_EnableIRQSrc(IN t_sbag_irq_src sbag_irq_src)
{
    t_sbag_register *p_sbag_register = gp_sbag_register;

    /*Get the interrupts to be enabled*/
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_irq_src >> SBAG_SHIFT_BY_SIXTEEN);

    if (sbag_device > SBAG_TMSAT)
    {
        return;
    }

    sbag_irq_src = sbag_irq_src & SBAG_GET_INTERRUPTS;  /*Just get the Interrupts so that registers can be written*/

    switch (sbag_device)
    {
        case SBAG_WPSAT:
            p_sbag_register->wpsat_itm_set |= sbag_irq_src;
            break;

        case SBAG_PI:
            p_sbag_register->pi_itm_set |= sbag_irq_src;
            break;

        case SBAG_TMSAT:
            p_sbag_register->tmsat_itm_set |= sbag_irq_src;
            break;

        default:
            return;
    }
}

/****************************************************************************/
/* NAME			:  	SBAG_DisableIRQSrc		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the interrupt source.				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_irq_src     :  Interrupts that will be disabled	*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	none													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC void
SBAG_DisableIRQSrc(IN t_sbag_irq_src sbag_it_to_disable)
{
    t_sbag_register *p_sbag_register = gp_sbag_register;

    /*Get the interrupts to be disabled*/
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_it_to_disable >> SBAG_SHIFT_BY_SIXTEEN);

    if (sbag_device > SBAG_TMSAT)
    {
        return;
    }

    sbag_it_to_disable = sbag_it_to_disable & SBAG_GET_INTERRUPTS;  /*Get the interrupts to be disabled*/

    switch (sbag_device)
    {
        case SBAG_WPSAT:
            p_sbag_register->wpsat_itm_set = (p_sbag_register->wpsat_itm_set &~(sbag_it_to_disable));
            break;

        case SBAG_PI:
            p_sbag_register->pi_itm_set = (p_sbag_register->pi_itm_set &~(sbag_it_to_disable));
            break;

        case SBAG_TMSAT:
            p_sbag_register->tmsat_itm_set = (p_sbag_register->tmsat_itm_set &~(sbag_it_to_disable));
            break;

        default:
            return;
    }
}

/****************************************************************************/
/* NAME			:  	SBAG_GetIRQSrc			                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the irq source from itm registers.	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_device     :  The device in SBAG whose interrupt	*/
/*										 status is to be known				*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	none													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC t_sbag_irq_src
SBAG_GetIRQSrc(IN t_sbag_device sbag_device)
{
    t_sbag_register *p_sbag_register = gp_sbag_register;

    if (sbag_device > SBAG_TMSAT)
    {
        return (SBAG_TMSAT);
    }

    switch (sbag_device)
    {
        case SBAG_WPSAT:
            return (((t_sbag_irq_src) p_sbag_register->wpsat_itm) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN));

        case SBAG_PI:
            return (((t_sbag_irq_src) p_sbag_register->pi_itm) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN));

        case SBAG_TMSAT:
            return (((t_sbag_irq_src) p_sbag_register->tmsat_itm) | (((t_uint32) sbag_device) << SBAG_SHIFT_BY_SIXTEEN));

        default:
            return (SBAG_TMSAT);
    }
}

/****************************************************************************/
/* NAME			:  	SBAG_ClearIRQSrc		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the irq source from itm registers.	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_irq_src     :  The interrupts to be cleared 		*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	none													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC void
SBAG_ClearIRQSrc(IN t_sbag_irq_src sbag_it_to_clear)
{
    t_sbag_register *p_sbag_register = gp_sbag_register;

    /*Get the Interrupts to be cleared */
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_it_to_clear >> SBAG_SHIFT_BY_SIXTEEN);

    if (sbag_device > SBAG_TMSAT)
    {
        return;
    }

    sbag_it_to_clear = sbag_it_to_clear & SBAG_GET_INTERRUPTS;  /*Get the interrupts to be cleared*/

    switch (sbag_device)
    {
        case SBAG_WPSAT:
            p_sbag_register->wpsat_itm_clr = p_sbag_register->wpsat_itm_clr | sbag_it_to_clear;
            break;

        case SBAG_PI:
            p_sbag_register->pi_itm_clr = p_sbag_register->pi_itm_clr | sbag_it_to_clear;
            break;

        case SBAG_TMSAT:
            p_sbag_register->wpsat_itm_clr = p_sbag_register->tmsat_itm_clr | sbag_it_to_clear;
            break;

        default:
            return;
    }
}

/****************************************************************************/
/* NAME			:  	SBAG_IsPendingIRQSrc		                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine returns the irq source from itm registers.	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	t_sbag_irq_src     :  Interrupt ID to be checked for 	*/
/*										  Pending Status					*/
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	TRUE/FALSE												*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:	None												*/
/*																			*/

/****************************************************************************/
PUBLIC t_bool
SBAG_IsPendingIRQSrc(t_sbag_irq_src sbag_pending_its)
{
    t_sbag_register *p_sbag_register = gp_sbag_register;

    /*Get the device from the src*/
    t_sbag_device   sbag_device = (t_sbag_device) (sbag_pending_its >> SBAG_SHIFT_BY_SIXTEEN);
    if (sbag_device > SBAG_TMSAT)
    {
        return (FALSE);
    }

    /*Get the interrupts to be checked for */
    sbag_pending_its = sbag_pending_its & SBAG_GET_INTERRUPTS;

    switch (sbag_device)
    {
        case SBAG_WPSAT:
            if ((p_sbag_register->wpsat_itm | sbag_pending_its) != 0)
            {
                return (TRUE);
            }
            else
            {
                return (FALSE);
            }

        case SBAG_PI:
            if ((p_sbag_register->pi_itm | sbag_pending_its) != 0)
            {
                return (TRUE);
            }
            else
            {
                return (FALSE);
            }

        case SBAG_TMSAT:
            if ((p_sbag_register->pi_itm | sbag_pending_its) != 0)
            {
                return (TRUE);
            }
            else
            {
                return (FALSE);
            }

        default:
            return (FALSE);
    }
}

/*End of File - sbag_irq.c*/
