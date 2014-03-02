/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <bass_app.h>
#include <tee_test_data.h>
#include <uuid.h>

#define MAX(a, b)  ((a) > (b)) ? (a) : (b)

static TEEC_UUID SHA_AES_uuid = TA_SHA_AES_UUID;

#define TA_CMD_ID_SHA256            0x00000007
#define TA_CMD_ID_AES256ECB_ENCRYPT 0x00000008
#define TA_CMD_ID_AES256ECB_DECRYPT 0x00000009

typedef struct {
    enum TestTypes  testType;
    TEEC_UUID       uuid;
    uint32_t        commandID;
} TestTypes_UUID_CommandID;

typedef struct {
    const char *info;
    uint8_t *inData;
    uint32_t inDataLen;
    uint8_t *refDigest;
    uint32_t refDigestLen;
} SHA_TestData;

typedef struct {
    const char *info;
    uint8_t *inData;
    uint32_t inDataLen;
    uint8_t *outData;
    uint32_t outDataLen;
} AES_TestData;

SHA_TestData SHA_TD[] = {
    {
        "SHA-256 test, 3 bytes input",
        (uint8_t *)"abc",
        3,
        (uint8_t *)"\xba\x78\x16\xbf\x8f\x01\xcf\xea"
        "\x41\x41\x40\xde\x5d\xae\x22\x23"
        "\xb0\x03\x61\xa3\x96\x17\x7a\x9c"
        "\xb4\x10\xff\x61\xf2\x00\x15\xad",
        32
    }
};

AES_TestData AES_Encrypt_TD[] = {
    {
        "AES-256 ECB encrypt test, 32 bytes input, with fixed key",
        (uint8_t *)"\x00\x01\x02\x03\x04\x05\x06\x07"
        "\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
        "\x10\x11\x12\x13\x14\x15\x16\x17"
        "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
        32,
        (uint8_t *)"\x5A\x6E\x04\x57\x08\xFB\x71\x96"
        "\xF0\x2E\x55\x3D\x02\xC3\xA6\x92"
        "\xE9\xC3\xEF\x8A\xB2\x34\x53\xE6"
        "\xF0\x74\x9C\xD6\x36\xE7\xA8\x8E",
        32
    }
};

AES_TestData AES_Decrypt_TD[] = {
    {
        "AES-256 ECB decrypt test, 32 bytes input, with fixed key",
        (uint8_t *)"\x5A\x6E\x04\x57\x08\xFB\x71\x96"
        "\xF0\x2E\x55\x3D\x02\xC3\xA6\x92"
        "\xE9\xC3\xEF\x8A\xB2\x34\x53\xE6"
        "\xF0\x74\x9C\xD6\x36\xE7\xA8\x8E",
        32,
        (uint8_t *)"\x00\x01\x02\x03\x04\x05\x06\x07"
        "\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
        "\x10\x11\x12\x13\x14\x15\x16\x17"
        "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f",
        32
    }
};

