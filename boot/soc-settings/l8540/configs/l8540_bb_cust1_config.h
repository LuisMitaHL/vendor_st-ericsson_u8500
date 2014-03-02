/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Mattias Wallin <mattias.wallin at stericsson.com> for ST-Ericsson
 *         Bengt Jonsson <bengt.g.jonsson at stericsson.com> for ST-Ericsson
 *
 * This file is based on u8500/configs/hrefp_v20_1000mhz_config.h written by
 * Joakim Axelsson <joakim.axelsson at stericsson.com>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <options.h> /* We configure and select some of these options */
#include <peripheral.h> /* Needed addresses to use for config */

/*
 * Memory test settings
 **********************
 */
/* DDR memory test/validation method to use in the end of soc-settings */
#define CONFIG_MEMTEST_METHOD		OPTION_MEMTEST_METHOD_FAST
/* Disable the cache before memtest? */
#define CONFIG_MEMTEST_DISABLECACHE	OPTION_NO

/*
 * Debugging configuration
 **********************
 */
/* Enable debug log level. Must also enable the console UART */
#ifndef CONFIG_DEBUG_LOG
#define CONFIG_DEBUG_LOG	OPTION_DEBUG_LOG_NONE
#endif

/*
 * Configure console UART
 **********************
 */
/* Enable UART logging */
#define CONFIG_UART_ENABLED	OPTION_YES
/* String to prepend all log line */
#define CONFIG_UART_STRING	OPTION_UART_STRING
/* Base address of UART */
#define CONFIG_UART_BASE	UART2_BASE
/* Which GPIO pins are the UART on */
#define CONFIG_UART_GPIOPINS	OPTION_UART_GPIOPINS_120_121
/* What baud rate to use for UART */
#define CONFIG_UART_BAUD	OPTION_UART_BAUD115200

/*
 * Configure modem STM
 *********************
 */
/* Enable Modem STM trace */
#define CONFIG_MODEMSTM_ENABLED	OPTION_NO
/* What GPIO pins are the modem STM on */
#define CONFIG_MODEMSTM_GPIOPINS	OPTION_MODEMSTM_GPIOPINS_70_74

/*
 * Configure modem UART
 **********************
 */
/* Enable Modem UART */
#define CONFIG_MODEMUART_ENABLED	OPTION_NO
/* Which GPIO pins to use for Modem UART */
#define CONFIG_MODEMUART_GPIOPINS	OPTION_MODEMUART_GPIOPINS_75_76

/*
 * Configure PRCMU STM
 *********************
 */
/* Enable PRCMU STM trace */
#define CONFIG_PRCMUSTM_ENABLED	OPTION_NO
/* What GPIO pins are the PRCMU/APE STM on */
#define CONFIG_PRCMUSTM_GPIOPINS	OPTION_PRCMUSTM_GPIOPINS_70_74

/*
 * Configure Boot Info
 *********************
 */
/* Board version: V5X */
#define CONFIG_BOARD_VERSION            OPTION_BOARD_VERSION_V5X

/*
 * Configure Memory Die Info
 ***************************
 */
#define CONFIG_TWO_DIE		OPTION_TWO_DIE

#define CONFIG_DDR_PREINIT OPTION_NO

#endif /* __CONFIG_H__ */
