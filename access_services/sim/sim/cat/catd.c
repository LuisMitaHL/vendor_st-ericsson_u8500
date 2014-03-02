/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd.c
 * Description     : Card Application Toolkit main.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "catd_reader.h"
#include "cat_internal.h"
#include "sim_internal.h"

#include "cat_barrier.h"
#include "msgq.h"
#include "state_machine.h"
#include "client_data.h"
#include "catd.h"
#include "catd_msg.h"
#include "catd_modem.h"
#include "catd_tpl.h"
#include "apdu.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uiccd.h"
#include "sim_unused.h"
#include "func_trace.h"
#include "pc_handler.h"
#include "pc_utilities.h"

#include "catd_cn_client.h"
#include "simd_timer.h"

#include "r_sms.h"
#include "r_sms_cb_session.h"
#include "util_security.h"

const ste_cat_cmd_filter_t STE_CATD_CMD_FILTER[] = {
    { STE_APDU_CMD_TYPE_NONE,                   0 },
    /** Commands handled by client. */
    { STE_APDU_CMD_TYPE_DISPLAY_TEXT,           STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_GET_INKEY,              STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_GET_INPUT,              STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_SET_UP_MENU,            STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_PLAY_TONE,              STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_SELECT_ITEM,            STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_SET_UP_IDLE_MODE_TEXT,  STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_LAUNCH_BROWSER,         STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
#ifdef SAT_BIP_SUPPORT
    { STE_APDU_CMD_TYPE_OPEN_CHANNEL,           STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_CLOSE_CHANNEL,          STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_RECEIVE_DATA,           STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_SEND_DATA,              STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    { STE_APDU_CMD_TYPE_GET_CHANNEL_STATUS,     STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
#endif
    /** Commands handled by CATD. */
    { STE_APDU_CMD_TYPE_POLL_INTERVAL,          STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_APDU_CMD_TYPE_POLLING_OFF,            STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_APDU_CMD_TYPE_TIMER_MANAGEMENT,       STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_APDU_CMD_TYPE_MORE_TIME,              STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    /** Commands handled by CATD or client. */
    { STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO,     STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
    /** Commands handled by CATD and notify client for confirmation. */
    { STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST,      STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_SET_UP_CALL,            STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE,     STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_SEND_DTMF,              STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_SEND_SS,                STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_SEND_USSD,              STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_REFRESH,                STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION,  STE_CAT_CMD_FILTER_HANDLE_BY_CATD | STE_CAT_CMD_FILTER_NOTIFY_CLIENT },
    { STE_APDU_FIRST_RESERVED_COMMAND_TYPE,     0 } // TERMINATOR
};

const ste_cat_cmd_filter_t STE_CATD_CMD_QUALIFIER_FILTER_PROVIDE_LOCAL_INFO[] = {
    /** Commands handled by CATD. */
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LOCATION_INFO,        STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEI,              STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_NW_MEASURE_RESULT,    STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_DATE_TIME_ZONE,       STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_TIMING_ADVANCE,       STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ACCESS_TECH,          STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ESN,                  STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEISV,            STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_SEARCH_MODE,          STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_BATTERY,              STE_CAT_CMD_FILTER_HANDLE_BY_CATD },
    /** Commands handled by client. */
#ifdef SAT_PLI_LANGUAGE_SUPPORT
    { STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LANGUAGE,             STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT },
#endif
    { STE_APDU_FIRST_RESERVED_COMMAND_TYPE,                 0 } // TERMINATOR
};

//max number of clients CATD supports, in theory, this figure can be any value
#define STE_CATD_MAX_NBR_OF_CLIENTS 16

//Set default poll interval to 30 seconds according to 3GPP, (Interval = 1/10th of a second)
#ifndef SET_POLL_INTERVALL
#define SET_POLL_INTERVALL 300
#endif

// Events in Setup Event List
#define SET_UP_EVENT_LIST_LOCATION_STATUS                             (0x03)
#define SET_UP_EVENT_LIST_DATA_AVAILABLE                              (0x09)

//The state of CAT service, this enum value is also mapped to the value of ste_cat_service_status_t in sim.h for client
typedef enum {
    CATD_STATE_NULL  = 0,    //corresponding to the value of STE_CAT_SERVICE_NULL      = 0x00 in sim.h
    CATD_STATE_INIT,         //corresponding to the value of STE_CAT_SERVICE_INIT in sim.h
    CATD_STATE_TPDL,         //corresponding to the value of STE_CAT_SERVICE_TPDL in sim.h
    CATD_STATE_READY,        //corresponding to the value of STE_CAT_SERVICE_READY in sim.h
    CATD_STATE_REFRESH,      //corresponding to the value of STE_CAT_SERVICE_REFRESH in sim.h
    CATD_STATE_MODEM_ERROR   //corresponding to the value of STE_CAT_SERVICE_MODEM_ERROR in sim.h
} catd_state_t;

//the structure to hold client related data
typedef struct {
    int                      registered_fd;      //the registered socket fd for this client
    int                      req_fd;             //if each client has two sockets to handle sync and async events in the future, save the sync socket fd in this member
    uint32_t                 registered_events;  //the events that this client is interested in hearing
} ste_catd_client_t;

/*
 * Data type to contain the sockets needed to communicate with the SMS/CB server
 */
typedef struct {
    int response_socket;        /* The socket where responses will be read from (set to -1 if/when invalid) */
    bool response_socket_es;    /* Event stream exists for socket flag */
    int event_socket;           /* The socket where events will be read from (set to -1 if/when invalid) */
    bool event_socket_es;       /* Event stream exists for socket flag */
} ste_catd_sms_cb_sockets_t;

/*
 * Data type to contain the sockets needed to communicate with
 * the UICCd thread (as a "fake" client).
 */
typedef struct {
    int read_socket;  /* Socket to read responses from (set to -1 if/when invalid) */
    int write_socket; /* Socket to for UICCd to write responses to (set to -1 if/when invalid) */
} ste_catd_pipe_t;

/*
 * Data type to contain the sockets needed to communicate with
 * CATD reader when a timer expire.
 */
typedef struct {
    int read_socket;  /* Socket to read responses from (set to -1 if/when invalid) */
    int write_socket; /* Socket to for CATD to write responses to (set to -1 if/when invalid) */
} ste_catd_timer_pipe_t;

typedef enum {
    CATD_TPDL_STATE_NULL  = 0,     //TPDL not init yet, startup state
    CATD_TPDL_STATE_START,         //TPDL request has been sent down to modem
    CATD_TPDL_STATE_SUCCESS,       //TPDL succeeded
    CATD_TPDL_STATE_FAIL           //TPDL failed
} catd_tpdl_state_t;

typedef enum {
    CATD_TPDL_INITIATOR_NULL = 0,        //NOBODY
    CATD_TPDL_INITIATOR_STARTUP,         //TPDL requested by start up
    CATD_TPDL_INITIATOR_MODEM_READY,     //TPDL requested by modem ready
    CATD_TPDL_INITIATOR_REFRESH,         //TPDL requested by refresh
    CATD_TPDL_INITIATOR_POWER_ON_OFF,    //TPDL requested by power on/off request
    CATD_TPDL_INITIATOR_SILENT_DOWNLOAD  //TPDL requested by power on/off request
} catd_tpdl_initiator_t;

typedef enum {
    CATD_TPDL_EVENT_REQUEST = 0,         //TPDL request
    CATD_TPDL_EVENT_RESP_OK,             //TPDL response OK
    CATD_TPDL_EVENT_RESP_NOK,            //TPDL response NOK
    CATD_TPDL_EVENT_MODEM_READY_IND,     //get modem ready indication
    CATD_TPDL_EVENT_MODEM_IND_REG_OK,    //get modem registration ok indication
    CATD_TPDL_EVENT_MODEM_IND_REG_FAIL,  //get modem registration fail indication
    CATD_TPDL_EVENT_INVALID              //invalid event
} catd_tpdl_event_t;

#define CATD_TPDL_MAX_TRY_TIMES  2   //max retry times of tpdl

//the data structure for TPDL
typedef struct {
    catd_tpdl_state_t      state;       //To keep track of terminal profile download state
    catd_tpdl_initiator_t  initiator;   //To keep track of TPDL initiator
    uint8_t                retry;       //To keep track of if TPDL retry is needed or not
    uint8_t                try_times;   //To keep track of total TPDL try times
} catd_tpdl_t;

static catd_tpdl_t g_tpdl;   //the global value for tpdl operation

typedef enum {
    CATD_CAT_ENABLE_STATUS_DISABLED  = 0,   //cat disabled, enable is not done yet
    CATD_CAT_ENABLE_STATUS_ENABLED,         //cat enabled
    CATD_CAT_ENABLE_STATUS_DISABLED_TEMP,   //cat disabled, but requested to be enabled by client. This will be the case as result of internal operation or if not possible to enable cat when requested by client.
} catd_cat_enable_status_t;

static catd_cat_enable_status_t enable_status = CATD_CAT_ENABLE_STATUS_DISABLED; //To keep track of cat enable status

//msg queue for requests
static ste_msgq_t      *catd_mq_req = 0;
//msg queue for responses
static ste_msgq_t      *catd_mq_rsp = 0;

//mask values for different commands
#define STE_CATD_CMD_MASK_PC   0x1000
#define STE_CATD_CMD_MASK_EC   0x2000

//the data structure for CATD
typedef struct {
    catd_state_t              state;       // The CATD state, which is the value sent to client when it requests for CAT STATE
    ste_catd_client_t         client_list[STE_CATD_MAX_NBR_OF_CLIENTS];  // Array to hold different clients
    ste_msgq_t*               working_mq;  // The current queue that CATD is querying
    uint16_t                  ongoing_cmd; // The ongoing command that CATD is dealing with, use this to trace multiple requests
    ste_catd_pipe_t           uiccd_pipe;  // Pipe to communicate with the UICCd thread as a "fake" client
    ste_catd_sms_cb_sockets_t sms_cb;      // Sockets used to communicate with the SMS/CB server
    ste_catd_timer_pipe_t     timer_pipe;  // Socket used for writing timer information data from CATD to CATD reader.
    cat_adapt_status_t        adapt_status; // Whether Adapt is supported, unsupported, or unknown for the sim card in use
} catd_data_t;

static catd_data_t gcatd;   //the global value for this CATD server. We only have one CATD so put a global value here
static uintptr_t current_CT; //the global value for the current setup_call session.
static int current_fd;       //the file descriptor for the current setup_call session.

//macros to set the working mq. Which can be used to set the CATD to different working mode, Ex.,waiting for responses or not
#define CATD_WAITING_FOR_RESPONSE     catd_switch_working_mq(catd_mq_rsp);
#define CATD_PROCEED_WITH_REQUEST     catd_switch_working_mq(catd_mq_req);

static void catd_sim_timer_expiration_send_ec(const uint16_t timer_info, ste_apdu_t *apdu_p);
static void catd_disconnect_from_sms_cb();
static int catd_enable_cat_service(uintptr_t client_tag);

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
static void catd_disable_resp_handler();
static catd_msg_ec_t            env_cahed_apdu;
static uint8_t pc_outstanding = 0;
static uint8_t cat_disable_outstanding = 0;
ste_sat_apdu_error_t  ste_apdu_check_event_data_available(ste_apdu_t *apdu_p);
static int catd_disable_cat_service(uintptr_t client_tag);
static int catd_cat_disable();
static void catd_cat_polling_req();
#endif


// #############################################################################
//temporary solution, things need to be throw away in the future
//FIXME: there should not be any startup, Remove them!
static int startup_fd = 0; // Until we get the fd into client tag
static int startup_ct = 0; // Saved until startup completed
// #############################################################################

/*************************************************************************
 * @brief:    Handle commands from the client.
 * @params:
 *            cmd:           the type of the command.
 *            client_tag:    the client tag of the client that sends this cmd.
 *            buf:           the data related to the command.
 *            len:           the length of the data.
 *            cmd:           sim client context.
 *
 * @return:   Status of the operation.
 *            Success: 0; Failed: -1
 *
 * Notes:     This is executed by the reader thread
 *************************************************************************/
void
ste_catd_handle_command(uint16_t  cmd,
                        uintptr_t client_tag,
                        const char *buf,
                        uint16_t len,
                        ste_sim_client_context_t * cc)
{
    switch (cmd)
    {
        case STE_CAT_REQ_REGISTER:
        {
            catd_sig_register(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_DEREGISTER:
        {
            catd_sig_deregister(cc->fd, client_tag);
        }
        break;

        case STE_CAT_REQ_TR:
        {
            ste_apdu_t *apdu = ste_apdu_new((uint8_t *) buf, len);
            if (apdu) {
                catd_sig_tr(cc->fd, apdu, client_tag);
            }
            ste_apdu_delete(apdu);
        }
        break;

        case STE_CAT_REQ_EC:
        {
            ste_apdu_t *apdu = ste_apdu_new((uint8_t *) buf, len);
            if (apdu) {
                catd_sig_ec(cc->fd, apdu, client_tag);
            }
            ste_apdu_delete(apdu);
        }
        break;

        case STE_CAT_REQ_RAW_APDU:
        {
            ste_apdu_t *apdu = ste_apdu_new((uint8_t *) buf, len);
            if (apdu) {
                catd_sig_raw_apdu(cc->fd, apdu, client_tag);
            }
        }
        break;

        case STE_CAT_REQ_CALL_CONTROL:
        {
            catd_sig_call_control(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_SET_TERMINAL_PROFILE:
        {
            catd_sig_set_terminal_profile(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_GET_TERMINAL_PROFILE:
        {
            catd_sig_get_terminal_profile(cc->fd, client_tag);
        }
        break;

        case STE_CAT_REQ_GET_CAT_STATUS:
        {
            catd_sig_get_cat_status(cc->fd, client_tag);
        }
        break;

        case STE_CAT_REQ_ANSWER_CALL:
        {
            catd_sig_answer_call(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_EVENT_DOWNLOAD:
        {
            catd_sig_event_download(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_SMS_CONTROL:
        {
            catd_sig_sms_control(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        case STE_CAT_REQ_ENABLE:
        {
            catd_sig_cat_enable(cc->fd, (uint8_t *) buf, len, client_tag);
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "read : BAD COMMAND: %04x len=%d", cmd, len);
        }
    }
}

/*************************************************************************
 * @brief:    switch the working msg queue.
 * @params:
 *            working_mq:    the msg queue to switch to.
 *
 * @return:   void.
 *
 * Notes:     In the future if we do not care about the stage of the session
 *            such as a PC, just use the same msg queue
 *************************************************************************/
static void catd_switch_working_mq(ste_msgq_t * working_mq)
{
    gcatd.working_mq = working_mq;
}

/*************************************************************************
 * @brief:    get the filter value for a proactive command.
 * @params:
 *            cmd_type:    the type id of the cmd under filtering.
 *
 * @return:   the filter value.
 *
 * Notes:
 *************************************************************************/
static uint8_t catd_get_filter_for_cmd(uint8_t cmd_type)
{
    uint8_t   i;

    for (i = 0; i < (sizeof(STE_CATD_CMD_FILTER) / sizeof(STE_CATD_CMD_FILTER[0])); i++)
    {
        if (STE_CATD_CMD_FILTER[i].cmd_type == cmd_type)
        {
            return (STE_CATD_CMD_FILTER[i].filter);
        }
    }
    return 0;
}

/******************************************************************************
 * @brief:    get the filter value for a provide local info proactive command.
 * @params:
 *            cmd_qualifier:    the qualifier of the cmd under filtering.
 *
 * @return:   the filter value.
 *
 * Notes:
 ******************************************************************************/
static uint8_t catd_get_filter_for_provide_local_info(uint8_t cmd_qualifier)
{
    uint8_t   i;

    for (i = 0; i < (sizeof(STE_CATD_CMD_QUALIFIER_FILTER_PROVIDE_LOCAL_INFO) / sizeof(STE_CATD_CMD_QUALIFIER_FILTER_PROVIDE_LOCAL_INFO[0])); i++)
    {
        if (STE_CATD_CMD_QUALIFIER_FILTER_PROVIDE_LOCAL_INFO[i].cmd_type == cmd_qualifier)
        {
            return (STE_CATD_CMD_QUALIFIER_FILTER_PROVIDE_LOCAL_INFO[i].filter);
        }
    }
    return 0;
}

/*************************************************************************
 * @brief:    send the response to a terminal response to the client
 * @params:
 *            fd:           the file descriptor of the socket to be sent over.
 *            client_tag:   the client tag of the message receiver/client.
 *            msg_type:     which req is this response for.
 *            status:       the status value to be sent.
 *            status_words: Status words.
 *
 * @return:   void.
 *
 * Notes:
 *************************************************************************/
static void catd_send_tr_response(int fd, uintptr_t client_tag, uint16_t msg_type, int status, ste_cat_status_words_t *status_words)
{
    char                   * p = NULL;
    char                   * buf = NULL;
    size_t                   buf_len = 0;

    buf_len += sizeof(status);
    buf_len += sizeof(status_words->sw1);
    buf_len += sizeof(status_words->sw2);

    buf = malloc(buf_len);
    if (buf == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_tr_response, memory allocation failed");
        return;
    }

    p = buf;
    p = sim_enc(p, &status, sizeof(status));
    p = sim_enc(p, &status_words->sw1, sizeof(status_words->sw1));
    p = sim_enc(p, &status_words->sw2, sizeof(status_words->sw2));

    if (buf_len != (size_t)(p - buf)) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_tr_response, assert failure");
    }

    assert( buf_len == (size_t)(p - buf) );

    sim_send_generic(fd, msg_type, buf, buf_len, client_tag);
    free( buf );
}

/*************************************************************************
 * @brief:    send the response with only status value to particular client.
 * @params:
 *            fd:           the file descriptor of the socket to be sent over.
 *            client_tag:   the client tag of the message receiver/client.
 *            msg_type:     which req is this response for.
 *            status:       the status value to be sent.
 *
 * @return:   Status of the operation.
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_send_status_response(int fd, uintptr_t client_tag, uint16_t msg_type, int status)
{
    char                   * p = NULL;
    char                   * buf = NULL;
    unsigned                 status_len = 0;
    size_t                   buf_len;
    int                      result = -1;

    status_len = sizeof(status);

    buf_len = sizeof(char) * (status_len);
    buf_len += sizeof(status_len);

    buf = malloc(buf_len);
    if (buf == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_status_response, memory allocation failed");
        return -1;
    }
    p = buf;
    p = sim_enc(p, &status_len, sizeof(status_len));
    p = sim_enc(p, &status,      sizeof(char) * (status_len));

    if (buf_len != (size_t)(p - buf)) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_status_response, assert failure");
    }

    assert( buf_len == (size_t)(p - buf) );

    result = sim_send_generic(fd, msg_type, buf, buf_len, client_tag);
    free( buf );
    return result;
}

/*************************************************************************
 * @brief:    change the CAT service status.
 * @params:
 *            status:       the CAT service status to change to.
 *
 * @return:   void.
 *
 * Notes:
 *************************************************************************/
static void catd_change_cat_status(int status)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s Change status from 0x%X to 0x%X", __func__, gcatd.state, status);
    gcatd.state = status;
}

/*************************************************************************
 * @brief:    broadcast the CAT service status to all clients that are interested in this.
 * @params:
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_cat_status_ind()
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    //go through the client list to find those clients who are interested in CAT status
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_CAT_STATUS) ==
                STE_CAT_CLIENT_REG_EVENTS_CAT_STATUS)
        {
            result = catd_send_status_response(gcatd.client_list[i].registered_fd,
                                               (uintptr_t)0,
                                               STE_CAT_IND_CAT_STATUS,
                                               gcatd.state);
            if (result == 0) {
                success_count++;
            }
        }
    }
    return success_count;
}

/*************************************************************************
 * @brief:    get the client id from the client list according to the given registered fd.
 * @params:
 *            fd:       the registered fd.
 *
 * @return:   the client id.
 *            Success: client id; Failed: invalid client id
 *
 * Notes:     we suppose one client is registered only one time in the list
 *************************************************************************/
static int catd_get_client_id_from_fd(int fd)
{
    uint8_t i;

    if (fd == -1)
    {
        return -1;
    }
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == fd)
        {
            return i;
        }
    }
    return -1;
}

/*************************************************************************
 * @brief:    get the number of client registered for the specified event(s).
 * @params:
 *            event_mask:       particular event(s) to compare with the registered interests.
 *
 * @return:   number of client that match the query
 *
 *************************************************************************/
static uint8_t catd_get_client_count_from_registered_events(uint32_t event_mask)
{
    uint8_t i;
    uint8_t count = 0;

    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++) {
        if (gcatd.client_list[i].registered_fd == -1) {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & event_mask) == event_mask) {
            count++;
        }
    }
    return count;
}

/*************************************************************************
 * @brief:    Notify clients about the cat info.
 * @params:
 *            cat_info:    The status of the CAT service.
 *
 * @return:   void.
 *
 * Notes:
 *************************************************************************/
int catd_send_cat_info_ind(ste_cat_info_t cat_info)
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    catd_log_f(1, "catd : catd_send_cat_info_ind. Sending CAT info ind to registered clients! cat_info = %d", cat_info);

    //go through the client list to find those clients who are interested in the session end indication
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_CAT_INFO) ==
                STE_CAT_CLIENT_REG_EVENTS_CAT_INFO)
        {
            result = catd_send_status_response(gcatd.client_list[i].registered_fd,
                                               0,  //for indication, the client tag should be set to 0
                                               STE_CAT_IND_CAT_INFO,
                                               (int)cat_info);
            if (result == 0) {
                success_count++;
            }
        }
    }
    return success_count;
}

/*************************************************************************
 * @brief:    reset the client data of one particular client.
 * @params:
 *            client_p:       the registered client data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_client_reset(ste_catd_client_t * client_p)
{
    if (client_p) {
        client_p->registered_fd = -1;
        client_p->req_fd = -1;
        client_p->registered_events = 0;
    }
}

/*************************************************************************
 * @brief:    reset the tpdl data.
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_reset()
{
    g_tpdl.state = CATD_TPDL_STATE_NULL;
    g_tpdl.initiator = CATD_TPDL_INITIATOR_NULL;
    g_tpdl.retry = 0;
    g_tpdl.try_times = 0;
}

/*************************************************************************
 * @brief:    download terminal profile to modem.
 * @params:
 *            client_tag:       the client tag.
 *
 * @return:   Status of the operation.
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_download_terminal_profile(uintptr_t client_tag)
{
    ste_modem_t            *modem_p;
    uint8_t                *profile_p = 0;
    int                     rv;

    modem_p = catd_get_modem();
    if (!modem_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        return -1;
    }

    //Initialise the TPDL
    profile_p = (uint8_t *)catd_main_load_terminal_profile();
    if (NULL == profile_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : Initialize of terminal profile failed");
        return -1;
    }

    rv = ste_catd_modem_tpdl(modem_p, client_tag, profile_p, sizeof(catd_terminal_profile_download_t));

    free(profile_p);

    if (rv)
    {
        catd_log_f(SIM_LOGGING_E, "catd : terminal profile download failed");
        return -1;
    }

    catd_change_cat_status(CATD_STATE_TPDL);
    g_tpdl.try_times++;

    //waiting for the response
    CATD_WAITING_FOR_RESPONSE;

    return 0;
}

/*************************************************************************
 * @brief:    event handler for tpdl state machine state NULL.
 * @params:
 *            tpdl_event:       the input event for state machine.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_state_null_event_handler(catd_tpdl_event_t tpdl_event)
{
    int rv = 0;

    switch (tpdl_event)
    {
        case CATD_TPDL_EVENT_REQUEST:
        {
            rv = catd_download_terminal_profile((uintptr_t)0);

            if (rv)
            {
                catd_log_f(SIM_LOGGING_E, "catd : tpdl failed in NULL state");
                //The state will not be changed here
                return;
            }
            g_tpdl.state = CATD_TPDL_STATE_START;
        }
        break;
        case CATD_TPDL_EVENT_RESP_OK:
        case CATD_TPDL_EVENT_RESP_NOK:
        {
            //invalid event for NULL state, do nothing
        }
        break;
        case CATD_TPDL_EVENT_MODEM_READY_IND:
        {
            rv = catd_download_terminal_profile((uintptr_t)0);

            if (rv)
            {
                catd_log_f(SIM_LOGGING_E, "catd : tpdl failed in NULL state");
                g_tpdl.state = CATD_TPDL_STATE_FAIL;
                return;
            }
            g_tpdl.state = CATD_TPDL_STATE_START;
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: wrong tpdl event!");
            return;
        }
    }
}

/*************************************************************************
 * @brief:    event handler for tpdl state machine state START.
 * @params:
 *            tpdl_event:       the input event for state machine.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_state_start_event_handler(catd_tpdl_event_t tpdl_event)
{
    int rv = 0;

    switch (tpdl_event)
    {
        case CATD_TPDL_EVENT_REQUEST:
        {
            //the response for previous try is not received yet, so just set the retry flag
            g_tpdl.retry = 1;
        }
        break;
        case CATD_TPDL_EVENT_RESP_OK:
        {
            catd_log_f(SIM_LOGGING_E, "catd : terminal profile download succeeded!");
            g_tpdl.state = CATD_TPDL_STATE_SUCCESS;

            //check the initiator of TPDL, perform accordingly the following procedure
            if (g_tpdl.initiator == CATD_TPDL_INITIATOR_REFRESH)
            {
                //SEND REFRESH TPDL ready signal to uiccd
                catd_log_f(SIM_LOGGING_D, "catd : TPDL succeeded for PC refresh.");
                uiccd_sig_modem_state( 0, 0 );
            }
            else if (g_tpdl.initiator == CATD_TPDL_INITIATOR_POWER_ON_OFF)
            {
                catd_send_cat_info_ind(STE_CAT_INFO_SUCCESS);
                catd_change_cat_status(CATD_STATE_READY);
                uiccd_sig_modem_state( 0, 0 );   //SEND ready signal to uiccd
                catd_send_cat_status_ind();
                // Enable CAT service in modem if requested to be enabled by client before.
                if (enable_status == CATD_CAT_ENABLE_STATUS_DISABLED_TEMP)
                {
                    catd_cat_enable();
                    catd_log_f(SIM_LOGGING_I, "catd : cat enabled again after power off/on");
                }
                gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
                CATD_PROCEED_WITH_REQUEST;
            }
            else
            {
                //normal startup procedure
                //TPDL operation succeeded, so notify client and continue with other startup procedures
                catd_send_cat_info_ind(STE_CAT_INFO_SUCCESS);
                catd_change_cat_status(CATD_STATE_READY);
                uiccd_sig_modem_state( 0, 0 );   //SEND ready signal to uiccd
                catd_send_cat_status_ind();
                simd_sig_startup_completed(startup_fd, SIMD_DAEMON_CATD, 0, startup_ct);
                // Enable CAT service in modem if requested to be enabled by client before.
                if (enable_status == CATD_CAT_ENABLE_STATUS_DISABLED_TEMP) {
                    //this is initiated from modem reset/fallback, cat enable should be performed
                    //enable cat service since client has done it previously
                    catd_cat_enable();
                    catd_log_f(SIM_LOGGING_I, "catd : cat enabled again");
                }
                gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
                CATD_PROCEED_WITH_REQUEST;
            }
        }
        break;
        case CATD_TPDL_EVENT_RESP_NOK:
        {
            //check the initiator of TPDL
            if (g_tpdl.initiator == CATD_TPDL_INITIATOR_REFRESH)
            {
                //for refresh, TPDL will only be triggered once, so retry is not applicable
                //get into fail state
                g_tpdl.state = CATD_TPDL_STATE_FAIL;
                catd_log_f(SIM_LOGGING_D, "catd : TPDL failed for PC refresh.");
                //SEND REFRESH TPDL error signal to uiccd
                catd_send_cat_info_ind(STE_CAT_INFO_PROFILE_FAIL);
                catd_change_cat_status(CATD_STATE_MODEM_ERROR);
                uiccd_sig_modem_state( 0, 1 );
                catd_send_cat_status_ind();
            }
            else if (g_tpdl.initiator == CATD_TPDL_INITIATOR_POWER_ON_OFF)
            {
                //for power on after power off, TPDL will only be triggered once too, retry is not applicable here either.
                //get into fail state
                g_tpdl.state = CATD_TPDL_STATE_FAIL;

                catd_log_f(SIM_LOGGING_D, "catd : TPDL failed for power on/off.");
                //send a CAT info profile fail ind to registered clients
                catd_send_cat_info_ind(STE_CAT_INFO_PROFILE_FAIL);
                catd_change_cat_status(CATD_STATE_MODEM_ERROR);
                uiccd_sig_modem_state( 0, 1 );  //SEND error signal to uiccd
                catd_send_cat_status_ind();
            }
            else
            {
                //this is normal TPDL from startup, check if retry is needed
                if (g_tpdl.retry || g_tpdl.try_times < CATD_TPDL_MAX_TRY_TIMES)
                {
                    rv = catd_download_terminal_profile((uintptr_t)0);

                    if (rv)
                    {
                        catd_log_f(SIM_LOGGING_E, "catd : tpdl failed in START state");
                        g_tpdl.state = CATD_TPDL_STATE_FAIL;
                        return;
                    }
                    if (g_tpdl.retry)
                    {
                        //just retry for once, now reset the value
                        g_tpdl.retry = 0;
                    }
                }
                else
                {
                    //get into fail state
                    g_tpdl.state = CATD_TPDL_STATE_FAIL;

                    if (g_tpdl.try_times == CATD_TPDL_MAX_TRY_TIMES)
                    {
                        catd_log_f(SIM_LOGGING_D, "catd : TPDL failed for startup.");
                        //send a CAT info profile fail ind to registered clients
                        catd_send_cat_info_ind(STE_CAT_INFO_PROFILE_FAIL);
                        catd_change_cat_status(CATD_STATE_MODEM_ERROR);
                        uiccd_sig_modem_state( 0, 1 );  //SEND error signal to uiccd
                        catd_send_cat_status_ind();
                    }
                }
            }
        }
        break;
        case CATD_TPDL_EVENT_MODEM_READY_IND:
        {
            //the response for previous try is not received yet, so just set the retry flag
            g_tpdl.retry = 1;
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: wrong tpdl event!");
            return;
        }
    }
}

/*************************************************************************
 * @brief:    event handler for tpdl state machine state FAIL.
 * @params:
 *            tpdl_event:       the input event for state machine.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_state_fail_event_handler(catd_tpdl_event_t tpdl_event)
{
    int rv = 0;

    switch (tpdl_event)
    {
        case CATD_TPDL_EVENT_REQUEST:
        {
            rv = catd_download_terminal_profile((uintptr_t)0);

            if (rv)
            {
                catd_log_f(SIM_LOGGING_E, "catd : tpdl retry failed in fail state");
                //keep the current state
                return;
            }
            g_tpdl.state = CATD_TPDL_STATE_START;
        }
        break;
        case CATD_TPDL_EVENT_RESP_OK:
        case CATD_TPDL_EVENT_RESP_NOK:
        {
            //invalid event for NULL state, do nothing
        }
        break;
        case CATD_TPDL_EVENT_MODEM_READY_IND:
        {
            catd_log_f(SIM_LOGGING_I, "catd : got CATD_TPDL_MODEM_READY_IND in Fail State. Initiate Recovery");
            g_tpdl.retry = 0;
            g_tpdl.initiator = CATD_TPDL_INITIATOR_MODEM_READY;
            g_tpdl.try_times = 0;
            rv = catd_download_terminal_profile((uintptr_t)0);

            if (rv)
            {
                catd_log_f(SIM_LOGGING_E, "catd : tpdl retry failed in fail state");
                //keep the current state
                return;
            }
            if( enable_status == CATD_CAT_ENABLE_STATUS_ENABLED ) {
                enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
            }
            /* Trigger PIN caching if supported in UICCD */
            uiccd_sig_uicc_status_ind((uintptr_t)0, STE_UICC_STATUS_INTERNAL_CARD_RECOVERY );
            g_tpdl.state = CATD_TPDL_STATE_START;
        }
        break;
        case CATD_TPDL_EVENT_MODEM_IND_REG_OK:
        {
            //silent download returned ok
            catd_log_f(SIM_LOGGING_I, "catd : got CATD_TPDL_EVENT_MODEM_IND_REG_OK in failed state, silent download.");

            catd_log_f(SIM_LOGGING_D, "catd : silent terminal profile download succeeded!");
            g_tpdl.state = CATD_TPDL_STATE_SUCCESS;

            catd_send_cat_info_ind(STE_CAT_INFO_SUCCESS);
            catd_change_cat_status(CATD_STATE_READY);
            uiccd_sig_modem_state( 0, 0 );   //SEND ready signal to uiccd
            catd_send_cat_status_ind();
            //currently the initiator can only be CATD_TPDL_INITIATOR_SILENT_DOWNLOAD
            if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED) //this means previous download was successful
            {
                //for silent download, enable cat service has been done previously
                catd_log_f(SIM_LOGGING_I, "catd : cat has been enabled previously by client.");
            }
            else
            {
                //continue with normal startup procedure
                //TPDL operation succeeded, so notify client and continue with other startup procedures
                catd_log_f(SIM_LOGGING_I, "catd : previous tpdl failed.");
                simd_sig_startup_completed(startup_fd, SIMD_DAEMON_CATD, 0, startup_ct);
                //cat enable will be done by client
            }
            gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
            CATD_PROCEED_WITH_REQUEST;
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: wrong tpdl event!");
            return;
        }
    }
}

/*************************************************************************
 * @brief:    event handler for tpdl state machine state success.
 * @params:
 *            tpdl_event:       the input event for state machine.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_state_success_event_handler(catd_tpdl_event_t tpdl_event)
{
    switch (tpdl_event)
    {
        case CATD_TPDL_EVENT_MODEM_IND_REG_FAIL:
        {
            catd_log_f(SIM_LOGGING_E, "catd : got CATD_TPDL_EVENT_MODEM_IND_REG_FAIL, tpdl failed.");
            g_tpdl.state = CATD_TPDL_STATE_FAIL;
        }
        break;
        case CATD_TPDL_EVENT_MODEM_IND_REG_OK:
        {
            catd_log_f(SIM_LOGGING_I, "catd : got CATD_TPDL_EVENT_MODEM_IND_REG_OK.");
        }
        break;
        case CATD_TPDL_EVENT_MODEM_READY_IND: // Card Recovery
        {
            catd_log_f(SIM_LOGGING_I, "catd : got CATD_TPDL_MODEM_READY_IND. Initiate Recovery");

            g_tpdl.retry = 0;
            g_tpdl.initiator = CATD_TPDL_INITIATOR_MODEM_READY;
            g_tpdl.try_times = 0;
            if ( catd_download_terminal_profile((uintptr_t)0))
            {
                catd_log_f(SIM_LOGGING_E, "catd : tpdl failed in SUCCESS state(Recovery)");
                g_tpdl.state = CATD_TPDL_STATE_FAIL;
                return;
            }
            g_tpdl.state = CATD_TPDL_STATE_START;
            if( enable_status == CATD_CAT_ENABLE_STATUS_ENABLED ) {
                enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
            }
            uiccd_sig_uicc_status_ind((uintptr_t)0, STE_UICC_STATUS_INTERNAL_CARD_RECOVERY );
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: wrong tpdl event!");
            return;
        }
    }
}

/*************************************************************************
 * @brief:    state machine for tpdl operation.
 * @params:
 *            tpdl_event:       the input event for state machine.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_tpdl_state_machine(catd_tpdl_event_t tpdl_event)
{
    catd_log_f(SIM_LOGGING_D, "catd : TPDL state machine: get event %d in state %d.", tpdl_event, g_tpdl.state);

    //check the validity of event
    if (tpdl_event >= CATD_TPDL_EVENT_INVALID)
    {
        catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: invalid event!");
        //keep the current state
        return;
    }

    catd_log_f(SIM_LOGGING_I, "catd : catd_tpdl_state_machine: g_tpdl.initiator = %d, enable_status = %d", g_tpdl.initiator, enable_status);
    switch (g_tpdl.state)
    {
        case CATD_TPDL_STATE_NULL:
        {
            catd_tpdl_state_null_event_handler(tpdl_event);
        }
        break;
        case CATD_TPDL_STATE_START:
        {
            catd_tpdl_state_start_event_handler(tpdl_event);
        }
        break;
        case CATD_TPDL_STATE_SUCCESS:
        {
            //All current defined events except CATD_TPDL_EVENT_MODEM_IND_REG_FAIL are ignored in success state
            catd_tpdl_state_success_event_handler(tpdl_event);
        }
        break;
        case CATD_TPDL_STATE_FAIL:
        {
            catd_tpdl_state_fail_event_handler(tpdl_event);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : TPDL state machine: wrong state!");
            return;
        }
    }
}

/*************************************************************************
 * @brief:    convert the two bytes status value into CATD/client format.
 * @params:
 *            status_words:       status words from modem.
 *            client_value_p:     the status value mapped to the #define in sim.h.
 *
 * @return:   void
 *
 * Notes:     please refer to specification 3gpp 51.011 for specific status words values
 *            some CATD format values are grouped into one value for the client
 *************************************************************************/
void catd_convert_modem_status_words( ste_cat_status_words_t status_words,
                                      uint16_t * client_value_p)
{
    if (!client_value_p) {
        return;
    }

    switch (status_words.sw1)
    {
        case 0x90:
        case 0x91:
        {
            *client_value_p = STE_CAT_RESPONSE_OK;
        }
        break;
        case 0x92:
        {
            if (status_words.sw2 == 0x40) {
                *client_value_p = STE_CAT_RESPONSE_CARD_MEMORY_PROBLEM;
            }
            else {
                *client_value_p = STE_CAT_RESPONSE_OK;
            }
        }
        break;
        case 0x93:
        {
            *client_value_p = STE_CAT_RESPONSE_CARD_BUSY;
        }
        break;
        case 0x67:
        {
            *client_value_p = STE_CAT_RESPONSE_INCORRECT_PARAMETER;
        }
        break;
        case 0x6B:
        case 0x6D:
        case 0x6E:
        case 0x6F:
        {
            *client_value_p = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
        }
        break;
        default:
        {
            *client_value_p = (status_words.sw1 * 256) + status_words.sw2;
        }
    }
}

/*************************************************************************
 * @brief:    Notify clients that the SIM preferred RAT mode has changed
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_rat_setting_updated_ind()
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    catd_log_f(1, "catd : catd_send_rat_setting_updated_ind. Sending RAT setting updated ind to registered clients!");

    //go through the client list to find those clients who are interested in the session end indication
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_RAT_SETTING_UPDATED) ==
                STE_CAT_CLIENT_REG_EVENTS_RAT_SETTING_UPDATED)
        {
            result = catd_send_status_response(gcatd.client_list[i].registered_fd,
                                               0,  //for indication, the client tag should be set to 0
                                               STE_CAT_IND_RAT_SETTING_UPDATED,
                                               0);
            if (result == 0) {
                success_count++;
            }
        }
    }
    return success_count;
}
/*************************************************************************
 * @brief:    Response handler for uiccd reset request
 *
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   0 on success
 *************************************************************************/
static int catd_uiccd_refresh_reset_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_uiccd_response_t*     msg    = (catd_msg_uiccd_response_t*)ste_msg;
    ste_sim_ctrl_block_t*          ctrl_p = NULL; // Use _p naming convention to allow searching for ctrl_p.

    catd_log_f(SIM_LOGGING_D, "catd : %s", __PRETTY_FUNCTION__);
    assert(msg);

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p); // There must be a control block in the message

    if (ctrl_p->client_tag == CATD_CLIENT_TAG
        && ctrl_p->fd == (int)CATD_FD
        && ctrl_p->transaction_id == (uint32_t)CATD_MSG_PC_REFRESH)
    {
        catd_log_f(SIM_LOGGING_D, "catd : get response for uiccd reset request for PC refresh");
    }

    return 0;
}

