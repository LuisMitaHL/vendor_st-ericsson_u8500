/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MSP_IRQ_H_
#define _MSP_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

/*--------------------------------------------------------------------------*
 * Data types																	*
 *--------------------------------------------------------------------------*/
/* Device Ids for MSP
-------------------------------------------*/
typedef enum
{
    MSP_DEVICE_ID_0     = 0,
    MSP_DEVICE_ID_1     = 1,
    MSP_DEVICE_ID_2     = 2,
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	MSP_DEVICE_ID_3 	= 3,
#endif
    MSP_LAST_DEVICE_ID
} t_msp_device_id;

/* Interrupts definition
-----------------------------*/
typedef enum
{
    MSP0_IRQ_SRC_RX_SERVICE         = 0x0001,
    MSP0_IRQ_SRC_RX_OVERRUN_ERROR   = 0x0002,
    MSP0_IRQ_SRC_RX_FRAME_ERROR     = 0x0004,
    MSP0_IRQ_SRC_RX_FRAME_SYNC      = 0x0008,
    MSP0_IRQ_SRC_TX_SERVICE         = 0x0010,
    MSP0_IRQ_SRC_TX_UNDERRUN_ERROR  = 0x0020,
    MSP0_IRQ_SRC_TX_FRAME_ERROR     = 0x0040,
    MSP0_IRQ_SRC_TX_FRAME_SYNC      = 0x0080,
    MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY  = 0x0100,
    MSP0_IRQ_SRC_TX_FIFO_NOT_FULL   = 0x0200,
    MSP0_IRQ_SRC_ALL                = 0x03FF,
    MSP0_IRQ_SRC_DISABLE_ALL        = 0x0000,

    MSP1_IRQ_SRC_RX_SERVICE         = 0x1001,
    MSP1_IRQ_SRC_RX_OVERRUN_ERROR   = 0x1002,
    MSP1_IRQ_SRC_RX_FRAME_ERROR     = 0x1004,
    MSP1_IRQ_SRC_RX_FRAME_SYNC      = 0x1008,
    MSP1_IRQ_SRC_TX_SERVICE         = 0x1010,
    MSP1_IRQ_SRC_TX_UNDERRUN_ERROR  = 0x1020,
    MSP1_IRQ_SRC_TX_FRAME_ERROR     = 0x1040,
    MSP1_IRQ_SRC_TX_FRAME_SYNC      = 0x1080,
    MSP1_IRQ_SRC_RX_FIFO_NOT_EMPTY  = 0x1100,
    MSP1_IRQ_SRC_TX_FIFO_NOT_FULL   = 0x1200,
    MSP1_IRQ_SRC_ALL                = 0x13FF,
    MSP1_IRQ_SRC_DISABLE_ALL        = 0x1000,

    MSP2_IRQ_SRC_RX_SERVICE         = 0x2001,
    MSP2_IRQ_SRC_RX_OVERRUN_ERROR   = 0x2002,
    MSP2_IRQ_SRC_RX_FRAME_ERROR     = 0x2004,
    MSP2_IRQ_SRC_RX_FRAME_SYNC      = 0x2008,
    MSP2_IRQ_SRC_TX_SERVICE         = 0x2010,
    MSP2_IRQ_SRC_TX_UNDERRUN_ERROR  = 0x2020,
    MSP2_IRQ_SRC_TX_FRAME_ERROR     = 0x2040,
    MSP2_IRQ_SRC_TX_FRAME_SYNC      = 0x2080,
    MSP2_IRQ_SRC_RX_FIFO_NOT_EMPTY  = 0x2100,
    MSP2_IRQ_SRC_TX_FIFO_NOT_FULL   = 0x2200,
    MSP2_IRQ_SRC_ALL                = 0x23FF,
    MSP2_IRQ_SRC_DISABLE_ALL        = 0x2000,
	
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	MSP3_IRQ_SRC_RX_SERVICE         = 0x3001,
    MSP3_IRQ_SRC_RX_OVERRUN_ERROR   = 0x3002,
    MSP3_IRQ_SRC_RX_FRAME_ERROR     = 0x3004,
    MSP3_IRQ_SRC_RX_FRAME_SYNC      = 0x3008,
    MSP3_IRQ_SRC_TX_SERVICE         = 0x3010,
    MSP3_IRQ_SRC_TX_UNDERRUN_ERROR  = 0x3020,
    MSP3_IRQ_SRC_TX_FRAME_ERROR     = 0x3040,
    MSP3_IRQ_SRC_TX_FRAME_SYNC      = 0x3080,
    MSP3_IRQ_SRC_RX_FIFO_NOT_EMPTY  = 0x3100,
    MSP3_IRQ_SRC_TX_FIFO_NOT_FULL   = 0x3200,
    MSP3_IRQ_SRC_ALL                = 0x33FF,
    MSP3_IRQ_SRC_DISABLE_ALL        = 0x3000,
#endif
  
} t_msp_irq_src_id;

/* Enumeration t_msp_irq_src_id or ORing of enumeration t_msp_irq_src_id *
 *-----------------------------------------------------------------------*/
typedef t_uint32        t_msp_irq_src;

/*--------------------------------------------------------------------------*
 * Functions declaration													*
 *--------------------------------------------------------------------------*/
/* Initialization functions *
 *--------------------------*/
PUBLIC void             MSP_SetBaseAddress(IN t_msp_device_id msp_device_id, IN t_logical_address msp_base_address);

/* Interrupt Management	* 
 *----------------------*/
PUBLIC t_msp_device_id  MSP_GetDeviceId(IN t_msp_irq_src msp_irq_src);
PUBLIC void             MSP_EnableIRQSrc(IN t_msp_irq_src enable_irq);
PUBLIC void             MSP_DisableIRQSrc(IN t_msp_irq_src disable_irq);
PUBLIC t_msp_irq_src    MSP_GetIRQSrc(IN t_msp_device_id msp_device_id);
PUBLIC void             MSP_ClearIRQSrc(IN t_msp_irq_src clear_irq);
PUBLIC t_bool           MSP_IsPendingIRQSrc(IN t_msp_irq_src pending_irq);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _MSP_IRQ_H_ */

/* End of file - msp_irq.h */

