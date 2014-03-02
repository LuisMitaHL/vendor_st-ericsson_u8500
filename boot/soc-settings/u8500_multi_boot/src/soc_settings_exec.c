/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: WenHai Fang <wenhai.h.fang at stericsson.com>
 *  for ST-Ericsson.
 *
 * Origin: From the file u8500/src/soc_settings_core.c
 */

#include <config.h>
#include <gpio.h>
#include <io.h>
#include <log.h>
#include <peripheral.h>
#include <prcmu.h>
#include <memtest.h>
#include <soc_settings.h>
#include <await_battery_voltage.h>
#include <types.h>
#include <uart.h>
#include <stm.h>
#include <wait.h>
#include <ab8500.h>
#include <avs_calc.h>
#include <bass_app.h>
#include <security.h>
#include <ddr.h>


#define AVS_MAGIC_NUMBER	0x53564100

#define MASK_DEBUG_INFO		0xFFFFF8FF
#define	FLASHING_INDICATION	0xF0030002

/* Input from boot ROM to xloader, info on startup type */
struct xloader_info {
	void *a;
	u32   b;
	void *c;
	u32   d;
	u32   boot_indication;
	u32   e;
};

__attribute__((section(".type")))
const u32 exec_type = 2;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 exec_length = (u32)&linker_payload_length;

/*
 * Do not change the Order before(include) generic_4g_d2.c
 * Memory order after generic_4g_d2.c can be changed
 * Memory generic_4g_d2.c is the default settings if no match
 */
static struct ddr_data ddr_settings[] = {

/* Since pointers must be PC relative to work and pointers in a struct isn't */
#include "../ddr/generic_4g_d2.c"
#include "../ddr/elpida_4g_tyc0dh231234la10.c"
#include "../ddr/elpida_8g_edb4032b1pb.c"
#include "../ddr/elpida_8g_edb8132b3pb.c"
/*
 * Hynix 6g has 2 versions, use one of it
 * old version is H9TU33A6ADMC-LRKDM-104ES/112A
 *
 * "../ddr/hynix_6g_old_h9tu33a6admc_lrkdm_104es.c"
*/
#include "../ddr/hynix_6g_h9tu33a6admc_lrkdm_118es.c"
/*
 * Not used:
 *"../ddr/hynix_8g_h8tjr00x0mlr.c"
 */
#include "../ddr/micron_4g_mt42l128m32d2.c"
#include "../ddr/micron_6g_mt42l192m32d3.c"
#include "../ddr/micron_8g_mt42l256m32d2.c"
#include "../ddr/micron_8g_mt42l256m32d4.c"
#include "../ddr/nanya_8g_nt6tl256f32aq.c"
/*
 * Not used:
 * "../ddr/samsung_4g_k4p4g324ebagc1.c"
 */
#include "../ddr/samsung_6g_k4p6g304ebagc.c"
#include "../ddr/samsung_8g_k4p8g304ebagc1.c"
};

static struct config *config;

/* Turn on UART2 clock at PRCC level */
static void clock_init_uart(void)
{
	/* Enable bus clock */
	write32(CLKRST3_BASE, 0x00000040);
	/* Enable kernel clock */
	write32(CLKRST3_BASE + 8, 0x00000040);
}

/* Turn on all peripheral clocks at PRCC level */
static void clock_init(void)
{
	/*
	 * Try to enable everything, despite its parent
	 * clocks might not be running.
	 * TODO: Fix this mess!
	 */

	/* Enable all bus & kernel clocks of periph 1 */
	/* Except slimbus */
	write32(CLKRST1_BASE, 0xeff);
	write32(CLKRST1_BASE + 8, 0x6ff);

	/* Enable all bus & kernel clocks of periph 2 */
	/* Except pwl */
	write32(CLKRST2_BASE, 0xff7);
	write32(CLKRST2_BASE + 8, 0xfd);

	/* Enable all bus & kernel clocks of periph 3 */
	write32(CLKRST3_BASE, 0x1ff);
	write32(CLKRST3_BASE + 8, 0xfe);

	/* Enable all bus clocks of periph 5 */
	/* (There are no kernel clocks) */
	write32(CLKRST5_BASE, 3);

	/* Enable bus & kernel clocks of periph 6 */
	write32(CLKRST6_BASE, 0xff);
	write32(CLKRST6_BASE + 8, 0x1);
}

/* Configure the few GPIOs we need */
static void gpio_init(void)
{
	/*
	 * Settings for console UART
	 */
	switch (config->uart_gpiopins) {
	case UART_GPIOPINS_29_30:
		/* Enable UART2 on GPIO29+30 (alt mod C) */
		setbits32(GPIO0_BASE + GPIO_AFSLA, (1 << 30) | (1 << 29));
		setbits32(GPIO0_BASE + GPIO_AFSLB, (1 << 30) | (1 << 29));
		break;

	case UART_GPIOPINS_18_19:
		/* Enable UART2 on GPIO18+19 (alt mode B) */
		setbits32(GPIO0_BASE + GPIO_AFSLB, (1 << 19) | (1 << 18));
		break;
	default:
		logwarn("Unknown uart gpio pins setting!");
		return;
	}

	/* Release reset for all GPIOs */
	write32(PRCM_GPIO_RESETN_SET, 0xFFFF);
}

