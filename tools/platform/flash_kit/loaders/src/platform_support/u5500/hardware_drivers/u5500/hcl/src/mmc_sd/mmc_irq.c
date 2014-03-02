/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* This module provides some support routines for the MultiMedia Card
* SD-card host Interface.
*
* author : ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*/
/*      Includes                                                                 */
/*--------------------------------------------------------------------------*/
#include "mmc_irq.h"
#include "mmc_irqp.h"

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_mmc_register  *gp_mmc_register[NUM_MMC_INSTANCES];

/****************************************************************************/
/*       NAME : MMC_SetBaseAddress()                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the MMC registers and doesn't check*/
/*              Peripheral and PCell Id.                                    */
/* PARAMETERS :                                                             */
/*        IN : t_logical_address mmc_base_address:MMC registers base address*/
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                        */

/****************************************************************************/
PUBLIC void MMC_SetBaseAddress(IN t_logical_address mmc_base_address, IN t_mmc_device_id mmc_device_id)
{
    gp_mmc_register[mmc_device_id] = (t_mmc_register *) mmc_base_address;
}

/****************************************************************************/
/*       NAME : MMC_EnableIRQSrc()                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables single or combined interrupt sources.  */
/* PARAMETERS :                                                             */
/*         IN : t_uint32 irqsrc: ORed value of interrupt sources to be      */
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*               enabled                                                    */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                        */

/****************************************************************************/
PUBLIC void MMC_EnableIRQSrc(IN t_mmc_irq_src irqsrc, IN t_mmc_device_id mmc_device_id)
{
    gp_mmc_register[mmc_device_id]->mmc_mask0 |= irqsrc;
}

/****************************************************************************/
/*       NAME : MMC_DisableIRQSrc()                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables single or combined interrupt sources. */
/* PARAMETERS :                                                             */
/*         IN : t_uint32 irqsrc: ORed value of interrupt sources to be      */
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*               disabled                                                   */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN :  void                                                       */

/****************************************************************************/
PUBLIC void MMC_DisableIRQSrc(IN t_mmc_irq_src irqsrc, IN t_mmc_device_id mmc_device_id)
{
    gp_mmc_register[mmc_device_id]->mmc_mask0 &= ~irqsrc;
}

/****************************************************************************/
/*       NAME : MMC_GetIRQSrc()                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the ORed value of all the active       */
/*              interrupt sources                                           */
/* PARAMETERS :                                                             */
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : t_uint32: ORed value of all the active interrupt sources    */

/****************************************************************************/
PUBLIC t_mmc_irq_src MMC_GetIRQSrc(IN t_mmc_device_id mmc_device_id)
{
    return(gp_mmc_register[mmc_device_id]->mmc_status & MMCALLINTERRUPTS);
}

/****************************************************************************/
/*       NAME : MMC_ClearIRQSrc()                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears single or combined interrupt sources.   */
/* PARAMETERS :                                                             */
/*         IN : t_uint32 irqsrc: ORed value of interrupt sources to be      */
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*               cleared                                                    */
/*        OUT : NONE                                                        */
/*                                                                          */
/*     RETURN : void                                                        */

/****************************************************************************/
PUBLIC void MMC_ClearIRQSrc(IN t_mmc_irq_src irqsrc, IN t_mmc_device_id mmc_device_id)
{
    gp_mmc_register[mmc_device_id]->mmc_clear |= irqsrc;
}

/****************************************************************************/
/*       NAME : MMC_is_pendingIRQSrc()                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine tells whether any one of the combined          */
/*              interrupt sources is active or not.                         */
/* PARAMETERS :                                                             */
/*         IN : t_uint32 irqsrc: ORed value of interrupt sources            */
/*        IN : t_mmc_device_id mmc_device_id:Device Id for MMC Instance Used*/
/*        OUT :                                                             */
/*                                                                          */
/*     RETURN : t_bool: the status. TRUE => active                          */

/****************************************************************************/
PUBLIC t_bool MMC_IsPendingIRQSrc(IN t_mmc_irq_src irqsrc, IN t_mmc_device_id mmc_device_id)
{
    if (gp_mmc_register[mmc_device_id]->mmc_status & irqsrc)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}