/*
 * Invoke the transaction handler.
 *
 * This interprets the client tag inside a control block as a transaction handler
 * function pointer and then calls that function.
 *
 * Returns zero if all is well and non-zero if something went wrong.
 */
int catd_main_dispatch_using_clienttag(ste_msg_t* ste_msg)
{
    ste_msg_void_t* msg  = (ste_msg_void_t*)ste_msg; // Always safe to cast to base message
    int rc = -1; // Assume failure until all is well

    if (msg) {
        // Use _p naming convention to allow searching for ctrl_p.
        ste_sim_ctrl_block_t* ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
        if (ctrl_p && ctrl_p->transaction_handler_func) {
            catd_log_f(SIM_LOGGING_D, "catd : %s, func = 0x%08X", __PRETTY_FUNCTION__, ctrl_p->transaction_handler_func);
            if (!ctrl_p->transaction_handler_func(ste_msg)) {
                // Transaction completed.
                CATD_PROCEED_WITH_REQUEST;
            }
            rc = 0; // All is ok!
        }
        free(ctrl_p);
    }

    if (rc) {
        catd_log_f(SIM_LOGGING_E, "catd : %s error, msg = 0x%08X", __PRETTY_FUNCTION__, msg);
    }
    return rc;
}

/*************************************************************************
 * @brief:    register request from the client.
 * @params:
 *            fd:          the socket fd of the client.
 *            buf_p:       data related to this register operation.
 *            lenp:        length of the data.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_register(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_register_t    *msg;

    msg = catd_msg_register_create(fd, buf_p, len, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for register request from the client.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     one client is only allowed to register for one time, but may be changed
 *            if we want one client to register twice to listen to sync and async events
 *************************************************************************/
void catd_main_register(ste_msg_t * ste_msg)
{
    uint8_t   i;
    uint8_t  *p;
    uint8_t  *p_max;
    uint32_t  reg_events;
    int       is_registered = 0;
    catd_msg_register_t    *msg = (catd_msg_register_t *) ste_msg;

    catd_log_f(SIM_LOGGING_D, "catd : REGISTER message received fd = %d", msg->fd);
    //decode the reg_events data
    p = msg->buf_p;
    p_max = p + msg->len;
    p = (uint8_t*)sim_dec((char*)p, &reg_events, sizeof(reg_events), (char*)p_max);
    if ( !p )
    {
        catd_log_f(SIM_LOGGING_E, "catd : data from socket might have some error.");
    }

    //check if this client has been registered
    //if yes, just update the related info
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == msg->fd)
        {
            gcatd.client_list[i].registered_events = reg_events;
            gcatd.client_list[i].req_fd = 0;     //currently set to 0 because the infrastructure does not support two socket fd yet
            is_registered = 1;

            catd_log_f(SIM_LOGGING_V, "catd : re-registering fd = %d, events = 0x%08X, ctag = 0x%X",
                       msg->fd,
                       reg_events,
                       msg->client_tag);

            break;
        }
    }

    //not registered, check if there is place for this client
    if (!is_registered)
    {
        for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
        {
            if (gcatd.client_list[i].registered_fd == -1)
            {
                gcatd.client_list[i].registered_fd = msg->fd;
                gcatd.client_list[i].registered_events = reg_events;
                gcatd.client_list[i].req_fd = 0;     //currently set to 0 because the infrastructure does not support two socket fd yet

                catd_log_f(SIM_LOGGING_V, "catd : registering fd = %d, events = 0x%08X, ctag = 0x%X",
                           msg->fd,
                           reg_events,
                           msg->client_tag);

                break;
            }
        }
    }
    if (i == STE_CATD_MAX_NBR_OF_CLIENTS) {
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_REGISTER, -1);
    }
    else {
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_REGISTER, 0);
    }
}

/*************************************************************************
 * @brief:    deregister request from the client.
 * @params:
 *            fd:          the socket fd of the client.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_deregister(int fd, uintptr_t client_tag)
{
    catd_msg_deregister_t  *msg;

    msg = catd_msg_deregister_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for deregister request from the client.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     to deregister from the client list, not to listen to CAT events
 *************************************************************************/
void catd_main_deregister(ste_msg_t * ste_msg)
{
    int client_id = -1;
    catd_msg_deregister_t  *msg = (catd_msg_deregister_t *) ste_msg;

    catd_log_f(SIM_LOGGING_D, "catd : DEREGISTER message received fd = %d", msg->fd);
    //deregister this client
    client_id = catd_get_client_id_from_fd(msg->fd);

    if (client_id == -1) {
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_DEREGISTER, -1);
        catd_log_f(SIM_LOGGING_E, "catd : no registered client on fd = %d", msg->fd);
    }
    else {
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_DEREGISTER, 0);
        catd_client_reset(&(gcatd.client_list[client_id]));
    }
}

