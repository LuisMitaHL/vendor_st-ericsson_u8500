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

/*
 * Wait for CSM_RUNNING, all data sent for display
 */
static void wait_while_running(u8 *io, struct device *dev)
{
	u8 counter = DSI_READ_TIMEOUT_MS;

	while (dsi_rfld(io, DSI_CMD_MODE_STS, CSM_RUNNING) && --counter)
		udelay(DSI_READ_DELAY_US);
	WARN_ON(!counter);
	if (!counter)
		dev_warn(dev, "%s: read timeout!\n", __func__);
}

static u8 handle_irq(u8 *io)
{
	u32 irq_status;
	u8 ret = 0;

	irq_status = dsi_rreg(io, DSI_DIRECT_CMD_STS_FLAG);
	if (irq_status & DSI_DIRECT_CMD_STS_FLAG_TE_RECEIVED_FLAG_MASK)
		ret |= DSILINK_IRQ_BTA_TE;

	if (irq_status & DSI_DIRECT_CMD_STS_FLAG_TRIGGER_RECEIVED_FLAG_MASK)
		/* DSI TE polling answer received */
		ret |= DSILINK_IRQ_TRIGGER;

	if (irq_status &
		DSI_DIRECT_CMD_STS_FLAG_ACKNOWLEDGE_WITH_ERR_RECEIVED_FLAG_MASK)
		ret |= DSILINK_IRQ_ACK_WITH_ERR;

	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR, irq_status);

	irq_status = dsi_rreg(io, DSI_CMD_MODE_STS_FLAG);
	if (irq_status & DSI_CMD_MODE_STS_FLAG_ERR_NO_TE_FLAG_MASK)
		ret |= DSILINK_IRQ_NO_TE;

	if (irq_status & DSI_CMD_MODE_STS_FLAG_ERR_TE_MISS_FLAG_MASK)
		ret |= DSILINK_IRQ_TE_MISS;

	dsi_wreg(io, DSI_CMD_MODE_STS_CLR, irq_status);

	return ret;
}

static int write(u8 *io, struct device *dev, enum dsilink_cmd_datatype type,
						u8 cmd, u8 *data, int len)
{
	int i, ret = 0;
	u32 wrdat[4] = { 0, 0, 0, 0 };
	u32 settings;
	u32 counter = DSI_WRITE_CMD_TIMEOUT_MS;

	if (len > DSILINK_MAX_DSI_DIRECT_CMD_WRITE)
		return -EINVAL;

	if (type == DSILINK_CMD_DCS_WRITE) {
		wrdat[0] = cmd;
		for (i = 1; i <= len; i++)
			wrdat[i>>2] |= ((u32)data[i-1] << ((i & 3) * 8));
	} else if (type == DSILINK_CMD_GENERIC_WRITE) {
		/* no explicit cmd byte for generic_write, only params */
		for (i = 0; i < len; i++)
			wrdat[i>>2] |= ((u32)data[i] << ((i & 3) * 8));
	} else if (type == DSILINK_CMD_SET_MAX_PKT_SIZE) {
		wrdat[0] = cmd;
	}

	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_NAT_ENUM(WRITE) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LONGNOTSHORT(len > 1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_SIZE(len+1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LP_EN(true);
	if (type == DSILINK_CMD_DCS_WRITE) {
		if (len == 0)
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				DCS_SHORT_WRITE_0);
		else if (len == 1)
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				DCS_SHORT_WRITE_1);
		else
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				DCS_LONG_WRITE);
	} else if (type == DSILINK_CMD_GENERIC_WRITE) {
		if (len == 0)
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				GENERIC_SHORT_WRITE_0);
		else if (len == 1)
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				GENERIC_SHORT_WRITE_1);
		else if (len == 2)
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				GENERIC_SHORT_WRITE_2);
		else
			settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
				GENERIC_LONG_WRITE);
	} else if (type == DSILINK_CMD_SET_MAX_PKT_SIZE) {
		settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(
							SET_MAX_PKT_SIZE);
	}

	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS, settings);
	dsi_wreg(io, DSI_DIRECT_CMD_WRDAT0, wrdat[0]);
	if (len >  3)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT1, wrdat[1]);
	if (len >  7)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT2, wrdat[2]);
	if (len > 11)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT3, wrdat[3]);

	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR, ~0);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR, ~0);
	dsi_wreg(io, DSI_DIRECT_CMD_SEND, true);

	/* loop will normally run zero or one time until WRITE_COMPLETED */
	while (!dsi_rfld(io, DSI_DIRECT_CMD_STS, WRITE_COMPLETED)
			&& --counter)
		cpu_relax();

	if (!counter) {
		dev_err(dev,
			"%s: DSI write cmd 0x%x timeout!\n",
			__func__, cmd);
		ret = -ETIME;
	} else {
		/* inform if >100 loops before command completion */
		if (counter <
			(DSI_WRITE_CMD_TIMEOUT_MS-DSI_WRITE_CMD_TIMEOUT_MS/10))
			dev_vdbg(dev,
				"%s: %u loops for DSI command %x completion\n",
				__func__, (DSI_WRITE_CMD_TIMEOUT_MS - counter),
				cmd);

		dev_vdbg(dev, "DSI Write ok 0x%x error 0x%x\n",
			dsi_rreg(io, DSI_DIRECT_CMD_STS_FLAG),
			dsi_rreg(io, DSI_CMD_MODE_STS_FLAG));
	}

	return ret;
}

