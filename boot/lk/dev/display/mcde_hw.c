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

#include <stdlib.h>
#include <string.h>
#include <platform/timer.h>
#include <platform/timer.h>
#include <err.h>
#include <reg.h>
#include "config.h"
#include "target_config.h"
#include "mcde_kcompat.h"
#include "dbx500-prcmu-regs.h"
#include "dbx500-prcmu.h"

#include "mcde_device.h"

#include "mcde.h"
#include "nova_dsilink.h"

#include "mcde_regs.h"

#include "mcde_display.h"

static int set_channel_state_atomic(struct mcde_chnl_state *chnl,
							enum chnl_state state);
static int set_channel_state_sync(struct mcde_chnl_state *chnl,
							enum chnl_state state);
static void stop_channel(struct mcde_chnl_state *chnl);
static int _mcde_chnl_enable(struct mcde_chnl_state *chnl);
static int _mcde_chnl_apply(struct mcde_chnl_state *chnl);
static void disable_flow(struct mcde_chnl_state *chnl);
static void enable_flow(struct mcde_chnl_state *chnl);
static void do_softwaretrig(struct mcde_chnl_state *chnl);
static int probe_hw(struct platform_device *pdev);
static void wait_for_flow_disabled(struct mcde_chnl_state *chnl);

#define SCREEN_PPL_HIGH 1280
#define SCREEN_PPL_CEA2 720
#define SCREEN_LPF_CEA2 480
#define DSI_DELAY0_CEA2_ADD 10
#define MCDE_FIFO_AB_SIZE 640
#define MCDE_PIXFETCH_LARGE_WTRMRKLVL 128
#define MCDE_PIXFETCH_MEDIUM_WTRMRKLVL 32
#define MCDE_PIXFETCH_SMALL_WTRMRKLVL 16

#define DSI_READ_TIMEOUT 200
#define DSI_WRITE_CMD_TIMEOUT 1000
#define DSI_READ_DELAY 5
#define DSI_READ_NBR_OF_RETRIES 2
#define MCDE_FLOWEN_MAX_TRIAL 60
#define REFRESH_RATE_DEFAULT 60
#define PIXFETCH_DSICMDMODE_FACTOR	1
#define PIXFETCH_DSIVIDEOMODE_FACTOR	2
#define PIXFETCH_DPIMODE_FACTOR		2
#define PIXFETCH_WORD_SIZE		64
#define PIXFETCH_BURST_SIZE		512

#define MCDE_VERSION_4_1_3 0x04010300
#define MCDE_VERSION_4_0_4 0x04000400
#define MCDE_VERSION_3_0_8 0x03000800
#define MCDE_VERSION_3_0_5 0x03000500
#define MCDE_VERSION_1_0_4 0x01000400

static u8 *mcdeio;
static struct platform_device *mcde_dev;
static u8 num_channels;
static u8 num_overlays;
static u32 input_fifo_size;
static u32 output_fifo_ab_size;
static u32 output_fifo_c0c1_size;

static void *regulator_vana;
static void *regulator_mcde_epod;
static void *regulator_esram_epod;
static void *clock_mcde;

static u8 mcde_is_enabled;
static u8 dsi_pll_is_enabled;
static u8 dsi_ifc_is_supported;
static u8 dsi_use_clk_framework;
static u32 mcde_clk_rate; /* In Hz */
static u32 pixfetch_need_tot;

/* The lock functions are kept for compatibility with kernel */
static inline void mcde_lock(const char *func, int line)
{
	dev_vdbg(&mcde_dev->dev, "Enter MCDE: %s:%d\n", func, line);
}

static inline void mcde_unlock(const char *func, int line)
{
	dev_vdbg(&mcde_dev->dev, "Exit MCDE: %s:%d\n", func, line);
}

static inline bool mcde_trylock(const char *func, int line)
{
	dev_vdbg(&mcde_dev->dev, "Enter MCDE: %s:%d\n", func, line);
	return 1;
}

static u8 mcde_dynamic_power_management = false;

static inline u32 mcde_rreg(u32 reg)
{
	return readl(mcdeio + reg);
}
static inline void mcde_wreg(u32 reg, u32 val)
{
	writel(val, mcdeio + reg);
}


#define mcde_rfld(__reg, __fld) \
({ \
	const u32 mask = __reg##_##__fld##_MASK; \
	const u32 shift = __reg##_##__fld##_SHIFT; \
	((mcde_rreg(__reg) & mask) >> shift); \
})

#define mcde_wfld(__reg, __fld, __val) \
({ \
	const u32 mask = __reg##_##__fld##_MASK; \
	const u32 shift = __reg##_##__fld##_SHIFT; \
	const u32 oldval = mcde_rreg(__reg); \
	const u32 newval = ((__val) << shift); \
	mcde_wreg(__reg, (oldval & ~mask) | (newval & mask)); \
})

static struct mcde_ovly_state *overlays;

static int prcmu_wait_for(u32 mask, u32 addr, char *name)
{
	u32 temp;

	mdelay(PRCMU_MCDE_DELAY);
	for (temp = 100; temp > 0; temp--) {
		u32 value;
		value = readl(addr) & mask;
		if (value == mask)
			break;
		dev_vdbg(&mcde_dev->dev,
				"%s: %s is NOT set 0x%X\n",
							__func__, name, value);
		mdelay(PRCMU_MCDE_DELAY);
	}
	if (!temp) {
		dev_vdbg(&mcde_dev->dev,
					"%s: %s did NOT set\n",
							__func__, name);
		return -1;
	}
	return 0;
}

int clk_enable(void *c)
{
	/* First clk_enable() will enable all clocks */
	static u32 enabled = 0;
	u32 temp;

	if (enabled) {
		return 0;
	}
	enabled = 1;

	/* clamp dss and dsipll */
	writel(PRCMU_CLAMP_DSS | PRCMU_CLAMP_DSIPLL, PRCM_MMIP_LS_CLAMP_SET);
	mdelay(PRCMU_MCDE_DELAY);

	/* sequence according to the DS */
	/*  */
	writel(PRCMU_ENABLE_DSS_MEM, PRCM_EPOD_C_SET);
	mdelay(PRCMU_MCDE_DELAY);
	writel(PRCMU_ENABLE_DSS_LOGIC, PRCM_EPOD_C_SET);
	mdelay(PRCMU_MCDE_DELAY);

	/* */
	temp = readl(PRCM_SRAM_LS_SLEEP);
	writel(temp & ~PRCMU_DSS_SLEEP_OUTPUT_MASK, PRCM_SRAM_LS_SLEEP);
	mdelay(PRCMU_MCDE_DELAY);

	/* reset state */
	writel(PRCMU_RESET_DSS, PRCM_APE_RESETN_CLR);
	mdelay(PRCMU_MCDE_DELAY);

	/* start clocks */
	writel(PRCMU_MCDE_CLOCK_ENABLE, PRCM_YYCLKEN0_MGT_SET);
	mdelay(PRCMU_MCDE_DELAY);
	prcmu_set_display_clocks();

	/* unclamp outputs */
	writel(PRCMU_CLAMP_DSS, PRCM_MMIP_LS_CLAMP_CLR);
	mdelay(PRCMU_MCDE_DELAY);

	/*  */
	writel(PRCMU_RESET_DSS, PRCM_APE_RESETN_SET);
	mdelay(PRCMU_MCDE_DELAY);

	/*writel(PRCMU_POWER_ON_DSI, PRCM_POWER_STATE_SET);
	mdelay(PRCMU_MCDE_DELAY); */

	mcde_start_dsi();
	/*prcmu_enable_dsipll_lcd();*/
	prcmu_enable_dsipll();

	return 0;
}

static struct mcde_chnl_state *channels;

static void enable_clocks_and_power(struct platform_device *pdev)
{
	/* VANA should be enabled before a DSS hard reset */
	if (regulator_vana)
		WARN_ON_ONCE(regulator_enable(regulator_vana));

	WARN_ON_ONCE(regulator_enable(regulator_mcde_epod));

	WARN_ON_ONCE(clk_enable(clock_mcde));
}

static void disable_clocks_and_power(struct platform_device *pdev)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
}

static void update_mcde_registers(void)
{
	struct mcde_platform_data *pdata = (struct mcde_platform_data *)
						mcde_dev->dev.platform_data;

	/* Setup output muxing */
	mcde_wreg(MCDE_CONF0,
		MCDE_CONF0_IFIFOCTRLWTRMRKLVL(7) |
		MCDE_CONF0_OUTMUX0(pdata->outmux[0]) |
		MCDE_CONF0_OUTMUX1(pdata->outmux[1]) |
		MCDE_CONF0_OUTMUX2(pdata->outmux[2]) |
		MCDE_CONF0_OUTMUX3(pdata->outmux[3]) |
		MCDE_CONF0_OUTMUX4(pdata->outmux[4]) |
		pdata->syncmux);

	mcde_wfld(MCDE_RISPP, VCMPARIS, 1);
	mcde_wfld(MCDE_RISPP, VCMPBRIS, 1);
	mcde_wfld(MCDE_RISPP, VCMPC0RIS, 1);
	mcde_wfld(MCDE_RISPP, VCMPC1RIS, 1);

	/* Enable channel VCMP interrupts */
	mcde_wreg(MCDE_IMSCPP,
		MCDE_IMSCPP_VCMPAIM(false) |
		MCDE_IMSCPP_VCMPBIM(false) |
		MCDE_IMSCPP_VSCC0IM(false) |
		MCDE_IMSCPP_VCMPC0IM(false) |
		MCDE_IMSCPP_VCMPC1IM(false));
}

static void disable_dsi_pll(void)
{
	if (dsi_pll_is_enabled && (--dsi_pll_is_enabled == 0)) {
		struct mcde_platform_data *pdata =
				(struct mcde_platform_data *)
						mcde_dev->dev.platform_data;
		dev_dbg(&mcde_dev->dev, "%s disable dsipll\n", __func__);
		pdata->platform_disable_dsipll();
	}
}

static void enable_dsi_pll(void)
{
	if (!dsi_pll_is_enabled) {
		int ret;
		struct mcde_platform_data *pdata =
				(struct mcde_platform_data *)
						mcde_dev->dev.platform_data;
		ret = pdata->platform_enable_dsipll();
		if (ret < 0) {
			dev_warn(&mcde_dev->dev, "%s: "
				"enable_dsipll failed ret = %d\n",
							__func__, ret);
		}
		dev_dbg(&mcde_dev->dev, "%s enable dsipll\n",
							__func__);
	}
	dsi_pll_is_enabled++;
}

static void disable_mcde_hw(bool force_disable, bool suspend)
{
	int i;
	bool mcde_up = false;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (!mcde_is_enabled)
		return;

	for (i = 0; i < num_channels; i++) {
		struct mcde_chnl_state *chnl = &channels[i];
		if (force_disable || (chnl->enabled &&
					chnl->state != CHNLSTATE_RUNNING)) {
			stop_channel(chnl);
			set_channel_state_sync(chnl, CHNLSTATE_SUSPEND);

			if (chnl->formatter_updated) {
				if (chnl->port.type == MCDE_PORTTYPE_DSI) {
					nova_dsilink_wait_while_running(
								chnl->dsilink);
					nova_dsilink_disable(chnl->dsilink);
					if (!dsi_use_clk_framework)
						disable_dsi_pll();
				} else if (chnl->port.type
							== MCDE_PORTTYPE_DPI) {
					clk_disable(chnl->clk_dpi);
				}
				chnl->formatter_updated = false;
			}
		} else if (chnl->enabled && chnl->state == CHNLSTATE_RUNNING) {
			mcde_up = true;
		}
	}

	if (mcde_up)
		return;

	disable_clocks_and_power(mcde_dev);

	mcde_is_enabled = false;
}

static void dpi_video_mode_apply(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	chnl->tv_regs.interlaced_en = chnl->vmode.interlaced;

	chnl->tv_regs.sel_mode_tv = chnl->port.phy.dpi.tv_mode;
	if (chnl->tv_regs.sel_mode_tv) {
		/* TV mode */
		u32 bel;
		/* -4 since hsw is excluding SAV/EAV, 2 bytes each */
		chnl->tv_regs.hsw  = chnl->vmode.hbp + chnl->vmode.hfp - 4;
		/* vbp_field2 = vbp_field1 + 1 */
		chnl->tv_regs.fsl1 = chnl->vmode.vbp / 2;
		chnl->tv_regs.fsl2 = chnl->vmode.vbp - chnl->tv_regs.fsl1;
		/* +1 since vbp_field2 = vbp_field1 + 1 */
		bel = chnl->vmode.vbp + chnl->vmode.vfp;
		/* in TV mode: bel2 = bel1 + 1 */
		chnl->tv_regs.bel1 = bel / 2;
		chnl->tv_regs.bel2 = bel - chnl->tv_regs.bel1;
		if (chnl->port.phy.dpi.bus_width == 4)
			chnl->tv_regs.tv_mode = MCDE_TVCRA_TVMODE_SDTV_656P_BE;
		else
			chnl->tv_regs.tv_mode = MCDE_TVCRA_TVMODE_SDTV_656P;
		chnl->tv_regs.inv_clk = true;
	} else {
		/* LCD mode */
		u32 polarity;
		chnl->tv_regs.hsw  = chnl->vmode.hsw;
		chnl->tv_regs.dho  = chnl->vmode.hbp;
		chnl->tv_regs.alw  = chnl->vmode.hfp;
		chnl->tv_regs.bel1 = chnl->vmode.vsw;
		chnl->tv_regs.bel2 = chnl->tv_regs.bel1;
		chnl->tv_regs.dvo  = chnl->vmode.vbp;
		chnl->tv_regs.bsl  = chnl->vmode.vfp;
		chnl->tv_regs.fsl1 = 0;
		chnl->tv_regs.fsl2 = 0;
		polarity = chnl->port.phy.dpi.polarity;
		chnl->tv_regs.lcdtim1 = MCDE_LCDTIM1A_IHS(
				(polarity & DPI_ACT_LOW_HSYNC) != 0);
		chnl->tv_regs.lcdtim1 |= MCDE_LCDTIM1A_IVS(
				(polarity & DPI_ACT_LOW_VSYNC) != 0);
		chnl->tv_regs.lcdtim1 |= MCDE_LCDTIM1A_IOE(
				(polarity & DPI_ACT_LOW_DATA_ENABLE) != 0);
		chnl->tv_regs.lcdtim1 |= MCDE_LCDTIM1A_IPC(
				(polarity & DPI_ACT_ON_FALLING_EDGE) != 0);
	}
	chnl->tv_regs.dirty = true;
}

