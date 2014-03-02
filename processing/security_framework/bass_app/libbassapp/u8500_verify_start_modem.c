/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_hcl.h>

#define IPL_ITEM_ID 0x02

bass_return_code bass_u8500_verify_start_modem(uint32_t ab8500cutid)
{
#ifdef OS_FREE
    bass_return_code ret = BASS_RC_SUCCESS;

    SECURITY_CallSecureService(BASS_APP_ISSWAPI_SECURE_LOAD,
                   SEC_ROM_FORCE_CLEAN_MASK,
                   IPL_ITEM_ID, ab8500cutid);

    return ret;
#else
    (void)ab8500cutid;
    dprintf(ERROR, "Function only available in OSFree environment!\n");
    return BASS_RC_FAILURE;
#endif
}