static void te_request(u8 *io)
{
	u32 settings;

	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_NAT_ENUM(TE_REQ) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LONGNOTSHORT(false) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_SIZE(2) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LP_EN(true) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(DCS_SHORT_WRITE_1);
	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS, settings);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR,
		DSI_DIRECT_CMD_STS_CLR_TE_RECEIVED_CLR(true));
	dsi_wfld(io, DSI_DIRECT_CMD_STS_CTL, TE_RECEIVED_EN, true);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR,
		DSI_DIRECT_CMD_STS_CLR_ACKNOWLEDGE_WITH_ERR_RECEIVED_CLR(true));
	dsi_wfld(io, DSI_DIRECT_CMD_STS_CTL, ACKNOWLEDGE_WITH_ERR_EN, true);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR,
		DSI_CMD_MODE_STS_CLR_ERR_NO_TE_CLR(true));
	dsi_wfld(io, DSI_CMD_MODE_STS_CTL, ERR_NO_TE_EN, true);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR,
		DSI_CMD_MODE_STS_CLR_ERR_TE_MISS_CLR(true));
	dsi_wfld(io, DSI_CMD_MODE_STS_CTL, ERR_TE_MISS_EN, true);
	dsi_wreg(io, DSI_DIRECT_CMD_SEND, true);
}

