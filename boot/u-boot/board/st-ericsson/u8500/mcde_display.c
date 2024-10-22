/*
* Copyright (C) ST-Ericsson SA 2010
*
* Author: Jimmy Rubin <jimmy.rubin@stericsson.com>
* for ST-Ericsson.
*
* License terms: GNU General Public License (GPL), version 2.
*/

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/arch/common.h>
#include <asm/arch/ab8500.h>
#include <linux/err.h>
#include "mcde.h"
#include "mcde_regs.h"
#include "mcde_display.h"
#include "dsilink_regs.h"

#include "db8500_pincfg.h"
#include "db8500_pins.h"

static struct mcde_chnl_state *chnl;

#if CONFIG_SYS_DISPLAY_DSI
static struct mcde_port port0 = {
	.type = MCDE_PORTTYPE_DSI,
	.mode = MCDE_PORTMODE_CMD,
	.ifc = 1,
	.link = 0,
	.sync_src = MCDE_SYNCSRC_BTA,
	.phy = {
		.dsi = {
			.virt_id = 0,
			.num_data_lanes = 2,
			.ui = 9,
		},
	},
};

struct mcde_display_generic_platform_data main_display_data = {
	.reset_delay = CONFIG_SYS_DISPLAY_RST_DELAY,
};

struct mcde_platform_data platform_data = {
	0, {0}, 0,
};

struct mcde_video_mode video_mode = {
	.xres = CONFIG_SYS_DISPLAY_NATIVE_X_RES,
	.yres = CONFIG_SYS_DISPLAY_NATIVE_Y_RES,
	.pixclock = 37037,	/* from kernel */
	.interlaced = 0,
	.bckcol = {0, 0, 0}, /* R, G, B */
};

struct mcde_display_device main_display = {
	.port = &port0,
	.chnl_id = MCDE_CHNL_A,
#ifdef CONFIG_VIA_DISPLAY_DSI	
	.fifo = MCDE_FIFO_A,
#else	
 	.fifo = MCDE_FIFO_C0,
#endif	
	.default_pixel_format = MCDE_OVLYPIXFMT_RGB565,
	.port_pixel_format = MCDE_PORTPIXFMT_DSI_24BPP,
	.native_x_res = CONFIG_SYS_DISPLAY_NATIVE_X_RES,
	.native_y_res = CONFIG_SYS_DISPLAY_NATIVE_Y_RES,
};
#endif

#if CONFIG_SYS_DISPLAY_DPI
/*
 * STE VUIB card
 */
static struct mcde_port port0 = {
	.type = MCDE_PORTTYPE_DPI,
	.mode = MCDE_PORTMODE_VID,
	.ifc = 0,
	.link = 1,		/* DPI channel B can only be on link 1 */
	.sync_src = MCDE_SYNCSRC_OFF,   /* sync from output formatter  */
	.update_auto_trig = TRUE,
	.phy = {
		.dpi = {
			.tv_mode = FALSE,
			.clock_div = 2,
			.polarity = DPI_ACT_LOW_VSYNC | DPI_ACT_LOW_HSYNC,
		},
	},
};

struct mcde_display_generic_platform_data main_display_data = {
	0, 0,
};

struct mcde_platform_data platform_data = {
	/* DPI */
	/*
	 * [0] = 3: 24 bits DPI: connect LSB Ch B to D[0:7]
	 * [3] = 4: 24 bits DPI: connect MID Ch B to D[24:31]
	 * [4] = 5: 24 bits DPI: connect MSB Ch B to D[32:39]
	 *
	 * [1] = 3: TV out     : connect LSB Ch B to D[8:15]
	 */
#define DONT_CARE 0
#if defined(BOARD_C7_P0)
	.outmux = { 3, 3, DONT_CARE, 4, 5 },			/* vuib500 */
#else
	.outmux = { 3, 4, 5, DONT_CARE, DONT_CARE },	//joe debug
#endif	
#undef DONT_CARE
	.syncmux = 0x00,/*0x01,*/			/* vuib500 */	//joe debug
};

