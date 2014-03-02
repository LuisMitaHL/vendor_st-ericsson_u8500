/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_API_D2D_H
#define INCLUSION_GUARD_STE_ADM_API_D2D_H

#include "ste_adm_srv.h"

void d2d_connect(msg_d2d_connect_t* msg_p, struct srv_session_s** client_pp);

void d2d_disconnect(msg_d2d_disconnect_t* msg_p, struct srv_session_s** client_pp);

#endif // INCLUSION_GUARD_STE_ADM_API_D2D_H