// -----------------------------------------------------------------------------
// Startup
//FIXME: there should not be any startup, Remove them!
void catd_sig_startup(int fd, uintptr_t client_tag)
{
    catd_msg_startup_t   *msg;
    msg = catd_msg_startup_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    To tell CATD that modem is ready to proceed with TPDL.
 * @params:
 *            client_tag:  client tag of the request.
 *
 * @return:   void
 *
 * Notes:     The modem call the catd_sig_modem_ready() when it is ready to receive the
 *            terminal profile download.
 *            this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_ready(uintptr_t client_tag)
{
    catd_msg_modem_ready_t *msg;

    msg = catd_msg_modem_ready_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for modem ready eventfrom the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     the terminal profile will be sent down to modem upon getting this msg
 *************************************************************************/
static void catd_main_modem_ready(ste_msg_t* ste_msg)
{
    catd_msg_modem_ready_t *msg;

    catd_log_f(SIM_LOGGING_D, "catd : MODEM READY message received");
    msg = (catd_msg_modem_ready_t *) ste_msg;
    if (g_tpdl.initiator == CATD_TPDL_INITIATOR_NULL) {
        g_tpdl.initiator = CATD_TPDL_INITIATOR_MODEM_READY;
    }
    catd_tpdl_state_machine(CATD_TPDL_EVENT_MODEM_READY_IND);

    return;
}

/*************************************************************************
 * @brief:    To tell CATD that modem is not ready to proceed with CAT requests.
 * @params:
 *            client_tag:  client tag of the request.
 *
 * @return:   void
 *
 * Notes:     The modem responds with this if any commands are sent to the modem before
 *            Terminal Profile Download has happened.
 *            this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_not_ready(uintptr_t client_tag)
{
    catd_msg_modem_not_ready_t *msg;

    msg = catd_msg_modem_not_ready_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    main handler function for modem not ready event from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     do nothing currently
 *************************************************************************/
static void catd_main_modem_not_ready(ste_msg_t* UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_E, "catd : MODEM NOT READY message received");
    catd_tpdl_state_machine(CATD_TPDL_EVENT_MODEM_IND_REG_FAIL);
    if (gcatd.state == CATD_STATE_READY) {
        catd_change_cat_status(CATD_STATE_MODEM_ERROR);
        catd_send_cat_status_ind();
    }
}

/*************************************************************************
 * @brief:    To tell CATD that modem is reset, Ex., by a 3G to 2G fallback.
 * @params:
 *            client_tag:  client tag of the request.
 *
 * @return:   void
 *
 * Notes:     The modem call the catd_sig_modem_reset() when modem is reset,
 *            for example, by a 3G card to 2G card fallback.
 *            this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_reset(uintptr_t client_tag)
{
    catd_msg_modem_reset_t *msg;

    msg = catd_msg_modem_reset_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for modem reset event from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     the catd will be reset and terminal profile req will be issued
 *************************************************************************/
static void catd_main_modem_reset(ste_msg_t* UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_D, "catd : MODEM RESET message received");

    catd_change_cat_status(CATD_STATE_INIT);
    catd_send_cat_status_ind();
    catd_tpdl_reset();
    g_tpdl.initiator = CATD_TPDL_INITIATOR_MODEM_READY;
    if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED)
    {
        //previously client enabled cat successfully
        enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
    }

    //download terminal profile
    catd_tpdl_state_machine(CATD_TPDL_EVENT_MODEM_READY_IND);

    return;
}

/*************************************************************************
 * @brief:    Inform the cat thread the modem registration ok with cat server for terminal profile download.
 * @params:
 *            client_tag:  client tag of the ind.
 *
 * @return:   void
 *
 * Notes:     The modem sends out this when a tpdl is successfully done.
 *            this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_ind_reg_ok(uintptr_t client_tag)
{
    catd_msg_modem_ind_reg_ok_t *msg;

    msg = catd_msg_modem_ind_reg_ok_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    main handler function for modem not ready event from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     do nothing currently
 *************************************************************************/
static void catd_main_modem_ind_reg_ok(ste_msg_t* UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_D, "catd : MODEM IND REG OK message received");
    catd_tpdl_state_machine(CATD_TPDL_EVENT_MODEM_IND_REG_OK);
}

void catd_sig_timeout_event(uintptr_t timeout_event)
{
    catd_msg_timeout_event_t* msg;

    catd_log_f(SIM_LOGGING_D, "catd : %s, event = 0x%X", __PRETTY_FUNCTION__, timeout_event);

    msg = catd_msg_timeout_event_create(timeout_event);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*)msg);
    }
}

static void catd_main_timeout_event(ste_msg_t* ste_msg)
{
    catd_msg_timeout_event_t* msg = (catd_msg_timeout_event_t*)ste_msg;
    if (msg) {
        uint32_t timeout_event = (uint32_t)msg->client_tag;
        uintptr_t userdata_0;
        if (simd_timer_get_data_from_event(timeout_event, &userdata_0, NULL, NULL, NULL) == 0) {
            simd_timer_callback* callback = (simd_timer_callback*)userdata_0;
            if (callback) {
                catd_log_f(SIM_LOGGING_D, "catd : %s, callback = 0x%X, event = 0x%X", __PRETTY_FUNCTION__, callback, timeout_event);
                callback(timeout_event);
            } else {
                catd_log_f(SIM_LOGGING_D, "catd : %s, callback not set, event = 0x%X", __PRETTY_FUNCTION__, timeout_event);
            }
        } else {
            catd_log_f(SIM_LOGGING_D, "catd : %s, non-valid event = 0x%X", __PRETTY_FUNCTION__, timeout_event);
        }
    } else {
        catd_log_f(SIM_LOGGING_D, "catd : %s, null message received", __PRETTY_FUNCTION__);
    }
}

/*************************************************************************
 * @brief:    To tell CATD that modem does not support CAT service.
 * @params:
 *            client_tag:  client tag of the request.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_sig_modem_cat_not_supported(uintptr_t client_tag)
{
    catd_msg_modem_cat_not_supported_t *msg;

    msg = catd_msg_modem_cat_not_supported_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    To tell CATD that modem has activated app
 * @params:
 *            client_tag:  client tag of the request.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_sig_app_ready(uintptr_t client_tag, int status)
{
    catd_msg_app_ready_t *msg;

    msg = catd_msg_app_ready_create(client_tag, status);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for cat not supported event from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_main_modem_cat_not_supported(ste_msg_t* UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_E, "catd : MODEM CAT NOT SUPPORTED message received");
    catd_change_cat_status(CATD_STATE_MODEM_ERROR);
    catd_send_cat_status_ind();
    catd_send_cat_info_ind(STE_CAT_INFO_NOT_SUPPORTED);
}

static void catd_main_app_ready(ste_msg_t* ste_msg)
{
    catd_msg_app_ready_t *msg = (catd_msg_app_ready_t *)ste_msg;
    int rv;
    catd_adapt_terminal_support_table_t table;

    if (msg->i == 0) {
        catd_log_f(SIM_LOGGING_V, "catd : Got app ready OK");
        catd_main_load_adapt_terminal_support_table(&table);
        rv = ste_catd_modem_adapt_terminal_support_table_update(
            catd_get_modem(),
            0, 0,
            (const uint8_t *)&table,
            sizeof(table));
        if (rv != 0) {
            // Failing to write to the file, marks the end for Adapt
            gcatd.adapt_status = CAT_ADAPT_STATUS_UNSUPPORTED;
            catd_log_f(SIM_LOGGING_E, "catd : adapt dl call failed");
        }

    }
    else {
        catd_log_f(SIM_LOGGING_V, "catd : Got app ready NOT OK");
    }
}
/*************************************************************************
 * @brief:    Terminal profile download status callback.
 * @params:
 *            client_tag:  client tag of the request.
 *            status:      status of the TPDL operation.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_tpdl_status(uintptr_t client_tag, int status)
{
    catd_msg_tpdl_status_t *msg;

    catd_log_f(SIM_LOGGING_E, "catd : catd_sig_tpdl_status called");

    msg = catd_msg_tpdl_status_create(client_tag, status);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }

}

/*************************************************************************
 * @brief:    Adapt terminal support update status callback.
 * @params:
 *            client_tag:           client tag of the request.
 *            adapt_file_updated:   adapt file updated.
 *            status_word:          the status word retrieve.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_adapt_terminal_support_table_update_status(uintptr_t client_tag,
                                                         boolean adapt_file_updated,
                                                         sim_uicc_status_word_t status_word)
{
     catd_msg_adapt_terminal_support_table_update_status_t *msg;

     msg = catd_msg_adapt_terminal_support_table_update_create(client_tag,
                                                         adapt_file_updated,
                                                         status_word);
     if (msg) {
          ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
     }

}

/*************************************************************************
 * @brief:    Modem enable status callback.
 * @params:
 *            client_tag:  client tag of the request.
 *            status:      status of the modem enable operation.
 *
 * @return:   void
 *
 * Notes:     Based on the status value received from the modem, catd should
 *            update its state machine.
 *************************************************************************/
void catd_sig_enable_status(uintptr_t client_tag, int status)
{
    catd_msg_enable_status_t *msg;

    msg = catd_msg_enable_status_create(client_tag, status);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    Modem enable status callback.
 * @params:
 *            client_tag:  client tag of the request.
 *            status:      status of the modem disable operation.
 *
 * @return:   void
 *
 * Notes:     Based on the status value received from the modem, catd should
 *            update its state machine.
 *************************************************************************/
void catd_sig_disable_status(uintptr_t client_tag, int status)
{
    catd_msg_disable_status_t *msg;

    msg = catd_msg_disable_status_create(client_tag, status);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for tpdl status from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     upon getting this msg, the cat status will be broadcast to clients
 *            And CATD needs to notify UICCD about this
 *************************************************************************/
static void catd_main_tpdl_status(ste_msg_t* ste_msg)
{
    catd_msg_tpdl_status_t *msg;

    msg = (catd_msg_tpdl_status_t *) ste_msg;
    catd_log_f(SIM_LOGGING_D, "catd : TPDL status message received status = %d ", msg->i);

    if (msg->i == 0)
    {
        catd_tpdl_state_machine(CATD_TPDL_EVENT_RESP_OK);
    }
    else
    {
        catd_tpdl_state_machine(CATD_TPDL_EVENT_RESP_NOK);
    }
}

/*************************************************************************
 * @brief:    main handler function for adapt terminal support table update
 *            status from the modem.
 * @params:
 *            ste_msg: the msg for the particular request, with related data.
 *
 * @return:   void
 *
 *************************************************************************/
static void catd_main_adapt_terminal_support_table_update_status(ste_msg_t* ste_msg)
{
    catd_msg_adapt_terminal_support_table_update_status_t *msg = (catd_msg_adapt_terminal_support_table_update_status_t *)ste_msg;
    ste_sim_ctrl_block_t   *ctrl_p;
    int rv;

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (ctrl_p) {
        free(ctrl_p);
        ctrl_p = NULL;
    }


    if (msg->adapt_file_updated) {
        // If file update is ok, then adapt
        catd_log_f(SIM_LOGGING_V, "catd : Supporting adapt for this sim");
        gcatd.adapt_status = CAT_ADAPT_STATUS_SUPPORTED;

        if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED ||
            enable_status == CATD_CAT_ENABLE_STATUS_DISABLED_TEMP)
        {
            catd_log_f(SIM_LOGGING_E, "catd : cat service has already been enabled!");
            CATD_PROCEED_WITH_REQUEST;
            return;
        }

        // Enable CAT early for ADAPT to be able to process proactive commands during startup
        rv = catd_cat_enable();
        if (rv != 0) {
            catd_change_cat_status(CATD_STATE_MODEM_ERROR);
            catd_log_f(SIM_LOGGING_E, "catd : Could not enable cat after tpdl \n");
        }

    } else {
        // If file update failed, then no adapt
        catd_log_f(SIM_LOGGING_V, "catd : Not supporting adapt for this sim");
        gcatd.adapt_status = CAT_ADAPT_STATUS_UNSUPPORTED;
    }
    CATD_PROCEED_WITH_REQUEST;
}
/*************************************************************************
 * @brief:    Response handler for the default poll interval setting
 *
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   0 on success
 *************************************************************************/
static int catd_main_default_poll_interval_response_handler(ste_msg_t* ste_msg) {
    ste_sim_ctrl_block_t * ctrl_p;
    catd_msg_modem_set_poll_interval_response_t* msg_p;

    msg_p = (catd_msg_modem_set_poll_interval_response_t *) ste_msg;
    assert(msg_p);

    ctrl_p  = (ste_sim_ctrl_block_t *) msg_p->client_tag;
    assert(ctrl_p);

    if (msg_p->status) {
        catd_log_f(SIM_LOGGING_E, "catd : Unable to set the default poll intervall!");
    } else {
        catd_log_f(SIM_LOGGING_D, "catd : Default poll intervall has been set!");
    }

    return 0;
}

/*************************************************************************
 * @brief:    main handler function for modem enabled status from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     Upon getting this msg, the catd should update its state machine.
 *************************************************************************/
static void catd_main_modem_enable_status(ste_msg_t* ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p;
    catd_msg_enable_status_t * msg = (catd_msg_enable_status_t *)ste_msg;

    catd_log_f(1, "catd : MODEM_ENABLED status received");
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p);

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    if (ctrl_p->fd == (int)CATD_FD
        && ctrl_p->client_tag == CATD_CLIENT_TAG_INTERN){
        catd_log_f(SIM_LOGGING_V, "catd_main_modem_enable_status - internal - status = %d",msg->i);

        free(ctrl_p);
        ctrl_p = NULL;
        CATD_PROCEED_WITH_REQUEST;
        return;
    }
#endif

    if (ctrl_p->fd == (int)CATD_FD
        && ctrl_p->client_tag == CATD_CLIENT_TAG
        && enable_status == CATD_CAT_ENABLE_STATUS_DISABLED_TEMP)
    {
        // Initiated after some internal operation for example refresh,
        // SIM insert, SIM on/off where CAT has been enabled before by client.
        catd_log_f(1, "catd : MODEM_ENABLED status received");
    }
    else
    {
        //this is initiated by client
        catd_log_f(1, "catd : MODEM_ENABLED status received for client");
        //the response for the cat enable request should be sent back to client
        if (msg->i == 0)
        {
            catd_send_status_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_ENABLE, 0);
        }
        else
        {
            catd_send_status_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_ENABLE, -1);
        }
    }
    free(ctrl_p);
    ctrl_p = NULL;

    if (msg->i == 0)
    {
        ste_modem_t            *modem_p;

        catd_log_f(1, "catd : MODEM_ENABLED successfully for fetching proactive commands.");

        //cat enable is only done by client for now, even if power on off will trigger this as well
        enable_status = CATD_CAT_ENABLE_STATUS_ENABLED;

        if (gcatd.state != CATD_STATE_READY)
        {
            catd_change_cat_status(CATD_STATE_READY);
            catd_send_cat_status_ind();
        }

        modem_p = catd_get_modem();

        if (!modem_p)
        {
            catd_log_f(SIM_LOGGING_E, "catd : Unable to set the default poll interval, aborting!");
            abort();
        }
        else
        {
            ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                               CATD_FD,
                                               CATD_MSG_NONE,
                                               catd_main_default_poll_interval_response_handler,
                                               NULL);
            if (ctrl_p)
            {
                catd_log_f(SIM_LOGGING_D, "catd : Setting default poll interval to %d", SET_POLL_INTERVALL);
                if (ste_catd_modem_set_poll_intervall(modem_p, (uintptr_t)ctrl_p, SET_POLL_INTERVALL) != 0)
                {
                    catd_log_f(SIM_LOGGING_E, "catd : Could not set the default poll interval!");
                    free(ctrl_p);
                }
            }
            else
            {
                catd_log_f(SIM_LOGGING_E, "catd : Out of memory, could not set the default poll interval!");
            }
        }
    }
    else
    {
        catd_log_f(1, "catd : MODEM_ENABLED failed for fetching proactive commands.");
        if (gcatd.state == CATD_STATE_READY)
        {
            catd_change_cat_status(CATD_STATE_MODEM_ERROR);
        }
    }

    CATD_PROCEED_WITH_REQUEST;
}


/*************************************************************************
 * @brief:    main handler function for modem disabled status from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     Upon getting this msg, the catd should update its state machine.
 *************************************************************************/
static void catd_main_modem_disable_status(ste_msg_t* ste_msg)
{
    ste_sim_ctrl_block_t      * ctrl_p;
    catd_msg_disable_status_t * msg = (catd_msg_disable_status_t *)ste_msg;

    catd_log_f(1, "catd : MODEM_DISABLED status received");
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p);
#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    if(env_cahed_apdu.apdu!= NULL)
    {
        cat_disable_outstanding = 0;
        catd_disable_resp_handler();
        return;
    }
#endif

    //this is only initiated by client currently
    //the response for the cat enable request should be sent back to client
    if (msg->i == 0)
    {
        catd_send_status_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_ENABLE, 0);
        enable_status = CATD_CAT_ENABLE_STATUS_DISABLED;
        if (gcatd.state == CATD_STATE_READY)
        {
            catd_change_cat_status(CATD_STATE_MODEM_ERROR);
            catd_send_cat_status_ind();
        }
    }
    else
    {
        catd_send_status_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_ENABLE, -1);
    }
    free(ctrl_p);
    ctrl_p = NULL;

    CATD_PROCEED_WITH_REQUEST;
}



/*************************************************************************
 * @brief:    Inform the cat thread that the state of CAT server is ready.
 *
 * @params:
 *            client_tag:  client tag of the request.
 *            status:      status of the CAT server (0 = ready).
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_sig_cat_server_ready(uintptr_t client_tag, int status)
{
    catd_msg_cat_server_ready_t *msg;

    if (status == 0) {
        msg = catd_msg_cat_server_ready_create(client_tag);
        ste_msgq_add(catd_mq_req, (ste_msg_t *)msg);
    }
}

// Presence of this msg simply means we're ready
static void catd_main_cat_server_ready(ste_msg_t* UNUSED(ste_msg))
{
    if (gcatd.state != CATD_STATE_READY) {
        catd_change_cat_status(CATD_STATE_READY);
        catd_send_cat_status_ind();
    }
}

/*************************************************************************
 * @brief:    APDU request/PC from the modem. This is the main entry point of APDUs in the system.
 * @params:
 *            apdu:        the apdu to be dealt with.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_apdu(ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_apdu_t        *msg;

    //please be aware currently we are NOT saving the client_tag from modem, it is not used anywhere
    //so modem should pass 0 for this parameter. we leave it for possible future usage.
    //if this client_tag is needed in the future, please just save it into a ctrl block and require
    //client to pass it back to simd when TR is sent down
    msg = catd_msg_apdu_create(apdu, client_tag);
    if (msg) {
#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
        if(cat_disable_outstanding) {
            catd_log_f(SIM_LOGGING_E,"catd_sig_apdu:Cat Disable outstanding");
            pc_outstanding = 1;
            ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
            return;
        }
#endif
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    send the proactive command with related APDU to client
 *
 * @params:   apdu_p:      the apdu to be dealt with.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_pc_send_pc_notification_ind(ste_apdu_t *apdu_p) {
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION) ==
                STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION)
        {
            size_t                   buf_len = 0;
            char                   * buf_p = NULL;
            char                   * p = NULL;
            unsigned                 apdu_len = 0;
            uint8_t                * apdu_data_p = NULL;

            //if apdu_p is null, we just leave it
            if (apdu_p) {
                apdu_len = ste_apdu_get_raw_length(apdu_p);
                apdu_data_p = (uint8_t*)ste_apdu_get_raw(apdu_p);
            }

            buf_len += sizeof(apdu_len);
            buf_len += sizeof(char) * apdu_len;

            buf_p = malloc(buf_len);
            if (buf_p == NULL){
                catd_log_f(SIM_LOGGING_E, "catd : catd_pc_send_pc_notification_ind, memory allocation failed");
                continue;
            }

            p = buf_p;
            p = sim_enc(p, &apdu_len, sizeof(apdu_len));
            if (apdu_data_p) {
                p = sim_enc(p, apdu_data_p, sizeof(char) * (apdu_len));
            }

            if (buf_len != (size_t)(p - buf_p)) {
                catd_log_f(SIM_LOGGING_E, "catd : catd_pc_send_pc_notification_ind, assert failure");
            }

            assert( buf_len == (size_t)(p - buf_p) );

            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                      STE_CAT_IND_PC_NOTIFICATION,
                                      buf_p,
                                      buf_len,
                                      0);
            if (result == 0) {
                success_count++;
            }
            free( buf_p );
        }
    }
    return success_count;
}

/*************************************************************************
 * @brief:    send the proactive command with related APDU to client
 *
 * @params:   apdu_p:      the apdu to be dealt with.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_pc_ind(ste_apdu_t *apdu_p)
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_PC) ==
                STE_CAT_CLIENT_REG_EVENTS_PC)
        {
            size_t                   buf_len = 0;
            char                   * buf_p = NULL;
            char                   * p = NULL;
            unsigned                 apdu_len = 0;
            uint8_t                * apdu_data_p = NULL;

            //if apdu_p is null, we just leave it
            if (apdu_p) {
                apdu_len = ste_apdu_get_raw_length(apdu_p);
                apdu_data_p = (uint8_t*)ste_apdu_get_raw(apdu_p);
            }

            buf_len += sizeof(apdu_len);
            buf_len += sizeof(char) * apdu_len;

            buf_p = malloc(buf_len);
            if (buf_p == NULL){
                catd_log_f(SIM_LOGGING_E, "catd : catd_send_pc_ind, memory allocation failed");
                continue;
            }

            p = buf_p;
            p = sim_enc(p, &apdu_len, sizeof(apdu_len));
            if (apdu_data_p) {
                p = sim_enc(p, apdu_data_p, sizeof(char) * (apdu_len));
            }

            if (buf_len != (size_t)(p - buf_p)) {
                catd_log_f(SIM_LOGGING_E, "catd : catd_send_pc_ind, assert failure");
            }

            assert( buf_len == (size_t)(p - buf_p) );

            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                      STE_CAT_IND_PC,
                                      buf_p,
                                      buf_len,
                                      0);
            if (result == 0) {
                success_count++;
            }
            free( buf_p );
        }
    }
    return success_count;
}

/*************************************************************************
 * @brief:    handle proactive command internally
 * @params:
 *            apdu_type:          APDU type to be handled.
 *            msg:                The original message with APDU data.
 *            gui_capable_client: True if an external client that implements
 *                                notifications has indicated that it has a GUI.
 *
 * @return:  -1 if error, caller SHALL not send any PC notification to client.
 *            0 if ok, caller MAY send PC notification to client (if applicable).
 *            1 if ok, caller SHALL not send PC notification to client (will be
 *              taken care of later if applicable).
 *
 * Notes: The return value of 1 is used for asynchronous implementation of
 *        pro-active commands where PC notifications will be sent by the handler
 *        if needed and if applicable.
 *
 *
 *************************************************************************/
