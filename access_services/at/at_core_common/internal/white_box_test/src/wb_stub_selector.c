/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdbool.h>

#include "atc_selector.h"
#include "whitebox.h"
#include "wb_test.h"


bool selector_register_callback_for_fd(int fd, callback_t callback, void *data_p)
{
    switch (fd) {
#ifdef EXE_USE_CN_SERVICE
    case FD_CN_REQUEST:
        printf("registering EXECUTOR.cn_selector_callback_request\n");
        EXECUTOR.cn_selector_callback_request = callback;
        break;
    case FD_CN_EVENT:
        printf("registering EXECUTOR.cn_selector_callback_event\n");
        EXECUTOR.cn_selector_callback_event = callback;
        break;
#endif
    case FD_SIM_REQUEST:
        EXECUTOR.sim_selector_callback_request = callback;
        EXECUTOR.simclient_p = data_p;
        break;
    case FD_SIM_EVENT:
        EXECUTOR.sim_selector_callback_event = callback;
        EXECUTOR.simclient_p = data_p;
        break;
#ifdef EXE_USE_SIMPB_SERVICE
    case FD_SIMPB_REQUEST:
        EXECUTOR.simpb_selector_callback = callback;
        EXECUTOR.simpbclient_p = data_p;
        break;
#endif
    case FD_SMS_REQUEST:
        EXECUTOR.sms_selector_callback_request = callback;
        EXECUTOR.smsclient_p = data_p;
        break;
    case FD_SMS_EVENT:
        EXECUTOR.sms_selector_callback_event = callback;
        EXECUTOR.smsclient_p = data_p;
        break;
    case FD_PSCC_REQUEST:
        EXECUTOR.pscc_selector_callback_request = callback;
        EXECUTOR.psccclient_p = data_p;
        break;
    case FD_PSCC_EVENT:
        EXECUTOR.pscc_selector_callback_event = callback;
        EXECUTOR.psccclient_p = data_p;
        break;
    case FD_STERC_REQUEST:
        EXECUTOR.sterc_selector_callback_request = callback;
        EXECUTOR.psccclient_p = data_p;
        break;
    case FD_STERC_EVENT:
        EXECUTOR.sterc_selector_callback_event = callback;
        EXECUTOR.psccclient_p = data_p;
        break;
    }

    return true;
}

bool selector_deregister_callback_for_fd(int fd)
{
    return true;
}

bool atc_stop(void){
    return true;
}