/* Modem STM trace configuration */
static void modem_stm_config(void)
{
	switch (config->modem_stm_gpiopins) {
	case MODEM_STM_GPIOPINS_DISABLE:
		break;
	case MODEM_STM_GPIOPINS_70_74:
		/* GPIO: Enable STM_MOD on GPIO70-74 (AltC3) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_SLPM,  (0x1F << 6));

		/* PRCM: Enable STM_MOD_CMD1: AltC3 on GPIO70-74 */
		setbits32(PRCM_GPIOCR, (1 << 11));
		break;

	case MODEM_STM_GPIOPINS_155_159:
		/* GPIO: Enable STM_MOD on GPIO155-159 (AltC1) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_SLPM,  (0x1F << 27));

		/* PRCM: Enable STM_MOD_CMD2: AltC1 on GPIO155-159 */
		setbits32(PRCM_GPIOCR, (1 << 13));
		break;

	case MODEM_STM_GPIOPINS_163_167:
		/* GPIO: Enable STM_MOD on GPIO163-167 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_SLPM,  (0x1F << 3));
		break;
	default:
		logwarn("Unknown modem stm gpio pins settings");
		return;
	}
}

/* Modem UART configuration */
static void modem_uart_config(void)
{
	switch (config->modem_uart_gpiopins) {
	case MODEM_UART_GPIOPINS_DISABLE:
		break;
	case MODEM_UART_GPIOPINS_75_76:
		/* GPIO: UARTMOD_RXD UARTMOD_TXD on GPIO75+76 (AltC3) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (3 << 11));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (3 << 11));
		setbits32(GPIO2_BASE + GPIO_SLPM,  (3 << 11));

		 /* PRCM: Enable UARTMOD_CMD0: AltC3 on GPIO75+76 */
		setbits32(PRCM_GPIOCR, (1 << 0));
		break;

	case MODEM_UART_GPIOPINS_153_154:
		/* GPIO: UARTMOD_RXD, UARTMOD_TXD on GPIO153+154 (AltC1) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (3 << 25));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (3 << 25));
		setbits32(GPIO4_BASE + GPIO_SLPM,  (3 << 25));

		/* PRCM: Enable UARTMOD_CMD1: AltC1 on GPIO153+154 */
		setbits32(PRCM_GPIOCR, (1 << 1));
		break;

	case MODEM_UART_GPIOPINS_161_162:
		/* GPIO: UARTMOD_RXD, UARTMOD_TXD on GPIO161+162 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (3 << 1));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (3 << 1));
		setbits32(GPIO5_BASE + GPIO_SLPM,  (3 << 1));
		break;
	default:
		logwarn("Unknown modem uart gpio pins settings");
		return;
	}
}

static void prcmu_stm_config(void)
{
	switch (config->prcmu_stm_gpiopins) {
	case PRCMU_STM_GPIOPINS_DISABLE:
		return;
	case PRCMU_STM_GPIOPINS_70_74:
		logdebug("PRCMU_STM_GPIOPINS_70_74");
		/* GPIO: Enable STM_APE on GPIO70-74 (AltC) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_SLPM, (0x1F << 6));
		break;

	case PRCMU_STM_GPIOPINS_155_159:
		logdebug("PRCMU_STM_GPIOPINS_155_159");
		/* GPIO: Enable STM_APE on GPIO155-159 (AltC) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_SLPM, (0x1F << 27));
		break;

	case PRCMU_STM_GPIOPINS_163_167:
		logdebug("PRCMU_STM_GPIOPINS_163_167");
		/* GPIO: Enable STM_APE on GPIO163-167 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_SLPM, (0x1F << 3));
		break;
	default:
		logwarn("Unknown prcmu stm gpio pins settings");
		return;
	}

	/*
	 * STM CR:
	 *
	 * SWAP_PRCMU = LSN first
	 * XCKDIV = fCLK/4
	 * DWNG = 4 bit output
	 * TSNTS = With Time Stamp
	 * PLP0, PLP1, PLP2 = Lowest priority
	 */
	write32(STM_BASE + STM_CONF_BASE + STM_CR,
		STM_CR_XCKDIV_4 | STM_CR_TSNTS);

	/* MIPI mode control register, set prcmu as HW mode */
	write32(STM_BASE + STM_CONF_BASE + STM_MMC, STM_MMC_HWNSW5);

	/* Trace enable control register, enable STM for prcmu */
	write32(STM_BASE + STM_CONF_BASE + STM_TER, STM_TER_TE5);
}

static unsigned long calculate_memory_size(u8 cs0_MR8, u8 cs1_MR8)
{
	u32 cs0_size = 0;
	u32 cs1_size = 0;
	u8 cs0_io_width;
	u8 cs1_io_width;
	u8 cs0_density;
	u8 cs1_density;
	u32 memsize;

	/*
	 * Density bits (bits 2-5) from DDR MR8 used to calculate the size
	 * of the memory chip.
	 * size = 2^cs0_density * 8MB according to DDR manuals.
	 * Multiplying by 8M is equal to shift left by 23.
	 *
	 * Multiply with IO_WIDTH, we only have 32 bit bus width so if
	 * IO_WIDTH is 16 then multiply with 2.
	 */

	cs0_density = (cs0_MR8 & 0x3C) >> 2;
	cs1_density = (cs1_MR8 & 0x3C) >> 2;

	cs0_io_width = 1 << ((cs0_MR8 & 0xC0) >> 6);
	cs1_io_width = 1 << ((cs1_MR8 & 0xC0) >> 6);

	/*
	 * Density value 0, is actually equal to 64Mb but we don't use such
	 * small memories so it is used as no memory
	 */

	if (cs0_density)
		cs0_size = ((1 << cs0_density) << 23) * cs0_io_width;

	if (cs1_density)
		cs1_size = ((1 << cs1_density) << 23) * cs1_io_width;

	memsize = cs0_size + cs1_size;
	loginfo("Memory size=%x (%u MB), CS0 (%u MB) CS1 (%u MB)", memsize, memsize >> 20,
			cs0_size >> 20,cs1_size >> 20);

	return memsize;
}

static u32 ddr_test(u32 memsize)
{
	int res;
	int reenable_cache = 0;

	logdebug("Starting DDR test...");

	/* Disable cache */
	if (config->memtest_disable_cache) {
		/*
		 * We will only disable the dcache as we are only interested in
		 * testing data memory.
		 * Sadly, we can't disable the L2 cache in the insecure world
		 * of u8500
		 */
		logdebug("Checking L1 dcache status = %u.", l1dcache_status());
		if (l1dcache_status()) {
			reenable_cache = 1;
			l1dcache_disable();
			/*
			 * No need to flush the cache, we have not yet used
			 * the main DDR we are about to test.
			 */
			logdebug("Disabled L1 dcache.");
		}
	}

	/* Run the memory test */
	res = memtest_main(0x0, memsize);

	/* Re-enable cache */
	if (config->memtest_disable_cache && reenable_cache) {
		l1dcache_enable();
		logdebug("Re-enabled L1 dcache");
	}

	if (res) {
		logerror("DDR test FAILED! Result=%u", res);
		return BOOT_INTERNAL_ERROR;
	}

	logdebug("DDR test passed successfully.");
	return BOOT_OK;
}

void hwi2c_read(u16 addr, u8 *value)
{
	t_ReqMb5 reqmb5;
	t_AckMb5 ackmb5;
	u8 regbank = addr >> 8;
	u8 regaddr = addr & 0xFF;

	reqmb5.CompletField = read32(TCDM_BASE+0xE44);
	reqmb5.un.I2CopType = I2CRead;
	reqmb5.un.SlaveAdd = (regbank & 0x1F) | 0x20;
	reqmb5.un.HwGCEn = 0;
	reqmb5.un.StopEn = 1;
	reqmb5.un.RegAdd = regaddr;
	reqmb5.un.RegVal = 0;
	write32(TCDM_BASE+0xE44, reqmb5.CompletField);

	/* Check IT for ackMB5 is 0*/
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		0x0, "hwi2c_read");

	/* Send mb5 (IT17) interrupt */
	write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT17);

	/* Check IT for ackMB5 */
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		PRCM_MBOX_CPU_IT17, "hwi2c_read");

	ackmb5.CompletField = read32(TCDM_BASE+0xDF4);
	*value = ackmb5.un.RegVal;

	/* Clear IT1 ackmb5 */
	write32(PRCM_ARM_IT1_CLR, PRCM_MBOX_CPU_IT17);
}

