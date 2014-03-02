/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the test environement	
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "services.h"

/*--------------------------------------------------------------------------*
 * GIC Includes																*
 *--------------------------------------------------------------------------*/
#include "gic.h"
#include "gic_services.h"

/************** GLOBAL Variables ******************/
static volatile int         irq_error, fiq_error;
t_gic_func_ptr               irq_start = (t_gic_func_ptr) 0;  /* Called at start of every IRQ */
t_gic_func_ptr               irq_finish = (t_gic_func_ptr) 0; /* Called at end of every IRQ */




PRIVATE const t_gic_line    gic_init_tab[] =
{
    GIC_STI_LINE_0,						/*  00   */ 
    GIC_STI_LINE_1,						/*  01   */
    GIC_STI_LINE_2,						/*  02   */
    GIC_STI_LINE_3,						/*  03   */
    GIC_STI_LINE_4,						/*  04   */
    GIC_STI_LINE_5,						/*  05   */
    GIC_STI_LINE_6,						/*  06   */
    GIC_STI_LINE_7,						/*  07   */
    GIC_STI_LINE_8,						/*  08   */
    GIC_STI_LINE_9,						/*  09   */
    GIC_STI_LINE_10,					/*  10   */
	GIC_STI_LINE_11,					/*  11   */
	GIC_STI_LINE_12,					/*  12   */
	GIC_STI_LINE_13,					/*  13   */
	GIC_STI_LINE_14,					/*  14   */
	GIC_STI_LINE_15,					/*  15   */	
	GIC_RESERVED_LINE16,				/*  16   */
	GIC_RESERVED_LINE17,				/*  17   */
	GIC_RESERVED_LINE18,				/*  18   */
	GIC_RESERVED_LINE19,				/*  19   */
	GIC_RESERVED_LINE20,				/*  20   */
	GIC_RESERVED_LINE21,				/*  21   */
	GIC_RESERVED_LINE22,				/*  22   */
	GIC_RESERVED_LINE23,				/*  23   */
	GIC_RESERVED_LINE24,				/*  24   */
	GIC_RESERVED_LINE25,				/*  25   */
	GIC_RESERVED_LINE26,				/*  26   */
	GIC_RESERVED_LINE27,				/*  27 	 */
	GIC_PPI_0,                  		/*  28   */
	GIC_PPI_1,							/*  29   */				
	GIC_PPI_2,							/*  30   */
	GIC_PPI_3,							/*  31   */	
    GIC_RESERVED_0_LINE,                /*	32   */
    GIC_SW_LINE,                        /*	33   */
    GIC_COMM_RX_LINE,                   /*	34   */
    GIC_COMM_TX_LINE,                   /*	35   */
    GIC_TIMER_0_LINE,                   /*	36   */
    GIC_RESERVED_LINE37,                /*	37   */
    GIC_SPI_2_LINE,                     /*	38   */
    GIC_PMU_IRQ4_LINE,                  /*	39   */
    GIC_SPI_0_LINE,                     /*	40   */
    GIC_RTT_0_1_LINE,                   /*	41   */
    GIC_PKA_LINE,                       /*	42   */
    GIC_UART_0_LINE,                    /*	43   */
    GIC_I2C_3_LINE,                     /*  44   */
    GIC_L2CC_EVENT_LINE,                /*	45   */
    GIC_MSP_0_LINE,                     /*	46   */
    GIC_CRYP_1_LINE,                    /*	47	 */
    GIC_PMU_IRQ1_LINE,                  /*	48   */
    GIC_TIMER_1_LINE,	                /*	49   */
    
    GIC_RTC_RTT_LINE,                   /*	50   */
    GIC_UART_1_LINE,                    /*	51   */
    GIC_USB_WAKEUP_LINE,                /*	52   */
    GIC_I2C_0_LINE,                     /*	53   */
    GIC_I2C_1_LINE,                     /*	54   */
    GIC_USB_OTG_LINE,                   /*	55   */
    GIC_DMA_SECURE_LINE,                /*	56   */
    GIC_DMA_LINE,                       /*	57   */
    GIC_UART_2_LINE,                    /*	58   */
    GIC_ICN_PMU1_LINE,                  /*	59   */
    GIC_ICN_PMU2_LINE,                  /*	60   */
    GIC_UART_3_LINE,		            /*	61   */
    GIC_SPI_3_LINE,                     /*	62   */
    GIC_SD_MMC_4_LINE,                  /*	63   */
    GIC_RESERVED_LINE64,                /*  64   */
    GIC_IRCC_LINE,                      /*  65   */
    GIC_IRDA_FT_INT_LINE,               /*  66   */
    GIC_IRDA_SD_INT_LINE,               /*  67   */
    GIC_IRDA_FI_INT_LINE,               /*  68   */
    GIC_IRDA_FD_INT_LINE,               /*  69   */
    GIC_FSMC_INT_LINE,                  /*  70   */
    GIC_FSMC_INT_2_LINE,                /*  71   */
    GIC_IRQ_N_LINE,                     /*  72   */
    GIC_SD_MMC_2_LINE,                  /*  73   */
    GIC_IMAGING_IT_0_LINE,              /*  74   */
    GIC_IMAGING_IT_1_LINE,              /*  75   */
    GIC_VIDEO_IT_0_LINE,                /*  76   */
    GIC_VIDEO_IT_1_LINE,                /*  77   */
   
   
    GIC_PRCMU_SECURE_LINE,		        /*  78   */
    GIC_PRCMU_NON_SECURE_LINE,          /*  79   */
    GIC_DISP_LINE,                      /*  80   */
    GIC_RESERVED_LINE81,                /*	81   */
    GIC_SD_MMC_1_LINE,                  /*  82   */
    GIC_RESERVED_LINE83,                /*	83   */
    GIC_MSP_1_LINE,                     /*  84   */
    GIC_KB_LINE,                        /*  85   */
    GIC_RESERVED_LINE86,                /*	86   */
    GIC_I2C_2_LINE,                     /*	87	 */
    GIC_B2R2_LINE,                      /*	88	 */
    GIC_CRYP_0_LINE,                    /*	89	 */
    GIC_RESERVED_LINE90,                /*	90	 */
    GIC_SD_MMC_3_LINE,                  /*	91	 */
    GIC_SD_MMC_0_LINE,                  /*	92	 */
    GIC_HSEM_LINE,                      /*	93	 */
    GIC_RESERVED_LINE94,                /*	94   */
    GIC_SBAG_LINE,                      /*	95   */

/* Line 96-127 are reserved for Modem Interrupt Sources */

    GIC_MOD_2_APE_IRQ_LINE,				/* 96 */
	GIC_MOD_2_APE_FIQ_LINE,				/* 97 */
	GIC_RESERVED_LINE98,				/* 98 */
	GIC_RESERVED_LINE99,				/* 99 */
	GIC_RESERVED_LINE100,				/* 100 */
	GIC_RESERVED_LINE101,				/* 101 */
	GIC_RESERVED_LINE102,				/* 102 */
	GIC_RESERVED_LINE103,				/* 103 */
	GIC_RESERVED_LINE104,				/* 104 */
	GIC_RESERVED_LINE105,				/* 105 */
	GIC_RESERVED_LINE106,				/* 106 */
	GIC_RESERVED_LINE107,				/* 107 */
	GIC_RESERVED_LINE108,				/* 108 */
	GIC_RESERVED_LINE109,				/* 109 */
	GIC_RESERVED_LINE110,				/* 110 */
	GIC_RESERVED_LINE111,				/* 111 */
	GIC_RESERVED_LINE112,				/* 112 */
	GIC_RESERVED_LINE113,				/* 113 */
	GIC_RESERVED_LINE114,				/* 114 */
	GIC_RESERVED_LINE115,				/* 115 */
	GIC_RESERVED_LINE116,				/* 116 */
	GIC_RESERVED_LINE117,				/* 117 */
	GIC_RESERVED_LINE118,				/* 118 */
	GIC_RESERVED_LINE119,				/* 119 */
	GIC_RESERVED_LINE120,				/* 120 */
	GIC_RESERVED_LINE121,				/* 121 */
	GIC_RESERVED_LINE122,				/* 122 */
	GIC_RESERVED_LINE123,				/* 123 */
	GIC_RESERVED_LINE124,				/* 124 */
	GIC_RESERVED_LINE125,				/* 125 */
	GIC_RESERVED_LINE126,				/* 126 */
	GIC_RESERVED_LINE127,				/* 127 */

    GIC_SPI_1_LINE,					    /*	128   */

	GIC_RESERVED_LINE129,				/*	129   */

    GIC_MSP_2_LINE,                     /*  130   */
    GIC_RESERVED_LINE131,               /*	131	  */
    GIC_RESERVED_LINE132,               /*	132	  */
    GIC_SRPTIMER_INT_LINE,              /*	133   */
    
    GIC_DDR_CTRL_SEC_LINE,				/*	134   */
    
    GIC_DDR_LINE,  		                /*	135   */
    GIC_RESERVED_LINE136,               /*  136   */
    GIC_RESERVED_LINE137,               /*  137   */
    GIC_RESERVED_LINE138,               /*  138   */
    GIC_RESERVED_LINE139,               /*  139   */
    
	GIC_CA9_CTIIRQ0_LINE,				/* 140 */
	GIC_CA9_CTIIRQ1_LINE,				/* 141 */
    GIC_ICN_STBUS_ERROR_LINE,           /* 142 */
    GIC_RESERVED_LINE143,               /* 143 */
   
   	GIC_MALI400_PIXEL_PROC_MMU_LINE,	/* 144 */
   	GIC_MALI400_PIXEL_PROC_LINE,		/* 145 */
   	GIC_MALI400_GEOM_PROC_MMU_LINE,		/* 146 */
    GIC_MALI400_GEOM_PROC_LINE,			/* 147 */
   	GIC_MALI400_COMB_LINE,				/* 148 */
	
	GIC_RESERVED_LINE149,				/* 149 */
	GIC_PRCMU_SEMAPHORE_LINE,				/* 150 */
    
    GIC_GPIO_0_LINE,                    /*	151   */
    GIC_GPIO_1_LINE,                    /*	152   */
    GIC_GPIO_2_LINE,                    /*	153   */
    GIC_GPIO_3_LINE,                    /*	154   */
    GIC_GPIO_4_LINE,                    /*	155   */
    GIC_GPIO_5_LINE,                    /*	156   */
    GIC_GPIO_6_LINE,                    /*	157   */
    GIC_GPIO_7_LINE,                    /*	158   */
	GIC_RESERVED_LINE159,                /*	159   */

    GIC_NO_LINE

};



 

