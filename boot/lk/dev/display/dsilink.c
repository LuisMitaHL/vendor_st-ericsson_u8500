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
#include <err.h>
#include "target_config.h"
#include "mcde_kcompat.h"
#include "mcde_device.h"
#include "mcde_regs.h"
#include "mcde.h"
#include "reg.h"
#include "dbx500-prcmu-regs.h"

#include "nova_dsilink.h"

#include "dsilink_regs.h"
#include "dsilink.h"

#define CLK_DSI_SYS	"dsisys"
#define CLK_DSI_LP	"dsilp"
#define CLK_DSI_HS	"dsihs"
#define REG_EPOD_DSS	"vsupply"
#define REG_VANA	"vdddsi1v2"

#define DSILINK_VERSION_3 0x02327820
#define DSILINK_VERSION_2_1 0x02327568
#define DSILINK_VERSION_2_0 0x02327457

/* TODO Check how long these timeouts should be */
#define DSI_TE_NO_ANSWER_TIMEOUT_INIT_MS 2500
#define DSI_TE_NO_ANSWER_TIMEOUT_MS 250

#define clk_round_rate(c, f) (f)

static struct dsilink_device *dsilinks[MAX_NBR_OF_DSILINKS];

u32 _drvdata = 0;
static void platform_set_drvdata(void *dev, u32 data)
{
	_drvdata = data;
}

static void set_link_reset(u8 link)
{
	u32 value;

	value = prcmu_read(PRCM_DSI_SW_RESET);
	switch (link) {
	case 0:
		value &= ~DB8500_PRCM_DSI_SW_RESET_DSI0_SW_RESETN;
		break;
	case 1:
		value &= ~DB8500_PRCM_DSI_SW_RESET_DSI1_SW_RESETN;
		break;
	case 2:
		value &= ~DB8500_PRCM_DSI_SW_RESET_DSI2_SW_RESETN;
		break;
	default:
		break;
	}
	prcmu_write(PRCM_DSI_SW_RESET, value);
}

static void release_link_reset(u8 link)
{
	u32 value;

	value = prcmu_read(PRCM_DSI_SW_RESET);
	switch (link) {
	case 0:
		value |= DB8500_PRCM_DSI_SW_RESET_DSI0_SW_RESETN;
		break;
	case 1:
		value |= DB8500_PRCM_DSI_SW_RESET_DSI1_SW_RESETN;
		break;
	case 2:
		value |= DB8500_PRCM_DSI_SW_RESET_DSI2_SW_RESETN;
		break;
	default:
		break;
	}
	prcmu_write(PRCM_DSI_SW_RESET, value);
}

static void disable_clocks_and_power(struct dsilink_device *dsilink)
{
	clk_disable(dsilink->clk_dsi_hs);
	clk_disable(dsilink->clk_dsi_lp);
	clk_disable(dsilink->clk_dsi_sys);
	regulator_disable(dsilink->reg_epod_dss);
	if (dsilink->reg_vana)
		regulator_disable(dsilink->reg_vana);
}

int nova_dsilink_setup(struct dsilink_device *dsilink,
						const struct dsilink_port *port)
{
	if (!dsilink->reserved) {
		dev_err(dsilink->dev, "%s: EINVAL", __func__);
		return -EINVAL;
	}

	DSILINK_TRACE(dsilink->dev);

	dsilink->port = *port;

	if (port->mode == DSILINK_MODE_VID) {
		switch (port->phy.vid_mode) {
		case DSILINK_NON_BURST_MODE_WITH_SYNC_EVENT:
			dsilink->vid_regs.burst_mode = false;
			dsilink->vid_regs.sync_is_pulse = false;
			dsilink->vid_regs.tvg_enable = false;
			break;
		case DSILINK_NON_BURST_MODE_WITH_SYNC_EVENT_TVG_ENABLED:
			dsilink->vid_regs.burst_mode = false;
			dsilink->vid_regs.sync_is_pulse = false;
			dsilink->vid_regs.tvg_enable = true;
			break;
		case DSILINK_BURST_MODE_WITH_SYNC_EVENT:
			dsilink->vid_regs.burst_mode = true;
			dsilink->vid_regs.sync_is_pulse = false;
			dsilink->vid_regs.tvg_enable = false;
			break;
		case DSILINK_BURST_MODE_WITH_SYNC_PULSE:
			dsilink->vid_regs.burst_mode = true;
			dsilink->vid_regs.sync_is_pulse = true;
			dsilink->vid_regs.tvg_enable = false;
			break;
		default:
			dev_err(dsilink->dev, "Unsupported video mode");
			goto error;
		}
	}

	return 0;

error:
	return -EINVAL;
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
int nova_dsilink_send_max_read_len(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	return dsilink->ops.write(dsilink->io, dsilink->dev,
					DSILINK_CMD_SET_MAX_PKT_SIZE,
						DSILINK_MAX_DCS_READ, NULL, 1);
}

/*
 * Wait for CSM_RUNNING, all data sent for display
 */
void nova_dsilink_wait_while_running(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return;

	DSILINK_TRACE(dsilink->dev);

	dsilink->ops.wait_while_running(dsilink->io, dsilink->dev);
}

u8 nova_dsilink_handle_irq(struct dsilink_device *dsilink)
{
	u8 ret = 0;

	if (!dsilink->enabled)
		return ret;

	ret = dsilink->ops.handle_irq(dsilink->io);

	if (ret & DSILINK_IRQ_BTA_TE) {
		dev_vdbg(dsilink->dev, "BTA TE DSI\n");
	} else if (ret & DSILINK_IRQ_NO_TE) {
		dev_warn(dsilink->dev, "NO_TE DSI\n");
	} else if (ret & DSILINK_IRQ_TRIGGER) {
		dev_vdbg(dsilink->dev, "TRIGGER\n");
	}

	return ret;
}

int nova_dsilink_dcs_write(struct dsilink_device *dsilink, u8 cmd,
							u8 *data, int len)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	return dsilink->ops.write(dsilink->io, dsilink->dev,
							DSILINK_CMD_DCS_WRITE,
								cmd, data, len);
}

