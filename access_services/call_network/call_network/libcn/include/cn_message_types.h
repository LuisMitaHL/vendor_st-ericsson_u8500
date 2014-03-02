/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_message_types_h__
#define __cn_message_types_h__ (1)

#include "cn_data_types.h"

/***** Force structural alignment setting *****
 * When using different compilers and/or compiler settings the alignment may differ.
 * This is not acceptable for the IPC communication. A four byte alignment is chosen
 * since that is the word size of the ARM processor.
 */
#pragma pack(push,4)

/*************************************************************************
 *                     MESSAGE TYPE DEFINITIONS                          *
 *************************************************************************/

#define CN_EVENT_FLAG    0x80000000 /* usage: if (CN_EVENT_FLAG & type) { handle_event... } */
#define CN_REQUEST_FLAG  0x40000000 /* usage: if (CN_REQUEST_FLAG & type) { handle_request... } and if (!CN_REQUEST_FLAG & type) { handle_response... } */
#define CN_REQUEST_MASK  0x000002ff
#define CN_RESPONSE_MASK 0x000ffc00
#define CN_EVENT_MASK    0x2ff00000

typedef enum {
    CN_REQUEST_UNKNOWN = 0x00,      /* Request range: bit 0-9 */
    CN_REQUEST_READ_OPL_FROM_SIM,   /* Note: Internal request */
    CN_REQUEST_SET_DEFAULT_NVMD,
    CN_REQUEST_RF_ON,
    CN_REQUEST_RF_OFF,
    CN_REQUEST_RF_STATUS,
    CN_REQUEST_REGISTRATION_CONTROL,
    CN_REQUEST_RESET_MODEM,
    CN_REQUEST_SET_PREFERRED_NETWORK_TYPE,
    CN_REQUEST_GET_PREFERRED_NETWORK_TYPE,
    CN_REQUEST_REGISTRATION_STATE_NORMAL,
    CN_REQUEST_REGISTRATION_STATE_GPRS,
    CN_REQUEST_NETWORK_EMERGENCY_NUMBERS_UPDATE,
    CN_REQUEST_CELL_INFO,
    CN_REQUEST_AUTOMATIC_NETWORK_REGISTRATION,
    CN_REQUEST_MANUAL_NETWORK_REGISTRATION,
    CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK,
    CN_REQUEST_NETWORK_DEREGISTER,
    CN_REQUEST_NET_QUERY_MODE,
    CN_REQUEST_MANUAL_NETWORK_SEARCH,
    CN_REQUEST_INTERRUPT_NETWORK_SEARCH,
    CN_REQUEST_CURRENT_CALL_LIST,
    CN_REQUEST_DIAL,
    CN_REQUEST_HANGUP,
    CN_REQUEST_SWAP_CALLS,
    CN_REQUEST_HOLD_CALL,
    CN_REQUEST_RESUME_CALL,
    CN_REQUEST_CONFERENCE_CALL,
    CN_REQUEST_CONFERENCE_CALL_SPLIT,
    CN_REQUEST_EXPLICIT_CALL_TRANSFER,
    CN_REQUEST_ANSWER_CALL,
    CN_REQUEST_USSD,
    CN_REQUEST_USSD_ABORT,
    CN_REQUEST_SET_SIGNAL_INFO_CONFIG,
    CN_REQUEST_GET_SIGNAL_INFO_CONFIG,
    CN_REQUEST_SET_SIGNAL_INFO_REPORTING,
    CN_REQUEST_GET_SIGNAL_INFO_REPORTING,
    CN_REQUEST_RSSI_VALUE,
    CN_REQUEST_CLIP_STATUS,
    CN_REQUEST_CLIR_STATUS,
    CN_REQUEST_CNAP_STATUS,
    CN_REQUEST_COLR_STATUS,
    CN_REQUEST_SET_CLIR,
    CN_REQUEST_QUERY_CALL_FORWARD,
    CN_REQUEST_SET_CALL_FORWARD,
    CN_REQUEST_SET_MODEM_PROPERTY,
    CN_REQUEST_GET_MODEM_PROPERTY,
    CN_REQUEST_SET_CSSN,
    CN_REQUEST_DTMF_SEND,
    CN_REQUEST_DTMF_START,
    CN_REQUEST_DTMF_STOP,
    CN_REQUEST_SET_CALL_WAITING,
    CN_REQUEST_GET_CALL_WAITING,
    CN_REQUEST_CHANGE_BARRING_PASSWORD,
    CN_REQUEST_QUERY_CALL_BARRING,
    CN_REQUEST_SET_CALL_BARRING,
    CN_REQUEST_SET_PRODUCT_PROFILE_FLAG,
    CN_REQUEST_SET_L1_PARAMETER,
    CN_REQUEST_SET_USER_ACTIVITY_STATUS,
    CN_REQUEST_SET_NEIGHBOUR_CELLS_REPORTING,
    CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING,
    CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO,
    CN_REQUEST_SET_EVENT_REPORTING,
    CN_REQUEST_GET_EVENT_REPORTING,
    CN_REQUEST_GET_RAB_STATUS,
    CN_REQUEST_BASEBAND_VERSION,
    CN_REQUEST_GET_PP_FLAGS,
    CN_REQUEST_MODIFY_EMERGENCY_NUMBER_LIST,
    CN_REQUEST_GET_EMERGENCY_NUMBER_LIST,
    CN_REQUEST_TIMING_ADVANCE,
    CN_REQUEST_NMR_INFO,
    CN_REQUEST_SET_HSXPA_MODE,
    CN_REQUEST_GET_HSXPA_MODE,
    CN_REQUEST_SS,
    CN_REQUEST_SET_MANDATED_NETWORK_TYPE_FROM_SIM, /* Note: Internal request */
    CN_REQUEST_REG_STATUS_EVENT_CONFIG,
    CN_REQUEST_RAT_NAME,
    CN_REQUEST_MODEM_POWER_OFF,
    CN_REQUEST_SEND_TX_BACK_OFF_EVENT,
    CN_REQUEST_SET_EMPAGE,
    CN_REQUEST_RESET_MODEM_WITH_DUMP,
    CN_REQUEST_SLEEP_TEST_MODE,
    CN_REQUEST_GET_NEIGHBOUR_CELLS_EXTD_INFO,

    CN_RESPONSE_UNKNOWN = 0x300,    /* Response range: bit 10-19 */
    CN_RESPONSE_READ_OPL_FROM_SIM,  /* Note: Internal request    */
    CN_RESPONSE_SET_DEFAULT_NVMD,
    CN_RESPONSE_RF_ON,
    CN_RESPONSE_RF_OFF,
    CN_RESPONSE_RF_STATUS,
    CN_RESPONSE_REGISTRATION_CONTROL,
    CN_RESPONSE_RESET_MODEM,
    CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE,
    CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE,
    CN_RESPONSE_REGISTRATION_STATE_NORMAL,
    CN_RESPONSE_REGISTRATION_STATE_GPRS,
    CN_RESPONSE_CELL_INFO,
    CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION,
    CN_RESPONSE_MANUAL_NETWORK_REGISTRATION,
    CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK,
    CN_RESPONSE_NETWORK_DEREGISTER,
    CN_RESPONSE_NET_QUERY_MODE,
    CN_RESPONSE_MANUAL_NETWORK_SEARCH,
    CN_RESPONSE_INTERRUPT_NETWORK_SEARCH,
    CN_RESPONSE_CURRENT_CALL_LIST,
    CN_RESPONSE_DIAL,
    CN_RESPONSE_HANGUP,
    CN_RESPONSE_SWAP_CALLS,
    CN_RESPONSE_HOLD_CALL,
    CN_RESPONSE_RESUME_CALL,
    CN_RESPONSE_CONFERENCE_CALL,
    CN_RESPONSE_CONFERENCE_CALL_SPLIT,
    CN_RESPONSE_EXPLICIT_CALL_TRANSFER,
    CN_RESPONSE_ANSWER_CALL,
    CN_RESPONSE_USSD,
    CN_RESPONSE_USSD_ABORT,
    CN_RESPONSE_SET_SIGNAL_INFO_CONFIG,
    CN_RESPONSE_GET_SIGNAL_INFO_CONFIG,
    CN_RESPONSE_SET_SIGNAL_INFO_REPORTING,
    CN_RESPONSE_GET_SIGNAL_INFO_REPORTING,
    CN_RESPONSE_RSSI_VALUE,
    CN_RESPONSE_CLIP_STATUS,
    CN_RESPONSE_CLIR_STATUS,
    CN_RESPONSE_CNAP_STATUS,
    CN_RESPONSE_COLR_STATUS,
    CN_RESPONSE_SET_CLIR,
    CN_RESPONSE_QUERY_CALL_FORWARD,
    CN_RESPONSE_SET_CALL_FORWARD,
    CN_RESPONSE_SET_MODEM_PROPERTY,
    CN_RESPONSE_GET_MODEM_PROPERTY,
    CN_RESPONSE_SET_CSSN,
    CN_RESPONSE_DTMF_SEND,
    CN_RESPONSE_DTMF_START,
    CN_RESPONSE_DTMF_STOP,
    CN_RESPONSE_SET_CALL_WAITING,
    CN_RESPONSE_GET_CALL_WAITING,
    CN_RESPONSE_CHANGE_BARRING_PASSWORD,
    CN_RESPONSE_QUERY_CALL_BARRING,
    CN_RESPONSE_SET_CALL_BARRING,
    CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG,
    CN_RESPONSE_SET_L1_PARAMETER,
    CN_RESPONSE_SET_USER_ACTIVITY_STATUS,
    CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING,
    CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING,
    CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO,
    CN_RESPONSE_SET_EVENT_REPORTING,
    CN_RESPONSE_GET_EVENT_REPORTING,
    CN_RESPONSE_GET_RAB_STATUS,
    CN_RESPONSE_BASEBAND_VERSION,
    CN_RESPONSE_GET_PP_FLAGS,
    CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST,
    CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST,
    CN_RESPONSE_TIMING_ADVANCE,
    CN_RESPONSE_NMR_INFO,
    CN_RESPONSE_SET_HSXPA_MODE,
    CN_RESPONSE_GET_HSXPA_MODE,
    CN_RESPONSE_SS,
    CN_RESPONSE_SET_MANDATED_NETWORK_TYPE_FROM_SIM,  /* Note: Internal request    */
    CN_RESPONSE_REG_STATUS_EVENT_CONFIG,
    CN_RESPONSE_RAT_NAME,
    CN_RESPONSE_MODEM_POWER_OFF,
    CN_RESPONSE_SEND_TX_BACK_OFF_EVENT,
    CN_RESPONSE_SET_EMPAGE,
    CN_RESPONSE_RESET_MODEM_WITH_DUMP,
    CN_RESPONSE_SLEEP_TEST_MODE,
    CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO,


    CN_EVENT_UNKNOWN = 0x100000, /* event range: bit 20-29 */
    CN_EVENT_RADIO_STATUS,
    CN_EVENT_MODEM_REGISTRATION_STATUS,
    CN_EVENT_NETWORK_INFO,
    CN_EVENT_TIME_INFO,
    CN_EVENT_RING,
    CN_EVENT_RING_WAITING,
    CN_EVENT_CALL_STATE_CHANGED,
    CN_EVENT_NET_DETAILED_FAIL_CAUSE,
    CN_EVENT_CALL_SUPP_SVC_NOTIFICATION,
    CN_EVENT_USSD,
    CN_EVENT_NEIGHBOUR_CELLS_INFO,
    CN_EVENT_SIGNAL_INFO,
    CN_EVENT_RAB_STATUS,
    CN_EVENT_CALL_CNAP,
    CN_EVENT_CELL_INFO,
    CN_EVENT_GENERATE_LOCAL_COMFORT_TONES,
    CN_EVENT_SS_STATUS_INFO,
    CN_EVENT_RAT_NAME,
    CN_EVENT_EMPAGE_INFO
} cn_message_type_t;          /* request flag: bit 30 */
/* event flag: bit 31 */

