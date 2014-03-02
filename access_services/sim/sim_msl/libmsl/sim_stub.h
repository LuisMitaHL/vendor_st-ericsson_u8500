/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_stub.h
 * Description     : Simulated modem header file.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */



#ifndef __sim_stub_h__
#define __sim_stub_h__ (1)

#include <stdint.h>
#include <stdlib.h>

//CMD from modem simulator
typedef enum {
    SIM_MODEM_CMD_MODEM_DISCONNECT = 1,
    SIM_MODEM_CMD_PC_DISPLAY_TEXT,
    SIM_MODEM_CMD_PC_SETUP_MENU,
    SIM_MODEM_CMD_PC_SELECT_ITEM,
    SIM_MODEM_CMD_PC_GET_INKEY,
    SIM_MODEM_CMD_PC_GET_INPUT,
    SIM_MODEM_CMD_PC_SETUP_EVENT_LIST,
    SIM_MODEM_CMD_PC_SETUP_EVENT_LIST_01,
    SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE,
    SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_01,
    SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_02,
    SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_03,
    SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_04,
    SIM_MODEM_CMD_PC_SEND_DTMF,
    SIM_MODEM_CMD_PC_SEND_DTMF_01,
    SIM_MODEM_CMD_PC_SEND_DTMF_02,
    SIM_MODEM_CMD_PC_SEND_SS,
    SIM_MODEM_CMD_PC_SEND_SS_01,
    SIM_MODEM_CMD_PC_SEND_SS_02,
    SIM_MODEM_CMD_PC_SEND_SS_03,
    SIM_MODEM_CMD_PC_SEND_SS_04,
    SIM_MODEM_CMD_PC_SEND_SS_05,
    SIM_MODEM_CMD_PC_SEND_SS_06,
    SIM_MODEM_CMD_PC_SEND_SS_07,
    SIM_MODEM_CMD_PC_SEND_SS_08,
    SIM_MODEM_CMD_PC_SEND_SS_09,
    SIM_MODEM_CMD_PC_SEND_USSD,
    SIM_MODEM_CMD_PC_SEND_USSD_01,
    SIM_MODEM_CMD_PC_SEND_USSD_02,
    SIM_MODEM_CMD_PC_SEND_USSD_03,
    SIM_MODEM_CMD_PC_SEND_USSD_04,
    SIM_MODEM_CMD_PC_SEND_USSD_05,
    SIM_MODEM_CMD_PC_SEND_USSD_06,
    SIM_MODEM_CMD_PC_SEND_USSD_07,
    SIM_MODEM_CMD_PC_SEND_USSD_08,
    SIM_MODEM_CMD_PC_SEND_USSD_09,
    SIM_MODEM_CMD_PC_SETUP_CALL,
    SIM_MODEM_CMD_PC_SETUP_CALL_00,
    SIM_MODEM_CMD_PC_SETUP_CALL_01,
    SIM_MODEM_CMD_PC_SETUP_CALL_02,
    SIM_MODEM_CMD_PC_SETUP_CALL_03,
    SIM_MODEM_CMD_PC_SETUP_CALL_04,
    SIM_MODEM_CMD_PC_SETUP_ECALL,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_01,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_02,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_03,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_04,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_05,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_06,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_07,
    SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_08,
    SIM_MODEM_CMD_PC_SETUP_CALL_HOLD,
    SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_01,
    SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_02,
    SIM_MODEM_CMD_PC_POLL_INTERVAL,
    SIM_MODEM_CMD_PC_POLL_INTERVAL_01,
    SIM_MODEM_CMD_PC_POLL_INTERVAL_02,
    SIM_MODEM_CMD_PC_POLLING_OFF,
    SIM_MODEM_CMD_PC_POLLING_OFF_01,
    SIM_MODEM_CMD_PC_POLLING_OFF_02,
    SIM_MODEM_CMD_PC_MORE_TIME,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_01,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_02,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_03,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_04,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_05,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_06,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_07,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_08,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_09,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_10,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_11,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_12,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_13,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_14,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_15,
    SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_16,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LOCATION,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEI,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEISV,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_BATTERY,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NW_SEARCH_MODE,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_TIMING_ADVANCE,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_ACCESS_TECHNOLOGY,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NMR,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DATE_TIME,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LANGUAGE,
    SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DEFAULT,
    SIM_MODEM_CMD_IND_MODEM_RESET,
    SIM_MODEM_CMD_CAT_NOT_READY_IND,
    SIM_MODEM_CMD_CAT_NOT_SUPPORTED,
    SIM_MODEM_CMD_CARD_REMOVED_IND,
    SIM_MODEM_CMD_CARD_SHUTDOWN,
    SIM_MODEM_CMD_PC_REFRESH,
    SIM_MODEM_CMD_REFRESH_RESET_IND,
    SIM_MODEM_CMD_PC_REFRESH_01,
    SIM_MODEM_CMD_PC_REFRESH_02,
    SIM_MODEM_CMD_PC_REFRESH_03,
    SIM_MODEM_CMD_PC_REFRESH_04,
    SIM_MODEM_CMD_PC_REFRESH_RESET,
    SIM_MODEM_CMD_PC_REFRESH_RESET_01,
    SIM_MODEM_CMD_PC_REFRESH_RESET_02,
    SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION,
    SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_01,
    SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_02,
    SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_03,
    SIM_MODEM_CMD_PC_LANGUAGE_NOTIFICATION,
    SIM_MODEM_CMD_CARD_FALLBACK,
    SIM_MODEM_CMD_SET_UICC_FAIL_TESTS,
    SIM_MODEM_CMD_RESET_UICC_FAIL_TESTS,
    SIM_MODEM_CMD_PC_OPEN_CHANNEL,
    SIM_MODEM_CMD_PC_CLOSE_CHANNEL,
    SIM_MODEM_CMD_PC_SEND_DATA,
    SIM_MODEM_CMD_PC_SEND_DATA_01,
    SIM_MODEM_CMD_PC_RECEIVE_DATA,
    SIM_MODEM_CMD_SEND_CAT_READY,
    SIM_MODEM_CMD_SEND_UICC_READY
} sim_modem_cmd_t;

