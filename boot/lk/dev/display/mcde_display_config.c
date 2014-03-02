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
#include <err.h>
#include <kernel/thread.h>
#include "config.h"
#include "target_config.h"
#include "target.h"
#include "machineid.h"
#include "mcde.h"
#include "mcde_regs.h"
#include "mcde_display.h"

extern int sony_acx424akp_probe(struct mcde_display_device *dev);
extern int mcde_display_sony_acx424akp_init(void);
extern void mcde_display_sony_acx424akp_exit(void);
extern void samsung_s6d16d0_probe(struct mcde_display_device *ddev);
extern int mcde_display_himax_hx8392_init(void);
extern void mcde_display_himax_hx8392_exit(void);
extern int himax_hx8392_probe(struct mcde_display_device *ddev);
extern int mcde_probe(struct platform_device *pdev);

static int splash_thread_func (void *arg);

enum {
	PRIMARY_DISPLAY_ID,
};

static struct mcde_port sony_port0 = {
	.link = 0,
	.sync_src = MCDE_SYNCSRC_OFF,
	.frame_trig = MCDE_TRIG_SW,
};

static struct mcde_display_dsi_platform_data
					sony_acx424akp_display0_pdata = {
	.num_data_lanes = 2,
	.port_mode = MCDE_PORTMODE_CMD,
};

static struct mcde_display_device sony_acx424akp_display0 = {
	.name = "mcde_disp_sony_acx424akp",
	.id = PRIMARY_DISPLAY_ID,
	.port = &sony_port0,
	.chnl_id = MCDE_CHNL_A,
	.fifo = MCDE_FIFO_A,
	.orientation = MCDE_DISPLAY_ROT_0,
	.default_pixel_format = MCDE_OVLYPIXFMT_RGBA8888,
	.dev = {
		.platform_data = (struct platform_data *)
						&sony_acx424akp_display0_pdata,
	},
};

static struct mcde_port himax_hx8392_port = {
	.sync_src = MCDE_SYNCSRC_OFF,
	.frame_trig = MCDE_TRIG_SW,
};

static struct mcde_display_dsi_platform_data himax_hx8392_display_pdata = {
	.link = 1,
	.num_data_lanes = 3,
};

static struct mcde_display_device himax_hx8392_display = {
	.name = "himax_hx8392",
	.id = PRIMARY_DISPLAY_ID,
	.port = &himax_hx8392_port,
	.chnl_id = MCDE_CHNL_A,
	.fifo = MCDE_FIFO_A,
	.orientation = MCDE_DISPLAY_ROT_0,
	.default_pixel_format = MCDE_OVLYPIXFMT_RGBA8888,
	.dev = {
		.platform_data = (struct platform_data *)
						&himax_hx8392_display_pdata,
	},
};

u8 mcde_platform_outmux[5] = {0, 0, 0, 0, 0 };
u8 mcde_platform_syncmux =0;

#if CONFIG_SYS_DISPLAY_DPI
struct mcde_port port0 = {
	.mode = MCDE_PORTMODE_VID,
	.ifc = 0,
	.link = 0,
	.sync_src = MCDE_SYNCSRC_OFF,	/* sync from output formatter  */
	.dpi = {
		.clock_div = 2,
		.polarity = DPI_ACT_LOW_VSYNC | DPI_ACT_LOW_HSYNC |
						DPI_ACT_LOW_DATA_ENABLE |
						DPI_ACT_ON_FALLING_EDGE ,
		.prcmu_clock_setting = PRCMU_CLK_EN |
						(4 << PRCMU_CLK_PLL_SW_SHIFT) |
						(8 << PRCMU_CLK_PLL_DIV_SHIFT);
	},
};
struct mcde_display_generic_platform_data main_display_data = { 0 };
u8 mcde_platform_outmux[5] = { 0, 1, 0, 0, 2 };
u8 mcde_platform_syncmux = 0;

struct mcde_video_mode video_mode = {
	.xres = CONFIG_SYS_DISPLAY_NATIVE_X_RES ,	/* display width */
	.yres = CONFIG_SYS_DISPLAY_NATIVE_Y_RES,	/* display height */
	.pixel_format = MCDE_OVLYPIXFMT_RGB888,/* framebuffer pixel format */
	.bckcol = {255, 255, 255},			/* R, G, B */
	.hbp = 16 - 2,	/* Horizontal Back Porch */
	.hfp = 16,	/* Horizontal Front Porch */
	.hsw = 2,	/* Horizontal Synchronization pulse Width */
	.vbp1 = 3,	/* Vertical Back Porch 1 */
	.vfp1 = 28 - 2,	/* Vertical Front Porch 1 */
	.vbp2 = 0,	/* Vertical Back Porch 2 */
	.vfp2 = 0,	/* Vertical Front Porch 2 */
	.vsw = 2,	/* Vertical Synchronization pulse Width */
};
#endif

static struct mcde_platform_data mcde_pdata;

struct platform_device ux500_mcde_device = {
	.name = "mcde",
	.id = -1,
	.dev = {
		.platform_data = &mcde_pdata,
	},
};

struct mcde_display_device *display0;