/*************************************************************************
 *                  GENERIC MESSAGE STRUCT DEFINITION                    *
 *************************************************************************/

/* DESIGN NOTE:
 * All messages that do not require any payload doesn't have a special struct defined below. All those that need
 * a structure have specific ones even if that sometimes means identical structures. This makes it
 * easier to use, if a function has something in addition to a client_tag, the payload structure has
 * the same name as the request/response (plus a "_t" suffix). Consistency is important.
 * Payload is only valid if CN_SUCCESS is returned in the response.
 */

/* The cn_message_t structure is used for both request, response and event messages */
typedef struct {
    cn_message_type_t type;         /* message type. Determines which payload struct to use, if any.                    */
    cn_error_code_t   error_code;   /* request status code. given in the response message. Undefined for event messages */
    cn_client_tag_t   client_tag;   /* the client_tag is returned in the response. Has value zero for events messages.  */
    cn_uint32_t       payload_size; /* size of message payload.                                                         */
    cn_uint8_t        payload[1];   /* start of payload (if any).                                                       */
} cn_message_t;

#define CN_MESSAGE_HEADER_SIZE (sizeof(cn_message_t))


/*************************************************************************
 *                      REQUEST STRUCT DEFINITIONS                       *
 *************************************************************************/

typedef struct {
    cn_network_type_t type;
} cn_request_set_preferred_network_type_t;

