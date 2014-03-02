/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * Author: Szymon Bigos <szymon.bigos@tieto.com> for ST-Ericsson
 *         Tomasz Hliwiak <tomasz.hliwiak@tieto.com> for ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ste_pcsc_utils.h"

unsigned char strToHex(char a)
{
    if ((a >= '0') && (a <= '9'))
    {
        return a - '0';
    }
    else if ((a >= 'A') && (a <= 'F'))
    {
        return a - 'A' + 10;
    }
    else if ((a >= 'a') && (a <= 'f'))
    {
        return a - 'a' + 10;
    }

    return NOT_HEX_VALUE;
}

