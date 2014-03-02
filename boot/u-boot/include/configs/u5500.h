/*
 * Copyright (C) ST-Ericsson SA 2010
 * Author: <Name and email address of author> for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/hardware.h>
/*
#define DEBUG 1
#define DBG_LVL_VERBOSE 1
*/
/*-----------------------------------------------------------------------
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_U5500		1
#define CONFIG_L2_OFF		1

#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END	0x1FFFFFFF
#define CONFIG_SYS_HZ		1000		/* must be 1000 */

#define CONFIG_BOOTTIME
#define CONFIG_SYS_TIMERBASE	0x80126000      /* MTU0 timer */

#define BOARD_LATE_INIT		1

/*-----------------------------------------------------------------------
 * Size of environment and malloc() pool
 */
/*
 * If you use U-Boot as crash kernel, make sure that it does not overwrite
 * information saved by kexec during panic. Kexec expects the start
 * address of the executable 32K above "crashkernel" address.
 */
#define CONFIG_ENV_SIZE		(4*1024)
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + 16*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* for initial data */

#define CONFIG_ENV_IS_IN_EMMC
#define CONFIG_ENV_TOC_NAME		"UBOOT_ENV"
#define CONFIG_CMD_SAVEENV
/*-----------------------------------------------------------------------
 * PL011 Configuration
 */

#define CONFIG_PL011_SERIAL
/*
 * U5500 UART registers base for 3 serial devices
 */
#define CFG_UART0_BASE		0xA0023000
#define CFG_UART1_BASE		0x80104000
#define CFG_UART2_BASE		0x80105000
#define CFG_SERIAL0		CFG_UART0_BASE
#define CFG_SERIAL1		CFG_UART1_BASE
#define CFG_SERIAL2		CFG_UART2_BASE
#define CONFIG_PL011_CLOCK	36360000
#define CONFIG_PL01x_PORTS	{ (void *)CFG_SERIAL0, (void *)CFG_SERIAL1, \
				  (void *)CFG_SERIAL2 }
#define CONFIG_CONS_INDEX	0
#define CONFIG_BAUDRATE		115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Devices and file systems
 */
#define CONFIG_MMC		1
#define CONFIG_GENERIC_MMC	1
#define CONFIG_DOS_PARTITION	1
#define CONFIG_TOC_PARTITION	1
#define CONFIG_ROCKBOX_FAT	1
#define CONFIG_U_BOOT		1 /* needed by Rockbox code */
#define CONFIG_SUPPORT_VFAT	1 /* Rockbox */

/*
 * Commands
 */
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_BDI
#define CONFIG_CMD_IMI
#define CONFIG_CMD_MISC
#define CONFIG_CMD_RUN
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_MMC
#define CONFIG_CMD_MMC_UTILS

#define CONFIG_CMD_FAT
#define CONFIG_CMD_TREE_FAT	/* Rockbox */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EMMC
#define CONFIG_CMD_SOURCE
#define CONFIG_CMD_TOC
#define CONFIG_CMD_DATE
#define CONFIG_CMD_KEYPAD_DETECT
#define CONFIG_CMD_I2C

#define CONFIG_RTC_PL031_ST
#define CONFIG_SYS_RTC_PL031_BASE U5500_RTC_BASE

#ifdef CONFIG_USB_TTY
#define CONFIG_BOOTDELAY	-1	/* disable autoboot */
#endif /* CONFIG_USB_TTY */

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	1
#endif
#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */

/* preboot is set dynamically to "checkcrash" if U-Boot was executed by kexec */
#define CONFIG_PREBOOT

#undef CONFIG_BOOTARGS
#define CONFIG_BOOTCOMMAND	"run emmcboot"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"verify=n\0"							\
	"loadaddr=0x00100000\0"						\
	"console=ttyAMA0,115200n8\0"					\
	"memargs256=mem=24M@0 mem=208M@48M "				\
		"memmap=0x01800000$0x01800000 "				\
		"mloader_helper.shm_total_size=0x00030000\0"		\
	"memargs512=mem=96M@0 mem_modem=32M@96M hwmem=32M@128M "        \
		"mem=64M@160M mali.mali_mem=32M@224M "                       \
		"pmem_hwb=128M@256M mem=128M@384M "                     \
		"memmap=0x01800000$0x01800000 "				\
		"mloader_helper.shm_total_size=0x00030000\0"		\
	"commonargs=setenv bootargs noinitrd "				\
		"init=init "						\
		"crashkernel=${crashkernel}\0"				\
		"logo.${logo} "						\
		"startup_graphics=${startup_graphics}\0"		\
	"emmcargs=setenv bootargs ${bootargs} "				\
		"root=/dev/mmcblk0p2 "					\
		"rootwait\0"						\
	"addcons=setenv bootargs ${bootargs} "				\
		"console=${console}\0"					\
	"emmcboot=echo Booting from eMMC ...; "				\
		"run commonargs emmcargs addcons memargs;"		\
		"mmc rescan 0;"						\
		"write_partition_block;"				\
		"mmc read 0 ${loadaddr} 0xA0000 0x4000;"		\
		"bootm ${loadaddr}\0"					\
	"cmdfile=mmc rescan 1;mmc_read_cmd_file;run bootcmd\0"		\
	"flash=mmc rescan 1;fat load mmc 1 ${loadaddr} /flash.scr;"	\
		"source ${loadaddr}\0"					\
	"loaduimage=mmc rescan 1;fat load mmc 1 ${loadaddr} /uImage\0"	\
	"usbtty=cdc_acm\0"						\
	"stdout=serial,usbtty\0"					\
	"stdin=serial,usbtty\0"						\
	"stderr=serial,usbtty\0"

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_LONGHELP			/* undef to save memory     */
#define CONFIG_SYS_PROMPT	"U5500 $ "	/* Monitor Command Prompt   */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size  */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	32	/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE /* Boot Arg Buffer Size */

