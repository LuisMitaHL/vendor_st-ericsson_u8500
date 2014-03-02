/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Generic Interrupt Controller (GIC) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _GIC_H_
#define _GIC_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "debug.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Constants and new types													*
 *--------------------------------------------------------------------------*/
#define GIC_HCL_VERSION     1
#define GIC_HCL_MAJOR       0
#define GIC_HCL_MINOR       0

#define GIC_PMASK_DEFAULT   0x20        /* 0x20 is the lowest prio, 0x0 is the highest.*/
#define MASK_REG_OFFSET     32


/* A function with no argument returning pointer to a void function */
typedef void (*t_gic_func_ptr) (t_uint32);


/* Errors related values */
typedef enum
{
    GIC_OK                      = HCL_OK,
    GIC_ERROR                   = HCL_ERROR,
    GIC_UNSUPPORTED_HW          = HCL_UNSUPPORTED_HW,
    GIC_UNSUPPORTED_FEATURE     = HCL_UNSUPPORTED_FEATURE,
    GIC_INVALID_PARAMETER       = HCL_INVALID_PARAMETER,
    GIC_LINE_NOT_CONFIGURED     = HCL_MAX_ERROR_VALUE - 1,
    GIC_LINE_ALREADY_CONFIGURED = HCL_MAX_ERROR_VALUE - 2,
    GIC_NO_LINE_ACTIVE          = HCL_MAX_ERROR_VALUE - 3,
    GIC_LINE_NOT_AUTHORIZED     = HCL_MAX_ERROR_VALUE - 4,
    GIC_LINE_IS_ENABLED         = HCL_MAX_ERROR_VALUE - 5
} t_gic_error;

/*
   GIC lines configurations for each HW platform
   GIC_NO_LINE does not represent a real interrupt line, it is 
   only used to initialize internal variables. It SHOULD 
   be called used by user only when specified in the API
*/


typedef enum
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

    GIC_NO_LINE,
    GIC_ALL_LINES
} t_gic_line;





/*** CPU ICR ****/

typedef enum
{
    GIC_CPU_ICR_DISS              = 0x0,
    GIC_CPU_ICR_ENS               = 0x1
    
} t_gic_cpu_icr_enables;

typedef enum
{
    GIC_CPU_ICR_DISNS              = 0x0,
    GIC_CPU_ICR_ENNS               = 0x2
    
} t_gic_cpu_icr_enablens;


typedef enum
{
    GIC_IT_PENDING              = 0x0,
    GIC_IT_ACTIVE               = 0x4
} t_gic_cpu_icr_ackctl;

typedef enum
{
    GIC_DISABLE_FIQ             = 0x0,
    GIC_ENABLE_FIQ              = 0x8
} t_gic_cpu_icr_fiqen;

typedef enum
{
    GIC_SBPR_0                  = 0,
    GIC_SBPR_1
} t_gic_cpu_icr_sbpr;

typedef enum
{
    GIC_PR_MASK_0,
    GIC_PR_MASK_1,
    GIC_PR_MASK_2,
    GIC_PR_MASK_3,
    GIC_PR_MASK_4,
    GIC_PR_MASK_5,
    GIC_PR_MASK_6,
    GIC_PR_MASK_7,
    GIC_PR_MASK_8,
    GIC_PR_MASK_9,
    GIC_PR_MASK_10,
    GIC_PR_MASK_11,
    GIC_PR_MASK_12,
    GIC_PR_MASK_13,
    GIC_PR_MASK_14,
    GIC_PR_MASK_15,
    GIC_PR_MASK_16,
    GIC_PR_MASK_17,
    GIC_PR_MASK_18,
    GIC_PR_MASK_19,
    GIC_PR_MASK_20,
    GIC_PR_MASK_21,
    GIC_PR_MASK_22,
    GIC_PR_MASK_23,
    GIC_PR_MASK_24,
    GIC_PR_MASK_25,
    GIC_PR_MASK_26,
    GIC_PR_MASK_27,
    GIC_PR_MASK_28,
    GIC_PR_MASK_29,
    GIC_PR_MASK_30,
    GIC_PR_MASK_31,
    GIC_PR_INVALID_MASK
} t_gic_it_prio_mask;


/* ----------------------------------------------------------*/
/******* INT_DISTRIBUTOR **********/

/****** INT_DIST_CNTRL_TYPE *********/

typedef enum
{
	GIC_IT_DISABLE ,
	GIC_IT_ENABLE  	
						
}t_gic_it_enable_ctrl;
/************** INT_SECURITY ******************/
typedef enum
{
    GIC_IT_SEC,
    GIC_IT_NON_SEC
} t_gic_it_security;