static void update_dpi_registers(enum mcde_chnl chnl_id, struct tv_regs *regs)
{
	u8 idx = chnl_id;

	dev_dbg(&mcde_dev->dev, "%s\n", __func__);
	mcde_wreg(MCDE_TVCRA + idx * MCDE_TVCRA_GROUPOFFSET,
			MCDE_TVCRA_SEL_MOD(regs->sel_mode_tv)             |
			MCDE_TVCRA_INTEREN(regs->interlaced_en)           |
			MCDE_TVCRA_IFIELD(0)                              |
			MCDE_TVCRA_TVMODE(regs->tv_mode)                  |
			MCDE_TVCRA_SDTVMODE(MCDE_TVCRA_SDTVMODE_Y0CBY1CR) |
			MCDE_TVCRA_CKINV(regs->inv_clk)                   |
			MCDE_TVCRA_AVRGEN(0));
	mcde_wreg(MCDE_TVBLUA + idx * MCDE_TVBLUA_GROUPOFFSET,
		MCDE_TVBLUA_TVBLU(MCDE_CONFIG_TVOUT_BACKGROUND_LUMINANCE) |
		MCDE_TVBLUA_TVBCB(MCDE_CONFIG_TVOUT_BACKGROUND_CHROMINANCE_CB)|
		MCDE_TVBLUA_TVBCR(MCDE_CONFIG_TVOUT_BACKGROUND_CHROMINANCE_CR));

	/* Vertical timing registers */
	mcde_wreg(MCDE_TVDVOA + idx * MCDE_TVDVOA_GROUPOFFSET,
					MCDE_TVDVOA_DVO1(regs->dvo) |
					MCDE_TVDVOA_DVO2(regs->dvo));
	mcde_wreg(MCDE_TVBL1A + idx * MCDE_TVBL1A_GROUPOFFSET,
					MCDE_TVBL1A_BEL1(regs->bel1) |
					MCDE_TVBL1A_BSL1(regs->bsl));
	mcde_wreg(MCDE_TVBL2A + idx * MCDE_TVBL1A_GROUPOFFSET,
					MCDE_TVBL2A_BEL2(regs->bel2) |
					MCDE_TVBL2A_BSL2(regs->bsl));
	mcde_wreg(MCDE_TVISLA + idx * MCDE_TVISLA_GROUPOFFSET,
					MCDE_TVISLA_FSL1(regs->fsl1) |
					MCDE_TVISLA_FSL2(regs->fsl2));

	/* Horizontal timing registers */
	mcde_wreg(MCDE_TVLBALWA + idx * MCDE_TVLBALWA_GROUPOFFSET,
				MCDE_TVLBALWA_LBW(regs->hsw) |
				MCDE_TVLBALWA_ALW(regs->alw));
	mcde_wreg(MCDE_TVTIM1A + idx * MCDE_TVTIM1A_GROUPOFFSET,
				MCDE_TVTIM1A_DHO(regs->dho));
	if (!regs->sel_mode_tv)
		mcde_wreg(MCDE_LCDTIM1A + idx * MCDE_LCDTIM1A_GROUPOFFSET,
								regs->lcdtim1);
	regs->dirty = false;
}

static void update_oled_registers(enum mcde_chnl chnl_id,
							struct oled_regs *regs)
{
	u8 idx = chnl_id;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	mcde_wreg(MCDE_OLEDCONV1A + idx * MCDE_OLEDCONV1A_GROUPOFFSET,
				MCDE_OLEDCONV1A_ALPHA_RED(regs->alfa_red) |
				MCDE_OLEDCONV1A_ALPHA_GREEN(regs->alfa_green));
	mcde_wreg(MCDE_OLEDCONV2A + idx * MCDE_OLEDCONV2A_GROUPOFFSET,
				MCDE_OLEDCONV2A_ALPHA_BLUE(regs->alfa_blue) |
				MCDE_OLEDCONV2A_BETA_RED(regs->beta_red));
	mcde_wreg(MCDE_OLEDCONV3A + idx * MCDE_OLEDCONV3A_GROUPOFFSET,
				MCDE_OLEDCONV3A_BETA_GREEN(regs->beta_green) |
				MCDE_OLEDCONV3A_BETA_BLUE(regs->beta_blue));
	mcde_wreg(MCDE_OLEDCONV4A + idx * MCDE_OLEDCONV4A_GROUPOFFSET,
				MCDE_OLEDCONV4A_GAMMA_RED(regs->gamma_red) |
				MCDE_OLEDCONV4A_GAMMA_GREEN(regs->gamma_green));
	mcde_wreg(MCDE_OLEDCONV5A + idx * MCDE_OLEDCONV5A_GROUPOFFSET,
				MCDE_OLEDCONV5A_GAMMA_BLUE(regs->gamma_blue) |
				MCDE_OLEDCONV5A_OFF_RED(regs->off_red));
	mcde_wreg(MCDE_OLEDCONV6A + idx * MCDE_OLEDCONV6A_GROUPOFFSET,
				MCDE_OLEDCONV6A_OFF_GREEN(regs->off_green) |
				MCDE_OLEDCONV6A_OFF_BLUE(regs->off_blue));
	regs->dirty = false;
}


static void update_col_registers(enum mcde_chnl chnl_id, struct col_regs *regs)
{
	u8 idx = chnl_id;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	mcde_wreg(MCDE_RGBCONV1A + idx * MCDE_RGBCONV1A_GROUPOFFSET,
				MCDE_RGBCONV1A_YR_RED(regs->y_red) |
				MCDE_RGBCONV1A_YR_GREEN(regs->y_green));
	mcde_wreg(MCDE_RGBCONV2A + idx * MCDE_RGBCONV2A_GROUPOFFSET,
				MCDE_RGBCONV2A_YR_BLUE(regs->y_blue) |
				MCDE_RGBCONV2A_CR_RED(regs->cr_red));
	mcde_wreg(MCDE_RGBCONV3A + idx * MCDE_RGBCONV3A_GROUPOFFSET,
				MCDE_RGBCONV3A_CR_GREEN(regs->cr_green) |
				MCDE_RGBCONV3A_CR_BLUE(regs->cr_blue));
	mcde_wreg(MCDE_RGBCONV4A + idx * MCDE_RGBCONV4A_GROUPOFFSET,
				MCDE_RGBCONV4A_CB_RED(regs->cb_red) |
				MCDE_RGBCONV4A_CB_GREEN(regs->cb_green));
	mcde_wreg(MCDE_RGBCONV5A + idx * MCDE_RGBCONV5A_GROUPOFFSET,
				MCDE_RGBCONV5A_CB_BLUE(regs->cb_blue) |
				MCDE_RGBCONV5A_OFF_RED(regs->off_cr));
	mcde_wreg(MCDE_RGBCONV6A + idx * MCDE_RGBCONV6A_GROUPOFFSET,
				MCDE_RGBCONV6A_OFF_GREEN(regs->off_y) |
				MCDE_RGBCONV6A_OFF_BLUE(regs->off_cb));
	regs->dirty = false;
}

/* MCDE internal helpers */
static u8 portfmt2dsipacking(enum mcde_port_pix_fmt pix_fmt)
{
	switch (pix_fmt) {
	case MCDE_PORTPIXFMT_DSI_16BPP:
		return MCDE_DSIVID0CONF0_PACKING_RGB565;
	case MCDE_PORTPIXFMT_DSI_18BPP_PACKED:
		return MCDE_DSIVID0CONF0_PACKING_RGB666;
	case MCDE_PORTPIXFMT_DSI_18BPP:
	case MCDE_PORTPIXFMT_DSI_24BPP:
	default:
		return MCDE_DSIVID0CONF0_PACKING_RGB888;
	case MCDE_PORTPIXFMT_DSI_YCBCR422:
		return MCDE_DSIVID0CONF0_PACKING_HDTV;
	}
}

static u8 portfmt2bpp(enum mcde_port_pix_fmt pix_fmt)
{
	/* TODO: Check DPI spec *//* REVIEW: Remove or check */
	switch (pix_fmt) {
	case MCDE_PORTPIXFMT_DPI_16BPP_C1:
	case MCDE_PORTPIXFMT_DPI_16BPP_C2:
	case MCDE_PORTPIXFMT_DPI_16BPP_C3:
	case MCDE_PORTPIXFMT_DSI_16BPP:
	case MCDE_PORTPIXFMT_DSI_YCBCR422:
		return 16;
	case MCDE_PORTPIXFMT_DPI_18BPP_C1:
	case MCDE_PORTPIXFMT_DPI_18BPP_C2:
	case MCDE_PORTPIXFMT_DSI_18BPP_PACKED:
		return 18;
	case MCDE_PORTPIXFMT_DSI_18BPP:
	case MCDE_PORTPIXFMT_DPI_24BPP:
	case MCDE_PORTPIXFMT_DSI_24BPP:
		return 24;
	default:
		return 1;
	}
}

static u8 bpp2outbpp(u8 bpp)
{
	switch (bpp) {
	case 16:
		return MCDE_CRA1_OUTBPP_16BPP;
	case 18:
		return MCDE_CRA1_OUTBPP_18BPP;
	case 24:
		return MCDE_CRA1_OUTBPP_24BPP;
	default:
		return 0;
	}
}

static u8 portfmt2cdwin(enum mcde_port_pix_fmt pix_fmt)
{
	switch (pix_fmt) {
	case MCDE_PORTPIXFMT_DPI_16BPP_C1:
		return MCDE_CRA1_CDWIN_16BPP_C1;
	case MCDE_PORTPIXFMT_DPI_16BPP_C2:
		return MCDE_CRA1_CDWIN_16BPP_C2;
	case MCDE_PORTPIXFMT_DPI_16BPP_C3:
		return MCDE_CRA1_CDWIN_16BPP_C3;
	case MCDE_PORTPIXFMT_DPI_18BPP_C1:
		return MCDE_CRA1_CDWIN_18BPP_C1;
	case MCDE_PORTPIXFMT_DPI_18BPP_C2:
		return MCDE_CRA1_CDWIN_18BPP_C2;
	case MCDE_PORTPIXFMT_DPI_24BPP:
		return MCDE_CRA1_CDWIN_24BPP;
	default:
		/* only DPI formats are relevant */
		return 0;
	}
}

static u32 get_output_fifo_size(enum mcde_fifo fifo)
{
	u32 ret = 1; /* Avoid div by zero */

	switch (fifo) {
	case MCDE_FIFO_A:
	case MCDE_FIFO_B:
		ret = output_fifo_ab_size;
		break;
	case MCDE_FIFO_C0:
	case MCDE_FIFO_C1:
		ret = output_fifo_c0c1_size;
		break;
	default:
		dev_warn(&mcde_dev->dev, "Unsupported fifo");
		break;
	}
	return ret;
}

static inline u8 get_dsi_formatter_id(const struct mcde_port *port)
{
	int ret;

	if (dsi_ifc_is_supported) {
		ret = 2 * port->link;
	}
	else {
		ret = port->link;
	}
	return ret;
}

char *channel_state_name(enum chnl_state state)
{
	switch (state) {
	case CHNLSTATE_SUSPEND:
		return "SUSPEND";
	case CHNLSTATE_IDLE:
		return "IDLE";
	case CHNLSTATE_DSI_READ:
		return "DSI_READ";
	case CHNLSTATE_DSI_WRITE:
		return "DSI_WRITE";
	case CHNLSTATE_SETUP:
		return "SETUP";
	case CHNLSTATE_WAIT_TE:
		return "WAIT_TE";
	case CHNLSTATE_RUNNING:
		return "RUNNING";
	case CHNLSTATE_STOPPING:
		return "STOPPING";
	case CHNLSTATE_STOPPED:
		return "STOPPED";
	}
	return "unknown";
}

/* Transitions allowed: WAIT_TE -> UPDATE -> STOPPING */
static int set_channel_state_atomic(struct mcde_chnl_state *chnl,
							enum chnl_state state)
{
	enum chnl_state chnl_state = chnl->state;

	dev_dbg(&mcde_dev->dev, "Channel state change"
		" (chnl=%d, old=%s, new=%s)\n", chnl->id,
		channel_state_name(chnl_state),
		channel_state_name(state));

	if ((chnl_state == CHNLSTATE_SETUP && state == CHNLSTATE_WAIT_TE) ||
	    (chnl_state == CHNLSTATE_SETUP && state == CHNLSTATE_RUNNING) ||
	    (chnl_state == CHNLSTATE_WAIT_TE && state == CHNLSTATE_RUNNING) ||
	    (chnl_state == CHNLSTATE_RUNNING && state == CHNLSTATE_STOPPING)) {
		/* Set wait TE, running, or stopping state */
		chnl->state = state;
		return 0;
	} else if ((chnl_state == CHNLSTATE_STOPPING &&
						state == CHNLSTATE_STOPPED) ||
		   (chnl_state == CHNLSTATE_WAIT_TE &&
						state == CHNLSTATE_STOPPED)) {
		/* Set stopped state */
		chnl->state = state;
		return 0;
	} else if (state == CHNLSTATE_IDLE) {
		/* Set idle state */
		WARN_ON_ONCE(chnl_state != CHNLSTATE_DSI_READ &&
			     chnl_state != CHNLSTATE_DSI_WRITE &&
			     chnl_state != CHNLSTATE_SUSPEND);
		chnl->state = state;
		return 0;
	} else {
		/* Invalid atomic state transition */
		dev_warn(&mcde_dev->dev, "Channel state change error (chnl=%d,"
			" old=%d, new=%d)\n", chnl->id, chnl_state, state);
		WARN_ON_ONCE(true);
		return -EINVAL;
	}
}