#undef	CONFIG_SYS_CLKS_IN_HZ		/* everything, incl board info, in Hz */
#define CONFIG_SYS_LOAD_ADDR		0x00100000 /* default load address */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1

#define CONFIG_SYS_HUSH_PARSER		1
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING

#define CONFIG_SETUP_MEMORY_TAGS	2
#define CONFIG_INITRD_TAG		1
#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs  */

#define CONFIG_ITP_LOAD			1
#define CONFIG_ITP_TOC_ITP_NAME		"ITP"
#define CONFIG_MLOADER_TOC_MODEM_NAME	"ITP_MODEM"
#define CONFIG_MODEM_MEM_START		0x01800000
#define CONFIG_MODEM_MEM_TOTAL_SIZE	0x01800000
#define CONFIG_MODEM_MEM_SHARED_SIZE	0x00030000

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */

#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ		(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ		(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * I2C
 */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C_SPEED            100000
#define CONFIG_SYS_I2C_SLAVE            0       /* slave addr of controller */
#define CONFIG_SYS_I2C0_BASE            U5500_PRCMU_BASE /* I2C0 is used for PRCMU */
#define CONFIG_SYS_I2C1_BASE            U5500_I2C1_BASE
#define CONFIG_SYS_I2C2_BASE            U5500_I2C2_BASE
#define CONFIG_SYS_I2C3_BASE            U5500_I2C3_BASE
#define CONFIG_SYS_I2C_BUS_MAX          4

#define CONFIG_SYS_I2C_GPIOE_ADDR       0x42    /* GPIO expander chip addr */
#define CONFIG_TC35892_GPIO


/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x00000000	/* DDR-SDRAM Bank #1 */
#define PHYS_SDRAM_SIZE_1	0x20000000	/* 512 MB */

/*
 * U5500 has no NOR FLASH
 */
#define CONFIG_SYS_NO_FLASH

/*-----------------------------------------------------------------------
 * MMC related configs
 */
#define CONFIG_MMC_DEV_NUM	1

/*-----------------------------------------------------------------------
 * EMMC related configs
 */

#define CONFIG_EMMC_DEV_NUM	0

#define CONFIG_CMD_ENV
#define CONFIG_CMD_SAVEENV
#define CONFIG_ENV_OFFSET_START	0x13F80000
#define CONFIG_ENV_OFFSET_END	0x13FE0000
#define CONFIG_U8500_MMC	1

/*-----------------------------------------------------------------------
 * KEYPAD related configs
 */
#define CONFIG_U5500_KEYPAD	1
/*------------------------------------------------------------------------------
 * base register values for U5500
 */
#define CFG_PRCMU_BASE	0x80157000 /* Power, reset and clock Management Unit */

/*
 * U5500 GPIO bank register base
 */
#define CONFIG_DB8500_GPIO
#define CFG_GPIO_0_BASE		U5500_GPIOBANK0_BASE
#define CFG_GPIO_1_BASE		U5500_GPIOBANK1_BASE
#define CFG_GPIO_2_BASE		U5500_GPIOBANK2_BASE
#define CFG_GPIO_3_BASE		U5500_GPIOBANK3_BASE
#define CFG_GPIO_4_BASE		U5500_GPIOBANK4_BASE
#define CFG_GPIO_5_BASE		U5500_GPIOBANK5_BASE
#define CFG_GPIO_6_BASE		U5500_GPIOBANK6_BASE
#define CFG_GPIO_7_BASE		U5500_GPIOBANK7_BASE

/* U5500 boot logo changes */

#define CONFIG_VIDEO_LOGO		 1 /* Enable startup logo */
#define CONFIG_SYS_DISPLAY_DSI		1
#define CONFIG_SYS_DISPLAY_DPI		0
#define CONFIG_SYS_VIDEO_FB_ADRS	0x12f00000
#if CONFIG_SYS_DISPLAY_DPI
#define CONFIG_SYS_DISPLAY_NATIVE_X_RES	480	/* VGA */
#else
#define CONFIG_SYS_DISPLAY_NATIVE_X_RES	480	/* HREF */
#define CONFIG_SYS_DISPLAY_RST_DELAY	10	/* ms */
#endif
#define CONFIG_SYS_DISPLAY_NATIVE_Y_RES	854	/* VGA + HREF */
#define MCDE_TOC_SPLASH_NAME		"SPLASH"

/*
 * U5500 Display register base
 */
#define CFG_MCDE_BASE 			0xA0400000
#define CFG_DSI_BASE  			0xA0401000

#endif	/* __CONFIG_H */
