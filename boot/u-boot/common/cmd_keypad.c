/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Seshagiri HOLI<seshagiri.holi@stericsson.com> for
 * ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2.
 */

/* Support for key combination detection. */
#include <common.h>
#include <linux/bitops.h>
#include <u5500_keypad.h>

int do_keypress_wait(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *ep;
	u32 delay = 0;
	int ret = 0;
	switch (argc) {
	case 1: /* No Arguments  passed */
		ret = wait_for_keypress_forever();
		break;
	case 2: /*One arguments passed */
		delay = simple_strtoul(argv[1], &ep, 10);
		ret = wait_for_keypress(delay);
		break;
	default:
		cmd_usage(cmdtp);
		break;
	}
	return ret;
}

U_BOOT_CMD(
	detect_keypress,       2,      0,      do_keypress_wait,
	"waits for keypress",
	"<delay in msec>\n"
	"     Waits for keypress\n"
	"     If no arguments passed it will wait forever.\n"
);
