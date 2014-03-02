/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: lib handling request to simpbd.
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef __i386
#include <linux/sockios.h>
#endif

#include "log.h"

#include "simpb.h"
#include "simpb_internal.h"
#include "simpbd_common.h"
#include "simpbd_socket.h"

/* internal utilities */

static int parse_messages_from_simpbd(ste_simpb_t *simpb_p)
{
    simpb_message_t *msg_p;
    size_t msg_size;
    uintptr_t decoded = 0;

    if (!simpb_p) {
        SIMPB_LOG_E("simpb_p is NULL!");
        goto error;
    }

    /* Need at least the header to know if the message is complete */
    while (SIMPB_MESSAGE_HEADER_SIZE <= (simpb_p->used - decoded)) {

        msg_p = (simpb_message_t *)(simpb_p->buf + decoded);
        msg_size = SIMPB_MESSAGE_HEADER_SIZE + msg_p->payload_size;

        /* Check that we have all of the message before processing it further */
        if (msg_size <= (simpb_p->used - decoded)) {

            switch (msg_p->request_id) {
                /* Response: Startup request */
            case STE_SIMPB_REQUEST_ID_STARTUP:
                SIMPB_LOG_D("STE_SIMPB_REQUEST_ID_STARTUP, result %d", msg_p->result);

                if (STE_SIMPB_SUCCESS == msg_p->result) {
                    simpb_p->status = STE_SIMPB_STATUS_INITIALIZED;
                } else {
                    simpb_p->status = STE_SIMPB_STATUS_CONNECTED;
                }

                break;

            case STE_SIMPB_REQUEST_ID_SHUTDOWN:
                SIMPB_LOG_D("STE_SIMPB_REQUEST_ID_SHUTDOWN, result %d", msg_p->result);

                if (STE_SIMPB_SUCCESS == msg_p->result) {
                    simpb_p->status = STE_SIMPB_STATUS_CONNECTED;
                } else {
                    simpb_p->status = STE_SIMPB_STATUS_INITIALIZED;
                }

                break;

            default:
                break;
            }

            if (simpb_p->client_cb) {
                simpb_p->client_cb(msg_p->cause, msg_p->client_tag, msg_p->request_id, msg_p->payload, msg_p->result,
                                   simpb_p->client_context);
            }

            decoded += msg_size;

        } else {
            /* Need more */
            break;
        }
    } /* while(SIMPB_MESSAGE_HEADER_SIZE <= (simpb_p->used - decoded)) */

    /* Return number of decoded bytes */
    return decoded;

error:
    /* Fatal message parsing error */
    return -1;
}


static int receive_from_simpbd(ste_simpb_t *simpb_p)
{
    int errnum, rc;
    size_t to_recv, unread;
    SIMPB_LOG_D("simpb_p=%p", simpb_p);

    if (!simpb_p) {
        SIMPB_LOG_E("simpb_p is NULL!");
        goto error;
    }

    /* Find out how many bytes are queued to be read on the socket */
    if (ioctl(simpb_p->fd, SIOCINQ, &unread) < 0) {
        /* Error handling */
        errnum = errno;
        SIMPB_LOG_E("ioctl() returns error(%d)=\"%s\".", errnum, strerror(errnum));
        goto error;
    }

    /* Always try to read at least 1 byte */
    unread = (unread > 0) ? unread : 1;

    /* Make sure we have enough buffer to hold it all */
    if ((simpb_p->used + unread) > simpb_p->size) {
        uint8_t *buf;
        buf = realloc(simpb_p->buf, (simpb_p->used + unread));

        if (buf) {
            simpb_p->buf = buf;
            simpb_p->size = simpb_p->used + unread;
        } else {
            SIMPB_LOG_E("buffer (re-)allocation failure!");
            goto error;
        }
    }

    do {
        rc = recv(simpb_p->fd, simpb_p->buf + simpb_p->used, unread, 0);

        /* recv() Successful handling */
        if (rc > 0) {
            unread -= rc;
            simpb_p->used += rc;
            /* Make call to decode and process message(s).
             * The decoder MUST process all complete messages even if
             * more than one message is present in the buffer. This is
             * because the callback is made only once per indication
             * as there is no way to determine the number of complete
             * messages in the buffer, if any.
             */
            rc = parse_messages_from_simpbd(simpb_p);

            /* Remove decoded data from buffer */
            if (rc > 0) {
                /* Partial decoding, remove decoded part */
                if (rc < (int)simpb_p->used) {
                    memmove(simpb_p->buf, (simpb_p->buf + rc),
                            (simpb_p->used - rc));
                    simpb_p->used -= rc;
                } else {
                    /* All of it was decoded */
                    simpb_p->used = 0;
                }

                return 0;
            } else if (rc < 0) {
                /* Unrecoverable error occurred, get out */
                break;
            } else {
                /* Incomplete message, wait for more */
                return 0;
            }
        }
        /* recv() Error handling */
        else if (rc < 0) {
            if (errno == EINTR) {
                /* Interrupted syscall, try again */
                continue;
            } else {
                /* Socket related error, get out */
                errnum = errno;
                SIMPB_LOG_E("recv(%d) returns error(%d)=\"%s\".", simpb_p->fd, errnum, strerror(errnum));
                break;
            }
        } else {                /* recv() rc == 0 */
            /* Orderly shutdown by peer, shut down this end */
            break;
        }
    } while (unread > 0);

error:
    /* Fatal error or socket was closed on us */
    return -1;
}


