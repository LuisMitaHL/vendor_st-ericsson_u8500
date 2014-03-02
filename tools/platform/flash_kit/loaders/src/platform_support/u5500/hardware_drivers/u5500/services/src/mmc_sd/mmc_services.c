/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* This provides initialization services for MMC / SD
* 
*
* author : ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/

#include "hcl_defs.h" 
#include "memory_mapping.h"
#include "services.h"
#include "mmc_services.h"
#include "dma_services.h"
#include "gpio_services.h"
#include "gpio.h"

#include "gic.h"
#include "mmc_irq.h"
#include "mmc.h"
#include "dma.h"






#define MIN_CLOCK_DIV 0x10

//#define SDI_4_REG_BASE_ADDR 0x80110000

PRIVATE t_callback g_mmc_callback = {0,0};

/****************************************************************************/
/* NAME:    SER_MMC_0_InterruptHandler                              	            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MMC  	          		*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_MMC_0_InterruptHandler(t_uint32  irq) 
{
#if defined (__MMCI_HCL_ENHANCED)	
	t_mmc_error	 error;
	t_mmc_irq_status itstatus;
	t_mmc_event eventstatus;
	t_bool done;
	t_ser_mmc_param mmc_param;
	t_gic_error gic_error;
    
  	MMC_GetIRQSrcStatus(MMC_IRQ_DATA_CRC_FAIL | MMC_IRQ_DATA_TIMEOUT | \
		              MMC_IRQ_DATA_END | MMC_IRQ_RX_FIFO_HALF_FULL | \
		              MMC_IRQ_TX_FIFO_HALF_EMPTY | MMC_IRQ_RX_OVERRUN | \
		              MMC_IRQ_TX_UNDERRUN, &itstatus,MMC_DEVICE_ID_0);
  
  	gic_error = GIC_DisableItLine(GIC_SD_MMC_0_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_DisableItLine() %d \n",gic_error);
  		return;
  	}
  	  
  	do 
  	{
  		error = MMC_FilterProcessIRQSrc(&itstatus, &eventstatus, NO_FILTER_MODE,MMC_DEVICE_ID_0);
	 	done = (t_bool)FALSE;
	 	switch(error)
	 	{
	 		case 	MMC_NO_PENDING_EVENT_ERROR:
	 		case 	MMC_NO_MORE_PENDING_EVENT:
	 		case 	MMC_NO_MORE_FILTER_PENDING_EVENT:
	 				done = (t_bool)TRUE;
	 				break;
	 		
	 		case 	MMC_REMAINING_PENDING_EVENTS:
	 				if (g_mmc_callback.fct != NULL)
	                {
	                    mmc_param.event = eventstatus;
	                    g_mmc_callback.fct(g_mmc_callback.param, &mmc_param);
	                }
		 			MMC_AcknowledgeEvent(&eventstatus,MMC_DEVICE_ID_0);
			 		break;
			 		
	 		case 	MMC_INTERNAL_EVENT:
			 		break;
	 		default:
	 				PRINT("INVALID IRQ SRC\n");
			 		break;
	 	} 
  	} while(!done);

  	gic_error = GIC_EnableItLine(GIC_SD_MMC_0_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_EnableItLine() %d \n",gic_error);
  		return;
  	}

	#endif
}

/****************************************************************************/
/* NAME:    SER_MMC_1_InterruptHandler                              	            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MMC  	          		*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_MMC_1_InterruptHandler(t_uint32  irq) 
{
#if defined (__MMCI_HCL_ENHANCED)	
	t_mmc_error	 error;
	t_mmc_irq_status itstatus;
	t_mmc_event eventstatus;
	t_bool done;
	t_ser_mmc_param mmc_param;
	t_gic_error gic_error;
    
  	MMC_GetIRQSrcStatus(MMC_IRQ_DATA_CRC_FAIL | MMC_IRQ_DATA_TIMEOUT | \
		              MMC_IRQ_DATA_END | MMC_IRQ_RX_FIFO_HALF_FULL | \
		              MMC_IRQ_TX_FIFO_HALF_EMPTY | MMC_IRQ_RX_OVERRUN | \
		              MMC_IRQ_TX_UNDERRUN, &itstatus,MMC_DEVICE_ID_1);
  
  	gic_error = GIC_DisableItLine(GIC_SD_MMC_1_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_DisableItLine() %d \n",gic_error);
  		return;
  	}
    
  	do 
  	{
  		error = MMC_FilterProcessIRQSrc(&itstatus, &eventstatus, NO_FILTER_MODE,MMC_DEVICE_ID_1);
	 	done = (t_bool)FALSE;
	 	switch(error)
	 	{
	 		case 	MMC_NO_PENDING_EVENT_ERROR:
	 		case 	MMC_NO_MORE_PENDING_EVENT:
	 		case 	MMC_NO_MORE_FILTER_PENDING_EVENT:
	 				done = (t_bool)TRUE;
	 				break;
	 		
	 		case 	MMC_REMAINING_PENDING_EVENTS:
	 				if (g_mmc_callback.fct != NULL)
	                {
	                    mmc_param.event = eventstatus;
	                    g_mmc_callback.fct(g_mmc_callback.param, &mmc_param);
	                }
		 			MMC_AcknowledgeEvent(&eventstatus,MMC_DEVICE_ID_1);
			 		break;
			 		
	 		case 	MMC_INTERNAL_EVENT:
			 		break;
	 		default:
	 				PRINT("INVALID IRQ SRC\n");
			 		break;
	 	} 
  	} while(!done);

  	
  	gic_error = GIC_EnableItLine(GIC_SD_MMC_1_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_EnableItLine() %d \n",gic_error);
  		return;
  	}

	#endif
}

/****************************************************************************/
/* NAME:    SER_MMC_2_InterruptHandler                              	            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MMC  	          		*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_MMC_2_InterruptHandler(t_uint32  irq) 
{
#if defined (__MMCI_HCL_ENHANCED)	
	t_mmc_error	 error;
	t_mmc_irq_status itstatus;
	t_mmc_event eventstatus;
	t_bool done;
	t_ser_mmc_param mmc_param;
	t_gic_error gic_error;
    
  	MMC_GetIRQSrcStatus(MMC_IRQ_DATA_CRC_FAIL | MMC_IRQ_DATA_TIMEOUT | \
		              MMC_IRQ_DATA_END | MMC_IRQ_RX_FIFO_HALF_FULL | \
		              MMC_IRQ_TX_FIFO_HALF_EMPTY | MMC_IRQ_RX_OVERRUN | \
		              MMC_IRQ_TX_UNDERRUN, &itstatus,MMC_DEVICE_ID_2);
  
  	gic_error = GIC_DisableItLine(GIC_SD_MMC_2_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_DisableItLine() %d \n",gic_error);
  		return;
  	}
    
  	do 
  	{
  		error = MMC_FilterProcessIRQSrc(&itstatus, &eventstatus, NO_FILTER_MODE,MMC_DEVICE_ID_2);
	 	done = (t_bool)FALSE;
	 	switch(error)
	 	{
	 		case 	MMC_NO_PENDING_EVENT_ERROR:
	 		case 	MMC_NO_MORE_PENDING_EVENT:
	 		case 	MMC_NO_MORE_FILTER_PENDING_EVENT:
	 				done = (t_bool)TRUE;
	 				break;
	 		
	 		case 	MMC_REMAINING_PENDING_EVENTS:
	 				if (g_mmc_callback.fct != NULL)
	                {
	                    mmc_param.event = eventstatus;
	                    g_mmc_callback.fct(g_mmc_callback.param, &mmc_param);
	                }
		 			MMC_AcknowledgeEvent(&eventstatus,MMC_DEVICE_ID_2);
			 		break;
			 		
	 		case 	MMC_INTERNAL_EVENT:
			 		break;
	 		default:
	 				PRINT("INVALID IRQ SRC\n");
			 		break;
	 	} 
  	} while(!done);

  	gic_error = GIC_EnableItLine(GIC_SD_MMC_2_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_EnableItLine() %d \n",gic_error);
  		return;
  	}

	#endif
}

/****************************************************************************/
/* NAME:    SER_MMC_3_InterruptHandler                              	            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MMC  	          		*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_MMC_3_InterruptHandler(t_uint32  irq) 
{
#if defined (__MMCI_HCL_ENHANCED)	
	t_mmc_error	 error;
	t_mmc_irq_status itstatus;
	t_mmc_event eventstatus;
	t_bool done;
	t_ser_mmc_param mmc_param;
	t_gic_error gic_error;
    
  	MMC_GetIRQSrcStatus(MMC_IRQ_DATA_CRC_FAIL | MMC_IRQ_DATA_TIMEOUT | \
		              MMC_IRQ_DATA_END | MMC_IRQ_RX_FIFO_HALF_FULL | \
		              MMC_IRQ_TX_FIFO_HALF_EMPTY | MMC_IRQ_RX_OVERRUN | \
		              MMC_IRQ_TX_UNDERRUN, &itstatus,MMC_DEVICE_ID_3);
  
  	gic_error = GIC_DisableItLine(GIC_SD_MMC_3_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_DisableItLine() %d \n",gic_error);
  		return;
  	}
    
  	do 
  	{
  		error = MMC_FilterProcessIRQSrc(&itstatus, &eventstatus, NO_FILTER_MODE,MMC_DEVICE_ID_3);
	 	done = (t_bool)FALSE;
	 	switch(error)
	 	{
	 		case 	MMC_NO_PENDING_EVENT_ERROR:
	 		case 	MMC_NO_MORE_PENDING_EVENT:
	 		case 	MMC_NO_MORE_FILTER_PENDING_EVENT:
	 				done = (t_bool)TRUE;
	 				break;
	 		
	 		case 	MMC_REMAINING_PENDING_EVENTS:
	 				if (g_mmc_callback.fct != NULL)
	                {
	                    mmc_param.event = eventstatus;
	                    g_mmc_callback.fct(g_mmc_callback.param, &mmc_param);
	                }
		 			MMC_AcknowledgeEvent(&eventstatus,MMC_DEVICE_ID_3);
			 		break;
			 		
	 		case 	MMC_INTERNAL_EVENT:
			 		break;
	 		default:
	 				PRINT("INVALID IRQ SRC\n");
			 		break;
	 	} 
  	} while(!done);

  	gic_error = GIC_EnableItLine(GIC_SD_MMC_3_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_EnableItLine() %d \n",gic_error);
  		return;
  	}

	#endif
}


/****************************************************************************/
/* NAME:    SER_MMC_4_InterruptHandler                              	            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MMC  	          		*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_MMC_4_InterruptHandler(t_uint32  irq) 
{
#if defined (__MMCI_HCL_ENHANCED)	
	t_mmc_error	 error;
	t_mmc_irq_status itstatus;
	t_mmc_event eventstatus;
	t_bool done;
	t_ser_mmc_param mmc_param;
	t_gic_error gic_error;
    
  	MMC_GetIRQSrcStatus(MMC_IRQ_DATA_CRC_FAIL | MMC_IRQ_DATA_TIMEOUT | \
		              MMC_IRQ_DATA_END | MMC_IRQ_RX_FIFO_HALF_FULL | \
		              MMC_IRQ_TX_FIFO_HALF_EMPTY | MMC_IRQ_RX_OVERRUN | \
		              MMC_IRQ_TX_UNDERRUN, &itstatus,MMC_DEVICE_ID_4);
  
  	gic_error = GIC_DisableItLine(GIC_SD_MMC_4_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_DisableItLine() %d \n",gic_error);
  		return;
  	}
    
  	do 
  	{
  		error = MMC_FilterProcessIRQSrc(&itstatus, &eventstatus, NO_FILTER_MODE,MMC_DEVICE_ID_4);
	 	done = (t_bool)FALSE;
	 	switch(error)
	 	{
	 		case 	MMC_NO_PENDING_EVENT_ERROR:
	 		case 	MMC_NO_MORE_PENDING_EVENT:
	 		case 	MMC_NO_MORE_FILTER_PENDING_EVENT:
	 				done = (t_bool)TRUE;
	 				break;
	 		
	 		case 	MMC_REMAINING_PENDING_EVENTS:
	 				if (g_mmc_callback.fct != NULL)
	                {
	                    mmc_param.event = eventstatus;
	                    g_mmc_callback.fct(g_mmc_callback.param, &mmc_param);
	                }
		 			MMC_AcknowledgeEvent(&eventstatus,MMC_DEVICE_ID_4);
			 		break;
			 		
	 		case 	MMC_INTERNAL_EVENT:
			 		break;
	 		default:
	 				PRINT("INVALID IRQ SRC\n");
			 		break;
	 	} 
  	} while(!done);

  	gic_error = GIC_EnableItLine(GIC_SD_MMC_4_LINE);
  	if (GIC_OK != gic_error)
  	{
  		PRINT("SER_MMC_InterruptHandler : GIC_EnableItLine() %d \n",gic_error);
  		return;
  	}

	#endif
}







/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/

/************************************************************************************/
/* NAME:    SER_MMC_RegisterCallback                                           		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                          		*/
/* PARAMETERS: t_callback_fct fct: function name.                             		*/
/*			   void* param: param for user identification.							*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           		*/
/************************************************************************************/
PUBLIC t_ser_error SER_MMC_RegisterCallback(t_callback_fct fct, void * param)
{
	if (fct== NULL) 
	{
		PRINT("NULL CALLBACK FUNCTION\n");
		return SERVICE_FAILED;
	}
    g_mmc_callback.fct = fct;
    g_mmc_callback.param = param;
    
    return SERVICE_OK;
    
}
/************************************************************************************/
/* NAME:    SER_MMC_WaitEnd                                            				*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine provides wait loop till the transfer gets ended.		*/
/*                                                                          		*/
/* PARAMETERS: none		                                                        	*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           		*/
/************************************************************************************/
PUBLIC t_ser_error SER_MMC_WaitEnd(IN t_mmc_device_id mmc_device_id)
{
	
#if defined(__MMCI_HCL_ENHANCED)
	t_uint32 timeout = 0x0FFFFFF;
	
	while((MMC_TRANSFER_IN_PROGRESS == MMC_GetTransferState(mmc_device_id)) && (timeout != 0))
	{
		timeout--;
	}
	if (0 == timeout)
	{
		return SERVICE_FAILED;
	}
	else
	{
		return SERVICE_OK;
	}

#else 
	return SERVICE_OK;
	
#endif	
		
}

