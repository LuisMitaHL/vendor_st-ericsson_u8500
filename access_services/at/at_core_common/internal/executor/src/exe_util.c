/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>

#include "exe.h"
#include "exe_internal.h"

bool exe_check(bool a, char *file, int line)
{
    if (!a) {
        ATC_LOG_E("Failure in file: %s \t line: %d\n", file, line);
    }

    return a;
}

