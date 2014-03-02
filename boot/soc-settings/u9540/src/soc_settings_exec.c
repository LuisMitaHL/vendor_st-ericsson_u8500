/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: David Paris <david.paris at stericsson.com>
 *  for ST-Ericsson.
 *
 * Origin: From the file u8500_multi_boot/src/soc_settings_exec.c
 */

#include <config.h>
#include <gpio.h>
#include <io.h>
#include <log.h>
#include <peripheral.h>
#include <prcmu.h>
#include <memtest.h>
#include <soc_settings.h>
#include <types.h>
#include <uart.h>
#include <stm.h>
#include <wait.h>
#include <avs_calc.h>
#include <bass_app.h>
#include <security.h>
#include <cpu.h>

/* From DPA */
#include <opp_init.h>
#include <prcmuApi.h>
#include <xp70_MemMap.h>

#include <nomadik_registers.h>
#include <nomadik_mapping.h>

#ifdef IMAD_CONFIG
#include <imad.h>
#endif

/* Round up the even multiple of size, size has to be a multiple of 2 */
#define ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))

/* Declare extern API */
extern u8 AB9540_init_fct(u8*);

/* !!! In final IMAD management, IMAD table should be genareted as a separate binary file as it must be
   signed using the PRCMU private key. Then it will have to be read from by soc-settings:
   - concatenated to DDR/AB/OPP ?
   - extra bin paremeter passed to soc-settings-exec ?
   - completey independant binary fully managed by soc-settings ?
*/

struct section_head {
	enum section_type type;
	u32 len;
	const u8 *bin;
};

struct ddr_data {
	/*
	 * One bit pattern and one mask for each DDR0_cfg register,
	 * hence the "* 2" below.
	 */
	u8 ddr0_id_regs_and_mask[DDR_REGISTERS_LEN * 2];
	t_DDRInit ddr0_cfgs_settings;
	u32 ddr0_frequency;

	u8 ddr1_id_regs_and_mask[DDR_REGISTERS_LEN * 2];
	t_DDRInit ddr1_cfgs_settings;
	u32 ddr1_frequency;
};

struct custom_config {
	u32 DDRInterlv;
	sPrcmuApi_securePasrData_t PASRSegMask;
	u32 Lvl0Firewall[7];
};

__attribute__((section(".type")))
const u32 exec_type = SECTION_TYPE_EXECUTABLE;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 exec_length = (u32)&linker_payload_length;

/* Turn on all peripheral clocks at PRCC level */
static void clock_init(void)
{
	unsigned i;
	static u32 clkrst_base[] = {
		CLKRST1_BASE,
		CLKRST2_BASE,
		CLKRST3_BASE,
		CLKRST5_BASE,
		CLKRST6_BASE,
	};

	for (i = 0; i < ARRAY_SIZE(clkrst_base); i++) {
		/* Enable bus clock */
		write32(clkrst_base[i], 0xFFFFFFFF);
		/* Enable kernel clock */
		write32(clkrst_base[i] + 8, 0xFFFFFFFF);
	}
}

/* Configure the few GPIOs we need */
static void gpio_init(void)
{
	/*
	 * Settings for console UART
	 */
	switch (CONFIG_UART_GPIOPINS) {
	case OPTION_UART_GPIOPINS_29_30:
		/* Enable UART2 on GPIO29+30 (alt mod C) */
		setbits32(GPIO0_BASE + GPIO_AFSLA, (1 << 30) | (1 << 29));
		setbits32(GPIO0_BASE + GPIO_AFSLB, (1 << 30) | (1 << 29));
		break;

	case OPTION_UART_GPIOPINS_18_19:
		/* Enable UART2 on GPIO18+19 (alt mode B) */
		setbits32(GPIO0_BASE + GPIO_AFSLB, (1 << 19) | (1 << 18));
		break;
	}

	/* Release reset for all GPIOs */
	write32(PRCM_GPIO_RESETN_SET, 0xFFFF);
}

