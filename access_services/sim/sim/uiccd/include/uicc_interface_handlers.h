/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "msgq.h"
#include "catd_modem_types.h"

/* SAP related functions */
int uiccd_main_sim_file_read_generic(ste_msg_t * ste_msg);
int uiccd_main_sim_disconnect(const ste_msg_t * ste_msg);
int uiccd_main_sim_reset(const ste_msg_t *msg);
int uiccd_main_sim_connect(const ste_msg_t * ste_msg);
void uiccd_main_sap_handle_cn_event(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_poweron_card(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_poweroff_card(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_coldreset_card(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_warmreset_card(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_atr_get(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_apdu_data(const ste_msg_t * ste_msg);

int uiccd_main_sap_session_start_response(int status, int client_tag);
int uiccd_main_sap_session_exit_response(int status, int client_tag);
int uiccd_encode_and_send_sap_session_poweroff_card_response(int fd,
                                                             uintptr_t client_tag,
                                                             ste_sim_status_t status);

int uiccd_encode_and_send_sap_session_ctrlcard_reset_response(int fd,
                                                              uintptr_t client_tag,
                                                              ste_sim_status_t status);

int uiccd_encode_and_send_sap_session_atr_get_response(int fd,
                                                       uintptr_t client_tag,
                                                       sim_uicc_status_code_t  uicc_status_code,
                                                       const uint8_t* data,
                                                       size_t data_len );

int uiccd_main_sap_session_atr_get_response(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_ctrlcard_activate_response(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_ctrlcard_deactivate_response(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_ctrlcard_coldreset_response(const ste_msg_t * ste_msg);
int uiccd_encode_and_send_sap_session_apdu_data_response(int fd,
                                                         uintptr_t client_tag,
                                                         sim_uicc_status_code_t  uicc_status_code,
                                                         const uint8_t* data,
                                                         size_t data_len );
int  uiccd_main_sap_session_apdu_data_response(const ste_msg_t * ste_msg);
int uiccd_main_sap_session_status_get_response(int status, int client_tag);
int uiccd_main_sap_session_status_ind(int status);

/* Transaction handlers */
int uiccd_main_smsc_get_record_max(ste_msg_t * ste_msg);
int uiccd_main_update_plmn(ste_msg_t * ste_msg);
int uiccd_main_sim_icon_read(ste_msg_t *ste_msg);
int uiccd_main_read_plmn(ste_msg_t * ste_msg);
int uiccd_main_read_fdn(ste_msg_t *ste_msg);
int uiccd_main_read_ecc(ste_msg_t *ste_msg);
int uiccd_main_sim_channel_send(const ste_msg_t * ste_msg);
int uiccd_main_sim_channel_send_response(ste_msg_t * ste_msg);
int uiccd_main_sim_channel_close(const ste_msg_t * ste_msg);
int uiccd_main_sim_channel_close_response(ste_msg_t * ste_msg);
int uiccd_main_sim_channel_open(const ste_msg_t * ste_msg);
int uiccd_main_sim_channel_open_response(ste_msg_t * ste_msg);
int uiccd_main_read_smsc(ste_msg_t * ste_msg);
int uiccd_main_smsc_restore_from_record(ste_msg_t * ste_msg);
int uiccd_main_smsc_save_to_record(ste_msg_t * ste_msg);
int uiccd_main_update_smsc(ste_msg_t * ste_msg);
int uiccd_main_read_subscriber_number(ste_msg_t *ste_msg);
int uiccd_main_app_status(const ste_msg_t * ste_msg);
int uiccd_main_card_status(ste_msg_t * ste_msg);
void uiccd_main_stop(ste_msg_t * ste_msg);
void uiccd_main_disconnect(ste_msg_t * ste_msg);
void uiccd_main_get_sim_state(ste_msg_t * ste_msg, const ste_sim_state_t  *current_state);
void uiccd_received_startup_complete();
void uiccd_reset_startup_complete();
void uiccd_main_send_state_change_event(const ste_sim_state_t  *current_state, ste_uicc_status_t uicc_status);
int uiccd_main_app_info(const ste_msg_t * ste_msg);
int uiccd_main_sim_status(ste_msg_t * ste_msg);
int uiccd_main_sim_power_on(const ste_msg_t *ste_msg);
int uiccd_main_sim_power_off(const ste_msg_t *ste_msg);
int uiccd_main_sim_power_on_response(ste_msg_t * ste_msg);
int uiccd_main_sim_power_off_response(ste_msg_t * ste_msg);
int uiccd_main_disconnect_uicc_status_ind(uiccd_msg_uicc_status_ind_t *msg);
int uiccd_main_sim_read_preferred_rat_setting(ste_msg_t * ste_msg);
int uiccd_main_sim_read_preferred_rat_setting_response(ste_msg_t * ste_msg);

/* PIN */
int uiccd_main_pin_verify(const ste_msg_t * ste_msg);
int uiccd_main_pin_verify_response(ste_msg_t * ste_msg);
int uiccd_main_pin_enable(const ste_msg_t * ste_msg);
int uiccd_main_pin_enable_response(ste_msg_t * ste_msg);
int uiccd_main_pin_disable(const ste_msg_t * ste_msg);
int uiccd_main_pin_disable_response(ste_msg_t * ste_msg);
int uiccd_main_pin_info(const ste_msg_t * ste_msg);
int uiccd_main_pin_info_response(ste_msg_t * ste_msg);
int uiccd_main_pin_change(ste_msg_t * ste_msg);
int uiccd_main_pin_change_response(ste_msg_t * ste_msg);
int uiccd_main_pin_unblock(const ste_msg_t * ste_msg);
int uiccd_main_pin_unblock_response(ste_msg_t * ste_msg);

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
int uiccd_msr_pin_verify(char * pin_p, uint8_t pin_len);
void uiccd_msr_cache_pin();
#endif
int uiccd_main_modem_silent_reset(const ste_msg_t * ste_msg);

/* SIM I/O */
int uiccd_main_read_sim_file_record(ste_msg_t * ste_msg);
int uiccd_main_read_sim_file_record_response(ste_msg_t * ste_msg);
int uiccd_main_sim_file_get_format(ste_msg_t * ste_msg);
int uiccd_main_sim_file_get_format_response(ste_msg_t * ste_msg);
int uiccd_main_get_file_information(ste_msg_t * ste_msg);
int uiccd_main_get_file_information_response(ste_msg_t * ste_msg);
int uiccd_main_read_sim_file_binary(ste_msg_t * ste_msg);
int uiccd_main_read_sim_file_binary_response(ste_msg_t * ste_msg);
int uiccd_main_update_sim_file_record(ste_msg_t * ste_msg);
int uiccd_main_update_sim_file_record_response(ste_msg_t * ste_msg);
int uiccd_main_update_sim_file_binary(ste_msg_t * ste_msg);
int uiccd_main_update_sim_file_binary_response(ste_msg_t * ste_msg);
int uiccd_main_get_service_table(ste_msg_t * ste_msg);
int uiccd_main_get_service_availability(ste_msg_t * ste_msg);
int uiccd_main_update_service_table(ste_msg_t * ste_msg);

/* Deprecated functions. To be removed or implemented in another way */
void uiccd_main_register(ste_msg_t * ste_msg);
int uiccd_main_appl_apdu_send(ste_msg_t * ste_msg);
int uiccd_main_appl_apdu_send_response(ste_msg_t * ste_msg);
