/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_data.h>
#include <cops_common.h>
#include <string.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_data_lock_c
#endif

#define LOCK_MODE_OFFSET 0

cops_return_code_t
cops_data_lock_type_to_setting_paramid(cops_simlock_lock_type_t lt,
                                       cops_parameter_id_t *id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(id != NULL);
    COPS_CHK_ASSERTION(lt <= COPS_SIMLOCK_LOCK_TYPE_TESTSIM);
    *id = lt + COPS_PARAMETER_ID_LOCK_SETTING_NL;

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_lock_type_to_control_paramid(cops_simlock_lock_type_t lt,
                                       cops_parameter_id_t *id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(id != NULL);
    COPS_CHK_ASSERTION(lt <= COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK);
    *id = lt + COPS_PARAMETER_ID_LOCK_CONTROL_NL;

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_lock_type_to_definition_paramid(cops_simlock_lock_type_t lt,
        cops_parameter_id_t *id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(id != NULL);
    COPS_CHK_ASSERTION(lt <= COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK);
    *id = lt + COPS_PARAMETER_ID_LOCK_DEFINITION_NL;

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_lock_type_to_key_paramid(cops_simlock_lock_type_t lt,
                                   cops_parameter_id_t *id)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_CHK_ASSERTION(id != NULL);
    COPS_CHK_ASSERTION(lt <= COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK);
    *id = lt * 2 + COPS_PARAMETER_ID_SIMLOCK_KEY_NL;

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_get_lock_definition(const cops_data_t *cd,
                              cops_simlock_lock_type_t lt,
                              uint32_t *lock_def)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    COPS_CHK_RC(cops_data_lock_type_to_definition_paramid(lt, &d.id));
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK(d.length == sizeof(*lock_def), COPS_RC_DATA_TAMPERED_ERROR);
    memcpy(lock_def, d.data, sizeof(*lock_def));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_set_lock_definition(cops_data_t *cd,
                              cops_simlock_lock_type_t lt,
                              const uint32_t lock_def)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    COPS_CHK_RC(cops_data_lock_type_to_definition_paramid(lt, &d.id));
    d.data = (uint8_t *) &lock_def;
    d.length = sizeof(lock_def);
    COPS_CHK_RC(cops_data_set_parameter(cd, &d));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_get_lock_setting(const cops_data_t *cd,
                           cops_simlock_lock_type_t lt,
                           uint8_t *lock_setting)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    cops_data_lock_mode_t lm;

    COPS_CHK_RC(cops_data_lock_type_to_setting_paramid(lt, &d.id));
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK(d.length >= 1, COPS_RC_DATA_TAMPERED_ERROR);
    /* The first byte of the lock setting data in cd is LockMode */
    lm = (cops_data_lock_mode_t)(d.data[LOCK_MODE_OFFSET]);

    switch (lm) {
    case COPS_DATA_LOCK_MODE_UNLOCKED:
        *lock_setting = (uint8_t)COPS_SIMLOCK_LOCK_SETTING_UNLOCKED;
        break;
    case COPS_DATA_LOCK_MODE_LOCKED:
        *lock_setting = (uint8_t)COPS_SIMLOCK_LOCK_SETTING_LOCKED;
        break;
    case COPS_DATA_LOCK_MODE_DISABLED:
        *lock_setting = (uint8_t)COPS_SIMLOCK_LOCK_SETTING_DISABLED;
        break;
    case COPS_DATA_LOCK_MODE_AUTOLOCK_ENABLED:
        *lock_setting = (uint8_t)COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED;
        break;
    case COPS_DATA_LOCK_MODE_AUTOLOCK_TO_CNL_ENABLED:
        *lock_setting =
            (uint8_t)COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED;
        break;
    default:
        ret_code = COPS_RC_INTERNAL_ERROR;
        break;
    }

function_exit:
    return ret_code;

}

cops_return_code_t
cops_data_get_lock_control(const cops_data_t *cd,
                           cops_simlock_lock_type_t lt,
                           cops_data_lock_control_t *ctrl)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    COPS_CHK_RC(cops_data_lock_type_to_control_paramid(lt, &d.id));
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK(d.length == sizeof(cops_data_lock_control_t),
             COPS_RC_DATA_TAMPERED_ERROR);
    memcpy(ctrl, d.data, d.length);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_set_lock_control(cops_data_t *cd, cops_simlock_lock_type_t lt,
                           const cops_data_lock_control_t *ctrl)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    COPS_CHK_RC(cops_data_lock_type_to_control_paramid(lt, &d.id));
    d.data = (uint8_t *) ctrl;
    d.length = sizeof(cops_data_lock_control_t);
    COPS_CHK_RC(cops_data_set_parameter(cd, &d));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_get_mck_attempts(const cops_data_t *cd, mck_attempts_t *attempts)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    d.id = COPS_STORAGE_PARAMETER_ID_MCK_ATTEMPTS;
    COPS_CHK_RC(cops_data_get_parameter(cd, &d));
    COPS_CHK(d.length == sizeof(mck_attempts_t), COPS_RC_DATA_TAMPERED_ERROR);
    memcpy(attempts, d.data, d.length);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_set_mck_attempts(cops_data_t *cd, const mck_attempts_t *attempts)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    cops_parameter_data_t d;

    d.id = COPS_STORAGE_PARAMETER_ID_MCK_ATTEMPTS;
    d.data = (uint8_t *) attempts;
    d.length = sizeof(mck_attempts_t);
    COPS_CHK_RC(cops_data_set_parameter(cd, &d));

function_exit:
    return ret_code;
}