/************* PRIORITY_LEVEL *******************/
typedef enum
{
    GIC_PR_LEVEL_0,
    GIC_PR_LEVEL_1,
    GIC_PR_LEVEL_2,
    GIC_PR_LEVEL_3,
    GIC_PR_LEVEL_4,
    GIC_PR_LEVEL_5,
    GIC_PR_LEVEL_6,
    GIC_PR_LEVEL_7,
    GIC_PR_LEVEL_8,
    GIC_PR_LEVEL_9,
    GIC_PR_LEVEL_10,
    GIC_PR_LEVEL_11,
    GIC_PR_LEVEL_12,
    GIC_PR_LEVEL_13,
    GIC_PR_LEVEL_14,
    GIC_PR_LEVEL_15,
    GIC_PR_LEVEL_16,
    GIC_PR_LEVEL_17,
    GIC_PR_LEVEL_18,
    GIC_PR_LEVEL_19,
    GIC_PR_LEVEL_20,
    GIC_PR_LEVEL_21,
    GIC_PR_LEVEL_22,
    GIC_PR_LEVEL_23,
    GIC_PR_LEVEL_24,
    GIC_PR_LEVEL_25,
    GIC_PR_LEVEL_26,
    GIC_PR_LEVEL_27,
    GIC_PR_LEVEL_28,
    GIC_PR_LEVEL_29,
    GIC_PR_LEVEL_30,
    GIC_PR_LEVEL_31,
    GIC_PR_INVALID_LEVEL
} t_gic_it_prio;

/************* INT_CONFIGURATION REGISTER *******************/
typedef enum
{
    GIC_IT_LEVEL_SENSITIVE      = 1,
    GIC_IT_EDGE_SENSITIVE       = 3
} t_gic_it_config;

typedef enum
{
    GIC_CPU_NO_TARGET,
    GIC_CPU_CORE_0,
    GIC_CPU_CORE_1,
    GIC_CPU_CORE0_CORE1
} t_gic_it_cpu_num;

/* FOR STI Trigger only */
typedef enum
{
	GIC_CPU_TARGET_LIST,
	GIC_CPU_ALL,
	GIC_CPU_REQUEST
}t_gic_it_cpu_list_filter;

typedef enum
{
	GIC_CPU_INTERFACE_0  = 1,
	GIC_CPU_INTERFACE_1
}t_gic_target_list;


typedef struct
{
	t_gic_it_enable_ctrl    it_en;
    t_gic_it_security        it_sec;
    t_gic_it_prio            prio_level;
    t_gic_it_config          it_config;
    t_gic_it_cpu_num         cpu_num;
    t_gic_it_cpu_list_filter list_filter;
    t_gic_target_list		 target_list;
} t_gic_config_cntrl;



typedef struct
{
	t_gic_cpu_icr_enables	 en_set_s;
	t_gic_cpu_icr_enablens	 en_set_ns;	
	t_gic_cpu_icr_sbpr       sbpr;
    t_gic_cpu_icr_fiqen      fiqen;
    t_gic_cpu_icr_ackctl     ackctl;
    t_gic_it_prio_mask		 prio_mask;	
    
}t_gic_interface_cntrl;

/*Maximum lines in GIC, 1 subracted form it because 
  GIC_NO_LINE will also consume one GIC line */
#define GIC_MAX_IT_LINES   (t_gic_line) ((t_uint32) GIC_ALL_LINES) /*0-160*/


/*--------------------------------------------------------------------------*
 * Functions declaration													*
 *--------------------------------------------------------------------------*/
PUBLIC t_gic_error  GIC_GetVersion(t_version *);
PUBLIC t_gic_error  GIC_SetDbgLevel(t_dbg_level);

PUBLIC t_gic_error  GIC_Init(t_logical_address);

PUBLIC void         GIC_SaveDeviceContext(void);
PUBLIC void         GIC_RestoreDeviceContext(void);

PUBLIC t_gic_error  GIC_ConfigureIrqLine(t_gic_line it_line, t_gic_func_ptr datum, t_gic_config_cntrl *p_gic_config);
PUBLIC t_gic_error  GIC_ConfigureFiqLine(t_gic_line it_line, t_gic_func_ptr datum, t_gic_config_cntrl *p_gic_config);
PUBLIC t_gic_error  GIC_IsActiveStatus(t_gic_line it_line, t_bool *p_active_line);
PUBLIC t_gic_error  GIC_GetSpiStatus(t_gic_line it_line, t_bool *p_active_spi_line);
PUBLIC t_gic_error  GIC_SetItPending(t_gic_line it_line);
PUBLIC t_gic_error  GIC_ClrItPending(t_gic_line it_line);
PUBLIC t_gic_error  GIC_ChangeDatum(t_gic_line it_line, t_gic_func_ptr new_datum, t_gic_func_ptr *p_old_datum);
PUBLIC t_gic_error  GIC_GetItAckId(t_uint32 *ack_id, t_uint32 *src_cpu_id);
PUBLIC t_gic_error  GIC_ItHandler(t_uint32 ack_id, t_uint32 src_cpu_id);
PUBLIC t_gic_error  GIC_EnableItLine(t_gic_line it_line);
PUBLIC t_gic_error  GIC_DisableItLine(t_gic_line it_line);
PUBLIC t_gic_error  GIC_IsLineEnabled(IN t_gic_line it_line, OUT t_bool *p_enabled_line);
PUBLIC t_gic_error  GIC_SetSoftSti(t_gic_line it_line,t_gic_target_list target_list, t_gic_it_cpu_list_filter cpu_list_filter);
PUBLIC t_gic_error  GIC_SetCpuInterfaceParameters(t_gic_interface_cntrl *p_gic_cntrl);
#ifdef __cplusplus
}   /* allow C++ to use these headers	*/
#endif /* __cplusplus */
#endif /* _GIC_H_ */

