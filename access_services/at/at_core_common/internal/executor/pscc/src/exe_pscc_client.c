/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h> /* must be included before sys/un.h, which is badly written */
#include <sys/un.h>     /* <-- */
#include <sys/types.h>

#include "atc_log.h"
#include "atc_selector.h"

#include "exe.h"
#include "exe_internal.h"
#include "exe_start.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_events.h"
#include "exe_pscc_util.h"
#include "exe_request_record.h"
#include "exe_request_record_p.h"

#include "libstecom.h"  /* send/receive for  pscc sockets. MACROs for handling CONNId and CT*/
#include "mpl_config.h" /* libmpl */
#include "pscc_msg.h"
#include "sterc_msg.h"
#include "stecom_i.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "exe_extended_pscc_events.h"
#endif

#define REQUEST_SOCKET_PSCC  "/dev/socket/pscc_ctrl_server"
#define EVENT_SOCKET_PSCC    "/dev/socket/pscc_event_server"
#define REQUEST_SOCKET_STERC "/dev/socket/sterc_ctrl_server"
#define EVENT_SOCKET_STERC   "/dev/socket/sterc_event_server"
#define SOCKET_NAME_MAX_LENGTH 107 /* declared in un.h */

#define DEFAULT_TIMEOUT 5
#define REQUEST_BUFFER_SIZE 4096
#define RESPONSE_BUFFER_SIZE 4096
#define EVENT_BUFFER_SIZE 4096


static char   request_buffer_p[REQUEST_BUFFER_SIZE];
static char   response_buffer_p[RESPONSE_BUFFER_SIZE];
static char   event_buffer_p[EVENT_BUFFER_SIZE];


struct psccclient_s {
    int pscc_fd_request;
    int sterc_fd_request;
    int pscc_fd_event;
    int sterc_fd_event;
    struct stec_ctrl *pscc_ctrl_request;
    struct stec_ctrl *sterc_ctrl_request;
    struct stec_ctrl *pscc_ctrl_event;
    struct stec_ctrl *sterc_ctrl_event;
};


/*  container of data to be used in various phases of the request sequence
 *  The pscc_event_p and pscc_response_p entries below are set entering the
 *  any call-backs, to allow for request functions to know where to send requests.
 *  For a response, the pscc_event_p is NULL and vice versa.
 *
 *  request is not the request function as exposed to the parser, but rather the next request
 *  against pscc in the chain to execute the full AT command. Should be set to NULL after it has been
 *  extracted, and if found to be NULL when handling the response, then the chain is complete.
 *
 *
 */


/*static pscc_context_t pscc_context;*/ /* There should be only one pscc context at all times, so avoid heap handling */

/* Local functions */


/********************************************************************
 * Private prototypes
 ********************************************************************
 */

static void pscc_log(void *user_p, int level, const char *file, int line, const char *format_p, ...);
static bool psccclient_selector_callback_request(int fd, void *data_p);
static bool psccclient_selector_callback_event(int fd, void *data_p);
static int psccclient_receive_event(psccclient_t *psccclient_p, mpl_msg_event_t *event_p, bool sterc_event);
static int psccclient_receive_response(psccclient_t *psccclient_p, mpl_msg_resp_t *response_p);
static bool psccclient_set_pscc_response_p(exe_request_record_t *record_p, mpl_msg_resp_t *response_p);
static exe_request_record_t *get_request_record_from_response_by_client_tag(mpl_msg_resp_t *response_p);
static exe_request_record_t *get_request_record_from_response_by_sterc_client_tag(mpl_msg_resp_t *response_p);
static int get_connection_id(mpl_msg_event_t *event_p);
static exe_request_record_t *get_request_record_from_event_by_connection_id(mpl_msg_event_t *event_p);
static void psccclient_print_sterc_response_error(const char *prefix_string_p, pscc_result_t result);
static bool add_client_tag_to_request(pscc_msg_t *request_p, exe_request_record_t *record_p);


/********************************************************************
 * Private methods
 ********************************************************************
 */
static void pscc_log(void *user_p, int level, const char *file, int line, const char *format_p, ...) /* TODO: Fix compiler warning (strict build flag settings)... */
{
    char string_buffer_p[200];
    va_list arg_list;
    va_start(arg_list, format_p);
    vsnprintf(string_buffer_p, 200, format_p, arg_list);
    va_end(arg_list);
    ATC_LOG_E("pscc error: %s (%s line=%d)", string_buffer_p, file, line);
}

