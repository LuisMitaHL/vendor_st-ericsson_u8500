/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_API_DEVSET_H
#define INCLUSION_GUARD_STE_ADM_API_DEVSET_H

#include "ste_adm_srv.h"

void topapi_open(msg_device_t* msg_p,
                 srv_session_t** client_pp);

void topapi_close(msg_device_t* msg_p,
                 srv_session_t** client_pp);

void topapi_feed(msg_data_t* msg_p, srv_session_t** client_pp);
void topapi_datareq(msg_data_t* msg_p, srv_session_t** client_pp);
void topapi_drain(msg_base_t* msg_p, srv_session_t** client_pp);
void devapi_get_sink_latency(msg_sink_latency_t* msg_p, srv_session_t** client_pp);
void devapi_set_pcm_probe(msg_set_pcm_probe_t* msg_p, srv_session_t** client_pp);
void devapi_set_app_vol(msg_app_volume_t* msg_p, srv_session_t** client_pp);
void devapi_get_app_vol(msg_app_volume_t* msg_p, srv_session_t** client_pp);
void devapi_get_config(msg_get_set_config* msg_p, srv_session_t** client_pp);
void devapi_set_config(msg_get_set_config* msg_p, srv_session_t** client_pp);
void devapi_get_active_devices(msg_get_active_devices_t* msg_p, srv_session_t** client_pp);
void devapi_reload_settings(msg_base_t* msg_p, srv_session_t** client_pp);


#endif // INCLUSION_GUARD_STE_ADM_API_DEVSET_H