void hwi2c_write(u16 addr, u8 value)
{
	t_ReqMb5 reqmb5;
	u8 regbank = addr >> 8;
	u8 regaddr = addr & 0xFF;

	reqmb5.CompletField = read32(TCDM_BASE+0xE44);
	reqmb5.un.I2CopType = I2CWrite;

	reqmb5.un.SlaveAdd = (regbank & 0x1F) | 0x20;

	reqmb5.un.HwGCEn = 0;
	reqmb5.un.StopEn = 1;
	reqmb5.un.RegAdd = regaddr;
	reqmb5.un.RegVal = value;
	write32(TCDM_BASE+0xE44, reqmb5.CompletField);

	/* Check IT for ackMB5 is 0*/
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		0x0, "hwi2c_write");

	/* Send mb5 (IT17) interrupt */
	write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT17);

	/* Check IT for ackMB5 */
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		PRCM_MBOX_CPU_IT17, "hwi2c_write");

	/* Clear IT1 ackmb5 */
	write32(PRCM_ARM_IT1_CLR, PRCM_MBOX_CPU_IT17);
}

void hwi2c_masked_write(u16 addr, u8 bitmask, u8 bitval)
{
	u8 regval;

	hwi2c_read(addr, &regval);
	regval = (~bitmask & regval) | (bitmask & bitval);
	hwi2c_write(addr, regval);
}

static void version_write(void)
{
	write32(BOOT_INFO_BACKUPRAM1, config->board_version);

	logdebug("Boardversion set to 0x%x",
		config->board_version);
}

static void avs_info_print(u32 *avs_fuses)
{
	u32 FVR21_data = avs_fuses[0];
	u32 FVR22_data = avs_fuses[1];

	if (has_avs()) {
		if (FVR21_data == 0 && FVR22_data == 0)
			loginfo("AVS Enable, but still use default settings");
		else
			loginfo("AVS Enable, FVR21 value = : %x, FVR22 value = : %x",
				FVR21_data, FVR22_data);
	} else {
		if (FVR21_data == 0 && FVR22_data == 0)
			loginfo("AVS Disable, using default settings");
		else
			loginfo("AVS Disable, FVR21 value = : %x, FVR22 value = : %x",
				FVR21_data, FVR22_data);

	}
}

