/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* The white box test program doesn't want real clock_gettime() data since that increases
 * the complexity of the testcases. As long as we don't need sharp implementations of
 * anything in librt (which isn't added in the build) it is possible to override these functions.
 */
int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
   /* supply the expected testcase value */
    if (tp) {
        memset(tp, 0, sizeof(*tp));
        if (clock_id == CLOCK_MONOTONIC) {
            tp->tv_sec = 2005366; /* 1970/01/24,05:02:46 */
        } else
        {
            tp->tv_sec = 909515366; /* 1970/01/24,05:02:46 */
        }
    }

    return 0;
}
