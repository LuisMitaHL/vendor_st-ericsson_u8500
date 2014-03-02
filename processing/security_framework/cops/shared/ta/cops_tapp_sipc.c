/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_common.h>
#include <cops_shared.h>
#include <string.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_tapp_sipc_c
#endif

cops_return_code_t
cops_tapp_sipc_get_value_pointer(cops_sipc_message_t *msg, size_t *offset,
                                 uint8_t **value, size_t vlength)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(*offset + vlength <= (COPS_SIPC_DATA_LENGTH(msg)));

    *value = COPS_SIPC_DATA(msg) + *offset;
    *offset += vlength;

function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_sipc_set_opaque(cops_sipc_message_t *msg, size_t *offset,
                          const void *value, size_t vlength)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t  *p;

    COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(msg, offset, &p, vlength));
    memcpy(p, value, vlength);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_sipc_get_opaque(const cops_sipc_message_t *msg, size_t *offset,
                          void *value, size_t vlength)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t  *p;

    COPS_CHK_RC(cops_tapp_sipc_get_value_pointer(COPS_UNCONST(msg), offset,
                &p, vlength));
    memcpy(value, p, vlength);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_sipc_set_bool(cops_sipc_message_t *msg, size_t *offset, bool value)
{
    return cops_tapp_sipc_set_opaque(msg, offset, &value, sizeof(value));
}

cops_return_code_t
cops_tapp_sipc_get_bool(const cops_sipc_message_t *msg, size_t *offset,
                        bool *value)
{
    return cops_tapp_sipc_get_opaque(msg, offset, value, sizeof(*value));
}

cops_return_code_t
cops_tapp_sipc_set_uint32(cops_sipc_message_t *msg, size_t *offset,
                          uint32_t value)
{
    return cops_tapp_sipc_set_opaque(msg, offset, &value, sizeof(value));
}

cops_return_code_t
cops_tapp_sipc_get_uint32(const cops_sipc_message_t *msg, size_t *offset,
                          uint32_t *value)
{
    return cops_tapp_sipc_get_opaque(msg, offset, value, sizeof(*value));
}

cops_return_code_t
cops_tapp_sipc_set_uint16(cops_sipc_message_t *msg, size_t *offset,
                          uint16_t value)
{
    return cops_tapp_sipc_set_opaque(msg, offset, &value, sizeof(value));
}

cops_return_code_t
cops_tapp_sipc_get_uint16(const cops_sipc_message_t *msg, size_t *offset,
                          uint16_t *value)
{
    return cops_tapp_sipc_get_opaque(msg, offset, value, sizeof(*value));
}

cops_return_code_t
cops_tapp_sipc_set_uint8(cops_sipc_message_t *msg, size_t *offset,
                         uint8_t value)
{
    return cops_tapp_sipc_set_opaque(msg, offset, &value, sizeof(value));
}

cops_return_code_t
cops_tapp_sipc_get_uint8(const cops_sipc_message_t *msg, size_t *offset,
                         uint8_t *value)
{
    return cops_tapp_sipc_get_opaque(msg, offset, value, sizeof(*value));
}
