/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#ifndef COPS_ENABLE_ENV_MODE_ITP
#include "unistd.h"
#endif

#include <cops_data.h>
#include <cops_common.h>
#include <cops_storage.h>
#include <cspsa.h>

#ifndef COPS_ENABLE_ENV_MODE_ITP
#define COPS_AREA_NAME "CSPSA0"
#else
#define COPS_AREA_NAME "/flash0/TOC/CSPSA0"
#endif
#define COPS_KEY 0x10200

cops_return_code_t cops_storage_read(cops_data_t *cd,
                                     enum cops_data_type data_type)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    CSPSA_Result_t result;
    CSPSA_Handle_t  handle;
    CSPSA_Size_t size;

    if (cd->data == NULL) {
        COPS_LOG(LOG_ERROR, "cd->data == NULL\n");
        return COPS_RC_ARGUMENT_ERROR;
    }

    if (COPS_DATA != data_type) {
        COPS_LOG(LOG_ERROR, "Data type:%u not handled\n", data_type);
        return COPS_RC_ARGUMENT_ERROR;
    }

    /* Open CSPSA */
    result = CSPSA_Open(COPS_AREA_NAME, &handle);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_Open(%s, %p) returned 0x%x\n",
                 COPS_AREA_NAME, handle, result);
        return COPS_RC_STORAGE_ERROR;
    }

    /* Get the size of the COPS area */
    result = CSPSA_GetSizeOfValue(handle, COPS_KEY, &size);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_GetSizeOfValue(%p, 0x%x, 0x%x) "
                 "returned 0x%x\n", handle, COPS_KEY, size, result);
        ret_code = COPS_RC_STORAGE_ERROR;
        goto close_and_exit;
    }

    if (size > cd->max_length) {
        ret_code = COPS_RC_STORAGE_ERROR;
        COPS_LOG(LOG_ERROR, "size > cd->max_length\n");
        goto close_and_exit;
    }

    cd->length = size;
    result =  CSPSA_ReadValue(handle, COPS_KEY, size, cd->data);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_ReadValue(%p, 0x%x, 0x%x) returned 0x%x\n",
                 handle, COPS_KEY, size, result);
        ret_code = COPS_RC_STORAGE_ERROR;
    }

close_and_exit:
    result = CSPSA_Close(&handle);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_Close(%p) returned 0x%x\n", handle, result);
        ret_code = COPS_RC_STORAGE_ERROR;
    }

    return ret_code;
}

cops_return_code_t cops_storage_write(cops_data_t *cd,
                                      enum cops_data_type data_type)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    CSPSA_Result_t result;
    CSPSA_Handle_t handle;

    if (cd->data == NULL) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    if (COPS_DATA != data_type) {
        COPS_LOG(LOG_ERROR, "Data type:%u not handled\n", data_type);
        return COPS_RC_ARGUMENT_ERROR;
    }

    /* Initialize CSPSA */
    result =  CSPSA_Open(COPS_AREA_NAME, &handle);

    if (result != T_CSPSA_RESULT_OK) {
        if (result == T_CSPSA_RESULT_E_NO_VALID_IMAGE) {
            result = CSPSA_Create(COPS_AREA_NAME, &handle);

            if (result != T_CSPSA_RESULT_OK) {
                COPS_LOG(LOG_ERROR, "CSPSA_Create failed, code 0x%x\n",
                         (unsigned) result);
                ret_code = COPS_RC_STORAGE_ERROR;
                goto close_and_exit;
            }
        } else {
            COPS_LOG(LOG_ERROR, "CSPSA_Open failed, code 0x%x\n",
                     (unsigned)result);
            ret_code = COPS_RC_STORAGE_ERROR;
            goto close_and_exit;
        }
    }

    /* Write cops data to CSPSA */
    result = CSPSA_WriteValue(handle, COPS_KEY, cd->length, cd->data);

    if (result > T_CSPSA_RESULT_W_INVALID_HEADER_FOUND) {
        COPS_LOG(LOG_ERROR, "CSPSA_WriteValue(%p, 0x%x, %zu, %p) "
                 "returned 0x%x\n", handle, COPS_KEY, cd->length,
                 cd->data, result);
        ret_code = COPS_RC_STORAGE_ERROR;
        goto close_and_exit;
    }

    /* Flush CSPSA to make sure it is written*/
    result = CSPSA_Flush(handle);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_Flush(%p) returned 0x%x\n", handle, result);
        ret_code = COPS_RC_STORAGE_ERROR;
    }

close_and_exit:
    result = CSPSA_Close(&handle);

    if (result != T_CSPSA_RESULT_OK) {
        COPS_LOG(LOG_ERROR, "CSPSA_Close(%p) returned 0x%x\n", handle, result);
        ret_code = COPS_RC_STORAGE_ERROR;
    }

    return ret_code;
}