int getSessionCommandInfo(enum TestTypes test, SessionCommandInfo *sci)
{
    int i;
    int numOperationInfos;

    if (sci == NULL) {
        return -1;
    }

    switch (test) {
    case SHA_256: {
        sci->uuid      = SHA_AES_uuid;
        sci->commandID = TA_CMD_ID_SHA256;

        /* Now build the OpInfo */
        numOperationInfos = sizeof(SHA_TD) / sizeof(SHA_TestData);

        /* Allocate memory for the operationInfos */
        sci->opInfo = (OperationInfo *)malloc(numOperationInfos *
                                              sizeof(OperationInfo));

        sci->numOperationInfos = numOperationInfos;

        if (sci->opInfo == NULL) {
            return -1;
        }

        for (i = 0; i < numOperationInfos; i++) {
            /* The descriptive text. */
            sci->opInfo[i].info = SHA_TD[i].info;

            /* For this TA, memRef[0] is input */
            sci->opInfo[i].operation.memRefs[0].buffer =
                SHA_TD[i].inData;

            sci->opInfo[i].operation.memRefs[0].size =
                SHA_TD[i].inDataLen;

            sci->opInfo[i].operation.memRefs[0].flags =
                TEEC_MEM_INPUT;

            /*
             * For this TA, memRef[1] is output. Allocate memory for the output
             * data.
             */
            sci->opInfo[i].operation.memRefs[1].buffer =
                malloc(SHA_TD[i].refDigestLen);

            sci->opInfo[i].operation.memRefs[1].size =
                SHA_TD[i].refDigestLen;

            sci->opInfo[i].operation.memRefs[1].flags =
                TEEC_MEM_OUTPUT;

            sci->opInfo[i].operation.flags =
                TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

            /* For comparing the computed result. */
            sci->opInfo[i].referenceData    = SHA_TD[i].refDigest;
            sci->opInfo[i].referenceDataLen = SHA_TD[i].refDigestLen;
        }

        break;
    }

    case AES256ECB_ENCRYPT: {
        sci->uuid = SHA_AES_uuid;
        sci->commandID = TA_CMD_ID_AES256ECB_ENCRYPT;

        /* Now build the OpInfo. */
        numOperationInfos = sizeof(AES_Encrypt_TD) /
                            sizeof(AES_TestData);
        sci->numOperationInfos = numOperationInfos;

        /* Allocate memory for the operationInfos. */
        sci->opInfo = (OperationInfo *)malloc(numOperationInfos *
                                              sizeof(OperationInfo));

        if (sci->opInfo == NULL) {
            return -1;
        }

        for (i = 0; i < numOperationInfos; i++) {
            /* The descriptive text. */
            sci->opInfo[i].info = AES_Encrypt_TD[i].info;

            /* For this TA, memRef[0] is input. */
            sci->opInfo[i].operation.memRefs[0].buffer =
                AES_Encrypt_TD[i].inData;

            sci->opInfo[i].operation.memRefs[0].size =
                AES_Encrypt_TD[i].inDataLen;

            sci->opInfo[i].operation.memRefs[0].flags =
                TEEC_MEM_INPUT;

            /*
             * For this TA, memRef[1] is output. Allocate memory for the output
             * data.
             */
            sci->opInfo[i].operation.memRefs[1].buffer =
                malloc(AES_Encrypt_TD[i].outDataLen);

            sci->opInfo[i].operation.memRefs[1].size =
                AES_Encrypt_TD[i].outDataLen;

            sci->opInfo[i].operation.memRefs[1].flags =
                TEEC_MEM_OUTPUT;

            sci->opInfo[i].operation.flags =
                TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

            /* For comparing the computed result. */
            sci->opInfo[i].referenceData =
                AES_Encrypt_TD[i].outData;
            sci->opInfo[i].referenceDataLen =
                AES_Encrypt_TD[i].outDataLen;
        }

        break;
    }

    case AES256ECB_DECRYPT: {
        sci->uuid = SHA_AES_uuid;
        sci->commandID = TA_CMD_ID_AES256ECB_DECRYPT;

        /* Now build the OpInfo. */
        numOperationInfos = sizeof(AES_Decrypt_TD) /
                            sizeof(AES_TestData);
        sci->numOperationInfos = numOperationInfos;

        /* Allocate memory for the operationInfos. */
        sci->opInfo = (OperationInfo *)malloc(numOperationInfos *
                                              sizeof(OperationInfo));

        if (sci->opInfo == NULL) {
            return -1;
        }

        for (i = 0; i < numOperationInfos; i++) {
            /* The descriptive text. */
            sci->opInfo[i].info = AES_Decrypt_TD[i].info;

            /* For this TA, memRef[0] is input. */
            sci->opInfo[i].operation.memRefs[0].buffer =
                AES_Decrypt_TD[i].inData;

            sci->opInfo[i].operation.memRefs[0].size =
                AES_Decrypt_TD[i].inDataLen;

            sci->opInfo[i].operation.memRefs[0].flags =
                TEEC_MEM_INPUT;

            /*
             * For this TA, memRef[1] is output Allocate memory for the output
             * data.
             */
            sci->opInfo[i].operation.memRefs[1].buffer =
                malloc(AES_Decrypt_TD[i].outDataLen);

            sci->opInfo[i].operation.memRefs[1].size =
                AES_Decrypt_TD[i].outDataLen;

            sci->opInfo[i].operation.memRefs[1].flags =
                TEEC_MEM_OUTPUT;

            sci->opInfo[i].operation.flags =
                TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

            /* For comparing the computed result. */
            sci->opInfo[i].referenceData    =
                AES_Decrypt_TD[i].outData;
            sci->opInfo[i].referenceDataLen =
                AES_Decrypt_TD[i].outDataLen;
        }

        break;
    }

    default:
        return -1;

    }

    return 0;
}