static int send_message_to_simpbd(ste_simpb_t *simpb_p, void *msg_p, size_t msg_size)
{
    ssize_t rc, to_send;
    int errnum;
    SIMPB_LOG_D("simpb_p=%p, msg_p=%p, msg_size=%d", simpb_p, msg_p, msg_size);

    if (!simpb_p) {
        SIMPB_LOG_E("simpb_p is NULL!");
        goto error;
    }

    if (!msg_p) {
        SIMPB_LOG_E("msg_p is NULL!");
        goto error;
    }

    if (1 > msg_size) {
        SIMPB_LOG_E("msg_size is zero!");
        goto error;
    }

    to_send = msg_size;

    do {
        rc = send(simpb_p->fd, msg_p, to_send, 0);

        if (rc > 0) {
            to_send -= rc;

            if (to_send < 1) {
                /* All of it sent: return success */
                return 0;
            }
        } else if (rc < 0) {
            /* Interrupted system-call: retry the operation */
            if (errno == EINTR) {
                continue;
            }
            /* All other errors: return failure */
            else {
                errnum = errno;
                SIMPB_LOG_E("send(%d) returns error(%d)=\"%s\".",
                            simpb_p->fd, errnum, strerror(errnum));
                break;
            }
        } else {
            /* Nothing was sent, socket closed?: return failure */
            SIMPB_LOG_E("send(%d) socket was closed.", simpb_p->fd);
            break;
        }
    } while (to_send > 0);

    /* If we drop out of the loop an error has occurred */
error:
    return -1;
}

/* Externally visible functions */

ste_simpb_result_t ste_simpb_connect(ste_simpb_t        **simpb_pp,
                                     int                 *fd_p,
                                     ste_simpb_status_t  *status_p,
                                     ste_simpb_cb_t       client_cb,
                                     void                *client_context)
{
    ste_simpb_t *simpb_p = NULL;
    struct sockaddr_un addr;
    int fd = -1;
    int i;
    SIMPB_LOG_D("entered");

    if (!simpb_pp) {
        SIMPB_LOG_E("ste_simpb_pp is NULL!");
        goto error_param;
    }

    *simpb_pp = NULL;

    if (!fd_p) {
        SIMPB_LOG_E("fd_p is NULL!");
        goto error_param;
    }

    *fd_p = -1;

    if (!status_p) {
        SIMPB_LOG_E("status_p is NULL!");
        goto error_param;
    }

    simpb_p = (ste_simpb_t *)calloc(1, sizeof(ste_simpb_t));

    if (!simpb_p) {
        SIMPB_LOG_E("failed to allocate memory for ste_simpb_t!");
        goto error_param;
    }

    simpb_p->client_cb = client_cb;

    /* Setup socket, get fd for it, set it non-blocking and return */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, simpbd_socket_name, sizeof(addr.sun_path));
    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0) {
        SIMPB_LOG_E("failed to create socket");
        goto error_socket;
    }

    i = connect(fd, (struct sockaddr *) &addr, sizeof(addr));

    if (0 > i) {
        SIMPB_LOG_E("failed to connect");
        goto error_connect;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);

    if (0 > i) {
        SIMPB_LOG_E("failed to set non-blocking");
        goto error_socket;
    }

    /* Copy to out arguments */
    *simpb_pp = simpb_p;
    simpb_p->fd = fd;
    simpb_p->status = STE_SIMPB_STATUS_CONNECTED;
    simpb_p->sim_state = STE_SIMPB_SIM_STATE_UNKNOWN;
    simpb_p->client_context = client_context;

    *fd_p = fd;
    *status_p = STE_SIMPB_STATUS_CONNECTED;

    return STE_SIMPB_SUCCESS;

