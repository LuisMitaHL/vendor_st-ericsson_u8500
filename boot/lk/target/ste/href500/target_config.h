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

#ifndef __8500_REGS_H
#define __8500_REGS_H


#define U8500_L2CC_BASE     0xa0412000


/* Boot ROM */
#define BOOT_ROM_START_ADDR         0x90017000
#define BOOT_ROM_LENGTH             0x00006C00
#define BOOT_ROM_END_ADDR           (BOOT_ROM_START_ADDR+BOOT_ROM_LENGTH-1)

/* Secure ROM */
#define SEC_ROM_START_ADDR          0x90000000
#define SEC_ROM_LENGTH              0x00017000
#define SEC_ROM_END_ADDR            (SEC_ROM_START_ADDR+SEC_ROM_LENGTH-1)

#define ASIC_ID_ADDR                (SEC_ROM_START_ADDR + 0x1DBF4)

/* Peripheral base addresses */
#define U8500_PERIPH1_BASE          0x80120000
#define U8500_PERIPH2_BASE          0x80110000
#define U8500_PERIPH3_BASE          0x80000000
#define U8500_PERIPH4_BASE          0x80150000
#define U8500_PERIPH5_BASE          0xa03e0000
#define U8500_PERIPH6_BASE          0xa03c0000
#define U8500_PERIPH7_BASE          0xa03d0000


/* PRCC defines */
#define U8500_PER1_PRCC_BASE        (U8500_PERIPH1_BASE + 0xf000)
#define U8500_PER2_PRCC_BASE        (U8500_PERIPH2_BASE + 0xf000)
#define U8500_PER3_PRCC_BASE        (U8500_PERIPH3_BASE + 0xf000)
#define U8500_PER5_PRCC_BASE        (U8500_PERIPH5_BASE + 0x1f000)
#define U8500_PER6_PRCC_BASE        (U8500_PERIPH6_BASE + 0xf000)
#define U8500_PER7_PRCC_BASE        (U8500_PERIPH7_BASE + 0xf000)

/* GIC defines */
#define U8500_GIC_DIST_BASE         0xa0411000
#define U8500_GIC_CPU_BASE          0xa0410100

/* UART defines */
#define U8500_UART0_BASE            (U8500_PERIPH1_BASE + 0x0000)
#define U8500_UART1_BASE            (U8500_PERIPH1_BASE + 0x1000)
#define U8500_UART2_BASE            (U8500_PERIPH3_BASE + 0x7000)

/* GPIO defines */
#define U8500_GPIO0_BASE            (U8500_PERIPH1_BASE + 0xe000)
#define U8500_GPIO1_BASE            (U8500_PERIPH1_BASE + 0xe080)
#define U8500_GPIO2_BASE            (U8500_PERIPH3_BASE + 0xe000)
#define U8500_GPIO3_BASE            (U8500_PERIPH3_BASE + 0xe080)
#define U8500_GPIO4_BASE            (U8500_PERIPH3_BASE + 0xe100)
#define U8500_GPIO5_BASE            (U8500_PERIPH3_BASE + 0xe180)
#define U8500_GPIO6_BASE            (U8500_PERIPH2_BASE + 0xe000)
#define U8500_GPIO7_BASE            (U8500_PERIPH2_BASE + 0xe080)
#define U8500_GPIO8_BASE            (U8500_PERIPH5_BASE + 0x1e000)

/* SD/MMC 0 */
#define U8500_SDI0_BASE             (U8500_PERIPH1_BASE + 0x6000)

/* SD/MMC 2 */
#define U8500_SDI2_BASE             (U8500_PERIPH3_BASE + 0x5000)

/* SD/MMC 4 */
#define U8500_SDI4_BASE             (U8500_PERIPH2_BASE + 0x4000)

/* MISC defines */
#define U8500_SDRAM_BASE            0x00000000
#define U8500_PRCMU_BASE            (U8500_PERIPH4_BASE + 0x7000)
#define U8500_MTU0_BASE             (U8500_PERIPH6_BASE + 0x6000)

#define DMC_BASE                    (U8500_PERIPH4_BASE +0x6000)
#define DMC_CTL_96                  (DMC_BASE + 0x180)
#define DMC_CTL_97                  (DMC_BASE + 0x184)

/* 8500_v2 spec, 17.3.2 */
#define DMC_MAX_ROWS                15
#define DMC_MAX_COLS                11
#define DMC_N_BANKS                 8
#define DMC_WIDTH                   4


