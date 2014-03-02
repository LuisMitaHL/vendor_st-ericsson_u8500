/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _COPS_WAKELOCK_H
#define _COPS_WAKELOCK_H

#include <stdbool.h>

/* When first receiving a COPS packet, a wake lock will be acquired if it is
   not already held. This is the time in milliseconds that COPS will wait,
   after sending a response, for the next incoming COPS packet, before
   releasing a held wake lock. */
#define COPS_WAKELOCK_TIMEOUT_MS (1000)

void cops_wakelock_init(void);
void cops_wakelock_final(void);
int cops_wakelock_acquire(void);
int cops_wakelock_active(void);
int cops_wakelock_release(void);

#endif
