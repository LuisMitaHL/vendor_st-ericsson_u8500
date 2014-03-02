/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup DMA
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file dma.h
 *  \author ST Ericsson
 *  \brief This module provides functions declaration used for the DMA driver
 */
/*---------------------------------------------------------------------------*/

#ifndef __INC_DMA_H
#define __INC_DMA_H

#if 0
#include <stdio.h>
#include <string.h>
#endif
#include "boot_types.h"
#include "boot_error.h"
#include "nomadik_registers.h"

#define MAX_LLI_NB 50

/* element size used for both USB and MMC */
#define DMA_ELEM_SIZE  4 // in bytes
#define DMA_ELEM_SIZE_REG   2 // 4 bytes 

/** This type stores the DMA use flag */
typedef enum{
DMA_NOT_USED,    /**< (0), DMA not use to transfer binary */
DMA_USED         /**< (1), DMA use to transfer binary */
}t_dma_use;


/** \internal  Lists the different state of the dma transfer. */	
typedef enum
{
    DMA_USB_STOP     = 0,
    DMA_USB_RUN      = 1,
    DMA_USB_ERROR    = 2,
    DMA_USB_FINISHED = 3
} t_usb_dma_status;


/** \internal  Lists the different sources of DMA request. */	
typedef enum {
    	DMA_USB,    /**< \internal (0), USB source for DMA transfer. */
        DMA_MMC     /**< \internal (1), MMC source for DMA transfer */
} t_dma_periph_src;


/*---------------------------------------------------------------------------*/
/* DMA Public functions                                                      */
/*---------------------------------------------------------------------------*/
PUBLIC void DMA_Init(void);
PUBLIC t_boot_error DMA_ItProcess(void);


#endif /*__INC_DMA_H */
/* end of dma.h */
/** @} */
