/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file for MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MSP_IRQP_H_
#define _MSP_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "msp_irq.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/* Number of MSPs */
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define MSP_INSTANCES   4
#else
#define MSP_INSTANCES 	3
#endif

#define MSP_SHIFT_BY_TWELVE 12

#define MSP_MASK_INTERRUPT_BITS 0x000003FF

#define MSP_WRITE_BITS(reg, val, bit_nb, pos)   ((reg) = (((reg) &~(bit_nb << pos)) | (((val) & bit_nb) << pos)))
#define MSP_READ_BITS(reg, bit_nb, pos)         ((reg) & (bit_nb << pos))
#define MSP_SET_BIT(reg, pos)                   ((reg) |= (t_uint32) (1 << pos))
#define MSP_CLEAR_BIT(reg, pos)                 ((reg) &= ~(t_uint32) (1 << pos))
#define ONE_BIT                                 0x1UL
#define TWO_BITS                                0x3UL
#define THREE_BITS                              0x7UL
#define SIX_BITS                                0x3FUL
#define SEVEN_BITS                              0x7FUL
#define EIGHT_BITS                              0xFFUL
#define TEN_BITS                                0x3FFUL
#define THIRTEEN_BITS                           0x1FFFUL

/* Global configuration register
--------------------------------*/
#define RXEN_BIT        0
#define RFFEN_BIT       1
#define RFSPOL_BIT      2
#define DCM_BIT         3
#define RFSSEL_BIT      4
#define RCKPOL_BIT      5
#define RCKSEL_BIT      6
#define LBM_BIT         7
#define TXEN_BIT        8
#define TFFEN_BIT       9
#define TFSPOL_BIT      10
#define TFSSEL_BIT      11
#define TCKPOL_BIT      13
#define TCKSEL_BIT      14
#define TXDDL_BIT       15
#define TBSWAP_BIT      28
#define RBSWAP_BIT      28
#define SGEN_BIT        16
#define SCKPOL_BIT      17
#define SCKSEL_BIT      18
#define FGEN_BIT        20
#define SPICKM_BIT      21
#define SPIBURST_BIT    23

/* Transmit and receive configuration register
----------------------------------------------*/
#define P1ELEN_BIT  0
#define P1FLEN_BIT  3
#define DTYP_BIT    10
#define ENDN_BIT    12
#define DDLY_BIT    13
#define FSIG_BIT    15
#define P2ELEN_BIT  16
#define P2FLEN_BIT  19
#define P2SM_BIT    26
#define P2EN_BIT    27

/* Flag register
--------------------*/
#define RBUSY_BIT   0
#define RFE_BIT     1
#define RFU_BIT     2
#define TBUSY_BIT   3
#define TFE_BIT     4
#define TFU_BIT     5

/* Multichannel control register
---------------------------------*/
#define RMCEN_BIT   0
#define RMCSF_BIT   1
#define RCMPM_BIT   3
#define TMCEN_BIT   5
#define TNCSF_BIT   6

/* Sample rate generator register
------------------------------------*/
#define SCKDIV_BIT  0
#define FRWID_BIT   10
#define FRPER_BIT   16

/* DMA controller register
---------------------------*/
#define RDMAE_BIT   0
#define TDMAE_BIT   1

/* Watermark Register Control
-----------------------------*/
#define TWMRK_BIT 						3
#define RWMRK_BIT						0
#define RWMRK_MASK 						0x7
#define TWMRK_MASK						0x38
#define DEFAULT_WMRK_LEVEL				4
#define ELEMENTS_2						2
#define ELEMENTS_4						4
#define	ELEMENTS_8						8
#define ELEMENTS_16						16
/*Interrupt Register
-----------------------------------------*/
#define RECEIVE_SERVICE_INTERRUPT_BIT   0
#define RECEIVE_OVERRUN_ERROR_BIT       1
#define RECEIVE_FRAME_SYNC_ERR_INT_BIT  2
#define RECEIVE_FRAME_SYNC_INT_BIT      3
#define TRANSMIT_INTERRUPT_SERVICE_BIT  4
#define TRANSMIT_UNDERRUN_ERR_INT_BIT   5
#define TRANSMIT_FRAME_SYNC_ERR_INT_BIT 6
#define TRANSMIT_FRAME_SYNC_INT_BIT     7
#define RECEIVE_FIFO_NOT_EMPTY_INT_BIT  8
#define TRANSMIT_FIFO_NOT_FULL_INT_BIT  9

