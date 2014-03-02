/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_api_internal.h>
#include <cops_sipc.h>
#include <cops_fd.h>
#include <cops_common.h>
#include <cops_shared_util.h>

cops_return_code_t
cops_context_create(cops_context_id_t **ctxpp,
                    const cops_event_callbacks_t *event_cbs, void *event_aux)
{
    cops_context_id_t *ctx = NULL;
    int       e;
    uint32_t  client_id;

    if (ctxpp == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory arguments NULL\n");
        goto error_return;
    }

    ctx = calloc(1, sizeof(*ctx));

    if (ctx == NULL) {
        COPS_LOG(LOG_ERROR, "calloc failed\n");
        goto error_return;
    }

    ctx->request_fd = -1;
    ctx->async_fd = -1;

    if (event_cbs != NULL) {
        ctx->event_cbs = *event_cbs;
    }

    ctx->event_aux = event_aux;

    e = cops_api_connect_request(ctx, &client_id);

    if (e != 0) {
        goto error_return;
    }

    e = cops_api_connect_async(ctx, client_id);

    if (e != 0) {
        goto error_return;
    }

    *ctxpp = ctx;
    return COPS_RC_OK;

error_return:
    cops_context_destroy(&ctx);
    return COPS_RC_IPC_ERROR;
}

cops_return_code_t
cops_context_create_async(cops_context_id_t **ctxpp,
                          const cops_event_callbacks_t *event_cbs,
                          void *event_aux,
                          const cops_async_callbacks_t *async_cbs,
                          void *async_aux)
{
    cops_context_id_t *ctx = NULL;
    int       e;
    uint32_t  client_id;

    if (ctxpp == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory arguments NULL\n");
        goto error_return;
    }

    ctx = calloc(1, sizeof(*ctx));

    if (ctx == NULL) {
        COPS_LOG(LOG_ERROR, "calloc failed\n");
        goto error_return;
    }

    ctx->request_fd = -1;
    ctx->async_fd = -1;

    if (event_cbs != NULL) {
        ctx->event_cbs = *event_cbs;
    }

    ctx->event_aux = event_aux;

    if (async_cbs != NULL) {
        ctx->async_cbs = *async_cbs;
    }

    ctx->async_aux = async_aux;

    client_id = (uint32_t)-1;
    e = cops_api_connect_async(ctx, client_id);

    if (e != 0) {
        goto error_return;
    }

    *ctxpp = ctx;
    return COPS_RC_OK;

error_return:
    cops_context_destroy(&ctx);
    return COPS_RC_IPC_ERROR;
}

void cops_context_set_async_aux(cops_context_id_t *ctxp, void *async_aux)
{
    if (ctxp != NULL) {
        ctxp->async_aux = async_aux;
    }
}

void cops_context_destroy(cops_context_id_t **ctxpp)
{
    cops_context_id_t *ctx;

    if (ctxpp == NULL || *ctxpp == NULL) {
        return;
    }

    ctx = *ctxpp;

    if (ctx->request_fd != -1) {
        (void)cops_close(ctx->request_fd);
    }

    if (ctx->async_fd != -1) {
        (void)cops_close(ctx->async_fd);
    }

    memset(ctx, 0, sizeof(*ctx));
    free(ctx);
    *ctxpp = NULL;
}

cops_return_code_t cops_context_async_fd_get(cops_context_id_t *ctxp, int *fdp)
{
    if (ctxp == NULL || fdp == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory arguments NULL\n");
        return COPS_RC_IPC_ERROR;
    }

    *fdp = ctxp->async_fd;
    return COPS_RC_OK;
}

bool cops_context_has_async(cops_context_id_t *ctxp)
{
    struct timeval tv = { 0, 0 };
    fd_set    fds;

    if (ctxp == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory argument NULL\n");
        return false;
    }

    FD_ZERO(&fds);
    FD_SET(ctxp->async_fd, &fds);

    if (select(ctxp->async_fd + 1, &fds, NULL, NULL, &tv) == -1) {
        COPS_LOG(LOG_ERROR, "select returned %s\n", strerror(errno));
        return false;
    }

    /* Normalize result */
    if (FD_ISSET(ctxp->async_fd, &fds)) {
        return true;
    } else {
        return false;
    }
}

