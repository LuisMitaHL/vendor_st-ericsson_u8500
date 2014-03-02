/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include "sim_internal.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_interface_handlers.h"

int uiccd_main_modem_silent_reset(const ste_msg_t * ste_msg)
{
    uiccd_msg_modem_silent_reset_t *msg = (uiccd_msg_modem_silent_reset_t *) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uiccd : modem silent reset received. fd = %d", msg->fd);

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    uiccd_msr_cache_pin();
#endif

    return 0;
}

