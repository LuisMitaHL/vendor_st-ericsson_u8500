/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __DSILINK__H__
#define __DSILINK__H__

#include "dbx500-prcmu.h"

static inline void cpu_relax(void) { udelay(1); }

#define DSI_READ_TIMEOUT_MS 200
#define DSI_WRITE_CMD_TIMEOUT_MS 1000
#define DSI_READ_DELAY_US 5
#define DSI_READ_NBR_OF_RETRIES 2
#define DSI_WAIT_FOR_ULPM_STATE_MS 1
#define DSI_ULPM_STATE_NBR_OF_RETRIES 10
#define DSI_CMD_TRANSMISSION	1

#define DSILINK_TRACE(x)	dev_dbg((struct device *)x, "%s\n", __func__)

static inline u32 dsi_rreg(u8 *io, u32 reg)
{
	return readl(io + reg);
}

static inline void dsi_wreg(u8 *io, u32 reg, u32 val)
{
	writel(val, io + reg);
}

#define dsi_rfld(__i, __reg, __fld) \
({ \
	const u32 mask = __reg##_##__fld##_MASK; \
	const u32 shift = __reg##_##__fld##_SHIFT; \
	((dsi_rreg(__i, __reg) & mask) >> shift); \
})

#define dsi_wfld(__i, __reg, __fld, __val) \
({ \
	const u32 mask = __reg##_##__fld##_MASK; \
	const u32 shift = __reg##_##__fld##_SHIFT; \
	const u32 oldval = dsi_rreg(__i, __reg); \
	const u32 newval = ((__val) << shift); \
	dsi_wreg(__i, __reg, (oldval & ~mask) | (newval & mask)); \
})

#define dsi_wfld_ro(__i, __reg, __fld, __val) \
({ \
	const u32 mask = __reg##_##__fld##_MASK; \
	const u32 shift = __reg##_##__fld##_SHIFT; \
	const u32 oldval = 0; \
	const u32 newval = ((__val) << shift); \
	dsi_wreg(__i, __reg, (oldval & ~mask) | (newval & mask)); \
})

static inline void switch_byteclk_to_sys_clk(u8 link)
{
	u32 value;

	value = prcmu_read(PRCM_DSI_GLITCHFREE_EN);
	switch (link) {
	case 0:
		value |= DB8500_PRCM_DSI_GLITCHFREE_EN_DSI0_BYTE_CLK;
		break;
	case 1:
		value |= DB8500_PRCM_DSI_GLITCHFREE_EN_DSI1_BYTE_CLK;
		break;
	case 2:
		value |= DB8500_PRCM_DSI_GLITCHFREE_EN_DSI2_BYTE_CLK;
		break;
	default:
		break;
	}
	prcmu_write(PRCM_DSI_GLITCHFREE_EN, value);
}

static inline void switch_byteclk_to_hs_clk(u8 link)
{
	u32 value;

	value = prcmu_read(PRCM_DSI_GLITCHFREE_EN);
	switch (link) {
	case 0:
		value &= ~DB8500_PRCM_DSI_GLITCHFREE_EN_DSI0_BYTE_CLK;
		break;
	case 1:
		value &= ~DB8500_PRCM_DSI_GLITCHFREE_EN_DSI1_BYTE_CLK;
		break;
	case 2:
		value &= ~DB8500_PRCM_DSI_GLITCHFREE_EN_DSI2_BYTE_CLK;
		break;
	default:
		break;
	}
	prcmu_write(PRCM_DSI_GLITCHFREE_EN, value);
}

struct dsilink_ops;

void __init nova_dsilink_v2_init(struct dsilink_ops *ops);
void __init nova_dsilink_v3_init(struct dsilink_ops *ops);

#endif
