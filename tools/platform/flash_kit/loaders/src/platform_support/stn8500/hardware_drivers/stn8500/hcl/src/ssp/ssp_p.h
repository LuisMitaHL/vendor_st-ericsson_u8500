 /*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of Synchronous Serial Port (PL022) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_SSP_P_H
#define __INC_SSP_P_H

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ssp_irqp.h"
#include "ssp.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define BIT_MASK(__bws)                     ((t_uint32) (((1U << (WB ## __bws)) - 1)) << (SB ## __bws))
#define PCL_WRITE_BITS(reg, val, mask, sb)  ((reg) = (((reg) &~(mask)) | (((val) << (sb)) & (mask))))
#define PCL_READ_BITS(reg,mask,sb)            (reg & (mask << sb))                  

/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/
#define SSP_MASKBIT0                0x01
#define MIN_CPSDVR                  0x02
#define MAX_CPSDVR                  0xFE
#define MIN_SCR                     0x00
#define MAX_SCR                     0xFF
#define SSP_ENABLE_ALL_INTERRUPT    0x0F
#define SSP_DISABLE_ALL_INTERRUPT   0x00
#define SSP_LAST8BITS               0x000000FF
#define SSP_ALLZERO                 0x00000000

/* Define depending on SSP Type: Lite or Full */
#define SSP_FIFOSIZE    32
#define SSP_FIFOWIDTH   32
#define SSP_PERIPHID0   0x22
#define SSP_PERIPHID1   0x00
#define SSP_PERIPHID2   0x08
#define SSP_PERIPHID3   0x01
#define SSP_PCELLID0    0x0D
#define SSP_PCELLID1    0xF0
#define SSP_PCELLID2    0x05
#define SSP_PCELLID3    0xB1

/* SSP Control Register 0  */
/* dataSize (check)*/
#define WBSSP_CR0_DSS       5
#define SBSSP_CR0_DSS       0
#define SSP_CR0_MASK_DSS    BIT_MASK(SSP_CR0_DSS)

/* half duplex mode */
#define WBSSP_CR0_HALFDUP       1
#define SBSSP_CR0_HALFDUP       5
#define SSP_CR0_MASK_HALFDUP    BIT_MASK(SSP_CR0_HALFDUP)

/* clockPolarity */
#define WBSSP_CR0_SPO       1
#define SBSSP_CR0_SPO       6
#define SSP_CR0_MASK_SPO    BIT_MASK(SSP_CR0_SPO)

/* clockPhase */
#define WBSSP_CR0_SPH       1
#define SBSSP_CR0_SPH       7
#define SSP_CR0_MASK_SPH    BIT_MASK(SSP_CR0_SPH)

/* clockRate */
#define WBSSP_CR0_SCR       8
#define SBSSP_CR0_SCR       8
#define SSP_CR0_MASK_SCR    BIT_MASK(SSP_CR0_SCR)

/*command size select */
#define WBSSP_CR0_CSS       5
#define SBSSP_CR0_CSS       16
#define SSP_CR0_MASK_CSS    BIT_MASK(SSP_CR0_CSS)

/* frameFormat	*/
#define WBSSP_CR0_FRF       2
#define SBSSP_CR0_FRF       21
#define SSP_CR0_MASK_FRF    BIT_MASK(SSP_CR0_FRF)

/* SSP Control Register 1 */
/* Loop back mode */
#define WBSSP_CR1_LBM       1
#define SBSSP_CR1_LBM       0
#define SSP_CR1_MASK_LBM    BIT_MASK(SSP_CR1_LBM)

/* SSP Enable */
#define WBSSP_CR1_SSE       1
#define SBSSP_CR1_SSE       1
#define SSP_CR1_MASK_SSE    BIT_MASK(SSP_CR1_SSE)

/* Master or Slave mode Select */
#define WBSSP_CR1_MS    1
#define SBSSP_CR1_MS    2
#define SSP_CR1_MASK_MS BIT_MASK(SSP_CR1_MS)