error_connect:
    close(fd);
    free(simpb_p);
    *status_p = STE_SIMPB_STATUS_DISCONNECTED;

    return STE_SIMBP_ERROR_NOT_CONNECTED;

error_socket:

    if (fd > -1) {
        close(fd);
    }

    free(simpb_p);
    *status_p = STE_SIMPB_STATUS_DISCONNECTED;

    return STE_SIMBP_ERROR_SOCKET;

error_param:

    if (status_p) {
        *status_p = STE_SIMPB_STATUS_DISCONNECTED;
    }

    return STE_SIMPB_ERROR_PARAM;
}


ste_simpb_result_t ste_simpb_disconnect(ste_simpb_t *simpb_p)
{
    SIMPB_LOG_D("simpb_p=%p", simpb_p);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    /* Disconnect can only be done from a connected state */
    if (STE_SIMPB_STATUS_CONNECTED != simpb_p->status &&
            STE_SIMPB_STATUS_SHUTTING_DOWN != simpb_p->status) {
        return STE_SIMBP_ERROR_SEQUENCE;
    }

    if (-1 < simpb_p->fd) {
        close(simpb_p->fd);
        simpb_p->fd = -1;
    }

    simpb_p->status = STE_SIMPB_STATUS_DISCONNECTED;

    free(simpb_p);

    return STE_SIMPB_SUCCESS;
}


int ste_simpb_fd_get(ste_simpb_t *simpb_p)
{
    SIMPB_LOG_D("simpb_p=%p", simpb_p);

    if (!simpb_p) {
        goto error;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        goto error;
    }

    return simpb_p->fd;

error:
    return -1;
}


ste_simpb_status_t ste_simpb_state_get(ste_simpb_t *simpb_p)
{
    SIMPB_LOG_D("simpb_p=%p", simpb_p);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    return simpb_p->status;
}


/* receive */

