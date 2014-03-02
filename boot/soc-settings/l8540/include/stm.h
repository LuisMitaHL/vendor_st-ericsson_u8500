/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Magnus Templing <magnus.templing at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __STM_H__
#define __STM_H__

/* memory mapping */
#define STM_CONF_BASE	0xF000

/* Register in the logic block */
#define STM_CR		0x00
#define STM_MMC		0x08
#define STM_TER		0x10

/* control register bits */
#define STM_CR_SWAP_PRCMU	(1 << 9)
#define STM_CR_XCKDIV_2		(0 << 6)
#define STM_CR_XCKDIV_4		(1 << 6)
#define STM_CR_XCKDIV_6		(2 << 6)
#define STM_CR_XCKDIV_8		(3 << 6)
#define STM_CR_XCKDIV_10	(4 << 6)
#define STM_CR_XCKDIV_12	(5 << 6)
#define STM_CR_XCKDIV_14	(6 << 6)
#define STM_CR_XCKDIV_16	(7 << 6)
#define STM_CR_DWNG_4		(0 << 4)
#define STM_CR_DWNG_2		(1 << 4)
#define STM_CR_DWNG_1		(2 << 4)
#define STM_CR_DWNG_UNUSED	(3 << 4)
#define STM_CR_TSNTS		(1 << 3)
#define STM_CR_PLP2		(1 << 2)
#define STM_CR_PLP1		(1 << 1)
#define STM_CR_PLP0		(1 << 0)

/* MIPI modes control register bits */
#define STM_MMC_HWNSW5		(1 << 5)
#define STM_MMC_HWNSW4		(1 << 4)
#define STM_MMC_HWNSW3		(1 << 3)
#define STM_MMC_HWNSW2		(1 << 2)
#define STM_MMC_HWNSW1		(1 << 1)
#define STM_MMC_HWNSW0		(1 << 0)

/* trace enable register bits */
#define STM_TER_TE9		(1 << 9)
#define STM_TER_TE5		(1 << 5)
#define STM_TER_TE4		(1 << 4)
#define STM_TER_TE3		(1 << 3)
#define STM_TER_TE2		(1 << 2)
#define STM_TER_TE0		(1 << 0)

#endif /* __STM_H__ */
