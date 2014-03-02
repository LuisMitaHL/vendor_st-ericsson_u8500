/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_API_CSCALL_H
#define INCLUSION_GUARD_STE_ADM_API_CSCALL_H

#include "ste_adm_srv.h"
#include "ste_adm_api_devset.h"

void csapi_init();

void csapi_set_upstream_mute(msg_cscall_mute_t* msg_p, struct srv_session_s** client_pp);
void csapi_set_downstream_mute(msg_cscall_mute_t* msg_p, struct srv_session_s** client_pp);
void csapi_get_upstream_mute(msg_cscall_mute_t* msg_p, struct srv_session_s** client_pp);
void csapi_get_downstream_mute(msg_cscall_mute_t* msg_p, struct srv_session_s** client_pp);

void csapi_set_upstream_volume(msg_cscall_volume_t* msg_p, struct srv_session_s** client_pp);
void csapi_set_downstream_volume(msg_cscall_volume_t* msg_p, struct srv_session_s** client_pp);
void csapi_get_upstream_volume(msg_cscall_volume_t* msg_p, struct srv_session_s** client_pp);
void csapi_get_downstream_volume(msg_cscall_volume_t* msg_p, struct srv_session_s** client_pp);

void csapi_init_downstream_volume(msg_cscall_init_volume_t* msg_p, struct srv_session_s** client_pp);


void csapi_set_devices(msg_cscall_devices_t* msg_p, struct srv_session_s** client_pp);

int csapi_dict_in_open(msg_device_t* msg_p, void** client_pp);
void csapi_dict_in_close(msg_device_t* msg_p, void** client_pp);
void csapi_dict_in_datareq(msg_data_t* msg_p, void** client_pp);
void csapi_dict_in_dtor(void* client_p);

int csapi_dict_out_open(msg_device_t* msg_p, void** client_pp);
void csapi_dict_out_close(msg_device_t* msg_p, void** client_pp);
void csapi_dict_out_feed(msg_data_t* msg_p, void** client_pp);
void csapi_dict_out_drain(msg_base_t* msg_p, void** client_pp);
void csapi_dict_out_dtor(void* client_p);

void csapi_dict_set_mode(msg_cscall_dict_mode_t* msg_p, struct srv_session_s** client_pp);
void csapi_dict_set_mute(msg_cscall_dict_mute_t* msg_p, srv_session_t** client_pp);
void csapi_dict_get_mute(msg_cscall_dict_mute_t* msg_p, srv_session_t** client_pp);

void csapi_get_modem_type(msg_get_modem_type_t* msg_p, struct srv_session_s** client_pp);

void csapi_get_tty(msg_cscall_tty_t* msg_p, struct srv_session_s** client_pp);
void csapi_set_tty(msg_cscall_tty_t* msg_p, struct srv_session_s** client_pp);
void csapi_set_voip(msg_cscall_voip_t* msg_p, struct srv_session_s** client_pp);
void csapi_set_loopback(msg_cscall_loopback_t* msg_p, struct srv_session_s** client_pp);

int  csapi_voip_in_open(msg_device_t* msg_p, void** client_pp);
void csapi_voip_in_close(msg_device_t* msg_p, void** client_pp);
void csapi_voip_in_datareq(msg_data_t* msg_p, void** client_pp);
void csapi_voip_in_dtor(void* client_p);

int  csapi_voip_out_open(msg_device_t* msg_p, void** client_pp);
void csapi_voip_out_close(msg_device_t* msg_p, void** client_pp);
void csapi_voip_out_feed(msg_data_t* msg_p, void** client_pp);
void csapi_voip_out_drain(msg_base_t* msg_p, void** client_pp);
void csapi_voip_out_dtor(void* client_p);

void csapi_rescan_config(msg_rescan_config_t* msg_p, struct srv_session_s** client_pp);
void csapi_rescan_speechproc(msg_base_t* msg_p, struct srv_session_s** client_pp);

ste_adm_res_t csapi_reload_device_settings();
int csapi_get_samplerate();
int csapi_cscall_active();
// Used for debugging
void csapi_destroy();
void csapi_remap(msg_set_toplevel_map_t* msg_p, struct srv_session_s** client_pp);




typedef void (*csapi_modem_vc_state_cb_fp_t)(void* param);
ste_adm_vc_modem_status_t csapi_current_vc_state(void);
void csapi_notify_vc_state_change(void);
void csapi_modem_vc_state_cb(void* param);
void csapi_modem_vc_state_close(srv_session_t* param);
void csapi_request_modem_vc_state(msg_modem_vc_state_t* msg_p, srv_session_t** client_pp);
ste_adm_res_t csapi_subscribe_modem_vc_state(csapi_modem_vc_state_cb_fp_t cb_fp,
                                     void* param,
                                     int* subscription_handle_p);
void csapi_unsubscribe_modem_vc_state(int handle);
#endif // INCLUSION_GUARD_STE_ADM_API_CSCALL_H
