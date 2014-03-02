/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_SIM_CLIENT_H
#define EXE_SIM_CLIENT_H 1


typedef struct sim_client_s sim_client_t;


#include "exe_internal.h"
#include "sim.h"

void *sim_client_open_session(void);
void sim_client_close_session(void *service_p);
ste_sim_t *sim_client_get_handle(sim_client_t *sim_client_p);

#endif
