/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HMAC_SHA_H
#define HMAC_SHA_H

#include "gps_ptypes.h"

#define     HMAC_DEBUG_LOG_PRINT( x )


/****************************** SHA256 procedure *************************************/
/***********************************************************************************/
#define     SUPL_SHA256_SIZE            32
#define     SUPL_SHA256_MSG_BLOCK_SIZE  64

#define GN_SUPL_GetHmac_sha256 GN_SUPL_GetHmacSha256

#define     SIZE_SHA256_HASH_IN_BYTES     32


/*
 *  This structure will hold context information for the SHA-256
 *  hashing operation.
 */
typedef struct {
    U4 Intermediate_HashValue[SUPL_SHA256_SIZE/4]; /* Message Digest */
    U4 Length_Lsb;                                 /* Message length in bits */
    U4 Length_Msb;                                 /* Message length in bits */
    U2 Message_Block_Index;                        /* Index into message block array   */                                       
    U1 Message_Block[SUPL_SHA256_MSG_BLOCK_SIZE];  /* 512-bit message blocks */
} s_GN_SUPL_SHA256Context;

U1 pHashedKeysha256[SIZE_SHA256_HASH_IN_BYTES];

int GN_SUPL_GetHmacSha256(
   U1*  pKey,
   U4  keyLen,
   U1*  pData,
   U4  dataLen,
   U1*  pHash
);



/****************************** SHA1 procedure *************************************/
/***********************************************************************************/

/* 
 *  This structure will hold context information for the hashing
 *  operation
 */

typedef struct SHA1Context
{
    U4 Message_Digest[5]; /* Message Digest (output)          */

    U4 Length_Low;        /* Message length in bits           */
    U4 Length_High;       /* Message length in bits           */

    U1 Message_Block[64]; /* 512-bit message blocks      */
    U1 Message_Block_Index;    /* Index into message block array   */

    U1 Computed;               /* Is the digest computed?          */
    U1 Corrupted;              /* Is the message digest corruped?  */
} SHA1Context;

#define GN_SUPL_GetHmac_sha1 GN_SUPL_GetHmacSha1Block64

#define     SIZE_SHA1_HASH_IN_BYTES     20

U1 pHashedKeysha1[SIZE_SHA1_HASH_IN_BYTES];

int GN_SUPL_GetHmacSha1Block64(
   U1* pKey,            
   void *msg,           
   U4 length,         
   U1 *output 
); 

#endif     // HMAC_SHA1_H
