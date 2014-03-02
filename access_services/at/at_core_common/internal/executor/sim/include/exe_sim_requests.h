/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_SIM_REQUESTS_H
#define EXE_SIM_REQUESTS_H 1

#include "exe_request_record.h"
#include "sim.h"

/* TODO add when sim is delivered #include "cat.h" */

#if 0
/* Recode bytes to hex */
static char *ril_code_from_bytetohex(const uint8_t *data,
                                     const unsigned len);
/*Recode hex to bytes */
static uint8_t *ril_code_from_hextobyte(const char *data,
                                        const unsigned len);
#endif

/*
 * Utility
 */
exe_cmee_error_t convert_sim_status_codes_to_cmee_error(ste_uicc_status_code_t              uicc_status_code,
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details,
        ste_uicc_status_word_t              status_word);

/*
 * SIM IO
 */
exe_request_result_t request_sim_io_file_information_command(exe_request_record_t *record_p);
exe_request_result_t request_sim_io_read_binary(exe_request_record_t *record_p);
exe_request_result_t request_sim_io_service_availability_read_binary(exe_request_record_t *record_p);
exe_request_result_t request_sim_io_read_record(exe_request_record_t *record_p);
exe_request_result_t request_sim_io_update_binary(exe_request_record_t *record_p);
exe_request_result_t request_sim_io_update_record(exe_request_record_t *record_p);
exe_request_result_t request_generic_sim_access(exe_request_record_t *record_p);
/*
 * SIM
 */
exe_request_result_t request_get_sim_state_cpin(exe_request_record_t *record_p);
exe_request_result_t request_get_sim_state(exe_request_record_t *record_p);
exe_request_result_t request_get_pin_info(exe_request_record_t *record_p);
exe_request_result_t request_get_full_pin_info(exe_request_record_t *record_p);
exe_request_result_t request_get_imsi(exe_request_record_t *record_p);
exe_request_result_t request_sim_pin_change(exe_request_record_t *record_p);
exe_request_result_t request_verify_sim_pin(exe_request_record_t *record_p);
exe_request_result_t request_verify_sim_puk(exe_request_record_t *record_p);
exe_request_result_t request_verify_sim_pin2(exe_request_record_t *record_p);
exe_request_result_t request_verify_sim_puk2(exe_request_record_t *record_p);
exe_request_result_t request_sim_set_smsc(exe_request_record_t *record_p);
exe_request_result_t request_sim_get_smsc(exe_request_record_t *record_p);
exe_request_result_t request_restore_smsc_profile(exe_request_record_t *record_p);
exe_request_result_t request_save_smcs_to_profile(exe_request_record_t *record_p);
exe_request_result_t request_smsc_profiles_max(exe_request_record_t *record_p);
exe_request_result_t request_get_sim_type(exe_request_record_t *record_p);
exe_request_result_t request_sim_lock(exe_request_record_t *record_p);
exe_request_result_t request_fixed_dialing(exe_request_record_t *record_p);
exe_request_result_t request_set_cpol_data(exe_request_record_t *record_p);
exe_request_result_t request_get_full_sim_info(exe_request_record_t *record_p);
exe_request_result_t request_get_cpol_data(exe_request_record_t *record_p);
exe_request_result_t request_cpol_data_test(exe_request_record_t *record_p);
exe_request_result_t request_open_channel(exe_request_record_t *record_p);
exe_request_result_t request_close_channel(exe_request_record_t *record_p);
exe_request_result_t request_get_sim_apps_info(exe_request_record_t *record_p);
exe_request_result_t request_sim_restart(exe_request_record_t *record_p);
/*
 * STK
 */
exe_request_result_t request_stkc_pc_control(exe_request_record_t *record_p);
exe_request_result_t request_get_terminal_profile(exe_request_record_t *record_p);
exe_request_result_t request_sat_envelope_command(exe_request_record_t *record_p);
exe_request_result_t request_send_terminal_response(exe_request_record_t *record_p);
exe_request_result_t request_get_sim_app_type(exe_request_record_t *record_p);
exe_request_result_t request_cnum_subscriber_number(exe_request_record_t *record_p);
exe_request_result_t request_answer_setup_call(exe_request_record_t *record_p);

#endif
