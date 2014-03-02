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

#define u32 uint32_t

#include <reg.h>
#include <platform/timer.h>
#include <debug.h>
#include <err.h>

#include "target.h"
#include "machineid.h"
#include "target_config.h"
#include "dbx500-prcmu-regs.h"
#include "mcde_prcmu.h"

#define EIO	-ERR_IO

#define PRCMU_RESET_DSIPLLTV			0x00004000
#define PRCMU_RESET_DSIPLLLCD			0x00008000
#define PRCMU_UNCLAMP_DSIPLL			0x00400800
#define PRCMU_ENABLE_PLLDSI			0x00000001
#define PRCMU_DSI_SW_RESET			0x00000007
#define PRCMU_PLLDSI_LOCKP_LOCKED		0x3

/* Generic settings */

#define PRCMU_PLLDSI_FREQ_SETTING		0x00040165 /* for probe */
#define PRCMU_DPI_CLOCK_SETTING			0x00000030 /* for DPI */
#define PRCMU_DSI_LP_CLOCK_SETTING		0x00000700 /* 38.4M */
#define PRCMU_ENABLE_ESCAPE_CLOCK_DIV		0x07020202 /* TVCLK / 2 */

/* U8500 settings */
#define U8500_PRCMU_HDMI_CLOCK_SETTING		0x0000018C /* TVPLL src */
#define U8500_PRCMU_DSI_PLLOUT_SEL_SETTING	0x00000002 /* PLL / 2 */

/* U9540 settings */
#define U9540_PRCMU_SPARE1_CLOCK_SETTING	0x0000012D /* LCDPLL src */
#define U9540_PRCMU_DSI_PLLOUT_SEL_SETTING	0x0000000A /* LCDPLL / 2 */

static inline bool machine_is_x540(void)
{
	unsigned int mach_id = get_machine_id();

	return (mach_id == MACH_TYPE_U9540 ||
		mach_id == MACH_TYPE_U8540);
}

static inline bool machine_is_8xx0(void)
{
	unsigned int mach_id = get_machine_id();

	return (mach_id == MACH_TYPE_U8500 ||
		mach_id == MACH_TYPE_A9500 ||
		mach_id == MACH_TYPE_U8520 ||
		mach_id == MACH_TYPE_HREFV60);
}

int set_tv_pll(u32 freq)
{
        int i;
        int ret = -EIO;

        /* Clear DSIPLLLCD_RESETN */
        writel(PRCMU_RESET_DSIPLLTV, PRCM_APE_RESETN_CLR);
        /* Unclamp DSIPLL in/out */
        writel(PRCMU_UNCLAMP_DSIPLL, PRCM_MMIP_LS_CLAMP_CLR);
        /* Set DSI PLL FREQ */
        writel(freq, PRCM_PLLDSITV_FREQ);
        /* Start DSI LCD PLL */
        writel(PRCMU_ENABLE_PLLDSI, PRCM_PLLDSITV_ENABLE);
        /* Reset DSI PLL */
        writel(PRCMU_DSI_RESET_SW, PRCM_DSI_SW_RESET);

        for (i = 0; i < 10; i++) {
                if ((readl(PRCM_PLLDSITV_LOCKP) & PRCMU_PLLDSI_LOCKP_LOCKED)
                                        == PRCMU_PLLDSI_LOCKP_LOCKED) {
                        ret = 0;
                        break;
                }
                udelay(100);
        }
        if (ret)
                dprintf(CRITICAL, "prcmu: Unable to lock PLLDSITV in %s %X\n",
                                        __func__, readl(PRCM_PLLDSITV_LOCKP));

        /* Set DSIPLLTV_RESETN */
        writel(PRCMU_RESET_DSIPLLTV, PRCM_APE_RESETN_SET);

        return ret;
}

