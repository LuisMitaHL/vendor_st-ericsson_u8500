/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Touareg2(STw4820) Core Services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _AB8500_CORE_SERVICES_H_
#define _AB8500_CORE_SERVICES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SER_AB8500_CORE_HWI2C_BIT              0x00000020
#define SER_AB8500_CORE_REQMB5                 0x8015FE44
#define SER_AB8500_CORE_ACKMB5                 0x8015FDF4
#define SER_AB8500_CORE_PRCM_MBOX_CPU_VAL      0x80157494
#define SER_AB8500_CORE_PRCM_MBOX_CPU_SET      0x80157100
#define SER_AB8500_CORE_PRCM_ARM_IT1_CLR       0x8015748C

#define SER_AB8500_CORE_TEST_BIT(reg_name,val)  HCL_READ_BITS(reg_name, val)

#define SER_AB8500_CORE_RX_FIFONOTEMPTY        0x04
#define SER_AB8500_CORE_FIFOFULL               0x1E
#define SER_AB8500_CORE_SSPFIFO                0x20

#define AB8500_CORE_MAX_SSPWRITE_DELAY         10000

#define SHIFT_AB8500_DATA 0x0A


#define AB8500_CORE_INTERRUPT_BLOCK            0x0E
/*-------------------------- Register offsets in AB8500_CORE_INTERRUPT_BLOCK --------------*/
#define AB8500_CORE_IT_LATCH_1_REGISTER        0x20
#define AB8500_CORE_IT_LATCH_2_REGISTER        0x21
#define AB8500_CORE_IT_LATCH_3_REGISTER        0x22
#define AB8500_CORE_IT_LATCH_4_REGISTER        0x23
#define	AB8500_CORE_IT_LATCH_5_REGISTER        0x24

#define AB8500_CORE_IT_MASK_1_REGISTER         0x40
#define AB8500_CORE_IT_MASK_19_REGISTER        0x52
	
/*--------Offsets in IT_LATCH_1 Register-------------*/
#define AB8500_CORE_IT_PONKEY1_DBR             0x80
#define AB8500_CORE_IT_PONKEY1_DBF             0x40
#define AB8500_CORE_IT_PONKEY2_DBR             0x20
#define AB8500_CORE_IT_PONKEY2_DBF             0x10

/*--------Offsets in IT_LATCH_2 Register-------------*/
#define AB8500_CORE_IT_VBUS_DETECTR            0x80//0x01       /*Bug due to which bits are reversed */
#define AB8500_CORE_IT_VBUS_DETECTF            0x40//0x02
#define AB8500_CORE_IT_USBID_DETECTR           0x20//0x04
#define AB8500_CORE_IT_USBID_DETECTF           0x10//x08
	
/*--------Offsets in IT_LATCH_3 Register-------------*/
#define AB8500_CORE_IT_RTC_ALARM               0x04

/*--------Offsets in IT_LATCH_4 Register-------------*/
#define AB8500_CORE_IT_INT_AUDIO               0x02//0x40

/*--------Offsets in IT_LATCH_5 Register-------------*/
#define AB8500_CORE_IT_ID_GPADC_VALID          0x01


#define SOC_VERSION_ADDRESS					   0x80151FD4
	
	
typedef enum
{
    SER_AB8500_CORE_OK                        = HCL_OK,
    SER_AB8500_CORE_ERROR                     = HCL_ERROR,
    SER_AB8500_CORE_UNSUPPORTED_FEATURE       = HCL_UNSUPPORTED_FEATURE,
    SER_AB8500_CORE_INVALID_PARAMETER         = HCL_INVALID_PARAMETER,
    SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED = HCL_MAX_ERROR_VALUE - 0,
    SER_AB8500_CORE_HWI2C_OPERATION_FAILED    = HCL_MAX_ERROR_VALUE - 1
} t_ser_ab8500_core_error;

typedef struct
{
    t_uint8 irq_src_register;
    t_uint8 irq_src;
}t_ser_ab8500_core_param;

PUBLIC void SER_AB8500_CORE_InterruptHandler(t_uint32);
PUBLIC void SER_AB8500_CORE_Init(void);
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Write(t_uint8,t_uint8,t_uint32,t_uint8 *);
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Read(t_uint8,t_uint8,t_uint32,t_uint8 *,t_uint8 *);
PUBLIC void SER_AB8500_CORE_RegisterCallback(t_callback_fct,void *);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

