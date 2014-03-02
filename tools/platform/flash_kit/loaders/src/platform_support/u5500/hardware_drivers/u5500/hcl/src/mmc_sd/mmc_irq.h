/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Public Header file of MultiMedia Card Interface/ SD-card Interface module.
* 
* author : ST-Ericsson
*/
/*****************************************************************************


****************************************************************************/
#ifndef _MMC_IRQ_H_
#define _MMC_IRQ_H_

/*---------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif


/* Defines for instances */
#define NUM_MMC_INSTANCES   6
/* THERE IS ONE MORE INTERRUPT SOURCE START_BIT_ERROR */

#if (defined ST_5500V2)
#define MMCALLINTERRUPTS    (0x1FFFFFFF)
#else
#define MMCALLINTERRUPTS    (0x007FFFFF)
#endif

/* Enum to identify MMC_SD device blocks */
typedef enum { 
    MMC_DEVICE_ID_0,
    MMC_DEVICE_ID_1,
    MMC_DEVICE_ID_2,
    MMC_DEVICE_ID_3,
    MMC_DEVICE_ID_4,
    MMC_DEVICE_ID_5,
    MMC_DEVICE_ID_INVALID =0x6
} t_mmc_device_id;

/*---------------------------------------------------------------------------
 * Types
 *---------------------------------------------------------------------------*/
typedef t_uint32    t_mmc_irq_src;

/*---------------------------------------------------------------------------
 * Enums
 *---------------------------------------------------------------------------*/
typedef enum
{
    MMC_IRQ_CMD_CRC_FAIL        = 0x00000001,
    MMC_IRQ_DATA_CRC_FAIL       = 0x00000002,
    MMC_IRQ_CMD_TIMEOUT         = 0x00000004,
    MMC_IRQ_DATA_TIMEOUT        = 0x00000008,
    MMC_IRQ_TX_UNDERRUN         = 0x00000010,
    MMC_IRQ_RX_OVERRUN          = 0x00000020,
    MMC_IRQ_CMD_RESP_OK         = 0x00000040,
    MMC_IRQ_CMD_SENT            = 0x00000080,
    MMC_IRQ_DATA_END            = 0x00000100,
    MMC_IRQ_START_BIT_ERR       = 0x00000200,
    MMC_IRQ_DATA_BLOCK_OK       = 0x00000400,
    MMC_IRQ_CMD_ACTIVE          = 0x00000800,
    MMC_IRQ_TX_ACTIVE           = 0x00001000,
    MMC_IRQ_RX_ACTIVE           = 0x00002000,
    MMC_IRQ_TX_FIFO_HALF_EMPTY  = 0x00004000,
    MMC_IRQ_RX_FIFO_HALF_FULL   = 0x00008000,
    MMC_IRQ_TX_FIFO_FULL        = 0x00010000,
    MMC_IRQ_RX_FIFO_FULL        = 0x00020000,
    MMC_IRQ_TX_FIFO_EMPTY       = 0x00040000,
    MMC_IRQ_RX_FIFO_EMPTY       = 0x00080000,
    MMC_IRQ_TX_DATA_AVLBL       = 0x00100000,
    MMC_IRQ_RX_DATA_AVLBL       = 0x00200000,
    MMC_IRQ_SDIO                = 0x00400000,
    MMC_IRQ_ALL_STATIC          = 0x004007FF,
    MMC_IRQ_ALL                 = 0x007FFFFF
} t_mmc_irq_src_id;

/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC void             MMC_SetBaseAddress(t_logical_address, t_mmc_device_id );
PUBLIC void             MMC_EnableIRQSrc(t_mmc_irq_src,t_mmc_device_id);
PUBLIC void             MMC_DisableIRQSrc(t_mmc_irq_src,t_mmc_device_id);
PUBLIC t_mmc_irq_src    MMC_GetIRQSrc(t_mmc_device_id);
PUBLIC void             MMC_ClearIRQSrc(t_mmc_irq_src,t_mmc_device_id);
PUBLIC t_bool           MMC_IsPendingIRQSrc(t_mmc_irq_src,t_mmc_device_id);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _MMC_IRQ_H_ */
