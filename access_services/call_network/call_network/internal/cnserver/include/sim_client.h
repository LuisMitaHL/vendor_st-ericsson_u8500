/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __sim_client_h__
#define __sim_client_h__ (1)

#include "cn_general.h"
#include "sim.h"

/* SIM client functions */
cn_bool_t  sim_client_init(void);
cn_bool_t  sim_client_shutdown(void);
ste_sim_t *sim_client_get_handle(void);
cn_bool_t  sim_client_open_session(void);
cn_void_t  sim_client_close_session(void);
cn_void_t  sim_client_callback(int cause, uintptr_t client_tag, void *data_p, void *user_data_p);
int sim_client_select_callback(const int fd, const void *data_p);

#endif
