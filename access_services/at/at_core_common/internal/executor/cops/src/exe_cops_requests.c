/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "cops.h"
#include "atc_log.h"

static exe_clck_status_t cops_lock2exe_lock(cops_simlock_lock_setting_t lock_setting);
static cops_simlock_lock_setting_t get_lock_setting_for_type(exe_cops_sim_lock_type_t lock_type, cops_simlock_status_t *simlock_status_p);

/*
 * All COPS symbols that differs between COPS versions,
 * shall be defined here and dynamically loaded in runtime.
 */
#include <dlfcn.h>


static cops_return_code_t (*at_cops_lock_bootpartition)(cops_context_id_t *ctxp) = NULL;

#define AT_COPS_ARB_DATA_TYPE_MODELID 0
typedef struct at_cops_init_arb_table_args {
    int arb_data_type;
    size_t    data_length;
    uint8_t *data;
} at_cops_init_arb_table_args_t;

static cops_return_code_t (*at_cops_init_arb_table)(cops_context_id_t *ctxp,
                                               at_cops_init_arb_table_args_t *arg) = NULL;

enum at_cops_sec_profile_data_type {
    AT_COPS_SEC_PROFILE_DATA_NONE
};

typedef struct at_cops_write_secprofile_args {
    /** @brief Version */
    uint32_t version;

    /** @brief Flags */
    uint32_t flags;

    /** @brief size of the list */
    size_t hashlist_len;

    /** @brief Pointer to the hashes */
    uint8_t *hashlist;

    /** @brief Type of data supplied  */
    enum at_cops_sec_profile_data_type sec_profile_data_type;

    /** @brief Length (in bytes) of following Data */
    size_t data_length;

    /** @brief Pointer to Data */
    uint8_t *data;
} at_cops_write_secprofile_args_t;

static cops_return_code_t (*at_cops_write_secprofile)(cops_context_id_t *ctxp,
    at_cops_write_secprofile_args_t *arg) = NULL;

void __load_dynamic_cops_symbols() {

    at_cops_lock_bootpartition = dlsym(RTLD_NEXT, "cops_lock_bootpartition");

    at_cops_init_arb_table = dlsym(RTLD_NEXT, "cops_init_arb_table");

    at_cops_write_secprofile = dlsym(RTLD_NEXT, "cops_write_secprofile");
}

exe_cmee_error_t lookup_cops_cmee_error_code(cops_simlock_lock_type_t lock_type, cops_return_code_t cops_result)
{
    if (cops_result == COPS_RC_OK) {
        return CMEE_OK;
    } else if (cops_result == COPS_RC_INCORRECT_SIM) {
        return CMEE_SIM_FAILURE;
    }

    switch (lock_type) {
        case EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY) {
                return CMEE_PH_NET_PIN_REQUIRED;
            } else if (cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                /* When number of attempts reaches 0 a timer is set in COPS
                 * that when it expires restores number of attempts to 1. */
                return CMEE_PH_NET_PIN_REQUIRED;
            }
        break;
        case EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY) {
                return CMEE_PH_NETSUB_PIN_REQUIRED;
            } else if (cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                return CMEE_PH_NETSUB_PUK_REQUIRED;
            }
        break;
        case EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY) {
                return CMEE_PH_SP_PIN_REQUIRED;
            } else if (cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                return CMEE_PH_SP_PUK_REQUIRED;
            }
        break;
        case EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY) {
                return CMEE_PH_CORP_PIN_REQUIRED;
            } else if (cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                return CMEE_PH_CORP_PUK_REQUIRED;
            }
        break;
        case EXE_COPS_SIM_LOCK_TYPE_USER_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY) {
                return CMEE_PH_SIMLOCK_PIN_REQUIRED;
            } else if (cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                return CMEE_INCORRECT_PASSWORD;
            }
        break;
        case EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK:
            if (cops_result == COPS_RC_INVALID_SIMLOCK_KEY ||
                cops_result == COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT) {
                return CMEE_INCORRECT_PASSWORD;
            }
        break;
        default:

        break;
    }

    return CMEE_UNKNOWN;
}