cops_return_code_t cops_context_invoke_callback(cops_context_id_t *ctxp)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    ssize_t handled = 0;
    ssize_t res = cops_sipc_recv(ctxp->async_fd, &msg);

    /* Several events can be queued before we handle them so we have to loop */
    while (-1 != res && handled < res) {
        size_t offset = 0;
        cops_sipc_message_t *ptr =
            (cops_sipc_message_t *)((uint8_t *)msg + handled);

        if (COPS_SIPC_READ_IMEI_MRESP == ptr->msg_type) {

            if (ctxp->async_cbs.read_imei_cb != NULL) {
                cops_return_code_t rc;
                cops_imei_t imei;

                COPS_CHK_RC(cops_tapp_sipc_get_uint32(ptr, &offset,
                                                      (uint32_t *)&rc));

                if (rc == COPS_RC_OK) {
                    /* Skip 4 bytes, containing number of IMEIs and padding */
                    offset += 4;
                    COPS_CHK_RC(cops_tapp_sipc_get_opaque(ptr, &offset, &imei,
                                                          sizeof(cops_imei_t)));
                } else {
                    memset(&imei, 0, sizeof(imei));
                }
                ctxp->async_cbs.read_imei_cb(ctxp->async_aux, rc, &imei);
            }
        } else  if (COPS_SIPC_GENERIC_EVENT == ptr->msg_type) {
            uint8_t event;
            COPS_CHK_RC(cops_tapp_sipc_get_uint8(ptr, &offset, &event));

            switch (event) {
            case COPS_API_EVENT_SIMLOCK_STATUS_CHANGED:

                if (ctxp->event_cbs.simlock_status_changed_cb != NULL) {
                    cops_sim_status_t status;

                    COPS_CHK_RC(cops_tapp_sipc_get_opaque(ptr, &offset, &status,
                                                    sizeof(cops_sim_status_t)));

                    ctxp->event_cbs.simlock_status_changed_cb(ctxp->event_aux,
                                                              &status);
                }

                break;
            case COPS_API_EVENT_OTA_UNLOCK_STATUS_UPDATED:

                if (ctxp->event_cbs.simlock_status_changed_cb != NULL) {
                    cops_simlock_ota_unlock_status_t status;

                    COPS_CHK_RC(cops_tapp_sipc_get_opaque(ptr, &offset, &status,
                                     sizeof(cops_simlock_ota_unlock_status_t)));

                    ctxp->event_cbs.ota_unlock_status_updated_cb(
                                                                ctxp->event_aux,
                                                                &status);
                }

                break;
            case COPS_API_EVENT_AUTHENTICATION_CHANGED:

                if (ctxp->event_cbs.authentication_changed_cb != NULL) {
                    ctxp->event_cbs.authentication_changed_cb(ctxp->event_aux);
                }

                break;

            default:
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Ignoring event id %d\n", event);
            }
        } else {
            COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Unhandled message type\n");
        }

        handled += ptr->length;
    }

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

static cops_return_code_t
cops_api_client_get_fd(cops_context_id_t *ctxp, int *fdp, bool *is_async)
{
    if (ctxp->request_fd == -1) {
        *fdp = ctxp->async_fd;

        if (is_async == NULL) {
            return COPS_RC_ASYNC_IPC_ERROR;
        }

        *is_async = true;
    } else {
        *fdp = ctxp->request_fd;

        if (is_async != NULL) {
            *is_async = false;
        }
    }

    return COPS_RC_OK;
}

