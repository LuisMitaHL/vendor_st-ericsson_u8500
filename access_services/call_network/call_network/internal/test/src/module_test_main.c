/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "cn_client.h"
#include "cn_log.h"
#include "testcases.h"

extern int kill(pid_t pid, int sig); /* The system call is not defined in standardized POSIX header <signal.h> */

static void determine_length_of_testcase_table(void);
static void print_test_results(void);
static void setup_test_environment(void);
static void clear_message_queues(void);
static void clear_global_pdc_variables();
static void shutdown_test_environment_and_process(void);

/* Global variables */
static int g_nr_of_positive = 0;
static int g_nr_of_negative = 0;
static int g_nr_of_passed = 0;
static int g_nr_of_failed = 0;
static int g_not_implemented = 0;
static int g_total_nr_of_testcases = 0;
int g_request_fd = -1;
int g_event_fd = -1;
cn_context_t *g_context_p = NULL;
cn_bool_t g_tf_pdc_check_enabled = FALSE;

static tc_table_entry_t testcase_table[] = {
    { TESTCASE_TYPE_POSITIVE, "request_table_print_begin",                                                 request_table_print,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "rat_test_sim_state_changed",                                                rat_test_sim_state_changed,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rf_on_positive_1",                                               cn_request_rf_on_positive_1,                                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rf_on_negative_1",                                               cn_request_rf_on_negative_1,                                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rf_on_negative_2",                                               cn_request_rf_on_negative_2,                                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rf_off_positive_1",                                              cn_request_rf_off_positive_1,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rf_off_negative_1",                                              cn_request_rf_off_negative_1,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rf_status_positive_1",                                           cn_request_rf_status_positive_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rf_status_negative_1",                                           cn_request_rf_status_negative_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_registration_control_positive_1",                                cn_request_registration_control_positive_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_registration_control_negative_1",                                cn_request_registration_control_negative_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_modem_reset_positive_1",                                         cn_request_reset_modem_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_modem_reset_negative_1",                                         cn_request_reset_modem_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_preferred_network_type_positive_1",                          cn_request_set_preferred_network_type_positive_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_preferred_network_type_negative_1",                          cn_request_set_preferred_network_type_negative_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_preferred_network_type_negative_2",                          cn_request_set_preferred_network_type_negative_2,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_preferred_network_type_positive_1",                          cn_request_get_preferred_network_type_positive_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_preferred_network_type_negative_1",                          cn_request_get_preferred_network_type_negative_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_registration_state_normal_positive_1",                           cn_request_registration_state_normal_positive_1,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_registration_state_normal_negative_1",                           cn_request_registration_state_normal_negative_1,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_registration_state_gprs_positive_1",                             cn_request_registration_state_gprs_positive_1,                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_registration_state_gprs_negative_1",                             cn_request_registration_state_gprs_negative_1,                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_cell_info_positive_1",                                           cn_request_cell_info_positive_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_cell_info_negative_1",                                           cn_request_cell_info_negative_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_neighbour_cells_complete_info_positive_1",                   cn_request_get_neighbour_cells_complete_info_positive_1,                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_complete_info_negative_1",                   cn_request_get_neighbour_cells_complete_info_negative_1,                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_complete_info_negative_2",                   cn_request_get_neighbour_cells_complete_info_negative_2,                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_neighbour_cells_extd_info_positive_1",                       cn_request_get_neighbour_cells_extd_info_positive_1,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_extd_info_negative_1",                       cn_request_get_neighbour_cells_extd_info_negative_1,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_automatic_network_registration_positive_1",                      cn_request_automatic_network_registration_positive_1,                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_automatic_network_registration_negative_1",                      cn_request_automatic_network_registration_negative_1,                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_neighbour_cells_extd_info_positive_2",                       cn_request_get_neighbour_cells_extd_info_positive_2,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_extd_info_negative_2",                       cn_request_get_neighbour_cells_extd_info_negative_2,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_extd_info_negative_3",                       cn_request_get_neighbour_cells_extd_info_negative_3,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_neighbour_cells_extd_info_negative_4",                       cn_request_get_neighbour_cells_extd_info_negative_4,                       TC_RESULT_FAILED},
    /*
     * Manual network registration:
     * 1:st of two sets of tests. It is intentionally run 3 times in a row to test various conditions
     */
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},

    { TESTCASE_TYPE_NEGATIVE, "cn_request_manual_network_registration_negative_1",                         cn_request_manual_network_registration_negative_1,                         TC_RESULT_FAILED},
    /*
     * Manual network registration with automatic fallback:
     * 1:st of two sets of tests. It is intentionally run 3 times in a row to test various conditions
     */
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},

    { TESTCASE_TYPE_NEGATIVE, "cn_request_manual_network_registration_with_automatic_fallback_negative_1", cn_request_manual_network_registration_with_automatic_fallback_negative_1, TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_manual_network_registration_with_automatic_fallback_negative_2", cn_request_manual_network_registration_with_automatic_fallback_negative_2, TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_network_deregister_positive_1",                                  cn_request_network_deregister_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_network_deregister_negative_1",                                  cn_request_network_deregister_negative_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_net_query_mode_positive_1",                                      cn_request_net_query_mode_positive_1,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_net_query_mode_negative_1",                                      cn_request_net_query_mode_negative_1,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_search_positive_1",                               cn_request_manual_network_search_positive_1,                               TC_RESULT_FAILED},
    /*
     * Manual network registration:
     * 2:nd of two sets of tests. It is intentionally run 3 times in a row to test various conditions
     */
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_positive_1",                         cn_request_manual_network_registration_positive_1,                         TC_RESULT_FAILED},
    /*
     * Manual network registration with automatic fallback:
     * 2:nd of two sets of tests. It is intentionally run 3 times in a row to test various conditions
     */
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_manual_network_registration_with_automatic_fallback_positive_1", cn_request_manual_network_registration_with_automatic_fallback_positive_1, TC_RESULT_FAILED},

    { TESTCASE_TYPE_NEGATIVE, "cn_request_manual_network_search_negative_1",                               cn_request_manual_network_search_negative_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_interrupt_network_search_positive_1",                            cn_request_interrupt_network_search_positive_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_interrupt_network_search_negative_1",                            cn_request_interrupt_network_search_negative_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_current_call_list_positive_1",                                   cn_request_current_call_list_positive_1,                                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_current_call_list_negative_1",                                   cn_request_current_call_list_negative_1,                                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dial_positive_1",                                                cn_request_dial_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dial_positive_2",                                                cn_request_dial_positive_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dial_positive_3",                                                cn_request_dial_positive_3,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dial_positive_4",                                                cn_request_dial_positive_4,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dial_negative_1",                                                cn_request_dial_negative_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dial_negative_2",                                                cn_request_dial_negative_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_hangup_positive_1",                                              cn_request_hangup_positive_1,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_hangup_negative_1",                                              cn_request_hangup_negative_1,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_hangup_negative_2",                                              cn_request_hangup_negative_2,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_hangup_negative_3",                                              cn_request_hangup_negative_3,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_swap_calls_positive_1",                                          cn_request_swap_calls_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_swap_calls_negative_1",                                          cn_request_swap_calls_negative_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_hold_call_positive_1",                                           cn_request_hold_call_positive_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_hold_call_negative_1",                                           cn_request_hold_call_negative_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_resume_call_positive_1",                                         cn_request_resume_call_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_resume_call_negative_1",                                         cn_request_resume_call_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_conference_call_positive_1",                                     cn_request_conference_call_positive_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_conference_call_negative_1",                                     cn_request_conference_call_negative_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_conference_call_split_positive_1",                               cn_request_conference_call_split_positive_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_conference_call_split_negative_1",                               cn_request_conference_call_split_negative_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_explicit_call_transfer_positive_1",                              cn_request_explicit_call_transfer_positive_1,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_explicit_call_transfer_negative_1",                              cn_request_explicit_call_transfer_negative_1,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_answer_call_positive_1",                                         cn_request_answer_call_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_answer_call_negative_1",                                         cn_request_answer_call_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ussd_positive_1",                                                cn_request_ussd_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_ussd_negative_1",                                                cn_request_ussd_negative_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_ussd_negative_2",                                                cn_request_ussd_negative_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ussd_abort_positive_1",                                          cn_request_ussd_abort_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_ussd_abort_negative_1",                                          cn_request_ussd_abort_negative_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rssi_value_positive_1",                                          cn_request_rssi_value_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rssi_value_negative_1",                                          cn_request_rssi_value_negative_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_clip_status_positive_1",                                         cn_request_clip_status_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_clip_status_negative_1",                                         cn_request_clip_status_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_clir_status_positive_1",                                         cn_request_clir_status_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_clir_status_negative_1",                                         cn_request_clir_status_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_cnap_status_positive_1",                                         cn_request_cnap_status_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_cnap_status_negative_1",                                         cn_request_cnap_status_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_colr_status_positive_1",                                         cn_request_colr_status_positive_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_colr_status_negative_1",                                         cn_request_colr_status_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_clir_positive_1",                                            cn_request_set_clir_positive_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_clir_negative_1",                                            cn_request_set_clir_negative_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_sleep_on_positive_1",                                  cn_request_set_modem_sleep_on_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_modem_sleep_on_negative_1",                                  cn_request_set_modem_sleep_on_negative_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_sleep_off_positive_1",                                 cn_request_set_modem_sleep_off_positive_1,                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_modem_sleep_off_negative_1",                                 cn_request_set_modem_sleep_off_negative_1,                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_property_positive_1",                                  cn_request_set_modem_property_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_property_positive_2",                                  cn_request_set_modem_property_positive_2,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_property_positive_3",                                  cn_request_set_modem_property_positive_3,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_modem_property_positive_4",                                  cn_request_set_modem_property_positive_4,                                  TC_RESULT_FAILED},

    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_modem_property_negative_1",                                  cn_request_set_modem_property_negative_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_cssn_positive_1",                                                cn_request_cssn_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_default_nvmd_positive_1",                                    cn_request_set_default_nvmd_positive_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_signal_info_config_positive_1",                              cn_request_set_signal_info_config_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_signal_info_config_1",                                       cn_request_set_signal_info_config_1,                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_signal_info_config_1",                                       cn_request_get_signal_info_config_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_signal_info_reporting_1",                                    cn_request_set_signal_info_reporting_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_signal_info_reporting_1",                                    cn_request_get_signal_info_reporting_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_hsxpa_mode_1",                                               cn_request_set_hsxpa_mode_1,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_hsxpa_mode_1",                                               cn_request_get_hsxpa_mode_1,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_event_reporting_1",                                          cn_request_set_event_reporting_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_event_reporting_1",                                          cn_request_get_event_reporting_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rab_status_1",                                                   cn_request_rab_status_1,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_modem_property_negative_1",                                  cn_request_set_modem_property_negative_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_send_positive_1",                                           cn_request_dtmf_send_positive_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_send_positive_2",                                           cn_request_dtmf_send_positive_2,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_send_positive_3",                                           cn_request_dtmf_send_positive_3,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_send_positive_4",                                           cn_request_dtmf_send_positive_4,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_1",                                           cn_request_dtmf_send_negative_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_2",                                           cn_request_dtmf_send_negative_2,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_3",                                           cn_request_dtmf_send_negative_3,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_4",                                           cn_request_dtmf_send_negative_4,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_5",                                           cn_request_dtmf_send_negative_5,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_send_negative_6",                                           cn_request_dtmf_send_negative_6,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_start_positive_1",                                          cn_request_dtmf_start_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_start_negative_1",                                          cn_request_dtmf_start_negative_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_dtmf_stop_positive_1",                                           cn_request_dtmf_stop_positive_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_dtmf_stop_negative_1",                                           cn_request_dtmf_stop_negative_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_call_waiting_positive_1",                                    cn_request_set_call_waiting_positive_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_call_waiting_negative_1",                                    cn_request_set_call_waiting_negative_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_call_waiting_positive_1",                                    cn_request_get_call_waiting_positive_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_call_waiting_negative_1",                                    cn_request_get_call_waiting_negative_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_call_barring_positive_1",                                    cn_request_set_call_barring_positive_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_call_barring_negative_1",                                    cn_request_set_call_barring_negative_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_call_forward_positive_1",                                    cn_request_set_call_forward_positive_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_call_forward_negative_1",                                    cn_request_set_call_forward_negative_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_emergency_number_list_positive_1",                           cn_request_get_emergency_number_list_positive_1,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_emergency_number_list_positive_2",                           cn_request_get_emergency_number_list_positive_2,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_emergency_number_list_positive_3",                           cn_request_get_emergency_number_list_positive_3,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_emergency_number_list_positive_4",                           cn_request_get_emergency_number_list_positive_4,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_reg_status_event_config_1",                                      cn_request_reg_status_event_config_1,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_reg_status_event_config_2",                                      cn_request_reg_status_event_config_2,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_reg_status_event_config_3",                                      cn_request_reg_status_event_config_3,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_reg_status_event_config_4",                                      cn_request_reg_status_event_config_4,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_reg_status_event_config_5",                                      cn_request_reg_status_event_config_5,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_reg_status_event_config_6",                                      cn_request_reg_status_event_config_6,                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rat_name_positive_1",                                            cn_request_rat_name_positive_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rat_name_positive_2",                                            cn_request_rat_name_positive_2,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rat_name_negative_1",                                            cn_request_rat_name_negative_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_event_rat_name_positive_1",                                              cn_event_rat_name_positive_1,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_event_rat_name_positive_2",                                              cn_event_rat_name_positive_2,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_modem_power_off_positive_1",                                     cn_request_modem_power_off_positive_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_modem_power_off_negative_1",                                     cn_request_modem_power_off_negative_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_radio_status_positive_1",                                          cn_event_radio_status_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_modem_registration_status_positive_1",                             cn_event_modem_registration_status_positive_1,                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_time_info_positive_1",                                             cn_event_time_info_positive_1,                                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_name_info_positive_gsm7",                                          cn_event_name_info_positive_gsm7,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_name_info_positive_ucs2",                                          cn_event_name_info_positive_ucs2,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_net_modem_detailed_fail_cause_positive_1",                         cn_event_net_modem_detailed_fail_cause_positive_1,                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_detailed_fail_cause_positive_1",                                cn_event_ss_detailed_fail_cause_positive_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_detailed_fail_cause_positive_2",                                cn_event_ss_detailed_fail_cause_positive_2,                                TC_RESULT_FAILED},
    //{ TESTCASE_TYPE_NEGATIVE, "cn_event_ss_detailed_fail_cause_negative_1",                                cn_event_ss_detailed_fail_cause_negative_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_neighbour_cells_info_positive_1",                                  cn_event_neighbour_cells_info_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_barring_1",                                                      cn_request_ss_command_barring_1,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_register",                                               cn_request_ss_command_forward_register,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_erasure",                                                cn_request_ss_command_forward_erasure,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_register_2",                                             cn_request_ss_command_forward_register_2,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_register_3",                                             cn_request_ss_command_forward_register_3,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_activation",                                             cn_request_ss_command_forward_activation,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_command_forward_deactivation",                                           cn_request_ss_command_forward_deactivation,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ss_command_forward_error_1",                                     cn_request_ss_command_forward_error_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_status_info_positive_1",                                        cn_event_ss_status_info_positive_1,                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_status_ussd_stop",                                              cn_event_ss_status_ussd_stop,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_status_ussd_failed",                                            cn_event_ss_status_ussd_failed,                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ss_command_forward_query_1",                                     cn_request_ss_command_forward_query_1,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ss_command_forward_query_2",                                     cn_request_ss_command_forward_query_2,                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ussd_positive_1",                                                  cn_event_ussd_positive_1,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_gsm_ussd_req",                                                     cn_event_gsm_ussd_req,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_gsm_ussd_notify",                                                  cn_event_gsm_ussd_notify,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_gsm_ussd_command",                                                 cn_event_gsm_ussd_command,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ring_positive_1",                                                  cn_event_ring_positive_1,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_call_cnap_positive_1",                                             cn_event_call_cnap_positive_1,                                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_call_state_changed_positive_1",                                    cn_event_call_state_changed_positive_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_call_state_changed_positive_2",                                    cn_event_call_state_changed_positive_2,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_call_state_changed_positive_3",                                    cn_event_call_state_changed_positive_3,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_timing_advance",                                             cn_request_get_timing_advance,                                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_timing_advance_fail",                                        cn_request_get_timing_advance_fail,                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cb_request_nmr_geran_info",                                                 cn_request_nmr_geran_info,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cb_request_nmr_utran_info",                                                 cn_request_nmr_utran_info,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_nmr_utran_info_fail",                                            cn_request_nmr_utran_info_fail,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_cell_info_1",                                                      cn_event_cell_info_1,                                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_generate_local_comfort_tones_positive_1",                          cn_event_generate_local_comfort_tones_positive_1,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_generate_local_comfort_tones_positive_2",                          cn_event_generate_local_comfort_tones_positive_2,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_radio_info_positive_1",                                            cn_event_radio_info_positive_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_send_tx_back_off_event_positive_1",                              cn_request_send_tx_back_off_event_positive_1,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_send_tx_back_off_event_positive_2",                              cn_request_send_tx_back_off_event_positive_2,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_send_tx_back_off_event_negative_1",                              cn_request_send_tx_back_off_event_negative_1,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_emergency_number_ind_empty_list",                              cn_event_enl_emergency_number_ind_empty_list,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_emergency_number_ind_populated_list",                          cn_event_enl_emergency_number_ind_populated_list,                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_registration_status_ind_empty_list",                           cn_event_enl_registration_status_ind_empty_list,                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_registration_status_ind_populated_list",                       cn_event_enl_registration_status_ind_populated_list,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_registration_status_response_empty_list",                      cn_event_enl_registration_status_response_empty_list,                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_registration_status_response_populated_list",                  cn_event_enl_registration_status_response_populated_list,                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_enl_mixed_ind_responses_and_indications_populated_list",           cn_event_enl_mixed_ind_responses_and_indications_populated_list,           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_1",                                                     ss_decoder_positive_1,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_2",                                                     ss_decoder_positive_2,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_3",                                                     ss_decoder_positive_3,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_4",                                                     ss_decoder_positive_4,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_5",                                                     ss_decoder_positive_5,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_6",                                                     ss_decoder_positive_6,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_7",                                                     ss_decoder_positive_7,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_8",                                                     ss_decoder_positive_8,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_9",                                                     ss_decoder_positive_9,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_10",                                                    ss_decoder_positive_10,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_11",                                                    ss_decoder_positive_11,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_12",                                                    ss_decoder_positive_12,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_13",                                                    ss_decoder_positive_13,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_decoder_positive_14",                                                    ss_decoder_positive_14,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_decoder_negative_1",                                                     ss_decoder_negative_1,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_decoder_negative_2",                                                     ss_decoder_negative_2,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_decoder_negative_3",                                                     ss_decoder_negative_3,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_decoder_negative_4",                                                     ss_decoder_negative_4,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_decoder_negative_5",                                                     ss_decoder_negative_5,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_1",                                                     ss_encoder_positive_1,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_2",                                                     ss_encoder_positive_2,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_3",                                                     ss_encoder_positive_3,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_4",                                                     ss_encoder_positive_4,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_5",                                                     ss_encoder_positive_5,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_6",                                                     ss_encoder_positive_6,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_7",                                                     ss_encoder_positive_7,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_8",                                                     ss_encoder_positive_8,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_9",                                                     ss_encoder_positive_9,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_10",                                                    ss_encoder_positive_10,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_11",                                                    ss_encoder_positive_11,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "ss_encoder_positive_12",                                                    ss_encoder_positive_12,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_encoder_negative_1",                                                     ss_encoder_negative_1,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_encoder_negative_2",                                                     ss_encoder_negative_2,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "ss_encoder_negative_3",                                                     ss_encoder_negative_3,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_ecc_negative_1",                                                        pdc_ecc_negative_1,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_ecc_negative_2",                                                        pdc_ecc_negative_2,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_ecc_negative_3",                                                        pdc_ecc_negative_3,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_default_positive_1",                                                pdc_ecc_default_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_default_positive_2",                                                pdc_ecc_default_positive_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_simlist_positive_1",                                                pdc_ecc_simlist_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_simlist_positive_2",                                                pdc_ecc_simlist_positive_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_filelist_positive_1",                                               pdc_ecc_filelist_positive_1,                                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_postsat_positive_1",                                                pdc_ecc_postsat_positive_1,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_postsat_positive_2",                                                pdc_ecc_postsat_positive_2,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_postsat_positive_3",                                                pdc_ecc_postsat_positive_3,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_all_positive_1",                                                    pdc_ecc_all_positive_1,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_all_positive_2",                                                    pdc_ecc_all_positive_2,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_pdc_set_emergency_number_filepath_postive_1",                            cn_pdc_set_emergency_number_filepath_postive_1,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "util_continue_as_privileged_user_postive_1",                                util_continue_as_privileged_user_postive_1,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_ecc_all_positive_3",                                                    pdc_ecc_all_positive_3,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_sat_voice_1",                                                           pdc_sat_voice_1,                                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_sat_ss_1",                                                              pdc_sat_ss_1,                                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_sat_ss_2",                                                              pdc_sat_ss_2,                                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_sat_ussd_1",                                                            pdc_sat_ussd_1,                                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_sat_no_app_1",                                                          pdc_sat_no_app_1,                                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_1",                                                        pdc_sat_negative_1,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_2",                                                        pdc_sat_negative_2,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_3",                                                        pdc_sat_negative_3,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_4",                                                        pdc_sat_negative_4,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_5",                                                        pdc_sat_negative_5,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_sat_negative_6",                                                        pdc_sat_negative_6,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_make_ss_request",                                                       pdc_make_ss_request,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_make_ss_request_negative_1",                                            pdc_make_ss_request_negative_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_make_ss_request_negative_2",                                            pdc_make_ss_request_negative_2,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_make_ussd_request",                                                     pdc_make_ussd_request,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_voice_call_failure_1",                                                  pdc_voice_call_failure_1,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_voice_call_accept_without_modification_1",                              pdc_voice_call_accept_without_modification_1,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_voice_call_accept_with_modification_1",                                 pdc_voice_call_accept_with_modification_1,                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_number_comparison_positive_1",                                          pdc_number_comparison_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_number_comparison_negative_1",                                          pdc_number_comparison_negative_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_fdn_negative_1",                                                        pdc_fdn_negative_1,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_fdn_negative_2",                                                        pdc_fdn_negative_2,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_fdn_negative_3",                                                        pdc_fdn_negative_3,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "pdc_fdn_negative_4",                                                        pdc_fdn_negative_4,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_fdn_positive_1",                                                        pdc_fdn_positive_1,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_fdn_positive_2",                                                        pdc_fdn_positive_2,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_fdn_positive_3",                                                        pdc_fdn_positive_3,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_fdn_positive_4",                                                        pdc_fdn_positive_4,                                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_fdn_negative_24",                                                       pdc_fdn_negative_24,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_1",                                            pdc_msg_translation_positive_1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_2",                                            pdc_msg_translation_positive_2,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_3",                                            pdc_msg_translation_positive_3,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_4",                                            pdc_msg_translation_positive_4,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_5",                                            pdc_msg_translation_positive_5,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_6",                                            pdc_msg_translation_positive_6,                                            TC_RESULT_FAILED},
    //{ TESTCASE_TYPE_POSITIVE, "pdc_msg_translation_positive_7",                                          pdc_msg_translation_positive_7,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_reset",                                                           plmn_test_reset,                                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "set_default_non_volatile_modem_data_positive_1",                            set_default_non_volatile_modem_data_positive_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_list_populate",                                                   plmn_test_list_populate,                                                   TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_list_repopulate",                                                 plmn_test_list_repopulate,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_list_populate_negative",                                          plmn_test_list_populate_negative,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_file_read_negative",                                              plmn_test_file_read_negative,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_file_dump",                                                       plmn_test_file_dump,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_file_read",                                                       plmn_test_file_read,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_nitz_update",                                                     plmn_test_nitz_update,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_sim_state_ready",                                                 plmn_test_sim_state_ready,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_sim_change",                                                      plmn_test_sim_change,                                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_sim_file_change",                                                 plmn_test_sim_file_change,                                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "plmn_test_sim_file_change_negative",                                        plmn_test_sim_file_change_negative,                                        TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_mcc_mnc_search",                                                  plmn_test_mcc_mnc_search,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_lac_range_search",                                                plmn_test_lac_range_search,                                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_name_search",                                                     plmn_test_name_search,                                                     TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "plmn_test_reset",                                                           plmn_test_reset,                                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "etl_reg_status_table_handling",                                             etl_reg_status_table_handling,                                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "etl_trigger_level_handling",                                                etl_trigger_level_handling,                                                TC_RESULT_FAILED},
#ifdef ENABLE_RAT_BALANCING
    { TESTCASE_TYPE_POSITIVE, "rat_test_indication",                                                       rat_test_indication,                                                       TC_RESULT_FAILED},
#endif
    { TESTCASE_TYPE_POSITIVE, "message_strings",                                                           message_strings,                                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_oneshot",                                                    cn_request_ftd_oneshot,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_error_measurements - measurement",                       cn_event_ftd_issue_error_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_start_periodic, page 1",                                     cn_request_ftd_start_periodic1,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_stop_periodic",                                              cn_request_ftd_stop_periodic,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_empage_deactivate_success",                              cn_event_ftd_issue_empage_deactivate_success,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_reset",                                                      cn_request_ftd_reset,                                                      TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_start_periodic, page 2",                                     cn_request_ftd_start_periodic2,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_stop_periodic",                                              cn_request_ftd_stop_periodic,                                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_empage_deactivate_success",                              cn_event_ftd_issue_empage_deactivate_success,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_oneshot",                                                    cn_request_ftd_oneshot,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_event_ftd_issue_empage_activate_failure",                                cn_event_ftd_issue_empage_activate_failure,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_event_ftd_issue_empage_deactivate_failure",                              cn_event_ftd_issue_empage_deactivate_failure,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_start_periodic, not finalized",                              cn_request_ftd_start_periodic_not_finish,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_ftd_stop_periodic prior to finalizing start",                    cn_request_ftd_stop_periodic_err,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_measurements - measurement",                             cn_event_ftd_issue_measurements,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_ftd_stop_periodic, not finalized",                               cn_request_ftd_stop_periodic_not_finish,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_ftd_start_periodic prior to finalizing stop",                    cn_request_ftd_start_periodic_err,                                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ftd_issue_empage_deactivate_success",                              cn_event_ftd_issue_empage_deactivate_success,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ring_waiting_positive_1",                                          cn_event_ring_waiting_positive_1,                                          TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_rf_on_negative_3",                                               cn_request_rf_on_negative_3,                                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_reset_modem_with_dump_positive_1",                               cn_request_reset_modem_with_dump_positive_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_signal_info_1",                                                    cn_event_signal_info_1,                                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_supp_svc_notifn_positive_1",                                       cn_event_supp_svc_notifn_positive_1,                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_supp_svc_notifn_negative_1",                                       cn_event_supp_svc_notifn_negative_1,                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ussd_positive_2",                                                  cn_event_ussd_positive_2,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ussd_positive_3",                                                  cn_event_ussd_positive_3,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ussd_positive_4",                                                  cn_event_ussd_positive_4,                                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_neighbour_cells_reporting_positive_1",                       cn_request_get_neighbour_cells_reporting_positive_1,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_neighbour_cells_reporting_positive_1",                       cn_request_set_neighbour_cells_reporting_positive_1,                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_baseband_version_1",                                             cn_request_baseband_version_1,                                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_baseband_version_negative_1",                                    cn_request_baseband_version_negative_1,                                    TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_product_profile_flag_positive",                              cn_request_set_product_profile_flag_positive,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_product_profile_flag_negative",                              cn_request_set_product_profile_flag_negative,                              TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_set_user_activity_status_positive_1",                            cn_request_set_user_activity_status_positive_1,                            TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_rf_on_negative_cllient_1",                                       cn_request_rf_on_negative_cllient_1,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_name_info_positive_convert_postive",                               cn_event_name_info_positive_convert_postive,                               TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_detailed_gsm_cause_positive_1",                                 cn_event_ss_detailed_gsm_cause_positive_1,                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_event_ss_detailed_gsm_cause_positive_2",                                 cn_event_ss_detailed_gsm_cause_positive_2,                             TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_query_call_barring_positive_1",                                  cn_request_query_call_barring_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_query_call_barring_negative_1",                                  cn_request_query_call_barring_negative_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_set_l1_parameter_positive_1",                                    cn_request_set_l1_parameter_positive_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_modem_property_positive_1",                                  cn_request_get_modem_property_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_call_pas_positive_1",                                        cn_request_get_call_pas_positive_1,                                TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_negative_1",                                                     cn_request_negative_1,                                 TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_pp_flag_positive",                                           cn_request_get_pp_flag_positive,                                           TC_RESULT_FAILED},
    { TESTCASE_TYPE_NEGATIVE, "cn_request_get_pp_flag_negative_1",                                         cn_request_get_pp_flag_negative_1,                                         TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_modem_property_positive_1",                                  cn_request_get_modem_property_positive_1,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "cn_request_get_modem_property_positive_2",                                  cn_request_get_modem_property_positive_2,                                  TC_RESULT_FAILED},
    { TESTCASE_TYPE_POSITIVE, "request_table_print_end", /* this should be the last test case */                   request_table_print,                                                       TC_RESULT_FAILED},
    { TESTCASE_TYPE_UNKNOWN, NULL, NULL, TC_RESULT_FAILED}, /* Table termination entry. Used by determine_length_of_testcase_table() */
};

void *module_test_main(void *data_p)
{
    int i = 0;
    (void)data_p;

    CN_LOG_D("enter");

    /* Give the main thread time to setup sockets and go into select() */
    for (i = 3; i > 0; i--) {
        printf("\nstarting module tests in %d\n", i);
        sleep(1);
    }

    setup_test_environment();

    determine_length_of_testcase_table();

    for (i = 0; i < g_total_nr_of_testcases; i++) {

        /* Ensure that testcases are not affecting each other by cleaning up *
         * resources prior to the execution of each testcase.                */
        select_cb_finished = FALSE;
        clear_message_queues();
        clear_global_stub_variables();
        clear_global_pdc_variables();
        clear_global_sim_stub_variables();

        printf("\nEXECUTING TESTCASE #%03d: %s\n", i, testcase_table[i].name_p);
        testcase_table[i].result = testcase_table[i].tc_func_p();

        switch (testcase_table[i].type) {
        case TESTCASE_TYPE_POSITIVE:
            g_nr_of_positive++;
            break;
        case TESTCASE_TYPE_NEGATIVE:
            g_nr_of_negative++;
            break;
        default:
            printf("Illegal testcase type, aborting!\n");
            goto exit;
        }

        switch (testcase_table[i].result) {
        case TC_RESULT_OK:
            printf("RESULT: OK\n\n");
            g_nr_of_passed++;
            break;
        case TC_RESULT_NOT_IMPLEMENTED:
            printf("RESULT: NOT IMPLEMENTED\n\n");
            g_not_implemented++;
            break;
        case TC_RESULT_FAILED:
        default:
            printf("RESULT: FAILED\n\n");
            g_nr_of_failed++;
            break;
        }
    } /* end of for loop */

    print_test_results();

exit:
    shutdown_test_environment_and_process();
    return NULL;
}

static void determine_length_of_testcase_table()
{
    int i = 0;

    while (testcase_table[i].name_p != NULL) {
        i++;
    }

    g_total_nr_of_testcases = i;
}

static const char *get_tc_result_string(tc_result_t result)
{
    switch (result) {
    case TC_RESULT_OK:
        return "ok";
    case TC_RESULT_NOT_IMPLEMENTED:
        return "not_implemented";
    case TC_RESULT_FAILED:
    default:
        return "failed";
    }
}

static void print_test_results()
{
    int i = 0;
    printf("\n\n");
    printf("=============================\n");
    printf("Test results:\n");
    printf("=============================\n");
    printf("\n");
    printf("TESTCASE RESULTS:\n");

    for (i = 0; i < g_total_nr_of_testcases; i++) {
        printf("  #%03d, %s: %s\n", i, testcase_table[i].name_p, get_tc_result_string(testcase_table[i].result));
    }

    printf("\n");
    printf("TESTCASE SUMMARY - Call & Networking Module:\n");
    printf("  total number of testcases: %d\n", g_total_nr_of_testcases);
    printf("  passed: %d\n", g_nr_of_passed);
    printf("  failed: %d\n", g_nr_of_failed);
    printf("  not implemented: %d\n", g_not_implemented);
    printf("\n");
    printf("  positive testcases: %d\n", g_nr_of_positive);
    printf("  negative testcases: %d\n", g_nr_of_negative);

    printf("\n");
}

static void setup_test_environment()
{
    cn_error_code_t result = 0;

    CN_LOG_D("enter");

    /* Reset global variables */
    g_nr_of_positive = 0;
    g_nr_of_negative = 0;
    g_nr_of_passed = 0;
    g_nr_of_failed = 0;
    g_total_nr_of_testcases = 0;
    g_request_fd = -1;
    g_event_fd = -1;

    /* Setup socket connections */
    result = cn_client_init(&g_context_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_init failed!");
        goto error;
    }

    result = cn_client_get_request_fd(g_context_p, &g_request_fd);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_get_request_fd failed!");
        goto error;
    }

    result = cn_client_get_event_fd(g_context_p, &g_event_fd);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_get_event_fd failed!");
        goto error;
    }

    /* Let the other thread prepare for client requests and underlying signals */
    sleep(1);

error:
    return;
}

static void clear_message_queues()
{
    (void)cn_clear_message_queue(g_request_fd);
    (void)cn_clear_message_queue(g_event_fd);
}

static void clear_global_pdc_variables()
{
    g_tf_pdc_check_enabled = FALSE;
}

static void shutdown_test_environment_and_process()
{
    CN_LOG_D("enter");
    (void)cn_client_shutdown(g_context_p);

    sleep(1); /* give the CNS thread time to cleanup resources (in fdmon) before sending SIGTERM. */

    /* This thread will exit automatically but the main thread will not. But it is listening on process signals.
     * In order to shutdown the process we send SIGTERM to ourselves.
     */
    printf("sending SIGTERM to close this process (pid:%d)\n", getpid());
    kill(getpid(), SIGTERM);
}


