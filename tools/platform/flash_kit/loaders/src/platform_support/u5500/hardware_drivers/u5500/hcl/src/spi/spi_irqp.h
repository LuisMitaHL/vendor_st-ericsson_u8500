/************************************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Interrupt management header file for the SPI Synchronous Serial Port module
*		   for U5500
* \author  ST-Ericsson
*/
/************************************************************************************************/
#ifndef _HCL_SPI_IRQP_H_
#define _HCL_SPI_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "spi_irq.h"

/*------------------------------------------------------------------------
 * Type definations
 *----------------------------------------------------------------------*/
/* SPI Interrupt Mask Set/Clear Register */
#define SPI_IRQ_ZERO    0x0UL

/* Receive Overrun Interrupt mask */
#define SPI_IMSC_MASK_RORIM 0x01
#define SBSPI_IMSC_RORIM    0x00

/* Receive timeout Interrupt mask */
#define SPI_IMSC_MASK_RTIM  0x02
#define SBSPI_IMSC_RTIM     0x01

/* Receive FIFO Interrupt mask */
#define SPI_IMSC_MASK_RXIM  0x04
#define SBSPI_IMSC_RXIM     0x02

/* Transmit FIFO Interrupt mask */
#define SPI_IMSC_MASK_TXIM  0x08
#define SBSPI_IMSC_TXIM     0x03

/* SPI Raw Interrupt Status Register */
/* Receive Overrun Raw Interrupt status */
#define SPI_RIS_MASK_RORRIS 0x01

/* Receive Timeout Raw Interrupt status */
#define SPI_RIS_MASK_RTRIS  0x02

/* Receive FIFO Raw Interrupt status */
#define SPI_RIS_MASK_RXRIS  0x04

/* Transmit FIFO Raw Interrupt status */
#define SPI_RIS_MASK_TXRIS  0x08

/* SPI Masked Interrupt Status Register */
/* Receive Overrun Masked Interrupt status */
#define SPI_MIS_MASK_RORMIS 0x01

/* Receive Timeout Masked Interrupt status */
#define SPI_MIS_MASK_RTMIS  0x02

/* Receive FIFO Masked Interrupt status */
#define SPI_MIS_MASK_RXMIS  0x04

/* Transmit FIFO Masked Interrupt status */
#define SPI_MIS_MASK_TXMIS  0x08

/* SPI Interrupt Clear Register */
/* Receive Overrun Raw Clear Interrupt bit */
#define SPI_ICR_MASK_RORIC  0x01
#define SPI_ICR_SB_RORIC    0x00

/* Receive Timeout Clear Interrupt bit */
#define SPI_ICR_MASK_RTIC   0x02
#define SPI_ICR_SB_RTIC     0x01


/* Instances based Macros */
/* SPI Registers */
typedef volatile struct
{
    t_uint32    spi_cr0;                            /* SPI Control Register 0 *//*0x000*/
    t_uint32    spi_cr1;                            /* SPI Control Register 1 *//*0x004*/
    t_uint32    spi_dr;                             /* SPI Receive and Transmit FIFO Data Register *//*0x008*/
    t_uint32    spi_sr;                             /* SPI Status Register *//*0x00C*/
    t_uint32    spi_cpsr;                           /* SPI Clock Prescale Register *//*0x010*/
    t_uint32    spi_imsc;                           /* SPI Interrupt Mask Set and Clear Register *//*0x014*/
    t_uint32    spi_ris;                            /* SPI Raw Interrupt Status Register *//*0x018*/
    t_uint32    spi_mis;                            /* SPI Masked Interrupt Status Register *//*0x01C*/
    t_uint32    spi_icr;                            /* SPI Interrupt Clear Register *//*0x020*/
    t_uint32    spi_dmacr;                          /* SPI DMA Control Register *//*0x024*/
    t_uint32    reserved_1[(0xFE0 - 0x028) >> 2];   /* Reserved *//* 0xFE0 - 0x028*/
    t_uint32    spi_periphid0;                      /* SPI Peripheral Id Register bits 7:0 *//*0xFE0*/
    t_uint32    spi_periphid1;                      /* SPI Peripheral Id Register bits 15:8 *//*0xFE4*/
    t_uint32    spi_periphid2;                      /* SPI Peripheral Id Register bits 23:16 *//*0xFE8*/
    t_uint32    spi_periphid3;                      /* SPI Peripheral Id Register bits 31:24 *//*0xFEC*/
    t_uint32    spi_pcellid0;                       /* PrimeCell Id Register bits 7:0 *//*0xFF0*/
    t_uint32    spi_pcellid1;                       /* PrimeCell id register: bits 15:8 *//*0xFF4*/
    t_uint32    spi_pcellid2;                       /* PrimeCell id register: bits 23:16 *//*0xFF8*/
    t_uint32    spi_pcellid3;                       /* PrimeCell id register: bits 31:24 *//*0xFFC*/
} t_spi_register;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _HCL_SPI_H_ */