/* Modem STM trace configuration */
static void modem_stm_config(void)
{
	switch (CONFIG_MODEMSTM_GPIOPINS) {
	case OPTION_MODEMSTM_GPIOPINS_70_74:
		/* GPIO: Enable STM_MOD on GPIO70-74 (AltC3) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_SLPM,  (0x1F << 6));

		/* PRCM: Enable STM_MOD_CMD1: AltC3 on GPIO70-74 */
		setbits32(PRCM_GPIOCR, (1 << 11));
		break;

	case OPTION_MODEMSTM_GPIOPINS_155_159:
		/* GPIO: Enable STM_MOD on GPIO155-159 (AltC1) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_SLPM,  (0x1F << 27));

		/* PRCM: Enable STM_MOD_CMD2: AltC1 on GPIO155-159 */
		setbits32(PRCM_GPIOCR, (1 << 13));
		break;

	case OPTION_MODEMSTM_GPIOPINS_163_167:
		/* GPIO: Enable STM_MOD on GPIO163-167 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_SLPM,  (0x1F << 3));
		break;
	}
}

/* Modem UART configuration */
static void modem_uart_config(void)
{
	switch (CONFIG_MODEMUART_GPIOPINS) {
	case OPTION_MODEMUART_GPIOPINS_75_76:
		/* GPIO: UARTMOD_RXD UARTMOD_TXD on GPIO75+76 (AltC3) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (3 << 11));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (3 << 11));
		setbits32(GPIO2_BASE + GPIO_SLPM,  (3 << 11));

		 /* PRCM: Enable UARTMOD_CMD0: AltC3 on GPIO75+76 */
		setbits32(PRCM_GPIOCR, (1 << 0));
		break;

	case OPTION_MODEMUART_GPIOPINS_153_154:
		/* GPIO: UARTMOD_RXD, UARTMOD_TXD on GPIO153+154 (AltC1) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (3 << 25));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (3 << 25));
		setbits32(GPIO4_BASE + GPIO_SLPM,  (3 << 25));

		/* PRCM: Enable UARTMOD_CMD1: AltC1 on GPIO153+154 */
		setbits32(PRCM_GPIOCR, (1 << 1));
		break;

	case OPTION_MODEMUART_GPIOPINS_161_162:
		/* GPIO: UARTMOD_RXD, UARTMOD_TXD on GPIO161+162 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (3 << 1));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (3 << 1));
		setbits32(GPIO5_BASE + GPIO_SLPM,  (3 << 1));
		break;
	}
}

static void prcmu_stm_config(void)
{
	switch (CONFIG_PRCMUSTM_GPIOPINS) {
	case OPTION_PRCMUSTM_GPIOPINS_70_74:
		logdebug("OPTION_PRCMUSTM_GPIOPINS_70_74");
		/* GPIO: Enable STM_APE on GPIO70-74 (AltC) */
		setbits32(GPIO2_BASE + GPIO_AFSLA, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_AFSLB, (0x1F << 6));
		setbits32(GPIO2_BASE + GPIO_SLPM, (0x1F << 6));
		break;

	case OPTION_PRCMUSTM_GPIOPINS_155_159:
		logdebug("OPTION_PRCMUSTM_GPIOPINS_155_159");
		/* GPIO: Enable STM_APE on GPIO155-159 (AltC) */
		setbits32(GPIO4_BASE + GPIO_AFSLA, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_AFSLB, (0x1F << 27));
		setbits32(GPIO4_BASE + GPIO_SLPM, (0x1F << 27));
		break;

	case OPTION_PRCMUSTM_GPIOPINS_163_167:
		logdebug("OPTION_PRCMUSTM_GPIOPINS_163_167");
		/* GPIO: Enable STM_APE on GPIO163-167 (AltC) */
		setbits32(GPIO5_BASE + GPIO_AFSLA, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_AFSLB, (0x1F << 3));
		setbits32(GPIO5_BASE + GPIO_SLPM, (0x1F << 3));
		break;
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

/* Memory controller register*/
#define DDR0_DMC_BASE_ADDR	0x80156000
#define DDR1_DMC_BASE_ADDR	0x8015A000
#define DMC_CTL_12			0x30
#define DMC_CTL_96			0x180
#define DMC_CTL_97			0x184
static void get_memory_size(unsigned long *size0, unsigned long *size1)
{
	u32 dmc_clt_96_value;
	u32 dmc_clt_97_value;
	u32 dmc_clt_12_value;
	unsigned nrows_cs0;
	unsigned ncols_cs0;
	unsigned nrows_cs1;
	unsigned ncols_cs1;

	*size0 = *size1 = 0;

	/*
	 * Assumption: 2 CS active
	 *             15 rows max, 11 cols max (controller spec).
	 *             memory chip has 8 banks, I/O width 32 bit.
	 * The correct way would be to read MR#8: I/O width and density,
	 * but this requires locking against the PRCMU firmware.
	 * Simplified approach:
	 * Read number of unused rows and columns from mem controller.
	 * size for each cs = 2^(rows+cols) x nbanks x buswidth_bytes
	 */
	/* DDR0 */
	dmc_clt_12_value = read32(DDR0_DMC_BASE_ADDR + DMC_CTL_12);
	dmc_clt_96_value = read32(DDR0_DMC_BASE_ADDR + DMC_CTL_96);
	dmc_clt_97_value = read32(DDR0_DMC_BASE_ADDR + DMC_CTL_97);

	if ((dmc_clt_12_value >> 8) & 0x1) {
		nrows_cs0 = 15 - ((dmc_clt_96_value  & 0x07000000) >> 24);
		ncols_cs0 = 11 - ((dmc_clt_97_value  & 0x0700) >> 8);
		*size0 =  (1 << (nrows_cs0 + ncols_cs0)) * 8 * 4;
	}

	if ((dmc_clt_12_value >> 8) & 0x2) {
		nrows_cs1 = 15 - (dmc_clt_97_value & 0x07);
		ncols_cs1 = 11 - ((dmc_clt_97_value  & 0x070000) >> 16);
		*size0 +=  (1 << (nrows_cs1 + ncols_cs1)) * 8 * 4;
	}


	/* DDR1 */
	dmc_clt_12_value = read32(DDR1_DMC_BASE_ADDR + DMC_CTL_12);
	dmc_clt_96_value = read32(DDR1_DMC_BASE_ADDR + DMC_CTL_96);
	dmc_clt_97_value = read32(DDR1_DMC_BASE_ADDR + DMC_CTL_97);

	if ((dmc_clt_12_value >> 8) & 0x1) {
		nrows_cs0 = 15 - ((dmc_clt_96_value  & 0x07000000) >> 24);
		ncols_cs0 = 11 - ((dmc_clt_97_value  & 0x0700) >> 8);
		*size1 =  (1 << (nrows_cs0 + ncols_cs0)) * 8 * 4;
	}

	if ((dmc_clt_12_value >> 8) & 0x2) {
		nrows_cs1 = 15 - (dmc_clt_97_value & 0x07);
		ncols_cs1 = 11 - ((dmc_clt_97_value  & 0x070000) >> 16);
		*size1 +=  (1 << (nrows_cs1 + ncols_cs1)) * 8 * 4;
	}
}

u32 interleaver_test(u32 interlv_config) {
	int res = BOOT_OK;

	if (interlv_config & 0x1) {
		intlvtest(interlv_config);
	}
	else {
		logdebug("interleaver is off, no test required");
		res = BOOT_OK;
	}

	return res;
}

u32 ddr_test(void)
{
	int res = 0;
	int reenable_cache = 0;
	unsigned long memsize0, memsize1;

	logdebug("Starting DDR test...");

	/* Get memory size */
	get_memory_size(&memsize0, &memsize1);
	loginfo("DDR0 Memory size = %u MB", memsize0 / 1024 / 1024);
	loginfo("DDR1 Memory size = %u MB", memsize1 / 1024 / 1024);

	/* Disable cache */
	if (CONFIG_MEMTEST_DISABLECACHE) {
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
	/* ER 404440 : Mapped memory is lower than DDR capacity
	 * Test only accessible memory space
	 */
	res = memtest_main((volatile datum_t *)AP9540_DDR0_OFFSET, AP9540_DDR0_SIZE);

	res |= memtest_main((volatile datum_t *)AP9540_DDR1_OFFSET, AP9540_DDR1_SIZE);


	/* Re-enable cache */
	if (CONFIG_MEMTEST_DISABLECACHE && reenable_cache) {
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

static void version_write(void)
{
	u32 boot_info;

	boot_info = read32(BOOT_INFO_BACKUPRAM1);
	boot_info &= ~BOARD_VERSION_MASK;
	boot_info |= BOARD_VERSION_MASK & (CONFIG_BOARD_VERSION << 24);
	write32(BOOT_INFO_BACKUPRAM1, boot_info);

	logdebug("Boardversion set to %x",
		CONFIG_BOARD_VERSION);
}

void prcmufw_version_print(void)
{
	u32 version = read32(TCPM_BASE + PRCMU_FW_VERSION_OFFSET);
	char proj_index = version & 0xFF;
	char major = version >> 8 & 0xFF;
	char minor = version >> 16 & 0xFF;
	char patch = version >> 24 & 0xFF;

	if (major != PRCMU_FW_MAJOR_VERSION)
	{
		loginfo("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		loginfo("!!! WARNING - PRCMU Fw version used not in line with soc-settings build");
		loginfo("!!! PRCMU Fw version : %u.%u.%u - SocSettings built with %u.x.x", \
				major, minor, patch, PRCMU_FW_MAJOR_VERSION);
		loginfo("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
    else
		loginfo("PRCMU Fw [Project Index: %u Fw Version: %u.%u.%u]\n", proj_index, major, minor, patch);
}

static void version_print(void)
{
	loginfo("%s", VERSION_STR);

#ifdef IMAD_CONFIG
	loginfo("IMAD CONFIGURATION : ON");
#else
	loginfo("IMAD CONFIGURATION : OFF");
#endif

#ifdef IMAD_CONFIG_LVL0_FIREWALL
	loginfo("LEVEL0 FIREWALLING : ON");
#else
	loginfo("LEVEL0 FIREWALLING : OFF");
#endif

	if (COMPILE_AVS)
		loginfo("AVS Algorithm : ON");
	else
		loginfo("AVS Algorithm : OFF");
}

/*
 * This function finds next section in the SoC-settings binary.
 *
 * bin		Should point to the beginning of the SoC-settings binary
 *		in memory.
 * len		Length of the SoC-settings binary
 * type		Next section type to find
 * head		Continues search from last returned section if head->bin
 *		is != NULL. If head->bin is NULL restart from the beginning
 *		of the SoC-Settings binary.
 */
static u32 next_section(const u8 *bin, u32 bin_len, enum section_type type,
			struct section_head *head)
{
	u32 t; /* type in header */
	u32 l; /* length in header */
	const u8 *p;
	const u32 hdr_size = sizeof(t) + sizeof(l);
	const u8 *ep = bin + bin_len;

	if (head->bin == NULL) {
		/*
		 * Skip over version, it's not needed internally in
		 * SoC-settings.
		 */
		head->bin = bin + sizeof(u32);
		head->len = 0;
	}

	do {
		/*
		 * head->bin is supposed to point at the payload of the
		 * section before the first section to inspect.
		 *
		 * If it isn't we will not bother to try to detect it here.
		 */
		p = head->bin + head->len;

		/* Skip alignment padding. */
		p = (u8 *)ROUNDUP((u32)p, 4);

		/* Check if we're at the end of the binary. */
		if (p == ep) {
			head->bin = NULL;
			/*
			 * Return OK as there wasn't a problem with the
			 * data format, the just wasn't a "next section".
			 */
			return BOOT_OK;
		}

		/* Check that there's space for the header. */
		if ((p + hdr_size) >= ep)
			return BOOT_INTERNAL_ERROR;

		t = *(u32 *)p;
		p += sizeof(t);
		l = *(u32 *)p;
		p += sizeof(l);

		/*
		 * Check that there's space for the payload, second test
		 * for overflow of p + l.
		 */
		if ((p + l) > ep || (p + l) < p)
			return BOOT_INTERNAL_ERROR;

		head->bin = p;
		head->len = l;
		head->type = t;
	} while (head->type != type);

	return 0;
}

#if 0
static u32 ddr_data_is_matching(const struct ddr_data *ddr_data,
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
#endif

void memcpy(u8 *dest, u8* src, unsigned length)
{
	unsigned n;

	for (n = 0; n < length; n++)
		dest[n] = src[n];
}

#ifdef IMAD_CONFIG
#ifdef IMAD_CONFIG_READ_FUSES
/*
 * This function gets fuses values
 *
 * fuses	buffer in which should be get back fuses values for FVR21/FVR22
 * head		Read of fuses (only available in secure mode)
 *              will be done thanks to IMAD service
 */
static bass_return_code get_avs_fuses(u32 *fuses)
{
  size_t faulty_entry;
  bass_return_code return_code = BASS_RC_SUCCESS;
  const struct bass_imad_entry avs_fuses[] = {
    { IMAD_SEQUENCE_READ_AVS_FUSES, (u32)fuses},
    { IMAD_SEQUENCE_READ_AVS_FUSES, (u32)(fuses + 4)}
  };

  return_code = bass_imad_exec(avs_fuses,
			       sizeof(avs_fuses)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: get AVS fuses");
    }

  return return_code;
}
#endif

/*
 * This function send message to PRCMU
 *
 * req       request message
 * resp      expected response
 * head      send a message to the PRCMU and wait the acknowledge
 *           and compare to expected response.
 */
static u32 prcmu_mb4_req(t_ReqMb4H req, t_AckMb4Status resp)
{
    u32 res = 0;
    u32 n=0;

    /* Service request => MB4 */
    TCDMnSec->Header.Req.un.Mb4 = req;

    /* Send IT to XP70 to request service */
    PRCMU_HW_REGS->prcm_mbox_cpu_set = XP70_IT_EVENT_MB4;

    /* Wait for IT mailbox 4 */
    for (n = 0; (PRCMU_HW_REGS->prcm_mbox_cpu_val & XP70_IT_EVENT_MB4) != 0; n++)
      {
	if (n == (u32)-1)
	  {
            loginfo("prcmu_mb4_req(%d) timeout %d\n", req, n);
            return 1;
	  }
      }

    if (TCDMnSec->AckMb4.field.AckMb4Status != resp)
      {
        logerror("Req 0x%x failed\n", req);
        res = 1;
      }

    /* Clear AckMB4 */
    TCDMnSec->AckMb4.field.AckMb4Status = AckMb4Init;

    /* Clear IT mailbox 4 */
    PRCMU_HW_REGS->prcm_arm_it1_clr = XP70_IT_EVENT_MB4;

    return res;
}

/*
 * This function applies the computed AVS settings into PRCMU
 *
 * init_opp_data	contains freq/voltage and body bias data 
 * head                 gets the init opp data previsouly computed
 *                      and send them to PRCMU thanks to InitOppH
 *                      message.
 */
static bass_return_code prcmu_apply_avs_settings(sPrcmuApi_InitOppData_t *init_opp_data)
{
  size_t faulty_entry;
  bass_return_code return_code = BASS_RC_SUCCESS;
  const struct bass_imad_entry opp_data_entry[] = {
    { IMAD_SEQUENCE_WRITE_AVS_IN_PRCMU_TDCM, (size_t)init_opp_data}
  };

  if(init_opp_data == NULL)
    {
      logerror("prcmu_apply_avs_settings: sanity checks failed");
      return BASS_RC_FAILURE;
    }

  return_code = bass_imad_exec(opp_data_entry,
			       sizeof(opp_data_entry)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: init opp data");
      return return_code;
    }

  if (prcmu_mb4_req(InitOppH, InitOpp_Ok) != 0)
    {
      logerror("error on InitOppH req");
      return BASS_RC_FAILURE;
    }

  return return_code;
}

/*
 * This function sets the PRCMU in EPEXEC state
 *
 * head                 gets the init opp data previsouly computed
 *                      and send them to PRCMU thanks to InitOppH
 *                      message.
 */
static bass_return_code prcmu_set_ap_exec(void)
{
  u32 n=0;
  bass_return_code return_code = BASS_RC_SUCCESS;

  /*
   * Switch xP70 Fw in ApExecute
   */
  
  /* power state transition request (header of MailBox0) */
  TCDMnSec->Header.Req.un.Mb0 = PwrStTrH;
  TCDMnSec->ReqMb0.PwrStTr.un.ApPwrStTr = ApBootToApExecute;
  
  /* Send IT to XP70 */
  PRCMU_HW_REGS->prcm_mbox_cpu_set = XP70_IT_EVENT_MB0;
  
  /*
   * Wait until PRCMU is in ApExecute.
   */
  while (TCDMnSec->ShVar1.pwr_current_state_var != ApExecute)
    {
      if (n > (u32)-1)
	{
	  logerror("ApExecute failed\n");
	  return BASS_RC_FAILURE;
	}
      n++;
    }

  return return_code;
}

/*
 * This function sets the max ddr freq
 *
 * ddr_freq             maxddr freq for each controllers (DDR0/DDR1)
 * head                 gets the settings to set minimum bandwidth
 *                      request by APE on all DDR controllers
 */
static bass_return_code prcmu_set_ddr_speed(u32* ddr_freq)
{
  size_t faulty_entry;
  bass_return_code return_code = BASS_RC_SUCCESS;
  const struct bass_imad_entry ddr_speed[] = {
    { IMAD_SEQUENCE_SET_APE_MIN_DDR_BW, (u32)&(ddr_freq[0])},
    { IMAD_SEQUENCE_SET_APE_MIN_DDR_BW, (u32)&(ddr_freq[1])},
    { IMAD_SEQUENCE_SET_DDR1_GFX_MIN_BW, (u32)0},
  };

  return_code = bass_imad_exec(ddr_speed,
			       sizeof(ddr_speed)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: set DDR speed");
    }

  return return_code;
}

#ifdef IMAD_CONFIG_DDR_INIT
/*
 * This function sets ddr init data
 *
 * ddr_init_data        contains full settings + interleaving
 * head                 gets the init opp data previsouly computed
 *                      and send them to PRCMU thanks to InitOppH
 *                      message.
 */
static bass_return_code prcmu_ddr_init(struct ddr_data * ddr_init_data)
{
  size_t faulty_entry;
  bass_return_code return_code = BASS_RC_SUCCESS;

  const struct bass_imad_entry shared_L2cc_scu[] = {
    { IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS, (u32)0},
    { IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS, (u32)0},
    { IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS, (u32)0}
  };
  const struct bass_imad_entry ddr0_init[] = {
    { IMAD_SEQUENCE_DDR0_INIT, (u32)&(ddr_init_data->ddr0_cfgs_settings)},
  };
  const struct bass_imad_entry ddr1_init[] = {
    { IMAD_SEQUENCE_DDR1_INIT, (u32)&(ddr_init_data->ddr1_cfgs_settings)},
  };

  /* Set default values for Modem/APE shared DDR, L2CC and SCU prior to DDR inits */
  return_code = bass_imad_exec(shared_L2cc_scu,
			       sizeof(shared_L2cc_scu)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: shared DDR, L2CC and SCU inits");
      return return_code;
    }

  /* DDR0 Init */
  return_code = bass_imad_exec(ddr0_init,
			       sizeof(ddr0_init)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: init ddr0");
      return return_code;
    }
  logdebug("IMAD: init ddr0 done");

  /* TODO: possible to do after ddr1 config send */
  if (prcmu_mb4_req(ddrCtrl0InitH, DDROn_Ok) != 0)
    {
      logerror("error on ddrCtrl0InitH req");
      return BASS_RC_FAILURE;
    }
  logdebug("ddrCtrl0InitH req sent");

  /*
   * TODO: According to AP9540 DesignSpec v1.4 chap. 3.3.1, address filtering is
   * different when DDR0 is On and DDR1 is Off, compared when both DDR0 and
   * DDR1 are On.
   *
   * FIXME: following code is a workaround of a bug which makes the board fail to boot
   * on l9540-android-4.0_v3.0 baseline. Please rework this according to the
   * todo comment above.
   */
  return_code = bass_imad_exec(shared_L2cc_scu,
			       sizeof(shared_L2cc_scu)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: shared DDR, L2CC and SCU inits");
      return return_code;
    }

  logdebug("shared_L2cc_scu req sent");

  /* DDR1 Init */
  return_code = bass_imad_exec(ddr1_init,
			       sizeof(ddr1_init)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: init ddr1");
      return return_code;
    }
  logdebug("IMAD: init ddr1 done");

  /* TODO: possible to do after ddr1 config send */
  if (prcmu_mb4_req(ddrCtrl1InitH, DDROn_Ok) != 0)
    {
      logerror("error on ddrCtrl1InitH req");
      return BASS_RC_FAILURE;
    }
  logdebug("ddrCtrl1InitH req sent");

  return return_code;
}
#endif

bass_return_code set_ddr_interlv_config(u32 interlv_config)
{
  size_t faulty_entry;
  bass_return_code return_code = BASS_RC_SUCCESS;
  u32 local_interlv_config = interlv_config & 0xFFFE;

  const struct bass_imad_entry interleave[] = {
    { IMAD_SEQUENCE_INTERLEAVING, (u32)0}, /* disable interlv */
    { IMAD_SEQUENCE_INTERLEAVING, (u32)&local_interlv_config}, /* Configure interlv */
    { IMAD_SEQUENCE_INTERLEAVING, (u32)&interlv_config} /* enable interlv if requested (bit0 of interlv_config) */
  };

  /* Apply interleaving config: config is in imad.c file as managed as embedded data */
  return_code = bass_imad_exec(interleave,
			       sizeof(interleave)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
  {
      logerror("IMAD error: interleave");
      return return_code;
  }
  logdebug("Interleaving config loaded");

  return return_code;
}
#endif

#ifdef CONFIG_PRCMU_C2C_INIT_SERVICE
static bass_return_code prcmu_c2c_init(sPrcmuApi_C2cInitData_t *c2c_init_data)
{
  bass_return_code return_code = BASS_RC_SUCCESS;

#ifdef IMAD_CONFIG
  size_t faulty_entry;

  const struct bass_imad_entry c2c_init[] = {
    { IMAD_SEQUENCE_C2C_INIT, (u32)c2c_init_data },
  };

  return_code = bass_imad_exec(c2c_init,
			       sizeof(c2c_init)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS)
    {
      logerror("IMAD error: init c2c");
      return return_code;
    }
  logdebug("IMAD: init c2c done");
#else
  /* Initialize PRCMU FW C2CInit structure */
  memcpy((u8*)&TCDMsec->C2cInitData, (u8*)c2c_init_data, sizeof(sPrcmuApi_C2cInitData_t));
#endif

  /* invoke PRCMU Fw c2C Init service */
  sPrcmuApi_uMailbox_t *p_mailbox = (sPrcmuApi_uMailbox_t *)UniqPAP_mb;//(sPrcmuApi_uMailbox_t *)0x801B8A30;

  /* Wait for UniqPAP service to be available */
  while (p_mailbox->req_state != ePrcmuApi_ReqIdle);

  /* Send C2CInit service request */
  p_mailbox->req.header.srvId = ePrcmuApi_Service_C2C;
  p_mailbox->req.header.cmdId = ePrcmuApi_C2cInit;
  p_mailbox->req_state = ePrcmuApi_ReqSent;
  write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT11);

  /* Wait service completion */
  while (p_mailbox->req_state != ePrcmuApi_AckSent);
  write32(PRCM_ARM_IT1_CLR, PRCM_ARM_IT1_CLR_ALL);
  p_mailbox->req_state = ePrcmuApi_ReqIdle;

  logdebug("PRCMU fw C2C service initialized");

  return return_code;
}
#endif

/*
 * This function sends PASR info to PRCMU
 *              will be done thanks to IMAD service
 */
static bass_return_code send_pasr_segment_mask_to_prcmu(sPrcmuApi_securePasrData_t *pasr_segment)
{
  bass_return_code return_code = BASS_RC_SUCCESS;

#ifdef IMAD_CONFIG
  size_t faulty_entry;

  const struct bass_imad_entry pasr_segment_ie[] = {
    { IMAD_SEQUENCE_WRITE_PASR_SEG, (u32)pasr_segment}
  };

  return_code = bass_imad_exec(pasr_segment_ie,
			       sizeof(pasr_segment_ie)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS) {
      logerror("IMAD error: send PASR segments to prcmu failed");
  }

  logdebug("IMAD: init PASR segment mask done");
#else
  /* Initialize PRCMU FW PASR structure */
  memcpy((u8*)&TCDMsec->securePasrData, (u8*)pasr_segment, sizeof(sPrcmuApi_securePasrData_t));
#endif

  logdebug("PRCMU fw PASR segment mask initialized");

  return return_code;
}

/*
 *
 */
void ThSensor_configure_dcorrect( u32 compensate_value)
{
	((t_prcmu_hw_registers*)PRCMU_REG_START_ADDR)->thsens_registers.dcorrect = compensate_value;
}

#ifdef IMAD_CONFIG_LVL0_FIREWALL
static bass_return_code configure_lvl0_firewalling(u32 *lvl0_config)
{
  bass_return_code return_code = BASS_RC_SUCCESS;
  size_t faulty_entry;

  const struct bass_imad_entry lvl0_firewall_ie[] = {
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)lvl0_config},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 1)},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 2)},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 3)},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 4)},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 5)},
    { IMAD_SEQUENCE_WRITE_LVL0_FIREWALL, (u32)(lvl0_config + 6)},
  };

  return_code = bass_imad_exec(lvl0_firewall_ie,
			       sizeof(lvl0_firewall_ie)/sizeof(struct bass_imad_entry),
			       &faulty_entry);

  if(return_code != BASS_RC_SUCCESS) {
      logerror("IMAD error: Configure level0 firewall failed");
  }

  logdebug("IMAD: Level0 firewall configuration done");

  return return_code;
}
#endif