static cops_return_code_t
cops_api_client_send_and_return_ret_code(cops_context_id_t *ctxp,
                                        cops_sipc_message_t **msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int fd;
    size_t offset = 0;
    uint32_t return_code;
    uint8_t msg_type = (*msg)->msg_type + 1;

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_send(fd, *msg));
    cops_sipc_free_message(msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, msg));

    COPS_CHK_ASSERTION((*msg)->msg_type == msg_type);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(*msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    return ret_code;
}

cops_return_code_t cops_read_imei(cops_context_id_t *ctxp, cops_imei_t *imei)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    bool is_async;
    int fd;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, &is_async));

    if (!is_async) {
        COPS_CHK(imei != NULL, COPS_RC_ARGUMENT_ERROR);
    } else {
        COPS_CHK(ctxp->async_cbs.read_imei_cb != NULL, COPS_RC_ASYNC_IPC_ERROR);
    }

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_READ_IMEI_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    if (is_async) {
        COPS_SET_RC(COPS_RC_ASYNC_CALL, "Async call\n");
    }
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_READ_IMEI_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    /* Skip 4 bytes, containing the number of IMEIs and padding */
    offset += 4;
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, imei, sizeof(*imei)));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_get_nbr_of_otp_rows(cops_context_id_t *ctxp,
                                            uint32_t *nbr_of_otp_rows)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    enum cops_otp_command command = GET_NBR_OF_OTP_ROWS;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && nbr_of_otp_rows != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t), /* command */
                                        COPS_SIPC_GET_NBR_OF_OTP_ROWS_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, command));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_GET_NBR_OF_OTP_ROWS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, nbr_of_otp_rows));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_read_otp(cops_context_id_t *ctxp, uint32_t *buffer,
                                 size_t size)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    enum cops_otp_command command = READ_OTP;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && buffer != NULL && size != 0,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* size */ +
                                        sizeof(uint32_t) /* command */ +
                                        size, COPS_SIPC_READ_OTP_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, command));
    /* We assume it is ok to send size as an uint32_t. */
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, size));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, buffer, size));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_READ_OTP_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, buffer, size));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_write_otp(cops_context_id_t *ctxp,
                                  uint32_t *buffer, size_t size)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    int fd;
    enum cops_otp_command command = WRITE_OTP;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && buffer != NULL && size != 0,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* size */ +
                                        sizeof(uint32_t) /* command */ +
                                        size, COPS_SIPC_WRITE_OTP_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, command));
    /* We assume it is ok to send size as an uint32_t. */
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, size));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, buffer, size));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_WRITE_OTP_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, buffer, size));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_read_data(cops_context_id_t *ctxp, uint8_t *buf, size_t *blen)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    bool query_size = (buf == NULL);
    int fd;
    size_t offset = 0;
    uint32_t return_code;
    uint32_t length;

    COPS_CHK(ctxp != NULL && blen != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(query_size || buf != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_READ_DATA_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_READ_DATA_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));

    if (return_code != COPS_RC_OK &&
        return_code != COPS_RC_DATA_TAMPERED_ERROR) {
        COPS_SET_RC((cops_return_code_t)return_code, "Invalid return code\n");
    }

    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &length));

    if (buf != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset,
                                              (void *)buf, length));
    }

    *blen = length;

    /* if data is tampered return such error code, but always return the data */
    ret_code = (cops_return_code_t)return_code;

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_bind_properties(cops_context_id_t *ctxp, cops_bind_properties_arg_t *arg)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;
    size_t length;

    COPS_CHK(ctxp != NULL && arg != NULL, COPS_RC_ARGUMENT_ERROR);

    length = sizeof(uint8_t) +  /* Auth type */
             sizeof(uint32_t) + /* COPS data length */
             sizeof(uint8_t) +  /* Merge COPS data */
             arg->cops_data_length +
             (arg->imei != NULL ? sizeof(cops_imei_t) + 1 : 1);

    if (arg->num_new_auth_data > 0) {

        if (arg->num_new_auth_data != 1) {
            COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Invalid number of auth data");
        }

        if (arg->auth_data->auth_type == COPS_AUTH_TYPE_SIMLOCK_KEYS) {
            cops_simlock_control_keys_t simlock_keys;
            if (arg->auth_data->length != sizeof(cops_simlock_control_keys_t)) {
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Invalid length of auth data");
            }

            memcpy(&simlock_keys, arg->auth_data->data,
                   sizeof(cops_simlock_control_keys_t));

            COPS_CHK_RC(cops_util_bp_pad_control_keys(&simlock_keys));

            length += sizeof(simlock_keys);
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, length,
                                                COPS_SIPC_BIND_PROPERTIES_MREQ,
                                                COPS_SENDER_UNSECURE,
                                                &msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset,
                                           (uint8_t)arg->auth_data->auth_type));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &simlock_keys,
                                                  sizeof(simlock_keys)));

        } else if (arg->auth_data->auth_type == COPS_AUTH_TYPE_MCK) {
            cops_simlock_control_key_t master_control_key;
            if (arg->auth_data->length != sizeof(cops_simlock_control_key_t))
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Invalid length of auth data");

            memcpy(&master_control_key, arg->auth_data->data,
                   sizeof(cops_simlock_control_key_t));
            COPS_CHK_RC(cops_util_bp_pad_control_key(&master_control_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));

            length += sizeof(master_control_key);
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, length,
                                                COPS_SIPC_BIND_PROPERTIES_MREQ,
                                                COPS_SENDER_UNSECURE,
                                                &msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset,
                                          (uint8_t) arg->auth_data->auth_type));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset,
                                                  &master_control_key,
                                                  sizeof(master_control_key)));
        } else {
            COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Invalid auth type");
        }
    } else {

        COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, length,
                                            COPS_SIPC_BIND_PROPERTIES_MREQ,
                                            COPS_SENDER_UNSECURE, &msg));
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset,
                                             COPS_AUTH_TYPE_NONE));
    }

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, arg->cops_data_length));
    COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset, arg->merge_cops_data));

    if (arg->cops_data_length > 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg->cops_data,
                                              arg->cops_data_length));
    }

    if (NULL != arg->imei) {
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset, true));
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg->imei,
                                              sizeof(cops_imei_t)));
    } else {
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset, false));
    }

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_get_challenge(cops_context_id_t *ctxp, cops_auth_type_t auth_type,
                   uint8_t *buf, size_t *blen)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    bool query_size = (buf == NULL);
    int fd;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && blen != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(query_size || buf != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(auth_type == COPS_AUTH_TYPE_RSA_CHALLENGE, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_GET_CHALLANGE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_GET_CHALLANGE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

    if (buf != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, buf,
                                              COPS_CHALLENGE_SIZE));
    }

    *blen = COPS_CHALLENGE_SIZE;

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_authenticate(cops_context_id_t *ctxp, bool permanent,
                  cops_auth_data_t *auth_data)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && auth_data != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(bool) +
                                        sizeof(cops_auth_data_t) +
                                        auth_data->length,
                                        COPS_SIPC_AUTHENTICATE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));


    COPS_CHK_RC(cops_tapp_sipc_set_bool(msg, &offset, permanent));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque
                (msg, &offset, auth_data, sizeof(cops_auth_data_t)));

    if (auth_data->auth_type == COPS_AUTH_TYPE_SIMLOCK_KEYS) {
        cops_simlock_control_keys_t simlock_keys;

        memcpy(&simlock_keys, auth_data->data,
               sizeof(cops_simlock_control_keys_t));

        COPS_CHK_RC(cops_util_bp_pad_control_keys(&simlock_keys));

        COPS_CHK_RC(cops_tapp_sipc_set_opaque
                    (msg, &offset, &simlock_keys, sizeof(simlock_keys)));

    } else {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque
                    (msg, &offset, auth_data->data, auth_data->length));
    }

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_deauthenticate(cops_context_id_t *ctxp, bool permanent)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(bool),
                                        COPS_SIPC_DEAUTHENTICATE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_bool(msg, &offset, permanent));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_unlock(cops_context_id_t *ctxp,
                    cops_simlock_lock_type_t lock_type,
                    cops_simlock_control_key_t *control_key)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    cops_simlock_control_key_t key;
    int fd;
    size_t offset = 0;
    uint32_t return_code;
    uint32_t error_code;
    uint8_t len = COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH;

    COPS_CHK(ctxp != NULL && control_key != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(cops_simlock_lock_type_t) +
                                        sizeof(cops_simlock_control_key_t) + 0,
                                        COPS_SIPC_SIMLOCK_UNLOCK_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    /* Convert lock_type to uint8_t before sending as sipc-message to
     * avoid problem with enum having different sizes in different
     * occasions */
    COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset, lock_type));

    if (lock_type == COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK) {
        len = COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH;
    }

    memcpy(&key, control_key, sizeof(cops_simlock_control_key_t));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&key, len));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &key,
                                          sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_SIMLOCK_UNLOCK_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &error_code));
    COPS_CHK_RC((cops_return_code_t)error_code);

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_lock(cops_context_id_t *ctxp,
                  cops_simlock_lock_arg_t *lockarg)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;
    cops_simlock_control_key_t key;
    size_t key_offset = sizeof(uint32_t) +
        sizeof(uint32_t);
    uint32_t explicit_data_len = 0;

    COPS_CHK(ctxp != NULL && lockarg != NULL, COPS_RC_ARGUMENT_ERROR);

    /*
     * Only possible to lock to the four standard locks (NL, NSL, SP, CP) and
     * to SIM Personalization lock via the Lock command.
     */
    if (lockarg->lockdata.locktype > COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK) {
        COPS_SET_RC(COPS_RC_LOCKING_ERROR, "Lock operation not allowed for "
                "this lock type (%d)", lockarg->lockdata.locktype);
    }

    /* These parameters are checked here as incorrect data may affect
     * message encoding */
    if (lockarg->explicitlockdata != NULL) {
        if ((lockarg->explicitlockdata->length == 0 &&
             lockarg->explicitlockdata->data != NULL) ||
            (lockarg->explicitlockdata->data == NULL &&
             lockarg->explicitlockdata->length != 0)) {
            ret_code = COPS_RC_ARGUMENT_ERROR;
            goto function_exit;
        }
        explicit_data_len = lockarg->explicitlockdata->length;
    }

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                       sizeof(cops_simlock_lock_arg_t) +
                                      sizeof(cops_simlock_explicitlockdata_t) +
                                        explicit_data_len,
                                        COPS_SIPC_SIMLOCK_LOCK_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));


    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, lockarg,
                                          sizeof(cops_simlock_lock_arg_t)));

    if (lockarg->explicitlockdata != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset,
                                           lockarg->explicitlockdata,
                                     sizeof(cops_simlock_explicitlockdata_t)));

        if (explicit_data_len != 0) {
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset,
                                           lockarg->explicitlockdata->data,
                                           lockarg->explicitlockdata->length));
        }
    }

    memcpy(&key, &(lockarg->lockdata.controlkey),
           sizeof(cops_simlock_control_key_t));

    if (lockarg->lockdata.locktype == COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK) {
        COPS_CHK_RC(cops_util_bp_pad_control_key(&key,
                                       COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH));

    } else {
        COPS_CHK_RC(cops_util_bp_pad_control_key(&key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
    }

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &key_offset, &key,
                                          sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_verify_control_keys(cops_context_id_t *ctxp,
                                 cops_simlock_control_keys_t *control_keys)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    cops_simlock_control_keys_t keys;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && control_keys != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                     sizeof(cops_simlock_control_keys_t),
                                     COPS_SIPC_SIMLOCK_VERIFY_CONTROL_KEYS_MREQ,
                                     COPS_SENDER_UNSECURE, &msg));


    memcpy(&keys, control_keys,
           sizeof(cops_simlock_control_keys_t));

    COPS_CHK_RC(cops_util_bp_pad_control_keys(&keys));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &keys,
                                          sizeof(cops_simlock_control_keys_t)));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_get_status(cops_context_id_t *ctxp,
                        cops_simlock_status_t *simlock_status)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    uint32_t return_code;
    size_t offset = 0;
    cops_taf_simlock_status_t taf_status;

    COPS_CHK(ctxp != NULL && simlock_status != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_SIMLOCK_GET_STATUS_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_SIMLOCK_GET_STATUS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, &taf_status,
                                          sizeof(cops_taf_simlock_status_t)));

    /* Mark that if changing cops_simlock_status_t the correponding type
     * taf_cops_simlock_status_t also needs to be changed */
    COPS_CHK_RC(cops_util_convert_status(&taf_status, simlock_status));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_change_sim_control_key(cops_context_id_t *ctxp,
                                    cops_simlock_control_key_t *old_key,
                                    cops_simlock_control_key_t *new_key)
{
    cops_return_code_t ret_code;
    cops_simlock_control_key_t oldkey; /* old_key + padding */
    cops_simlock_control_key_t newkey; /* new_key + padding */
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && old_key != NULL && new_key != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                sizeof(cops_simlock_control_key_t) +
                sizeof(cops_simlock_control_key_t),
                COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MREQ,
                COPS_SENDER_UNSECURE, &msg));

    memcpy(&oldkey, old_key, sizeof(cops_simlock_control_key_t));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&oldkey,
                                       COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH));

    memcpy(&newkey, new_key, sizeof(cops_simlock_control_key_t));
    /* Do not pad or check for valid digits if reset key is entered */
    /*lint -e155 -e50 -e69 Compound statement is not accepted by Lint */
    if (0 != memcmp(&newkey, COPS_SIMLOCK_SIM_RESET_KEY,
                    sizeof(cops_simlock_control_key_t))) {
        COPS_CHK_RC(cops_util_bp_pad_control_key(&newkey,
                                       COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH));
    }
    /*lint +e155 +e50 +e69 */

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &oldkey,
                sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &newkey,
                sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_ota_unlock(cops_context_id_t *ctxp,
                        const uint8_t *buf, size_t blen,
                        cops_simlock_ota_reply_message_t *reply)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && buf != NULL && reply != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) + blen,
                                        COPS_SIPC_SIMLOCK_OTA_UNLOCK_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, blen));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, buf, blen));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_SIMLOCK_OTA_UNLOCK_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, reply, sizeof(*reply)));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_modem_sipc_mx(cops_context_id_t *ctxp, const uint8_t *in, size_t inlen,
                   uint8_t **out, size_t *outlen)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && in != NULL && out != NULL && outlen != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, inlen,
                                        COPS_SIPC_MODEM_SIPC_MX_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, in, inlen));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_MODEM_SIPC_MX_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    *outlen = 0;
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, (uint32_t *)outlen));

    *out = malloc(*outlen);
    COPS_CHK_ALLOC(*out);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, *out, *outlen));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