/* LOCKING: mcde_hw_lock */
static int set_channel_state_sync(struct mcde_chnl_state *chnl,
							enum chnl_state state)
{
	int ret = 0;
	enum chnl_state chnl_state = chnl->state;

	dev_dbg(&mcde_dev->dev, "Channel state change"
		" (chnl=%d, old=%s, new=%s)\n", chnl->id,
		channel_state_name(chnl->state),
		channel_state_name(state));

	/* No change */
	if (chnl_state == state)
		return 0;
	/* Do manual transition from STOPPED to IDLE */
	if (chnl_state == CHNLSTATE_STOPPED)
		wait_for_flow_disabled(chnl);

	/* State is IDLE, do transition to new state */
	chnl->state = state;

	return ret;
}

/* for ease of maintenance and compatability with kernel code */
static int wait_for_vcmp(struct mcde_chnl_state *chnl)
{
	return 1;
}

static void get_vid_operating_mode(const struct mcde_port *port,
		bool *burst_mode, bool *sync_is_pulse, bool *tvg_enable)
{
	switch (port->phy.dsi.vid_mode) {
	case NON_BURST_MODE_WITH_SYNC_EVENT:
		*burst_mode = false;
		*sync_is_pulse = false;
		*tvg_enable = false;
		break;
	case NON_BURST_MODE_WITH_SYNC_EVENT_TVG_ENABLED:
		*burst_mode = false;
		*sync_is_pulse = false;
		*tvg_enable = true;
		break;
	case BURST_MODE_WITH_SYNC_EVENT:
		*burst_mode = true;
		*sync_is_pulse = false;
		*tvg_enable = false;
		break;
	case BURST_MODE_WITH_SYNC_PULSE:
		*burst_mode = true;
		*sync_is_pulse = true;
		*tvg_enable = false;
		break;
	default:
		dev_err(&mcde_dev->dev, "Unsupported video mode");
		break;
	}
}

static int update_channel_static_registers(struct mcde_chnl_state *chnl)
{
	const struct mcde_port *port = &chnl->port;
	struct dsilink_port dsi_port;

	switch (chnl->fifo) {
	case MCDE_FIFO_A:
		mcde_wreg(MCDE_CHNL0MUXING + chnl->id *
			MCDE_CHNL0MUXING_GROUPOFFSET,
			MCDE_CHNL0MUXING_FIFO_ID_ENUM(FIFO_A));
		if (port->type == MCDE_PORTTYPE_DPI) {
			mcde_wfld(MCDE_CTRLA, FORMTYPE,
					MCDE_CTRLA_FORMTYPE_DPITV);
			mcde_wfld(MCDE_CTRLA, FORMID, port->link);
		} else if (port->type == MCDE_PORTTYPE_DSI) {
			mcde_wfld(MCDE_CTRLA, FORMTYPE,
					MCDE_CTRLA_FORMTYPE_DSI);
			mcde_wfld(MCDE_CTRLA, FORMID,
						get_dsi_formatter_id(port));
		}
		break;
	case MCDE_FIFO_B:
		mcde_wreg(MCDE_CHNL0MUXING + chnl->id *
			MCDE_CHNL0MUXING_GROUPOFFSET,
			MCDE_CHNL0MUXING_FIFO_ID_ENUM(FIFO_B));
		if (port->type == MCDE_PORTTYPE_DPI) {
			mcde_wfld(MCDE_CTRLB, FORMTYPE,
					MCDE_CTRLB_FORMTYPE_DPITV);
			mcde_wfld(MCDE_CTRLB, FORMID, port->link);
		} else if (port->type == MCDE_PORTTYPE_DSI) {
			mcde_wfld(MCDE_CTRLB, FORMTYPE,
					MCDE_CTRLB_FORMTYPE_DSI);
			mcde_wfld(MCDE_CTRLB, FORMID,
						get_dsi_formatter_id(port));
		}

		break;
	case MCDE_FIFO_C0:
		mcde_wreg(MCDE_CHNL0MUXING + chnl->id *
			MCDE_CHNL0MUXING_GROUPOFFSET,
			MCDE_CHNL0MUXING_FIFO_ID_ENUM(FIFO_C0));
		if (port->type == MCDE_PORTTYPE_DPI)
			return -EINVAL;
		mcde_wfld(MCDE_CTRLC0, FORMTYPE,
					MCDE_CTRLC0_FORMTYPE_DSI);
		mcde_wfld(MCDE_CTRLC0, FORMID, get_dsi_formatter_id(port));
		break;
	case MCDE_FIFO_C1:
		mcde_wreg(MCDE_CHNL0MUXING + chnl->id *
			MCDE_CHNL0MUXING_GROUPOFFSET,
			MCDE_CHNL0MUXING_FIFO_ID_ENUM(FIFO_C1));
		if (port->type == MCDE_PORTTYPE_DPI)
			return -EINVAL;
		mcde_wfld(MCDE_CTRLC1, FORMTYPE,
					MCDE_CTRLC1_FORMTYPE_DSI);
		mcde_wfld(MCDE_CTRLC1, FORMID, get_dsi_formatter_id(port));
		break;
	default:
		return -EINVAL;
	}

	/* Formatter */
	if (port->type == MCDE_PORTTYPE_DSI) {
		u8 idx;
		idx = get_dsi_formatter_id(port);

		dsi_port.mode = port->mode;
		switch (port->sync_src) {
		case MCDE_SYNCSRC_TE_POLLING:
			dsi_port.sync_src = DSILINK_SYNCSRC_TE_POLLING;
			break;
		case MCDE_SYNCSRC_BTA:
			/*Intentional */
		default:
			dsi_port.sync_src = DSILINK_SYNCSRC_BTA;
			break;
		}

		dsi_port.link = port->link;
		dsi_port.refresh_rate = port->refresh_rate;
		dsi_port.phy = port->phy.dsi;

		(void)nova_dsilink_setup(chnl->dsilink, &dsi_port);

		if (!dsi_use_clk_framework)
			enable_dsi_pll();

		if (nova_dsilink_enable(chnl->dsilink))
			goto dsi_link_error;

		nova_dsilink_exit_ulpm(chnl->dsilink);

		mcde_wreg(MCDE_DSIVID0CONF0 +
			idx * MCDE_DSIVID0CONF0_GROUPOFFSET,
			MCDE_DSIVID0CONF0_BLANKING(0) |
			MCDE_DSIVID0CONF0_VID_MODE(
				port->mode == MCDE_PORTMODE_VID) |
			MCDE_DSIVID0CONF0_CMD8(true) |
			MCDE_DSIVID0CONF0_BIT_SWAP(false) |
			MCDE_DSIVID0CONF0_BYTE_SWAP(false) |
			MCDE_DSIVID0CONF0_DCSVID_NOTGEN(true));
	}

	if (port->type == MCDE_PORTTYPE_DPI) {
/* TODO: DPI support
		if (port->phy.dpi.lcd_freq != clk_round_rate(chnl->clk_dpi,
							port->phy.dpi.lcd_freq))
			dev_warn(&mcde_dev->dev, "Could not set lcd freq"
					" to %d\n", port->phy.dpi.lcd_freq);
*/
		WARN_ON_ONCE(clk_set_rate(chnl->clk_dpi,
						port->phy.dpi.lcd_freq));
		WARN_ON_ONCE(clk_enable(chnl->clk_dpi));
	}
	mcde_wfld(MCDE_CR, MCDEEN, true);
	chnl->formatter_updated = true;

	dev_vdbg(&mcde_dev->dev, "Static registers setup, chnl=%d\n", chnl->id);

	return 0;
dsi_link_error:
	return -EINVAL;
}

static void mcde_chnl_oled_convert_apply(struct mcde_chnl_state *chnl,
					struct mcde_oled_transform *transform)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (chnl->oled_transform != transform) {
		chnl->oled_regs.alfa_red     = transform->matrix[0][0];
		chnl->oled_regs.alfa_green   = transform->matrix[0][1];
		chnl->oled_regs.alfa_blue    = transform->matrix[0][2];
		chnl->oled_regs.beta_red     = transform->matrix[1][0];
		chnl->oled_regs.beta_green   = transform->matrix[1][1];
		chnl->oled_regs.beta_blue    = transform->matrix[1][2];
		chnl->oled_regs.gamma_red    = transform->matrix[2][0];
		chnl->oled_regs.gamma_green  = transform->matrix[2][1];
		chnl->oled_regs.gamma_blue   = transform->matrix[2][2];
		chnl->oled_regs.off_red      = transform->offset[0];
		chnl->oled_regs.off_green    = transform->offset[1];
		chnl->oled_regs.off_blue     = transform->offset[2];
		chnl->oled_regs.dirty = true;

		chnl->oled_transform = transform;
	}

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

void mcde_chnl_col_convert_apply(struct mcde_chnl_state *chnl,
					struct mcde_col_transform *transform)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (chnl->transform != transform) {

		chnl->col_regs.y_red     = transform->matrix[0][0];
		chnl->col_regs.y_green   = transform->matrix[0][1];
		chnl->col_regs.y_blue    = transform->matrix[0][2];
		chnl->col_regs.cb_red    = transform->matrix[1][0];
		chnl->col_regs.cb_green  = transform->matrix[1][1];
		chnl->col_regs.cb_blue   = transform->matrix[1][2];
		chnl->col_regs.cr_red    = transform->matrix[2][0];
		chnl->col_regs.cr_green  = transform->matrix[2][1];
		chnl->col_regs.cr_blue   = transform->matrix[2][2];
		chnl->col_regs.off_y     = transform->offset[0];
		chnl->col_regs.off_cb    = transform->offset[1];
		chnl->col_regs.off_cr    = transform->offset[2];
		chnl->col_regs.dirty = true;

		chnl->transform = transform;
	}

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

static void setup_channel_and_overlay_color_conv(struct mcde_chnl_state *chnl,
						struct mcde_ovly_state *ovly)
{
	struct ovly_regs *regs = &ovly->regs;

	static struct mcde_oled_transform yuv240_2_rgb = {
		/* Note that in MCDE YUV 422 pixels come as VYU pixels */
		.matrix = {
			{0x1990, 0x12A0, 0x0000},
			{0x2D00, 0x12A0, 0x2640},
			{0x0000, 0x12A0, 0x1FFF},
		},
		.offset = {0x2DF0, 0x0870, 0x3150},
	};
	static struct mcde_col_transform rgb_2_yuv240 = {
		/* Note that in MCDE YUV 422 pixels come as VYU pixels */
		.matrix = {
			{0x0042, 0x0081, 0x0019},
			{0xffda, 0xffb5, 0x0071},
			{0x0070, 0xffa2, 0xffee},
		},
		.offset = {0x0010, 0x0080, 0x0080},
	};
	struct mcde_ovly_state *ovly0 = chnl->ovly0;
	struct mcde_ovly_state *ovly1 = chnl->ovly1;

	if (chnl->port.update_auto_trig &&
				chnl->port.type == MCDE_PORTTYPE_DSI) {
		if (ovly->pix_fmt == MCDE_OVLYPIXFMT_YCbCr422) {
			regs->col_conv = MCDE_OVL0CR_COLCCTRL_ENABLED_SAT;
			mcde_chnl_col_convert_apply(chnl, &chnl->ycbcr_2_rgb);
		}
		return;
	}

	if (regs->enabled && ovly->pix_fmt == MCDE_OVLYPIXFMT_YCbCr422) {
		chnl->oled_color_conversion = true;
		mcde_chnl_oled_convert_apply(chnl, &yuv240_2_rgb);
		mcde_chnl_col_convert_apply(chnl, &rgb_2_yuv240);
		regs->col_conv = MCDE_OVL0CR_COLCCTRL_DISABLED;
		regs->dirty = true;
		if (ovly0->pix_fmt == MCDE_OVLYPIXFMT_YCbCr422) {
			ovly1->regs.col_conv = MCDE_OVL0CR_COLCCTRL_ENABLED_SAT;
			ovly1->regs.dirty = true;
		} else {
			ovly0->regs.col_conv = MCDE_OVL0CR_COLCCTRL_ENABLED_SAT;
			ovly0->regs.dirty = true;
		}
		chnl->regs.oled_enable = true;
		chnl->regs.background_enable = true;
		chnl->regs.dirty = true;
	} else if (chnl->oled_color_conversion) {
		/* Turn off if no overlay needs YUV conv */
		if (ovly0->pix_fmt != MCDE_OVLYPIXFMT_YCbCr422 &&
				ovly1->pix_fmt != MCDE_OVLYPIXFMT_YCbCr422)
			chnl->oled_color_conversion = false;
		if (!chnl->oled_color_conversion) {
			ovly0->regs.col_conv = MCDE_OVL0CR_COLCCTRL_DISABLED;
			ovly1->regs.col_conv = MCDE_OVL0CR_COLCCTRL_DISABLED;
			ovly0->regs.dirty = true;
			ovly1->regs.dirty = true;
			chnl->regs.background_enable = false;
			chnl->regs.oled_enable = false;
			chnl->regs.dirty = true;
		}
	}
}

static void chnl_ovly_pixel_format_apply(struct mcde_chnl_state *chnl,
						struct mcde_ovly_state *ovly)
{
	struct mcde_port *port = &chnl->port;
	struct ovly_regs *regs = &ovly->regs;

