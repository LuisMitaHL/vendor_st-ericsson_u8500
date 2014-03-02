/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_EXTENDED_SIMPB_CLIENT_H
#define EXE_EXTENDED_SIMPB_CLIENT_H 1

typedef struct simpb_client_s simpb_client_t;

#include "exe_internal.h"
#include "simpb.h"

void *simpb_client_open_session(void);
ste_simpb_t *simpb_client_get_handle(simpb_client_t *simpb_client_p);
void simpb_client_close_session(simpb_client_t *simpb_client_p);

bool simpb_client_is_adn_file(uint16_t file_id);

#endif /* EXE_EXTENDED_SIMPB_CLIENT_H */
