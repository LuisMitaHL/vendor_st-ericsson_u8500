/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the Linux code
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <linux/list.h>
#include <div64.h>
#include <asm/arch/ab8500.h>

static struct list_head mmc_devices;
static int cur_dev_num = -1;

static int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	return mmc->send_cmd(mmc, cmd, data);
}

static int mmc_set_blocklen(struct mmc *mmc, uint len)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

static int mmc_set_block_count(struct mmc *mmc, uint blkcnt)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCK_COUNT;
	cmd.resp_type = MMC_RSP_R1;
	if (mmc->card_caps & MMC_MODE_REL_WR)
		cmd.cmdarg = 0x80000000 | blkcnt;
	else
		cmd.cmdarg = blkcnt;
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		if (m->block_dev.dev == dev_num)
			return m;
	}

	printf("MMC Device %d not found\n", dev_num);

	return NULL;
}

static unsigned long
mmc_bwrite_multi(struct mmc *mmc, ulong start, ulong blkcnt, const void *src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;
	ulong blkwritecnt;
	ulong blkleftcnt = blkcnt;
	void *src_p = (void *) src;
	uint max_block_cnt = 0xffff;

	/*
	 * Each mmc host controller has a size limit in it's register, used
	 * when initializing a new data transfer. Thus we need to wrap larger
	 * bulk of requests. At the moment the limit is hardcoded to 0xFFFF
	 * blocks. This should maybe be configurable by each host driver
	 * instead.
	 */

	if ((mmc->card_caps & MMC_MODE_REL_WR) &&
	   !(mmc->wr_rel_param & EXT_CSD_WR_REL_PARAM_EN_REL_WR))
		max_block_cnt = mmc->rel_wr_sec_c;

	while (blkleftcnt > 0) {

		if (blkleftcnt > max_block_cnt)
			blkwritecnt = max_block_cnt;
		else
			blkwritecnt = blkleftcnt;

		if (mmc->high_capacity)
			cmd.cmdarg = start;
		else
			cmd.cmdarg = start * mmc->write_bl_len;

		if (mmc->card_caps & MMC_MODE_REL_WR) {
			err = mmc_set_block_count(mmc, blkwritecnt);
			if (err) {
				printf("MMC set block count failed, err=%d\n",
					err);
				return 0;
			}
		}

		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
		cmd.resp_type = MMC_RSP_R1;
		cmd.flags = 0;

		data.blocksize = mmc->write_bl_len;
		data.flags = MMC_DATA_WRITE;
		data.src = src_p;
		data.blocks = blkwritecnt;

		err = mmc_send_cmd(mmc, &cmd, &data);
		if (err) {
			printf("MMC write multi failed, err=%d\n", err);
			return 0;
		}

		if (!(mmc->card_caps & MMC_MODE_REL_WR) ||
		     (max_block_cnt != mmc->rel_wr_sec_c)) {
			cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
			cmd.cmdarg = 0;
			cmd.resp_type = MMC_RSP_R1b;
			cmd.flags = 0;

			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err) {
				printf("MMC write - stop cmd failed, err=%d\n",
					err);
				return 0;
			}
		}

		blkleftcnt -= blkwritecnt;
		start += blkwritecnt;
		src_p += blkwritecnt * mmc->write_bl_len;
	}

	return blkcnt;
}

static unsigned long
mmc_bwrite_single(struct mmc *mmc, ulong start, const void *src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->write_bl_len;

	data.src = src;
	data.blocks = 1;
	data.blocksize = mmc->write_bl_len;
	data.flags = MMC_DATA_WRITE;

	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err) {
		printf("MMC write single failed, err=%d\n", err);
		return 0;
	}

	return 1;
}

static unsigned long
mmc_bwrite(int dev_num, unsigned long start, lbaint_t blkcnt, const void *src)
{
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!mmc) {
		printf("MMC Device %d not found\n", dev_num);
		return 0;
	}

	if (blkcnt > 1)
		return mmc_bwrite_multi(mmc, start, blkcnt, src);
	else if (blkcnt == 1)
		return mmc_bwrite_single(mmc, start, src);

	return 0;
}

