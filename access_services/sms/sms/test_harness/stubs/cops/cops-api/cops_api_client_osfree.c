/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * Implemented interface
 */
#include <stdio.h>
#include <string.h>
#include "cops.h"

cops_return_code_t cops_context_create(
    cops_context_id_t ** ctxpp,
    const cops_event_callbacks_t * event_cbs,
    void *event_aux)
{
    fprintf(stdout, "\n[COPS] cops_context_create\n"); fflush(stdout);
    return COPS_RC_OK;
}

void cops_context_destroy(
    cops_context_id_t ** ctxpp)
{
    fprintf(stdout, "\n[COPS] cops_context_destroy"); fflush(stdout);
    *ctxpp = NULL;
}



cops_return_code_t cops_simlock_ota_unlock(
    cops_context_id_t * ctxp,
    const uint8_t * buf,
    size_t blen,
    cops_simlock_ota_reply_message_t * reply)
{
    (void) ctxp;
    (void) buf;
    (void) blen;

    fprintf(stdout, "\n[COPS] cops_simlock_ota_unlock"); fflush(stdout);

    memset(reply, 0, sizeof(cops_simlock_ota_reply_message_t));

    return (COPS_RC_OK);
}
