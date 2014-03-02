/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd.h
 * Description     : Internal definitions.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#ifndef __catd_h__
#define __catd_h__ (1)

#include "apdu.h"
#include "simd.h"
#include "cn_message_types.h"

// -----------------------------------------------------------------------------
// proactive command filter table
#define  STE_CAT_CMD_FILTER_HANDLE_BY_CATD            0x01
#define  STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT          0x02
#define  STE_CAT_CMD_FILTER_NOTIFY_CLIENT             0x04

//if the Proactive command is handled internally by CATD, we will use this as fd and client tag
//if there is anything needs to be handled by CATD internally, use these values
//Ex. the call control for CALL SETUP proactive command
#define  CATD_FD            0xEFFFFFFF
#define  CATD_CLIENT_TAG    0x00000001
#define  CATD_CLIENT_TAG_INTERN    0x00000002

typedef struct {
    uint8_t   cmd_type;
    uint8_t   filter;
} ste_cat_cmd_filter_t;

/**
 * @brief Handle input command from the socket.
 * @param cmd   Command to handle
 * @param client_tag The tag supplied by the client
 * @param buf   Payload of command
 * @param len   Sizeof payload
 * @param cc    Client context
 */
void
ste_catd_handle_command(uint16_t cmd,uintptr_t client_tag,const char *buf,
                        uint16_t len,ste_sim_client_context_t * cc);

/**
 * @brief This defines the interface for CATD callback responses.
 *
 * The callback function is passed three parameters:
 * @param apdu      The current APDU.
 * @param client_tag The tag supplied by the client.
 * @param user_data The user-specific data when the callback was registered.
 */
typedef void            catd_response_callback(ste_apdu_t * apdu,
                                               uintptr_t client_tag,
                                               void *user_data);

/**
 * @brief This maintains a callback, and the associated user-data.
 */
typedef struct {
    catd_response_callback *func;       /**< The callback function */
    uintptr_t              client_tag; /**< The tag suppled by the client */
    void                   *user_data;  /**< The user data to pass to the callback */
} catd_response_closure_t;


/**
 * @brief This function starts the CAT service.
 *
 * The purpose of this function is to start the CAT service.  A message channel
 * is created, and a thread to read those messages is also started.
 * @param client_tag The tag supplied by the client
 */
int                     catd_start(uintptr_t client_tag);

/**
 * @brief This function stops the CAT service.
 *
 * A message is sent to the CAT service thread to stop the service and free
 * associated resources.  This function returns when all the associated
 * resources have been freed, and the service has stopped.
 * @param client_tag The tag supplied by the client.
 */
int                     catd_stop(uintptr_t client_tag);


/* Signals to main */
/* =============== */

/**
 * Signals a timeout event. The timeout event must stem from a timer defined in simd_timer.h
 */
void catd_sig_timeout_event(uintptr_t timeout_event);

/**
 * @brief This function registers a handler for pro-active commands.
 *
 * @param fd    The input socket file descriptor for sending pro-active commands to.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_register(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function deregisters a client according to the fd.
 *
 * @param fd    The input socket file descriptor for the client.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_deregister(int fd, uintptr_t client_tag);

/**
 * @brief This function
 *
 * @param fd    The input socket file descriptor.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_startup(int fd, uintptr_t client_tag);

/**
 * @brief This function handles pro-active commands.
 *
 * @param apdu       The APDU for the pro-active command.
 * @param client_tag The tag supplied by the client.
 * @param resp       The response context registered for handling this command.
 */
void                    catd_sig_apdu(ste_apdu_t * apdu, uintptr_t client_tag);

/**
 * @brief This function handles terminal responses.
 *
 * @param fd    The input socket file descriptor.
 * @param apdu      The APDU for the terminal response.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_tr(int fd, ste_apdu_t * apdu,uintptr_t client_tag);

/**
 * @brief This function handles envelope commands.
 *
 * @param fd    The input socket file descriptor.
 * @param apdu      The APDU for the envelope command.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_ec(int fd, ste_apdu_t * apdu, uintptr_t client_tag);

void                    catd_sig_cn(cn_message_t * msg);

void                    catd_sig_raw_apdu(int fd, ste_apdu_t * apdu, uintptr_t client_tag);

/**
 * @brief       Inform the cat thread the modem is ready for
 *              terminal profile download.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_modem_ready(uintptr_t client_tag);

/**
 * @brief       Inform the cat thread the modem is NOT ready for
 *              terminal profile download.  This occurs if a terminal profile
 *              has not been downloaded before attempting other commands.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_modem_not_ready(uintptr_t client_tag);

/**
 * @brief       Inform the cat thread the modem is reset
 *              terminal profile should be downloaded again.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_modem_reset(uintptr_t client_tag);

/**
 * @brief       Inform the cat thread the modem registration ok with cat server for
 *              terminal profile download.  This occurs after terminal profile
 *              download successfully, including silent download.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_modem_ind_reg_ok(uintptr_t client_tag);

/**
 * @brief       Inform the cat thread the modem does not support
 *              CAT service.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_modem_cat_not_supported(uintptr_t client_tag);


/**
 * @brief       Inform the cat thread of the success/fail result of the
 *              terminal profile download.
 * @param client_tag The tag supplied by the client.
 * @param status    The result status of the download (0=success)
 */
