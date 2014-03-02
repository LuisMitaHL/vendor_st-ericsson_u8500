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

	
	/* AMBA clock enable for Peripheral6-CFG register */

	(*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR)))   |= CFG_AMBA_CLK_EN_VAL;
	

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

#ifdef SER_TMR
    PRINT(" TMR......");
    if (services_mask.system_mask & SYSTEM_INIT_TMR)   /* Timer Initialization */
    {	
		SER_TMR_Init(DEFAULT_INIT_MASK);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif



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

	PRINT(" UART3....");
    if (services_mask.communication_mask & COMM_INIT_UART3)  /* UART3 Initialization */
    {	
		SER_UART_Init(INIT_UART3);
        PRINT("OK\n");
    }
    else PRINT("Skipped\n");


#endif  /* SER_UART */

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


#ifdef SER_USB
    PRINT(" USB......");
    if (services_mask.communication_mask & COMM_INIT_USB)   /* USB Initialization */
    {	
		SER_USB_Init(DEFAULT_INIT_MASK);
		PRINT("OK\n");
    }
    else PRINT("Skipped\n");
#endif /* SER_USB */


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

#ifdef SER_TMR
    if (services_mask.system_mask & SYSTEM_INIT_TMR)   /* TMR Reset */
    {	
		SER_TMR_Close();
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

#ifdef SER_SPI
    if ((services_mask.communication_mask & COMM_INIT_SPI0) ||
       (services_mask.communication_mask & COMM_INIT_SPI1) ||
       (services_mask.communication_mask & COMM_INIT_SPI2) ||
       (services_mask.communication_mask & COMM_INIT_SPI3))   /* SPI Reset */
    {	
		SER_SPI_Close();
    }
#endif


#ifdef SER_USB
    if (services_mask.communication_mask & COMM_INIT_USB) /* USB Reset */
    {	
		SER_USB_Close();
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
