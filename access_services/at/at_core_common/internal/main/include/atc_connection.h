/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_CONNECTION_H
#define ATC_CONNECTION_H

typedef enum {
    ATC_CONNECTION_TYPE_UNUSED,
    ATC_CONNECTION_TYPE_INTERNAL,
    ATC_CONNECTION_TYPE_EXTERNAL, /* Anchor connection */
} atc_connection_type_t;

typedef enum {
    ATC_INPUT_MODE_WAIT_FOR_NEW_LINE,
    ATC_INPUT_MODE_NEW_LINE,
    ATC_INPUT_MODE_AT_COMMAND,
    ATC_INPUT_MODE_TRANSPARENT,
} atc_input_mode_t;

#endif /* ATC_CONNECTION_H */
