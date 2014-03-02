/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fdmon.h"
#include "modem_client.h"

#include "cn_log.h"
#include "cn_client.h"
#include "request_handling.h"
#include "event_handling.h"


struct modem_client_s {
    int fd; /* TODO */
};

/**
 * Starts a session towards the modem
 * returns mflclient on success, NULL on error
 */
modem_client_t *modem_client_open_session(void)
{
    modem_client_t *modem_client_p = NULL;

    modem_client_p = calloc(1, sizeof(*modem_client_p));

    if (!modem_client_p) {
        CN_LOG_E("calloc failed for modem_client_p!");
        goto error;
    }

    return modem_client_p;

error:
    /* Failed to initialize all connections, close down. */
    modem_client_close_session(modem_client_p);
    return NULL;
}

void modem_client_close_session(modem_client_t *modem_client_p)
{
    free(modem_client_p);
}