int set_lcd_pll(u32 freq)
{
        int i;
        int ret = -EIO;

        /* Clear DSIPLLLCD_RESETN */
        writel(PRCMU_RESET_DSIPLLLCD, PRCM_APE_RESETN_CLR);
        /* Unclamp DSIPLL in/out */
        writel(PRCMU_UNCLAMP_DSIPLL, PRCM_MMIP_LS_CLAMP_CLR);
        /* Set DSI PLL FREQ */
        writel(freq, PRCM_PLLDSILCD_FREQ);
        /* Start DSI LCD PLL */
        writel(PRCMU_ENABLE_PLLDSI, PRCM_PLLDSILCD_ENABLE);
        /* Reset DSI PLL */
        writel(PRCMU_DSI_RESET_SW, PRCM_DSI_SW_RESET);

        for (i = 0; i < 10; i++) {
                if ((readl(PRCM_PLLDSILCD_LOCKP) & PRCMU_PLLDSI_LOCKP_LOCKED)
                                        == PRCMU_PLLDSI_LOCKP_LOCKED) {
                        ret = 0;
                        break;
                }
                udelay(100);
        }
        if (ret)
                dprintf(CRITICAL, "prcmu: Unable to lock PLLDSILCD in %s %X\n",
                                        __func__, readl(PRCM_PLLDSILCD_LOCKP));

        /* Set DSIPLLLCD_RESETN */
        writel(PRCMU_RESET_DSIPLLLCD, PRCM_APE_RESETN_SET);

        return ret;
}

int prcmu_enable_dsipll(void)
{
	if (machine_is_x540())
		return set_lcd_pll(PRCMU_PLLDSI_FREQ_SETTING);
	else if (machine_is_8xx0())
		return set_tv_pll(PRCMU_PLLDSI_FREQ_SETTING);

	return -EIO;
}

int clk_set_rate(void *c, u32 r)
{
	u32 freq;
	u32 divsel;

	dprintf(INFO, "%s: f=%d\n", __func__, r);

	if (machine_is_x540()) {
		switch (r) {
		case 332000000:		/* sony vid mode */
			freq = 0x00050164;
			divsel = 2;
			break;
		case 420160000:		/* sony cmd mode */
			freq = 0x00050144;
			divsel = 9;
			break;
		case 570000000:		/* toshiba & himax */
			freq = 0x00040144;
			divsel = 1;
			break;
		default:
			dprintf(CRITICAL, "%s: unsupported clock rate %d\n",
								__func__, r);
			return -EIO;
		}
	} else if (machine_is_8xx0()) {
		switch (r) {
		case 420160000:		/* sony cmd mode */
			freq = PRCMU_PLLDSI_FREQ_SETTING;
			divsel = U8500_PRCMU_DSI_PLLOUT_SEL_SETTING;
			break;
		default:
			dprintf(CRITICAL, "%s: unsupported clock rate %d\n",
								__func__, r);
			return -EIO;
		}
	} else {
		return -EIO;
	}

	writel(divsel, PRCM_DSI_PLLOUT_SEL);
	if (machine_is_x540())
		return set_lcd_pll(freq);
	else if (machine_is_8xx0())
		return set_tv_pll(freq);

	return -EIO;
}

void prcmu_set_display_clocks(void)
{
	writel(PRCMU_DSI_LP_CLOCK_SETTING, PRCM_TVCLK_MGT);
	writel(PRCMU_DPI_CLOCK_SETTING, PRCM_LCDCLK_MGT);
	writel(PRCMU_ENABLE_ESCAPE_CLOCK_DIV, PRCM_DSITVCLK_DIV);
	if (machine_is_x540()) {
		writel(U9540_PRCMU_SPARE1_CLOCK_SETTING, PRCM_SPARE1CLK_MGT);
		writel(U9540_PRCMU_DSI_PLLOUT_SEL_SETTING, PRCM_DSI_PLLOUT_SEL);
	} else if (machine_is_8xx0()) {
		writel(U8500_PRCMU_HDMI_CLOCK_SETTING, PRCM_HDMICLK_MGT);
		writel(U8500_PRCMU_DSI_PLLOUT_SEL_SETTING, PRCM_DSI_PLLOUT_SEL);
	}
}

