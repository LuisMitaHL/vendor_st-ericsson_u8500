/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------   */
/* 	                             Wireless Multimedia                     	 */
/*                           ROM code and Boot Image Team                    */
/*------------------------------------------------------------------------   */
/* This module provides template xloader.c file implementation               */
/* Specification release related to this implementation:                     */
/*------------------------------------------------------------------------   */
/*                                                                           */
/*****************************************************************************/
#ifndef __INC_XLOADER_H
#define __INC_XLOADER_H

#include "security.h"
#include "boot_types.h"
#include "boot_api.h"
#include "boot_error.h"
#include "public_code_header.h"
#include "nomadik_mapping.h"
#include "nomadik_registers.h"
#include "toc_mgmt.h"
#include "xloader_debug.h"
#include "xloader_common.h"
#include "xloader_modem.h"
#include "xloader_header.h"
#include "public_rom_api.h"
#include "mmc_mgmt.h"

#define PRCM_XP70_DATA_COMM        0x8015FFFC

extern  t_bool TOC_SearchItemInToc(t_uint32 tocAddress,
                                   char *p_stringItem,
                                   t_boot_toc_item *p_tocItem);


extern void XL_EnableIcache(void);
extern void XL_EnableDcache(void);
extern void XL_DisableIcache(void);
extern void XL_DisableMmu(void);
extern void XL_DisableDcache(void);
extern void XL_CleanInvDcache(void);
extern void XL_InvalidateDcache(void);
extern void XL_CleanInvDcache(void);
extern void XL_L2CacheCleanInvAllWays(void);

extern void XL_InvalidateIcacheArea(void *p_start, void *p_end);
extern void XL_InvalidateDcacheArea(void *p_start, void *p_end);
extern void XL_CleanDcacheArea(void *p_start, void *p_end);
extern void XL_InvalidateIcache(void);


#endif /*__INC_XLOADER_H*/
