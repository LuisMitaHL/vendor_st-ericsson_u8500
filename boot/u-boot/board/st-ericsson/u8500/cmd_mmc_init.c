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
#include <mmc.h>

int IsMmcInitialized=0;

int MmcInitialization(void)
{
  IsMmcInitialized=1;
  mmc_initialize(NULL);
  return 0;
}

static int do_MmcInitialization(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
  return (MmcInitialization());
}

U_BOOT_CMD(
	mmcinit,	1,	1,	do_MmcInitialization,
	"MMC initialization",
	"MMC initialization"
);

