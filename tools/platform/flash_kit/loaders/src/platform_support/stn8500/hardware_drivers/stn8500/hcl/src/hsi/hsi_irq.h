/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of High Speed Serial Interface (HSI) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HSI_IRQ_H_
#define _HSI_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 * Includes							                                   
 *---------------------------------------------------------------------------*/
#include "hcl_defs.h"

/*-----------------------------------------------------------------------------
 * Devices in HSI module
 *---------------------------------------------------------------------------*/
typedef enum
{
    HSI_DEVICE_ID_INVALID   = -1,
    HSI_DEVICE_ID_TX        = 0,
    HSI_DEVICE_ID_RX        = 1
} t_hsi_device_id;

/*-----------------------------------------------------------------------------
 * IRQ sources
 *---------------------------------------------------------------------------*/
typedef enum
{
    HSI_IRQ_SRC_NO_IT          = 0x0,
    HSI_IRQ_SRC_TX_IRQ0        = 0x1,
    HSI_IRQ_SRC_TX_IRQ1        = 0x2,
    HSI_IRQ_SRC_TX_IRQ2        = 0x4,
    HSI_IRQ_SRC_TX_IRQ3        = 0x8,
    HSI_IRQ_SRC_TX_IRQ4        = 0x10,
    HSI_IRQ_SRC_TX_IRQ5        = 0x20,
    HSI_IRQ_SRC_TX_IRQ6        = 0x40,
    HSI_IRQ_SRC_TX_IRQ7        = 0x80,
    HSI_IRQ_SRC_RX_IRQ0        = 0x100,
    HSI_IRQ_SRC_RX_IRQ1        = 0x200,
    HSI_IRQ_SRC_RX_IRQ2        = 0x400,
    HSI_IRQ_SRC_RX_IRQ3        = 0x800,
    HSI_IRQ_SRC_RX_IRQ4        = 0x1000,
    HSI_IRQ_SRC_RX_IRQ5        = 0x2000,
    HSI_IRQ_SRC_RX_IRQ6        = 0x4000,
    HSI_IRQ_SRC_RX_IRQ7        = 0x8000,
    HSI_IRQ_SRC_RX_EX_TIMEOUT  = 0x10000,
    HSI_IRQ_SRC_RX_EX_OVERRUN  = 0x20000,
    HSI_IRQ_SRC_RX_EX_BREAK    = 0x40000,
    HSI_IRQ_SRC_RX_EX_PARITY   = 0x80000,
    HSI_IRQ_SRC_RX_EX_OV0      = 0x100000,
	HSI_IRQ_SRC_RX_EX_OV1      = 0x200000,
	HSI_IRQ_SRC_RX_EX_OV2      = 0x400000,
	HSI_IRQ_SRC_RX_EX_OV3      = 0x800000,
	HSI_IRQ_SRC_RX_EX_OV4      = 0x1000000,
	HSI_IRQ_SRC_RX_EX_OV5      = 0x2000000,
	HSI_IRQ_SRC_RX_EX_OV6      = 0x4000000,
	HSI_IRQ_SRC_RX_EX_OV7      = 0x8000000,
    HSI_IRQ_SRC_ALL_IT         = 0xFFFFFFF
} t_hsi_irq_src_id;

/*-----------------------------------------------------------------------------
 * Type defs for ORing of the interrupt sources.
 *---------------------------------------------------------------------------*/
typedef t_uint32        t_hsi_irq_src;

/*-----------------------------------------------------------------------------
 * Public M0 functions						                                   
 *---------------------------------------------------------------------------*/
PUBLIC void             HSI_SetBaseAddress(t_hsi_device_id device, t_logical_address base_address);

PUBLIC void             HSI_EnableIRQSrc(t_hsi_irq_src irq_src);
PUBLIC void             HSI_DisableIRQSrc(t_hsi_irq_src irq_src);
PUBLIC void             HSI_ClearIRQSrc(t_hsi_irq_src irq_src);

PUBLIC t_hsi_irq_src    HSI_GetIRQSrc(t_hsi_device_id device);
PUBLIC t_hsi_device_id  HSI_GetDeviceID(t_hsi_irq_src irq_src);
PUBLIC t_bool           HSI_IsPendingIRQSrc(t_hsi_irq_src irq_src);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _HSI_IRQ_H_ */

/* End of file - hsi_irq.h */

