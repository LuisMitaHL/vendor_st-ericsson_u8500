/*
 *  Copyright (C) 2010 ST-Ericsson AB
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 *
 *
 * File Name     : soc_settings_core.c
 * Author        : Mian Yousaf Kaukab <mian.yousaf.kaukab@st-ericsson.com>
 *
 * This file is based on soc-settings (meminit) code for U8500 platform
 */
#include <ddr_settings.h>
#include <uart.h>
#include <bass_app.h>
#include <security.h>
#include <avs/avutils_avs.h>

#define DDR_DATA_LEN	616
#define DDR_CS_DATA_LEN	16

#define CS0_BC1 1
#define CS0_BC4 4

struct ddr_data {
	struct ddr_init_mrr init_mrr;
	u8 ddr_buf[DDR_DATA_LEN];
	u8 ddr_regs[DDR_CS_DATA_LEN];
	struct frequency set_freq;
	struct memory_speed ddr_clock;
};

/* Version string */
char version_str[] = "@#MEM_INIT_"__FILE__"_"__DATE__"/"__TIME__;

/* Test DDR data access */
int test_ddr_data_access(void)
{
	u32 address;
	u32 i;
	u32 data;
	u32 data_written[33];
	u8 byte1;
	u8 byte2;

	logmsg("soc-settings: Starting DDR test...\r\n");

	/* Write in RAM to test address wires [0:12] and data wires [0:27]
	 * address (0x8000) to (0x8000000) (=> some address wires will be
	 * tested twice, doesn't matter)
	 * data   0x1 to 0x8000000
	 */
	address = 1;
	data = 1;
	for (i=0; i<2; i++ ) {
		data_written[i] = data;
		*((u8*)(address|0x8000)) = (u8)(data);

		address = 1 << (i+1);
		data = data << 1;
	}

	/* write in RAM DDR 32bit data
	 * address 0x8004 to 0x8000000
	 * data   0x4 to 0x8000000
	 */
	for (i=2; i<28; i++) {
		data_written[i] = data;

		if (address == 0x8000)
			goto skip_write;

		if (address < 0x8000)
			address |= 0x8000;

		*((u32*)(address)) = (u32)(data);

	skip_write:
		address = 1 << (i+1);
		data = data << 1;
	}

	/* write in RAM to test data wires [28:31]
	 * address 0x8800 + 4*n*(1 word)
	 * data: 0x10000000 to 0x80000000 => all 32 data wires will
	 * then be tested.
	 */
	address = 0x800;
	for (i=1; i<=4; i++) {
		/* write in address 0x804 data 0x10000000
		 * then write in address 0x808 data 0x20000000
		 * then write in address 0x80C data 0x40000000
		 * then write in address 0x810 data 0x80000000
		 */
		data_written[i+27] = 1<<(i+27);
		*((u32*)((address+(4*i))|0x8000)) = 1<<(i+27);
	}

	/* write in RAM DDR - to check bit 0 and 1 of data bus
	 * address 0x8814
	 * data 0x3
	 */
	address = 0x814;
	data = 0x3;
	data_written[32] = data;
	*((u32*)(address|0x8000)) = data;

	/* now check the written data by the way of a readback
	 * same loops as the both above...
	 */

	/* test bit 0 and bit 1 of address bus */
	address = 0;
	byte1 = ((*(u32*)(address|0x8000)) & (0x00FF00)) >> 8;
	if (byte1 != (u8)(data_written[0]))
		goto ddr_error;

	byte2 = ((*(u32*)(address|0x8000)) & (0xFF0000)) >> 16;
	if (byte2 != (u8)(data_written[1]))
		goto ddr_error;

	/* Set address for next reading back */
	address = 0x4;
	for (i=2; i<28; i++ ) {
		if (address == 0x8000)
			goto skip_read;

		if (address < 0x8000)
			address |= 0x8000;

		if (*((u32*)(address)) != (u32)(data_written[i]))
			goto ddr_error;
	skip_read:
		address = 1<<(i+1);
	}

	address = 0x800;
	for (i=1; i<=4; i++) {
		if (*((u32*)((address+(4*i))|0x8000)) != data_written[i+27])
			goto ddr_error;
	}

	/* test bit 0 and bit 1 of data bus */
	address = 0x814;
	if (*((u32*)(address|0x8000)) != data_written[32])
		goto ddr_error;

	logmsg("soc-settings: DDR test passed \r\n");
	return BOOT_OK;

ddr_error:
	logerror("soc-settings: DDR test failed\r\n");
	return BOOT_INTERNAL_ERROR;

}

