/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __testcases_h__
#define __testcases_h__ (1)

#include <pthread.h>
#include "cn_client.h"

#define SET_TC_RESULT_AND_GOTO_EXIT(result) tc_result = (result);goto exit

#define TC_ASSERT(condition) tc_result &= (condition); \
                             if (!tc_result) { \
                                 printf("%s: TC_ASSERT failure on line %d\n",__FUNCTION__, __LINE__); \
                                 goto exit; \
                             }

typedef enum {
    TC_RESULT_FAILED = 0, /* FAILED = 0, OK = 1 should be used due to bitwise AND in TC_ASSERT */
    TC_RESULT_OK     = 1,
    TC_RESULT_NOT_IMPLEMENTED,
    TC_RESULT_UNKNOWN
} tc_result_t;


typedef enum {
    TESTCASE_TYPE_UNKNOWN,
    TESTCASE_TYPE_POSITIVE,
    TESTCASE_TYPE_NEGATIVE
} testcase_type_t;

typedef tc_result_t (*testcase_func_t)(void);


typedef struct {
    testcase_type_t  type;
    const char      *name_p;
    testcase_func_t  tc_func_p;
    tc_result_t      result;
} tc_table_entry_t;


/* ===========================
 * TESTCASE DEVELOPMENT NOTES:
 * ===========================
 * There should be at lease one positive and one negative testcase per request/event. To increase code
 * coverage  additional ones should be added. Please use the "tc_result_t <request/event name>_<positive/negative>_<nr>(void);"
 * syntax when naming the testcase so the purpose of the function is very clear.
 */

/* =================
 * REQUEST TESTCASES
 * =================
 */

