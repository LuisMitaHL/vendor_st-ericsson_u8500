/*
 * Copyright (C) ST-Ericsson SA 2012
 * Author: Rickard Evertsson <rickard.evertsson@stericsson.com>
 * for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL) version 2
 *
 * Use this file to customize your reboot / sw reset reasons. Add, remove or
 * modify reasons in reboot_reasons[].
 * The reboot reasons will be saved to a secure location in TCDM memory and
 * can be read at bootup by e.g. the bootloader.
 */

#ifndef _REBOOT_REASON_H
#define _REBOOT_REASON_H

/* U8500 PRCMU BASE */
#define U8500_PER4_BASE 		0x80150000
#define U8500_PRCMU_TCDM_BASE 		(U8500_PER4_BASE + 0x68000) 	/*PRCMU TCDM base*/
#define PRCM_SW_RST_REASON 		0xFF8

/*
 * These defines contains the codes that will be written down to a secure
 * location before resetting. These values should sync with the definitions
 * in the Linux kernel.
 */
#define SW_RESET_NO_ARGUMENT 	0xBEEF
#define SW_RESET_FACTORY_RESET 	0x4242
#define SW_RESET_CRASH 		0xDEAD
#define SW_RESET_NORMAL 	0xc001
#define SW_RESET_CHARGING 	0xCAFE
#define SW_RESET_COLDSTART 	0x0
#define SW_RESET_RECOVERY 	0x5502
#define SW_RESET_CHGONLY_EXIT 	0xCAFF

/*
 * The array reboot_reasons[] is used when you want to map a string to a reboot
 * reason code
 */
struct reboot_reason {
	const char *reason;
	u16 code;
};

int get_reboot_reason(void);

#endif /*_REBOOT_REASON_H*/