static int read(u8 *io, struct device *dev, u8 cmd, u32 *data, int *len)
{
	int ret = 0;
	u32 settings;
	bool error = false, ok = false;
	bool ack_with_err = false;
	u8 nbr_of_retries = DSI_READ_NBR_OF_RETRIES;

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, BTA_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, READ_EN, true);
	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_NAT_ENUM(READ) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LONGNOTSHORT(false) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_SIZE(1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_LP_EN(true) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_CMD_HEAD_ENUM(DCS_READ);
	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS, settings);

	dsi_wreg(io, DSI_DIRECT_CMD_WRDAT0, cmd);

	do {
		u8 wait  = DSI_READ_TIMEOUT_MS;
		dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR, ~0);
		dsi_wreg(io, DSI_DIRECT_CMD_RD_STS_CLR, ~0);
		dsi_wreg(io, DSI_DIRECT_CMD_SEND, true);

		while (wait-- && !(error = dsi_rfld(io, DSI_DIRECT_CMD_STS,
					READ_COMPLETED_WITH_ERR)) &&
				!(ok = dsi_rfld(io, DSI_DIRECT_CMD_STS,
							READ_COMPLETED)))
			udelay(DSI_READ_DELAY_US);

		ack_with_err = dsi_rfld(io, DSI_DIRECT_CMD_STS,
						ACKNOWLEDGE_WITH_ERR_RECEIVED);
		if (ack_with_err)
			dev_warn(dev,
					"DCS Acknowledge Error Report %.4X\n",
				dsi_rfld(io, DSI_DIRECT_CMD_STS, ACK_VAL));
	} while (--nbr_of_retries && ack_with_err);

	if (ok) {
		int rdsize;
		u32 rddat;

		rdsize = dsi_rfld(io, DSI_DIRECT_CMD_RD_PROPERTY, RD_SIZE);
		rddat = dsi_rreg(io, DSI_DIRECT_CMD_RDDAT);
		if (rdsize < *len) {
			ret = -EINVAL;
		} else {
			*len = min(*len, rdsize);
			memcpy(data, &rddat, *len);
		}
	} else {
		u8 dat1_status;
		u32 sts;

		sts = dsi_rreg(io, DSI_DIRECT_CMD_STS);
		dat1_status = dsi_rfld(io, DSI_MCTL_LANE_STS, DATLANE1_STATE);
		dev_err(dev, "DCS read failed, err=%d, D0 state %d sts %X\n",
						error, dat1_status, sts);
		dsi_wreg(io, DSI_DIRECT_CMD_RD_INIT, true);
		/* If dat1 is still in read to a force stop */
		if (dat1_status == DSILINK_LANE_STATE_READ ||
						sts == DSI_CMD_TRANSMISSION)
			ret = -EAGAIN;
		else
			ret = -EIO;
	}

	dsi_wreg(io, DSI_CMD_MODE_STS_CLR, ~0);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR, ~0);

	return ret;
}

static void force_stop(u8 *io)
{
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL, FORCE_STOP_MODE, true);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL, CLOCK_FORCE_STOP_MODE, true);
	udelay(20);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL, FORCE_STOP_MODE, false);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL, CLOCK_FORCE_STOP_MODE, false);
}