static    bool
cops_api_get_uint16(uint8_t *buf, size_t blen, size_t *offset, uint16_t *val)
{
    if ((*offset + sizeof(*val)) > blen) {
        return false;
    }

    memcpy(val, buf + *offset, sizeof(*val));
    *offset += sizeof(*val);
    return true;
}

cops_return_code_t
cops_util_find_parameter(uint8_t *buf, size_t blen, int param_id,
                         uint8_t **param_data, size_t *param_len)
{
    size_t    offset = sizeof(uint16_t) * 2;    /* skip major and minor */

    if (buf == NULL || param_data == NULL || param_len == NULL) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    while (blen != offset) {
        uint16_t  id;
        uint16_t  len;

        if (!cops_api_get_uint16(buf, blen, &offset, &id)) {
            return COPS_RC_DATA_CONFIGURATION_ERROR;
        }

        if (!cops_api_get_uint16(buf, blen, &offset, &len)) {
            return COPS_RC_DATA_CONFIGURATION_ERROR;
        }

        len -= sizeof(uint16_t) * 2;    /* id and len are already read */

        if (len + offset > blen) {
            return COPS_RC_DATA_CONFIGURATION_ERROR;
        }

        if (id == (uint16_t)param_id) {
            *param_data = buf + offset;
            *param_len = len;
            return COPS_RC_OK;
        }

        offset += len;
    }

    /* parameter ID not found */
    return COPS_RC_DATA_MISSING_ERROR;
}


