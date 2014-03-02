/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : uiccd_msg.h
 * Description     : message handling
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


// FIXME: Rename catd_msg* sim_msg*

#ifndef __uiccd_msg_h__
#define __uiccd_msg_h__ (1)


#include "uiccd.h"
#include "catd_modem_types.h"
#include "msgq.h"
#include "cat_barrier.h"


// -----------------------------------------------------------------------------
// Message types

// xsel | perl -n -e 'if ( /^\s+(\w+,)/ ) { printf("  %-40s// %#04x\n", $1, ++$count ); }'
typedef enum {
  UICCD_MSG_NONE                       = STE_MSG_RANGE_UICC,
  UICCD_MSG_CONNECT,
  UICCD_MSG_DISCONNECT,
  UICCD_MSG_STARTUP,
  UICCD_MSG_STARTUP_DONE,
  UICCD_MSG_SHUTDOWN,
  UICCD_MSG_STOP,
  UICCD_MSG_REGISTER,
  UICCD_MSG_PIN_VERIFY,
  UICCD_MSG_PIN_VERIFY_RSP,
  UICCD_MSG_PIN_CHANGE,
  UICCD_MSG_PIN_CHANGE_RSP,
  UICCD_MSG_READ_SIM_FILE_RECORD,
  UICCD_MSG_READ_SIM_FILE_RECORD_RSP,
  UICCD_MSG_READ_SIM_FILE_BINARY,
  UICCD_MSG_READ_SIM_FILE_BINARY_RSP,
  UICCD_MSG_UPDATE_SIM_FILE_RECORD,
  UICCD_MSG_UPDATE_SIM_FILE_RECORD_RSP,
  UICCD_MSG_UPDATE_SIM_FILE_BINARY,
  UICCD_MSG_UPDATE_SIM_FILE_BINARY_RSP,
  UICCD_MSG_SIM_FILE_GET_FORMAT,
  UICCD_MSG_SIM_FILE_GET_FORMAT_RSP,
  UICCD_MSG_GET_FILE_INFORMATION,
  UICCD_MSG_GET_FILE_INFORMATION_RSP,
  UICCD_MSG_APP_LIST_RSP,
  UICCD_MSG_APP_ACTIVATE_RSP,
  UICCD_MSG_PIN_VERIFY_NEEDED_IND,
  UICCD_MSG_SERVER_STATUS_RSP,
  UICCD_MSG_GET_SIM_STATE,
  UICCD_MSG_PIN_DISABLE,
  UICCD_MSG_PIN_DISABLE_RSP,
  UICCD_MSG_PIN_ENABLE,
  UICCD_MSG_PIN_ENABLE_RSP,
  UICCD_MSG_PIN_INFO,
  UICCD_MSG_PIN_INFO_RSP,
  UICCD_MSG_PIN_UNBLOCK_NEEDED_IND,
  UICCD_MSG_PIN_PERMANENTLY_BLOCKED_IND,
  UICCD_MSG_PIN_UNBLOCK,
  UICCD_MSG_PIN_UNBLOCK_RSP,
  UICCD_MSG_APP_INFO,
  UICCD_MSG_MODEM_STATE,
  UICCD_MSG_SERVER_CARD_STATUS_RSP,
  UICCD_MSG_CARD_STATUS_IND,
  UICCD_MSG_UICC_STATUS_IND,
  UICCD_MSG_SIM_FILE_READ_GENERIC,
  UICCD_MSG_SIM_FILE_READ_GENERIC_RSP,
  UICCD_MSG_UPDATE_SERVICE_TABLE,
  UICCD_MSG_UPDATE_SERVICE_TABLE_RSP,
  UICCD_MSG_GET_SERVICE_TABLE,
  UICCD_MSG_GET_SERVICE_TABLE_RSP,
  UICCD_MSG_GET_SERVICE_AVAILABILITY,
  UICCD_MSG_GET_SERVICE_AVAILABILITY_RSP,
  UICCD_MSG_SIM_STATUS,
  UICCD_MSG_APP_STATUS,
  UICCD_MSG_CARD_STATUS,
  UICCD_MSG_SIM_ICON_READ,
  UICCD_MSG_SIM_ICON_READ_RSP,
  UICCD_MSG_SIM_CONNECT_RSP,
  UICCD_MSG_SIM_DISCONNECT_RSP,
  UICCD_MSG_SAP_SESSION_START,
  UICCD_MSG_SAP_SESSION_START_RSP,
  UICCD_MSG_SAP_SESSION_EXIT,
  UICCD_MSG_SAP_SESSION_EXIT_RSP,
  UICCD_MSG_SAP_SESSION_APDU_DATA,
  UICCD_MSG_SAP_SESSION_APDU_DATA_RSP,
  UICCD_MSG_SAP_SESSION_WARMRESET_CARD,
  UICCD_MSG_SAP_SESSION_WARMRESET_CARD_RSP,
  UICCD_MSG_SAP_SESSION_COLDRESET_CARD,
  UICCD_MSG_SAP_SESSION_COLDRESET_CARD_RSP,
  UICCD_MSG_SAP_SESSION_POWEROFF_CARD,
  UICCD_MSG_SAP_SESSION_POWEROFF_CARD_RSP,
  UICCD_MSG_SAP_SESSION_POWERON_CARD,
  UICCD_MSG_SAP_SESSION_POWERON_CARD_RSP,
  UICCD_MSG_SAP_SESSION_ATR_GET,
  UICCD_MSG_SAP_SESSION_ATR_GET_RSP,
  UICCD_MSG_SAP_SESSION_RESET_CARD_IND,
  UICCD_MSG_SAP_SESSION_STATUS_GET,
  UICCD_MSG_READ_SUBSCRIBER_NUMBER,
  UICCD_MSG_READ_SUBSCRIBER_NUMBER_RSP,
  UICCD_MSG_READ_PLMN,
  UICCD_MSG_UPDATE_PLMN,
  UICCD_MSG_APPL_APDU_SEND,
  UICCD_MSG_APPL_APDU_SEND_RSP,
  UICCD_MSG_READ_SMSC,
  UICCD_MSG_READ_SMSC_RSP,
  UICCD_MSG_UPDATE_SMSC,
  UICCD_MSG_UPDATE_SMSC_RSP,
  UICCD_MSG_SMSC_GET_RECORD_MAX,
  UICCD_MSG_SMSC_SAVE_TO_RECORD,
  UICCD_MSG_SMSC_RESTORE_FROM_RECORD,
  UICCD_MSG_SIM_CHANNEL_SEND,
  UICCD_MSG_SIM_CHANNEL_SEND_RSP,
  UICCD_MSG_SIM_CHANNEL_OPEN,
  UICCD_MSG_SIM_CHANNEL_OPEN_RSP,
  UICCD_MSG_SIM_CHANNEL_CLOSE,
  UICCD_MSG_SIM_CHANNEL_CLOSE_RSP,
  UICCD_MSG_READ_FDN,
  UICCD_MSG_READ_ECC,
  UICCD_MSG_RESET,
  UICCD_MSG_SIM_POWER_ON,
  UICCD_MSG_SIM_POWER_ON_RSP,
  UICCD_MSG_SIM_POWER_OFF,
  UICCD_MSG_SIM_POWER_OFF_RSP,
  UICCD_MSG_SIM_READ_PREFERRED_RAT_SETTING,
  UICCD_MSG_SIM_READ_PREFERRED_RAT_SETTING_RSP,
  UICCD_MSG_READ_SIM_FILE_BINARY_TRANSACTION,
  UICCD_MSG_PIN_DISABLE_TRANSACTION,
  UICCD_MSG_MODEM_SILENT_RESET,
  UICCD_MSG_PIN_VERIFY_MSR,
  UICCD_MSG_CN_RSP,
  UICCD_MSG_CN_EVENT,
  UICCD_MSG_CARD_FALLBACK,
  UICCD_MSG_APP_SHUTDOWN_INITIATE_RSP,
  UICCD_MSG__MAX                              /* highest msg id no */
} UICCD_MSG_TYPES_t;

