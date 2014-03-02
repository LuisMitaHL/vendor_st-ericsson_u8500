/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *         martin.xm.hovang@stericsson.com
 */

/* Based on GP TEE Internal API Specification Version 0.11 */
#ifndef TEE_API_TYPES_H
#define TEE_API_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <tee_api_defines.h>

/*
 * Common Definitions
 */

typedef uint32_t TEE_Result;

#ifdef TEEC_COMPAT
typedef TEE_Result TEEC_Result;
#endif

typedef struct
{
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t  clockSeqAndNode[8];
} TEE_UUID;

#ifdef TEEC_COMPAT
typedef TEE_UUID TEEC_UUID;
#endif

/*
 * The TEE_Identity structure defines the full identity of a Client:
 * - login is one of the TEE_LOGIN_XXX constants
 * - uuid contains the client UUID or Nil if not applicable
 */
typedef struct {
    uint32_t login;
    TEE_UUID uuid;
} TEE_Identity;

/*
 * This union describes one parameter passed by the Trusted Core Framework
 * to the entry points TA_OpenSessionEntryPoint or
 * TA_InvokeCommandEntryPoint or by the TA to the functions
 * TEE_OpenTASession or TEE_InvokeTACommand.
 *
 * Which of the field value or memref to select is determined by the
 * parameter type specified in the argument paramTypes passed to the entry
 * point.
*/
typedef union {
    struct {
        void *buffer;
        size_t size;
    } memref;
    struct {
        uint32_t a;
        uint32_t b;
    } value;
} TEE_Param;

/*
 * The type of opaque handles on TA Session. These handles are returned by
 * the function TEE_OpenTASession.
 */
typedef struct __TEE_TASessionHandle* TEE_TASessionHandle;

/*
 * The type of opaque handles on property sets or enumerators. These
 * handles are either one of the pseudo handles TEE_PROPSET_XXX or are
 * returned by the function TEE_AllocatePropertyEnumerator.
*/
typedef struct __TEE_PropSetHandle* TEE_PropSetHandle;

typedef struct __TEE_ObjectHandle* TEE_ObjectHandle;
typedef struct __TEE_OperationHandle* TEE_OperationHandle;

/*
 * Storage Definitions
 */

typedef uint32_t TEE_ObjectType;

typedef struct {
    uint32_t objectType;
    uint32_t objectSize;
    uint32_t maxObjectSize;
    uint32_t objectUsage;
    uint32_t dataSize;
    uint32_t dataPosition;
    uint32_t handleFlags;
} TEE_ObjectInfo;

typedef enum {
   TEE_DATA_SEEK_SET = 0,
   TEE_DATA_SEEK_CUR,
   TEE_DATA_SEEK_END
} TEE_Whence;

typedef struct {
    uint32_t attributeID;
    union
    {
        struct
        {
            void *buffer;
            size_t length;
        } ref;
        struct
        {
            uint32_t a, b;
        } value;
    } content;
} TEE_Attribute;

/* Cryptographic Operations API */

typedef enum {
    TEE_MODE_ENCRYPT,
    TEE_MODE_DECRYPT,
    TEE_MODE_SIGN,
    TEE_MODE_VERIFY,
    TEE_MODE_MAC,
    TEE_MODE_DIGEST,
    TEE_MODE_DERIVE
} TEE_OperationMode;

typedef struct {
    uint32_t algorithm;
    uint32_t algorithmClass;
    uint32_t mode;
    uint32_t digestLength;
    uint32_t maxKeySize;
    uint32_t keySize;
    uint32_t requiredKeyUsage;
    uint32_t handleState;
} TEE_OperationInfo;

/* Time & Date API */

typedef struct
{
    uint32_t seconds;
    uint32_t millis;
} TEE_Time;

/* TEE Arithmetical APIs */

typedef uint32_t TEE_BigInt;

typedef uint32_t TEE_BigIntFMM;

typedef uint32_t TEE_BigIntFMMContext;



#endif /*TEE_API_TYPES_H*/