exe_request_result_t request_imei(exe_request_record_t *record_p)
{
    cops_return_code_t result = 0;
    int i;
    char *imei_p = record_p->request_data_p;
    cops_imei_t imei;
    cops_context_id_t *id_p;
    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        goto error;
    }

    if (NULL == record_p->request_data_p) {
        goto error;
    }

    imei_p = &(((exe_cgsn_response_t *)record_p->request_data_p)->imei[0]);
    ATC_LOG_I("request_imei");
    result = cops_read_imei(id_p, &imei);

    if (COPS_RC_OK != result) {
        goto error;
    }

    for (i = 0; i < COPS_UNPACKED_IMEI_LENGTH; i++) {
        if (imei.digits[i] > 9) {
            goto error;
        }

        *(imei_p + i) = '0' + imei.digits[i];
    }

    *(imei_p + COPS_UNPACKED_IMEI_LENGTH) = '\0';

    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}

/*
 * Enable / query personalization locks
 */
exe_request_result_t request_handle_facility_personalization_lock(exe_request_record_t *record_p,
                                                                  exe_cops_facility_personalization_data_t *lock_data_p,
                                                                  cops_simlock_lock_setting_t *lock_setting_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));
    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    switch (lock_data_p->operation) {
    case EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_QUERY_STATUS: {
        cops_simlock_status_t simlock_status;
        result = cops_simlock_get_status(id_p, &simlock_status);
        if (COPS_RC_OK != result) {
            goto error;
        }

        if (NULL != lock_setting_p) {
            *lock_setting_p = get_lock_setting_for_type(lock_data_p->sim_lock_type, &simlock_status);
        }

        break;
    }
    case EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_UNLOCK: {

        cops_simlock_control_key_t control_key;
        cops_simlock_lock_type_t lock_type;

        memset(&control_key, 0, sizeof(cops_simlock_control_key_t));
        strncpy(control_key.value, lock_data_p->passwd_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);

        switch (lock_data_p->sim_lock_type) {
            case EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
                break;
            case EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
                break;
            case EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK;
                break;
            case EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
                break;
            case EXE_COPS_SIM_LOCK_TYPE_USER_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_USER_LOCK;
                break;
            case EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK:
                lock_type = COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK;
                break;
            default:
                ATC_LOG_E("%s, unknown sim lock type: %d", __func__, lock_data_p->sim_lock_type);
                goto error;
        }

        result = cops_simlock_unlock(id_p, lock_type, &control_key);

        lock_data_p->error_code = lookup_cops_cmee_error_code(lock_data_p->sim_lock_type, result);

        if (COPS_RC_OK != result) {
            goto error;
        }

        break;
    }

    case EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_LOCK:
    default:
        ATC_LOG_E("%s, unsupported operation: %d", __func__, lock_data_p->operation);
        goto error;
        break;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;

}

static cops_simlock_lock_setting_t get_lock_setting_for_type(exe_cops_sim_lock_type_t lock_type, cops_simlock_status_t *simlock_status_p)
{
    cops_simlock_lock_setting_t lock_setting;
    switch (lock_type) {
    case EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK:
        lock_setting = simlock_status_p->nl_status.lock_setting;
        break;
    case EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK:
        lock_setting = simlock_status_p->nsl_status.lock_setting;
        break;
    case EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK:
        lock_setting = simlock_status_p->spl_status.lock_setting;
        break;
    case EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK:
        lock_setting = simlock_status_p->cl_status.lock_setting;
        break;
    case EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK:
        lock_setting = simlock_status_p->esll_status.lock_setting;
        break;
    default:
        return -1;
    }
    return lock_setting;
}

static exe_clck_status_t cops_lock2exe_lock(cops_simlock_lock_setting_t lock_setting)
{
    exe_clck_status_t exe_lock = EXE_CLCK_STATUS_NOT_AVAILABLE;

    switch (lock_setting) {
    case COPS_SIMLOCK_LOCK_SETTING_UNLOCKED:
    case COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED:
    case COPS_SIMLOCK_LOCK_SETTING_DISABLED:
    case COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED:
        exe_lock = EXE_CLCK_STATUS_NOT_ACTIVE;
        break;
    case COPS_SIMLOCK_LOCK_SETTING_LOCKED:
    case COPS_SIMLOCK_LOCK_SETTING_AUTO:
        exe_lock = EXE_CLCK_STATUS_ACTIVE;
        break;

    default:
        exe_lock = EXE_CLCK_STATUS_NOT_AVAILABLE;
        break;
    }

    return exe_lock;

}

