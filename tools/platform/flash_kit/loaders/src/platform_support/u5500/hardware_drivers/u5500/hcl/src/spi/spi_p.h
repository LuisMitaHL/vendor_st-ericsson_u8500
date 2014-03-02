/**************************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of Serial Periphiral Interface (PL022) module for U5500
* \author  ST-Ericsson
*/
/****************************************************************************************/
#ifndef __INC_SPI_P_H
#define __INC_SPI_P_H

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "spi_irqp.h"
#include "spi.h"
/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define BIT_MASK(__bws)                     ((t_uint32) (((1U << (WB ## __bws)) - 1)) << (SB ## __bws))
#define PCL_WRITE_BITS(reg, val, mask, sb)  ((reg) = (((reg) &~(mask)) | (((val) << (sb)) & (mask))))
#define PCL_READ_BITS(reg,mask)             HCL_READ_BITS(reg,mask)

/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/
#define SPI_MIN_CPSDVR                  0x02
#define SPI_MAX_CPSDVR                  0xFE
#define SPI_MIN_SCR                     0x00
#define SPI_MAX_SCR                     0xFF
#define SPI_ENABLE_ALL_INTERRUPT        0x0F
#define SPI_DISABLE_ALL_INTERRUPT       0x00
#define SPI_LAST8BITS                   0x000000FF
#define SPI_ALLZERO                     0x00000000


#define SPI_MASK_SLAVE_ENABLE           0x07

/* Define depending on SPI Type: Lite or Full */
#define SPI_FIFOSIZE    32
#define SPI_FIFOWIDTH   32
#define SPI_PERIPHID0   0x22
#define SPI_PERIPHID1   0x00
#define SPI_PERIPHID2   0x08
#define SPI_PERIPHID3   0x00
#define SPI_PCELLID0    0x0D
#define SPI_PCELLID1    0xF0
#define SPI_PCELLID2    0x05
#define SPI_PCELLID3    0xB1

/* SPI Control Register 0  */
/* dataSize (check)*/
#define WBSPI_CR0_DSS       5
#define SBSPI_CR0_DSS       0
#define SPI_CR0_MASK_DSS    BIT_MASK(SPI_CR0_DSS)

/* half duplex mode */
#define WBSPI_CR0_HALFDUP       1
#define SBSPI_CR0_HALFDUP       5
#define SPI_CR0_MASK_HALFDUP    BIT_MASK(SPI_CR0_HALFDUP)

/* clockPolarity */
#define WBSPI_CR0_SPO       1
#define SBSPI_CR0_SPO       6
#define SPI_CR0_MASK_SPO    BIT_MASK(SPI_CR0_SPO)

/* clockPhase */
#define WBSPI_CR0_SPH       1
#define SBSPI_CR0_SPH       7
#define SPI_CR0_MASK_SPH    BIT_MASK(SPI_CR0_SPH)

/* clockRate */
#define WBSPI_CR0_SCR       8
#define SBSPI_CR0_SCR       8
#define SPI_CR0_MASK_SCR    BIT_MASK(SPI_CR0_SCR)


/* SPI Control Register 1 */

/* SPI Enable */
#define WBSPI_CR1_SSE       1
#define SBSPI_CR1_SSE       1
#define SPI_CR1_MASK_SSE    BIT_MASK(SPI_CR1_SSE)

/* Master or Slave mode Select */
#define WBSPI_CR1_MS    1
#define SBSPI_CR1_MS    2
#define SPI_CR1_MASK_MS BIT_MASK(SPI_CR1_MS)

/* slave mode output disable */
#define WBSPI_CR1_SOD       1
#define SBSPI_CR1_SOD       3
#define SPI_CR1_MASK_SOD    BIT_MASK(SPI_CR1_SOD)

/* receive Endian Format */
#define WBSPI_CR1_RENDN     1
#define SBSPI_CR1_RENDN     4
#define SPI_CR1_MASK_RENDN  BIT_MASK(SPI_CR1_RENDN)

/* Transmit Endian Format */
#define WBSPI_CR1_TENDN     1
#define SBSPI_CR1_TENDN     5
#define SPI_CR1_MASK_TENDN  BIT_MASK(SPI_CR1_TENDN)

/* Receive Interrupt FIFO Level Select */
#define WBSPI_CR1_RXIFLSEL      3
#define SBSPI_CR1_RXIFLSEL      7
#define SPI_CR1_MASK_RXIFLSEL   BIT_MASK(SPI_CR1_RXIFLSEL)

/* Transmit Interrupt FIFO Level Select */
#define WBSPI_CR1_TXIFLSEL      3
#define SBSPI_CR1_TXIFLSEL      10
#define SPI_CR1_MASK_TXIFLSEL   BIT_MASK(SPI_CR1_TXIFLSEL)

/* SPI Data Register */
/* Transmit or Receive Data */
#define SPI_DR_MASK_DATA    0xFFFFFFFF

/* SPI Status Register */
/* Transmit FIFO empty */
#define SPI_SR_MASK_TFE 0x01

/* Transmit FIFO not full */
#define SPI_SR_MASK_TNF 0x02

/* Receive FIFO not empty */
#define SPI_SR_MASK_RNE 0x04

/* Receive FIFO full */
#define SPI_SR_MASK_RFF 0x08

/* Busy Flag */
#define SPI_SR_MASK_BSY 0x10

/* SPI Clock Prescale Register */
#define WBSPI_CPSR_CPSDVSR      8
#define SBSPI_CPSR_CPSDVSR      0
#define SPI_CPSR_MASK_CPSDVSR   BIT_MASK(SPI_CPSR_CPSDVSR)

/* All interrupts Mask */
#define WBSPI_MIS_ALL           0x04
#define SBSPI_MIS_ALL           0x00
#define SPI_MIS_MASK_ALL        BIT_MASK(SPI_MIS_ALL)

/* SPI DMA Control Register */
/* Receive DMA Enable bit */
#define SPI_DMACR_MASK_RXDMAE   0x01

/* Transmit DMA Enable bit */
#define SPI_DMACR_MASK_TXDMAE   0x02

/* SPI Integration Test Control Register */
#define WBSPI_ITCR_ITEN         1
#define SBSPI_ITCR_ITEN         0
#define SPI_ITCR_MASK_ITEN      0x01

#define WBSPI_ITCR_TESTFIFO     1
#define SBSPI_ITCR_TESTFIFO     1
#define SPI_ITCR_MASK_TESTFIFO  0x02

/* SPI Integration Test Input Register */
#define ITIP_MASK_SPIRXD    0x01
#define ITIP_MASK_SPIFSSIN  0x02
#define ITIP_MASK_SPICLKIN  0x04
#define ITIP_MASK_RXDMAC    0x08
#define ITIP_MASK_TXDMAC    0x10
#define ITIP_MASK_SPITXDIN  0x20

/* SPI Integration Test Output Register */
#define ITOP_MASK_SPITXD    0x01
#define ITOP_MASK_SPIFSSOUT 0x02
#define ITOP_MASK_SPICLKOUT 0x04
#define ITOP_MASK_SPIOEn    0x08
#define ITOP_MASK_SPICTLOEn 0x10
#define ITOP_MASK_RORINTR   0x20
#define ITOP_MASK_RTINTR    0x40
#define ITOP_MASK_RXINTR    0x80
#define ITOP_MASK_TXINTR    0x100
#define ITOP_MASK_INTR      0x200
#define ITOP_MASK_RXDMABREQ 0x400
#define ITOP_MASK_RXDMASREQ 0x800
#define ITOP_MASK_TXDMABREQ 0x1000
#define ITOP_MASK_TXDMASREQ 0x2000

/* SPI Test Data Register */
#define TDR_MASK_TESTDATA   0xFFFFFFFF

/* Setting Control Register 0 bitfields */
#define SPI_SET_DSS(cr0, a)     PCL_WRITE_BITS(cr0, a, SPI_CR0_MASK_DSS, SBSPI_CR0_DSS)
#define SPI_SET_HALFDUP(cr0, a) PCL_WRITE_BITS(cr0, a, SPI_CR0_MASK_HALFDUP, SBSPI_CR0_HALFDUP)
#define SPI_SET_SPO(cr0, a)     PCL_WRITE_BITS(cr0, a, SPI_CR0_MASK_SPO, SBSPI_CR0_SPO)
#define SPI_SET_SPH(cr0, a)     PCL_WRITE_BITS(cr0, a, SPI_CR0_MASK_SPH, SBSPI_CR0_SPH)
#define SPI_SET_SCR(cr0, a)     PCL_WRITE_BITS(cr0, a, SPI_CR0_MASK_SCR, SBSPI_CR0_SCR)

/* Setting Control Register 1 bitfields */
#define SPI_SET_SSE(cr1, a)         PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_SSE, SBSPI_CR1_SSE)
#define SPI_GET_SSE(cr1)            PCL_READ_BITS(cr1,SPI_CR1_MASK_SSE)
#define SPI_SET_MS(cr1, a)          PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_MS, SBSPI_CR1_MS)
#define SPI_SET_SOD(cr1, a)         PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_SOD, SBSPI_CR1_SOD)
#define SPI_SET_RENDN(cr1, a)       PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_RENDN, SBSPI_CR1_RENDN)
#define SPI_SET_TENDN(cr1, a)       PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_TENDN, SBSPI_CR1_TENDN)
#define SPI_SET_RXIFLSEL(cr1, a)    PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_RXIFLSEL, SBSPI_CR1_RXIFLSEL)
#define SPI_SET_TXIFLSEL(cr1, a)    PCL_WRITE_BITS(cr1, a, SPI_CR1_MASK_TXIFLSEL, SBSPI_CR1_TXIFLSEL)

/* Setting SPI Clock Prescale Register bitfield */
#define SPI_SET_CPSDVSR(cpsr, a)    PCL_WRITE_BITS(cpsr, a, SPI_CPSR_MASK_CPSDVSR, SBSPI_CPSR_CPSDVSR)

/* Setting the Interrupts(i.e. Masking\Unmasking) the interrupts */
#define SPI_SET_RORIM(imsc,a)       PCL_WRITE_BITS(imsc,a,SPI_IMSC_MASK_RORIM,SBSPI_IMSC_RORIM)
#define SPI_SET_RTIM(imsc,a)        PCL_WRITE_BITS(imsc,a,SPI_IMSC_MASK_RTIM ,SBSPI_IMSC_RTIM)
#define SPI_SET_RXIM(imsc,a)        PCL_WRITE_BITS(imsc,a,SPI_IMSC_MASK_RXIM ,SBSPI_IMSC_RXIM)
#define SPI_SET_TXIM(imsc,a)        PCL_WRITE_BITS(imsc,a,SPI_IMSC_MASK_TXIM ,SBSPI_IMSC_TXIM) 
#define SPI_GET_INTR(mis)           PCL_READ_BITS(mis,SPI_MIS_MASK_ALL)

/* Setting the bits in the Interrupt Clear Register */
#define SPI_SET_ICR_RORIC(icr,a)    PCL_WRITE_BITS(icr,a,SPI_ICR_MASK_RORIC,SPI_ICR_SB_RORIC)   
#define SPI_SET_ICR_RTIC(icr,a)     PCL_WRITE_BITS(icr,a,SPI_ICR_MASK_RTIC,SPI_ICR_SB_RTIC)   
/* Setting SPI Integration Test Control Register bitfield */
#define SPI_SET_ITEN(itcr, a)       PCL_WRITE_BITS(itcr, a, SPI_ITCR_MASK_ITEN, SBSPI_ITCR_ITEN)
#define SPI_SET_TESTFIFO(itcr, a)   PCL_WRITE_BITS(itcr, a, SPI_ITCR_MASK_TESTFIFO, SBSPI_ITCR_TESTFIFO)


/*------------------------------------------------------------------------
 * Structures
 *----------------------------------------------------------------------*/

/* SPI system context */
typedef struct
{
             t_spi_register                *p_spi_register;
             t_spi_config                  *p_spi_config;
             t_spi_mode                     tx_com_mode;
             t_spi_mode                     rx_com_mode;
             t_spi_event                    spi_event;
             t_spi_status                   tx_status;
             t_spi_status                   rx_status;
             t_spi_transfer_data_size       transfer_data_size;
    		 t_spi_event                    spi_event_status;           /* Each event corresponds to a bit set to TRUE when the event is active.*/
             t_uint32                       device_context[8];
} t_spi_system_context;

PRIVATE t_spi_error spip_InterruptRecieveData(IN t_spi_device_id);
PRIVATE void spip_InterruptRecieverTimeout(IN t_spi_device_id);
PRIVATE t_spi_error spip_InterruptTransmitData(IN t_spi_device_id);
PRIVATE t_spi_error spi_checkevent(IN t_spi_event *,IN t_spi_filter_mode);
PRIVATE t_watermark_level spi_setrxlowerwatermark(IN t_spi_rx_level_trig);
PRIVATE t_watermark_level spi_setrxupperwatermark(IN t_spi_rx_level_trig);
PRIVATE t_watermark_level spi_settxlowerwatermark(IN t_spi_tx_level_trig);
PRIVATE t_watermark_level spi_settxupperwatermark(IN t_spi_tx_level_trig);
PRIVATE t_bool spi_IsSpiMaster(IN t_spi_device_id); 

#endif /* __INC_SPI_P_H */

// End of file - spi_p.h

