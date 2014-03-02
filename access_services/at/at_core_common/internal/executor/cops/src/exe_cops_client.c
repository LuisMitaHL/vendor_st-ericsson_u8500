/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include "atc_log.h"
#include "exe_cops_client.h"
#include "exe_internal.h"
#include "exe_start.h"
#include "cops.h"

struct copsclient_s {
    cops_context_id_t *cops_id_p;
};

extern void __load_dynamic_cops_symbols();

/********************************************************************
 * Public methods
 ********************************************************************
 */

void *copsclient_open_session()
{
    copsclient_t *copsclient_p = (copsclient_t *) malloc(sizeof(*copsclient_p));
    cops_return_code_t result;

    ATC_LOG_D("%s: called", __FUNCTION__);

    if (NULL == copsclient_p) {
        goto error;
    }

    /* Load dynamic cops symbols */
    __load_dynamic_cops_symbols();

    copsclient_p->cops_id_p = NULL;
    result = cops_context_create(&copsclient_p->cops_id_p, NULL, NULL);

    if (COPS_RC_OK != result) {
        goto error;
    }

    /* Set service status */
    exe_start_set_status(EXE_START_COPS, EXE_START_CONNECTED_READY);

    ATC_LOG_I("copsclient_open_session SUCCEDED");

    return copsclient_p;

error:
    free(copsclient_p);
    return NULL;
}

void copsclient_close_session(void *service_p)
{
    copsclient_t *copsclient_p = (copsclient_t *)service_p;
    ATC_LOG_D("%s: called", __FUNCTION__);

    if (NULL != copsclient_p) {
        cops_context_destroy(&copsclient_p->cops_id_p);
        free(copsclient_p);

        /* Set service status */
        exe_start_set_status(EXE_START_COPS, EXE_START_NOT_CONNECTED);
    }
}

void *copsclient_get_id(copsclient_t *copsclient_p)
{
    if (NULL != copsclient_p) {
        return copsclient_p->cops_id_p;
    } else {
        return NULL;
    }
}