exe_cops_facility_personalization_operation_t convert_clck_mode_to_facility_operation(exe_clck_mode_t mode)
{
    switch (mode) {
        case EXE_CLCK_MODE_QUERY_STATUS:
            return EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_QUERY_STATUS;
        case EXE_CLCK_MODE_UNLOCK:
             return EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_UNLOCK;
        case EXE_CLCK_MODE_LOCK:
             return EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_LOCK;
        default:
             return EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_NO_OP;
    }
}

exe_request_result_t handle_clck_set_query_request(exe_request_record_t *record_p, exe_cops_sim_lock_type_t lock_type)
{
    exe_cops_facility_personalization_data_t data;
    exe_clck_t *clck_data_p = NULL;
    exe_cops_facility_personalization_operation_t operation;
    cops_simlock_lock_setting_t lock_setting;
    exe_request_result_t exe_request_result;

    clck_data_p = (exe_clck_t *) record_p->request_data_p;

    if (NULL == clck_data_p) {
        ATC_LOG_E("%s: clck_data_p is null", __func__);
        return EXE_FAILURE;
    }

    memset(&data, 0, sizeof(data));
    data.operation = convert_clck_mode_to_facility_operation(clck_data_p->mode);
    data.sim_lock_type = lock_type;
    data.passwd_p = clck_data_p->passwd_p;

    if (EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_NO_OP == data.operation) {
        ATC_LOG_E("%s: data.operation conversion failure", __func__);
        return EXE_FAILURE;
    }

    exe_request_result = request_handle_facility_personalization_lock(record_p,
                                                                      &data,
                                                                      &lock_setting);

    if (data.operation == EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_QUERY_STATUS) {
        clck_data_p->lock_status = cops_lock2exe_lock(lock_setting);
    }

    return exe_request_result;
}

exe_request_result_t request_set_query_net_lock(exe_request_record_t *record_p)
{
    return handle_clck_set_query_request(record_p, EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK);
}

exe_request_result_t request_set_query_net_sub_lock(exe_request_record_t *record_p)
{
    return handle_clck_set_query_request(record_p, EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK);
}

exe_request_result_t request_set_query_service_lock(exe_request_record_t *record_p)
{
    return handle_clck_set_query_request(record_p, EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK);
}

exe_request_result_t request_set_query_corporate_lock(exe_request_record_t *record_p)
{
    return handle_clck_set_query_request(record_p, EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK);
}

exe_request_result_t request_set_query_extended_lock(exe_request_record_t *record_p)
{
    return handle_clck_set_query_request(record_p, EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK);
}

exe_request_result_t request_unlock_sim_lock(exe_request_record_t *record_p)
{
    return request_handle_facility_personalization_lock(record_p,
                                                        (exe_cops_facility_personalization_data_t *) record_p->request_data_p,
                                                        NULL); /* Lock setting not needed */
}

