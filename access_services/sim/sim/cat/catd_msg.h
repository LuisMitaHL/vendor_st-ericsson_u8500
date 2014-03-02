/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_msg.h
 * Description     : message handling
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


// FIXME: Rename catd_msg* sim_msg*

#ifndef __catd_msg_h__
#define __catd_msg_h__ (1)


#include "catd.h"
#include "msgq.h"
#include "cat_barrier.h"


// -----------------------------------------------------------------------------
// Message types

typedef enum {
    CATD_MSG_NONE                               = STE_MSG_RANGE_CATD,
    CATD_MSG_CONNECT,
    CATD_MSG_DISCONNECT,
    CATD_MSG_STARTUP,
    CATD_MSG_SHUTDOWN,
    CATD_MSG_STOP,
    CATD_MSG_REGISTER,
    CATD_MSG_APDU,
    CATD_MSG_TR,
    CATD_MSG_MODEM_READY,
    CATD_MSG_EC,
    CATD_MSG_MODEM_NOT_READY,
    CATD_MSG_TPDL_STATUS,
    CATD_MSG_TPDL,
    CATD_MSG_CALL_CONTROL,
    CATD_MSG_MODEM_CAT_RESPONSE,
    CATD_MSG_DEREGISTER,
    CATD_MSG_CAT_SESSION_EXPIRED,
    CATD_MSG_SET_TERMINAL_PROFILE,
    CATD_MSG_GET_TERMINAL_PROFILE,
    CATD_MSG_GET_CAT_STATUS,
    CATD_MSG_ANSWER_CALL,
    CATD_MSG_EVENT_DOWNLOAD,
    CATD_MSG_SMS_CONTROL,
    CATD_MSG_MODEM_SET_POLL_INTERVAL_RESPONSE,
    CATD_MSG_MODEM_ENABLE_STATUS,
    CATD_MSG_MODEM_DISABLE_STATUS,
    CATD_MSG_CN_RESPONSE,
    CATD_MSG_CN_EVENT,
    CATD_MSG_SMS_CB_PERFORM_CONNECT,
    CATD_MSG_SMS_CB_RESPONSE,
    CATD_MSG_SMS_CB_EVENT,
    CATD_MSG_UICCD_RESPONSE,
    CATD_MSG_TIMER_EXPIRATION_SEND_EC,
    CATD_MSG_PC_REFRESH,
    CATD_MSG_MODEM_REFRESH_STATUS,
    CATD_MSG_MODEM_REFRESH_IND,
    CATD_MSG_TIMEOUT_EVENT,
    CATD_MSG_CAT_ENABLE,
    CATD_MSG_CAT_DISABLE,
    CATD_MSG_MODEM_CAT_NOT_SUPPORTED,
    CATD_MSG_ADAPT_TERMINAL_SUPPORT_TABLE_UPDATE_STATUS,
    CATD_MSG_APP_READY,
    CATD_MSG_CAT_SERVER_READY,
    CATD_MSG_SIM_POWER_ON_OFF,
    CATD_MSG_RESET,
    CATD_MSG_MODEM_RESET,
    CATD_MSG_MODEM_IND_REG_OK
} CATD_MSG_TYPES_t;




// -----------------------------------------------------------------------------
// General empty messages

typedef ste_msg_void_t  catd_msg_modem_ready_t;
catd_msg_modem_ready_t *catd_msg_modem_ready_create(uintptr_t client_tag);

typedef ste_msg_void_t  catd_msg_modem_not_ready_t;
catd_msg_modem_not_ready_t *catd_msg_modem_not_ready_create(uintptr_t client_tag);

typedef ste_msg_void_t  catd_msg_modem_reset_t;
catd_msg_modem_reset_t *catd_msg_modem_reset_create(uintptr_t client_tag);

typedef ste_msg_void_t  catd_msg_modem_ind_reg_ok_t;
catd_msg_modem_ind_reg_ok_t *catd_msg_modem_ind_reg_ok_create(uintptr_t client_tag);

typedef ste_msg_void_t catd_msg_timeout_event_t;
catd_msg_timeout_event_t *catd_msg_timeout_event_create(uintptr_t timeout_event);

typedef ste_msg_void_t  catd_msg_modem_cat_not_supported_t;
catd_msg_modem_cat_not_supported_t *catd_msg_modem_cat_not_supported_create(uintptr_t client_tag);

typedef ste_msg_void_t catd_msg_cat_server_ready_t;
catd_msg_cat_server_ready_t *catd_msg_cat_server_ready_create(uintptr_t client_tag);

