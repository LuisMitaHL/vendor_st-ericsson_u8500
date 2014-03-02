/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tapp_common.h>
#include <cops_common.h>

#define COPS_CHALLENGE_VERSION (0x00000001)

static cops_return_code_t create_default_tapp_io(cops_tapp_io_t *tapp_io,
        cops_return_code_t return_code)
{
    cops_return_code_t   ret_code = COPS_RC_OK;
    size_t               offset   = 0;
    cops_sipc_message_t *msg      = NULL;

    COPS_CHK_RC(cops_sipc_alloc_message(return_code,
                                        sizeof(return_code),
                                        0,
                                        cops_sipc_get_sender(),
                                        &msg));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, ret_code));
    COPS_CHK_RC(cops_data_set_sipc_message(&tapp_io->rvs, msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;

}

static cops_return_code_t create_default_nok_tapp_io(cops_tapp_io_t *tapp_io)
{
    cops_return_code_t   ret_code = COPS_RC_OK;
    COPS_CHK_RC(create_default_tapp_io(tapp_io,
                                       COPS_RC_SERVICE_NOT_AVAILABLE_ERROR));
function_exit:
    return ret_code;
}

static cops_return_code_t create_default_ok_tapp_io(cops_tapp_io_t *tapp_io)
{
    cops_return_code_t   ret_code = COPS_RC_OK;
    COPS_CHK_RC(create_default_tapp_io(tapp_io, COPS_RC_OK));
function_exit:
    return ret_code;
}

static cops_return_code_t create_get_challenge_tapp_io(cops_tapp_io_t *tapp_io)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;
    cops_sipc_message_t *msg = NULL;
    uint8_t challenge[COPS_CHALLENGE_SIZE];
    uint32_t version = COPS_CHALLENGE_VERSION;
    uint8_t die_id[] = { 0x01, 0x02, 0x03, 0x04,
                         0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c
                       };
    uint8_t data[] = { 0x73, 0x80, 0x55, 0x7c,
                       0xf4, 0x4e, 0x4f, 0xfe,
                       0xd5, 0xc7, 0x22, 0xd0,
                       0xaa, 0xe2, 0x76, 0x6a,
                       0xab, 0x18, 0x19, 0x47,
                       0xee, 0x94, 0x37, 0xc9,
                       0x93, 0x9d, 0xe0, 0x85,
                       0xa2, 0x0c, 0x86, 0x16
                     };

    memcpy(challenge, &version, sizeof(version));
    memcpy(challenge + 4, die_id, 12);
    memcpy(challenge + 16, data, 32);

    COPS_CHK_RC(cops_sipc_alloc_message(ret_code,
                                        sizeof(ret_code) +
                                        COPS_CHALLENGE_SIZE,
                                        0,
                                        cops_sipc_get_sender(),
                                        &msg));
    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, ret_code));
    COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, challenge,
                                          COPS_CHALLENGE_SIZE));
    COPS_CHK_RC(cops_data_set_sipc_message(&tapp_io->rvs, msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;

}

static cops_return_code_t create_read_imei_tapp_io(cops_tapp_io_t *tapp_io)
{
    cops_return_code_t  ret_code = COPS_RC_OK;
    cops_return_code_t  return_code = COPS_RC_OK;
    size_t offset = 0;
    cops_sipc_message_t *msg = NULL;
    uint8_t imei_padding[4] = {0};
    cops_imei_t unpacked_imei;
    cops_data_imei_t packed_imei;

    COPS_CHK_RC(cops_data_get_imei(&tapp_io->data, &packed_imei));
    util_cops_unpack_imei(&packed_imei, &unpacked_imei);

    COPS_CHK_RC(cops_sipc_alloc_message(return_code, sizeof(return_code) +
                                        sizeof(imei_padding) +
                                        sizeof(unpacked_imei) + 1 /* padding */,
                                        COPS_SIPC_READ_IMEI_MRESP,
                                        cops_sipc_get_sender(),
                                        &msg));

    COPS_CHK_RC(cops_tapp_sipc_set_uint32(msg, &offset, return_code));

    if (return_code == COPS_RC_OK) {
        /* For now, we only support one IMEI */
        imei_padding[0] = 1;
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, imei_padding,
                                              sizeof(imei_padding)));
        COPS_CHK_RC(cops_tapp_sipc_set_opaque(msg, &offset, &unpacked_imei,
                                              sizeof(unpacked_imei)));
        /* Use any of the padding parts that contain a 0 */
        COPS_CHK_RC(cops_tapp_sipc_set_uint8(msg, &offset, imei_padding[2]));
    }

    COPS_CHK_RC(cops_data_set_sipc_message(&tapp_io->rvs, msg));

function_exit:
    cops_sipc_free_message(&msg);
    return ret_code;
}

cops_return_code_t
cops_tapp_invoke(cops_state_t *state, cops_tapp_io_t **tapp_io,
                 cops_taf_id_t taf_id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_LOG(LOG_INFO, "Invoke stubbed command (%u)\n", taf_id);

    switch (taf_id) {
    case COPS_SERVICE_GET_CHALLENGE:
        COPS_LOG(LOG_INFO, "Stubbed get challange\n");
        COPS_CHK_RC(create_get_challenge_tapp_io(*tapp_io));
        break;
    case COPS_SERVICE_READ_IMEI:
        COPS_LOG(LOG_INFO, "Stubbed read imei\n");
        COPS_CHK_RC(create_read_imei_tapp_io(*tapp_io));
        break;
    case COPS_SERVICE_BIND_PROPERTIES:
        COPS_LOG(LOG_INFO, "Stubbed bind properties\n");
        COPS_CHK_RC(create_default_ok_tapp_io(*tapp_io));
        break;
    case COPS_SERVICE_AUTHENTICATE:
        COPS_LOG(LOG_INFO, "Stubbed authenticate\n");
        COPS_CHK_RC(create_default_ok_tapp_io(*tapp_io));
        break;
        /* Fall through for the following services
         * that are NOT used by loaders */
    case COPS_SERVICE_SIMLOCK_GET_STATUS:
    case COPS_SERVICE_SIMLOCK_VERIFY_CONTROL_KEYS:
    case COPS_SERVICE_SIMLOCK_UNLOCK:
    case COPS_SERVICE_DEAUTHENTICATE:
#ifdef EXTENDED_SIPC_MSGS
    case COPS_SERVICE_SET_MODEM_BAND:
    case COPS_SERVICE_SIMLOCK_GET_LOCK_SETTINGS:
    case COPS_SERVICE_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT:
#endif
    case COPS_SERVICE_SIMLOCK_LOCK:
#ifdef EXTENDED_SIPC_MSGS
    case COPS_SERVICE_SIMLOCK_MCK_RESET:
#endif
    default:
        COPS_LOG(LOG_WARNING, "Command (%u) not supported, "
                 "returning COPS_RC_SERVICE_NOT_AVAILABLE_ERROR\n", taf_id);
        COPS_CHK_RC(create_default_nok_tapp_io(*tapp_io));
        break;
    }

    COPS_LOG(LOG_INFO, "Invoke stubbed command (%u) done\n", taf_id);

function_exit:
    return ret_code;
}
