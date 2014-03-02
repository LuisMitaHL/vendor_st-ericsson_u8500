/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __module_test_main_h__
#define __module_test_main_h__ (1)

#include "cn_client.h"

void *module_test_main(void *data_p);

typedef struct {
    int status;
} module_test_data_t;

extern int g_request_fd;
extern int g_event_fd;
extern cn_context_t *g_context_p;

#endif /* __module_test_main_h__ */