static int enable(u8 *io, struct device *dev, const struct dsilink_port *port,
				struct dsilink_dsi_vid_registers *vid_regs)
{
	int i = 0;

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, LINK_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, BTA_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, READ_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, REG_TE_EN, true);

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, HOST_EOT_GEN,
						port->phy.host_eot_gen);
	dsi_wfld(io, DSI_CMD_MODE_CTL, TE_TIMEOUT, 0x3FF);

	if (port->sync_src == DSILINK_SYNCSRC_TE_POLLING) {
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, IF1_TE_EN, true);
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, TE_POLLING_EN, true);
	}

	dsi_wreg(io, DSI_MCTL_DPHY_STATIC,
		DSI_MCTL_DPHY_STATIC_UI_X4(port->phy.ui));

	dsi_wreg(io, DSI_MCTL_MAIN_PHY_CTL,
		DSI_MCTL_MAIN_PHY_CTL_WAIT_BURST_TIME(0xf) |
		DSI_MCTL_MAIN_PHY_CTL_LANE2_EN(
			port->phy.num_data_lanes >= 2) |
		DSI_MCTL_MAIN_PHY_CTL_CLK_ULPM_EN(true) |
		DSI_MCTL_MAIN_PHY_CTL_DAT1_ULPM_EN(true) |
		DSI_MCTL_MAIN_PHY_CTL_DAT2_ULPM_EN(true) |
		DSI_MCTL_MAIN_PHY_CTL_CLK_CONTINUOUS(
			false));
	dsi_wreg(io, DSI_MCTL_ULPOUT_TIME,
		DSI_MCTL_ULPOUT_TIME_CKLANE_ULPOUT_TIME(1) |
		DSI_MCTL_ULPOUT_TIME_DATA_ULPOUT_TIME(1));
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, DLX_REMAP_EN,
				port->phy.data_lanes_swap);
	dsi_wreg(io, DSI_DPHY_LANES_TRIM,
		DSI_DPHY_LANES_TRIM_DPHY_SPECS_90_81B_ENUM(0_90));
	dsi_wreg(io, DSI_MCTL_DPHY_TIMEOUT,
		DSI_MCTL_DPHY_TIMEOUT_CLK_DIV(0xf) |
		DSI_MCTL_DPHY_TIMEOUT_HSTX_TO_VAL(0x3fff) |
		DSI_MCTL_DPHY_TIMEOUT_LPRX_TO_VAL(0x3fff));
	dsi_wfld(io, DSI_CMD_MODE_CTL, ARB_MODE, false);
	dsi_wfld(io, DSI_CMD_MODE_CTL, ARB_PRI, false);
	dsi_wreg(io, DSI_MCTL_MAIN_EN,
		DSI_MCTL_MAIN_EN_PLL_START(true) |
		DSI_MCTL_MAIN_EN_CKLANE_EN(true) |
		DSI_MCTL_MAIN_EN_DAT1_EN(true) |
		DSI_MCTL_MAIN_EN_DAT2_EN(port->phy.num_data_lanes == 2) |
		DSI_MCTL_MAIN_EN_IF1_EN(true) |
		DSI_MCTL_MAIN_EN_IF2_EN(false));

	while (dsi_rfld(io, DSI_MCTL_MAIN_STS, CLKLANE_READY) == 0 ||
		dsi_rfld(io, DSI_MCTL_MAIN_STS, DAT1_READY) ==
						DSILINK_LANE_STATE_START ||
		(dsi_rfld(io, DSI_MCTL_MAIN_STS, DAT2_READY) ==
					DSILINK_LANE_STATE_START &&
						port->phy.num_data_lanes > 1)) {
		usleep_range(1000, 1000);
		if (i++ == 10) {
			dev_warn(dev, "DSI lane not ready! (v2)\n");
			return -EINVAL;
		}
	}

	if (port->mode == DSILINK_MODE_VID) {
		/* burst mode or non-burst mode */
		dsi_wfld(io, DSI_VID_MAIN_CTL, BURST_MODE,
							vid_regs->burst_mode);

		/* sync is pulse or event */
		dsi_wfld(io, DSI_VID_MAIN_CTL, SYNC_PULSE_ACTIVE,
						vid_regs->sync_is_pulse);
		dsi_wfld(io, DSI_VID_MAIN_CTL, SYNC_PULSE_HORIZONTAL,
						vid_regs->sync_is_pulse);

		/* disable video stream when using TVG */
		if (vid_regs->tvg_enable) {
			dsi_wfld(io, DSI_MCTL_MAIN_EN, IF1_EN, false);
			dsi_wfld(io, DSI_MCTL_MAIN_EN, IF2_EN, false);
		}

		/*
		 * behavior during blanking time
		 * 00: NULL packet 1x:LP 01:blanking-packet
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL, REG_BLKLINE_MODE, 1);

		/*
		 * behavior during eol
		 * 00: NULL packet 1x:LP 01:blanking-packet
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL, REG_BLKEOL_MODE, 2);

		/* time to perform LP->HS on D-PHY */
		dsi_wfld(io, DSI_VID_DPHY_TIME, REG_WAKEUP_TIME,
						port->phy.vid_wakeup_time);

		/*
		 * video stream starts on VSYNC packet
		 * and stops at the end of a frame
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL, VID_ID, 0);
		dsi_wfld(io, DSI_VID_MAIN_CTL, START_MODE, 0);
		dsi_wfld(io, DSI_VID_MAIN_CTL, STOP_MODE, 0);

		/* 1: if1 in video mode, 0: if1 in command mode */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, IF1_MODE, 1);

		/* 1: enables the link, 0: disables the link */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, VID_EN, 1);
	} else {
		dsi_wfld(io, DSI_CMD_MODE_CTL, IF1_ID, 0);
	}
	return 0;
}

static void disable(u8 *io)
{
}

