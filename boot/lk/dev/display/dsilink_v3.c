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

#include "dsilink_regs_v3.h"
#include "dsilink.h"

/*
 * Wait for CSM_RUNNING, all data sent for display
 */
static void wait_while_running(u8 *io, struct device *dev)
{
	u8 counter = DSI_READ_TIMEOUT_MS;

	while (dsi_rfld(io, DSI_CMD_MODE_STS_V3, CSM_RUNNING) && --counter)
		udelay(DSI_READ_DELAY_US);
	WARN_ON(!counter);
	if (!counter)
		dev_warn(dev, "%s: read timeout!\n", __func__);
}

static u8 handle_irq(u8 *io)
{
	u32 irq_status;
	u8 ret = 0;

	irq_status = dsi_rreg(io, DSI_DIRECT_CMD_STS_FLAG_V3);
	if (irq_status & DSI_DIRECT_CMD_STS_FLAG_V3_TE_RECEIVED_FLAG_MASK)
		ret |= DSILINK_IRQ_BTA_TE;

	if (irq_status & DSI_DIRECT_CMD_STS_FLAG_V3_TRIGGER_RECEIVED_FLAG_MASK)
		/* DSI TE polling answer received */
		ret |= DSILINK_IRQ_TRIGGER;

	if (irq_status &
		DSI_DIRECT_CMD_STS_FLAG_V3_ACKNOWLEDGE_WITH_ERR_RECEIVED_FLAG_MASK)
		ret |= DSILINK_IRQ_ACK_WITH_ERR;

	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3, irq_status);

	irq_status = dsi_rreg(io, DSI_CMD_MODE_STS_FLAG_V3);
	if (irq_status & DSI_CMD_MODE_STS_FLAG_V3_ERR_NO_TE_FLAG_MASK)
		ret |= DSILINK_IRQ_NO_TE;

	if (irq_status & DSI_CMD_MODE_STS_FLAG_V3_ERR_TE_MISS_FLAG_MASK)
		ret |= DSILINK_IRQ_TE_MISS;

	dsi_wreg(io, DSI_CMD_MODE_STS_CLR_V3, irq_status);

	return ret;
}

static int write(u8 *io, struct device *dev, enum dsilink_cmd_datatype type,
						u8 cmd, u8 *data, int len)
{
	int i, ret = 0;
	u32 wrdat[4] = { 0, 0, 0, 0 };
	u32 settings;
	u32 counter = DSI_WRITE_CMD_TIMEOUT_MS;

	if (len > DSILINK_MAX_DSI_DIRECT_CMD_WRITE) {
		dprintf(CRITICAL, "%s: -EINVAL\n", __func__);
		return -EINVAL;
	}

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

	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_NAT_ENUM(WRITE) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LONGNOTSHORT(len > 1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_SIZE(len+1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LP_EN(true);
	if (type == DSILINK_CMD_DCS_WRITE) {
		if (len == 0)
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							DCS_SHORT_WRITE_0);
		else if (len == 1)
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							DCS_SHORT_WRITE_1);
		else
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
								DCS_LONG_WRITE);
	} else if (type == DSILINK_CMD_GENERIC_WRITE) {
		if (len == 0)
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							GENERIC_SHORT_WRITE_0);
		else if (len == 1)
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							GENERIC_SHORT_WRITE_1);
		else if (len == 2)
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							GENERIC_SHORT_WRITE_2);
		else
			settings |=
				DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							GENERIC_LONG_WRITE);
	} else if (type == DSILINK_CMD_SET_MAX_PKT_SIZE) {
		settings |= DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							SET_MAX_PKT_SIZE);
	}

	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS_V3, settings);
	dsi_wreg(io, DSI_DIRECT_CMD_WRDAT_V3, wrdat[0]);
	if (len >  3)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT_V3, wrdat[1]);
	if (len >  7)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT_V3, wrdat[2]);
	if (len > 11)
		dsi_wreg(io, DSI_DIRECT_CMD_WRDAT_V3, wrdat[3]);

	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3, ~0);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR_V3, ~0);
	dsi_wreg(io, DSI_DIRECT_CMD_SEND_V3, true);

	/* loop will normally run zero or one time until WRITE_COMPLETED */
	while (!dsi_rfld(io, DSI_DIRECT_CMD_STS_V3, WRITE_COMPLETED)
			&& --counter)
		cpu_relax();

	if (!counter) {
		dev_err(dev,
			"%s: (v3) DSI write cmd 0x%x timeout!\n",
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

		dev_vdbg(dev, "(v3) DSI Write ok %x error %x\n",
			dsi_rreg(io, DSI_DIRECT_CMD_STS_FLAG_V3),
			dsi_rreg(io, DSI_CMD_MODE_STS_FLAG_V3));
	}

	return ret;
}

