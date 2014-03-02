/*
* Copyright (C) ST-Ericsson SA 2011
*
* Author: Jimmy Rubin <jimmy.rubin@stericsson.com>
* for ST-Ericsson.
*
* License terms: GNU General Public License (GPL), version 2.
*/

#include <common.h>
#include <command.h>
#include <asm/arch/common.h>
#include "mcde_display.h"
#include "dsilink_regs.h"
#include "mcde_regs.h"
#include <malloc.h>
#include "mcde.h"
#include <linux/err.h>
#include <asm/arch/prcmu-db5500.h>

#define DCS_CMD_EXIT_SLEEP_MODE       0x11
#define DCS_CMD_SET_DISPLAY_ON        0x29
#define DCS_CMD_SET_TEAR_ON           0x35
#define DCS_CMD_SET_ADDRESS_MODE      0x36

int mcde_turn_on_display_dsi(void)
{
	int ret = 0;
	u8 m = 0;
	u8 param[15];

	debug("%s: enter\n", __func__);

	param[0] = 0xAA;
	ret = mcde_dsi_dcs_write(main_display.port, 1, 0xf3, param, 1);
	param[0] = 0x00;
	param[1] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0, 0, param, 3);
	param[0] = 0xC9;
	param[1] = 0x01;
	ret = mcde_dsi_dcs_write(main_display.port, 0, 0, param, 3);
	param[0] = 0xA2;
	param[1] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0, 0, param, 3);
	param[0] = 0xFF;
	param[1] = 0xAA;
	ret = mcde_dsi_dcs_write(main_display.port, 0, 0, param, 3);

	ret = mcde_dsi_dcs_write(main_display.port,
				1, DCS_CMD_EXIT_SLEEP_MODE, NULL, 0);
	 mdelay(1);
	if (!ret) {
		debug("mcde_dsi_dcs_write "
				"DCS_CMD_EXIT_SLEEP_MODE success!\n");

		ret = mcde_dsi_dcs_write(main_display.port,
					1, DCS_CMD_SET_TEAR_ON, &m, 1);
		mdelay(1);

		ret = mcde_dsi_dcs_write(main_display.port,
				1, DCS_CMD_SET_DISPLAY_ON,  NULL, 0);
		if (!ret) {
			debug("mcde_dsi_dcs_write "
				"DCS_CMD_SET_DISPLAY_ON success!\n");
			mdelay(1);
		}
	}
	return ret;
}

static int mcde_display_power_init(void)
{ /*place holder*/
	return 0;
}



int mcde_startup_dsi(struct mcde_platform_data *pdata)
{

	u8 num_dsilinks;
	int ret;

	debug("%s: enter\n", __func__);

	if (main_display.port->mode != MCDE_PORTMODE_CMD) {
		printf("%s: only CMD mode supported\n", __func__);
		return -ENODEV;
	}
	num_dsilinks = main_display.port->phy.dsi.num_data_lanes;
	mcde_init();
	ret = mcde_probe(num_dsilinks, pdata);
	if (ret) {
		printf("%s: mcde_probe() -> %d\n", __func__, ret);
		return ret;
	}
	ret = mcde_display_power_init();
	if (ret) {
		printf("%s: mcde_display_power_init() -> %d\n", __func__, ret);
		return ret;
	}

	ret = board_mcde_display_reset();
	if (ret) {
		printf("%s: board_mcde_display_reset() -> %d\n", __func__, ret);
		return ret;
	}

	return 0;
}
