/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_data.h>
#include <cops_shared.h>
#include <cops_common.h>
#include <string.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_data_sipc_c
#endif

cops_return_code_t
cops_data_get_sipc_message(cops_data_t *cd, cops_sipc_message_t **msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_parameter_data_t d;
    uint8_t csender;

    d.id = COPS_ARGUMENT_ID_SIPC;
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK_ASSERTION(d.data != NULL);

    COPS_CHK_ASSERTION(d.length >=
                       sizeof(cops_sipc_message_t) + COPS_SIPC_MAC_SIZE);

    csender = cops_sipc_get_sender_from_msg((cops_sipc_message_t *)d.data);

    COPS_CHK_RC(cops_sipc_alloc_message((cops_return_code_t)ret_code,
          (size_t)(d.length - sizeof(cops_sipc_message_t) - COPS_SIPC_MAC_SIZE),
          COPS_SIPC_READ_UNKNOWN, csender, msg));
    /* TODO handle endianess of msg->size vs. MAC verification */
    memcpy(*msg, d.data, d.length);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_set_sipc_message(cops_data_t *cd, cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_parameter_data_t d;

    COPS_CHK_ASSERTION(msg->length >=
                       sizeof(cops_sipc_message_t) + COPS_SIPC_MAC_SIZE);

    d.id = COPS_ARGUMENT_ID_SIPC;
    d.data = (uint8_t *)msg;
    d.length = msg->length;
    COPS_CHK_RC(cops_data_set_parameter(cd, &d));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_clear_args_rvs(cops_data_t *rvs)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (rvs == NULL || rvs->data == NULL) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    memset(rvs->data, 0, rvs->max_length);

    return ret_code;
}