// -----------------------------------------------------------------------------
// General empty messages

// -----------------------------------------------------------------------------
// Generic file descriptor messages
typedef struct {
    STE_MSG_COMMON;
    int                     fd;
} uiccd_msg_fd_t;

typedef uiccd_msg_fd_t  uiccd_msg_connect_t;
typedef uiccd_msg_fd_t  uiccd_msg_disconnect_t;
typedef uiccd_msg_fd_t  uiccd_msg_shutdown_t;
typedef uiccd_msg_fd_t  uiccd_msg_modem_silent_reset_t;
typedef uiccd_msg_fd_t  uiccd_msg_startup_t;
typedef uiccd_msg_fd_t  uiccd_msg_startup_done_t;
typedef uiccd_msg_fd_t  uiccd_msg_register_t;
typedef uiccd_msg_fd_t  uiccd_msg_app_info_t;
typedef uiccd_msg_fd_t  uiccd_msg_card_status_t;
typedef uiccd_msg_fd_t  uiccd_msg_read_fdn_t;
typedef uiccd_msg_fd_t  uiccd_msg_read_ecc_t;
typedef uiccd_msg_fd_t  uiccd_msg_reset_t;
typedef uiccd_msg_fd_t  uiccd_msg_sim_power_on_t;
typedef uiccd_msg_fd_t  uiccd_msg_sim_power_off_t;
typedef uiccd_msg_fd_t  uiccd_msg_sim_read_preferred_rat_setting_t;

