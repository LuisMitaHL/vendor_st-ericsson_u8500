/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 *
 * \file issw_api.h
 * \brief definition of the secure API
 *
 * This header file provides security types and definitions related to the ISSW API.
 *
 */
/*---------------------------------------------------------------------------*/
#ifndef ISSW_API_H
#define ISSW_API_H

#include "isswapi_types.h"
#include "isswapi_indexes.h"

/** Secure services identification numbers
 *
 * Here is given the description of the functions that are called by the dispatcher \n
 * when user requests a service number */
typedef enum
{
    // "ISSWAPI_ISSW_EXECUTE_SERV" is linked to ROM code ("SSAPI_ISSW_EXECUTE_SERV")
#if defined(__STN_8500) && (__STN_8500 == 01 )
    ISSWAPI_ISSW_EXECUTE_SERV       = 5,
#else
    ISSWAPI_ISSW_EXECUTE_SERV       = 6,
#endif
    ISSWAPI_PROT_APPL_MSG_SEND      = 0x10000000,
    ISSWAPI_EXTERNAL_CODE_CHECK     = 0x10000001,
    ISSWAPI_SECURE_LOAD             = 0x10000002,
    ISSWAPI_ISSW_REIMPORT_PUB_KEYS  = 0x10000003,

    // Accessible only on specific images delivered to "specific" people
    ISSWAPI_WRITE_L2CC              = 0x10000004,
    ISSWAPI_WRITE_CP15_SCTLR        = 0x10000005,
    ISSWAPI_READ_CP15_SCTLR         = 0x10000006,
    ISSWAPI_WRITE_CP15_ACTLR        = 0x10000007,
    ISSWAPI_READ_CP15_ACTLR         = 0x10000008,
    ISSWAPI_WRITE_CP15_DIAGR        = 0x10000009,
    ISSWAPI_READ_CP15_DIAGR         = 0x1000000A,

    ISSWAPI_EXECUTE_TA              = 0x11000001,
    ISSWAPI_CLOSE_TA                = 0x11000002,
    ISSWAPI_FLUSH_BOOT_CODE         = 0x11000003,
    /* 5500 only */
    ISSWAPI_DDR_CONFIG              = 0x11000004,
    /* Generic, restricted to be used by u-boot */
    ISSWAPI_VERIFY_SIGNED_HEADER    = 0x11000005,
    ISSWAPI_VERIFY_HASH             = 0x11000006,

    ISSWAPI_L2CC_DISABLE_INDEX      = 0x20000002,
    ISSWAPI_L2CC_ENABLE_INDEX       = 0x20000003
} t_issw_service_id;


#ifndef FIND_SERVICE_ADDR_DEFINED
#define FIND_SERVICE_ADDR_DEFINED
typedef void (*find_service_addr_ret_t)(void);
extern find_service_addr_ret_t (*find_service_addr) (t_uint32);
#endif

// ISSWAPI_VA2PA
#define  ISSWAPI_VA2PA(virtual_addr, physical_addr) \
    ((t_issw_return_value (*)(void *, void **))(*find_service_addr)(ISSWAPI_VA2PA_INDEX)) \
    (virtual_addr, physical_addr)

// ISSWAPI_INVALIDATE_MMU_TLB
#define  ISSWAPI_INVALIDATE_MMU_TLB() \
    ((t_issw_return_value (*)(void))(*find_service_addr)(ISSWAPI_INVALIDATE_MMU_TLB_INDEX)) \
    ()

/* tee_gstate_alloc() */
#define ISSWAPI_GSTATE_ALLOC(tgid, len) \
    ((void * (*)(int, size_t))(*find_service_addr)\
        (ISSWAPI_GSTATE_ALLOC_INDEX))(tgid, len)

/* tee_gstate_free() */
#define ISSWAPI_GSTATE_FREE(tgid) \
    ((void (*)(int))(*find_service_addr)(ISSWAPI_GSTATE_FREE_INDEX))(tgid)

/* tee_gstate_get() */
#define ISSWAPI_GSTATE_GET(tgid) \
    ((void * (*)(int))(*find_service_addr)(ISSWAPI_GSTATE_GET_INDEX))(tgid)

/* tee_verify_signed_header() */
#define ISSWAPI_VERIFY_SIGNED_HEADER(hdr, pl_type, pub_key) \
    ((t_uint32 (*)(const issw_signed_header_t *, enum issw_payload_type, \
                   t_rsa_public_key *)) \
        (*find_service_addr)(ISSWAPI_VERIFY_SIGNED_HEADER_INDEX))(hdr, pl_type, pub_key)

/* tee_pk_load_key() */
#define ISSWAPI_PK_LOAD_KEY(pt, pk) \
    ((t_uint32 (*)(enum issw_payload_type, t_rsa_public_key **)) \
     (*find_service_addr)(ISSWAPI_PK_LOAD_KEY_INDEX))(pt, pk)

/* tee_pk_unload_key() */
#define ISSWAPI_PK_UNLOAD_KEY(pk) \
    ((void (*)(t_rsa_public_key **)) \
     (*find_service_addr)(ISSWAPI_PK_UNLOAD_KEY_INDEX))(pk)

/*
 * Copies the TA of the current session to reserved memory and disables
 * closing of the current session.
 *
 * Note, this function is only to be called from the TA.
 *
 * Returns TEEC_SUCCESS on success and TEEC_ERROR_OUT_OF_MEMORY
 * if no reserved memory was available.
 */
#define ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT() \
    ((t_uint32 (*)(void)) \
        (*find_service_addr)(ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT_INDEX))()

/*
 * Enables closing of the current session again.
 *
 * Note, this function is only to be called from the TA.
 */
#define ISSWAPI_TEE_UNLOCK_CURRENT_SESSION() \
    ((void (*)(void)) \
        (*find_service_addr)(ISSWAPI_TEE_UNLOCK_CURRENT_SESSION_INDEX))()

/* ISSWAPI_IsProdChip() */
#define ISSWAPI_IS_PRODCHIP() \
    ((t_bool (*)(void)) \
     (*find_service_addr)(ISSWAPI_IS_PRODCHIP_INDEX))()


/* tee_get_cutid() */
#define ISSWAPI_GET_CUTID() \
    ((t_uint32 (*)(void)) \
     (*find_service_addr)(ISSWAPI_GET_CUTID_INDEX))()

/* tee_get_rt_flags() */
#define ISSWAPI_GET_RT_FLAGS() \
    ((t_uint32 (*)(void)) \
     (*find_service_addr)(ISSWAPI_GET_RT_FLAGS_INDEX))()

/* ISSWAPI_Enable_SecWD() */
#define ISSWAPI_ENABLE_SECWD() \
    ((void (*)(void)) \
     (*find_service_addr)(ISSWAPI_ENABLE_SECWD_INDEX))()

#endif
