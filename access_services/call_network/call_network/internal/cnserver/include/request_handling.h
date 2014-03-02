/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __request_handling_h__
#define __request_handling_h__ (1)

#include "cn_message_types.h"
#include "message_handler.h"

request_status_t handle_request_rf_on(void *data_p, request_record_t *record_p);
request_status_t handle_request_rf_off(void *data_p, request_record_t *record_p);
request_status_t handle_request_rf_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_registration_control(void *data_p, request_record_t *record_p);
request_status_t handle_request_modem_reset(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_preferred_network_type(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_preferred_network_type(void *data_p, request_record_t *record_p);
request_status_t handle_request_registration_state_normal(void *data_p, request_record_t *record_p);
request_status_t handle_request_registration_state_gprs(void *data_p, request_record_t *record_p);
request_status_t handle_request_network_emergency_numbers_update(void *data_p, request_record_t *record_p);
request_status_t handle_request_cell_info(void *data_p, request_record_t *record_p);
request_status_t handle_request_automatic_network_registration(void *data_p, request_record_t *record_p);
request_status_t handle_request_manual_network_registration(void *data_p, request_record_t *record_p);
request_status_t handle_request_manual_network_registration_with_automatic_fallback(void *data_p, request_record_t *record_p);
request_status_t handle_request_network_deregister(void *data_p, request_record_t *record_p);
request_status_t handle_request_net_query_mode(void *data_p, request_record_t *record_p);
request_status_t handle_request_manual_network_search(void *data_p, request_record_t *record_p);
request_status_t handle_request_interrupt_network_search(void *data_p, request_record_t *record_p);
request_status_t handle_request_current_call_list(void *data_p, request_record_t *record_p);
request_status_t handle_request_dial(void *data_p, request_record_t *record_p);
request_status_t handle_request_hangup(void *data_p, request_record_t *record_p);
request_status_t handle_request_swap_calls(void *data_p, request_record_t *record_p);
request_status_t handle_request_hold_call(void *data_p, request_record_t *record_p);
request_status_t handle_request_resume_call(void *data_p, request_record_t *record_p);
request_status_t handle_request_conference_call(void *data_p, request_record_t *record_p);
request_status_t handle_request_conference_call_split(void *data_p, request_record_t *record_p);
request_status_t handle_request_explicit_call_transfer(void *data_p, request_record_t *record_p);
request_status_t handle_request_answer_call(void *data_p, request_record_t *record_p);
request_status_t handle_request_ussd(void *data_p, request_record_t *record_p);
request_status_t handle_request_ussd_abort(void *data_p, request_record_t *record_p);
request_status_t handle_request_rssi_value(void *data_p, request_record_t *record_p);
request_status_t handle_request_clip_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_cnap_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_colr_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_clir_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_clir(void *data_p, request_record_t *record_p);
request_status_t handle_request_query_call_forward(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_call_forward(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_modem_property(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_modem_property(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_cssn(void *data_p, request_record_t *record_p);
request_status_t handle_request_dtmf_send(void *data_p, request_record_t *record_p);
request_status_t handle_request_dtmf_start(void *data_p, request_record_t *record_p);
request_status_t handle_request_dtmf_stop(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_call_waiting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_call_waiting(void *data_p, request_record_t *record_p);
request_status_t handle_request_change_barring_password(void *data_p, request_record_t *record_p);
request_status_t handle_request_query_call_barring(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_call_barring(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_product_profile_flag(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_l1_parameter(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_user_activity_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_neighbour_cells_reporting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_neighbour_cells_reporting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_neighbour_cells_basic_extd_info(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_neighbour_cells_extd_info(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_event_reporting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_rab_status(void *data_p, request_record_t *record_p);
request_status_t handle_request_baseband_version(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_pp_flags(void *data_p, request_record_t *record_p);
request_status_t handle_request_modify_emergency_number_list(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_emergency_number_list(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_signal_info_config(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_signal_info_config(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_signal_info_reporting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_signal_info_reporting(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_timing_advance(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_nmr_info(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_hsxpa_mode(void *data_p, request_record_t *record_p);
request_status_t handle_request_get_hsxpa_mode(void *data_p, request_record_t *record_p);
request_status_t handle_request_ss_command(void *data_p, request_record_t *record_p);
request_status_t handle_request_reg_status_event_config(void *data_p, request_record_t *record_p);
request_status_t handle_request_rat_name(void *data_p, request_record_t *record_p);
request_status_t handle_request_modem_power_off(void *data_p, request_record_t *record_p);
request_status_t handle_request_send_tx_back_off_event(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_default_nvmd(void *data_p, request_record_t *record_p);
request_status_t handle_request_set_empage(void *data_p, request_record_t *record_p);
request_status_t handle_request_modem_reset_with_dump(void *data_p, request_record_t *record_p);
request_status_t handle_request_sleep_test_mode(void *data_p, request_record_t *record_p);

int send_response(int fd, cn_message_type_t type, cn_error_code_t error_code, cn_client_tag_t client_tag, cn_uint32_t payload_size, void *payload_p);

#endif /* __request_handling_h__ */