uiccd_msg_connect_t    *uiccd_msg_connect_create(int fd, uintptr_t client_tag);
uiccd_msg_disconnect_t *uiccd_msg_disconnect_create(int fd, uintptr_t client_tag);
uiccd_msg_startup_t    *uiccd_msg_startup_create(int fd, uintptr_t client_tag);
uiccd_msg_startup_done_t    *uiccd_msg_startup_done_create(int fd, uintptr_t client_tag);
uiccd_msg_shutdown_t   *uiccd_msg_shutdown_create(int fd, uintptr_t client_tag);
uiccd_msg_modem_silent_reset_t   *uiccd_msg_modem_silent_reset_create(int fd, uintptr_t client_tag);
uiccd_msg_register_t   *uiccd_msg_register_create(int fd, uintptr_t client_tag);
uiccd_msg_app_info_t   *uiccd_msg_app_info_create(int fd, uintptr_t client_tag);
uiccd_msg_card_status_t   *uiccd_msg_card_status_create(int fd, uintptr_t client_tag);
uiccd_msg_read_fdn_t   *uiccd_msg_read_fdn_create(int fd, uintptr_t client_tag);
uiccd_msg_read_ecc_t   *uiccd_msg_read_ecc_create(int fd, uintptr_t client_tag);
uiccd_msg_reset_t      *uiccd_msg_reset_create(int fd, uintptr_t client_tag);
uiccd_msg_sim_power_on_t     *uiccd_msg_sim_power_on_create(int fd, uintptr_t client_tag);
uiccd_msg_sim_power_off_t    *uiccd_msg_sim_power_off_create(int fd, uintptr_t client_tag);
uiccd_msg_sim_read_preferred_rat_setting_t *uiccd_msg_sim_read_preferred_rat_setting_create(int fd, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// Generic int messages
typedef struct {
    STE_MSG_COMMON;
    int                     i;
} uiccd_msg_int_t;

typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t              uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  sim_uicc_status_word_t              status_word;
} uiccd_msg_status_t;

typedef uiccd_msg_status_t  uiccd_msg_pin_change_response_t;
typedef uiccd_msg_status_t  uiccd_msg_pin_verify_response_t;
typedef uiccd_msg_status_t  uiccd_msg_pin_disable_response_t;
typedef uiccd_msg_status_t  uiccd_msg_pin_enable_response_t;
typedef uiccd_msg_status_t  uiccd_msg_pin_unblock_response_t;

uiccd_msg_pin_change_response_t* uiccd_msg_pin_change_response_create(uintptr_t                           client_tag,
                                                                      sim_uicc_status_code_t              uicc_status_code,
                                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                      sim_uicc_status_word_t              status_word);

uiccd_msg_pin_verify_response_t* uiccd_msg_pin_verify_response_create(uintptr_t                           client_tag,
                                                                      sim_uicc_status_code_t              uicc_status_code,
                                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                      sim_uicc_status_word_t              status_word);

uiccd_msg_status_t* uiccd_msg_pin_disable_response_create(uintptr_t                           client_tag,
                                                          sim_uicc_status_code_t              uicc_status_code,
                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                          sim_uicc_status_word_t              status_word);