static int catd_main_handle_proactive_command(uint8_t          apdu_type,
                                              catd_msg_apdu_t* msg,
                                              boolean          gui_capable_client)
{
    int rc = 0;

    //now handle the PC internally, after it is handled, put a TR message into the catd_mq_rsp by calling catd_sig_tr
    switch (apdu_type)
    {
        case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
        {
            catd_handle_pc_provide_local_info(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_POLL_INTERVAL:
        {
            catd_handle_pc_poll_interval(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_POLLING_OFF:
        {
            catd_handle_pc_polling_off(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
        {
            catd_handle_pc_timer_management(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_MORE_TIME:
        {
            catd_handle_pc_more_time(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
        {
            rc = catd_handle_pc_setup_event_list(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_SET_UP_CALL:
        {
            catd_handle_pc_setup_call(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_SEND_SS:
        {
            rc = catd_handle_pc_send_ss(msg, gui_capable_client);
        }
        break;

        case STE_APDU_CMD_TYPE_SEND_USSD:
        {
            rc = catd_handle_pc_send_ussd(msg, gui_capable_client);
        }
        break;

        case STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE:
        {
            rc = catd_handle_pc_send_short_message(msg, gui_capable_client);
        }
        break;

        case STE_APDU_CMD_TYPE_SEND_DTMF:
        {
            rc = catd_handle_pc_send_dtmf(msg, gui_capable_client);
        }
        break;

        case STE_APDU_CMD_TYPE_REFRESH:
        {
            rc = catd_handle_pc_refresh(msg);
        }
        break;

        case STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION:
        {
            catd_handle_pc_language_notification(msg);
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : unsupported apdu type by CATD.");
        }
    }

    return rc;
}

/*************************************************************************
 * @brief:    main handler function for apdu request from the modem.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     upon getting this msg, CATD will decide if this should be handled
 *            internally or handled by client
 *************************************************************************/
static void catd_main_apdu(ste_msg_t * ste_msg)
{
    catd_msg_apdu_t *msg = (catd_msg_apdu_t *) ste_msg;
    ste_apdu_t      *apdu = msg->apdu;
    uint8_t         apdu_kind;
    uint8_t         cmd_type;
    uint8_t         cmd_qualifier;
    uint8_t         cmd_number;
    uint8_t         apdu_filter;
    boolean         gui_capable_client = FALSE;
    int             rc;
    ste_sat_apdu_error_t rv = STE_SAT_APDU_ERROR_NONE;

    catd_log_f(SIM_LOGGING_D, "catd : APDU message received");
    //this client_tag in the ste_msg is useless for now. But we leave it for possible future usage
    //if needed, we can create a ctrl block and save it with the PC

    catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));

    //check the state, this will never happen since APDU comes from modem
    //if CATD does not deal with the PC, then the session on CARD should be expired after some time
    if (CATD_STATE_READY != gcatd.state) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD, apdu);
        return;
    }

    apdu_kind = ste_apdu_get_kind(apdu);
    //the apdu kind can only be PC
    if (apdu_kind != STE_APDU_KIND_PC) {
        catd_log_f(SIM_LOGGING_E, "catd : incorrect apdu kind");
        return;
    }

    rv = ste_apdu_get_cmd_details(apdu, &cmd_type, &cmd_qualifier, &cmd_number);
    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : error retriving command details from apdu");
        pc_send_terminal_response_error(rv, apdu);
        return;
    }

    //handle this PC
    //first to decide through the filter who should handle this PC
    apdu_filter = catd_get_filter_for_cmd(cmd_type);

    // If command can be handled both internally and by client we need to decide who should terminate the command
    if ((apdu_filter & STE_CAT_CMD_FILTER_HANDLE_BY_CATD) && (apdu_filter & STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT)) {
        if (cmd_type == STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
            apdu_filter = catd_get_filter_for_provide_local_info(cmd_qualifier);
        } else {
            // Command will be handled internally
            apdu_filter -= STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT;
        }
    }

    //check if this APDU should be handled internally or by client
    if (apdu_filter & STE_CAT_CMD_FILTER_HANDLE_BY_CATD) {
        catd_log_f(SIM_LOGGING_D, "catd : handle PC by CATD internally, PC type = 0x%x", cmd_type);

        if (apdu_filter & STE_CAT_CMD_FILTER_NOTIFY_CLIENT) {

            // A client is required for setup call
            if (cmd_type == STE_APDU_CMD_TYPE_SET_UP_CALL &&
                    !catd_get_client_count_from_registered_events(STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION)) {
                catd_log_f(SIM_LOGGING_D, "catd : %s SET_UP_CALL should not be handled, no registered client \n", __PRETTY_FUNCTION__);
                pc_send_terminal_response_error(STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD, apdu);
                return;
            }

            //Find if we have a gui capable client that handle PCs
            gui_capable_client = catd_get_client_count_from_registered_events(STE_CAT_CLIENT_REG_EVENTS_GUI_CAPABLE_CLIENT +
                                                                              STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION);
        }

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
        pc_outstanding = 1;
#endif
        rc = catd_main_handle_proactive_command(cmd_type, msg, gui_capable_client);

        // If notification should be sent and the pro-active handler said it's ok,
        // then send the APDU to the client now
        if (!rc && (apdu_filter & STE_CAT_CMD_FILTER_NOTIFY_CLIENT)) {
            catd_pc_send_pc_notification_ind(apdu);
        }
        gcatd.ongoing_cmd = cmd_type | STE_CATD_CMD_MASK_PC;
        CATD_WAITING_FOR_RESPONSE;
    } else if (apdu_filter & STE_CAT_CMD_FILTER_HANDLE_BY_CLIENT) {
        if (catd_send_pc_ind(apdu) < 1) {
            //no client is interested in PC
            pc_send_terminal_response_error(STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD, apdu);
            catd_log_f(SIM_LOGGING_E, "catd : NO client is interested in proactive command!, cmd type: 0x%x", cmd_type);
            return;
        }

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
        pc_outstanding = 1;
#endif

        gcatd.ongoing_cmd = cmd_type | STE_CATD_CMD_MASK_PC;
        CATD_WAITING_FOR_RESPONSE;
    } else {
        catd_log_f(SIM_LOGGING_E, "catd : Unsupported proactive command !, cmd type: 0x%x qualifier: 0x%x", cmd_type, cmd_qualifier);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_COMMAND_BEYOND_ME_CAPABILITY, apdu);
        return;
    }
}

/*************************************************************************
 * @brief:    Envelope command request from the client (or CATD internally).
 * @params:
 *            fd:          the socket fd of the request.
 *            apdu:        the apdu to be dealt with.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_ec(int fd, ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_ec_t          *msg;

    msg = catd_msg_ec_create(fd, apdu, client_tag);
    if (msg) {
        //Add to tail of queue, ec are not prioritized over other msg
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    Call and Network request response/event callback
 * @params:
 *            cn_msg:      The original CN msg.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_cn(cn_message_t * cn_msg)
{
    catd_msg_cn_t *msg;
    msg = catd_msg_cn_create(cn_msg);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*) msg);
    }
}

/*************************************************************************
 * @brief:    send the CAT response from modem to client.
 * @params:
 *            fd:          the socket fd of the request.
 *            client_tag:  client tag of the client.
 *            msg_type:    the message type for the client.
 *            apdu_p:      the apdu to be dealt with.
 *            status:      the status words for client.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_send_cat_response_to_client(int fd,
                                             uintptr_t client_tag,
                                             uint16_t msg_type,
                                             ste_apdu_t *apdu_p,
                                             int status)
{
    char                   * p = NULL;
    char                   * buf = NULL;
    unsigned                 status_len = 0;
    unsigned                 apdu_len = 0;
    uint8_t                * apdu_data_p = NULL;
    size_t                   buf_len;

    if (fd == -1) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_cat_response_to_client, invalid fd");
        return;
    }
    status_len = sizeof(status);
    //if apdu_p is null, we just leave it
    if (apdu_p) {
        apdu_len = ste_apdu_get_raw_length(apdu_p);
        apdu_data_p = (uint8_t*)ste_apdu_get_raw(apdu_p);
    }

    buf_len = sizeof(char) * (status_len) + sizeof(status_len);
    buf_len += sizeof(apdu_len);
    buf_len += sizeof(char) * apdu_len;

    buf = malloc(buf_len);
    if (buf == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_cat_response_to_client, memory allocation failed");
        return;
    }
    p = buf;
    p = sim_enc(p, &status_len, sizeof(status_len));
    p = sim_enc(p, &status, sizeof(char) * (status_len));

    p = sim_enc(p, &apdu_len, sizeof(apdu_len));
    if (apdu_data_p) {
        p = sim_enc(p, apdu_data_p, sizeof(char) * (apdu_len));
    }

    if (buf_len != (size_t)(p - buf)) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_cat_response_to_client, assert failure");
    }

    assert( buf_len == (size_t)(p - buf) );

    sim_send_generic(fd, msg_type, buf, buf_len, client_tag);
    free( buf );
}

/*************************************************************************
 * @brief:    the CAT response handler for a envelope command.
 * @params:
 *            ste_msg:     the msg for the particular response, with related data.
 *
 * @return:   the state of the session/transaction
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_cat_response_for_ec_handler(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t  *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    ste_sim_ctrl_block_t           *ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    ste_cat_status_words_t          status_words;
    int                             result;
    uint16_t                        client_rsp;


    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : Control Block is NULL in handler function for CAT response");
        return 0;  //the transaction is over anyway even if something is wrong during the session handle process
    }
    if (ctrl_p->transaction_id != CATD_MSG_EC) {
        catd_log_f(SIM_LOGGING_E, "catd : error transaction id for CAT response");
        return 0;  //the transaction is over anyway even if something is wrong during the session handle process
    }

    result = ste_apdu_get_catd_status_words(msg->apdu,
                                            (ste_apdu_status_words_t *)&status_words);

    if (result) {
        catd_log_f(1, "catd : catd_cat_response_for_ec_handler. No status words found in the apdu!");
        client_rsp = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
    } else {
        //check the status words, and based on that, create a client response
        catd_convert_modem_status_words(status_words, &client_rsp);
    }

    catd_log_f(1, "catd : catd_cat_response_for_ec_handler msg->status=0x%x sw1=0x%x sw2=0x%x", msg->status,
                                                                                                status_words.sw1,
                                                                                                status_words.sw2);
    CATD_PROCEED_WITH_REQUEST;

    if (ctrl_p->fd != (int)CATD_FD) {
        catd_send_cat_response_to_client(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_EC, msg->apdu, client_rsp);
    }

    if ( ctrl_p->transaction_data_p != NULL ) {

        uint8_t apdu_type = *(uint8_t*)ctrl_p->transaction_data_p;
        if( apdu_type == SET_UP_EVENT_LIST_LOCATION_STATUS ) {
            if ( client_rsp == STE_CAT_RESPONSE_OK ) {
                catd_reset_loc_status(1);
            } else {
                catd_reset_loc_status(0);
            }
#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
        } else if ( apdu_type == SET_UP_EVENT_LIST_DATA_AVAILABLE ) {
            catd_log_f(SIM_LOGGING_V, "catd_cat_response_for_ec_handler:free cached Envelope");
            if(env_cahed_apdu.apdu != NULL) {
                ste_apdu_delete(env_cahed_apdu.apdu);
                env_cahed_apdu.apdu->raw_data = NULL;
                env_cahed_apdu.apdu=NULL;
                catd_cat_enable();
                catd_cat_polling_req();
                CATD_WAITING_FOR_RESPONSE;
            }
#endif
        }
        else {
            catd_log_f(SIM_LOGGING_E, "Received EC Resp for Unknown Envelope %02x", apdu_type);
        }
        free(ctrl_p->transaction_data_p);
        ctrl_p->transaction_data_p = NULL;
    } else if ( client_rsp == STE_CAT_RESPONSE_OK ) {
        catd_pc_check_for_location_status_retransmissions();
    }

    return 0;
}

/*************************************************************************
 * @brief:    main handler function for envelope command request from the client.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_ec(ste_msg_t * ste_msg)
{
    catd_msg_ec_t          *msg;
    ste_modem_t            *modem_p;
    ste_apdu_t             *apdu = 0;
    int                     rv;
    ste_sim_ctrl_block_t   *ctrl_p = NULL;
    // Hack the APDU to get the message type from command details */
    uint8_t                 apdu_ec_command_type = 0;

    catd_log_f(SIM_LOGGING_D, "catd : ENVELOPE COMMAND message received");

    msg = (catd_msg_ec_t *) ste_msg;
    apdu = msg->apdu;

    if (CATD_STATE_READY != gcatd.state) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        goto Error;
    }

    if (!apdu) {
        catd_log_f(SIM_LOGGING_E, "catd : EC message has not apdu!");
        goto Error;
    }

    catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        goto Error;
    }
#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    if(!ste_apdu_check_event_data_available(msg->apdu)) {
        catd_log_f(SIM_LOGGING_V, "Event Download received. Cache the envelope");
        env_cahed_apdu.type = msg->type;
        env_cahed_apdu.delete_func = msg->delete_func;
        env_cahed_apdu.client_tag = msg->client_tag;
        env_cahed_apdu.fd = msg->fd;
        env_cahed_apdu.apdu = ste_apdu_copy(msg->apdu);
        rv = catd_cat_disable();

        if (rv) {
            catd_log_f(SIM_LOGGING_E, "catd : envelope caching failed");
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_EC, -1);
            free(ctrl_p);
            ste_apdu_delete(env_cahed_apdu.apdu);
            env_cahed_apdu.apdu->raw_data = NULL;
            env_cahed_apdu.apdu = NULL;
            return;
        }
        return;
    }
#endif
    if ( STE_SAT_APDU_ERROR_NONE == ste_apdu_get_event_from_event_download( apdu, &apdu_ec_command_type) &&
         apdu_ec_command_type == SET_UP_EVENT_LIST_LOCATION_STATUS ) {
        /* Download Loc status */
        uint8_t* command_data = (uint8_t*)malloc(sizeof(uint8_t));
        if ( command_data == NULL ) {
            catd_reset_loc_status(0);
            goto Error;
        }
        *command_data = apdu_ec_command_type;
        catd_log_f(SIM_LOGGING_D, "catd : EC Loc Status!");
        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag,
                                           msg->fd,
                                           msg->type,
                                           catd_cat_response_for_ec_handler,
                                           command_data);
    } else {
    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, catd_cat_response_for_ec_handler, NULL);
    }
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        goto Error;
    }

    rv = ste_catd_modem_ec(modem_p, (uintptr_t)ctrl_p, apdu);
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : envelope command failed");
        if ( ctrl_p->transaction_data_p ) { // Valid only for Location Status currently
            /* Need to reset the location status event flag so that the message may be sent
             * at another point, some other time */
            catd_reset_loc_status(0);
            free(ctrl_p->transaction_data_p);
        }
        free(ctrl_p);
        goto Error;
    }

    gcatd.ongoing_cmd = STE_CATD_CMD_MASK_EC;
    //wait for the response
    CATD_WAITING_FOR_RESPONSE;
    return;
Error:
    if ( msg->fd != (int)CATD_FD ) {
        /* Do Not trigger this response if internal CATD message */
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_EC, -1);
    }
}

/*************************************************************************
 * @brief:    main handler function for call network responses.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_cn_response_handler(ste_msg_t * ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    catd_main_dispatch_using_clienttag(ste_msg);
}

/*************************************************************************

 * @brief:    Terminal response request from the client (or CATD internally).
 * @params:
 *            fd:          the socket fd of the request.
 *            apdu:        the apdu to be dealt with.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_tr(int fd, ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_tr_t          *msg;

    msg = catd_msg_tr_create(fd, apdu, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    internal CAT response handler for CATD
 * @params:
 *            status_words:  status words from modem for the CAT operation.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_handle_tr_cat_response(uint16_t status_words)
{
    //currently, we just print out the status words value. Not too much to do with the CAT response
    catd_log_f(SIM_LOGGING_D, "catd : returned CAT response with status: 0x%x", status_words);
}

/*************************************************************************
 * @brief:    the CAT response handler for a terminal response.
 * @params:
 *            ste_msg:     the msg for the particular response, with related data.
 *
 * @return:   the state of the session/transaction
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_cat_response_for_tr_handler(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t  *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    ste_sim_ctrl_block_t           *ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    ste_cat_status_words_t          status_words;
    uint16_t                        client_rsp;
    uint8_t                         result;
    int                             resend_timer_expiration_ec;
    uint16_t                        timer_info;
    ste_apdu_t                      *apdu_p = NULL;
    uint8_t                         status;

    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : Control Block is NULL in handler function for CAT response");
        return 0;
    }
    if (ctrl_p->transaction_id != CATD_MSG_TR) {
        catd_log_f(SIM_LOGGING_E, "catd : error transaction id for CAT response");
        return 0;
    }

    // Translate status != 0 to failure
    status = (msg->status == 0 ? 0 : 1);

    result = ste_apdu_get_catd_status_words(msg->apdu,
                                            (ste_apdu_status_words_t *)&status_words);
    if (result) {
        catd_log_f(1, "catd : catd_cat_response_for_tr_handler. No status words found in the apdu!");
        client_rsp = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
    } else {
        //check the status words, and based on that, create a client response
        catd_convert_modem_status_words(status_words, &client_rsp);
    }

    catd_log_f(1, "catd : catd_cat_response_for_tr_handler msg->status=0x%02X sw1=0x%02X sw2=0x%02X", msg->status,
                                                                                                      status_words.sw1,
                                                                                                      status_words.sw2);
    CATD_PROCEED_WITH_REQUEST;

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    pc_outstanding = 0;
    catd_log_f(SIM_LOGGING_V, "catd_cat_response_for_tr_handler: TR resp received from the modem");
#endif

    if (status_words.sw1 != 0x91) { // The session has ended
        catd_send_session_end_ind();
    }

    if (ctrl_p->fd == (int)CATD_FD && ctrl_p->client_tag == CATD_CLIENT_TAG) {
        catd_handle_tr_cat_response(client_rsp);
    }
    else {
        catd_send_tr_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_TR, status, &status_words);
    }

    /* After we have sent a response, check if there is any Timer Expiration Envelope Command to send.
     * But, only send it if the response was OK. */
    if (client_rsp == STE_CAT_RESPONSE_OK) {
        resend_timer_expiration_ec = catd_timer_check_for_ec_retransmission(&timer_info, &apdu_p);

        if (resend_timer_expiration_ec == 0) {
            catd_sim_timer_expiration_send_ec(timer_info, apdu_p);
            return 0;
        }
        catd_pc_check_for_location_status_retransmissions();
    }

/*Processing the Response*/
#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    catd_disable_resp_handler();
#endif

    return 0;
}

/*************************************************************************
 * @brief:    main handler function for terminal response request.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 * Notes:
 *************************************************************************/
void catd_main_tr(ste_msg_t * ste_msg)
{
    catd_msg_tr_t          *msg = (catd_msg_tr_t *) ste_msg;
    ste_modem_t            *modem_p;
    ste_apdu_t             *apdu = 0;
    int                     rv;
    ste_sim_ctrl_block_t   *ctrl_p;
    ste_cat_status_words_t  status_words = { 0,0 };

    catd_log_f(SIM_LOGGING_D, "catd : TERMINAL RESPONSE message received");
    //check the state, this will never happen since the TR is after PC and PC is initiated from modem
    if (CATD_STATE_READY != gcatd.state) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        catd_send_tr_response(msg->fd, msg->client_tag, STE_CAT_RSP_TR, 2, &status_words);
        return;
    }

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        catd_send_tr_response(msg->fd, msg->client_tag, STE_CAT_RSP_TR, 2, &status_words);
        return;
    }

    apdu = msg->apdu;
    if (!apdu) {
        catd_log_f(SIM_LOGGING_E, "catd : TR message has not apdu!");
        catd_send_tr_response(msg->fd, msg->client_tag, STE_CAT_RSP_TR, 2, &status_words);
        return;
    }

    catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));

    //create ctrl_p since we are sending down a TR to modem and expecting a CAT response
    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, catd_cat_response_for_tr_handler, NULL);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        catd_send_tr_response(msg->fd, msg->client_tag, STE_CAT_RSP_TR, 2, &status_words);
        return;
    }

    CATD_WAITING_FOR_RESPONSE;
    rv = ste_catd_modem_tr(modem_p, (uintptr_t)ctrl_p, apdu);
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : terminal response failed");
        catd_send_tr_response(ctrl_p->fd, ctrl_p->client_tag, STE_CAT_RSP_TR, 2, &status_words);
        free(ctrl_p);
        return;
    }

    //we are still waiting for the response for TR, so keep the working_mq as catd_mq_rsp
}


/*************************************************************************
 * @brief:    handle CAT response from the modem
 * @params:
 *            client_tag:    the original client_tag sent down to modem.
 *            apdu:          the apdu to be dealt with.
 *            status_words:  status words for the particular CAT operation.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_cat_response(uintptr_t client_tag, ste_apdu_t * apdu, int status)
{
    catd_msg_modem_cat_response_t *msg;

    msg = catd_msg_modem_cat_response_create(status, apdu, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for CAT response request.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     use the callback function in the ctrl block to handle
 *************************************************************************/
void catd_main_modem_cat_response(ste_msg_t* ste_msg)
{
    catd_msg_modem_cat_response_t *msg = (catd_msg_modem_cat_response_t*) ste_msg;
    ste_sim_ctrl_block_t   *ctrl_p;

    catd_log_f(SIM_LOGGING_D, "catd : CAT RESPONSE message received");

    //this is a response from client, the client tag in the message is a ctrl block created by CATD
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Control Block is NULL in CAT response");
        return;
    }
    //call the callback function in ctrl block to handle the cat response
    if (ctrl_p->transaction_handler_func) {
        if (0 == ctrl_p->transaction_handler_func(ste_msg)) {
            //the transaction is done, set to catd_mq_req
            gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
        }
    }
    else {
        catd_log_f(SIM_LOGGING_E, "catd : transaction handler function is null.");
    }
    //free the ctrl block at the end
    free(ctrl_p);
}

