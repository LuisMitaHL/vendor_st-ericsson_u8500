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

#ifndef _MMCSD_SERVICES
#define _MMCSD_SERVICES

#include "mmc.h"

#if defined(__MMCI_HCL_ENHANCED)
typedef struct
{
	t_mmc_device_mode transfer_mode;
}t_ser_mmc_context, *t_p_ser_mmc_context;

typedef struct
{
	t_mmc_event	event;
} t_ser_mmc_param;

#endif


/* Function Protypes */
PRIVATE void SER_MMC_0_InterruptHandler(t_uint32 irq);
PRIVATE void SER_MMC_1_InterruptHandler(t_uint32 irq);
PRIVATE void SER_MMC_2_InterruptHandler(t_uint32 irq);
PRIVATE void SER_MMC_3_InterruptHandler(t_uint32 irq);
PRIVATE void SER_MMC_4_InterruptHandler(t_uint32 irq);
PRIVATE void SER_MMC_5_InterruptHandler(t_uint32 irq);
PRIVATE void SER_mmc_gpio_setting(void);
PRIVATE void SER_mmc2_gpio_setting(void);
PRIVATE void SER_mmc0_gpio_setting(void);
PUBLIC void SER_MMC_Init( t_uint8 );
PUBLIC void SER_MMC_Close( void );
PUBLIC t_ser_error SER_MMC_RegisterCallback(t_callback_fct,void* );
PUBLIC t_ser_error SER_MMC_WaitEnd(t_mmc_device_id );


#define PRCC_PERI1_SDI0_BUS_CLKEN		(*(volatile t_uint32 *)(0x8012F000)) |= 0x20
#define PRCC_PERI1_SDI0_KBUS_CLKEN      (*(volatile t_uint32 *)(0x8012F008)) |= 0x20

#define PRCC_PERI2_SDI1_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x10
#define PRCC_PERI2_SDI1_KBUS_CLKEN		(*(volatile t_uint32 *)(0x8011F008)) |= 0x04

#define PRCC_PERI2_SDI3_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x80
#define PRCC_PERI2_SDI3_KBUS_CLKEN		(*(volatile t_uint32 *)(0x8011F008)) |= 0x10

// ER324895, do not reroute MMC clock, rely on PRCMU to handle clock routing
//#define PRCMU_MMC_SET	(*(volatile t_uint32 *)(0x80157024)) = 0x130
#define PRCMU_MMC_SET

#define PRCC_PERI2_SDI4_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x10
#define PRCC_PERI2_SDI4_KBUS_CLKEN 		(*(volatile t_uint32 *)(0x8011F008)) |= 0x04


#define PRCC_PERI3_SDI2_BUS_CLKEN		(*(volatile t_uint32 *)(0x8000F000)) |= 0xFFFF
#define PRCC_PERI3_SDI2_KBUS_CLKEN	    (*(volatile t_uint32 *)(0x8000F008)) |= 0xFFFF

#define PRCC_PERI3_SDI5_BUS_CLKEN		(*(volatile t_uint32 *)(0x8000F000)) |= 0x80
#define PRCC_PERI3_SDI5_KBUS_CLKEN	    (*(volatile t_uint32 *)(0x8000F008)) |= 0x80


#define GPIO_SET_RST (*(volatile t_uint32 *) (0x8011E00C)) &= ~0x0000FFE0;
#define GPIO_SET_RST1 (*(volatile t_uint32 *) (0x8011E004)) |= 0x0000FFE0;
#define GPIO_SET_RST2 (*(volatile t_uint32 *) (0x8011E020)) |= 0x0000FFE0;
#define GPIO_SET_RST3 (*(volatile t_uint32 *) (0x8011E024)) &= ~0x0000FFE0;

#define GPIO_SET_MC2_RST (*(volatile t_uint32 *)(0x8000E104)) |= 0x000007FF
#define GPIO_SET_MC2_RST1 (*(volatile t_uint32 *)(0x8000E10C)) &= ~(0x000007FF)
#define GPIO_SET_MC2_RST2 (*(volatile t_uint32 *)(0x8000E120)) |= 0x000007FF
#define GPIO_SET_MC2_RST3 (*(volatile t_uint32 *)(0x8000E124)) &= ~(0x000007FF)

#define GPIO_SET_MC0_RST (*(volatile t_uint32 *)(0x8012E004)) |= 0xFFFC0000
#define GPIO_SET_MC0_RST1 (*(volatile t_uint32 *)(0x8012E00C)) &= ~(0xFFFC0000)
#define GPIO_SET_MC0_RST2 (*(volatile t_uint32 *)(0x8012E020)) |= 0xFFFC0000
#define GPIO_SET_MC0_RST3 (*(volatile t_uint32 *)(0x8012E024)) &= ~(0xFFFC0000)


#define CmdCrcFail          (0x00000001)
#define DataCrcFail         (0x00000002)
#define CmdTimeOut          (0x00000004)
#define DataTimeOut         (0x00000008)
#define TxUnderrun          (0x00000010)
#define RxOverrun           (0x00000020)
#define CmdRespEnd          (0x00000040)
#define CmdSent          	(0x00000080)
#define DataEnd          	(0x00000100)
#define DataBlockEnd        (0x00000400)
#define CmdActive           (0x00000800)
#define TxActive            (0x00001000)
#define RxActive          	(0x00002000)
#define TxFifoHalfEmpty     (0x00004000)
#define RxFifoHalfFull      (0x00008000)
#define TxFifoFull          (0x00010000)
#define RxFifoFull          (0x00020000)
#define TxFifoEmpty         (0x00040000)
#define RxFifoEmpty         (0x00080000)
#define TxDataAvlbl         (0x00100000)
#define RxDataAvlbl         (0x00200000)
#define AllInterrupts		(0x003FFDFF)

#define ClrStaticFlags		(0x000005FF)



#endif /*End of File */