static bool psccclient_selector_callback_request(int fd, void *data_p)
{
    psccclient_t            *psccclient_p  = (psccclient_t *)data_p;
    exe_request_t            request       = NULL;
    exe_request_record_t    *record_p      = NULL;
    mpl_msg_resp_t          *response_p    = NULL;
    int res;

    ATC_LOG_D("fd = %d, data_p = %p", fd, data_p);

    if (NULL == psccclient_p) {
        ATC_LOG_E("psccclient_p is NULL!");
        goto error;
    }

    if (psccclient_p->pscc_fd_request == fd) {
        response_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
        exe_pscc_action_t action = EXE_PSCC_ACTION_UNDEFINED;

        if (NULL == response_p) {
            ATC_LOG_E("mpl_resp_msg_alloc failed!");
            goto error;
        }

        res = psccclient_receive_response(psccclient_p, response_p);

        if (res < 0) {
            ATC_LOG_E("receive_response failed!");
            goto error;
        } else if (res == 0) {
            /* Zero is returned if peer has closed the socket */
            goto disconnect;
        }

        record_p = get_request_record_from_response_by_client_tag(response_p);

        if (NULL != record_p) {
            action = psccclient_get_action(record_p);
        }
    } else if (psccclient_p->sterc_fd_request == fd) {
        response_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);

        if (NULL == response_p) {
            ATC_LOG_E("sterc mpl_resp_msg_alloc failed!");
            goto error;
        }

        res = psccclient_receive_response(psccclient_p, response_p);

        if (res < 0) {
            ATC_LOG_E("receive_response from sterc failed!");
            goto error;
        } else if (res == 0) {
            /* Zero is returned if peer has closed the socket */
            goto disconnect;
        }

        record_p = get_request_record_from_response_by_sterc_client_tag(response_p);

        if (sterc_result_ok != response_p->result &&
                sterc_result_failed_object_not_found != response_p->result) {
            psccclient_print_sterc_response_error("sterc response failed:", response_p->result);
            goto error;
        }

    } else {
        ATC_LOG_E("invalid fd_request");
        goto error;
    }

    if (NULL == record_p) {
        ATC_LOG_E("exe request record is NULL, failed!");
        goto error;
    }

    if (!psccclient_set_pscc_response_p(record_p, response_p)) {
        ATC_LOG_E("failed to set response_p to record!");
        goto error;
    }

    request = request_record_get_request(record_p);

    if (NULL == request) {
        ATC_LOG_E("Failed to obtained next leg in request sequence. Call back to original exe_request.");
        goto error;
    }

    /* Reset the event pointer in client data as it may be checked in the request. */
    psccclient_set_pscc_event_p(record_p, NULL);


    if (EXE_SUCCESS != request(record_p)) {
        ATC_LOG_E("request returned unsuccessfully, failed!");
    }

error:
    mpl_msg_free((mpl_msg_t *)response_p);
    return true;

disconnect:
    ATC_LOG_E("********************************************************");
    ATC_LOG_E("***** at_core lost contact with pscc daemon        *****");
    ATC_LOG_E("********************************************************");

    /* Shut down and clean-up to allow reconnection */
    psccclient_close_session(psccclient_p);
    mpl_msg_free((mpl_msg_t *)response_p);

    return false;   /* Returning false will remove the fd from the selector. */
}

