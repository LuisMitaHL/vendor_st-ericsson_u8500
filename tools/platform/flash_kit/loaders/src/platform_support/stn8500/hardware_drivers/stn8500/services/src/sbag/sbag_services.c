/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Services File for ST-Bus Analyser SBAG
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "services.h"
#include "gic.h"
/* Removing DMA & GPIO calls from PEPS */
#if !(defined __PEPS_8500)
#include "gpio.h"
#endif


#include "sbag.h"
#include "sbag_irq.h"
#include "sbag_services.h"

/****************************************************************************/
/* NAME:    SER_SBAG_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the sbag                            */
/*                                                                          */
/* PARAMETERS: 		                                                        */
/* IN:																		*/
/* 											                                */
/* OUT:                                                                     */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SBAG_Init(t_uint8 mask)
{
	t_sbag_error sbag_error;
	t_gic_error gic_error;
    t_gpio_error  error_gpio; 
	t_gpio_config config;
	t_gic_func_ptr old_datum;

	config.mode = GPIO_MODE_ALT_FUNCTION_C;
	config.trig = GPIO_TRIG_BOTH_EDGES;
	config.direction = GPIO_DIR_OUTPUT;
	config.debounce = GPIO_DEBOUNCE_UNCHANGED;
	config.level = GPIO_LEVEL_LEAVE_UNCHANGED;

		
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_70,config);
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_71,config);
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_72,config);
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_73,config);
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_74,config);
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_77,config);
	if(GPIO_OK != error_gpio)
	{
		PRINT("GPIO Settings Failed ");
		return;
	}
	(*(volatile t_uint32 *)(0x80157138))|= 0x100;
	sbag_error = SBAG_Init(SBAG_REG_BASE_ADDR);
	if(SBAG_OK != sbag_error)
	{
		PRINT("SBAG Initialization Failed");
		return;
	}
	
	SBAG_SetBaseAddress(SBAG_REG_BASE_ADDR);
				
	gic_error = GIC_DisableItLine(GIC_SBAG_LINE);
	if(GIC_OK != gic_error)
	{
		PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
		return;
	}
	gic_error = GIC_ChangeDatum(GIC_SBAG_LINE, SER_SBAG_InterruptHandler, &old_datum);
	if (GIC_OK != gic_error)
	{
		PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
		return;
	}
	gic_error = GIC_EnableItLine(GIC_SBAG_LINE);
	if(GIC_OK != gic_error)
	{
		PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
		return;
	}
	return;
}
/****************************************************************************/
/* NAME:    SER_SBAG_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the sbag                            */
/*                                                                          */
/* PARAMETERS: 		                                                        */
/* IN:																		*/
/* 											                                */
/* OUT:                                                                     */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SBAG_InterruptHandler(IN t_uint32 not_used)
{
	
	t_sbag_error sbag_error;
	t_sbag_irq_src sbag_irq_src;
	/*coverity[var_decl]*/
	t_sbag_irq_status sbag_irq_status;
	t_gic_error gic_error;
	t_sbag_device sbag_device; 
	
	not_used = not_used + 1; /*Done to remove Coverity Warning*/ 
	
	sbag_device = SBAG_GetSBAGCurrentDevice(); 
	switch(sbag_device)
	{
		case SBAG_WPSAT:
				sbag_irq_src = SBAG_WPSAT_IRQ_SRC_ALL;
				break;
		case SBAG_PI:
				sbag_irq_src = SBAG_PI_IRQ_SRC_ALL;
				break;
		case SBAG_TMSAT:
				sbag_irq_src = SBAG_TMSAT_IRQ_SRC_ALL;
				break;
		default: 
			return; 
	}
	
	SBAG_GetIRQSrcStatus(sbag_irq_src,&sbag_irq_status);

	gic_error = GIC_DisableItLine(GIC_SBAG_LINE);
	if(GIC_OK != gic_error)
	{
		PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
		return;
	}
	
	/*coverity[uninit_use_in_call]*/
	sbag_error = SBAG_ProcessIRQSrc(&sbag_irq_status);
	if(SBAG_OK != sbag_error)
	{
		PRINT("Error Processing the IRQ");
		return;
	}

	gic_error = GIC_EnableItLine(GIC_SBAG_LINE);
	if(GIC_OK != gic_error)
	{
		PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
		return;
	}
	return;
}

/****************************************************************************/
/* NAME:    SER_SBAG_Close			                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the sbag                            */
/*                                                                          */
/* PARAMETERS: 		                                                        */
/* IN:																		*/
/* 											                                */
/* OUT:                                                                     */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SBAG_Close(void)
{
	/*To be done*/
}

/*end of sbag_services.c*/
