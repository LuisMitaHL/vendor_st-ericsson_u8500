/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __INC_XL_L1_CACHE_MMU_MGMT_GCC_H
#define __INC_XL_L1_CACHE_MMU_MGMT_GCC_H

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

extern t_uint32 BOOT_TTBR0_ADDR;
#endif
