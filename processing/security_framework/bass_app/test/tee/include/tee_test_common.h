/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef NW_TO_TEE_TEST_COMMON_H
#define NW_TO_TEE_TEST_COMMON_H

#include <stdint.h>
#include <tee_client_api.h>

#define NW_TO_TEE_TEST_SUCCESS  0
#define NW_TO_TEE_TEST_FAILURE  -1

enum TestTypes {
    SHA_256 = 0,
    AES256ECB_ENCRYPT,
    AES256ECB_DECRYPT,
    NUMBER_OF_TEST_TYPES /* Determine the number of test types */
};

typedef struct {
    const char *info;
    TEEC_Operation operation;
    uint8_t *referenceData;
    uint32_t referenceDataLen;
} OperationInfo;

typedef struct {
    TEEC_UUID uuid;
    uint32_t commandID;
    OperationInfo *opInfo;
    uint16_t numOperationInfos;
} SessionCommandInfo;

#endif