/* Returns 0 on success. */
int send_prcmu_abb_init(void)
{
	volatile struct n_sec_map *prcmu_dm_nsec = TCDMnSec;
	/* ABB_INIT */
	prcmu_dm_nsec->header.req.mb4 = RMB4H_ABB_INIT;

	/* No parameter for ABB init request in mailbox 4 */

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_16;

	logmsg("soc-settings: IT16 sent to XP70 for ABB_INIT\r\n");

	/* Wait for completion */
	while((IO(PRCM_ARM_IT1_VAL) & XP70_IT_EVENT_16) == 0);

	/* Check status */
	if(prcmu_dm_nsec->ack_mb_4.rc != RC_SUCCESS)
		return -1;

	/* Clear IT */
	IO(PRCM_ARM_IT1_CLEAR) = XP70_IT_EVENT_16;

	return 0;
}


/* Returns 0 on success. */
int send_prcmu_abb_write(volatile struct n_sec_map *prcmu_dm_nsec,
			u8 slave_addr,
			u8 reg_addr,
			u8 data_len,
			u8 data0,
			u8 data1,
			u8 data2,
			u8 data3)

{
	/* ABB_INIT */
	prcmu_dm_nsec->header.req.mb5 = RMB5H_I2C_WRITE;


	prcmu_dm_nsec->req_mb_5.slave_addr = slave_addr;
	prcmu_dm_nsec->req_mb_5.reg_addr   = reg_addr;
	prcmu_dm_nsec->req_mb_5.data_len   = data_len;
	prcmu_dm_nsec->req_mb_5.data[0]    = data0;
	prcmu_dm_nsec->req_mb_5.data[1]    = data1;
	prcmu_dm_nsec->req_mb_5.data[2]    = data2;
	prcmu_dm_nsec->req_mb_5.data[3]    = data3;

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_17;

	logmsg("soc-settings: IT17 sent to XP70 for ABB_WRITE\r\n");

	/* Wait for completion */
	while((IO(PRCM_ARM_IT1_VAL) & XP70_IT_EVENT_17) == 0);

	/* Check status */
	if(prcmu_dm_nsec->ack_mb_5.return_code != RC_SUCCESS)
		return -1;

	/* Clear IT */
	IO(PRCM_ARM_IT1_CLEAR) = XP70_IT_EVENT_17;

	return 0;
}

/* Returns 0 on success. */
int send_prcmu_pwr_req(volatile struct n_sec_map *prcmu_dm_nsec,
		       u8 client,
		       u8 state)
{
	prcmu_dm_nsec->header.req.mb2 = MB2H_PWR_REQ;

	prcmu_dm_nsec->req_mb_2.pwr_client = client;
	prcmu_dm_nsec->req_mb_2.pwr_st = state;

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_12;

	logmsg("soc-settings: IT12 sent to XP70 for PWR_REQ\r\n");

	/* Wait for completion */
	while((IO(PRCM_ARM_IT1_VAL) & XP70_IT_EVENT_12) == 0);

	/* Check status */
	if(prcmu_dm_nsec->ack_mb_2.pwr_rc != RC_SUCCESS)
		return -1;

	/* Clear IT */
	IO(PRCM_ARM_IT1_CLEAR) = XP70_IT_EVENT_12;

	return 0;
}

/* Returns 0 on success. */
int send_prcmu_pll_req(volatile struct n_sec_map *prcmu_dm_nsec,
		       u8 pll,
		       u8 state)
{
	prcmu_dm_nsec->header.req.mb2 = MB2H_PLL_REQ;

	prcmu_dm_nsec->req_mb_2.pll = pll;
	prcmu_dm_nsec->req_mb_2.pll_st = state;

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_12;

	logmsg("soc-settings: IT12 sent to XP70 for PLL_REQ\r\n");

	/* Wait for completion */
	while((IO(PRCM_ARM_IT1_VAL) & XP70_IT_EVENT_12) == 0);

	/* Check status */
	if(prcmu_dm_nsec->ack_mb_2.pll_rc != RC_SUCCESS)
		return -1;

	/* Clear IT */
	IO(PRCM_ARM_IT1_CLEAR) = XP70_IT_EVENT_12;

	return 0;
}