uiccd_msg_status_t* uiccd_msg_pin_enable_response_create(uintptr_t                           client_tag,
                                                         sim_uicc_status_code_t              uicc_status_code,
                                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                         sim_uicc_status_word_t              status_word);

uiccd_msg_status_t* uiccd_msg_pin_unblock_response_create(uintptr_t                           client_tag,
                                                          sim_uicc_status_code_t              uicc_status_code,
                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                          sim_uicc_status_word_t              status_word);

typedef struct {
  STE_MSG_COMMON;
  uint32_t status;
  int server_status;
} uiccd_msg_server_status_t;

uiccd_msg_server_status_t* uiccd_msg_server_status_response_create(uintptr_t client_tag, uint32_t status, int startup_completed);

typedef uiccd_msg_server_status_t  uiccd_msg_sap_session_status_t;

// typedef uiccd_msg_int_t  uiccd_msg_app_list_t;
typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t status;
  int app_len; // Number of apps found on card
} uiccd_msg_app_list_t;

uiccd_msg_app_list_t* uiccd_msg_app_list_create(uintptr_t client_tag, sim_uicc_status_code_t status, int app_len);

typedef struct {
  STE_MSG_COMMON;
  int app_id;
  ste_sim_app_type_t app_type;
  ste_uicc_card_type_t card_type;
} uiccd_msg_card_fallback_t;

uiccd_msg_card_fallback_t* uiccd_msg_card_fallback_create(uintptr_t client_tag, ste_sim_app_type_t app_type, int app_id, ste_uicc_card_type_t card_type);

typedef struct {
  STE_MSG_COMMON;
  ste_uicc_status_t uicc_status;
  ste_sim_app_type_t app_type; // Only supplied for uicc_status==READY
  ste_uicc_card_type_t card_type; // Only supplied for uicc_status==INIT
} uiccd_msg_uicc_status_ind_t;

uiccd_msg_uicc_status_ind_t* uiccd_msg_uicc_status_ind_create(uintptr_t client_tag, ste_uicc_status_t uicc_status);

uiccd_msg_uicc_status_ind_t* uiccd_msg_uicc_status_payload_ind_create(uintptr_t client_tag, ste_uicc_status_t uicc_status, ste_sim_app_type_t app_type, ste_uicc_card_type_t card_type);

typedef struct {
  STE_MSG_COMMON;
  uint32_t status;
  ste_sim_card_status_t card_status;
} uiccd_msg_server_card_status_t;

uiccd_msg_server_card_status_t* uiccd_msg_server_card_status_create(uintptr_t client_tag, uint32_t status, ste_sim_card_status_t card_status);

typedef struct {
  STE_MSG_COMMON;
  ste_sim_card_status_t card_status;
} uiccd_msg_card_status_ind_t;

uiccd_msg_card_status_ind_t* uiccd_msg_card_status_ind_create(uintptr_t client_tag, ste_sim_card_status_t card_status);



typedef struct {
  STE_MSG_COMMON;
  uint32_t status;
  int app_id;
  ste_sim_app_type_t app_type;
} uiccd_msg_app_activate_t;

uiccd_msg_app_activate_t* uiccd_msg_app_activate_create(uintptr_t client_tag, uint32_t status, int app_id, ste_sim_app_type_t app_type);

typedef struct {
  STE_MSG_COMMON;
  uint32_t status;
} uiccd_msg_app_shutdown_init_t;

uiccd_msg_app_shutdown_init_t* uiccd_msg_app_shutdown_init_create(uintptr_t client_tag, uint32_t status);

typedef struct {
  STE_MSG_COMMON;
  uint32_t status;
  int app_id;
  int pin_id;
} uiccd_msg_pin_verify_needed_t;

// -----------------------------------------------------------------------------
// STOP Message
typedef struct {
    STE_MSG_COMMON;
    cat_barrier_t          *bar;
} uiccd_msg_stop_t;

uiccd_msg_stop_t       *uiccd_msg_stop_create(cat_barrier_t * bar, uintptr_t client_tag);

typedef struct {
    STE_MSG_COMMON;
    cn_message_t* cn_msg;
} uiccd_msg_cn_t;

uiccd_msg_cn_t *uiccd_msg_cn_response_create(cn_message_t *msg_p);

uiccd_msg_cn_t *uiccd_msg_cn_event_create(cn_message_t *msg_p);

