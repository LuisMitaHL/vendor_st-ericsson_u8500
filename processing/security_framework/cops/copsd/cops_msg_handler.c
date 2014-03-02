/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#include <cops_msg_handler.h>
#include <cops_router.h>
#include <cops_bootpartitionhandler.h>
#include <cops_tocaccess.h>
#include <cops_tapp_common.h>
#include <cops_common.h>
#include <cops_api_internal.h>
#include <cops_shared_util.h>
#ifdef COPS_OSE_ENVIRONMENT
#include <strings.h>
#endif

extern uint8_t run_mode;

typedef struct {
    uint8_t key[SIMLOCK_OTA_SIZE_OF_UNLOCK_KEY];
} cops_ota_key_t;

typedef struct {
    uint8_t not_used[40];
    /* Order: Network, NetworkSubset, ServiceProvicer, Coporate */
    cops_ota_key_t unlock_key[4];
    uint8_t imei[SIMLOCK_OTA_SIZE_OF_IMEI];
    /* Order: Network, NetworkSubset, ServiceProvicer, Coporate */
    cops_ota_key_t unlock_key_extension[4];
} cops_ota_message_t;

static cops_return_code_t cops_msg_handle_modem(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg);

static cops_return_code_t cops_msg_handle_relay(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg);

static cops_return_code_t cops_msg_handle_daemon(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg);

static cops_return_code_t cops_msg_get_taf_id(cops_sipc_message_t *msg,
                                              cops_taf_id_t *taf_id);

static cops_return_code_t cops_msg_postinvoke(struct cops_state *state,
                                              cops_sipc_message_t **msg);

static cops_return_code_t cops_msg_update_data(struct cops_state *state,
                                               cops_sipc_message_t *msg);

static cops_return_code_t cops_msg_preinvoke(struct cops_state *state,
                                             cops_sipc_message_t *in_msg,
                                             cops_sipc_message_t **out_msg);

static cops_return_code_t cops_msg_ota_unlock_perform_unlocking(
                           struct cops_state *state,
                           const cops_ota_message_t *ota_message,
                           cops_simlock_ota_reply_message_t *ota_reply_message);

#ifndef COPS_IN_LOADERS
static cops_return_code_t cops_msg_create_ota_status(
                                       cops_simlock_ota_unlock_status_t *status,
                                       cops_simlock_ota_reply_message_t *msg);
#endif

static cops_return_code_t cops_msg_bind_properties(struct cops_state *state,
                                                   cops_sipc_message_t *msg);

static cops_return_code_t cops_msg_update_arb_tables(cops_sipc_message_t *msg,
                                                     size_t offset);

static cops_return_code_t cops_msg_ota_unlock(struct cops_state *state,
                                       cops_sipc_message_t *msg,
                                       cops_simlock_ota_reply_message_t *reply);

static cops_return_code_t cops_msg_ota_unlock_check_and_load_imei(
                            struct cops_state *state,
                            const cops_ota_message_t *ota_message,
                            cops_simlock_ota_reply_message_t *ota_reply_message,
                            bool *match);

static cops_return_code_t cops_msg_ota_unlock_set_current_lock_mode(
                           struct cops_state *state,
                           cops_simlock_ota_reply_message_t *ota_reply_message);


/*
 * A message sent to this function should result in a out_msg being created.
 * If a correct out_msg cannot be produced to the receiver
 * this should be considered fatal. All other errors are NOT fatal.
 */
cops_return_code_t cops_msg_handle(struct cops_state *state,
                                   cops_sipc_message_t *in_msg,
                                   cops_sipc_message_t **out_msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *temp_out_msg = NULL;

    /* IPC_STATE should bypass tapp_invoke */
    if (COPS_SIPC_IPC_STATE_REQ == in_msg->msg_type ||
        COPS_SIPC_IPC_STATE_RESP == in_msg->msg_type) {
        temp_out_msg = in_msg;
        goto function_exit;
    }

    /* Handle message depending on run mode */
    if (COPS_RUN_MODE_MODEM == run_mode) {
        COPS_CHK_RC(cops_msg_handle_modem(state, in_msg, &temp_out_msg));
    } else if (COPS_RUN_MODE_RELAY == run_mode) {
        COPS_CHK_RC(cops_msg_handle_relay(state, in_msg, &temp_out_msg));
    } else if (COPS_RUN_MODE_DAEMON == run_mode) {
        COPS_CHK_RC(cops_msg_handle_daemon(state, in_msg, &temp_out_msg));
    }

    /* Sanity check */
    COPS_CHK_ASSERTION(!(NULL == temp_out_msg && COPS_RC_OK == ret_code));

function_exit:
    /* Assign out_msg if applicable */
    if (NULL != temp_out_msg && NULL != out_msg) {
        *out_msg = temp_out_msg;
    }

    return ret_code;
}

static cops_return_code_t cops_msg_handle_modem(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = in_msg;

    COPS_CHK_RC(cops_tapp_init_io(state, state->tapp_io));

    /* Received a special trigger message */
    if (COPS_SIPC_TRIGGER_MSG == msg->msg_type) {
        size_t offset = 0;
        uint8_t trigger_msg;
        COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset, &trigger_msg));

        /* We are requested to perform a new secure verify imsi */
        if (COPS_TRIGGER_VERIFY_IMSI_REQUEST == trigger_msg) {
            msg = NULL;
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                                COPS_SIPC_VERIFY_IMSI_MREQ,
                                                COPS_SENDER_UNSECURE, &msg));
        } else {
            *out_msg = msg;
            goto function_exit;
        }
    }

    /* Run invoke */
    COPS_CHK_RC(cops_data_set_sipc_message(&state->tapp_io->args, msg));
    COPS_CHK_RC(cops_tapp_invoke(state, &state->tapp_io, (cops_taf_id_t)0));
    COPS_CHK_RC(cops_data_get_sipc_message(&state->tapp_io->rvs, out_msg));

function_exit:
    if (msg != in_msg) {
        cops_sipc_free_message(&msg);
    }

    return ret_code;
}

