/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

enum avs_type {
	AVS_TYPE_NONE = 0,
	AVS_TYPE_8500,
	AVS_TYPE_8500_BOOST,
	AVS_TYPE_8420,
	AVS_TYPE_8520,
};

 /*
 * Memory test settings
 **********************
 */

/* DDR memory test/validation method to use in the end of soc-settings */
/* Old version found in meminit. Executes in matter of milli seconds. */
enum memtest_method {
	MEMTEST_METHOD_OLD = 1,

/* Fast, checking data and address pins. Executes in matter of milli seconds. */
	MEMTEST_METHOD_FAST,

/* Full version, includes fast version, but will also check each and every
 *memory location. Will also try to expose any power issues by fliping bits in
 * the memory.
 * Can only be used for debugging as it takes too long
 * time to execute.
 */
	MEMTEST_METHOD_FULL,
};

/*
 * Overclocking
 *************
 */
/* Overclock the main ARM CPU. Values are in Mhz */
enum overclock {
	OVERCLOCK_1150MHZ = 1150,
	OVERCLOCK_1000MHZ = 1000,
	OVERCLOCK_800MHZ = 800,
	DOWNCLOCK_600MHZ = 600
};

/*
 * Logging configuration
 **********************
 */
/* log levels. Must also enable the console UART */
enum log_level {
	LOG_ERROR		= 0,
	LOG_MEMERR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG_LVL1,		/* Debug prints enabled */
	LOG_DEBUG_LVL2,		/* More debug prints */
	LOG_DEBUG_LVL3,		/* Extra debug prints, in loops */
};

/*
 * UART loggins options
 ********************
 */
/* String prepended on uart output */
#define	UART_STRING "SoC settings: "

/* Predefined values for UART baud rates */
enum uart_baud {
	UART_BAUD115200 = 0x350014
};

/* Selections of GPIO pin config to enable UART2 */
enum uart_gpiopins {
	UART_GPIOPINS_DISABLE = 0,	/* Disabled */
	UART_GPIOPINS_29_30,		/* AltC pin 29+30 */
	UART_GPIOPINS_18_19,		/* AltB pin 18+19 */
	UART_GPIOPINS_75_76,		/* AltC pin 75+76 NO SUPPORT YET */
	UART_GPIOPINS_153_154,		/* AltC pin 153+154 NO SUPPORT YET */
};

/*
 * Modem STM options
 *******************
 */
/* Selections of GPIO pin config for modem STM */
enum modem_stm_gpiopins {
	MODEM_STM_GPIOPINS_DISABLE = 0,	/* Disabled */
	MODEM_STM_GPIOPINS_70_74,	/* AltC3 on pin 70-74 */
	MODEM_STM_GPIOPINS_155_159,	/* AltC1 on pin 155-159 */
	MODEM_STM_GPIOPINS_163_167,	/* AltC on pin 163-167 */
};

/*
 * Modem UART options
 ********************
 */
/* Selections of GPIO pin config for modem UART */
enum modem_uart_gpiopins {
	MODEM_UART_GPIOPINS_DISABLE = 0, /* Disabled */
	MODEM_UART_GPIOPINS_75_76,	/* AltC3 on pin 75+76 */
	MODEM_UART_GPIOPINS_153_154,	/* AltC1 on pin 153+154 */
	MODEM_UART_GPIOPINS_161_162,	/* AltC on pin 161+162 */
};

/*
 * PRCMU STM options
 *******************
 */
/* Selections of GPIO pin config for prcmu STM */
enum prcmu_stm_gpiopins {
	PRCMU_STM_GPIOPINS_DISABLE = 0,	 /* Disabled */
	PRCMU_STM_GPIOPINS_70_74,	 /* AltC on pin 70-74 */
	PRCMU_STM_GPIOPINS_155_159,	 /* AltC on pin 155-159 */
	PRCMU_STM_GPIOPINS_163_167,	 /* AltC on pin 163-167 */
};

/*
 * Board version options
 * We use bit 0-7 for board versions
 * and 8-15 for chip versions.
 * Information is stored in BackUp RAM
 ***********************
 */
/* Power save configuration */
enum board_version {
	BOARD_VERSION_DEFAULT = 0x1000,
	BOARD_VERSION_500_V5X = 0x1010,
	BOARD_VERSION_520     = 0x2000
};

#endif /* __OPTIONS_H__ */

