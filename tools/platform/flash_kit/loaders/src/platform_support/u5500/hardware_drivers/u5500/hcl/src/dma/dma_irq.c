/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_irq.c
* This file contains the HCL irq functions
* -------------------------------------------------------------------------------------------- */

#include "dma.h"
#include "dma_irq.h"
#include "dma_base.h"
#include "dma_p.h"

/*------------------------------------------------------------------------
 * Global Variables                                   
 *------------------------------------------------------------------------*/

PRIVATE t_dma_register  *gp_dma_register;

/*******************************************************************************************/
/* NAME:  DMA_SetBaseAddress                                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function initialises the DMA Hardware base address for IRQ Management */
/* PARAMETERS:                                                                             */
/* IN:    dma_base_address : Base address of DMA                                           */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_SetBaseAddress(IN t_logical_address dma_base_address)
{
    gp_dma_register = (t_dma_register *)dma_base_address;
}

/*******************************************************************************************/
/* NAME:  DMA_EnableIRQSrc                                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables the Interrupts for DMA                               */
/* PARAMETERS:                                                                             */
/* IN:    dma_irq_src :Interrupt sources to be enabled(can be individual or combined)      */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_EnableIRQSrc(IN t_dma_chan_type chan_type,IN t_dma_irq_src dma_irq_src)
{
}