// -----------------------------------------------------------------------------
// Generic data message
typedef struct {
    STE_MSG_COMMON;
    int                     fd;
    char                   *data;
    size_t                  len;
} uiccd_msg_data_t;

typedef uiccd_msg_data_t  uiccd_msg_pin_info_t;
uiccd_msg_pin_info_t   *uiccd_msg_pin_info_create(int fd,
                                                  uintptr_t client_tag,
                                                  const char *data,
                                                  size_t len);

typedef uiccd_msg_data_t uiccd_msg_pin_verify_t;
uiccd_msg_pin_verify_t *uiccd_msg_pin_verify_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_pin_change_t;
uiccd_msg_pin_change_t *uiccd_msg_pin_change_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);


typedef uiccd_msg_data_t uiccd_msg_pin_disable_t;
uiccd_msg_pin_disable_t *uiccd_msg_pin_disable_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);


typedef uiccd_msg_data_t uiccd_msg_pin_enable_t;
uiccd_msg_pin_enable_t *uiccd_msg_pin_enable_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_pin_unblock_t;
uiccd_msg_pin_unblock_t *uiccd_msg_pin_unblock_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_read_sim_file_record_t;
uiccd_msg_read_sim_file_record_t *uiccd_msg_read_sim_file_record_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_read_sim_file_binary_t;
uiccd_msg_read_sim_file_binary_t *uiccd_msg_read_sim_file_binary_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_update_sim_file_record_t;
uiccd_msg_update_sim_file_record_t *uiccd_msg_update_sim_file_record_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_update_sim_file_binary_t;
uiccd_msg_update_sim_file_binary_t *uiccd_msg_update_sim_file_binary_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_file_get_format_t;
uiccd_msg_sim_file_get_format_t *uiccd_msg_sim_file_get_format_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_get_file_information_t;
uiccd_msg_get_file_information_t *uiccd_msg_get_file_information_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_appl_apdu_send_t;

typedef uiccd_msg_data_t uiccd_msg_get_sim_state_t;
uiccd_msg_get_sim_state_t *uiccd_msg_get_sim_state_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_read_smsc_t;
uiccd_msg_read_smsc_t *uiccd_msg_read_smsc_create(int fd,
                                                  uintptr_t client_tag,
                                                  const char *data,
                                                  size_t len);

typedef uiccd_msg_data_t uiccd_msg_update_smsc_t;
uiccd_msg_update_smsc_t *uiccd_msg_update_smsc_create(int fd,
                                                      uintptr_t client_tag,
                                                      const char *data,
                                                      size_t len);

typedef uiccd_msg_data_t uiccd_msg_smsc_get_record_max_t;
uiccd_msg_smsc_get_record_max_t *uiccd_msg_smsc_get_record_max_create(int fd,
                                                                      uintptr_t client_tag,
                                                                      const char *data,
                                                                      size_t len);

typedef uiccd_msg_data_t uiccd_msg_smsc_save_to_record_t;
uiccd_msg_smsc_save_to_record_t *uiccd_msg_smsc_save_to_record_create(int fd,
                                                                      uintptr_t client_tag,
                                                                      const char *data,
                                                                      size_t len);

typedef uiccd_msg_data_t uiccd_msg_smsc_restore_from_record_t;
uiccd_msg_smsc_restore_from_record_t *uiccd_msg_smsc_restore_from_record_create(int fd,
                                                                                uintptr_t client_tag,
                                                                                const char *data,
                                                                                size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_file_read_generic_t;
uiccd_msg_sim_file_read_generic_t *uiccd_msg_sim_file_read_generic_create(int fd,
                                                                          uintptr_t client_tag,
                                                                          const char *data,
                                                                          size_t len);

typedef uiccd_msg_data_t uiccd_msg_update_service_table_t;
uiccd_msg_update_service_table_t *uiccd_msg_update_service_table_create(int fd,
                                                                        uintptr_t client_tag,
                                                                        const char *data,
                                                                        size_t len);

typedef uiccd_msg_data_t uiccd_msg_get_service_table_t;
uiccd_msg_get_service_table_t *uiccd_msg_get_service_table_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len);

