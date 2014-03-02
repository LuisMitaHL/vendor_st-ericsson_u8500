/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Michael Brandt <michael.brandt@stericsson.com> for ST-Ericsson
 * License terms: GNU General Public License (GPL), version 2.
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/hardware.h>
#include <mmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/common.h>
#include <asm/arch/prcmu.h>
#include <part.h>
#include <asm/arch/itp.h>
#include <asm/arch/mloader.h>
#include <asm/arch/prcmu-db5500.h>
#include "malloc.h"

#include "db5500_pins.h"
#include "db8500_pincfg.h"

#include <u8500_mmc_host.h>
#include <u5500_keypad.h>
#ifdef CONFIG_VIDEO_LOGO
#include "mcde_display.h"
#include <asm/arch-db5500/hardware.h>
/* ePOD and memory power signal control registers */
#define PRCM_EPOD_C_SET            (U5500_PRCMU_BASE + 0x410)
#define PRCM_SRAM_LS_SLEEP         (U5500_PRCMU_BASE + 0x304)
/* Debug power control unit registers */
#define PRCM_POWER_STATE_SET       (U5500_PRCMU_BASE + 0x254)
#define PRCM_PLLDSI_ENABLE         (U5500_PRCMU_BASE + 0x504)
#define PRCM_YYCLKEN0_MGT_SET      (U5500_PRCMU_BASE + 0x510)
#define PRCM_LCDCLK_MGT            (U5500_PRCMU_BASE + 0x044)
#define PRCM_HDMICLK_MGT           (U5500_PRCMU_BASE + 0x058)
#define PRCM_TVCLK_MGT             (U5500_PRCMU_BASE + 0x07c)
/* Reset values */
#define PRCM_HDMICLK_MGT_RESET     0x208
#define PRCM_TVCLK_MGT_RESET	 0x2A
#endif
/* offset for GPIO ALTB register */
#define DB5500_GPIO_AFSLB	0x24
/* BOOT ROM backup bit value to peripheral boot */
#define PERIPHERAL_BOOT_MODE (1 << 29)

#define BACKUPRAM_ROM_DEBUG_ADDR 0xFFC
#define MMC_BLOCK_ID	0x20

/*
 * GPIO pin config
 */

static pin_cfg_t gpio_config[] = {

	/* I2C */
	GPIO3_I2C1_SCL		| PIN_INPUT_PULLUP,
	GPIO4_I2C1_SDA		| PIN_INPUT_PULLUP,
	GPIO218_I2C2_SCL	| PIN_INPUT_PULLUP,
	GPIO219_I2C2_SDA	| PIN_INPUT_PULLUP,

	/* Backlight Enable */
	GPIO224_GPIO        | PIN_OUTPUT_HIGH,
	/* MMC2 (POP eMMC) */
	GPIO5_MC0_DAT0		| PIN_INPUT_PULLUP,
	GPIO6_MC0_DAT1		| PIN_INPUT_PULLUP,
	GPIO7_MC0_DAT2		| PIN_INPUT_PULLUP,
	GPIO8_MC0_DAT3		| PIN_INPUT_PULLUP,
	GPIO9_MC0_DAT4		| PIN_INPUT_PULLUP,
	GPIO10_MC0_DAT5		| PIN_INPUT_PULLUP,
	GPIO11_MC0_DAT6		| PIN_INPUT_PULLUP,
	GPIO12_MC0_DAT7		| PIN_INPUT_PULLUP,
	GPIO13_MC0_CMD		| PIN_INPUT_PULLUP,
	GPIO14_MC0_CLK		| PIN_OUTPUT_LOW,

	GPIO16_MC2_CMD		| PIN_INPUT_PULLUP,
	GPIO17_MC2_CLK		| PIN_OUTPUT_LOW,
	GPIO23_MC2_DAT0		| PIN_INPUT_PULLUP,
	GPIO19_MC2_DAT1		| PIN_INPUT_PULLUP,
	GPIO24_MC2_DAT2		| PIN_INPUT_PULLUP,
	GPIO20_MC2_DAT3		| PIN_INPUT_PULLUP,
	GPIO25_MC2_DAT4		| PIN_INPUT_PULLUP,
	GPIO21_MC2_DAT5		| PIN_INPUT_PULLUP,
	GPIO26_MC2_DAT6		| PIN_INPUT_PULLUP,
	GPIO22_MC2_DAT7		| PIN_INPUT_PULLUP,

	/* Keypad GPIO */
	GPIO128_KP_I0		| PIN_INPUT_PULLUP,
	GPIO130_KP_I1		| PIN_INPUT_PULLUP,
	GPIO132_KP_I2		| PIN_INPUT_PULLUP,
	GPIO134_KP_I3		| PIN_INPUT_PULLUP,
	GPIO137_KP_O4		| PIN_INPUT_PULLUP,
	GPIO139_KP_O5		| PIN_INPUT_PULLUP,

	#ifdef CONFIG_VIDEO_LOGO
	/* display config */
	GPIO226_GPIO        | PIN_OUTPUT_LOW
	#endif
};