/*************************************************************************
 * @brief:    Handle CAT set poll interval response from the modem
 * @params:
 *            status:      The status of the set poll interval operation, 0 == success, !0 == fail.
 *            interval:    The poll interval used by the modem in tenths (1/10) of seconds (may differ from the value specified).
 *            client_tag:  The client tag used when the request was initiated.
 *
 * @return:   void
 *
 * Notes:     This request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_set_poll_interval_response(int status, int interval, uintptr_t client_tag)
{
    catd_msg_modem_set_poll_interval_response_t *msg;

    msg = catd_msg_modem_set_poll_interval_response_create(status, interval, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*) msg);
    }
}

/*************************************************************************
 * @brief:    Main handler function for CAT set poll interval response.
 * @params:
 *            ste_msg:     the msg for the particular request, with related
 *                         data.
 *
 * @return:   void
 *
 * Notes:     Use the callback function in the ctrl block to point to the real handler.
 *************************************************************************/
void catd_main_modem_set_poll_interval_response(ste_msg_t* ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    catd_main_dispatch_using_clienttag(ste_msg);
}


/*************************************************************************
 * @brief:    Handle CAT refresh response from the modem
 * @params:
 *            status:      The status of the refresh operation, 0 == success, !0 == fail.
 *            client_tag:  The client tag used when the request was initiated.
 *
 * @return:   void
 *
 * Notes:     This request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_refresh_response(uintptr_t client_tag, int status)
{
    catd_msg_modem_refresh_status_t *msg;

    msg = catd_msg_modem_refresh_status_create(client_tag, status);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*) msg);
    }
}

/*************************************************************************
 * @brief:    Main handler function for CAT refresh response.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     Use the callback function in the ctrl block to point to the real handler.
 *************************************************************************/
void catd_main_modem_refresh_response(ste_msg_t* ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    catd_main_dispatch_using_clienttag(ste_msg);
}

/*************************************************************************
 * @brief:    Warm reset CATD after get UICC reset refresh
 * @params:
 *
 * @return:   void
 *
 * Notes:     Only CATD state, TPDL will be affected, the clients, socket, etc. will not
 *************************************************************************/
void catd_main_warm_reset()
{
    catd_tpdl_reset();
    g_tpdl.initiator = CATD_TPDL_INITIATOR_REFRESH;
    catd_reset_pc_setup_event_list();
    catd_reset_pc_send_ussd();
    catd_reset_pc_setup_call();
    catd_timer_clean_up_data();
    catd_change_cat_status(CATD_STATE_INIT);
}

/*************************************************************************
 * @brief:    Request modem to do "CAT ENABLE".
 * @params:
 *
 * @return:   result of the operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_cat_enable()
{
    int                     rv;
    ste_sim_ctrl_block_t   *ctrl_p;
    catd_log_f(SIM_LOGGING_D, "Enter: catd_cat_enable");

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG_INTERN, CATD_FD, CATD_MSG_CAT_ENABLE, NULL, NULL);
    if (!ctrl_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        return -1;
    }

    rv = catd_enable_cat_service((uintptr_t)ctrl_p);
    if (rv != 0) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to request modem cat enable!");
        free(ctrl_p);
        return -1;
    }
    catd_log_f(SIM_LOGGING_D, "Exit: catd_cat_enable");

    return 0;
}

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
/*************************************************************************
 * @brief:    Request modem to do "CAT DISABLE".
 * @params:
 *
 * @return:   result of the operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_cat_disable()
{
    int                     rv;
    ste_sim_ctrl_block_t   *ctrl_p;
    catd_log_f(SIM_LOGGING_D, "Enter: catd_cat_disable");

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, CATD_FD, CATD_MSG_CAT_DISABLE, NULL, NULL);
    if (!ctrl_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        return -1;
    }

    rv = catd_disable_cat_service((uintptr_t)ctrl_p);
    if (rv != 0) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to request modem cat enable!");
        free(ctrl_p);
        return -1;
    }

    cat_disable_outstanding = 1;
    CATD_WAITING_FOR_RESPONSE;
    catd_log_f(SIM_LOGGING_D, "Exit: catd_cat_disable");
    return 0;
}



/*************************************************************************
 * @brief:    Handle CAT disableresponse from the modem in case it's disabled
 * to avoid sending Envelope before sending TR for outstanding PC.
 * @params:
 *             ste_msg_t*:    received modem resp
 * @return:   int
 *
 * Notes:     This request will only send a corresponding message to message queue
 *************************************************************************/

static void catd_disable_resp_handler()
{

    ste_modem_t            *modem_p;
    int                     rv;
    ste_sim_ctrl_block_t   *ctrl_p;
    uint8_t                *command_data_p = NULL;
    catd_log_f(SIM_LOGGING_V, "Enter: catd_disable_resp_handler");

    if(env_cahed_apdu.apdu == NULL)
    {
        catd_log_f(SIM_LOGGING_E, "catd_disable_resp_handler: env_cahed_apdu.apdu  == NULL");
        return;
    }
    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        catd_send_status_response(env_cahed_apdu.fd, env_cahed_apdu.client_tag, STE_CAT_RSP_EC, -1);
        return;
    }

    if(pc_outstanding== 1 || cat_disable_outstanding == 1)/*TR pending for outstanding PC command*/
    {
        catd_log_f(SIM_LOGGING_D, "catd_disable_resp_handler: TR resp pending from the modem");
        CATD_WAITING_FOR_RESPONSE;
        return;
    }
    catd_log_f(SIM_LOGGING_D, "catd_disable_resp_handler: Sending cached Envelope");

    command_data_p = (uint8_t*)malloc(sizeof(uint8_t));
    if ( command_data_p == NULL ) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create command data!");
        catd_send_status_response(env_cahed_apdu.fd, env_cahed_apdu.client_tag, STE_CAT_RSP_EC, -1);
        return;
    }
    *command_data_p = SET_UP_EVENT_LIST_DATA_AVAILABLE;

    ctrl_p = ste_sim_create_ctrl_block(env_cahed_apdu.client_tag, env_cahed_apdu.fd, env_cahed_apdu.type, catd_cat_response_for_ec_handler, command_data_p);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        free(command_data_p);
        catd_send_status_response(env_cahed_apdu.fd, env_cahed_apdu.client_tag, STE_CAT_RSP_EC, -1);
        return;
    }

    rv = ste_catd_modem_ec(modem_p, (uintptr_t)ctrl_p,env_cahed_apdu.apdu);
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : envelope command failed");
        catd_send_status_response(env_cahed_apdu.fd, env_cahed_apdu.client_tag, STE_CAT_RSP_EC, -1);
        free(command_data_p);
        free(ctrl_p);
        return;
    }

    gcatd.ongoing_cmd = STE_CATD_CMD_MASK_EC;
    //wait for the response
    CATD_WAITING_FOR_RESPONSE;
}

/*************************************************************************
 * @brief:    Handle the POLLING_OFF pro-active command internally
 * @params:
 *            msg: The original message with APDU data, client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_cat_polling_req()
{
    ste_modem_t* modem_p  = catd_get_modem();
    catd_log_f(SIM_LOGGING_V, "Enter: catd_cat_polling_req");
    if (modem_p) {
            /*
             * The control block is used to "divert" the modem response to the handler function
             * as well as to hold data we need when handling the response from the modem
             */
         ste_sim_ctrl_block_t* ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, CATD_FD, CATD_MSG_NONE,
                                                                      NULL, NULL);
             if (ctrl_p) {
                 if (ste_catd_modem_poll_req(modem_p, (uintptr_t)ctrl_p) == 0) {
                     /*
                      *  Set the pointer to the raw APDU in the message to NULL in order to prevent the
                      *  framework from deleting the raw APDU when it deletes the message. We will delete
                      *  the raw APDU ourselves later on.
                      */
                      catd_log_f(SIM_LOGGING_E, "Exit: catd_cat_polling_req");
                      return; // Request sent ok, now we wait for the response!
                 }
                 free(ctrl_p);
             }
        }
}
#endif


/*************************************************************************
 * @brief:    Handle CAT refresh indications from the modem
 * @params:
 *            client_tag:      Client tag, can be null.
 *            refresh_type:    Type of the refresh.
 *            ind_type:        Type of the indications
 *            app_id:          App id the refresh is on
 *
 * @return:   void
 *
 * Notes:     This request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_modem_refresh_ind(uintptr_t client_tag,
                                ste_apdu_refresh_type_t refresh_type,
                                ste_cat_refresh_ind_type_t ind_type,
                                uint8_t app_id)
{
    catd_msg_modem_refresh_ind_t *msg;

    msg = catd_msg_modem_refresh_ind_create(client_tag, refresh_type, ind_type, app_id);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*) msg);
    }
}

/*************************************************************************
 * @brief:    Main handler function for CAT refresh indications.
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_modem_refresh_ind(ste_msg_t* ste_msg)
{
    ste_sim_ctrl_block_t             * ctrl_p = NULL; // Use _p naming convention to allow searching for ctrl_p.
    catd_msg_modem_refresh_ind_t     * msg;

    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    msg = (catd_msg_modem_refresh_ind_t *) ste_msg;
    catd_log_f(SIM_LOGGING_D, "catd : Refresh Ind: refresh type = 0x%x", msg->refresh_type);
    catd_log_f(SIM_LOGGING_D, "catd : Refresh Ind: ind type = 0x%X", msg->ind_type);
    catd_log_f(SIM_LOGGING_D, "catd : Refresh Ind: app id = 0x%X", msg->app_id);

    switch (msg->refresh_type)
    {
        case STE_APDU_REFRESH_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT_AND_FILE_CHANGE:
        case STE_APDU_REFRESH_3G_SESSION_RESET:
        case STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT:
        case STE_APDU_REFRESH_3G_APP_RESET:
        {
            //for these refresh types, not much to do right now
        }
        break;
        case STE_APDU_REFRESH_UICC_RESET:
        {
            if (msg->ind_type == STE_CAT_REFRESH_IND_START)
            {
                int socket;

                //Inform UICCD about the refresh
                socket = catd_get_uiccd_request_socket();
                if (socket >= 0)
                {
                    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                                       CATD_FD,
                                                       CATD_MSG_PC_REFRESH,
                                                       catd_uiccd_refresh_reset_response_handler,
                                                       NULL);
                    if (ctrl_p)
                    {
                        uiccd_sig_reset(socket, (uintptr_t)ctrl_p);
                    }
                }

                //REFRESH started from modem
                catd_change_cat_status(CATD_STATE_REFRESH);
                catd_send_cat_status_ind();
                //do the reset
                catd_main_warm_reset();
            }
            //do the reset of CATD and UICCD internally
            if (msg->ind_type == STE_CAT_REFRESH_IND_NOW)
            {
            }
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_D, "catd : invalid refresh type");
        }
        break;
    }
}

/*
 * See catd.h
 */
void catd_sig_sms_cb_perform_connect(uintptr_t client_tag)
{
    catd_msg_sms_cb_perform_connect_t* msg = catd_msg_sms_cp_perform_connect_create(client_tag);

    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*)msg);
    }
}

static void catd_connect_to_sms_cb(); // Forward declaration, implemented later in this file.

/*************************************************************************
 * @brief:    Main handler function for CATd perform SMS/CB connect
 * @params:
 *            ste_msg:  The message for the particular request.
 *
 * @return:   void
 *
 * Notes:     Uses the call-back function in the control block to invoke
 *            to the real handler.
 *************************************************************************/
void catd_main_sms_cb_perform_connect(ste_msg_t* ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);

    catd_connect_to_sms_cb(); // Safe to connect several times, it will only connect once....
    catd_main_dispatch_using_clienttag(ste_msg);
}

/**
 * @brief INTERNAL USE ONLY: Send a SMS/CB response message to the CATd thread.
 *
 * This function is in CATd to send a message to itself. The client tag should
 * point to a ste_sim_ctrl_block_t with a valid transaction_handler_func call-back set.
 *
 * @param client_tag  Pointer to ste_sim_ctrl_block_t.
 */
void catd_sig_sms_cb_response(void* signal, uint32_t primitive, uintptr_t client_tag)
{
    catd_msg_sms_cb_signal_t* msg = catd_msg_sms_cb_response_create(signal, primitive, client_tag);

    catd_log_f(SIM_LOGGING_D, "catd : SMS/CB response signaled: primitive = 0x%X, tag = 0x%08X", primitive, client_tag);

    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*)msg);
    }
}

/*************************************************************************
 * @brief:    Main handler function for SMS/CB responses.
 * @params:
 *            ste_msg:  The message for the particular request.
 *
 * @return:   void
 *
 * Notes:     Uses the call-back function in the control block to invoke
 *            to the real handler.
 *************************************************************************/
void catd_main_sms_cb_response(ste_msg_t* ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    catd_main_dispatch_using_clienttag(ste_msg);
}

/**
 * @brief INTERNAL USE ONLY: Send a SMS/CB event message to the CATd thread.
 *
 * This function is in CATd to send a message to itself. The client tag should
 * point to a ste_sim_ctrl_block_t with a valid transaction_handler_func call-back set.
 *
 * @param client_tag  Pointer to ste_sim_ctrl_block_t.
 */
void catd_sig_sms_cb_event(void* signal, uint32_t primitive, uintptr_t client_tag)
{
    catd_msg_sms_cb_signal_t* msg = catd_msg_sms_cb_event_create(signal, primitive, client_tag);

    catd_log_f(SIM_LOGGING_D, "catd : SMS/CB event signaled: primitive = 0x%X, tag = 0x%08X", primitive, client_tag);

    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t*)msg);
    }
}

/*************************************************************************
 * @brief:    Main handler function for SMS/CB responses.
 * @params:
 *            ste_msg:  The message for the particular request.
 *
 * @return:   void
 *
 * Notes:     Uses the call-back function in the control block to invoke
 *            to the real handler.
 *************************************************************************/
void catd_main_sms_cb_event(ste_msg_t* ste_msg)
{
    catd_msg_sms_cb_signal_t* msg = (catd_msg_sms_cb_signal_t*) ste_msg;
    catd_log_f(SIM_LOGGING_E, "catd : SMS/CB event received: primitive = 0x%X, tag = 0x%08X", msg->primitive, msg->client_tag);
}

/**
 * @brief: INTERNAL USE ONLY: Signals that a UICCd response has arrived.
 *
 * @params
 *          buf         Pointer to data buffer that is associated with the particular response.
 *                      A null pointer is valid and means that there is no data associated with
 *                      the particlar response.
 *
 *          size        The size of the data buffer should be zero if buf is NULL.
 *
 *          command     The response id.
 *
 *          client_tag  The client_tag specified when the original UICCd request was issued.
 *
 * Note: Bad arguments or internal failure is handled by silently "dropping the message",
 *       e.g. no message is created or sent to the CATd main thread. The other real option
 *       of handling such a scenario is to add an assertion if this happens. This is in
 *       accordance with the framework design that is in place.
 */
void catd_sig_uiccd_response(const char* buf, ssize_t size, uint16_t command, uintptr_t client_tag)
{
    catd_msg_uiccd_response_t* msg;
    char* response = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : UICCd response signaled: buf=0x%X, size=%d, cmd=0x%X, tag = 0x%08X", buf, size, command, client_tag);

    /*
     * Check if there is any associated data that we have to copy into a response buffer.
     */
    if (size > 0 && buf)
    {
        response = malloc(size);
        if (response) {
            memcpy(response, buf, size);
        }
    }

    /*
     * If we have a response buffer (i.e. response != NULL), or if a response buffer is not needed
     * (i.e. size <= 0), then we create & send a message to the main thread of CATd
     */
    if (response || (size <= 0)) {
        msg = catd_msg_uiccd_response_create(response, size, command, client_tag);
        if (msg) {
            ste_msgq_add(catd_mq_rsp, (ste_msg_t*)msg);
        }
    }
}

void catd_main_uiccd_response(ste_msg_t* ste_msg)
{
    catd_log_f(SIM_LOGGING_D, "catd : %s, msg = 0x%08X", __PRETTY_FUNCTION__, ste_msg);
    catd_main_dispatch_using_clienttag(ste_msg);
}

/*************************************************************************
 * @brief:    SET TERMINAL PROFILE request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the terminal profile binary.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_set_terminal_profile(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_set_terminal_profile_t          *msg;

    msg = catd_msg_set_terminal_profile_create(fd, buf_p, len, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for SET TERMINAL PROFILE request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     currently we will save the profile into some place and use it
 *            next time when the phone is restarted
 *************************************************************************/
void catd_main_set_terminal_profile(ste_msg_t * ste_msg)
{
    catd_msg_set_terminal_profile_t  *msg;
    char                             *profile_p;
    size_t                            profile_len;

    catd_log_f(SIM_LOGGING_D, "catd : SET TERMINAL PROFILE message received");

    msg = (catd_msg_set_terminal_profile_t *) ste_msg;
    //decode the data and length
    profile_p = ste_sim_decode_length_data(&profile_len, (char*)(msg->buf_p), msg->len);
    if (!profile_p) {
        catd_log_f(SIM_LOGGING_E, "catd : terminal profile from client is NULL");
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_SET_TERMINAL_PROFILE, -1);
        return;
    }

    catd_main_store_terminal_profile((uint8_t*)profile_p, profile_len);
    //FIXME: Set terminal profile down to modem if this is supported (and reset sim??)

    //send a success response to client
    catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_SET_TERMINAL_PROFILE, 0);
    free(profile_p);
}


