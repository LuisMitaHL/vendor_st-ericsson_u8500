/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
/*
 * Implemented interface
 */
#include "cops.h"
/*
 * Used interfaces
 */
#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
#include "cn_client.h"
#endif
#include <cops_api_internal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>

#define LOG_TAG "COPS"   /* Define prior to Log.h inclusion */

#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
#include <utils/Log.h>
#endif

#define DEFAULT_DTE_ID 255
#define IMEI_REQUEST   "at@imei?\r"

#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
static cops_return_code_t convert_imei(char imei_string[],
                                       cops_imei_t *cops_imei);

static char calc_luhn_digit(cops_imei_t imei);
#endif

cops_return_code_t cops_context_create(cops_context_id_t **ctxpp,
                                       const cops_event_callbacks_t *event_cbs,
                                       void *event_aux)
{
    cops_context_id_t *ctx = NULL;
    cops_return_code_t result = COPS_RC_OK;


    if (ctxpp == NULL) {
        result = COPS_RC_ARGUMENT_ERROR;
#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
        LOGE("%s: Mandatory arguments NULL,"
             "result=%d", __func__, result);
#endif
        goto function_exit;
    }

    ctx = calloc(1, sizeof(*ctx));

    if (ctx == NULL) {
        result = COPS_RC_MEMORY_ALLOCATION_ERROR;
#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
        LOGE("%s: calloc error,"
             "result=%d", __func__, result);
#endif
        goto function_exit;
    }

    /* Setup a dummy context */
    if (event_cbs != NULL) {
        ctx->event_cbs  = *event_cbs;
    }

    ctx->event_aux  = event_aux;

    *ctxpp = ctx;

function_exit:
    if (result != COPS_RC_OK)
        cops_context_destroy(&ctx);
    return result;
}

void cops_context_set_async_aux(cops_context_id_t *ctxp,
                                void *async_aux)
{
    (void) ctxp;
    (void) async_aux;
}

void cops_context_destroy(cops_context_id_t **ctxpp)
{
    cops_context_id_t *ctx;

    if (ctxpp == NULL || *ctxpp == NULL) {
        return;
    }

    ctx = *ctxpp;

    /* Release dummy context */
    free(ctx);
    *ctxpp = NULL;
}

cops_return_code_t cops_context_async_fd_get(cops_context_id_t *ctxp,
        int *fdp)
{
    (void) ctxp;
    (void) fdp;
    return COPS_RC_IPC_ERROR;
}

bool cops_context_has_async(cops_context_id_t *ctxp)
{
    (void) ctxp;
    return false;
}

#ifndef DUMMY_WITHOUT_IMEI_SUPPORT
cops_return_code_t cops_context_invoke_async_callback(cops_context_id_t *ctxp)
{
    (void) ctxp;
    return COPS_RC_IPC_ERROR;
}

static char calc_luhn_digit(cops_imei_t imei)
{
    uint32_t number = 0;
    int i;
    uint32_t  weigth;
    uint32_t  weighted_digit;

    for (i = 0; i < 14; i++) {

        weigth = ((uint32_t)i % 2) + 1;
        weighted_digit = imei.digits[i] * weigth;
        number += (weighted_digit / 10) + (weighted_digit % 10);
    }
    number %= 10;
    number = 10 - number;
    if (number == 10) {
        number = 0;
    }

    return number;
}

static cops_return_code_t convert_imei(char imei_string[],
                                       cops_imei_t *cops_imei)
{
    char digit;
    uint32_t char_pos = 0;
    uint32_t read_digits = 0;
    cops_imei_t imei;
    cops_return_code_t result = COPS_RC_OK;

    while ((imei_string[char_pos] < '0') | (imei_string[char_pos] > '9'))
        char_pos++;

    while (read_digits < 14) {
        if (imei_string[char_pos] == ' ')
            char_pos++;
        else {
            if ((imei_string[char_pos] < '0') |
                (imei_string[char_pos] > '9')) {
                result = COPS_RC_ARGUMENT_ERROR;
                LOGE("%s: imei digit nr %d is not a number 0-9 ,"
                     "result=%d", __func__, char_pos, result);
                goto function_exit;
            } else {
                imei.digits[read_digits] = imei_string[char_pos]-'0';
                read_digits++;
                char_pos++;
            }
        }
    }

    imei.digits[14] = calc_luhn_digit(imei);
    memcpy(cops_imei, &imei, sizeof(cops_imei_t));

function_exit:
    return result;
}

