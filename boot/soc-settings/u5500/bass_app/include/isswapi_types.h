/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
    \file       issw_types.h
    \brief      This file contains types related to the secure library API.

     \addtogroup SLA_TYPES
    @{
 */
/*----------------------------------------------------------------------*/

#ifndef ISSWAPI_TYPES_H
#define ISSWAPI_TYPES_H

#include "boot_secure_types.h"

/* Hardware Cut IDs */
#define CUT_ID_DB8500_ED                        0x00850001
#define CUT_ID_DB8500_V1                        0x008500a0
#define CUT_ID_DB8500_V11                       0x008500a1
#define CUT_ID_DB8500_V2                        0x008500b0
#define CUT_ID_DB8500_V21                       0x008500b1
#define CUT_ID_DB8500_V22                       0x008500b2
#define CUT_ID_DB8500_V1_40                     0x408500a0
#define CUT_ID_AB8500_ED                        0x00
#define CUT_ID_AB8500_V1                        0x10
#define CUT_ID_AB8500_V1_1                      0x11
#define CUT_ID_AB8500_V2                        0x20
#define CUT_ID_AB8500_V3                        0x30
/* CUT_ID_AB8500_V3 implies CUT_ID V3_1 and V3_2 */
#define CUT_ID_AB8500_V3_3                      0x33
#define CUT_ID_AB8500_MPW1                      0x90
#define CUT_ID_AB8500_MPW2                      0xA0

/**
 * Product id numbers
 * Note: Needs to be replicated for bass_app.
 */
enum tee_product_id {
    PRODUCT_ID_UNKNOWN = 0,
    PRODUCT_ID_8400    = 1,
    PRODUCT_ID_8500    = 2,
    PRODUCT_ID_9500    = 3,
    PRODUCT_ID_5500    = 4,
    PRODUCT_ID_7400    = 5,
    PRODUCT_ID_8500C   = 6,
};

/* Flags indicating run-time environment */
#define TEE_RT_FLAGS_NORMAL             0x00000000
#define TEE_RT_FLAGS_MASK_ITP_PROD      0x00000001
#define TEE_RT_FLAGS_MODEM_DEBUG        0x00000002
#define TEE_RT_FLAGS_RNG_REG_PUBLIC     0x00000004
#define TEE_RT_FLAGS_JTAG_ENABLED       0x00000008

/* Flags indicating fuses */
#define TEE_FUSE_FLAGS_MODEM_DISABLE    0x00000001

struct tee_product_config {
    t_uint32    product_id;
    t_uint32    rt_flags;
    t_uint32    fuse_flags;
};

#define MODDDR_LOW_ADDRESS                      0x06000000
#define MODDDR_SHAREHIGH_ADDRESS                0x06FFFFFF
/* Workaround for ER316693: Problem with getting modem started when
 * modem memory boundary is set in ISSW.
 * When we set MODHIGH to 0x07FFFFF according to modem momery usage
 * specification, the kernel dumps at startup due to the fact that
 * modem is not actually started! As a workaround, we set MODHIGH to
 * 0x0800FFFF, then it works OK. So we suspect either misusing of
 * memory in modem side or some issues in memory protection unit in
 * hardware! */
//#define MODDDR_HIGH_ADDRESS                     0x07FFFFFF
#define MODDDR_HIGH_ADDRESS                     0x0800FFFF

//
//
// ISSW API return value type
//
//

// Return code type
typedef t_uint32 t_issw_return_value;

// Structure of SLA_RET type
//  -----------------------------------------------------------------
//  | flags |        domain         |             code              |
//  -----------------------------------------------------------------
//   31   28 27                   16 15                            0
//
// flags: 0x0 = success / 0x8 = failure

// Macros for checking for success or failure
#define ISSW_SERVICE_SUCCEEDED(ret)          (0 == ((t_issw_return_value)(ret) & 0x90000000))
#define ISSW_SERVICE_FAILED(ret)             (0 != ((t_issw_return_value)(ret) & 0x90000000))

// Success codes (domain = D, error code =Y)
// #define ISSW_RET_OK_REASON_X      ((SLA_RET)0x000Y000X)
#define ISSW_RET_OK      ((t_issw_return_value)0x00000001)

// Failure codes (domain = D, error code =Y)
// #define ISSW_RET_FAIL_ERROR_X      ((SLA_RET)0x900Y000X)
#define ISSW_RET_FAIL                   ((t_issw_return_value)0x90000001)
#define ISSW_RET_NON_SUPPORTED_APPL     ((t_issw_return_value)0x90000002)
#define ISSW_RET_NON_VALID_ADDRESS      ((t_issw_return_value)0x90000003)
#define ISSW_RET_MMU_TRANSLATION_FAULT  ((t_issw_return_value)0x90000004)
#define ISSW_RET_INVALID_ARGS           ((t_issw_return_value)0x90000005)

/*
 * The following enum must be shared between the ISSW and all
 * other trusted applications. Consider binary compatability
 * before changing value of any value in the enum.
 */
typedef enum tee_gid {
    TEE_GID_SIPC_KEY                = 0,    /* non-persistent, gen by TEE */
    TEE_GID_COPS_STATE              = 1,
    TEE_GID_COPS_CHALLENGE          = 3,
    TEE_GID_COPS_DATA_KEY           = 4,    /* persistent, gen by TEE */
    TEE_GID_COPS_BINDDATA_KEY       = 5,    /* persistent, gen by TEE */
    TEE_GID_COPS_SIPC_MODEM_COUNTER = 6,    /* message counter, modem -> APE */
    TEE_GID_COPS_SIPC_APE_COUNTER   = 7,    /* message counter, APE -> modem */
    /* Offset from which a customer can add */
    TEE_GID_CUSTOM_OFFSET           = 8,
} tee_gid;

#define TEE_GID_SIPC_KEY_SIZE           64
#define TEE_GID_COPS_DATA_KEY_SIZE      64
#define TEE_GID_COPS_BINDDATA_KEY_SIZE  64

enum issw_payload_type {
  ISSW_PL_TYPE_TAPP = 0,
  ISSW_PL_TYPE_PRCMU,
  ISSW_PL_TYPE_MEMINIT,
  ISSW_PL_TYPE_X_LOADER,
  ISSW_PL_TYPE_OS_LOADER,
  ISSW_PL_TYPE_APE_NW_CODE,
  ISSW_PL_TYPE_FC_LOADER,
  ISSW_PL_TYPE_MODEM_CODE,
  ISSW_PL_TYPE_FOTA,
  ISSW_PL_TYPE_DNTCERT,
  ISSW_PL_TYPE_AUTHCERT,
  ISSW_PL_TYPE_IPL,
  ISSW_PL_TYPE_FLASH_ARCHIVE,
  ISSW_PL_TYPE_ITP,
  ISSW_PL_TYPE_AUTH_CHALLENGE = -1 /* 0xffffffff */
};

typedef struct issw_signed_header {
    t_uint32    magic;
    t_uint16    size_of_signed_header;
    t_uint16    size_of_signature;
    t_uint32    sign_hash_type; /* see t_hash_type */
    t_uint32    signature_type; /* see t_signature_type */
    t_uint32    hash_type;      /* see t_hash_type */
    t_uint32    payload_type;   /* see enum issw_payload_type */
    t_uint32    flags;          /* reserved */
    t_uint32    size_of_payload;
    t_uint32    sw_vers_nbr;
    t_uint32    load_address;
    t_uint32    startup_address;
    t_uint32    spare;          /* reserved */
#if 0
    /* Pseudo code visualize layout of signed header */
    t_uint8     hash[get_hash_length(this.hash_type)];
    t_uint8     signature[size_of_signature];
#endif
} issw_signed_header_t;

#define ISSW_SIGNED_HEADER_MAGIC  0x53484452

#define ISSW_SIGNED_HEADER_HASH(hdr) \
    ((t_uint8 *)((issw_signed_header_t *)(hdr) + 1))

#define ISSW_SIGNED_HEADER_HASH_SIZE(hdr) \
    (((issw_signed_header_t *)(hdr))->size_of_signed_header - \
        ((issw_signed_header_t *)(hdr))->size_of_signature - \
        sizeof(issw_signed_header_t))

#define ISSW_SIGNED_HEADER_SIGNATURE(hdr) \
    (ISSW_SIGNED_HEADER_HASH(hdr) + ISSW_SIGNED_HEADER_HASH_SIZE(hdr))

#define ISSW_SIGNED_HEADER_PAYLOAD(hdr) \
    ((t_uint8 *)(hdr) + ((issw_signed_header_t *)(hdr))->size_of_signed_header)

/*
 * Flags passed in r1 to xloader when invoked at boot
 */

/* PRCMU was reset */
#define BOOT_STATUS_PRCMU_RESET 0x1
/* PRCMU failed to start => stop somewhere in xloader to simplify debug */
#define BOOT_STATUS_PRCMU_FAIL  0x2


#define ISSW_NUM_DDR_PARAMETERS  16

#define ISSW_AVS_PARAMETERS_LEN  40

#endif /* End of issw_types.h */

/** @} */
