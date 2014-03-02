 /************************************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Synchronous Serial Port 
* \author  ST-Ericsson
*/
/************************************************************************************************/

#ifndef __INC_SSP_IRQ_H
#define __INC_SSP_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ssp.h"

#define NUM_SSP_INSTANCES   2

/*------------------------------------------------------------------------
 * Structures and enums
 *----------------------------------------------------------------------*/
/* Interrupt related enum */
/* Enum to check which interrupt is asserted */
typedef enum
{
    SSP_IRQ_SRC_TRANSMIT        = 0x01, /* Asserted when the number of elements in Tx
										FIFO is less than the programmed Watermark level */
    SSP_IRQ_SRC_RECEIVE         = 0x02, /* Asserted when the number of elements in Rx
										FIFO is more than the programmed Watermark level */
    SSP_IRQ_SRC_RECEIVE_TIMEOUT = 0x04, /* Asserted when Rx FIFO is not empty & no
										further data is received over a 32 bit period */
    SSP_IRQ_SRC_RECEIVE_OVERRUN = 0x08  /* Receive FIFO is already full & an additional
										frame is received */
} t_ssp_irq_src_id;

typedef t_uint32    t_ssp_irq_src;


/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
PUBLIC void SSP_SetBaseAddress(t_ssp_device_id, t_logical_address);
PUBLIC void SSP_EnableIRQSrc(t_ssp_device_id, t_ssp_irq_src);
PUBLIC void SSP_DisableIRQSrc(t_ssp_device_id, t_ssp_irq_src);
PUBLIC void SSP_ClearIRQSrc(t_ssp_device_id, t_ssp_irq_src);
PUBLIC t_ssp_irq_src SSP_GetIRQSrc(t_ssp_device_id);
PUBLIC t_bool SSP_IsPendingIRQSrc(t_ssp_device_id, t_ssp_irq_src_id);
#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_SSP_IRQ_H */

/* End of file - ssp_irq.h */