__attribute__((section(".entry_function")))
u32 soc_settings_exec(const u8 *bin, u32 bin_len, u32 boot_status)
{
	u32 upass = BOOT_OK;
	bass_return_code ret;
	struct section_head	head;
	sPrcmuApi_InitOppData_t	init_opp_data;
//	u8 ddr_regs[DDR_REGISTERS_LEN * 2];
	struct ddr_data *ddr_data;
	struct custom_config *custom_config;
#ifdef CONFIG_PRCMU_C2C_INIT_SERVICE
	sPrcmuApi_C2cInitData_t *c2c_init_data;
#endif
	u32 fuses[6] = { 0, 0, 0, 0, 0, 0 };
	u32 ddr_freq[2] = { 0, 0 };
	u32 ddr_interlv_config;
	u32 n;
	Opp_status opp_status;
	sPrcmuApi_securePasrData_t *pasr_segment_to_prcmu;
	(void)&boot_status; /* gcc warning */
#ifndef IMAD_CONFIG_DDR_INIT
	u8 ddr_data_to_issw[2 * sizeof(t_DDRInit) + sizeof(u32)];
#endif
#ifdef IMAD_CONFIG_LVL0_FIREWALL
	u32 *lvl0_firewall;
#endif

	/* Initialize the bridge interface used by TEE Client API. */
	SECURITY_Init();

	/* The followings should be done before AVS settings */

	/* Clear AckMB interrupt */
	write32(PRCM_ARM_IT1_CLR, PRCM_ARM_IT1_CLR_ALL);

#ifdef IMAD_CONFIG
	/*
	 * Look for IMAD table
	 */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_IMAD, &head);
	if (ret != BOOT_OK || head.bin == NULL) {
		logerror("Failed to find IMAD section");
		return BOOT_INTERNAL_ERROR;
	}

	/* No sanity check on size at size not known */

	ret = bass_imad_store((const uint8_t *)head.bin, head.len);
	if(ret != BASS_RC_SUCCESS)
	  {
		logerror("error during IMAD store operation");
		return BOOT_INTERNAL_ERROR;
	  }
