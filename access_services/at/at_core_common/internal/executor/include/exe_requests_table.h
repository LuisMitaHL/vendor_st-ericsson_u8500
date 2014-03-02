/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_REQUESTS_TABLE_H
#define EXE_REQUESTS_TABLE_H 1

#include "exe.h"

#ifdef EXE_USE_AUDIO_SERVICE
#include "exe_audio_requests.h"
#endif

#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_requests.h"
#endif

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_requests.h"
#endif

#ifdef EXE_USE_SMS_SERVICE
#include "exe_sms_requests.h"
#endif

#ifdef EXE_USE_PSCC_SERVICE
#include "exe_pscc_requests.h"
#endif

#ifdef EXE_USE_CSPSA_SERVICE
#include "exe_cspsa_requests.h"
#endif

#ifdef EXE_USE_SIM_SERVICE
#include "exe_sim_requests.h"
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <exe_extended_sim_requests.h>
#endif
#endif

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <exe_extended_requests.h>
#endif

#include "exe_requests.h"

typedef struct {
    exe_request_id_t    request_id;
    exe_request_t       request;
    exe_service_t       service;
    const char         *description;
} exe_request_table_t;

exe_request_table_t exe_request_table[] = {
#ifdef EXE_USE_AUDIO_SERVICE
    {EXE_CMUT_READ,                 request_mute_read,                      EXE_SERVICE_AUDIO,  "EXE_CMUT_READ"},
    {EXE_CMUT_SET,                  request_mute_set,                       EXE_SERVICE_AUDIO,  "EXE_CMUT_SET"},
    {EXE_ETTY_GET,                  request_get_tty_mode,                   EXE_SERVICE_AUDIO,  "EXE_ETTY_GET"},
    {EXE_ETTY_SET,                  request_set_tty_mode,                   EXE_SERVICE_AUDIO,  "EXE_ETTY_SET"},
#endif
#ifdef EXE_USE_CN_SERVICE
    {EXE_A,                         request_answer,                         EXE_SERVICE_CN,     "EXE_A"},
    {EXE_CCFC,                      request_call_forwarding,                EXE_SERVICE_CN,     "EXE_CCFC"},
    {EXE_CCWA_SET,                  request_set_call_waiting,               EXE_SERVICE_CN,     "EXE_CCWA_SET"},
    {EXE_CCWA_QUERY,                request_query_call_waiting_status,      EXE_SERVICE_CN,     "EXE_CCWA_QUERY"},
    {EXE_CEER,                      request_ceer,                           EXE_SERVICE_CN,     "EXE_CEER"},
    {EXE_CFUN_RADIO_ON,             request_radio_on,                       EXE_SERVICE_CN,     "EXE_CFUN_RADIO_ON"},
    {EXE_CFUN_RADIO_OFF,            request_radio_off,                      EXE_SERVICE_CN,     "EXE_CFUN_RADIO_OFF"},
    {EXE_CFUN_MODEM_SHUTDOWN,       request_modem_shutdown,                 EXE_SERVICE_CN,     "EXE_CFUN_MODEM_SHUTDOWN"},
    {EXE_CFUN_SET_NW,               request_preferred_network,              EXE_SERVICE_CN,     "EXE_CFUN_SET_NW"},
    {EXE_CFUN_READ,                 request_radio_status,                   EXE_SERVICE_CN,     "EXE_CFUN_READ"},
    {EXE_CGREG_READ,                request_gprs_registration_state_read,   EXE_SERVICE_CN,     "EXE_CGREG_READ"},
    {EXE_CGREG_SET,                 request_gprs_registration_state_set,    EXE_SERVICE_CN,     "EXE_CGREG_SET"},
    {EXE_CHLD,                      request_call_hold,                      EXE_SERVICE_CN,     "EXE_CHLD"},
    {EXE_CLCC,                      request_get_current_calls,              EXE_SERVICE_CN,     "EXE_CLCC"},
    {EXE_CLCK_BARRING,              request_set_query_barring,              EXE_SERVICE_CN,     "EXE_CLCK_BARRING"},
    {EXE_CLIP_READ,                 request_clip_read,                      EXE_SERVICE_CN,     "EXE_CLIP_READ"},
    {EXE_CLIR,                      request_clir_set,                       EXE_SERVICE_CN,     "EXE_CLIR"},
    {EXE_CLIR_READ,                 request_clir_read,                      EXE_SERVICE_CN,     "EXE_CLIR_READ"},
    {EXE_CNAP_READ,                 request_cnap_read,                      EXE_SERVICE_CN,     "EXE_CNAP_READ"},
    {EXE_COLR_DO,                   request_colr_do,                        EXE_SERVICE_CN,     "EXE_COLR_DO"},
    {EXE_COPS_AUTOMATIC_REGISTER,   request_network_automatic_register,     EXE_SERVICE_CN,     "EXE_COPS_AUTOMATIC_REGISTER"},
    {EXE_COPS_MANUAL_REGISTER,      request_network_manual_register,        EXE_SERVICE_CN,     "EXE_COPS_MANUAL_REGISTER"},
    {EXE_COPS_DEREGISTER,           request_network_deregister,             EXE_SERVICE_CN,     "EXE_COPS_DEREGISTER"},
    {EXE_COPS_GET_SETTINGS,         request_network_get_settings,           EXE_SERVICE_CN,     "EXE_COPS_GET_SETTINGS"},
    {EXE_COPS_SEARCH_NETWORKS,      request_network_search,                 EXE_SERVICE_CN,     "EXE_COPS_SEARCH_NETWORKS"},
    {EXE_COPS_MANUAL_REGISTER_AUTO_FALLBACK, request_network_manual_register_auto_fallback, EXE_SERVICE_CN, "EXE_COPS_MANUAL_REGISTER_AUTO_FALLBACK"},
    {EXE_CPWD_BARRING,              request_change_barring_password,        EXE_SERVICE_CN,     "EXE_CPWD_BARRING"},
    {EXE_CREG_READ,                 request_creg_read,                      EXE_SERVICE_CN,     "EXE_CREG_READ"},
    {EXE_CREG_SET,                  request_creg_set,                       EXE_SERVICE_CN,     "EXE_CREG_SET"},
    {EXE_CSQ,                       request_signal_strength,                EXE_SERVICE_CN,     "EXE_CSQ"},
    {EXE_CSSN,                      request_set_supp_svc_notification,      EXE_SERVICE_CN,     "EXE_CSSN"},
    {EXE_CUSD_REQUEST_USSD,         request_ussd_send,                      EXE_SERVICE_CN,     "EXE_CUSD_REQUEST_USSD"},
    {EXE_CUSD_CANCEL_USSD,          request_ussd_cancel,                    EXE_SERVICE_CN,     "EXE_CUSD_CANCEL_USSD"},
    {EXE_DIAL_UP,                   request_call_dial_up,                   EXE_SERVICE_CN,     "EXE_DIAL_UP"},
    {EXE_ECRAT_SET,                 request_rat_name_ind_reporting,         EXE_SERVICE_CN,     "EXE_ECRAT_SET"},
    {EXE_ECRAT_READ,                request_rat_name,                       EXE_SERVICE_CN,     "EXE_ECRAT_READ"},
    {EXE_ENNIR_SET,                 request_nitz_name_ind_reporting,       EXE_SERVICE_CN,     "EXE_ENNIR_SET"},
    {EXE_EHSTACT,                   request_user_activity_set,              EXE_SERVICE_CN,     "EXE_EHSTACT"},
    {EXE_EMPAGE_SET,                request_set_empage,                     EXE_SERVICE_CN,     "EXE_EMPAGE_SET"},
    {EXE_EREG_READ,                 request_ereg_read,                      EXE_SERVICE_CN,     "EXE_EREG_READ"},
    {EXE_EREG_SET,                  request_ereg_set,                       EXE_SERVICE_CN,     "EXE_EREG_SET"},
    {EXE_EPWRRED_SET,               request_send_tx_back_off_event,         EXE_SERVICE_CN,     "EXE_EPWRRED_SET"},
    {EXE_ESVN_READ,                 request_read_imeisv,                    EXE_SERVICE_CN,     "EXE_ESVN_READ"},
    {EXE_HOOK,                      request_hangup,                         EXE_SERVICE_CN,     "EXE_HOOK"},
    {EXE_HOOK_ALL,                  request_hangup_all,                     EXE_SERVICE_CN,     "EXE_HOOK_ALL"},
    {EXE_MODEM_ESLEEP,              request_modem_sleep,                    EXE_SERVICE_CN,     "EXE_MODEM_ESLEEP"},
    {EXE_SET_DTMF_DURATION,         request_set_dtmf_duration,              EXE_SERVICE_CN,     "EXE_SET_DTMF_DURATION"},
    {EXE_GET_DTMF_DURATION,         request_get_dtmf_duration,              EXE_SERVICE_CN,     "EXE_GET_DTMF_DURATION"},
    {EXE_VTS,                       request_dtmf_send,                      EXE_SERVICE_CN,     "EXE_VTS"},
    {EXE_NEIGH_CELLINFO_2G_GET,     request_neighbour_2g_cellinfo,          EXE_SERVICE_CN,     "EXE_NEIGH_CELLINFO_2G_GET"},
    {EXE_NEIGH_CELLINFO_3G_GET,     request_neighbour_3g_cellinfo,          EXE_SERVICE_CN,     "EXE_NEIGH_CELLINFO_3G_GET"},
    {EXE_SERVING_CELLINFO_2G_GET,   request_serving_2g_cellinfo,            EXE_SERVICE_CN,     "EXE_SERVING_CELLINFO_2G_GET"},
    {EXE_SERVING_CELLINFO_3G_GET,   request_serving_3g_cellinfo,            EXE_SERVICE_CN,     "EXE_SERVING_CELLINFO_3G_GET"},
    {EXE_CMER,                      request_signal_info_reporting_set,      EXE_SERVICE_CN,     "EXE_CMER"},
    {EXE_ELIN_READ,                 request_als_read,                       EXE_SERVICE_CN,     "EXE_ELIN_READ"},
    {EXE_ELIN_SET,                  request_als_set,                        EXE_SERVICE_CN,     "EXE_ELIN_SET"},
    {EXE_EMSR,                      request_modem_reset,                    EXE_SERVICE_CN,     "EXE_EMSR"},
#endif
#ifdef EXE_USE_COPS_SERVICE
    {EXE_CGSN,                      request_imei,                           EXE_SERVICE_COPS,   "EXE_CGSN"},
    {EXE_CLCK_PN,                   request_set_query_net_lock,             EXE_SERVICE_COPS,   "EXE_CLCK_PN"},
    {EXE_CLCK_PU,                   request_set_query_net_sub_lock,         EXE_SERVICE_COPS,   "EXE_CLCK_PU"},
    {EXE_CLCK_PP,                   request_set_query_service_lock,         EXE_SERVICE_COPS,   "EXE_CLCK_PP"},
    {EXE_CLCK_PC,                   request_set_query_corporate_lock,       EXE_SERVICE_COPS,   "EXE_CLCK_PC"},
    {EXE_CLCK_EL,                   request_set_query_extended_lock,        EXE_SERVICE_COPS,   "EXE_CLCK_EL"},
    {EXE_GET_CHALLENGE_DATA,        request_get_challenge_data,             EXE_SERVICE_COPS,   "EXE_GET_CHALLENGE_DATA"},
    {EXE_COPS_AUTHENTICATE,         request_authenticate,                   EXE_SERVICE_COPS,   "EXE_COPS_AUTHENTICATE"},
    {EXE_COPS_DEAUTHENTICATE,       request_deauthenticate,                 EXE_SERVICE_COPS,   "EXE_COPS_DEAUTHENTICATE"},
    {EXE_COPS_SET_IMEI,             request_set_imei,                       EXE_SERVICE_COPS,   "EXE_COPS_SET_IMEI"},
    {EXE_COPS_SET_SIMLOCK,          request_set_simlock_keys,               EXE_SERVICE_COPS,   "EXE_COPS_SET_SIMLOCK"},
    {EXE_COPS_WRITE_SL_DATA,        request_write_simlock_data,             EXE_SERVICE_COPS,   "EXE_COPS_WRITE_SL_DATA"},
    {EXE_COPS_READ_SL_DATA,         request_read_simlock_data,              EXE_SERVICE_COPS,   "EXE_COPS_READ_SL_DATA"},
    {EXE_COPS_UNLOCK_SIM_LOCK,      request_unlock_sim_lock,                EXE_SERVICE_COPS,   "EXE_COPS_UNLOCK_SIM_LOCK"},
    {EXE_COPS_AUTH_ENG_MODE,        request_authenticate_eng_mode,          EXE_SERVICE_COPS,   "EXE_COPS_AUTH_ENG_MODE"},
    {EXE_COPS_DEAUTH_ENG_MODE,      request_deauthenticate_eng_mode,        EXE_SERVICE_COPS,   "EXE_COPS_DEAUTH_ENG_MODE"},
    {EXE_COPS_LOCK_BOOTPARTITION,   request_lock_bootpartition,             EXE_SERVICE_COPS,   "EXE_COPS_LOCK_BOOTPARTITION"},
    {EXE_COPS_INIT_ANTIROLLBACK,    request_init_antirollback,              EXE_SERVICE_COPS,   "EXE_COPS_INIT_ANTIROLLBACK"},
    {EXE_COPS_WRITE_D_AND_T,        request_write_d_and_t,                  EXE_SERVICE_COPS,   "EXE_COPS_WRITE_D_AND_T"},
#endif
#ifdef EXE_USE_SIM_SERVICE
    {EXE_CIMI,                      request_get_imsi,                       EXE_SERVICE_SIM,    "EXE_CIMI"},
    {EXE_CLCK_FD,                   request_fixed_dialing,                  EXE_SERVICE_SIM,    "EXE_CLCK_FD"},
    {EXE_CLCK_SC,                   request_sim_lock,                       EXE_SERVICE_SIM,    "EXE_CLCK_SC"},
    {EXE_CPIN_GET_SIM_STATE,        request_get_sim_state_cpin,             EXE_SERVICE_SIM,    "EXE_CPIN_GET_SIM_STATE"},
    {EXE_CPOL_SET,                  request_set_cpol_data,                  EXE_SERVICE_SIM,    "EXE_CPOL_SET"},
    {EXE_CPOL_READ,                 request_get_cpol_data,                  EXE_SERVICE_SIM,    "EXE_CPOL_READ"},
    {EXE_CPOL_TEST,                 request_cpol_data_test,                 EXE_SERVICE_SIM,    "EXE_CPOL_TEST"},
    {EXE_ESIMSR_GET_STATE,          request_get_sim_state,                  EXE_SERVICE_SIM,    "EXE_ESIMSR_GET_STATE"},
    {EXE_CPIN_VERIFY_SIM_PIN2,      request_verify_sim_pin2,                EXE_SERVICE_SIM,    "EXE_CPIN_VERIFY_SIM_PIN2"},
    {EXE_CPIN_VERIFY_SIM_PIN,       request_verify_sim_pin,                 EXE_SERVICE_SIM,    "EXE_CPIN_VERIFY_SIM_PIN"},
    {EXE_CPIN_VERIFY_SIM_PUK2,      request_verify_sim_puk2,                EXE_SERVICE_SIM,    "EXE_CPIN_VERIFY_SIM_PUK2"},
    {EXE_CPIN_VERIFY_SIM_PUK,       request_verify_sim_puk,                 EXE_SERVICE_SIM,    "EXE_CPIN_VERIFY_SIM_PUK"},
    {EXE_CPWD_P2,                   request_sim_pin_change,                 EXE_SERVICE_SIM,    "EXE_CPWD_P2"},
    {EXE_CPWD_SC,                   request_sim_pin_change,                 EXE_SERVICE_SIM,    "EXE_CPWD_SC"},
    {EXE_CRSM_SIM_IO_READ_BINARY,   request_sim_io_read_binary,             EXE_SERVICE_SIM,    "EXE_CRSM_SIM_IO_READ_BINARY"},
    {EXE_CRSM_SIM_IO_READ_RECORD,   request_sim_io_read_record,             EXE_SERVICE_SIM,    "EXE_CRSM_SIM_IO_READ_RECORD"},
    {EXE_CRSM_SIM_IO_UPDATE_BINARY, request_sim_io_update_binary,           EXE_SERVICE_SIM,    "EXE_CRSM_SIM_IO_UPDATE_BINARY"},
    {EXE_CRSM_CHECK_SERVICE_SIM_IO_READ_BINARY, request_sim_io_service_availability_read_binary, EXE_SERVICE_SIM,  " EXE_CRSM_CHECK_SERVICE_SIM_IO_READ_BINARY"},
    {EXE_CRSM_SIM_IO_UPDATE_RECORD, request_sim_io_update_record,           EXE_SERVICE_SIM,    "EXE_CRSM_SIM_IO_UPDATE_RECORD"},
    {EXE_CRSM_SIM_IO_GET_RESPONSE,  request_sim_io_file_information_command, EXE_SERVICE_SIM,   "EXE_CRSM_SIM_IO_GET_RESPONSE"},
    {EXE_CRSM_SIM_IO_STATUS,        request_sim_io_file_information_command, EXE_SERVICE_SIM,   "EXE_CRSM_SIM_IO_STATUS"},
    {EXE_CUAD_APPS,                 request_get_sim_apps_info,              EXE_SERVICE_SIM,    "EXE_CUAD_APPS"},
    {EXE_STKE,                      request_sat_envelope_command,           EXE_SERVICE_SIM,    "EXE_STKE"},
    {EXE_STKC_PC_CONTROL,           request_stkc_pc_control,                EXE_SERVICE_SIM,    "EXE_STKC_PC_CONTROL"},
    {EXE_GET_TERMPROFILE,           request_get_terminal_profile,           EXE_SERVICE_SIM,    "EXE_GET_TERMPROFILE"},
    {EXE_PROACTIVE_COMMAND_RESPONSE, request_send_terminal_response,        EXE_SERVICE_SIM,    "EXE_PROACTIVE_COMMAND_RESPONSE"},
    {EXE_CNUM,                      request_cnum_subscriber_number,         EXE_SERVICE_SIM,    "EXE_CNUM"},
    {EXE_CSCA_SET_SMSC,             request_sim_set_smsc,                   EXE_SERVICE_SIM,    "EXE_CSCA_SET_SMSC"},
    {EXE_CSCA_GET_SMSC,             request_sim_get_smsc,                   EXE_SERVICE_SIM,    "EXE_CSCA_GET_SMSC"},
    {EXE_CRES,                      request_restore_smsc_profile,           EXE_SERVICE_SIM,    "EXE_CRES"},
    {EXE_CRES_TEST,                 request_smsc_profiles_max,              EXE_SERVICE_SIM,    "EXE_CRES_TEST"},
    {EXE_CSAS,                      request_save_smcs_to_profile,           EXE_SERVICE_SIM,    "EXE_CSAS"},
    {EXE_CSAS_TEST,                 request_smsc_profiles_max,              EXE_SERVICE_SIM,    "EXE_CSAS_TEST"},
    {EXE_CSIM,                      request_generic_sim_access,             EXE_SERVICE_SIM,    "EXE_CSIM"},
    {EXE_CCHO,                      request_open_channel,                   EXE_SERVICE_SIM,    "EXE_CCHO"},
    {EXE_CCHC,                      request_close_channel,                  EXE_SERVICE_SIM,    "EXE_CCHC"},
    {EXE_CGLA,                      request_generic_sim_access,             EXE_SERVICE_SIM,    "EXE_CGLA"},
    {EXE_EPINR,                     request_get_pin_info,                   EXE_SERVICE_SIM,    "EXE_EPINR"},
    {EXE_EPINR_FULL,                request_get_full_pin_info,              EXE_SERVICE_SIM,    "EXE_EPINR_FULL"},
    {EXE_EFSIM,                     request_get_full_sim_info,              EXE_SERVICE_SIM,    "EXE_EFSIM"},
    {EXE_ESHLVOCR,                  request_answer_setup_call,              EXE_SERVICE_SIM,    "EXE_ESHLVOCR"},
    {EXE_SIM_RESTART,               request_sim_restart,                    EXE_SERVICE_SIM,    "EXE_SIM_RESTART"},
#endif
#ifdef EXE_USE_SMS_SERVICE
    {EXE_CMGD_DELETE,               request_sms_delete,                     EXE_SERVICE_SMS,    "EXE_CMGD_DELETE"},
    {EXE_CMGD_DELETE_ALL,           request_sms_delete_all,                 EXE_SERVICE_SMS,    "EXE_CMGD_DELETE_ALL"},
    {EXE_CMGS,                      request_sms_send,                       EXE_SERVICE_SMS,    "EXE_CMGS"},
    {EXE_CMGW,                      request_sms_write,                      EXE_SERVICE_SMS,    "EXE_CMGW"},
    {EXE_CPMS,                      request_sms_storage_info,               EXE_SERVICE_SMS,    "EXE_CPMS"},
    {EXE_CNMA,                      request_sms_send_ack,                   EXE_SERVICE_SMS,    "EXE_CNMA"},
    {EXE_CNMI,                      request_message_event_subscribe,        EXE_SERVICE_SMS,    "EXE_CNMI"},
    {EXE_CSMS_SET,                  request_select_message_service_set,     EXE_SERVICE_SMS,    "EXE_CSMS_SET"},
    {EXE_CSMS_READ,                 request_select_message_service_get,     EXE_SERVICE_SMS,    "EXE_CSMS_READ"},
    {EXE_ESMSFULL,                  request_sms_memory_capacity,            EXE_SERVICE_SMS,    "EXE_ESMSFULL"},
    {EXE_ESMSFULL_READ,             request_sms_memory_capacity_get,        EXE_SERVICE_SMS,    "EXE_ESMSFULL_READ"},
    {EXE_CMMS_SET,                  request_sms_relay_link_control_set,     EXE_SERVICE_SMS,    "EXE_CMMS_SET"},
    {EXE_CMMS_READ,                 request_sms_relay_link_control_get,     EXE_SERVICE_SMS,    "EXE_CMMS_READ"},
    {EXE_CMGL,                      request_sms_list,                       EXE_SERVICE_SMS,    "EXE_CMGL"},
    {EXE_CMGR,                      request_sms_read,                       EXE_SERVICE_SMS,    "EXE_CMGR"},
    {EXE_CMSS,                      request_send_sms_from_preferred_storage, EXE_SERVICE_SMS,   "EXE_CMSS"},
    {EXE_CGSMS_SET,                 request_cgsms_set,                      EXE_SERVICE_SMS,    "EXE_CGSMS_SET"},
    {EXE_CGSMS_READ,                request_cgsms_read,                     EXE_SERVICE_SMS,    "EXE_CGSMS_READ"},
    {EXE_CSCB_SET,                  request_cbs_set_subscription,           EXE_SERVICE_SMS,    "EXE_CSCB_SET"},
    {EXE_CSCB_READ,                 request_cbs_get_subscription,           EXE_SERVICE_SMS,    "EXE_CSCB_READ"},
#endif
#ifdef EXE_USE_PSCC_SERVICE
    {EXE_CGACT,                     request_set_PDP_context_state,          EXE_SERVICE_PSCC,   "EXE_CGACT"},
    {EXE_CGACT_READ,                request_get_activation_stats,           EXE_SERVICE_PSCC,   "EXE_CGACT_READ"},
    {EXE_CGATT_SET_GPRS_STATE,      request_set_GPRS_state,                 EXE_SERVICE_PSCC,   "EXE_CGATT_SET_GPRS_STATE"},
    {EXE_CGATT_GET_GPRS_STATE,      reguest_get_GPRS_state,                 EXE_SERVICE_PSCC,   "EXE_CGATT_GET_GPRS_STATE"},
    {EXE_CGCMOD,                    request_set_PDP_modify,                 EXE_SERVICE_PSCC,   "EXE_CGCMOD"},
    {EXE_CGDCONT,                   request_set_PDP_context,                EXE_SERVICE_PSCC,   "EXE_CGDCONT"},
    {EXE_CGDCONT_READ,              request_get_PDP_context,                EXE_SERVICE_PSCC,   "EXE_CGDCONT_READ"},
    {EXE_CGEQMIN_SET,               request_set_PDP_minimum_qos,            EXE_SERVICE_PSCC,   "EXE_CGEQMIN_SET"},
    {EXE_CGEQMIN_GET,               request_get_PDP_minimum_qos,            EXE_SERVICE_PSCC,   "EXE_CGEQMIN_GET"},
    {EXE_CGEQREQ_SET,               request_set_PDP_required_qos,           EXE_SERVICE_PSCC,   "EXE_CGEQREQ_SET"},
    {EXE_CGEQREQ_GET,               request_get_PDP_required_qos,           EXE_SERVICE_PSCC,   "EXE_CGEQREQ_GET"},
    {EXE_CGQMIN_SET,                request_set_PDP_minimum_qos_2G,         EXE_SERVICE_PSCC,   "EXE_CGQMIN_SET"},
    {EXE_CGQMIN_GET,                request_get_PDP_minimum_qos_2G,         EXE_SERVICE_PSCC,   "EXE_CGQMIN_GET"},
    {EXE_CGQREQ_SET,                request_set_PDP_required_qos_2G,        EXE_SERVICE_PSCC,   "EXE_CGQREQ_SET"},
    {EXE_CGQREQ_GET,                request_get_PDP_required_qos_2G,        EXE_SERVICE_PSCC,   "EXE_CGQREQ_GET"},
    {EXE_CGEQNEG_SET,               request_set_PDP_negotiated_qos,         EXE_SERVICE_PSCC,   "EXE_CGEQNEG_SET"},
    {EXE_CGPADDR,                   request_get_PDP_address,                EXE_SERVICE_PSCC,   "EXE_CGPADDR"},
    {EXE_EIAAUW,                    request_set_PDP_auth,                   EXE_SERVICE_PSCC,   "EXE_EIAAUW"},
    {EXE_ENAP_CONNECT,              request_nap_connect,                    EXE_SERVICE_PSCC,   "EXE_ENAP_CONNECT"},
    {EXE_ENAP_DISCONNECT,           request_nap_disconnect,                 EXE_SERVICE_PSCC,   "EXE_ENAP_DISCONNECT"},
    {EXE_ENAP_READ,                 request_nap_read,                       EXE_SERVICE_PSCC,   "EXE_ENAP_READ"},
    {EXE_EPPSD,                     request_activate_PDP_context,           EXE_SERVICE_PSCC,   "EXE_EPPSD"},
    {EXE_EUPLINK,                   request_set_PDP_send_data,              EXE_SERVICE_PSCC,   "EXE_EUPLINK"},
    {EXE_PS_DIAL_UP,                request_psdun,                          EXE_SERVICE_PSCC,   "EXE_PS_DIAL_UP"},
    {EXE_PSCC_EVENT_DISCONNECTED,   internal_request_disconnected_failure_cause, EXE_SERVICE_PSCC, "EXE_PSCC_EVENT_DISCONNECTED"},
#endif
#ifdef EXE_USE_CSPSA_SERVICE
    {EXE_CSPSA_WRITE_DATA,          request_write_value,                    EXE_SERVICE_CSPSA,  "EXE_CSPSA_WRITE_DATA"},
    {EXE_CSPSA_READ_DATA,           request_read_value,                     EXE_SERVICE_CSPSA,  "EXE_CSPSA_READ_DATA"},
#endif
    {EXE_RADIOVER_READ,             request_radiover_read,                  EXE_SERVICE_CN,     "EXE_RADIOVER_READ"},
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_exe_extended_requests_table.h>
#endif
};

#endif
