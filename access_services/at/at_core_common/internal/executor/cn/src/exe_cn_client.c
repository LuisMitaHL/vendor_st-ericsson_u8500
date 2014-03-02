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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "atc.h"
#include "atc_log.h"
#include "atc_selector.h"
#include "exe_internal.h"
#include "exe_cn_client.h"
#include "exe_cn_events.h"
#include "exe_start.h"
#include "cn_client.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <exe_extended_cn_client.h>
#include <atc_extended_exe_glue.h>
#endif

/* The one and only C&N context */
static cn_context_t *cn_context_p = NULL;

/* Used as client tag when events are received from cn  */
#define EXE_CN_EVENT_CLIENT_TAG 0


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static void cn_client_handle_unsolicited(int cause, void *data, void *user_data);
static bool cn_client_response_callback(int fd, void *data_p);
static bool cn_client_event_callback(int fd, void *data_p);

/********************************************************************
 * Private methods
 ********************************************************************/

bool cn_client_response_callback(int fd, void *data_p)
{
    int errnum;
    cn_uint32_t unread;
    cn_message_t *msg_p = NULL;
    cn_error_code_t result = 0;
    cn_context_t *cn_context_p = (cn_context_t *)data_p;
    exe_request_record_t *request_record_p;
    exe_request_t request;

    do {
        result = cn_message_receive(fd, &unread, &msg_p);

        if (result == CN_FAILURE) {
            goto error_return;
        }

        if (result != CN_SUCCESS) {
            goto normal_return;
        }

        request_record_p = request_record_get_record_from_client_tag(msg_p->client_tag);

        if (!request_record_p) {
            ATC_LOG_E("request_record_get_record_from_client_tag failed, client_tag=%lu!",
                      msg_p->client_tag);
            goto normal_return;
        }

        request_record_set_response_data(request_record_p, (0 == msg_p->payload_size) ? NULL : (void *)msg_p->payload);
        request_record_set_resultcode(request_record_p, msg_p->error_code);

        request = request_record_get_request(request_record_p);

        if (request != NULL) {
            request(request_record_p);
        } else { /* Unsupported and unexpected signal; consume it. */
            ATC_LOG_E("%s: ERROR, Response 0x%0x Client tag data corrupt", __FUNCTION__,  msg_p->type);
        }

        free(msg_p);
        msg_p = NULL;

    } while (unread > 0);

normal_return:
    free(msg_p);
    return true;

error_return:
    ATC_LOG_E("********************************************************");
    ATC_LOG_E("***** at_core lost contact with cn daemon          *****");
    ATC_LOG_E("********************************************************");
    cn_client_close_session(cn_context_p);

    return false;
}


bool cn_client_event_callback(int fd, void *data_p)
{
    int errnum;
    cn_uint32_t unread;
    cn_message_t *msg_p = NULL;
    cn_error_code_t result = 0;
    cn_context_t *cn_context_p = (cn_context_t *)data_p;
    exe_request_record_t *request_record_p;
    exe_request_t request;
    bool event_processed = false;

    do {
        result = cn_message_receive(fd, &unread, &msg_p);

        if (result == CN_FAILURE) {
            goto error_return;
        }

        if (result != CN_SUCCESS) {
            goto normal_return;
        }

        /* Invoke handler(s) to process the event */
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
        /* Note: The extended event handler needs to return false if processing
         * is not complete and further processing is required by the generic
         * handler. Return true to prevent the generic handler from ever seeing
         * the event.
         */
        event_processed = handle_event_extended(msg_p->type, (void *)msg_p->payload);
#else
        event_processed = false;
#endif

        if (!event_processed) {
            event_processed = exe_handle_event(msg_p->type, (void *)msg_p->payload);

            if (!event_processed) {
                ATC_LOG_E("NOT IMPLEMENTED YET: Event 0x%0x in cn_client_event_callback() not processed", msg_p->type);
            }
        }

        free(msg_p);
        msg_p = NULL;

    } while (unread > 0);

normal_return:
    free(msg_p);
    return true;

error_return:
    ATC_LOG_E("********************************************************");
    ATC_LOG_E("***** at_core lost contact with cn daemon          *****");
    ATC_LOG_E("********************************************************");

    /* Do nothing. Let the request callback handle the server disconnect */

    /* Returning false will remove the fd from the selector. */
    return false;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

/**
 * Starts a session towards the service
 * returns cnclient on success, NULL on error
 */
void *cn_client_open_session()
{
    cn_error_code_t result = 0;
    int fd;

    ATC_LOG_D("%s: called", __FUNCTION__);

    /* Initialise client library */
    result = cn_client_init(&cn_context_p);

    if (CN_SUCCESS != result) {
        ATC_LOG_E("cn_client_open_session failed to init cn: result = %d!", result);
        goto error_exit;
    }

    if (!cn_context_p) {
        goto error_exit;
    }

    /* Get event socket file descriptor */
    result = cn_client_get_event_fd(cn_context_p, &fd);

    if (CN_SUCCESS != result) {
        ATC_LOG_E("cn_client_get_event_fd failed to get a fd from CN!");
        goto shutdown_exit;
    }

    selector_register_callback_for_fd(fd, cn_client_event_callback, (void *)cn_context_p);
    ATC_LOG_I("cn_client_get_event_fd done fd = %d", fd);

    /* Get request socket file descriptor */
    result = cn_client_get_request_fd(cn_context_p, &fd);

    if (CN_SUCCESS != result) {
        ATC_LOG_E("cn_client_get_request_fd failed to get a fd from CN!");
        goto shutdown_exit;
    }

    selector_register_callback_for_fd(fd, cn_client_response_callback, (void *)cn_context_p);
    ATC_LOG_I("cn_client_get_request_fd done fd = %d", fd);

    /* Set service status */
    exe_start_set_status(EXE_START_CN, EXE_START_CONNECTED_READY);

    return cn_context_p;

shutdown_exit:
    /* Failed to initialise all connections, close down. */
    cn_client_close_session(cn_context_p);

error_exit:
    return (cn_context_t *)NULL;
}


/**
 * Get client context
 */
cn_context_t *cn_client_get_context(void)
{
    return cn_context_p;
}

/**
 * Do service specific clean up when closing session.
 */
void cn_client_close_session(void *service_p)
{
    cn_context_t *cn_context_p = (cn_context_t *)service_p;
    cn_error_code_t result = 0;
    int fd;

    ATC_LOG_D("%s: called", __FUNCTION__);

    if (NULL != cn_context_p) {
        result = cn_client_get_request_fd(cn_context_p, &fd);

        if (CN_SUCCESS == result) {
            selector_deregister_callback_for_fd(fd);
        }

        result = cn_client_get_event_fd(cn_context_p, &fd);

        if (CN_SUCCESS == result) {
            selector_deregister_callback_for_fd(fd);
        }

        cn_client_shutdown(cn_context_p);
        cn_context_p = NULL;

        /* Set service status */
        exe_start_set_status(EXE_START_CN, EXE_START_NOT_CONNECTED);
        (void)atc_stop();
    }
}
