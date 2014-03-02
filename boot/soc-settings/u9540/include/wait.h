/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __WAIT_H__
#define __WAIT_H__

#include <cpu.h>
#include <io.h>
#include <log.h>

#define MAXWAIT_SHORT	100	/* Max loops for a short wait */
#define MAXWAIT_LONG	20000	/* Max loops for a long wait */

/* These functions are inline static to
 * become inline with source code using it.
 */
static inline int wait_max(u32 addr, u32 mask, u32 status,
	u32 maxwait, const char *what)
{
	unsigned i = 0;
	while (((read32(addr) & mask) != status) && (i < maxwait)) {
		cpu_relax();
		i++; /* Don't wait too long */
	}

	logdebug2("wait_for_status('%s'): End waiting. %u loops.", what, i);

	if (i == maxwait) {
		logdebug("wait_for_status('%s'): "
			"Waiting was aborted. Max of %u loops reached.",
			what, i);
		return 1;
	}

	return 0;
}

/* Short wait, normal one */
static inline int wait(u32 addr, u32 mask, u32 status,
	const char *what)
{
	return wait_max(addr, mask, status, MAXWAIT_SHORT, what);
}

/* Longer wait */
static inline int wait_long(u32 addr, u32 mask, u32 status,
	const char *what)
{
	return wait_max(addr, mask, status, MAXWAIT_LONG, what);
}

/* Delay function, only used during debugging */
static inline void delay(u32 loops, const char *what)
{
	unsigned i;
	for (i = 0; i < loops; i++)
		cpu_relax();

	logdebug2("delay('%s'): End delaying. %u loops.", what, loops);
}

#endif /* __WAIT_H__ */
