/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

#include <cspsa.h>

#define DRM_KEY_CSPSA_AREA  "CSPSA0"
#define DRM_KEY_CSPSA_KEY   0x10400
#define DRM_KEY_SIZE        128

static bass_return_code drm_key_ta_execute(uint8_t cmd, TEEC_Operation *op) {
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    bass_return_code ret = BASS_RC_SUCCESS;
    TEEC_Session session;
    TEEC_UUID uuid = DRM_KEY_TA_UUID;

    memset(&session, 0, sizeof(TEEC_Session));

    result = TEEC_InitializeContext(NULL, &ctx);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed\n");
        goto out;
    }

    result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC, NULL,
                              NULL, &org);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_OpenSession failed\n");
        goto out_finalize;
    }

    result = TEEC_InvokeCommand(&session, cmd, op, &org);
    if (result != TEEC_SUCCESS)
        dprintf(ERROR, "TEEC_InvokeCommand failed\n");

    if (TEEC_CloseSession(&session) != TEEC_SUCCESS)
        dprintf(ERROR, "TEEC_CloseSession failed\n");

out_finalize:
    if (TEEC_FinalizeContext(&ctx) != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext failed\n");
        /*
         * Special handling since there is a risk of overwriting old failing
         * result code from previous calls to tee.
         */
        if (result == TEEC_SUCCESS)
            result = TEEC_ERROR_GENERIC;
    }
out:
    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "result = 0x%X, errorOrigin = %d\n", result, org);

        switch (org) {
        case TEEC_ORIGIN_API:
            ret = BASS_RC_ERROR_TEE_API;
            break;
        case TEEC_ORIGIN_COMMS:
            ret = BASS_RC_ERROR_TEE_COMMS;
            break;
        case TEEC_ORIGIN_TEE:
            ret = BASS_RC_ERROR_TEE_CORE;
            break;
        case TEEC_ORIGIN_TRUSTED_APP:
        case TEEC_ERROR_BAD_STATE:
            ret = BASS_RC_ERROR_TEE_TRUSTED_APP;
            break;
        default:
            ret = BASS_RC_ERROR_UNKNOWN;
            break;
        }
    }

    return ret;
}

static bass_return_code drm_key_storage_write(uint8_t *key, size_t key_size)
{
    bass_return_code ret = BASS_RC_SUCCESS;
    CSPSA_Result_t cspsa_result = T_CSPSA_RESULT_OK;
    CSPSA_Handle_t cspsa_handle;

    if (NULL == key || DRM_KEY_SIZE != key_size) {
        dprintf(ERROR, "Invalid parameter\n");
        return BASS_RC_FAILURE;
    }

    cspsa_result = CSPSA_Open(DRM_KEY_CSPSA_AREA, &cspsa_handle);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        if (cspsa_result == T_CSPSA_RESULT_E_NO_VALID_IMAGE) {
            cspsa_result = CSPSA_Create(DRM_KEY_CSPSA_AREA, &cspsa_handle);
            if (cspsa_result != T_CSPSA_RESULT_OK) {
                dprintf(ERROR, "CSPSA_Create failed: %d\n", cspsa_result);
                ret = BASS_RC_FAILURE;
                goto out;
            }
        } else {
            dprintf(ERROR, "CSPSA_Open failed: %d\n", cspsa_result);
            ret = BASS_RC_FAILURE;
            goto out;
        }
    }

    cspsa_result = CSPSA_WriteValue(cspsa_handle, DRM_KEY_CSPSA_KEY,
                                    key_size, key);
    if (cspsa_result > T_CSPSA_RESULT_W_INVALID_HEADER_FOUND) {
        dprintf(ERROR, "CSPSA_WriteValue failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
        goto out;
    }

    cspsa_result = CSPSA_Flush(cspsa_handle);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_Flush failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
    }

out:
    cspsa_result = CSPSA_Close(&cspsa_handle);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_Close failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
    }

    return ret;
}