static bool psccclient_selector_callback_event(int fd, void *data_p)
{
    psccclient_t            *psccclient_p  = (psccclient_t *)data_p;
    mpl_msg_event_t         *event_p       = NULL;
    exe_request_record_t    *record_p      = NULL;
    int                      conn_id       = 0;
    bool                     in_iadb       = false;
    exe_request_t            request;
    bool                     event_processed = false;
    bool                     network_initiated = false;
    mpl_param_element_t      *param_elem_p = NULL;
    int res;
    bool                     sterc_event = false;

    ATC_LOG_D("fd = %d, data_p = %p", fd, data_p);

    if (NULL == psccclient_p) {
        ATC_LOG_E("psccclient_p is NULL!");
        goto exit;
    }

    if (psccclient_p->pscc_fd_event == fd) {
        event_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
    } else if (psccclient_p->sterc_fd_event == fd) {
        event_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
        sterc_event = true;
    } else {
        ATC_LOG_E("invalid fd_request");
        goto exit;
    }

    if (NULL == event_p) {
        ATC_LOG_E("mpl_event_msg_alloc failed!");
        goto exit;
    }

    res = psccclient_receive_event(psccclient_p, event_p, sterc_event);

    if (res < 0) {
        ATC_LOG_E("receive_event failed!");
        goto exit;
    } else if (res == 0) {
        /* Zero is returned if peer has closed the socket */
        goto disconnect;
    }

    ATC_LOG_I("received event %d", event_p->id);

    if (PSCC_CT_PRESENT((pscc_msg_t *) event_p)) {
        /* The event included a client tag.
         * Concerns attach and detach.
         */
        int client_tag = (int)PSCC_GET_CT((pscc_msg_t *) event_p);
        record_p = request_record_get_record_from_client_tag((int)PSCC_GET_CT((pscc_msg_t *) event_p));

        ATC_LOG_I("Client Tag Present");
        if (NULL == record_p) {
            ATC_LOG_E("could not find request record");
            goto exit;
        }
    } else if (PSCC_CONNID_PRESENT((pscc_msg_t *) event_p)) {
        /* The event included a conn id.
         * Get the corresponding request from iadb.
         */

        ATC_LOG_I("CONNID Present");
        conn_id = get_connection_id(event_p);
        in_iadb = psccclient_iadb_get_at_ct_on_conn_id(conn_id, (int *)(&record_p));

        if(in_iadb && (event_p->id == pscc_event_modified) && (NULL != record_p) && (EXE_CGCMOD != record_p->request_id)) {
            ATC_LOG_I("pscc_event_modified recvd and record_p->request_id = %d",record_p->request_id);
            network_initiated = true;
        }

        /* Even if there is a pscc call waiting for a return for a particular connid, there is a chance
         * that the event might be unsolicited. Currently only occurs for (network initiated)
         * disconnected events. In that case, handle it as unsolicited. */

        if (in_iadb && event_p->id == pscc_event_disconnected) {
            param_elem_p = mpl_param_list_find(pscc_paramid_reason, event_p->param_list_p);

            if (param_elem_p != NULL && (pscc_reason_pdp_context_nw_deactivated == *(int *)(param_elem_p->value_p))) {
                ATC_LOG_I("nw initiated pscc_event_disconnected. Treat as unsolicited.");
                network_initiated = true;
            } else if (param_elem_p != NULL && (pscc_reason_pdp_context_nw_deactivated_reset == *(int *)(param_elem_p->value_p))) {
                ATC_LOG_I("nw initiated pscc_event_disconnected with reset indication. Treat as unsolicited.");
                network_initiated = true;
            }
        }

        if (NULL == record_p || !in_iadb || network_initiated) {
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
            /* Note: The extended event handler needs to return false if processing
             * is not complete and further processing is required by the generic
             * handler. Return true to prevent the generic handler from ever seeing
             * the event.
             */
            event_processed = exe_extended_pscc_event_handler(psccclient_p, event_p, conn_id);
#else
            event_processed = false;
#endif
            ATC_LOG_I("Received unsolicited event not related to an outstanding request");

            /* Unsolicited event not related to an outstanding request. */
            if (!event_processed) {
                event_processed = exe_pscc_event_handler(psccclient_p, event_p, conn_id);

                if (!event_processed) {
                    ATC_LOG_E("Unknown/Unhandled event! (%d)", event_p->id);
                }
            }

            goto exit;
        }
    } else if (STERC_HANDLE_PRESENT((pscc_msg_t *) event_p)) {
        int sterc_handle = (int)STERC_GET_HANDLE((pscc_msg_t *) event_p);

        in_iadb = psccclient_iadb_get_at_ct_on_sterc_handle(sterc_handle, (int *)(&record_p));

        ATC_LOG_I("Sterc Handle Present");
        if (in_iadb && event_p->id == sterc_event_disconnected) {
            param_elem_p = mpl_param_list_find(sterc_paramid_reason, event_p->param_list_p);

            if (param_elem_p != NULL) {
                ATC_LOG_I("Received sterc disconnect reason: (%d)", *(int *)(param_elem_p->value_p));
            }
        }

        if (NULL == record_p || !in_iadb) {
            ATC_LOG_I("Received unsolicited event not related to an outstanding request with Sterc Handle Present");
            /* Unsolicited event not related to an outstanding request. */
            event_processed = exe_pscc_event_handler(psccclient_p, event_p, sterc_handle);

            if (!event_processed) {
                ATC_LOG_E("Received unknown sterc event! (%d)", event_p->id);
            }

            goto exit;
        }
    } else {
        /* No client tag and no connection id. */
        ATC_LOG_E("No CT and no CONNID");
        goto exit;
    }

    ATC_LOG_I("Setting event to record");
    if (!psccclient_set_pscc_event_p(record_p, event_p)) {
        ATC_LOG_E("failed to set event_p to record!");
        goto exit;
    }

    request = request_record_get_request(record_p);

    if (EXE_SUCCESS != request(record_p)) {
        ATC_LOG_E("request returned unsuccessfully, failed!");
    }

exit:
    mpl_msg_free((mpl_msg_t *)event_p);
    return true;   /* Returning false will remove the fd from the selector. */

disconnect:
    ATC_LOG_E("********************************************************");
    ATC_LOG_E("***** at_core lost contact with pscc daemon        *****");
    ATC_LOG_E("********************************************************");
    mpl_msg_free((mpl_msg_t *)event_p);

    /* Do nothing. Let the receive socket callback clean-up. */

    return false;   /* Returning false will remove the fd from the selector. */
}