exe_request_result_t request_get_challenge_data(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    exe_cops_challenge_t *challenge_p = NULL;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    challenge_p = (exe_cops_challenge_t *) record_p->request_data_p;

    if (NULL == challenge_p) {
        ATC_LOG_E("%s: challenge_p is null", __func__);
        return EXE_FAILURE;
    }
    result = cops_get_challenge(id_p, COPS_AUTH_TYPE_RSA_CHALLENGE,
            NULL, &challenge_p->challenge_len);
    if (COPS_RC_OK != result) {
        goto error;
    }
    if (EXE_COPS_MAX_AUTH_CHALLENGE_LEN < challenge_p->challenge_len) {
        ATC_LOG_E("%s: Internal error, internal buffer is to small! Internal buffer: %d, needed buffer: %u",
                __func__, EXE_COPS_MAX_AUTH_CHALLENGE_LEN, (uint32_t)challenge_p->challenge_len);
        goto error;
    }

    result = cops_get_challenge(id_p, COPS_AUTH_TYPE_RSA_CHALLENGE,
            &challenge_p->rsa_challenge_data[0], &challenge_p->challenge_len);
    if (COPS_RC_OK != result) {
        goto error;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_authenticate(exe_request_record_t *record_p)
{
    exe_cops_auth_data_t *exe_auth_data_p = NULL;
    cops_auth_data_t cops_auth_data;
    cops_simlock_control_keys_t sim_lock_keys;
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;


    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    exe_auth_data_p = (exe_cops_auth_data_t *) record_p->request_data_p;

    if (NULL == exe_auth_data_p) {
        ATC_LOG_E("%s: auth_data_p is null", __func__);
        goto error;
    }
    switch (exe_auth_data_p->auth_type) {
    case EXE_COPS_AUTH_TYPE_SIMLOCK:
        memset(&sim_lock_keys, 0, sizeof(cops_simlock_control_keys_t));
        if (exe_auth_data_p->nl_key_p) {
            strncpy(sim_lock_keys.nl_key.value, exe_auth_data_p->nl_key_p,
                    COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
        }
        if (exe_auth_data_p->nsl_key_p) {
            strncpy(sim_lock_keys.nsl_key.value, exe_auth_data_p->nsl_key_p,
                    COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
        }
        if (exe_auth_data_p->spl_key_p) {
            strncpy(sim_lock_keys.spl_key.value, exe_auth_data_p->spl_key_p,
                    COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
        }
        if (exe_auth_data_p->cl_key_p) {
            strncpy(sim_lock_keys.cl_key.value, exe_auth_data_p->cl_key_p,
                    COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
        }
        if (exe_auth_data_p->esl_key_p) {
            strncpy(sim_lock_keys.esll_key.value, exe_auth_data_p->esl_key_p,
                    COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
        }
        cops_auth_data.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
        cops_auth_data.data = (uint8_t*) &sim_lock_keys;
        cops_auth_data.length = sizeof(sim_lock_keys);
        break;
    case EXE_COPS_AUTH_TYPE_RSA:
        cops_auth_data.auth_type = COPS_AUTH_TYPE_RSA_CHALLENGE;
        cops_auth_data.length = exe_auth_data_p->rsa_size;
        cops_auth_data.data = exe_auth_data_p->rsa_challenge_response_p;
        break;

    default:
        ATC_LOG_E("%s, unknown auth type: %d", __func__, exe_auth_data_p->auth_type);
        goto error;
        break;
    }

    result = cops_authenticate(id_p, false, &cops_auth_data);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, authentication failed: %d", __func__, result);
        goto error;
    }
    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_deauthenticate(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }
    result = cops_deauthenticate(id_p, false);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, de-authentication failed: %d", __func__, result);
        goto error;
    }
    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_set_imei(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    exe_cops_imei_t *exe_imei_p;
    cops_bind_properties_arg_t bind_properties;
    cops_imei_t cops_imei;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    exe_imei_p = (exe_cops_imei_t *) record_p->request_data_p;

    if (NULL == exe_imei_p) {
        ATC_LOG_E("%s: exe_imei_p is null", __func__);
        goto error;
    }
    memcpy(cops_imei.digits, exe_imei_p->imei, COPS_UNPACKED_IMEI_LENGTH);
    bind_properties.imei = &cops_imei;
    bind_properties.auth_data = NULL;
    bind_properties.cops_data = NULL;
    bind_properties.cops_data_length = 0;
    bind_properties.merge_cops_data = true;
    bind_properties.num_new_auth_data = 0;

    result = cops_bind_properties(id_p, &bind_properties);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, cops_bind_properties failed: %d", __func__, result);
        goto error;
    }
    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_set_simlock_keys(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    exe_cops_simlock_keys_t *exe_simlock_p;
    cops_bind_properties_arg_t bind_properties;
    cops_auth_data_t cops_auth_data;
    cops_simlock_control_keys_t cops_keys;


    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    exe_simlock_p = (exe_cops_simlock_keys_t *) record_p->request_data_p;

    if (NULL == exe_simlock_p) {
        ATC_LOG_E("%s: exe_simlock_p is null", __func__);
        goto error;
    }
    if(!exe_simlock_p->nl_key_p ||
       !exe_simlock_p->nsl_key_p ||
       !exe_simlock_p->spl_key_p ||
       !exe_simlock_p->cl_key_p ||
       !exe_simlock_p->esl_key_p) {
        ATC_LOG_E("%s, input is NULL", __func__);
        goto error;
    }
    strncpy(cops_keys.nl_key.value, exe_simlock_p->nl_key_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    strncpy(cops_keys.nsl_key.value, exe_simlock_p->nsl_key_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    strncpy(cops_keys.spl_key.value, exe_simlock_p->spl_key_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    strncpy(cops_keys.cl_key.value, exe_simlock_p->cl_key_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    strncpy(cops_keys.esll_key.value, exe_simlock_p->esl_key_p, COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);

    cops_auth_data.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    cops_auth_data.data = (uint8_t*)&cops_keys;
    cops_auth_data.length = sizeof(cops_keys);

    bind_properties.auth_data = &cops_auth_data;
    bind_properties.cops_data = NULL;
    bind_properties.cops_data_length = 0;
    bind_properties.merge_cops_data = true;
    bind_properties.num_new_auth_data = 1;
    bind_properties.imei = NULL;

    result = cops_bind_properties(id_p, &bind_properties);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, cops_bind_properties failed: %d", __func__, result);
        goto error;
    }
    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_write_simlock_data(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    exe_cops_simlock_data_t *exe_simlock_data_p;
    cops_bind_properties_arg_t bind_properties;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }

    exe_simlock_data_p = (exe_cops_simlock_data_t*) record_p->request_data_p;

    if (NULL == exe_simlock_data_p) {
        ATC_LOG_E("%s: exe_simlock_data_p is null", __func__);
        goto error;
    }
    bind_properties.auth_data = NULL;
    bind_properties.imei = NULL;
    bind_properties.merge_cops_data = true;
    bind_properties.num_new_auth_data = 0;
    bind_properties.cops_data = exe_simlock_data_p->simlock_data;
    bind_properties.cops_data_length = exe_simlock_data_p->simlock_data_size;

    result = cops_bind_properties(id_p, &bind_properties);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, cops_bind_properties failed: %d", __func__, result);
        goto error;
    }
    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_read_simlock_data(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    exe_cops_simlock_data_t *exe_simlock_data_p = NULL;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("%s, could not get copsclient id.", __func__);
        goto error;
    }
    exe_simlock_data_p = (exe_cops_simlock_data_t*) record_p->request_data_p;

    if (NULL == exe_simlock_data_p) {
        ATC_LOG_E("%s: exe_simlock_data_p is null", __func__);
        goto error;
    }

    result = cops_read_data(id_p, NULL, (size_t *)&exe_simlock_data_p->simlock_data_size);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, cops_read_data failed: %d", __func__, result);
        goto error;
    }

    exe_simlock_data_p->simlock_data = calloc(1, exe_simlock_data_p->simlock_data_size);
    if(!exe_simlock_data_p->simlock_data){
        goto error;
    }

    result = cops_read_data(id_p, exe_simlock_data_p->simlock_data, (size_t *)&exe_simlock_data_p->simlock_data_size);
    if (COPS_RC_OK != result) {
        ATC_LOG_E("%s, cops_read_data failed: %d", __func__, result);
        goto error;
    }

    return EXE_SUCCESS;

error:
    if (exe_simlock_data_p) {
        free(exe_simlock_data_p->simlock_data);
        exe_simlock_data_p->simlock_data = NULL;
    }
    return EXE_FAILURE;
}

exe_request_result_t request_authenticate_eng_mode(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    cops_auth_data_t auth_data;
    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));

    if (NULL == id_p) {
        ATC_LOG_E("Could not get copsclient id.");
        return EXE_FAILURE;
    }

    /* For permanent authentication (engineering mode)
     * the struct is initiated as follows:
     */
    auth_data.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
    auth_data.length = 0;
    auth_data.data = NULL;
    /* Note that engineering mode is triggered through auth_data,
     * not the second parameter of cops_authenticate.
     */
    result = cops_authenticate(id_p, false, &auth_data);

    if (COPS_RC_OK != result) {
        ATC_LOG_E("Cops_authenticate error: %d", result);
        return EXE_FAILURE;
    }

    return EXE_SUCCESS;
}

exe_request_result_t request_deauthenticate_eng_mode(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));
    if (NULL == id_p) {
        ATC_LOG_E("Could not get copsclient id.");
        return EXE_FAILURE;
    }

    result = cops_deauthenticate(id_p, true);

    if (COPS_RC_OK != result) {
        ATC_LOG_E("Cops_deauthenticate error: %d", result);
        return EXE_FAILURE;
    }

    return EXE_SUCCESS;
}

exe_request_result_t request_lock_bootpartition(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;

    if (!at_cops_lock_bootpartition) {
        ATC_LOG_E("The required functionality is not available!");
        return EXE_FAILURE;
    }

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));
    if (NULL == id_p) {
        ATC_LOG_E("Could not get copsclient id.");
        return EXE_FAILURE;
    }

    result = at_cops_lock_bootpartition(id_p);

    if (COPS_RC_OK != result) {
        ATC_LOG_E("Cops_lock_bootpartition error: %d", result);
        return EXE_FAILURE;
    }

    return EXE_SUCCESS;
}

exe_request_result_t request_init_antirollback(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    at_cops_init_arb_table_args_t arb_arg;
    /*
     * For the MODELID type we use an uint16_t but since we need to manipulate
     * (switch) the byte-order its easier to use a byte-vector.
     */
    uint8_t model[2];
    uint16_t tmp_model_id = 0;

    if (!at_cops_init_arb_table) {
        ATC_LOG_E("The required functionality is not available!");
        return EXE_FAILURE;
    }

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));
    if (NULL == id_p) {
        ATC_LOG_E("Could not get copsclient id.");
        return EXE_FAILURE;
    }
    tmp_model_id = *((uint16_t*)record_p->request_data_p);
    model[0] = tmp_model_id & 0x00FF;
    model[1] = tmp_model_id >> 8;
    arb_arg.arb_data_type = AT_COPS_ARB_DATA_TYPE_MODELID;
    arb_arg.data_length = sizeof(uint16_t);
    arb_arg.data = (uint8_t*) model;

    result = at_cops_init_arb_table(id_p, &arb_arg);

    if (COPS_RC_OK != result) {
        ATC_LOG_E("Cops_init_arb_table error: %d", result);
        return EXE_FAILURE;
    }

    return EXE_SUCCESS;
}