/* slave mode output disable */
#define WBSSP_CR1_SOD       1
#define SBSSP_CR1_SOD       3
#define SSP_CR1_MASK_SOD    BIT_MASK(SSP_CR1_SOD)

/* receive Endian Format */
#define WBSSP_CR1_RENDN     1
#define SBSSP_CR1_RENDN     4
#define SSP_CR1_MASK_RENDN  BIT_MASK(SSP_CR1_RENDN)

/* Transmit Endian Format */
#define WBSSP_CR1_TENDN     1
#define SBSSP_CR1_TENDN     5
#define SSP_CR1_MASK_TENDN  BIT_MASK(SSP_CR1_TENDN)

/* microwire control length */
#define WBSSP_CR1_MWAIT     1
#define SBSSP_CR1_MWAIT     6
#define SSP_CR1_MASK_MWAIT  BIT_MASK(SSP_CR1_MWAIT)

/* Receive Interrupt FIFO Level Select */
#define WBSSP_CR1_RXIFLSEL      3
#define SBSSP_CR1_RXIFLSEL      7
#define SSP_CR1_MASK_RXIFLSEL   BIT_MASK(SSP_CR1_RXIFLSEL)

/* Transmit Interrupt FIFO Level Select */
#define WBSSP_CR1_TXIFLSEL      3
#define SBSSP_CR1_TXIFLSEL      10
#define SSP_CR1_MASK_TXIFLSEL   BIT_MASK(SSP_CR1_TXIFLSEL)

/* SSP Data Register */
/* Transmit or Receive Data */
#define SSP_DR_MASK_DATA    0xFFFFFFFF

/* SSP Status Register */
/* Transmit FIFO empty */
#define SSP_SR_MASK_TFE 0x01

/* Transmit FIFO not full */
#define SSP_SR_MASK_TNF 0x02

/* Receive FIFO not empty */
#define SSP_SR_MASK_RNE 0x04

/* Receive FIFO full */
#define SSP_SR_MASK_RFF 0x08

/* Busy Flag */
#define SSP_SR_MASK_BSY 0x10

/* SSP Clock Prescale Register */
#define WBSSP_CPSR_CPSDVSR      8
#define SBSSP_CPSR_CPSDVSR      0
#define SSP_CPSR_MASK_CPSDVSR   BIT_MASK(SSP_CPSR_CPSDVSR)

/* SSP DMA Control Register */
/* Receive DMA Enable bit */
#define SSP_DMACR_MASK_RXDMAE   0x01

/* Transmit DMA Enable bit */
#define SSP_DMACR_MASK_TXDMAE   0x02

/* SSP Integration Test Control Register */
#define WBSSP_ITCR_ITEN         1
#define SBSSP_ITCR_ITEN         0
#define SSP_ITCR_MASK_ITEN      0x01

#define WBSSP_ITCR_TESTFIFO     1
#define SBSSP_ITCR_TESTFIFO     1
#define SSP_ITCR_MASK_TESTFIFO  0x02

/* SSP Integration Test Input Register */
#define ITIP_MASK_SSPRXD    0x01
#define ITIP_MASK_SSPFSSIN  0x02
#define ITIP_MASK_SSPCLKIN  0x04
#define ITIP_MASK_RXDMAC    0x08
#define ITIP_MASK_TXDMAC    0x10
#define ITIP_MASK_SSPTXDIN  0x20

/* SSP Integration Test Output Register */
#define ITOP_MASK_SSPTXD    0x01
#define ITOP_MASK_SSPFSSOUT 0x02
#define ITOP_MASK_SSPCLKOUT 0x04
#define ITOP_MASK_SSPOEn    0x08
#define ITOP_MASK_SSPCTLOEn 0x10
#define ITOP_MASK_RORINTR   0x20
#define ITOP_MASK_RTINTR    0x40
#define ITOP_MASK_RXINTR    0x80
#define ITOP_MASK_TXINTR    0x100
#define ITOP_MASK_INTR      0x200
#define ITOP_MASK_RXDMABREQ 0x400
#define ITOP_MASK_RXDMASREQ 0x800
#define ITOP_MASK_TXDMABREQ 0x1000
#define ITOP_MASK_TXDMASREQ 0x2000