tc_result_t cn_request_answer_call_negative_1(void);
tc_result_t cn_request_answer_call_positive_1(void);
tc_result_t cn_request_automatic_network_registration_negative_1(void);
tc_result_t cn_request_automatic_network_registration_positive_1(void);
tc_result_t cn_request_clip_status_negative_1(void);
tc_result_t cn_request_clip_status_positive_1(void);
tc_result_t cn_request_clir_status_negative_1(void);
tc_result_t cn_request_clir_status_positive_1(void);
tc_result_t cn_request_cnap_status_negative_1(void);
tc_result_t cn_request_cnap_status_positive_1(void);
tc_result_t cn_request_colr_status_negative_1(void);
tc_result_t cn_request_colr_status_positive_1(void);
tc_result_t cn_request_conference_call_negative_1(void);
tc_result_t cn_request_conference_call_positive_1(void);
tc_result_t cn_request_conference_call_split_negative_1(void);
tc_result_t cn_request_conference_call_split_positive_1(void);
tc_result_t cn_request_current_call_list_negative_1(void);
tc_result_t cn_request_current_call_list_positive_1(void);
tc_result_t cn_request_dial_negative_1(void);
tc_result_t cn_request_dial_negative_2(void);
tc_result_t cn_request_dial_positive_1(void);
tc_result_t cn_request_dial_positive_2(void);
tc_result_t cn_request_dial_positive_3(void);
tc_result_t cn_request_dial_positive_4(void);
tc_result_t cn_request_dtmf_send_negative_1(void);
tc_result_t cn_request_dtmf_send_negative_2(void);
tc_result_t cn_request_dtmf_send_negative_3(void);
tc_result_t cn_request_dtmf_send_negative_4(void);
tc_result_t cn_request_dtmf_send_negative_5(void);
tc_result_t cn_request_dtmf_send_negative_6(void);
tc_result_t cn_request_dtmf_send_positive_1(void);
tc_result_t cn_request_dtmf_send_positive_2(void);
tc_result_t cn_request_dtmf_send_positive_3(void);
tc_result_t cn_request_dtmf_send_positive_4(void);
tc_result_t cn_request_dtmf_start_negative_1(void);
tc_result_t cn_request_dtmf_start_positive_1(void);
tc_result_t cn_request_dtmf_stop_negative_1(void);
tc_result_t cn_request_dtmf_stop_positive_1(void);
tc_result_t cn_request_explicit_call_transfer_negative_1(void);
tc_result_t cn_request_explicit_call_transfer_positive_1(void);
tc_result_t cn_request_get_call_waiting_negative_1(void);
tc_result_t cn_request_get_call_waiting_positive_1(void);
tc_result_t cn_request_get_emergency_number_list_positive_1(void);
tc_result_t cn_request_get_emergency_number_list_positive_2(void);
tc_result_t cn_request_get_emergency_number_list_positive_3(void);
tc_result_t cn_request_get_emergency_number_list_positive_4(void);
tc_result_t cn_request_get_preferred_network_type_negative_1(void);
tc_result_t cn_request_get_preferred_network_type_positive_1(void);
tc_result_t cn_request_get_timing_advance_fail(void);
tc_result_t cn_request_get_timing_advance(void);
tc_result_t cn_request_hangup_negative_1(void);
tc_result_t cn_request_hangup_negative_2(void);
tc_result_t cn_request_hangup_negative_3(void);
tc_result_t cn_request_hangup_positive_1(void);
tc_result_t cn_request_hold_call_negative_1(void);
tc_result_t cn_request_hold_call_positive_1(void);
tc_result_t cn_request_interrupt_network_search_negative_1(void);
tc_result_t cn_request_interrupt_network_search_positive_1(void);
tc_result_t cn_request_manual_network_registration_negative_1(void);
tc_result_t cn_request_manual_network_registration_positive_1(void);
tc_result_t cn_request_manual_network_registration_with_automatic_fallback_negative_1(void);
tc_result_t cn_request_manual_network_registration_with_automatic_fallback_negative_2(void);
tc_result_t cn_request_manual_network_registration_with_automatic_fallback_positive_1(void);
tc_result_t cn_request_manual_network_search_negative_1(void);
tc_result_t cn_request_manual_network_search_positive_1(void);
tc_result_t cn_request_get_neighbour_cells_complete_info_positive_1(void);
tc_result_t cn_request_get_neighbour_cells_complete_info_negative_1(void);
tc_result_t cn_request_get_neighbour_cells_complete_info_negative_2(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_positive_1(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_positive_2(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_negative_1(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_negative_2(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_negative_3(void);
tc_result_t cn_request_get_neighbour_cells_extd_info_negative_4(void);
tc_result_t cn_request_net_query_mode_negative_1(void);
tc_result_t cn_request_net_query_mode_positive_1(void);
tc_result_t cn_request_network_deregister_negative_1(void);
tc_result_t cn_request_network_deregister_positive_1(void);
tc_result_t cn_request_nmr_geran_info(void);
tc_result_t cn_request_nmr_utran_info_fail(void);
tc_result_t cn_request_nmr_utran_info(void);
tc_result_t cn_request_registration_state_gprs_negative_1(void);
tc_result_t cn_request_registration_state_gprs_positive_1(void);
tc_result_t cn_request_cell_info_negative_1(void);
tc_result_t cn_request_cell_info_positive_1(void);
tc_result_t cn_request_registration_state_normal_negative_1(void);
tc_result_t cn_request_registration_state_normal_positive_1(void);
tc_result_t cn_request_resume_call_negative_1(void);
tc_result_t cn_request_resume_call_positive_1(void);
tc_result_t cn_request_rf_off_negative_1(void);
tc_result_t cn_request_rf_off_positive_1(void);
tc_result_t cn_request_rf_on_negative_1(void);
tc_result_t cn_request_rf_on_negative_2(void);
tc_result_t cn_request_rf_on_positive_1(void);
tc_result_t cn_request_rf_status_negative_1(void);
tc_result_t cn_request_rf_status_positive_1(void);
tc_result_t cn_request_registration_control_positive_1(void);
tc_result_t cn_request_registration_control_negative_1(void);
tc_result_t cn_request_reset_modem_positive_1(void);
tc_result_t cn_request_reset_modem_negative_1(void);
tc_result_t cn_request_rssi_value_negative_1(void);
tc_result_t cn_request_rssi_value_positive_1(void);
tc_result_t cn_request_set_call_barring_negative_1(void);
tc_result_t cn_request_set_call_barring_positive_1(void);
tc_result_t cn_request_set_call_forward_negative_1(void);
tc_result_t cn_request_set_call_forward_positive_1(void);
tc_result_t cn_request_set_call_waiting_negative_1(void);
tc_result_t cn_request_set_call_waiting_positive_1(void);
tc_result_t cn_request_set_clir_negative_1(void);
tc_result_t cn_request_set_clir_positive_1(void);
tc_result_t cn_request_set_modem_sleep_on_positive_1(void);
tc_result_t cn_request_set_modem_sleep_on_negative_1(void);
tc_result_t cn_request_set_modem_sleep_off_positive_1(void);
tc_result_t cn_request_set_modem_sleep_off_negative_1(void);
tc_result_t cn_request_set_modem_property_negative_1(void);
tc_result_t cn_request_set_modem_property_positive_1(void);
tc_result_t cn_request_set_modem_property_positive_2(void);
tc_result_t cn_request_set_modem_property_positive_3(void);
tc_result_t cn_request_set_modem_property_positive_4(void);
tc_result_t cn_request_cssn_positive_1(void);
tc_result_t cn_request_set_default_nvmd_positive_1(void);
tc_result_t cn_request_set_signal_info_config_positive_1(void);
tc_result_t cn_request_set_signal_info_config_1(void);
tc_result_t cn_request_get_signal_info_config_1(void);
tc_result_t cn_request_set_signal_info_reporting_1(void);
tc_result_t cn_request_get_signal_info_reporting_1(void);
tc_result_t cn_request_set_hsxpa_mode_1(void);
tc_result_t cn_request_get_hsxpa_mode_1(void);
tc_result_t cn_request_set_event_reporting_1(void);
tc_result_t cn_request_get_event_reporting_1(void);
tc_result_t cn_request_rab_status_1(void);
tc_result_t cn_request_set_registration_control_positive_1(void);
tc_result_t cn_request_set_registration_control_negative_1(void);
tc_result_t cn_request_set_modem_reset_positive_1(void);
tc_result_t cn_request_set_modem_reset_negative_1(void);
tc_result_t cn_request_set_preferred_network_type_negative_1(void);
tc_result_t cn_request_set_preferred_network_type_negative_2(void);
tc_result_t cn_request_set_preferred_network_type_positive_1(void);
tc_result_t cn_request_ss_command_barring_1(void);
tc_result_t cn_request_ss_command_forward_register(void);
tc_result_t cn_request_ss_command_forward_erasure(void);
tc_result_t cn_request_ss_command_forward_register_2(void);
tc_result_t cn_request_ss_command_forward_register_3(void);
tc_result_t cn_request_ss_command_forward_activation(void);
tc_result_t cn_request_ss_command_forward_deactivation(void);
tc_result_t cn_request_ss_command_forward_error_1(void);
tc_result_t cn_request_ss_command_forward_query_1(void);
tc_result_t cn_request_ss_command_forward_query_2(void);
tc_result_t cn_request_swap_calls_negative_1(void);
tc_result_t cn_request_swap_calls_positive_1(void);
tc_result_t cn_request_ussd_abort_negative_1(void);
tc_result_t cn_request_ussd_abort_positive_1(void);
tc_result_t cn_request_ussd_negative_1(void);
tc_result_t cn_request_ussd_negative_2(void);
tc_result_t cn_request_ussd_positive_1(void);
tc_result_t cn_request_reg_status_event_config_1(void);
tc_result_t cn_request_reg_status_event_config_2(void);
tc_result_t cn_request_reg_status_event_config_3(void);
tc_result_t cn_request_reg_status_event_config_4(void);
tc_result_t cn_request_reg_status_event_config_5(void);
tc_result_t cn_request_reg_status_event_config_6(void);
tc_result_t cn_request_rat_name_positive_1(void);
tc_result_t cn_request_rat_name_positive_2(void);
tc_result_t cn_request_rat_name_negative_1(void);
tc_result_t cn_request_modem_power_off_positive_1(void);
tc_result_t cn_request_modem_power_off_negative_1(void);
tc_result_t cn_request_send_tx_back_off_event_positive_1(void);
tc_result_t cn_request_send_tx_back_off_event_positive_2(void);
tc_result_t cn_request_send_tx_back_off_event_negative_1(void);
tc_result_t cn_request_ftd_oneshot(void);
tc_result_t cn_request_ftd_start_periodic1(void);
tc_result_t cn_request_ftd_start_periodic2(void);
tc_result_t cn_request_ftd_stop_periodic(void);
tc_result_t cn_request_ftd_reset(void);
tc_result_t cn_request_ftd_start_periodic_not_finish(void);
tc_result_t cn_request_ftd_stop_periodic_err(void);
tc_result_t cn_request_ftd_stop_periodic_not_finish(void);
tc_result_t cn_request_ftd_start_periodic_err(void);
/* =================
 * EVENT TESTCASES
 * =================
 */
tc_result_t cn_event_radio_status_positive_1(void);
tc_result_t cn_event_modem_registration_status_positive_1(void);
tc_result_t cn_event_time_info_positive_1(void);
tc_result_t cn_event_name_info_positive_gsm7(void);
tc_result_t cn_event_name_info_positive_ucs2(void);
tc_result_t cn_event_net_modem_detailed_fail_cause_positive_1(void);
tc_result_t cn_event_ss_detailed_fail_cause_positive_1(void);
tc_result_t cn_event_ss_detailed_fail_cause_positive_2(void);
tc_result_t cn_event_ss_detailed_fail_cause_negative_1(void);
tc_result_t cn_event_ussd_positive_1(void);
tc_result_t cn_event_gsm_ussd_req(void);
tc_result_t cn_event_gsm_ussd_command(void);
tc_result_t cn_event_gsm_ussd_notify(void);
tc_result_t cn_event_ring_positive_1(void);
tc_result_t cn_event_call_cnap_positive_1(void);
tc_result_t cn_event_call_state_changed_positive_1(void);
tc_result_t cn_event_call_state_changed_positive_2(void);
tc_result_t cn_event_call_state_changed_positive_3(void);
tc_result_t cn_event_cell_info_1(void);
tc_result_t cn_event_neighbour_cells_info_positive_1(void);
tc_result_t cn_event_generate_local_comfort_tones_positive_1();
tc_result_t cn_event_generate_local_comfort_tones_positive_2();
tc_result_t cn_event_ss_status_info_positive_1(void);
tc_result_t cn_event_ss_status_ussd_stop(void);
tc_result_t cn_event_ss_status_ussd_failed(void);
tc_result_t cn_event_radio_info_positive_1(void);
tc_result_t cn_event_rat_name_positive_1(void);
tc_result_t cn_event_rat_name_positive_2(void);
tc_result_t cn_event_enl_emergency_number_ind_empty_list(void);
tc_result_t cn_event_enl_emergency_number_ind_populated_list(void);
tc_result_t cn_event_enl_registration_status_ind_empty_list(void);
tc_result_t cn_event_enl_registration_status_ind_populated_list(void);
tc_result_t cn_event_enl_registration_status_response_empty_list(void);
tc_result_t cn_event_enl_registration_status_response_populated_list(void);
tc_result_t cn_event_enl_mixed_ind_responses_and_indications_populated_list(void);
tc_result_t cn_event_ftd_issue_measurements(void);
tc_result_t cn_event_ftd_issue_error_measurements(void);
tc_result_t cn_event_ftd_issue_empage_activate_failure(void);
tc_result_t cn_event_ftd_issue_empage_deactivate_success(void);
tc_result_t cn_event_ftd_issue_empage_deactivate_failure(void);

/* =============================
 * SS ENCODER/DECODER TESTCASES
 * =============================
 */
tc_result_t ss_decoder_positive_1();
tc_result_t ss_decoder_positive_2();
tc_result_t ss_decoder_positive_3();
tc_result_t ss_decoder_positive_4();
tc_result_t ss_decoder_positive_5();
tc_result_t ss_decoder_positive_6();
tc_result_t ss_decoder_positive_7();
tc_result_t ss_decoder_positive_8();
tc_result_t ss_decoder_positive_9();
tc_result_t ss_decoder_positive_10();
tc_result_t ss_decoder_positive_11();
tc_result_t ss_decoder_positive_12();
tc_result_t ss_decoder_positive_13();
tc_result_t ss_decoder_positive_14();
tc_result_t ss_decoder_negative_1();
tc_result_t ss_decoder_negative_2();
tc_result_t ss_decoder_negative_3();
tc_result_t ss_decoder_negative_4();
tc_result_t ss_decoder_negative_5();
tc_result_t ss_encoder_positive_1();
tc_result_t ss_encoder_positive_2();
tc_result_t ss_encoder_positive_3();
tc_result_t ss_encoder_positive_4();
tc_result_t ss_encoder_positive_5();
tc_result_t ss_encoder_positive_6();
tc_result_t ss_encoder_positive_7();
tc_result_t ss_encoder_positive_8();
tc_result_t ss_encoder_positive_9();
tc_result_t ss_encoder_positive_10();
tc_result_t ss_encoder_positive_11();
tc_result_t ss_encoder_positive_12();
tc_result_t ss_encoder_positive_13();
tc_result_t ss_encoder_negative_1();
tc_result_t ss_encoder_negative_2();
tc_result_t ss_encoder_negative_3();

/* =============================
 * CN PREDIAL CHECK TESTCASES
 * =============================
 */

/* MSG TRANSLATION */
tc_result_t pdc_msg_translation_positive_1();
tc_result_t pdc_msg_translation_positive_2();
tc_result_t pdc_msg_translation_positive_3();
tc_result_t pdc_msg_translation_positive_4();
tc_result_t pdc_msg_translation_positive_5();
tc_result_t pdc_msg_translation_positive_6();
//tc_result_t pdc_msg_translation_positive_7();

/* ECC */
tc_result_t pdc_ecc_negative_1();
tc_result_t pdc_ecc_negative_2();
tc_result_t pdc_ecc_negative_3();

tc_result_t pdc_ecc_default_positive_1();
tc_result_t pdc_ecc_default_positive_2();
tc_result_t pdc_ecc_simlist_positive_1();
tc_result_t pdc_ecc_simlist_positive_2();
tc_result_t pdc_ecc_filelist_positive_1();
tc_result_t pdc_ecc_postsat_positive_1();
tc_result_t pdc_ecc_postsat_positive_2();
tc_result_t pdc_ecc_postsat_positive_3();

tc_result_t pdc_ecc_all_positive_1();
tc_result_t pdc_ecc_all_positive_2();
tc_result_t cn_pdc_set_emergency_number_filepath_postive_1();
tc_result_t util_continue_as_privileged_user_postive_1();
tc_result_t pdc_ecc_all_positive_3();
tc_result_t pdc_voice_call_failure_1();
tc_result_t pdc_voice_call_accept_without_modification_1();
tc_result_t pdc_voice_call_accept_with_modification_1();

/* FDN */
tc_result_t pdc_number_comparison_positive_1();
tc_result_t pdc_number_comparison_positive_2();
tc_result_t pdc_number_comparison_negative_1();
tc_result_t pdc_fdn_negative_1();
tc_result_t pdc_fdn_negative_2();
tc_result_t pdc_fdn_negative_3();
tc_result_t pdc_fdn_negative_4();
tc_result_t pdc_fdn_positive_1();
tc_result_t pdc_fdn_positive_2();
tc_result_t pdc_fdn_positive_3();
tc_result_t pdc_fdn_positive_4();
tc_result_t pdc_fdn_negative_24();

/* SAT CC */
tc_result_t pdc_sat_voice_1();
tc_result_t pdc_sat_ss_1();
tc_result_t pdc_sat_ss_2();
tc_result_t pdc_sat_ussd_1();
tc_result_t pdc_sat_no_app_1();
tc_result_t pdc_sat_negative_1();
tc_result_t pdc_sat_negative_2();
tc_result_t pdc_sat_negative_3();
tc_result_t pdc_sat_negative_4();
tc_result_t pdc_sat_negative_5();
tc_result_t pdc_sat_negative_6();

tc_result_t pdc_make_ss_request();
tc_result_t pdc_make_ss_request_negative_1();
tc_result_t pdc_make_ss_request_negative_2();
tc_result_t pdc_make_ussd_request();

/* =============================
 * PLMN TESTCASES
 * =============================
 */

tc_result_t plmn_test_reset();

/* PLMN list populate */
tc_result_t plmn_test_list_populate();
tc_result_t plmn_test_list_repopulate();
tc_result_t plmn_test_list_populate_negative();

/* PLMN file handling */
tc_result_t plmn_test_file_read_negative();
tc_result_t plmn_test_file_dump();
tc_result_t plmn_test_file_read();

/* PLMN NITZ update */
tc_result_t plmn_test_nitz_update();

/* PLMN SIM handling
 * Also exercises string character set conversion
 */
tc_result_t plmn_test_sim_state_ready();
tc_result_t plmn_test_sim_change();
tc_result_t plmn_test_sim_file_change();
tc_result_t plmn_test_sim_file_change_negative();

/* PLMN list search */
tc_result_t plmn_test_mcc_mnc_search();
tc_result_t plmn_test_lac_range_search();
tc_result_t plmn_test_name_search();

/* =============================
 * ETL TESTCASES
 * =============================
 */
tc_result_t etl_reg_status_table_handling();
tc_result_t etl_trigger_level_handling();

/* =============================
 * SIM RAT CONTROL TESTCASES
 * =============================
 */
tc_result_t rat_test_indication();
tc_result_t rat_test_sim_state_changed();

/* ===========================================
 * MESSAGE STRINGS AND REQUEST TABLE PRINTING
 * ===========================================
 */
tc_result_t message_strings();
tc_result_t request_table_print();

/* ====================
 * MODEM INITIALIZATION
 * ====================
 */
tc_result_t set_default_non_volatile_modem_data_positive_1(void);


tc_result_t cn_request_rf_on_negative_3(void);
tc_result_t cn_event_ring_waiting_positive_1(void);
tc_result_t cn_request_reset_modem_with_dump_positive_1(void);
tc_result_t cn_event_signal_info_1(void);
tc_result_t cn_event_supp_svc_notifn_positive_1(void);
tc_result_t cn_event_supp_svc_notifn_negative_1(void);
tc_result_t cn_event_ussd_positive_2(void);
tc_result_t cn_event_ussd_positive_3(void);
tc_result_t cn_event_ussd_positive_4(void);
tc_result_t cn_request_get_neighbour_cells_reporting_positive_1(void);
tc_result_t cn_request_set_neighbour_cells_reporting_positive_1(void);

tc_result_t cn_request_baseband_version_1(void);
tc_result_t cn_request_baseband_version_negative_1(void);
tc_result_t cn_request_set_product_profile_flag_positive(void);
tc_result_t cn_request_set_product_profile_flag_negative(void);

tc_result_t cn_request_set_user_activity_status_positive_1(void);
tc_result_t cn_request_rf_on_negative_cllient_1(void);
tc_result_t cn_event_name_info_positive_convert_postive(void);
tc_result_t cn_event_ss_detailed_gsm_cause_positive_1(void);
tc_result_t cn_event_ss_detailed_gsm_cause_positive_2(void);
tc_result_t cn_request_query_call_barring_positive_1(void);
tc_result_t cn_request_query_call_barring_negative_1(void);
tc_result_t cn_request_set_l1_parameter_positive_1(void);
tc_result_t cn_request_get_modem_property_positive_1(void);
tc_result_t cn_request_get_modem_property_positive_2(void);
tc_result_t cn_request_negative_1(void);
tc_result_t cn_request_get_call_pas_positive_1(void);
tc_result_t cn_request_get_pp_flag_positive(void);
tc_result_t cn_request_get_pp_flag_negative_1(void);


/* =================
 * UTILITY FUNCTIONS
 * =================
 */
void clear_global_stub_variables(void);
void clear_global_sim_stub_variables(void);


/* IMPLEMENTATION NOTE:
 * In an ideal world, the module test framework would have been designed to use real MAL sockets to provoke select() behaviour entirely. However, with the
 * current MAL design, the Call&networking server is responsible for using select() directly against the phonet sockets. Then a MAL function is invoked that
 * will decode the modem signals and itself call an event callback in the Call&Networking code. For legal reasons (and in some degree to reduce the testcase code)
 * the testcases won't use MAL file descriptors and provoke select() because it requires construction of modem signals. Instead the MAL callbacks are called directly
 * (running in the test thread).
 */

pthread_mutex_t select_cb_mutex;
extern cn_bool_t select_cb_finished;

#define WAIT_FOR_SELECT_TO_FINISH() \
    CN_LOG_I("Waiting for select() to finish..."); \
    while(1) { \
        (void)pthread_mutex_lock(&select_cb_mutex); \
        if (select_cb_finished) { \
            CN_LOG_I("select() has finished."); \
            select_cb_finished = 0; \
            (void)pthread_mutex_unlock(&select_cb_mutex); \
            usleep(20000); \
            break; \
         } \
        (void)pthread_mutex_unlock(&select_cb_mutex); \
        usleep(20000); \
    }

cn_error_code_t cn_get_message_queue_size(int fd, cn_uint32_t *size_p);
#define WAIT_FOR_MESSAGE(fd) \
    CN_LOG_I("Waiting for message..."); \
    while(1) { \
        cn_uint32_t _cn_size = 0; \
        (void)cn_get_message_queue_size((fd),&_cn_size); \
        if (_cn_size) { \
            CN_LOG_I("message has arrived."); \
            break; \
         } \
        usleep(20000); \
    }
/* NOTE: We don't need complete message data here, a single byte is enough since
 * cn_message_receive() will take care of that */

#define EVENT_NOT_SENT_THRESHOLD 500000 /* usleep() value, 500ms */

#endif /* __testcases_h__ */
