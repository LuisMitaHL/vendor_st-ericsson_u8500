/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Olivier Arnaud/o.arnaud@stericsson.com for
 * ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 */

/*
 * cmd_hats.c - custom HATS commands
 *  - droid: command to boot Android upon HATS build
 *  - hats:  command to boot HATS upon HATS build
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#define BOOT_MODE_ANDROID		"Android"
#define BOOT_MODE_HATS			"HATS"
#define BOOT_RDINIT_ANDROID		"init"
#define BOOT_RDINIT_HATS		"init.hats.rc"
#define BOOT_PARAM_SAVE			"save"
#define BOOT_PARAM_NOBOOT		"noboot"
#define BOOT_PARAM_UART			"uart"
#define BOOT_ACTION_SAVE		0x0001
#define BOOT_ACTION_NOBOOT		0x0002
#define BOOT_ACTION_UART		0x0004
#define HWTOOL_PARAM			"hwtoolonuart"
#define CONSOLE_PARAM			"console"
#define CONSOLE_STORE_PARAM		"oldconsole"
#define CONSOLE_DISABLE_VALUE		"/dev/null"

static int launch_boot(void)
{
	int rcode = 0;
	char *cmd = getenv("bootcmd");

	if (cmd == NULL)
		return 1;

#ifndef CONFIG_SYS_HUSH_PARSER
	if (run_command(cmd, 0) < 0)
		rcode = 1;
#else
	if (parse_string_outer(cmd,
			FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
		rcode = 1;
#endif

	return rcode;
}

static int changeboot(const char *mode, int actions)
{
	char *val_hats = BOOT_RDINIT_HATS;
	char *val_droid = BOOT_RDINIT_ANDROID;
	char *val = NULL;
	int res;

	if (!strncmp(mode, BOOT_MODE_ANDROID, strlen(BOOT_MODE_ANDROID)))
		val = val_droid;
	else if (!strncmp(mode, BOOT_MODE_HATS, strlen(BOOT_MODE_HATS)))
		val = val_hats;
	else
		return 1;

	res = setenv("rdinit", val);
	if (res)
		return res;

	printf("- Variable changed to boot %s\n", mode);
	if (actions & BOOT_ACTION_SAVE) {
		res = !!saveenv();
		if (res == 0)
			printf("- Env boot mode changed to %s\n", mode);
		else
			return res;
	}
	if (!(actions & BOOT_ACTION_NOBOOT)) {
		printf("- Will boot %s\n", mode);
		res = launch_boot();
	}
	return res;
}

static int changeuart(int actions)
{
	char *cons;
	char *hwtool;
	int res = 0;
	int len;

	if (actions & BOOT_ACTION_UART) {
		cons = getenv(CONSOLE_PARAM);
		if (!cons) {
			printf("- Failed to get param %s\n", CONSOLE_PARAM);
			return 1;
		}
		if (!strncmp(cons,
					CONSOLE_DISABLE_VALUE,
					strlen(CONSOLE_DISABLE_VALUE))) {
			/*
			 * Console param value already set,
			 * no need to change or store it
			 */
			goto set_hwtoolparam;
		}
		res = setenv(CONSOLE_STORE_PARAM, cons);
		if (res) {
			printf("- Failed to set %s with %s\n",
					CONSOLE_STORE_PARAM, cons);
			return res;
		}
		res = setenv(CONSOLE_PARAM, CONSOLE_DISABLE_VALUE);
		if (res) {
			printf("- Failed to set %s with %s\n",
					CONSOLE_PARAM, CONSOLE_DISABLE_VALUE);
			return res;
		}