int nova_dsilink_dsi_write(struct dsilink_device *dsilink, u8 cmd,
							u8 *data, int len)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	return dsilink->ops.write(dsilink->io, dsilink->dev,
						DSILINK_CMD_GENERIC_WRITE,
								cmd, data, len);
}

int nova_dsilink_dsi_read(struct dsilink_device *dsilink,
						u8 cmd, u32 *data, int *len)
{
	int ret;
	if (WARN_ON_ONCE(!dsilink->reserved))
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	ret = dsilink->ops.read(dsilink->io, dsilink->dev, cmd, data, len);
	if (ret == -EAGAIN)
		dsilink->ops.force_stop(dsilink->io);

	return ret;
}

void nova_dsilink_te_request(struct dsilink_device *dsilink)
{
	if (WARN_ON_ONCE(!dsilink->reserved))
		return;

	DSILINK_TRACE(dsilink->dev);
	dsilink->ops.te_request(dsilink->io);
}

int nova_dsilink_enable(struct dsilink_device *dsilink)
{
	if (dsilink->enabled || !dsilink->reserved)
		return -EBUSY;

	DSILINK_TRACE(dsilink->dev);

	if (dsilink->reg_vana)
		regulator_enable(dsilink->reg_vana);

	regulator_enable(dsilink->reg_epod_dss);

	if (dsilink->update_dsi_freq) {
		u32 dsi_hs_freq;
		u32 ui_value;
		dsi_hs_freq = clk_round_rate(
				dsilink->clk_dsi_hs, dsilink->port.phy.hs_freq);

		if (dsilink->port.phy.hs_freq != dsi_hs_freq)
			dev_dbg(dsilink->dev, "Could not set dsi hs freq"
				" to %u got %u\n",
					dsilink->port.phy.hs_freq, dsi_hs_freq);
		/*
		 * If this warn on is triggered it could be that
		 * the dsilink->clk_dsi_hs already is enabled
		 */
		WARN_ON_ONCE(clk_set_rate(dsilink->clk_dsi_hs,
						dsilink->port.phy.hs_freq));
		/* if a display driver has set a value this should be used */
		if (dsilink->port.phy.ui == 0) {
			/* Calulate in MHz instead */
			dsi_hs_freq = dsi_hs_freq / 1000000;
			ui_value = 4000 / dsi_hs_freq;
			dsilink->port.phy.ui = (u8) ui_value;
			dev_dbg(dsilink->dev,
					"ui value %u dsi_hs_freq %u in MHz\n",
					dsilink->port.phy.ui, dsi_hs_freq);
		}
	}

	clk_enable(dsilink->clk_dsi_sys);
	clk_enable(dsilink->clk_dsi_hs);
	clk_enable(dsilink->clk_dsi_lp);

	release_link_reset(dsilink->port.link);

	if (dsilink->ops.enable(dsilink->io, dsilink->dev, &dsilink->port,
							&dsilink->vid_regs))
		goto error;

	dsilink->enabled = true;
	dev_dbg(dsilink->dev, "%s link enabled\n", __func__);
	return 0;

error:
	dev_warn(dsilink->dev, "%s enable failed\n", __func__);
	disable_clocks_and_power(dsilink);
	return -EINVAL;
}