#define DPI_PIXCLK_FREQ	25000000

struct mcde_video_mode video_mode = {
	.hbp = 8/*40*/,	/* Horizontal Back Porch */
	.hfp = 8,	/* Horizontal Front Porch */
	.hsw = 12/*96*/,	/* Horizontal Synchronization pulse Width */
	.vbp1 = 8/*25*/,	/* Vertical Back Porch 1 */
	.vfp1 = 8/*2*/,	/* Vertical Front Porch 1 */
	.vbp2 = 0,	/* Vertical Back Porch 2 */
	.vfp2 = 0,	/* Vertical Front Porch 2 */
	.vsw = 4,	/* Vertical Synchronization pulse Width */
	.pixclock = (int)(1e+12 * (1.0 / DPI_PIXCLK_FREQ)),
	.interlaced = 0,
	.bckcol = {255, 255, 255}, /* R, G, B */
};

struct mcde_display_device main_display = {
	.port = &port0,
	.chnl_id = MCDE_CHNL_B,//MCDE_CHNL_A,
	.fifo = MCDE_FIFO_B,//MCDE_FIFO_A,
	.default_pixel_format = MCDE_OVLYPIXFMT_RGB565,
	.port_pixel_format = /*MCDE_PORTPIXFMT_DPI_24BPP*/MCDE_PORTPIXFMT_DPI_18BPP_C1,	//joe debug
	.native_x_res = CONFIG_SYS_DISPLAY_NATIVE_X_RES,
	.native_y_res = CONFIG_SYS_DISPLAY_NATIVE_Y_RES,
};
#endif

int mcde_turn_on_display(void)
{
	debug("%s: Enter\n", __func__);

	if (main_display.port->type == MCDE_PORTTYPE_DSI)
		return mcde_turn_on_display_dsi();
	else if (main_display.port->type == MCDE_PORTTYPE_DPI)
		return mcde_turn_on_display_dpi();
	return -ENODEV;	/* Should never occur */
}