/* Returns 0 on success. */
int send_prcmu_modem_req(volatile struct n_sec_map *prcmu_dm_nsec)
{
	prcmu_dm_nsec->header.req.mb0 = RMB0H_WAKE_UP_CFG;

	prcmu_dm_nsec->req_mb_0.dbb_event = MODEM_REQ_ACK;

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_10;

	logmsg("soc-settings: IT10 sent to XP70 for MODEM_REQ\r\n");

	/* Wait for completion */
	while((IO(PRCM_MBOX_CPU_VAL) & XP70_IT_EVENT_10) != 0);

	return 0;
}


static void db5500_init(void)
{

	/* Periph1 PRCC settings */
	*(volatile u32 *)0xA002F000 = 0xFFFFFFFF; /*      enable bus clock () */
	*(volatile u32 *)0xA002F008 = 0xFFFFFFFF;/*      enable kernel clock () */

	/* Periph2 PRCC settings */
	*(volatile u32 *)0xA001F000 = 0xFFFFFFFF;/*      enable bus clock () */
	*(volatile u32 *)0xA001F008 = 0xFFFFFFFF;/*      enable kernel clock () */

	/* Periph3 PRCC settings */
	*(volatile u32 *)0x8014F000 = 0xFFFFFFFF;/*      enable bus clock () */
	*(volatile u32 *)0x8014F008 = 0xFFFFFFFF;/*      enable kernel clock () */

	/* Periph5 PRCC settings */
	*(volatile u32 *)0x8011F000 = 0xFFFFFFFF;/*      enable bus clock () */
	*(volatile u32 *)0x8011F008 = 0xFFFFFFFF;/*      enable kernel clock () */

	/* Periph6 PRCC settings */
	*(volatile u32 *)0x8012F000 = 0xFFFFFFFF;/*      enable bus clock () */
	*(volatile u32 *)0x8012F008 = 0xFFFFFFFF;/*      enable kernel clock () */

	/* Release reset for all GPIOs */
	IO(PRCM_GPIO_RESETN_SET) = 0xFFFF;

	/* Setup GPIO to read HW I2C */
	*(volatile u32 *)0x8015A020 |=  0x00000030;
	*(volatile u32 *)0x8015A024 &= ~0x00000030;

	/* Re-program ICN registers */
	*(volatile u32 *)0x8015BE10  =  0x7f;

	/* Reset MTU reg to proper value: */
	*(volatile u32 *)0x801571c8 = 0x00030000;
}