/*************************************************************************
 * @brief:    GET TERMINAL PROFILE request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_get_terminal_profile(int fd, uintptr_t client_tag)
{
    catd_msg_get_terminal_profile_t          *msg;

    msg = catd_msg_get_terminal_profile_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for GET TERMINAL PROFILE request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_get_terminal_profile(ste_msg_t * ste_msg)
{
    catd_msg_get_terminal_profile_t  *msg;
    char                             *profile_p = NULL;
    size_t                            profile_len = 0;
    char                             *buf_p = NULL;
    size_t                            buf_len = 0;

    catd_log_f(SIM_LOGGING_D, "catd : GET TERMINAL PROFILE message received");

    msg = (catd_msg_get_terminal_profile_t *) ste_msg;

    profile_p = (char *)catd_main_load_terminal_profile();
    profile_len = sizeof(catd_terminal_profile_download_t);

    buf_p = ste_sim_encode_length_data(&buf_len, profile_p, profile_len);

    sim_send_generic(msg->fd, STE_CAT_RSP_GET_TERMINAL_PROFILE, buf_p, buf_len, msg->client_tag);
    free(buf_p);
    free(profile_p);
}


/*************************************************************************
 * @brief:    GET CAT STATUS request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_get_cat_status(int fd, uintptr_t client_tag)
{
    catd_msg_get_cat_status_t          *msg;

    msg = catd_msg_get_cat_status_create(fd, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for GET CAT STATUS request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_get_cat_status(ste_msg_t * ste_msg)
{
    catd_msg_get_cat_status_t    *msg;

    catd_log_f(SIM_LOGGING_D, "catd : GET CAT STATUS message received");

    msg = (catd_msg_get_cat_status_t *) ste_msg;
    catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_GET_CAT_STATUS, gcatd.state);
}


/*************************************************************************
 * @brief:    CALL CONTROL request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the call control info.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_call_control(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_call_control_t          *msg;

    msg = catd_msg_call_control_create(fd, buf_p, len, client_tag);

    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    send the response data back to client through socket for CALL CONTROL
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            status:        status of the operation.
 *            data_p:        data to be sent over socket.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_send_call_control_response(int fd, uintptr_t client_tag, int status, char * data_p, uint16_t length)
{
    char                   * p = NULL;
    char                   * buf_p = NULL;
    size_t                   buf_len;
    unsigned                 status_len;
    unsigned                 data_len;

    status_len = sizeof(status);
    data_len = length;

    buf_len = sizeof(char) * (status_len + data_len);
    buf_len += sizeof(status_len) + sizeof(data_len);

    buf_p = malloc(buf_len);
    if (!buf_p) {
        catd_log_f(SIM_LOGGING_E, "catd : memory allocation failed");
        sim_send_generic(fd,
                         STE_CAT_RSP_CALL_CONTROL,
                         NULL,
                         0,
                         client_tag);
        return;
    }

    p = buf_p;

    p = sim_enc(p, &status_len,  sizeof(status_len));
    p = sim_enc(p, &status, sizeof(char)*(status_len));

    p = sim_enc(p, &data_len, sizeof(data_len));
    if (data_p) {
        p = sim_enc(p, data_p, sizeof(char)*(data_len));
    }

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_call_control_response, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_CAT_RSP_CALL_CONTROL,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);
}

/*************************************************************************
 * @brief:    CAT reponse handler function for EC CALL CONTROL
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   the state of the session/transaction
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_main_call_control_response_handler(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t  *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    ste_apdu_t                     *apdu = 0;
    ste_cat_status_words_t          status_words;
    uint16_t                        client_rsp;
    ste_sim_ctrl_block_t           *ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    char                           *buf_p = NULL;
    size_t                          buf_len = 0;
    ste_sat_apdu_error_t            rv;
    uint8_t                         result;

    catd_log_f(SIM_LOGGING_E, "catd : catd_call_control_response_handler ENTER");
    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : Control Block is NULL in handler function for CAT response");
        return 0;
    }
    if (ctrl_p->transaction_id != CATD_MSG_CALL_CONTROL) {
        catd_log_f(SIM_LOGGING_E, "catd : error transaction id for CAT response");
        return 0;
    }

    catd_log_f(SIM_LOGGING_E, "catd : status: %d", msg->status);
    if (msg->status == 0) {
        client_rsp = STE_CAT_RESPONSE_OK;
    }
    else {
        client_rsp = STE_CAT_RESPONSE_UNKNOWN;
    }
    apdu = msg->apdu;

    result = ste_apdu_get_catd_status_words(msg->apdu,
                                            (ste_apdu_status_words_t *)&status_words);

    if (result) {
        catd_log_f(1, "catd : catd_call_control_response_handler. No status words found in the apdu!");
        client_rsp = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
    } else {
        //check the status words, and based on that, create a client response
        catd_convert_modem_status_words(status_words, &client_rsp);
    }
    catd_log_b(SIM_LOGGING_D, "catd_call_control_response_handler CC RESPONSE APDU = ", ste_apdu_get_raw(msg->apdu), ste_apdu_get_raw_length(msg->apdu));
    catd_log_f(1, "catd : catd_call_control_response_handler msg->status=0x%x sw1=0x%x sw2=0x%x", msg->status,
                                                                                                  status_words.sw1,
                                                                                                  status_words.sw2);
    if (client_rsp == STE_CAT_RESPONSE_OK) {
        //parse the APDU if there is
        if (apdu) {
            ste_cat_call_control_response_t   cc_rsp;
            ste_cat_call_control_result_t     cc_result;

            rv = ste_apdu_parse_call_control_response(&cc_rsp, apdu, &cc_result);
            if (rv == STE_SAT_APDU_ERROR_NONE) {
                buf_p = ste_sim_encode_cc_response(cc_result, &cc_rsp, &buf_len);
            }
            else {
                //check the result only
                buf_p = ste_sim_encode_cc_response(cc_result, NULL, &buf_len);
            }
            sim_free_call_control_response_data(&cc_rsp);
        }
        else {
            //if there is no apdu, but the EC status is ok, then it is allowed
            buf_p = ste_sim_encode_cc_response(STE_CAT_CC_ALLOWED_NO_MODIFICATION, NULL, &buf_len);
        }
    }
    catd_send_call_control_response(ctrl_p->fd, ctrl_p->client_tag, client_rsp, buf_p, buf_len);
    free(buf_p);

    return 0;
}

static int catd_call_control_cell_info_response_handler(ste_msg_t *ste_msg)
{
    ste_apdu_t                       *apdu = 0;
    ste_cat_call_control_t           *cc_p = NULL;
    ste_sim_ctrl_block_t             *ctrl_p;
    ste_apdu_location_info_t         *local_info_p;
    ste_modem_t                      *modem_p;
    catd_msg_cn_t                    *msg_p;
    cn_message_t                     *cn_msg_p;
    int                               rv;
    catd_msg_call_control_t          *cc_msg_p; //Original msg
    uint8_t                          *data_p;

    catd_log_f(SIM_LOGGING_D, "ENTER %s", __func__);

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*)ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*)msg_p->cn_msg;

    assert(msg_p->client_tag);
    ctrl_p = (ste_sim_ctrl_block_t*)msg_p->client_tag;
    cc_msg_p = (catd_msg_call_control_t*)(ctrl_p->transaction_data_p);
    assert(cc_msg_p);
    data_p = cc_msg_p->buf_p;

    if (CATD_STATE_READY != gcatd.state) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        goto Error;
    }

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        goto Error;
    }

    cc_p = malloc(sizeof(*cc_p));
    if (!cc_p) {
        catd_log_f(SIM_LOGGING_E, "catd : memory allocation for call control failed!");
        goto Error;
    }
    memset(cc_p, 0, sizeof(*cc_p));
    // fetch the CC data from Client
    if (sim_decode_call_control_data(cc_p, data_p, cc_msg_p->len) != 0) {
        catd_log_f(SIM_LOGGING_E, "catd : decode call control data failed!");
        free(cc_p);
        goto Error;
    }

    if (CN_RESPONSE_CELL_INFO == cn_msg_p->type && cn_msg_p->error_code == CN_SUCCESS) {

        cn_cell_info_t *cell_info_p = (cn_cell_info_t *)cn_msg_p->payload;

        local_info_p = create_local_info_from_cell_info(cell_info_p);
        if (local_info_p == NULL) {
            sim_free_call_control_data(cc_p);
            free(cc_p);
            goto Error;
        }
    } else if ( CN_RESPONSE_CELL_INFO == cn_msg_p->type ) {
        /* This error type is due to a temporary condition in modem where modem is not ready
         * to send cell info. Possibly the PLMN selection procedures are still going on. In this
         * case, the terminal response should indicate a temporary failure so that the request
         * will get resend from SIM */
         catd_send_call_control_response(cc_msg_p->fd, cc_msg_p->client_tag, STE_CAT_RESPONSE_TEMP_PROBLEM_IND, NULL, 0);
         sim_free_call_control_data(cc_p);
         free(cc_p);
         ste_msg_delete((ste_msg_t *)cc_msg_p);
         return -1;
    } else {
         sim_free_call_control_data(cc_p);
         free(cc_p);
         goto Error;
    }

    apdu = ste_apdu_call_control(cc_p, local_info_p);
    if (!apdu) {
        catd_log_f(SIM_LOGGING_E, "catd : create APDU for call control failed!");
        sim_free_call_control_data(cc_p);
        free(cc_p);
        free(local_info_p);
        goto Error;
    }
    sim_free_call_control_data(cc_p);
    free(local_info_p);
    free(cc_p);

    ctrl_p = ste_sim_create_ctrl_block(cc_msg_p->client_tag,
                                       cc_msg_p->fd,
                                       cc_msg_p->type,
                                       catd_main_call_control_response_handler,
                                       NULL);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        ste_apdu_delete(apdu);
        goto Error;
    }

    catd_log_b(SIM_LOGGING_D, "catd_call_control_cell_info_response_handler CC REQUEST APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));

    rv = ste_catd_modem_ec(modem_p, (uintptr_t)ctrl_p, apdu);
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : envelope command failed");
        ste_apdu_delete(apdu);
        free(ctrl_p);
        goto Error;
    }

    ste_apdu_delete(apdu);
    ste_msg_delete((ste_msg_t *)cc_msg_p);
    gcatd.ongoing_cmd = STE_CATD_CMD_MASK_EC;
    //wait for the response
    CATD_WAITING_FOR_RESPONSE;
    return 0;

    Error:
        catd_send_call_control_response(cc_msg_p->fd, cc_msg_p->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)cc_msg_p);
        return -1;
}

/*************************************************************************
 * @brief:    main handler function for call control request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_call_control(ste_msg_t * ste_msg)
{
    catd_msg_call_control_t          *msg;
    ste_sim_ctrl_block_t             *ctrl_p;
    cn_context_t                     *cn_context_p = NULL;
    catd_msg_call_control_t          *cc_msg_p;

    catd_log_f(SIM_LOGGING_D, "catd : CALL CONTROL message received");

    msg = (catd_msg_call_control_t *) ste_msg;
    //since this ste_msg will be deleted after the msg is handled, we cannot put it directly into ctrl block
    //so we make a copy of it
    cc_msg_p = catd_msg_call_control_create(msg->fd, msg->buf_p, msg->len, msg->client_tag);

    cn_context_p = cn_get_client();
    if(cn_context_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to get cn client context", __func__);
        catd_send_call_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)cc_msg_p);
        return;
    }

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                       CATD_FD,
                                       CATD_MSG_NONE,
                                       catd_call_control_cell_info_response_handler,
                                       cc_msg_p);

    if (ctrl_p) {
        if (CN_SUCCESS != cn_request_cell_info(cn_context_p, (uintptr_t)ctrl_p)) {
            catd_log_f(SIM_LOGGING_E, "%s : Failed to request cn", __func__);
            catd_send_call_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
            ste_msg_delete((ste_msg_t *)cc_msg_p);
            return;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to create ctrl block", __func__);
        catd_send_call_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)cc_msg_p);
        return;
    }
}

/*************************************************************************
 * @brief:    SMS CONTROL request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the sms control info.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_sms_control(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_sms_control_t          *msg;

    msg = catd_msg_sms_control_create(fd, buf_p, len, client_tag);

    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    send the response data back to client through socket for SMS CONTROL
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            status:        status of the operation.
 *            data_p:        data to be sent over socket.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_send_sms_control_response(int fd, uintptr_t client_tag, int status, char * data_p, uint16_t length)
{
    char                   * p = NULL;
    char                   * buf_p = NULL;
    size_t                   buf_len;
    unsigned                 status_len;
    unsigned                 data_len;

    status_len = sizeof(status);
    data_len = length;

    buf_len = sizeof(char) * (status_len + data_len);
    buf_len += sizeof(status_len) + sizeof(data_len);

    buf_p = malloc(buf_len);
    if (!buf_p) {
        catd_log_f(SIM_LOGGING_E, "catd : memory allocation failed");
        sim_send_generic(fd,
                         STE_CAT_RSP_SMS_CONTROL,
                         NULL,
                         0,
                         client_tag);
        return;
    }

    p = buf_p;

    p = sim_enc(p, &status_len,  sizeof(status_len));
    p = sim_enc(p, &status, sizeof(char)*(status_len));

    p = sim_enc(p, &data_len, sizeof(data_len));
    if (data_p) {
        p = sim_enc(p, data_p, sizeof(char)*(data_len));
    }

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_send_sms_control_response, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_CAT_RSP_SMS_CONTROL,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);
}

/*************************************************************************
 * @brief:    CAT response handler function for EC SMS CONTROL
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   the state of the session/transaction
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_main_sms_control_response_handler(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t  *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    ste_apdu_t                     *apdu = 0;
    ste_cat_status_words_t          status_words;
    uint16_t                        client_rsp;
    ste_sim_ctrl_block_t           *ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    char                           *buf_p = NULL;
    size_t                          buf_len = 0;
    ste_sat_apdu_error_t            rv;
    uint8_t                         result;

    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : Control Block is NULL in handler function for CAT response");
        return 0;
    }
    if (ctrl_p->transaction_id != CATD_MSG_SMS_CONTROL) {
        catd_log_f(SIM_LOGGING_E, "catd : error transaction id for CAT response");
        return 0;
    }

    apdu = msg->apdu;

    result = ste_apdu_get_catd_status_words(msg->apdu,
                                            (ste_apdu_status_words_t *)&status_words);

    if (result) {
        catd_log_f(1, "catd : catd_sms_control_response_handler. No status words found in the apdu!");
        client_rsp = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
    } else {
        //check the status words, and based on that, create a client response
        catd_convert_modem_status_words(status_words, &client_rsp);
    }

    catd_log_b(SIM_LOGGING_D, "catd_main_sms_control SC RESPONSE APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));
    catd_log_f(1, "catd : catd_sms_control_response_handler msg->status=0x%x sw1=0x%x sw2=0x%x", msg->status,
                                                                                                 status_words.sw1,
                                                                                                 status_words.sw2);
    if (client_rsp == STE_CAT_RESPONSE_OK) {
        //parse the APDU if there is
        if (apdu) {
            ste_cat_sms_control_response_t   sc_rsp;
            ste_cat_sms_control_result_t     sc_result;

            rv = ste_apdu_parse_sms_control_response(&sc_rsp, apdu, &sc_result);
            if (rv == STE_SAT_APDU_ERROR_NONE) {
                buf_p = ste_sim_encode_sc_response(sc_result, &sc_rsp, &buf_len);
            }
            else {
                //check the result only
                buf_p = ste_sim_encode_sc_response(sc_result, NULL, &buf_len);
            }
            sim_free_sms_control_response_data(&sc_rsp);
        }
        else {
            //if there is no apdu, but the EC status is ok, then it is allowed
            buf_p = ste_sim_encode_sc_response(STE_CAT_CC_ALLOWED_NO_MODIFICATION, NULL, &buf_len);
        }
    }
    catd_send_sms_control_response(ctrl_p->fd, ctrl_p->client_tag, client_rsp, buf_p, buf_len);
    free(buf_p);

    return 0;
}

static int catd_sms_control_cell_info_response_handler(ste_msg_t *ste_msg)
{
    ste_apdu_t                       *apdu = 0;
    ste_cat_sms_control_t            *sc_p = NULL;
    ste_sim_ctrl_block_t             *ctrl_p;
    ste_apdu_location_info_t         *local_info_p;
    ste_modem_t                      *modem_p;
    catd_msg_cn_t                    *msg_p;
    cn_message_t                     *cn_msg_p;
    int                               rv;
    catd_msg_sms_control_t           *sc_msg_p; //Original msg
    uint8_t                          *data_p;

    catd_log_f(SIM_LOGGING_D, "ENTER %s", __func__);

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*)ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*)msg_p->cn_msg;

    assert(msg_p->client_tag);
    ctrl_p = (ste_sim_ctrl_block_t*)msg_p->client_tag;
    sc_msg_p = (catd_msg_sms_control_t*)(ctrl_p->transaction_data_p);
    assert(sc_msg_p);
    data_p = sc_msg_p->buf_p;

    if (CATD_STATE_READY != gcatd.state) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        goto Error;
    }

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        goto Error;
    }

    sc_p = malloc(sizeof(*sc_p));
    if (!sc_p) {
        catd_log_f(SIM_LOGGING_E, "catd : memory allocation for call control failed!");
        goto Error;
    }
    memset(sc_p, 0, sizeof(*sc_p));
    // fetch the SC data from Client
    if (sim_decode_sms_control_data(sc_p, data_p, sc_msg_p->len) != 0) {
        catd_log_f(SIM_LOGGING_E, "catd : decode call control data failed!");
        free(sc_p);
        goto Error;
    }

    if (CN_RESPONSE_CELL_INFO == cn_msg_p->type && cn_msg_p->error_code == CN_SUCCESS) {

        cn_cell_info_t *cell_info_p = (cn_cell_info_t *)cn_msg_p->payload;

        local_info_p = create_local_info_from_cell_info(cell_info_p);
        if (local_info_p == NULL) {
            sim_free_sms_control_data(sc_p);
            free(sc_p);
            goto Error;
        }

    } else {
        sim_free_sms_control_data(sc_p);
        free(sc_p);
        goto Error;
    }

    apdu = ste_apdu_sms_control(sc_p, local_info_p);
    if (!apdu) {
        catd_log_f(SIM_LOGGING_E, "catd : create APDU for sms control failed!");
        sim_free_sms_control_data(sc_p);
        free(sc_p);
        free(local_info_p);
        goto Error;
    }
    sim_free_sms_control_data(sc_p);
    free(local_info_p);
    free(sc_p);

    ctrl_p = ste_sim_create_ctrl_block(sc_msg_p->client_tag,
                                       sc_msg_p->fd,
                                       sc_msg_p->type,
                                       catd_main_sms_control_response_handler,
                                       NULL);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        ste_apdu_delete(apdu);
        goto Error;
    }

    catd_log_b(SIM_LOGGING_D, "catd_sms_control_cell_info_response_handler SC REQUEST APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));

    rv = ste_catd_modem_ec(modem_p, (uintptr_t)ctrl_p, apdu);
    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : envelope command failed");
        ste_apdu_delete(apdu);
        free(ctrl_p);
        goto Error;
    }

    ste_apdu_delete(apdu);
    ste_msg_delete((ste_msg_t *)sc_msg_p);
    gcatd.ongoing_cmd = STE_CATD_CMD_MASK_EC;
    //wait for the response
    CATD_WAITING_FOR_RESPONSE;
    return 0;

    Error:
        catd_send_sms_control_response(sc_msg_p->fd, sc_msg_p->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)sc_msg_p);
        return -1;
}

/*************************************************************************
 * @brief:    main handler function for sms control request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_sms_control(ste_msg_t * ste_msg)
{
    catd_msg_sms_control_t           *msg;
    catd_msg_sms_control_t           *sc_msg_p;
    ste_sim_ctrl_block_t             *ctrl_p;
    cn_context_t                     *cn_context_p = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : SMS CONTROL message received");

    msg = (catd_msg_sms_control_t *) ste_msg;
    //since this ste_msg will be deleted after the msg is handled, we cannot put it directly into ctrl block
    //so we make a copy of it
    sc_msg_p = catd_msg_sms_control_create(msg->fd, msg->buf_p, msg->len, msg->client_tag);

    cn_context_p = cn_get_client();
    if(cn_context_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to get cn client context", __func__);
        catd_send_sms_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)sc_msg_p);
        return;
    }

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                       CATD_FD,
                                       CATD_MSG_NONE,
                                       catd_sms_control_cell_info_response_handler,
                                       sc_msg_p);

    if (ctrl_p) {
        if (CN_SUCCESS != cn_request_cell_info(cn_context_p, (uintptr_t)ctrl_p)) {
            catd_log_f(SIM_LOGGING_E, "%s : Failed to request cn", __func__);
            catd_send_sms_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
            ste_msg_delete((ste_msg_t *)sc_msg_p);
            return;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to create ctrl block", __func__);
        catd_send_sms_control_response(msg->fd, msg->client_tag, STE_CAT_RESPONSE_UNKNOWN, NULL, 0);
        ste_msg_delete((ste_msg_t *)sc_msg_p);
        return;
    }
}

/*************************************************************************
 * @brief:    signals the timer of this ongoing session is expired
 * @params:
 *            client_tag:    the client_tag for the request.
 *            session_id:    the expiration for which session, Ex. the PC, EC, etc.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_cat_session_expired(uintptr_t client_tag, uint16_t session_id)
{
    catd_msg_cat_session_expired_t *msg;

    msg = catd_msg_cat_session_expired_create(client_tag, session_id);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}


/*************************************************************************
 * @brief:    main handler function for session timer expired event
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:     This is for future usage, if we are going to set a timer for each session/transaction
 *************************************************************************/
static void catd_main_cat_session_expired(ste_msg_t* ste_msg)
{
    catd_msg_cat_session_expired_t *msg = (catd_msg_cat_session_expired_t*) ste_msg;

    catd_log_f(SIM_LOGGING_D, "catd : CAT SESSION EXPIRED message received, session_id = %d", msg->session_id);
    //go through the saved ctrl block list, free the corresponding ctrl block to this session

    //will not wait for the response anymore, switch back to req mq
    CATD_PROCEED_WITH_REQUEST;
    gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
}


/*************************************************************************
 * @brief:    send a raw APDU directly to modem from the client
 * @params:
 *            fd:          the socket fd of the request.
 *            apdu:        the apdu to be dealt with.
 *            client_tag:  client tag of the client.
 *
 * @return:   void
 *
 * Notes:     this function just passes the APDU to the modem directly without knowing anything about it.
 *            this is for possible future usage such as SAP.
 *            A handler for the CAT response should also be created if this interface is going to be used
 *************************************************************************/
void catd_sig_raw_apdu(int UNUSED(fd), ste_apdu_t * apdu,uintptr_t UNUSED(client_tag))
{
    ste_modem_t            *modem_p;

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        return;
    }

    //ste_catd_modem_raw_apdu(modem_p, client_tag, fd, apdu);  //modem needs to provide an interface to send this APDU down
    ste_apdu_delete(apdu);
}

/*************************************************************************
 * @brief:    Respond to client's answer call.
 * @params:
 *            status:       the call setup status, failed or succeeded(0 == success).
 *
 * @return:   void.
 *
 * Notes:
 *************************************************************************/
void catd_send_answer_call_response(int status)
{
    if (current_CT != 0 && current_fd != -1) {
        catd_send_status_response(current_fd,
                                  current_CT,
                                  STE_CAT_RSP_ANSWER_CALL,
                                  status);

        //clean up
        current_CT = 0;
        current_fd = -1;
    } else {
        catd_log_f(SIM_LOGGING_E,"catd : current_fd if -1 or current_CT is 0");
    }
}

/*************************************************************************
 * @brief:    Notify client about the setup call pc.
 * @params:
 *            call_data_p:    the call setup related data.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_pc_setup_call_ind(ste_cat_pc_setup_call_ind_t * call_data_p)
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    //go through the client list to find those clients who are interested in pc setup call
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_PC_SETUP_CALL_IND) ==
                STE_CAT_CLIENT_REG_EVENTS_PC_SETUP_CALL_IND)
        {
            size_t                  buf_len = 0;
            char                   *buf_p = NULL;
            char                   *p = NULL;

            buf_len = sizeof(ste_cat_pc_setup_call_ind_t);
            buf_p = malloc(buf_len);
            if (!buf_p)
            {
                continue;
            }

            p = buf_p;
            p = sim_enc(p, call_data_p, buf_len);

            if (buf_len != (size_t)(p - buf_p)) {
                catd_log_f(SIM_LOGGING_E, "catd : catd_send_pc_setup_call_ind, assert failure");
            }

            assert( buf_len == (size_t)(p - buf_p) );

            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                       STE_CAT_IND_PC_SETUP_CALL,
                                       buf_p,
                                      buf_len,
                                      0); //No client tag in events

            if (result == 0) {
                success_count++;
            }
            free( buf_p );
        }
    }

    return success_count;
}

/*************************************************************************
 * @brief:    ANSWER PC SETUP CALL request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the decision of answer or not.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_answer_call(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_answer_call_t          *msg;

    msg = catd_msg_answer_call_create(fd, buf_p, len, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_rsp, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for ANSWER PC SETUP CALL request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_answer_call(ste_msg_t * ste_msg)
{
    catd_msg_answer_call_t      *msg;
    int                          answer;
    uint8_t                     *p;
    uint8_t                     *p_max;

    catd_log_f(SIM_LOGGING_D, "catd : ANSWER PC SETUP CALL message received");

    msg = (catd_msg_answer_call_t *) ste_msg;
    //decode the answer
    p = msg->buf_p;
    p_max = p + msg->len;
    p = (uint8_t*)sim_dec((char*)p, &answer, sizeof(answer), (char*)p_max);
    if ( !p )
    {
        catd_log_f(SIM_LOGGING_E, "catd : data from socket might have some error.");
    }

    //Store current setup_call data until we send the response
    current_fd = msg->fd;
    current_CT = msg->client_tag;

    //continue with the PC call setup
    catd_handle_pc_setup_call_answer(answer);
    //a response is sent back to client after dial is confirmed by the modem
}

/*************************************************************************
 * @brief:    Notify clients about the session end.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_session_end_ind()
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    catd_log_f(1, "catd : catd_send_session_end_ind. Sending session end to registered clients!");

    //go through the client list to find those clients who are interested in the session end indication
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_SESSION_END_IND) ==
                STE_CAT_CLIENT_REG_EVENTS_SESSION_END_IND)
        {
            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                      STE_CAT_IND_SESSION_END,
                                      NULL, // No payload for this indication
                                      0,
                                      0);

            if (result == 0) {
                success_count++;
            }
        }
    }
    return success_count;
}

/*************************************************************************
 * @brief:    Notify client about the refresh pc.
 * @params:
 *            refresh_p:    the refresh related data.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_pc_refresh_ind(ste_cat_pc_refresh_ind_t * refresh_p)
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    //go through the client list to find those clients who are interested in pc refresh
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH) ==
                STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH)
        {
            size_t                  buf_len = 0;
            char                   *buf_p = NULL;
            char                   *p = NULL;

            buf_len += sizeof(refresh_p->simd_tag);
            buf_len += sizeof(refresh_p->type);
            buf_p = malloc(buf_len);
            if (!buf_p)
            {
                continue;
            }

            p = buf_p;
            p = sim_enc(p, &refresh_p->simd_tag, sizeof(refresh_p->simd_tag));
            p = sim_enc(p, &refresh_p->type, sizeof(refresh_p->type));

            if (buf_len != (size_t)(p - buf_p)) {
                catd_log_f(0, "catd : Assert failure");
            }

            assert( buf_len == (size_t)(p - buf_p) );

            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                      STE_CAT_IND_PC_REFRESH,
                                      buf_p,
                                      buf_len,
                                      0);

            if (result == 0) {
                success_count++;
            }
            free( buf_p );
        }
    }

    return success_count;
}

/*************************************************************************
 * @brief:    Notify client about the refresh pc.
 * @params:
 *            refresh_p:    the refresh related data.
 *
 * @return:   Number of client that the indication was sent to.
 *            Success: >= 0;
 *            Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_send_pc_refresh_file_ind(ste_cat_pc_refresh_file_ind_t * refresh_p)
{
    uint8_t i;
    uint8_t success_count = 0;
    int result = -1;

    //go through the client list to find those clients who are interested in pc refresh
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++)
    {
        if (gcatd.client_list[i].registered_fd == -1)
        {
            continue;
        }
        if ((gcatd.client_list[i].registered_events & STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH) ==
                STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH)
        {
            size_t                  buf_len = 0;
            char                   *buf_p = NULL;
            char                   *p = NULL;

            buf_len += sizeof(refresh_p->simd_tag);
            buf_len += sizeof(refresh_p->path.pathlen);
            buf_len += sizeof(refresh_p->path.path);
            buf_p = malloc(buf_len);
            if (!buf_p)
            {
                continue;
            }

            p = buf_p;
            p = sim_enc(p, &refresh_p->simd_tag, sizeof(refresh_p->simd_tag));
            p = sim_enc(p, &refresh_p->path.pathlen, sizeof(refresh_p->path.pathlen));
            p = sim_enc(p,  refresh_p->path.path, sizeof(refresh_p->path.path));

            if (buf_len != (size_t)(p - buf_p)) {
                catd_log_f(0, "catd : Assert failure");
            }

            assert( buf_len == (size_t)(p - buf_p) );

            result = sim_send_generic(gcatd.client_list[i].registered_fd,
                                      STE_CAT_IND_PC_REFRESH_FILE,
                                      buf_p,
                                      buf_len,
                                      0);
            if (result == 0) {
                success_count++;
            }
            free( buf_p );
        }
    }

    return success_count;
}

/*************************************************************************
 * @brief:    Create a timer expiration message and put it in a message
 *            queue to ensure that the message will be handled within
 *            CATD's context.
 * @params:
 *            timer_info:    Timer information.
 *
 * @return:   None.
 *
 *
 * Notes:
 *************************************************************************/
