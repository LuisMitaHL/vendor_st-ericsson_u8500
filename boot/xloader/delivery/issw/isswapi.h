/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*
 * This header file provides security types and definitions related to the
 * ISSW API.
 */

#ifndef ISSW_API_H
#define ISSW_API_H

#include "isswapi_types.h"
#include "isswapi_indexes.h"

/** Secure services identification numbers
 *
 * Here is given the description of the functions that are called by the
 * dispatcher when user requests a service number
 */
typedef enum
{
    /*
     * "ISSWAPI_ISSW_EXECUTE_SERV" is linked to ROM code
     * ("SSAPI_ISSW_EXECUTE_SERV")
     */
    ISSWAPI_ISSW_EXECUTE_SERV       = 6,
    ISSWAPI_PROT_APPL_MSG_SEND      = 0x10000000,
    ISSWAPI_EXTERNAL_CODE_CHECK     = 0x10000001,
    ISSWAPI_SECURE_LOAD             = 0x10000002,
    ISSWAPI_ISSW_REIMPORT_PUB_KEYS  = 0x10000003,

    /* Accessible only on request */
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
    /* Generic, restricted to be used by u-boot */
    ISSWAPI_VERIFY_SIGNED_HEADER    = 0x11000005,
    ISSWAPI_VERIFY_HASH             = 0x11000006,
    /* 8500 only, restricted to be used by u-boot */
    ISSWAPI_GET_RT_FLAGS            = 0x11000007,

    /* For TEE Client API 1.0 */
    ISSWAPI_TEEC_OPEN_SESSION       = 0x11000008,
    ISSWAPI_TEEC_CLOSE_SESSION      = 0x11000009,
    ISSWAPI_TEEC_INVOKE_COMMAND     = 0x1100000a,
    ISSWAPI_TEEC_REGISTER_RPC       = 0x1100000b,
    ISSWAPI_TEEC_SET_SEC_DDR        = 0x1100000c,

    ISSWAPI_L2CC_DISABLE_INDEX      = 0x20000002,
    ISSWAPI_L2CC_ENABLE_INDEX       = 0x20000003,
    ISSWAPI_LOAD_TEE                = 0x20000004
} t_issw_service_id;


#ifndef FIND_SERVICE_ADDR_DEFINED
#define FIND_SERVICE_ADDR_DEFINED
typedef void (*find_service_addr_ret_t)(void);
extern find_service_addr_ret_t (*find_service_addr) (t_uint32);
#endif

/*
 * ISSWAPI_VA2PA
 *
 * Translate a virtual address into a physical address
 *
 * IN:         virtual_addr  Virtual address to be translated
 * OUT:        physical_addr Results of the translation
 *
 * RETURN:     ISSW_RET_OK                       Translation OK
 *             ISSW_RET_MMU_TRANSLATION_FAULT    Fault during translation
*/
/* ISSWAPI_Va2Pa */
#define ISSWAPI_VA2PA(virtual_addr, physical_addr)                      \
    ((t_issw_return_value (*)(void *, void **))                         \
     (*find_service_addr)(ISSWAPI_VA2PA_INDEX))(virtual_addr, physical_addr)

/*
 * ISSWAPI_INVALIDATE_MMU_TLB
 *
 * This function invalidates the whole MMU TLB
 *
 * RETURN:     ISSW_RET_OK                       Invalidate operation OK
 */
/* ISSWAPI_InvalidateMmuTbl */
#define ISSWAPI_INVALIDATE_MMU_TLB()                            \
    ((t_issw_return_value (*)(void))                            \
     (*find_service_addr)(ISSWAPI_INVALIDATE_MMU_TLB_INDEX))()

/*
 * ISSWAPI_GSTATE_ALLOC
 *
 * Allocates memory in the global heap.
 *
 * If the memory already is allocated for the gid, NULL is
 * returned.
 *
 * IN:         tgid    The id of the global data buffer
 *             len     Length of buffer to allcoate
 * OUT:        Pointer to data allocated for gid. NULL if failure
 */