/****************************************************************************/
/* NAME: SER_GIC_IntHandlerIRQ                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt handler which is used to dispatch the     */
/*              specific ISR												*/
/*                                                                          */
/* PARAMETERS:                                                              */
/*   IN :                                                                   */
/*   OUT:                                                                   */
/*																			*/
/* RETURN:                                                                  */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_GIC_IntHandlerIRQ(void)
{
    t_uint32    ack_id = 0x0, src_cpu_id = 0x0;
    t_gic_error gic_error;

    /* dispatch it */
    gic_error = GIC_GetItAckId(&ack_id, &src_cpu_id);

    if (gic_error != GIC_NO_LINE_ACTIVE)
    {   

        gic_error = GIC_ItHandler(ack_id, src_cpu_id);
                
    }
    else
    {
      irq_error++;  
    }
    
    return;
    
}

/****************************************************************************/
/* NAME: SER_GIC_IntHandlerFIQ                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt handler which is used to dispatch the     */
/*              specific ISR												*/
/*                                                                          */
/* PARAMETERS:                                                              */
/*   IN :                                                                   */
/*   OUT:                                                                   */
/*																			*/
/* RETURN:                                                                  */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_GIC_IntHandlerFIQ(void)
{
    t_uint32    ack_id = 0x0, src_cpu_id = 0x0;
    t_gic_error gic_error;

    /* dispatch it */
    gic_error = GIC_GetItAckId(&ack_id, &src_cpu_id);

    if (gic_error != GIC_NO_LINE_ACTIVE)
    {   
        gic_error = GIC_ItHandler(ack_id, src_cpu_id);
    }
    else
    {
        fiq_error++;
    }
    
    return;
    
}