/* Initialize library and establish connection with C&N server */
static bool call_network_connect(cn_context_t **cn_context_pp, int *fdr_p,
                                 int *fde_p)
{
    cn_error_code_t result = 0;

    result = cn_client_init(cn_context_pp);
    if (CN_SUCCESS == result) {
        /* Get request socket file descriptor */
        result = cn_client_get_request_fd(*cn_context_pp, fdr_p);
        if (CN_SUCCESS == result) {
            /* Get event socket file descriptor */
           result = cn_client_get_event_fd(*cn_context_pp, fde_p);
        }
    }
    return (CN_SUCCESS == result);
}

/* Release connection to C&N server */
static void call_network_disconnect(cn_context_t **cn_context_pp)
{
    (void)cn_client_shutdown(*cn_context_pp);
    if (*cn_context_pp != NULL)
        *cn_context_pp = NULL;
}

static bool isFinalResponseFromCN_OK(char result_string[])
{
    return !memcmp(result_string, "\r\nOK", 4) | !memcmp(result_string, "0", 1);
}

#endif
cops_return_code_t cops_read_imei(cops_context_id_t *ctxp,
                                  cops_imei_t *imei)
{
#ifdef DUMMY_WITHOUT_IMEI_SUPPORT
    (void) ctxp;
    (void) imei;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
    cn_context_t                *cn_context_p = NULL;
    int                          fdr, fde;
    cn_error_code_t              result = 0;
    cn_message_t                *msg_p  = NULL;
    cn_response_at_trans_data_t *cn_response_p;
    cn_uint32_t                  unread = 0;
    cops_return_code_t           return_result = COPS_RC_INTERNAL_ERROR;
    fd_set                       fds;
    struct timeval               timeout;
    int                          rc;

    if (NULL == ctxp) {
        LOGE("%s: failed - Context is NULL", __func__);
        goto function_exit;
    }

    if (NULL == imei) {
        LOGE("%s: failed - Input parameter IMEI is NULL", __func__);
        goto function_exit;
    }

    /* Connect to C&N */
    if (!call_network_connect(&cn_context_p, &fdr, &fde)) {
        LOGE("%s: call_network_connect failed", __func__);
        goto function_exit;
    }

    /* Issue IMEI request */
    result = cn_request_send_transparent_cmd(cn_context_p, DEFAULT_DTE_ID,
                                             IMEI_REQUEST,
                                             (cn_uint16_t)strlen(IMEI_REQUEST),
                                             (cn_client_tag_t)NULL);
    if (CN_SUCCESS != result) {
        LOGE("%s: cn_request_send_transparent_cmd failed, result=%d",
               __func__, result);
        goto function_exit;
    }

    /* Get response */
    timeout.tv_sec  = 5;  /* Setup timeout */
    timeout.tv_usec = 0;

    FD_ZERO(&fds);       /* Create a descriptor set containing our socket.  */
    FD_SET(fdr, &fds);

    do {
        rc = select(fdr+1, &fds, NULL, NULL, &timeout);
    } while (rc == -1 && errno == EINTR);

    if (rc == -1) {
        LOGE("%s: select failed", __func__);
        goto function_exit;
    }

    if (!FD_ISSET(fdr, &fds)) {
        LOGE("%s: FD_ISSET failure", __func__);
        goto function_exit;
    }

    do {
        result = cn_message_receive(fdr, &unread, (cn_message_t **)&msg_p);

        switch (result) {

        case CN_SUCCESS:
            LOGI("%s: cn_message_receive was successful", __func__);
            break;

        case CN_FAILURE:
            /* The CN server has disconnected. Handle error. */
            LOGE("%s: cn_message_receive failed, CN server has disconnected",
                 __func__);
            goto function_exit;

        case CN_REQUEST_BUSY:
           LOGI("%s: Waiting for complete message response", __func__);
           break;

        default:
            LOGE("%s: cn_message_receive failed", __func__);
            goto function_exit;
        }

    } while (CN_REQUEST_BUSY == result);

    /* Now we have the result from C&N in msg_p
     * msg_p->type    defines type
     * msg_p->payload contains actual data
     * */
    cn_response_p = (cn_response_at_trans_data_t *)msg_p->payload;

    /*
     * Result in (char *)cn_response_p->inter_resp.data;
     * and       (char *)cn_response_p->final_resp.data
     *
     * Need to return response back to client ... but first check that final
     * result is "OK"
     *
     * */
    if (!isFinalResponseFromCN_OK((char *)cn_response_p->final_resp.data)) {
        LOGE("%s: isFinalResponseFromCN_OK failed", __func__);
        goto function_exit;
    }

    return_result = convert_imei((char *)cn_response_p->inter_resp.data,
                                  imei);

    if (COPS_RC_OK != return_result) {
        LOGE("%s: imei conversion failed,"
                     "return_result=%d", __func__, return_result);
        goto function_exit;
    }

    return_result = COPS_RC_OK;

function_exit:
    free(msg_p);
    call_network_disconnect(&cn_context_p);
    return return_result;
#endif
}

