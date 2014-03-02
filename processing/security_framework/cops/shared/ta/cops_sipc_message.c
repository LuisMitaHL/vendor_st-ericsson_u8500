/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_common.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_sipc_message_c
#endif

#include <cops_shared.h>
#include <stdlib.h>
#include <string.h>

#ifdef COPS_TA
#define COPS_ALOG (void)
#include "issw_printf.h"
#else
#define PUTS (void)
#endif

#ifdef COPS_TAPP_LOCAL
#include <cops_gstate.h>
#endif

uint8_t run_mode = COPS_RUN_MODE_UNKNOWN;

#define SIPC_MAX_MSG_SIZE (4096)

cops_return_code_t
cops_sipc_alloc_message(cops_return_code_t rc, size_t data_length,
                        cops_sipc_mtype_t msg_type, uint8_t csender,
                        cops_sipc_message_t **msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    size_t    len;

    if (rc == COPS_RC_OK) {
        len = sizeof(cops_sipc_message_t) + data_length +
              COPS_SIPC_MAC_SIZE;
    } else {
        len = sizeof(cops_sipc_message_t) + sizeof(uint32_t) +
              COPS_SIPC_MAC_SIZE;
    }

    COPS_DEBUG_CHK_ASSERTION(len < SIPC_MAX_MSG_SIZE);
    COPS_DEBUG_CHK_ASSERTION(msg != NULL);
    COPS_DEBUG_CHK_ASSERTION(*msg == NULL);

    *msg = malloc(len);
    COPS_CHK_ALLOC(*msg);

    memset(*msg, 0, len);
    /* These fields are constants so we access them like this */
    *((uint8_t *)COPS_UNCONST(&(*msg)->msg_type)) = msg_type;

    *((uint8_t *)COPS_UNCONST(&(*msg)->prot_scm)) =
        (csender << 6) | COPS_SIPC_PROTSCM_HMAC_SHA256;

    *((uint16_t *)COPS_UNCONST(&(*msg)->length)) = len;

function_exit:
    return ret_code;
}

void cops_sipc_free_message(cops_sipc_message_t **msg)
{
    if (*msg != NULL) {
        memset(*msg, 0, (*msg)->length); /* Can contain sensitive stuff */
        free(*msg);
        *msg = NULL;
    }
}

enum cops_sender cops_sipc_get_sender_from_msg(const cops_sipc_message_t *msg)
{
    return (enum cops_sender)((msg->prot_scm >> 6) & 0x03);
}

void cops_sipc_clear_sender_in_msg(cops_sipc_message_t *msg)
{
    *((uint8_t *)COPS_UNCONST(&msg->prot_scm)) = COPS_SIPC_PROTSCM_HMAC_SHA256;
}

#if !defined COPS_TAPP_EMUL || defined COPS_TAPP || defined COPS_TAPP_LOCAL

#if defined COPS_TAPP || defined COPS_TAPP_LOCAL
static cops_return_code_t cops_sipc_verify_counter(
                                                 const cops_sipc_message_t *msg,
                                                 cops_sipc_session_t *sess,
                                                 bool internal_counter);
#endif

cops_return_code_t cops_sipc_increment_session(cops_sipc_session_t *sess)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_DEBUG_CHK_ASSERTION(sess != NULL);

    sess->counter++;

    goto function_exit;         /* Remove warning */
function_exit:
    return ret_code;
}

cops_return_code_t
cops_sipc_protect_message(const cops_sipc_session_t *sess,
                          cops_sipc_message_t *msg, bool mac_ok)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t  *mac = COPS_SIPC_MAC(msg);

    COPS_DEBUG_CHK_ASSERTION(sess != NULL);
    COPS_DEBUG_CHK_ASSERTION(msg != NULL);
    COPS_DEBUG_CHK_ASSERTION(mac != NULL);

    msg->counter = sess->counter;

    if (mac_ok) {
        COPS_CHK_RC(cops_mac_calc(COPS_MAC_TYPE_SIPC,
                                  (uint8_t *)msg,
                                  msg->length - COPS_SIPC_MAC_SIZE, mac,
                                  COPS_SIPC_MAC_SIZE));
    }

function_exit:
    return ret_code;
}

cops_return_code_t
cops_sipc_protect_message_with_key(const cops_mac_type_t mac_type,
                                   const cops_sipc_session_t *sess,
                                   cops_sipc_message_t *msg, bool mac_ok)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t  *mac = COPS_SIPC_MAC(msg);

    COPS_DEBUG_CHK_ASSERTION(sess != NULL);
    COPS_DEBUG_CHK_ASSERTION(msg != NULL);
    COPS_DEBUG_CHK_ASSERTION(mac != NULL);

    msg->counter = sess->counter;

    if (mac_ok) {
        COPS_CHK_RC(cops_mac_calc(mac_type,
                                  (uint8_t *)msg,
                                  msg->length - COPS_SIPC_MAC_SIZE, mac,
                                  COPS_SIPC_MAC_SIZE));
    }

