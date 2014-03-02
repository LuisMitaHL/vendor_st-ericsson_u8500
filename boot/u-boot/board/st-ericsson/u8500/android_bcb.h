/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Bootloader control block handling for Android Recovery mode
 * Author: Pengzhou ZHAN <pengzhou.zhan@stericsson.com> for ST-Ericsson
 *
 * License terms: GNU General Public License (GPL), version 2.
 *
 */

#ifndef __ANDROID_BCB_H__
#define __ANDROID_BCB_H__

#define BCB_TOC_MISC_NAME 	"MISC"

#define BCB_ERR 	-1
#define BCB_INT 	0x52
#define BCB_NORMAL 	0

enum bcb_case {
	BCB_RECOVERY_INTERRUPT = 1,
	BCB_RECOVERY_MISC_ERASE,
	BCB_RECOVERY_MISC_DEBUG
};

struct bootloader_message {
	char command[32];
	char status[32];
	char recovery[1024];
};

int android_bcb_boot(block_dev_desc_t *block_dev,int flag);

#endif /*__ANDROID_BCB_H__*/