static void modem_start(void)
{
	int			res;

	logdebug("Modem: starting modem...");

	/*
	 * Modem start-up
	 */
	/* Needed to send SYSCLK_OK to modem,
	 * but should be corrected in fw xP70
	 */
	/*PRCM_SYSCLKOK_DELAY (val * 32K) 0 no delay */
	write32(PRCM_SYSCLKOK_DELAY, PRCM_SYSCLKOK_NO_DELAY);

	/* Release Modem PURX and SWReset */
	write32(PRCM_MOD_RESETN_SET_REG,
		PRCM_MOD_RESETN_SET_REG_0 | PRCM_MOD_RESETN_SET_REG_1);

	/* Ack SWReset from Modem,
	 * in case of reboot due to this reason
	 */
	write32(PRCM_MOD_SWRESET_IRQ_ACK, PRCM_MOD_SWRESET_IRQ_ACK_SET);
	write32(PRCM_MOD_SWRESET_IRQ_ACK, PRCM_MOD_SWRESET_IRQ_ACK_CLR);

	res = wait_long(PRCM_MOD_AWAKE_STATUS,
		PRCM_MOD_AWAKE_STATUS_COREPD_AWAKE,
		PRCM_MOD_AWAKE_STATUS_COREPD_AWAKE,
		"PRCM_MOD_AWAKE_STATUS_COREPD_AWAKE");
	if (res)
		logdebug("Waiting for "
			"'modem PRCM_MOD_AWAKE_STATUS_COREPD_AWAKE' "
			"was aborted. Is modem flashed/loaded? "
			"Continuing with boot.");
}

static int transistor_fs_check(const u8 vbb)
{
	/* Check transistor fast or slow
	 * 0 - Very Fast, 1 - Fast
	 * 2 - Slow, 3 - Very Slow
	 */
	if ((vbb == 0x4) || (vbb == 0x5) || (vbb == 0x6) || (vbb == 0x7))
		/* <= -100mV */
		return 0;
	else if (vbb == 0x0) /* 0mv */
		return 1;
	else if ((vbb == 0x1) || (vbb == 0x2)) /* 100mV and 200mV */
		return 2;
	else /* 300mV */
		return 3;
}

static void process_detection_print(const int vbbn_fs, const int vbbp_fs,
					const u8 varmsel1, const u8 vbbn,
					const u8 vbbp)
{
	char *vbbnp_info[8] = {"", "", "", "", "", "", "", ""};
	char *varmsel1_info = "", *db_info = "", *bar_info = "";
	char *np_trans[4] = {"", "", "", ""};

	vbbnp_info[0] = "0mV"; vbbnp_info[1] = "+100mV";
	vbbnp_info[2] = "+200mV"; vbbnp_info[3] = "+300mV";
	vbbnp_info[4] = vbbnp_info[5] = "-100mV";
	vbbnp_info[6] = vbbnp_info[7] = "-100mV";

	np_trans[0] = "Very Fast"; np_trans[1] = "Fast";
	np_trans[2] = "Slow"; np_trans[3] = "Very Slow";

	/* Range [1.275, 1.35] Corresponds to [0x2E, 0x34] */
	if (varmsel1 <= 0x2E) {
		bar_info = "[>>>>>>>]";
		if (varmsel1 < 0x2E)
			varmsel1_info = "less than 1.275V";
		else
			varmsel1_info = "1.275V";
	} else if (varmsel1 == 0x2F) {
		bar_info = "[>>>>>>.]";
		varmsel1_info = "1.2875V";
	} else if (varmsel1 == 0x30) {
		bar_info = "[>>>>>..]";
		varmsel1_info = "1.3V";
	} else if (varmsel1 == 0x31) {
		bar_info = "[>>>>...]";
		varmsel1_info = "1.3125V";
	} else if (varmsel1 == 0x32) {
		bar_info = "[>>>....]";
		varmsel1_info = "1.325V";
	} else if (varmsel1 == 0x33) {
		bar_info = "[>>.....]";
		varmsel1_info = "1.3375V";
	} else if (varmsel1 >= 0x34) {
		bar_info = "[>......]";
		if (varmsel1 == 0x34)
			varmsel1_info = "1.35V";
		else
			varmsel1_info = "above 1.35V";
	}

	if ((vbbn_fs <= 1) && (vbbp_fs <= 1))
		if ((vbbn_fs == 0) && (vbbp_fs == 0))
			db_info = "Very Fast";
		else
			db_info = "Fast";
	else if ((vbbn_fs <= 1) && (vbbp_fs > 1))
		db_info = "Unbalanced FS";
	else if ((vbbn_fs > 1) && (vbbp_fs <= 1))
		db_info = "Unbalanced SF";
	else if ((vbbn_fs > 1) && (vbbp_fs > 1)) {
		if ((vbbn_fs == 3) && (vbbp_fs == 3))
			db_info = "Very Slow";
		else
			db_info = "Slow";
	}

	loginfo("vmin = %s, vbbp = %s, vbbn = %s",
			varmsel1_info, vbbnp_info[vbbp], vbbnp_info[vbbn]);

	loginfo("%s Parts %s(%s for N Transistor, %s for P transistor)",
			 db_info,  bar_info, np_trans[vbbn_fs],
			 np_trans[vbbp_fs]);
}

