/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_API_UTIL_H
#define INCLUSION_GUARD_STE_ADM_API_UTIL_H

#include "ste_adm_srv.h"

void util_api_get_toplevel_map(msg_get_toplevel_map_t* msg_p, struct srv_session_s** client_pp);
void util_api_get_toplevel_device(msg_get_toplevel_device_t* msg_p, struct srv_session_s** client_pp);
void util_api_set_external_delay(msg_set_external_delay_t* msg_p, struct srv_session_s** client_pp);
void util_api_get_max_out_latency(msg_get_max_out_latency_t* msg_p, struct srv_session_s** client_pp);
void util_api_execute_sql(msg_execute_sql_t* msg_p, struct srv_session_s** client_pp);

#endif // INCLUSION_GUARD_STE_ADM_API_UTIL_H
