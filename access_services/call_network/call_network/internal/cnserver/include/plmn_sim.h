/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __plmn_sim_h__
#define __plmn_sim_h__ (1)

#include <stdbool.h>
#include "message_handler.h"
#include "sim.h"

int plmn_sim_load(void);
bool plmn_sim_load_active(void);
bool plmn_sim_path_match(sim_path_t *path_p);
request_status_t plmn_sim_handle_request(void *data_p, request_record_t *record_p);

#endif /* __plmn_sim_h__ */
