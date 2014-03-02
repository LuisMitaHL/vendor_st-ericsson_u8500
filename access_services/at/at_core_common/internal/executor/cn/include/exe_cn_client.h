/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_CN_CLIENT_H
#define EXE_CN_CLIENT_H 1

#include "cn_client.h"
#include "exe_internal.h"

void *cn_client_open_session(void);
cn_context_t *cn_client_get_context(void);
void cn_client_close_session(void *service_p);

#endif