/* regulator init */
int db5500_regulator_init(void)
{
	volatile struct n_sec_map *prcmu_dm_nsec = TCDMnSec;
	u32 status = BOOT_OK;

	/* Send LDO_H request to prcmu */
	if(send_prcmu_abb_write(prcmu_dm_nsec,
				0x3, /* Slave addr */
				0x7b,/* Reg addr */
				0x1, /* Data len */
				0x36,0,0,0) < 0) {
		logerror("soc-settings: Error in LDO_H request to prcmu\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	/* Send LDO_L request to prcmu*/
	if(send_prcmu_abb_write(prcmu_dm_nsec,
				0x3, /* Slave addr */
				0x81,/* Reg addr */
				0x1, /* Data len */
				0x3E,0,0,0) < 0) {
		logerror("soc-settings: Error in LDO_L request to prcmu\r\n");
		return BOOT_INTERNAL_ERROR;
	 }

	/* Enable PLLSOC1 */
	if(send_prcmu_pll_req(prcmu_dm_nsec, PLLSOC1, OOS_ON)) {
		logerror("soc-settings: Error in PLLSOC1 request to prcmu\r\n");
		return BOOT_INTERNAL_ERROR;
	}

        if(send_prcmu_pwr_req(prcmu_dm_nsec, ESRAM12, OORS_ON)) {
                logerror("soc-settings: Error in ESRAM12 pwr request to prcmu\r\n");
                return BOOT_INTERNAL_ERROR;
        }

	return status;

}

/* clock and modem init */
int db5500_clock_init(void)
{
	u32 status = BOOT_OK;
	volatile struct n_sec_map *prcmu_dm_nsec = TCDMnSec;

	/* Enable clocks
	 *
	 * b:28 B2R2
	 * b:24 MCDE
	 * b:21 HDMI
	 * b:4  SGA
	 * b:3  SIA
	 * b:2  HVA (SVA)
	 */

	/* work around for B2R2 reset problem */
	*(volatile u32 *)0x80157510 = 0x1000000;/* enable mcde clk */
	*(volatile u32 *)0x801571e8 = 0xC;      /* apply reset */
	*(volatile u32 *)0x801571e4 = 0xC;      /* release reset */


	*(volatile u32 *)0x80157510 = 0x11000000;

	/* Release Access Subsystem Resets, PRCM_MOD_RESETN_SET */
	*(volatile u32 *)0x801571FC = 0x00000007;

	/* Tell PRCMU to check writes to PRCM_HOSTACCESS_REQ */
	if(send_prcmu_modem_req(prcmu_dm_nsec)){
		logerror("soc-settings: send_prcmu_modem_req failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	/* Wake Up Access Subsystem, PRCM_HOSTACCESS_REQ */
	*(volatile u32 *)0x80157334 = 0x00000001;

	/* Wait for completion */
	while((IO(PRCM_ARM_IT1_VAL) & XP70_IT_EVENT_10) == 0);

	/* Check status */
	if (prcmu_dm_nsec->header.ack.mb0 != AMB0H_WAKE_UP)
		return BOOT_INTERNAL_ERROR;

	if (prcmu_dm_nsec->ack_mb_0.wk_up_reason[prcmu_dm_nsec->ack_mb_0.rd_ix]
					.dbb_event != MODEM_REQ_ACK)
		return BOOT_INTERNAL_ERROR;

	/* Clear IT */
	IO(PRCM_ARM_IT1_CLEAR) = XP70_IT_EVENT_10;


	prcmu_dm_nsec->header.req.mb0 = RMB0H_RD_WAKE_UP_ACK;

	prcmu_dm_nsec->req_mb_0.dbb_event = MODEM_REQ_ACK;

	/* Send IT to XP70 */
	IO(PRCM_MBOX_CPU_SET) = XP70_IT_EVENT_10;

	/* Wait for completion */
	while((IO(PRCM_MBOX_CPU_VAL) & XP70_IT_EVENT_10) != 0);

	return status;
}

/* configure AVS settings */
int db5500_avs_configure(void)
{
	/* congigure operating points & not supported yet */
	bass_return_code	bass_ret;
	struct avs avs_data;
	u32 fuses[2] = { 0, 0 };
	u8 avs_ret_status = AVS_EXIT_UNFUSEDDEVICE;
	u32 i;

	bass_ret = bass_get_avs_fuses((u8 *)fuses, sizeof(fuses));
	if (bass_ret != BASS_RC_SUCCESS) {
		logmsg("get_avs_fuses failed: %x", bass_ret);
		return BOOT_INTERNAL_ERROR;
	}

	logmsg("avs fuses[0] %x, fuses[1] %x \r\n",fuses[0], fuses[1]);
	memcpy((void *)&avs_data, (void *)&TCDMsec->avs, sizeof(struct avs));
	logmsg("avs varm");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.varm.abb_reg_setting[i]);
	logmsg("\r\n");
	logmsg("avs vape");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.vape.abb_reg_setting[i]);
	logmsg("\r\n");
	logmsg("avs vsafe");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.vsafe.abb_reg_setting[i]);
	logmsg("\r\n");
	avs_ret_status = avs_configure(fuses[0], fuses[1],
					(const int *)&avs_data);

	if (AVS_EXIT_SUCCESS == avs_ret_status) {
		/* Apply the AVS settings */
		bass_ret = bass_prcmu_apply_avs_settings((u8 *)&avs_data,
				sizeof(struct avs));
		if (bass_ret != BASS_RC_SUCCESS) {
			logmsg("apply_avs_settings failed \n");
			return BOOT_INTERNAL_ERROR;
		}

	} else if(AVS_EXIT_UNKNOWNREV == avs_ret_status) {
		logerror("AVS Configuration failed. Return status = AVS_EXIT_UNKNOWNREV \r\n");
		logerror("Please upgrade you soc setting with new AVS software.\r\n");
		return BOOT_INTERNAL_ERROR;
	} else if(AVS_EXIT_OUTOFLIMITS == avs_ret_status) {
		logerror("AVS Configuration failed. Return status = AVS_EXIT_OUTOFLIMITS \r\n");
		logerror("Report Board and DBB version, batch details to AVS.\r\n");
		return BOOT_INTERNAL_ERROR;
	} else {
		/*
		 * For a AVS return value as AVS_EXIT_UNFUSEDDEVICE
		 * continue with default values.
		 */
		logmsg("Not an fused device with default AVS values \r\n");
	}
	logmsg("after avs varm ");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.varm.abb_reg_setting[i]);
	logmsg("\r\n");
	logmsg("after avs vape ");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.vape.abb_reg_setting[i]);
	logmsg("\r\n");
	logmsg("after avs vsafe ");
	for (i = 0; i < 8; i++)
		logmsg("%x ",avs_data.vsafe.abb_reg_setting[i]);
	logmsg("\r\n");
	return BOOT_OK;
}

