/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private Header file of MultiMedia Card Interface/ SD-card Interface module.
* 
* author : ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HCL_MMC_IRQP_H_
#define _HCL_MMC_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif

/*------------------------------------------------------------------------
 * Type definations
 *----------------------------------------------------------------------*/
/* MMC registers */
typedef volatile struct
{
    t_uint32    mmc_power;          /* 0x00 */
    t_uint32    mmc_clock;          /* 0x04 */
    t_uint32    mmc_argument;       /* 0x08 */
    t_uint32    mmc_command;        /* 0x0c */
    t_uint32    mmc_resp_command;   /* 0x10 */
    t_uint32    mmc_response0;      /* 0x14 */
    t_uint32    mmc_response1;      /* 0x18 */
    t_uint32    mmc_response2;      /* 0x1c */
    t_uint32    mmc_response3;      /* 0x20 */
    t_uint32    mmc_data_timer;     /* 0x24 */
    t_uint32    mmc_data_length;    /* 0x28 */
    t_uint32    mmc_data_ctrl;      /* 0x2c */
    t_uint32    mmc_data_count;     /* 0x30 */
    t_uint32    mmc_status;         /* 0x34 */
    t_uint32    mmc_clear;          /* 0x38 */
    t_uint32    mmc_mask0;          /* 0x3c */
    t_uint32    mmc_mask1;          /* 0x40 */
    t_uint32    mmc_cardsel;        /* 0x44 */
    t_uint32    mmc_fifo_count;     /* 0x48 */
    t_uint32    mmc_unused1[(0x80 - 0x4C) >> 2];
    t_uint32    mmc_fifo;           /* 0x80 */
    t_uint32    mmc_unused2[(0xC0 - 0x84) >> 2];
    t_uint32    mmc_dbtimer;        /* 0xC0 */
    t_uint32    mmc_trsca;          /* 0xC4 */
    t_uint32    mmc_unused3[(0xFE0 - 0xC8) >> 2];
    t_uint32    mmc_periph_id0;     /* 0xFE0    mmc Peripheral Identi.cation Register */
    t_uint32    mmc_periph_id1;     /* 0xFE4 */
    t_uint32    mmc_periph_id2;     /* 0xFE8 */
    t_uint32    mmc_periph_id3;     /* 0xFEC */
    t_uint32    mmc_pcell_id0;      /* 0xFF0    mmc PCell Identi.cation Register */
    t_uint32    mmc_pcell_id1;      /* 0xFF4 */
    t_uint32    mmc_pcell_id2;      /* 0xFF8 */
    t_uint32    mmc_pcell_id3;      /* 0xFFc */
} t_mmc_register;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _HCL_MMC_IRQP_H_ */
