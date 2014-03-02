/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file dma.h
 * \brief This file holds the public functions and data structures used by ROM code.
 * \author ST-Ericsson
 *
 * \addtogroup DMA
 *
 *
 * @{
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

/**
 * \internal 
 * Channel identificator
 * Physical channel number
 */
typedef enum
{
    CHANNEL_0 = 0,
    CHANNEL_1 = 1,
    CHANNEL_2 = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
    CHANNEL_ERROR = 8
} t_dma_phys_chl_id;

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