ste_simpb_result_t ste_simbp_receive(ste_simpb_t *simpb_p)
{
    int result;
    SIMPB_LOG_D("simpb_p=%p", simpb_p);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    result = receive_from_simpbd(simpb_p);

    if (0 > result) {
        SIMPB_LOG_E("**** Contact lost with SIM daemon! ****");
        /* Call ste_simpb_disconnect() to clean-up */
        simpb_p->status = STE_SIMPB_STATUS_DISCONNECTED;
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    return STE_SIMPB_SUCCESS;
}


ste_simpb_result_t ste_simpb_send_request(ste_simpb_t *simpb_p, uintptr_t client_tag, simpbd_request_id_t request_id, void *data_p, size_t data_size)
{
    simpb_message_t *message_p = NULL;
    int result;
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d, request_id=%d, data_size=%d", simpb_p, client_tag, request_id, data_size);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    if (0 < data_size) {
        if (!data_p) {
            SIMPB_LOG_E("Data pointer NULL while size > 0!");
            return STE_SIMPB_ERROR_PARAM;
        }
    } else {
        if (data_p) {
            SIMPB_LOG_E("Data pointer buffer not NULL while size = 0!");
            return STE_SIMPB_ERROR_PARAM;
        }
    }

    /* Allocate and assemble message */
    message_p = (simpb_message_t *)calloc(1, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    if (!message_p) {
        SIMPB_LOG_E("Unable to allocate memory for message!");
        return STE_SIMPB_ERROR_RESOURCE;
    }

    message_p->cause        = STE_SIMPB_CAUSE_REQUEST_RESPONSE;
    message_p->client_tag   = client_tag;
    message_p->request_id   = request_id;
    message_p->result       = STE_SIMPB_SUCCESS;
    message_p->payload_size = data_size;

    if (0 < data_size) {
        memcpy(message_p->payload, data_p, data_size);
    }

    result = send_message_to_simpbd(simpb_p, message_p, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    free(message_p);

    if (0 > result) {
        SIMPB_LOG_E("**** Contact lost with SIM daemon! ****");
        /* Call ste_simpb_disconnect() to clean-up */
        simpb_p->status = STE_SIMPB_STATUS_DISCONNECTED;
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    return STE_SIMPB_SUCCESS;
}

/* -------------------------- */

ste_simpb_result_t ste_simpb_startup(ste_simpb_t *simpb_p,
                                     uintptr_t    client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    /* Startup needs to follow a connect */
    if (STE_SIMPB_STATUS_CONNECTED != simpb_p->status) {
        return STE_SIMBP_ERROR_SEQUENCE;
    }

    simpb_p->status = STE_SIMPB_STATUS_INITIALIZING;

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_STARTUP, NULL, 0);
}


ste_simpb_result_t ste_simpb_shutdown(ste_simpb_t *simpb_p,
                                      uintptr_t    client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    /* Shutdown can only be done from an initialized state */
    if (STE_SIMPB_STATUS_INITIALIZED != simpb_p->status) {
        return STE_SIMBP_ERROR_SEQUENCE;
    }

    simpb_p->status = STE_SIMPB_STATUS_SHUTTING_DOWN;

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_SHUTDOWN, NULL, 0);
}


ste_simpb_result_t ste_simpb_cache_pbr(ste_simpb_t *simpb_p, uintptr_t client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    /* Caching of PBR can only be done from an initialized state */
    if (STE_SIMPB_STATUS_INITIALIZED != simpb_p->status) {
        return STE_SIMBP_ERROR_SEQUENCE;
    }

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_CACHE_PBR, NULL, 0);
}

ste_simpb_result_t ste_simpb_update_uids(ste_simpb_t *simpb_p, uintptr_t client_tag, uint16_t file_id, uint8_t rec_num, char *file_path)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);
    ste_simpb_result_t result;
    simpb_adn_update_data_t  *update_data = NULL;
    update_data = (simpb_adn_update_data_t *) calloc(1, sizeof(simpb_adn_update_data_t));

    if (NULL == update_data) {
        SIMPB_LOG_D("Calloc failed for update_data");
        return STE_SIMPB_FAILURE;
    }

    if (!simpb_p) {
        free(update_data);
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        free(update_data);
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    update_data->file_id = file_id;
    update_data->record_num = rec_num;

    result = ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_CACHE_UPDATE_UID, (void *) update_data, sizeof(simpb_adn_update_data_t));
    free(update_data);
    return result;
}

ste_simpb_result_t ste_simpb_status_get(ste_simpb_t *simpb_p,
                                        uintptr_t    client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_STATUS_GET, NULL, 0);
}

ste_simpb_result_t ste_simpb_phonebook_select(ste_simpb_t                *simpb_p,
        uintptr_t                   client_tag,
        ste_simpb_phonebook_type_t  phonebook_type)
{
    simpbd_request_phonebook_select_t simpbd_request_phonebook_select;

    SIMPB_LOG_D("simpb_p=%p, client_tag=%d, phonebook_type=%d", simpb_p, client_tag, phonebook_type);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    if (STE_SIMPB_PHONEBOOK_TYPE_GLOBAL != phonebook_type &&
            STE_SIMPB_PHONEBOOK_TYPE_LOCAL != phonebook_type &&
            STE_SIMPB_PHONEBOOK_TYPE_DEFAULT != phonebook_type) {
        return STE_SIMPB_ERROR_PARAM;
    }

    simpbd_request_phonebook_select.selected_phonebook = phonebook_type;

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_PHONEBOOK_SELECT, &simpbd_request_phonebook_select, sizeof(simpbd_request_phonebook_select));
}