void nova_dsilink_disable(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return;

	DSILINK_TRACE(dsilink->dev);

	dsilink->ops.disable(dsilink->io);

	disable_clocks_and_power(dsilink);

	dsilink->enabled = false;

	dev_dbg(dsilink->dev, "link disabled\n");
}

/*
 * The rgb_header identifies the pixel stream format,
 * as described in the MIPI DSI Specification:
 *
 * 0x0E: Packed pixel stream, 16-bit RGB, 565 format
 * 0x1E: Packed pixel stream, 18-bit RGB, 666 format
 * 0x2E: Loosely Packed pixel stream, 18-bit RGB, 666 format
 * 0x3E: Packed pixel stream, 24-bit RGB, 888 format
 */
int nova_dsilink_update_frame_parameters(struct dsilink_device *dsilink,
				struct dsilink_video_mode *vmode, u8 bpp,
						u8 pixel_mode, u8 rgb_header)
{
	u32 hs_byte_clk = 0;
	u32 pck_len = 0;
	u32 blkline_pck, line_duration;
	u32 blkeol_pck, blkeol_duration;
	int hfp = 0, hbp = 0, hsa = 0;

	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	if (dsilink->port.mode == DSILINK_MODE_CMD)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	dsilink->vid_regs.pixel_mode = pixel_mode;
	dsilink->vid_regs.rgb_header = rgb_header;
	/*
	 * vmode->hfp, vmode->hbp and vmode->hsw are given in pixels
	 * and must be re-calculated into bytes
	 *
	 * 6 + 2 is HFP header + checksum
	 */
	hfp = vmode->hfp * bpp - 6 - 2;
	if (dsilink->vid_regs.sync_is_pulse) {
		/*
		 * 6 is HBP header + checksum
		 * 4 is RGB header + checksum
		 */
		hbp = vmode->hbp * bpp - 4 - 6;
		/*
		 * 6 is HBP header + checksum
		 * 4 is HSW packet bytes
		 * 4 is RGB header + checksum
		 */
		hsa = vmode->hsw * bpp - 4 - 4 - 6;
	} else {
		/*
		 * 6 is HBP header + checksum
		 * 4 is HSW packet bytes
		 * 4 is RGB header + checksum
		 */
		hbp = (vmode->hbp + vmode->hsw) * bpp - 4 - 4 - 6;
		/* HSA is not considered in this mode and set to 0 */
		hsa = 0;
	}
	if (hfp < 0) {
		hfp = 0;
		dev_warn(dsilink->dev,
			"%s: negative calc for hfp, set to 0\n", __func__);
	}
	if (hbp < 0) {
		hbp = 0;
		dev_warn(dsilink->dev,
			"%s: negative calc for hbp, set to 0\n", __func__);
	}
	if (hsa < 0) {
		hsa = 0;
		dev_warn(dsilink->dev,
			"%s: negative calc for hsa, set to 0\n", __func__);
	}

	/*
	 * vmode->pixclock is the time between two pixels (in picoseconds)
	 *
	 * hs_byte_clk is the amount of transferred bytes per lane and
	 * second (in MHz)
	 */
	hs_byte_clk = 1000000 / vmode->pixclock / 8;
	pck_len = 1000000 * hs_byte_clk / dsilink->port.refresh_rate /
			(vmode->vsw + vmode->vbp + vmode->yres + vmode->vfp) *
					dsilink->port.phy.num_data_lanes;
	/*
	 * 6 is header + checksum, header = 4 bytes, checksum = 2 bytes
	 * 4 is short packet for vsync/hsync
	 */
	if (dsilink->vid_regs.sync_is_pulse)
		blkline_pck = pck_len - vmode->hsw - 6;
	else
		blkline_pck = pck_len - 4 - 6;

	line_duration = (blkline_pck + 6) / dsilink->port.phy.num_data_lanes;
	blkeol_pck = pck_len -
		(vmode->hsw + vmode->hbp + vmode->xres + vmode->hfp) * bpp - 6;
	/* safety */
	if ((int)blkeol_pck < 0) {
		dev_warn(dsilink->dev, "%s: blkeol_pck set to 0\n", __func__);
		blkeol_pck = 0;
	}

	blkeol_duration = (blkeol_pck + 6) / dsilink->port.phy.num_data_lanes;

	dsilink->vid_regs.hfp = hfp;
	dsilink->vid_regs.hbp = hbp;
	dsilink->vid_regs.hsa = hsa;
	dsilink->vid_regs.blkline_pck = blkline_pck;
	dsilink->vid_regs.line_duration = line_duration;
	dsilink->vid_regs.blkeol_pck = blkeol_pck;
	dsilink->vid_regs.blkeol_duration = blkeol_duration;

	return dsilink->ops.update_frame_parameters(dsilink->io, vmode,
						&dsilink->vid_regs, bpp);
}