typedef uiccd_msg_data_t uiccd_msg_get_service_availability_t;
uiccd_msg_get_service_availability_t *uiccd_msg_get_service_availability_create(int fd,
                                                                                uintptr_t client_tag,
                                                                                const char *data,
                                                                                size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_icon_read_t;
uiccd_msg_sim_icon_read_t *uiccd_msg_sim_icon_read_create(int fd,
                                                          uintptr_t client_tag,
                                                          const char *data,
                                                          size_t len);

typedef uiccd_msg_data_t uiccd_msg_subscriber_number_t;
uiccd_msg_subscriber_number_t *uiccd_msg_subscriber_number_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len);


typedef uiccd_msg_data_t uiccd_msg_read_plmn_t;
uiccd_msg_read_plmn_t *uiccd_msg_read_plmn_create(int fd,
                                                  uintptr_t client_tag,
                                                  const char *data,
                                                  size_t len);

typedef uiccd_msg_data_t uiccd_msg_update_plmn_t;
uiccd_msg_update_plmn_t *uiccd_msg_update_plmn_create(int fd,
                                                      uintptr_t client_tag,
                                                      const char *data,
                                                      size_t len);

typedef uiccd_msg_data_t uiccd_msg_app_status_t;
uiccd_msg_app_status_t *uiccd_msg_app_status_create(int fd,
                                                    uintptr_t client_tag,
                                                    const char *data,
                                                    size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_channel_send_t;
uiccd_msg_sim_channel_send_t *uiccd_msg_sim_channel_send_create(int fd,
                                                                uintptr_t client_tag,
                                                                const char *data,
                                                                size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_channel_open_t;
uiccd_msg_sim_channel_open_t *uiccd_msg_sim_channel_open_create(int fd,
                                                                uintptr_t client_tag,
                                                                const char *data,
                                                                size_t len);

typedef uiccd_msg_data_t uiccd_msg_sim_channel_close_t;
uiccd_msg_sim_channel_close_t *uiccd_msg_sim_channel_close_create(int fd,
                                                                  uintptr_t client_tag,
                                                                  const char *data,
                                                                  size_t len);


// -----------------------------------------------------------------------------
// Generic SAP message

typedef uiccd_msg_fd_t  uiccd_msg_sap_session_t;
typedef uiccd_msg_data_t uiccd_msg_sap_session_apdu_data_t;

uiccd_msg_sap_session_t*    uiccd_msg_sap_session_start_create(int fd, uintptr_t client_tag);
uiccd_msg_sap_session_t*    uiccd_msg_sap_session_exit_create(int fd, uintptr_t client_tag);
uiccd_msg_sap_session_t*    uiccd_msg_sap_session_ctrl_card_create(int fd, uintptr_t client_tag, UICCD_MSG_TYPES_t msgt);
uiccd_msg_sap_session_apdu_data_t*  uiccd_msg_sap_session_apdu_data_create(int fd,
                                                                                     uintptr_t client_tag,
                                                                                     const char *data,
                                                                                     size_t len);
uiccd_msg_sap_session_t*    uiccd_msg_sap_session_status_get_create(int fd, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// Generic data and int message
typedef struct {
    STE_MSG_COMMON;
    uint8_t                            *data;
    size_t                              len;
    sim_uicc_status_code_t              uicc_status_code;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
    sim_uicc_status_word_t              status_word;
} uiccd_msg_char_data_status_t;

//FIXME: Remove handle of data and data_len, not used in this response.
typedef uiccd_msg_char_data_status_t  uiccd_msg_update_sim_file_record_response_t;
uiccd_msg_update_sim_file_record_response_t* uiccd_msg_update_sim_file_record_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len);

//FIXME: Remove handle of data and data_len, not used in this response.
typedef uiccd_msg_char_data_status_t  uiccd_msg_update_sim_file_binary_response_t;
uiccd_msg_update_sim_file_binary_response_t* uiccd_msg_update_sim_file_binary_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len);

typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t              uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  sim_uicc_status_word_t              status_word;
  sim_file_structure_t                file_type;
  int                                 file_size;
  int                                 record_len;
  int                                 num_records;
} uiccd_msg_sim_file_get_format_response_t;

uiccd_msg_sim_file_get_format_response_t*
uiccd_msg_sim_file_get_format_response_create(uintptr_t                           client_tag,
                                              sim_uicc_status_code_t              uicc_status_code,
                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                              sim_uicc_status_word_t              status_word,
                                              sim_file_structure_t                file_type,
                                              int                                 file_size,
                                              int                                 record_len,
                                              int                                 num_records);

