/*
 * Copyright (C) ST-Ericsson SA 2012
 * Author: Rickard Evertsson <rickard.evertsson@stericsson.com>
 * for ST-Ericsson.
 * 	   Pengzhou ZHAN <pengzhou.zhan@stricsson.com>
 * for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 *
 * Use this file to customize your reboot / sw reset reasons. Add, remove or
 * modify reasons in reboot_reasons[].
 */

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/types.h>
#include <linux/bitops.h>
#include "reboot_reason.h"

/*
 * The structure is copied from Linux kernel defined to
 * indicate different reboot reasons.this should sync with
 * kernel definition.
 */
struct reboot_reason reboot_reasons[] = {
	{"crash", SW_RESET_CRASH},
	{"factory-reset", SW_RESET_FACTORY_RESET},
	{"recovery", SW_RESET_RECOVERY},
	{"charging", SW_RESET_CHARGING},
	{"coldstart", SW_RESET_COLDSTART},
	{"none", SW_RESET_NO_ARGUMENT}, /* Normal Boot */
	{"chgonly-exit", SW_RESET_CHGONLY_EXIT}, /* Exit Charge Only Mode */
};

unsigned int reboot_reasons_size = ARRAY_SIZE(reboot_reasons);

int get_reboot_reason(void)
{

	return readw(U8500_PRCMU_TCDM_BASE + PRCM_SW_RST_REASON);

}

static const char *reboot_reason_string(int code)
{
	unsigned int i;

	/* Search through reboot reason list */
	for (i = 0; i < reboot_reasons_size; i++) {
		if (reboot_reasons[i].code == code)
			return reboot_reasons[i].reason;
	}

	/* No valid reboot reason code found */
	return "unknown";
}

static int do_get_reboot_reason(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int code;

	code = get_reboot_reason();

	printf("reboot code is 0x%04x,reason is %s\n", code, reboot_reason_string(code));

	return 0;
}

U_BOOT_CMD(
	rbtreason,       1,      0,      do_get_reboot_reason,
	"Get reboot reason from PRCMU TCDM area",
	"\nreboot reason:\n"
	"\t--'crash':0xDEAD\n"
	"\t--'none:0xBEEF\n"
	"\t--'charging':0xCAFE\n"
	"\t--'recovery':0x5502\n"
	"\t--'coldstart':0x0000\n"
	"\t--'chgonly-exit':0xCAFE\n"
	"\t--'factory-reset':0x4242"
);
