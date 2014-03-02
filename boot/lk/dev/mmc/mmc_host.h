/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef __U8500_MMC_HOST_H__
#define __U8500_MMC_HOST_H__

#include <mmc_if.h>

struct sdi_registers;

int u8500_mmc_host_init(mmc_properties_t *dev, struct sdi_registers *base);
int u8500_emmc_host_init(mmc_properties_t *dev, struct sdi_registers *base);
mmc_properties_t *u8500_alloc_mmc_struct(void);
int host_request(mmc_properties_t *dev, mmc_cmd_t *cmd, mmc_data_t *data);
void host_set_ios(mmc_properties_t *dev);
int mmc_host_reset(mmc_properties_t *dev);
int sd_host_reset(mmc_properties_t *dev);
void u8500_mmc_host_printregs(mmc_properties_t *dev);

#endif