typedef uiccd_msg_char_data_status_t  uiccd_msg_get_file_information_response_t;
uiccd_msg_get_file_information_response_t*
uiccd_msg_get_file_information_response_create(uintptr_t                           client_tag,
                                               sim_uicc_status_code_t              uicc_status_code,
                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                               sim_uicc_status_word_t              status_word,
                                               uint8_t*                            data,
                                               size_t                              len);

typedef uiccd_msg_status_t uiccd_msg_sim_connect_response_t;
uiccd_msg_sim_connect_response_t*
uiccd_msg_sim_connect_response_create(uintptr_t                           client_tag,
                                      sim_uicc_status_code_t              uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details);

typedef uiccd_msg_status_t uiccd_msg_sim_disconnect_response_t;
uiccd_msg_sim_disconnect_response_t*
uiccd_msg_sim_disconnect_response_create(uintptr_t                           client_tag,
                                         sim_uicc_status_code_t              uicc_status_code,
                                         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details);

// -----------------------------------------------------------------------------


// Generic integer data and status message
typedef struct {
    STE_MSG_COMMON;
    uint8_t                            *data;
    size_t                              len;
    sim_uicc_status_code_t              uicc_status_code;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
    sim_uicc_status_word_t              status_word;
} uiccd_msg_byte_data_status_t;


typedef uiccd_msg_byte_data_status_t  uiccd_msg_read_sim_file_record_response_t;
uiccd_msg_read_sim_file_record_response_t* uiccd_msg_read_sim_file_record_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len);


typedef uiccd_msg_byte_data_status_t  uiccd_msg_read_sim_file_binary_response_t;
uiccd_msg_read_sim_file_binary_response_t* uiccd_msg_read_sim_file_binary_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len);

typedef uiccd_msg_byte_data_status_t  uiccd_msg_sim_icon_read_response_t;
uiccd_msg_sim_icon_read_response_t *uiccd_msg_sim_icon_read_response_create(
                                                   uintptr_t                           client_tag,
                                                   sim_uicc_status_code_t              uicc_status_code,
                                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                   sim_uicc_status_word_t              status_word,
                                                   const uint8_t*                      data,
                                                   size_t                              len);

typedef uiccd_msg_byte_data_status_t  uiccd_msg_subscriber_number_response_t;
uiccd_msg_subscriber_number_response_t *uiccd_msg_subscriber_number_response_create(
                                                   uintptr_t                           client_tag,
                                                   sim_uicc_status_code_t              uicc_status_code,
                                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                   sim_uicc_status_word_t              status_word,
                                                   const uint8_t*                      data,
                                                   size_t                              len);

typedef uiccd_msg_byte_data_status_t  uiccd_msg_sim_channel_send_response_t;
uiccd_msg_sim_channel_send_response_t*
uiccd_msg_sim_channel_send_response_create(uintptr_t                           client_tag,
                                           sim_uicc_status_code_t              uicc_status_code,
                                           sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                           sim_uicc_status_word_t              status_word,
                                           const uint8_t*                      data,
                                           size_t                              len);

typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t              uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  sim_uicc_status_word_t              status_word;
  uint16_t                            session_id;
} uiccd_msg_sim_channel_open_response_t;
uiccd_msg_sim_channel_open_response_t*
uiccd_msg_sim_channel_open_response_create(uintptr_t                           client_tag,
                                           sim_uicc_status_code_t              uicc_status_code,
                                           sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                           sim_uicc_status_word_t              status_word,
                                           uint16_t                            session_id);

typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t              uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  sim_uicc_status_word_t              status_word;
} uiccd_msg_sim_channel_close_response_t;
uiccd_msg_sim_channel_close_response_t*
uiccd_msg_sim_channel_close_response_create(uintptr_t                           client_tag,
                                            sim_uicc_status_code_t              uicc_status_code,
                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                            sim_uicc_status_word_t              status_word);