static bool add_client_tag_and_connid_to_request(pscc_msg_t *request_p, exe_request_record_t *record_p)
{
    bool result = false;
    int client_tag = 0;
    int conn_id = 0;

    if (NULL == request_p ||
            NULL == request_p->common.message_set_descr_p ||
            NULL == request_p->common.message_set_descr_p->param_descr_set_p) {
        ATC_LOG_E("request_p not fully allocated");
        goto error;
    }

    client_tag = request_record_get_client_tag(record_p);


    switch (request_p->common.message_set_descr_p->param_descr_set_p->param_set_id) {
    case PSCC_PARAM_SET_ID:

        if (psccclient_iadb_get_conn_id_on_at_ct((uintptr_t)record_p, &conn_id)) {
            if (mpl_add_param_to_list(&request_p->req.param_list_p, pscc_paramid_connid, &conn_id) < 0) {
                ATC_LOG_E("mpl_add_param_to_list failed!");
                goto error;
            }
        }

        if (mpl_add_param_to_list(&request_p->req.param_list_p, pscc_paramid_ct, &client_tag) < 0) {
            ATC_LOG_E("mpl_add_param_to_list (pscc) failed!");
            goto error;
        }

        ATC_LOG_I("(pscc): CT=%d connId=%d", client_tag, conn_id);
        break;
    case STERC_PARAM_SET_ID:

        if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag) < 0) {
            ATC_LOG_E("mpl_add_param_to_list (sterc) failed!");
            goto error;
        }

        ATC_LOG_I("(sterc): CT=%d ", client_tag);
        break;
    default:
        ATC_LOG_E("invalid param_set_id!");
        goto error;
    }

    result = true;

error:

    return result;
}


static bool psccclient_set_pscc_response_p(exe_request_record_t *record_p, mpl_msg_resp_t *response_p)
{
    bool                      result        = NULL;
    psccclient_record_data_t *client_data_p = NULL;

    client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("record_p is NULL. Failed.");
        goto exit;
    }

    client_data_p->pscc_response_p = response_p;
    result = true;

exit:
    return result;
}


int psccclient_receive_event(psccclient_t *psccclient_p, mpl_msg_event_t *event_p, bool sterc_event)
{
    int res;

    /* Retrieve event data from pscc or sterc */
    if (sterc_event) {
        res = stec_recv(psccclient_p->sterc_ctrl_event, event_buffer_p, EVENT_BUFFER_SIZE, DEFAULT_TIMEOUT);
    } else {
        res = stec_recv(psccclient_p->pscc_ctrl_event, event_buffer_p, EVENT_BUFFER_SIZE, DEFAULT_TIMEOUT);
    }

    if (res < 0) {
        ATC_LOG_E("stec_recv failed! (%s)", strerror(errno));
        goto error;
    } else if (res > 0) {

        /* Unpack event data into a usable form */
        if (mpl_msg_unpack(event_buffer_p, EVENT_BUFFER_SIZE, (mpl_msg_t *)event_p) < 0) {
            ATC_LOG_E("mpl_msg_unpack failed!");
            goto error;
        }

        if (event_p->type != mpl_msg_type_event) {
            ATC_LOG_E("bad event data!");
            goto error;
        }

    } else {
        /*
         * If ste_recv returns zero this means the peer has closed the socket.
         * Clean-up at this end, and then reconnect.
         */
        ATC_LOG_E("Server disconnect detected!");
    }

exit:
    return res;

error:
    res = -1;
    goto exit;
}

int psccclient_receive_response(psccclient_t *psccclient_p, mpl_msg_resp_t *response_p)
{
    int  param_set_id;
    struct stec_ctrl *ctrl_request_p = NULL;
    mpl_msg_type_t msgtype_resp;
    int res;

    if (NULL == psccclient_p) {
        ATC_LOG_E("psccclient_p is NULL! Failed.");
        goto error;
    }

    if (NULL == response_p->message_set_descr_p ||
            NULL == response_p->message_set_descr_p->param_descr_set_p) {
        ATC_LOG_E("response_p not fully allocated");
        goto error;
    }

    param_set_id = response_p->message_set_descr_p->param_descr_set_p->param_set_id;

    switch (param_set_id) {
    case PSCC_PARAM_SET_ID:
        ctrl_request_p = psccclient_p->pscc_ctrl_request;
        msgtype_resp = pscc_msgtype_resp;
        break;
    case STERC_PARAM_SET_ID:
        ctrl_request_p = psccclient_p->sterc_ctrl_request;
        msgtype_resp = sterc_msgtype_resp;
        break;
    default:
        ATC_LOG_E("invalid param_set_id!");
        goto error;
    }

    res = stec_recv(ctrl_request_p, response_buffer_p, RESPONSE_BUFFER_SIZE, DEFAULT_TIMEOUT);

    if (res < 0) {
        ATC_LOG_E("stec_recv failed! (%s)", strerror(errno));
        goto error;
    } else if (res > 0) {

        if (mpl_msg_unpack(response_buffer_p, RESPONSE_BUFFER_SIZE, (mpl_msg_t *)response_p) < 0) {
            ATC_LOG_E("mpl_msg_unpack failed!");
            goto error;
        }

        if (response_p->type != msgtype_resp) {
            ATC_LOG_E("wrong message type!");
            goto error;
        }
    } else {
        /*
         * If ste_recv returns zero this means the peer has closed the socket.
         * Clean-up at this end, and then reconnect.
         */
        ATC_LOG_E("Server disconnect detected!");
    }

exit:
    return res;

error:
    res = -1;
    goto exit;
}

