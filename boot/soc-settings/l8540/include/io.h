/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/*
 * We won't use writel and readl here since definition of
 * writel(value, addr) is backwards
 */

#define write32(addr, value)	(*(volatile u32 *)(addr) = (u32)(value))
#define read32(addr)		(*(volatile u32 *)(addr))

#define clrbits32(addr, clear) \
	write32((addr), read32(addr) & ~(u32)(clear))
#define setbits32(addr, set) \
	write32((addr), read32(addr) | (u32) (set))

#endif /* __IO_H__ */