void                    catd_sig_tpdl_status(uintptr_t client_tag, int status);

/**
 * @brief       Inform the cat thread of the result of updating the Adapt
 *              Terminal Support elementary file.
 * @param client_tag The tag supplied by the client.
 * @param status 0 if the file was updated, 1 otherwise
 * @param status_word the status word retrieved.
 */
void catd_sig_adapt_terminal_support_table_update_status(uintptr_t client_tag,
                                                         boolean adapt_file_updated,
                                                         sim_uicc_status_word_t status_word);

/**
 * @brief       Inform the cat thread when the telecom SIM application has
 *              been selected for activation.
 * @param client_tag  The tag supplied by the client.
 * @param status      The status of the app selection (0=success, 1=failed)
 *
 * Upon getting this message, the client can start accessing files of the
 *              of the SIM application
 */
void catd_sig_app_ready(uintptr_t client_tag, int status);

/**
 * @brief       Inform the cat thread of the success/fail result of the
 *              request to enable modem to fetch proactive commands.
 * @param client_tag The tag supplied by the client.
 * @param status    The result status of the enable request (0=success)
 */
void                    catd_sig_enable_status(uintptr_t client_tag, int status);

/**
 * @brief       Inform the cat thread of the success/fail result of the
 *              request to disable modem to fetch proactive commands.
 * @param client_tag The tag supplied by the client.
 * @param status    The result status of the disable (0=success)
 */
void                    catd_sig_disable_status(uintptr_t client_tag, int status);

/**
 * @brief       Inform the cat thread that the state of CAT server is ready
 *              this should be called from uiccd after it gets app activated.
 * @param client_tag  The tag supplied by the client.
 * @param status      The status of the CAT server (0 = ready)
 */
void                    catd_sig_cat_server_ready(uintptr_t client_tag, int status);

/**
 * Status Words returned by the card for CAT response
 */
typedef struct {
    uint8_t               sw1;    /**< status word 1 from modem */
    uint8_t               sw2;    /**< status word 2 from modem */
} ste_cat_status_words_t;

/**
 * @brief This function handles cat response from modem.
 *
 * @param apdu           The APDU for the cat response, could be NULL.
 * @param client_tag     The same client tag sent down by SIMD.
 * @param status_words   The status words part of CAT response from modem.
 */
void catd_sig_modem_cat_response(uintptr_t client_tag, ste_apdu_t * apdu, int status);

/**
 * @brief This function handles set poll interval response from modem.
 *
 * @param status         The result status of the function (0 = success).
 * @param interval       The actual poll interval used, in tenths of seconds (eg. in 100ms units).
 * @param client_tag     The same client tag used when ste_catd_modem_set_poll_intervall() was called.
 */
void catd_sig_modem_set_poll_interval_response(int status, int interval, uintptr_t client_tag);

/**
 * @brief This function handles refresh response from modem.
 *
 * @param status         The result status of the function (0 = success).
 * @param client_tag     The same client tag used when ste_catd_modem_refresh_request() was called.
 */
void catd_sig_modem_refresh_response(uintptr_t client_tag, int status);

/**
 * Refresh Indication type
 */
typedef enum {
  STE_CAT_REFRESH_IND_PERMINSSION = 0,
  STE_CAT_REFRESH_IND_START,
  STE_CAT_REFRESH_IND_NOW,
  STE_CAT_REFRESH_IND_END
} ste_cat_refresh_ind_type_t;

/**
 * Modem state for Refresh
 */
typedef enum {
  STE_CAT_MODEM_STATE_READY = 0,
  STE_CAT_MODEM_STATE_ERROR,
  STE_CAT_MODEM_STATE_REFRESH_TPDL_OK,
  STE_CAT_MODEM_STATE_REFRESH_TPDL_ERROR
} ste_cat_modem_state_t;

/**
 * @brief       Inform the cat thread the modem is ready for
 *              terminal profile download.
 * @param client_tag The tag supplied by the client.
 */
void catd_sig_modem_refresh_ind(uintptr_t client_tag,
                                ste_apdu_refresh_type_t refresh_type,
                                ste_cat_refresh_ind_type_t ind_type,
                                uint8_t app_id);


/**
 * @brief INTERNAL USE ONLY: Have the CATd thread connect to the SMS/CB server
 *
 * This function is in catd to send a message to itself in order to asynchronously
 * connect to the SMS/CB server. The client tag should point to a ste_sim_ctrl_block_t
 * with a valid transaction_handler_func call-back set.
 *
 * @param client_tag  Pointer to ste_sim_ctrl_block_t.
 */
void catd_sig_sms_cb_perform_connect(uintptr_t client_tag);