static void process_detection(u32 *avs_fuses)
{
	u8 varmsel1_value, vbbn, vbbp;
	int vbbn_fs, vbbp_fs;

	hwi2c_read(AB8500_VARMSEL1, &varmsel1_value);

	vbbn = (avs_fuses[0] >> 5) & 0x7;
	vbbp = (avs_fuses[0] >> 8) & 0x7;

	vbbn_fs = transistor_fs_check(vbbn);
	vbbp_fs = transistor_fs_check(vbbp);

	process_detection_print(vbbn_fs, vbbp_fs, varmsel1_value, vbbn, vbbp);
}

/* phase out the use of this function */
static void regulator_config(u8 mask, u16 regulator_addr, u8 bit_val)
{
	/* change order of arguments */
	hwi2c_masked_write(regulator_addr, mask, bit_val);
}

static void ab8500_part_init(void)
{
	/*
	 * AB8500 update to config2 to be accessible in HWI2C
	 * => Bank 4 registers will be accessible with APE HWI2C
	 * => Sim Control registers will be accessible with Modem I2C
	 * => other registers will be accessible with APE HWI2C
	 */
	hwi2c_write(AB8500_REGUSERIALCTRL1, 0x02);
}

static void ab8500_pre_init(void)
{
	if (config->board_version != BOARD_VERSION_520)
		return;

	/* Setup GPIO 13 and 50 for u8520 */

	/* GpioSel2 @0x1001 */
	regulator_config(0x10, AB8500_GPIOSEL2, 0x10);
	/* GpioDir2 @0x1011 */
	regulator_config(0x10, AB8500_GPIODIR2, 0x0);
	/* GpioPud2 @0x1031 */
	regulator_config(0x10, AB8500_GPIOPUD2, 0x10);

	/* GpioSel7 @0x1006 */
	regulator_config(0x2, AB8500_GPIOSEL7, 0x2);
	/* GpioDir7 @0x1016 */
	regulator_config(0x2, AB8500_GPIODIR7, 0x0);
	/* GpioPud7 @0x1036 */
	regulator_config(0x2, AB8500_GPIOPUD7, 0x2);

	/* Mask interrupt for GPIO 13 and 50 */
	/* ITMask7 @0x0E46 */
	regulator_config(0x80, AB8500_ITMASK7, 0x80);
	/* ITMask9 @0x0E48 */
	regulator_config(0x80, AB8500_ITMASK9, 0x80);
	/* ITMask13 @0x0E4C */
	regulator_config(0x2, AB8500_ITMASK13, 0x2);
	/* ITMask14 @0x0E4D */
	regulator_config(0x2, AB8500_ITMASK14, 0x2);

}

static void ab8500_init(void)
{

	/* STw4500Ctrl2 @0x181 */
	hwi2c_write(AB8500_STW4500CTRL2, 0xF7);

	/* VsimSysClkCtrl @0x233 (bit0 = 1) */
	regulator_config(0x1, AB8500_VSIMSYSCLKCTRL, 0x1);

	/* ReguRequestCtrl1 @0x303 (bit0-7 = 0x0) */
	hwi2c_write(AB8500_REGUREQUESTCTRL1, 0x0);

	/* ReguRequestCtrl2 @0x304 (bit0-3 = 0x0) */
	regulator_config(0xF, AB8500_REGUREQUESTCTRL2, 0x0);

	/* ReguSysClkReq1HPValid1 @0x307 (bit0-2 bit4 = 0x17) */
	regulator_config(0x17, AB8500_REGUSYSCLKREQ1HPVALID1, 0x17);

	/* ReguSysClkReq1HPValid2 @0x308 (bit0-3 = 0x0) */
	regulator_config(0xF, AB8500_REGUSYSCLKREQ1HPVALID2, 0x0);

	/* ReguHwHPReq1Valid1 @0x309 (bit0-2 bit4 = 0x2) */
	regulator_config(0x17, AB8500_REGUHWHPREQ1VALID1, 0x2);

	/* ReguHwHPReq1Valid2 @0x30A (bit3 = 0x0)*/
	regulator_config(0x8, AB8500_REGUHWHPREQ1VALID2, 0x0);

	/* ReguHwHPReq2Valid1 @0x30B (bit0-2 bit4 = 0x0) */
	regulator_config(0x17, AB8500_REGUHWHPREQ2VALID1, 0x0);

	/* ReguHwHPReq2Valid2 @0x30C (bit8 = 0x0) */
	regulator_config(0x8, AB8500_REGUHWHPREQ2VALID2, 0x0);

	/* ReguSwHPReqValid1 @0x30D (bit0-3 bit4 bit6 = 0x0) */
	regulator_config(0x5F, AB8500_REGUSWHPREQVALID1, 0x0);

	/* ReguSwHPReqValid2 @0x30E (bit5 = 0x0) */
	regulator_config(0x20, AB8500_REGUSWHPREQVALID2, 0x0);

	/* Varmregu1 @0x400  */
	hwi2c_write(AB8500_VARMREGU1, 0x01);

	/* Varmregu2 @0x401 */
	hwi2c_write(AB8500_VARMREGU2, 0x45);

	/* Vaperegu @0x402 */
	hwi2c_write(AB8500_VAPEREGU, 0x01);

	/* Vsmps1regu @0x403 (bit0-5 = 0x6) */
	regulator_config(0x3F, AB8500_VSMPS1REGU, 0x6);

	/* Vsmps2regu @0x404 (bit0-5 = 0x6) */
	regulator_config(0x3F, AB8500_VSMPS2REGU, 0x6);

	/* Vsmps3regu @0x405 (bit0-5 = 0x2) */
	regulator_config(0x3F, AB8500_VSMPS3REGU, 0x2);

	/* VpllVanaregu @0x406 (bit0-1 = 0x2) */
	regulator_config(0x3, AB8500_VPLLVANAREGU, 0x2);

	/* VRF1Vaux3regu @0x40A (bit2-3 = 0x8) */
	regulator_config(0xC, AB8500_VRF1AUX3REGU, 0x8);

	/* Vsmps1Sel1 @0x413 */
	hwi2c_write(AB8500_VSMPS1SEL1, 0X28);
	/* Vsmps1Sel2 @0x0x414 */
	hwi2c_write(AB8500_VSMPS1SEL2, 0X28);
	/* Vsmps1Sel3 @0x415 */
	hwi2c_write(AB8500_VSMPS1SEL3, 0X28);
	/* Vsmps2Sel1 @0x417 */
	hwi2c_write(AB8500_VSMPS2SEL1, 0X39);
	/* Vsmps2Sel2 @0x418 */
	hwi2c_write(AB8500_VSMPS2SEL2, 0X39);
	/* Vsmps2Sel3 @0x419 */
	hwi2c_write(AB8500_VSMPS2SEL3, 0X39);

	/* ReguCtrlDisch @0x443 (bit0-1 = 0x0) */
	regulator_config(0x3, AB8500_REGUCTRLDISCH, 0x0);

	/* ReguCtrlDisch2 @0x444 (bit0 bit3) */
	regulator_config(0x9, AB8500_REGUCTRLDISCH2, 0x0);

	if (config->board_version == BOARD_VERSION_520) {

		/* Bank12Access @0x1100 */
		regulator_config(0x01, AB8500_BANK12ACCESS, 0x01);

		/* DebugVarmRegu @0x120C */
		hwi2c_write(AB8500_DEBUGVARMREGU, 0x30);

		/* DebugVmodRegu @0x120D */
		hwi2c_write(AB8500_DEBUGVMODREGU, 0x30);

		/* DebugVapeRegu @0x120E */
		hwi2c_write(AB8500_DEBUGVAPEREGU, 0x30);

		/* DebugVsmps1Regu @0x120F */
		hwi2c_write(AB8500_DEBUGVSMPS1REGU, 0x30);

		/* DebugVsmps2Regu @0x1210 */
		hwi2c_write(AB8500_DEBUGVSMPS2REGU, 0x30);

		/* DebugVsmps3Regu @0x1211 */
		hwi2c_write(AB8500_DEBUGVSMPS3REGU, 0x30);
	}

}

