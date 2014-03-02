/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private Header file of HASH Processor
*  Specification release related to this implementation: A_V2.2
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HASH_P_H_
#define _HASH_P_H_

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "hash.h"

/*--------------------------------------------------------------------------*
 * Error Functions															*
 *--------------------------------------------------------------------------*/
#define HASH_ERROR(msg)         DBGPRINT(DBGL_ERROR, msg)

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
/* Version defines */
#define HASH_HCL_VERSION_ID     1
#define HASH_HCL_MAJOR_ID       2
#define HASH_HCL_MINOR_ID       2

#define MAX_HASH_DEVICE     2

/* Maximum value of the length's high word */
#define HASH_HIGH_WORD_MAX_VAL  0xFFFFFFFFUL

/* Power on Reset values HASH registers */
#define HASH_RESET_CONTROL_REG_VALUE    0x0
#define HASH_RESET_START_REG_VALUE      0x0

/* Number of context swap registers */
#define HASH_CSR_COUNT              52

#define HASH_RESET_CSRX_REG_VALUE      0x0
#define HASH_RESET_CSFULL_REG_VALUE    0x0
#define HASH_RESET_CSDATAIN_REG_VALUE  0x0

#define HASH_RESET_INDEX_VAL        0x0
#define HASH_RESET_BIT_INDEX_VAL    0x0
#define HASH_RESET_BUFFER_VAL       0x0
#define HASH_RESET_LEN_HIGH_VAL     0x0
#define HASH_RESET_LEN_LOW_VAL      0x0

/*----------------------------*
 * Control register bitfields *
 *----------------------------*/
#define HASH_CR_RESUME_MASK     0x11FCF

#define HASH_CR_SWITCHON_POS    31
#define HASH_CR_SWITCHON_MASK   MASK_BIT31

#define HASH_CR_EMPTYMSG_POS    20
#define HASH_CR_EMPTYMSG_MASK   MASK_BIT20

#define HASH_CR_DINF_POS        12
#define HASH_CR_DINF_MASK       MASK_BIT12

#define HASH_CR_NBW_POS         8
#define HASH_CR_NBW_MASK        0x00000F00UL

#define HASH_CR_LKEY_POS        16
#define HASH_CR_LKEY_MASK       MASK_BIT16

#define HASH_CR_ALGO_POS        7
#define HASH_CR_ALGO_MASK       MASK_BIT7

#define HASH_CR_MODE_POS        6
#define HASH_CR_MODE_MASK       MASK_BIT6

#define HASH_CR_DATAFORM_POS    4
#define HASH_CR_DATAFORM_MASK   (MASK_BIT4 | MASK_BIT5)

#define HASH_CR_DMAE_POS        3
#define HASH_CR_DMAE_MASK       MASK_BIT3

#define HASH_CR_INIT_POS        2
#define HASH_CR_INIT_MASK       MASK_BIT2

#define HASH_CR_PRIVN_POS       1
#define HASH_CR_PRIVN_MASK      MASK_BIT1

#define HASH_CR_SECN_POS        0
#define HASH_CR_SECN_MASK       MASK_BIT0

/*--------------------------*
 * Start register bitfields *
 *--------------------------*/
#define HASH_STR_DCAL_POS   8
#define HASH_STR_DCAL_MASK  MASK_BIT8

#define HASH_STR_NBLW_POS   0
#define HASH_STR_NBLW_MASK  0x0000001FUL

#define HASH_NBLW_MAX_VAL   0x1F

/*------------------*
 * PrimeCell IDs    *
 *------------------*/
#define HASH_P_ID0          0xE0
#define HASH_P_ID1          0x05
#define HASH_P_ID2          0x38
#define HASH_P_ID3          0x00
#define HASH_CELL_ID0       0x0D
#define HASH_CELL_ID1       0xF0
#define HASH_CELL_ID2       0x05
#define HASH_CELL_ID3       0xB1

#define HASH_SET_DIN(val)   HCL_WRITE_REG(g_hash_system_context.p_hash_register[hash_device_id]->hash_din, (val))
#define HASH_INITIALIZE     HCL_WRITE_BITS \
        ( \
            g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, \
            0x01 << HASH_CR_INIT_POS, \
            HASH_CR_INIT_MASK \
        )
#define HASH_SET_DATA_FORMAT(data_format) HCL_WRITE_BITS \
        ( \
            g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, \
            (t_uint32) (data_format) << HASH_CR_DATAFORM_POS, \
            HASH_CR_DATAFORM_MASK \
        )