exe_request_result_t request_write_d_and_t(exe_request_record_t *record_p)
{
    cops_context_id_t *id_p;
    cops_return_code_t result = COPS_RC_OK;
    at_cops_write_secprofile_args_t secdata_arg;
    /*
     * The data will be delivered in a struct which copies the first
     * parts of cops_write_secdata_args_t,
     * so we'll cast to above type for value checking
     */
    exe_cops_d_and_t_data_t *transfer_data_p =
        (exe_cops_d_and_t_data_t*) record_p->request_data_p;

    if (!at_cops_write_secprofile) {
        ATC_LOG_E("The required functionality is not available!");
        return EXE_FAILURE;
    }

    id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(record_p->exe_p));
    if (NULL == id_p) {
        ATC_LOG_E("Could not get copsclient id.");
        return EXE_FAILURE;
    }

    secdata_arg.version = transfer_data_p->version;
    secdata_arg.flags = transfer_data_p->flags;
    secdata_arg.hashlist_len = transfer_data_p->hashlist_len;
    secdata_arg.hashlist = transfer_data_p->hashlist;
    secdata_arg.sec_profile_data_type = AT_COPS_SEC_PROFILE_DATA_NONE;
    secdata_arg.data_length = 0;
    secdata_arg.data = NULL;

    result = at_cops_write_secprofile(id_p, &secdata_arg);

    if (COPS_RC_OK != result) {
        ATC_LOG_E("Cops_write_secdata error: %d", result);
        return EXE_FAILURE;
    }
    return EXE_SUCCESS;
}

