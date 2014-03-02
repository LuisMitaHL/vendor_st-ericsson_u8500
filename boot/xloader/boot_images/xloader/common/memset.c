/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stddef.h>
void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    size_t m;

    for (m = 0; m < n; m++)
        p[m] = c;
    return s;
}