/* tee_gstate_alloc() */
#define ISSWAPI_GSTATE_ALLOC(tgid, len)                             \
    ((void * (*)(int, size_t))                                      \
     (*find_service_addr)(ISSWAPI_GSTATE_ALLOC_INDEX))(tgid, len)

/*
 * ISSWAPI_GSTATE_FREE
 *
 * Frees memory previously allocated. Verifies the address
 * towards the supplied gid.
 *
 * IN:         tgid     The id of the global data allocated.
 */
/* tee_gstate_free() */
#define ISSWAPI_GSTATE_FREE(tgid)                                       \
    ((void (*)(int))(*find_service_addr)(ISSWAPI_GSTATE_FREE_INDEX))(tgid)

/*
 * ISSWAPI_GSTATE_GET
 *
 * Returns a previously allocated pointer to global storage.
 * NULL if it doesn't exist
 *
 * IN:         tgid      The id of the global data allocated.
 */
/* tee_gstate_get() */
#define ISSWAPI_GSTATE_GET(tgid)                                        \
    ((void * (*)(int))(*find_service_addr)(ISSWAPI_GSTATE_GET_INDEX))(tgid)

/*
 * ISSWAPI_VERIFY_SIGNED_HEADER
 *
 * Verifies STE signed header
 *
 * IN:         hdr       Pointer to signed header
 *             pl_type   Expected payload type. Payload type defined as
 *                       ISSW_PL_TYPE* in issw_types.h
 *             pub_key   Public key used for verification. Only used for
 *                       payload type ISSW_PL_TYPE_AUTH_CHALLENGE.
 * RETURN:     TEEC_SUCESS if verification is ok
 */
/* tee_signed_header_verify() */
#define ISSWAPI_VERIFY_SIGNED_HEADER(hdr, pl_type, pub_key)             \
    ((t_uint32 (*)(const issw_signed_header_t *, enum issw_payload_type, \
                   t_rsa_public_key *))(*find_service_addr)             \
     (ISSWAPI_VERIFY_SIGNED_HEADER_INDEX))(hdr, pl_type, pub_key)
/*
 * ISSWAPI_PK_LOAD_KEY
 *
 * Load the current registered key for the payload type defined by pt.
 *
 * IN:         pt        Payload type, defined as ISSW_PL_TYPE* in issw_types.h
 *             pk        Key associated with payload type pt
 * RETURN:     TEEC_SUCESS if payload type exists
 */
/* tee_pk_load_key() */
#define ISSWAPI_PK_LOAD_KEY(pt, pk)                                 \
    ((t_uint32 (*)(enum issw_payload_type, t_rsa_public_key **))    \
     (*find_service_addr)(ISSWAPI_PK_LOAD_KEY_INDEX))(pt, pk)

/*
 * ISSWAPI_PK_UNLOAD_KEY
 *
 * Unload the key recieved by ISSWAPI_PK_LOAD_KEY.
 *
 * IN:         pk        The key
 */
/* tee_pk_unload_key() */
#define ISSWAPI_PK_UNLOAD_KEY(pk)                           \
    ((void (*)(t_rsa_public_key **))                        \
     (*find_service_addr)(ISSWAPI_PK_UNLOAD_KEY_INDEX))(pk)

/*
 * Copies the TA of the current session to reserved memory and disables
 * closing of the current session.
 *
 * Note, this function is only to be called from the TA itself.
 *
 * RETURN:     TEEC_SUCCESS on success and TEEC_ERROR_OUT_OF_MEMORY
 *             if no reserved memory was available.
 */
#define ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT()                     \
    ((t_uint32 (*)(void))                                               \
     (*find_service_addr)(ISSWAPI_TEE_MAKE_CURRENT_SESSION_RESIDENT_INDEX))()

/*
 * Enables closing of the current session again.
 *
 * Note, this function is only to be called from the TA itself.
 */
#define ISSWAPI_TEE_UNLOCK_CURRENT_SESSION()                            \
    ((void (*)(void))                                                   \
     (*find_service_addr)(ISSWAPI_TEE_UNLOCK_CURRENT_SESSION_INDEX))()

