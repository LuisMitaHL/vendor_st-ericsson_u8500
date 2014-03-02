/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include "omxilosaltimerimpl.h"

OMX_U64 OmxILOsalTimer::GetSystemTime(void)
{
    return ((OMX_U64)LOS_getSystemTime());
}