static cops_return_code_t cops_msg_handle_relay(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_taf_id_t taf_id = COPS_SERVICE_NUM_SERVICES;

    /* Trigger messages should bypass tapp_invoke */
    if (COPS_SIPC_TRIGGER_MSG == in_msg->msg_type) {
        *out_msg = in_msg;
        goto function_exit;
    }

    COPS_CHK_RC(cops_tapp_init_io(state, state->tapp_io));

    /* Run preinvoke */
    if (COPS_SIPC_DEAUTHENTICATE_MREQ == in_msg->msg_type) {
        COPS_CHK_RC(cops_msg_preinvoke(state, in_msg, out_msg));
    }

    /* Check if message can be handled locally or must be sent on */
    switch (in_msg->msg_type) {
    /* lint -e616 Suppress control flows into case/default.Expected behaviour */
    case COPS_SIPC_AUTHENTICATE_MREQ: {
        cops_auth_data_t *authdata =
            (cops_auth_data_t *)(void *)(COPS_SIPC_DATA(in_msg) + 1);

        if (COPS_AUTH_TYPE_SIMLOCK_KEYS == authdata->auth_type) {
            size_t offset = 0;
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_ARGUMENT_ERROR, 0,
                                                COPS_SIPC_AUTHENTICATE_MRESP,
                                                COPS_SENDER_UNSECURE, out_msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint32(*out_msg, &offset,
                                                  COPS_RC_ARGUMENT_ERROR));
            goto function_exit;
        }
    }
    /* lint +e616 */
    case COPS_SIPC_DEAUTHENTICATE_MREQ:
    case COPS_SIPC_CALCULATE_HASH_MREQ:
    case COPS_SIPC_VERIFY_SIGNED_HEADER_MREQ:
    case COPS_SIPC_GET_NBR_OF_OTP_ROWS_MREQ:
    case COPS_SIPC_READ_OTP_MREQ:
    case COPS_SIPC_WRITE_OTP_MREQ:
    case COPS_SIPC_GET_CHALLANGE_MREQ:
    case COPS_SIPC_BIND_DATA_MREQ:
    case COPS_SIPC_VERIFY_DATA_BINDING_MREQ:
    case COPS_SIPC_GET_DEVICE_STATE_MREQ:
    case COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MREQ:
    case COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MREQ:
        COPS_CHK_RC(cops_msg_get_taf_id(in_msg, &taf_id));
        break;
    default:
        taf_id = COPS_SERVICE_ROUTER;
        break;
    }

    /* Run invoke */
    COPS_CHK_RC(cops_data_set_sipc_message(&state->tapp_io->args, in_msg));
    COPS_CHK_RC(cops_tapp_invoke(state, &state->tapp_io, taf_id));
    COPS_CHK_RC(cops_data_get_sipc_message(&state->tapp_io->rvs, out_msg));

    /* (De)authenticate can modify data */
    if (COPS_SIPC_AUTHENTICATE_MRESP == (*out_msg)->msg_type ||
        COPS_SIPC_DEAUTHENTICATE_MRESP == (*out_msg)->msg_type) {
        COPS_CHK_RC(cops_msg_update_data(state, *out_msg));
    }

function_exit:
    return ret_code;
}

static cops_return_code_t cops_msg_handle_daemon(struct cops_state *state,
                                                cops_sipc_message_t *in_msg,
                                                cops_sipc_message_t **out_msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = in_msg;

    /* Get the tunneled request message in COPS_SIPC_MODEM_SIPC_MX_MREQ */
    if (COPS_SIPC_MODEM_SIPC_MX_MREQ == msg->msg_type) {
        msg = (cops_sipc_message_t *)COPS_SIPC_DATA(msg);
    }

    /* Trigger messages should bypass tapp_invoke */
    if (COPS_SIPC_TRIGGER_MSG == msg->msg_type) {
        *out_msg = msg;
        goto function_exit;
    }

    COPS_CHK_RC(cops_tapp_init_io(state, state->tapp_io));

    COPS_CHK_RC(cops_msg_preinvoke(state, msg, out_msg));

    /* Only run invoke if out_msg == NULL */
    if (NULL == *out_msg) {
        cops_taf_id_t taf_id = COPS_SERVICE_NUM_SERVICES;

        COPS_CHK_RC(cops_msg_get_taf_id(msg, &taf_id));
        COPS_CHK_RC(cops_data_set_sipc_message(&state->tapp_io->args, msg));
        COPS_CHK_RC(cops_tapp_invoke(state, &state->tapp_io, taf_id));
        COPS_CHK_RC(cops_data_get_sipc_message(&state->tapp_io->rvs, out_msg));
    }

    COPS_CHK_RC(cops_msg_postinvoke(state, out_msg));

    COPS_CHK_RC(cops_msg_update_data(state, *out_msg));

function_exit:
    /* Tunnel the reponse message in a COPS_SIPC_MODEM_SIPC_MX_RESP */
    if (COPS_SIPC_MODEM_SIPC_MX_MREQ == in_msg->msg_type) {
        size_t offset = 0;
        size_t length = 0;
        cops_sipc_message_t *temp_msg = NULL;

        if (NULL != *out_msg) {
            length = (*out_msg)->length;
            temp_msg = *out_msg;
            *out_msg = NULL;
        }

        (void) cops_sipc_alloc_message(ret_code, sizeof(uint32_t) +
                                       sizeof(uint32_t) + length,
                                       COPS_SIPC_MODEM_SIPC_MX_MRESP,
                                       COPS_SENDER_UNSECURE, out_msg);

        if (NULL != *out_msg) {
            (void) cops_tapp_sipc_set_uint32(*out_msg, &offset, ret_code);

            if (COPS_RC_OK == ret_code && NULL != temp_msg) {
                (void) cops_tapp_sipc_set_uint32(*out_msg, &offset,
                                                 temp_msg->length);
                (void) cops_tapp_sipc_set_opaque(*out_msg, &offset, temp_msg,
                                                 temp_msg->length);
            }
        } else {
            ret_code = COPS_RC_MEMORY_ALLOCATION_ERROR;
        }

        cops_sipc_free_message(&temp_msg);
    }

    return ret_code;
}