typedef struct {
    char plmn_operator[CN_NETWORK_OPERATOR_MAX_LONG_LEN + 1];
    uint8_t act;
    cn_operator_format_t format;
} cn_request_manual_network_registration_t;

typedef struct {
    char plmn_operator[CN_NETWORK_OPERATOR_MAX_LONG_LEN + 1];
    uint8_t act;
    cn_operator_format_t format;
} cn_request_manual_network_registration_with_automatic_fallback_t;

typedef struct {
    cn_cellular_system_command_t command;
} cn_request_network_deregister_t;

typedef struct {
    cn_network_access_config_data_t config_data;
} cn_request_network_access_config_data_t;

typedef struct {
    cn_uint8_t active_call_id;
} cn_request_swap_calls_t;

typedef struct {
    cn_uint8_t active_call_id;
} cn_request_hold_call_t;

typedef struct {
    cn_uint8_t hold_call_id;
} cn_request_resume_call_t;

typedef struct {
    cn_uint8_t active_call_id;
} cn_request_conference_call_t;

typedef struct {
    cn_uint8_t active_call_id;
} cn_request_conference_call_split_t;

typedef struct {
    cn_uint8_t call_id;
} cn_request_explicit_call_transfer_t;

typedef struct {
    cn_uint8_t call_id;
} cn_request_answer_call_t;