ste_simpb_result_t ste_simpb_phonebook_selected_get(ste_simpb_t                *simpb_p,
        uintptr_t                   client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_SELECTED_PHONEBOOK_GET, NULL, 0);
}

ste_simpb_result_t ste_simpb_supported_phonebooks_get(ste_simpb_t *simpb_p,
        uintptr_t    client_tag)
{
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d", simpb_p, client_tag);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_SUPPORTED_PHONEBOOKS_GET, NULL, 0);
}

ste_simpb_result_t ste_simpb_file_information_get(ste_simpb_t            *simpb_p,
        uintptr_t               client_tag,
        ste_simpb_file_info_t  *file_info_p)
{
    simpbd_request_file_information_get_t simpbd_request_file_information_get;

    SIMPB_LOG_D("simpb_p=%p, client_tag=%d, file_info_p=%p", simpb_p, client_tag, file_info_p);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (!file_info_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    simpbd_request_file_information_get.file_id = file_info_p->file_type;
    simpbd_request_file_information_get.instance = file_info_p->instance;
    simpbd_request_file_information_get.slice = file_info_p->slice;

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_FILE_INFORMATION_GET, &simpbd_request_file_information_get, sizeof(simpbd_request_file_information_get));
}

ste_simpb_result_t ste_simpb_read(ste_simpb_t           *simpb_p,
                                  uintptr_t              client_tag,
                                  ste_simpb_file_info_t *file_info_p)
{
    simpbd_request_read_t simpbd_request_read;
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d, file_info_p=%p", simpb_p, client_tag, file_info_p);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (!file_info_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    simpbd_request_read.file_id = file_info_p->file_type;
    simpbd_request_read.instance = file_info_p->instance;
    simpbd_request_read.slice = file_info_p->slice;
    simpbd_request_read.length = file_info_p->length;
    simpbd_request_read.record_id = file_info_p->record_number;
    simpbd_request_read.p2 = file_info_p->p2;

    return ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_READ, &simpbd_request_read, sizeof(simpbd_request_read));
}

ste_simpb_result_t ste_simpb_update(ste_simpb_t           *simpb_p,
                                    uintptr_t              client_tag,
                                    ste_simpb_file_info_t *file_info_p,
                                    uint8_t               *data_p,
                                    uint8_t                data_size)
{
    simpbd_request_update_t *simpbd_request_update_p;
    ste_simpb_result_t result;
    size_t size;
    SIMPB_LOG_D("simpb_p=%p, client_tag=%d, file_info_p=%p, data_p=%p, data_size=%d", simpb_p, client_tag, file_info_p, data_p, data_size);

    if (!simpb_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (!file_info_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (!data_p) {
        return STE_SIMPB_ERROR_PARAM_NULL;
    }

    if (1 > data_size) {
        return STE_SIMPB_ERROR_PARAM;
    }

    if (STE_SIMPB_STATUS_DISCONNECTED == simpb_p->status) {
        return STE_SIMBP_ERROR_NOT_CONNECTED;
    }

    size = sizeof(simpbd_request_update_t) + data_size;
    simpbd_request_update_p = malloc(size);

    if (!simpbd_request_update_p) {
        SIMPB_LOG_E("Cannot allocate memory!");
        return STE_SIMPB_ERROR_RESOURCE;
    }

    simpbd_request_update_p->file_id = file_info_p->file_type;
    simpbd_request_update_p->instance = file_info_p->instance;
    simpbd_request_update_p->slice = file_info_p->slice;
    simpbd_request_update_p->length = file_info_p->length;
    simpbd_request_update_p->record_id = file_info_p->record_number;
    simpbd_request_update_p->p2 = file_info_p->p2;
    simpbd_request_update_p->data_size = data_size;
    memcpy(simpbd_request_update_p->data, data_p, data_size);

    result = ste_simpb_send_request(simpb_p, client_tag, STE_SIMPB_REQUEST_ID_UPDATE, simpbd_request_update_p, size);

    free(simpbd_request_update_p);

    return result;
}
