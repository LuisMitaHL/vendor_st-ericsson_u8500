/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This provides global services defintion for different modules initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef __INC_SERVICES_H
#define __INC_SERVICES_H

/****************************************************************************/
/******************************* Includes ***********************************/
/****************************************************************************/

#include "hcl_defs.h"



#ifdef __DEBUG

#include <stdio.h>
#define PRINT     printf

#endif

#ifdef __RELEASE

#define PRINT(x,...) 

#endif

/****************************************************************************/
/******************************* Structures *********************************/
/****************************************************************************/

typedef void (*t_callback_fct)(void *,void *);

typedef struct
{    t_callback_fct     fct;
    void             *param;
} t_callback;

typedef enum 
{
    SERVICE_FAILED = -1,
    SERVICE_OK
} t_ser_error;

typedef t_uint32 t_tmr_req_id;

/* complete system initialization masks */

typedef struct
{
    t_uint32    system_mask;
    t_uint32    memory_mask;
    t_uint32    communication_mask;
    t_uint32    debug_mask;
} t_ser_mask;

extern t_ser_mask services_mask;

 /* all system masks */
/* ENUMS */

typedef enum 
{
    SYSTEM_INIT_GIC        =    MASK_BIT0,
    SYSTEM_INIT_UART       =    MASK_BIT1,
    SYSTEM_INIT_GPIO       =    MASK_BIT2,
    SYSTEM_INIT_DMA        =    MASK_BIT3,
    SYSTEM_INIT_USB        =    MASK_BIT4,
    SYSTEM_INIT_MMCSD      =    MASK_BIT5,
    SYSTEM_INIT_TMR        =    MASK_BIT6,
    SYSTEM_INIT_SPI        =    MASK_BIT7,
    SYSTEM_INIT_PRCC       =    MASK_BIT8,
    SYSTEM_INIT_PCRMU      =    MASK_BIT9,
  } t_ser_system_mask; 

 /* all memory masks */
typedef enum 
{
    MEMORY_INIT_MMC0       =     MASK_BIT0, 
    MEMORY_INIT_MMC1       =     MASK_BIT1,  
    MEMORY_INIT_MMC2       =     MASK_BIT2,
    MEMORY_INIT_MMC3       =     MASK_BIT3,
    MEMORY_INIT_MMC4       =     MASK_BIT4,
    MEMORY_INIT_MMC5       =     MASK_BIT5,
    } t_ser_memory_mask;


/* all communication masks */
typedef enum 
{
    COMM_INIT_UART0        =     MASK_BIT0,
    COMM_INIT_UART1        =     MASK_BIT1,
    COMM_INIT_UART2        =     MASK_BIT2,
    COMM_INIT_UART3        =     MASK_BIT3,
    COMM_INIT_SPI0         =     MASK_BIT4, 
    COMM_INIT_SPI1         =     MASK_BIT5,
	COMM_INIT_SPI2		   =	 MASK_BIT6,
	COMM_INIT_SPI3		   = 	 MASK_BIT7,
    COMM_INIT_USB          =     MASK_BIT8,
   } t_ser_communication_mask;


 /* all debug masks */
typedef enum 
{
    DEBUG_INIT_SER         =     MASK_BIT0,
    DEBUG_INIT_ETM         =     MASK_BIT1,
} t_ser_debug_mask;


/****************************************************************************/
/******************************* Defines ************************************/
/****************************************************************************/

/* Clk defines */

#define CFG_AMBA_CLK_EN_VAL      0x00000040




/* Debug defines */

#define DEBUG_WITH_UART_MODE                1
#define DEBUG_WITH_TIMER_INTERRUPT_MODE     2
#define DEBUG_BASE_ADDRESS                  0x00000000

#define NULL_IF                             0
#define ALL_ZERO                            0x00000000

/* MASK Defines */
                                            
#define    SYSTEM_DEFAULT_MASK              0xFFFFFFFF
#define    MEMORY_DEFAULT_MASK              0x0
#define    COMM_DEFAULT_MASK                0x0

/* Only software debugging configured by default */
#define    DEBUG_DEFAULT_MASK               0x1

/* Passed to Init API's of HCL's which do not need mask */
#define    DEFAULT_INIT_MASK                0x0

/* Passed to SER_UART_Init() */
#define    INIT_UART0                       MASK_BIT0
#define    INIT_UART1                       MASK_BIT1
#define    INIT_UART2                       MASK_BIT2
#define    INIT_UART3                       MASK_BIT3




/* Passed to SER_SPI_Init() */
#define    INIT_SPI0                        MASK_BIT0
#define    INIT_SPI1                        MASK_BIT1
#define    INIT_SPI2                        MASK_BIT2
#define    INIT_SPI3                        MASK_BIT3

/* Passed to SER_MMC_Init() */
#define    INIT_MMC0                        MASK_BIT0
#define    INIT_MMC1                        MASK_BIT1
#define    INIT_MMC2                        MASK_BIT2
#define    INIT_MMC3                        MASK_BIT3
#define    INIT_MMC4                        MASK_BIT4
#define    INIT_MMC5                        MASK_BIT5


/****************************************************************************/
/************************ Public Functions Prototype ************************/
/****************************************************************************/

PUBLIC void SER_Init(void);
PUBLIC void SER_Close( void );
PUBLIC void SER_WaitForInterrupt(void);

PUBLIC void SER_DEBUG_Init(t_uint8);
PUBLIC void SER_GIC_Init(t_uint8);
PUBLIC void SER_DMA_Init(t_uint8);
PUBLIC void SER_GPIO_Init(t_uint8);
PUBLIC void SER_PRCC_Init(t_uint8);
PUBLIC void SER_PRCMU_Init(t_uint8);
PUBLIC void SER_TMR_Init(t_uint8);
PUBLIC void SER_MMC_Init(t_uint8);
PUBLIC void SER_UART_Init(t_uint8);
PUBLIC void SER_SPI_Init(t_uint8);
PUBLIC void SER_USB_Init(t_uint8);
PUBLIC void SER_RTC_Init(t_uint8);

PUBLIC void SER_DEBUG_Close(t_uint8);
PUBLIC void SER_GIC_Close(void);
PUBLIC void SER_DMA_Close(void);
PUBLIC void SER_GPIO_Close(void);
PUBLIC void SER_PRCC_Close(void);
PUBLIC void SER_PRCMU_Close(void);
PUBLIC void SER_TMR_Close(void);
PUBLIC void SER_MMC_Close(void);
PUBLIC void SER_UART_Close(void);
PUBLIC void SER_USB_Close(void);
PUBLIC void SER_SPI_Close(void);

#endif 
/* 
__INC_SERVICES_H 
*/

/* End of file - services.h */