typedef ste_msg_void_t catd_msg_reset_t;
catd_msg_reset_t *catd_msg_reset_create(uintptr_t client_tag);

// -----------------------------------------------------------------------------
// General data messages
typedef struct {
    STE_MSG_COMMON;
    int                     fd;
    uint8_t               * buf_p;
    uint16_t                len;
} catd_msg_data_t;

// -----------------------------------------------------------------------------
// Generic file descriptor messages
typedef struct {
    STE_MSG_COMMON;
    int                     fd;
} catd_msg_fd_t;

typedef catd_msg_fd_t   catd_msg_connect_t;
typedef catd_msg_fd_t   catd_msg_disconnect_t;
typedef catd_msg_fd_t   catd_msg_startup_t;
typedef catd_msg_fd_t   catd_msg_shutdown_t;
typedef catd_msg_fd_t   catd_msg_deregister_t;

catd_msg_connect_t     *catd_msg_connect_create(int fd, uintptr_t client_tag);
catd_msg_disconnect_t  *catd_msg_disconnect_create(int fd, uintptr_t client_tag);
catd_msg_startup_t     *catd_msg_startup_create(int fd, uintptr_t client_tag);
catd_msg_shutdown_t    *catd_msg_shutdown_create(int fd, uintptr_t client_tag);
catd_msg_deregister_t  *catd_msg_deregister_create(int fd, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// Register messages
typedef catd_msg_data_t   catd_msg_register_t;

catd_msg_register_t    *catd_msg_register_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// STOP Message
typedef struct {
    STE_MSG_COMMON;
    cat_barrier_t          *bar;
} catd_msg_stop_t;

catd_msg_stop_t        *catd_msg_stop_create(cat_barrier_t * bar, uintptr_t client_tag);


// -----------------------------------------------------------------------------
// Generic int messages

typedef struct {
    STE_MSG_COMMON;
    int                     i;
} catd_msg_int_t;

typedef struct {
    STE_MSG_COMMON;
    boolean adapt_file_updated;
    sim_uicc_status_word_t status_word;
} catd_msg_adapt_terminal_support_table_update_status_t;

typedef catd_msg_int_t catd_msg_tpdl_status_t;

catd_msg_tpdl_status_t *catd_msg_tpdl_status_create(uintptr_t client_tag, int status);

catd_msg_adapt_terminal_support_table_update_status_t *catd_msg_adapt_terminal_support_table_update_create(uintptr_t client_tag, int status, sim_uicc_status_word_t status_word);

typedef catd_msg_int_t catd_msg_enable_status_t;

catd_msg_enable_status_t *catd_msg_enable_status_create(uintptr_t client_tag, int status);

typedef catd_msg_int_t  catd_msg_disable_status_t;

catd_msg_disable_status_t *catd_msg_disable_status_create(uintptr_t client_tag, int status);

typedef catd_msg_int_t  catd_msg_modem_refresh_status_t;

catd_msg_modem_refresh_status_t *catd_msg_modem_refresh_status_create(uintptr_t client_tag, int status);

typedef catd_msg_int_t  catd_msg_app_ready_t;

catd_msg_app_ready_t *catd_msg_app_ready_create(uintptr_t client_tag, int i);

typedef catd_msg_int_t  catd_msg_sim_power_on_off_t;

catd_msg_sim_power_on_off_t *catd_msg_sim_power_on_off_create(uintptr_t client_tag, int power_on);

// -----------------------------------------------------------------------------
// APDU messages


typedef struct {
    STE_MSG_COMMON;
    ste_apdu_t             *apdu;
} catd_msg_apdu_t;

catd_msg_apdu_t        *catd_msg_apdu_create(ste_apdu_t * apdu, uintptr_t client_tag);


// -----------------------------------------------------------------------------
// TR messages

typedef struct {
    STE_MSG_COMMON;
    int                     fd;
    ste_apdu_t             *apdu;
} catd_msg_tr_t;

catd_msg_tr_t          *catd_msg_tr_create(int fd, ste_apdu_t * apdu, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// EC messages

typedef struct {
    STE_MSG_COMMON;
    int                     fd;
    ste_apdu_t             *apdu;
} catd_msg_ec_t;

catd_msg_ec_t          *catd_msg_ec_create(int fd, ste_apdu_t * apdu, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CALL NETWORKING messages

typedef struct {
    STE_MSG_COMMON;
    cn_message_t* cn_msg;
} catd_msg_cn_t;

catd_msg_cn_t *
catd_msg_cn_create(cn_message_t * cn_msg);


// -----------------------------------------------------------------------------
// CAT response messages

typedef struct {
    STE_MSG_COMMON;
    ste_apdu_t             *apdu;
    int                    status;
} catd_msg_modem_cat_response_t;

catd_msg_modem_cat_response_t *
catd_msg_modem_cat_response_create(int status, ste_apdu_t * apdu, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT set poll interval response message

typedef struct {
    STE_MSG_COMMON;
    int status;
    int interval;
} catd_msg_modem_set_poll_interval_response_t;

catd_msg_modem_set_poll_interval_response_t *
catd_msg_modem_set_poll_interval_response_create(int status, int interval, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT SMS/CB signal messages

typedef struct {
    STE_MSG_COMMON;
} catd_msg_sms_cb_perform_connect_t;

catd_msg_sms_cb_perform_connect_t* catd_msg_sms_cp_perform_connect_create(uintptr_t client_tag);

typedef struct {
    STE_MSG_COMMON;
    uint32_t primitive;
    void*    signal;
} catd_msg_sms_cb_signal_t;

catd_msg_sms_cb_signal_t* catd_msg_sms_cb_response_create(void* signal, uint32_t primitive, uintptr_t client_tag);
catd_msg_sms_cb_signal_t* catd_msg_sms_cb_event_create(void* signal, uint32_t primitive, uintptr_t client_tag);

typedef struct {
    STE_MSG_COMMON;
    uint16_t command;
    char*    response;
    ssize_t  response_len;
} catd_msg_uiccd_response_t;

catd_msg_uiccd_response_t* catd_msg_uiccd_response_create(char* response, ssize_t response_len,
                                                          uint16_t command, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// Call Control messages

typedef catd_msg_data_t   catd_msg_call_control_t;

catd_msg_call_control_t *
catd_msg_call_control_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// SET TERMINAL PROFILE messages

typedef catd_msg_data_t   catd_msg_set_terminal_profile_t;

catd_msg_set_terminal_profile_t *
catd_msg_set_terminal_profile_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// GET TERMINAL PROFILE messages
typedef catd_msg_fd_t   catd_msg_get_terminal_profile_t;

catd_msg_get_terminal_profile_t  *catd_msg_get_terminal_profile_create(int fd, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// GET CAT STATUS messages
typedef catd_msg_fd_t   catd_msg_get_cat_status_t;

catd_msg_get_cat_status_t  *catd_msg_get_cat_status_create(int fd, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT session expired messages

typedef struct {
    STE_MSG_COMMON;
    int                     session_id;
} catd_msg_cat_session_expired_t;

catd_msg_cat_session_expired_t *
catd_msg_cat_session_expired_create(int session_id, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT answer call messages

typedef catd_msg_data_t  catd_msg_answer_call_t;

catd_msg_answer_call_t *catd_msg_answer_call_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT event download messages

typedef catd_msg_data_t  catd_msg_event_download_t;

catd_msg_event_download_t *catd_msg_event_download_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// SMS Control messages

typedef catd_msg_data_t   catd_msg_sms_control_t;

catd_msg_sms_control_t *
catd_msg_sms_control_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT setup call result messages

typedef catd_msg_data_t  catd_msg_setup_call_result_t;

catd_msg_setup_call_result_t *catd_msg_setup_call_result_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// CAT cat enable messages

typedef catd_msg_data_t  catd_msg_cat_enable_t;

catd_msg_cat_enable_t *catd_msg_cat_enable_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

// -----------------------------------------------------------------------------
// Timer expiration data messages.
typedef struct {
    STE_MSG_COMMON;
    uint16_t timer_info;
} catd_msg_timeout_t;

catd_msg_timeout_t *
catd_msg_timer_expiration_ec_create(const uint16_t timer_info);

// -----------------------------------------------------------------------------
// CAT MODEM REFRESH IND message

typedef struct {
    STE_MSG_COMMON;
    ste_apdu_refresh_type_t     refresh_type;
    ste_cat_refresh_ind_type_t  ind_type;
    uint8_t                     app_id;
} catd_msg_modem_refresh_ind_t;

catd_msg_modem_refresh_ind_t *
catd_msg_modem_refresh_ind_create(uintptr_t client_tag,
                                  ste_apdu_refresh_type_t refresh_type,
                                  ste_cat_refresh_ind_type_t ind_type,
                                  uint8_t app_id);


#endif
