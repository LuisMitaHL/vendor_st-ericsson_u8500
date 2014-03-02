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
PRIVATE void SER_mmc4_gpio_setting(void);
PRIVATE void SER_mmc1_gpio_setting(void);
PRIVATE void SER_mmc3_gpio_setting(void);
PRIVATE void SER_mmc2_gpio_setting(void);
PRIVATE void SER_mmc0_gpio_setting(void);
PUBLIC void SER_MMC_Init( t_uint8 );
PUBLIC void SER_MMC_Close( void );
PUBLIC t_ser_error SER_MMC_RegisterCallback(t_callback_fct,void* );
PUBLIC t_ser_error SER_MMC_WaitEnd(t_mmc_device_id );


#define PRCC_PERI1_SDI0_BUS_CLKEN		(*(volatile t_uint32 *)(0xA002F000)) |= 0x02
#define PRCC_PERI1_SDI0_KBUS_CLKEN      (*(volatile t_uint32 *)(0xA002F008)) |= 0x02

#define PRCC_PERI2_SDI1_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x10
#define PRCC_PERI2_SDI1_KBUS_CLKEN		(*(volatile t_uint32 *)(0x8011F008)) |= 0x10

#define PRCC_PERI2_SDI3_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x20
#define PRCC_PERI2_SDI3_KBUS_CLKEN		(*(volatile t_uint32 *)(0x8011F008)) |= 0x20


#define PRCMU_MMC_SET	(*(volatile t_uint32 *)(0x80157024)) = 0x128

#define PRCC_PERI2_SDI4_BUS_CLKEN		(*(volatile t_uint32 *)(0x8011F000)) |= 0x40
#define PRCC_PERI2_SDI4_KBUS_CLKEN 		(*(volatile t_uint32 *)(0x8011F008)) |= 0x40


#define PRCC_PERI1_SDI2_BUS_CLKEN		(*(volatile t_uint32 *)(0xA002F000)) |= 0x04
#define PRCC_PERI1_SDI2_KBUS_CLKEN	    (*(volatile t_uint32 *)(0xA002F008)) |= 0x04


/*MC Card 0*/
#define GPIO_SET_MC0_RST (*(volatile t_uint32 *)(0xA002E004)) |= 0xC0007FE0
#define GPIO_SET_MC0_RST1 (*(volatile t_uint32 *)(0xA002E00C)) &= ~(0xC0007FE0)
#define GPIO_SET_MC0_RST2 (*(volatile t_uint32 *)(0xA002E020)) |= 0xC0007FE0
#define GPIO_SET_MC0_RST3 (*(volatile t_uint32 *)(0xA002E024)) &= ~(0xC0007FE0)

#define GPIO_SET_MC0_RST_OUT (*(volatile t_uint32 *)(0xA002E014)) |= (0x0008000)
#define GPIO_SET_MC0_RST_HI (*(volatile t_uint32 *)(0xA002E004)) |= (0x0008000)

/*MC Card 1*/
/*Instance 5 related changes*/
#define GPIO_SET_MC1_RST (*(volatile t_uint32 *)(0x8011E004)) |= 0x80000000
#define GPIO_SET_MC1_RST1 (*(volatile t_uint32 *)(0x8011E00C)) &= ~(0x80000000)
#define GPIO_SET_MC1_RST2 (*(volatile t_uint32 *)(0x8011E020)) |= 0x80000000
#define GPIO_SET_MC1_RST3 (*(volatile t_uint32 *)(0x8011E024)) &= ~(0x80000000)

/*Instance 6 Related Changes*/
#define GPIO_SET_MC1_RST4 (*(volatile t_uint32 *)(0x8011E084)) |= 0x000000FF
#define GPIO_SET_MC1_RST5 (*(volatile t_uint32 *)(0x8011E08C)) &= ~(0x000000FF)
#define GPIO_SET_MC1_RST6 (*(volatile t_uint32 *)(0x8011E0A0)) |= 0x000000FF
#define GPIO_SET_MC1_RST7 (*(volatile t_uint32 *)(0x8011E0A4)) &= ~(0x000000FF)

/*MC Card 2*/
#define GPIO_SET_MC2_RST (*(volatile t_uint32 *)(0xA002E004)) |= 0x07FB0000
#define GPIO_SET_MC2_RST1 (*(volatile t_uint32 *)(0xA002E00C)) &= ~(0x07FB0000)
#define GPIO_SET_MC2_RST2 (*(volatile t_uint32 *)(0xA002E020)) |= 0x07FB0000
#define GPIO_SET_MC2_RST3 (*(volatile t_uint32 *)(0xA002E024)) &= ~(0x07FB0000)

/*MC Card 3*/
#define GPIO_SET_MC3_RST (*(volatile t_uint32 *)(0x8011E004)) |= 0x0001F800
#define GPIO_SET_MC3_RST1 (*(volatile t_uint32 *)(0x8011E00C)) &= ~(0x0001F800)
#define GPIO_SET_MC3_RST2 (*(volatile t_uint32 *)(0x8011E020)) |= 0x0001F800
#define GPIO_SET_MC3_RST3 (*(volatile t_uint32 *)(0x8011E024)) &= ~(0x0001F800)

/*MC Card 4*/
/*Instance 5 related changes*/
#define GPIO_SET_MC4_RST (*(volatile t_uint32 *)(0x8011E004)) |= 0xFE000000
#define GPIO_SET_MC4_RST1 (*(volatile t_uint32 *)(0x8011E00C)) &= ~(0xFE000000)
#define GPIO_SET_MC4_RST2 (*(volatile t_uint32 *)(0x8011E020)) &= ~(0xFE000000)
#define GPIO_SET_MC4_RST3 (*(volatile t_uint32 *)(0x8011E024)) |= 0xFE000000
/*Instance 6 Related Changes*/
#define GPIO_SET_MC4_RST4 (*(volatile t_uint32 *)(0x8011E084)) |= 0x00000007
#define GPIO_SET_MC4_RST5 (*(volatile t_uint32 *)(0x8011E08C)) &= ~(0x00000007)
#define GPIO_SET_MC4_RST6 (*(volatile t_uint32 *)(0x8011E0A0)) &= ~(0x00000007)
#define GPIO_SET_MC4_RST7 (*(volatile t_uint32 *)(0x8011E0A4)) |= 0x00000007


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