/* MSP registers description
-----------------------------*/
typedef volatile struct
{
    /* type	 name				   description								offset*/
    t_uint32    msp_dr;                         /* fifo data (receive or transmit)			0x000 */
    t_uint32    msp_gcr;                        /* global control							0x004 */
    t_uint32    msp_tcf;                        /* transmit configuration					0x008 */
    t_uint32    msp_rcf;                        /* receive configuration					0x00c */
    t_uint32    msp_srg;                        /* sample rate generator control			0x010 */
    t_uint32    msp_flr;                        /* status flag								0x014 */
    t_uint32    msp_dmacr;                      /* dma control								0x018 */  
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	t_uint32	msp_wmrk;						/* msp Fifo watermark Level					0x01C */
#else
    t_uint32    padding0;                       /* reserved									0x01c */    
#endif
    t_uint32    msp_imsc;                       /* interrupt mask set and clear				0x020 */
    t_uint32    msp_ris;                        /* raw interrupt status						0x024 */
    t_uint32    msp_mis;                        /* masked interrupt status					0x028 */
    t_uint32    msp_icr;                        /* interrupt clear							0x02c */
    t_uint32    msp_mcr;                        /* multichannel control						0x030 */
    t_uint32    msp_rcv;                        /* receive comparison value					0x034 */
    t_uint32    msp_rcm;                        /* receive comparison mask					0x038 */
    t_uint32    padding1;                       /* reserved									0x03c */
    t_uint32    msp_tce0;                       /* transmit channel 0 enable				0x040 */
    t_uint32    msp_tce1;                       /* transmit channel 1 enable				0x044 */
    t_uint32    msp_tce2;                       /* transmit channel 2 enable				0x048 */
    t_uint32    msp_tce3;                       /* transmit channel 3 enable				0x04c */
    t_uint32    padding2[4];                    /* reserved									0x050 */
    t_uint32    msp_rce0;                       /* receive channel 0 enable					0x060 */
    t_uint32    msp_rce1;                       /* receive channel 1 enable					0x064 */
    t_uint32    msp_rce2;                       /* receive channel 2 enable					0x068 */
    t_uint32    msp_rce3;                       /* receive channel 3 enable					0x06c */
    t_uint32    padding3[4];                    /* reserved									0x070 */
    t_uint32    msp_tstcr;                      /* test control								0x080 */
    t_uint32    msp_itip;                       /* integration test input					0x084 */
    t_uint32    msp_itop;                       /* integration test output					0x088 */
    t_uint32    msp_tstdr;                      /* test data								0x08c */
    t_uint32    padding4[(0xfe0 - 0x90) >> 2];  /* reserved						0x090 */
    t_uint32    msp_periphid0;                  /* peripheral identification (bits7:0)		0xfe0 */
    t_uint32    msp_periphid1;                  /* peripheral identification (bits15:8)		0xfe4 */
    t_uint32    msp_periphid2;                  /* peripheral identification (bits23:16)	0xfe8 */
    t_uint32    msp_periphid3;                  /* peripheral identification (bits31:24)	0xfec */
    t_uint32    msp_cellid0;                    /* ipcell identification (bits7:0)			0xff0 */
    t_uint32    msp_cellid1;                    /* ipcell identification (bits15:8)			0xff4 */
    t_uint32    msp_cellid2;                    /* ipcell identification (bits23:16)		0xff8 */
    t_uint32    msp_cellid3;                    /* ipcell identification (bits31:24)		0xffc */
} t_msp_register;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _MSP_IRQP_H_*/

/* End of file - msp_irqp.h*/