static pin_cfg_t sdcard_gpio_config[] = {

	/* SDI1 (SD-CARD) */
	GPIO191_MC1_DAT0        | PIN_INPUT_PULLUP,
	GPIO192_MC1_DAT1        | PIN_INPUT_PULLUP,
	GPIO193_MC1_DAT2        | PIN_INPUT_PULLUP,
	GPIO194_MC1_DAT3        | PIN_INPUT_PULLUP,
	GPIO195_MC1_CLK         | PIN_OUTPUT_LOW,
	GPIO196_MC1_CMD         | PIN_INPUT_PULLUP,
	GPIO197_MC1_CMDDIR      | PIN_OUTPUT_HIGH,
	GPIO198_MC1_FBCLK       | PIN_INPUT_NOPULL,
	GPIO199_MC1_DAT0DIR     | PIN_OUTPUT_HIGH,

	/* SD_CARD detect */
	GPIO180_GPIO		| PIN_INPUT_PULLUP,
	GPIO227_GPIO		| PIN_OUTPUT_HIGH,
	GPIO185_GPIO		| PIN_OUTPUT_LOW
};

#ifdef CONFIG_VIDEO_LOGO
static int mcde_error;
#endif
int errno;

/*
 * Flag to indicate from where to where we have to copy the initialised data.
 * In case we were loaded, its value is -1 and .data must be saved for an
 * eventual restart. It is 1 if .data was restored, i.e. we were restarted,
 * e.g. by kexec.
 */
static volatile int data_init_flag = -1; /* -1 to get it into .data section */

DECLARE_GLOBAL_DATA_PTR;
#if defined(CONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
    printf("Boot reached stage %d\n", progress);
}
#endif

#ifdef CONFIG_VIDEO_LOGO
/*
 * Reset the primary display - called from mcde driver.
 */
int board_mcde_display_reset(void)
{
	int ret = 0;
		debug("%s: enter\n", __func__);
		db8500_gpio_set_output(GPIO226_GPIO, 1);
		mdelay(main_display_data.reset_delay);
		db8500_gpio_set_output(GPIO226_GPIO, 0);
		mdelay(main_display_data.reset_delay);
		db8500_gpio_set_output(GPIO226_GPIO, 1);
		mdelay(main_display_data.reset_delay);
	return ret;
}


static int dss_init(void)
{
	puts("  MCDE:  ");
	boottime_tag("splash");
	if (mcde_splash_image()) {
		printf("startup failed\n");
		goto mcde_error;
	}
	printf("ready\n");
	return 0;
mcde_error:
	return -EINVAL;
}

#endif
/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_access(block_dev_desc_t *block_dev)
{

#ifdef CONFIG_VIDEO_LOGO
	mcde_error = dss_init();
   #endif

	/*
	 * Don't load itp and modem if restarted (eg crashdump).
	 */
	if (!(data_init_flag > 0)) {
#ifdef CONFIG_ITP_LOAD
		itp_read_config(block_dev);
		if (itp_is_modem_in_config())
			if (mloader_load_modem(block_dev))
				return 1;

		if (itp_is_itp_in_config())
			if (itp_load_itp(block_dev))
				return 1;
#endif
	}
	return 0;
}

int board_init(void)
{
	extern char _idata_start[];
	extern char _data_start[];
	extern char _data_end[];
	unsigned long data_len;
	u32 gpio_value;

	data_len = _data_end - _data_start;
	if (++data_init_flag == 0) {
		/*
		 * first init after reset/loading
		 * save .data section for restart
		 */
		memcpy(_idata_start, _data_start, data_len);
	} else {
		/*
		 * restart, e.g. by kexec
		 * copy back .data section
		 */
		memcpy(_data_start, _idata_start, data_len);
		/* memcpy set data_init_flag back to zero */
		++data_init_flag;
	}

	gd->bd->bi_arch_number = 2890;	/* MACH_TYPE_U5500 */
	gd->bd->bi_boot_params = 0x00000100;

	u5500_clock_enable(1, 1, 1); /* SDI0 */
	u5500_clock_enable(5, 4, 4); /* SDI1 */

	/* Disable the altB functionality for GPIO64,65 */
	gpio_value = readl(U5500_GPIOBANK2_BASE + DB5500_GPIO_AFSLB);
	writel((gpio_value & ~(MASK_BIT0 | MASK_BIT1)),
	       (U5500_GPIOBANK2_BASE + DB5500_GPIO_AFSLB));

	 /* Configure GPIO settings */
	db8500_gpio_config_pins(gpio_config,
					ARRAY_SIZE(gpio_config));

	prcmu_init();

	return 0;
}

