/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _XL_DRIVERS_H
#define _XL_DRIVERS_H

t_uint8 read_hw_i2c(t_uint16 base_addr);
t_uint32 XL_GetLastReset(t_boot_last_reset *p_lastReset);
t_uint8 XL_GetAB8500CutId(void);
void write_hw_i2c(t_uint16 base_addr, t_uint8 value);
void XL_WriteI2C(t_uint8 block, t_uint8 address, t_uint8 data);
t_uint8 XL_ReadI2C(t_uint8 block, t_uint8 address);
void XL_SendToken(t_pub_rom_loader_info   *p_loaderInfo,
		  t_uint32 token);
void XL_DisableCachesMmu(void);
void XL_CleanInvalidateL2(void);

t_boot_error MMC_switch_to_boot_partition(struct data_collection *data,
		struct toc_data *toc_data, t_uint32 boot_part);
t_MmcType *get_MmcType(void);
t_uint8 emmc_voltage_check(void);
t_boot_error init_emmc(struct data_collection *data);
#endif