static void te_request(u8 *io)
{
	u32 settings;

	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_NAT_ENUM(TE_REQ) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LONGNOTSHORT(false) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_SIZE(2) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LP_EN(true) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(
							DCS_SHORT_WRITE_1);
	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS_V3, settings);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3,
		DSI_DIRECT_CMD_STS_CLR_V3_TE_RECEIVED_CLR(true));
	dsi_wfld(io, DSI_DIRECT_CMD_STS_CTL_V3, TE_RECEIVED_EN, true);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3,
		DSI_DIRECT_CMD_STS_CLR_V3_ACKNOWLEDGE_WITH_ERR_RECEIVED_CLR(true));
	dsi_wfld(io, DSI_DIRECT_CMD_STS_CTL_V3, ACKNOWLEDGE_WITH_ERR_EN, true);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR_V3,
		DSI_CMD_MODE_STS_CLR_V3_ERR_NO_TE_CLR(true));
	dsi_wfld(io, DSI_CMD_MODE_STS_CTL_V3, ERR_NO_TE_EN, true);
	dsi_wreg(io, DSI_CMD_MODE_STS_CLR_V3,
		DSI_CMD_MODE_STS_CLR_V3_ERR_TE_MISS_CLR(true));
	dsi_wfld(io, DSI_CMD_MODE_STS_CTL_V3, ERR_TE_MISS_EN, true);
	dsi_wreg(io, DSI_DIRECT_CMD_SEND_V3, true);
}

static int read(u8 *io, struct device *dev, u8 cmd, u32 *data, int *len)
{
	int ret = 0;
	u32 settings;
	bool error = false, ok = false;
	bool ack_with_err = false;
	u8 nbr_of_retries = DSI_READ_NBR_OF_RETRIES;

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, BTA_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, READ_EN, true);
	settings = DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_NAT_ENUM(READ) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LONGNOTSHORT(false) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_ID(0) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_SIZE(1) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_LP_EN(true) |
		DSI_DIRECT_CMD_MAIN_SETTINGS_V3_CMD_HEAD_ENUM(DCS_READ);
	dsi_wreg(io, DSI_DIRECT_CMD_MAIN_SETTINGS_V3, settings);

	dsi_wreg(io, DSI_DIRECT_CMD_WRDAT_V3, cmd);

	do {
		u8 wait  = DSI_READ_TIMEOUT_MS;
		dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3, ~0);
		dsi_wreg(io, DSI_DIRECT_CMD_RD_STS_CLR_V3, ~0);
		dsi_wreg(io, DSI_DIRECT_CMD_SEND_V3, true);

		while (wait-- && !(error = dsi_rfld(io, DSI_DIRECT_CMD_STS_V3,
					READ_COMPLETED_WITH_ERR)) &&
				!(ok = dsi_rfld(io, DSI_DIRECT_CMD_STS_V3,
							READ_COMPLETED)))
			udelay(DSI_READ_DELAY_US);

		ack_with_err = dsi_rfld(io, DSI_DIRECT_CMD_STS_V3,
						ACKNOWLEDGE_WITH_ERR_RECEIVED);
		if (ack_with_err)
			dev_warn(dev,
					"DCS Acknowledge Error Report %.4X\n",
				dsi_rfld(io, DSI_DIRECT_CMD_STS_V3, ACK_VAL));
	} while (--nbr_of_retries && ack_with_err);

	if (ok) {
		int rdsize;
		u32 rddat;

		rdsize = dsi_rfld(io, DSI_DIRECT_CMD_RD_PROPERTY_V3, RD_SIZE);
		rddat = dsi_rreg(io, DSI_DIRECT_CMD_RDDAT_V3);
		if (rdsize < *len) {
			ret = -EINVAL;
		} else {
			*len = min(*len, rdsize);
			memcpy(data, &rddat, *len);
		}
	} else {
		u8 dat1_status;
		u32 sts;

		sts = dsi_rreg(io, DSI_DIRECT_CMD_STS_V3);
		dat1_status = dsi_rfld(io, DSI_MCTL_LANE_STS_V3,
							DATLANE1_STATE);
		dev_err(dev, "DCS read failed, err=%d, D0 state %d sts %X\n",
						error, dat1_status, sts);
		dsi_wreg(io, DSI_DIRECT_CMD_RD_INIT_V3, true);
		/* If dat1 is still in read to a force stop */
		if (dat1_status == DSILINK_LANE_STATE_READ ||
						sts == DSI_CMD_TRANSMISSION)
			ret = -EAGAIN;
		else
			ret = -EIO;
	}

	dsi_wreg(io, DSI_CMD_MODE_STS_CLR_V3, ~0);
	dsi_wreg(io, DSI_DIRECT_CMD_STS_CLR_V3, ~0);

	return ret;
}