cops_return_code_t
cops_get_device_state(cops_context_id_t *ctxp,
                      cops_device_state_t *device_state)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    int fd;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK(ctxp != NULL && device_state != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_GET_DEVICE_STATE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_GET_DEVICE_STATE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset,
                                          (uint32_t *)device_state));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_bind_data(cops_context_id_t *ctxp, const uint8_t *buf,
               uint32_t blen, const struct cops_digest *mac)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    uint32_t t = (size_t) buf;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && buf != NULL && mac != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* buf pointer */ +
                                        sizeof(uint32_t) /* buf length */ ,
                                        COPS_SIPC_BIND_DATA_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, t));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, blen));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_BIND_DATA_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, (void *)mac,
                                          sizeof(struct cops_digest)));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_verify_data_binding(cops_context_id_t *ctxp, const uint8_t *buf,
                         uint32_t blen, const struct cops_digest *mac)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    uint32_t t = (size_t) buf;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && buf != NULL && mac != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* buf pointer */ +
                                        sizeof(uint32_t) /* buf length */ +
                                        sizeof(*mac),
                                        COPS_SIPC_VERIFY_DATA_BINDING_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, t));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, blen));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, mac,
                                          sizeof(struct cops_digest)));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_verify_signed_header(cops_context_id_t *ctxp, const uint8_t *header,
                          enum cops_payload_type pltype,
                          struct cops_vsh *hinfo)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int fd;
    struct cops_signed_header *sh = NULL;
    size_t offset = 0;
    uint32_t return_code;
    struct cops_digest hash;

    COPS_CHK(ctxp != NULL && header != NULL && hinfo != NULL,
             COPS_RC_ARGUMENT_ERROR);

    sh = (struct cops_signed_header *)header;
    COPS_CHK_ASSERTION(sh->magic == SIGNED_HEADER_MAGIC);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* pl type */+
                                        sh->size_of_signed_header,
                                        COPS_SIPC_VERIFY_SIGNED_HEADER_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, pltype));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, header,
                                          sh->size_of_signed_header));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_VERIFY_SIGNED_HEADER_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, &hash,
                                          sizeof(struct cops_digest)));

    hinfo->hdr_size = sh->size_of_signed_header;
    hinfo->pl_size = sh->size_of_payload;
    hinfo->ht = (enum cops_hash_type)(sh->hash_type);
    hinfo->plt = (enum cops_payload_type)(sh->payload_type);
    memcpy(&hinfo->ehash, &hash, sizeof(hash));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_calcdigest(cops_context_id_t *ctxp, enum cops_hash_type ht,
                const uint8_t *data, size_t datalen,
                struct cops_digest *hash)