	/* Note: YUV -> YUV: blending YUV overlays will not make sense. */
	static struct mcde_col_transform crycb_2_ycbcr = {
		/* Note that in MCDE YUV 422 pixels come as VYU pixels */
		.matrix = {
			{0x0000, 0x0100, 0x0000},
			{0x0000, 0x0000, 0x0100},
			{0x0100, 0x0000, 0x0000},
		},
		.offset = {0, 0, 0},
	};


	if (port->type == MCDE_PORTTYPE_DPI && port->phy.dpi.tv_mode) {
		regs->col_conv = MCDE_OVL0CR_COLCCTRL_ENABLED_NO_SAT;
		if (ovly->pix_fmt != MCDE_OVLYPIXFMT_YCbCr422)
			mcde_chnl_col_convert_apply(chnl, &chnl->rgb_2_ycbcr);
		else
			mcde_chnl_col_convert_apply(chnl, &crycb_2_ycbcr);
	} else {
		if (port->pixel_format != MCDE_PORTPIXFMT_DSI_YCBCR422) {
			setup_channel_and_overlay_color_conv(chnl, ovly);
		} else {
			if (ovly->pix_fmt != MCDE_OVLYPIXFMT_YCbCr422)
				/* DSI: RGB -> YUV */
				mcde_chnl_col_convert_apply(chnl,
							&chnl->rgb_2_ycbcr);
			else
				/* DSI: YUV -> YUV */
				mcde_chnl_col_convert_apply(chnl,
							&crycb_2_ycbcr);
			regs->col_conv = MCDE_OVL0CR_COLCCTRL_ENABLED_NO_SAT;
		}
	}
}

/* REVIEW: Make update_* an mcde_rectangle? */
static void update_overlay_registers(u8 idx, struct ovly_regs *regs,
			struct mcde_port *port, enum mcde_fifo fifo,
			u16 update_x, u16 update_y, u16 update_w,
			u16 update_h, s16 stride, bool interlaced,
			enum mcde_display_rotation rotation)
{
	/* TODO: fix clipping for small overlay */
	u32 lmrgn = (regs->cropx + update_x) * regs->bits_per_pixel;
	u32 tmrgn = (regs->cropy + update_y) * stride;
	u32 ppl = regs->ppl - update_x;
	u32 lpf = regs->lpf - update_y;
	s32 ljinc = stride;
	u32 pixelfetchwtrmrklevel;
	u8  nr_of_bufs = 1;
	u32 sel_mod = MCDE_EXTSRC0CR_SEL_MOD_SOFTWARE_SEL;
	struct mcde_platform_data *pdata = (struct mcde_platform_data *)
						mcde_dev->dev.platform_data;

	if (rotation == MCDE_DISPLAY_ROT_180_CCW) {
		ljinc = -ljinc;
		tmrgn += stride * (regs->lpf - 1) / 8;
	}

	/*
	 * Preferably most of this is done in some apply function instead of for
	 * every update. However lpf has a dependency on update_y.
	 */
	if (interlaced && port->type == MCDE_PORTTYPE_DSI) {
		nr_of_bufs = 2;
		lpf = lpf / 2;
		ljinc *= 2;
	}

	pixelfetchwtrmrklevel = pdata->pixelfetchwtrmrk[idx];
	if (pixelfetchwtrmrklevel == 0) {
		/* Not set: Use default value */
		switch (idx) {
		case 0:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL0;
			break;
		case 1:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL1;
			break;
		case 2:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL2;
			break;
		case 3:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL3;
			break;
		case 4:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL4;
			break;
		case 5:
			pixelfetchwtrmrklevel = MCDE_PIXFETCH_WTRMRKLVL_OVL5;
			break;
		}
	}
	if (regs->enabled)
		dev_dbg(&mcde_dev->dev, "ovly%d pfwml:%d %dbpp\n", idx,
				pixelfetchwtrmrklevel, regs->bits_per_pixel);

	if (port->update_auto_trig && port->type == MCDE_PORTTYPE_DSI) {
		switch (port->sync_src) {
		case MCDE_SYNCSRC_OFF:
			sel_mod = MCDE_EXTSRC0CR_SEL_MOD_SOFTWARE_SEL;
			break;
		case MCDE_SYNCSRC_TE0:
		case MCDE_SYNCSRC_TE1:
		case MCDE_SYNCSRC_TE_POLLING:
		default:
			sel_mod = MCDE_EXTSRC0CR_SEL_MOD_AUTO_TOGGLE;
			break;
		}
	} else if (port->type == MCDE_PORTTYPE_DPI)
		sel_mod = MCDE_EXTSRC0CR_SEL_MOD_SOFTWARE_SEL;

	mcde_wreg(MCDE_EXTSRC0CONF + idx * MCDE_EXTSRC0CONF_GROUPOFFSET,
		MCDE_EXTSRC0CONF_BUF_ID(0) |
		MCDE_EXTSRC0CONF_BUF_NB(nr_of_bufs) |
		MCDE_EXTSRC0CONF_PRI_OVLID(idx) |
		MCDE_EXTSRC0CONF_BPP(regs->bpp) |
		MCDE_EXTSRC0CONF_BGR(regs->bgr) |
		MCDE_EXTSRC0CONF_BEBO(regs->bebo) |
		MCDE_EXTSRC0CONF_BEPO(false));
	mcde_wreg(MCDE_EXTSRC0CR + idx * MCDE_EXTSRC0CR_GROUPOFFSET,
		MCDE_EXTSRC0CR_SEL_MOD(sel_mod) |
		MCDE_EXTSRC0CR_MULTIOVL_CTRL_ENUM(PRIMARY) |
		MCDE_EXTSRC0CR_FS_DIV_DISABLE(false) |
		MCDE_EXTSRC0CR_FORCE_FS_DIV(false));
	mcde_wreg(MCDE_OVL0CR + idx * MCDE_OVL0CR_GROUPOFFSET,
		MCDE_OVL0CR_OVLEN(regs->enabled) |
		MCDE_OVL0CR_COLCCTRL(regs->col_conv) |
		MCDE_OVL0CR_CKEYGEN(false) |
		MCDE_OVL0CR_ALPHAPMEN(false) |
		MCDE_OVL0CR_OVLF(false) |
		MCDE_OVL0CR_OVLR(false) |
		MCDE_OVL0CR_OVLB(false) |
		MCDE_OVL0CR_FETCH_ROPC(0) |
		MCDE_OVL0CR_STBPRIO(0) |
		MCDE_OVL0CR_BURSTSIZE_ENUM(HW_8W) |
		/* TODO: enum, get from ovly */
		MCDE_OVL0CR_MAXOUTSTANDING_ENUM(8_REQ) |
		/* TODO: _HW_8W, calculate? */
		MCDE_OVL0CR_ROTBURSTSIZE_ENUM(HW_8W));
	mcde_wreg(MCDE_OVL0CONF + idx * MCDE_OVL0CONF_GROUPOFFSET,
		MCDE_OVL0CONF_PPL(ppl) |
		MCDE_OVL0CONF_EXTSRC_ID(idx) |
		MCDE_OVL0CONF_LPF(lpf));
	mcde_wreg(MCDE_OVL0CONF2 + idx * MCDE_OVL0CONF2_GROUPOFFSET,
		MCDE_OVL0CONF2_BP(regs->alpha_source) |
		MCDE_OVL0CONF2_ALPHAVALUE(regs->alpha_value) |
		MCDE_OVL0CONF2_OPQ(regs->opq) |
		MCDE_OVL0CONF2_PIXOFF(lmrgn & 63) |
		MCDE_OVL0CONF2_PIXELFETCHERWATERMARKLEVEL(
			pixelfetchwtrmrklevel));
	mcde_wreg(MCDE_OVL0LJINC + idx * MCDE_OVL0LJINC_GROUPOFFSET,
		ljinc);
	mcde_wreg(MCDE_OVL0CROP + idx * MCDE_OVL0CROP_GROUPOFFSET,
		MCDE_OVL0CROP_TMRGN(tmrgn) |
		MCDE_OVL0CROP_LMRGN(lmrgn >> 6));
	regs->dirty = false;

	dev_vdbg(&mcde_dev->dev, "Overlay registers setup, idx=%d\n", idx);
}

static void update_overlay_registers_on_the_fly(u8 idx, struct ovly_regs *regs)
{
	mcde_wreg(MCDE_OVL0COMP + idx * MCDE_OVL0COMP_GROUPOFFSET,
		MCDE_OVL0COMP_XPOS(regs->xpos) |
		MCDE_OVL0COMP_CH_ID(regs->ch_id) |
		MCDE_OVL0COMP_YPOS(regs->ypos) |
		MCDE_OVL0COMP_Z(regs->z));

	mcde_wreg(MCDE_EXTSRC0A0 + idx * MCDE_EXTSRC0A0_GROUPOFFSET,
		regs->baseaddress0);
	mcde_wreg(MCDE_EXTSRC0A1 + idx * MCDE_EXTSRC0A1_GROUPOFFSET,
		regs->baseaddress1);
	regs->dirty_buf = false;
}

static void do_softwaretrig(struct mcde_chnl_state *chnl)
{
	unsigned long flags = 0;

	local_irq_save(flags);

	enable_flow(chnl);
	mcde_wreg(MCDE_CHNL0SYNCHSW +
		chnl->id * MCDE_CHNL0SYNCHSW_GROUPOFFSET,
		MCDE_CHNL0SYNCHSW_SW_TRIG(true));
	disable_flow(chnl);

	local_irq_restore(flags);

dev_err(&mcde_dev->dev, "Software TRIG on channel %d\n", chnl->id);
	dev_vdbg(&mcde_dev->dev, "Software TRIG on channel %d\n", chnl->id);
}

static void disable_flow(struct mcde_chnl_state *chnl)
{
	unsigned long flags;

	if (WARN_ON_ONCE(chnl->state != CHNLSTATE_RUNNING))
		return;

	local_irq_save(flags);

	switch (chnl->id) {
	case MCDE_CHNL_A:
		mcde_wfld(MCDE_CRA0, FLOEN, false);
		break;
	case MCDE_CHNL_B:
		mcde_wfld(MCDE_CRB0, FLOEN, false);
		break;
	case MCDE_CHNL_C0:
		mcde_wfld(MCDE_CRC, C1EN, false);
		break;
	case MCDE_CHNL_C1:
		mcde_wfld(MCDE_CRC, C2EN, false);
		break;
	}

	set_channel_state_atomic(chnl, CHNLSTATE_STOPPING);

	local_irq_restore(flags);
}

static void stop_channel(struct mcde_chnl_state *chnl)
{
	const struct mcde_port *port = &chnl->port;
	bool dpi_lcd_mode;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (chnl->state != CHNLSTATE_RUNNING)
		return;

	/*
	 * clk_cont has to be false before the FLOEN is false for the channel
	 */
	if (port->type == MCDE_PORTTYPE_DSI)
		nova_dsilink_set_clk_continous(chnl->dsilink, false);

	disable_flow(chnl);
	/*
	 * Needs to manually trigger VCOMP after the channel is
	 * disabled. For all channels using video mode
	 * except for dpi lcd.
	*/
	dpi_lcd_mode = (port->type == MCDE_PORTTYPE_DPI &&
				!chnl->port.phy.dpi.tv_mode);

	if (chnl->port.update_auto_trig && !dpi_lcd_mode)
		mcde_wreg(MCDE_SISPP, 1 << chnl->id);
}

static void wait_for_flow_disabled(struct mcde_chnl_state *chnl)
{
	int i = 0;

	switch (chnl->id) {
	case MCDE_CHNL_A:
		for (i = 0; i < MCDE_FLOWEN_MAX_TRIAL; i++) {
			if (!mcde_rfld(MCDE_CRA0, FLOEN)) {
				dev_vdbg(&mcde_dev->dev,
					"Flow (A) disable after >= %d ms\n", i);
				break;
			}
			usleep_range(1000, 1500);
		}
		break;
	case MCDE_CHNL_B:
		for (i = 0; i < MCDE_FLOWEN_MAX_TRIAL; i++) {
			if (!mcde_rfld(MCDE_CRB0, FLOEN)) {
				dev_vdbg(&mcde_dev->dev,
				"Flow (B) disable after >= %d ms\n", i);
				break;
			}
			usleep_range(1000, 1500);
		}
		break;
	case MCDE_CHNL_C0:
		for (i = 0; i < MCDE_FLOWEN_MAX_TRIAL; i++) {
			if (!mcde_rfld(MCDE_CRC, C1EN)) {
				dev_vdbg(&mcde_dev->dev,
				"Flow (C1) disable after >= %d ms\n", i);
				break;
			}
			usleep_range(1000, 1500);
		}
		break;
	case MCDE_CHNL_C1:
		for (i = 0; i < MCDE_FLOWEN_MAX_TRIAL; i++) {
			if (!mcde_rfld(MCDE_CRC, C2EN)) {
				dev_vdbg(&mcde_dev->dev,
				"Flow (C2) disable after >= %d ms\n", i);
				break;
			}
			usleep_range(1000, 1500);
		}
		break;
	}
	if (i == MCDE_FLOWEN_MAX_TRIAL)
		dev_err(&mcde_dev->dev, "%s: channel %d timeout\n",
							__func__, chnl->id);
}

