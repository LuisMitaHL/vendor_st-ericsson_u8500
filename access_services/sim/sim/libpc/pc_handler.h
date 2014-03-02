/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_handler.h
 * Description     : Handler function for proactive commands declaration.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */

#ifndef __pc_handler_h__
#define __pc_handler_h__ (1)

void catd_handle_pc_provide_local_info(catd_msg_apdu_t  * msg);

/*************************************************************************
 * @brief:    Handle the send short message pro-active command.
 * @params:
 *            msg               : The original message with APDU data, and client_tag, etc.
 *            gui_capable_client: True if an external client that implements
 *                                notifications has indicated that it has a GUI.
 *
 * @return: -1 if error
 *           0 ok, caller may send PC notification to client
 *           1 ok, caller must not PC send notification to client (will be taken care of later)
 *
 * Notes:
 *************************************************************************/
int  catd_handle_pc_send_short_message(catd_msg_apdu_t* msg, boolean gui_capable_client);

int catd_handle_pc_send_ss(catd_msg_apdu_t  * msg, boolean gui_capable_client);

int catd_handle_pc_send_ussd(catd_msg_apdu_t* msg, boolean gui_capable_client);
void catd_handle_pc_send_ussd_event_handler(ste_msg_t* ste_msg);

int  catd_handle_pc_send_dtmf(catd_msg_apdu_t* msg, boolean gui_capable_client);

void catd_handle_pc_setup_call(catd_msg_apdu_t  * msg);

int catd_handle_pc_setup_event_list(catd_msg_apdu_t  * msg);

void catd_handle_pc_poll_interval(catd_msg_apdu_t* msg);

void catd_handle_pc_polling_off(catd_msg_apdu_t* msg);

void catd_handle_pc_more_time(catd_msg_apdu_t  * msg);

void catd_handle_pc_timer_management(catd_msg_apdu_t  * msg);

int catd_timer_create_signal_handler(void);

void catd_timer_reset_expiration_data(const uint16_t timer_info);

void catd_timer_reset_ec_started_flag(const uint16_t timer_info);

ste_apdu_t *catd_timer_create_expiration_ec(const uint16_t timer_info);

boolean catd_timer_check_outstanding_timer_expiration_ec(const uint16_t timer_info);

int catd_timer_check_for_ec_retransmission(uint16_t *timer_info_p, ste_apdu_t **apdu_p);

void catd_timer_clean_up_data(void);

void catd_handle_pc_setup_call_answer(int answer);

int catd_handle_pc_refresh(catd_msg_apdu_t  * msg);

void catd_handle_event(ste_msg_t* ste_msg);

cn_call_state_t catd_get_current_call_state(void);

void catd_handle_cn_event_pc_setup_call(ste_msg_t* ste_msg);

void set_stash_for_cc_ussd(uint32_t sessionID, ste_apdu_t* apdu_raw_p);

void catd_reset_pc_setup_event_list(void);

void catd_reset_pc_send_ussd(void);

void catd_reset_pc_setup_call(void);

void catd_handle_pc_language_notification(catd_msg_apdu_t  * msg);

void catd_pc_check_for_location_status_retransmissions(void);

void catd_reset_loc_status( uint8_t is_complete_reset );

#endif
