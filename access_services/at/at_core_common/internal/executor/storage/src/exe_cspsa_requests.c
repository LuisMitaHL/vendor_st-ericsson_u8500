/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "atc_log.h"
#include "cspsa.h"

#define D_CSPSA_DEFAULT_NAME "CSPSA0"

exe_request_result_t request_write_value(exe_request_record_t *record_p)
{
    exe_cspsa_data_t *exe_cspsa_data_p = NULL;
    CSPSA_Result_t result;
    CSPSA_Handle_t handle;
    exe_request_result_t exe_result = EXE_FAILURE;
    char *parameter_area_name_p = D_CSPSA_DEFAULT_NAME;

    exe_cspsa_data_p = (exe_cspsa_data_t *) record_p->request_data_p;
    if (NULL == exe_cspsa_data_p) {
        ATC_LOG_E("%s: exe_cspsa_data_p is null", __func__);
        return EXE_FAILURE;
    }
    if (NULL != exe_cspsa_data_p->storage) {
        parameter_area_name_p = exe_cspsa_data_p->storage;
    }

    result = CSPSA_Open(parameter_area_name_p, &handle);
    if (result >= T_CSPSA_RESULT_ERRORS) {
        ATC_LOG_E("%s, CSPSA_Open failed: %d", __func__, result);
        return EXE_FAILURE;
    }

    result = CSPSA_WriteValue(handle, (CSPSA_Key_t) exe_cspsa_data_p->key,
            (CSPSA_Size_t) exe_cspsa_data_p->data_len, (CSPSA_Data_t*) exe_cspsa_data_p->value);
    if (result >= T_CSPSA_RESULT_ERRORS) {
        ATC_LOG_E("%s, CSPSA_WriteValue failed: %d", __func__, result);
        goto cspsa_finished;
    }

    exe_result = EXE_SUCCESS;

cspsa_finished:
    CSPSA_Close(&handle);
    return exe_result;
}

exe_request_result_t request_read_value(exe_request_record_t *record_p)
{
    exe_cspsa_data_t *exe_cspsa_data_p = NULL;
    CSPSA_Result_t result;
    CSPSA_Handle_t handle;
    exe_request_result_t exe_result = EXE_SUCCESS;
    char *parameter_area_name_p = D_CSPSA_DEFAULT_NAME;

    exe_cspsa_data_p = (exe_cspsa_data_t *) record_p->request_data_p;
    if (NULL == exe_cspsa_data_p) {
        ATC_LOG_E("%s: exe_cspsa_data_p is null", __func__);
        return EXE_FAILURE;
    }
    if (NULL != exe_cspsa_data_p->storage) {
        parameter_area_name_p = exe_cspsa_data_p->storage;
    }

    result = CSPSA_Open(parameter_area_name_p, &handle);
    if (result >= T_CSPSA_RESULT_ERRORS) {
        ATC_LOG_E("%s, CSPSA_Open failed: %d", __func__, result);
        return EXE_FAILURE;
    }

    result = CSPSA_GetSizeOfValue(handle, exe_cspsa_data_p->key,
            (CSPSA_Size_t*) &exe_cspsa_data_p->data_len);
    if (result >= T_CSPSA_RESULT_ERRORS) {
        ATC_LOG_E("%s, CSPSA_GetSizeOfValue failed: %d", __func__, result);
        goto error;
    }

    exe_cspsa_data_p->value = calloc(1, exe_cspsa_data_p->data_len);
    if(!exe_cspsa_data_p->value){
        goto error;
    }
    result = CSPSA_ReadValue(handle, (CSPSA_Key_t) exe_cspsa_data_p->key,
            (CSPSA_Size_t) exe_cspsa_data_p->data_len, (CSPSA_Data_t*)exe_cspsa_data_p->value);
    if (result >= T_CSPSA_RESULT_ERRORS) {
        ATC_LOG_E("%s, CSPSA_ReadValue failed: %d", __func__, result);
        goto error;
    }

cspsa_finished:
    CSPSA_Close(&handle);
    return exe_result;

error:
    exe_result = EXE_FAILURE;
    if (exe_cspsa_data_p) {
        free(exe_cspsa_data_p->value);
        exe_cspsa_data_p->value = NULL;
    }
    goto cspsa_finished;
}