/* configure thermal sensor data */
int db5500_thsens_configure(void)
{
	bass_return_code ret;
	struct thermal_sensor_data thsens_data;

        /* temp physical limit for sensor in degreeC - 125C */
        thsens_data.SensorMaxThresholdTemp = 0x7D;
	/* temp limit for system shutdown/reboot in degreeC - 85C */
	thsens_data.SensorAlertThresholdTemp = 0x55;
	/* max measuring period in milliseconds - 60sec */
	thsens_data.SensorMaxPeriod = 0xEA60;
	/* min measuring period in milliseonds - 1sec */
	thsens_data.SensorMinPeriod = 0x3E8;
	/* max temp limit for TEMP_HIGH event in degreeC - 84C */
	thsens_data.SensorMaxTempRange = 0x54;
	/* min temp limit for TEMP_LOW event in degreeC - 20C */
	thsens_data.SensorMinTempRange = 0x14;
        /* last read sensor temperature in degreeC */
	thsens_data.SensorCurrentTemp = 0x0;
	/* current measuring period in milliseconds - 5sec */
	thsens_data.SensorMeasPeriod = 0x1388;

	ret = bass_prcmu_set_thermal_sensors((u8 *)&thsens_data,
				sizeof(struct thermal_sensor_data));
	if (ret != BASS_RC_SUCCESS) {
		logmsg("bass_prcmu_set_thermal_sensors failed: %x\r\n", ret);
		return BOOT_INTERNAL_ERROR;
	}

	return BOOT_OK;
}

void db5500_ddr_mrr_init(struct ddr_init_mrr *ddr_init_mrr, bool one_die)
{

	u32 m;

	ddr_init_mrr->cfgadd[0] = 0;  /* Device info */
	ddr_init_mrr->cfgadd[1] = 5;  /* BC-1 : Manufacturer ID */
	ddr_init_mrr->cfgadd[2] = 6;  /* BC-2 : Revision ID1 */
	ddr_init_mrr->cfgadd[3] = 7;  /* BC-3 : Revision ID2 */
	ddr_init_mrr->cfgadd[4] = 8;  /* BC-4 : I/O width + Density + Type */
	ddr_init_mrr->cfgadd[5] = 0;  /* Not used */
	ddr_init_mrr->cfgadd[6] = 0;  /* Not used */
	ddr_init_mrr->cfgadd[7] = 0;  /* Not used */

	/*
	 * Hardcode to read both CS0 and CS1 because PRCMU firmware
	 * will not read the corresponding register unless we've put
	 * something that isn't 0 in cs[x].DataCS[n].
	 */
	for (m = 0; m < sizeof(ddr_init_mrr->cs[0].data_cs) /
				sizeof(ddr_init_mrr->cs[0].data_cs[0]); m++) {
		if ( m != 5 && m != 6 && m != 7) {
			ddr_init_mrr->cs[0].data_cs[m] = 0xFF;
			if (one_die)
				ddr_init_mrr->cs[1].data_cs[m] = 0x00;
			else
				ddr_init_mrr->cs[1].data_cs[m] = 0xFF;
		} else {
			/* Avoid reading CS1 register in case of One CS die */
			ddr_init_mrr->cs[0].data_cs[m] = 0x00;
			ddr_init_mrr->cs[1].data_cs[m] = 0x00;
		}
	}

}