static void enable_flow(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (chnl->port.type == MCDE_PORTTYPE_DSI)
		nova_dsilink_set_clk_continous(chnl->dsilink, true);

	/*
	 * When ROTEN is set, the FLOEN bit will also be set but
	 * the flow has to be started anyway.
	 */
	switch (chnl->id) {
	case MCDE_CHNL_A:
		WARN_ON_ONCE(mcde_rfld(MCDE_CRA0, FLOEN));
		mcde_wfld(MCDE_CRA0, ROTEN, chnl->regs.roten);
		mcde_wfld(MCDE_CRA0, FLOEN, true);
		break;
	case MCDE_CHNL_B:
		WARN_ON_ONCE(mcde_rfld(MCDE_CRB0, FLOEN));
		mcde_wfld(MCDE_CRB0, ROTEN, chnl->regs.roten);
		mcde_wfld(MCDE_CRB0, FLOEN, true);
		break;
	case MCDE_CHNL_C0:
		WARN_ON_ONCE(mcde_rfld(MCDE_CRC, C1EN));
		mcde_wfld(MCDE_CRC, C1EN, true);
		break;
	case MCDE_CHNL_C1:
		WARN_ON_ONCE(mcde_rfld(MCDE_CRC, C2EN));
		mcde_wfld(MCDE_CRC, C2EN, true);
		break;
	}

	set_channel_state_atomic(chnl, CHNLSTATE_RUNNING);
}

static u32 get_pkt_div(u32 disp_ppl,
		struct mcde_port *port,
		enum mcde_fifo fifo)
{
	/*
	 * The lines can be split in several packets only on DSI CMD mode.
	 * In DSI VIDEO mode, 1 line = 1 packet.
	 * DPI is like DSI VIDEO (watermark = 1 line).
	 * DPI waits for fifo ready only for the first line of the first frame.
	 * If line is wider than fifo size, one can set watermark
	 * at fifo size, or set it to line size as watermark will be
	 * saturated at fifo size inside MCDE.
	 */
	switch (port->type) {
	case MCDE_PORTTYPE_DSI:
		if (port->mode == MCDE_PORTMODE_CMD)
			/* Equivalent of ceil(disp_ppl/fifo_size) */
			return (disp_ppl - 1) / get_output_fifo_size(fifo) + 1;
		else
			return 1;
		break;
	case MCDE_PORTTYPE_DPI:
		return 1;
		break;
	default:
		break;
	}
	return 1;
}



static void update_vid_frame_parameters(struct mcde_chnl_state *chnl,
				struct mcde_video_mode *vmode, u8 bpp)
{
	u8 pixel_mode;
	u8 rgb_header;

	/*
	 * The rgb_header identifies the pixel stream format,
	 * as described in the MIPI DSI Specification:
	 *
	 * 0x0E: Packed pixel stream, 16-bit RGB, 565 format
	 * 0x1E: Packed pixel stream, 18-bit RGB, 666 format
	 * 0x2E: Loosely Packed pixel stream, 18-bit RGB, 666 format
	 * 0x3E: Packed pixel stream, 24-bit RGB, 888 format
	 */
	switch (chnl->port.pixel_format) {
	case MCDE_PORTPIXFMT_DSI_16BPP:
		pixel_mode = 0;
		rgb_header = 0x0E;
		break;
	case MCDE_PORTPIXFMT_DSI_18BPP:
		pixel_mode = 2;
		rgb_header = 0x2E;
		break;
	case MCDE_PORTPIXFMT_DSI_18BPP_PACKED:
		pixel_mode = 1;
		rgb_header = 0x1E;
		break;
	case MCDE_PORTPIXFMT_DSI_24BPP:
		pixel_mode = 3;
		rgb_header = 0x3E;
		break;
	default:
		pixel_mode = 3;
		rgb_header = 0x3E;
		dev_warn(&mcde_dev->dev,
			"%s: invalid pixel format %d\n",
			__func__, chnl->port.pixel_format);
		break;
	}

	nova_dsilink_update_frame_parameters(chnl->dsilink,
					(struct dsilink_video_mode *)vmode, bpp,
							pixel_mode, rgb_header);
}

static void set_vsync_method(u8 idx, struct mcde_port *port)
{
	u32 out_synch_src = MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
	u32 src_synch = MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;

	if (port->type == MCDE_PORTTYPE_DSI) {
		switch (port->frame_trig) {
		case MCDE_TRIG_HW:
			src_synch = MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			break;
		case MCDE_TRIG_SW:
			src_synch = MCDE_CHNL0SYNCHMOD_SRC_SYNCH_SOFTWARE;
			break;
		default:
			src_synch = MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			break;
		}

		switch (port->sync_src) {
		case MCDE_SYNCSRC_OFF:
			out_synch_src =
				MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
			break;
		case MCDE_SYNCSRC_TE0:
			out_synch_src = MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_TE0;
			if (src_synch ==
				MCDE_CHNL0SYNCHMOD_SRC_SYNCH_SOFTWARE) {
				dev_dbg(&mcde_dev->dev, "%s: badly configured "
						"frame sync, TE0 defaulting "
						"to hw frame trig\n", __func__);
				src_synch =
					MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			}
			break;
		case MCDE_SYNCSRC_TE1:
			out_synch_src = MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_TE1;
			if (src_synch ==
				MCDE_CHNL0SYNCHMOD_SRC_SYNCH_SOFTWARE) {
				dev_dbg(&mcde_dev->dev, "%s: badly configured "
						"frame sync, TE1 defaulting "
						"to hw frame trig\n", __func__);
				src_synch =
					MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			}
			break;
		case MCDE_SYNCSRC_BTA:
			out_synch_src =
				MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
			break;
		case MCDE_SYNCSRC_TE_POLLING:
			out_synch_src =
				MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
			if (src_synch ==
				MCDE_CHNL0SYNCHMOD_SRC_SYNCH_SOFTWARE) {
				dev_dbg(&mcde_dev->dev, "%s: badly configured "
					"frame sync, TE_POLLING defaulting "
						"to hw frame trig\n", __func__);
				src_synch =
					MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			}
			break;
		default:
			out_synch_src =
				MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
			src_synch = MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE;
			dev_dbg(&mcde_dev->dev, "%s: no sync src selected, "
						"defaulting to DSI BTA with "
						"hw frame trig\n", __func__);
			break;
		}
	} else if (port->type == MCDE_PORTTYPE_DPI) {
		out_synch_src = MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC_FORMATTER;
		src_synch = port->update_auto_trig ?
					MCDE_CHNL0SYNCHMOD_SRC_SYNCH_HARDWARE :
					MCDE_CHNL0SYNCHMOD_SRC_SYNCH_SOFTWARE;
	}

	mcde_wreg(MCDE_CHNL0SYNCHMOD +
		idx * MCDE_CHNL0SYNCHMOD_GROUPOFFSET,
		MCDE_CHNL0SYNCHMOD_SRC_SYNCH(src_synch) |
		MCDE_CHNL0SYNCHMOD_OUT_SYNCH_SRC(out_synch_src));
}

void update_channel_registers(enum mcde_chnl chnl_id, struct chnl_regs *regs,
				struct mcde_port *port, enum mcde_fifo fifo,
				struct mcde_video_mode *video_mode)
{
	u8 idx = chnl_id;
	u32 fifo_wtrmrk = 0;
	u8 red;
	u8 green;
	u8 blue;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	/*
	 * Select appropriate fifo watermark.
	 * Watermark will be saturated at fifo size inside MCDE.
	 */
	fifo_wtrmrk = video_mode->xres /
		get_pkt_div(video_mode->xres, port, fifo);

	dev_vdbg(&mcde_dev->dev, "%s fifo_watermark=%d for chnl_id=%d\n",
		__func__, fifo_wtrmrk, chnl_id);

	switch (chnl_id) {
	case MCDE_CHNL_A:
		mcde_wfld(MCDE_CTRLA, FIFOWTRMRK, fifo_wtrmrk);
		break;
	case MCDE_CHNL_B:
		mcde_wfld(MCDE_CTRLB, FIFOWTRMRK, fifo_wtrmrk);
		break;
	case MCDE_CHNL_C0:
		mcde_wfld(MCDE_CTRLC0, FIFOWTRMRK, fifo_wtrmrk);
		break;
	case MCDE_CHNL_C1:
		mcde_wfld(MCDE_CTRLC1, FIFOWTRMRK, fifo_wtrmrk);
		break;
	default:
		break;
	}

	set_vsync_method(idx, port);

	mcde_wreg(MCDE_CHNL0CONF + idx * MCDE_CHNL0CONF_GROUPOFFSET,
		MCDE_CHNL0CONF_PPL(regs->ppl-1) |
		MCDE_CHNL0CONF_LPF(regs->lpf-1));
	mcde_wreg(MCDE_CHNL0STAT + idx * MCDE_CHNL0STAT_GROUPOFFSET,
		MCDE_CHNL0STAT_CHNLBLBCKGND_EN(regs->background_enable) |
		MCDE_CHNL0STAT_CHNLRD(true));
	if (regs->background_enable) {
		red = 0x80;
		green = 0x10;
		blue = 0x80;
	} else {
		red = video_mode->bckcol[0];
		green = video_mode->bckcol[1];
		blue = video_mode->bckcol[2];
	}
	mcde_wreg(MCDE_CHNL0BCKGNDCOL + idx * MCDE_CHNL0BCKGNDCOL_GROUPOFFSET,
		MCDE_CHNL0BCKGNDCOL_B(blue) |
		MCDE_CHNL0BCKGNDCOL_G(green) |
		MCDE_CHNL0BCKGNDCOL_R(red));

	if (chnl_id == MCDE_CHNL_A || chnl_id == MCDE_CHNL_B) {
		u32 mcde_crx1;
		u32 mcde_pal0x;
		u32 mcde_pal1x;
		if (chnl_id == MCDE_CHNL_A) {
			mcde_crx1 = MCDE_CRA1;
			mcde_pal0x = MCDE_PAL0A;
			mcde_pal1x = MCDE_PAL1A;
			mcde_wfld(MCDE_CRA0, PALEN, regs->palette_enable);
			mcde_wfld(MCDE_CRA0, OLEDEN, regs->oled_enable);
		} else {
			mcde_crx1 = MCDE_CRB1;
			mcde_pal0x = MCDE_PAL0B;
			mcde_pal1x = MCDE_PAL1B;
			mcde_wfld(MCDE_CRB0, PALEN, regs->palette_enable);
			mcde_wfld(MCDE_CRB0, OLEDEN, regs->oled_enable);
		}
		mcde_wreg(mcde_crx1,
			MCDE_CRA1_PCD(regs->pcd) |
			MCDE_CRA1_CLKSEL(regs->clksel) |
			MCDE_CRA1_CDWIN(regs->cdwin) |
			MCDE_CRA1_OUTBPP(bpp2outbpp(regs->bpp)) |
			MCDE_CRA1_BCD(regs->bcd) |
			MCDE_CRA1_CLKTYPE(regs->internal_clk));
	}

	/* Formatter */
	if (port->type == MCDE_PORTTYPE_DSI) {
		u8 fidx;
		u32 temp, packet;
		/* pkt_div is used to avoid underflow in output fifo for
		 * large packets */
		u32 pkt_div = 1;
		u32 dsi_delay0 = 0;
		u32 screen_ppl, screen_lpf;

		fidx = get_dsi_formatter_id(port);

		screen_ppl = video_mode->xres;
		screen_lpf = video_mode->yres;
		pkt_div = get_pkt_div(screen_ppl, port, fifo);

		if (video_mode->interlaced)
			screen_lpf /= 2;

		/* pkt_delay_progressive = pixelclock * htot /
		 * (1E12 / 160E6) / pkt_div */
		dsi_delay0 = (video_mode->pixclock) *
			(video_mode->xres + video_mode->hbp +
				video_mode->hfp) /
			(100000000 / ((mcde_clk_rate / 10000))) / pkt_div;

		if ((screen_ppl == SCREEN_PPL_CEA2) &&
				(screen_lpf == SCREEN_LPF_CEA2))
			dsi_delay0 += DSI_DELAY0_CEA2_ADD;

		temp = mcde_rreg(MCDE_DSIVID0CONF0 +
			fidx * MCDE_DSIVID0CONF0_GROUPOFFSET);
		mcde_wreg(MCDE_DSIVID0CONF0 +
			fidx * MCDE_DSIVID0CONF0_GROUPOFFSET,
			(temp & ~MCDE_DSIVID0CONF0_PACKING_MASK) |
			MCDE_DSIVID0CONF0_PACKING(regs->dsipacking));
		/* no extra command byte in video mode */
		if (port->mode == MCDE_PORTMODE_CMD)
			packet = ((screen_ppl / pkt_div * regs->bpp) >> 3) + 1;
		else
			packet = ((screen_ppl / pkt_div * regs->bpp) >> 3);
		mcde_wreg(MCDE_DSIVID0FRAME +
			fidx * MCDE_DSIVID0FRAME_GROUPOFFSET,
			MCDE_DSIVID0FRAME_FRAME(packet * pkt_div * screen_lpf));
		mcde_wreg(MCDE_DSIVID0PKT + fidx * MCDE_DSIVID0PKT_GROUPOFFSET,
			MCDE_DSIVID0PKT_PACKET(packet));
		mcde_wreg(MCDE_DSIVID0SYNC +
			fidx * MCDE_DSIVID0SYNC_GROUPOFFSET,
			MCDE_DSIVID0SYNC_SW(0) |
			MCDE_DSIVID0SYNC_DMA(0));
		mcde_wreg(MCDE_DSIVID0CMDW +
			fidx * MCDE_DSIVID0CMDW_GROUPOFFSET,
			MCDE_DSIVID0CMDW_CMDW_START(DCS_CMD_WRITE_START) |
			MCDE_DSIVID0CMDW_CMDW_CONTINUE(DCS_CMD_WRITE_CONTINUE));
		mcde_wreg(MCDE_DSIVID0DELAY0 +
			fidx * MCDE_DSIVID0DELAY0_GROUPOFFSET,
			MCDE_DSIVID0DELAY0_INTPKTDEL(dsi_delay0));
		mcde_wreg(MCDE_DSIVID0DELAY1 +
			fidx * MCDE_DSIVID0DELAY1_GROUPOFFSET,
			MCDE_DSIVID0DELAY1_TEREQDEL(0) |
			MCDE_DSIVID0DELAY1_FRAMESTARTDEL(0));

		if (port->sync_src == MCDE_SYNCSRC_TE0) {
			mcde_wreg(MCDE_VSCRC0,
				MCDE_VSCRC0_VSDBL(0) |
				MCDE_VSCRC0_VSSEL_ENUM(VSYNC0) |
				MCDE_VSCRC0_VSPOL(port->vsync_polarity) |
				MCDE_VSCRC0_VSPDIV(port->vsync_clock_div) |
				MCDE_VSCRC0_VSPMAX(port->vsync_max_duration) |
				MCDE_VSCRC0_VSPMIN(port->vsync_min_duration));
		} else if (port->sync_src == MCDE_SYNCSRC_TE1) {
			mcde_wreg(MCDE_VSCRC1,
				MCDE_VSCRC1_VSDBL(0) |
				MCDE_VSCRC1_VSSEL_ENUM(VSYNC1) |
				MCDE_VSCRC1_VSPOL(port->vsync_polarity) |
				MCDE_VSCRC1_VSPDIV(port->vsync_clock_div) |
				MCDE_VSCRC1_VSPMAX(port->vsync_max_duration) |
				MCDE_VSCRC1_VSPMIN(port->vsync_min_duration));
		}

		if (port->mode == MCDE_PORTMODE_VID)
			update_vid_frame_parameters(&channels[chnl_id],
						video_mode, regs->bpp / 8);
	} else if (port->type == MCDE_PORTTYPE_DPI &&
						!port->phy.dpi.tv_mode) {
		/* DPI LCD Mode */
		if (chnl_id == MCDE_CHNL_A) {
			mcde_wreg(MCDE_SYNCHCONFA,
				MCDE_SYNCHCONFA_HWREQVEVENT_ENUM(
							ACTIVE_VIDEO) |
				MCDE_SYNCHCONFA_HWREQVCNT(
							video_mode->yres - 1) |
				MCDE_SYNCHCONFA_SWINTVEVENT_ENUM(
							ACTIVE_VIDEO) |
				MCDE_SYNCHCONFA_SWINTVCNT(
							video_mode->yres - 1));
		} else if (chnl_id == MCDE_CHNL_B) {
			mcde_wreg(MCDE_SYNCHCONFB,
				MCDE_SYNCHCONFB_HWREQVEVENT_ENUM(
							ACTIVE_VIDEO) |
				MCDE_SYNCHCONFB_HWREQVCNT(
							video_mode->yres - 1) |
				MCDE_SYNCHCONFB_SWINTVEVENT_ENUM(
							ACTIVE_VIDEO) |
				MCDE_SYNCHCONFB_SWINTVCNT(
							video_mode->yres - 1));
		}
	}