typedef struct {
    cn_ussd_info_t ussd_info;
} cn_request_ussd_t;

typedef struct {
    cn_clir_setting_t clir_setting;
} cn_request_set_clir_t;

typedef struct {
    cn_sleep_mode_setting_t sleep_mode;
} cn_request_sleep_mode_t;

typedef struct {
    cn_call_forward_info_t call_forward_info;
} cn_request_call_forward_info_t;

typedef struct {
    cn_modem_property_t modem_property;
} cn_request_set_modem_property_t;

typedef struct {
    cn_modem_property_type_t type;
} cn_request_get_modem_property_t;

typedef struct {
    cn_cssn_setting_t cssn_setting;
} cn_request_set_cssn_t;

typedef struct {
    cn_dtmf_string_type_t string_type;
    cn_uint16_t length;
    char dtmf_string[CN_MAX_STRING_SIZE];
    cn_uint16_t duration_time;
    cn_uint16_t pause_time;
} cn_request_dtmf_string_t;

typedef struct {
    cn_request_dtmf_string_t dtmf_string;
} cn_request_dtmf_send_t;

typedef struct {
    char character;
} cn_request_dtmf_start_t;

typedef struct {
    cn_call_waiting_t call_waiting;
} cn_request_set_call_waiting_t;

typedef struct {
    cn_uint32_t service_class;
} cn_request_get_call_waiting_t;

typedef struct {
    cn_dial_t dial;
} cn_request_dial_t;

