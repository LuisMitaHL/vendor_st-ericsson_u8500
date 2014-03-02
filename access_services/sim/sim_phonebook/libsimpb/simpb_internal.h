/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _INCLUSION_GUARD_SIMPB_INTERNAL_H_
#define _INCLUSION_GUARD_SIMPB_INTERNAL_H_

#include <stdint.h>

#include "simpb.h"

struct ste_simpb_s {
    int                     fd;
    int                     simpb_handle;
    ste_simpb_cb_t          client_cb;      /* Client call-back function to close the asynchronous request */
    void                   *client_context; /* Client context */
    ste_simpb_status_t      status;
    ste_simpb_sim_state_t   sim_state;
    uint8_t                *buf;
    uintptr_t               size;
    uintptr_t               used;
};

#endif /* _INCLUSION_GUARD_SIMPB_INTERNAL_H_ */