static unsigned long
mmc_bread_multi(struct mmc *mmc, ulong start, ulong blkcnt, void *dst)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;
	ulong blkreadcnt;
	ulong blkleftcnt = blkcnt;

	/*
	 * Each mmc host controller has a size limit in it's register, used
	 * when initializing a new data transfer. Thus we need to wrap larger
	 * bulk of requests. At the moment the limit is hardcoded to 0xFFFF
	 * blocks. This should maybe be configurable by each host driver
	 * instead.
	 */

	while (blkleftcnt > 0) {
		if (blkleftcnt > 0xffff)
			blkreadcnt = 0xffff;
		else
			blkreadcnt = blkleftcnt;

		if (mmc->high_capacity)
			cmd.cmdarg = start;
		else
			cmd.cmdarg = start * mmc->read_bl_len;

		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
		cmd.resp_type = MMC_RSP_R1;
		cmd.flags = 0;

		data.blocksize = mmc->read_bl_len;
		data.flags = MMC_DATA_READ;
		data.dest = dst;
		data.blocks = blkreadcnt;

		err = mmc_send_cmd(mmc, &cmd, &data);
		if (err) {
			printf("MMC read multi failed, err=%d\n", err);
			return 0;
		}

		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (err) {
			printf("MMC read - stop cmd failed, err=%d\n", err);
			return 0;
		}

		blkleftcnt -= blkreadcnt;
		start += blkreadcnt;
		dst += blkreadcnt * mmc->read_bl_len;
	}

	return blkcnt;
}

static unsigned long
mmc_bread_single(struct mmc *mmc, ulong start, void *dst)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;
	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	data.dest = dst;
	data.blocks = 1;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err) {
		printf("MMC read single failed, err=%d\n", err);
		return 0;
	}

	return 1;
}

static unsigned long
mmc_bread(int dev_num, unsigned long start, lbaint_t blkcnt, void *dst)
{
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!mmc) {
		printf("MMC Device %d not found\n", dev_num);
		return 0;
	}

	if (blkcnt > 1)
		return mmc_bread_multi(mmc, start, blkcnt, dst);
	else if (blkcnt == 1)
		return mmc_bread_single(mmc, start, dst);

	return 0;
}

static int mmc_go_idle(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	udelay(2000);

	return 0;
}

static int
sd_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;

	do {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = mmc->voltages;

		if (mmc->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(1000);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (mmc->version != SD_VERSION_2) {
		mmc->version = SD_VERSION_1_0;
		mmc->high_capacity = 0;
	} else {
		mmc->ocr = cmd.response[0];
		mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	}
	mmc->rca = 0;

	return 0;
}

static int mmc_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	struct mmc_cmd cmd;
	int err;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

	do {
		cmd.cmdidx = MMC_CMD_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = OCR_HCS | mmc->voltages;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(1000);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

static int mmc_send_ext_csd(struct mmc *mmc, char *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = ext_csd;
	data.blocks = 1;
	data.blocksize = 512;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

static int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		(index << 16) |
		(value << 8);
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

static int mmc_change_freq(struct mmc *mmc)
{
	char ext_csd[512];
	char cardtype;
	int err;

	mmc->card_caps = 0;

	/*
	 * Instead of probing according to the bus testing procedure,
	 * the buswitdh that is supported from the MMC device is hardcoded
	 * to both 8 and/or 4 bit. It is up to the host driver to set
	 * other limitations. This also applies to DDR mode.
	 */
	mmc->card_caps = MMC_MODE_4BIT | MMC_MODE_8BIT | MMC_MODE_DDR |
		MMC_MODE_REL_WR;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	if (mmc->high_capacity)
		mmc->capacity = (u64)(ext_csd[EXT_CSD_SEC_CNT + 0] << 0 |
				ext_csd[EXT_CSD_SEC_CNT + 1] << 8 |
				ext_csd[EXT_CSD_SEC_CNT + 2] << 16 |
				ext_csd[EXT_CSD_SEC_CNT + 3] << 24) *
				mmc->read_bl_len;

	mmc->wr_rel_param = ext_csd[EXT_CSD_WR_REL_PARAM];
	mmc->rel_wr_sec_c = ext_csd[EXT_CSD_REL_WR_SEC_C];

	if (mmc->rel_wr_sec_c == 1)
		mmc->card_caps &= ~MMC_MODE_REL_WR;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);
	if (err)
		return err;

	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);
	if (err)
		return err;

	/* No high-speed support */
	if (!ext_csd[EXT_CSD_HS_TIMING])
		return 0;

	/*
	 * High Speed mode is set, two types: SDR 52MHz or SDR 26MHz
	 * DDR mode is not supported yet.
	 */
	cardtype = ext_csd[EXT_CSD_CARD_TYPE];
	if (cardtype & MMC_HS_52MHZ)
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	else
		mmc->card_caps |= MMC_MODE_HS;

	if (mmc->wr_rel_param & EXT_CSD_WR_REL_PARAM_HS_CTRL_REL)
		err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
				 EXT_CSD_WR_REL_SET, 1);

	return 0;
}

