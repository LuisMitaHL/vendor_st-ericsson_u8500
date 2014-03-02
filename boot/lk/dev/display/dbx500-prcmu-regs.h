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

#ifndef __DB8500_PRCMU_REGS_H
#define __DB8500_PRCMU_REGS_H

/* temporary file */

#define IO_ADDRESS(a) (a)
#define _PRCMU_BASE IO_ADDRESS(U8500_PRCMU_BASE)

#define BITS(_start, _end) ((BIT(_end) - BIT(_start)) + BIT(_end))

#define PRCM_CLK_MGT(_offset) (void *)(IO_ADDRESS(U8500_PRCMU_BASE) \
	+ _offset)

#define PRCM_LCDCLK_MGT		PRCM_CLK_MGT(0x044)
#define PRCM_SPARE1CLK_MGT	PRCM_CLK_MGT(0x048)
#define PRCM_HDMICLK_MGT	PRCM_CLK_MGT(0x058)
#define PRCM_MCDECLK_MGT	PRCM_CLK_MGT(0x064)
#define PRCM_TVCLK_MGT		PRCM_CLK_MGT(0x07C)


#define PRCM_POWER_STATE_VAL       (_PRCMU_BASE + 0x25C)
#define PRCM_MMIP_LS_CLAMP_SET     (_PRCMU_BASE + 0x420)
#define PRCM_MMIP_LS_CLAMP_CLR     (_PRCMU_BASE + 0x424)
#define PRCM_MMIP_LS_CLAMP_VAL     (_PRCMU_BASE + 0x428)
#define PRCM_YYCLKEN0_MGT_SET      (_PRCMU_BASE + 0x510)
#define PRCM_YYCLKEN0_MGT_VAL      (_PRCMU_BASE + 0x520)
#define PRCM_PLLDSITV_FREQ         (_PRCMU_BASE + 0x500)
#define PRCM_PLLDSITV_ENABLE       (_PRCMU_BASE + 0x504)
#define PRCM_PLLDSITV_LOCKP        (_PRCMU_BASE + 0x508)
#define PRCM_PLLDSILCD_FREQ        (_PRCMU_BASE + 0x290)
#define PRCM_PLLDSILCD_ENABLE      (_PRCMU_BASE + 0x294)
#define PRCM_PLLDSILCD_LOCKP       (_PRCMU_BASE + 0x298)
#define PRCM_DSI_PLLOUT_SEL        (_PRCMU_BASE + 0x530)
#define PRCM_DSITVCLK_DIV          (_PRCMU_BASE + 0x52C)
#define PRCM_APE_RESETN_SET        (_PRCMU_BASE + 0x1E4)
#define PRCM_APE_RESETN_CLR        (_PRCMU_BASE + 0x1E8)
#define PRCM_APE_RESETN_VAL        (_PRCMU_BASE + 0x1EC)

#define PRCMU_CLAMP_DSS			(BIT(21) | BIT(10))
#define PRCMU_CLAMP_DSIPLL		(BIT(22) | BIT(11))
#define PRCMU_RESET_DSS			0x0000000C
#define PRCMU_RESET_DSIPLL		0x00004000
#define PRCMU_ENABLE_DSS_MEM		0x00200000
#define PRCMU_ENABLE_DSS_LOGIC		0x00100000
#define PRCMU_DSS_SLEEP_OUTPUT_MASK	0x400
#define PRCMU_POWER_ON_DSI		0x00008000
#define PRCMU_CLK_PLL_DIV_SHIFT		0
#define PRCMU_CLK_PLL_SW_SHIFT		5
#define PRCMU_CLK_EN			BIT(8)
#define PRCMU_MCDE_CLOCK_SETTING	0x00000125
#define PRCMU_LCDCLKEN			BIT(17)
#define PRCMU_MCDE_CLOCK_ENABLE		0x01000000

/* ePOD and memory power signal control registers */
#define PRCM_EPOD_C_SET            (_PRCMU_BASE + 0x410)
#define PRCM_EPOD_C_VAL            (_PRCMU_BASE + 0x418)
#define PRCM_EPOD_VOK              (_PRCMU_BASE + 0x41C)
#define PRCM_SRAM_LS_SLEEP         (_PRCMU_BASE + 0x304)

/* Debug power control unit registers */
#define PRCM_POWER_STATE_SET       (_PRCMU_BASE + 0x254)

/* Miscellaneous unit registers */
#define PRCM_DSI_SW_RESET		(_PRCMU_BASE + 0x324)
#define PRCM_DSI_GLITCHFREE_EN		(_PRCMU_BASE + 0x534)

/* PRCMU HW semaphore */
#define PRCM_SEM                   (_PRCMU_BASE + 0x400)
#define PRCM_SEM_PRCM_SEM BIT(0)

#define PRCMU_ENABLE_PLLDSI		0x00000001
#define PRCMU_DSI_PLLOUT_SEL_SETTING	0x00000202
#define PRCMU_ENABLE_ESCAPE_CLOCK	0x07030101
#define PRCMU_DSI_RESET_SW		0x00000007
#define PRCMU_MCDE_DELAY		2

#define PRCM_PLLDSI_LOCKP10	BIT(0)
#define PRCM_PLLDSI_LOCKP3	BIT(1)

#define BIT(_x) (1 << (_x))

#define DB8500_PRCM_DSI_SW_RESET_DSI0_SW_RESETN		BIT(0)
#define DB8500_PRCM_DSI_SW_RESET_DSI1_SW_RESETN		BIT(1)
#define DB8500_PRCM_DSI_SW_RESET_DSI2_SW_RESETN		BIT(2)

#define DB8500_PRCM_DSI_GLITCHFREE_EN_DSI0_BYTE_CLK	BIT(0)
#define DB8500_PRCM_DSI_GLITCHFREE_EN_DSI1_BYTE_CLK	BIT(8)
#define DB8500_PRCM_DSI_GLITCHFREE_EN_DSI2_BYTE_CLK	BIT(16)

#define PRCMU_DSS_L_VOK		BIT(20)
#define PRCMU_DSS_M_VOK		BIT(21)

#endif
