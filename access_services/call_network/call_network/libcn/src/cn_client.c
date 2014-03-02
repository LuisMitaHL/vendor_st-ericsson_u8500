/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Functionality for connecting and communicating with
 * the Call and Network (CN) service.
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifndef HAVE_ANDROID_OS
#include <linux/sockios.h>
#endif

#include "cn_client.h"
#include "cn_log.h"
#include "cn_macros.h"

#include "cnsocket.h"
#include "cnserver.h"

#define CLOSED_FD -1
#define CLIENT_TAG_UNDEFINED 0

#define GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(ctx_p) if (!ctx_p) { \
  CN_LOG_E("client context is undefined!"); \
  goto error; }

struct cn_context_s { /* Complete the incomplete structure */
    int request_fd; /* Request/response socket fd */
    int event_fd; /* Event socket fd */
};

static int cn_client_connect(const char *path);


cn_error_code_t cn_message_send(int fd, cn_message_t *msg_p, size_t size)
{
    int errnum;
    ssize_t rc, to_send;
    CN_LOG_D("fd = %d, size = %d", fd, size);
    cn_error_code_t result = CN_FAILURE;


    if (!msg_p) {
        CN_LOG_E("Message buffer NULL");
        goto exit;
    }

    if (size < (sizeof(cn_message_t) + msg_p->payload_size)) {
        CN_LOG_E("Buffer size < header(%d) + payload(%d)",
                 sizeof(cn_message_t), msg_p->payload_size);
        goto exit;
    }

    to_send = size;

    do {
        rc = send(fd, msg_p, to_send, 0);
        errnum = errno;

        if (rc > 0) {
            to_send -= rc;

            if (to_send < 1) {
                /* All of it sent: return success */
                result = CN_SUCCESS;
                goto exit;
            }
        } else if (rc < 0) {
            /* Interrupted system-call: retry the operation */
            if (errnum == EINTR) {
                continue;
            }
            /* All other errors: return failure */
            else {
                CN_LOG_E("send(%d) returns error(%d)=\"%s\".",
                         fd, errnum, strerror(errnum));
                break;
            }
        } else {
            /* Nothing was sent, socket closed?: return failure */
            CN_LOG_E("send(%d) socket error: %s", fd, strerror(errnum));
            break;
        }
    } while (to_send > 0);

exit:
    return result;
}

cn_error_code_t cn_get_message_queue_size(int fd, cn_uint32_t *size_p)
{
    cn_sint32_t result = 0;
    cn_sint32_t errnum = 0;


    if (!size_p) {
        CN_LOG_E("size_p is NULL!");
        goto error;
    }

    result = ioctl(fd, SIOCINQ, (int *)size_p);

    if (result < 0) {
        errnum = errno;
        CN_LOG_W("ioctl(%d) returns error(%d)=\"%s\".", fd, errnum, strerror(errnum));
    }

    return CN_SUCCESS;

error:
    return CN_FAILURE;
}

cn_error_code_t cn_clear_message_queue(int fd)
{
    cn_uint32_t     size = 0;
    cn_error_code_t result = 0;
    cn_message_t    *msg_p = NULL;


    do {
        result = cn_get_message_queue_size(fd, &size);

        if (CN_FAILURE == result) {
            CN_LOG_E("cn_get_message_queue_size failed!");
            goto error;
        }

        if (size) {
            result = cn_message_receive(fd, &size, &msg_p);

            if (CN_FAILURE == result) {
                CN_LOG_E("cn_message_receive failed!");
                goto error;
            }

            free(msg_p);
            msg_p = NULL;
        }

    } while (size);

    return CN_SUCCESS;

error:
    return CN_FAILURE;
}


cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp)
{
    int errnum, flags, rc;
    unsigned long unread = 0;
    size_t to_recv;
    cn_message_t msg_hdr;
    cn_message_t *msg_p = NULL;


    if (!size_p) {
        CN_LOG_E("Param 'size_p' NULL");
        goto error_exit;
    }

    if (!msg_pp) {
        CN_LOG_E("Param 'msg_pp' NULL");
        goto error_exit;
    }

    /* Find out how many bytes are queued to be read on the socket */
    if (ioctl(fd, SIOCINQ, &unread) < 0) {
        /* Error handling */
        errnum = errno;
        CN_LOG_E("ioctl() returns error(%d)=\"%s\".", errnum, strerror(errnum));
        goto error_exit;
    }

    /* Read loop, 2-passes:
     * On first pass just peek at the header to find out the payload size.
     * On second pass read the entire packet (if we have it all).
     */
    /* Set things up for first pass */
    memset(&msg_hdr, 0, sizeof(msg_hdr));
    to_recv = CN_MESSAGE_HEADER_SIZE;
    msg_p = &msg_hdr;
    flags = MSG_PEEK;

read_loop:

    do {
        rc = recv(fd, msg_p, to_recv, flags);
        errnum = errno;

        if (rc > 0) {
            to_recv -= rc;
        } else if (rc < 0) {
            if (errnum == EINTR) {
                /* Interrupted syscall, try again */
                continue;
            } else {
                /* Socket related error, get out */
                CN_LOG_E("recv(%d) returns error(%d)=\"%s\".",
                         fd, errnum, strerror(errnum));
                goto error_exit;
            }
        } else {
            /* Socket was closed, get out */
            CN_LOG_E("recv() = %s", strerror(errnum));
            goto error_exit;
        }
    } while (to_recv > 0);

    if (flags == MSG_PEEK) {
        /* Header read, now read the entire packet - if it's all there */
        to_recv = CN_MESSAGE_HEADER_SIZE + msg_hdr.payload_size;

        if (unread >= to_recv) {
            /* It's all there, set things up for the second pass */
            msg_p = malloc(to_recv);

            if (!msg_p) {
                CN_LOG_E("memory allocation of %d bytes failed!", to_recv);
                goto error_exit;
            }

            flags = 0;
            goto read_loop;
        } else {
            /* The packet is not all there, wait for rest to arrive */
            CN_LOG_W("incomplete packet, got=%d, need=%d", unread, to_recv);
            goto incomplete_packet_exit;
        }
    }

    *size_p = unread - (CN_MESSAGE_HEADER_SIZE + msg_hdr.payload_size);
    *msg_pp = msg_p;

    return CN_SUCCESS;

incomplete_packet_exit:
    return CN_REQUEST_BUSY;

error_exit:
    if ((msg_p != NULL) && (msg_p != &msg_hdr)) {
        free(msg_p);
    }
    return CN_FAILURE;
}


/*************************************************************************
 *       FUNCTIONS FOR HANDLING THE CONNECTION TO THE C&N SERVICE        *
 *************************************************************************/

cn_error_code_t cn_client_init(cn_context_t **context_pp)
{

    CN_LOG_D("context_pp=%p", context_pp);

    if (!context_pp) {
        CN_LOG_E("context_pp is NULL!");
        goto error_exit;
    }

    *context_pp = calloc(1, sizeof(cn_context_t));

    if (!*context_pp) {
        CN_LOG_E("calloc failed for *context_pp!");
        goto error_exit;
    }

    (*context_pp)->request_fd = cn_client_connect(cn_request_socket_name);

    if (CLOSED_FD == (*context_pp)->request_fd) {
        CN_LOG_E("could not establish socket connection to \"%s\"", cn_request_socket_name);
        goto shutdown_exit;
    }

    (*context_pp)->event_fd = cn_client_connect(cn_event_socket_name);

    if (CLOSED_FD == (*context_pp)->event_fd) {
        CN_LOG_E("could not establish socket connection to \"%s\"", cn_event_socket_name);
        goto shutdown_exit;
    }

    CN_LOG_D("successful, context_p=%p request_fd=%d event_fd=%d",
             *context_pp, (*context_pp)->request_fd, (*context_pp)->event_fd);

    return CN_SUCCESS;

shutdown_exit:
    /* If anything went wrong all resources are freed */
    cn_client_shutdown(*context_pp);
    *context_pp = NULL;

error_exit:
    return CN_FAILURE;
}


