/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Paer-Olof Haakansson <par-olof.hakansson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __AWAIT_BATTERY_VOLTAGE_H__
#define __AWAIT_BATTERY_VOLTAGE_H__

#define BOOT_OK				0
#define BOOT_INTERNAL_ERROR		0xA0010022

u8 enable_usb_charging(void);
void await_required_battery_voltage(void);
void vbat_ok_to_boot_or_reset(void);

#endif /* __AWAIT_BATTERY_VOLTAGE_H__ */