int mcde_splash_image(void)
{
	int ret = -ENODEV;
	struct mcde_rectangle rect;

	debug("%s: enter\n", __func__);

	if (main_display.port->type == MCDE_PORTTYPE_DSI)
		ret = mcde_startup_dsi(&platform_data);
	else if (main_display.port->type == MCDE_PORTTYPE_DPI)
		ret = mcde_startup_dpi(&platform_data);
	if (ret != 0) {
		printf("%s: mcde_startup... -> %d\n", __func__, ret);
		return ret;
	}

	/* dss enable display */
	chnl = mcde_chnl_get(main_display.chnl_id, main_display.fifo,
							main_display.port);
	if (IS_ERR(chnl)) {
		ret = PTR_ERR(chnl);
		printf("%s: Failed to acquire MCDE channel ret=%d\n",
				__func__, ret);
		goto get_chnl_failed;
	}

	ret = mcde_chnl_set_power_mode(chnl, MCDE_DISPLAY_PM_STANDBY);
	if (ret) {
		printf("%s: mcde_chnl_set_power_mode() -> %d\n",
				__func__, ret);
		goto get_chnl_failed;
	}

	/* dss set video mode */
	ret = mcde_chnl_set_video_mode(chnl, &video_mode);
	if (ret < 0) {
		printf("%s:Failed to set video mode on channel ret=%d\n",
			__func__, ret);
		goto set_video_mode_failed;
	}

	mcde_chnl_set_pixel_format(chnl, main_display.port_pixel_format);
	mcde_chnl_set_update_area(chnl, 0, 0, main_display.native_x_res,
						main_display.native_y_res);
	mcde_chnl_apply(chnl);

	/* chnl setup ok, display image */
	ret = mcde_display_image(chnl);
	if (ret != 0) {
		debug("%s: mcde_display_image() -> %d\n",
			__func__, ret);
		goto display_image_failed;
	}
#if defined(BOARD_C7_P0)
	/* vdd power on */
	db8500_gpio_set_output(GPIO167_GPIO, 1);
	udelay(400);
	/* stbyb pull high */
	db8500_gpio_set_output(GPIO154_GPIO, 1);
	udelay(400);
	/* reset pull high */
	db8500_gpio_set_output(GPIO139_GPIO, 1);
	mdelay(20);
	db8500_gpio_set_output(GPIO156_GPIO, 1);
	mdelay(20);
	db8500_gpio_set_output(GPIO168_GPIO, 1);
	mdelay(20);
	db8500_gpio_set_output(GPIO218_GPIO, 1);
	mdelay(20);
	db8500_gpio_set_output(GPIO157_GPIO, 1);
	mdelay(20);
#endif

#ifdef CONFIG_VIA_DISPLAY_DSI
	ret = mcde_turn_on_display();
	if (ret) {
		printf("%s: mcde_turn_on_display() -> %d\n", __func__, ret);
		goto get_chnl_failed;
	}
#endif
#if defined(BOARD_C7_P0)
	/* avdd vcom vgl vgh is on */
	db8500_gpio_set_output(GPIO153_GPIO, 1);
	mdelay(40);
	
	db8500_gpio_set_output(GPIO139_GPIO, 0);
	mdelay(1);
	db8500_gpio_set_output(GPIO139_GPIO, 1);
#endif
	mcde_chnl_apply(chnl);
	rect.x = 0;
	rect.y = 0;
	rect.w = main_display.native_x_res;
	rect.h = main_display.native_y_res;
	mcde_chnl_update(chnl, &rect);

#ifndef CONFIG_VIA_DISPLAY_DSI
	ret = mcde_turn_on_display();
	if (ret) {
		printf("%s: mcde_turn_on_display() -> %d\n", __func__, ret);
		goto get_chnl_failed;
	}
#endif

	ret = mcde_chnl_set_power_mode(chnl, MCDE_DISPLAY_PM_ON);
	if (ret) {
		printf("%s: mcde_chnl_set_power_mode() -> %d\n", __func__, ret);
		goto get_chnl_failed;
	}

	mdelay(50);

	db8500_gpio_set_output(GPIO68_GPIO, 1);

	return ret;

get_chnl_failed:
display_image_failed:
set_video_mode_failed:
	mcde_exit();
	return ret;
}

int mcde_splash_txt(void)
{
	int err = 0;
	struct mcde_rectangle rect;
	static struct mcde_ovly_state *ovly = NULL;
	u32 width;
	u32 height;
	u32 lcd_bpp;
	u8 *paddr;
	u32 stride;
	u32 xpos = 0;
	u32 ypos = 0;

	paddr = (u8 *) CONFIG_SYS_VIDEO_FB_ADRS;
	width = CONFIG_SYS_DISPLAY_NATIVE_X_RES;
	height = CONFIG_SYS_DISPLAY_NATIVE_Y_RES;

	lcd_bpp = 16;
	stride = width * lcd_bpp / 8;
	stride = (stride + 7) & 0xFFFFFFF8; /* pad to 64-bit boundary */

	if(ovly == NULL){
		/* dss_enable_overlay */
		ovly = mcde_ovly_get(chnl);
		if (IS_ERR(ovly)) {
			err = PTR_ERR(ovly);
			printf("%s: Failed to get channel\n", __func__);
			return -err;
		}
	}

	mcde_ovly_set_source_buf(ovly, (u32) paddr);
	mcde_ovly_set_source_info(ovly, stride,main_display.default_pixel_format);
	mcde_ovly_set_source_area(ovly, 0, 0, width, height);

	if (width == main_display.native_x_res)
		xpos = 0;
	else
		xpos = (main_display.native_x_res - width) / 2;

	if (height == main_display.native_y_res)
		ypos = 0;
	else
		ypos = (main_display.native_y_res - height) / 2;
	mcde_ovly_set_dest_pos(ovly, xpos, ypos, 0);
	mcde_ovly_apply(ovly);

	rect.x = 0;
	rect.y = 0;
	rect.w = main_display.native_x_res;
	rect.h = main_display.native_y_res;

	mcde_chnl_update(chnl,&rect);

	return 0;
}