int cleanUpSessionCommandInfo(enum TestTypes tt, SessionCommandInfo *sci)
{
    int i = 0;
    int j = 0;
    int numOperationInfos;

    if (sci == NULL) {
        return -1;
    }

    switch (tt) {
    case SHA_256:
        numOperationInfos = sizeof(SHA_TD) / sizeof(SHA_TestData);
        break;

    case AES256ECB_ENCRYPT:
        numOperationInfos = sizeof(AES_Encrypt_TD) /
                            sizeof(AES_TestData);
        break;

    case AES256ECB_DECRYPT:
        numOperationInfos = sizeof(AES_Decrypt_TD) /
                            sizeof(AES_TestData);
        break;

    default:
        return -1;
    }

    switch (tt) {
    case SHA_256:
    case AES256ECB_ENCRYPT:
    case AES256ECB_DECRYPT: {
        for (i = 0; i < numOperationInfos; i++) {
            for (j = 0; j < 4; j++) {
                /*
                 * This functions only cleans output buffers, since it uses
                 * static buffers for the input buffers in this test code.
                 */
                if (sci->opInfo[i].operation.memRefs[j].flags &
                        TEEC_MEM_OUTPUT) {
                    free(sci->opInfo[i].operation.memRefs[j].buffer);
                }
            }
        }

        /* Free memory for the operationInfos. */
        free(sci->opInfo);

        break;
    }

    default:
        return -1;
    }

    return 0;
}

/*
 * FIXME: This should be moved into a generic debug file and there should
 * be compile flags to enable/disable this.
 */
#ifdef DEBUG
#include <ctype.h>
#define MIN(x, y) ((x) < (y)) ? (x) : (y)
static void dump_buf(const char *bname, uint8_t *buf, size_t blen)
{
    fprintf(stderr, "#### %s\n", bname);

    while (blen > 0) {
        size_t    n;

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                fprintf(stderr, "%02x ", (int)buf[n]);
            } else {
                fprintf(stderr, "   ");
            }

            if (n == 7) {
                fprintf(stderr, " ");
            }
        }

        fprintf(stderr, " |");

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                if (isprint(buf[n]))
                    fprintf(stderr, "%c",
                            (int)buf[n]);
                else {
                    fprintf(stderr, ".");
                }
            }
        }

        fprintf(stderr, "|\n");

        blen -= MIN(blen, 16);
        buf += 16;
    }
}
#endif

int compareResult(enum TestTypes tt, OperationInfo *opInfo)
{
    int     retVal;
    uint8_t outputBufferIndex;
    int bufsize = 0;

    /* Get the index of the output buffer. */
    switch (tt) {
    case SHA_256:
    case AES256ECB_ENCRYPT:
    case AES256ECB_DECRYPT:
        outputBufferIndex = 1;
        break;

    default:
        /* error */
        return 2;
    }

    bufsize = MAX(opInfo->operation.memRefs[outputBufferIndex].size,
                  opInfo->referenceDataLen);

    if (memcmp(opInfo->operation.memRefs[outputBufferIndex].buffer,
               opInfo->referenceData,
               bufsize)) {
        retVal = 1;
    } else {
        retVal = 0;
    }

#if DEBUG
    dump_buf("Expected", (uint8_t *)opInfo->referenceData, bufsize);
    dump_buf("Got",
             (uint8_t *) opInfo->operation.memRefs[outputBufferIndex].buffer,
             bufsize);
#endif

    return retVal;
}