/*******************************************************************************************/
/* NAME:  DMA_DisableIRQSrc                                                                */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function disables the Interrupts for DMA                              */
/* PARAMETERS:                                                                             */
/* IN:    dma_irq_src :Interrupt sources to be disabled(can be individual or combined)     */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_DisableIRQSrc(IN t_dma_chan_type chan_type, IN t_dma_irq_src dma_irq_src)
{
}
/*******************************************************************************************/
/* NAME:  DMA_GetIRQSrcNonSecure                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets masked non secure interrupt status.                     */
/* PARAMETERS:                                                                             */
/* IN:    chan_type                                                                        */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: Interrupt sources(combined) which has occurred                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC t_dma_irq_src DMA_GetIRQSrcNonSecure(IN t_dma_chan_type chan_type)
{
    t_uint8 i = NULL;
    t_dma_irq_src intr = (t_dma_irq_src)DMA_NO_INTR;

    switch (chan_type) 
    {
        case DMA_CHAN_PHYSICAL_NOT_SECURE:

            if(gp_dma_register->dmac_pcmis)
            {
                for(i = (t_uint8)DMA_IRQ_SRC_0; i < (t_uint8)DMA_IRQ_SRC_8; i++)
                {
                    if(gp_dma_register->dmac_pcmis & (1UL << i))
                    {
                        intr = (t_dma_irq_src)i;
                        break;
                    }            
                }
            }

            break;    


        case DMA_CHAN_LOGICAL_NOT_SECURE:

                if(gp_dma_register->dmac_lcmis[0])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_0; i < (t_uint8)DMA_IRQ_SRC_32; i++)
                    {
                        if(gp_dma_register->dmac_lcmis[0] & (1UL << i))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }            
                    }
                }

                else if(gp_dma_register->dmac_lcmis[1])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_32; i < (t_uint8)DMA_IRQ_SRC_64; i++)    
                    {
                        if(gp_dma_register->dmac_lcmis[1] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
       
                else if(gp_dma_register->dmac_lcmis[2])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_64; i < (t_uint8)DMA_IRQ_SRC_96; i++)    
                    {
                        if(gp_dma_register->dmac_lcmis[2] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
        
                if(gp_dma_register->dmac_lcmis[3])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_96; i < (t_uint8)DMA_IRQ_SRC_128; i++)    
                    {
                        if(gp_dma_register->dmac_lcmis[3] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
        
            break;
    }
    return (intr);
}

/*******************************************************************************************/
/* NAME:  DMA_GetIRQSrcSecure                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets masked secure interrupt status.                         */
/* PARAMETERS:                                                                             */
/* IN:    chan_type                                                                        */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: Interrupt sources(combined) which has occurred                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC t_dma_irq_src DMA_GetIRQSrcSecure(IN t_dma_chan_type chan_type)
{
    t_uint8 i = 0;
    t_dma_irq_src intr = (t_dma_irq_src)DMA_NO_INTR;

    switch (chan_type) 
    {
        case DMA_CHAN_PHYSICAL_SECURE:
        
            if(gp_dma_register->dmac_spcmis)
            {
                for(i = (t_uint8)DMA_IRQ_SRC_0; i < (t_uint8)DMA_IRQ_SRC_8; i++)
                {
                    if(gp_dma_register->dmac_spcmis & (1UL << i))
                    {
                        intr = (t_dma_irq_src)i;
                        break;
                    }            
                }
            }
        
            break;
        
        case DMA_CHAN_LOGICAL_SECURE :
        
                if(gp_dma_register->dmac_slcmis[0])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_0; i < (t_uint8)DMA_IRQ_SRC_32; i++)
                    {
                        if(gp_dma_register->dmac_slcmis[0] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }            
                    }
                }
        
                else if(gp_dma_register->dmac_slcmis[1])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_32; i < (t_uint8)DMA_IRQ_SRC_64; i++)    
                    {
                        if(gp_dma_register->dmac_slcmis[1] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
        
                else if(gp_dma_register->dmac_slcmis[2])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_64; i < (t_uint8)DMA_IRQ_SRC_96; i++)    
                    {
                        if(gp_dma_register->dmac_slcmis[2] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
        
                if(gp_dma_register->dmac_slcmis[3])
                {
                    for(i = (t_uint8)DMA_IRQ_SRC_96; i < (t_uint8)DMA_IRQ_SRC_128; i++)    
                    {
                        if(gp_dma_register->dmac_slcmis[3] & (1UL << (i % DMA_MASK_WORD)))
                        {
                            intr = (t_dma_irq_src)i;
                            break;
                        }    
                    }
                }
            
                break;
    }
    return (intr);
}

/*******************************************************************************************/
/* NAME:  DMA_GetIRQSrcStatusNonSecure                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the non secure interrupt status                         */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_irq_src_id : interrupt source id                                           */
/*        t_dma_chan_type : channel type (Secured/Not secured) (Physical/Logical)          */
/* INOUT: None                                                                             */
/* OUT:   t_dma_irq_status : interrupt status information                                  */
/*                                                                                         */
/* RETURN: Interrupt sources(combined) which has occurred                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_GetIRQSrcStatusNonSecure(IN t_dma_chan_type chan_type,
                                         IN t_dma_irq_src irq_src, 
                                         OUT t_dma_irq_status *irq_status)
{

    switch(chan_type)
    {
        case DMA_CHAN_PHYSICAL_NOT_SECURE :
    
                if(gp_dma_register->dmac_pctis & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                     irq_status->tc_intr = TRUE;
                else
                     irq_status->tc_intr = FALSE;

                if(gp_dma_register->dmac_pceis & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                     irq_status->err_intr = TRUE;
                else
                     irq_status->err_intr = FALSE;
    
            break;
            
   
        case DMA_CHAN_LOGICAL_NOT_SECURE :
            
               if(gp_dma_register->dmac_lctis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                    irq_status->tc_intr = TRUE;
               else
                    irq_status->tc_intr = FALSE;

               if(gp_dma_register->dmac_lceis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                    irq_status->err_intr = TRUE;
               else
                    irq_status->err_intr = FALSE;
            break;
    }
}


/*******************************************************************************************/
/* NAME:  DMA_GetIRQSrcStatusSecure                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the  secure interrupt status                            */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_irq_src_id : interrupt source id                                           */
/*        t_dma_chan_type : channel type (Secured/Not secured) (Physical/Logical)          */
/* INOUT: None                                                                             */
/* OUT:   t_dma_irq_status : interrupt status                                              */
/*                                                                                         */
/* RETURN: Interrupt sources(combined) which has occurred                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_GetIRQSrcStatusSecure(IN t_dma_chan_type chan_type,
                                      IN t_dma_irq_src irq_src, 
                                      OUT t_dma_irq_status *irq_status)
{
    switch(chan_type)
    {
        case DMA_CHAN_PHYSICAL_SECURE :

            if(gp_dma_register->dmac_spctis & (1UL << ((t_uint8)irq_src  % DMA_MASK_WORD)))
                 irq_status->tc_intr = TRUE;
            else
                 irq_status->tc_intr = FALSE;

            if(gp_dma_register->dmac_spceis & (1UL << ((t_uint8)irq_src  % DMA_MASK_WORD)))
                 irq_status->err_intr = TRUE;
            else
                 irq_status->err_intr = FALSE;

            break;

        case DMA_CHAN_LOGICAL_SECURE:

                if(gp_dma_register->dmac_slctis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                     irq_status->tc_intr = TRUE;
                else
                     irq_status->tc_intr = FALSE;

                if(gp_dma_register->dmac_slceis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD)))
                     irq_status->err_intr = TRUE;
                else
                     irq_status->err_intr = FALSE;
            break;
    }
    
}

/*******************************************************************************************/
/* NAME:  DMA_IsIRQSrcTC                                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the TC interrupt status information.                    */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_irq_src_id : interrupt source id                                           */
/*        t_dma_chan_type : channel type (Secured/Not secured) (Physical/Logical)           */
/* INOUT: None                                                                             */
/* OUT:                                                                                         */
/*                                                                                         */
/* RETURN: t_bool :  TRUE or FALSE                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC t_bool DMA_IsIRQSrcTC( IN t_dma_chan_type chan_type ,IN t_dma_irq_src irq_src)
{
    switch(chan_type)
    {
        case DMA_CHAN_PHYSICAL_SECURE :

            return((t_bool)(gp_dma_register->dmac_spctis & (1UL << (t_uint8)irq_src)));
            
        case DMA_CHAN_PHYSICAL_NOT_SECURE :

            return((t_bool)(gp_dma_register->dmac_pctis & (1UL << (t_uint8)irq_src)));

        case DMA_CHAN_LOGICAL_SECURE :

            return((t_bool)(gp_dma_register->dmac_slctis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));

        case DMA_CHAN_LOGICAL_NOT_SECURE :

            return((t_bool)(gp_dma_register->dmac_slctis[(t_uint8)irq_src/DMA_MASK_WORD] & (1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));

    }
    return(TRUE);
}

/*******************************************************************************************/
/* NAME:  DMA_ClearIRQSrc                                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function clears the Interrupts for DMA                                */
/* PARAMETERS:                                                                             */
/* IN:    dma_irq_src :Interrupt sources to be cleared(can be individual or combined)      */
/*        chan_type   : channel type (Secured/not secured) (physical/logical)              */  
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */
/*******************************************************************************************/
PUBLIC void DMA_ClearIRQSrc(IN t_dma_chan_type chan_type , IN t_dma_irq_src irq_src)
{
    t_uint32 int_mask;

    int_mask = (t_uint8)irq_src % DMA_MASK_WORD;

    switch(chan_type)
    {
    case DMA_CHAN_PHYSICAL_NOT_SECURE:
        gp_dma_register->dmac_pcicr                         |=     (1UL << int_mask);
        break;

    case DMA_CHAN_LOGICAL_NOT_SECURE:
        gp_dma_register->dmac_lcicr[(t_uint8)irq_src/DMA_MASK_WORD]    |=     (1UL << int_mask);
        break;

    case DMA_CHAN_PHYSICAL_SECURE:
        gp_dma_register->dmac_spcicr                        |=     (1UL << int_mask);
        break;
        
    case DMA_CHAN_LOGICAL_SECURE:
        gp_dma_register->dmac_slcicr[(t_uint8)irq_src/DMA_MASK_WORD]     |=     (1UL << int_mask);
        break;
    }
}

/*******************************************************************************************/
/* NAME:  DMA_IsPendingIRQSrc                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function tells whether required the Interrupt(s) are active or not    */
/*              for DMA                                                                    */
/* PARAMETERS:                                                                             */
/* IN:    dma_irq_src :Interrupt sources(can be individual or combined)                    */
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
PUBLIC t_bool DMA_IsPendingIRQSrc(IN t_dma_chan_type chan_type, IN t_dma_irq_src irq_src)
{
    t_uint32 irq_status = 0;

    switch(chan_type)
    {
    case DMA_CHAN_PHYSICAL_NOT_SECURE:
        irq_status |= (t_uint32)(gp_dma_register->dmac_pcmis & ((t_uint32)(1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));
        break;

    case DMA_CHAN_LOGICAL_NOT_SECURE:
        irq_status |= (t_uint32)(gp_dma_register->dmac_lcmis[(t_uint8)irq_src/DMA_MASK_WORD] & ((t_uint32)(1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));
        break;

    case DMA_CHAN_PHYSICAL_SECURE:
        irq_status |= (t_uint32)(gp_dma_register->dmac_spcmis & ((t_uint32)(1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));
        break;
        
    case DMA_CHAN_LOGICAL_SECURE:
        irq_status |= (t_uint32)(gp_dma_register->dmac_slcmis[(t_uint8)irq_src/DMA_MASK_WORD] & ((t_uint32)(1UL << ((t_uint8)irq_src % DMA_MASK_WORD))));
        break;
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


