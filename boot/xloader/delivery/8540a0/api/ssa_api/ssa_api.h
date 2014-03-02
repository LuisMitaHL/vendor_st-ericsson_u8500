/*-----------------------------------------------------------------------------
 * STMicroelectronics
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *---------------------------------------------------------------------------*/
/*
 * \file ssa_api.h
 * \author STMicroelectronics
 *
 * This module defines macros and types used to call secure services (SSA API).
 *
 *---------------------------------------------------------------------------*/
/** \internal
 *  \addtogroup SSA_API
 * @{
 */
 
#ifndef SSA_API_H
#define SSA_API_H

#include "boot_secure_types.h"

/*
 * This structure is read-only for public ROM code but sec code writes to it
 * to pass parameters to the public side. Public code has a single statically
 * allocated instance of this structure the address of which is passed to
 * secure ROM in the call to SSA_PRE_INIT.
 */
typedef struct
{
    t_uint32 checksum;                  /**< Secure ROM checksum. */
    t_uint8 chip_type;                  /**< 0 for Normal / 1 for emulator chip. */
    t_uint8 efuse_status;               /**< OK / FAIL. */
    t_uint8 public_ID[PUBLIC_ID_SIZE];
    t_uint32 root_key_hash_type;
    t_uint8 root_key_hash_size;
    t_uint8 root_key_hash[MAX_HASH_SIZE];
    t_uint8 chip_id_lsb;
    t_uint8 chip_option[2];
    t_uint8 chip_cust_id[4];
} t_secure_state;



#endif      //SSA_API_H
/** @} */
