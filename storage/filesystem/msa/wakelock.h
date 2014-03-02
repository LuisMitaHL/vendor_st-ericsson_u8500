/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _WAKELOCK_H
#define _WAKELOCK_H

#include <stdbool.h>

void check_wakelock(void);
int wakelock_acquire(void);
int wakelock_active(void);
int wakelock_release(void);

#endif // _WAKELOCK_H
