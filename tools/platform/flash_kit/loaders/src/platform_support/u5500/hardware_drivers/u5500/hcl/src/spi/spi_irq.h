 /*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Serial Periphiral Interface for U5500
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __INC_SPI_IRQ_H
#define __INC_SPI_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"

#define NUM_SPI_INSTANCES   4


/*------------------------------------------------------------------------
 * Structures and enums
 *----------------------------------------------------------------------*/
/* Interrupt related enum */
/* Enum to check which interrupt is asserted */
typedef enum
{
    SPI_IRQ_SRC_TRANSMIT        = 0x08, /* Asserted when the number of elements in Tx
										FIFO is less than the programmed Watermark level */
    SPI_IRQ_SRC_RECEIVE         = 0x04, /* Asserted when the number of elements in Rx
										FIFO is more than the programmed Watermark level */
    SPI_IRQ_SRC_RECEIVE_TIMEOUT = 0x02, /* Asserted when Rx FIFO is not empty & no
										further data is received over a 32 bit period */
    SPI_IRQ_SRC_RECEIVE_OVERRUN = 0x01, /* Receive FIFO is already full & an additional
										frame is received */
    SPI_IRQ_SRC_ALL_IT          = 0x10, /* Asserted when there are all the interrupts */
} t_spi_irq_src_id;

typedef t_uint32    t_spi_irq_src;

/* Enum to identify SPI device blocks */
typedef enum
{
    SPI_DEVICE_ID_0             = 0x00,
    SPI_DEVICE_ID_1             = 0x01,
    SPI_DEVICE_ID_2             = 0x02,
    SPI_DEVICE_ID_3             = 0x03,
    SPI_DEVICE_ID_INVALID       = 0x04,
} t_spi_device_id;

/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
PUBLIC void SPI_SetBaseAddress(t_spi_device_id, t_logical_address);
PUBLIC void SPI_EnableIRQSrc(t_spi_device_id, t_spi_irq_src);
PUBLIC void SPI_DisableIRQSrc(t_spi_device_id, t_spi_irq_src);
PUBLIC void SPI_ClearIRQSrc(t_spi_device_id, t_spi_irq_src);
PUBLIC t_spi_irq_src SPI_GetIRQSrc(t_spi_device_id);
PUBLIC t_bool SPI_IsPendingIRQSrc(t_spi_device_id, t_spi_irq_src_id);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_SPI_IRQ_H */

/* End of file - spi_irq.h */