static void catd_sig_timer_expiration_response(const uint16_t timer_info)
{
    catd_msg_timeout_t *msg_p;

    catd_log_f(SIM_LOGGING_D, "catd_sig_timer_expiration_response: timer_info = 0x%04X", timer_info);

    msg_p = catd_msg_timer_expiration_ec_create(timer_info);
    /* Send message to the stack. When the message is being processed, it
     * will be within the CATD context, not the CATD reader.*/
    if (msg_p) {
        ste_msgq_add(catd_mq_req, (ste_msg_t*)msg_p);
    }
}

/*************************************************************************
 * @brief:    Read timer information from CATD reader's buffer/pipe.
 *
 * @params:
 *            buf_p:     Start of buffer containing timeout information.
 *            buf_max_p  End of buffer containing timeout information.
 *
 * @return:   Number of bytes read from the buffer.
 *
 *
 * Notes: At each timer expiration, CATD writes two bytes data to CATD
 *        reader. CATD reader informs CATD when there are data to read from
 *        the buffer. In case more than one timer timed out, there can be
 *        "many" bytes to read from CATD reader's buffer. However, there
 *        must be at least two bytes to read. If not, leave any remaining
 *        byte in the buffer and read it at next time CATD notifies CATD
 *        reader that there are more data to read.
 *
 *************************************************************************/
ssize_t ste_catd_timer_response_parse(char *buf_p, char *buf_max_p, void* UNUSED(ud))
{

    const char *p = buf_p;
    uint16_t    len;
    uint16_t    timer_info;
    ssize_t     consumed = 0;


    catd_log_f(SIM_LOGGING_D, "catd : ste_catd_timer_response_parse");

    if (buf_p && buf_max_p) {           // Normal operation.
        len = (uint16_t) (buf_max_p - buf_p);
        if (len < 1) {         // If less than two bytes, return and let CATD
            return consumed;   // inform us again when new data is there for us to read.
        } else {
            do {
                p = sim_dec(p, &timer_info, sizeof(timer_info), buf_max_p);

                if (!p)
                    break;

                catd_sig_timer_expiration_response(timer_info);

                consumed += sizeof(timer_info);
                len -= sizeof(timer_info); // len is remaining number of bytes in this data packet.
            } while (len > 1);
        }
    } else if (!buf_p && !buf_max_p) {  // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_D, "catd : timer pipe ES is shutting down");
        if (gcatd.timer_pipe.read_socket >= 0) {
            close(gcatd.timer_pipe.read_socket);
            gcatd.timer_pipe.read_socket = -1;
        }

        if (gcatd.timer_pipe.write_socket >= 0) {
            close(gcatd.timer_pipe.write_socket);
            gcatd.timer_pipe.write_socket = -1;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "catd : weird uiccd ES parse scenario");
        consumed = -1;
    }

    catd_log_f(SIM_LOGGING_D, "catd : consumed = %d", consumed);
    return consumed;
}

/*************************************************************************
 * @brief:    Response handler for an envelope command at timer expiration.
 *
 * @params:
 *            ste_msg:  The msg for the particular response, with related data.
 *
 * @return:   The state of the session/transaction
 *            0 : Success: 0
 *           -1 : SIM card busy. Send the EC again at some other time.
 *           -2 : Failed:
 *
 * Notes:
 *************************************************************************/
static int catd_timeout_ec_response_handler(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t  *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    ste_sim_ctrl_block_t           *ctrl_p;
    uint16_t                        timer_info;
    ste_cat_status_words_t          status_words;
    uint16_t                        client_rsp;
    uint8_t                         result;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timeout_ec_response_handler");

    /* The client tag is actually a pointer to the control block we created
     * when we set up the envelope command. */
    ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);

    assert(ctrl_p);

    if (ctrl_p->transaction_id != CATD_MSG_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : erroneous transaction id for timeout EC response. ctrl_p->transaction_id = %d", ctrl_p->transaction_id);
        return -1;
    }

    timer_info = (uint16_t) (uintptr_t)ctrl_p->transaction_data_p;

    catd_log_f(SIM_LOGGING_V, "catd : timer_info = 0x%04X", timer_info);

    result = ste_apdu_get_catd_status_words(msg->apdu,
                                            (ste_apdu_status_words_t *)&status_words);
    if (result) {
        catd_log_f(SIM_LOGGING_E, "catd : catd_timeout_ec_response_handler. No status words found in the apdu!");
        client_rsp = STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR;
    } else {
        //check the status words, and based on that, create a client response
        catd_convert_modem_status_words(status_words, &client_rsp);
    }

    catd_log_f(SIM_LOGGING_V, "catd : catd_timeout_ec_response_handler msg->status=0x%02X sw1=0x%02X sw2=0x%02X", msg->status,
                                                                                                                  status_words.sw1,
                                                                                                                  status_words.sw2);
    CATD_PROCEED_WITH_REQUEST;
    if (client_rsp == STE_CAT_RESPONSE_OK) {
        ste_apdu_t *apdu_p = NULL;
        catd_timer_reset_expiration_data(timer_info);  // Free allocated APDU data.
        /* After we have sent a response, check if there is any Timer
         * Expiration Envelope Command to send.
         * But, only send it if the response was OK*/
        if (catd_timer_check_for_ec_retransmission(&timer_info, &apdu_p) == 0 ) {
            catd_sim_timer_expiration_send_ec(timer_info, apdu_p);
            return 0;
        }

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    catd_log_f(SIM_LOGGING_V, "catd_timeout_ec_response_handler: EC timer resp received from the modem");
    catd_disable_resp_handler();
#endif

        return 0;
    }
    /* The SIM is busy. We must resend the Timer Expiration Envelope Command
     * when we get Terminal Response, with status code 90 00 (STE_CAT_RESPONSE_OK). */
    catd_timer_reset_ec_started_flag(timer_info);
    if (client_rsp == STE_CAT_RESPONSE_CARD_BUSY) {
        catd_log_f(SIM_LOGGING_D, "catd : the SIM card is busy. Send the EC at some other time.");
        return -1;
    }
    else {
        catd_log_f(SIM_LOGGING_E, "catd : failure in sending EC. Send the EC at some other time.");
        return -2;
    }
}

/*************************************************************************
 * @brief:    Send a Timer Expiration Envelope Command to the SIM.
 *
 * @params:
 *            timer_info:  Timer information.
 *
 * @return:   void
 *
 * Notes: If anything fails in this function, there is no one to report it to.
 *        So, just clean up any allocated data and return.
 *************************************************************************/
static void catd_sim_timer_expiration_send_ec(const uint16_t timer_info, ste_apdu_t *apdu_p)
{
    ste_modem_t            *modem_p;
    ste_sim_ctrl_block_t   *ctrl_p;
    int                     rv;


    catd_log_f(SIM_LOGGING_D, "catd : catd_sim_timer_expiration_send_ec, timer_info = 0x%04X", timer_info);
    catd_log_b(SIM_LOGGING_D, "catd : APDU = ", ste_apdu_get_raw(apdu_p), ste_apdu_get_raw_length(apdu_p));

    if (gcatd.state != CATD_STATE_READY) {
        catd_log_f(SIM_LOGGING_E, "catd : CATD is not ready to proceed.");
        goto error;
    }

    modem_p = catd_get_modem();

    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "catd : Missing modem ready!");
        goto error;
    }

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, CATD_FD, CATD_MSG_NONE, catd_timeout_ec_response_handler, (void*)(uintptr_t)timer_info);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
        goto error;
    }

    // Try to send the Timer Expiration Envelope Command.
    rv = ste_catd_modem_ec(modem_p, (uintptr_t)ctrl_p, apdu_p);

    if (rv) {
        catd_log_f(SIM_LOGGING_E, "catd : envelope command failed.");
        free(ctrl_p);
        goto error;
    }

    gcatd.ongoing_cmd = STE_CATD_CMD_MASK_EC;
    // Wait for the response.
    CATD_WAITING_FOR_RESPONSE;

    return;

error:
    catd_timer_reset_expiration_data(timer_info);
}

/*************************************************************************
 * @brief:    Create an Timer Expiration Envelope Command APDU and then
 *            send it to the SIM, unless we have one outstanding EC. In
 *            that case, send it when we get a Terminal Response with
 *            status OK.
 * @params:
 *            ste_msg_p:     Timer information.
 *
 * @return:   Void.
 *
 * Notes:
 *************************************************************************/
void catd_main_sim_timer_expiration_send_ec(ste_msg_t *ste_msg_p)
{
    ste_apdu_t   *apdu_p = NULL;
    catd_msg_timeout_t *msg_p = (catd_msg_timeout_t *) ste_msg_p;


    catd_log_f(SIM_LOGGING_D, "catd_main_sim_timer_expiration_send_ec");

    if (msg_p && msg_p->timer_info) {
        apdu_p = catd_timer_create_expiration_ec(msg_p->timer_info);
        if (apdu_p) {
            uint16_t timer_info;
            // We can only have one outstanding EC.
            if (catd_timer_check_for_ec_retransmission(&timer_info, &apdu_p) == 0 ) {
                catd_log_f(SIM_LOGGING_D, "Check and send any Outstanding EC.");
                catd_sim_timer_expiration_send_ec(timer_info, apdu_p);
            } else {
                catd_log_f(SIM_LOGGING_E, "No EC found for timer expiry.");
            }
        }
    }
    else {
        catd_log_f(SIM_LOGGING_D, "msg_p or msg_p->timer_info are NULL");
    }
}

// The application shall call this function when refresh is complete
int ste_cat_refresh_result(ste_sim_t                  * UNUSED(cat),
                           uintptr_t                    UNUSED(client_tag),
                           ste_sim_pc_refresh_type_t    UNUSED(type),
                           ste_cat_refresh_result_t     UNUSED(result))
{
    catd_log_f(0, "catd : ste_cat_refresh_result to be implemented");
    // This calls a similar function in the modem to trigger a
    // call to refresh_req(refresh_ok) or refresh_req(refresh_not_ok)
    return 0;
}

/*************************************************************************
 * @brief:    EVENT DOWNLOAD request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the event type.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_event_download(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_event_download_t          *msg;

    msg = catd_msg_event_download_create(fd, buf_p, len, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    CAT ENABLE request from the client
 * @params:
 *            fd:            the socket fd for the request
 *            client_tag:    the client_tag for the request.
 *            buf_p:         related data for this operation, the decision of answer or not.
 *            len:           the length of the data.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_cat_enable(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_cat_enable_t          *msg;

    msg = catd_msg_cat_enable_create(fd, buf_p, len, client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    to enable cat service
 * @params:
 *            client_tag:       the client tag.
 *
 * @return:   result of the operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_enable_cat_service(uintptr_t client_tag)
{
    ste_modem_t                 *modem_p;
    int                          rv;

    modem_p = catd_get_modem();
    if (modem_p == NULL)
    {
        catd_log_f(0, "catd : Missing modem ready!");
        return -1;
    }
    else
    {
        rv = ste_catd_modem_enable(modem_p, client_tag);
        if (rv != 0)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to request modem cat enable!");
            return -1;
        }
        catd_log_f(SIM_LOGGING_E, "catd : request modem to enable cat service!");
        CATD_WAITING_FOR_RESPONSE
    }
    return 0;
}

/*************************************************************************
 * @brief:    to disable cat service
 * @params:
 *            client_tag:       the client tag.
 *
 * @return:   result of the operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_disable_cat_service(uintptr_t client_tag)
{
    ste_modem_t                 *modem_p;
    int                          rv;

    modem_p = catd_get_modem();
    if (modem_p == NULL)
    {
        catd_log_f(0, "catd : Missing modem ready!");
        return -1;
    }
    else
    {
        rv = ste_catd_modem_disable(modem_p, client_tag);
        if (rv != 0)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to request modem cat enable!");
            return -1;
        }
        catd_log_f(SIM_LOGGING_E, "catd : request modem to disable cat service!");
        CATD_WAITING_FOR_RESPONSE
    }
    return 0;
}

/*************************************************************************
 * @brief:    main handler function for CAT ENABLE request
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_cat_enable(ste_msg_t * ste_msg)
{
    catd_msg_cat_enable_t       *msg;
    ste_cat_enable_type_t        enable_type;
    uint8_t                     *p;
    uint8_t                     *p_max;
    int                          rv;
    ste_sim_ctrl_block_t        *ctrl_p;

    catd_log_f(SIM_LOGGING_D, "catd : CAT ENABLE message received");

    msg = (catd_msg_cat_enable_t *) ste_msg;
    //decode the enable type
    p = msg->buf_p;
    p_max = p + msg->len;
    p = (uint8_t*)sim_dec((char*)p, &enable_type, sizeof(enable_type), (char*)p_max);
    if ( !p )
    {
        catd_log_f(SIM_LOGGING_E, "catd : data from socket might have some error.");
    }

    //check the enable type
    if (enable_type == STE_CAT_ENABLE_TYPE_ENABLE)
    {
        if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED)
        {
            catd_log_f(SIM_LOGGING_E, "catd : cat service has already been enabled!");
            //send an normal response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, 0);
            return;
        }
        if (g_tpdl.state != CATD_TPDL_STATE_SUCCESS) {
            //TPDL not done yet, so save the CAT REQ
            catd_log_f(SIM_LOGGING_I, "catd : TPDL not ready yet!");
            enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
            //send an normal response
            catd_send_status_response(msg->fd, msg->client_tag,    STE_CAT_RSP_ENABLE, 0);
            return;
        }
        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, CATD_MSG_CAT_ENABLE, NULL, NULL);
        if (!ctrl_p)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
            //send an error response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
            return;
        }
        //enable the cat service on modem
        rv = catd_enable_cat_service((uintptr_t)ctrl_p);
        if (rv != 0)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to enable cat service!");
            //send an error response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
            free(ctrl_p);
            return;
        }
    }
    else if (enable_type == STE_CAT_ENABLE_TYPE_DISABLE)
    {
        if (enable_status != CATD_CAT_ENABLE_STATUS_ENABLED)
        {
            catd_log_f(SIM_LOGGING_E, "catd : cat service has not been enabled!");
            //send an error response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
            return;
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, CATD_MSG_CAT_DISABLE, NULL, NULL);
        if (!ctrl_p)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to create ctrl block!");
            //send an error response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
            return;
        }
        //disable the cat service on modem
        rv = catd_disable_cat_service((uintptr_t)ctrl_p);
        if (rv != 0)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to enable cat service!");
            //send an error response
            catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
            free(ctrl_p);
            return;
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : invalid cat enable type!");
        //send an error response
        catd_send_status_response(msg->fd, msg->client_tag, STE_CAT_RSP_ENABLE, -1);
        return;
    }
}

/*************************************************************************
 * @brief:    SIM POWER ON/OFF request from uiccd
 * @params:
 *            client_tag:    the client_tag for the request.
 *            power_on:      The sim power status (1=power on, 0=power off).
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_sim_power_on_off(uintptr_t client_tag, int power_on)
{
    catd_msg_sim_power_on_off_t          *msg;

    msg = catd_msg_sim_power_on_off_create(client_tag, power_on);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for SIM POWER ON/OFF request from uiccd
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_main_sim_power_on_off(ste_msg_t * ste_msg)
{
    catd_msg_sim_power_on_off_t    *msg;
    int                             power_on;

    catd_log_f(SIM_LOGGING_D, "catd : SIM POWER ON/OFF message received");

    msg = (catd_msg_sim_power_on_off_t *) ste_msg;
    //check the power on/off status
    power_on = msg->i;
    if (power_on == 0)
    {
        //sim is power off
        catd_change_cat_status(CATD_STATE_INIT);
        catd_tpdl_reset();
        g_tpdl.initiator = CATD_TPDL_INITIATOR_POWER_ON_OFF;
        catd_reset_pc_setup_event_list();
        catd_reset_pc_setup_call();
        catd_reset_pc_send_ussd();
        catd_timer_clean_up_data();
        gcatd.adapt_status = CAT_ADAPT_STATUS_UNKNOWN;
        gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
        if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED)
        {
            enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
        }
    }
    else
    {
        //sim is power on
        g_tpdl.initiator = CATD_TPDL_INITIATOR_POWER_ON_OFF;
    }
}

/*************************************************************************
 * @brief:    CATD reset request from uiccd
 * @params:
 *            client_tag:    the client_tag for the request.
 *
 * @return:   void
 *
 * Notes:     this request will only send a corresponding message to message queue
 *************************************************************************/
void catd_sig_reset(uintptr_t client_tag)
{
    catd_msg_reset_t          *msg;
    msg = catd_msg_reset_create(client_tag);
    if (msg) {
        ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);
    }
}

/*************************************************************************
 * @brief:    main handler function for reset request from uiccd
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   void
 *
 * Notes: Warm reset of CATD. Only CATD state, TPDL will be affected,
 * the clients, socket, etc. will not
 *************************************************************************/
void catd_main_reset(ste_msg_t * UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_D, "catd : reset message received");

    catd_change_cat_status(CATD_STATE_INIT);
    catd_tpdl_reset();
    catd_reset_pc_setup_event_list();
    catd_reset_pc_setup_call();
    catd_reset_pc_send_ussd();
    catd_timer_clean_up_data();
    gcatd.adapt_status = CAT_ADAPT_STATUS_UNKNOWN;
    gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
    if (enable_status == CATD_CAT_ENABLE_STATUS_ENABLED)
    {
        enable_status = CATD_CAT_ENABLE_STATUS_DISABLED_TEMP;
    }
}

static void catd_sms_destroy_session()
{
    SMS_RequestStatus_t rc       = SMS_REQUEST_PENDING;
    SMS_Error_t         err      = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_RequestControlBlock_t cb = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, gcatd.sms_cb.response_socket };

    /*
     * Disconnect w/o unsubscribing. Destroying the session closes the sockets.
     *
     *  CATd SHOULD un-subscribe to any subscribed events before disconnecting.
     *
     *  However because of the following reasons we will disconnect w/o unsubscribing:
     *  1) Currently no part of CATd subscribes to any event
     *  2) CATd have no implementation which can handle events (other than ignore them)
     *  3) There is no SMS/CB API which semantically means "unsubscribe all my event subscriptions"
     */

    rc = Request_SMS_CB_SessionDestroy(&cb, gcatd.sms_cb.event_socket, &err);

    if (rc != SMS_REQUEST_OK || err == SMS_ERROR_INTERN_NO_ERROR) {
        catd_log_f(SIM_LOGGING_E, "catd : error when disconnecting from SMS/CB daemon: %d %d", rc, err);
    }
    gcatd.sms_cb.response_socket = -1;
    gcatd.sms_cb.event_socket = -1;
}

static ssize_t ste_catd_sms_cb_parse(int socket) {
    void*                     signal = NULL;
    uint32_t                  primitive;
    SMS_ClientTag_t           tag;
    ssize_t                   rc;

    signal = Util_SMS_SignalReceiveOnSocket(socket, &primitive, &tag);

    if (signal) {
        if (socket == gcatd.sms_cb.response_socket) {
            catd_sig_sms_cb_response(signal, primitive, tag);
        } else if (socket == gcatd.sms_cb.event_socket) {
            catd_sig_sms_cb_event(signal, primitive, tag);
        } else {
           catd_log_f(SIM_LOGGING_E, "catd : SMS / CB signal received on unknown socket");
           Util_SMS_SignalFree(signal);
        }

        rc = 1;
    } else {
        /*
         * Peer end of socket is closed if Util_SMS_SignalReceiveOnSocket
         * returns a NULL pointer. Shut down the event stream at this end.
         */
        catd_rem_es(socket);
        rc = -1;
    }

    return 1;
}