/* PRCMU */
#define PRCM_RESET_STATUS               (U8500_PRCMU_BASE + 0x103C)
#define PRCM_A9_CPU0_WATCHDOG_RESET     0x00000001
#define PRCM_A9_CPU1_WATCHDOG_RESET     0x00000002
#define PRCM_A9_SOFTWARE_RESET          0x00000004
#define PRCM_A9_CPU_RESET               0x00000008
#define PRCM_A9_SEC_WATCHDOG_RESET      0x00000010
#define PRCM_A9_POWER_ON_RESET	        0x00000020
#define PRCM_A9_RESTART_RESET	        0x00000040
#define PRCM_APE_RESTART_RESET	        0x00000080
#define PRCM_MODEM_SW_RESET	        0x00000100
#define U8500_PRCMU_TCDM_BASE           (U8500_PERIPH4_BASE + 0x68000)
#define PRCM_SW_RST_REASON              0xFF8
#define PRCM_APE_SOFTRST                (U8500_PRCMU_BASE + 0x228)
#define PRCM_TIMER4_DOWNCOUNT           (U8500_PERIPH4_BASE + 0x07454)
#define DB8500_PRCM_DSI_GLITCHFREE_EN   (U8500_PRCMU_BASE + 0x534)

/* HS USB */
#define USB_HS_BASE         (U8500_PERIPH5_BASE)

/* USB OTG */
#define OTG_BASE            (U8500_PERIPH5_BASE)

#define OTG_REVISION        (OTG_BASE + 0x00)
#define OTG_SYSCONFIG       (OTG_BASE + 0x04)
#define OTG_SYSSTATUS       (OTG_BASE + 0x08)
#define OTG_INTERFSEL       (OTG_BASE + 0x0C)
#define OTG_SIMENABLE       (OTG_BASE + 0x10)
#define OTG_FORCESTDBY      (OTG_BASE + 0x14)

/* I2C */
#define I2C_TC35893_KEYPAD_ADDR         0x44    /* GPIO expander chip addr */
#define I2C_GPIOE_ADDR                  0x42    /* GPIO expander chip addr */
#define I2C_STMPE1601_KEYPAD_ADDR       0x40    /* GPIO expander chip addr */
#define I2C_SPEED           100000
#define I2C_LAST_BUS        3
#define U8500_I2C0_BASE     (U8500_PERIPH3_BASE + 0x4000)
#define U8500_I2C1_BASE     (U8500_PERIPH1_BASE + 0x2000)
#define U8500_I2C2_BASE     (U8500_PERIPH1_BASE + 0x8000)
#define U8500_I2C3_BASE     (U8500_PERIPH2_BASE + 0x0)
#define I2C0_BASE           U8500_I2C0_BASE
#define I2C1_BASE           U8500_I2C1_BASE
#define I2C2_BASE           U8500_I2C2_BASE
#define I2C3_BASE           U8500_I2C3_BASE

/* Display */
#define CONFIG_SYS_VIDEO_FB_ADRS    0x14000000
#define CONFIG_SYS_DISPLAY_NATIVE_X_RES 864
#define CONFIG_SYS_DISPLAY_NATIVE_X_RES2    854
#define CONFIG_SYS_DISPLAY_NATIVE_Y_RES 480
#define CONFIG_SYS_DISPLAY_RST_DELAY    5   /* ms */
#define CONFIG_SYS_DISPLAY_GPIO_RESET   65
/* 2.5V */
#define CONFIG_SYS_DISPLAY_VOLTAGE  2500000
#define CFG_MCDE_BASE               0xA0350000
#define CFG_DSI_BASE                0xA0351000

/* Monitor Command Prompt   */
#if defined(ENABLE_FEATURE_BUILD_HBTS)
#define CONFIG_SYS_PROMPT   "HBTS> "
#else
#define CONFIG_SYS_PROMPT   "U8500 $ "
#endif

/* android boot serial number */
#define PUBLIC_ID_BACKUPRAM1            (U8500_PERIPH4_BASE + 0x01000 + 0x0FC0)
#define MAX_ANDROID_SERIAL_NUMBER_LEN   32

/* SoC settings AVS */
#define SOC_AVS_BACKUPRAM               (U8500_PERIPH4_BASE + 0x1C30)
#define SOC_AVS_BACKUPRAM_LEN           32

#ifndef ASSEMBLY
void db8500_l2_clean_invalidate_lock(void);
//void db8500_l2_enable(void);
//void db8500_l2_sync(void);
//void db8500_clocks_init(void);
#endif /* !ASSEMBLY */

#endif /*__8500_REGS_H*/

