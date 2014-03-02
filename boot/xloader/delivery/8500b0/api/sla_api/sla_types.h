/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
    \file       sla_types.h
    \brief      This file contains types related to the secure library API.
    \author     STMicroelectronics

     \addtogroup SLA_TYPES
    @{
 */
/*----------------------------------------------------------------------*/

#ifndef SLA_TYPES_H
#define SLA_TYPES_H

#include "boot_secure_types.h"

/** \internal Structure containing all reset-related information */
typedef struct
{
    t_uint32 hw_reset_type;             /**< HW reset reason, see t_hw_reset_reason type for values */
    t_uint32 reason;                    /**< Reason */
    t_uint32 link_register;             /**< R14 register value */
} t_reset_info;


typedef enum
{
    A9_CPU0_WDG_RESET_REASON,      // First one should be equal to zero for compatibility with sources
    A9_CPU1_WDG_RESET_REASON,
    APE_SW_RESET_REASON,
    APE_RESET_REASON,
    SEC_WDG_RESET_REASON,
    POR_RESET_REASON,
    A9_DEEPSLEEP_RESET_REASON,
    APE_DEEPSLEEP_RESET_REASON,
    UNKNOWN_RESET_REASON           // Last one not linked to HW registers
} t_hw_reset_reason;

// Size of random key returned by sla_random_key_read in bytes
#define SIZE_OF_RANDOM_KEY      16

// Size of customer key returned by sla_customer_key_read in bytes
#define SIZE_OF_CUSTOMER_KEY    8

// Size of root key hash returned by sla_root_key_hash_read in bytes
#define SIZE_OF_ROOT_KEY_HASH   20

// Operations for SLA_CACHE_MAINTENANCE
typedef enum
{
    DCACHE_CLEAN            = 0x0001UL,
    DCACHE_AREA_CLEAN       = 0x0002UL,
    DCACHE_INVALIDATE       = 0x0003UL,
    DCACHE_AREA_INVALIDATE  = 0x0004UL,
    ICACHE_INVALIDATE       = 0x0005UL,
    ICACHE_AREA_INVALIDATE  = 0x0006UL,
    WRITE_BUFFER_DRAIN      = 0x0007UL,
    DCACHE_CLEAN_INV        = 0x0008UL,
    DCACHE_AREA_CLEAN_INV   = 0x0009UL,
    L2CACHE_INVALIDATE      = 0x000AUL,
    L2CACHE_AREA_INVALIDATE = 0x000BUL,
    L2CACHE_CLEAN           = 0x000CUL,
    L2CACHE_AREA_CLEAN      = 0x000DUL,
    L2CACHE_CLEAN_INV       = 0x000EUL,
    L2CACHE_AREA_CLEAN_INV  = 0x000FUL
    
} t_cache_operation_id;

////////////////////////////////////////////////
// Secure Library API return value type       //
////////////////////////////////////////////////

// Return code type
typedef t_uint32 SLA_RET;

// Structure of SLA_RET type
//  -----------------------------------------------------------------
//  | flags |        domain         |             code              |
//  -----------------------------------------------------------------
//   31   28 27                   16 15                            0
//
// flags: 0x0 = success / 0x8 = failure

// Macros for checking for success or failure
#define SUCCEEDED(ret)          (0 == ((SLA_RET)(ret) & 0x80000000))
#define FAILED(ret)             (0 != ((SLA_RET)(ret) & 0x80000000))

// Generic success codes
#define SLA_RET_FALSE                           ((SLA_RET)0x00000000)
#define SLA_RET_TRUE                            ((SLA_RET)0x00000001)
#define SLA_RET_OK                              ((SLA_RET)0x00000002)

// Generic failure codes (domain = 0x0000)
#define SLA_RET_FAIL                            ((SLA_RET)0x80000000)
#define SLA_RET_FAIL_INVALID_ARG                ((SLA_RET)0x80000001)
#define SLA_RET_FAIL_NULL_POINTER               ((SLA_RET)0x80000002)
#define SLA_RET_FAIL_INVALID_POINTER            ((SLA_RET)0x80000003)
#define SLA_RET_FAIL_DATA_SIZE                  ((SLA_RET)0x80000004)
#define SLA_RET_FAIL_CALL_SEQUENCE              ((SLA_RET)0x80000005)

// Hardware success codes (domain = 0x0001)
#define SLA_RET_DISABLED                        ((SLA_RET)0x00010001)
#define SLA_RET_FROZEN                          ((SLA_RET)0x00010002)
#define SLA_RET_ON                              ((SLA_RET)0x00010003)
#define SLA_RET_OFF                             ((SLA_RET)0x00010004)
#define SLA_RET_HIDDEN                          ((SLA_RET)0x00010005)
#define SLA_RET_TIMER_INTERRUPT_DISABLED        ((SLA_RET)0x00010006)
#define SLA_RET_TIMER_INTERRUPT_ENABLED         ((SLA_RET)0x00010007)
#define SLA_RET_TIMER_INTERRUPT_PENDING         ((SLA_RET)0x00010008)

// Hardware failure codes (domain = 0x0001)
#define SLA_RET_FAIL_HW_TIMEOUT                 ((SLA_RET)0x80010001)
#define SLA_RET_FAIL_HW_RNG                     ((SLA_RET)0x80010002)
#define SLA_RET_FAIL_HW_RESOURCE_BUSY           ((SLA_RET)0x80010003)
#define SLA_RET_FAIL_HW_NOT_INITIALIZED         ((SLA_RET)0x80010004)
#define SLA_RET_FAIL_HW_OVERFLOW                ((SLA_RET)0x80010005)

// Math success codes (domain = 0x0002)
#define SLA_RET_MATH_NEG_RESULT                     ((SLA_RET)0x00020001)
#define SLA_RET_MATH_NON_NEG_RESULT                 ((SLA_RET)0x00020002)
#define SLA_RET_MATH_IS_SMALLER                     ((SLA_RET)0x00020003)
#define SLA_RET_MATH_IS_BIGGER_OR_EQUAL             ((SLA_RET)0x00020004)
#define SLA_RET_MATH_ITH_BIT_ZERO                   ((SLA_RET)0x00020005)
#define SLA_RET_MATH_ITH_BIT_ONE                    ((SLA_RET)0x00020006)
#define SLA_RET_MATH_GCD_EQUALS_ONE                 ((SLA_RET)0x00020007)

// Math failure codes (domain = 0x0002)
#define SLA_RET_FAIL_MATH_DIVIDE_BY_ZERO        ((SLA_RET)0x80020001)
#define SLA_RET_FAIL_MATH_RESULT_TOO_BIG        ((SLA_RET)0x80020002)
#define SLA_RET_FAIL_MATH_NO_INVERSE                ((SLA_RET)0x80020003)
#define SLA_RET_FAIL_MATH_CANNOT_CONVERT_TO_POINT   ((SLA_RET)0x80020004)

// Crypto failure codes (domain = 0x0003)
#define SLA_RET_FAIL_UNSUPPORTED_ALGO           ((SLA_RET)0x80030001)
#define SLA_RET_FAIL_UNSUPPORTED_MODE           ((SLA_RET)0x80030002)
#define SLA_RET_FAIL_UNSUPPORTED_BITS               ((SLA_RET)0x80030003)
#define SLA_RET_FAIL_UNSUPPORTED_CURVE              ((SLA_RET)0x80030004)
#define SLA_RET_FAIL_TOO_LONG_NUMBER                ((SLA_RET)0x80030005)

// Callback success codes (domain = 0x0004)

// Callback failure codes (domain = 0x0004)
#define SLA_RET_FAIL_CALLBACK_UNREGISTERED      ((SLA_RET)0x80040001)
#define SLA_RET_FAIL_CALLBACK_IGNORE            ((SLA_RET)0x80040002)


////////////////////////////////////////////////
// Defines for SLA_MOVE_STACK                 //
////////////////////////////////////////////////
#define STACK_MODE_NORMAL               0
#define STACK_MODE_FIQ                  1
#define STACK_MODE_IRQ                  2
#define STACK_MODE_SVC                  3
#define STACK_MODE_ABORT                4
#define STACK_MODE_UNDEF                5
#define STACK_MODE_SYSTEM               6
#define STACK_MODE_MONITOR              7
#define STACK_MODE_USER                 8


////////////////////////////////////////////////
// Crypto-related types                       //
////////////////////////////////////////////////
typedef enum
{
    SHA1_HASH   = 0x1UL,
    SHA256_HASH = 0x2UL,
    SHA384_HASH = 0x4UL,
    SHA512_HASH = 0x8UL,
    SHA1_HMAC   = 0x10UL,
    SHA256_HMAC = 0x20UL,
    SHA384_HMAC = 0x40UL,
    SHA512_HMAC = 0x80UL
} t_hash_type;


#define SHA1_HASH_SIZE      20
#define SHA256_HASH_SIZE    32
#define SHA384_HASH_SIZE    48
#define SHA512_HASH_SIZE    64
#define MAX_HASH_SIZE       64

typedef enum
{
    RSASSA_PKCS1_V1_5_SIGNATURE  = 0x1UL,
    RSASSA_PSS_SIGNATURE = 0x2UL,
    ELGAMAL_SIGNATURE = 0x4UL,
    ECDSA_SIGNATURE = 0x8UL,
    DSA_SIGNATURE = 0x10UL
} t_signature_type;

/* Symmetric encryption schemes */ 
#define AES_CBC_SCHEME                  0x0001
#define TDES_CBC_SCHEME                 0x0002
#define ARC4_SCHEME                     0x0004              
#define XOR_ENC_SCHEME 					0x0008
#define AES_CBC_SCHEME128               0x0001
#define AES_CBC_SCHEME192               0x0010
#define AES_CBC_SCHEME256               0x0020   

/* MAC options */ 
#define HMAC_SHA1_160                   0x0001
#define HMAC_SHA1_80                    0x0002
#define HMAC_SHA256                     0x0004
  
/* KDF options */ 
#define KDF2_SHA1                       0x0001
#define KDF2_SHA256                     0x0002 
  
/* Used instead of point compression */ 
#define ECIES_NON_DHAES_MODE            0x0001
#define ECIES_DHAES_MODE                0x0002

/* Predefined curves for ECC */
#define NIST_P256_CURVE                 0x0001
#define WMDRM_CURVE                     0x0002

typedef enum
{
    CRYP_MODE_ECB    = 0x0001,
    CRYP_MODE_CBC    = 0x0002,
    CRYP_MODE_CTR32  = 0x0004,    
    CRYP_MODE_CTR64  = 0x0008,        
    CRYP_MODE_CTR96  = 0x0010,
    CRYP_MODE_CTR128 = 0x0020,
    CRYP_MODE_XTS    = 0x0040            
} t_cryp_mode;

#define ECB                             0x0001
#define CBC                             0x0002
#define CTR32                           0x0004
#define CTR64                           0x0008
#define CTR96                           0x0010
#define CTR128                          0x0020
#define XTS                             0x0040

#define DIR_ENC                         0x8
#define DIR_DEC                         0x0

// Cutting end for hash in op_mode
#define END_LEFT		0x0
#define END_RIGHT		0x8

// DMA interrupt callback function signature
typedef void (*DmaCallback)(t_uint32 err);

#define xts_DEC                         DIR_DEC
#define xts_ENC                         DIR_ENC

#define NC_AES_BLK_BYTES (16)


// DMA flags
#define DMA_OFF                 0x0
#define DMA_ON                  0x1
#define DMA_INTERRUPT           0x2
#define DMA_CONDITIONAL_ON      0x4

// Other flags in aes op_mode
#define CRYPTION_DIRECTION     0x8

// Other flags in hash/hmac op_mode
#define CUTTING_END            0x8

// Integer size definitions
#define MAX_RSABITS             2048
#define MAX_ELGAMALBITS         2048
#define MAX_DHBYTES             256
#define DSABITS                 2048

#define SHORT_BIGINT_COORDS     (MAX_RSABITS/32+3)
#define BIGINT_COORDS           (MAX_RSABITS/16+3)
#define LONG_BIGINT_COORDS      (MAX_RSABITS/8+3)
#define LLONG_BIGINT_COORDS     (MAX_RSABITS/4+3)
#define MAX_RSA_MODULUS_SIZE    (MAX_RSABITS/8)        


typedef struct
{
  t_uint16 dummy;
  t_uint16 d[SHORT_BIGINT_COORDS+3];
} SHORT_BIGINT_STRUCT;

typedef struct
{
  t_uint16 dummy;
  t_uint16 d[BIGINT_COORDS+5];
} BIGINT_STRUCT;

typedef struct
{
  t_uint16 dummy;
  t_uint16 d[LONG_BIGINT_COORDS+3];
} LONG_BIGINT_STRUCT;

typedef union
{
  SHORT_BIGINT_STRUCT a;
  t_uint32 dummy;
} t_short_bigint;

typedef union
{
  BIGINT_STRUCT a;
  t_uint32 dummy;
} t_bigint;

typedef union
{
  LONG_BIGINT_STRUCT a;
  t_uint32 dummy;
} t_long_bigint;

/* Small bigint-style struct to allign small integers for ASM-optimisation */

typedef struct
{
    t_uint16 dummy;
    t_uint16 d[3];
} TINY_BIGINT_STRUCT;

typedef union
{
    TINY_BIGINT_STRUCT a;
    t_uint32 dummy;
} tiny_bigint;


//
//
// Encryption support
//
//

//ARC4
typedef struct
{
  t_uint8 S[256]; /* Permutation table. */
  t_uint8 counti;   /* internal counter.*/
  t_uint8 countj;   /* internal counter.*/
} t_arc4_context;

//TDES
#define NC_DES_DATASIZE 8


//AES
typedef struct
{
    t_uint32 IvReg1;
    t_uint32 IvReg2;
    t_uint32 IvReg3;
    t_uint32 IvReg4;
} t_aes_init_vect_desc;


typedef struct
{
    t_uint32 Size;
    t_uint32 Key1L;
    t_uint32 Key1H;
    t_uint32 Key2L;
    t_uint32 Key2H;
    t_uint32 Key3L;
    t_uint32 Key3H;
    t_uint32 Key4L;
    t_uint32 Key4H;
} t_aes_key_desc;

typedef struct
{
    t_aes_key_desc *KeyBunch;
    t_aes_init_vect_desc *IvRegPtr;
    t_uint8 Mode;
    t_uint8 AutoIdle;
    t_uint8 direction;
} t_aes_init_desc;


typedef struct
{
    t_uint32* data_Loc;
    t_uint32* data_Dest;
    t_uint32  data_Size;
} t_aes_update_desc;

typedef struct
{
    t_uint32 count;
    t_uint8 buffer[64];
    t_uint8 index;
    t_uint8 algo;
    t_uint8 spare;
} SHA1_BUFF_STR;

typedef struct
{
    SHA1_BUFF_STR in;
    t_uint32 digest[SHA1_HASH_SIZE/sizeof(t_uint32)];
} SHA1_SAVE_STR;


//
// ELGamal structures
//
typedef struct
{
    t_bigint p;
    t_bigint g;
    t_uint32 privkeyLen;
} t_elgamal_params;

// RSA public key descriptor
typedef struct
{
    t_uint32 e;           /**< Public exponent*/
    t_bigint n;           /**< Modulus*/
} t_rsa_public_key;

// RSA private key descriptor
typedef struct
{
  t_short_bigint p;              /**< First prime number*/
  t_short_bigint q;              /**< Second prime number*/
  t_short_bigint exponent1;      /**< First exponent*/
  t_short_bigint exponent2;      /**< Second exponent*/
  t_short_bigint coefficient;    /**< Coefficient*/
} t_rsa_private_key;



//
// DSA
//

typedef struct
{
    t_bigint p;
    t_short_bigint q;
    t_bigint g;
} t_dsa_params;


//
// Diffie Hellman
//

typedef struct
{
  t_bigint p;
  t_bigint g;
  t_uint32 privLen;
} t_dh_params;

/* Diffie Hellman public value */
typedef t_uint8    DHPublicValue[128];

/* Diffie Hellman private value */
typedef t_bigint   DHPrivateValue;

/*****************************************************
 * Elliptic Curve Cryptography
 */

#define GFP_COORDS        (32)

typedef struct
{
    t_uint16 dummy;
    t_uint16 d[GFP_COORDS+3];
} t_gfp_struct;

typedef union
{
    t_gfp_struct a;
    t_uint32 dummy;
} t_gfp_coord;

typedef struct
{
    t_gfp_coord x;
    t_gfp_coord y;
} t_gfp_point;

typedef struct
{
    t_gfp_coord a;
    t_gfp_coord b;
    t_gfp_coord p;
} t_gfp_red_curve;

typedef struct
{
    t_gfp_red_curve e;
    t_gfp_point g;
    t_gfp_coord order;
} t_gfp_curve;



////////////////////////////////////////////////
// Reset reasons                              //
////////////////////////////////////////////////

/** \internal Several reset reasons are defined in assemble file too. In case of changes, both files must be impacted.\n
    These reasons are SEC_RESET_UNDEF_INSTR (1), SEC_RESET_SWI_CALLED (2), SEC_RESET_PREFETCH_ABORT (3), SEC_RESET_DATA_ABORT (4), SEC_RESET_EXIT_FAILURE (5), SEC_RESET_DEEP_SLEEP_WATCHDOG (12), SEC_RESET_DEEP_SLEEP_BAD_MAGIC (13), SEC_RESET_UNEXPECTED_EXCEPTION (19).\n*/
#define SEC_RESET_REASONS
#define SEC_RESET_UNDEF_INSTR                1      /**< Undef exception occured */
#define SEC_RESET_SWI_CALLED                 2      /**< No more used, kept for compatibility reason */
#define SEC_RESET_PREFETCH_ABORT             3      /**< Prefetch abort exception occured */
#define SEC_RESET_DATA_ABORT                 4      /**< Data abort exception occured */
#define SEC_RESET_EXIT_FAILURE               5      /**< Incorrect public exit address */
#define SEC_RESET_OTP_FAILURE                6      /**< Incorrect root key hash value */
#define SEC_RESET_CHIP_TYPE_FAILURE          7      /**< No more used, kept for compatibility reason */
#define SEC_RESET_ISSW_CERT_FAILURE          8      /**< Checked ISSW certificate has been found to be faulty. */
#define SEC_RESET_NON_VALID_ADDRESS          9      /**< Non-valid address has been passed to secure mode( pointing to secure mode memories or I/O) */
#define SEC_RESET_ISSW_LOAD_FAILURE          10     /**< There has been an attempt to load ISSW again while ISSW is already loaded. */
#define SEC_RESET_C_LIBRARY_FAILURE          11     /**< C library has reported a failure (div by zero etc) */
#define SEC_RESET_SLA_SERVICE_NOT_FOUND      12     /**< Security Library service not found from secure ROM */
#define SEC_RESET_DIVIDE_BY_ZERO             13     /**< Divide bye zero attempt. */

#define SEC_RESET_DEEP_SLEEP_BAD_MAGIC       20     /**< Bad magic number during exit from deep sleep */
#define SEC_RESET_DEEP_SLEEP_SECTION         21     /**< Proble while restoring memory areas during exit from deep sleep*/
#define SEC_RESET_DEEP_SLEEP_SECTION_HASH    22     /**< Bad hash verification during exit from deep sleep */
#define SEC_RESET_DEEP_SLEEP_DECRYPT_ERROR   23     /**< Error during page decryption */
#define SEC_RESET_DEEP_SLEEP_HASH_ERROR      24     /**< Error during hash calculation */
#define SEC_RESET_DEEP_SLEEP_POWER_MODE      25     /**< Error Bad power mode detected */
#define SEC_RESET_DEEP_SLEEP_UNEXPECTED      26     /**< Unexpected exception caught during deep sleep exit */

#define SEC_RESET_UNEXPECTED_EXCEPTION       30     /**< Unexpected exception occured */

#endif /* End of sla_types.h */

/** @} */
