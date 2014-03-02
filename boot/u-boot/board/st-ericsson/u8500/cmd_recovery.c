/*
 * Copyright (C) ST-Ericsson SA 2012
 * Author:Pengzhou ZHAN pengzhou.zhan@stericsson.com for
 * ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include "reboot_reason.h"
#include "u8500_keypad.h"
#include "android_bcb.h"

#ifdef CONFIG_VIA_KEYPAD
/*Volume up for Recovery mode*/
#define MAGIC_VOLUME_UP 	(MATRIX_SCAN_CODE(1, 3, SKE_KEYPAD_ROW_SHIFT))
/*Volume down for fastboot*/
#define MAGIC_VOLUME_DOWN 	(MATRIX_SCAN_CODE(2, 3, SKE_KEYPAD_ROW_SHIFT))
#else
/*Volume up for Recovery mode*/
#define MAGIC_VOLUME_UP 	(MATRIX_SCAN_CODE(5, 3, SKE_KEYPAD_ROW_SHIFT))
/*Volume down for fastboot*/
#define MAGIC_VOLUME_DOWN 	(MATRIX_SCAN_CODE(5, 2, SKE_KEYPAD_ROW_SHIFT))
#endif

#define EMMC_DEV 		"mmc"

static int do_recovery(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	block_dev_desc_t * dev_desc;
	int ret;
#ifndef CONFIG_VIA_KEYPAD
	int keycode[] =
	    {
		MAGIC_VOLUME_UP,
	    };
#endif
	ret = get_reboot_reason();
	if (ret == SW_RESET_RECOVERY){
		printf("RECOERY:reboot recovery detected!\n");
		setenv("bootcmd", CONFIG_RECOVERY_BOOTCOMMAND);
		return 0;
	} else {
		/*always emmc 0 here*/
		dev_desc = get_dev(EMMC_DEV, 0);
		if(dev_desc == NULL) {
			printf("RECOVERY:ERROR -- Invalid Device!\n");
			return 1;
		}

#ifndef CONFIG_BOOT_MENU
		/*check if there is magic key pressed*/
		udelay(20000);
#ifdef CONFIG_VIA_KEYPAD
        ret = detect_lunch_key();
#else
		lunch_key_detect();
		ret = ske_keypad_getkey_pressed(ARRAY_SIZE(keycode), keycode);
#endif

		if (!ret) {
			printf("RECOVERY:magic key detected!\n");
			setenv("bootcmd", CONFIG_RECOVERY_BOOTCOMMAND);
			ret = android_bcb_boot(dev_desc, BCB_RECOVERY_MISC_ERASE);
			reset_key_pressed();
			return 0;

		} else
#endif
		{
			/*check if recovery mode was interrupted*/
			ret = android_bcb_boot(dev_desc, BCB_RECOVERY_INTERRUPT);
			if(ret == BCB_INT){
				setenv("bootcmd", CONFIG_RECOVERY_BOOTCOMMAND);
				printf("continue recovery mode...\n");
			}
		}
	}

	return 0;
}

U_BOOT_CMD(
	recovery,       1,      0,      do_recovery,
	"Android recovery mode detection",
	"Three reasons can be detected:\n"
	"-Magic key pressed by user\n"
	"-Recovery reboot reason set by Android system\n"
	"-Recovery mode was interrupted"
);