bool ddr_two_die_found(u8 *ddr_regs)
{
	u32 i;
	bool found = false;
	u8 data_cs0[8];
	u8 data_cs1[8];

	for (i = 0; i < 8; i++) {
		if ((i == 1) ||(i == 4)) {
			data_cs0[i] = ddr_regs[i];
			data_cs1[i] = ddr_regs[i + 8];
		}
	}

	/*
	 * If we require to support the new 2CS DDR vendor,
	 * we need to add into this list of vendors.
	 */
	if ((data_cs0[1] == EDB4032B1PB_CS0_BC1) &&
		(data_cs1[1] == EDB4032B1PB_CS1_BC1) &&
		(data_cs0[4] == EDB4032B1PB_CS0_BC4) &&
		(data_cs1[4] == EDB4032B1PB_CS1_BC4)) {
		found = true;
		logmsg("soc-settings: EDB4032B1PB two die DDR found \r\n");
	} else if ((data_cs0[1] == H8TBR00U0MLR_CS0_BC1) &&
		(data_cs1[1] == H8TBR00U0MLR_CS1_BC1) &&
		(data_cs0[4] == H8TBR00U0MLR_CS0_BC4) &&
		(data_cs1[4] == H8TBR00U0MLR_CS1_BC4)) {
		found = true;
		logmsg("soc-settings: H8TBR00U0MLR two die DDR found \r\n");
	} else if ((data_cs0[1] == K4P8G304EB_CS0_BC1) &&
		(data_cs1[1] == K4P8G304EB_CS1_BC1) &&
		(data_cs0[4] == K4P8G304EB_CS0_BC4) &&
		(data_cs1[4] == K4P8G304EB_CS1_BC4)) {
		found = true;
		logmsg("soc-settings: K4P8G304EB two die DDR found \r\n");
	} else if ((data_cs0[1] == MT42L192M32D3_CS0_BC1) &&
		(data_cs1[1] == MT42L192M32D3_CS1_BC1) &&
		(data_cs0[4] == MT42L192M32D3_CS0_BC4) &&
		(data_cs1[4] == MT42L192M32D3_CS1_BC4)) {
		found = true;
		logmsg("soc-settings: MT42L192M32D3 two die DDR found \r\n");
	} else
		logmsg("soc-settings: no two die DDR found \r\n");

	return found;
}

