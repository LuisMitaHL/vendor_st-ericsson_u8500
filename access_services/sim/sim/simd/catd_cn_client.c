/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "sim_unused.h"
#include "simd.h"
#include "catd_cn_client.h"
#include "cn_client.h"
#include "catd.h"
#include "catd_msg.h"
#include "sim_internal.h"
#include "uiccd.h"

/* The one and only C&N context for sim/sat */
static cn_context_t *s_cn_context_p = NULL;
static pthread_mutex_t cn_ref_mutex;

/* Parameter used for determining if the CN event should
 * be dispatched to the uiccd thread */
static uint8_t uiccd_subscribe = 0;

/* Track status of the event streams */
static bool s_request_es = false;
static bool s_event_es = false;


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static ssize_t cn_client_receive_message(int fd);
void cn_client_open_session(void);
static ssize_t cn_client_response_callback(char *buf, char *buf_max, void *UNUSED(ud));
static ssize_t cn_client_event_callback(char *buf, char *buf_max, void *UNUSED(ud));

/********************************************************************
 * Private methods
 ********************************************************************/

// Event Stream callback for cn responses
static ssize_t cn_client_response_callback(char *buf, char *buf_max, void *UNUSED(ud))
{
    ssize_t rc = 0;
    catd_log_f(SIM_LOGGING_I, "cn: ENTER %s", __func__);

    /*
     * Always null for non-buf ES!
     * If this ever changes, then this code structure must be modified to work as a buffered ES.
     */
    assert(!buf_max);

    if (buf) { /* Normal operation */
        rc = cn_client_receive_message(*((int*)buf));
    } else {    // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_I, "cn : response ES is shutting down");
        s_request_es = false;
        if (s_cn_context_p && !s_event_es) {
            cn_client_shutdown(s_cn_context_p);
            s_cn_context_p = NULL;
            catd_log_f(SIM_LOGGING_E, "cn: %s completed", __func__);
        }
    }

    catd_log_f(SIM_LOGGING_I, "cn: EXIT %s", __func__);
    return rc;
}

// Event Stream callback for cn events
static ssize_t cn_client_event_callback(char *buf, char *buf_max, void *UNUSED(ud))
{
    ssize_t rc = 0;
    catd_log_f(SIM_LOGGING_I, "cn: ENTER %s", __func__);
    /*
     * Always null for non-buf ES!
     * If this ever changes, then this code structure must be modified to work as a buffered ES.
     */
    assert(!buf_max);

    if (buf) { /* Normal operation */
        rc = cn_client_receive_message(*((int*)buf));
    } else {    // ES is shutting down (as per docs for ES)
        catd_log_f(SIM_LOGGING_I, "cn : event ES is shutting down");
        s_event_es = false;
        if (s_cn_context_p && !s_request_es) {
            cn_client_shutdown(s_cn_context_p);
            s_cn_context_p = NULL;
            catd_log_f(SIM_LOGGING_E, "cn: %s completed", __func__);
        }
    }

    catd_log_f(SIM_LOGGING_I, "cn: EXIT %s", __func__);
    return rc;
}

static ssize_t cn_client_receive_message(int fd)
{
    ssize_t rc = 0;
    cn_uint32_t unread;
    cn_message_t *msg_p = NULL;
    cn_error_code_t result = CN_UNKNOWN;
    catd_log_f(SIM_LOGGING_I, "cn: ENTER %s fd=%d", __func__, fd);
    do {
        result = cn_message_receive(fd, &unread, &msg_p);
        if (result == CN_SUCCESS) {
            catd_log_f(SIM_LOGGING_I, "cn: %s : Succeeded to get message", __func__);
            catd_log_f(SIM_LOGGING_I, "cn: %s : Message type=0x%X", __func__, msg_p->type);

            if ((msg_p->type & CN_RESPONSE_UNKNOWN) == 0) {
                // Message is a CN event

                if (uiccd_subscribe) {
                    // The uiccd thread has subscribed to the event
                    cn_message_t *tmp_p = NULL;

                    // Duplicate the message and send it the the uiccd thread
                    tmp_p = malloc(CN_MESSAGE_HEADER_SIZE + msg_p->payload_size);
                    memmove(tmp_p, msg_p, CN_MESSAGE_HEADER_SIZE + msg_p->payload_size);
                    uiccd_sig_cn_event(tmp_p);
                }

                catd_sig_cn(msg_p);
            } else {
                ste_sim_ctrl_block_t* ctrl_p = (ste_sim_ctrl_block_t*)(msg_p->client_tag);

                if (ctrl_p->client_tag == CATD_CLIENT_TAG) {
                    // The response is for a CAT initiated CN request
                    catd_sig_cn(msg_p);
                } else {
                    // The response is for a uiccd initiated CN request
                    uiccd_sig_cn_response(msg_p);
                }
            }

            rc = 1;
            break;
        } else if (result == CN_FAILURE) {
            /* The CN server has disconnected. Remove the associated event stream. */
            catd_log_f(SIM_LOGGING_E, "cn : %s : Server disconnected fd=%d", __func__, fd);

            catd_rem_es(fd);
            rc = -1;
            break;
        } else if (result == CN_REQUEST_BUSY) {
            catd_log_f(SIM_LOGGING_I, "cn: %s : Waiting for complete message", __func__);
        } else {
            catd_log_f(SIM_LOGGING_E, "cn: %s : Failed to get message, result=0x%x", __func__, result);
        }
    } while (result == CN_REQUEST_BUSY);

    catd_log_f(SIM_LOGGING_I, "cn: EXIT %s", __func__);
    return rc;
}