static exe_request_record_t *get_request_record_from_response_by_client_tag(mpl_msg_resp_t *response_p)
{
    int client_tag = 0;
    exe_request_record_t *record_p = NULL;

    if (NULL == response_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }

    if (!PSCC_CT_PRESENT((pscc_msg_t *)response_p)) {
        ATC_LOG_E("Client tag not present in response message!");
        goto exit;
    }

    client_tag = (int)PSCC_GET_CT((pscc_msg_t *)response_p);
    record_p = request_record_get_record_from_client_tag(client_tag);

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL");
        goto exit;
    }

exit:
    return record_p;
}

static exe_request_record_t *get_request_record_from_response_by_sterc_client_tag(mpl_msg_resp_t *response_p)
{
    int client_tag = 0;
    exe_request_record_t *record_p = NULL;

    if (NULL == response_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }

    if (!STERC_CT_PRESENT((pscc_msg_t *) response_p)) {
        ATC_LOG_E("Client tag not present in response message!");
        goto exit;
    }

    client_tag = (int)STERC_GET_CT((pscc_msg_t *)response_p);
    record_p = request_record_get_record_from_client_tag(client_tag);

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL");
        goto exit;
    }

exit:
    return record_p;
}

static int get_connection_id(mpl_msg_event_t *event_p)
{
    int conn_id = 0;

    if (NULL == event_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }

    if (!PSCC_CONNID_PRESENT((pscc_msg_t *)event_p)) {
        ATC_LOG_E("No connection id present in response message!");
        goto exit;
    }

    conn_id = (int)PSCC_GET_CONNID((pscc_msg_t *)event_p);

    if (0 == conn_id) {
        ATC_LOG_E("conn_id == 0");
    }

exit:
    return conn_id;
}

static exe_request_record_t *get_request_record_from_event_by_connection_id(mpl_msg_event_t *event_p)
{
    exe_request_record_t *record_p = NULL;

    if (NULL == event_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }

    if (!PSCC_CONNID_PRESENT((pscc_msg_t *)event_p)) {
        ATC_LOG_E("Client tag not present in response message!");
        goto exit;
    }

    record_p = (exe_request_record_t *)((uintptr_t)PSCC_GET_CONNID((pscc_msg_t *)event_p));

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL");
    }

exit:
    return record_p;
}

static void psccclient_print_sterc_response_error(const char *prefix_string_p, pscc_result_t result)
{
    switch (result) {
    case sterc_result_ok:/* Not an error */
        break;
    case sterc_result_failed_operation_not_allowed:
        ATC_LOG_E("%s sterc_result_failed_operation_not_allowed", prefix_string_p);
        break;
    case sterc_result_failed_object_exists:
        ATC_LOG_E("%s sterc_result_failed_object_exists", prefix_string_p);
        break;
    case sterc_result_failed_object_not_found:
        ATC_LOG_E("%s sterc_result_failed_object_not_found", prefix_string_p);
        break;
    case sterc_result_failed_parameter_not_found:
        ATC_LOG_E("%s sterc_result_failed_parameter_not_found", prefix_string_p);
        break;
    case sterc_result_failed_authentication_error:
        ATC_LOG_E("%s sterc_result_failed_authentication_error", prefix_string_p);
        break;
    case sterc_result_failed_unspecified:
        ATC_LOG_E("%s sterc_result_failed_unspecified", prefix_string_p);
        break;
    case sterc_number_of_results:
        ATC_LOG_E("%s sterc_result_failed_unspecified (illegal error code)", prefix_string_p);
        break;
    default:
        ATC_LOG_E("%s Unknown sterc result code!", prefix_string_p);
        break;
    }
}


/********************************************************************
 * Public methods
 ********************************************************************
 */
/**
 * Starts a session towards the service
 * returns 0 on success, -1 on error
 *
 * Executed in the context of the queue runner thread
 */
