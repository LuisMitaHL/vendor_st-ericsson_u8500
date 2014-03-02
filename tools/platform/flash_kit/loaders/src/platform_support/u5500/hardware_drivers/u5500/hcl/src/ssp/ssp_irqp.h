 /************************************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Interrupt management header file for the SSP Synchronous Serial Port module
* \author  ST-Ericsson
*/
/************************************************************************************************/

#ifndef _HCL_SSP_IRQP_H_
#define _HCL_SSP_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ssp_irq.h"

/*------------------------------------------------------------------------
 * Type definations
 *----------------------------------------------------------------------*/
/* SSP Interrupt Mask Set/Clear Register */
#define SSP_IRQ_ZERO    0x0UL

/* Receive Overrun Interrupt mask */
#define SSP_IMSC_MASK_RORIM 0x01

/* Receive timeout Interrupt mask */
#define SSP_IMSC_MASK_RTIM  0x02

/* Receive FIFO Interrupt mask */
#define SSP_IMSC_MASK_RXIM  0x04

/* Transmit FIFO Interrupt mask */
#define SSP_IMSC_MASK_TXIM  0x08

/* SSP Raw Interrupt Status Register */
/* Receive Overrun Raw Interrupt status */
#define SSP_RIS_MASK_RORRIS 0x01

/* Receive Timeout Raw Interrupt status */
#define SSP_RIS_MASK_RTRIS  0x02

/* Receive FIFO Raw Interrupt status */
#define SSP_RIS_MASK_RXRIS  0x04

/* Transmit FIFO Raw Interrupt status */
#define SSP_RIS_MASK_TXRIS  0x08

/* SSP Masked Interrupt Status Register */
/* Receive Overrun Masked Interrupt status */
#define SSP_MIS_MASK_RORMIS 0x01

/* Receive Timeout Masked Interrupt status */
#define SSP_MIS_MASK_RTMIS  0x02

/* Receive FIFO Masked Interrupt status */
#define SSP_MIS_MASK_RXMIS  0x04

/* Transmit FIFO Masked Interrupt status */
#define SSP_MIS_MASK_TXMIS  0x08

/* SSP Interrupt Clear Register */
/* Receive Overrun Raw Clear Interrupt bit */
#define SSP_ICR_MASK_RORIC  0x01

/* Receive Timeout Clear Interrupt bit */
#define SSP_ICR_MASK_RTIC   0x02

/* Instances based Macros */
/* SSP Registers */
typedef volatile struct
{
    t_uint32    ssp_cr0;                            /* SSP Control Register 0 *//*0x000*/
    t_uint32    ssp_cr1;                            /* SSP Control Register 1 *//*0x004*/
    t_uint32    ssp_dr;                             /* SSP Receive and Transmit FIFO Data Register *//*0x008*/
    t_uint32    ssp_sr;                             /* SSP Status Register *//*0x00C*/
    t_uint32    ssp_cpsr;                           /* SSP Clock Prescale Register *//*0x010*/
    t_uint32    ssp_imsc;                           /* SSP Interrupt Mask Set and Clear Register *//*0x014*/
    t_uint32    ssp_ris;                            /* SSP Raw Interrupt Status Register *//*0x018*/
    t_uint32    ssp_mis;                            /* SSP Masked Interrupt Status Register *//*0x01C*/
    t_uint32    ssp_icr;                            /* SSP Interrupt Clear Register *//*0x020*/
    t_uint32    ssp_dmacr;                          /* SSP DMA Control Register *//*0x024*/
    t_uint32    reserved_1[(0x080 - 0x028) >> 2];   /* Reserved *//*0x028*/
    t_uint32    ssp_itcr;                           /* SSP Integration Test Control Register *//*0x080*/
    t_uint32    ssp_itip;                           /* SSP Integration Test Input Register *//*0x084*/
    t_uint32    ssp_itop;                           /* SSP Integration Test Output Register *//*0x088*/
    t_uint32    ssp_tdr;                            /* SSP Test Data Register *//*0x08C*/
    t_uint32    reserved_2[(0xFE0 - 0x090) >> 2];   /* Reserved *//*0x090*/
    t_uint32    ssp_periphid0;                      /* SSP Peripheral Id Register bits 7:0 *//*0xFE0*/
    t_uint32    ssp_periphid1;                      /* SSP Peripheral Id Register bits 15:8 *//*0xFE4*/
    t_uint32    ssp_periphid2;                      /* SSP Peripheral Id Register bits 23:16 *//*0xFE8*/
    t_uint32    ssp_periphid3;                      /* SSP Peripheral Id Register bits 31:24 *//*0xFEC*/
    t_uint32    ssp_pcellid0;                       /* PrimeCell Id Register bits 7:0 *//*0xFF0*/
    t_uint32    ssp_pcellid1;                       /* PrimeCell id register: bits 15:8 *//*0xFF4*/
    t_uint32    ssp_pcellid2;                       /* PrimeCell id register: bits 23:16 *//*0xFF8*/
    t_uint32    ssp_pcellid3;                       /* PrimeCell id register: bits 31:24 *//*0xFFC*/
} t_ssp_register;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _HCL_SSP_H_ */

