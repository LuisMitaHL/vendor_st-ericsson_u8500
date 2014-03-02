/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef UTEE_DEFINES_H
#define UTEE_DEFINES_H

/*
 * Copied from TEE Internal API specificaion v1.0 table 6-9 "Structure of
 * Algorithm Identifier".
 */
#define TEE_MAIN_ALGO_MD5       0x01
#define TEE_MAIN_ALGO_SHA1      0x02
#define TEE_MAIN_ALGO_SHA224    0x03
#define TEE_MAIN_ALGO_SHA256    0x04
#define TEE_MAIN_ALGO_SHA384    0x05
#define TEE_MAIN_ALGO_SHA512    0x06
#define TEE_MAIN_ALGO_AES       0x10
#define TEE_MAIN_ALGO_DES       0x11
#define TEE_MAIN_ALGO_DES2      0x12
#define TEE_MAIN_ALGO_DES3      0x13
#define TEE_MAIN_ALGO_RSA       0x30
#define TEE_MAIN_ALGO_DSA       0x31
#define TEE_MAIN_ALGO_DH        0x32

#define TEE_CHAIN_MODE_ECB_NOPAD        0x0
#define TEE_CHAIN_MODE_CBC_NOPAD        0x1
#define TEE_CHAIN_MODE_CTR              0x2
#define TEE_CHAIN_MODE_CTS              0x3
#define TEE_CHAIN_MODE_XTS              0x4
#define TEE_CHAIN_MODE_CBC_MAC_PKCS5    0x5
#define TEE_CHAIN_MODE_CMAC             0x6
#define TEE_CHAIN_MODE_CCM              0x7
#define TEE_CHAIN_MODE_GCM              0x8
#define TEE_CHAIN_MODE_PKCS1_PSS_MGF1   0x9 /* ??? */


        /* Bits [31:28] */
#define TEE_ALG_GET_CLASS(algo)         (((algo) >> 28) & 0xF)

#define TEE_ALG_GET_KEY_TYPE(algo, with_private_key) \
        (TEE_ALG_GET_MAIN_ALG(algo) | \
            ((with_private_key) ? 0xA1000000 : 0xA0000000))

        /* Bits [7:0] */
#define TEE_ALG_GET_MAIN_ALG(algo)      ((algo) & 0xFF)

        /* Bits [11:8] */
#define TEE_ALG_GET_CHAIN_MODE(algo)    (((algo) >> 8) & 0xF)

#define TEE_AES_BLOCK_SIZE  16
#define TEE_DES_BLOCK_SIZE  8

        /* SHA-512 */
#define TEE_MAX_HASH_SIZE    64

#define MAX(a, b) \
       __extension__({ __typeof__(a) _a = (a); \
           __typeof__(b) _b = (b); \
         _a > _b ? _a : _b; })

#define MIN(a, b) \
       __extension__({ __typeof__(a) _a = (a); \
           __typeof__(b) _b = (b); \
         _a < _b ? _a : _b; })


#endif /*UTEE_DEFINES_H*/
