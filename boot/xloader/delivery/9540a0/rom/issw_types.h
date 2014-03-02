/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * \file issw_types.h
 * \author STMicroelectronics
 *
 * Types definitions used by both ISSW and secure ROM code
 *
 *---------------------------------------------------------------------------*/
/** \internal
 *  \addtogroup ISSW
 * @{
 */
 
#ifndef ISSW_TYPES_H
#define ISSW_TYPES_H

#include "boot_secure_types.h"
#include "sla_types.h"

#define ISSW_CERTIFICATE_MAGIC		0x49535357
#define SPEEDUP_MAGIC_VALUE			0x16793a22

typedef struct 
{ 
        t_uint32 mask; 
        t_uint32 data[32][2]; 
        t_uint8 poll[4]; 
        t_uint8 spare[4]; 
} t_speedup_desc; /* 268 bytes */

typedef struct
{
	t_uint32 magic;
	union
	{
		t_uint32 data[67];
		t_speedup_desc commands;
	} data;
} t_sec_rom_issw_speedup;


typedef struct
{
	t_uint32 code;
	t_uint32 rw_data;
	t_uint32 zi_data;
} t_sec_rom_iss_sect_lengths;

typedef struct
{
	t_uint32 magic;      // 0x49535357

	t_uint32 size_of_root_key;
	t_uint32 size_of_customer_part;
	t_uint32 size_of_unchecked_blob;
	t_uint32 size_of_signature;

	t_sec_rom_issw_speedup speedup;

	t_uint32 issw_hash_type;   // hash algo for issw code & data (XXX_HASH)
	t_uint8 issw_hash[MAX_HASH_SIZE];
	t_sec_rom_iss_sect_lengths issw_sect_len;
	t_uint32 issw_load_location; // absolute addr of ISSW in sec RAM
	t_uint32 issw_start_addr;    // absolute addr of issw_init function

	t_uint32 root_key_type;    // type of public key algo (XXX_SIGNATURE)
	t_uint32 root_key_hash_type; // hash algo for root key hash in OTP (XXX_HASH)
	t_uint32 signature_hash_type;  // hash algo used to generate ISSW certificate signature (XXX_HASH)
} t_sec_rom_issw_header;


/** \internal Structure for shared params between boot ROM code and ISSW or Loader application. */
typedef struct{
void* rpcHandlerAddr;   /**< Logical address of the boot ROM RPC handler*/
void* rpcParams;        /**< Physical address of the structure that contains RPC index function parameters */
void* tocAddrPhy;       /**< Physical TOC address */
void* tocAddrVirt;      /**< virtual TOC address.*/
void* pLoaderTable;     /**< Pysical pointer on the structure that contains 
                             information needed to call boot ROM API functions. */
} t_pub_rom_issw_info;


// Parameter structure for ISSW_INIT.
typedef struct {
    void* (*find_service_address)(t_uint32);
    t_pub_rom_issw_info issw_info;
    t_uint32  customer_part_hash_type;
    t_uint8    customer_part_digest[MAX_HASH_SIZE];
    void*   customer_part_ptr;
    t_uint32  customer_part_size;
    void*   unchecked_blob_ptr;
    t_uint32  unchecked_blob_size;
} t_issw_init_params;

#endif
/** @} */