/*****************************************************************************/
/* NAME:    SER_MMC_Init                                                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the mmc HCL and binds to relevant ISR*/
/*                                                                           */
/* PARAMETERS: void                                                          */
/*                                                                           */
/* RETURN: int                                                               */
/*              															 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/
PUBLIC void SER_MMC_Init( t_uint8 mmc_services_mask) 
{
	t_mmc_error mmc_error;
	t_gic_error gic_error;
	t_gic_func_ptr old_datum;
	//t_uint32 i;

	t_gpio_error gpio_error;
	 switch(mmc_services_mask)
	{
     case INIT_MMC0:

		 PRCC_PERI1_SDI0_BUS_CLKEN;
		 PRCC_PERI1_SDI0_KBUS_CLKEN;


		 gpio_error = GPIO_EnableAltFunction(GPIO_ALT_MM_CARD0);
		 if (GPIO_OK != gpio_error)
		 {
			 PRINT("SER_MMC_Init(): GPIO_EnableAltFunction %d\n",gpio_error);
		 }

            SER_mmc0_gpio_setting();
				PRCMU_MMC_SET;
				/* Initialize the base address of MMC-SD */
				mmc_error = MMC_Init(SDI_0_REG_BASE_ADDR , MMC_DEVICE_ID_0);
				if (MMC_OK != mmc_error)
				{
					PRINT("SER_MMC_Init():MMC_Init() %d \n",mmc_error);
					return;
				}
	
				MMC_SetBaseAddress(SDI_0_REG_BASE_ADDR,MMC_DEVICE_ID_0);
				
				gic_error = GIC_DisableItLine(GIC_SD_MMC_0_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
					return;
				}
				gic_error = GIC_ChangeDatum(GIC_SD_MMC_0_LINE, SER_MMC_0_InterruptHandler, &old_datum);
				if (GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
					return;
				}
				gic_error = GIC_EnableItLine(GIC_SD_MMC_0_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
					return;
				}
			break;
	 case INIT_MMC1:

		 PRCC_PERI2_SDI1_BUS_CLKEN;
		 PRCC_PERI2_SDI1_KBUS_CLKEN;

		 gpio_error = GPIO_EnableAltFunction(GPIO_ALT_MM_CARD1);
		 if (GPIO_OK != gpio_error)
		 {
			 PRINT("SER_MMC_Init(): GPIO_EnableAltFunction %d\n",gpio_error);
		 }
					SER_mmc1_gpio_setting();
				PRCMU_MMC_SET;
				/* Initialize the base address of MMC-SD */
				mmc_error = MMC_Init(SDI_1_REG_BASE_ADDR, MMC_DEVICE_ID_1);
				if (MMC_OK != mmc_error)
				{
					PRINT("SER_MMC_Init():MMC_Init() %d \n",mmc_error);
					return;
				}
	
				MMC_SetBaseAddress(SDI_1_REG_BASE_ADDR,MMC_DEVICE_ID_1);
				
				gic_error = GIC_DisableItLine(GIC_SD_MMC_1_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
					return;
				}
				gic_error = GIC_ChangeDatum(GIC_SD_MMC_1_LINE, SER_MMC_1_InterruptHandler, &old_datum);
				if (GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
					return;
				}
				gic_error = GIC_EnableItLine(GIC_SD_MMC_1_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
					return;
				}
				break;
	 case INIT_MMC2:


   		PRCC_PERI1_SDI2_BUS_CLKEN;
		PRCC_PERI1_SDI2_KBUS_CLKEN;

        
         gpio_error = GPIO_EnableAltFunction(GPIO_ALT_MM_CARD2);
		 if (GPIO_OK != gpio_error)
		 {
			 PRINT("SER_MMC_Init(): GPIO_EnableAltFunction %d\n",gpio_error);
		 }

          SER_mmc2_gpio_setting();

            PRCMU_MMC_SET;

				/* Initialize the base address of MMC-SD */
				mmc_error = MMC_Init(SDI_2_REG_BASE_ADDR, MMC_DEVICE_ID_2);
				if (MMC_OK != mmc_error)
				{
					PRINT("SER_MMC_Init():MMC_Init() %d \n",mmc_error);
					return;
				}
	
				MMC_SetBaseAddress(SDI_2_REG_BASE_ADDR,MMC_DEVICE_ID_2);
				
				gic_error = GIC_DisableItLine(GIC_SD_MMC_2_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
					return;
				}
				gic_error = GIC_ChangeDatum(GIC_SD_MMC_2_LINE, SER_MMC_2_InterruptHandler, &old_datum);
				if (GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
					return;
				}
				gic_error = GIC_EnableItLine(GIC_SD_MMC_2_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
					return;
				}
				break;
	 case INIT_MMC3:

		 PRCC_PERI2_SDI3_BUS_CLKEN;
		 PRCC_PERI2_SDI3_KBUS_CLKEN;

		 gpio_error = GPIO_EnableAltFunction(GPIO_ALT_MM_CARD3);
		 if (GPIO_OK != gpio_error)
		 {
			 PRINT("SER_MMC_Init(): GPIO_EnableAltFunction %d\n",gpio_error);
		 }
		 			SER_mmc3_gpio_setting();
		 
				PRCMU_MMC_SET;
				/* Initialize the base address of MMC-SD */
				mmc_error = MMC_Init(SDI_3_REG_BASE_ADDR, MMC_DEVICE_ID_3);
				if (MMC_OK != mmc_error)
				{
					PRINT("SER_MMC_Init():MMC_Init() %d \n",mmc_error);
					return;
				}
	
				MMC_SetBaseAddress(SDI_3_REG_BASE_ADDR,MMC_DEVICE_ID_3);
				
				gic_error = GIC_DisableItLine(GIC_SD_MMC_3_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
					return;
				}
				gic_error = GIC_ChangeDatum(GIC_SD_MMC_3_LINE, SER_MMC_3_InterruptHandler, &old_datum);
				if (GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
					return;
				}
				gic_error = GIC_EnableItLine(GIC_SD_MMC_3_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
					return;
				}
				break;
	 case INIT_MMC4:

		 PRCC_PERI2_SDI4_BUS_CLKEN;
		 PRCC_PERI2_SDI4_KBUS_CLKEN;

         gpio_error = GPIO_EnableAltFunction(GPIO_ALT_MM_CARD4);
		 if (GPIO_OK != gpio_error)
		 {
			 PRINT("SER_MMC_Init(): GPIO_EnableAltFunction %d\n",gpio_error);
		 }

					SER_mmc4_gpio_setting();

    				PRCMU_MMC_SET;

				/* Initialize the base address of MMC-SD */
				mmc_error = MMC_Init(SDI_4_REG_BASE_ADDR, MMC_DEVICE_ID_4);
				if (MMC_OK != mmc_error)
				{
					PRINT("SER_MMC_Init():MMC_Init() %d \n",mmc_error);
					return;
				}
	
				MMC_SetBaseAddress(SDI_4_REG_BASE_ADDR,MMC_DEVICE_ID_4);
				
				gic_error = GIC_DisableItLine(GIC_SD_MMC_4_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init():GIC_DisableItLine %d\n",gic_error);
					return;
				}
				gic_error = GIC_ChangeDatum(GIC_SD_MMC_4_LINE, SER_MMC_4_InterruptHandler, &old_datum);
				if (GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_ChangeDatum() %d\n",gic_error);
					return;
				}
				gic_error = GIC_EnableItLine(GIC_SD_MMC_4_LINE);
				if(GIC_OK != gic_error)
				{
					PRINT("SER_MMC_Init(): GIC_EnableItLine %d\n",gic_error);
					return;
				}
				break;

	 }
	/*ENABLE ALT FUNCTION */

}