static int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);
	cmd.flags = 0;

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

static int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	uint switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	timeout = 3;

	do {
		data.dest = (char *)&scr;
		data.blocksize = 8;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		err = mmc_send_cmd(mmc, &cmd, &data);
	} while (err && timeout--);
	if (!timeout)
		return err;

	mmc->scr[0] = __be32_to_cpu(scr[0]);
	mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *)&switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* If high-speed isn't supported, we return */
	if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)&switch_status);

	if (err)
		return err;

	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

/*
 * frequency bases
 * divided by 10 to be nice to platforms without floating point
 */
static int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/*
 * Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static int multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

static void mmc_set_ios(struct mmc *mmc)
{
	mmc->set_ios(mmc);
}

static void mmc_set_clock(struct mmc *mmc, uint clock)
{
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	if (clock < mmc->f_min)
		clock = mmc->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, uint width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

static int mmc_startup(struct mmc *mmc)
{
	int err;
	uint mult, freq;
	u64 cmult, csize;
	struct mmc_cmd cmd;

	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = mmc->rca << 16;
	cmd.resp_type = MMC_RSP_R6;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if (IS_SD(mmc))
		mmc->rca = (cmd.response[0] >> 16) & 0xffff;

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) {
			case 0:
				mmc->version = MMC_VERSION_1_2;
				break;
			case 1:
				mmc->version = MMC_VERSION_1_4;
				break;
			case 2:
				mmc->version = MMC_VERSION_2_2;
				break;
			case 3:
				mmc->version = MMC_VERSION_3;
				break;
			case 4:
				mmc->version = MMC_VERSION_4;
				break;
			default:
				mmc->version = MMC_VERSION_1_2;
				break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;

	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	/* This is not correct for MMC cards bigger than 2GB.
	 * C_SIZE=0xFFF and C_SIZE_MULT=0x7 for bigger than 2GB.
	 * READ_BL_LEN < 12 (2k sectors) to do the calculation.
	 * High capasity cards: Use EXT_CSD instead.
	 * Check for SD!
	 */
	if (mmc->high_capacity) {
		csize = CSD_HC_SIZE(mmc->csd);
		cmult = 8;
	} else {
		csize = CSD_C_SIZE(mmc->csd);
		cmult = CSD_C_SIZE_MULT(mmc->csd);
	}

	/* This is only correct for MMC cards up to 2GB. SD? */
	mmc->capacity = (csize + 1) << (cmult + 2);
	mmc->capacity *= mmc->read_bl_len;

	if (mmc->read_bl_len > 512)
		mmc->read_bl_len = 512;

	if (mmc->write_bl_len > 512)
		mmc->write_bl_len = 512;

	/* Select the card, and put it into Transfer Mode */
	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;
	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if (IS_SD(mmc)) {
		err = sd_change_freq(mmc);
		debug("sd_change_freq returns %d\n", err);
	} else {
		err = mmc_change_freq(mmc);
		debug("mmc_change_freq returns %d\n", err);
	}

	if (err)
		return err;

	/*
	 * Restrict card capabilities by the host capabilities.
	 * FIXME: Host caps are ignored when setting high speed in
	 * mmc_change_freq and sd_change_freq.
	 */
	mmc->card_caps &= mmc->host_caps;

	if (IS_SD(mmc)) {
		if (mmc->card_caps & MMC_MODE_4BIT) {
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = mmc->rca << 16;
			cmd.flags = 0;

			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			cmd.flags = 0;
			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc_set_clock(mmc, 50000000);
		else
			mmc_set_clock(mmc, 25000000);
	} else {
		if ((mmc->card_caps & MMC_MODE_DDR_8BIT) == MMC_MODE_DDR_8BIT) {
			/* Set the card to use 8 bit*/
			printf("EXT_CSD_BUS_WIDTH_DDR_8\n");
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_DDR_8);
			if (err)
				return err;

			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_POWER_CLASS,
					0xAA);
			if (err)
				return err;
			printf("EXT_CSD_BUS_WIDTH_DDR_8\n");
			mmc->ddr_en = 1;
			mmc_set_bus_width(mmc, 8);
		} else if ((mmc->card_caps & MMC_MODE_DDR_4BIT) ==
			    MMC_MODE_DDR_4BIT) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_DDR_4);
			if (err)
				return err;

			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_POWER_CLASS,
					0xAA);
			if (err)
				return err;
			printf("EXT_CSD_BUS_WIDTH_DDR_4\n");
			mmc->ddr_en = 1;
			mmc_set_bus_width(mmc, 4);
		} else if (mmc->card_caps & MMC_MODE_8BIT) {
			/* Set the card to use 8 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_8);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 8);
		} else if (mmc->card_caps & MMC_MODE_4BIT) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_4);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		}

		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc_set_clock(mmc, 52000000);
			else
				mmc_set_clock(mmc, 26000000);
		} else
			mmc_set_clock(mmc, 20000000);

		if (mmc->card_caps & MMC_MODE_REL_WR) {
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					 EXT_CSD_WR_REL_SET,
					 0x1F);

			if (err)
				return err;
		}
	}

	/* fill in device description */
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	sprintf(mmc->block_dev.vendor, "Man %06x Snr %08x", mmc->cid[0] >> 8,
			(mmc->cid[2] << 8) | (mmc->cid[3] >> 24));
	sprintf(mmc->block_dev.product, "%c%c%c%c%c", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);
	sprintf(mmc->block_dev.revision, "%d.%d", mmc->cid[2] >> 28,
			(mmc->cid[2] >> 24) & 0xf);
	init_part(&mmc->block_dev);

	return 0;
}

