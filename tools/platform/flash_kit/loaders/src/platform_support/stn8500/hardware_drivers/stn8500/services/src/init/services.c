/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This provides call to global services for different modules initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hcl_defs.h"
#include "services.h"  
#include "memory_mapping.h"     /* Global Services header File */

#define CONFIG_FAIL 0
#define CONFIG_PASS 1


/*--------------------------------------------------------------------------*
 * Global Variables															*
 *--------------------------------------------------------------------------*/
extern t_uint32 Image$$ER_SHARE_AHB_MEM$$Base;
extern t_uint32 Image$$ER_SHARE_AHB_MEM$$Length;
extern t_uint32 Image$$ER_SHARE_AHB_MEM$$ZI$$Length;


/* Public Mask Variable to call services */
t_ser_mask  services_mask = {
                                SYSTEM_DEFAULT_MASK,
                                MEMORY_DEFAULT_MASK,
                                DISPLAY_DEFAULT_MASK,
                                COMM_DEFAULT_MASK,
                                DEBUG_DEFAULT_MASK
                             };

/*******************************************************************************/
/* NAME:   SER_Init                                               			   */
/*-----------------------------------------------------------------------------*/
/* DESCRIPTION: Invokes all the Services initializations that are not masked   */
/*                                                                             */
/* PARAMETERS: NONE                                                            */
/*                                                                             */
/* RETURN: NONE                                                                */
/*              															   */
/*-----------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                              */
/*******************************************************************************/
PUBLIC void SER_Init()
{

PRINT("Services initialization: STARTED\n");

	#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

	
	/* AMBA clock enable for Peripheral6-CFG register */

	(*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR)))   |= CFG_AMBA_CLK_EN_VAL;
	
	#endif