function_exit:
    return ret_code;
}

#if defined COPS_TAPP || defined COPS_TAPP_LOCAL

cops_return_code_t
cops_sipc_verify_message(const cops_sipc_message_t *msg,
                         cops_sipc_session_t *sess, bool *mac_ok,
                         bool internal_counter)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    const uint8_t *mac = COPS_SIPC_CONST_MAC(msg);

    COPS_DEBUG_CHK_ASSERTION(sess != NULL);

    COPS_DEBUG_CHK_ASSERTION(msg != NULL);
    COPS_DEBUG_CHK_ASSERTION(mac != NULL);

    COPS_CHK_RC(cops_mac_verify(COPS_MAC_TYPE_SIPC,
                                (const uint8_t *)msg,
                                msg->length - COPS_SIPC_MAC_SIZE, mac,
                                COPS_SIPC_MAC_SIZE, mac_ok));

    /* skip counter check if message originated from swbp */
    if (mac_ok != NULL && !*mac_ok) {
        return COPS_RC_OK;
    }

    COPS_CHK_RC(cops_sipc_verify_counter(msg, sess, internal_counter));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_sipc_verify_message_with_key(const cops_mac_type_t mac_type,
                                  const cops_sipc_message_t *msg,
                                  cops_sipc_session_t *sess, bool *mac_ok,
                                  bool internal_counter)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    const uint8_t *mac = COPS_SIPC_CONST_MAC(msg);

    COPS_DEBUG_CHK_ASSERTION(sess != NULL);

    COPS_DEBUG_CHK_ASSERTION(msg != NULL);
    COPS_DEBUG_CHK_ASSERTION(mac != NULL);

    COPS_CHK_RC(cops_mac_verify(mac_type,
                                (const uint8_t *)msg,
                                msg->length - COPS_SIPC_MAC_SIZE, mac,
                                COPS_SIPC_MAC_SIZE, mac_ok));

    /* skip counter check if message originated from swbp */
    if (mac_ok != NULL && !*mac_ok) {
        return COPS_RC_OK;
    }

    COPS_CHK_RC(cops_sipc_verify_counter(msg, sess, internal_counter));

function_exit:
    return ret_code;
}

/*
 * This function checks if the counter in the message is OK with respect
 * to its incrementation. Request messages should always be +1 from the
 * last received one, and reply messages should always be equal to the value
 * sent to the modem.
 *
 * This function assumes that messages are sent and received synchronous,
 * that is; If a message is sent, it is assumed to be replied (or not at all).
 * There will not be another message received during that wait (except for
 * the verifyIMSI case).
 *
 */
static
cops_return_code_t cops_sipc_verify_counter(const cops_sipc_message_t *msg,
                                            cops_sipc_session_t *sess,
                                            bool internal_counter)
{
    uint32_t expected_counter;
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_session_t *gstate_session = NULL;

    /*
     * The following messages use ape counter.
     * The rest use modem counter
     */
    if (COPS_SIPC_READ_SIM_DATA_HREQ == msg->msg_type ||
        COPS_SIPC_READ_SIM_DATA_HRESP == msg->msg_type) {

        if (internal_counter) {
            gstate_session =
                ISSWAPI_GSTATE_GET(TEE_GID_COPS_SIPC_APE_COUNTER_INTERNAL);
        } else {
            gstate_session = ISSWAPI_GSTATE_GET(TEE_GID_COPS_SIPC_APE_COUNTER);
        }
    } else {

        if (internal_counter) {
            gstate_session =
                ISSWAPI_GSTATE_GET(TEE_GID_COPS_SIPC_MODEM_COUNTER_INTERNAL);
        } else {
            gstate_session =
                ISSWAPI_GSTATE_GET(TEE_GID_COPS_SIPC_MODEM_COUNTER);
        }
    }

    COPS_CHK(gstate_session != NULL, COPS_RC_ISSW_ERROR);

    expected_counter = gstate_session->counter;
    /* Requests are expected to be +1, responses unchanged */
    if (COPS_SIPC_IS_REQUEST(msg)) {
        expected_counter++;
    }

    /* assume gstate_counter is incremented by one */
    if (msg->counter == expected_counter) {
        /* store new counter */
        gstate_session->counter = msg->counter;
    } else {
        /* invalid counter */
        COPS_SET_RC(COPS_RC_SERVICE_ERROR, "Invalid counter. Got %u"
                    ", expected %u\n", (unsigned int) msg->counter,
                    (unsigned int) expected_counter);
    }

function_exit:
    if (gstate_session != NULL) {
        sess->counter = gstate_session->counter;
    }

    return ret_code;
}

