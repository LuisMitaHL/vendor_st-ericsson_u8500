/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_rat_control_h__
#define __cn_rat_control_h__ (1)

#include "request_handling.h"

int update_rat_from_sim(void);

request_status_t cn_rat_control_handle_request(void *data_p, request_record_t *record_p);

#endif /* __cn_rat_control_h__ */
