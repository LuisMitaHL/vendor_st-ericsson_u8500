/*
 * Code ported from tee in ST-Ericsson Linux kernel.
 *
 * Original Linux authors:
 * Copyright (C) ST-Ericsson SA 2010
 * Authors: Shujuan Chen <shujuan.chen@stericsson.com>
 *          Martin Hovang <martin.xm.hovang@stericsson.com>
 *
 * Ported to U-boot by:
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef _TEE_H
#define _TEE_H

#include <common.h>
#include <elf.h>

struct mcore_segment_descr {
	void *segment;
	void *hash;
	size_t size;
};

struct access_image_descr {
	struct elfhdr *elf_hdr;
	void *pgm_hdr_tbl;
	void *signature;
	unsigned long nbr_segment;
	struct mcore_segment_descr *descr;
};

int verify_start_modem(struct access_image_descr *access_image_descr);

#endif
