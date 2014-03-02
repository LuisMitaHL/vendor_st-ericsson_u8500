/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 */

#ifndef TEE_SYSCALL_NUMBERS_H
#define TEE_SYSCALL_NUMBERS_H

#define TEE_SCN_RETURN                0
#define TEE_SCN_LOG                   1
#define TEE_SCN_PANIC                 2
#define TEE_SCN_DUMMY                 3
#define TEE_SCN_DUMMY_7ARGS           4
#define TEE_SCN_SLA_3ARGS             5
#define TEE_SCN_SLA_8ARGS             6
#define TEE_SCN_SLA_16ARGS            7
#define TEE_SCN_GET_PROPERTY          8
#define TEE_SCN_OPEN_TA_SESSION       9
#define TEE_SCN_CLOSE_TA_SESSION      10
#define TEE_SCN_INVOKE_TA_COMMAND     11
#define TEE_SCN_CHECK_ACCESS_RIGHTS   12
#define TEE_SCN_GET_CANCELLATION_FLAG 13
#define TEE_SCN_UNMASK_CANCELLATION   14
#define TEE_SCN_MASK_CANCELLATION     15
#define TEE_SCN_WAIT                  16
#define TEE_SCN_GET_TIME              17
#define TEE_SCN_SET_TA_TIME           18
#define TEE_SCN_CRYP_STATE_ALLOC      19
#define TEE_SCN_CRYP_STATE_COPY       20
#define TEE_SCN_CRYP_STATE_FREE       21
#define TEE_SCN_HASH_INIT             22
#define TEE_SCN_HASH_UPDATE           23
#define TEE_SCN_HASH_FINAL            24
#define TEE_SCN_CIPHER_INIT           25
#define TEE_SCN_CIPHER_UPDATE         26
#define TEE_SCN_CIPHER_FINAL          27
#define TEE_SCN_CRYP_OBJ_GET_INFO     28
#define TEE_SCN_CRYP_OBJ_RESTRICT_USAGE 29
#define TEE_SCN_CRYP_OBJ_GET_ATTR     30
#define TEE_SCN_CRYP_OBJ_ALLOC        31
#define TEE_SCN_CRYP_OBJ_CLOSE        32
#define TEE_SCN_CRYP_OBJ_RESET        33
#define TEE_SCN_CRYP_OBJ_POPULATE     34
#define TEE_SCN_CRYP_OBJ_COPY         35
#define TEE_SCN_CRYP_DERIVE_KEY       36
#define TEE_SCN_CRYP_RANDOM_NUMBER_GENERATE 37

#define TEE_SCN_MAX                   37

#endif /*TEE_SYSCALL_NUMBERS_H*/
