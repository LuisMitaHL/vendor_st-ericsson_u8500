/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE IRQ file
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "mcde_irq.h"
#include "mcde.h"
#include "mcde_p.h"
#include "hcl_defs.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_mcde_register *gp_mcde_registers;

/****************************************************************************
* NAME:	 MCDE_SetBaseAddress()												
*---------------------------------------------------------------------------
* DESCRIPTION   : This routine initializes MCDE HCL.		
* PARAMETERS    :											
* IN            : t_logical_address  base_address 	
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none											
* TYPE       : Public
*---------------------------------------------------------------------------
* REENTRANCY: NA														
*****************************************************************************/
PUBLIC void MCDE_SetBaseAddress(t_logical_address base_address)
{
    /* initializating the MCDE base address */
    gp_mcde_registers = (t_mcde_register *) base_address;
}

/***************************************************************************
* NAME:     MCDE_EnableIRQSrc()
*---------------------------------------------------------------------------
* DESCRIPTION   :This routine allows to enable a specific interrupt
* PARAMETERS    : 											
* IN            : irq_src: ORed value of interrupt sources to be enabled.
*							(can also be used to enable all interrupts)
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none
* TYPE          : Public
*--------------------------------------------------------------------------
*REENTRANCY: NA														
*****************************************************************************/
#ifdef ST_8500ED
PUBLIC void MCDE_EnableIRQSrc(t_uint32 irq_src)
{
    gp_mcde_registers->mcde_imsc |= irq_src;

}

#else

PUBLIC void MCDE_EnableIRQSrc(t_mcde_irq_type irq_type,t_uint32 irq_src)
{
	switch(irq_type)
	{
		case MCDE_IRQ_TYPE_PIXELPROCESSING:
			gp_mcde_registers->mcde_imscpp |= irq_src;
			break;
			
		case MCDE_IRQ_TYPE_OVERLAY:
			gp_mcde_registers->mcde_imscovl |= irq_src;
			break;
			
		case MCDE_IRQ_TYPE_CHANNEL:
			gp_mcde_registers->mcde_imscchnl |= irq_src;
			break;
			
		case MCDE_IRQ_TYPE_ERROR:
			gp_mcde_registers->mcde_imscerr |= irq_src >> 4;
			break;
			
		default:
			break;
	}    
}
#endif
/***************************************************************************
* NAME:     MCDE_DisableIRQSrc()
*---------------------------------------------------------------------------
* DESCRIPTION   :This routine allows to disable a specific interrupt
* PARAMETERS    : 											
* IN            : irq_src: ORed value of interrupt sources to be disabled.
*							(can also be used to enable all interrupts)
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none
* TYPE          : Public
*--------------------------------------------------------------------------
*REENTRANCY: NA														
*****************************************************************************/
#ifdef ST_8500ED

PUBLIC void MCDE_DisableIRQSrc(t_uint32 irq_src)
{
    gp_mcde_registers->mcde_imsc &= ~irq_src;
}

#else
PUBLIC void MCDE_DisableIRQSrc(t_mcde_irq_type irq_type,t_uint32 irq_src)
{
    switch(irq_type)
	{
		case MCDE_IRQ_TYPE_PIXELPROCESSING:
			gp_mcde_registers->mcde_imscpp |= ~irq_src;
			break;
			
		case MCDE_IRQ_TYPE_OVERLAY:
			gp_mcde_registers->mcde_imscovl |= ~irq_src;
			break;
			
		case MCDE_IRQ_TYPE_CHANNEL:
			gp_mcde_registers->mcde_imscchnl |= ~irq_src;
			break;
			
		case MCDE_IRQ_TYPE_ERROR:
			gp_mcde_registers->mcde_imscerr |= ~(irq_src >>4);
			break;
			
		default:
			break;
	}
}
#endif
/***************************************************************************
*NAME:     MCDE_GetIRQSrc()												   *
*--------------------------------------------------------------------------*
*DESCRIPTION   :This routine allows to read it status					   *
*(only valid for enabled IT)											   *
*PARAMETERS    : 														   *
*IN            : None													   *
*INOUT         : None													   *
*OUT           : itstatus: it status for all interrupts					   *
*RETURN VALUE  :ORed value of all the active interrupt sources			   *
*TYPE          : Public													   *
*--------------------------------------------------------------------------*
*REENTRANCY: NA															   *
****************************************************************************/
#ifdef ST_8500ED

PUBLIC t_uint32 MCDE_GetIRQSrc(void)
{
    return(HCL_READ_REG(gp_mcde_registers->mcde_mis));

}

#else

PUBLIC t_uint32 MCDE_GetIRQSrc(t_mcde_irq_type irq_type)
{	
	if(MCDE_IRQ_TYPE_PIXELPROCESSING == irq_type)
		return(HCL_READ_REG(gp_mcde_registers->mcde_mispp));
	else if(MCDE_IRQ_TYPE_OVERLAY == irq_type)
		return(HCL_READ_REG(gp_mcde_registers->mcde_misovl));
	else if(MCDE_IRQ_TYPE_CHANNEL == irq_type)
		return(HCL_READ_REG(gp_mcde_registers->mcde_mischnl));
	else
		return(HCL_READ_REG(gp_mcde_registers->mcde_miserr));
	
}
#endif
/****************************************************************************
* NAME:     MCDE_ClearIRQSrc()												*
*---------------------------------------------------------------------------*
* DESCRIPTION   :This routine allows to clear interrupt status 				*
*(clear in fact raw status)													*
* PARAMETERS    : 															*
* IN            : irq_src: ORed value of interrupt sourcesto be cleared 	*
*					(all interrupts can also be cleared together)			*
* INOUT         : None														*
* OUT           : None														*
* RETURN VALUE  : none: 													*
* TYPE          : Public													*
*---------------------------------------------------------------------------*
*REENTRANCY: Non Reentrant													*
*****************************************************************************/
#ifdef ST_8500ED
PUBLIC void MCDE_ClearIRQSrc(t_uint32 irq_src)
{
    gp_mcde_registers->mcde_ris &= irq_src ;
}

#else

PUBLIC void MCDE_ClearIRQSrc(t_mcde_irq_type irq_type,t_uint32 irq_src)
{

	if(MCDE_IRQ_TYPE_PIXELPROCESSING == irq_type)
		gp_mcde_registers->mcde_rispp &= irq_src;
	else if(MCDE_IRQ_TYPE_OVERLAY == irq_type)
		gp_mcde_registers->mcde_risovl &= irq_src;
	else if(MCDE_IRQ_TYPE_CHANNEL == irq_type)
		gp_mcde_registers->mcde_rischnl &= irq_src;
	else
		gp_mcde_registers->mcde_riserr &= irq_src >> 4;
	
}
#endif
/****************************************************************************
* NAME:     MCDE_IsPendingIRQSrc()											*
*---------------------------------------------------------------------------*
* DESCRIPTION   :This routine allows to check the interrupt status of a     *
* 				 single irq													*
* PARAMETERS    : 															*
* IN            : irq_src: ORed value of interrupt sources      			*
* INOUT         : None														*
* OUT           : None														*
* RETURN VALUE  : none: 													*
* TYPE          : Public													*
*---------------------------------------------------------------------------*
*REENTRANCY: Re-entrant														*
*****************************************************************************/
PUBLIC t_bool MCDE_IsPendingIRQSrc(t_uint32 irq_src)
{
	#ifdef ST_8500ED
	  if (HCL_READ_BITS(gp_mcde_registers->mcde_mis, irq_src))
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
    #else
    
	return(FALSE);
	
	#endif

}