{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    int       fd;
    uint32_t t = (size_t) data;
    size_t offset = 0;
    cops_sipc_hashscm_t hs;
    uint32_t return_code;
    size_t hlen = 0;
    uint32_t hash_type;

    COPS_CHK(ctxp != NULL && data != NULL && hash != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_api_client_get_fd(ctxp, &fd, NULL));

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* hash type */ +
                                        sizeof(uint32_t) /* data pointer */ +
                                        sizeof(uint32_t) /* datalen */ ,
                                        COPS_SIPC_CALCULATE_HASH_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    switch (ht) {
    case COPS_HASH_TYPE_SHA1_HASH:
        hs = COPS_SIPC_HASHSCM_SHA1;
        hlen = COPS_HASH_LENGTH_SHA1;
        break;
    case COPS_HASH_TYPE_SHA256_HASH:
        hs = COPS_SIPC_HASHSCM_SHA256;
        hlen = COPS_HASH_LENGTH_SHA256;
        break;
    default:
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Unknown hashtype 0x%x\n", ht);
    }

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, hs));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, t));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, datalen));

    COPS_CHK_RC(cops_sipc_send(fd, msg));
    cops_sipc_free_message(&msg);
    COPS_CHK_RC(cops_api_block_until_ready(fd));
    COPS_CHK_ASSERTION(-1 != cops_sipc_recv(fd, &msg));

    offset = 0;
    COPS_CHK_ASSERTION(msg->msg_type == COPS_SIPC_CALCULATE_HASH_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &hash_type));
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, hash, hlen));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_simlock_verify_imsi(cops_context_id_t *ctxp,
                         const struct cops_simlock_imsi *imsi)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;
    uint8_t tmp_imsi[COPS_STORAGE_IMSI_LENGTH] = {0};

    COPS_CHK(ctxp != NULL && imsi != NULL, COPS_RC_ARGUMENT_ERROR);

    /* +1 for padding the IMSI*/
    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        COPS_STORAGE_IMSI_LENGTH + 1,
                                        COPS_SIPC_VERIFY_IMSI_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_util_unpack_imsi(imsi->data, tmp_imsi));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, tmp_imsi,
                                          sizeof(tmp_imsi)));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_lock_bootpartition(cops_context_id_t *ctxp)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_LOCK_BOOT_PART_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_init_arb_table(cops_context_id_t *ctxp,
                                       cops_init_arb_table_args_t *arg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && arg != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(cops_init_arb_table_args_t) +
                                        arg->data_length,
                                        COPS_SIPC_INIT_ARB_TABLE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg,
                                          sizeof(cops_init_arb_table_args_t)));

    if (arg->data != NULL && arg->data_length != 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg->data,
                                              arg->data_length));
    }

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_write_secprofile(cops_context_id_t *ctxp,
                                      cops_write_secprofile_args_t *arg)
{
    cops_sipc_message_t *msg = NULL;
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL && arg != NULL, COPS_RC_ARGUMENT_ERROR);

    if ((arg->hashlist != NULL && arg->hashlist_len == 0) ||
        (arg->hashlist == NULL && arg->hashlist_len != 0) ||
        (arg->data != NULL && arg->data_length == 0) ||
        (arg->data == NULL && arg->data_length != 0)) {
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto function_exit;
    }

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(cops_write_secprofile_args_t) +
                                        arg->hashlist_len +
                                        arg->data_length,
                                        COPS_SIPC_WRITE_SECPROFILE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque
                (msg, &offset, arg, sizeof(cops_write_secprofile_args_t)));

    if (arg->hashlist != NULL && arg->hashlist_len != 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg->hashlist,
                                              arg->hashlist_len));
    }

    if (arg->data != NULL && arg->data_length != 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, arg->data,
                                              arg->data_length));
    }

    ret_code = cops_api_client_send_and_return_ret_code(ctxp, &msg);

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_write_rpmb_key(cops_context_id_t *ctxp,
                                       uint16_t dev_id,
                                       bool commercial)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    size_t offset = 0;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint16_t) + sizeof(bool),
                                        COPS_SIPC_WRITE_RPMB_KEY_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint16(msg, &offset, dev_id));
    COPS_CHK_RC(cops_tapp_sipc_set_bool(msg, &offset, commercial));
    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t cops_get_product_debug_settings(cops_context_id_t *ctxp,
                                                   uint32_t *debug_settings)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;
    size_t offset = sizeof(uint32_t); /* Jump to debug data */

    COPS_CHK(ctxp != NULL && debug_settings != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                     COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MREQ,
                                     COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_api_client_send_and_return_ret_code(ctxp, &msg));

    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset,
                                          (uint32_t *)debug_settings));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}