	/* Blending */
	if (chnl_id == MCDE_CHNL_A) {
		mcde_wfld(MCDE_CRA0, BLENDEN, regs->blend_en);
		mcde_wfld(MCDE_CRA0, BLENDCTRL, regs->blend_ctrl);
		mcde_wfld(MCDE_CRA0, ALPHABLEND, regs->alpha_blend);
	} else if (chnl_id == MCDE_CHNL_B) {
		mcde_wfld(MCDE_CRB0, BLENDEN, regs->blend_en);
		mcde_wfld(MCDE_CRB0, BLENDCTRL, regs->blend_ctrl);
		mcde_wfld(MCDE_CRB0, ALPHABLEND, regs->alpha_blend);
	}

	dev_vdbg(&mcde_dev->dev, "Channel registers setup, chnl=%d\n", chnl_id);
	regs->dirty = false;
}

int enable_mcde_hw(void)
{
	int i;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	for (i = 0; i < num_channels; i++) {
		struct mcde_chnl_state *chnl = &channels[i];
		if (chnl->state == CHNLSTATE_SUSPEND) {
			/* Mark all registers as dirty */
			set_channel_state_atomic(chnl, CHNLSTATE_IDLE);
			chnl->ovly0->regs.dirty = true;
			chnl->ovly0->regs.dirty_buf = true;
			if (chnl->ovly1) {
				chnl->ovly1->regs.dirty = true;
				chnl->ovly1->regs.dirty_buf = true;
			}
			chnl->regs.dirty = true;
			chnl->col_regs.dirty = true;
			chnl->tv_regs.dirty = true;
			chnl->oled_regs.dirty = true;
		}
	}

	if (mcde_is_enabled) {
		dev_vdbg(&mcde_dev->dev, "%s - already enabled\n", __func__);
		return 0;
	}

	enable_clocks_and_power(mcde_dev);

	update_mcde_registers();

	dev_vdbg(&mcde_dev->dev, "%s - enable done\n", __func__);

	mcde_is_enabled = true;
	return 0;
}

/* DSI */
static int mcde_dsi_direct_cmd_write(struct mcde_chnl_state *chnl,
			bool dcs, u8 cmd, u8 *data, int len)
{
	int ret;

	if (len > MCDE_MAX_DSI_DIRECT_CMD_WRITE ||
			chnl->port.type != MCDE_PORTTYPE_DSI) {
		return -EINVAL;
	}

	mcde_lock(__func__, __LINE__);

	_mcde_chnl_enable(chnl);
	if (enable_mcde_hw()) {
		mcde_unlock(__func__, __LINE__);
		return -EINVAL;
	}
	if (!chnl->formatter_updated)
		(void)update_channel_static_registers(chnl);

	set_channel_state_sync(chnl, CHNLSTATE_DSI_WRITE);

	if (dcs)
		ret = nova_dsilink_dcs_write(chnl->dsilink, cmd, data, len);
	else
		ret = nova_dsilink_dsi_write(chnl->dsilink, cmd, data, len);

	set_channel_state_atomic(chnl, CHNLSTATE_IDLE);

	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s: ret=%d\n", __func__, ret);

	return ret;
}

int mcde_dsi_generic_write(struct mcde_chnl_state *chnl, u8* para, int len)
{
	return mcde_dsi_direct_cmd_write(chnl, false, 0, para, len);
}

int mcde_dsi_dcs_write(struct mcde_chnl_state *chnl, u8 cmd, u8* data, int len)
{
	return mcde_dsi_direct_cmd_write(chnl, true, cmd, data, len);
}

int mcde_dsi_dcs_read(struct mcde_chnl_state *chnl,
			u8 cmd, u32 *data, int *len)
{
	int ret = 0;

	if (*len > MCDE_MAX_DCS_READ || chnl->port.type != MCDE_PORTTYPE_DSI) {
		return -EINVAL;
	}

	mcde_lock(__func__, __LINE__);

	_mcde_chnl_enable(chnl);
	if (enable_mcde_hw()) {
		mcde_unlock(__func__, __LINE__);
		return -EINVAL;
	}
	if (!chnl->formatter_updated)
		(void)update_channel_static_registers(chnl);

	set_channel_state_sync(chnl, CHNLSTATE_DSI_READ);

	ret = nova_dsilink_dsi_read(chnl->dsilink, cmd, data, len);

	set_channel_state_atomic(chnl, CHNLSTATE_IDLE);

	mcde_unlock(__func__, __LINE__);

	return ret;
}

/*
 * Set Maximum Return Packet size is a command that specifies the
 * maximum size of the payload transmitted from peripheral back to
 * the host processor.
 *
 * During power-on or reset sequence, the Maximum Return Packet Size
 * is set to a default value of one. In order to be able to use
 * mcde_dsi_dcs_read for reading more than 1 byte at a time, this
 * parameter should be set by the host processor to the desired value
 * in the initialization routine before commencing normal operation.
 */
int mcde_dsi_set_max_pkt_size(struct mcde_chnl_state *chnl)
{
	if (chnl->port.type != MCDE_PORTTYPE_DSI)
		return -EINVAL;

	mcde_lock(__func__, __LINE__);

	if (enable_mcde_hw()) {
		mcde_unlock(__func__, __LINE__);
		return -EIO;
	}

	if (!chnl->formatter_updated)
		(void)update_channel_static_registers(chnl);

	set_channel_state_sync(chnl, CHNLSTATE_DSI_WRITE);

	nova_dsilink_send_max_read_len(chnl->dsilink);

	set_channel_state_atomic(chnl, CHNLSTATE_IDLE);

	mcde_unlock(__func__, __LINE__);

	return 0;
}

/* MCDE channels */
static struct mcde_chnl_state *_mcde_chnl_get(enum mcde_chnl chnl_id,
	enum mcde_fifo fifo, const struct mcde_port *port)
{
	int i;
	struct mcde_chnl_state *chnl = NULL;

	static struct mcde_col_transform ycbcr_2_rgb = {
		/* Note that in MCDE YUV 422 pixels come as VYU pixels */
		.matrix = {
			{0xff30, 0x012a, 0xff9c},
			{0x0000, 0x012a, 0x0204},
			{0x0199, 0x012a, 0x0000},
		},
		.offset = {0x0088, 0xfeeb, 0xff21},
	};

	static struct mcde_col_transform rgb_2_ycbcr = {
		.matrix = {
			{0x0042, 0x0081, 0x0019},
			{0xffda, 0xffb6, 0x0070},
			{0x0070, 0xffa2, 0xffee},
		},
		.offset = {0x0010, 0x0080, 0x0080},
	};

	/* Allocate channel */
	for (i = 0; i < num_channels; i++) {
		if (chnl_id == channels[i].id)
			chnl = &channels[i];
	}
	if (!chnl) {
		dev_dbg(&mcde_dev->dev, "Invalid channel, chnl=%d\n", chnl_id);
		return ERR_PTR(-EINVAL);
	}

	if (chnl->reserved) {
		dev_dbg(&mcde_dev->dev, "Channel in use, chnl=%d\n", chnl_id);
		return ERR_PTR(-EBUSY);
	}

	chnl->port = *port;
	chnl->fifo = fifo;
	chnl->formatter_updated = false;
	chnl->ycbcr_2_rgb = ycbcr_2_rgb;
	chnl->rgb_2_ycbcr = rgb_2_ycbcr;
	chnl->oled_color_conversion = false;

	chnl->blend_en = true;
	chnl->blend_ctrl = MCDE_CRA0_BLENDCTRL_SOURCE;
	chnl->alpha_blend = 0xFF;

	_mcde_chnl_apply(chnl);
	chnl->reserved = true;

	if (chnl->port.type == MCDE_PORTTYPE_DPI) {
/*TODO		chnl->clk_dpi = clk_get(&mcde_dev->dev, CLK_DPI); */
		if (chnl->port.phy.dpi.tv_mode)
			chnl->vcmp_per_field = true;
	} else if (chnl->port.type == MCDE_PORTTYPE_DSI) {
		chnl->dsilink = nova_dsilink_get(port->link);
		chnl->dsilink->update_dsi_freq = dsi_use_clk_framework;
	}

	return chnl;
}

static int _mcde_chnl_apply(struct mcde_chnl_state *chnl)
{
	bool roten = false;
	u8 rotdir = 0;

	dev_vdbg(&mcde_dev->dev, "%s: chnl=0x%x\n", __func__, (u32)chnl);
	if (chnl->rotation == MCDE_DISPLAY_ROT_90_CCW) {
		roten = true;
		rotdir = MCDE_ROTACONF_ROTDIR_CCW;
	} else if (chnl->rotation == MCDE_DISPLAY_ROT_90_CW) {
		roten = true;
		rotdir = MCDE_ROTACONF_ROTDIR_CW;
	}
	/* REVIEW: 180 deg? */

	chnl->regs.bpp = portfmt2bpp(chnl->port.pixel_format);
	chnl->regs.roten = roten;
	chnl->regs.rotdir = rotdir;

	if (chnl->port.type == MCDE_PORTTYPE_DSI) {
		chnl->regs.clksel = MCDE_CRA1_CLKSEL_MCDECLK;
		chnl->regs.dsipacking =
				portfmt2dsipacking(chnl->port.pixel_format);
	} else if (chnl->port.type == MCDE_PORTTYPE_DPI) {
		if (chnl->port.phy.dpi.tv_mode) {
			chnl->regs.internal_clk = false;
			chnl->regs.bcd = true;
			if (chnl->id == MCDE_CHNL_A)
				chnl->regs.clksel = MCDE_CRA1_CLKSEL_TV1CLK;
			else
				chnl->regs.clksel = MCDE_CRA1_CLKSEL_TV2CLK;
		} else {
			chnl->regs.internal_clk = true;
			chnl->regs.clksel = MCDE_CRA1_CLKSEL_CLKPLL72;
			chnl->regs.cdwin =
					portfmt2cdwin(chnl->port.pixel_format);
			chnl->regs.bcd = (chnl->port.phy.dpi.clock_div < 2);
			if (!chnl->regs.bcd)
				chnl->regs.pcd =
					chnl->port.phy.dpi.clock_div - 2;
		}
		dpi_video_mode_apply(chnl);
	}

	chnl->regs.blend_ctrl = chnl->blend_ctrl;
	chnl->regs.blend_en = chnl->blend_en;
	chnl->regs.alpha_blend = chnl->alpha_blend;

	chnl->regs.dirty = true;

	dev_vdbg(&mcde_dev->dev, "Channel applied, chnl=%d\n", chnl->id);
	return 0;
}