/* Entry point */
int soc_settings_init(u32 boot_indication, u32 boot_status)
{
	struct ddr_data ddr_init_data;
	u32 status = BOOT_OK;
	bass_return_code ret;
	u32 i;
	bool ddr_two_die = false;

	logmsg("soc-setting: init API_VESRION 0x%x \r\n",*api_version);

	/* Initialize the bridge interface used by TEE Client API. */
	SECURITY_Init(BOOTLOGBASEADDR);

	logmsg("soc-settings: starting configurations... \r\n");
	db5500_init();

	if (db5500_regulator_init() < 0) {
		logerror("soc-settings: ab5500_regulator_init failed \r\n");
		return BOOT_INTERNAL_ERROR;
	}

	/* Send ABB_INIT request to prcmu */
	if (send_prcmu_abb_init() < 0) {
		logerror("soc-settings: Error in ABB_INIT request to prcmu\r\n");
		return BOOT_INTERNAL_ERROR;
	}

        /* Configure AVS settings */
	if (db5500_avs_configure() < 0){
		logerror("soc-settings: db5500_avs_configure failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	/* configure thsens data */
	if (db5500_thsens_configure() < 0){
		logerror("soc-settings: db5500_thsens_configure failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	set_frequency(&ddr_init_data.set_freq);
	ret = bass_prcmu_set_ddr_speed((u8 *)&ddr_init_data.set_freq,
						sizeof(struct frequency));
	if (ret != BASS_RC_SUCCESS) {
		logerror("set_ddr_speed failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	set_memory_speed(&ddr_init_data.ddr_clock);
	ret = bass_prcmu_set_memory_speed((u8 *)&ddr_init_data.ddr_clock,
						sizeof(struct memory_speed));
	if (ret != BASS_RC_SUCCESS) {
		logerror("set_memory_speed failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}
	logmsg("soc-settings: prcmu memory speed done \r\n");

	/* Put PRCMU in ApExec */
	if (bass_prcmu_set_ap_exec() != BASS_RC_SUCCESS) {
		logerror("set_ap_exec failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}
	/* Intilize DDR MRR */
	db5500_ddr_mrr_init(&ddr_init_data.init_mrr , false);

	/* Write Generic DDR settings into XP70 data memory */
	hook_dmc_ddr((struct ddr_init *)ddr_init_data.ddr_buf);

	ret = bass_prcmu_ddr_pre_init(ddr_init_data.ddr_buf,
				sizeof(struct ddr_init),
				(void *)&ddr_init_data.init_mrr,
				sizeof(struct ddr_init_mrr),
				ddr_init_data.ddr_regs,
				sizeof(ddr_init_data.init_mrr.cs));
	if (ret != BASS_RC_SUCCESS) {
		logerror("ddr_pre_init failed\r\n");
		return ret;
	}

	ddr_two_die = ddr_two_die_found(ddr_init_data.ddr_regs);
	if (!ddr_two_die) {
		logmsg("soc-settings: try for one die \r\n");
		db5500_ddr_mrr_init(&ddr_init_data.init_mrr , true);
		/* Write Generic DDR settings into XP70 data memory */
		hook_dmc_ddr((struct ddr_init *)ddr_init_data.ddr_buf);
		ret = bass_prcmu_ddr_pre_init(ddr_init_data.ddr_buf,
				sizeof(struct ddr_init),
				(void *)&ddr_init_data.init_mrr,
				sizeof(struct ddr_init_mrr),
				ddr_init_data.ddr_regs,
				sizeof(ddr_init_data.init_mrr.cs));
		if (ret != BASS_RC_SUCCESS) {
			logerror("ddr_pre_init failed\r\n");
			return ret;
		}
	}

	for (i = 0; i < 8; i++) {
		if ((i == 1) ||(i == 4))
			logmsg("soc-settings: CS0 BC %x %x CS1 BC %x  %x \r\n",
				i, ddr_init_data.ddr_regs[i],
				i, ddr_init_data.ddr_regs[i + 8]);
	}

	/**
	 * At present, we have checking only the K4P4G324EB one die CS.
	 * In future if we need to support one CS other than the below,
	 * we need to add in the one CS list.
	 */
	if (!ddr_two_die) {
		if ((ddr_init_data.ddr_regs[1] == K4P4G324EB_CS0_BC1) &&
			(ddr_init_data.ddr_regs[4] == K4P4G324EB_CS0_BC4)) {
			logmsg("soc-settings: K4P4G324EB one die DDR found \r\n");
		}
#ifdef U5500C
		hook_dmc_333mhz_onedie
			((struct ddr_init *)ddr_init_data.ddr_buf);
#else
		hook_dmc_266mhz_onedie
			((struct ddr_init *)ddr_init_data.ddr_buf);
#endif
	} else {
#ifdef U5500C
		hook_dmc_333mhz_twodie
			((struct ddr_init *)ddr_init_data.ddr_buf);
#else
		hook_dmc_266mhz_twodie
			((struct ddr_init *)ddr_init_data.ddr_buf);
#endif

	}
	
	ret = bass_prcmu_ddr_init(ddr_init_data.ddr_buf,
					sizeof(struct ddr_init));
	if (ret != BASS_RC_SUCCESS) {
		logerror("soc-settings: bass_prcmu_ddr_init failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

	if (db5500_clock_init() < 0) {
		logerror("soc-settings: db5500_clock_init failed\r\n");
		return BOOT_INTERNAL_ERROR;
	}

#if 0
	/* Disabled, cannot be done until address filtering is
	 * set-up by ISSW, which is done by xloader after soc_settings...
	 */

	/* DDR test */
	if (boot_indication == SECOND_BOOT_INDICATION) {
	    status = test_ddr_data_access();
	}
#endif
	logmsg("soc-settings: done!\r\n");

	return status;
}


