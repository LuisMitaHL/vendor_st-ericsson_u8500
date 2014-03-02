/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/

#include <cops_shared_util.h>
#include <cops_common.h>
#include <ctype.h>
#include <string.h>
#ifdef COPS_OSE_ENVIRONMENT
#include <strings.h>
#endif

/* Definitions used to set meta data of IMSI according to 3GPP TS 31.102 */
#define IMSI_IDENTITY           1 /* 1=IMSI */
#define IMSI_COMPRESSED_LENGTH  8 /* Compressed length (4 bits/digit) */

cops_return_code_t cops_util_unpack_imsi(const uint8_t *const sim_imsi,
                                         uint8_t *const cops_imsi)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (NULL != sim_imsi && NULL != cops_imsi) {
        uint8_t i;
        uint8_t byte;

        cops_imsi[0] = (sim_imsi[1] >> 4) & 0xF;

        for (i = 1; i < (COPS_NUM_IMSI_DIGITS - 1); i += 2) {
            byte = sim_imsi[(i / 2) + 2];

            /* Two digits per byte */
            cops_imsi[i]     = byte & 0xF;
            cops_imsi[i + 1] = (byte >> 4) & 0xF;
        }
    } else {
        ret_code = COPS_RC_ARGUMENT_ERROR;
    }

    return ret_code;
}

cops_return_code_t cops_util_pack_imsi(const uint8_t *const cops_imsi,
                                       uint8_t *const sim_imsi)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (NULL != sim_imsi && NULL != cops_imsi) {
        uint8_t i;

        sim_imsi[0] = IMSI_COMPRESSED_LENGTH;
        sim_imsi[1] = (IMSI_IDENTITY & 0x0F) +
                      ((cops_imsi[0] << 4) & 0xF0);

        for (i = 1; i < (COPS_NUM_IMSI_DIGITS - 1); i += 2) {
            sim_imsi[(i / 2) + 2] = (cops_imsi[i] & 0x0F) +
                                    ((cops_imsi[i+1] << 4) & 0xF0);
        }
    } else {
        ret_code = COPS_RC_ARGUMENT_ERROR;
    }

    return ret_code;
}

cops_return_code_t
cops_util_convert_status(cops_taf_simlock_status_t *taf_status,
                         cops_simlock_status_t *status)
{
    size_t n;
    cops_taf_simlock_lock_status_t *taf_slls;
    cops_simlock_lock_status_t *slls;
    cops_return_code_t ret_code = COPS_RC_OK;

    status->sim_card_status =
        (cops_simlock_card_status_t)(taf_status->sim_card_status);

    for (n = 0; n < (sizeof(cops_taf_simlock_status_t)
                               / sizeof(cops_taf_simlock_lock_status_t)); n++) {
        switch (n) {
        case 0:
            taf_slls = &taf_status->nl_status;
            slls = &status->nl_status;
            break;
        case 1:
            taf_slls = &taf_status->nsl_status;
            slls = &status->nsl_status;
            break;
        case 2:
            taf_slls = &taf_status->spl_status;
            slls = &status->spl_status;
            break;
        case 3:
            taf_slls = &taf_status->cl_status;
            slls = &status->cl_status;
            break;
        case 4:
            taf_slls = &taf_status->siml_status;
            slls = &status->siml_status;
            break;
        case 5:
            taf_slls = &taf_status->esll_status;
            slls = &status->esll_status;
            break;
        default:
            ret_code = COPS_RC_INTERNAL_ERROR;
            goto function_exit;
        }

        slls->lock_type = (cops_simlock_lock_type_t)(taf_slls->lock_type);
        slls->lock_definition = taf_slls->lock_definition;
        slls->lock_setting =
            (cops_simlock_lock_setting_t)(taf_slls->lock_setting);
        slls->attempts_left = taf_slls->attempts_left;
        slls->timer_is_running = taf_slls->timer_is_running;
        slls->time_left_on_running_timer =
            taf_slls->time_left_on_running_timer;
        slls->timer_attempts_left = taf_slls->timer_attempts_left;
    }

function_exit:
    return ret_code;
}

cops_return_code_t
cops_util_bp_pad_control_keys(cops_simlock_control_keys_t *simlock_keys)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_RC(cops_util_bp_pad_control_key(&simlock_keys->nl_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&simlock_keys->nsl_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&simlock_keys->spl_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&simlock_keys->cl_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));
    COPS_CHK_RC(cops_util_bp_pad_control_key(&simlock_keys->esll_key,
                                           COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH));

function_exit:
    return ret_code;
}

cops_return_code_t cops_util_bp_pad_control_key(cops_simlock_control_key_t *key,
                                                uint8_t minlen)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t n;
    size_t l = strnlen(key->value, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);

    if (l < minlen) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Too short control key\n");
    }

    /*lint -e557 Unrecognized format %zu used in linux environment */
    for (n = 0; n < l; n++) {
        if (!isdigit(key->value[n]))
            COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                        "Invalid control key digit at pos %zu\n", n);
    }
    /* lint +e557 */

    /* Pad control key with 0xff */
    memset(key->value + l, 0xff, sizeof(key->value) - l);
function_exit:
    return ret_code;
}
