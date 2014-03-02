/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
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
#include "sla_types.h"

/* Secure mode environment setting bit masks. Note that if you modify
 * these definitions, you have to change them also in sec_conf.inc */
#define SEC_ROM_IRQ_ENABLE_MASK                     0x0004          /**< IRQ enabling/disabling*/
#define SEC_ROM_FIQ_ENABLE_MASK                     0x0008          /**< FIQ enabling/disabling*/
#define SEC_ROM_FORCE_CLEAN_MASK                    0x0020          /**< Force cache clean during sec mode exit*/
#define SEC_ROM_NO_FLAG_MASK                        0x0000          /**< NO FLAGS: no cache clean during sec mode exit*/

/*
 * Chip type (emulation or production one
 */
typedef enum
{
    FULL_SECURITY_CHIP  = 0,
    UNSECURE_CHIP       = 1
} t_chip_type;


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
    t_uint8 public_ID[SHA1_HASH_SIZE];
    t_uint32 root_key_hash_type;
    t_uint8 root_key_hash_size;
    t_uint8 root_key_hash[MAX_HASH_SIZE];
} t_secure_state;


typedef enum
{
    SEC_ROM_RET_COMPILER_NOT_SUPPORTED              = -1,           /**< Used only by the HCL\n*/
    SEC_ROM_RET_OK                                  = 0x01,         /**< Application called successfully\n*/
    SEC_ROM_RET_NON_SUPPORTED_APPL                  = 0x02,         /**< Secure ROM code or ISSW does not support called application.\n*/
    SEC_ROM_RET_ENTRY_DISABLED                      = 0x03,         /**< Secure mode entry disabled due to non-initialized secure mode. Not used, kept for compatibilty purpose\n*/
    SEC_ROM_RET_RESOURCE_RESERVED                   = 0x04,         /**< Secure mode is already in use and requested application call is not allowed. \nNot used, kept for compatibilty purpose\n*/
    SEC_ROM_RET_RNG_RESOURCE_BUSY                   = 0x05,         /**< RNG block is busy (probably doing initial entropy collection) and not able to generate proper random number\n*/
    SEC_ROM_RET_NON_VALID_ADDRESS                   = 0x06,         /**< Non-valid address has been passed to secure mode application. \nValid addresses are those that have entry in secure mode MMU table and that do not point to secure mode memories or to secure mode I/O ports.\n*/
    SEC_ROM_RET_HASH_FAILURE                        = 0x07,         /**< Failure noticed while doing hash checking\n*/
    SEC_ROM_RET_INIT_ALREADY_DONE                   = 0x08,         /**< Secure mode init called although init has been called already earlier. No actions done in secure mode.\n*/
    SEC_ROM_RET_FAIL                                = 0x09,         /**< General failure. Could be an illegal user parameter (no security threat)\n*/
    SEC_ROM_RET_CERTIFICATE_FAILURE                 = 0x0A,         /**< Certificate failure.\n Not used, kept for compatibilty purpose\n*/
    SEC_ROM_RET_DIVIDED_HASH_AREA_NOT_ALLOWED       = 0x0B,         /**< Extended hash check area defined in certificate although not supported.\n Not used, kept for compatibilty purpose\n*/
    SEC_ROM_RET_NOT_ENOUGH_MEMORY                   = 0x0C,         /**< Not enough memory available for dynamic memory allocation\n*/
    SEC_ROM_RET_ISSW_VERSION_WRONG                  = 0x0D,         /**< ISSW is not meant for this chip.\n This checking is done in ISSW init and usually it checks that secure ROM version matches with the value defined in ISSW.\n*/
    SEC_ROM_RET_ENTRY_FAILURE                       = 0x0E,         /**< Requested application call is not allowed due to secure mode being already active (running another application).\n*/
    SEC_ROM_RET_ISSW_LOAD_FAILURE                   = 0x0F,         /**< Importing ISSW to secure RAM failed. This may be due to following reasons. ISSW is already loaded in secure mode.\n*/
    SEC_ROM_RET_ISSW_HASH_FAILURE                   = 0x10,         /**< Hash calculated over ISSW code and RW data does not match with the hash in ISSW certificate.\n*/
    SEC_ROM_RET_ISSW_CERTIFICATE_FAILURE            = 0x11,         /**< ISSW certificate checking failed. This may be due to following reasons:\n
                                                                            . Magic in ISSW certificate is wrong
                                                                            . ISSW certificate signature is wrong
                                                                            . Signing key type or version information in ISSW certificate is wrong\n*/
    SEC_ROM_RET_ISSW_NOT_ENOUGH_MEMORY              = 0x12,         /**< There is not enough memory for loading ISSW into secure RAM.\n*/
    SEC_ROM_RET_ISSW_DIVIDED_HASH_AREA_NOT_ALLOWED  = 0x13,         /**< ISSW hash check does not support requested extended hash area checking.\n*/
    SEC_ROM_RET_DEEP_INIT_CLOCK_FAILURE             = 0x14,         /**< Incorrect clock configuration for deep sleep initialization\n*/
    SEC_ROM_RET_DEEP_INIT_MEMORY_ADDR               = 0x15,         /**< Incorrect memory area during deep sleep initialization\n*/
    SEC_ROM_RET_DEEP_ENTRY_INIT_NOT_DONE            = 0x16,         /**< Deep sleep initialization not performed before deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_BAD_FLAGS                = 0x17,         /**< Incorrect secure environment settings during deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_CLOCK_FAILURE            = 0x18,         /**< Incorrect clock configuration for deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_BAD_HEAP_STATE           = 0x19,         /**< PAs not unloaded/flushed properly before deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_SECTION_FAILURE          = 0x1A,         /**< Secure memory not saved correclty during deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_SELF_REFRESH_FAILURE     = 0x1B,         /**< Problem with SDRAM while entering in self-refresh mode for deep sleep entry\n*/
    SEC_ROM_RET_DEEP_ENTRY_FAILURE                  = 0x1C,         /**< Deep sleep entry failed due to SDRAM\n*/
    SEC_ROM_RET_DEEP_ENTRY_POWER_FAILURE            = 0x1D          /**< Bad SRC register configuration regarding interrupt power mode for deep sleep entry\n*/
} t_sec_rom_response;


#endif      //SSA_API_H
/** @} */