static int mmc_send_if_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

int mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read = mmc_bread;
	mmc->block_dev.block_write = mmc_bwrite;

	INIT_LIST_HEAD (&mmc->link);

	list_add_tail (&mmc->link, &mmc_devices);

	return 0;
}

block_dev_desc_t *mmc_get_dev(int dev)
{
	struct mmc *mmc = find_mmc_device(dev);

	return mmc ? &mmc->block_dev : NULL;
}

static int do_mmc_init(struct mmc *mmc, int shouldprint)
{
	int err;

	err = mmc->init(mmc);

	if (err)
		return err;

	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 1);

	/* Reset the Card */
	err = mmc_go_idle(mmc);

	if (err)
		return err;

	/* Test for SD version 2 */
	err = mmc_send_if_cond(mmc);
	debug("mmc_send_if_cond returns %d\n", err);

	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);
	debug("sd_send_op_cond returns %d\n", err);

	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
		err = mmc_send_op_cond(mmc);
		debug("mmc_send_op_cond returns %d\n", err);
		if (err) {
			if (shouldprint) {
				printf("Card did not respond to voltage select!\n");
			}
			return UNUSABLE_ERR;
		}
	}

	err = mmc_startup(mmc);

	if (!err) {
		err = mmc_set_blocklen(mmc, 512);
		if (err)
			printf("MMC set write bl len failed, err=%d\n", err);
	}

	debug("mmc_startup returns %d\n", err);
	return err;
}

int mmc_init(struct mmc *mmc)
{

	return do_mmc_init(mmc, 1);
}

int mmc_init_silent(struct mmc *mmc)
{

	return do_mmc_init(mmc, 0);
}

/*
 * CPU and board-specific MMC initializations.  Aliased function
 * signals caller to move on
 */
static int __def_mmc_init(bd_t *bis)
{
	return -1;
}

int cpu_mmc_init(bd_t *bis) __attribute__((weak, alias("__def_mmc_init")));
/*
 * It seems attribute 'weak' does not work as intended. With gcc 4.4.1 and
 * optimization O2 it always links in the weak function. Declare board_mmc_init
 * as external.
 */
extern int board_mmc_init(bd_t *bis);

void print_mmc_devices(char separator)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		printf("%s: %d", m->name, m->block_dev.dev);

		if (entry->next != &mmc_devices)
			printf("%c ", separator);
	}

	printf("\n");
}

int mmc_initialize(bd_t *bis)
{
	INIT_LIST_HEAD (&mmc_devices);
	cur_dev_num = 0;

	/* Voltage Check@VAUX2 before accessing eMMC */

	u8 vaux = ab8500_read(0x04, 0x20); //Vaux2Sel Register BaseAddress:0x420

	if ((vaux > AB8500_VAUX2_2V7) && (vaux <= AB8500_VAUX2_MAX))
		printf("Proper voltage 0x0%x applied to eMMC\n", vaux);
	else {
		if (ab8500_write(0x04, 0x20, AB8500_VAUX2_2V9) < 0)
			printf("EMMC: Failed to set default voltage\n");
		else
			printf("EMMC:Set to Default 0x0%x as Voltage is not in the Range!\n", vaux);
	}

	if (board_mmc_init(bis) < 0)
		cpu_mmc_init(bis);

	print_mmc_devices(',');

	return 0;
}