void nova_dsilink_set_clk_continous(struct dsilink_device *dsilink, bool on)
{
	bool value = false;

	if (!dsilink->reserved)
		return;

	DSILINK_TRACE(dsilink->dev);

	if (on)
		value = dsilink->port.phy.clk_cont;

	dsilink->ops.set_clk_continous(dsilink->io, value);
}

int nova_dsilink_enter_ulpm(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	return 0;
}

int nova_dsilink_exit_ulpm(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	return 0;
}

int nova_dsilink_force_stop(struct dsilink_device *dsilink)
{
	if (!dsilink->enabled || !dsilink->reserved)
		return -EINVAL;

	DSILINK_TRACE(dsilink->dev);

	dsilink->ops.force_stop(dsilink->io);

	return 0;
}

struct dsilink_device *nova_dsilink_get(int dev_idx)
{
	struct dsilink_device *dsilink = NULL;

	if (dev_idx >= MAX_NBR_OF_DSILINKS)
		return ERR_PTR(-EINVAL);

	dsilink = dsilinks[dev_idx];

	DSILINK_TRACE(dsilink->dev);

	if (dsilink == NULL)
		return ERR_PTR(-EINVAL);

	if (dsilink->reserved)
		return ERR_PTR(-EBUSY);

	dsilink->reserved = true;

	return dsilink;
}

void nova_dsilink_put(struct dsilink_device *dsilink)
{
	DSILINK_TRACE(dsilink->dev);

	if (dsilink->enabled)
		nova_dsilink_disable(dsilink);

	dsilink->reserved = false;
}

static int __devinit nova_dsilink_probe(struct platform_device *pdev)
{
	struct dsilink_device *dsilink;
	int ret = 0;

	dsilink = (struct dsilink_device *)kzalloc(sizeof(struct dsilink_device), GFP_KERNEL);
	if (!dsilink)
		return -ENOMEM;

	if (pdev->id >= MAX_NBR_OF_DSILINKS) {
		ret = -EINVAL;
		goto failed_to_get_correct_id;
	}

	dsilink->dev = &pdev->dev;

	dsilinks[pdev->id] = dsilink;

	dsilink->io = (u8 *)(CFG_DSI_BASE + pdev->id * 0x1000);

	dev_info(&pdev->dev, "dsio[%d]: 0x%08X\n",
						pdev->id, (u32)dsilink->io);

	clk_enable(dsilink->clk_dsi_sys);
	clk_enable(dsilink->clk_dsi_hs);
	clk_enable(dsilink->clk_dsi_lp);
	release_link_reset(pdev->id);

	dsilink->version = dsi_rreg(dsilink->io, DSI_ID_REG);

	dev_info(dsilink->dev, "hw revision 0x%08X\n", dsilink->version);

	set_link_reset(pdev->id);
	clk_disable(dsilink->clk_dsi_hs);
	clk_disable(dsilink->clk_dsi_lp);
	clk_disable(dsilink->clk_dsi_sys);
	regulator_disable(dsilink->reg_epod_dss);
	if (dsilink->reg_vana)
		regulator_disable(dsilink->reg_vana);
	if (dsilink->version == DSILINK_VERSION_3)
		nova_dsilink_v3_init(&dsilink->ops);
	else if (dsilink->version == DSILINK_VERSION_2_1 ||
				dsilink->version == DSILINK_VERSION_2_0)
		nova_dsilink_v2_init(&dsilink->ops);
	else
		goto failed_wrong_dsilink_version;

	platform_set_drvdata(pdev, (u32)dsilink);

	DSILINK_TRACE(dsilink->dev);

	return 0;

failed_wrong_dsilink_version:
	dev_warn(dsilink->dev, "Unsupported dsilink version\n");
failed_to_get_correct_id:
	kfree(dsilink);
	return ret;
}

static struct platform_driver dsilink_driver = {
	.driver = {
		.name = "dsilink",
	},
	.probe	= nova_dsilink_probe,
};

struct platform_device u8500_dsilink_device[] = {
	[0] = {
		.name = "dsilink",
		.id = 0,
	},
	[1] = {
		.name = "dsilink",
		.id = 1,
	},
	[2] = {
		.name = "dsilink",
		.id = 2,
	},
};

int dsilink_init(void)
{
	int ret;
	int i;

	for (i = 0; i < 3; i++) {
		dev_dbg(dsilink->dev, "%s: Probing dsilink device[%d]\n",
								__func__, i);
		ret = dsilink_driver.probe(&u8500_dsilink_device[i]);
		if (ret)
			dev_err(dsilink->dev,
				"%s: Failed probe dsilink device[%d] ret=%d\n",
							__func__, i, ret);
	}

	return ret;
}
