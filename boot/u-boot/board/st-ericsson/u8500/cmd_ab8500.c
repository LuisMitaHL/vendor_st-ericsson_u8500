/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Author: Rikard Olsson <rikard.p.olsson@stericsson.com> for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 *
 */

/* cmd_ab8500.c - read and write register in the ab8500 */

#include <common.h>
#include <command.h>
#include <asm/arch/ab8500.h>

static int do_ab8500_read(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong bankaddress;
	ulong offset;
	int ret;

	if (argc != 3) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Get bank and offset */
	bankaddress = simple_strtoul(argv[1], NULL, 16);
	offset = simple_strtoul(argv[2], NULL, 16);

	ret = ab8500_read(bankaddress, offset);
	if (ret < 0) {
		printf("ab8500 read failed at address 0x%lx,0x%lx\n",
		       bankaddress, offset);
		return 1;
	}

	/* Print the result */
	printf("ab8500_read(0x%lx,0x%lx) = 0x%x\n", bankaddress, offset, ret);

	return 0;
}

static int do_ab8500_write(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong bankaddress;
	ulong offset;
	ulong value;
	int ret;

	if (argc != 4) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Get bank,offset and value */
	bankaddress = simple_strtoul(argv[1], NULL, 16);
	offset = simple_strtoul(argv[2], NULL, 16);
	value = simple_strtoul(argv[3], NULL, 16);

	/* Write the reg value */
	ret = ab8500_write(bankaddress, offset, value);
	if (ret < 0) {
		printf("ab8500 write failed at address 0x%lx,0x%lx\n",
		bankaddress, offset);
		return 1;
	}

	/* Print the result */
	printf("ab8500_write(0x%lx,0x%lx,0x%lx)\n", bankaddress, offset, value);

	return 0;
}

U_BOOT_CMD(
	ab8500_read,	3,	1,	do_ab8500_read,
	"read ab8500 register",
	"ab8500_read bankadress offset"
);

U_BOOT_CMD(
	ab8500_write,	4,	1,	do_ab8500_write,
	"write XX value into ab8500 register",
	"ab8500_write bankadress offset value"
);
