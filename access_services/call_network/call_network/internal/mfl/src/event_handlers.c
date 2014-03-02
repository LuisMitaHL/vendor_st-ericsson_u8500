/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

/* TODO: MFL headers */

#include "cn_client.h"
#include "cn_log.h"

#include "cnsocket.h"
#include "cnserver.h"

#include "cn_log.h"
#include "event_handling.h"
#include "cn_mfl_assertions.h" /* do NOT remove this inclusion! */

/*
 * Local function prototypes
 */

void handle_event_radio_status(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_modem_registration_status(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_time_info(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_name_info(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_ring(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}


void handle_event_call_state_changed(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}


void handle_event_ussd(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}


void handle_event_call_cnap(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_empage_measurement(void *data_p)
{
    (void) data_p;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_empage_error(void *data_p, mal_ftd_error_type error_code)
{
    (void) data_p;
    (void) error_code;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_empage_activation(void *data_p, mal_ftd_error_type error_code)
{
    (void) data_p;
    (void) error_code;
    CN_LOG_E("Not implemented in MFL backend!");
}

void handle_event_empage_deactivation(void *data_p, mal_ftd_error_type error_code)
{
    (void) data_p;
    (void) error_code;
    CN_LOG_E("Not implemented in MFL backend!");
}
