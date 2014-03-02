/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#ifndef _POSIX_SOURCE
/* Done to avoid redefinition of struct timeval and struct timespec when including ste_adm_client.h */
#define _POSIX_SOURCE
#endif
#include "ste_adm_client.h"
#include "whitebox.h"

ste_adm_tty_mode_t tty_mode = 0;

int ste_adm_client_set_cscall_upstream_mute(int enable_mute)
{
    return 0;
}

int ste_adm_client_get_cscall_upstream_mute(int *mute_enabled)
{
    *mute_enabled = 1;
    return 0;
}

int ste_adm_client_set_cscall_tty_mode(ste_adm_tty_mode_t tty_enabled)
{
    tty_mode = tty_enabled;
    return 0;
}

int ste_adm_client_get_cscall_tty_mode(ste_adm_tty_mode_t *tty_enabled)
{
    *tty_enabled = tty_mode;
    return 0;
}

int ste_adm_client_set_cscall_downstream_mute(int enable_mute)
{
    return 0;
}