static int update_frame_parameters(u8 *io, struct dsilink_video_mode *vmode,
				struct dsilink_dsi_vid_registers *vid_regs,
									u8 bpp)
{
	dsi_wfld(io, DSI_VID_VSIZE, VFP_LENGTH, vmode->vfp);
	dsi_wfld(io, DSI_VID_VSIZE, VBP_LENGTH, vmode->vbp);
	dsi_wfld(io, DSI_VID_VSIZE, VSA_LENGTH, vmode->vsw);
	dsi_wfld(io, DSI_VID_HSIZE1, HFP_LENGTH, vid_regs->hfp);
	dsi_wfld(io, DSI_VID_HSIZE1, HBP_LENGTH, vid_regs->hbp);
	dsi_wfld(io, DSI_VID_HSIZE1, HSA_LENGTH, vid_regs->hsa);
	dsi_wfld(io, DSI_VID_VSIZE, VACT_LENGTH, vmode->yres);
	dsi_wfld(io, DSI_VID_HSIZE2, RGB_SIZE, vmode->xres * bpp);
	dsi_wfld(io, DSI_VID_MAIN_CTL, VID_PIXEL_MODE, vid_regs->pixel_mode);
	dsi_wfld(io, DSI_VID_MAIN_CTL, HEADER, vid_regs->rgb_header);

	if (vid_regs->tvg_enable) {
		/*
		 * with these settings, expect to see 64 pixels wide
		 * red and green vertical stripes on the screen when
		 * tvg_enable = 1
		 */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL, TVG_SEL, 1);

		dsi_wfld(io, DSI_TVG_CTL, TVG_STRIPE_SIZE, 6);
		dsi_wfld(io, DSI_TVG_CTL, TVG_MODE, 2);
		dsi_wfld(io, DSI_TVG_CTL, TVG_STOPMODE, 2);
		dsi_wfld(io, DSI_TVG_CTL, TVG_RUN, 1);

		dsi_wfld(io, DSI_TVG_IMG_SIZE, TVG_NBLINE, vmode->yres);
		dsi_wfld(io, DSI_TVG_IMG_SIZE, TVG_LINE_SIZE,
							vmode->xres * bpp);
		dsi_wfld(io, DSI_TVG_COLOR1, COL1_BLUE, 0);
		dsi_wfld(io, DSI_TVG_COLOR1, COL1_GREEN, 0);
		dsi_wfld(io, DSI_TVG_COLOR1, COL1_RED, 0xFF);

		dsi_wfld(io, DSI_TVG_COLOR2, COL2_BLUE, 0);
		dsi_wfld(io, DSI_TVG_COLOR2, COL2_GREEN, 0xFF);
		dsi_wfld(io, DSI_TVG_COLOR2, COL2_RED, 0);
	}
	if (vid_regs->sync_is_pulse)
		dsi_wfld(io, DSI_VID_BLKSIZE2, BLKLINE_PULSE_PCK,
							vid_regs->blkline_pck);
	else
		dsi_wfld(io, DSI_VID_BLKSIZE1, BLKLINE_EVENT_PCK,
							vid_regs->blkline_pck);
	dsi_wfld(io, DSI_VID_DPHY_TIME, REG_LINE_DURATION,
						vid_regs->line_duration);
	if (vid_regs->burst_mode) {
		dsi_wfld(io, DSI_VID_BLKSIZE1, BLKEOL_PCK,
							vid_regs->blkeol_pck);
		dsi_wfld(io, DSI_VID_PCK_TIME, BLKEOL_DURATION,
						vid_regs->blkeol_duration);
		dsi_wfld(io, DSI_VID_VCA_SETTING1, MAX_BURST_LIMIT,
						vid_regs->blkeol_pck - 6);
		dsi_wfld(io, DSI_VID_VCA_SETTING2, EXACT_BURST_LIMIT,
							vid_regs->blkeol_pck);
	}
	if (vid_regs->sync_is_pulse)
		dsi_wfld(io, DSI_VID_VCA_SETTING2, MAX_LINE_LIMIT,
						vid_regs->blkline_pck - 6);

	return 0;
}

static void set_clk_continous(u8 *io, bool value)
{
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL, CLK_CONTINUOUS, value);
}

void __init nova_dsilink_v2_init(struct dsilink_ops *ops)
{
	ops->force_stop = force_stop;
	ops->read = read;
	ops->write = write;
	ops->handle_irq = handle_irq;
	ops->wait_while_running = wait_while_running;
	ops->te_request = te_request;
	ops->enable = enable;
	ops->disable = disable;
	ops->update_frame_parameters = update_frame_parameters;
	ops->set_clk_continous = set_clk_continous;
	dev_info(dev, "%s: initiated\n", __func__);
}

