/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Based on reference-ril by The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <stdio.h>
#include <telephony/ril.h>
#include "u300-ril.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

/**
 * RIL_REQUEST_OEM_HOOK_RAW
 *
 * This request reserved for OEM-specific uses. It passes raw byte arrays
 * back and forth.
 * Note to OEM: to send RIL_UNSOL_OEM_HOOK_RAW responses, use
 * sendUnsolOemHookRaw(uint8_t *data, size_t datalen)
 */
void requestOEMHookRaw(void *data, size_t datalen, RIL_Token t)
{
    /* Echo back data */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, data, datalen);
    return;
}

/**
 * RIL_REQUEST_OEM_HOOK_STRINGS
 *
 * This request reserved for OEM-specific uses. It passes strings
 * back and forth.
 */
void requestOEMHookStrings(void *data, size_t datalen, RIL_Token t)
{
    int i;
    const char **cur;

    ALOGD("got OEM_HOOK_STRINGS: 0x%8p %lu", data, (long) datalen);

    for (i = (datalen / sizeof(char *)), cur = (const char **) data;
         i > 0; cur++, i--)
        ALOGD("> '%s'", *cur);

    /* Echo back strings. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, data, datalen);
    return;
}