void *psccclient_open_session()
{
    psccclient_t *psccclient_p = (psccclient_t *)malloc(sizeof(psccclient_t));
    struct sockaddr_un pscc_addr_request;
    struct sockaddr_un pscc_addr_event;
    struct sockaddr_un sterc_addr_request;
    struct sockaddr_un sterc_addr_event;
    bool sterc_init_ok = true;
    pscc_msg_t *request_p = NULL;
    char *req_buffer_p  = NULL;
    size_t temp_request_buffer_size = RESPONSE_BUFFER_SIZE;

    ATC_LOG_D("called");

    if (NULL == psccclient_p) {
        ATC_LOG_E("psccclient is NULL!");
        goto error;
    }

    /* Initialize psccclient */
    psccclient_p->pscc_fd_request = 0;
    psccclient_p->pscc_fd_event = 0;
    psccclient_p->sterc_fd_request = 0;
    psccclient_p->sterc_fd_event = 0;

    psccclient_p->pscc_ctrl_request = NULL;
    psccclient_p->pscc_ctrl_event   = NULL;
    psccclient_p->sterc_ctrl_request = NULL;
    psccclient_p->sterc_ctrl_event   = NULL;

    /* Create and connect to PS Connection Manager request socket */
    memset(&pscc_addr_request, 0, sizeof(pscc_addr_request));
    pscc_addr_request.sun_family = AF_UNIX;
    strncpy(pscc_addr_request.sun_path, REQUEST_SOCKET_PSCC, sizeof(pscc_addr_request.sun_path));
    pscc_addr_request.sun_path[sizeof(pscc_addr_request.sun_path) - 1] = '\0';
    psccclient_p->pscc_ctrl_request = stec_open((struct sockaddr *)&pscc_addr_request, sizeof(struct sockaddr_un));

    if (NULL == psccclient_p->pscc_ctrl_request) {
        ATC_LOG_E("Failed to connect to the request socket: %s", REQUEST_SOCKET_PSCC);
        goto error;
    }

    /* Create and connect to PS Connection Manager event socket */
    memset(&pscc_addr_event, 0, sizeof(pscc_addr_event));
    pscc_addr_event.sun_family = AF_UNIX;
    strncpy(pscc_addr_event.sun_path, EVENT_SOCKET_PSCC, sizeof(pscc_addr_event.sun_path));
    pscc_addr_event.sun_path[sizeof(pscc_addr_event.sun_path) - 1] = '\0';
    psccclient_p->pscc_ctrl_event = stec_open((struct sockaddr *)&pscc_addr_event, sizeof(struct sockaddr_un));

    if (NULL == psccclient_p->pscc_ctrl_event) {
        ATC_LOG_E("Failed to connect to the event socket: %s", EVENT_SOCKET_PSCC);
        goto error;
    }

    /* Create and connect to STERC request socket */
    memset(&sterc_addr_request, 0, sizeof(sterc_addr_request));
    sterc_addr_request.sun_family = AF_UNIX;
    strncpy(sterc_addr_request.sun_path, REQUEST_SOCKET_STERC, sizeof(sterc_addr_request.sun_path));
    sterc_addr_request.sun_path[ sizeof(sterc_addr_request.sun_path) - 1] = '\0';
    psccclient_p->sterc_ctrl_request = stec_open((struct sockaddr *)&sterc_addr_request, sizeof(struct sockaddr_un));

    if (NULL == psccclient_p->sterc_ctrl_request) {
        ATC_LOG_E("Failed to connect to the request socket: %s", REQUEST_SOCKET_STERC);
        sterc_init_ok = false;
    }

    /* Create and connect to STERC event socket */
    memset(&sterc_addr_event, 0, sizeof(sterc_addr_event));
    sterc_addr_event.sun_family = AF_UNIX;
    strncpy(sterc_addr_event.sun_path, EVENT_SOCKET_STERC, sizeof(sterc_addr_event.sun_path));
    sterc_addr_event.sun_path[sizeof(sterc_addr_event.sun_path) - 1] = '\0';
    psccclient_p->sterc_ctrl_event = stec_open((struct sockaddr *)&sterc_addr_event, sizeof(struct sockaddr_un));

    if (NULL == psccclient_p->sterc_ctrl_event) {
        ATC_LOG_E("Failed to connect to the event socket: %s", EVENT_SOCKET_STERC);
        sterc_init_ok = false;
    }

    /* Save file descriptors in structure (so that the function calling select() can access them) */
    psccclient_p->pscc_fd_request = psccclient_p->pscc_ctrl_request->sock;
    psccclient_p->pscc_fd_event = psccclient_p->pscc_ctrl_event->sock;

    if (sterc_init_ok) {
        psccclient_p->sterc_fd_request = psccclient_p->sterc_ctrl_request->sock;
        psccclient_p->sterc_fd_event = psccclient_p->sterc_ctrl_event->sock;
    }

    /* Initialize libmpl library through libpscc (a precondition for mpl calls) */
    if (pscc_init(NULL, pscc_log) == -1) {
        ATC_LOG_E("pscc_init failed!");
        /* This is ok if it is a restart since we only need to init mpl once */
    }

    /* Initialize libmpl library through libsterc (a precondition for mpl calls) */
    if (sterc_init_ok) {
        if (sterc_init(NULL, pscc_log) == -1) {
            ATC_LOG_E("sterc_init failed!");
            /* This is ok if it is a restart since we only need to init mpl once */
        }
    }

    /* Subscribe to events (this is a precondition for some of the RIL requests */
    if (stec_subscribe(psccclient_p->pscc_ctrl_event) != 0) {
        ATC_LOG_E("failed to activate pscc event subscription");
        goto error;
    }

    if (stec_subscribe(psccclient_p->sterc_ctrl_event) != 0) {
        ATC_LOG_E("failed to activate sterc event subscription");
        goto error;
    }

    /* Set service status */
    exe_start_set_status(EXE_START_PSCC, EXE_START_CONNECTED_READY);

    /* Register file descriptor callbacks */
    if (!selector_register_callback_for_fd(psccclient_p->pscc_fd_event, psccclient_selector_callback_event, (void *)psccclient_p)) {
        ATC_LOG_E(" selector_register_callback_for_fd, pscc_fd_event %d", psccclient_p->pscc_fd_event);
        goto error;
    }

    if (!selector_register_callback_for_fd(psccclient_p->pscc_fd_request, psccclient_selector_callback_request, (void *)psccclient_p)) {
        ATC_LOG_E("selector_register_callback_for_fd, pscc_fd_request %d", psccclient_p->pscc_fd_request);
        goto error;
    }

    ATC_LOG_I("SUCCEEDED PSCC-FD %d / %d", psccclient_p->pscc_fd_request, psccclient_p->pscc_fd_event);

    if (sterc_init_ok) {
        if (!selector_register_callback_for_fd(psccclient_p->sterc_fd_request, psccclient_selector_callback_request, (void *)psccclient_p)) {
            ATC_LOG_E("selector_register_callback_for_fd, sterc_fd_request %d", psccclient_p->sterc_fd_request);
            goto error;
        }

        if (!selector_register_callback_for_fd(psccclient_p->sterc_fd_event, psccclient_selector_callback_event, (void *)psccclient_p)) {
            ATC_LOG_E("selector_register_callback_for_fd, sterc_fd_event %d", psccclient_p->sterc_fd_event);
            goto error;
        }

        ATC_LOG_I("SUCCEEDED STERC-FD %d / %d", psccclient_p->sterc_fd_request, psccclient_p->sterc_fd_event);
    }

    /* call pscc init function */
    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("failed to allocate memory for pscc_request_p");
        goto error;
    }

    request_p->req_init_handler.id = pscc_init_handler;

    if (mpl_msg_pack((mpl_msg_t *)request_p, &req_buffer_p, &temp_request_buffer_size) != 0) {
        ATC_LOG_E("mpl_msg_pack failed!");
        goto error;
    }

    if (stec_send(psccclient_p->pscc_ctrl_request, req_buffer_p, temp_request_buffer_size) < 0) {
        ATC_LOG_E("pscc stec_send failed!");
        goto error;
    }

    mpl_msg_free((mpl_msg_t *) request_p);
    return psccclient_p;

