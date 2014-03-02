/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * License terms: GNU General Public License (GPL), version 2.
 */


#ifndef __U8500_MMC_HOST_H__
#define __U8500_MMC_HOST_H__

#include <mmc.h>

struct sdi_registers;

int u8500_mmc_host_init(struct mmc *dev, struct sdi_registers *base);
int u8500_emmc_host_init(struct mmc *dev, struct sdi_registers *base);
struct mmc *u8500_alloc_mmc_struct(void);

#endif