set_hwtoolparam:
		cons = getenv(CONSOLE_STORE_PARAM);
		if (cons) {
#define TWO_CHARS_STRING_LEN	3
			len = strlen(cons) + TWO_CHARS_STRING_LEN;
			hwtool = malloc(len);
			if (hwtool) {
				hwtool[0] = '1';
				hwtool[1] = ',';
				hwtool[2] = '\0';
				strncpy(&hwtool[2], cons, strlen(cons));
				hwtool[len - 1] = '\0';
#undef TWO_CHARS_STRING_LEN
			} else {
				return 1;
			}
		} else {
				return 1;
		}
		res = setenv(HWTOOL_PARAM, hwtool);
		free(hwtool);
		if (res) {
			printf("- Failed to set %s\n", HWTOOL_PARAM);
			return res;
		}
		printf("- Variables changed to reserve uart for HATS\n");
	} else {
		hwtool = getenv(HWTOOL_PARAM);
		if (hwtool && (hwtool[0] != '0')) {
			res = setenv(HWTOOL_PARAM, "0");
			if (res) {
				printf("- Failed to set %s with 0\n",
						HWTOOL_PARAM);
				return res;
			}
			cons = getenv(CONSOLE_STORE_PARAM);
			if (!cons) {
				printf("- Failed to get param %s,"
						" nothing to restore\n",
						CONSOLE_STORE_PARAM);
				return 1;
			} else {
				res = setenv(CONSOLE_PARAM, cons);
				if (res)
					printf("- Failed to restore %s"
							" with value %s\n",
							CONSOLE_PARAM, cons);
				else
					printf("- Variables changed to"
						" restore uart settings\n");
			}
		}
	}
	return res;
}

static int do_droid(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	int actions = 0;
	int res;
	for (i = 1; i < argc; i++) {
		if (!strncmp(argv[i],
					BOOT_PARAM_SAVE,
					strlen(BOOT_PARAM_SAVE))) {
			actions |= BOOT_ACTION_SAVE;
		} else if (!strncmp(argv[i],
					BOOT_PARAM_NOBOOT,
					strlen(BOOT_PARAM_NOBOOT))) {
			actions |= BOOT_ACTION_NOBOOT;
		} else {
			printf("- ERROR: arg [%d]=[%s] not supported\n",
					i, argv[i]);
			return 1;
		}
	}
	res = changeuart(actions);
	if (!res)
		return changeboot(BOOT_MODE_ANDROID, actions);
	else
		return res;
}

static int do_hats(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	int actions = 0;
	int res;
	for (i = 1; i < argc; i++) {
		if (!strncmp(argv[i],
					BOOT_PARAM_SAVE,
					strlen(BOOT_PARAM_SAVE))) {
			actions |= BOOT_ACTION_SAVE;
		} else if (!strncmp(argv[i],
					BOOT_PARAM_NOBOOT,
					strlen(BOOT_PARAM_NOBOOT))) {
			actions |= BOOT_ACTION_NOBOOT;
		} else if (!strncmp(argv[i],
					BOOT_PARAM_UART,
					strlen(BOOT_PARAM_UART))) {
			actions |= BOOT_ACTION_UART;
		} else {
			printf("- ERROR: arg [%d]=[%s] not supported\n",
					i, argv[i]);
			return 1;
		}
	}
	res = changeuart(actions);
	if (!res)
		return changeboot(BOOT_MODE_HATS, actions);
	else
		return res;
}

U_BOOT_CMD(
	droid,	3,	0,	do_droid,
	"Change rdinit env var to boot Android instead of HATS",
	"[save] [noboot]\n"
	"- Parameters:\n"
	"  * save: save change(s) for next boot(s)\n"
	"  * noboot: do not automatically boot after making change(s)\n"
	"You can also issue a combination of the 2 above parameters. ex:\n"
	"  - droid save noboot:\n"
	"    * will save rdinit change but not boot\n"
);

U_BOOT_CMD(
	hats,	4,	0,	do_hats,
	"Change env var(s) to boot HATS instead of Android + HATS boot tuning",
	"[save] [noboot] [uart]\n"
	"- Parameters:\n"
	"  * save: save change(s) for next boot(s)\n"
	"  * noboot: do not automatically boot after making change(s)\n"
	"  * uart: change console parameter to reserve uart for HATS\n"
	"You can issue any combination of the above parameters. ex:\n"
	"  - hats save noboot:\n"
	"    * will save rdinit change but not boot\n"
	"  - hats uart save:\n"
	"    * will save rdinit and uart changes, then boot\n"
	"  - hats noboot uart:\n"
	"    * will change rdinit and uart, but neither save nor boot\n"
	"  - hats uart save noboot\n"
	"    * will change rdinit and uart, save but not boot\n"
	"NB:\n"
	"'hats ...' commands not including 'uart' will restore console\n"
	"settings.\n"
);


