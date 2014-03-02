/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL), version 2.
 */
#ifndef _SEC_BRIDGE_H
#define _SEC_BRIDGE_H

#include <common.h>

int sec_bridge_init_bridge(void);
u32 sec_bridge_call_secure_service(const u32 serviceid,
				   const u32 secureconfig, ...);
#endif
