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


typedef struct
{
	t_uint32 ChecksumAddr;
	t_uint32 ChecksumValue;
	t_uint32 ConversionFunc;
	t_uint32 BridgeFunc;
	t_uint32 CutName;
} t_sec_rom_cut_desc;


#endif      // _SECURE_CODE_P_H
