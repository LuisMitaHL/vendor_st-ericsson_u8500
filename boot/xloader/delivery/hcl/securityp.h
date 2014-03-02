/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------
 *                               TPA / MPU
 *                       MultiMedia Platform Unit
 *------------------------------------------------------------------------
 * This module stands for the private header file of the secure ROM code API.
 *------------------------------------------------------------------------
 *
 ****************************************************************************/

#ifndef _SECURE_CODE_P_H
#define _SECURE_CODE_P_H

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "security.h"

/*--------------------------------------------------------------------------*
 * Constants                                                                *
 *--------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------*
 * Constants                                                                *
 *--------------------------------------------------------------------------*/
 
#define BOOT_PHYSICAL_BASE_ADDR			0x90000000
#define NOT_INITIALIZED					0xFF
 
// Current secure ROM HCL version
#define SECURE_HCL_VERSION_ID				7
#define SECURE_HCL_MAJOR_ID					0
#define SECURE_HCL_MINOR_ID					0


/*GGG



// For L2CC management due to ROM code bugs
#define L2CC_CTRL_REG_OFFSET				0x100
#define L2CC_INVBYWAY_REG_OFFSET			0x7FC
#define L2CC_BYWAY_REG_OFFSET			    0x7BC
#define L2CC_SYNC_REG_OFFSET			    0x730
#define L2CC_CLEANBYPA_REG_OFFSET			0x7B0
#define L2CC_ENABLED						0x1UL
#define L2CC_DISABLED						0x0UL
#define L2CC_PROCESS_ALL_WAYS				0xFFUL
#define L2CC_ALL_WAYS_PROCESSED				0x00UL
#define L2CC_NEXT_LINE						0x20

GG*/

typedef struct
{
	t_uint32 ChecksumAddr;
	t_uint32 ChecksumValue;
	t_uint32 ConversionFunc;
	t_uint32 BridgeFunc;
	t_uint32 CutName;
} t_sec_rom_cut_desc;


#endif      // _SECURE_CODE_P_H