cn_error_code_t cn_client_get_request_fd(cn_context_t *context_p, int *fd_p)
{

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!context_p) {
        CN_LOG_E("context_p is NULL!");
        goto error;
    }

    if (!fd_p) {
        CN_LOG_E("fd_p is NULL!");
        goto error;
    }

    *fd_p = context_p->request_fd;

    return CN_SUCCESS;

error:
    return CN_FAILURE;
}


cn_error_code_t cn_client_get_event_fd(cn_context_t *context_p, int *fd_p)
{

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!context_p) {
        CN_LOG_E("context_p is NULL!");
        goto error;
    }

    if (!fd_p) {
        CN_LOG_E("fd_p is NULL!");
        goto error;
    }

    *fd_p = context_p->event_fd;

    return CN_SUCCESS;

error:
    return CN_FAILURE;
}


cn_error_code_t cn_client_shutdown(cn_context_t *context_p)
{

    CN_LOG_D("context_p = %p", context_p);

    if (context_p) {
        if (context_p->request_fd != CLOSED_FD) {
            (void) close(context_p->request_fd); /* EBADF silenced case of bad fd */
            context_p->request_fd = CLOSED_FD;
        }

        if (context_p->event_fd != CLOSED_FD) {
            (void) close(context_p->event_fd);   /* EBADF silenced case of bad fd */
            context_p->event_fd = CLOSED_FD;
        }

        free(context_p);
    }

    return CN_SUCCESS;
}


/*************************************************************************
 *                           REQUEST FUNCTIONS                           *
 *************************************************************************/