static bass_return_code drm_key_storage_read(uint8_t *key, size_t key_size)
{
    bass_return_code ret = BASS_RC_SUCCESS;
    CSPSA_Result_t cspsa_result = T_CSPSA_RESULT_OK;
    CSPSA_Handle_t cspsa_handle;
    CSPSA_Size_t cspsa_size;

    if (NULL == key || DRM_KEY_SIZE != key_size) {
        dprintf(ERROR, "Invalid parameter\n");
        return BASS_RC_FAILURE;
    }

    cspsa_result = CSPSA_Open(DRM_KEY_CSPSA_AREA, &cspsa_handle);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_Open failed: %d\n", cspsa_result);
        return BASS_RC_FAILURE;
    }

    cspsa_result = CSPSA_GetSizeOfValue(cspsa_handle,
                                        DRM_KEY_CSPSA_KEY, &cspsa_size);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_GetSizeOfValue failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
        goto out;
    }

    if (cspsa_size != DRM_KEY_SIZE && key_size != DRM_KEY_SIZE) {
        dprintf(ERROR, "Wrong size: %d\n", cspsa_size);
        ret = BASS_RC_FAILURE;
        goto out;
    }

    cspsa_result = CSPSA_ReadValue(cspsa_handle, DRM_KEY_CSPSA_KEY,
                                   cspsa_size, key);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_ReadValue failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
        goto out;
    }

out:
    cspsa_result = CSPSA_Close(&cspsa_handle);
    if (cspsa_result != T_CSPSA_RESULT_OK) {
        dprintf(ERROR, "CSPSA_Close failed: %d\n", cspsa_result);
        ret = BASS_RC_FAILURE;
    }

    return ret;
}

bass_return_code drm_key_get_random(uint8_t *random_data,
                                    const size_t random_size)
{
    TEEC_Operation operation;

    if (NULL == random_data) {
        dprintf(ERROR, "NULL pointer error\n");
        return BASS_RC_FAILURE;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = random_data;
    operation.memRefs[0].size = random_size;
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    return drm_key_ta_execute(DRM_KEY_GET_RANDOM, &operation);
}

bass_return_code drm_key_identify_device(uint8_t *public_id,
                                         const size_t public_id_size)
{
    TEEC_Operation operation;

    if (NULL == public_id) {
        dprintf(ERROR, "NULL pointer error\n");
        return BASS_RC_FAILURE;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = public_id;
    operation.memRefs[0].size = public_id_size;
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    return drm_key_ta_execute(DRM_KEY_GET_PUBLIC_ID, &operation);
}

bass_return_code drm_key_encrypt_and_store_key(uint8_t *key,
                                               const size_t key_size)
{
    TEEC_Operation operation;
    bass_return_code ret = BASS_RC_SUCCESS;
    uint8_t *protected_key = NULL;

    if (NULL == key) {
        dprintf(ERROR, "NULL pointer error\n");
        ret = BASS_RC_FAILURE;
        goto out;
    }

    protected_key = malloc(key_size);
    if (NULL == protected_key) {
        dprintf(ERROR, "Allocation error\n");
        ret = BASS_RC_ERROR_INTERNAL;
        goto out;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = key;
    operation.memRefs[0].size = key_size;
    operation.memRefs[0].flags = TEEC_MEM_INPUT;

    operation.memRefs[1].buffer = protected_key;
    operation.memRefs[1].size = key_size;
    operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

    /* Encrypt and integrity protect the DRM key */
    ret = drm_key_ta_execute(DRM_KEY_ENCRYPT, &operation);
    if (ret != BASS_RC_SUCCESS)
        goto out;

    /* Store the protected key */
    ret = drm_key_storage_write(protected_key, key_size);

out:
    if (NULL != protected_key)
        free(protected_key);

    return ret;
}

bass_return_code drm_key_get_key_data(uint8_t *key, const size_t key_size)
{
    TEEC_Operation operation;
    bass_return_code ret = BASS_RC_SUCCESS;
    uint8_t protected_key[DRM_KEY_SIZE];

    if (NULL == key) {
        dprintf(ERROR, "NULL pointer error\n");
        ret = BASS_RC_FAILURE;
        goto out;
    }

    /* Get protected key */
    ret = drm_key_storage_read(&protected_key[0], sizeof(protected_key));
    if (ret != BASS_RC_SUCCESS) {
        dprintf(ERROR, "drm_key_storage_read failed\n");
        goto out;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = protected_key;
    operation.memRefs[0].size = key_size;
    operation.memRefs[0].flags = TEEC_MEM_INPUT;

    operation.memRefs[1].buffer = key;
    operation.memRefs[1].size = key_size;
    operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

    /* Encrypt and integrity protect the DRM key */
    ret = drm_key_ta_execute(DRM_KEY_GET_KEY_DATA, &operation);

out:
    return ret;
}