typedef uiccd_msg_byte_data_status_t  uiccd_msg_appl_apdu_send_response_t;
uiccd_msg_appl_apdu_send_response_t* uiccd_msg_appl_apdu_send_response_create(uintptr_t                           client_tag,
                                                                              sim_uicc_status_code_t              uicc_status_code,
                                                                              sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                              sim_uicc_status_word_t              status_word,
                                                                              const uint8_t*                      data,
                                                                              size_t                              len);

// -----------------------------------------------------------------------------

typedef struct {
  STE_MSG_COMMON;
  sim_uicc_status_code_t              uicc_status_code;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details;
  ste_sim_pin_status_t                pin_status;
  int                                 pin_att;
  int                                 attempts;
  int                                 attempts2;
} uiccd_msg_pin_info_rsp_t;



typedef uiccd_msg_pin_info_rsp_t  uiccd_msg_pin_info_response_t;
uiccd_msg_pin_info_response_t* uiccd_msg_pin_info_response_create(uintptr_t                           client_tag,
                                                                  sim_uicc_status_code_t              uicc_status_code,
                                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                  ste_sim_pin_status_t                pin_status,
                                                                  int                                 attempts,
                                                                  int                                 attempts2);


//typedef enum {
//    UICCD_MODEM_ALIVE,
//    UICCD_MODEM_DEAD
//} uiccd_modem_state_t;
// TODO: more restructuring required!
// TODO: This "everything is an int" interface we have is trouble for later
typedef struct {
  STE_MSG_COMMON;
  int status;
} uiccd_msg_modem_state_t;
uiccd_msg_modem_state_t* uiccd_msg_modem_state_create(uintptr_t client_tag,
                                                      int status);

typedef struct {
  STE_MSG_COMMON;
} uiccd_msg_not_ready_t;
uiccd_msg_not_ready_t* uiccd_msg_not_ready_create(uintptr_t client_tag);

typedef struct {
  STE_MSG_COMMON;
  ste_reason_t reason;
} uiccd_msg_sim_status_t;
uiccd_msg_sim_status_t* uiccd_msg_sim_status_create(uintptr_t client_tag,
                                                    ste_reason_t reason);
// -----------------------------------------------------------------------------
// Generic data and int message
typedef struct {
    STE_MSG_COMMON;
    int                     status;
    char                   *data;
    size_t                  len;
} uiccd_msg_read_smsc_response_t;

uiccd_msg_read_smsc_response_t* uiccd_msg_read_smsc_response_create(uintptr_t client_tag,
                                                                    int status,
                                                                    const char* data,
                                                                    size_t len);

typedef struct {
  STE_MSG_COMMON;
  int status;
} uiccd_msg_update_smsc_response_t;

uiccd_msg_update_smsc_response_t* uiccd_msg_update_smsc_response_create(uintptr_t client_tag,
                                                                        int status,
                                                                        const char* data,
                                                                        size_t len);

// -----------------------------------------------------------------------------
// Generic integer data and status message

// Generic data and int message
typedef uiccd_msg_byte_data_status_t uiccd_msg_sim_file_read_generic_response_t;
uiccd_msg_sim_file_read_generic_response_t* uiccd_msg_sim_file_read_generic_response_create(
                                                                          uintptr_t                           client_tag,
                                                                          sim_uicc_status_code_t              uicc_status_code,
                                                                          sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                          sim_uicc_status_word_t              status_word,
                                                                          const uint8_t*                      data,
                                                                          size_t                              len);



uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_start_response_create(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_exit_response_create(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_deactivate_create_response(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_activate_create_response(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_coldreset_create_response(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_ctrlcard_warmreset_create_response(uintptr_t client_tag, int status);
uiccd_msg_sap_session_status_t* uiccd_msg_sap_session_reset_ind_create_response(uintptr_t client_tag, int status);
uiccd_msg_byte_data_status_t* uiccd_msg_sap_session_atr_get_create_response(uintptr_t client_tag,
                                                                            sim_uicc_status_code_t              uicc_status_code,
                                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                            sim_uicc_status_word_t              status_word,
                                                                            const uint8_t* data,
                                                                            size_t len);
uiccd_msg_byte_data_status_t* uiccd_msg_sap_session_apdu_data_create_response(uintptr_t client_tag,
                                                                            sim_uicc_status_code_t              uicc_status_code,
                                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                            sim_uicc_status_word_t              status_word,
                                                                            const uint8_t* data,
                                                                            size_t len);

#endif
