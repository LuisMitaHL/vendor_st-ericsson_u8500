/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_COPS_CLIENT_H
#define EXE_COPS_CLIENT_H 1

#include "exe.h"

typedef struct copsclient_s copsclient_t;

void *copsclient_open_session(void);
void copsclient_close_session(void *service_p);
void *copsclient_get_id(copsclient_t *copsclient_p);

#endif /* EXE_COPS_CLIENT_H */

