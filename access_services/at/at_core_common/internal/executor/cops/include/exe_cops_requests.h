/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "exe_request_record.h"

#ifndef EXE_COPS_REQUESTS_H
#define EXE_COPS_REQUESTS_H 1

exe_request_result_t request_imei(exe_request_record_t *record_p);
exe_request_result_t request_set_query_facility_personalization_lock(exe_request_record_t *record_p);
exe_request_result_t request_set_query_net_lock(exe_request_record_t *record_p);
exe_request_result_t request_set_query_net_sub_lock(exe_request_record_t *record_p);
exe_request_result_t request_set_query_service_lock(exe_request_record_t *record_p);
exe_request_result_t request_set_query_corporate_lock(exe_request_record_t *record_p);
exe_request_result_t request_set_query_extended_lock(exe_request_record_t *record_p);
exe_request_result_t request_get_challenge_data(exe_request_record_t *record_p);
exe_request_result_t request_authenticate(exe_request_record_t *record_p);
exe_request_result_t request_deauthenticate(exe_request_record_t *record_p);
exe_request_result_t request_set_imei(exe_request_record_t *record_p);
exe_request_result_t request_set_simlock_keys(exe_request_record_t *record_p);
exe_request_result_t request_write_simlock_data(exe_request_record_t *record_p);
exe_request_result_t request_read_simlock_data(exe_request_record_t *record_p);
exe_request_result_t request_unlock_sim_lock(exe_request_record_t *record_p);
exe_request_result_t request_authenticate_eng_mode(exe_request_record_t *record_p);
exe_request_result_t request_deauthenticate_eng_mode(exe_request_record_t *record_p);
exe_request_result_t request_lock_bootpartition(exe_request_record_t *record_p);
exe_request_result_t request_init_antirollback(exe_request_record_t *record_p);
exe_request_result_t request_write_d_and_t(exe_request_record_t *record_p);

#endif /* EXE_COPS_REQUESTS_H */
