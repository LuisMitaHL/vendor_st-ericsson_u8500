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

/*
 * This file contains options which can be selected in config.h
 */

/*
 * We need to always define the ENABLED configs since
 * some code use if() and not #ifdef on them
 */
#define OPTION_NO	0
#define OPTION_YES	1

 /*
 * Memory test settings
 **********************
 */

/* DDR memory test/validation method to use in the end of soc-settings */
/* Old version found in meminit. Executes in matter of milli seconds. */
#define OPTION_MEMTEST_METHOD_OLD	1

/* Fast, checking data and address pins. Executes in matter of milli seconds. */
#define OPTION_MEMTEST_METHOD_FAST	2

/* Full version, includes fast version, but will also check each and every
 *memory location. Will also try to expose any power issues by fliping bits in
 * the memory.
 * Can only be used for debugging as it takes too long
 * time to execute.
 */
#define OPTION_MEMTEST_METHOD_FULL	3

/*
 * Overclocking
 *************
 */
/* Overclock the main ARM CPU. Values are in Mhz */
#define OPTION_OVERCLOCK_800MHZ	800
#define OPTION_OVERCLOCK_1000MHZ	1000
#define OPTION_DOWNCLOCK_600MHZ	600

/*
 * Debugging configuration
 **********************
 */
/* Debug log levels. Must also enable the console UART */
#define OPTION_DEBUG_LOG_NONE	0 /* No debug logging */
#define OPTION_DEBUG_LOG_LVL1	1 /* Debug prints enabled */
#define OPTION_DEBUG_LOG_LVL2	2 /* More debug prints */
#define OPTION_DEBUG_LOG_LVL3	3 /* Extra debug prints, in loops */

/*
 * UART loggins options
 ********************
 */
/* String prepended on uart output */
#define OPTION_UART_STRING "SoC settings: "

/* Predefined values for UART baud rates
   Baud Rate Divisor = BDR = UARTCLK/(16 x Baud Rate) * 64 = BRDI*64 + BRDF*64
			/16 because OVSFACT = 0b
			64 for have 6bit DIVFRAC (0x0-0x3F)
			BRDI = INT(BDR/64)
			BRDF = INT(BDR-BDRI*64+0.5) 0.5 added for rounding
UARTCLK=38.4 MHz
====================
baudRATE    DIVINT  (BRDI)          DIVFRAC BRDF
110         21818   (0x553A)        12      (0x0C)  0x0C553A
1200        2000    (0x07D0)        0       (0x00)  0x0007D0
2400        1000    (0x03E8)        0       (0x00)  0x0003E8
9600        250     (0x00FA)        0       (0x00)  0x0000FA
38400       62      (0x003E)        32      (0x20)  0x20003E
115200      20      (0x0014)        53      (0x35)  0x350014
460800      5       (0x0005)        13      (0x0D)  0x0D0005
921600      2       (0x0002)        39      (0x27)  0x270002
1843200     1       (0x0001)        19      (0x13)  0x130001
3000000     0       (0x0000)        51      (0x33)  0x330000
*/
#define OPTION_UART_BAUD110        0x0C553A
#define OPTION_UART_BAUD1200       0x0007D0
#define OPTION_UART_BAUD2400       0x0003E8
#define OPTION_UART_BAUD9600       0x0000FA
#define OPTION_UART_BAUD38400      0x20003E
#define OPTION_UART_BAUD115200	0x350014
#define OPTION_UART_BAUD460800     0x0D0005
#define OPTION_UART_BAUD921600     0x270002
#define OPTION_UART_BAUD1843200    0x130001
#define OPTION_UART_BAUD3000000    0x330000

/* Selections of GPIO pin config to enable UART2 */
#define OPTION_UART_GPIOPINS_DISABLE	0 /* Disabled */
#define OPTION_UART_GPIOPINS_29_30	1 /* AltC pin 29+30 */
#define OPTION_UART_GPIOPINS_18_19	2 /* AltB pin 18+19 */
#define OPTION_UART_GPIOPINS_75_76	3 /* AltC pin 75+76 NO SUPPORT YET */
#define OPTION_UART_GPIOPINS_153_154	4 /* AltC pin 153+154 NO SUPPORT YET */
#define OPTION_UART_GPIOPINS_120_121	5 /* AltA pin 120+121 */
/*
 * Modem STM options
 *******************
 */
/* Selections of GPIO pin config for modem STM */
#define OPTION_MODEMSTM_GPIOPINS_DISABLE	0 /*Disabled */
#define OPTION_MODEMSTM_GPIOPINS_70_74	1 /* AltC3 on pin 70-74 */
#define OPTION_MODEMSTM_GPIOPINS_155_159	2 /* AltC1 on pin 155-159 */
#define OPTION_MODEMSTM_GPIOPINS_163_167	3 /* AltC on pin 163-167 */

/*
 * Modem UART options
 ********************
 */
/* Selections of GPIO pin config for modem UART */
#define OPTION_MODEMUART_GPIOPINS_DISABLE	0 /*Disabled */
#define OPTION_MODEMUART_GPIOPINS_75_76	1 /* AltC3 on pin 75+76 */
#define OPTION_MODEMUART_GPIOPINS_153_154	2 /* AltC1 on pin 153+154 */
#define OPTION_MODEMUART_GPIOPINS_161_162	3 /* AltC on pin 161+162 */

/*
 * PRCMU STM options
 *******************
 */
/* Selections of GPIO pin config for prcmu STM */
#define OPTION_PRCMUSTM_GPIOPINS_DISABLE	0 /* Disabled */
#define OPTION_PRCMUSTM_GPIOPINS_70_74		1 /* AltC on pin 70-74 */
#define OPTION_PRCMUSTM_GPIOPINS_155_159	2 /* AltC on pin 155-159 */
#define OPTION_PRCMUSTM_GPIOPINS_163_167	3 /* AltC on pin 163-167 */

/*
 * Board version options
 ***********************
 */
/* Power save configuration */
#define OPTION_BOARD_VERSION_DEFAULT		0
#define OPTION_BOARD_VERSION_V5X		50

/*
 * Memory die options
 *************
 */
/* Selection of die(s) for memory */
#define OPTION_ONE_DIE	0
#define OPTION_TWO_DIE	1

#endif /* __OPTIONS_H__ */