typedef struct {
    cn_uint8_t call_id;
    cn_call_state_filter_t filter;
} cn_request_hangup_t;

typedef struct {
    cn_uint16_t flag_id;
    cn_uint16_t flag_value;
} cn_request_set_product_profile_flag_t;

typedef struct {
    cn_uint8_t command;
    cn_uint16_t parameter;
} cn_request_set_l1_parameter_t;

typedef struct {
    cn_user_status_t user_status;
    cn_battery_status_t battery_status;
} cn_request_set_user_activity_status_t;

typedef struct {
    cn_bool_t enable_reporting;
    cn_neighbour_rat_type_t rat_type;
} cn_request_set_neighbour_cells_reporting_t;

typedef struct {
    cn_neighbour_rat_type_t rat_type;
} cn_request_get_neighbour_cells_reporting_t;

typedef struct {
    cn_neighbour_rat_type_t rat_type;
} cn_request_get_neighbour_cells_info_t;

typedef struct {
    char facility[CN_MAX_STRING_SIZE];
    char old_passwd[CN_MAX_STRING_SIZE];
    char new_passwd[CN_MAX_STRING_SIZE];
} cn_request_change_barring_password_t;

typedef struct {
    char facility[CN_MAX_STRING_SIZE];
    cn_uint32_t service_class;
} cn_request_query_call_barring_t;

typedef struct {
    char facility[CN_MAX_STRING_SIZE];
    cn_call_barring_t call_barring;
    char passwd[CN_MAX_STRING_SIZE];
} cn_request_set_call_barring_t;

typedef struct {
    cn_bool_t read_all_flags;
    cn_uint16_t flag_id;
} cn_request_get_pp_flags_t;

typedef struct {
    cn_signal_info_config_t config;
} cn_request_set_signal_info_config_t;

typedef struct {
    cn_rssi_mode_t mode;
} cn_request_set_signal_info_reporting_t;

typedef struct {
    cn_event_reporting_type_t type;
    cn_bool_t                 enable_reporting;
} cn_request_set_event_reporting_t;

typedef struct {
    cn_event_reporting_type_t type;
} cn_request_get_event_reporting_t;

typedef struct {
    cn_emergency_number_operation_t operation;
    cn_emergency_number_config_t    config;
} cn_request_modify_emergency_number_list_t;

typedef struct {
    cn_nmr_rat_type_t rat;
    cn_nmr_utran_type_t utran;
} cn_request_nmr_info_t;

typedef struct {
    cn_hsxpa_mode_t hsxpa_mode;
} cn_request_hsxpa_t;

typedef struct {
    cn_ss_command_t    ss_command;
} cn_request_ss_command_t;

typedef struct {
    cn_reg_status_trigger_level_t trigger_level;
} cn_request_reg_status_event_config_t;

typedef struct {
    cn_tx_back_off_event_t event;
} cn_request_send_tx_back_off_event_t;

typedef struct {
    cn_empage_t config_data;
} cn_request_empage_config_data_t;

typedef struct {
    cn_cpu_type_t type;
} cn_request_reset_cpu_type_t;


/*************************************************************************
 *                             UTILITY MACROS                            *
 *************************************************************************/

/* standardized macros for NULL checks (to have it on a single line) */
#define REQUIRE_VALID_RECORD(p) \
            if (!(p)) { \
                CN_LOG_E("record_p is NULL!"); \
                return REQUEST_STATUS_ERROR; \
            }
#define REQUIRE_VALID_REQUEST_DATA(p) \
            if (!(p)) { \
                CN_LOG_E("request data_p is NULL!"); \
                return REQUEST_STATUS_ERROR; \
            }
#define REQUIRE_VALID_RESPONSE_DATA(p) \
            if (!(p)) { \
                CN_LOG_E("response data_p is NULL!"); \
                return REQUEST_STATUS_ERROR; \
            }
#define REQUIRE_VALID_EVENT_DATA(p) \
            if (!(p)) { \
                CN_LOG_E("event data_p is NULL!"); \
                goto exit; \
            }


#pragma pack(pop) /* go back to previous packing setting */

#endif /* __cn_message_types_h__ */
