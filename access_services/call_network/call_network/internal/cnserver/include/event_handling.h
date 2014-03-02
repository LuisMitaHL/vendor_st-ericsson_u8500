/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __event_handling_h__
#define __event_handling_h__ (1)

#include "cn_general.h"
#include "cn_message_types.h"

void handle_event_radio_status(void *data_p);
void handle_event_modem_registration_status(void *data_p);
void handle_event_time_info(void *data_p);
void handle_event_name_info(void *data_p);
void handle_event_ring(void *data_p);
void handle_event_ring_waiting(void *data_p);
void handle_event_call_state_changed(void *data_p);
void handle_event_net_modem_detailed_fail_cause(void *data_p);
void handle_event_ss_detailed_fail_cause(void *data_p);
void handle_event_ss_status_ind(void *data_p);
void handle_event_ussd(void *data_p);
void handle_event_neighbour_cells_info(void *data_p);
void handle_event_signal_info(void *data_p);
void handle_event_cell_info(void *data_p);
void handle_event_call_supp_svc_notification(void *data_p);
void handle_event_call_cnap(void *data_p);
void handle_event_generate_local_comfort_tones(void *data_p);
void handle_event_ss_status_ind(void *data_p);
void handle_event_radio_info(void *data_p);
void handle_event_rat_name(void *data_p);
void handle_event_modem_emergency_number_indication(void *data_p);
void handle_event_empage_measurement(void *data_p);
void handle_event_empage_error(void *data_p);
void handle_event_empage_activation(void *data_p);
void handle_event_empage_deactivation(void *data_p);

int send_event(cn_message_type_t type, cn_error_code_t error_code, cn_uint32_t payload_size, void *payload_p);

#endif /* __event_handling_h__ */