int dram_init(void)
{

	gd->bd->bi_dram[0].start = 0;
	gd->bd->bi_dram[0].size = 0x20000000;	/* 512 MiB */

	return 0;
}


int restarted(void)
{
	return data_init_flag > 0;
}


/*
 * board_mmc_init - initialize all the mmc/sd host controllers.
 * Called by generic mmc framework.
 */
int board_mmc_init(bd_t *bis)
{
	int error;
	struct mmc *dev;
	int mmc_blk_id = 0;

	debugX(DBG_LVL_VERBOSE, "mmc_host - board_mmc_init\n");

	(void) bis;  /* Parameter not used! */

	/*
		Dynamic detection of booting device by reading
		ROM debug register from BACKUP RAM and register the
		corresponding EMMC.
		This is done due to wrong configuration of MMC0 clock
		in ROM code for DB5500 v2.0.
	*/
	if (cpu_is_u5500v20() || cpu_is_u5500v21())
		mmc_blk_id = readl(U5500_BACKUPRAM1_BASE +
					BACKUPRAM_ROM_DEBUG_ADDR);

	dev = u8500_alloc_mmc_struct();
	if (!dev)
		return -1;

	if (mmc_blk_id & MMC_BLOCK_ID) {
		error = u8500_emmc_host_init(dev,
			(struct sdi_registers *)U5500_SDI2_BASE);
		if (error) {
			printf("emmc2 %d\n", error);
			return -1;
		}
	} else {
		error = u8500_emmc_host_init(dev,
			(struct sdi_registers *)U5500_SDI0_BASE);
		if (error) {
			printf("emmc_host_init() %d\n", error);
			return -1;
		}
	}
	mmc_register(dev);
	debugX(DBG_LVL_VERBOSE, "registered emmc interface number is:%d\n",
		dev->block_dev.dev);
	mmc_init(dev);

	/*
	 * In a perfect world board_early_access shouldn't be here but we want
	 * some functionality to be loaded as quickly as possible and putting it
	 * here will get the shortest time to start that functionality. Time
	 * saved by putting it here compared to later is somewhere between
	 * 0.3-0.7s. That is enough to be able to justify putting it here.
	 */

	/*
	 * Since we are interrupting the 'generic' MMC device printouts,
	 * we reformat a bit due to the early access code
	 */

	printf("\n");
	board_early_access(&dev->block_dev);
	printf("  ");

	dev = u8500_alloc_mmc_struct();
	if (!dev)
		return -1;

	error = u8500_mmc_host_init(dev,
			(struct sdi_registers *)U5500_SDI1_BASE);
	if (error) {
		printf("mmc_host_init() %d \n", error);
		return -1;
	}

	mmc_register(dev);
	debugX(DBG_LVL_VERBOSE, "registered mmc/sd interface number is:%d\n",
	      dev->block_dev.dev);

	return 0;
}

#ifdef BOARD_LATE_INIT

/*
 * Set the androidboot.serialno. USB is froming the serial number by
 * reading contents of Backup Ram (PUBLIC_ID_BACKUPRAM1). Exactly
 * same mechanism is used here.
 *
 */
#define UBOOT_PUBLIC_ID_BACKUPRAM1		(U5500_BACKUPRAM1_BASE + 0x0FC0)
#define UBOOT_MAX_USB_SERIAL_NUMBER_LEN 31