error:

    if (NULL != request_p) {
        mpl_msg_free((mpl_msg_t *) request_p);
    }

    if (NULL != psccclient_p) {
        stec_close(psccclient_p->pscc_ctrl_request);
        stec_close(psccclient_p->pscc_ctrl_event);
        stec_close(psccclient_p->sterc_ctrl_request);
        stec_close(psccclient_p->sterc_ctrl_event);
        free(psccclient_p);
    }

    return NULL;
}

/**
 * Do service specific clean up when closing down session.
 * Mutex is locked by caller.
 */
void psccclient_close_session(void *service_p)
{
    psccclient_t *psccclient_p = (psccclient_t *)service_p;
    ATC_LOG_D("called");

    if (NULL != psccclient_p) {
        /* Unsubscribe from events */
        if (stec_unsubscribe(psccclient_p->pscc_ctrl_event) != 0) {
            ATC_LOG_E("failed to unsubscibe pscc events!");
        }

        if (stec_unsubscribe(psccclient_p->sterc_ctrl_event) != 0) {
            ATC_LOG_E("failed to unsubscibe sterc events!");
        }

        /* Release iadb */
        psccclient_iadb_remove_all();
        psccclient_user_remove_all();

        /* De-register selector callbacks */
        selector_deregister_callback_for_fd(psccclient_p->pscc_fd_request);
        selector_deregister_callback_for_fd(psccclient_p->pscc_fd_event);
        selector_deregister_callback_for_fd(psccclient_p->sterc_fd_request);
        selector_deregister_callback_for_fd(psccclient_p->sterc_fd_event);

        /* Close sockets */
        stec_close(psccclient_p->pscc_ctrl_request);
        stec_close(psccclient_p->pscc_ctrl_event);
        stec_close(psccclient_p->sterc_ctrl_request);
        stec_close(psccclient_p->sterc_ctrl_event);
        psccclient_p->pscc_ctrl_request = NULL;
        psccclient_p->pscc_ctrl_event   = NULL;
        psccclient_p->sterc_ctrl_request = NULL;
        psccclient_p->sterc_ctrl_event   = NULL;

        /* Deallocate */
        free(psccclient_p);
        psccclient_p = NULL;

        /* Set service status */
        exe_start_set_status(EXE_START_PSCC, EXE_START_NOT_CONNECTED);
    }
}