/**
 * Starts a session towards the service and register request/response
 * and event fd to catd reader as eventstreams
 */
void cn_client_open_session()
{
    int                     ret;
    cn_error_code_t         result = 0;
    int                     fd;
    ste_es_t               *es_response = 0;
    ste_es_t               *es_event = 0;
    ste_es_parser_closure_t pc_response;
    ste_es_parser_closure_t pc_event;

    catd_log_f(SIM_LOGGING_I, "cn: ENTER %s", __func__);

    /* Initialise client library */
    result = cn_client_init(&s_cn_context_p);
    if (CN_SUCCESS != result || !s_cn_context_p) {
        catd_log_f(SIM_LOGGING_E, "cn: %s, Failed to connect to CN (result = 0x%x)", __func__, result);
        goto error_exit;
    }

    /* Get request socket file descriptor */
    result = cn_client_get_request_fd(s_cn_context_p, &fd);
    if (CN_SUCCESS != result) {
        catd_log_f(SIM_LOGGING_E, "cn: %s, Failed to get request fd from CN!", __func__);
        goto shutdown_exit;
    }

    pc_response.func = cn_client_response_callback;
    pc_response.user_data = s_cn_context_p;
    es_response = ste_es_new_nonbuf(fd, &pc_response);
    ret = catd_add_es(es_response);
    if (ret != 0) {
        catd_log_f(SIM_LOGGING_E, "cn: %s, internal failure %d", __func__, ret);
        goto shutdown_exit;
    }
    s_request_es = true;
    catd_log_f(SIM_LOGGING_I, "cn_client_get_request_fd done fd = %d", fd);


    /* Get event socket file descriptor */
    result = cn_client_get_event_fd(s_cn_context_p, &fd);
    if (CN_SUCCESS != result) {
        catd_log_f(SIM_LOGGING_E, "cn: %s, failed to get a fd from CN!", __func__);
        goto shutdown_exit;
    }

    pc_event.func = cn_client_event_callback;
    pc_event.user_data = s_cn_context_p;
    es_event = ste_es_new_nonbuf(fd, &pc_event);
    ret = catd_add_es(es_event);
    if (ret != 0) {
        catd_log_f(SIM_LOGGING_E, "cn: %s internal failure %d", __func__, ret);
        goto shutdown_exit;
    }
    s_event_es = true;
    catd_log_f(SIM_LOGGING_I, "cn_client_get_event_fd done fd = %d", fd);
    catd_log_f(SIM_LOGGING_I, "cn: EXIT %s", __func__);
    return;

shutdown_exit:
    /* Failed to initialise all connections, close down. */
    cn_client_close_session();

error_exit:
    catd_log_f(SIM_LOGGING_I, "cn: EXIT ERROR %s", __func__);
    return;
}

/********************************************************************
 * Public methods
 ********************************************************************
 */

void cn_client_close_session()
{
    int                     ret;
    cn_error_code_t         result = 0;
    int                     fd;

    catd_log_f(SIM_LOGGING_I, "cn: ENTER %s", __func__);

    if (NULL != s_cn_context_p) {
        /* Get request socket file descriptor */
        result = cn_client_get_request_fd(s_cn_context_p, &fd);
        if (CN_SUCCESS != result) {
            catd_log_f(SIM_LOGGING_E, "cn: %s, Failed to get request fd from CN!", __func__);
            goto exit;
        }

        ret = catd_rem_es(fd);
        if (ret != 0) {
            catd_log_f(SIM_LOGGING_E, "cn: %s, internal failure when remove es %d", __func__, ret);
            goto exit;
        }

        /* Get event socket file descriptor */
        result = cn_client_get_event_fd(s_cn_context_p, &fd);
        if (CN_SUCCESS != result) {
            catd_log_f(SIM_LOGGING_E, "cn: %s, failed to get event fd from CN!", __func__);
            goto exit;
        }

        ret = catd_rem_es(fd);
        if (ret != 0) {
            catd_log_f(SIM_LOGGING_E, "cn: %s, internal failure when remove es %d", __func__, ret);
            goto exit;
        }
    }

exit:
    catd_log_f(SIM_LOGGING_I, "cn: EXIT %s", __func__);
}

cn_context_t *cn_get_client()
{
    pthread_mutex_lock(&cn_ref_mutex);

    if (s_cn_context_p == NULL) {
        catd_log_f(SIM_LOGGING_I, "cn: %s CN client not previously initialized", __func__);
        cn_client_open_session();
    }

    pthread_mutex_unlock(&cn_ref_mutex);

    return s_cn_context_p;
}

void cn_uiccd_subscribe(uint8_t subscribe)
{
   uiccd_subscribe = subscribe;
}

cn_context_t *cn_get_started_client()
{
    return s_cn_context_p;
}