typedef enum ste_sim_stub_resp_type_t {
  STE_SIM_STUB_RSP_TYPE_SUCCESS, // Success
  STE_SIM_STUB_RSP_TYPE_FAILURE, // Failure
  // CN REQ failure
  STE_SIM_STUB_RSP_TYPE_CN_REQ_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_DURATION,
  STE_SIM_STUB_RSP_TYPE_CN_REQ_HANGUP_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_REQ_HOLD_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_REQ_CALL_LIST_FAILURE,
  // CN RSP Failure
  STE_SIM_STUB_RSP_TYPE_CN_RESP_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_RESP_DIAL_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_RESP_HANGUP_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_RESP_HOLD_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_RESP_CC_UNKNOWN,
  // Modem Req failure
  STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE,
  // Modem Rsp failure
  STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE,
  // Call Control Failure
  STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_CC_TEMP_FAILURE,
  STE_SIM_STUB_SEND_USDD_AFTER_CC,
  STE_SIM_STUB_SEND_USDD_AFTER_CC_ERROR,
  // SS USSD Specific failures
  STE_SIM_STUB_RSP_TYPE_ModMM_FAILURE,
  STE_SIM_STUB_RSP_TYPE_ModCC_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_SS_BUSY_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_SS_NOT_AVAILABLE_FAILURE,
  STE_SIM_STUB_RSP_TYPE_CN_USSD_BUSY_FAILURE,
  // Poll Failure
  STE_SIM_STUB_SEND_NON_ZERO_POLL_INTERVAL,
  // Event Config
  STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_GPRS_CELL,
  STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_ON_WCDMA_CELL,
  STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_NO_COVERAGE,
  STE_SIM_STUB_SEND_CN_CELL_INFO_EVENT_LIMITED_COVERAGE,
  STE_SIM_STUB_SEND_CN_REG_INFO_MANUAL_MODE,
  STE_SIM_STUB_SEND_CN_REG_INFO_AUTO_MODE,
  // Hold and send
  STE_SIM_STUB_HOLD_TIMER_EC_RESPONSE_TILL_TR_RESPONSE,
  STE_SIM_STUB_HOLD_TR_RESPONSE_FOR_ONE_MINUTE
}ste_sim_stub_resp_type_t;