#endif

	/* Looking for custom settings */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_CUSTOM_CONFIG, &head);
	if (ret != 0 || head.bin == NULL) {
		logerror("Failed to find coustom config data section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != sizeof(struct custom_config)) {
		logerror("Custom config data section is bad");
		return BOOT_INTERNAL_ERROR;
	}

	custom_config = (struct custom_config *)head.bin;
	ddr_interlv_config = custom_config->DDRInterlv;
	pasr_segment_to_prcmu = &(custom_config->PASRSegMask);

	/*
	 * Send PASR Segment mask to PRCMU
	 */
	ret = send_pasr_segment_mask_to_prcmu(pasr_segment_to_prcmu);
	if (ret != BASS_RC_SUCCESS) {
		logerror("PASR_init failed");
		return ret;
	}
	logdebug("PASR Init done");

	/*
	 * Look for AB9540 init settings
	 */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_AB9540, &head);
	if (ret != BOOT_OK || head.bin == NULL) {
		logerror("Failed to find AB9540 section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != AB9540_SETTINGS_LEN) {
		logerror("AB9540 section bad");
		return BOOT_INTERNAL_ERROR;
	}

	/*
	 * Initialize some ABx500 registers
	 */
	AB9540_init_fct((u8*)head.bin);
	logdebug("AB9540 Init done");

#ifdef IMAD_CONFIG_READ_FUSES
	ret = get_avs_fuses(fuses);
#else
	ret = bass_get_avs_fuses((u8 *)fuses, sizeof(fuses));
#endif
	if (ret != BASS_RC_SUCCESS)
	{
		logerror("get_avs_fuses failed: %x", ret);
		return ret;
	}
	logdebug("Read fuses OK");

	version_print();
	prcmufw_version_print();

	loginfo("FUSES : FVR4 = %x, FVR5 = %x", fuses[0], fuses[1]);
	loginfo("FUSES : FVR6 = %x, FVR7 = %x", fuses[2], fuses[3]);
	loginfo("FUSES : FVR21 = %x, FVR22 = %x", fuses[4], fuses[5]);

	/* 451826 - Update thsens compensation with value fused in FVR21(4..0) */
	ThSensor_configure_dcorrect(fuses[4] & 0x1F);

	/*
	 * Calculate AVS settings based on FUSES and other AVS
	 * parameters.
	 */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_OPP, &head);
	if (ret != BOOT_OK || head.bin == NULL) {
		logerror("Failed to find OPP section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != sizeof(sPrcmuApi_InitOppData_t)) {
		logerror("OPP section is bad");
		return BOOT_INTERNAL_ERROR;
	}

	opp_status = OPP_GetOppConfig(fuses, (u16*)head.bin, &init_opp_data);
	if (opp_status != OPP_STATUS_NO_ERROR)
	{
		logerror("OPP_GetOppConfig failed: %x", opp_status);
		return opp_status;
	}

	/*
	 * Wait end of UART transmission to do not have spurious characters
	 * because of OppInit service that disturb UART link
	 */
	while((read32(CONFIG_UART_BASE + UART_FR) & UART_FR_TXFE) != UART_FR_TXFE);

	logdebug("Opp Init ready");

	/* Apply the AVS settings */
#ifdef IMAD_CONFIG
	ret = prcmu_apply_avs_settings(&init_opp_data);
#else
	ret = bass_prcmu_apply_avs_settings((u8 *)&init_opp_data, sizeof(sPrcmuApi_InitOppData_t));
#endif
	if (ret != BASS_RC_SUCCESS) {
		logerror("apply_avs_settings failed");
		return ret;
	}
	logdebug("OPP Init sent");

	/*
	 * Wait end of UART transmission to do not have spurious characters
	 * because of OppInit service that disturb UART link
	 */
	while((read32(CONFIG_UART_BASE + UART_FR) & UART_FR_TXFE) != UART_FR_TXFE);

	/*
	 * Put PRCMU in ApExec
	 */
#ifdef IMAD_CONFIG
	ret = prcmu_set_ap_exec();
#else
	ret = bass_prcmu_set_ap_exec();
#endif

	if (CONFIG_UART_ENABLED) {
		uart_init();
		logdebug("UART initialized");
	}

	if (ret != BASS_RC_SUCCESS) {
		logerror("set_ap_exec failed");
		return ret;
	}
	logdebug("AP_EXEC sent");

	/*
	 * Do DDR_PreInit to identify memory.
	 */
	if (CONFIG_DDR_PREINIT)
	{
		t_DDRInitData ddr_init_data;

		ddr_init_data.CfgAdd[0] = 0;  /* Device info */
		ddr_init_data.CfgAdd[1] = 5;  /* BC-1 : Manufacturer ID */
		ddr_init_data.CfgAdd[2] = 6;  /* BC-2 : Revision ID1 */
		ddr_init_data.CfgAdd[3] = 7;  /* BC-3 : Revision ID2 */
		ddr_init_data.CfgAdd[4] = 8;  /* BC-4 : I/O width + Density + Type */
		ddr_init_data.CfgAdd[5] = 0;  /* Not used */
		ddr_init_data.CfgAdd[6] = 0;  /* Not used */
		ddr_init_data.CfgAdd[7] = 0;  /* Not used */

		/*
		 * Hardcode to read both CS0 and CS1 because PRCMU firmware
		 * will not read the corresponding register unless we've put
		 * something that isn't 0 in cs[x].DataCS[n].
		 */
		for (n = 0; n < sizeof(ddr_init_data.cs) / sizeof(ddr_init_data.cs[0]);
				n++) {
			u32 m;
			for (m = 0; m < sizeof(ddr_init_data.cs[0].DataCS) /
					sizeof(ddr_init_data.cs[0].DataCS[0]); m++) {
				if (CONFIG_TWO_DIE) {
					ddr_init_data.cs[n].DataCS[m] = 0xFF;
				} else {
					/* One die memory, assume connected to CS0 */
					if (n == 1)
						ddr_init_data.cs[n].DataCS[m] = 0x0;
					else
						ddr_init_data.cs[n].DataCS[m] = 0xFF;
				}
			}
		}

		/* Find first/default DDR data. */
		head.bin = NULL;
		ret = next_section(bin, bin_len, SECTION_TYPE_DDR_DATA, &head);
		if (ret != 0 || head.bin == NULL) {
			logerror("Failed to find first DDR data section");
			return BOOT_INTERNAL_ERROR;
		}
		if (head.len != sizeof(struct ddr_data)) {
			logerror("DDR data section bad");
			return BOOT_INTERNAL_ERROR;
		}
		ddr_data = (struct ddr_data *)head.bin;

#if 0
		ret = bass_prcmu_ddr_pre_init(ddr_data->settings,
				sizeof(ddr_data->settings), (void *)&ddr_init_data,
				sizeof(ddr_init_data), ddr_regs, sizeof(ddr_regs));
		if (ret != BASS_RC_SUCCESS) {
			logerror("ddr_pre_init failed");
			return ret;
		}
#endif
		/*
		 * Search for optimized DDR settings.
		 */

		/* Find a DDR id that matches the DDR parameters. */
		head.bin = NULL;
		while (1) {
			ret = next_section(bin, bin_len, SECTION_TYPE_DDR_DATA, &head);
			if (ret != 0 || head.bin == NULL) {
				/*
				 * Didn't find any better parameters, let's use the
				 * generic settings.
				 */
				break;
			}
			if (head.len != sizeof(struct ddr_data)) {
				logerror("DDR data section is bad");
				return BOOT_INTERNAL_ERROR;
			}

#if 0
			if (ddr_data_is_matching((struct ddr_data *)head.bin,
						ddr_regs, sizeof(ddr_regs))) {
				ddr_data = (struct ddr_data *)head.bin;
				break;
			}
#endif
		}
	}

	/*
	 * Looking for DDR settings
	 */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_DDR_DATA, &head);
	if (ret != 0 || head.bin == NULL) {
		logerror("Failed to find DDR data section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != sizeof(struct ddr_data)) {
		logerror("DDR data section is bad");
		return BOOT_INTERNAL_ERROR;
	}

	ddr_data = (struct ddr_data *)head.bin;
	ddr_freq[0] = ddr_data->ddr0_frequency;
	ddr_freq[1] = ddr_data->ddr1_frequency;

	/*
	 * Configure DDRO and DDR1 bandwidth
	 */
#ifdef IMAD_CONFIG
	ret = prcmu_set_ddr_speed(ddr_freq);
#else
	ret = bass_prcmu_set_ddr_speed((u8 *)ddr_freq, 2 * sizeof(u32));
#endif

	if (ret != BASS_RC_SUCCESS) {
		logerror("set_ddr_speed failed");
		return ret;
	}
	logdebug("DDR Speed init done");

	/*
	 * Configure Ctrl DDRO and Ctrl DDR1 settings
	 */
#ifndef IMAD_CONFIG_DDR_INIT
	memcpy(ddr_data_to_issw, (u8*)&ddr_data->ddr0_cfgs_settings, sizeof(t_DDRInit));
	memcpy(ddr_data_to_issw + sizeof(t_DDRInit), (u8*)&ddr_data->ddr1_cfgs_settings, sizeof(t_DDRInit));
	memcpy(ddr_data_to_issw + 2 * sizeof(t_DDRInit), (u8*)&ddr_interlv_config, sizeof(u32));

	ret = bass_prcmu_ddr_init(ddr_data_to_issw,	sizeof(ddr_data_to_issw));
#else
	ret = prcmu_ddr_init(ddr_data);
#endif

	if (ret != BASS_RC_SUCCESS) {
		logerror("ddr_init failed");
		return ret;
	}
	logdebug("prcmu_ddr_init successful");

#ifdef IMAD_CONFIG_DDR_INIT
	/*
	 * Set interleaver config depending on DDR size
	 */
	loginfo("INTERLV CONFIGURATION : %x", ddr_interlv_config);

	ret = set_ddr_interlv_config(ddr_interlv_config);
	if (ret != BASS_RC_SUCCESS) {
		logerror("Interlv config failed");
		return ret;
	}
	logdebug("Interlv config done");
#endif

#ifdef IMAD_CONFIG_LVL0_FIREWALL
	/* Looking for custom settings */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_CUSTOM_CONFIG, &head);
	if (ret != 0 || head.bin == NULL) {
		logerror("Failed to find coustom config data section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != sizeof(struct custom_config)) {
		logerror("Custom config data section is bad");
		return BOOT_INTERNAL_ERROR;
	}

	custom_config = (struct custom_config *)head.bin;
	lvl0_firewall = (u32*)&(custom_config->Lvl0Firewall);

	/* Configure Level0 firewalling */
	ret = configure_lvl0_firewalling(lvl0_firewall);
	if (ret != BASS_RC_SUCCESS) {
		logerror("Level0 Firewall configuration failed");
		return ret;
	}
	logdebug("Level0 Firewall configuration done");
#endif

#ifdef CONFIG_PRCMU_C2C_INIT_SERVICE
	/*
	 * Look for C2C init settings
	 */
	head.bin = NULL;
	ret = next_section(bin, bin_len, SECTION_TYPE_C2C_INIT, &head);
	if (ret != BOOT_OK || head.bin == NULL) {
		logerror("Failed to find C2C Init section");
		return BOOT_INTERNAL_ERROR;
	}
	if (head.len != sizeof(sPrcmuApi_C2cInitData_t)) {
		logerror("C2C init section bad");
		return BOOT_INTERNAL_ERROR;
	}
	logdebug("C2C init settings found");

	c2c_init_data = (sPrcmuApi_C2cInitData_t *)head.bin;
	ret = prcmu_c2c_init(c2c_init_data);
	if (ret != BASS_RC_SUCCESS) {
		logerror("c2c_init failed");
		return ret;
	}
	logdebug("C2C init done");
#endif

	clock_init();
	logdebug("clock initialized");

	gpio_init();
	logdebug("gpio initialized");

	/*
	 * Configure Modem STM trace and UART
	 */
	if (CONFIG_MODEMSTM_ENABLED)
		modem_stm_config();
	if (CONFIG_MODEMUART_ENABLED)
		modem_uart_config();

	/*
	 * Configure PRCMU STM trace
	 */
	if (CONFIG_PRCMUSTM_ENABLED)
		prcmu_stm_config();

	version_write();

	if ((read32(TCDM_BASE + PRCM_SW_RST_REASON) & 0xFFFF) == 0xDEAD) {
			logerror("Not performing DDR Test.");
			upass = BOOT_OK;
	} else {
		/* deactivate MMU */
		logdebug2("disabling MMU...");
		mmu_disable();

		/* interleaver test */
		upass = interleaver_test(ddr_interlv_config);
		/* DDR test */
		upass |= ddr_test();

		/* activate MMU */
		logdebug2("enabling MMU...");
		mmu_enable();

		if (upass == BOOT_OK)
			loginfo("DDR Test Ended successfully.");
		else
			logerror("DDR Test Ended with error.");
	}

#ifdef IMAD_CONFIG
	ret = bass_imad_release();
	if(ret != BASS_RC_SUCCESS)
	  {
		logerror("error during IMAD release operation");
		return BOOT_INTERNAL_ERROR;
	  }
#endif

	return upass;
}