/* SSP Test Data Register */
#define TDR_MASK_TESTDATA   0xFFFFFFFF

/* Setting Control Register 0 bitfields */
#define SSP_SET_DSS(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_DSS, SBSSP_CR0_DSS)
#define SSP_SET_FRF(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_FRF, SBSSP_CR0_FRF)
#define SSP_SET_SPO(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_SPO, SBSSP_CR0_SPO)
#define SSP_SET_SPH(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_SPH, SBSSP_CR0_SPH)
#define SSP_SET_SCR(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_SCR, SBSSP_CR0_SCR)
#define SSP_SET_HALFDUP(cr0, a) PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_HALFDUP, SBSSP_CR0_HALFDUP)
#define SSP_SET_CSS(cr0, a)     PCL_WRITE_BITS(cr0, a, SSP_CR0_MASK_CSS, SBSSP_CR0_CSS)

/* Setting Control Register 1 bitfields */
#define SSP_SET_LBM(cr1, a)         PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_LBM,SBSSP_CR1_LBM)
#define SSP_GET_LBM(cr1)            PCL_READ_BITS(cr1,SSP_CR1_MASK_LBM,SBSSP_CR1_LBM)
#define SSP_SET_SSE(cr1, a)         PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_SSE, SBSSP_CR1_SSE)
#define SSP_SET_MS(cr1, a)          PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_MS, SBSSP_CR1_MS)
#define SSP_SET_SOD(cr1, a)         PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_SOD, SBSSP_CR1_SOD)
#define SSP_SET_RENDN(cr1, a)       PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_RENDN, SBSSP_CR1_RENDN)
#define SSP_SET_TENDN(cr1, a)       PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_TENDN, SBSSP_CR1_TENDN)
#define SSP_SET_MWCLEN(cr1, a)      PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_MWCLEN, SBSSP_CR1_MWCLEN)
#define SSP_SET_RXIFLSEL(cr1, a)    PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_RXIFLSEL, SBSSP_CR1_RXIFLSEL)
#define SSP_SET_TXIFLSEL(cr1, a)    PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_TXIFLSEL, SBSSP_CR1_TXIFLSEL)
#define SSP_SET_MWAIT(cr1, a)       PCL_WRITE_BITS(cr1, a, SSP_CR1_MASK_MWAIT, SBSSP_CR1_MWAIT)

/* Setting SSP Clock Prescale Register bitfield */
#define SSP_SET_CPSDVSR(cpsr, a)    PCL_WRITE_BITS(cpsr, a, SSP_CPSR_MASK_CPSDVSR, SBSSP_CPSR_CPSDVSR)

/* Setting SSP Integration Test Control Register bitfield */
#define SSP_SET_ITEN(itcr, a)       PCL_WRITE_BITS(itcr, a, SSP_ITCR_MASK_ITEN, SBSSP_ITCR_ITEN)
#define SSP_SET_TESTFIFO(itcr, a)   PCL_WRITE_BITS(itcr, a, SSP_ITCR_MASK_TESTFIFO, SBSSP_ITCR_TESTFIFO)

/*------------------------------------------------------------------------
 *  Private Functions declaration
 *----------------------------------------------------------------------*/
PRIVATE t_bool  ssp_IsSspMaster(t_ssp_device_id);

/*------------------------------------------------------------------------
 * Structures
 *----------------------------------------------------------------------*/

/* SSP system context */
typedef struct
{
    t_ssp_register  *p_ssp_register;
    t_ssp_mode      tx_com_mode;
    t_ssp_mode      rx_com_mode;
    t_uint32        device_context[8];
} t_ssp_system_context;
#endif /* __INC_SSP_P_H */

// End of file - ssp_p.h