static cops_return_code_t cops_msg_get_taf_id(cops_sipc_message_t *msg,
                                              cops_taf_id_t *taf_id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    switch (msg->msg_type) {
    case COPS_SIPC_READ_IMEI_MREQ:
        *taf_id = COPS_SERVICE_READ_IMEI;
        break;
    case COPS_SIPC_BIND_PROPERTIES_MREQ:
        *taf_id = COPS_SERVICE_BIND_PROPERTIES;
        break;
    case COPS_SIPC_VERIFY_IMSI_MREQ:
        *taf_id = COPS_SERVICE_VERIFY_IMSI;
        break;
    case COPS_SIPC_READ_SIM_DATA_HRESP:
        *taf_id = COPS_SERVICE_HANDLE_SIMDATA;
        break;
    case COPS_SIPC_READ_MODEM_DATA_MREQ:
        *taf_id = COPS_SERVICE_READ_MODEM_DATA;
        break;
    case COPS_SIPC_GET_SEED_MREQ:
        *taf_id = COPS_SERVICE_GET_SEED;
        break;
    case COPS_SIPC_BIND_DATA_MREQ:
        *taf_id = COPS_SERVICE_BIND_DATA;
        break;
    case COPS_SIPC_VERIFY_DATA_BINDING_MREQ:
        *taf_id = COPS_SERVICE_VERIFY_DATA_BINDING;
        break;
    case COPS_SIPC_AUTHENTICATE_MREQ:
        *taf_id = COPS_SERVICE_AUTHENTICATE;
        break;
    case COPS_SIPC_DEAUTHENTICATE_MREQ:
        *taf_id = COPS_SERVICE_DEAUTHENTICATE;
        break;
    case COPS_SIPC_CALCULATE_HASH_MREQ:
        *taf_id = COPS_SERVICE_CALCULATE_HASH;
        break;
    case COPS_SIPC_GET_CHALLANGE_MREQ:
        *taf_id = COPS_SERVICE_GET_CHALLENGE;
        break;
    case COPS_SIPC_SIMLOCK_UNLOCK_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_UNLOCK;
        break;
    case COPS_SIPC_SIMLOCK_GET_STATUS_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_GET_STATUS;
        break;
    case COPS_SIPC_SIMLOCK_VERIFY_CONTROL_KEYS_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_VERIFY_CONTROL_KEYS;
        break;
    case COPS_SIPC_GET_DEVICE_STATE_MREQ:
        *taf_id = COPS_SERVICE_GET_DEVICE_STATE;
        break;
#ifdef EXTENDED_SIPC_MSGS
    case COPS_SIPC_SET_MODEM_BAND_MREQ:
        *taf_id = COPS_SERVICE_SET_MODEM_BAND;
        break;
    case COPS_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_GET_LOCK_SETTINGS;
        break;
    case COPS_SIPC_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT;
        break;
    case COPS_SIPC_SIMLOCK_MCK_RESET_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_MCK_RESET;
        break;
#endif
    case COPS_SIPC_SIMLOCK_LOCK_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_LOCK;
        break;
    case COPS_SIPC_VERIFY_SIGNED_HEADER_MREQ:
        *taf_id = COPS_SERVICE_VERIFY_SIGNED_HEADER;
        break;
    case COPS_SIPC_READ_DATA_MREQ:
        *taf_id = COPS_SERVICE_READ_DATA;
        break;
    case COPS_SIPC_INIT_ARB_TABLE_MREQ:
        *taf_id = COPS_SERVICE_INIT_ARB_TABLE;
        break;
    case COPS_SIPC_CHECK_ARB_UPDATE_MREQ:
        *taf_id = COPS_SERVICE_CHECK_ARB_UPDATE;
        break;
    case COPS_SIPC_GET_NBR_OF_OTP_ROWS_MREQ:
    case COPS_SIPC_READ_OTP_MREQ:
    case COPS_SIPC_WRITE_OTP_MREQ:
        *taf_id = COPS_SERVICE_COPS_OTP;
        break;
    case COPS_SIPC_WRITE_SECPROFILE_MREQ:
        *taf_id = COPS_SERVICE_WRITE_SECPROFILE;
        break;
    case COPS_SIPC_WRITE_RPMB_KEY_MREQ:
        *taf_id = COPS_SERVICE_WRITE_RPMB_KEY;
        break;
    case COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MREQ:
        *taf_id = COPS_SERVICE_SIMLOCK_CHANGE_CONTROL_KEY;
        break;
    case COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MREQ:
        *taf_id = COPS_SERVICE_GET_PRODUCT_DEBUG_SETTINGS;
        break;
    case COPS_SIPC_DERIVE_SESSION_KEY_MREQ:
    case COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MRESP:
    case COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MRESP:
        *taf_id = COPS_SERVICE_DERIVE_SESSION_KEY;
        break;
    case COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MREQ:
        *taf_id = COPS_SERVICE_GET_LONG_TERM_SHARED_KEY;
        break;
    case COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MREQ:
        *taf_id = COPS_SERVICE_GET_SESSION_KEY_MODEM_RAND;
        break;
    default:
        if (COPS_SIPC_IS_REQUEST(msg)) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR, "Message:0x%x not supported\n",
                        msg->msg_type);
        }
        break;
    }

function_exit:
    return ret_code;
}

/*
 * Messages that need to execute before invoke.
 * Invoke can also be invalidated by creating an out_msg.
 */