PRIVATE void SER_mmc0_gpio_setting(void){

                  
				
                    GPIO_SET_MC0_RST;
                    
                    GPIO_SET_MC0_RST1;
                    
                    GPIO_SET_MC0_RST2;
                 
                    GPIO_SET_MC0_RST3;
  
	// GPIO15 as output
//  GPIO_SET_MC0_RST_OUT;
	// GPIO15 high
//	GPIO_SET_MC0_RST_HI;
                    
}


PRIVATE void SER_mmc1_gpio_setting(void){

					 GPIO_SET_MC1_RST ;
					 GPIO_SET_MC1_RST1;
					 GPIO_SET_MC1_RST2;
					 GPIO_SET_MC1_RST3 ;
					/*Instance 6 Related Changes*/
					 GPIO_SET_MC1_RST4 ;
					 GPIO_SET_MC1_RST5 ;
					 GPIO_SET_MC1_RST6 ;
					 GPIO_SET_MC1_RST7 ;
					                    
}

PRIVATE void SER_mmc2_gpio_setting(void){

						 GPIO_SET_MC2_RST;
						 GPIO_SET_MC2_RST1;
						 GPIO_SET_MC2_RST2;
						 GPIO_SET_MC2_RST3;
}


PRIVATE void SER_mmc3_gpio_setting(void){

						GPIO_SET_MC3_RST;
						GPIO_SET_MC3_RST1;
						GPIO_SET_MC3_RST2;
						GPIO_SET_MC3_RST3;
}


PRIVATE void SER_mmc4_gpio_setting(void){

					 GPIO_SET_MC4_RST ;
					 GPIO_SET_MC4_RST1;
					 GPIO_SET_MC4_RST2;
					 GPIO_SET_MC4_RST3 ;
					/*Instance 6 Related Changes*/
					 GPIO_SET_MC4_RST4 ;
					 GPIO_SET_MC4_RST5 ;
					 GPIO_SET_MC4_RST6 ;
					 GPIO_SET_MC4_RST7 ;
}

/****************************************************************************/
/* NAME:    SER_MMC_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets the mmc services                        */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_MMC_Close( void ) 
{
	/* TBD */
}


