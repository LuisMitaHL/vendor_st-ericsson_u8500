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

#include "services.h"

#define SER_AB8500_CORE_HWI2C_BIT              0x00000020
#define SER_AB8500_CORE_REQMB5                 0x80168F90
#define SER_AB8500_CORE_ACKMB5                 0x80168F14
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

#define AB8500_CORE_IT_LATCH_1_REGISTER			       0x40
/*-------------------------- Register offsets in AB8500_CORE_INTERRUPT_BLOCK --------------*/
#define AB8500_CORE_IT_LATCH_9_REGISTER        0x49
#define AB8500_CORE_IT_LATCH_15_REGISTER       0x4F
#define AB8500_CORE_IT_LATCH_18_REGISTER       0x52

#define AB8500_CORE_IT_MASK_1_REGISTER         0x60
	

/*--------Offsets in IT_LATCH_18 Register-------------*/
#define AB8500_CORE_IT_VBUS_DETECTR            0x08       /*Bug due to which bits are reversed */
#define AB8500_CORE_IT_VBUS_DETECTF            0x10
//#define AB8500_CORE_IT_USBID_DETECTR           0x20
//#define AB8500_CORE_IT_USBID_DETECTF           0x10

/*--------Offsets in IT_LATCH_9 Register-------------*/
#define AB8500_CORE_IT_USBSUS_DETECT           0x08
#define AB8500_CORE_IT_USBCHARGER_DETECT           0x02
	
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
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Write(t_uint8,t_uint8,t_uint8,t_uint8 *);
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Read(t_uint8,t_uint8,t_uint8,t_uint8 *,t_uint8 *);
PUBLIC void SER_AB8500_CORE_RegisterCallback(t_callback_fct,void *);
//PRIVATE t_ser_ab8500_core_error SER_AB8500_CORE_WriteThruHWI2C(t_uint8 ,t_uint8,t_uint32,t_uint8 *);
//PRIVATE t_ser_ab8500_core_error SER_AB8500_CORE_ReadThruHWI2C(t_uint8 , t_uint8 , t_uint32 , t_uint8 *, t_uint8 *);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

