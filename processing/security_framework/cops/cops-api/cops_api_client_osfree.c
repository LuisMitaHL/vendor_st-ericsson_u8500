/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
/*
 * Implemented interface
 */
#include <cops.h>
#include <cops_storage.h>
/*
 * Used interfaces
 */

#include <cops_api_internal_osfree.h>
#include <cops_common.h>
#include <cops_tapp_common.h>
#include <cops_ta.h>
#include <cops_srv.h>
#include <cops_tocaccess.h>
#include <cops_shared_util.h>
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
#include <bass_app.h>
#endif
#include <string.h>

extern uint8_t run_mode;

static cops_server_callbacks_t cops_srv_cbs;

cops_return_code_t cops_context_create(cops_context_id_t **ctxpp,
                                       const cops_event_callbacks_t *event_cbs,
                                       void *event_aux)
{
    cops_return_code_t ret_code = COPS_RC_IPC_ERROR;
    cops_context_id_t *ctx = NULL;
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
    TEEC_Result teec_ret = TEEC_ERROR_ITEM_NOT_FOUND;
    TEEC_Operation op;
    TEEC_UUID *destination = NULL;
#ifdef COPS_USE_GP_TEEV0_17
    TEEC_ErrorOrigin tee_errorOrigin;
#else
    uint32_t tee_errorOrigin;
    TEEC_UUID cops_ta_uuid = {
        0x42beedc0,
        0x06ae,
        0x11e0,
        {0x85, 0x31, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}
    };
#endif
    bass_hash_t issw_ta_key_hash;
    uint32_t issw_ta_key_hash_size = 0;
    size_t i;
    uint32_t cops_ta_size = 0;
    bool found_ta = false;
#endif

    (void) event_cbs;
    (void) event_aux;

    COPS_LOG(LOG_INFO, "cops_context_create\n");

    if (ctxpp == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory argument \"cops context id\" is NULL\n");
        return COPS_RC_ARGUMENT_ERROR;
    }

    run_mode = COPS_RUN_MODE_DAEMON;

    /* Initialize the cops context */
    ctx = calloc(1, sizeof(*ctx));

    if (ctx == NULL) {
        COPS_LOG(LOG_ERROR, "Failed to allocate cops context.\n");
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    ctx->state = calloc(1, sizeof(*(ctx->state)));

    if (ctx->state == NULL) {
        COPS_LOG(LOG_ERROR, "Failed to allocate state in cops context.\n");
        free(ctx);
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    if (cops_tapp_setup(&ctx->state->tapp_io) != COPS_RC_OK) {
        COPS_LOG(LOG_ERROR, "cops_tapp_setup failed.\n");
        goto function_exit;
    }

    /* Cops data */
    ctx->state->data.data = calloc(1, ctx->state->tapp_io->data.max_length);

    if (ctx->state->data.data == NULL) {
        COPS_LOG(LOG_ERROR, "Failed to allocate cops state data.\n");
        ret_code = COPS_RC_MEMORY_ALLOCATION_ERROR;
        goto function_exit;
    }

    ctx->state->data.max_length = ctx->state->tapp_io->data.max_length;
    (void)cops_storage_read(&ctx->state->data, COPS_DATA);

    /* Perm auth state data */
    ctx->state->perm_auth_state_data.data =
        calloc(1, ctx->state->tapp_io->perm_auth_state_data.max_length);

    if (ctx->state->perm_auth_state_data.data == NULL) {
        COPS_LOG(LOG_ERROR, "Failed to allocate perm auth state data.\n");
        goto function_exit;
    }

    ctx->state->perm_auth_state_data.max_length =
        ctx->state->tapp_io->perm_auth_state_data.max_length;

    if (COPS_RC_OK != cops_storage_read(&ctx->state->perm_auth_state_data,
                                        PERM_AUTH_STATE_DATA)) {

        if (COPS_RC_OK != cops_data_init(&ctx->state->perm_auth_state_data)) {
            COPS_LOG(LOG_ERROR, "Failed to init perm auth state data.\n");
            goto function_exit;
        }
    }

#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
    teec_ret = TEEC_InitializeContext(NULL, &ctx->state->teec_sess.ctx);

    if (teec_ret != TEEC_SUCCESS) {
        COPS_LOG(LOG_ERROR, "TEEC_InitializeContext teec_ret != 0\n");
        ret_code = COPS_RC_TEEC_ERROR;
        goto function_exit;
    }

    /* Open and provide the TEEC session with the COPS TA address
     * Fill in address of TA, ignore other parameters
     * */
    memset(&ctx->state->teec_sess, 0, sizeof(TEEC_Session));
    /* Get the hash of the used ta key in the issw */
    teec_ret = get_ta_key_hash(&issw_ta_key_hash, &issw_ta_key_hash_size);
    if (teec_ret != TEEC_SUCCESS) {
        COPS_LOG(LOG_ERROR, "couldn't get ta key hash\n");
        ret_code = COPS_RC_TEEC_ERROR;
        goto function_exit;
    }

    for (i = 0; i < number_of_tas; i++) {
        cops_ta_size = cops_ta_size_list[i] - issw_ta_key_hash_size;
        if (memcmp(issw_ta_key_hash.value, &cops_ta_list[i][cops_ta_size],
                   issw_ta_key_hash_size) == 0) {
            found_ta = true;
            break;
        }
    }
    if (!found_ta) {
        COPS_LOG(LOG_ERROR, "TA not found\n");
        goto function_exit;
    }

#ifdef COPS_USE_GP_TEEV0_17
    op.memRefs[0].buffer  = (void *)cops_ta_list[i];
    op.memRefs[0].size = cops_ta_size;
    op.flags = TEEC_MEMREF_0_USED;
#else
    /* as we have already found the correct one, only register that one */
    TEEC_RegisterTA((void *)cops_ta_list[i], cops_ta_size_list[i]);
    destination = &cops_ta_uuid;
#endif

    teec_ret = TEEC_OpenSession(&ctx->state->teec_sess.ctx,
                                &ctx->state->teec_sess,
                                destination,
                                TEEC_LOGIN_PUBLIC,
                                NULL,
                                &op,
                                &tee_errorOrigin);

    if (teec_ret != TEEC_SUCCESS) {
        COPS_LOG(LOG_ERROR, "TEEC_OpenSession teec_ret != 0\n");
        ret_code = COPS_RC_TEEC_ERROR;
        goto function_exit;
    }

#endif

    if (cops_open_tochandler() != COPS_RC_OK) {
        COPS_LOG(LOG_ERROR, "Failed to open tochandler!\n");
        goto function_exit;
    }

    *ctxpp = ctx;
    cops_srv_cbs = cops_srv_common_cbs_minimal;

    COPS_LOG(LOG_INFO, "cops_context_create done\n");
    return COPS_RC_OK;

function_exit:
    cops_context_destroy(&ctx);
    COPS_LOG(LOG_ERROR, "cops_context_create done (with error)\n");
    return ret_code;
}

cops_return_code_t cops_context_create_async(cops_context_id_t **ctxpp,
        const cops_event_callbacks_t *event_cbs,
        void *event_aux,
        const cops_async_callbacks_t *async_cbs,
        void *async_aux)
{
    (void) ctxpp;
    (void) event_cbs;
    (void) event_aux;
    (void) async_cbs;
    (void) async_aux;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

void cops_context_set_async_aux(cops_context_id_t *ctxp,
                                void *async_aux)
{
    (void) ctxp;
    (void) async_aux;
}

void cops_context_destroy(cops_context_id_t **ctxpp)
{
    COPS_LOG(LOG_INFO, "cops_context_destroy\n");
    cops_context_id_t *ctx;
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
#ifdef COPS_USE_GP_TEEV0_17
    TEEC_Result teec_ret;
#endif
#endif

    if (ctxpp == NULL || *ctxpp == NULL) {
        return;
    }

    ctx = *ctxpp;

    (void)cops_close_tochandler();

#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
#ifdef COPS_USE_GP_TEEV0_17
    teec_ret = TEEC_CloseSession(&ctx->state->teec_sess);

    if (teec_ret == TEEC_SUCCESS) {
        /* Finalize the TEEC contexty and ignore error code */
        (void) TEEC_FinalizeContext(&ctx->state->teec_sess.ctx);
    }
#else
    TEEC_CloseSession(&ctx->state->teec_sess);

    (void) TEEC_FinalizeContext(&ctx->state->teec_sess.ctx);
#endif
#endif

    if (ctx->state != NULL) {
        if (ctx->state->data.data != NULL) {
            memset(ctx->state->data.data, 0, ctx->state->data.max_length);
            free(ctx->state->data.data);
            ctx->state->data.data = NULL;
        }

        memset(ctx->state, 0, sizeof(*(ctx->state)));
        free(ctx->state);
        ctx->state = NULL;
    }

    memset(ctx, 0, sizeof(*ctx));
    free(ctx);
    ctx = NULL;

    COPS_LOG(LOG_INFO, "cops_context_destroy done\n");
}

cops_return_code_t cops_context_async_fd_get(cops_context_id_t *ctxp,
        int *fdp)
{
    (void) ctxp;
    (void) fdp;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

bool cops_context_has_async(cops_context_id_t *ctxp)
{
    (void) ctxp;
    return false;
}

cops_return_code_t cops_context_invoke_callback(cops_context_id_t *ctxp)
{
    (void) ctxp;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_read_imei(cops_context_id_t *ctxp,
                                  cops_imei_t *imei)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && imei != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_READ_IMEI_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_READ_IMEI_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    /* Skip 4 bytes, containing the number of IMEIs and padding */
    offset += 4;
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, imei,
                                          sizeof(*imei)));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_get_nbr_of_otp_rows(cops_context_id_t *ctxp,
                                            uint32_t *nbr_of_otp_rows)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    enum cops_otp_command command = GET_NBR_OF_OTP_ROWS;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && nbr_of_otp_rows != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t), /* command */
                                        COPS_SIPC_GET_NBR_OF_OTP_ROWS_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset, command));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type ==
                       COPS_SIPC_GET_NBR_OF_OTP_ROWS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, nbr_of_otp_rows));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_read_otp(cops_context_id_t *ctxp,
                                 uint32_t *otp, size_t size)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    enum cops_otp_command command = READ_OTP;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && otp != NULL && size != 0,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* size */ +
                                        sizeof(uint32_t) /* command */ +
                                        size, COPS_SIPC_READ_OTP_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset, command));
    /* We assume it is ok to send size as an uint32_t. */
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset, size));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, otp, size));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_READ_OTP_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, otp, size));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_write_otp(cops_context_id_t *ctxp,
                                  uint32_t *otp, size_t size)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    enum cops_otp_command command = WRITE_OTP;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && otp != NULL && size != 0,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint32_t) /* size */ +
                                        sizeof(uint32_t) /* command */ +
                                        size, COPS_SIPC_WRITE_OTP_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset, command));
    /* We assume it is ok to send size as an uint32_t. */
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset, size));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, otp, size));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_WRITE_OTP_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, otp, size));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_read_data(cops_context_id_t *ctxp,
                                  uint8_t *buf,
                                  size_t *blen)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    bool query_size = (buf == NULL);
    size_t offset = 0;
    uint32_t return_code;
    uint32_t length;

    COPS_CHK(ctxp != NULL && blen != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(query_size || buf != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_READ_DATA_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_READ_DATA_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));

    if (return_code != COPS_RC_OK &&
        return_code != COPS_RC_DATA_TAMPERED_ERROR) {
        COPS_SET_RC((cops_return_code_t)return_code, "Invalid return code\n");
    }

    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &length));

    if (buf != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset,
                                              (void *)buf, length));
    }

    *blen = length;

    /*
     * if data was tampered return such error code, but always return the data
     */
    ret_code = (cops_return_code_t)return_code;

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_bind_properties(cops_context_id_t *ctxp,
                                        cops_bind_properties_arg_t *arg)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    size_t length;
    uint32_t return_code;

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
            if (arg->auth_data->length != sizeof(cops_simlock_control_keys_t))
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Invalid length of auth data");

            memcpy(&simlock_keys, arg->auth_data->data,
                   sizeof(cops_simlock_control_keys_t));

            COPS_CHK_RC(cops_util_bp_pad_control_keys(&simlock_keys));

            length += sizeof(simlock_keys);
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, length,
                                                COPS_SIPC_BIND_PROPERTIES_MREQ,
                                                COPS_SENDER_UNSECURE,
                                                &in_msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset,
                                          (uint8_t)arg->auth_data->auth_type));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset,
                                                  &simlock_keys,
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
                                                &in_msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset,
                                         (uint8_t) arg->auth_data->auth_type));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset,
                                                  &master_control_key,
                                                  sizeof(master_control_key)));
        } else {
            COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Invalid number of auth data");
        }
    } else {

        COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, length,
                                            COPS_SIPC_BIND_PROPERTIES_MREQ,
                                            COPS_SENDER_UNSECURE, &in_msg));
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset, 0));
    }

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(in_msg, &offset,
                                          arg->cops_data_length));
    COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset,
                                         arg->merge_cops_data));

    if (arg->cops_data_length > 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, arg->cops_data,
                                              arg->cops_data_length));
    }

    if (NULL != arg->imei) {
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset, true));
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, arg->imei,
                                              sizeof(cops_imei_t)));
    } else {
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(in_msg, &offset, false));
    }

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_BIND_PROPERTIES_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_get_challenge(cops_context_id_t *ctxp,
                                      cops_auth_type_t auth_type,
                                      uint8_t *buf,
                                      size_t *blen)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    bool query_size = (buf == NULL);
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && blen != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(query_size || buf != NULL, COPS_RC_ARGUMENT_ERROR);
    COPS_CHK(auth_type == COPS_AUTH_TYPE_RSA_CHALLENGE, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_GET_CHALLANGE_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_GET_CHALLANGE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

    if (buf != NULL) {

        COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, (void *)buf,
                                              COPS_CHALLENGE_SIZE));
    }

    *blen = COPS_CHALLENGE_SIZE;

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_authenticate(cops_context_id_t *ctxp,
                                     bool permanent,
                                     cops_auth_data_t *auth_data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && auth_data != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(bool) +
                                        sizeof(cops_auth_data_t) +
                                        auth_data->length,
                                        COPS_SIPC_AUTHENTICATE_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_bool(in_msg, &offset, permanent));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, auth_data,
                                          sizeof(cops_auth_data_t)));

    if (auth_data->auth_type == COPS_AUTH_TYPE_SIMLOCK_KEYS) {
        cops_simlock_control_keys_t simlock_keys;

        memcpy(&simlock_keys, auth_data->data,
               sizeof(cops_simlock_control_keys_t));

        COPS_CHK_RC(cops_util_bp_pad_control_keys(&simlock_keys));

        COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, &simlock_keys,
                                              sizeof(simlock_keys)));

    } else {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, auth_data->data,
                                              auth_data->length));
    }

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_AUTHENTICATE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_deauthenticate(cops_context_id_t *ctxp, bool permanent)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(bool),
                                        COPS_SIPC_DEAUTHENTICATE_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_bool(in_msg, &offset, permanent));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_DEAUTHENTICATE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_simlock_unlock(cops_context_id_t *ctxp,
                                       cops_simlock_lock_type_t lock_type,
                                       cops_simlock_control_key_t *control_key)
{
    (void) ctxp;
    (void) lock_type;
    (void) control_key;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_simlock_lock(cops_context_id_t *ctxp,
        cops_simlock_lock_arg_t *lockarg_p)
{
    (void) ctxp;
    (void) lockarg_p;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}


cops_return_code_t cops_simlock_verify_control_keys(cops_context_id_t *ctxp,
        cops_simlock_control_keys_t *control_keys)
{
    (void) ctxp;
    (void) control_keys;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_simlock_get_status(cops_context_id_t *ctxp,
        cops_simlock_status_t *simlock_status)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    uint32_t return_code;
    size_t offset = 0;
    cops_taf_simlock_status_t taf_status;

    COPS_CHK(ctxp != NULL && simlock_status != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_SIMLOCK_GET_STATUS_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_SIMLOCK_GET_STATUS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, &taf_status,
                                          sizeof(cops_taf_simlock_status_t)));

    /* Mark that if changing cops_simlock_status_t the correponding type
     * taf_cops_simlock_status_t also needs to be changed */
    COPS_CHK_RC(cops_util_convert_status(&taf_status, simlock_status));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_simlock_change_sim_control_key(cops_context_id_t *ctxp,
        cops_simlock_control_key_t *old_key,
        cops_simlock_control_key_t *new_key)
{
    cops_return_code_t ret_code;
    cops_simlock_control_key_t oldkey;
    cops_simlock_control_key_t newkey;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && old_key != NULL && new_key != NULL,
             COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                sizeof(cops_simlock_control_key_t) +
                sizeof(cops_simlock_control_key_t),
                COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MREQ,
                COPS_SENDER_UNSECURE, &in_msg));

    memcpy(&oldkey, old_key, sizeof(cops_simlock_control_key_t));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&oldkey,
                                       COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH));

    memcpy(&newkey, new_key, sizeof(cops_simlock_control_key_t));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&newkey,
                                       COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, &oldkey,
                sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, &newkey,
                sizeof(cops_simlock_control_key_t)));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type ==
                       COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_simlock_ota_unlock(cops_context_id_t *ctxp,
        const uint8_t *buf, size_t blen,
        cops_simlock_ota_reply_message_t *reply)
{
    (void) ctxp;
    (void) buf;
    (void) blen;
    (void) reply;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_modem_sipc_mx(cops_context_id_t *ctxp,
                                      const uint8_t *in,
                                      size_t inlen,
                                      uint8_t **out,
                                      size_t *outlen)
{
    (void) ctxp;
    (void) in;
    (void) inlen;
    (void) out;
    (void) outlen;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

static bool cops_api_get_uint16(uint8_t *buf,
                                size_t blen,
                                size_t *offset,
                                uint16_t *val)
{
    if ((*offset + sizeof(*val)) > blen) {
        return false;
    }

    memcpy(val, buf + *offset, sizeof(*val));
    *offset += sizeof(*val);
    return true;
}

cops_return_code_t cops_util_find_parameter(uint8_t *buf, size_t blen,
                                            int param_id, uint8_t **param_data,
                                            size_t *param_len)
{
    size_t offset = sizeof(uint16_t) * 2;    /* skip major and minor */

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

cops_return_code_t cops_get_device_state(cops_context_id_t *ctxp,
        cops_device_state_t *device_state)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && device_state != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_GET_DEVICE_STATE_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_GET_DEVICE_STATE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset,
                                          (uint32_t *)device_state));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_lock_bootpartition(cops_context_id_t *ctxp)
{
    (void) ctxp;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_init_arb_table(cops_context_id_t *ctxp,
                                       cops_init_arb_table_args_t *arg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && arg != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(cops_init_arb_table_args_t) +
                                        arg->data_length,
                                        COPS_SIPC_INIT_ARB_TABLE_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, arg,
                                          sizeof(cops_init_arb_table_args_t)));

    if (arg->data != NULL && arg->data_length != 0) {
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(in_msg, &offset, arg->data,
                                              arg->data_length));
    }

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_INIT_ARB_TABLE_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_write_secprofile(cops_context_id_t *ctxp,
                                      cops_write_secprofile_args_t *arg)
{
    (void) ctxp;
    (void) arg;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_write_rpmb_key(cops_context_id_t *ctxp,
                                       uint16_t dev_id,
                                       bool commercial)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                        sizeof(uint16_t) + sizeof(bool),
                                        COPS_SIPC_WRITE_RPMB_KEY_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint16(in_msg, &offset, dev_id));
    COPS_CHK_RC(cops_tapp_sipc_set_bool(in_msg, &offset, commercial));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    offset = 0;
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_WRITE_RPMB_KEY_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

cops_return_code_t cops_get_product_debug_settings(cops_context_id_t *ctxp,
                                                   uint32_t *debug_settings)
{
    cops_return_code_t ret_code;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    COPS_CHK(ctxp != NULL && debug_settings != NULL, COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                     COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MREQ,
                                     COPS_SENDER_UNSECURE, &in_msg));

    COPS_CHK_RC(cops_srv_cbs.msg_handle((void *)ctxp->state, in_msg, &out_msg));

    COPS_CHK_ASSERTION(out_msg->msg_type ==
                                   COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset,
                                          (uint32_t *)debug_settings));

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}