typedef void            ste_sim_stub_callback(int cause, const void *data, void *user_data, uintptr_t client_tag);

typedef struct
{
    ste_sim_stub_callback   *func;
    void                    *user_data;
} sim_stub_closure_t;

typedef struct
{
    uint8_t status;
} sim_stub_uicc_status_t;

typedef struct
{
    uint8_t status;
} sim_stub_cat_status_t;

typedef struct
{
    uint8_t status;
} sim_stub_card_status_t;

//####################################################################################
//functions defined in socket file
//####################################################################################
typedef struct sim_stub_socket_s sim_stub_socket_t;

sim_stub_socket_t* sim_stub_socket_new();

void sim_stub_socket_delete(sim_stub_socket_t * stub_socket_p);

int sim_stub_socket_start(sim_stub_socket_t * stub_socket_p);

int sim_stub_socket_stop(sim_stub_socket_t * stub_socket_p);

void sim_stub_socket_terminal_response_done();

//####################################################################################
//functions defined in stub file
//####################################################################################
void ste_stub_set_response_type( ste_sim_stub_resp_type_t type );

void ste_stub_pc_display_text();

void ste_stub_pc_setup_menu();

void ste_stub_pc_select_item();

void ste_stub_pc_setup_event_list();

void ste_stub_pc_setup_event_list_no_list();

void ste_stub_pc_provide_local_info(uint8_t command_qualifier);

void ste_stub_pc_get_input();

void ste_stub_pc_get_inkey();

void ste_stub_pc_send_short_message();

void ste_stub_pc_send_short_message_packing_required();

void ste_stub_pc_send_short_message_wrong_dcs();

void ste_stub_pc_send_dtmf();

void ste_stub_pc_send_ss();

void ste_stub_pc_send_ss_no_string();

void ste_stub_pc_send_ussd();

void ste_stub_pc_send_ussd_no_string();

void ste_stub_pc_setup_call(uint8_t apdu_option);

void ste_stub_pc_setup_call_with_sub_addr(uint8_t apdu_option);

void ste_stub_pc_poll_interval();

void ste_stub_pc_polling_off();

void ste_stub_pc_more_time();

int ste_stub_pc_timer_management_01();

int ste_stub_pc_timer_management_02();

int ste_stub_pc_timer_management_03();

int ste_stub_pc_timer_management_04();

int ste_stub_pc_timer_management_05();

int ste_stub_pc_timer_management_06();

int ste_stub_pc_timer_management_07();

int ste_stub_pc_timer_management_08();

int ste_stub_pc_timer_management_09();

int ste_stub_pc_timer_management_10();

int ste_stub_pc_timer_management_11();

int ste_stub_pc_timer_management_12();

int ste_stub_pc_timer_management_13();

int ste_stub_pc_timer_management_14();

int ste_stub_pc_timer_management_15();

void ste_stub_pc_refresh();

void ste_stub_pc_refresh_no_aid();

void ste_stub_pc_refresh_no_file_list();

void ste_stub_ind_modem_reset();

void ste_stub_pc_language_notification();

void ste_stub_pc_refresh_opt(unsigned char opt);

void ste_stub_pc_refresh_opt_with_aid(unsigned char opt);

void ste_stub_card_fallback_indication();

void ste_stub_pc_open_channel();

void ste_stub_pc_close_channel();

void ste_stub_pc_send_data();

void ste_stub_pc_receive_data();

void ste_stub_send_cat_ready();

void ste_stub_send_uicc_ready();

#endif