/**
 * @brief This function handles call control request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for call control.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_call_control(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles set terminal profile request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for terminal profile.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_set_terminal_profile(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles get terminal profile request.
 *
 * @param fd         The client fd.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_get_terminal_profile(int fd, uintptr_t client_tag);

/**
 * @brief This function handles get CAT status request.
 *
 * @param fd         The client fd.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_get_cat_status(int fd, uintptr_t client_tag);

/**
 * @brief This function handles answer the pc call setup request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for decision of answer or not.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_answer_call(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles event download request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for event type.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_event_download(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles sms control request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for sms control.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_sms_control(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles setup call result.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for setup call result.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_setup_call_result(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles cat enable request.
 *
 * @param fd         The client fd.
 * @param buf_p      The buf data pointer that holds the data for enable type.
 * @param len        The len of the buf.
 * @param client_tag The tag supplied by the client.
 */
void                    catd_sig_cat_enable(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag);

/**
 * @brief This function handles request from uiccd to notify the SIM power on/off status.
 *
 * @param client_tag  The tag supplied by the client.
 * @param power_on    The sim power status (1=power on, 0=power off)
 */
void                    catd_sig_sim_power_on_off(uintptr_t client_tag, int power_on);

/**
 * @brief This function handles request from uiccd to reset CATD.
 *
 * @param client_tag  The tag supplied by the client.
 */
void catd_sig_reset(uintptr_t client_tag);

//interfaces for libpc

/**
 * @brief This function send the PC setup call indication to interested client.
 *
 * @param call_data_p   The setup call related info.
 */
int catd_send_pc_setup_call_ind(ste_cat_pc_setup_call_ind_t * call_data_p);

/**
 * @brief This function send the PC refresh indication to interested client.
 *
 * @param refresh_p     The refresh related info.
 */
int catd_send_pc_refresh_ind(ste_cat_pc_refresh_ind_t * refresh_p);

/**
 * @brief This function send the PC refresh indication to interested client.
 *
 * @param refresh_p     The refresh related info.
 */
int catd_send_pc_refresh_file_ind(ste_cat_pc_refresh_file_ind_t * refresh_p);

/** @brief This function is used to send a RAT setting updated indication
 */
int catd_send_rat_setting_updated_ind();

/** @brief This function is used to send session end indications to clients.
 */
int catd_send_session_end_ind();

/** @brief This function is used to send cat status indications to clients.
 */
int catd_send_cat_status_ind();

/** @brief This function is used to send cat info indications to clients.
 *
 * @param cat_info     The related info.
 */
int catd_send_cat_info_ind(ste_cat_info_t cat_info);

/**
 * @brief Sends the APDU in a STE_CAT_IND_PC to the client.
 *
 * If there is any client subscribing to pro-active command notifications,
 * then this function sends the APDU in a STE_CAT_IND_PC_NOTIFICATION to the client.
 *
 * @param   apdu    A pointer to the APDU.
 *
 */
int catd_send_pc_ind(ste_apdu_t *apdu_p);

/**
 * @brief Sends the APDU in a STE_CAT_IND_PC_NOTIFICATION to the client.
 *
 * If there is any client subscribing to pro-active command notifications,
 * then this function sends the APDU in a STE_CAT_IND_PC_NOTIFICATION to the client.
 *
 * @param   apdu    A pointer to the APDU.
 *
 */
int catd_pc_send_pc_notification_ind(ste_apdu_t *apdu);

/**
 * @brief Respond to client's answer call.
 *
 * @param status     The call setup status, failed or succeeded(0 == success).
 */
void catd_send_answer_call_response(int status);

/**
 * Returns the socket to use as SMS/CB response socket in request functions
 */
int catd_get_sms_cb_request_socket();

/**
 * Returns the socket to use for SMS/CB event subscriptions
 *
 * Return: >= 0, a valid socket/file descriptor
 *          < 0, socket not valid
 */
int catd_get_sms_cb_event_socket();


/**
 * Returns the socket to use when internally requesting services from UICCd
 *
 * Return: >= 0, a valid socket/file descriptor
 *          < 0, socket not valid
 */
int catd_get_uiccd_request_socket();

/**
* Returns the socket to use for internally sending timeout data from CATD to
* CATD reader for further processing/routing.
*/
int catd_get_timer_write_socket();

/**
 * @brief Request modem to do "CAT ENABLE".
 *
 * @param void
 *
 * @return:   result of the operation
 *            Success: 0; Failed: -1
 */
int catd_cat_enable();

/**
 * @brief Adapt status constants
 */
typedef enum {
    CAT_ADAPT_STATUS_UNKNOWN,    /* Adapt support undecided */
    CAT_ADAPT_STATUS_SUPPORTED,  /* Supporting adapt */
    CAT_ADAPT_STATUS_UNSUPPORTED /* Not supporting adapt */
} cat_adapt_status_t;

/**
 * @brief Returns status of Adapt support
 * @param void
 * @return The adapt status
 *
 * CAT_ADAPT_STATUS_UNKNOWN means catd has not yet determined if the sim card
 * supports adapt. When catd enters the ready state, the function returns
 * either CAT_ADAPT_STATUS_SUPPORTED or CAT_ADAPT_STATUS_UNSUPPORTED for this
 * sim. Since Adapt is a static property of the telecom application, the value
 * will not change while the telecom app remains active.
 */
cat_adapt_status_t catd_adapt_get_status();

#endif
