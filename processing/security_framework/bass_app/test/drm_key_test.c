/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#include <stdio.h>

#include <bass_app.h>
#include <bass_app_test.h>
#include <debug.h>
#include <tee_client_api.h>

#define DRM_KEY_TEST_RANDOM_SIZE        10
#define DRM_KEY_TEST_PUBLIC_ID_SIZE     20
#define DRM_KEY_TEST_KEY_SIZE           128

static uint8_t drm_key_test_key[DRM_KEY_TEST_KEY_SIZE] = {
    0xcf, 0x67, 0x0a, 0xe5,  0x28, 0x0e, 0xf6, 0xc1,  0x0c, 0x0a, 0x71, 0xe1,
    0x6b, 0x8f, 0x73, 0x4b,  0xff, 0x26, 0x8b, 0x66,  0x1a, 0x69, 0x16, 0xa8,
    0x8d, 0x58, 0x96, 0xd5,  0x8a, 0x5d, 0x5a, 0xe1,  0x97, 0xed, 0xd4, 0x23,
    0x79, 0x98, 0x87, 0xf0,  0xdb, 0x47, 0xa1, 0x42,  0x7c, 0xc0, 0x08, 0x00,
    0xe6, 0x21, 0xaa, 0x7c,  0x6a, 0xc9, 0xe0, 0x0e,  0x9c, 0xcd, 0xf7, 0x54,
    0xfd, 0xf4, 0x2f, 0xc4,  0xf4, 0xca, 0x99, 0xb1,  0x6e, 0xe0, 0xff, 0x85,
    0xe3, 0xb7, 0x23, 0xc4,  0x5b, 0x2b, 0x20, 0xe7,  0x78, 0xf6, 0xd8, 0xcf,
    0x6c, 0x19, 0xc2, 0xc9,  0xc9, 0xd1, 0x4a, 0x73,  0x01, 0xd5, 0x54, 0xac,
    0xcd, 0x9b, 0x34, 0x71,  0xf7, 0x06, 0xc7, 0xdf,  0xec, 0x9f, 0x83, 0x0d,
    0xf7, 0xfb, 0x58, 0x45,  0xa5, 0xba, 0x67, 0x90,  0xd2, 0x64, 0xcf, 0x41,
    0x53, 0x6b, 0xbf, 0xfb,  0x15, 0xb9, 0xb2, 0xe1
};

static bass_return_code drm_key_test_get_random(void)
{
    bass_return_code result = BASS_RC_SUCCESS;
    uint8_t random[DRM_KEY_TEST_RANDOM_SIZE];

    dprintf(INFO, "Test: Get random with size %d\n", sizeof(random));
    result = drm_key_get_random((uint8_t *)&random[0], sizeof(random));

    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "Unexpected result! Error code = %x\n", result);
    } else {
        dprintf(INFO, "Successfully got random data\n");
        dump_buffer("random:", &random[0], sizeof(random));
    }

    return result;
}

static bass_return_code drm_key_test_identify_device(void)
{
    bass_return_code result = BASS_RC_SUCCESS;
    uint8_t public_id[DRM_KEY_TEST_PUBLIC_ID_SIZE];

    dprintf(INFO, "Test: Identify device (get public ID)\n");
    result = drm_key_identify_device((uint8_t *)&public_id[0],
                                     sizeof(public_id));

    if (BASS_RC_SUCCESS != result) {
        dprintf(ERROR, "Unexpected result! Error code = %x\n", result);
    } else {
        dprintf(INFO, "Successfully got public ID\n");
        dump_buffer("public ID:", &public_id[0], sizeof(public_id));
    }

    return result;
}

static bass_return_code drm_key_test_set_drm_state(void)
{
    bass_return_code result = BASS_RC_SUCCESS;

    dprintf(INFO, "Test: Set DRM state\n");
    result = bass_set_drm_state(true);

    if (result != BASS_RC_ERROR_TEE_TRUSTED_APP) {
        dprintf(ERROR, "Test: Set DRM state failed, "
            "expected result: TEEC_ERROR_BAD_STATE and "
            "errorOrigin: BASS_RC_ERROR_TEE_TRUSTED_APP!\n");
    } else {
        dprintf(INFO, "Expected result: TEEC_ERROR_BAD_STATE "
                    "and errorOrigin: BASS_RC_ERROR_TEE_TRUSTED_APP\n"
                    "NOTE! Setting DRM state is only valid/possible "
                    "prior to post-boot, e.g. u-boot or similar\n");
        result = BASS_RC_SUCCESS;
    }

    return result;
}

