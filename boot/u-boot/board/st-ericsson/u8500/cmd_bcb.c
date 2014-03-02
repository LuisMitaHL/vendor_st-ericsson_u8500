/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Bootloader control block handling for Android Recovery mode
 * Author: Pengzhou ZHAN <pengzhou.zhan@stericsson.com> for ST-Ericsson
 *
 * License terms: GNU General Public License (GPL), version 2.
 *
 */
#include <common.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <part.h>
#include <bzlib.h>
#include <environment.h>
#include <asm/byteorder.h>
#include "android_bcb.h"

/*
 * bcb handling interface to other functions.
 * write "boot-recovery" to bootloader->command memeber and
 * write "recovery" to bootloader->recovery member if magic
 * key is detected. write "recover\n--show_text\n" to
 * bootloader->recovery to ignore the menu key then enter the
 * recovery mode UI directly.
 * flag -- used for different situations handling
 * 	1 -- check if the recovery mode was interrupted
 *      2 -- erase MISC partition
 *      3 -- debug purpose,read/write MISC partition
 */

#define EMMC_BLOCK 512

int android_bcb_boot(block_dev_desc_t *block_dev, int flag)
{
	u32 offset, size, loadaddr, blks;
	struct bootloader_message *boot = NULL;

	if (block_dev == NULL) {
		printf("BCB:ERROR -- Invalid device!\n");
		return BCB_ERR;
	}

	/*Check if MISC partition is present*/
	if (get_entry_info_toc(block_dev, BCB_TOC_MISC_NAME, &offset,
				&size, &loadaddr)) {
		printf("BCB:ERROR -- misc toc entry not present\n");
		return BCB_ERR;
	}

	blks = sizeof(struct bootloader_message);

	size = (blks /block_dev->blksz) +
		(blks % block_dev->blksz ? 1 : 0);

	boot = (void *) malloc(size * EMMC_BLOCK);
	if (boot == NULL)
		goto error_exit;

	memset(boot, 0, size * EMMC_BLOCK);

	switch (flag){

	case BCB_RECOVERY_INTERRUPT:
		if (block_dev->block_read(block_dev->dev,
				offset,
				size,
				boot) != size)	{
			printf("BCB:ERROR -- Unable to read data from block device\n");
			goto error_exit;
		}

		if (strcmp(boot->command, "boot-recovery") == 0) {
			printf("BCB:recovery mode was interrupted detected.\n");
			printf("bootloader->command=%s,bootloader->recovery=%s\n",
				boot->command,boot->recovery);

			free(boot);
			return BCB_INT;
		}
		break;

	case BCB_RECOVERY_MISC_ERASE:

		if (block_dev->block_write(block_dev->dev,
				offset,
				size,
				boot) != size)	{
			printf("BCB:ERROR -- Unable to write data to block device\n");
			goto error_exit;
		}
		break;

	case BCB_RECOVERY_MISC_DEBUG:
		if (block_dev->block_read(block_dev->dev,
			offset,
			size,
			boot) != size)	{
			printf("BCB:ERROR -- Unable to read data from block device\n");
			goto error_exit;
		}

		if (strcmp(boot->command, "boot-recovery") == 0) {
			printf("BCB DEBUG: Bootloader Message is set in MISC partition.\n");
			printf("bootloader->command=%s,bootloader->recovery=%s\n",
				boot->command,boot->recovery);

		} else {
			printf("BCB DEBUG: Emulation...write recovery command and then clean it!\n");
			strcpy(boot->command,"boot-recovery");
			strcpy(boot->recovery,"recovery");

			if (block_dev->block_write(block_dev->dev,
					offset,
					size,
					boot) != size)	{
				printf("BCB:ERROR -- Unable to write data to block device\n");
				goto error_exit;
			}

			if (block_dev->block_read(block_dev->dev,
					offset,
					size,
					boot) != size)	{
				printf("BCB:ERROR -- Unable to read data from block device\n");
				goto error_exit;
			}
			printf("bootloader->command=%s,bootloader->recovery=%s\n",
				boot->command,boot->recovery);

			memset(boot, 0, size * EMMC_BLOCK);

			if (block_dev->block_write(block_dev->dev,
					offset,
					size,
					boot) != size)	{
				printf("BCB:ERROR -- Unable to write data to block device\n");
				goto error_exit;
			}

		}
		break;

	default:
	break;
	}
	free(boot);
	return BCB_NORMAL;

error_exit:
	if(boot != NULL)
		free(boot);
	return BCB_ERR;
}

static int do_android_bcb(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	block_dev_desc_t * dev_desc;
	char *ep;

	if (argc < 3) {
		cmd_usage(cmdtp);
		return BCB_ERR;
	}

	dev_desc = get_dev(argv[1], (int)simple_strtoul(argv[2], &ep, 16));

	if(dev_desc == NULL) {
		printf("BCB_CMD:ERROR -- Invalid Device!\n");

		return 1;
	}

	android_bcb_boot(dev_desc,BCB_RECOVERY_MISC_DEBUG);

	return BCB_NORMAL;
}

U_BOOT_CMD(
	bcb, 3, 0, do_android_bcb,
	"Android Bootloader Control Block handling command",
	"<device> <number>, for instance, bcb mmc 0\n"
	"Read Android recovery mode flag stored in MISC partition\n"
	"\t-if recovery flag detected, enter recovery mode\n"
	"\t-if recovery flag NOT detected, print flag message"
);