#define HASH_GET_HX(pos)        HCL_READ_REG(g_hash_system_context.p_hash_register[hash_device_id]->hash_hx[pos])
#define HASH_SET_HX(pos, val)   HCL_WRITE_REG(g_hash_system_context.p_hash_register[hash_device_id]->hash_hx[pos], (val));
#define HASH_SET_NBLW(val)      HCL_WRITE_BITS \
        ( \
            g_hash_system_context.p_hash_register[hash_device_id]->hash_str, \
            (t_uint32) (val) << HASH_STR_NBLW_POS, \
            HASH_STR_NBLW_MASK \
        )
#define HASH_SET_DCAL   HCL_WRITE_BITS \
        ( \
            g_hash_system_context.p_hash_register[hash_device_id]->hash_str, \
            0x01 << HASH_STR_DCAL_POS, \
            HASH_STR_DCAL_MASK \
        )

/*------------------------------*
 * HASH registers description   *
 *------------------------------*/
typedef volatile struct
{
    /* Type	    Name					            Description								   	   Offset */
    t_uint32    hash_cr;                        /* HASH Control Register  							0x000 */
    t_uint32    hash_din;                       /* Message Data Input register						0x004 */
    t_uint32    hash_str;                       /* HASH-1 digest computation Start register			0x008 */
    t_uint32    hash_hx[8];                     /* Different words of message digest (H0-H7)        0x00C-0x01C */
    t_uint32    padding0[(0x080 - 0x02C) >> 2]; /* Reserved								            0x02C */
    t_uint32    hash_itcr;                      /* Integration Test Control register                0x080 */
    t_uint32    hash_itip;                      /* Integration Test Input register					0x084 */
    t_uint32    hash_itop;                      /* Integration Test Output register					0x088 */
    t_uint32    padding1[(0x0F8 - 0x08C) >> 2]; /* Reserved								            0x08C */
    t_uint32    hash_csfull;                    /* Context full register (CS Flag)                  0x0F8 */
    t_uint32    hash_csdatain;                  /* Context swap data input register                 0x0FC */
    t_uint32    hash_csr0;                      /* Context swap registers 0                         0x100 */
    t_uint32    hash_csrx[51];                  /* Context swap registers 1-51                      0x104-0x1CC */
    t_uint32    padding2[(0xFE0 - 0x1D0) >> 2]; /* Reserved								            0x1D0 */
    t_uint32    hash_periphid0;                 /* Peripheral Identifiation register bits 7:0		0xFE0 */
    t_uint32    hash_periphid1;                 /* Peripheral Identification register bits 15:8		0xFE4 */
    t_uint32    hash_periphid2;                 /* Peripheral Identification register bits 23:16	0xFE8 */
    t_uint32    hash_periphid3;                 /* Peripheral Identification register bits 31:24	0xFEC */
    t_uint32    hash_cellid0;                   /* IPCell Identification register bits 7:0			0xFF0 */
    t_uint32    hash_cellid1;                   /* IPCell Identification register bits 15:8			0xFF4 */
    t_uint32    hash_cellid2;                   /* IPCell Identification register bits 23:16		0xFF8 */
    t_uint32    hash_cellid3;                   /* IPCell Identification register bits 31:24		0xFFC */
} t_hash_register;

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
typedef struct
{
    t_hash_register *p_hash_register[MAX_HASH_DEVICE];   /* Pointer to HASH registers structure */
    t_hash_state    hash_state[MAX_HASH_DEVICE];         /* State of HASH device */
} t_hash_system_context;

/*--------------------------------------------------------------------------*
 * Private functions declarations                                           *
 *--------------------------------------------------------------------------*/
#ifdef __HASH_ENHANCED   /* Enhanced layer APIs */
PRIVATE void hash_ProcessBlock(IN t_hash_device_id hash_device_id, IN const t_uint32 *message);
PRIVATE void hash_MessagePad(IN t_hash_device_id hash_device_id, IN const t_uint32 *message, IN t_uint8 index_bytes);
PRIVATE void hash_IncrementLength(IN t_hash_device_id hash_device_id, IN t_uint32 incr);
#endif /* __HASH_ENHANCED */

PRIVATE void hash_InitializeGlobals(IN t_hash_device_id hash_device_id);

#endif /* End _HASH_P_H_ */

/* End of file - hash_p.h */