#ifdef SER_DEBUG
    PRINT(" DEBUG....");
    if (services_mask.debug_mask & DEBUG_INIT_SER) /* Debug HCL Initialization */
    {	
		SER_DEBUG_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_GIC
    PRINT(" GIC......");
    if (services_mask.system_mask & SYSTEM_INIT_GIC)   /* GIC Initialization */
    {	
		SER_GIC_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_GPIO
    PRINT(" GPIO.....");
    if (services_mask.system_mask & SYSTEM_INIT_GPIO)  /* GPIO Initialization */
    {	
		SER_GPIO_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

	/* Initialize the IPs which are not being masked by Public Mask Variable */
    
#ifdef SER_DMA

	PRINT(" DMA.....");
    if (services_mask.system_mask & SYSTEM_INIT_DMA)  /* DMA Initialization */
    {
        SER_DMA_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
    
#endif /* SER_DMA */

#ifdef SER_PRCC
    PRINT(" PRCC......");
    if (services_mask.system_mask & SYSTEM_INIT_PRCC)   /* PRCC Initialization */
    {	
		SER_PRCC_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_PRCMU
    PRINT(" PRCMU......");
    if (services_mask.system_mask & SYSTEM_INIT_PRCMU)   /* PRCMU Initialization */
    {	
		SER_PRCMU_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_RTC
    PRINT(" RTC......");
    if (services_mask.system_mask & SYSTEM_INIT_RTC)   /* RTC Initialization */
    {	
		SER_RTC_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_TMR
    PRINT(" TMR......");
    if (services_mask.system_mask & SYSTEM_INIT_TMR)   /* Timer Initialization */
    {	
		SER_TMR_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif


#ifdef SER_PKA
    PRINT(" PKA......");
    if (services_mask.system_mask & SYSTEM_INIT_PKA)   /* PKA Initialization */
    {	
		SER_PKA_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_RNG
    PRINT(" RNG......");
    if (services_mask.system_mask & SYSTEM_INIT_RNG)   /* RNG Initialization */
    {	
		SER_RNG_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif



#ifdef SER_DMC
    PRINT(" DMC.....");
    if (services_mask.memory_mask & MEMORY_INIT_DMC)  /* DMC Initialization */
    {	
		SER_DMC_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif  /* SER_DMC */

#ifdef SER_FSMC
    PRINT(" FSMC NAND....");
    if (services_mask.memory_mask & MEMORY_INIT_FSMCNAND)  /* NAND Initialization */
    {	
		SER_FSMC_Init(INIT_FSMCNAND);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
    	
    PRINT(" FSMC NOR....");
    if (services_mask.memory_mask & MEMORY_INIT_FSMCNOR)  /* NOR Initialization */
    {	
		SER_FSMC_Init(INIT_FSMCNOR);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif  /* SER_FSMC */

#ifdef SER_MMC_SD
    PRINT(" MMC_SD0...");
    if (services_mask.memory_mask & MEMORY_INIT_MMC0)   /* MMC SD 0 Initialization */
    {	
		SER_MMC_Init(INIT_MMC0);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

    PRINT(" MMC_SD1...");
    if (services_mask.memory_mask & MEMORY_INIT_MMC1)   /* MMC SD 1 Initialization */
    {	
		SER_MMC_Init(INIT_MMC1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

    PRINT(" MMC_SD2...");
    if (services_mask.memory_mask & MEMORY_INIT_MMC2)   /* MMC SD 2 Initialization */
    {	
		SER_MMC_Init(INIT_MMC2);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

	PRINT(" MMC_SD3...");
    if (services_mask.memory_mask & MEMORY_INIT_MMC3)   /* MMC SD 3 Initialization */
    {	
		SER_MMC_Init(INIT_MMC3);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
	PRINT(" MMC_SD4...");
	if (services_mask.memory_mask & MEMORY_INIT_MMC4)   /* MMC SD 4 Initialization */
	{	
		SER_MMC_Init(INIT_MMC4);
		PRINT("OK\n");
	}
	else PRINT("Skipped\n");

	PRINT(" MMC_SD5...");
	if (services_mask.memory_mask & MEMORY_INIT_MMC5)   /* MMC SD 5 Initialization */
	{	
		SER_MMC_Init(INIT_MMC5);
		PRINT("OK\n");
	}
	else PRINT("Skipped\n");

#endif /* SER_MMC_SD */

#if defined SER_UART

	PRINT(" UART0....");
    if (services_mask.communication_mask & COMM_INIT_UART0)  /* UART0 Initialization */
    {	
		SER_UART_Init(INIT_UART0);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
    	
	PRINT(" UART1....");
    if (services_mask.communication_mask & COMM_INIT_UART1)  /* UART1 Initialization */
    {	
		SER_UART_Init(INIT_UART1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

	PRINT(" UART2....");
    if (services_mask.communication_mask & COMM_INIT_UART2)  /* UART2 Initialization */
    {	
		SER_UART_Init(INIT_UART2);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

#endif  /* SER_UART */

#ifdef SER_HSI
    PRINT(" HSI......");
    if (services_mask.communication_mask & COMM_INIT_HSI)   /* HSI Initialization */
    {	
		SER_HSI_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_HSI */

#ifdef SER_SSP
    PRINT(" SSP0......");
    if (services_mask.communication_mask & COMM_INIT_SSP0)   /* SSP0 Initialization */
    {	
		SER_SSP_Init(INIT_SSP0);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

    PRINT(" SSP1......");
    if (services_mask.communication_mask & COMM_INIT_SSP1)   /* SSP1 Initialization */
    {	
		SER_SSP_Init(INIT_SSP1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

#endif  /* SER_SSP */

#ifdef SER_SPI
    PRINT(" SPI0......");
    if (services_mask.communication_mask & COMM_INIT_SPI0)   /* SPI0 Initialization */
    {	
		SER_SPI_Init(INIT_SPI0);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

    PRINT(" SPI1......");
    if (services_mask.communication_mask & COMM_INIT_SPI1)   /* SPI1 Initialization */
    {	
		SER_SPI_Init(INIT_SPI1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

	PRINT(" SPI2......");
	if (services_mask.communication_mask & COMM_INIT_SPI2)   /* SPI2 Initialization */
	{	
		SER_SPI_Init(INIT_SPI2);
		PRINT("OK\n");
	}

	else PRINT("Skipped\n");

	PRINT(" SPI3......");
	if (services_mask.communication_mask & COMM_INIT_SPI3)   /* SPI3 Initialization */
	{	
		SER_SPI_Init(INIT_SPI3);
		PRINT("OK\n");
	}
	else PRINT("Skipped\n");

#endif  /* SER_SPI */


#ifdef SER_I2C

	PRINT(" I2C0.....");
	if (services_mask.communication_mask & COMM_INIT_I2C0)  /* I2C0 Initialization */
    {	
		SER_I2C_Init(INIT_I2C0);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
        
	PRINT(" I2C1.....");
    if (services_mask.communication_mask & COMM_INIT_I2C1)  /* I2C1 Initialization */
    {	
		SER_I2C_Init(INIT_I2C1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

	PRINT(" I2C2.....");
	if (services_mask.communication_mask & COMM_INIT_I2C2)  /* I2C2 Initialization */
    {	
		SER_I2C_Init(INIT_I2C2);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
        
	PRINT(" I2C3.....");
    if (services_mask.communication_mask & COMM_INIT_I2C3)  /* I2C3 Initialization */
    {	
		SER_I2C_Init(INIT_I2C3);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");

	#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)|| defined(ST_8500V2) || defined(ST_HREFV2)
	    
    PRINT(" I2C4.....");
    if (services_mask.communication_mask & COMM_INIT_I2C4)  /* I2C4 Initialization */
    {	
		SER_I2C_Init(INIT_I2C4);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif
#endif  /* SER_I2C */

#ifdef SER_MSP

	PRINT(" MSP0.....");
	if (services_mask.communication_mask & COMM_INIT_MSP0)  /* MSP0 Initialization */
    {	
		SER_MSP_Init(INIT_MSP0);
        PRINT("OK\n");
    }
    else
    {
    	PRINT("Skipped\n");	
    } 

	PRINT(" MSP1.....");
    if (services_mask.communication_mask & COMM_INIT_MSP1)  /* MSP1 Initialization */
    {	
		SER_MSP_Init(INIT_MSP1);
        PRINT("OK\n");
    }
    else
    {
    	PRINT("Skipped\n");
    } 
    
    PRINT(" MSP2.....");    
    if (services_mask.communication_mask & COMM_INIT_MSP2)  /* MSP2 Initialization */
    {	
		SER_MSP_Init(INIT_MSP2);
        PRINT("OK\n");
    }
    else
    {
    	PRINT("Skipped\n");
    }
    
    #if defined(ST_8500V2) || defined(ST_HREFV2)
    
    PRINT(" MSP3.....");    
    if (services_mask.communication_mask & COMM_INIT_MSP3)  /* MSP3 Initialization */
    {	
		SER_MSP_Init(INIT_MSP3);
        PRINT("OK\n");
    }
    else
    {
    	PRINT("Skipped\n");
    }
    #endif
#endif /* SER_MSP */


#ifdef SER_USB
    PRINT(" USB......");
    if (services_mask.communication_mask & COMM_INIT_USB)   /* USB Initialization */
    {	
		SER_USB_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_USB */

#ifdef SER_PWL
    PRINT(" PWL......");
    if (services_mask.display_mask & DISPLAY_INIT_PWL)    /* PWL Initialization */
    {	
		SER_PWL_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_PWL */

#ifdef SER_DSI
    PRINT(" DSI......");
    if (services_mask.display_mask & DISPLAY_INIT_DSI)    /* DSI Initialization */
    {	
		SER_DSI_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_MCDE */

#ifdef SER_MCDE
    PRINT(" MCDE......");
    if (services_mask.display_mask & DISPLAY_INIT_MCDE)    /* MCDE Initialization */
    {	
		SER_MCDE_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_MCDE */

#ifdef SER_HASH
    PRINT(" HASH0.....");
    if (services_mask.system_mask & SYSTEM_INIT_HASH0)       /* HASH0 Initialization */
    {
        SER_HASH_Init(INIT_HASH0);
        PRINT("OK\n");
    }
    else
    {
        PRINT("Skipped\n");
    }
    
    PRINT(" HASH1.....");
    if (services_mask.system_mask & SYSTEM_INIT_HASH1)       /* HASH1 Initialization */
    {
        SER_HASH_Init(INIT_HASH1);
        PRINT("OK\n");
    }
    else
    {
        PRINT("Skipped\n");
    }
#endif/* SER_HASH */

#ifdef SER_CRYP
	PRINT(" CRYP0...");
	if (services_mask.system_mask & SYSTEM_INIT_CRYP0)   /* CRYP0 Initialization */
    {	
		SER_CRYP_Init(INIT_CRYP0);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
    
    PRINT(" CRYP1...");
	if (services_mask.system_mask & SYSTEM_INIT_CRYP1)   /* CRYP1 Initialization */
    {	
		SER_CRYP_Init(INIT_CRYP1);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_CRYP*/

#ifdef SER_SKE
    PRINT(" SKE......");
    if (services_mask.communication_mask & COMM_INIT_SKE)   /* SKE Initialization */
    {	
		SER_SKE_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_STM
    PRINT(" STM......");
    if (services_mask.communication_mask & COMM_INIT_STM)   /* STM Initialization */
    {	
		SER_STM_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif

#ifdef SER_SBAG
    PRINT(" SBAG......");
    if (services_mask.communication_mask & COMM_INIT_SBAG)   /* SBAG Initialization */
    {	
		SER_SBAG_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif


	PRINT("Services initialization: COMPLETED\n\r\n");
}

/****************************************************************************/
/* NAME:    SER_CLOSE														*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Stops the Services invoked when system was up 				*/
/*                                                                          */
/* PARAMETERS:   NONE                                                       */
/*                                                                          */
/* RETURN:       NONE                                                       */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_Close(void)
{
    /* STOPS the services which were started already */

#ifdef SER_GIC
	if (services_mask.system_mask & SYSTEM_INIT_GIC)   /* GIC Reset */
	{	
		SER_GIC_Close();
	}
#endif

#ifdef SER_DMA
    if (services_mask.system_mask & SYSTEM_INIT_DMA)   /* DMA Reset */
    {	
		SER_DMA_Close();
    }
#endif

#ifdef SER_GPIO
    if (services_mask.system_mask & SYSTEM_INIT_GPIO)  /* GPIO Reset */
    {	
		SER_GPIO_Close();
    }
#endif

#ifdef SER_PRCC
    if (services_mask.system_mask & SYSTEM_INIT_PRCC)   /* PRCC Reset */
    {	
		SER_PRCC_Close();
    }
#endif

#ifdef SER_PRCMU
    if (services_mask.system_mask & SYSTEM_INIT_PRCMU)   /* PRCMU Reset */
    {	
		SER_PRCMU_Close();
    }
#endif

#ifdef SER_RTC
    if (services_mask.system_mask & SYSTEM_INIT_RTC)   /* RTC Reset */
    {	
		SER_RTC_Close();
    }
#endif

#ifdef SER_TMR
    if (services_mask.system_mask & SYSTEM_INIT_TMR)   /* TMR Reset */
    {	
		SER_TMR_Close();
    }
#endif

#ifdef SER_PKA
    if (services_mask.system_mask & SYSTEM_INIT_PKA)   /* PKA Reset */
    {	
		SER_PKA_Close();
    }
#endif

#ifdef SER_RNG
    if (services_mask.system_mask & SYSTEM_INIT_RNG)   /* RNG Reset */
    {	
		SER_RNG_Close();
    }
#endif

#ifdef SER_DMC
    if (services_mask.memory_mask & MEMORY_INIT_DMC)  /* DMC Reset */
    {	
		SER_DMC_Close();
    }
#endif

#ifdef SER_FSMC
   if ((services_mask.memory_mask & MEMORY_INIT_FSMCNAND) ||
       (services_mask.memory_mask & MEMORY_INIT_FSMCNOR)   )  /* FSMC Reset */
    {	
	   SER_FSMC_Close();
    }
#endif

#ifdef SER_MMC_SD
    if ((services_mask.memory_mask & MEMORY_INIT_MMC0) ||
        (services_mask.memory_mask & MEMORY_INIT_MMC1) ||
        (services_mask.memory_mask & MEMORY_INIT_MMC2) ||
        (services_mask.memory_mask & MEMORY_INIT_MMC3) ||
        (services_mask.memory_mask & MEMORY_INIT_MMC4) ||
        (services_mask.memory_mask & MEMORY_INIT_MMC5))   /* MMC SD Reset */
    {	
		SER_MMC_Close();
    }
#endif

#if defined SER_UART
    if ((services_mask.communication_mask & COMM_INIT_UART0) ||
        (services_mask.communication_mask & COMM_INIT_UART1) ||
        (services_mask.communication_mask & COMM_INIT_UART2))  /* UART Reset */
    {	
		SER_UART_Close();
    }
#endif

#ifdef SER_HSI
    if (services_mask.communication_mask & COMM_INIT_HSI)   /* HSI Reset */
    {	
		SER_HSI_Close();
    }
#endif

#ifdef SER_SSP
    if ((services_mask.communication_mask & COMM_INIT_SSP0) ||
        (services_mask.communication_mask & COMM_INIT_SSP1))   /* SSP Reset */
    {	
		SER_SSP_Close();
    }
#endif

#ifdef SER_I2C
    if ((services_mask.communication_mask & COMM_INIT_I2C0) ||
        (services_mask.communication_mask & COMM_INIT_I2C1) ||
        (services_mask.communication_mask & COMM_INIT_I2C2) ||
        (services_mask.communication_mask & COMM_INIT_I2C3))   /* I2C Reset */
    {	
        /* SER_I2C_Close(); */
    }
#endif

#ifdef SER_MSP
	if ((services_mask.communication_mask & COMM_INIT_MSP0) ||
        (services_mask.communication_mask & COMM_INIT_MSP1) ||
        (services_mask.communication_mask & COMM_INIT_MSP2) ||
        (services_mask.communication_mask & COMM_INIT_MSP3))	/* MSP Reset */
	{	

		SER_MSP_Close();
	}
#endif

#ifdef SER_USB
    if (services_mask.communication_mask & COMM_INIT_USB) /* USB Reset */
    {	
		SER_USB_Close();
    }
#endif

	#ifdef SER_DSI
    if (services_mask.display_mask & DISPLAY_INIT_DSI)  /* DSI Close */
    {	
		SER_DSI_Close();
    }
#endif


#ifdef SER_MCDE
    if (services_mask.display_mask & DISPLAY_INIT_MCDE)  /* MCDE Close */
    {	
		SER_MCDE_Close();
    }
#endif


#ifdef SER_HASH
    if (services_mask.system_mask & SYSTEM_INIT_HASH0)   /* HASH0 Close */
    {	
		SER_HASH_Close(INIT_HASH0);
    }
    
    if (services_mask.system_mask & SYSTEM_INIT_HASH1)   /* HASH1 Close */
    {	
		SER_HASH_Close(INIT_HASH1);
    }
#endif


#ifdef SER_CRYP
    if (services_mask.system_mask & SYSTEM_INIT_CRYP0)   /* CRYP0 Close */
    {	
		SER_CRYP_Close(INIT_CRYP0);
    }
    
    if (services_mask.system_mask & SYSTEM_INIT_CRYP1)   /* CRYP1 Close */
    {	
		SER_CRYP_Close(INIT_CRYP1);
    }
#endif

#ifdef SER_SKE 
   if (services_mask.communication_mask & COMM_INIT_SKE)  /* SKE Close */
    {	
	   SER_SKE_Close();
    }
#endif

#ifdef SER_STM 
   if (services_mask.communication_mask & COMM_INIT_STM)  /* STM Close */
    {	
	   SER_STM_Close();
    }
#endif

   #ifdef SER_SBAG
   if (services_mask.communication_mask & COMM_INIT_SBAG)  /* SBAG Close */
    {	
	   SER_SBAG_Close();
    }
#endif


#ifdef SER_PWL
    if (services_mask.display_mask & DISPLAY_INIT_PWL)    /* PWL Initialization */
    {
    	SER_PWL_Close();
    }
#endif
}

/****************************************************************************/
/* NAME:    SER_WaitForInterrupt											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine put the host processor (ARM) into low power 	*/
/* mode.This mode will be exited when servicing any interrupt source		*/
/*                                                                          */
/* PARAMETERS:   NONE                                                       */
/*                                                                          */
/* RETURN:       NONE                                                       */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_WaitForInterrupt(void)
{
 	t_uint32 r0 = 0;
    __asm
    {
        MOV r0, #0
        MCR p15, 0, r0, c7, c0, 4
    }
    /*coverity[self_assign]*/
    r0 = r0;
}

/****************************************************************************/
/* NAME:	t_uint32 C_SWI_Handler(int swi_num, int *regs)                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:  Routine for software intrrupt handling                     */
/*																			*/
/* PARAMETERS:																*/
/* IN  : 		int swi_num, int *regs		                                */        	  
/* OUT : none																*/
/* 																			*/
/* RETURN:																	*/
/*		  		t_uint32 :: return status	1:if swi handled	            */
/*              0:if swi not handled 										*/									
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA															*/
/****************************************************************************/

void C_SWI_Handler(int swi_num, int *regs)
{
	return;
}
