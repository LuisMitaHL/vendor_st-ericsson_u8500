/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __exe_cn_requests_h__
#define __exe_cn_requests_h__ (1)

#include "exe_request_record.h"


/*
 * Support
 */

/*
 * Misc
 */
exe_request_result_t request_answer(exe_request_record_t *record_p);
exe_request_result_t request_call_dial_up(exe_request_record_t *record_p);
exe_request_result_t request_call_hold(exe_request_record_t *record_p);
exe_request_result_t request_clip_read(exe_request_record_t *record_p);
exe_request_result_t request_cnap_read(exe_request_record_t *record_p);
exe_request_result_t request_colr_do(exe_request_record_t *record_p);
exe_request_result_t request_clir_read(exe_request_record_t *record_p);
exe_request_result_t request_clir_set(exe_request_record_t *record_p);
exe_request_result_t request_call_forwarding(exe_request_record_t *record_p);
exe_request_result_t request_get_current_calls(exe_request_record_t *record_p);
exe_request_result_t request_gprs_registration_state_read(exe_request_record_t *record_p);
exe_request_result_t request_gprs_registration_state_set(exe_request_record_t *record_p);
exe_request_result_t request_hangup(exe_request_record_t *record_p);
exe_request_result_t request_hangup_all(exe_request_record_t *record_p);
exe_request_result_t request_modem_property_set(exe_request_record_t *record_p);
exe_request_result_t request_network_automatic_register(exe_request_record_t *r);
exe_request_result_t request_network_get_settings(exe_request_record_t *r);
exe_request_result_t request_network_manual_register(exe_request_record_t *r);
exe_request_result_t request_network_manual_register_auto_fallback(exe_request_record_t *record_p);
exe_request_result_t request_network_deregister(exe_request_record_t *r);
exe_request_result_t request_network_search(exe_request_record_t *r);
exe_request_result_t request_modem_shutdown(exe_request_record_t *record_p);
exe_request_result_t request_modem_sleep(exe_request_record_t *record_p);
exe_request_result_t request_modem_reset(exe_request_record_t *record_p);
exe_request_result_t request_radio_off(exe_request_record_t *record_p);
exe_request_result_t request_radio_on(exe_request_record_t *record_p);
exe_request_result_t request_radio_status(exe_request_record_t *record_p);
exe_request_result_t request_preferred_network(exe_request_record_t *record_p);
exe_request_result_t request_creg_read(exe_request_record_t *record_p);
exe_request_result_t request_creg_set(exe_request_record_t *record_p);
exe_request_result_t request_ereg_read(exe_request_record_t *record_p);
exe_request_result_t request_ereg_set(exe_request_record_t *record_p);
exe_request_result_t request_signal_strength(exe_request_record_t *record_p);
exe_request_result_t request_dtmf_send(exe_request_record_t *record_p);
exe_request_result_t request_set_dtmf_duration(exe_request_record_t *record_p);
exe_request_result_t request_set_supp_svc_notification(exe_request_record_t *record_p);
exe_request_result_t request_get_dtmf_duration(exe_request_record_t *record_p);
exe_request_result_t request_ussd_cancel(exe_request_record_t *record_p);
exe_request_result_t request_ussd_send(exe_request_record_t *record_p);
exe_request_result_t request_set_call_waiting(exe_request_record_t *record_p);
exe_request_result_t request_query_call_waiting_status(exe_request_record_t *record_p);
exe_request_result_t request_set_query_barring(exe_request_record_t *record_p);
exe_request_result_t request_change_barring_password(exe_request_record_t *record_p);
exe_request_result_t request_signal_info_reporting_set(exe_request_record_t *record_p);
exe_request_result_t request_user_activity_set(exe_request_record_t *record_p);
exe_request_result_t request_neighbour_2g_cellinfo(exe_request_record_t *record_p);
exe_request_result_t request_neighbour_3g_cellinfo(exe_request_record_t *record_p);
exe_request_result_t request_serving_2g_cellinfo(exe_request_record_t *record_p);
exe_request_result_t request_serving_3g_cellinfo(exe_request_record_t *record_p);
exe_request_result_t request_rat_name_ind_reporting(exe_request_record_t *record_p);
exe_request_result_t request_rat_name(exe_request_record_t *record_p);
exe_request_result_t request_als_read(exe_request_record_t *record_p);
exe_request_result_t request_als_set(exe_request_record_t *record_p);
exe_request_result_t request_send_tx_back_off_event(exe_request_record_t *record_p);
exe_request_result_t request_set_empage(exe_request_record_t *record_p);
exe_request_result_t request_radiover_read(exe_request_record_t *record_p);
exe_request_result_t request_nitz_name_ind_reporting(exe_request_record_t *record_p);
exe_request_result_t request_read_imeisv(exe_request_record_t *record_p);


#endif /* __exe_cn_requests_h__ */
