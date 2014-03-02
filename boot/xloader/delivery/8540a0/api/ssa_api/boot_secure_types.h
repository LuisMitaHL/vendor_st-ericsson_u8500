/*-----------------------------------------------------------------------------
 * STMicroelectronics
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *---------------------------------------------------------------------------*/
/**
 *
 * \file boot_secure_types.h
 * \brief ROM codes' common types definition
 * \author STMicroelectronics
 *
 * This file covers all types common to secure and boot ROM codes.
 * 
 * @{
 */
/*---------------------------------------------------------------------------*/

#ifndef _BOOT_SECURE_TYPES_H
#define _BOOT_SECURE_TYPES_H

#include "hcl_defs.h"

/*-----------------------------------------------------------------------------
 * Constants definition				                                   
 *---------------------------------------------------------------------------*/

/* Secure mode environment setting bit masks. Note that if you modify
 * these definitions, you have to change them also in sec_conf.inc */
#define SEC_ROM_IRQ_ENABLE_MASK                     0x0004          /**< IRQ enabling/disabling*/
#define SEC_ROM_FIQ_ENABLE_MASK                     0x0008          /**< FIQ enabling/disabling*/
#define SEC_ROM_FORCE_CLEAN_MASK                    0x0020          /**< Force cache clean during sec mode exit*/
#define SEC_ROM_NO_FLAG_MASK                        0x0000          /**< NO FLAGS: no cache clean during sec mode exit*/

/* Public ID size in bytes */
#define PUBLIC_ID_SIZE    20

/*-----------------------------------------------------------------------------
 * Bit setting or clearing
 *---------------------------------------------------------------------------*/
#define SET_BITS(reg,mask)			((reg) |=  (mask))
#define CLEAR_BITS(reg,mask)		((reg) &= ~(mask))
#define READ_BITS(reg,mask)			((reg) &   (mask))
#define WRITE_BITS(reg,val,mask)	((reg) =   (((reg) & ~(mask)) | ((val) & (mask))))
#define READ_REG(reg)				(reg)
#define WRITE_REG(reg,val)			((reg) = (val))


/*-----------------------------------------------------------------------------
 * Byte management
 *---------------------------------------------------------------------------*/
#define WRITE_BYTE(word, byte, id)	(word = (word & ~(MASK_BYTE<<(id*BIT_TO_BYTE))) | ((t_uint32)byte<<(id*BIT_TO_BYTE)))


/*-----------------------------------------------------------------------------
 * Types conversion
 * BYTE: 8 bits, WORD: 16 bits, DWORD: 32 bits, LONGWORD: 64 bits
 *---------------------------------------------------------------------------*/
#define DWORD_TO_LONGWORD	2
#define DWORD_TO_BYTE		4
#define BYTE_TO_DWORD		4
#define BYTE_TO_BIT			8
#define BIT_TO_BYTE			8
#define LONGWORD_TO_BYTE    8
#define WORD_TO_BIT			16
#define BIT_TO_WORD			16
#define DWORD_TO_BIT		32
#define BIT_TO_DWORD		32


/*-----------------------------------------------------------------------------
 * Address alignment
 * To align address on 32-bits boundaries
 *---------------------------------------------------------------------------*/ 
#define ADDR_ALIGN( addr ) ((addr >> 2) << 2)
#define ADDR_ALIGN_16B( addr ) (((addr) >> 4) << 4)


/*-----------------------------------------------------------------------------
 * Tests defitions
 *---------------------------------------------------------------------------*/
#define IS_NULL_POINTER(ptr) ( (ptr == NULL) ? TRUE : FALSE )

#define YES				1
#define NO				0

#define OK				1
#define FAIL			0
  
  
  
/*-----------------------------------------------------------------------------
 * Types definition				                                   
 *---------------------------------------------------------------------------*/
typedef unsigned long long  t_bitfield64;

typedef enum
{
	SHA1_HASH_SIZE    = 20,
	SHA256_HASH_SIZE  = 32,
	SHA384_HASH_SIZE  = 48,
	SHA512_HASH_SIZE  = 64,
	MAX_HASH_SIZE     = 64,
	DUMMY_HASH_SIZE_32BIT = 0x7FFFFFFF
} t_hash_size;

/**
 * \internal 
 * Channel identificator
 * Physical channel number
 */
typedef enum
{
    CHANNEL_0 = 0,
    CHANNEL_1 = 1,
    CHANNEL_2 = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
    CHANNEL_ERROR = 8
} t_dma_phys_chl_id;

#if 0
// Operations for SLA_CACHE_MAINTENANCE
typedef enum
{
    DCACHE_CLEAN            = 0x1,
    DCACHE_AREA_CLEAN       = 0x2,
    DCACHE_INVALIDATE       = 0x3,
    DCACHE_AREA_INVALIDATE  = 0x4,
    ICACHE_INVALIDATE       = 0x5,
    ICACHE_AREA_INVALIDATE  = 0x6,
    WRITE_BUFFER_DRAIN      = 0x7,
    DCACHE_CLEAN_INV        = 0x8,
    DCACHE_AREA_CLEAN_INV   = 0x9,
    L2CACHE_INVALIDATE      = 0xA,
    L2CACHE_AREA_INVALIDATE = 0xB,
    L2CACHE_CLEAN           = 0xC,
    L2CACHE_AREA_CLEAN      = 0xD,
    L2CACHE_CLEAN_INV       = 0xE,
    L2CACHE_AREA_CLEAN_INV  = 0xF
} t_cache_operation_id;
#endif

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
    SEC_ROM_RET_DEEP_ENTRY_POWER_FAILURE            = 0x1D,         /**< Bad SRC register configuration regarding interrupt power mode for deep sleep entry\n*/
	DUMMY_SEC_ROM_RET_32BIT                         = 0x7FFFFFFF
#if 0
} t_sec_rom_response;
#else
} t_sec_rom_response_8540;
#endif

#endif		// _BOOT_SECURE_TYPES_H
/** @} */