cn_error_code_t cn_request_rf_on(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RF_ON;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_rf_off(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RF_OFF;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_rf_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RF_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_registration_control(cn_context_t *context_p, cn_network_access_config_data_t *config_data_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_network_access_config_data_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!config_data_p) {
        CN_LOG_E("config_data_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_network_access_config_data_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_REGISTRATION_CONTROL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_network_access_config_data_t);
    payload_p = (cn_request_network_access_config_data_t *) message_p->payload;
    payload_p->config_data = *config_data_p;
    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_reset_modem(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RESET_MODEM;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_reset_modem_with_dump(cn_context_t *context_p, cn_cpu_type_t cpu_type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_reset_cpu_type_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_reset_cpu_type_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RESET_MODEM_WITH_DUMP;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_reset_cpu_type_t);
    payload_p = (cn_request_reset_cpu_type_t *) message_p->payload;
    payload_p->type = cpu_type;


    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_preferred_network_type(cn_context_t *context_p, cn_network_type_t type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_preferred_network_type_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_preferred_network_type_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_PREFERRED_NETWORK_TYPE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_preferred_network_type_t);
    payload_p = (cn_request_set_preferred_network_type_t *) message_p->payload;
    payload_p->type = type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_preferred_network_type(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_PREFERRED_NETWORK_TYPE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_registration_state_normal(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_REGISTRATION_STATE_NORMAL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_registration_state_gprs(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_REGISTRATION_STATE_GPRS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_cell_info(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CELL_INFO;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_automatic_network_registration(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_AUTOMATIC_NETWORK_REGISTRATION;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_manual_network_registration(cn_context_t *context_p,
        cn_network_registration_req_data_t *req_data_p,
        cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_manual_network_registration_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!req_data_p) {
        CN_LOG_E("req_data_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_manual_network_registration_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_MANUAL_NETWORK_REGISTRATION;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_manual_network_registration_t);
    payload_p = (cn_request_manual_network_registration_t *) message_p->payload;

    (void)strncpy(payload_p->plmn_operator, req_data_p->operator, sizeof(payload_p->plmn_operator));
    payload_p->act = req_data_p->act;
    payload_p->format = req_data_p->format;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_manual_network_registration_with_automatic_fallback(
    cn_context_t *context_p,
    cn_network_registration_req_data_t *req_data_p,
    cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_manual_network_registration_with_automatic_fallback_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!req_data_p) {
        CN_LOG_E("req_data_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_manual_network_registration_with_automatic_fallback_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_manual_network_registration_with_automatic_fallback_t);
    payload_p = (cn_request_manual_network_registration_with_automatic_fallback_t *) message_p->payload;

    (void)strncpy(payload_p->plmn_operator, req_data_p->operator, sizeof(payload_p->plmn_operator));
    payload_p->act = req_data_p->act;
    payload_p->format = req_data_p->format;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_network_deregister(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_NETWORK_DEREGISTER;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_net_query_mode(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_NET_QUERY_MODE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_manual_network_search(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_MANUAL_NETWORK_SEARCH;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_interrupt_network_search(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_INTERRUPT_NETWORK_SEARCH;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_current_call_list(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CURRENT_CALL_LIST;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_dial(cn_context_t *context_p, cn_dial_t *dial_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_dial_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!dial_p) {
        CN_LOG_E("dial_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_dial_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    dial_p->call_type = CN_CALL_TYPE_VOICE_CALL;
    message_p->type         = CN_REQUEST_DIAL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_dial_t);
    payload_p = (cn_request_dial_t *) message_p->payload;
    payload_p->dial = *dial_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_hangup(cn_context_t *context_p, cn_call_state_filter_t filter, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_hangup_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_hangup_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_HANGUP;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_hangup_t);
    payload_p = (cn_request_hangup_t *) message_p->payload;
    payload_p->call_id      = call_id;
    payload_p->filter       = filter;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_swap_calls(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_swap_calls_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_swap_calls_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SWAP_CALLS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_swap_calls_t);
    payload_p = (cn_request_swap_calls_t *) message_p->payload;
    payload_p->active_call_id = active_call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_hold_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_hold_call_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_hold_call_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_HOLD_CALL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_hold_call_t);
    payload_p = (cn_request_hold_call_t *) message_p->payload;
    payload_p->active_call_id = active_call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_resume_call(cn_context_t *context_p, cn_uint8_t hold_call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_resume_call_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_resume_call_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RESUME_CALL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_resume_call_t);
    payload_p = (cn_request_resume_call_t *) message_p->payload;
    payload_p->hold_call_id = hold_call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_conference_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_conference_call_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_conference_call_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CONFERENCE_CALL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_conference_call_t);
    payload_p = (cn_request_conference_call_t *) message_p->payload;
    payload_p->active_call_id = active_call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_conference_call_split(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_conference_call_split_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_conference_call_split_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CONFERENCE_CALL_SPLIT;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_conference_call_split_t);
    payload_p = (cn_request_conference_call_split_t *) message_p->payload;
    payload_p->active_call_id = active_call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_explicit_call_transfer(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_explicit_call_transfer_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_explicit_call_transfer_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_EXPLICIT_CALL_TRANSFER;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_explicit_call_transfer_t);
    payload_p = (cn_request_explicit_call_transfer_t *) message_p->payload;
    payload_p->call_id = call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_answer_call(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_answer_call_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_answer_call_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_ANSWER_CALL;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_answer_call_t);
    payload_p = (cn_request_answer_call_t *) message_p->payload;
    payload_p->call_id = call_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_ussd(cn_context_t *context_p, cn_ussd_info_t *cn_ussd_info_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_ussd_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!cn_ussd_info_p) {
        CN_LOG_E(" cn_ussd_info_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_ussd_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_USSD;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_ussd_t);
    payload_p = (cn_request_ussd_t *) message_p->payload;
    payload_p->ussd_info    = *cn_ussd_info_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_ussd_abort(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_USSD_ABORT;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_signal_info_config(cn_context_t *context_p, cn_signal_info_config_t *config_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result  = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_signal_info_config_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!config_p) {
        CN_LOG_E("config_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_SIGNAL_INFO_CONFIG;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_signal_info_config_t);
    payload_p = (cn_request_set_signal_info_config_t *) message_p->payload;
    payload_p->config       = *config_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_signal_info_config(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result  = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_SIGNAL_INFO_CONFIG;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_signal_info_reporting(cn_context_t *context_p, cn_rssi_mode_t mode, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result  = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_signal_info_reporting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_SET_SIGNAL_INFO_REPORTING;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(cn_request_set_signal_info_reporting_t);
    payload_p = (cn_request_set_signal_info_reporting_t *) message_p->payload;
    payload_p->mode             = mode;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_signal_info_reporting(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result  = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_GET_SIGNAL_INFO_REPORTING;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_rssi_value(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RSSI_VALUE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_clip_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CLIP_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_cnap_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CNAP_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_colr_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p in cn_request_colr_status");
        goto error;
    }

    message_p->type         = CN_REQUEST_COLR_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_clir_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CLIR_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_clir(cn_context_t *context_p, cn_clir_setting_t clir_setting, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_clir_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_clir_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_CLIR;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_clir_t);
    payload_p = (cn_request_set_clir_t *) message_p->payload;
    payload_p->clir_setting = clir_setting;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_sleep_test_mode(cn_context_t *context_p, cn_sleep_mode_setting_t sleep_mode, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_sleep_mode_t *payload_p = NULL;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_sleep_mode_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SLEEP_TEST_MODE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_sleep_mode_t);
    payload_p = (cn_request_sleep_mode_t *) message_p->payload;
    payload_p->sleep_mode = sleep_mode;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_query_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_call_forward_info_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_call_forward_info_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_QUERY_CALL_FORWARD;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_call_forward_info_t);
    payload_p = (cn_request_call_forward_info_t *) message_p->payload;
    payload_p->call_forward_info = *call_forward_info_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_call_forward_info_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_call_forward_info_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_CALL_FORWARD;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_call_forward_info_t);
    payload_p = (cn_request_call_forward_info_t *) message_p->payload;
    payload_p->call_forward_info = *call_forward_info_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_hsxpa_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_SET_HSXPA_MODE;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_hsxpa_t *) message_p->payload;
    payload_p->hsxpa_mode = hsxpa_mode;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_HSXPA_MODE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_set_modem_property(cn_context_t *context_p, cn_modem_property_t modem_property, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_modem_property_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_modem_property_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_MODEM_PROPERTY;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_modem_property_t);
    payload_p = (cn_request_set_modem_property_t *) message_p->payload;
    payload_p->modem_property = modem_property;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_modem_property(cn_context_t *context_p, cn_modem_property_type_t type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_modem_property_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_MODEM_PROPERTY;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_get_modem_property_t);
    payload_p = (cn_request_get_modem_property_t *) message_p->payload;
    payload_p->type = type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_cssn(cn_context_t *context_p, cn_cssn_setting_t cssn_setting, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_cssn_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_cssn_setting_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_CSSN;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_cssn_t);
    payload_p = (cn_request_set_cssn_t *)message_p->payload;
    payload_p->cssn_setting = cssn_setting;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_dtmf_send(cn_context_t *context_p,
                                     char *dtmf_string_p,
                                     cn_uint16_t length,
                                     cn_dtmf_string_type_t string_type,
                                     cn_uint16_t dtmf_duration_time,
                                     cn_uint16_t dtmf_pause_time,
                                     cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_dtmf_send_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (NULL == dtmf_string_p) {
        CN_LOG_E("cn_request_dtmf_send received NULL dtmf string");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_dtmf_send_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_DTMF_SEND;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_dtmf_send_t);
    payload_p = (cn_request_dtmf_send_t *) message_p->payload;
    payload_p->dtmf_string.string_type = string_type;
    payload_p->dtmf_string.duration_time = dtmf_duration_time;
    payload_p->dtmf_string.pause_time = dtmf_pause_time;

    switch (string_type) {
    case CN_DTMF_STRING_TYPE_ASCII:

        if (sizeof(payload_p->dtmf_string.dtmf_string) < length) {
            CN_LOG_E("dtmf string too long, will be truncated");
            length = sizeof(payload_p->dtmf_string.dtmf_string) - 1;
        }

        memcpy(payload_p->dtmf_string.dtmf_string, dtmf_string_p, length);
        payload_p->dtmf_string.length = length;
        break;
    case CN_DTMF_STRING_TYPE_BCD:

        if (sizeof(payload_p->dtmf_string.dtmf_string) < length / 2) {
            CN_LOG_E("dtmf string too long, will be truncated");
            length = (sizeof(payload_p->dtmf_string.dtmf_string) - 1) * 2;
        }

        memcpy(payload_p->dtmf_string.dtmf_string, dtmf_string_p, length / 2);
        payload_p->dtmf_string.length = length;
        break;
    default:
        CN_LOG_E("Unknown dtmf string type");
        goto error_free;
    }

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_dtmf_start(cn_context_t *context_p, char character, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_dtmf_start_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_dtmf_start_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_DTMF_START;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_dtmf_start_t);
    payload_p = (cn_request_dtmf_start_t *) message_p->payload;
    payload_p->character = character;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_dtmf_stop(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_DTMF_STOP;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_call_waiting(cn_context_t *context_p, cn_call_waiting_t *call_waiting_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_call_waiting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!call_waiting_p) {
        CN_LOG_E("call_waiting_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(cn_request_set_call_waiting_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_CALL_WAITING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_call_waiting_t);
    payload_p = (cn_request_set_call_waiting_t *) message_p->payload;
    payload_p->call_waiting = *call_waiting_p;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_call_waiting(cn_context_t *context_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_call_waiting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_get_call_waiting_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_CALL_WAITING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_get_call_waiting_t);
    payload_p = (cn_request_get_call_waiting_t *) message_p->payload;
    payload_p->service_class = service_class;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_change_barring_password(cn_context_t *context_p, char *facility_p, char *old_passwd_p, char *new_passwd_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_change_barring_password_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_change_barring_password_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_CHANGE_BARRING_PASSWORD;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_change_barring_password_t);
    payload_p = (cn_request_change_barring_password_t *) message_p->payload;
    (void)strncpy(payload_p->facility, facility_p, CN_MAX_STRING_SIZE);
    (void)strncpy(payload_p->old_passwd, old_passwd_p, CN_MAX_STRING_SIZE);
    (void)strncpy(payload_p->new_passwd, new_passwd_p, CN_MAX_STRING_SIZE);

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_query_call_barring(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_query_call_barring_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_query_call_barring_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_QUERY_CALL_BARRING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_query_call_barring_t);
    payload_p = (cn_request_query_call_barring_t *) message_p->payload;
    (void)strncpy(payload_p->facility, facility_p, CN_MAX_STRING_SIZE);
    payload_p->service_class = service_class;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_call_barring(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_call_barring_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_call_barring_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_CALL_BARRING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_call_barring_t);
    payload_p = (cn_request_set_call_barring_t *) message_p->payload;
    (void)strncpy(payload_p->facility, facility_p, CN_MAX_STRING_SIZE);
    payload_p->call_barring = *call_barring_p;
    (void)strncpy(payload_p->passwd, passwd_p, CN_MAX_STRING_SIZE);

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_product_profile_flag(cn_context_t *context_p, cn_uint16_t flag_id, cn_uint16_t flag_value, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_product_profile_flag_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_product_profile_flag_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_PRODUCT_PROFILE_FLAG;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_product_profile_flag_t);
    payload_p = (cn_request_set_product_profile_flag_t *) message_p->payload;
    payload_p->flag_id      = flag_id;
    payload_p->flag_value   = flag_value;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_set_l1_parameter(cn_context_t *context_p, cn_uint8_t command, cn_uint16_t parameter, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_l1_parameter_t *payload_p = NULL;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_l1_parameter_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_L1_PARAMETER;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_l1_parameter_t);
    payload_p = (cn_request_set_l1_parameter_t *) message_p->payload;
    payload_p->command      = command;
    payload_p->parameter    = parameter;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);
    return CN_SUCCESS;

error_free:
    free(message_p);
error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_set_user_activity_status(cn_context_t *context_p, cn_user_status_t user_status, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_user_activity_status_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_user_activity_status_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_USER_ACTIVITY_STATUS;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_user_activity_status_t);
    payload_p = (cn_request_set_user_activity_status_t *) message_p->payload;
    payload_p->user_status  = user_status;
    payload_p->battery_status = CN_BATTERY_STATUS_UNKNOWN;
    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_neighbour_cells_reporting(cn_context_t *context_p, cn_bool_t enable_reporting, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_neighbour_cells_reporting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_set_neighbour_cells_reporting_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_NEIGHBOUR_CELLS_REPORTING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_set_neighbour_cells_reporting_t);
    payload_p = (cn_request_set_neighbour_cells_reporting_t *) message_p->payload;
    payload_p->enable_reporting = enable_reporting;
    payload_p->rat_type         = rat_type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_neighbour_cells_reporting(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_neighbour_cells_reporting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_get_neighbour_cells_reporting_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_get_neighbour_cells_reporting_t);
    payload_p = (cn_request_get_neighbour_cells_reporting_t *) message_p->payload;
    payload_p->rat_type     = rat_type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_neighbour_cells_complete_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_neighbour_cells_info_t *payload_p = NULL;
    CN_LOG_D("Entering cn_request_get_neighbour_cells_complete_info");


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_get_neighbour_cells_info_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_get_neighbour_cells_info_t);
    payload_p = (cn_request_get_neighbour_cells_info_t *) message_p->payload;
    payload_p->rat_type     = rat_type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_get_neighbour_cells_extd_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_neighbour_cells_info_t *payload_p = NULL;
    CN_LOG_D("Entering cn_request_get_neighbour_cells_extd_info");


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(cn_request_get_neighbour_cells_info_t);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_GET_NEIGHBOUR_CELLS_EXTD_INFO;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_get_neighbour_cells_info_t);
    payload_p = (cn_request_get_neighbour_cells_info_t *) message_p->payload;
    payload_p->rat_type     = rat_type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_set_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_bool_t enable_reporting, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_set_event_reporting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_SET_EVENT_REPORTING;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_set_event_reporting_t *) message_p->payload;
    payload_p->type             = type;
    payload_p->enable_reporting = enable_reporting;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_get_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_event_reporting_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_GET_EVENT_REPORTING;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_get_event_reporting_t *) message_p->payload;
    payload_p->type             = type;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_rab_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_GET_RAB_STATUS;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_baseband_version(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_BASEBAND_VERSION;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_get_pp_flags(cn_context_t *context_p, cn_bool_t all_flags,
                                        cn_uint16_t flag_id, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_get_pp_flags_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_GET_PP_FLAGS;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_get_pp_flags_t *) message_p->payload;
    payload_p->read_all_flags   = all_flags;
    payload_p->flag_id          = flag_id;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_modify_emergency_number_list(cn_context_t *context_p, cn_emergency_number_operation_t operation, cn_emergency_number_config_t *config_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_modify_emergency_number_list_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!config_p && CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST != operation) {
        CN_LOG_E("config_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_MODIFY_EMERGENCY_NUMBER_LIST;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_modify_emergency_number_list_t *) message_p->payload;
    payload_p->operation        = operation;

    if (config_p) {
        payload_p->config       = *config_p;
    }

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_get_emergency_number_list(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_GET_EMERGENCY_NUMBER_LIST;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_request_nmr_info(cn_context_t *context_p, cn_nmr_rat_type_t rat, cn_nmr_utran_type_t utran, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_nmr_info_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_NMR_INFO;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_nmr_info_t *) message_p->payload;
    payload_p->rat = rat;
    payload_p->utran = utran;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}

cn_error_code_t cn_get_timing_advance_value(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_TIMING_ADVANCE;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_ss(cn_context_t *context_p, cn_ss_command_t ss_command, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_ss_command_t *payload_p = NULL;


    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_SS;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_ss_command_t *) message_p->payload;
    payload_p->ss_command = ss_command;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_reg_status_event_config(cn_context_t *context_p, cn_reg_status_trigger_level_t trigger_level, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_reg_status_event_config_t *payload_p = NULL;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_REG_STATUS_EVENT_CONFIG;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_reg_status_event_config_t *) message_p->payload;
    payload_p->trigger_level    = trigger_level;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_rat_name(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_RAT_NAME;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_modem_power_off(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_MODEM_POWER_OFF;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_send_tx_back_off_event(cn_context_t *context_p, cn_tx_back_off_event_t event, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;
    cn_request_send_tx_back_off_event_t *payload_p = NULL;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type             = CN_REQUEST_SEND_TX_BACK_OFF_EVENT;
    message_p->client_tag       = client_tag;
    message_p->payload_size     = sizeof(*payload_p);
    payload_p = (cn_request_send_tx_back_off_event_t *) message_p->payload;
    payload_p->event            = event;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


cn_error_code_t cn_request_set_default_nvmd(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    cn_message_t *message_p = NULL;
    cn_error_code_t result = CN_FAILURE;
    cn_uint32_t size = 0;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    size = sizeof(*message_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_DEFAULT_NVMD;
    message_p->client_tag   = client_tag;
    message_p->payload_size = 0;

    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);

    return CN_SUCCESS;

error_free:
    free(message_p);

error:
    return CN_FAILURE;
}


/*** PRIVATE FUNCTIONS ***/

static int cn_client_connect(const char *path)
{
    struct sockaddr_un      addr;
    int                     i;
    int                     fd;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
        CN_LOG_E("failed to create socket");
        return CLOSED_FD;
    }

    i = connect(fd, (struct sockaddr *) &addr, sizeof(addr));

    if (i < 0) {
        CN_LOG_E("failed to connect");
        close(fd);
        return CLOSED_FD;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);

    if (i < 0) {
        CN_LOG_E("failed to set non-blocking");
        close(fd);
        return CLOSED_FD;
    }

    return fd;
}

cn_error_code_t cn_request_set_empage(cn_context_t *context_p, cn_empage_t *empage_p , cn_client_tag_t client_tag)
{
    cn_message_t                    *message_p = NULL;
    cn_error_code_t                  result    = CN_FAILURE;
    cn_uint32_t                      size      = 0;
    cn_request_empage_config_data_t *payload_p = NULL;

    GOTO_ERROR_IF_CONTEXT_TAG_IS_UNDEFINED(context_p);

    if (!empage_p) {
        CN_LOG_E("empage_p is NULL!");
        goto error;
    }

    size = sizeof(*message_p) + sizeof(*payload_p);
    message_p = calloc(1, size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = CN_REQUEST_SET_EMPAGE;
    message_p->client_tag   = client_tag;
    message_p->payload_size = sizeof(cn_request_empage_config_data_t);
    payload_p = (cn_request_empage_config_data_t *) message_p->payload;
    payload_p->config_data = *empage_p;
    result = cn_message_send(context_p->request_fd, message_p, size);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_send failed! (fd: %d)", context_p->request_fd);
        goto error_free;
    }

    free(message_p);
    return CN_SUCCESS;

error_free:
    free(message_p);
error:
    return CN_FAILURE;
}