static void force_stop(u8 *io)
{
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL_V3, FORCE_STOP_MODE, true);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL_V3, CLK_FORCE_STOP, true);
	udelay(20);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL_V3, FORCE_STOP_MODE, false);
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL_V3, CLK_FORCE_STOP, false);
}

static int enable(u8 *io, struct device *dev, const struct dsilink_port *port,
				struct dsilink_dsi_vid_registers *vid_regs)
{
	int i = 0;
	bool useMoreThan2DL = false;

	if (port->phy.num_data_lanes > 2)
		useMoreThan2DL = true;

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, LINK_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, BTA_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, READ_EN, true);
	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, REG_TE_EN, true);

	dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, HOST_EOT_GEN,
						port->phy.host_eot_gen);
	dsi_wfld(io, DSI_CMD_MODE_CTL2_V3, TE_TIMEOUT, 0x3FF);

	if (port->sync_src == DSILINK_SYNCSRC_TE_POLLING) {
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, IF1_TE_EN, true);
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, TE_HW_POLLING_EN, true);
	}

	dsi_wreg(io, DSI_MCTL_DPHY_STATIC_V3,
		DSI_MCTL_DPHY_STATIC_V3_UI_X4(port->phy.ui));

	dsi_wreg(io, DSI_MCTL_MAIN_PHY_CTL_V3,
		DSI_MCTL_MAIN_PHY_CTL_V3_WAIT_BURST_TIME(0xf) |
		DSI_MCTL_MAIN_PHY_CTL_V3_LANE2_EN(
			port->phy.num_data_lanes >= 2) |
		DSI_MCTL_MAIN_PHY_CTL_V3_LANE3_EN(
			port->phy.num_data_lanes >= 3) |
		DSI_MCTL_MAIN_PHY_CTL_V3_LANE4_EN(
			port->phy.num_data_lanes >= 4) |
		DSI_MCTL_MAIN_PHY_CTL_V3_CLK_ULPM_EN(false) |
		DSI_MCTL_MAIN_PHY_CTL_V3_DAT1_ULPM_EN(false) |
		DSI_MCTL_MAIN_PHY_CTL_V3_DAT2_ULPM_EN(false) |
		DSI_MCTL_MAIN_PHY_CTL_V3_DAT3_ULPM_EN(false) |
		DSI_MCTL_MAIN_PHY_CTL_V3_DAT4_ULPM_EN(false) |
		DSI_MCTL_MAIN_PHY_CTL_V3_CLK_CONTINUOUS(
			port->phy.clk_cont));
	dsi_wreg(io, DSI_DPHY_LANES_TRIM1_V3,
		DSI_DPHY_LANES_TRIM1_V3_SPEC_81_ENUM(0_90) |
		DSI_DPHY_LANES_TRIM1_V3_CTRL_4DL(useMoreThan2DL));
	dsi_wreg(io, DSI_MCTL_DPHY_TIMEOUT1_V3,
		DSI_MCTL_DPHY_TIMEOUT1_V3_CLK_DIV(0xf) |
		DSI_MCTL_DPHY_TIMEOUT1_V3_HSTX_TO_VAL(0x3fff));
	dsi_wreg(io, DSI_MCTL_DPHY_TIMEOUT2_V3,
		DSI_MCTL_DPHY_TIMEOUT2_V3_LPRX_TO_VAL(0x3fff));
	/* TODO: make enum */
	dsi_wfld(io, DSI_CMD_MODE_CTL2_V3, ARB_MODE, false);
	/* TODO: make enum */
	dsi_wfld(io, DSI_CMD_MODE_CTL2_V3, ARB_PRI, false);
	dsi_wreg(io, DSI_MCTL_MAIN_EN_V3,
		DSI_MCTL_MAIN_EN_V3_PLL_START(true) |
		DSI_MCTL_MAIN_EN_V3_CKLANE_EN(true) |
		DSI_MCTL_MAIN_EN_V3_DAT1_EN(true) |
		DSI_MCTL_MAIN_EN_V3_DAT2_EN(port->phy.num_data_lanes > 1) |
		DSI_MCTL_MAIN_EN_V3_DAT3_EN(port->phy.num_data_lanes > 2) |
		DSI_MCTL_MAIN_EN_V3_DAT4_EN(port->phy.num_data_lanes > 3) |
		DSI_MCTL_MAIN_EN_V3_IF1_EN(true) |
		DSI_MCTL_MAIN_EN_V3_IF2_EN(false));

	while (dsi_rfld(io, DSI_MCTL_MAIN_STS_V3, CLKLANE_READY) == 0 ||
		dsi_rfld(io, DSI_MCTL_MAIN_STS_V3, DAT1_READY) ==
						DSILINK_LANE_STATE_START ||
		(dsi_rfld(io, DSI_MCTL_MAIN_STS_V3, DAT2_READY) ==
					DSILINK_LANE_STATE_START &&
						port->phy.num_data_lanes > 1) ||
		(dsi_rfld(io, DSI_MCTL_MAIN_STS_V3, DAT3_READY) ==
					DSILINK_LANE_STATE_START &&
						port->phy.num_data_lanes > 2) ||
		(dsi_rfld(io, DSI_MCTL_MAIN_STS_V3, DAT4_READY) ==
					DSILINK_LANE_STATE_START &&
						port->phy.num_data_lanes > 3)) {
		usleep_range(1000, 1000);
		if (i++ == 10) {
			dev_warn(dev, "DSI lane not ready (v3)!\n");
			return -EINVAL;
		}
	}

	if (port->mode == DSILINK_MODE_VID) {
		/* burst mode or non-burst mode */
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, BURST_MODE,
							vid_regs->burst_mode);

		/* sync is pulse or event */
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, SYNC_PULSE_ACTIVE,
						vid_regs->sync_is_pulse);
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, SYNC_PULSE_HORIZONTAL,
						vid_regs->sync_is_pulse);

		/* disable video stream when using TVG */
		if (vid_regs->tvg_enable) {
			dsi_wfld(io, DSI_MCTL_MAIN_EN_V3, IF1_EN, false);
			dsi_wfld(io, DSI_MCTL_MAIN_EN_V3, IF2_EN, false);
		}

		/*
		 * behavior during blanking time
		 * 00: NULL packet 1x:LP 01:blanking-packet
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, REG_BLKLINE_MODE, 1);

		/*
		 * behavior during eol
		 * 00: NULL packet 1x:LP 01:blanking-packet
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, REG_BLKEOL_MODE, 2);

		/* time to perform LP->HS on D-PHY */
		dsi_wfld(io, DSI_VID_DPHY_TIME_V3, REG_WAKEUP_TIME,
						port->phy.vid_wakeup_time);

		/*
		 * video stream starts on VSYNC packet
		 * and stops at the end of a frame
		 */
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, VID_ID, 0);
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, START_MODE, 0);
		dsi_wfld(io, DSI_VID_MAIN_CTL_V3, STOP_MODE, 0);

		/* 1: if1 in video mode, 0: if1 in command mode */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, IF_VID_MODE, 1);

		/* 1: enables the link, 0: disables the link */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, VID_EN, 1);
	} else {
		dsi_wfld(io, DSI_CMD_MODE_CTL_V3, IF1_ID, 0);
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
	dsi_wfld(io, DSI_VID_VSIZE1_V3, VFP_LENGTH, vmode->vfp);
	dsi_wfld(io, DSI_VID_VSIZE1_V3, VBP_LENGTH, vmode->vbp);
	dsi_wfld(io, DSI_VID_VSIZE1_V3, VSA_LENGTH, vmode->vsw);
	dsi_wfld(io, DSI_VID_HSIZE1_V3, HFP_LENGTH, vid_regs->hfp);
	dsi_wfld(io, DSI_VID_HSIZE1_V3, HBP_LENGTH, vid_regs->hbp);
	dsi_wfld(io, DSI_VID_HSIZE1_V3, HSA_LENGTH, vid_regs->hsa);
	dsi_wfld(io, DSI_VID_VSIZE2_V3, VACT_LENGTH, vmode->yres);
	dsi_wfld(io, DSI_VID_HSIZE2_V3, RGB_SIZE, vmode->xres * bpp);
	dsi_wfld(io, DSI_VID_MAIN_CTL_V3, VID_PIXEL_MODE, vid_regs->pixel_mode);
	dsi_wfld(io, DSI_VID_MAIN_CTL_V3, HEADER, vid_regs->rgb_header);

	if (vid_regs->tvg_enable) {
		/*
		 * with these settings, expect to see 64 pixels wide
		 * red and green vertical stripes on the screen when
		 * tvg_enable = 1
		 */
		dsi_wfld(io, DSI_MCTL_MAIN_DATA_CTL_V3, TVG_SEL, 1);

		dsi_wfld(io, DSI_TVG_CTL_V3, TVG_STRIPE_SIZE, 6);
		dsi_wfld(io, DSI_TVG_CTL_V3, TVG_MODE, 2);
		dsi_wfld(io, DSI_TVG_CTL_V3, TVG_STOPMODE, 2);
		dsi_wfld(io, DSI_TVG_CTL_V3, TVG_RUN, 1);

		dsi_wfld(io, DSI_TVG_IMG_SIZE_V3, TVG_NBLINE, vmode->yres);
		dsi_wfld(io, DSI_TVG_IMG_SIZE_V3, TVG_LINE_SIZE,
							vmode->xres * bpp);
		dsi_wfld(io, DSI_TVG_COLOR1_BIS_V3, COL1_BLUE, 0);
		dsi_wfld(io, DSI_TVG_COLOR1_V3, COL1_GREEN, 0);
		dsi_wfld(io, DSI_TVG_COLOR1_V3, COL1_RED, 0xFF);

		dsi_wfld(io, DSI_TVG_COLOR2_BIS_V3, COL2_BLUE, 0);
		dsi_wfld(io, DSI_TVG_COLOR2_V3, COL2_GREEN, 0xFF);
		dsi_wfld(io, DSI_TVG_COLOR2_V3, COL2_RED, 0);
	}
	if (vid_regs->sync_is_pulse)
		dsi_wfld(io, DSI_VID_BLKSIZE2_V3, BLKLINE_PULSE_PCK,
							vid_regs->blkline_pck);
	else
		dsi_wfld(io, DSI_VID_BLKSIZE1_V3, BLKLINE_EVENT_PCK,
							vid_regs->blkline_pck);
	dsi_wfld(io, DSI_VID_DPHY_TIME_V3, REG_LINE_DURATION,
						vid_regs->line_duration);
	if (vid_regs->burst_mode) {
		dsi_wfld(io, DSI_VID_BLKSIZE1_V3, BLKEOL_PCK,
							vid_regs->blkeol_pck);
		dsi_wfld(io, DSI_VID_PCK_TIME_V3, BLKEOL_DURATION,
						vid_regs->blkeol_duration);
		dsi_wfld(io, DSI_VID_VCA_SETTING1_V3, MAX_BURST_LIMIT,
						vid_regs->blkeol_pck - 6);
		dsi_wfld(io, DSI_VID_VCA_SETTING2_V3, EXACT_BURST_LIMIT,
							vid_regs->blkeol_pck);
	}
	if (vid_regs->sync_is_pulse)
		dsi_wfld(io, DSI_VID_VCA_SETTING2_V3, MAX_LINE_LIMIT,
						vid_regs->blkline_pck - 6);

	return 0;
}

static void set_clk_continous(u8 *io, bool value)
{
	dsi_wfld(io, DSI_MCTL_MAIN_PHY_CTL_V3, CLK_CONTINUOUS, value);
}

void __init nova_dsilink_v3_init(struct dsilink_ops *ops)
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