static bass_return_code drm_key_test_encrypt_and_store_key(void)
{
    bass_return_code result = BASS_RC_SUCCESS;

    dprintf(INFO, "Test: Encrypt and store key\n");
    dump_buffer("DRM key:", &drm_key_test_key[0], sizeof(drm_key_test_key));

    result = drm_key_encrypt_and_store_key(&drm_key_test_key[0],
                sizeof(drm_key_test_key));
    if (BASS_RC_SUCCESS != result) {
        if (BASS_RC_ERROR_TEE_TRUSTED_APP == result) {
            dprintf(INFO, "Expected result: BASS_RC_ERROR_TEE_TRUSTD_APP "
                "(TEEC_ERROR_BAD_STATE)\n"
                "NOTE! Pre-condition: Need to set DRM state, "
                "which only is valid/possible prior to post-boot\n");
        } else {
            dprintf(ERROR, "Unexpected result! Error code = %x\n", result);
        }
    } else {
        dprintf(INFO, "Successfully encrypted and stored key\n");
    }

error_return:
    return result;
}

static bass_return_code drm_key_test_get_key_data(void)
{
    bass_return_code result = BASS_RC_SUCCESS;
    uint8_t key_data[DRM_KEY_TEST_KEY_SIZE];

    dprintf(INFO, "Test: Get key data\n");

    result = drm_key_get_key_data(&key_data[0], sizeof(key_data));
    if (BASS_RC_SUCCESS != result) {
        if (BASS_RC_ERROR_TEE_TRUSTED_APP == result) {
            dprintf(INFO, "Expected result: BASS_RC_ERROR_TEE_TRUSTED_APP "
                "(TEEC_ERROR_BAD_STATE)\n"
                "NOTE! Pre-condition: Need to set DRM state, "
                "which only is valid/possible prior to post-boot\n");
        } else {
            dprintf(ERROR, "Unexpected result! Error code = %x\n", result);
        }

        goto error_return;
    }

    if (memcmp(&key_data[0], &drm_key_test_key[0], sizeof(key_data)) != 0) {
        dump_buffer("Unexpected key_data:", &key_data[0], sizeof(key_data));
        dump_buffer("Expected key_data:", &drm_key_test_key[0],
            sizeof(key_data));
        goto error_return;
    }

    dprintf(INFO, "Successfully got key data\n");

error_return:
    return result;
}

/**
 * DRM key tests.
 */
uint32_t bass_app_test_drm_key(void)
{
    uint32_t result = BASS_RC_SUCCESS;
    if (drm_key_test_get_random() != BASS_RC_SUCCESS) {
        result = BASS_RC_FAILURE;
        dprintf(ERROR, "Test: Get random failed!\n");
    }

    if (drm_key_test_identify_device() != BASS_RC_SUCCESS) {
        result = BASS_RC_FAILURE;
        dprintf(ERROR, "Test: Identify device failed!\n");
    }

#ifdef DRM_KEY_PRE_CONDITION_DRM_STATE_ENABLED
    /**
     * Should also be tested from u-boot, since only allowed to set DRM state
     * prior to post-boot. Else expect error in drm_key_test_set_drm_state(),
     * which will be updated to TEEC_SUCCESS if expected error.
     */
    if (drm_key_test_set_drm_state() != BASS_RC_SUCCESS) {
        result = BASS_RC_FAILURE;
        dprintf(ERROR, "Test: Set DRM state failed!\n");
    }

    if (drm_key_test_encrypt_and_store_key() != BASS_RC_SUCCESS) {
        dprintf(ERROR, "Test: Encrypt and store key failed!\n");
        result = BASS_RC_FAILURE;
    } else if (drm_key_test_get_key_data() != BASS_RC_SUCCESS) {
        dprintf(ERROR, "Test: Get key data failed!");
        result = BASS_RC_FAILURE;
    }
#endif

    return result;
}