static void setup_channel(struct mcde_chnl_state *chnl)
{
	set_channel_state_sync(chnl, CHNLSTATE_SETUP);

	if (chnl->port.type == MCDE_PORTTYPE_DPI && chnl->tv_regs.dirty)
		update_dpi_registers(chnl->id, &chnl->tv_regs);

	/*
	 * For command mode displays using external sync (TE0/TE1), the first
	 * frame need special treatment to avoid garbage on the panel.
	 * This mechanism is placed here because it needs the chnl_state and
	 * modifies settings before they are committed to the registers.
	 */
	if (!chnl->port.update_auto_trig /*&& chnl->first_frame_vsync_fix*/) {
		switch (chnl->port.sync_src) {
		case MCDE_SYNCSRC_TE0:
		case MCDE_SYNCSRC_TE1:
			/* Save requested mode. */
			chnl->port.requested_sync_src = chnl->port.sync_src;
			chnl->port.requested_frame_trig = chnl->port.frame_trig;
			/*
			 * Temporarily set other mode.
			 * Requested mode will be set at next frame.
			 */
			chnl->port.sync_src = MCDE_SYNCSRC_OFF;
			chnl->port.frame_trig = MCDE_TRIG_SW;
			break;
		default:
			/* No vsync switch needed. */
			/*chnl->first_frame_vsync_fix = false; */
			break;
		}
	}

	if (chnl->id == MCDE_CHNL_A || chnl->id == MCDE_CHNL_B) {
		if (chnl->col_regs.dirty)
			update_col_registers(chnl->id, &chnl->col_regs);
		if (chnl->oled_regs.dirty)
			update_oled_registers(chnl->id, &chnl->oled_regs);
	}
	if (chnl->regs.dirty)
		update_channel_registers(chnl->id, &chnl->regs, &chnl->port,
						chnl->fifo, &chnl->vmode);
}

/*
 * Pixelfetcherwatermarklevel for each overlay is calculated.
 * The value is dependent of actual overlay bandwidth usage.
 * Input fifo is shared between all active overlays.
 */
static int calc_pixfetch_usage(struct mcde_ovly_state *ovly)
{
	int cnt;
	struct mcde_chnl_state *chnl = ovly->chnl;
	struct mcde_video_mode *vmode = &chnl->vmode;
	u32 pixfetch_avail;
	u32 old_val;
	u32 new_val;
	u8 fps = chnl->port.refresh_rate;
	u8 factor;
	struct mcde_ovly_state *ovlys = overlays;
	u32 level_high;
	struct mcde_ovly_state *ovly_high;
	u8 num_active_ovl = 0;
	u32 level_sum;
	int align_factor;

	/* If refresh_rate is not set, use default */
	if (fps == 0) {
		fps = REFRESH_RATE_DEFAULT;
		WARN_ON_ONCE(true);
	}

	/* Update pixfetch need for this overlay */
	switch (chnl->port.type) {
	case MCDE_PORTTYPE_DPI:
		factor = PIXFETCH_DPIMODE_FACTOR;
		break;
	case MCDE_PORTTYPE_DSI:
		if (chnl->port.update_auto_trig ||
				chnl->port.mode == MCDE_PORTMODE_VID)
			factor = PIXFETCH_DSIVIDEOMODE_FACTOR;
		else
			factor = PIXFETCH_DSICMDMODE_FACTOR;
		break;
	default:
		return -EINVAL;
		break;
	}

	old_val = ovly->pixfetch_need;
	if (ovly->inuse) {
		/* Divide to avoid overflow in calculation */
		new_val = vmode->xres * vmode->yres * fps / 1000 *
				factor * ovly->regs.bits_per_pixel / 1000;
	} else {
		new_val = 0;
		ovly->regs.pixfetchwtrmrklevel = 0;
	}

	/* Update total pixfetch need for all overlays */
	if (new_val != old_val) {
		pixfetch_need_tot -= old_val;
		pixfetch_need_tot += new_val;
		ovly->pixfetch_need = new_val;
	}

	/* Calculate available level (in bits) for all overlays */
	for (cnt = 0; cnt < num_overlays; cnt++) {
		if (!ovlys[cnt].pixfetch_need || !ovlys[cnt].inuse)
			continue;
		num_active_ovl++;
	}
	pixfetch_avail = input_fifo_size * PIXFETCH_WORD_SIZE -
			 ((num_active_ovl + 1) / 2) * PIXFETCH_BURST_SIZE;

	dev_dbg(&mcde_dev->dev, "num_ovl %d\n", num_active_ovl);

	/*
	 * Calculate distributed pixfetch levels for each overlay
	 * Level per overlay = need / total_need * avail / bpp
	 */
	level_sum = 0;
	level_high = 0;
	for (cnt = 0; cnt < num_overlays; cnt++) {
		if (!ovlys[cnt].pixfetch_need || !ovlys[cnt].inuse)
			continue;
		ovlys[cnt].regs.pixfetchwtrmrklevel_old =
					ovlys[cnt].regs.pixfetchwtrmrklevel;

		/*
		 * align_factor is used to align the overlays fifo usage to a
		 * multiple of PIXFETCH_BURST_SIZE
		 */
		switch (ovlys[cnt].regs.bits_per_pixel) {
		case 16:
		case 32:
		default:
			align_factor = 1;
			break;
		case 24:
			align_factor = 3;
			break;
		}

		/* Round up to multiple of burst size */
		new_val = ovlys[cnt].pixfetch_need * pixfetch_avail /
				pixfetch_need_tot;
		new_val -= 1;
		new_val /= (PIXFETCH_BURST_SIZE * align_factor);
		new_val += 1;
		new_val *= (PIXFETCH_BURST_SIZE * align_factor);
		level_sum += new_val;
		if (new_val > level_high) {
			level_high = new_val;
			ovly_high = &ovlys[cnt];
		}

		/* Unit pixels */
		new_val /= ovlys[cnt].regs.bits_per_pixel;
		ovlys[cnt].regs.pixfetchwtrmrklevel = new_val;
	}

	/* Check margin to fifo size */
	if (level_sum > pixfetch_avail) {
		int reduce;

		switch (ovly_high->regs.bits_per_pixel) {
		case 16:
		case 32:
		default:
			align_factor = 1;
			break;
		case 24:
			align_factor = 3;
			break;
		}

		/* Reduce the highest level */
		reduce = (level_sum - pixfetch_avail - 1) /
					(PIXFETCH_BURST_SIZE * align_factor);
		reduce += 1;
		reduce *= (PIXFETCH_BURST_SIZE * align_factor);
		ovly_high->regs.pixfetchwtrmrklevel -= reduce /
						ovly_high->regs.bits_per_pixel;
	}

	/* Set dirty flag */
	for (cnt = 0; cnt < num_overlays; cnt++) {
		if (!ovlys[cnt].pixfetch_need || !ovlys[cnt].inuse)
			continue;
		if (ovlys[cnt].regs.pixfetchwtrmrklevel !=
				ovlys[cnt].regs.pixfetchwtrmrklevel_old)
			ovlys[cnt].regs.dirty = true;
	}
	return 0;
}

static void chnl_update_continous(struct mcde_chnl_state *chnl,
						bool tripple_buffer)
{
	if (chnl->state == CHNLSTATE_RUNNING) {
		if (!tripple_buffer)
			wait_for_vcmp(chnl);
		return;
	}

	setup_channel(chnl);
	if (chnl->port.sync_src == MCDE_SYNCSRC_TE0)
		mcde_wfld(MCDE_CRC, SYCEN0, true);
	else if (chnl->port.sync_src == MCDE_SYNCSRC_TE1)
		mcde_wfld(MCDE_CRC, SYCEN1, true);

	enable_flow(chnl);
}

static void chnl_update_non_continous(struct mcde_chnl_state *chnl)
{
	/* Commit settings to registers */
	setup_channel(chnl);

	if (chnl->port.type == MCDE_PORTTYPE_DSI) {
		if (chnl->port.sync_src == MCDE_SYNCSRC_OFF) {
			if (chnl->port.frame_trig == MCDE_TRIG_SW) {
				do_softwaretrig(chnl);
			} else {
				enable_flow(chnl);
				disable_flow(chnl);
			}
			dev_vdbg(&mcde_dev->dev, "Channel update (no sync), "
							"chnl=%d\n", chnl->id);
		} else if (chnl->port.sync_src == MCDE_SYNCSRC_BTA) {
			nova_dsilink_te_request(chnl->dsilink);
			set_channel_state_atomic(chnl, CHNLSTATE_WAIT_TE);
			if (chnl->port.frame_trig == MCDE_TRIG_HW) {
				/*
				 * During BTA TE the MCDE block will be stalled,
				 * once the TE is received the DMA trig will
				 * happen
				 */
				enable_flow(chnl);
				disable_flow(chnl);
			}
		} else if (chnl->port.sync_src == MCDE_SYNCSRC_TE0) {
			enable_flow(chnl);
			mcde_wfld(MCDE_CRC, SYCEN0, true);
		}
	}
}

static void chnl_update_overlay(struct mcde_chnl_state *chnl,
						struct mcde_ovly_state *ovly)
{
	if (!ovly)
		return;

	if (ovly->regs.dirty_buf) {
		if (!chnl->port.update_auto_trig)
			set_channel_state_sync(chnl, CHNLSTATE_SETUP);
		update_overlay_registers_on_the_fly(ovly->idx, &ovly->regs);
	}
	if (ovly->regs.dirty) {
		if (!chnl->port.update_auto_trig)
			set_channel_state_sync(chnl, CHNLSTATE_SETUP);
		chnl_ovly_pixel_format_apply(chnl, ovly);
		update_overlay_registers(ovly->idx, &ovly->regs, &chnl->port,
			chnl->fifo, chnl->regs.x, chnl->regs.y,
			chnl->regs.ppl, chnl->regs.lpf, ovly->stride,
			chnl->vmode.interlaced, chnl->rotation);
		if (chnl->id == MCDE_CHNL_A || chnl->id == MCDE_CHNL_B) {
			update_oled_registers(chnl->id, &chnl->oled_regs);
			update_col_registers(chnl->id, &chnl->col_regs);
		}
	}
}

static int _mcde_chnl_update(struct mcde_chnl_state *chnl,
					struct mcde_rectangle *update_area,
					bool tripple_buffer)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	/* TODO: lock & make wait->trig async */
	if (!chnl->enabled || !update_area
			|| (update_area->w == 0 && update_area->h == 0)) {
		return -EINVAL;
	}

	if (chnl->port.update_auto_trig && tripple_buffer)
		wait_for_vcmp(chnl);

	chnl->regs.x   = update_area->x;
	chnl->regs.y   = update_area->y;
	/* TODO Crop against video_mode.xres and video_mode.yres */
	chnl->regs.ppl = update_area->w;
	chnl->regs.lpf = update_area->h;
	if (chnl->port.type == MCDE_PORTTYPE_DPI &&
						chnl->port.phy.dpi.tv_mode) {
		/* subtract border */
		chnl->regs.ppl -= chnl->tv_regs.dho + chnl->tv_regs.alw;
		/* subtract double borders, ie. for both fields */
		chnl->regs.lpf -= 2 * (chnl->tv_regs.dvo + chnl->tv_regs.bsl);
	} else if (chnl->port.type == MCDE_PORTTYPE_DSI &&
			chnl->vmode.interlaced)
		chnl->regs.lpf /= 2;

	chnl_update_overlay(chnl, chnl->ovly0);
	chnl_update_overlay(chnl, chnl->ovly1);

	if (chnl->port.update_auto_trig)
		chnl_update_continous(chnl, tripple_buffer);
	else
		chnl_update_non_continous(chnl);

	dev_vdbg(&mcde_dev->dev, "Channel updated, chnl=%d flow=%d\n", chnl->id, mcde_rfld(MCDE_CRA0, FLOEN));

	return 0;
}

static int _mcde_chnl_enable(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	chnl->enabled = true;
	return 0;
}

/* API entry points */
/* MCDE channels */
struct mcde_chnl_state *mcde_chnl_get(enum mcde_chnl chnl_id,
			enum mcde_fifo fifo, const struct mcde_port *port)
{
	struct mcde_chnl_state *chnl;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	if (port)
		chnl = _mcde_chnl_get(chnl_id, fifo, port);
	else
		chnl = NULL;
	return chnl;
}

int mcde_chnl_set_pixel_format(struct mcde_chnl_state *chnl,
					enum mcde_port_pix_fmt pix_fmt)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);
	if (!chnl->reserved)
		return -EINVAL;
	chnl->port.pixel_format = pix_fmt;

	return 0;
}

int mcde_chnl_set_video_mode(struct mcde_chnl_state *chnl,
					struct mcde_video_mode *vmode)
{
	if (chnl == NULL || vmode == NULL)
		return -EINVAL;

	chnl->vmode = *vmode;

	chnl->ovly0->dirty = true;
	if (chnl->ovly1)
		chnl->ovly1->dirty = true;

	return 0;
}

int mcde_chnl_set_rotation(struct mcde_chnl_state *chnl,
					enum mcde_display_rotation rotation)
{
	if (!chnl->reserved)
		return -EINVAL;

	if ((rotation == MCDE_DISPLAY_ROT_90_CW ||
			rotation == MCDE_DISPLAY_ROT_90_CCW) &&
			(chnl->id != MCDE_CHNL_A && chnl->id != MCDE_CHNL_B))
		return -EINVAL;

	chnl->rotation = rotation;

	return 0;
}

int mcde_chnl_set_power_mode(struct mcde_chnl_state *chnl,
				enum mcde_display_power_mode power_mode)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (!chnl->reserved)
		return -EINVAL;

	chnl->power_mode = power_mode;

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);

	return 0;
}

int mcde_chnl_apply(struct mcde_chnl_state *chnl)
{
	int ret ;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (!chnl->reserved)
		return -EINVAL;

	mcde_lock(__func__, __LINE__);
	ret = _mcde_chnl_apply(chnl);
	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit with ret %d\n", __func__, ret);

	return ret;
}

int mcde_chnl_update(struct mcde_chnl_state *chnl,
					struct mcde_rectangle *update_area,
					bool tripple_buffer)
{
	int ret;
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (!chnl->reserved)
		return -EINVAL;

	mcde_lock(__func__, __LINE__);
	enable_mcde_hw();
	if (!chnl->formatter_updated)
		(void)update_channel_static_registers(chnl);

	ret = _mcde_chnl_update(chnl, update_area, tripple_buffer);

	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit with ret %d\n", __func__, ret);

	return ret;
}

