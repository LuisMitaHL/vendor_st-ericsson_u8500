/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <atc_command_table.h>
#include <atc_context.h>
#include <atc_parser.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
/*#include "test.h"*/

unsigned int RESPONSE_BUFFER_LENGTH = 0;
char RESPONSE_BUFFER[2100];

static bool transparent = false;

int atc_write_to_client(atc_context_t *context_p, unsigned char *data, unsigned int length)
{
    if (sizeof(RESPONSE_BUFFER) <= RESPONSE_BUFFER_LENGTH + length) {
        printf("%s: Response (%d) > buffer!", __FUNCTION__, length);
        return -1;
    }

    memcpy(&RESPONSE_BUFFER[RESPONSE_BUFFER_LENGTH], data, length);
    RESPONSE_BUFFER_LENGTH += length;
    RESPONSE_BUFFER[RESPONSE_BUFFER_LENGTH] = '\0';

    return length;
}

/* Dummy functions to be able to link */

int atc_write_to_at_service(unsigned char contextID, unsigned char *cmd, size_t size)
{
    return 0;
}

bool atc_configure_serial(anchor_connection_t *conn_p, char veol, char verase)
{
    printf("%s: conn_p=%p, veol=%d, verase=%d\n", __FUNCTION__, conn_p, veol, verase);
    return true;
}

/* End of dummy functions to be able to link */