#endif /* #if defined COPS_TAPP || defined COPS_TAPP_LOCAL */

#endif /* #if defined COPS_TAPP */

#ifndef COPS_TAPP_EMUL
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD

#ifndef COPS_TA
#ifdef COPS_USE_GP_TEEV0_17
void convertTeeOp2CopsIO_n2s(TEEC_Operation *param, cops_tapp_io_t *cops_io)
{
    /* Update data pointers */
    cops_io->args.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t);
    cops_io->data.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE;
    cops_io->rvs.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE +
        COPS_TAPP_IO_DATA_SIZE;
    cops_io->perm_auth_state_data.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE +
        COPS_TAPP_IO_DATA_SIZE + COPS_TAPP_IO_RVS_SIZE;
}

void convertCopsIO2TeeOp_n2s(cops_tapp_io_t **cops_io, TEEC_Operation *param)
{
    param->flags = TEEC_MEMREF_0_USED;

    param->memRefs[0].flags  = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    param->memRefs[0].buffer = *cops_io;
    param->memRefs[0].size   = sizeof(cops_tapp_io_t) +
        COPS_TAPP_IO_ARGS_SIZE + COPS_TAPP_IO_DATA_SIZE +
        COPS_TAPP_IO_RVS_SIZE + COPS_TAPP_IO_PERM_AUTH_STATE_DATA_SIZE;
}
#else /* COPS_USE_GP_TEEV0_17 */
void convertTeeOp2CopsIO_n2s(TEEC_Operation *param, cops_tapp_io_t *cops_io)
{
    /* Update data pointers */
    cops_io->args.data = (uint8_t *)param->params[0].tmpref.buffer +
        sizeof(cops_tapp_io_t);
    cops_io->data.data = (uint8_t *)param->params[0].tmpref.buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE;
    cops_io->rvs.data = (uint8_t *)param->params[0].tmpref.buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE +
        COPS_TAPP_IO_DATA_SIZE;
    cops_io->perm_auth_state_data.data =
        (uint8_t *)param->params[0].tmpref.buffer + sizeof(cops_tapp_io_t) +
        COPS_TAPP_IO_ARGS_SIZE + COPS_TAPP_IO_DATA_SIZE + COPS_TAPP_IO_RVS_SIZE;
}

void convertCopsIO2TeeOp_n2s(cops_tapp_io_t **cops_io, TEEC_Operation *param)
{
    param->paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);

    param->params[0].tmpref.buffer = *cops_io;
    param->params[0].tmpref.size   = sizeof(cops_tapp_io_t) +
        COPS_TAPP_IO_ARGS_SIZE + COPS_TAPP_IO_DATA_SIZE +
        COPS_TAPP_IO_RVS_SIZE + COPS_TAPP_IO_PERM_AUTH_STATE_DATA_SIZE;
}
#endif
#else /* COPS_TA */
bool convertTeeOp2CopsIO_s2n(TEEC_Operation *param, cops_tapp_io_t *cops_io)
{
    if ((param->flags & TEEC_MEMREF_0_USED) != TEEC_MEMREF_0_USED) {
        PUTS("cnvrt: Wrong param->flags\n");
        return false;
    }

    /* Check that input/output buffers are correctly flagged */
    if ((param->memRefs[0].flags & (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)) !=
        (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)) {
        PUTS("cnvrt: Wrong memrefs[0]");
        return false;
    }

    /* Copy struct and update data pointers */
    memcpy(cops_io, param->memRefs[0].buffer, sizeof(cops_tapp_io_t));
    cops_io->args.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t);
    cops_io->data.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE;
    cops_io->rvs.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE +
        COPS_TAPP_IO_DATA_SIZE;
    cops_io->perm_auth_state_data.data = (uint8_t *)param->memRefs[0].buffer +
        sizeof(cops_tapp_io_t) + COPS_TAPP_IO_ARGS_SIZE +
        COPS_TAPP_IO_DATA_SIZE + COPS_TAPP_IO_RVS_SIZE;

    return true;
}

void convertCopsIO2TeeOp_s2n(cops_tapp_io_t *cops_io, TEEC_Operation *param)
{
    /* Copy struct to update size parameters */
    memcpy(param->memRefs[0].buffer, cops_io, sizeof(cops_tapp_io_t));
}
#endif
#endif
#endif