void mcde_chnl_put(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (chnl->enabled) {
		stop_channel(chnl);
		disable_mcde_hw(false, true);
		chnl->enabled = false;
	}

	chnl->reserved = false;
	if (chnl->port.type == MCDE_PORTTYPE_DPI) {
		clk_put(chnl->clk_dpi);
		if (chnl->port.phy.dpi.tv_mode) {
			chnl->vcmp_per_field = false;
		}
	} else if (chnl->port.type == MCDE_PORTTYPE_DSI) {
		nova_dsilink_put(chnl->dsilink);
	}

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

void mcde_chnl_stop_flow(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);
	if (mcde_is_enabled && chnl->enabled)
		stop_channel(chnl);
	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

void mcde_chnl_enable(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);
	_mcde_chnl_enable(chnl);
	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

void mcde_chnl_disable(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);
	/* The channel must be stopped before it is disabled */
	WARN_ON_ONCE(chnl->state == CHNLSTATE_RUNNING);
	disable_mcde_hw(false, true);
	chnl->enabled = false;
	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

void mcde_formatter_enable(struct mcde_chnl_state *chnl)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);
	_mcde_chnl_enable(chnl);
	if (enable_mcde_hw()) {
		mcde_unlock(__func__, __LINE__);
		dev_err(&mcde_dev->dev, "%s enable failed\n", __func__);
		return;
	}
	if (!chnl->formatter_updated)
		(void)update_channel_static_registers(chnl);
	mcde_dynamic_power_management = false;
	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "%s exit\n", __func__);
}

/* MCDE overlays */
struct mcde_ovly_state *mcde_ovly_get(struct mcde_chnl_state *chnl)
{
	struct mcde_ovly_state *ovly;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	/* FIXME: Add reserved check once formatter is split from channel */

	if (!chnl->ovly0->inuse)
		ovly = chnl->ovly0;
	else if (chnl->ovly1 && !chnl->ovly1->inuse)
		ovly = chnl->ovly1;
	else
		ovly = ERR_PTR(-EBUSY);

	if (!IS_ERR(ovly)) {
		ovly->inuse = true;
		ovly->paddr = 0;
		ovly->stride = 0;
		ovly->pix_fmt = MCDE_OVLYPIXFMT_RGB565;
		ovly->src_x = 0;
		ovly->src_y = 0;
		ovly->dst_x = 0;
		ovly->dst_y = 0;
		ovly->dst_z = 0;
		ovly->w = 0;
		ovly->h = 0;
		ovly->alpha_value = 0xFF;
		ovly->alpha_source = MCDE_OVL1CONF2_BP_PER_PIXEL_ALPHA;
		ovly->dirty = true;
		mcde_ovly_apply(ovly);
	}

	return ovly;
}

void mcde_ovly_put(struct mcde_ovly_state *ovly)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	if (!ovly->inuse)
		return;
	if (ovly->regs.enabled) {
		ovly->paddr = 0;
		ovly->dirty = true;
		mcde_ovly_apply(ovly);/* REVIEW: API call calling API call! */
	}
	ovly->inuse = false;
}

void mcde_ovly_set_source_buf(struct mcde_ovly_state *ovly, u32 paddr)
{
	if (!ovly->inuse) {
		dev_vdbg(&mcde_dev->dev, "%s: ovly->inuse=%d\n", __func__, ovly->inuse);
		return;
	}

	ovly->dirty = paddr == 0 || ovly->paddr == 0;
	ovly->dirty_buf = true;

	ovly->paddr = paddr;
}

void mcde_ovly_set_source_info(struct mcde_ovly_state *ovly,
	u32 stride, enum mcde_ovly_pix_fmt pix_fmt)
{
	if (!ovly->inuse) {
		dev_vdbg(&mcde_dev->dev, "%s: ovly->inuse=%d\n", __func__, ovly->inuse);
		return;
	}

	ovly->stride = stride;
	ovly->pix_fmt = pix_fmt;
	ovly->dirty = true;
}

void mcde_ovly_set_source_area(struct mcde_ovly_state *ovly,
	u16 x, u16 y, u16 w, u16 h)
{
	if (!ovly->inuse) {
		dev_vdbg(&mcde_dev->dev, "%s: ovly->inuse=%d\n", __func__, ovly->inuse);
		return;
	}

	ovly->src_x = x;
	ovly->src_y = y;
	ovly->w = w;
	ovly->h = h;
	ovly->dirty = true;
}

void mcde_ovly_set_dest_pos(struct mcde_ovly_state *ovly, u16 x, u16 y, u8 z)
{
	if (!ovly->inuse) {
		dev_vdbg(&mcde_dev->dev, "%s: ovly->inuse=%d\n", __func__, ovly->inuse);
		return;
	}

	ovly->dst_x = x;
	ovly->dst_y = y;
	ovly->dst_z = z;
	ovly->dirty = true;
}

void mcde_ovly_apply(struct mcde_ovly_state *ovly)
{
	if (!ovly->inuse) {
		dev_dbg(&mcde_dev->dev, "%s: ovly->inuse=%d\n", __func__, ovly->inuse);
		return;
	}

	mcde_lock(__func__, __LINE__);

	if (ovly->dirty || ovly->dirty_buf) {
		dev_dbg(&pdev->dev, "%s: ovly->dirty or ovly->dirty_buf!\n", __func__);
		ovly->regs.ch_id = ovly->chnl->id;
		ovly->regs.enabled = ovly->paddr != 0;
		ovly->regs.baseaddress0 = ovly->paddr;
		ovly->regs.baseaddress1 =
					ovly->regs.baseaddress0 + ovly->stride;
		ovly->regs.dirty_buf = true;
		ovly->dirty_buf = false;
	}
	if (!ovly->dirty) {
		mcde_unlock(__func__, __LINE__);
		return;
	}

	switch (ovly->pix_fmt) {/* REVIEW: Extract to table */
	case MCDE_OVLYPIXFMT_RGB565:
		ovly->regs.bits_per_pixel = 16;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_RGB565;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = true;
		break;
	case MCDE_OVLYPIXFMT_RGBA5551:
		ovly->regs.bits_per_pixel = 16;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_IRGB1555;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = false;
		break;
	case MCDE_OVLYPIXFMT_RGBA4444:
		ovly->regs.bits_per_pixel = 16;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_ARGB4444;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = false;
		break;
	case MCDE_OVLYPIXFMT_RGB888:
		ovly->regs.bits_per_pixel = 24;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_RGB888;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = true;
		break;
	case MCDE_OVLYPIXFMT_RGBX8888:
		ovly->regs.bits_per_pixel = 32;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_XRGB8888;
		ovly->regs.bgr = false;
		ovly->regs.bebo = true;
		ovly->regs.opq = true;
		break;
	case MCDE_OVLYPIXFMT_RGBA8888:
		ovly->regs.bits_per_pixel = 32;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_ARGB8888;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = false;
		break;
	case MCDE_OVLYPIXFMT_YCbCr422:
		ovly->regs.bits_per_pixel = 16;
		ovly->regs.bpp = MCDE_EXTSRC0CONF_BPP_YCBCR422;
		ovly->regs.bgr = false;
		ovly->regs.bebo = false;
		ovly->regs.opq = true;
		break;
	default:
		break;
	}

	ovly->regs.ppl = ovly->w;
	ovly->regs.lpf = ovly->h;
	ovly->regs.cropx = ovly->src_x;
	ovly->regs.cropy = ovly->src_y;
	ovly->regs.xpos = ovly->dst_x;
	ovly->regs.ypos = ovly->dst_y;
	ovly->regs.z = ovly->dst_z > 0; /* 0 or 1 */
	if (!ovly->chnl->oled_color_conversion)
		ovly->regs.col_conv = MCDE_OVL0CR_COLCCTRL_DISABLED;
	ovly->regs.alpha_source = ovly->alpha_source;
	ovly->regs.alpha_value = ovly->alpha_value;

	ovly->regs.dirty = true;
	ovly->dirty = false;

	mcde_unlock(__func__, __LINE__);

	dev_vdbg(&mcde_dev->dev, "Overlay applied, idx=%d chnl=%d\n",
						ovly->idx, ovly->chnl->id);
}

static int init_clocks_and_power(struct platform_device *pdev)
{
	int ret = 0;
	return ret;
}

static void remove_clocks_and_power(struct platform_device *pdev)
{
	/* REVIEW: Release only if exist */
	/* REVIEW: Remove make sure MCDE is done */
	clk_put(clock_mcde);
	if (regulator_vana)
		regulator_put(regulator_vana);
	regulator_put(regulator_mcde_epod);
	regulator_put(regulator_esram_epod);
}

static int probe_hw(struct platform_device *pdev)
{
	int i;
	int ret;
	u32 pid;

	dev_info(&mcde_dev->dev, "Probe HW\n");

	/* Get MCDE HW version */
	regulator_enable(regulator_mcde_epod);
	clk_enable(clock_mcde);
	pid = mcde_rreg(MCDE_PID);

	dev_info(&mcde_dev->dev, "MCDE HW revision 0x%08X\n", pid);

	clk_disable(clock_mcde);
	regulator_disable(regulator_mcde_epod);

	switch (pid) {
	case MCDE_VERSION_3_0_8:
		num_channels = 4;
		num_overlays = 6;
		dsi_ifc_is_supported = true;
		input_fifo_size = 128;
		output_fifo_ab_size = 640;
		output_fifo_c0c1_size = 160;
		dsi_use_clk_framework = true;
		dev_info(&mcde_dev->dev, "db8500 V2 HW\n");
		break;
	case MCDE_VERSION_4_0_4:
		num_channels = 2;
		num_overlays = 3;
		input_fifo_size = 80;
		output_fifo_ab_size = 320;
		dsi_ifc_is_supported = false;
		dsi_use_clk_framework = false;
		dev_info(&mcde_dev->dev, "db5500 V2 HW\n");
		break;
	case MCDE_VERSION_4_1_3:
		num_channels = 4;
		num_overlays = 6;
		dsi_ifc_is_supported = true;
		input_fifo_size = 192;
		output_fifo_ab_size = 640;
		output_fifo_c0c1_size = 160;
		dsi_use_clk_framework = true;
		dev_info(&mcde_dev->dev, "db9540 V1 HW\n");
		break;
	case MCDE_VERSION_3_0_5:
		/* Intentional */
	case MCDE_VERSION_1_0_4:
		/* Intentional */
	default:
		dev_err(&mcde_dev->dev, "Unsupported HW version\n");
		ret = -ENOTSUPP;
		goto unsupported_hw;
		break;
	}

	channels = kzalloc(num_channels * sizeof(struct mcde_chnl_state),
								GFP_KERNEL);
	if (!channels) {
		ret = -ENOMEM;
		goto failed_channels_alloc;
	}

	overlays = kzalloc(num_overlays * sizeof(struct mcde_ovly_state),
								GFP_KERNEL);
	if (!overlays) {
		ret = -ENOMEM;
		goto failed_overlays_alloc;
	}

	/* Init MCDE */
	for (i = 0; i < num_overlays; i++)
		overlays[i].idx = i;

	channels[0].ovly0 = &overlays[0];
	channels[0].ovly1 = &overlays[1];
	channels[1].ovly0 = &overlays[2];

	if (pid == MCDE_VERSION_3_0_8 || MCDE_VERSION_4_1_3) {
		channels[1].ovly1 = &overlays[3];
		channels[2].ovly0 = &overlays[4];
		channels[3].ovly0 = &overlays[5];
	}

	for (i = 0; i < num_channels; i++) {
		channels[i].id = i;

		channels[i].ovly0->chnl = &channels[i];
		if (channels[i].ovly1)
			channels[i].ovly1->chnl = &channels[i];

		channels[i].dsilink = NULL;
	}

	return 0;

failed_overlays_alloc:
	kfree(channels);
	channels = NULL;
unsupported_hw:
failed_channels_alloc:
	num_channels = 0;
	num_overlays = 0;
	return ret;
}

int __devinit mcde_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct mcde_platform_data *pdata = (struct mcde_platform_data *)
						pdev->dev.platform_data;

	if (!pdata) {
		dev_dbg(&pdev->dev, "No platform data\n");
		return -EINVAL;
	}

	mcde_dev = pdev;

	mcdeio = (u8 *)CFG_MCDE_BASE;

	ret = init_clocks_and_power(pdev);
	if (ret < 0) {
		dev_warn(&pdev->dev, "%s: init_clocks_and_power failed\n"
					, __func__);
		goto failed_init_clocks;
	}

	ret = probe_hw(pdev);
	if (ret)
		goto failed_probe_hw;

	ret = enable_mcde_hw();
	if (ret)
		goto failed_mcde_enable;

	dev_info(&mcde_dev->dev, "%s: EXIT 0\n", __func__);
	return 0;

failed_mcde_enable:
failed_probe_hw:
	remove_clocks_and_power(pdev);
failed_init_clocks:
	return ret;
}

static int mcde_remove(struct platform_device *pdev)
{
	remove_clocks_and_power(pdev);
	return 0;
}

#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM)
static int mcde_resume(struct platform_device *pdev)
{
	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);

	if (enable_mcde_hw()) {
		mcde_unlock(__func__, __LINE__);
		return -EINVAL;
	}

	mcde_unlock(__func__, __LINE__);

	return 0;
}

static int mcde_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret;

	dev_vdbg(&mcde_dev->dev, "%s\n", __func__);

	mcde_lock(__func__, __LINE__);

	if (!mcde_is_enabled) {
		mcde_unlock(__func__, __LINE__);
		return 0;
	}
	disable_mcde_hw(true, true);

	mcde_unlock(__func__, __LINE__);

	return ret;
}
#endif

void mcde_exit(void)
{
	/* REVIEW: shutdown MCDE? */
}