static cops_return_code_t cops_msg_preinvoke(struct cops_state *state,
                                             cops_sipc_message_t *in_msg,
                                             cops_sipc_message_t **out_msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint32_t return_code;
    size_t offset = 0;

    switch (in_msg->msg_type) {
    case COPS_SIPC_BIND_PROPERTIES_MREQ:
        COPS_CHK_RC(cops_msg_bind_properties(state, in_msg));
        break;
    case COPS_SIPC_DEAUTHENTICATE_MREQ: {
        bool permanent = false;
        COPS_CHK_RC(cops_tapp_sipc_get_bool(in_msg, &offset, &permanent));
        if (permanent) {
            COPS_CHK_RC(cops_bootpart_remove_record(RECORD_ID_AUTHMAGIC));
        }
        break;
    }
    case COPS_SIPC_LOCK_BOOT_PART_MREQ:
        return_code = cops_bootpart_lock_partition();
        COPS_CHK_RC(cops_sipc_alloc_message((cops_return_code_t)return_code,
                                            sizeof(uint32_t),
                                            COPS_SIPC_LOCK_BOOT_PART_MRESP,
                                            COPS_SENDER_UNSECURE, out_msg));
        COPS_CHK_RC(cops_tapp_sipc_set_uint32(*out_msg, &offset, return_code));
        break;
    case COPS_SIPC_SIMLOCK_OTA_UNLOCK_MREQ: {
        cops_simlock_ota_reply_message_t reply;
        return_code = cops_msg_ota_unlock(state, in_msg, &reply);
        COPS_CHK_RC(cops_sipc_alloc_message((cops_return_code_t)return_code,
                                            sizeof(uint32_t) + sizeof(reply),
                                            COPS_SIPC_SIMLOCK_OTA_UNLOCK_MRESP,
                                            COPS_SENDER_UNSECURE, out_msg));
        COPS_CHK_RC(cops_tapp_sipc_set_uint32(*out_msg, &offset, return_code));
        if (COPS_RC_OK == return_code) {
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(*out_msg, &offset, &reply,
                                                  sizeof(reply)));
        }
        break;
    }
    default:
        break;
    }

function_exit:
    /* An error has occurred. Create out_msg */
    if (COPS_RC_OK != ret_code) {
        offset = 0;
        cops_sipc_free_message(out_msg);
        (void) cops_sipc_alloc_message(ret_code, sizeof(uint32_t),
                                      (cops_sipc_mtype_t)(in_msg->msg_type + 1),
                                      COPS_SENDER_UNSECURE, out_msg);
        (void) cops_tapp_sipc_set_uint32(*out_msg, &offset, ret_code);
    }

    return ret_code;
}

/* Messages that need to continue to execute after invoke. */
static cops_return_code_t cops_msg_postinvoke(struct cops_state *state,
                                              cops_sipc_message_t **msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;
    uint32_t return_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(NULL != *msg);

    switch ((*msg)->msg_type) {
    case COPS_SIPC_INIT_ARB_TABLE_MRESP:
    case COPS_SIPC_CHECK_ARB_UPDATE_MRESP:
        /* Only the return code is sent back to the client but here the
         * tables are extracted and written to flash by cops */
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(*msg, &offset, &return_code));
        COPS_CHK_RC((cops_return_code_t)return_code);
        COPS_CHK_RC(cops_msg_update_arb_tables(*msg, offset));
        break;
    case COPS_SIPC_WRITE_SECPROFILE_MRESP: {
        uint32_t length = 0;
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(*msg, &offset, &return_code));
        COPS_CHK_RC((cops_return_code_t)return_code);

        length = COPS_SIPC_DATA_LENGTH(*msg) - sizeof(uint32_t);

        if (length != 0) {
            uint8_t *data = 0;
            COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(*msg, &offset,
                                                         &data, length));

            COPS_CHK_RC(cops_bootpart_write_record(RECORD_ID_SECPROFILE, length,
                                                   data));
        }
        break;
    }
    case COPS_SIPC_READ_DATA_MRESP:
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(*msg, &offset, &return_code));
        cops_sipc_free_message(msg);

        if (COPS_RC_OK == return_code ||
            COPS_RC_DATA_TAMPERED_ERROR == return_code) {
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                                sizeof(uint32_t) +
                                                sizeof(uint32_t) +
                                                state->data.length,
                                                COPS_SIPC_READ_DATA_MRESP,
                                                COPS_SENDER_UNSECURE, msg));
        } else {
            COPS_CHK_RC(cops_sipc_alloc_message((cops_return_code_t)return_code,
                                            0,
                                            COPS_SIPC_READ_DATA_MRESP,
                                            COPS_SENDER_UNSECURE, msg));
        }

        offset = 0;
        COPS_CHK_RC(cops_tapp_sipc_set_uint32(*msg, &offset, return_code));
        if (COPS_RC_OK == return_code ||
            COPS_RC_DATA_TAMPERED_ERROR == return_code) {
            COPS_CHK_RC(cops_tapp_sipc_set_uint32(*msg, &offset,
                                                  state->data.length));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(*msg, &offset,
                                                  state->data.data,
                                                  state->data.length));
        }
        break;
    case COPS_SIPC_DERIVE_SESSION_KEY_MRESP:
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(*msg, &offset, &return_code));
        COPS_CHK_RC((cops_return_code_t)return_code);
#ifndef COPS_IN_LOADERS
        /*
         * TODO: In case timing changes drastically and session key is not
         * ready when the last IPC state request is sent from modem we have to
         * trigger an IPC state response from here.
         */
        state->session_key_ready = true;
#endif
        break;
    default:
        break;
    }

function_exit:
    return ret_code;
}

/* Messages that can update COPS data */
static cops_return_code_t cops_msg_update_data(struct cops_state *state,
                                               cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    enum cops_data_type data_type = COPS_DATA;

    switch (msg->msg_type) {
    /*lint -e616 Suppress control flows into case/default. Expected behaviour */
    case COPS_SIPC_AUTHENTICATE_MRESP:
    case COPS_SIPC_DEAUTHENTICATE_MRESP:
        data_type = PERM_AUTH_STATE_DATA;
    case COPS_SIPC_SIMLOCK_GET_STATUS_MRESP:
    case COPS_SIPC_BIND_PROPERTIES_MRESP: {
        size_t offset = 0;
        uint32_t return_code;
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
        COPS_CHK_RC((cops_return_code_t)return_code);
    }
    case COPS_SIPC_SIMLOCK_UNLOCK_MRESP:
    case COPS_SIPC_VERIFY_IMSI_MRESP:
    case COPS_SIPC_READ_SIM_DATA_HREQ:
#ifdef EXTENDED_SIPC_MSGS
    case COPS_SIPC_SET_MODEM_BAND_MRESP:
    case COPS_SIPC_SIMLOCK_MCK_RESET_MRESP:
#endif
    case COPS_SIPC_SIMLOCK_LOCK_MRESP:
    case COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MRESP:
    /* lint +e616 */
        COPS_CHK_RC(cops_tapp_save_cops_data(state, state->tapp_io, data_type));
        break;
    default:
        break;
    }

function_exit:
    return ret_code;
}