/*
 * ISSWAPI_IS_PRODCHIP
 *
 * This function reads the AF "DEV" bit to know if it is a "Dev" chip or a
 * "Prod" chip.
 * AF "DEV" bit location : FVR7 bit14
 *
 * RETURN:     TRUE    If it is a "Prod" Chip
 *             FALSE   If it is a "Test" Chip
 */
/* ISSWAPI_IsProdChip() */
#define ISSWAPI_IS_PRODCHIP()                           \
    ((t_bool (*)(void))                                 \
     (*find_service_addr)(ISSWAPI_IS_PRODCHIP_INDEX))()

/*
 * ISSWAPI_GET_CUTID
 *
 * This function returns the cut id of DBB hardware
 *
 * RETURN:    CUT_ID_DB* defined in isswapi_types.h
 */
/* tee_get_cutid() */
#define ISSWAPI_GET_CUTID()                             \
    ((t_uint32 (*)(void))                               \
     (*find_service_addr)(ISSWAPI_GET_CUTID_INDEX))()

/*
 * ISSWAPI_GET_RT_FLAGS
 *
 * This function returns the run-time environment status defined in
 * isswapi_types.h as TEE_RT_FLAGS_*
 *
 * RETURN:    TEE_RT_FLAGS_* defined in isswapi_types.h
 */
/* tee_get_rt_flags() */
#define ISSWAPI_GET_RT_FLAGS()                              \
    ((t_uint32 (*)(void))                                   \
     (*find_service_addr)(ISSWAPI_GET_RT_FLAGS_INDEX))()

/*
 * ISSWAPI_ENABLE_SECWD
 *
 * This function enables secure watchdog
 */
/* ISSWAPI_Enable_SecWD() */
#define ISSWAPI_ENABLE_SECWD()                              \
    ((void (*)(void))                                       \
     (*find_service_addr)(ISSWAPI_ENABLE_SECWD_INDEX))()

/* tee_clear_eng_state */
#define ISSWAPI_CLEAR_ENG_STATE() \
    ((void (*)(void)) \
     (*find_service_addr)(ISSWAPI_CLEAR_ENG_STATE_INDEX))()

/* tee_arb_init_table */
#define ISSWAPI_INIT_ARB_TABLE(mi) \
    ((uint32_t (*)(uint16_t)) \
    (*find_service_addr)(ISSWAPI_INIT_ARB_TABLE_INDEX))(mi)

/* tee_arb_get_table */
#define ISSWAPI_GET_ARB_TABLE(order, tocstr, table, len) \
    ((uint32_t (*)(enum arb_table_order, char **, uint8_t **, size_t *)) \
    (*find_service_addr) \
        (ISSWAPI_GET_ARB_TABLE_INDEX))(order, tocstr, table, len)

/* tee_ste_dnt_init_data() */
#define ISSWAPI_STE_DNT_INIT_DATA(flags, hash_list, list_len, data, data_len) \
    ((uint32_t (*)(uint32_t, uint8_t *, uint32_t, uint8_t **, uint32_t *)) \
        (*find_service_addr)(ISSWAPI_STE_DNT_INIT_DATA_INDEX))(flags, \
            hash_list, list_len, data, data_len)

/* tee_ste_dnt_verify_hash() */
#define ISSWAPI_STE_DNT_VERIFY_HASH(data, data_len, boot_part_locked) \
    ((void (*)(const uint8_t *, uint32_t, bool)) \
        (*find_service_addr)(ISSWAPI_STE_DNT_VERIFY_HASH_INDEX))(data, \
            data_len, boot_part_locked)

#define ISSWAPI_VERIFY_SIGNED_HEADER2(hdr, hdr_len, pl_type, pub_key, eng_mode)\
    ((t_uint32 (*)(const issw_signed_header_t *, uint32_t, \
                    enum issw_payload_type, t_rsa_public_key *, uint32_t *)) \
    (*find_service_addr)(ISSWAPI_VERIFY_SIGNED_HEADER2_INDEX))(hdr, hdr_len, \
            pl_type, pub_key, eng_mode)

#define ISSWAPI_GET_CLIENT_ID(id) \
    ((uint32_t (*)(TEE_Identity *)) \
    (*find_service_addr)(ISSWAPI_GET_CLIENT_ID_INDEX))(id)

#endif