cops_return_code_t cops_read_otp(cops_context_id_t *ctxp,
                                 cops_otp_t *otp)
{
    (void) ctxp;
    (void) otp;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;

}

cops_return_code_t cops_write_otp(cops_context_id_t *ctxp,
                                  const cops_write_otp_arg_t *arg)
{
    (void) ctxp;
    (void) arg;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_read_data(cops_context_id_t *ctxp,
                                  uint8_t *buf,
                                  size_t *blen)
{
    (void) ctxp;
    (void) buf;
    (void) blen;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_bind_properties(cops_context_id_t *ctxp,
                                        cops_bind_properties_arg_t *arg)
{
    (void) ctxp;
    (void) arg;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_get_challenge(cops_context_id_t *ctxp,
                                      cops_auth_type_t auth_type,
                                      uint8_t *buf,
                                      size_t *blen)
{
    (void) ctxp;
    (void) auth_type;
    (void) buf;
    (void) blen;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_authenticate(cops_context_id_t *ctxp,
                                     bool permanent,
                                     cops_auth_data_t *auth_data)
{
    (void) ctxp;
    (void) permanent;
    (void) auth_data;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_deauthenticate(cops_context_id_t *ctxp, bool permanent)
{
    (void) ctxp;
    (void) permanent;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
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
    (void) ctxp;
    (void) simlock_status;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_simlock_change_sim_control_key(cops_context_id_t *ctxp,
        cops_simlock_control_key_t *old_key,
        cops_simlock_control_key_t *new_key)
{
    (void) ctxp;
    (void) old_key;
    (void) new_key;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
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

cops_return_code_t cops_util_find_parameter(uint8_t *buf,
        size_t blen,
        int param_id,
        uint8_t **param_data,
        size_t *param_len)
{
    (void) buf;
    (void) blen;
    (void) param_id;
    (void) param_data;
    (void) param_len;

    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t
cops_get_device_state(cops_context_id_t *ctxp,
                      cops_device_state_t *device_state)
{
    (void) ctxp;
    (void) device_state;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}
cops_return_code_t
cops_bind_data(cops_context_id_t *ctxp, const uint8_t *buf,
               uint32_t blen, const struct cops_digest *mac)
{
    (void) ctxp;
    (void) buf;
    (void) blen;
    (void) mac;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}
cops_return_code_t
cops_verify_data_binding(cops_context_id_t *ctxp, const uint8_t *buf,
                         uint32_t blen, const struct cops_digest *mac)
{
    (void) ctxp;
    (void) buf;
    (void) blen;
    (void) mac;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;

}

cops_return_code_t
cops_verify_signed_header(cops_context_id_t *ctxp, const uint8_t *header,
                          enum cops_payload_type pltype,
                          struct cops_vsh *hinfo)
{
    (void) ctxp;
    (void) header;
    (void) pltype;
    (void) hinfo;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t
cops_calcdigest(cops_context_id_t *ctxp, enum cops_hash_type ht,
                const uint8_t *data, size_t datalen,
                const struct cops_digest *hash)
{
    (void) ctxp;
    (void) ht;
    (void) data;
    (void) datalen;
    (void) hash;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;

}

cops_return_code_t
cops_simlock_verify_imsi(cops_context_id_t *ctxp,
                         const struct cops_simlock_imsi *imsi)
{
    (void) ctxp;
    (void) imsi;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_lock_bootpartition(cops_context_id_t *ctxp)
{
    (void) ctxp;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;

}

cops_return_code_t cops_init_arb_table(cops_context_id_t *ctxp,
                                       cops_init_arb_table_args_t *arg)
{
    (void) ctxp;
    (void) arg;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;

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
    (void) ctxp;
    (void) dev_id;
    (void) commercial;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}

cops_return_code_t cops_get_product_debug_settings(cops_context_id_t *ctxp,
                                                   uint32_t *debug_settings)
{
    (void) ctxp;
    (void) debug_settings;
    return COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
}