#ifndef COPS_IN_LOADERS
/* Messages that can trigger event handling */
cops_return_code_t cops_msg_trigger_event_handling(struct cops_state *state,
                                                   cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *temp_msg = NULL;
    cops_sipc_message_t *temp_out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    if (COPS_RUN_MODE_DAEMON == run_mode ||
        (COPS_RUN_MODE_RELAY == run_mode &&
        (COPS_SIPC_AUTHENTICATE_MRESP == msg->msg_type ||
        COPS_SIPC_DEAUTHENTICATE_MRESP == msg->msg_type))) {

        switch (msg->msg_type) {
        /*
         * Special case, handle event depending on tunneled message
         * 16 = header(8 bytes) + ret_code(4 bytes) +
         * length tunneled msg(4 bytes)
         */
        case COPS_SIPC_MODEM_SIPC_MX_MRESP: {
            COPS_CHK_RC(cops_msg_trigger_event_handling(state,
                                (cops_sipc_message_t *)((uint8_t *)msg + 16)));
            break;
        }
        case COPS_SIPC_SIMLOCK_OTA_UNLOCK_MRESP:
        case COPS_SIPC_BIND_PROPERTIES_MRESP:
        case COPS_SIPC_SIMLOCK_LOCK_MRESP:
        case COPS_SIPC_SIMLOCK_UNLOCK_MRESP:
            /*
             * These messages will perfom a verify imsi if 8500, or send a
             * trigger message to modem so verify imsi is performed from there.
             */
            COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
            COPS_CHK_RC((cops_return_code_t)return_code);
#ifdef COPS_8500
            /* Dont add IMSI so the cached IMSI(if available) will be used */
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                                COPS_SIPC_VERIFY_IMSI_MREQ,
                                                COPS_SENDER_UNSECURE,
                                                &temp_msg));

            COPS_CHK_RC(cops_msg_handle(state, temp_msg, &temp_out_msg));
            COPS_CHK_RC(cops_msg_dispatch_event(state, temp_out_msg));
#else
            /* Create trigger message */
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(uint8_t),
                                                COPS_SIPC_TRIGGER_MSG,
                                                COPS_SENDER_UNSECURE,
                                                &temp_msg));
            offset = 0;
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(temp_msg, &offset,
                                             COPS_TRIGGER_VERIFY_IMSI_REQUEST));
            COPS_LOG(LOG_INFO, "Triggering SIPC. Type = 0x%x, length = %u\n",
                     temp_msg->msg_type, temp_msg->length);
            COPS_CHK_RC(cops_router_handle_msg(state, NO_FD, temp_msg,
                                               temp_msg));
#endif /* COPS_8500 */
            break;
        case COPS_SIPC_AUTHENTICATE_MRESP:
        case COPS_SIPC_DEAUTHENTICATE_MRESP:
            /*
             * These messages will dispatch an authentication event if 8500,
             * or send a trigger message to modem so event is dispatched there.
             */
            COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));
            COPS_CHK_RC((cops_return_code_t)return_code);

            /* Create trigger message */
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, sizeof(uint8_t),
                                                COPS_SIPC_TRIGGER_MSG,
                                                COPS_SENDER_UNSECURE,
                                                &temp_msg));
            offset = 0;
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(temp_msg, &offset,
                                            COPS_TRIGGER_AUTHENTICATION_EVENT));

            COPS_LOG(LOG_INFO, "Triggering SIPC. Type = 0x%x, length = %u\n",
                     temp_msg->msg_type, temp_msg->length);
#ifdef COPS_8500
            COPS_CHK_RC(cops_msg_dispatch_event(state, temp_msg));

#else
            COPS_CHK_RC(cops_router_handle_msg(state, NO_FD, temp_msg,
                                               temp_msg));
#endif /* COPS_8500 */
            break;
        default:
            break;
        }
    }

function_exit:
    cops_sipc_free_message(&temp_msg);
    cops_sipc_free_message(&temp_out_msg);

    return ret_code;
}

