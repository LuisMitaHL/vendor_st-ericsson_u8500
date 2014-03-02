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
#include <platform/timer.h> /* udelay */
#include <reg.h>
#include <dev/keys.h>
#include <dev/display.h>

#include "target_config.h"
#include "abx500.h"
#include "usb_abx500.h"
#include "db_gpio.h"

#include "mmc_if.h"
#include "toc.h"
#include "tiny_env.h"
#include "partition_parser.h"
#include "i2c.h"
#include "platform.h"
#include <target.h>

#include "boottime.h"
#include "machineid.h"
#include "bass_app.h"
#include "nomadik_mapping.h"


/*******************************************************************************
 * Types, constants
 ******************************************************************************/

static void mop500_gpio_config(void)
{
    /* I2C */
    db_gpio_set(147, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* I2C0 SCL */
    db_gpio_set(148, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* I2C0 SDA */
    db_gpio_set(16, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C1 SCL */
    db_gpio_set(17, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C1 SDA */
    db_gpio_set(10, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C2 SDA */
    db_gpio_set(11, GPIO_ALTB_FUNC | GPIO_PULLUP);      /* I2C2 SCL */
    db_gpio_set(229, GPIO_ALTC_FUNC | GPIO_PULLUP);     /* I2C3 SDA */
    db_gpio_set(230, GPIO_ALTC_FUNC | GPIO_PULLUP);     /* I2C3 SCL */

    /* SSP0, to AB8500 */
    db_gpio_set(143, GPIO_ALTA_FUNC);
    db_gpio_set(144, GPIO_ALTA_FUNC);
    db_gpio_set(145, GPIO_ALTA_FUNC);
    db_gpio_set(146, GPIO_ALTA_FUNC);

    /* UART2, console */
    db_gpio_set(29, GPIO_ALTC_FUNC | GPIO_PULLUP);
    db_gpio_set(30, GPIO_ALTC_FUNC | GPIO_OUT_HIGH);
    db_gpio_set(31, GPIO_ALTC_FUNC | GPIO_PULLUP);
    db_gpio_set(32, GPIO_ALTC_FUNC | GPIO_OUT_HIGH);

    /* MMC2 (POP eMMC) */
    db_gpio_set(128, GPIO_ALTA_FUNC | GPIO_OUT_LOW);    /*  MC2 CLK   */
    db_gpio_set(129, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 CMD   */
    db_gpio_set(130, GPIO_ALTA_FUNC);                   /*  MC2 FBCLK */
    db_gpio_set(131, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT0  */
    db_gpio_set(132, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT1  */
    db_gpio_set(133, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT2  */
    db_gpio_set(134, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT3  */
    db_gpio_set(135, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT4  */
    db_gpio_set(136, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT5  */
    db_gpio_set(137, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT6  */
    db_gpio_set(138, GPIO_ALTA_FUNC | GPIO_PULLUP);     /*  MC2 DAT7  */

    /* MMC0 (SD) */
    db_gpio_set(18, GPIO_ALTA_FUNC | GPIO_OUT_HIGH);    /* MC0_CMDDIR   */
    db_gpio_set(19, GPIO_ALTA_FUNC | GPIO_OUT_HIGH);    /* MC0_DAT0DIR  */
    db_gpio_set(20, GPIO_ALTA_FUNC | GPIO_OUT_HIGH);    /* MC0_DAT2DIR  */
    db_gpio_set(21, GPIO_ALTA_FUNC | GPIO_OUT_HIGH);    /* MC0_DAT31DIR */
    db_gpio_set(22, GPIO_ALTA_FUNC);                    /* MC0_FBCLK    */
    db_gpio_set(23, GPIO_ALTA_FUNC | GPIO_OUT_LOW );    /* MC0_CLK      */
    db_gpio_set(24, GPIO_ALTA_FUNC | GPIO_PULLUP);      /* MC0_CMD      */
    db_gpio_set(25, GPIO_ALTA_FUNC | GPIO_PULLUP);      /* MC0_DAT0     */
    db_gpio_set(26, GPIO_ALTA_FUNC | GPIO_PULLUP);      /* MC0_DAT1     */
    db_gpio_set(27, GPIO_ALTA_FUNC | GPIO_PULLUP);      /* MC0_DAT2     */
    db_gpio_set(28, GPIO_ALTA_FUNC | GPIO_PULLUP);      /* MC0_DAT3     */

    /* MMC4 (OnBoard) */
    db_gpio_set(197, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT3    */
    db_gpio_set(198, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT2    */
    db_gpio_set(199, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT1    */
    db_gpio_set(200, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT0    */
    db_gpio_set(201, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_CMD     */
    db_gpio_set(202, GPIO_ALTA_FUNC);                   /* MC4_FBCLK   */
    db_gpio_set(203, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_CLK     */
    db_gpio_set(204, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT7    */
    db_gpio_set(205, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT6    */
    db_gpio_set(206, GPIO_ALTA_FUNC | GPIO_PULLUP);     /* MC4_DAT5    */
    db_gpio_set(207, GPIO_ALTA_FUNC);                   /* MC4_DAT4    */

    /* USB */
    db_gpio_set(256, GPIO_ALTA_FUNC);                   /* USB_NXT     */
    db_gpio_set(257, GPIO_ALTA_FUNC | GPIO_OUT_HIGH);   /* USB_STP     */
    db_gpio_set(258, GPIO_ALTA_FUNC);                   /* USB_XCLK    */
    db_gpio_set(259, GPIO_ALTA_FUNC);                   /* USB_DIR     */
    db_gpio_set(260, GPIO_ALTA_FUNC);                   /* USB_DAT7    */
    db_gpio_set(261, GPIO_ALTA_FUNC);                   /* USB_DAT6    */
    db_gpio_set(262, GPIO_ALTA_FUNC);                   /* USB_DAT5    */
    db_gpio_set(263, GPIO_ALTA_FUNC);                   /* USB_DAT4    */
    db_gpio_set(264, GPIO_ALTA_FUNC);                   /* USB_DAT3    */
    db_gpio_set(265, GPIO_ALTA_FUNC);                   /* USB_DAT2    */
    db_gpio_set(266, GPIO_ALTA_FUNC);                   /* USB_DAT1    */
    db_gpio_set(267, GPIO_ALTA_FUNC);                   /* USB_DAT0    */

    /* SD0 Level shifter */
    db_gpio_set(169, GPIO_OUT_HIGH);                    /* SD0 LS EN */
    db_gpio_set(5, GPIO_OUT_LOW);                       /* SD0 LS Voltage select */
    db_gpio_set(95, GPIO_PULLUP);                       /* SD0 CD */
    //db_gpio_set(226, GPIO_ALTC_FUNC);                   /* USBSIM_PDC  */

    /* Display Interface */
    db_gpio_set(65, GPIO_OUT_LOW);                      /* DISP1 RST */
    db_gpio_set(66, GPIO_OUT_LOW);                      /* DISP2 RST */
}


char asic_id_str[20] = "0x00000000";


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

/* defined in target specific keypad.c */
void keypad_init(void);



/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void target_init(void)
{
#if !defined(ENABLE_FEATURE_BUILD_HBTS)
    mmc_properties_t    *mmc;
#endif
    int                 chipid;
    int                 status;

#ifdef KEYPAD_TEST
    uint8_t key_return;
    uint8_t keys[MAX_MULT_KEYS];
#endif

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if WITH_TINY_ENV
    uint32_t            ramsize;
#endif
#endif

    boottime_tag("lk_init");

    dprintf(INFO, "mop500 target_init()\n");

    /* Configure GPIO pins */
    mop500_gpio_config();

    /* Init I2C_0 */
    (void) i2c_init(I2C_0);

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
    /* PoPed eMMC */
    mmc = mmc_init (U8500_SDI2_BASE, &status, EXPECT_MMC);
    if(status != MMC_OK) {
        /* on-board eMMC (SDI4) */
        mmc = mmc_init (U8500_SDI4_BASE, &status, EXPECT_MMC);
    }
    dprintf(INFO, "mmc init status %d\n", status);

    if (mmc) {
        toc_init_mmc (mmc);
        fb_partitions_init();

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
    else if (chipid < AB8500_CUT1P0)
        dprintf(CRITICAL, "Unsupported AB8500 chip_id=%d, ab8500 USB not initialized\n",chipid);
    else {
        /* Init ab8500 USB */
        status = ab8500_usb_init(chipid == AB8500_CUT3P0);
        dprintf(INFO, "ab8500 USB init status %d\n", status);
    }

    /* Init keys */
    keys_init();
    keypad_init();

#if !defined(ENABLE_FEATURE_BUILD_HBTS)
    /* Splashscren */
#if CONFIG_VIDEO_LOGO
    if(mmc) {
        boottime_tag("splash");
        status = mcde_splash_image();
        dprintf(INFO, "mcde_splash_image: %d\n", status);
        if(status)
            return;
    }
#endif
#endif

    return;
}


#define BOOT_INFO_BACKUPRAM         BACKUP_RAM_START_ADDR + 0x37C
#define BOARD_VERSION_MASK          0xffff
#define BOARD_FAMILY_MASK           0xff00
#define BOARD_FAMILY_8520           0x2000


unsigned int get_machine_id (void)
{
    unsigned int                cpu_id;
    unsigned int                asic_id;
    static unsigned int         mach_type = MACH_TYPE_INVALID;
    struct tee_product_config   config;
    bass_return_code            bass_ret;
    unsigned char               byte;

    bass_ret = get_product_config(&config);

    if (BASS_RC_SUCCESS != bass_ret) {
        dprintf (CRITICAL, "get_machine_id: Unable to retreive product config\n");
        return MACH_TYPE_INVALID;
    }

    if (PRODUCT_ID_9500 == config.product_id) {
        dprintf (INFO, "get_machine_id: MACH_TYPE_A9500\n");
        return MACH_TYPE_A9500;
    }

    if (mach_type != MACH_TYPE_INVALID)
        return mach_type;

    /* Get CPU ID from Main ID register (MIDR) */
    __asm__("mrc        p15, 0, %0, c0, c0, 0"
        : "=r" (cpu_id)
        :
        : "cc");
    dprintf (INFO, "get_machine_id: cpu_id 0x%08x\n", cpu_id);

    switch (cpu_id) {
    case CPUID_DB8500ED:
    /* Fall through */
    case CPUID_DB8500V1:
    /* Fall through */
    case CPUID_DB8500V2:
        asic_id = readl(ASIC_ID_ADDR);
        dprintf (INFO, "get_machine_id: asic_id 0x%08x\n", asic_id);

        if (ASICID_DB8520V22 == asic_id) {
            dprintf (INFO, "get_machine_id: MACH_TYPE_U8520\n");
            mach_type = MACH_TYPE_U8520;
        } else {
            /* HREF+ V60 and above have no GPIO expander, use that
             * to set machine type*/
            if (!i2c_read_reg(I2C_0, I2C_GPIOE_ADDR, 0x80, &byte)) {
                dprintf (INFO, "get_machine_id: MACH_TYPE_U8500\n");
                mach_type = MACH_TYPE_U8500;
            } else {
                /* No response, i.e. no expander */
                dprintf (INFO, "get_machine_id: MACH_TYPE_HREFV60\n");
                mach_type = MACH_TYPE_HREFV60;
            }

            /* check board profile in backup ram */
            uint32_t *boot_info = (uint32_t*) (BOOT_INFO_BACKUPRAM);

            if ((*boot_info & BOARD_FAMILY_MASK) == BOARD_FAMILY_8520) {
                mach_type = MACH_TYPE_U8520;
                dprintf (INFO, "get_machine_id overridden: MACH_TYPE_U8520\n");
            }
        }
        sprintf (asic_id_str, "0x%08x", asic_id);
        break;

    case CPUID_DB9540V1: /* 9540 is separated at compile time */
    /* Fall through */
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

    /* Set the voltage to 2.91 V or 2.9 V without overriding VRF1 value */
    status = ab8500_reg_read(AB8500_REGU_VRF1VAUX3_SEL_REG);
    if (status < 0) {
        return status;
    }

    vaux3_sel = status;

    if (ab8500_revision < AB8500_CUT2P0) {
        vaux3_sel &= ~VAUX3_SEL_MASK;
        vaux3_sel |= VAUX3_SEL_2V9;
    } else {
        vaux3_sel &= ~VAUX3_V2_SEL_MASK;
        vaux3_sel |= VAUX3_V2_SEL_2V91;
    }

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
    uint32_t size_cs0;
    uint32_t size_cs1;

    /* Determine RAM size by examining which rows and columns are
     * NOT used
     * Assuming CS0 and CS1 are used
     * 8500_v2 spec, Ch 17
     */
    reg = readl(DMC_CTL_96);
    addr_pins_0 = (reg >> 24) & 0x00000007;
    reg = readl(DMC_CTL_97);
    column_size_1 = (reg >> 16) & 0x00000007;
    column_size_0 = (reg >>8) & 0x00000007;
    addr_pins_1 = reg & 0x00000007;

    size_cs0 = (1 << ((DMC_MAX_ROWS - addr_pins_0) + (DMC_MAX_COLS - column_size_0)));
    size_cs0 *= DMC_N_BANKS * DMC_WIDTH;

    size_cs1 = (1 << ((DMC_MAX_ROWS - addr_pins_1) + (DMC_MAX_COLS - column_size_1)));
    size_cs1 *= DMC_N_BANKS * DMC_WIDTH;

    return size_cs0 + size_cs1;
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
    return gpio_get(95);
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
    /* no L/S on 8500, return 0 */
    return 0;
}