int init_display_devices(void)
{
	u32 machine_id = get_machine_id ();
	u32 reset_gpio;
	int ret;

	/* Sony display on U8500UIBV3 */
	if (machine_id == MACH_TYPE_U9540 ||
		machine_id == MACH_TYPE_U8540)
		reset_gpio = CONFIG_SYS_DISPLAY_GPIO_RESET;
	else
		reset_gpio = MOP500_DISP1_RST_GPIO;

	if (machine_id == MACH_TYPE_U9540 ||
		machine_id == MACH_TYPE_U8540 ||
		machine_id == MACH_TYPE_U8500 ||
		machine_id == MACH_TYPE_A9500 ||
		machine_id == MACH_TYPE_U8520 ||
		machine_id == MACH_TYPE_HREFV60) {

		sony_acx424akp_display0_pdata.reset_gpio = reset_gpio;
		mcde_display_sony_acx424akp_init();
		display0 = &sony_acx424akp_display0;
		(void)mcde_display_device_register(display0);
		ret = sony_acx424akp_probe(display0);
		if (!ret)
			goto panel_found;

		dprintf(INFO, "%s: sony_acx424akp_probe ret=%d\n",
								__func__, ret);
		/* unregister this panel */
		mcde_display_device_unregister(display0);
		mcde_display_sony_acx424akp_exit();
	}

	if (machine_id == MACH_TYPE_U9540 ||
		machine_id == MACH_TYPE_U8540) {
		himax_hx8392_display_pdata.reset_gpio = reset_gpio;
		mcde_display_himax_hx8392_init();
		display0 = &himax_hx8392_display;
		(void)mcde_display_device_register(display0);
		ret = himax_hx8392_probe(display0);
		if (!ret)
			goto panel_found;

		dprintf(INFO, "%s: himax_hx8392_probe ret=%d\n",
								__func__, ret);
		/* unregister this panel */
		mcde_display_device_unregister(display0);
		mcde_display_himax_hx8392_exit();
	}

	/* no valid panel found, return error */
	return ENODEV;

panel_found:
	return 0;
}

int mcde_splash_image(void)
{
	dprintf(SPEW, "%s: start\n", __func__);

	return thread_resume(thread_create("SPLASH", &splash_thread_func,
				NULL, HIGH_PRIORITY, 8 * 1024));
}

struct mcde_chnl_state *chnl;

int splash_thread_func(void *arg)
{
	struct mcde_video_mode video_mode;
	int ret;
	int i;

	memset((char *)&video_mode, 0, sizeof(video_mode));

	ret = mcde_probe(&ux500_mcde_device);
	if (ret) {
		dprintf(CRITICAL, "%s: mcde_probe() returned %d\n",
								__func__, ret);
		return ret;
	}

	dsilink_init();

	/* find correct display device and probe it */
	ret = init_display_devices();
	if (ret) {
		dprintf(CRITICAL, "%s: unable to detect display device\n",
								__func__);
		goto error;
	}

	chnl = mcde_chnl_get(display0->chnl_id, display0->fifo, display0->port);
	if (IS_ERR(chnl)) {
		dprintf(CRITICAL, "%s: mcde_chnl_get returned NULL\n",
								__func__);
		goto error;
	}
	display0->chnl_state = chnl;

	if (chnl->port.type == MCDE_PORTTYPE_DSI) {
		chnl->regs.clksel = MCDE_CRA1_CLKSEL_MCDECLK;
	} else if (chnl->port.type == MCDE_PORTTYPE_DPI) {
		/*TODO	mcde_startup_dpi(chnl); */
		chnl->regs.internal_clk = 1;
		chnl->regs.clksel = MCDE_CRA1_CLKSEL_CLKPLL72;
		chnl->regs.cdwin = MCDE_CRA1_CDWIN_24BPP;
		chnl->regs.bcd = (chnl->port.phy.dpi.clock_div < 2);
		if (!chnl->regs.bcd)
			chnl->regs.pcd = chnl->port.phy.dpi.clock_div - 2;
	}

	video_mode.xres = display0->native_x_res;
	video_mode.yres = display0->native_y_res;
	video_mode.bckcol[0] = 255;	/* Red */
	video_mode.bckcol[1] = 255;	/* Green */
	video_mode.bckcol[2] = 255;	/* Blue */

	if (display0->port->mode == MCDE_PORTMODE_VID) {
		ret = display0->try_video_mode(display0, &video_mode);
		if (ret) {
			dprintf(CRITICAL,
					"%s: Failed try_video_mode() ret=%d\n",
								__func__, ret);
		}

		ret = display0->set_video_mode(display0, &video_mode);
		if (ret) {
			dprintf(CRITICAL,
					"%s: Failed set_video_mode() ret=%d\n",
								__func__, ret);
		}
	}
	else {
		/* dss set video mode */
		ret = mcde_chnl_set_video_mode(chnl, &video_mode);
		if (ret) {
			dprintf(CRITICAL,
			"%s: Failed to set video mode on channel ret=%d\n",
								__func__, ret);
		}
	}

	mcde_chnl_enable(chnl);

	if (chnl->port.type == MCDE_PORTTYPE_DPI) {
		/* TODO	ret = mcde_turn_on_dpi(chnl); */
		dprintf(CRITICAL, "%s: turn_on_dpi() not implemented\n",
								__func__);
	}
	mcde_chnl_apply(chnl);

	/* display image with rotation */
	ret = mcde_display_image(chnl, display0->orientation);
	if (ret) {
		dprintf(CRITICAL, "%s: mcde_display_image() failed ret=%d\n",
								__func__, ret);
		goto error;
	}

	display0->update_area.x = 0;
	display0->update_area.y = 0;
	display0->update_area.w = video_mode.xres;
	display0->update_area.h = video_mode.yres;
	/*
	 * This loop is a workaround for some displays need several updates 
	 * to display the splash image.
	 */
	for (i = 0; i < 2; i++) {
		ret = display0->update(display0, 0);
		if (ret) {
			dprintf(CRITICAL,
				"%s: display0->update(display0, 0) ret = %d\n",
								__func__, ret);
			goto error;
		}
		mdelay(20);
	}

	goto done;

error:
	dprintf(CRITICAL, "%s: failed ret=%d\n", __func__, ret);

done:

	return ret;
}
