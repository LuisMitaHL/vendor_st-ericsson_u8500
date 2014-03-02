/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \internal
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/**  \internal
*    \file issw_types.h
 *  \author ST Ericsson
 *  \brief Types definitions used by both ISSW and secure ROM code.
 */
 
#ifndef ISSW_TYPES_H
#define ISSW_TYPES_H

#include "boot_secure_types.h"
#include "sla_types.h"

#define ISSW_CERTIFICATE_MAGIC		0x49535357
#define SPEEDUP_MAGIC_VALUE			0x16793a22


/* defines for CRKC management */
#define VSK_CERTIFICATE_MAGIC		0x6E6F794C
#define CRK_CERTIFICATE_MAGIC		0x6E656741
#define KEY_HEADER_SIZE             8
#define CRKC_MAX_SIZE_FOR_KEY       (KEY_HEADER_SIZE+256)
#define CRKC_MAX_SIZE_FOR_SIGNATURE 256
#define CRKC_MAX_SIZE_FOR_HASH      64


typedef struct 
{ 
        t_uint32 mask; 
        volatile t_uint32 data[32][2]; 
        t_uint8 poll[4]; 
        t_uint8 spare[4]; 
} t_speedup_desc; /* 268 bytes */

typedef struct
{
	t_uint32 magic;
	union
	{
		volatile t_uint32 data[67];
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



//------ structures for CRKC management

/** \internal Structure for Customer Root Key Certificate header. */
typedef struct {
	t_uint32 magic;  /**< Magic number for CRKC header : 0x6E6F794C */
	t_signature_type CRK_SignatureType;  /**< Type of algorithm used for CRK signature */
	t_hash_type CRK_SignatureHashType;  /**< Type of hash algorithm used for CRK signature */	
	t_uint32  unused[5];  /**< For alignment and possible evolution */		
} t_sec_rom_crkc_header;


/** \internal Structure for Vendor Secondary Key Certificate header. */
typedef struct {
	t_uint32 magic;  /**< Magic number for VSKC header : 0x6E656741 */
	t_uint32 VRK_KeyId;  /**<  Identifier for the Vendor Root Key*/
	t_uint32 VRK_IssuerId;	 /**< Magic number for VSKC header : 0x454E4E41 */	
	t_uint32 SK_Size;  /**< Size in bytes of the Secondary Key */
	t_signature_type SK_SignatureType;	 /**< Type of algorithm used for SK signature */
	t_hash_type SK_SignatureHashType;  /**< Type of hash algorithm used for SK signature*/
	t_uint32 SK_RevocationVersion;  /**< Global version for the VSK certificate */
	t_uint32 SK_KeyId;  /**< Secondary key ID, used for revocation purpose */
	t_uint32  unused[4];  /**< For alignment and possible evolution */		
} t_sec_rom_vskc_header;



/** \internal Structure for the complete CRKC section. */
typedef struct {
    t_sec_rom_crkc_header sec_rom_crkc_header;
    t_sec_rom_vskc_header sec_rom_vskc_header;
	t_uint8               VRK[CRKC_MAX_SIZE_FOR_KEY];
	t_uint8               SK[CRKC_MAX_SIZE_FOR_KEY];	
	t_uint8               SKSignature[CRKC_MAX_SIZE_FOR_SIGNATURE];		
	t_uint8               CRKHash[CRKC_MAX_SIZE_FOR_HASH];		
	t_uint8               CRKHashSignature[CRKC_MAX_SIZE_FOR_SIGNATURE];			
} t_sec_rom_crkc_section;



#endif
/** @} */