/* Messages that can cause events to be dispatched */
cops_return_code_t cops_msg_dispatch_event(struct cops_state *state,
                                           cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *temp_msg = NULL;
    cops_sipc_message_t *temp_out_msg = NULL;
    size_t offset = 0;
    uint32_t return_code;

    if (COPS_SIPC_SIMLOCK_OTA_UNLOCK_MRESP == msg->msg_type) {
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));

        if (COPS_RC_OK == return_code) {
            cops_simlock_ota_reply_message_t reply;
            cops_simlock_ota_unlock_status_t status;

            COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, &reply,
                                                  sizeof(reply)));
            COPS_CHK_RC(cops_msg_create_ota_status(&status, &reply));

            offset = 0;
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                       sizeof(uint8_t) +
                                       sizeof(cops_simlock_ota_unlock_status_t),
                                       COPS_SIPC_GENERIC_EVENT,
                                       COPS_SENDER_UNSECURE, &temp_msg));

            COPS_CHK_RC(cops_tapp_sipc_set_uint8(temp_msg, &offset,
                                     COPS_API_EVENT_OTA_UNLOCK_STATUS_UPDATED));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(temp_msg, &offset, &status,
                                     sizeof(cops_simlock_ota_unlock_status_t)));
            cops_server_send_event(state->server_ctx, temp_msg);
        }
    } else if (COPS_SIPC_VERIFY_IMSI_MRESP == msg->msg_type) {
        uint8_t active_locks = 0;
        cops_sim_status_t status = {COPS_SIMLOCK_CARD_STATUS_NOT_CHECKED,
                                    (cops_simlock_lock_type_t)0xff};

        COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &return_code));

        if (COPS_RC_OK == return_code) {
            status.card_status = COPS_SIMLOCK_CARD_STATUS_APPROVED;
        } else {
            status.card_status = COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED;
        }
        COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset, &active_locks));
        COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset,
                                             (uint8_t *)&status.failed_lt));

        offset = 0;
        COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                            sizeof(uint8_t) +
                                            sizeof(cops_sim_status_t),
                                            COPS_SIPC_GENERIC_EVENT,
                                            COPS_SENDER_UNSECURE, &temp_msg));
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(temp_msg, &offset,
                                        COPS_API_EVENT_SIMLOCK_STATUS_CHANGED));
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(temp_msg, &offset, &status,
                                              sizeof(cops_sim_status_t)));
        cops_server_send_event(state->server_ctx, temp_msg);
    } else if (COPS_SIPC_TRIGGER_MSG == msg->msg_type) {
        uint8_t trigger_msg;

        COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset, &trigger_msg));

        if (COPS_TRIGGER_AUTHENTICATION_EVENT == trigger_msg) {

            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                                sizeof(uint8_t),
                                                COPS_SIPC_GENERIC_EVENT,
                                                COPS_SENDER_UNSECURE,
                                                &temp_msg));
            offset = 0;
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(temp_msg, &offset,
                                        COPS_API_EVENT_AUTHENTICATION_CHANGED));
            cops_server_send_event(state->server_ctx, temp_msg);

            /* If derive session key has failed, retry again */
            if (COPS_RUN_MODE_DAEMON == run_mode &&
                false == state->session_key_ready) {
                cops_sipc_free_message(&temp_msg);
                COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                              COPS_SIPC_DERIVE_SESSION_KEY_MREQ,
                                              COPS_SENDER_UNSECURE,
                                              &temp_msg));
                COPS_LOG(LOG_INFO, "Triggering SIPC. Type = 0x%x, length= %u\n",
                         temp_msg->msg_type, temp_msg->length);
                COPS_CHK_RC(cops_msg_handle(state, temp_msg, &temp_out_msg));
                COPS_CHK_RC(cops_router_handle_msg(state, NO_FD, temp_msg,
                                                   temp_out_msg));
            }
        }
    }

function_exit:
    cops_sipc_free_message(&temp_msg);
    cops_sipc_free_message(&temp_out_msg);
    return ret_code;
}

static cops_return_code_t cops_msg_create_ota_status(
                                       cops_simlock_ota_unlock_status_t *status,
                                       cops_simlock_ota_reply_message_t *msg)
{
    cops_simlock_lock_type_t lock_type;

    status->network = false;
    status->network_subset = false;
    status->service_provider = false;
    status->corporate = false;

    /* Peek into reply message to get information to build up ota event */
    for (lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
         lock_type <= COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK; lock_type++) {

        if (SIMLOCK_OTA_CURRENTLY_NOT_PERSONALISED ==
            msg->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type]) {

            switch (lock_type) {
            case COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK:
                status->network = true;
                break;
            case COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK:
                status->network_subset = true;
                break;
            case COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK:
                status->service_provider = true;
                break;
            case COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK:
                status->corporate = true;
                break;
            default:
                COPS_LOG(LOG_WARNING, "Unknown lock type %d\n", lock_type);
            }
        }
    }

    return COPS_RC_OK;
}
#endif /* COPS_IN_LOADERS */

static cops_return_code_t cops_msg_bind_properties(struct cops_state *state,
                                                   cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;
    uint8_t authentication_type;
    size_t cops_data_length = 0;
    bool merge_cops_data;
    cops_imei_t imei;
    uint8_t *cops_data = NULL;
    bool imei_available;

    memset(&imei, 0, sizeof(cops_imei_t));

    COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset, &authentication_type));

    if (COPS_AUTH_TYPE_SIMLOCK_KEYS == authentication_type) {
        cops_simlock_control_keys_t keys;
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset,
                                              &keys, sizeof(keys)));
    } else if (COPS_AUTH_TYPE_MCK == authentication_type) {
        cops_simlock_control_key_t mck;
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset,
                                              &mck, sizeof(mck)));
    }

    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset,
                                          (uint32_t *)&cops_data_length));
    COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset,
                                         (uint8_t *)&merge_cops_data));

    if (cops_data_length > 0) {
        cops_data = (uint8_t *)malloc(cops_data_length);
        COPS_CHK_ALLOC(cops_data);
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, cops_data,
                                              cops_data_length));
    }

    COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset,
                                         (uint8_t *)&imei_available));

    if (imei_available) {
        COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, &imei,
                                              sizeof(cops_imei_t)));
    }

    if (cops_data_length > 0) {
        cops_data_t cd;
        cops_parameter_data_t d;

        if (!merge_cops_data) {
            /* Delete old cops data */
            state->tapp_io->data.length = 0;
            COPS_CHK_RC(cops_data_init(&state->tapp_io->data));
        }

        /*
         * A side effect of adding the data in this way is that
         * the parameter ID:s of COPS Data are sorted regardless of
         * the input.
         *
         * If there is no data to read from flash, we will not try to
         * merge the new and the old data.
         */

        if (state->tapp_io->data.length == 0) {
            COPS_CHK_RC(cops_data_init(&state->tapp_io->data));
        }

        offset = 0;
        cd.data = cops_data;
        cd.length = cops_data_length;
        cd.max_length = cops_data_length;

        for (;;) {
            COPS_CHK_RC(cops_data_get_next_parameter(&cd, &offset, &d));

            if (d.data == NULL) {
                break;    /* got end of supplied COPS Data */
            }

            COPS_CHK_RC(cops_data_set_parameter
                        (&state->tapp_io->data, &d));
        }
    }

    /* Add IMEI to COPS Data after we have added supplied COPS Data */
    if (imei_available) {
        size_t n;

        cops_data_imei_t packed_imei;

        for (n = 0; n < COPS_UNPACKED_IMEI_LENGTH - 1; n++) {
            if (imei.digits[n] > 9) {
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Invalid IMEI");
            }
        }

        util_cops_pack_imei(&imei, &packed_imei);

        /* Ignore comparison if the luhn is set to 0xFF */
        if (imei.digits[COPS_UNPACKED_IMEI_LENGTH - 1] != 0xFF) {
            cops_imei_t unpacked_imei;

            util_cops_unpack_imei(&packed_imei, &unpacked_imei);

            if (memcmp
                    (imei.digits, unpacked_imei.digits,
                     sizeof(unpacked_imei.digits)) != 0) {
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Invalid IMEI (invalid Luhn digit, "
                            "exptected '%c' got '%c')",
                            '0' +
                            unpacked_imei.
                            digits[COPS_UNPACKED_IMEI_LENGTH - 1],
                            '0' +
                            imei.digits[COPS_UNPACKED_IMEI_LENGTH -
                                              1]);
            }
        }

        COPS_CHK_RC(cops_data_set_imei
                    (&state->tapp_io->data, &packed_imei));
    }

