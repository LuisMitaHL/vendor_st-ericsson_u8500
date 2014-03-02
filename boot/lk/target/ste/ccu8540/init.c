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

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string.h>
#include <debug.h>
#include <platform/timer.h>	/* udelay */
#include <reg.h>
#include <dev/keys.h>
#include <dev/display.h>

#include "target_config.h"
#include "abx500.h"
#include "usb_abx500.h"
#include "db_gpio.h"
#include "prcmu.h"

#include "mmc_if.h"
#include "toc.h"
#include "tiny_env.h"
#include "partition_parser.h"
#include "i2c.h"
#include "platform.h"
#include <target.h>

#include "boottime.h"
#include "machineid.h"

#include <bass_app.h>

#include "keypad.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/


static void ccu8540_gpio_config(void)
{
	/* I2C */
	db_gpio_set(147, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* I2C0 SCL         */
	db_gpio_set(148, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* I2C0 SDA         */
	db_gpio_set(16, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C1 SCL         */
	db_gpio_set(17, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C1 SDA         */
	db_gpio_set(10, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C2 SDA         */
	db_gpio_set(11, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C2 SCL         */
	db_gpio_set(122, GPIO_ALTB_FUNC | GPIO_PULLUP);     /* I2C4 SCL         */
	db_gpio_set(123, GPIO_ALTB_FUNC | GPIO_PULLUP);     /* I2C4 SDA         */
	db_gpio_set(118, GPIO_ALTC_FUNC | GPIO_PULLUP);     /* I2C5 SCL         */
	db_gpio_set(119, GPIO_ALTC_FUNC | GPIO_PULLUP);     /* I2C5 SDA         */

	/* UART2, console */
	db_gpio_set(120, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* U2 RXD           */
	db_gpio_set(121, GPIO_ALTA_FUNC);                   /* U2 TXD           */

	/* MMC0 (SD) */
	db_gpio_set(23, GPIO_ALTA_FUNC);                    /* MC0_CLK          */
	db_gpio_set(24, GPIO_ALTA_FUNC);                    /* MC0_CMD          */
	db_gpio_set(25, GPIO_ALTA_FUNC);                    /* MC0_DAT0         */
	db_gpio_set(26, GPIO_ALTA_FUNC);                    /* MC0_DAT1         */
	db_gpio_set(27, GPIO_ALTA_FUNC);                    /* MC0_DAT2         */
	db_gpio_set(28, GPIO_ALTA_FUNC);                    /* MC0_DAT3         */

	/* MMC4 (OnBoard) */
	db_gpio_set(197, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT3         */
	db_gpio_set(198, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT2         */
	db_gpio_set(199, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT1         */
	db_gpio_set(200, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT0         */
	db_gpio_set(201, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_CMD          */
	db_gpio_set(202, GPIO_ALTC_FUNC | GPIO_PULLUP);     /* MC4_RSTN         */
	db_gpio_set(203, GPIO_ALTA_FUNC | GPIO_PULLDOWN);   /* MC4_CLK          */
	db_gpio_set(204, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT7         */
	db_gpio_set(205, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT6         */
	db_gpio_set(206, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT5         */
	db_gpio_set(207, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT4         */

	/* USB */
	db_gpio_set(256, GPIO_ALTA_FUNC);                   /* USB_NXT          */
	db_gpio_set(257, GPIO_ALTA_FUNC);                   /* USB_STP          */
	db_gpio_set(258, GPIO_ALTA_FUNC);                   /* USB_XCLK         */
	db_gpio_set(259, GPIO_ALTA_FUNC);                   /* USB_DIR          */
	db_gpio_set(260, GPIO_ALTA_FUNC);                   /* USB_DAT7         */
	db_gpio_set(261, GPIO_ALTA_FUNC);                   /* USB_DAT6         */
	db_gpio_set(262, GPIO_ALTA_FUNC);                   /* USB_DAT5         */
	db_gpio_set(263, GPIO_ALTA_FUNC);                   /* USB_DAT4         */
	db_gpio_set(264, GPIO_ALTA_FUNC);                   /* USB_DAT3         */
	db_gpio_set(265, GPIO_ALTA_FUNC);                   /* USB_DAT2         */
	db_gpio_set(266, GPIO_ALTA_FUNC);                   /* USB_DAT1         */
	db_gpio_set(267, GPIO_ALTA_FUNC);                   /* USB_DAT0         */

	/* SDMMC */
	db_gpio_set(22, GPIO_PULLUP);                       /* SERVICE_DB       */
	db_gpio_set(230, GPIO_PULLUP);                      /* SDMMC_CD         */

	/* Display Interface */
	db_gpio_set(125, GPIO_OUT_LOW);                     /* DISP0_RST        */
}


char asic_id_str[20] = "0x00000000";


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

static int get_ddr_interleave_config(void);


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void target_init(void)
{
#if !defined(ENABLE_FEATURE_BUILD_HBTS)
	mmc_properties_t	*mmc;
#endif
	int					chipid;
	int 				status;
#ifdef KEYPAD_TEST
	uint8_t key_return;
	uint8_t keys[MAX_MULT_KEYS];
#endif

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if WITH_TINY_ENV
	uint32_t			ramsize;
#endif
#endif

	boottime_tag("lk_init");

	dprintf(INFO, "ccu8540 target_init()\n");

	/* Configure GPIO pins */
	ccu8540_gpio_config();

	/* Init I2C_1 */
	(void) i2c_init(I2C_1);

#ifdef KEYPAD_TEST
	key_return = keys_pressed(keys);
	if (key_return != KEYPAD_NOT_SUPPORTED ) {
		int i;
		for(i=0;i<key_return;i++) {
			if (keys[i] == FB_MAGIC_KEY_BACK)
				printf("KEY_BACK pressed\n");
			else if (keys[i] == RC_MAGIC_KEY_HOME)
				printf("KEY_HOME pressed\n");
			else if (keys[i] == RC_MAGIC_KEY_VOLUMEUP)
				printf("KEY_VOL+ pressed\n");
			else if (keys[i] == FB_MAGIC_KEY_VOLUMEDOWN)
				printf("KEY_VOL- pressed\n");
		}
	}
	else
		printf("No keypad found\n");

	close_keypad();
#endif


#if !defined(ENABLE_FEATURE_BUILD_HBTS)
	/* on-board eMMC (SDI4) */
	mmc = mmc_init (U8500_SDI4_BASE, &status, EXPECT_MMC);
	dprintf(INFO, "mmc init status %d\n", status);

	if (mmc) {
		toc_init_mmc (mmc);
		fb_partitions_init();

/* Splashscreen */
#if CONFIG_VIDEO_LOGO
		boottime_tag("splash");
		status = mcde_splash_image();
		dprintf(INFO, "mcde_splash_image: %d\n", status);
#endif

#if WITH_TINY_ENV
		tenv_init();

		if (0 == tenv_load()) {
			char *str;
			/* Extract partitions for later use */
			str = tenv_getval("blkdevparts");
			dprintf(INFO,"blkdevparts : %s\n", str);
			if (str == NULL) {
				dprintf(INFO,
					"no blkdevparts found for partition parsing\n");
			} else {
				if (0 == parse_blkdev_parts(str, "mmcblk0")) {
					dprintf(INFO,
					"no partion defined on Linux cmdline\n");
				}
			}

			/* Sort out which memory parameters to use */
			if (!tenv_getval("memargs")) {
				ramsize = get_dmc_size();
				dprintf(INFO, "RAM size 0x%08x\n", ramsize);

				switch (ramsize) {
				case MEM_256:
					dprintf (INFO, "Setting memargs to MEM_256\n");
					create_env("memargs", "${memargs256}", true);
					break;
				case MEM_512:
					dprintf (INFO, "Setting memargs to MEM_512\n");
					create_env("memargs", "${memargs512}", true);
					break;
				case MEM_768:
					dprintf (INFO, "Setting memargs to MEM_768\n");
					create_env("memargs", "${memargs768}", true);
					break;
				case MEM_1024:
					dprintf (INFO, "Setting memargs to MEM_1024\n");
					create_env("memargs", "${memargs1024}", true);
					create_env("ddrdie", "${ddrdie1024}", true);
					get_ddr_interleave_config();
					break;
				case MEM_2048:
					dprintf (INFO, "Setting memargs to MEM_2048\n");
					create_env("memargs", "${memargs2048}", true);
					create_env("ddrdie", "${ddrdie2048}", true);
					get_ddr_interleave_config();
					break;
				default:
					dprintf (INFO, "Invalid size, falling back to 512MB\n");
					create_env("memargs", "${memargs512}", true);
					break;
				}
			}
			/* Now memargs is properly set, remove templates */
			tenv_remove_env ("memargs256");
			tenv_remove_env ("memargs512");
			tenv_remove_env ("memargs768");
			tenv_remove_env ("memargs1024");
			tenv_remove_env ("ddrdie1024");
			tenv_remove_env ("memargs2048");
			tenv_remove_env ("ddrdie2048");

			set_androidboot_serialno();
		}
#endif
	}
#endif

	/* Init ab8500 */
	status = ab8500_dev_init();
	dprintf(INFO, "ab8500 init status %d\n", status);

	/* Check chip version */
	chipid = ab8500_chip_id_read();
	if (chipid < 0)
		dprintf(CRITICAL, "chipid_get failed: %d, ab8500 USB not initialized\n",chipid);
	else {
		/* Init ab8500 USB: no PHY tunning for AB8540 bring-up */
		status = ab8500_usb_init(false);
		dprintf(INFO, "ab8500 USB init status %d\n", status);
	}

	/* Init keypad */
	keys_init();
	keypad_init();

	return;
}


unsigned int get_machine_id (void)
{
	unsigned int 				cpu_id;
	unsigned int 				asic_id;
	unsigned int				mach_type;

	/* Get CPU ID from Main ID register (MIDR) */
	__asm__("mrc        p15, 0, %0, c0, c0, 0"
		: "=r" (cpu_id)
		:
		: "cc");
	dprintf (INFO, "get_machine_id: cpu_id 0x%08x\n", cpu_id);

	switch (cpu_id) {
	case CPUID_DB8540V1:
		asic_id = readl(ASIC_ID_ADDR);
		dprintf (INFO, "get_machine_id: asic_id 0x%08x\n", asic_id);
		mach_type = MACH_TYPE_U8540;
		dprintf (INFO, "get_machine_id: MACH_TYPE_U8540\n");
		sprintf (asic_id_str, "0x%08x", asic_id);
		break;

	default:
		dprintf (INFO, "get_machine_id: MACH_TYPE_INVALID\n");
		mach_type = MACH_TYPE_INVALID;
		break;
	}

	return mach_type;
}


char *get_asic_id_str ()
{
	return asic_id_str;
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

int enable_vaux3 ()
{
	int status;
	int vaux3_sel;
	int vaux3_regu;
	int ab8500_revision;

	/* Determine AB8500 revision */
	status = ab8500_reg_read(AB8500_REV_REG);
	if (status < 0) {
		return status;
	}
	ab8500_revision = status;

	/* Switch off VAUX3 prior to changing setting  */
	status = ab8500_reg_read(AB8500_REGU_VRF1VAUX3_REGU_REG);
	if (status < 0) {
		return status;
	}

	vaux3_regu = status;

	status = ab8500_reg_write(AB8500_REGU_VRF1VAUX3_REGU_REG,
							  vaux3_regu & ~VAUX3_REGU_MASK);
	if (status < 0) {
		return status;
	}

	/* Delay */
	udelay(10 * 1000);

	/* Set the voltage to 2.91 V without overriding VRF1 value */
	status = ab8500_reg_read(AB8500_REGU_VRF1VAUX3_SEL_REG);
	if (status < 0) {
		return status;
	}

	vaux3_sel = status;

	vaux3_sel &= ~VAUX3_V2_SEL_MASK;
	vaux3_sel |= VAUX3_V2_SEL_2V91;

	status = ab8500_reg_write(AB8500_REGU_VRF1VAUX3_SEL_REG, vaux3_sel);
	if (status < 0) {
		return status;
	}

	/* Turn on the supply */
	vaux3_regu &= ~VAUX3_REGU_MASK;
	vaux3_regu |= VAUX3_REGU_VAL;

	status = ab8500_reg_write(AB8500_REGU_VRF1VAUX3_REGU_REG, vaux3_regu);

	return status;
}

int disable_vaux3 ()
{
    int status;
    int vaux3_regu;

    /* Switch off VAUX3 */
    status = ab8500_reg_read(AB8500_REGU_VRF1VAUX3_REGU_REG);
    if (status < 0) {
        return status;
    }

    vaux3_regu = status;

    status = ab8500_reg_write(AB8500_REGU_VRF1VAUX3_REGU_REG,
                              vaux3_regu & ~VAUX3_REGU_MASK);

    return status;
}

uint32_t get_dmc_size (void)
{
	uint32_t addr_pins_0;
	uint32_t addr_pins_1;
	uint32_t column_size_1;
	uint32_t column_size_0;
	uint32_t reg;
	uint32_t size_cs0, size_cs0_2;
	uint32_t size_cs1, size_cs1_2;

	/* Determine RAM size by examining which rows and columns are
	 * NOT used
	 * 8500_v2 spec, Ch 17
	 */
	reg = readl(DMC_CTL_96);
	addr_pins_0 = (reg >> 24) & 0x00000007;

	reg = readl(DMC_CTL_97);
	column_size_1 = (reg >> 16) & 0x00000007;
	column_size_0 = (reg >>8) & 0x00000007;
	addr_pins_1 = reg & 0x00000007;

	reg = readl(DMC_CTL_12);
	if ((reg >> 8) & 0x1) {
		/* cs0 is active */
		size_cs0 = (1 << ((DMC_MAX_ROWS - addr_pins_0) + (DMC_MAX_COLS - column_size_0)));
		size_cs0 *= DMC_N_BANKS * DMC_WIDTH;
	} else
		size_cs0 = 0;

	if ((reg >> 8) & 0x2) {
		/* cs1 is active */
		size_cs1 = (1 << ((DMC_MAX_ROWS - addr_pins_1) + (DMC_MAX_COLS - column_size_1)));
		size_cs1 *= DMC_N_BANKS * DMC_WIDTH;
	} else
		size_cs1=0;

	/* CCU8540 has two DDR controllers, just repeat the sequence */
	reg = readl(DMC1_CTL_96);
	addr_pins_0 = (reg >> 24) & 0x00000007;

	reg = readl(DMC1_CTL_97);
	column_size_1 = (reg >> 16) & 0x00000007;
	column_size_0 = (reg >>8) & 0x00000007;
	addr_pins_1 = reg & 0x00000007;

	reg = readl(DMC1_CTL_12);
	if ((reg >> 8) & 0x1) {
		/* cs0 is active */
		size_cs0_2 = (1 << ((DMC_MAX_ROWS - addr_pins_0) + (DMC_MAX_COLS - column_size_0)));
		size_cs0_2 *= DMC_N_BANKS * DMC_WIDTH;
	} else
		size_cs0_2=0;

	if ((reg >> 8) & 0x2) {
		/* cs1 is active */
		size_cs1_2 = (1 << ((DMC_MAX_ROWS - addr_pins_1) + (DMC_MAX_COLS - column_size_1)));
		size_cs1_2 *= DMC_N_BANKS * DMC_WIDTH;
	} else
		size_cs1_2=0;

	return size_cs0 + size_cs1 + size_cs0_2 + size_cs1_2;
}

/*
 * mmc_sd_detected
 *
 * Detect SD card
 *
 * Parameters: -
 *
 * Returns: 0 if no card is detected, 1 otherwise
 */
int mmc_sd_detected(void)
{
	return !gpio_get(230);
}

/*
 * mmc_sd_set_startup_iosv
 *
 * Set io signal voltage when board startup
 * Enable internal L/S, provide signal voltage as 3v3
 *
 * Returns: status of operation performed
 */
int mmc_sd_set_startup_iosv(void)
{
    int status;
    int value;

    prcmu_set_psw(true);
    value = ab8500_reg_read(AB8540_REGU_VSDIO_REG);
    if (value < 0)
        return value;
    else {
        value |= AB8540_REGU_VSDIO_REG_ENA;
        value &= AB8540_REGU_VSDIO_REG_EN_VAL_CLEAR;
        value |= AB8540_REGU_VSDIO_REG_EN_VAL_3V3;
        status = ab8500_reg_write(AB8540_REGU_VSDIO_REG, value);
    }
    if (status < 0)
        return status;
    udelay(3000);

    return status;
}

/*
 * get_ddr_interleave_config
 *
 * Get ddr interleave configuration and set interleaved env parameter
 *
 * interleaved env parameter format:
 * interleaveaved=<size>@<start_1>:<start_2>_<page_size>
 *
 * Returns: 0 if successfull, -1 in case of error
 */
static int get_ddr_interleave_config(void)
{
    uint8_t             intlv_en;
    uint16_t            intlv_page_size;
    uint32_t            intlv_end_addr;
    uint32_t            intlv_start_addr;
    uint32_t            intlv_size;
    uint16_t            start1, start2;
    char                intlv[20];
    uint32_t            interlv_config=0;
    size_t              faulty_entry;
    bass_return_code    return_code = BASS_RC_SUCCESS;

    const uint32_t imad_descr[5] = {
        0x44414D49, /* magic: "IMAD" */

        /* sequence INTERLEAVING: Get interleaving config */
        (uint32_t)IMAD_SEQUENCE_INTERLEAVING,
        (uint32_t)IMAD_TYPE_READ_TO_EXTERNAL,
        (uint32_t)(SECR_INTLV_ADDR), /* get interleaving config */
        (uint32_t)sizeof(uint32_t)
    };

    /* Store indirect memory access descriptor in secure memory */
    return_code = bass_imad_store((const uint8_t *)imad_descr, sizeof(imad_descr));
    if(return_code != BASS_RC_SUCCESS) {
        dprintf(INFO, "error during IMAD store operation\n");
        return return_code;
    }

    const struct bass_imad_entry interleave[] = {
        { IMAD_SEQUENCE_INTERLEAVING, (uint32_t)&interlv_config}
    };

    /* Get interleaving config */
    return_code = bass_imad_exec(interleave, sizeof(interleave)/sizeof(struct bass_imad_entry),
                                 &faulty_entry);
    if(return_code != BASS_RC_SUCCESS)
    {
        dprintf(INFO, "error during IMAD exec operation\n");
        return return_code;
    }

    intlv_en=interlv_config&0x1;
    if (intlv_en) {
        intlv_page_size = (interlv_config >> 1) & 0x1F;
        intlv_end_addr = (interlv_config >> 6) & 0x3F;
        intlv_end_addr = (intlv_end_addr << 24) | 0xFFFFFF;
        intlv_start_addr = (interlv_config >> 12) & 0x3F;
        intlv_start_addr = intlv_start_addr << 24;
        intlv_size = intlv_end_addr - intlv_start_addr + 1;
        intlv_size = (intlv_size / 1024) / 1024;

        start1 = (DDR0_PHYSICAL_OFFSET + intlv_start_addr) / 1024 / 1024;
        start2 = (DDR1_PHYSICAL_OFFSET + intlv_start_addr) / 1024 / 1024;

        switch(intlv_page_size) {
        case 0x4:
                intlv_page_size = 64;
                break;
        case 0x5:
                intlv_page_size = 128;
                break;
        case 0x6:
                intlv_page_size = 256;
                break;
        default:
                dprintf(INFO, "page size error, ddr interleave configuration not set");
                return -1;
                break;
        }
        snprintf(intlv, sizeof(intlv), "%dM@%d:%dM_%dK", intlv_size, start1, start2, intlv_page_size);
        create_env("interleaved", intlv, true);
    }
    else {
        dprintf(INFO, "interleave disabled");
    }

    /* Release the previously stored indirect memory access descriptor */
    return_code = bass_imad_release();
    if(return_code != BASS_RC_SUCCESS) {
        dprintf(INFO, "error during IMAD release operation\n");
        return return_code;
    }
    return 0;
}