static u32 ddr_data_is_matching(struct ddr_data *ddr_data,
				const u8 ddr_regs[], const int ddr_regs_size)
{
	int n;

	for (n = 0; n < ddr_regs_size; n++) {
		/*
		 * "* 2" because for each register there's one byte of
		 * bitpattern and one byte of mask.
		 */
		u8 reg = ddr_data->id_regs_and_mask[n * 2];
		u8 mask = ddr_data->id_regs_and_mask[n * 2 + 1];

		if ((ddr_regs[n] & mask) != (reg & mask))
			return 0;
	}
	return 1;
}

__attribute__((section(".entry_function")))
u32 soc_settings_exec(const u8 *bin, u32 bin_len,
		      const struct xloader_info *boot_status)
{
	u32			upass = BOOT_OK;
	bass_return_code	ret;
	t_AVSData		avs_data;
	u8			ddr_regs[DDR_REGISTERS_LEN];
	struct ddr_data		*ddr_data;
	t_DDRInitData		ddr_init_data;
	u32			fuses[2] = { 0, 0 };
	u32			n;
	int			maxopp_enable = 0;
	u8			usb_charging_enabled;
	u32			memory_size;
	u8			one_die_config = FALSE;
	u8			avs_changed = FALSE;
	u32			i;

	/* Initialize the bridge interface used by TEE Client API. */
	SECURITY_Init(BOOTLOGBASEADDR);

	/* The followings should be done before AVS settings */

	/* Clear AckMB interrupt */
	write32(PRCM_ARM_IT1_CLR, PRCM_ARM_IT1_CLR_ALL);

	config = get_config();

	ab8500_part_init();

	/* Enable peripheral clocks for UART prints */
	clock_init_uart();
	uart_init();

	/* Print SoC settings version + build date and time for soc_setting_exec.c */
	avs_init(__DATE__" "__TIME__, (u32)bin, bin_len);

	if ((boot_status->boot_indication & MASK_DEBUG_INFO) !=
	    FLASHING_INDICATION) {
		/* enable charging as soon as possible */
		usb_charging_enabled = enable_usb_charging();

		if (usb_charging_enabled)
			await_required_battery_voltage();
		else
			vbat_ok_to_boot_or_reset();
	} else {
		loginfo("Xloader says flashing boot\n");
	}

	ab8500_pre_init();
	ab8500_init();

	/* Enable all peripheral clocks */
	clock_init();

	/*
	 * Calculate AVS settings based on FUSES and other AVS
	 * parameters.
	 */

	ret = bass_get_avs_fuses((u8 *)fuses, sizeof(fuses));
	if (ret != BASS_RC_SUCCESS) {
		loginfo("get_avs_fuses failed: %x", ret);
		return ret;
	}

	if (has_avs() && is_fused(fuses)) {
		avs_calculate_settings(fuses, &avs_data);
	} else {
		no_avs_calculate_settings(fuses, &avs_data);
	}

	/* Check if phone is about to dump */
	if ((read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xDEAD ||
		read32(PRCM_RESET_STATUS) & PRCM_A9_CPU_WATCHDOG_RESET) {

		loginfo("Crash occured, check for old AVS values and save new ones...");
		/* Check if old AVS data exists */
		if (AVS_MAGIC_NUMBER == (read32(AVS_BACKUP_BACKUPRAM1) &
					 0xFFFFFF00)) {
			loginfo("Old AVS data found in Backup RAM");
			for (n=0; n< sizeof(t_AVSData);n++) {
				write8(AVS_BACKUP_BACKUPRAM1 + 18 + n,
				       ((u8 *)&avs_data)[n]);
				if ( !avs_changed && (((u8 *)&avs_data)[n]!=
				       read8(AVS_BACKUP_BACKUPRAM1 + 4 + n))) {
					avs_changed = TRUE;
				}
			}
			if (avs_changed){
				loginfo("AVS values check: NOK");
			} else {
				loginfo("AVS values check: OK");
			}
		}
	} else {
		/*
		 * Clear AVS_MAGIC_NUMBER be sure that we compare
		 * to fresh ones next time
		 */
		write32(AVS_BACKUP_BACKUPRAM1, 0x0);
	}
	/* Apply the AVS settings */
	ret = bass_prcmu_apply_avs_settings((u8 *)&avs_data, sizeof(t_AVSData));
	if (ret != BASS_RC_SUCCESS) {
		loginfo("apply_avs_settings failed");
		return ret;
	}

	/* Put PRCMU in ApExec */
	ret = bass_prcmu_set_ap_exec();
	if (ret != BASS_RC_SUCCESS) {
		loginfo("set_ap_exec failed");
		return ret;
	}

	/*
	 * Do DDR_PreInit to identify memory.
	 */


	/*
	 * Set up the list of registers that we can read
	 * from memory. The first time we set it with this
	 * pattern to be able to detect if there is a memory
	 * mounted at CS1.
	 */
	ddr_init_data.CfgAdd[0] = 0;  /* Device info */
	ddr_init_data.CfgAdd[1] = 0;  /* Device info */
	ddr_init_data.CfgAdd[2] = 0;  /* Device info */
	ddr_init_data.CfgAdd[3] = 0;  /* Device info */
	ddr_init_data.CfgAdd[4] = 5;  /* BC-1 : Manufacturer ID */
	ddr_init_data.CfgAdd[5] = 6;  /* BC-2 : Revision ID1 */
	ddr_init_data.CfgAdd[6] = 7;  /* BC-3 : Revision ID2 */
	ddr_init_data.CfgAdd[7] = 8;  /* BC-4 : I/O width + Density + Type */

	/* Clear all register to be sure to only read the one we asking for */
	for (n = 0; n < sizeof(ddr_init_data.cs[0].DataCS) /
			sizeof(ddr_init_data.cs[0].DataCS[0]); n++) {
		ddr_init_data.cs[0].DataCS[n] = 0x00;
		ddr_init_data.cs[1].DataCS[n] = 0x00;
	}

	/*
	 * Read the same register four times from CS0 before
	 * reading registers from CS1. This is how we detect one
	 * or two die memory configuration
	 */
	ddr_init_data.cs[0].DataCS[0] = 0xff;
	ddr_init_data.cs[0].DataCS[1] = 0xff;
	ddr_init_data.cs[0].DataCS[2] = 0xff;
	ddr_init_data.cs[0].DataCS[3] = 0xff;
	ddr_init_data.cs[1].DataCS[4] = 0xff;
	ddr_init_data.cs[1].DataCS[5] = 0xff;
	ddr_init_data.cs[1].DataCS[6] = 0xff;
	ddr_init_data.cs[1].DataCS[7] = 0xff;

	ddr_data = &ddr_settings[0];
	/* Do first pre_init and read out MR from memory */
	ret = bass_prcmu_ddr_pre_init((uint8_t *)ddr_data->settings,
			sizeof(ddr_data->settings), (void *)&ddr_init_data,
			sizeof(ddr_init_data), ddr_regs, sizeof(ddr_regs));
	if (ret != BASS_RC_SUCCESS) {
		loginfo("ddr_pre_init failed");
		return ret;
	}

	/* Check if all values from CS1 are the same, if so then there is no memory monted at CS1 */
	if ((ddr_regs[9] == ddr_regs[11]) && (ddr_regs[9] == ddr_regs[13])) {
		one_die_config = TRUE;
		loginfo("No memory mounted on CS1");
	}

	/* Now select to read all Mode registers from mounted memory */
	for (n = 0; n < sizeof(ddr_init_data.cs[0].DataCS) /
		 sizeof(ddr_init_data.cs[0].DataCS[0]); n++) {
		ddr_init_data.cs[0].DataCS[n] = 0xFF;
		if (one_die_config)
			ddr_init_data.cs[1].DataCS[n] = 0x00;
		else
			ddr_init_data.cs[1].DataCS[n] = 0xFF;
	}

	/* Set up a new register list to be compliant with old solution*/
	ddr_init_data.CfgAdd[0] = 0;  /* Device info */
	ddr_init_data.CfgAdd[1] = 5;  /* BC-1 : Manufacturer ID */
	ddr_init_data.CfgAdd[2] = 6;  /* BC-2 : Revision ID1 */
	ddr_init_data.CfgAdd[3] = 7;  /* BC-3 : Revision ID2 */
	ddr_init_data.CfgAdd[4] = 8;  /* BC-4 : I/O width + Density + Type */
	ddr_init_data.CfgAdd[5] = 0;  /* Not used */
	ddr_init_data.CfgAdd[6] = 0;  /* Not used */
	ddr_init_data.CfgAdd[7] = 0;  /* Not used */

	/*
	 * Use the default settings for the DDR
	 * ddr_init_data set to only read Mode Reg for CS0
	 */

	/* Second pre_init to get the Mode registers that we want to use for matching */
	ret = bass_prcmu_ddr_pre_init((uint8_t *)ddr_data->settings,
			sizeof(ddr_data->settings), (void *)&ddr_init_data,
			sizeof(ddr_init_data), ddr_regs, sizeof(ddr_regs));
	if (ret != BASS_RC_SUCCESS) {
		loginfo("ddr_pre_init failed");
		return ret;
	}

	/*
	 * Search for optimized DDR settings.
	 */
	for (i = 1; i < (sizeof(ddr_settings) / sizeof(struct ddr_data)); i++) {
		if (ddr_data_is_matching(&ddr_settings[i],
					ddr_regs, sizeof(ddr_regs))) {
			ddr_data = &ddr_settings[i];
			break;
		}
	}

	if (i == (sizeof(ddr_settings) / sizeof(struct ddr_data)))
		loginfo("No better DDR settings found, generic will be used");

	loginfo("Vendor ID = %x, CS0 density = %x, CS1 density = %x",
		ddr_regs[2], ddr_regs[8], ddr_regs[9]);
	loginfo("CS0 revision ID1 = %x, CS0 revision ID2 = %x",
		ddr_regs[4], ddr_regs[6]);

	if (!one_die_config)
		loginfo("CS1 revision ID1 = %x, CS1 revision ID2 = %x",
			ddr_regs[5], ddr_regs[7]);

	/* Set ddr frequency before doing ddr_init */
	ret = bass_prcmu_set_ddr_speed((u8 *)&ddr_data->frequency,
				DDR_FREQUENCY_LEN);
	if (ret != BASS_RC_SUCCESS) {
		loginfo("set_ddr_speed failed");
		return ret;
	}

	ret = bass_prcmu_ddr_init((uint8_t *)ddr_data->settings,
				sizeof(ddr_data->settings));
	if (ret != BASS_RC_SUCCESS) {
		loginfo("ddr_init failed");
		return ret;
	}

	gpio_init();

	avs_info_print(fuses);

	/*
	 * Configure Modem STM trace and UART
	 */
	modem_stm_config();
	modem_uart_config();

	/*
	 * Configure PRCMU STM trace
	 */
	prcmu_stm_config();

	/*
	 * Configure Modem Start up
	 */
	if (config->modem_start)
		modem_start();

	version_write();

	/* Calculate memory size from what was read from MR registers */
	memory_size = calculate_memory_size(ddr_regs[8],ddr_regs[9]);

	if ((read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xDEAD ||
	    (read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xD0D0 ||
	    (read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xD0D1 ||
	    (read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xD0D2)
	{
			loginfo("Not performing DDR Test.");
			upass = BOOT_OK;
	} else {
		/* DDR test */
		upass = ddr_test(memory_size);

		if (upass == BOOT_OK)
			loginfo("DDR Test Ended successfully.");
		else
			logerror("DDR Test Ended with error.");
	}
	/*  Check if sample is 1GHz capable */
	if ((config->overclock > OVERCLOCK_800MHZ) &&
	    (is_fused(fuses) && is_maxopp_fused(fuses)))
		maxopp_enable = 1;

	if (maxopp_enable) {
		t_ReqMb1 mb1;
		volatile t_Header *header;
		volatile t_AckMb1 *mbAck;
		header	= (t_Header *)(TCDM_BASE + PRCM_MBOX_HEADER);

		loginfo("Requesting MAX OPP from PRCMU");

		wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT11, 0x00, "MB1");

		header->Req.un.Mb1 = ArmApeOppH;

		mb1.dvfs.un.ArmOpp = ArmMaxOpp;
		mb1.dvfs.un.ApeOpp = ApeNoChg;
		mb1.dvfs.un.BoostMode = 0;

		write32(TCDM_BASE + PRCM_REQ_MB1_ARM_OPP,
			mb1.dvfs.un.ArmOpp);

		write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT11);

		/* Check IT for ackMB1 */
		wait_long(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT11,
			  PRCM_MBOX_CPU_IT11, "1GHZ_MB1");

		/* Check status of last request */
		mbAck = (t_AckMb1 *)(TCDM_BASE + PRCM_ACK_MB1);
		if (mbAck->dvfs.un.ArmDvfsStatus == DVFSOk) {
			loginfo("Set MAX OPP OK");
			/*
			* Configure Process Detection
			*/
			if (config->process_detection && has_avs())
				process_detection(fuses);
		} else {
			loginfo("Error setting MAX OPP: %x",
				mbAck->dvfs.un.ArmDvfsStatus);
		}
		/* Clear IT1 ackmb1 */
		write32(PRCM_ARM_IT1_CLR, PRCM_MBOX_CPU_IT11);
	}
	return upass;
}