static void db5500_set_androidboot_serialnum(void)
{
	u32 bufer[4], len;
	char buf[UBOOT_MAX_USB_SERIAL_NUMBER_LEN + 10];
	char *env_ptr = NULL;
	char *arg_str = NULL;
	char def_arg_str[] = "setenv bootargs ${bootargs}";
	u32 *backup_ram = (u32 *) (UBOOT_PUBLIC_ID_BACKUPRAM1);

	if (backup_ram) {
		bufer[0] = backup_ram[0];
		bufer[1] = backup_ram[1];
		bufer[2] = backup_ram[2];
		bufer[3] = backup_ram[3];

		/* Form the serial number */
		sprintf(buf, "%.8X%.8X%.8X%.8X",
				bufer[0], bufer[1], bufer[2],
				bufer[3]);

	} else
		return;

	/* Append the Serial number to the boot arguments */
	env_ptr = getenv("extraargs");

	if (env_ptr == NULL) {
		/* extraargs is not defined. Define it */
		env_ptr = def_arg_str;
	}

	len = UBOOT_MAX_USB_SERIAL_NUMBER_LEN + strlen(env_ptr)
		+ strlen(" androidboot.serialno=") + 1;

	arg_str = (char *) malloc(len);

	if (arg_str) {
		sprintf(arg_str, "%s androidboot.serialno=", env_ptr);
	} else {
		printf("WARN: maloc failled.\n");
		return;
	}

	strncat(arg_str, buf, UBOOT_MAX_USB_SERIAL_NUMBER_LEN);
	setenv("extraargs", arg_str);
	free(arg_str);

	return;
}


/*
 * called after all initialisation were done, but before the generic
 * mmc_initialize().
 */
int board_late_init(void)
{
	char strbuf[80];
	u8 byte;
	unsigned int  prcmu_reset_status_reg;

	#ifdef CONFIG_VIDEO_LOGO
	if (mcde_error) {
		setenv("startup_graphics", "0");
		setenv("logo", "0");
	} else {
		setenv("startup_graphics", "1");
		setenv("logo", "nologo");
	}
#endif
	/*
	 * Create a memargs variable which points uses either the memargs256 or
	 * memargs512 environment variable, depending on the memory size.
	 * memargs is used to build the bootargs, memargs256 and memargs512 are
	 * stored in the environment.
	 */
	if (gd->bd->bi_dram[0].size == 0x10000000) {
		setenv("memargs", "setenv bootargs ${bootargs} ${memargs256}");
		setenv("mem", "256M");
	} else {
		setenv("memargs", "setenv bootargs ${bootargs} ${memargs512}");
		setenv("mem", "512M");
	}

	/* LDO G for SD card (2.91V) */
	byte = 0x3f;
	(void) prcmu_abb_write(0x3, 0x78, &byte, (u8)sizeof(byte));
	/*
	 * Set MMC_CARD_VSEL (GPIO185) to '0', since we use 2.9V for
	 * interfacing the SD-card (set '1' if it is 1.8V).
	 * Set MMC_CARD_CTRL (GPIO227) to '1' to enable the levelshifter
	 * outputs.
	 */
	db8500_gpio_make_output(GPIO187_GPIO, 0);
	db8500_gpio_make_output(GPIO227_GPIO, 1);

#ifdef CONFIG_MMC

	/* Configure SD Card GPIO settings */
	db8500_gpio_config_pins(sdcard_gpio_config,
					ARRAY_SIZE(sdcard_gpio_config));

#endif
#ifdef CONFIG_U5500_KEYPAD
	/* init keypad */
	init_keypad();
#endif /* CONFIG_U5500_KEYPAD */

	if (itp_is_lbp_modem_in_config())
		setenv("modem_boot_type", "itp");
	else
		setenv("modem_boot_type", "normal");
	/*
	* Create crashkernel env dynamically since it depends on U-Boot start
	* address. U-Boot itself is used for dumping.
	* The 32K offset is hardcoded in the kexec-tools.
	* Parsed by Linux setup.c:reserve_crashkernel() using
	* lib/cmdline.c:memparse().
	* crashkernel=ramsize-range:size[,...][@offset]
	*/
	sprintf(strbuf, "crashkernel=1M@0x%lx", _armboot_start - 0x8000);
	setenv("crashkernel", strbuf);

	/*
	* Check for a crashdump, if data_init_flag > 0, i.e. we were
	* restarted e.g. by kexec. Do not check for crashdump if we were just
	* loaded from the x-loader.
	*/

	prcmu_reset_status_reg = readl(PRCM_RESET_STATUS);
	if (restarted() ||
		(prcmu_reset_status_reg & (PRCM_A9_CPU_WATCHDOG_RESET)) ||
		prcmu_crashed())
			setenv("preboot", "checkcrash");

	db5500_set_androidboot_serialnum();
	backlight_init();
	return 0;
}
#endif /* BOARD_LATE_INIT */
/* sets the peripheral boot mode */
void set_to_peripheral_boot_mode(void)
{
	writel(PERIPHERAL_BOOT_MODE,
		U5500_BACKUPRAM1_BASE + BACKUPRAM_ROM_DEBUG_ADDR);
}