function_exit:
    if (cops_data != NULL) {
        free(cops_data);
    }

    return ret_code;
}

static cops_return_code_t cops_msg_update_arb_tables(cops_sipc_message_t *msg,
                                                     size_t offset)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_arb_table_info_t arbinfo;

    /* This part is consumed here but not returned to client */
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, &arbinfo,
                                          sizeof(cops_arb_table_info_t)));

    if (arbinfo.tocids[0] != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(msg, &offset,
                                                 (uint8_t **)&arbinfo.tocids[0],
                                                 arbinfo.tocidsize[0]));
    }

    if (arbinfo.tocids[1] != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(msg, &offset,
                                                 (uint8_t **)&arbinfo.tocids[1],
                                                 arbinfo.tocidsize[1]));
    }

    if (arbinfo.tables[0] != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(msg, &offset,
                                                     &arbinfo.tables[0],
                                                     arbinfo.tablesize[0]));

        COPS_CHK_RC(cops_write_tocdata(arbinfo.tocids[0], arbinfo.tables[0],
                                       arbinfo.tablesize[0]));
    }

    if (arbinfo.tables[1] != NULL) {
        COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(msg, &offset,
                                                     &arbinfo.tables[1],
                                                     arbinfo.tablesize[1]));

        COPS_CHK_RC(cops_write_tocdata(arbinfo.tocids[1], arbinfo.tables[1],
                                       arbinfo.tablesize[1]));
    }

function_exit:
    return ret_code;
}

static cops_return_code_t cops_msg_ota_unlock(struct cops_state *state,
                                        cops_sipc_message_t *msg,
                                        cops_simlock_ota_reply_message_t *reply)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;
    size_t blen = 0;
    cops_ota_message_t *ota_message = NULL;
    bool match = false;

    COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, (uint32_t *)&blen));

    COPS_CHK(blen <= sizeof(cops_ota_message_t), COPS_RC_ARGUMENT_ERROR);
    /* Check that the data length covers at least the mandatory data */
    COPS_CHK(blen >= (sizeof(cops_ota_message_t) - sizeof(cops_ota_key_t) * 4),
            COPS_RC_ARGUMENT_ERROR);

    ota_message = malloc(sizeof(cops_ota_message_t));
    COPS_CHK_ALLOC(ota_message);
    memset(ota_message->unlock_key_extension, 'F',
           sizeof(ota_message->unlock_key_extension));
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(msg, &offset, ota_message, blen));

    /* Set up reply with default data */
    memset(reply, 0, sizeof(cops_simlock_ota_reply_message_t));

    /* Fill reply with current data */
    COPS_CHK_RC(cops_msg_ota_unlock_check_and_load_imei(state, ota_message,
                                                        reply, &match));

    if (!match) {
        /* Exit with positive returnvalue! */
        goto function_exit;
    }

    COPS_CHK_RC(cops_msg_ota_unlock_set_current_lock_mode(state, reply));

    COPS_CHK_RC(cops_msg_ota_unlock_perform_unlocking(state, ota_message,
                                                      reply));

function_exit:
    if (NULL != ota_message) {
        free(ota_message);
    }

    return ret_code;
}

static cops_return_code_t cops_msg_ota_unlock_check_and_load_imei(
                            struct cops_state *state,
                            const cops_ota_message_t *ota_message,
                            cops_simlock_ota_reply_message_t *ota_reply_message,
                            bool *match)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    cops_imei_t imei;
    cops_simlock_lock_type_t lock_type;
    size_t n;
    size_t offset = 0;
    uint32_t return_code;

    /* Read the IMEI */
    COPS_CHK_RC(cops_sipc_alloc_message(ret_code, 0, COPS_SIPC_READ_IMEI_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));
    COPS_CHK_RC(cops_msg_handle(state, in_msg, &out_msg));
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_READ_IMEI_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    /* Skip 4 bytes, containing the number of IMEIs and padding */
    offset += 4;
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, &imei,
                                          sizeof(imei)));

    for (n = 0; n < COPS_UNPACKED_IMEI_LENGTH; n++) {
        ota_reply_message->data[n] = imei.digits[n] + '0';
    }

    ota_reply_message->data[COPS_UNPACKED_IMEI_LENGTH] = '0';

    /*
     * Check if IMEI of message and reply is identical. Ignore byte nr 16
     * when comparing according to 3gpp-standard
     */
    if (memcmp(ota_reply_message->data, ota_message->imei,
               SIMLOCK_OTA_SIZE_OF_IMEI - 1) != 0) {
        for (lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
                lock_type <= COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
                lock_type++) {
            ota_reply_message->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type] =
                SIMLOCK_OTA_IMEI_MISMATCH;
        }

        *match = false;
    } else {
        *match = true;
    }

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