bool psccclient_send_request(exe_request_record_t *record_p, pscc_msg_t *request_p)
{
    psccclient_record_data_t *client_data_p = NULL;
    psccclient_t             *psccclient_p  = NULL;
    char                     *req_buffer_p  = NULL;
    size_t                    temp_request_buffer_size = RESPONSE_BUFFER_SIZE;
    int                       param_set_id;

    req_buffer_p = (char *) request_buffer_p; /* Get rid of annoying compiler warning ("cast increases required alignment of target type") */

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL! Failed.");
        goto error;
    }

    if (exe_start_get_status(EXE_START_PSCC) == EXE_START_NOT_CONNECTED) {
        ATC_LOG_E("psccclient not connected!");
        goto error;
    }


    if (!add_client_tag_and_connid_to_request(request_p, record_p)) {
        ATC_LOG_E("add_client_tag_and_connid_to_request failed!");
        goto error;
    }

    client_data_p = (psccclient_record_data_t *) request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data_p is NULL! Failed.");
        goto error;
    }

    psccclient_p = client_data_p->psccclient_p;

    if (NULL == psccclient_p) {
        ATC_LOG_E("psccclient_p is NULL! Failed.");
        goto error;
    }

    if (mpl_msg_pack((mpl_msg_t *)request_p, &req_buffer_p, &temp_request_buffer_size) != 0) {
        ATC_LOG_E("mpl_msg_pack failed!");
        goto error;
    }

    param_set_id = request_p->common.message_set_descr_p->param_descr_set_p->param_set_id;

    switch (param_set_id) {
    case PSCC_PARAM_SET_ID:

        if (stec_send(psccclient_p->pscc_ctrl_request, request_buffer_p, temp_request_buffer_size) < 0) {
            ATC_LOG_E("pscc stec_send failed!");
            goto disconnect;
        }

        break;
    case STERC_PARAM_SET_ID:

        if (stec_send(psccclient_p->sterc_ctrl_request, request_buffer_p, temp_request_buffer_size) < 0) {
            ATC_LOG_E("sterc stec_send failed!");
            goto error;
        }

        break;
    default:
        ATC_LOG_E("invalid param_set_id!");
        goto error;
    }

    return true;
error:
    return false;

disconnect:

    /* Since PSCCD use datagram socket this is the only way to know if the socket has been closed from the other side */
    ATC_LOG_E("********************************************************");
    ATC_LOG_E("***** at_core lost contact with pscc daemon        *****");
    ATC_LOG_E("********************************************************");

    psccclient_close_session(client_data_p->psccclient_p);

    return false;
}

exe_pscc_action_t psccclient_get_action(exe_request_record_t *record_p)
{
    exe_pscc_action_t result = EXE_PSCC_ACTION_UNDEFINED;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data_p is NULL. Failed.");
        goto exit;
    }

    return client_data_p->action;

exit:
    return result;
}


bool psccclient_set_pscc_event_p(exe_request_record_t *record_p, mpl_msg_event_t *event_p)
{
    bool                      result        = NULL;
    psccclient_record_data_t *client_data_p = NULL;


    client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("record_p is NULL. Failed.");
        goto exit;
    }

    client_data_p->pscc_event_p = event_p;
    result = true;

exit:
    return result;
}

mpl_msg_resp_t *psccclient_get_pscc_response_p(exe_request_record_t *record_p)
{
    mpl_msg_resp_t           *result        = NULL;
    psccclient_record_data_t *client_data_p = NULL;

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL. Failed.");
        goto exit;
    }

    client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("record_p is NULL. Failed.");
        goto exit;
    }

    result = client_data_p->pscc_response_p;

exit:
    return result;
}