/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_GIC_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the gic module                      */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: NONE                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_GIC_Init(t_uint8 default_ser_mask)
{
    t_uint32            index = 0;
    t_gic_error         gic_error;
    t_gic_config_cntrl  gic_config;
    t_gic_interface_cntrl gic_cntrl;
    
    irq_error = 0;
    fiq_error = 0;
    
    gic_error = GIC_Init(GIC_0_CTRL_REG_BASE_ADDR);
    
    if(GIC_OK!=gic_error) 
    {
    	PRINT("GIC INIT FAILED");
    	return;
    }
    /* For Spi's 32-224 */
    (gic_config.prio_level)  = GIC_PR_LEVEL_0;
    (gic_config.it_sec) 	 = GIC_IT_NON_SEC;
    (gic_config.it_config)   = GIC_IT_LEVEL_SENSITIVE;
    (gic_config.cpu_num)  	 = GIC_CPU_CORE_0;
    
     	
	(gic_cntrl.en_set_s) 		= GIC_CPU_ICR_ENS;
	(gic_cntrl.en_set_ns)	    = GIC_CPU_ICR_ENNS;
	(gic_cntrl.fiqen)			= GIC_DISABLE_FIQ;
	(gic_cntrl.ackctl)			= GIC_IT_ACTIVE;
	(gic_cntrl.prio_mask) 		= GIC_PR_MASK_31;
	
	
	gic_error =	GIC_SetCpuInterfaceParameters(&gic_cntrl); 
	
	if(GIC_OK!=gic_error) 
    {
    	PRINT("GIC INIT FAILED");
    	return;
    }
      
      while (gic_init_tab[index] != GIC_NO_LINE)
    {
        gic_error = GIC_ConfigureIrqLine(gic_init_tab[index], (t_gic_func_ptr)SER_DummyHandler, &gic_config);
        index++;
    }

    /*coverity[self_assign]*/
    gic_error = gic_error;
    return;
    

}

/****************************************************************************/
/* NAME:    SER_GIC_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops the gic services		                    */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: NONE                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_GIC_Close(void)
{
    /* TBD	*/
}

/****************************************************************************/
/* NAME:    SER_GIC_DefineStartFinishIRQ                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine defines the start and finish routines for IRQ  */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: NONE                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_GIC_DefineStartFinishIRQ(t_gic_func_ptr start, t_gic_func_ptr finish)
{
    /* These vectors can be zero'd if required */
    irq_start = start;
    irq_finish = finish;
}



/****************************************************************************/
/* NAME:    SER_DummyHandler                                  				 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine defines the default handler for IRQ			*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: NONE                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_DummyHandler(void)
{
      
   	PRINT("Inside the Dummy Handler");
   	return;

}