static cops_return_code_t cops_msg_ota_unlock_set_current_lock_mode(
                            struct cops_state *state,
                            cops_simlock_ota_reply_message_t *ota_reply_message)
{
    cops_simlock_status_t status;
    cops_simlock_lock_type_t lock_type;
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_simlock_lock_status_t *lock_status;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
    cops_taf_simlock_status_t taf_status;
    uint32_t return_code;
    size_t offset = 0;

    COPS_CHK_RC(cops_sipc_alloc_message(ret_code, 0,
                                        COPS_SIPC_SIMLOCK_GET_STATUS_MREQ,
                                        COPS_SENDER_UNSECURE, &in_msg));
    COPS_CHK_RC(cops_msg_handle(state, in_msg, &out_msg));
    COPS_CHK_ASSERTION(out_msg->msg_type == COPS_SIPC_SIMLOCK_GET_STATUS_MRESP);
    COPS_CHK_RC(cops_tapp_sipc_get_uint32(out_msg, &offset, &return_code));
    COPS_CHK_RC((cops_return_code_t)return_code);
    COPS_CHK_RC(cops_tapp_sipc_get_opaque(out_msg, &offset, &taf_status,
                                          sizeof(cops_taf_simlock_status_t)));
    /* Mark that if changing cops_simlock_status_t the correponding type
     * taf_cops_simlock_status_t also needs to be changed */
    COPS_CHK_RC(cops_util_convert_status(&taf_status, &status));

    for (lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
            lock_type <= COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK; lock_type++) {
        switch (lock_type) {
        case COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK:
            lock_status = &(status.nl_status);
            break;
        case COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK:
            lock_status = &(status.nsl_status);
            break;
        case COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK:
            lock_status = &(status.spl_status);
            break;
        case COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK:
            lock_status = &(status.cl_status);
            break;
        default:
            ret_code = COPS_RC_ARGUMENT_ERROR;
            goto function_exit;
        }

        switch (lock_status->lock_setting) {
        case COPS_SIMLOCK_LOCK_SETTING_DISABLED:
            ota_reply_message->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type] =
                SIMLOCK_OTA_PERMANENTLY_NOT_PERSONALISED;
            break;
        case COPS_SIMLOCK_LOCK_SETTING_UNLOCKED:
            ota_reply_message->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type] =
                SIMLOCK_OTA_CURRENTLY_NOT_PERSONALISED;
            break;
        case COPS_SIMLOCK_LOCK_SETTING_LOCKED:
        case COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED:
        case COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED:
            ota_reply_message->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type] =
                SIMLOCK_OTA_PERSONALISED;
            break;
        default:
            ret_code = COPS_RC_ARGUMENT_ERROR;
            goto function_exit;
        }
    }

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

static cops_return_code_t cops_msg_ota_unlock_perform_unlocking(
                            struct cops_state *state,
                            const cops_ota_message_t *ota_message,
                            cops_simlock_ota_reply_message_t *ota_reply_message)
{
    cops_simlock_lock_type_t lock_type;
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_simlock_control_key_t control_key;

    char *end;
    size_t i;
    cops_sipc_message_t *req_msg = NULL;
    cops_sipc_message_t *resp_msg = NULL;

    for (lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
            lock_type <= COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK; lock_type++) {

        /* Init ControlKey with zeros to ensure zero termination */
        memset(&control_key, 0, sizeof(cops_simlock_control_key_t));

        /* Copy the normal key */
        memcpy(control_key.value, ota_message->unlock_key[lock_type].key,
               SIMLOCK_OTA_SIZE_OF_UNLOCK_KEY);

        /* Copy the extended key */
        for (i = 0; i < 8; i++) {
            control_key.value[8+i] =
                ota_message->unlock_key_extension[lock_type].key[i];
        }

        /* NULL terminate at the first 'F' to comply with
         * TS.22.022 version 8.00 */
        end = strchr((char *) &control_key, 'F');

        if (end != NULL) {
            *end = '\0';
        }

        /* Control keys of zero length should not be unlocked */
        if (strnlen((char *) &control_key,
                    sizeof(cops_simlock_control_key_t)) > 0) {
            uint32_t return_code;
            uint32_t error_code;
            size_t offset = 0;

            /* NOTE:
             * The OTA unlock also uses retries counter and timers as
             * "normal" unlock. But in contradiction it does not stop
             * on a failing unlock it will just continue with the other
             * locks. */

            /* Not an invalid key try to unlock */
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK,
                                         sizeof(cops_simlock_lock_type_t) +
                                         sizeof(cops_simlock_control_key_t) + 0,
                                         COPS_SIPC_SIMLOCK_UNLOCK_MREQ,
                                         COPS_SENDER_UNSECURE, &req_msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint8(req_msg, &offset, lock_type));
            COPS_CHK_RC(cops_util_bp_pad_control_key(&control_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
            COPS_CHK_RC(cops_tapp_sipc_set_opaque(req_msg, &offset,
                                                  &control_key,
                                           sizeof(cops_simlock_control_key_t)));
            COPS_CHK_RC(cops_msg_handle(state, req_msg, &resp_msg));
            offset = 0;
            COPS_CHK_ASSERTION(resp_msg->msg_type ==
                               COPS_SIPC_SIMLOCK_UNLOCK_MRESP);
            COPS_CHK_RC(cops_tapp_sipc_get_uint32(resp_msg, &offset,
                                                  &return_code));
            COPS_CHK_RC(cops_tapp_sipc_get_uint32(resp_msg, &offset,
                                                  &error_code));
            ret_code = (cops_return_code_t)return_code;
            cops_sipc_free_message(&req_msg);
            cops_sipc_free_message(&resp_msg);

            /* Set indication that lock was unlocked in return message */
            if (COPS_RC_OK == error_code) {
                COPS_LOG(LOG_INFO, "lock %d unlocked\n", lock_type);
                ota_reply_message->data[SIMLOCK_OTA_SIZE_OF_IMEI + lock_type] =
                    SIMLOCK_OTA_CURRENTLY_NOT_PERSONALISED;
            }
        }
    }

function_exit:
    cops_sipc_free_message(&req_msg);
    cops_sipc_free_message(&resp_msg);
    return ret_code;
}