static ssize_t ste_catd_sms_cb_response_parse(char* buf, char* buf_max, void* UNUSED(ud))
{
    ssize_t rc = 1;

    assert(!buf_max);

    if (buf) {  // Normal operation

        /*
         * For non-buf ES buf actually points to an int which is the read socket
         * associated with the ES.
         */
        rc = ste_catd_sms_cb_parse(*((int*)buf));

    } else {    // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_D, "catd : sms_cb response ES is shutting down");
        gcatd.sms_cb.response_socket_es = false;

        // Disconnect from SMS/CB if both event streams have been removed
        if (!gcatd.sms_cb.event_socket_es) {
            catd_sms_destroy_session();
        }
    }
    return rc;
}

static ssize_t ste_catd_sms_cb_event_parse(char* buf, char* UNUSED(buf_max), void* UNUSED(ud))
{
    ssize_t rc = 1;
    if (buf) {  // Normal operation

        /*
         * For non-buf ES buf actually points to an int which is the read socket
         * associated with the ES.
         */
        rc = ste_catd_sms_cb_parse(*((int*)buf));

    } else {    // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_D, "catd : sms_cb event ES is shutting down");
        gcatd.sms_cb.event_socket_es = false;

        // Disconnect from SMS/CB if both event streams have been removed
        if (!gcatd.sms_cb.response_socket_es) {
            catd_sms_destroy_session();
        }
    }
    return rc;
}

/*
 * Disconnect from SMS/SB server
 */
static void catd_disconnect_from_sms_cb()
{
    catd_log_f(SIM_LOGGING_D, "catd : disconnecting from SMS/CB daemon");

    /*
     * Check if we are connected. Need to check both sockets since disconnect
     * is used in error recovery scenarios.
     */
    if (gcatd.sms_cb.response_socket >= 0 || gcatd.sms_cb.event_socket >= 0) {
        if (gcatd.sms_cb.response_socket_es) {
            catd_rem_es(gcatd.sms_cb.response_socket); // Doesn't do anything if socket is recognized, i.e. -1
        }

        if (gcatd.sms_cb.event_socket_es) {
            catd_rem_es(gcatd.sms_cb.event_socket); // Doesn't do anything if socket is recognized, i.e. -1
        }
    }
}

/*
 * Set up connection to SMS/CB server
 */
static void catd_connect_to_sms_cb()
{
    SMS_RequestStatus_t     rc  = SMS_REQUEST_PENDING;
    SMS_Error_t             err = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    ste_es_t*               es  = NULL;
    ste_es_parser_closure_t pc;

    catd_log_f(SIM_LOGGING_D, "catd : connecting to SMS/CB daemon");

    // Either both sockets are -1 or both are not -1...
    assert((gcatd.sms_cb.response_socket == -1) == (gcatd.sms_cb.event_socket == -1));

    // Are we connected? The assert above means we only have to check one socket...
    if (gcatd.sms_cb.response_socket == -1) {
        rc = Request_SMS_CB_SessionCreate(&gcatd.sms_cb.response_socket,
                                          &gcatd.sms_cb.event_socket,
                                          &err);
        if (rc == SMS_REQUEST_OK && err == SMS_ERROR_INTERN_NO_ERROR) {

            pc.func = ste_catd_sms_cb_response_parse;
            pc.user_data = NULL;
            es = ste_es_new_nonbuf(gcatd.sms_cb.response_socket, &pc); // create ES for response socket
            if (es && !catd_add_es(es)) {
                gcatd.sms_cb.response_socket_es = true;
                pc.func = ste_catd_sms_cb_event_parse;
                es = ste_es_new_nonbuf(gcatd.sms_cb.event_socket, &pc); // create ES for event socket
                if (es && !catd_add_es(es)) {
                    gcatd.sms_cb.event_socket_es = true;
                    return; // Alles gut! :)
                }
            }
            /*
             * If we get here then we either failed to create two ES or we failed to add them to the
             * reader thread. The recovery is the same procedure as for disconnect, so we just call it...
             */
            catd_log_f(SIM_LOGGING_E, "catd : unable to create/add ES for SMS/CB");
            catd_disconnect_from_sms_cb();
        } else {
            catd_log_f(SIM_LOGGING_E, "catd : unable to connect to SMS/CB daemon: %d %d", rc, err);
            if ((gcatd.sms_cb.response_socket != -1) && (gcatd.sms_cb.event_socket != -1)) {
                gcatd.sms_cb.response_socket = -1;
                gcatd.sms_cb.event_socket = -1;
            }
        }
    } else {
        catd_log_f(SIM_LOGGING_D, "catd : already connected to SMS/CB daemon");
    }
}

/*
 * Parser function for ES connected to UICCd responses
 */
static ssize_t ste_catd_uiccd_response_parse(char* buf, char *buf_max, void* UNUSED(ud))
{
    const char* p = buf;
    uint16_t    len;
    uint16_t    cmd;
    uintptr_t   client_tag;
    ssize_t     consumed = 0;

    if (buf && buf_max) {           // Normal operation
        do {
            p = sim_dec(p, &len, sizeof(len), buf_max);
            if (!p)
                break;

            if (buf_max - p < len) // len is the size of this message excl. itself.
                break;

            p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
            if (!p)
                break;

            p = sim_dec(p, &client_tag, sizeof(client_tag), buf_max);
            if (!p)
                break;

            len -= sizeof(cmd) + sizeof(client_tag); // len is remaining number of bytes in this command.

            catd_sig_uiccd_response(p, len, cmd, client_tag);

            consumed = sizeof(len) + sizeof(cmd) + sizeof(client_tag) + len;

        } while (0);
    } else if (!buf && !buf_max) {  // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_D, "catd : uiccd pipe ES is shutting down");
        close(gcatd.uiccd_pipe.read_socket);
        close(gcatd.uiccd_pipe.write_socket);
        gcatd.uiccd_pipe.read_socket = -1;
        gcatd.uiccd_pipe.write_socket = -1;
    } else {
        catd_log_f(SIM_LOGGING_E, "catd : weird uiccd ES parse scenario");
        consumed = -1;
    }
    return consumed;
}

#define CATD_UICCD_ES_BUF_SIZE 2048 /* Same size as used in uiccd client side reader */

/*
 * Set up a "fake" client connection towards UICCd
 */
static void catd_connect_uiccd_es()
{
    ste_es_t*               es  = NULL;
    ste_es_parser_closure_t pc;

    catd_log_f(SIM_LOGGING_D, "catd : creating ES for uiccd connection");
    if (gcatd.uiccd_pipe.read_socket == -1)
    {
        int mypipe[2];
        int rc = pipe(mypipe);
        if (!rc)
        {
            gcatd.uiccd_pipe.read_socket = mypipe[0];
            gcatd.uiccd_pipe.write_socket = mypipe[1];

            // Pipe read socket must be non-blocking for normal ES.
            rc = fcntl(gcatd.uiccd_pipe.read_socket, F_SETFL, O_NONBLOCK);
            if (!rc)
            {
                pc.func = ste_catd_uiccd_response_parse;
                pc.user_data = NULL;
                es = ste_es_new_normal(gcatd.uiccd_pipe.read_socket, CATD_UICCD_ES_BUF_SIZE, &pc);

                if (es && !catd_add_es(es)) {
                    return; // successfull
                }
            }
        }
    }
    catd_log_f(SIM_LOGGING_E, "catd : unable to create/add ES for uiccd connection");
}

/*
 * Disconnect from UICCd
 */
static void catd_disconnect_uiccd_es()
{
    if (gcatd.uiccd_pipe.read_socket >= 0) {
        catd_rem_es(gcatd.uiccd_pipe.read_socket);
    }
}

/*************************************************************************
 * @brief:    Create pipes for timer expiration and then add the reader
 *            pipe to CATD's event stream.
 *
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_connect_timer_es(void)
{
    ste_es_t*               es  = NULL;
    ste_es_parser_closure_t pc;


    catd_log_f(SIM_LOGGING_D, "catd : creating ES for timer expiration.");

    if (gcatd.timer_pipe.read_socket == -1 && gcatd.timer_pipe.write_socket == -1) {
        int mypipe[2];
        int rc = pipe(mypipe);

        if (!rc)
        {
            gcatd.timer_pipe.read_socket = mypipe[0];
            gcatd.timer_pipe.write_socket = mypipe[1];

            catd_log_f(SIM_LOGGING_D, "catd : set read socket to non-blocking.");

            // Pipe read socket must be non-blocking for normal ES.
            rc = fcntl(gcatd.timer_pipe.read_socket, F_SETFL, O_NONBLOCK);

            if (!rc) {
                pc.func = ste_catd_timer_response_parse;
                pc.user_data = NULL;
                es = ste_es_new_normal(gcatd.timer_pipe.read_socket, CATD_UICCD_ES_BUF_SIZE, &pc);

                catd_log_f(SIM_LOGGING_D, "catd : add timer read pipe to event stream handler.");
                if (es && !catd_add_es(es)) {
                    return; // Successful.
                }
                catd_log_f(SIM_LOGGING_E, "catd : failed adding timer read pipe to event stream handler!!.");
            }
        }
    }
    catd_log_f(SIM_LOGGING_E, "catd : unable to create/add ES for timer expiration.");
}

/*************************************************************************
 * @brief:    Remove the timer expiration read pipe from CATD's event,
 *            stream and close the pipes. Also clean up/reset data related
 *            to each timer.
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_disconnect_timer_es(void)
{
    catd_log_f(SIM_LOGGING_D, "catd : removing ES for timer expiration.");

    if (gcatd.timer_pipe.read_socket >= 0) {
        catd_rem_es(gcatd.timer_pipe.read_socket);
    }

    catd_timer_clean_up_data();
}

/*************************************************************************
 * @brief:    init the CATD module, create the msg queue, reset the client list
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void init_catd()
{
    uint8_t i;
    catd_mq_req = ste_msgq_new();
    catd_mq_rsp = catd_mq_req;

    catd_change_cat_status(CATD_STATE_INIT);
    catd_tpdl_reset();
    enable_status = CATD_CAT_ENABLE_STATUS_DISABLED;
    gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
    for (i = 0; i < STE_CATD_MAX_NBR_OF_CLIENTS; i++) {
        catd_client_reset(&(gcatd.client_list[i]));
    }

    gcatd.uiccd_pipe.read_socket  = -1;
    gcatd.uiccd_pipe.write_socket = -1;
    gcatd.sms_cb.response_socket  = -1;
    gcatd.sms_cb.event_socket     = -1;
    gcatd.timer_pipe.read_socket  = -1;
    gcatd.timer_pipe.write_socket = -1;
    gcatd.adapt_status = CAT_ADAPT_STATUS_UNKNOWN;

    CATD_PROCEED_WITH_REQUEST;
}

/*************************************************************************
 * @brief:    reset the CATD module state to NULL
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void reset_catd()
{
    catd_change_cat_status(CATD_STATE_NULL);
    catd_tpdl_reset();
    enable_status = CATD_CAT_ENABLE_STATUS_DISABLED;
    catd_reset_pc_setup_event_list();
    catd_reset_pc_setup_call();
    catd_reset_pc_send_ussd();
    catd_timer_clean_up_data();
    gcatd.ongoing_cmd = STE_APDU_CMD_TYPE_NONE;
}

/*************************************************************************
 * @brief:    msg handler function for the CATD module
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   result of the msg handling
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
static int catd_msg_handler(ste_msg_t* ste_msg)
{
    if (!ste_msg) {
        catd_log_f(SIM_LOGGING_E, "catd : null message");
        return 0;
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s handling message type = 0x%X, CATD state: %d.", __func__ , ste_msg->type, gcatd.state);

    switch (ste_msg->type)
    {
        case CATD_MSG_TIMEOUT_EVENT:
        {
            catd_main_timeout_event(ste_msg);
        }
        break;

        case CATD_MSG_REGISTER:
        {
            catd_main_register(ste_msg);
        }
        break;

        case CATD_MSG_DEREGISTER:
        {
            catd_main_deregister(ste_msg);
        }
        break;

        case CATD_MSG_CAT_ENABLE:
        {
            catd_main_cat_enable(ste_msg);
        }
        break;

        case CATD_MSG_STARTUP:
        {
            catd_log_f(SIM_LOGGING_D, "catd : STARTUP message received");

            startup_fd = ((catd_msg_startup_t*)ste_msg)->fd;
            startup_ct = ((catd_msg_startup_t*)ste_msg)->client_tag;

            util_continue_as_non_privileged_user();

            if (g_tpdl.initiator == CATD_TPDL_INITIATOR_NULL)
            {
                catd_log_f(SIM_LOGGING_E, "catd : Initiating TPDL in STARTUP");
                g_tpdl.initiator = CATD_TPDL_INITIATOR_STARTUP;
                catd_tpdl_state_machine(CATD_TPDL_EVENT_REQUEST);
            }
            else
            {
                catd_log_f(SIM_LOGGING_D, "catd : TPDL is done already, skipping in STARTUP");
            }
        }
        break;

        case CATD_MSG_MODEM_NOT_READY:
        {
            catd_main_modem_not_ready(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_READY:
        {
            catd_main_modem_ready(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_RESET:
        {
            catd_main_modem_reset(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_IND_REG_OK:
        {
            catd_main_modem_ind_reg_ok(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_CAT_NOT_SUPPORTED:
        {
            catd_main_modem_cat_not_supported(ste_msg);
        }
        break;

        case CATD_MSG_TPDL_STATUS:
        {
            catd_main_tpdl_status(ste_msg);
        }
        break;

        case CATD_MSG_ADAPT_TERMINAL_SUPPORT_TABLE_UPDATE_STATUS:
        {
            catd_main_adapt_terminal_support_table_update_status(ste_msg);
        }
        break;

        case CATD_MSG_APP_READY:
        {
            catd_main_app_ready(ste_msg);
        }
        break;

        case CATD_MSG_CAT_SERVER_READY:
        {
            catd_main_cat_server_ready(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_ENABLE_STATUS:
        {
            catd_main_modem_enable_status(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_DISABLE_STATUS:
        {
            catd_main_modem_disable_status(ste_msg);
        }
        break;

        case CATD_MSG_GET_CAT_STATUS:
        {
            catd_main_get_cat_status(ste_msg);
        }
        break;

        case CATD_MSG_APDU:
        {
            catd_main_apdu(ste_msg);        /* Only STE_APDU_KIND_PC */
        }
        break;

        case CATD_MSG_TR:
        {
            catd_main_tr(ste_msg);
        }
        break;

        case CATD_MSG_EC:
        {
            catd_main_ec(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_CAT_RESPONSE:
        {
            catd_main_modem_cat_response(ste_msg);
        }
        break;

        case CATD_MSG_CALL_CONTROL:
        {
            catd_main_call_control(ste_msg);
        }
        break;

        case CATD_MSG_CAT_SESSION_EXPIRED:
        {
            catd_main_cat_session_expired(ste_msg);
        }
        break;

        case CATD_MSG_SET_TERMINAL_PROFILE:
        {
            catd_main_set_terminal_profile(ste_msg);
        }
        break;

        case CATD_MSG_GET_TERMINAL_PROFILE:
        {
            catd_main_get_terminal_profile(ste_msg);
        }
        break;

        case CATD_MSG_ANSWER_CALL:
        {
            catd_main_answer_call(ste_msg);
        }
        break;

        case CATD_MSG_SMS_CONTROL:
        {
            catd_main_sms_control(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_SET_POLL_INTERVAL_RESPONSE:
        {
            catd_main_modem_set_poll_interval_response(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_REFRESH_STATUS:
        {
            catd_main_modem_refresh_response(ste_msg);
        }
        break;

        case CATD_MSG_MODEM_REFRESH_IND:
        {
            catd_main_modem_refresh_ind(ste_msg);
        }
        break;

        case CATD_MSG_CN_RESPONSE:
        {
            catd_main_cn_response_handler(ste_msg);
        }
        break;

        case CATD_MSG_CN_EVENT:
        {
            catd_handle_pc_send_ussd_event_handler(ste_msg); // Let the USSD functionality have a look at the message...
            catd_handle_cn_event_pc_setup_call(ste_msg);
            // Event download should be handled last so that terminal responses are sent before any event downloads!
            catd_handle_event(ste_msg); // Not a good function name. This is the CN event handler associated with PC Setup event list...
        }
        break;

        case CATD_MSG_SMS_CB_PERFORM_CONNECT:
        {
            catd_main_sms_cb_perform_connect(ste_msg);
        }
        break;

        case CATD_MSG_SMS_CB_RESPONSE:
        {
            catd_main_sms_cb_response(ste_msg);
        }
        break;

        case CATD_MSG_SMS_CB_EVENT:
        {
            catd_main_sms_cb_event(ste_msg);
        }
        break;

        case CATD_MSG_UICCD_RESPONSE:
        {
            catd_main_uiccd_response(ste_msg);
        }
        break;

        case CATD_MSG_TIMER_EXPIRATION_SEND_EC:
        {
            catd_main_sim_timer_expiration_send_ec(ste_msg);
        }
        break;

        case CATD_MSG_SIM_POWER_ON_OFF:
        {
            catd_main_sim_power_on_off(ste_msg);
        }
        break;

        case CATD_MSG_RESET:
        {
            catd_main_reset(ste_msg);
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : unknown message received %08x", ste_msg->type);
        }
    }

    ste_msg_delete(ste_msg);
    return 0;
}

/*************************************************************************
 * @brief:    handler function for stopping CATD module
 * @params:
 *            ste_msg:     the msg for the particular request, with related data.
 *
 * @return:   the barrier of the msg
 *
 * Notes:
 *************************************************************************/
static cat_barrier_t    *catd_main_stop(ste_msg_t * ste_msg)
{
    cat_barrier_t    *result = ((catd_msg_stop_t *) ste_msg)->bar;

    cn_client_close_session();

    // Disconnect pipes from timer expiration, and clean up/reset data related to each timer.
    catd_disconnect_timer_es();

    //Disconnect pipe from uiccd
    catd_disconnect_uiccd_es();

    // Disconnect from SMS/CB daemon
    catd_disconnect_from_sms_cb();

    ste_msg_delete(ste_msg);
    return result;
}

/*************************************************************************
 * @brief:    main thread function for the CATD module, query the mq and process the msg
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void            *catd_main_loop_th(void *UNUSED(arg))
{
    ste_msg_t              *msg;
    cat_barrier_t          *bar = 0;
    int                     rv = 0;

    setThreadName("catd");
    catd_log_f(SIM_LOGGING_D, "catd : New Thread 0x%08lx", (unsigned long)pthread_self());

    util_continue_as_non_privileged_user();

    // Connect pipes for timer expiration.
    catd_connect_timer_es();

    rv = catd_timer_create_signal_handler();
    /*If we fail in creating signal handler for timers, we will not receive events in CATD reader.
     * so remove the file descriptors from the event stream.
     * */
    if (rv != 0) {
        catd_disconnect_timer_es();
    }

    catd_connect_uiccd_es();

    catd_log_f(SIM_LOGGING_D, "catd : Entering main loop");

    while (!bar) {
        msg = ste_msgq_pop( gcatd.working_mq );

        if (!msg) {
            catd_log_s(SIM_LOGGING_E, "catd : NULL message received", 0);
            continue;
        }

        if (msg->type == CATD_MSG_STOP) {
            bar = catd_main_stop(msg);
            continue;
        }

        rv = catd_msg_handler(msg);
    }

    catd_log_s(SIM_LOGGING_D, "catd : exit main loop, thread dying.", 0);

//    ste_msgq_delete(catd_mq_rsp);
    catd_mq_rsp = 0;

    ste_msgq_delete(catd_mq_req);
    catd_mq_req = 0;

    if (bar) {
        cat_barrier_release(bar, 0);
    }

    return 0;
}


/*************************************************************************
 * @brief:    start the CATD module, launch the thread
 * @params:
 *            client_tag:    the client_tag for the request.
 *
 * @return:   result of the startup operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_start(uintptr_t client_tag)
{
    int                     rv;
    pthread_t               tid;

    catd_log_s(SIM_LOGGING_D, "catd : start", client_tag);
    init_catd();

    rv = sim_launch_thread(&tid, PTHREAD_CREATE_DETACHED, catd_main_loop_th, 0);
    if (rv < 0) {
        catd_log_s(SIM_LOGGING_E, "catd : Failed to launch thread", client_tag);
        return rv;
    }

    return 0;
}


/*************************************************************************
 * @brief:    stop the CATD module, kill the thread
 * @params:
 *            client_tag:    the client_tag for the request.
 *
 * @return:   result of the stop operation
 *            Success: 0; Failed: -1
 *
 * Notes:
 *************************************************************************/
int catd_stop(uintptr_t client_tag)
{
    catd_msg_stop_t        *msg;
    cat_barrier_t          *bar;        /* Barrier to sync shutdown */

    bar = cat_barrier_new();
    if (!bar)
        return -1;

    cat_barrier_set(bar);

    msg = catd_msg_stop_create(bar, client_tag);
    if (!msg)
        return -1;

    ste_msgq_add(catd_mq_req, (ste_msg_t *) msg);

    if ( cat_barrier_wait(bar, 0) != 0 ) {  /* Wait for thread to die */
        catd_log_f(SIM_LOGGING_E, "%s: cat_barrier_wait failed", __func__);
        return -1;
    }
    cat_barrier_delete(bar);
    reset_catd();

    return 0;
}

/**
 * Returns the socket to use as SMS/CB response socket in request functions
 */
int catd_get_sms_cb_request_socket()
{
    return gcatd.sms_cb.response_socket;
}

/**
 * Returns the socket to use for SMS/CB event subscription
 */
int catd_get_sms_cb_event_socket()
{
    return gcatd.sms_cb.event_socket;
}

/**
 * Returns the socket to use for internally requesting services from uiccd
 * (e.g. the socket uiccd should write the tresponse to).
 */
int catd_get_uiccd_request_socket()
{
    return gcatd.uiccd_pipe.write_socket;
}

/**
* Returns the socket to use for internally sending timeout data from CATD to
* CATD reader for further processing/routing.
*/
int catd_get_timer_write_socket()
{
    return gcatd.timer_pipe.write_socket;
}

/**
 * Returns adapt status
 */
cat_adapt_status_t catd_adapt_get_status()
{
    return gcatd.adapt_status;
}
