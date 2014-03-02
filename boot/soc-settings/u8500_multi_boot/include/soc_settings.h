/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __SOC_SETTINGS_H__
#define __SOC_SETTINGS_H__

#define BOOT_OK				0
#define BOOT_INTERNAL_ERROR		0xA0010022

void hwi2c_read(u16 addr, u8 *value);
void hwi2c_write(u16 addr, u8 value);
void hwi2c_masked_write(u16 addr, u8 bitmask, u8 bitval);

#endif /* __SOC_SETTINGS_H__ */

