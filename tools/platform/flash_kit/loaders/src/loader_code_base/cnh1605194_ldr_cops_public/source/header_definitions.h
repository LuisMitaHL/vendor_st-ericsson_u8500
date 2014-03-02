/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSIONGUARD_HEADER_DEFINITIONS_H
#define _INCLUSIONGUARD_HEADER_DEFINITIONS_H
/**
 * @addtogroup ldr_security_library
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_copsrom.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#define HEADER_EXTENSION_BLOCK_SIZE_OFFSET       0x20
#define HEADER_EXTENSION_NUMBER_OF_BLOCKS_OFFSET 0x24
#define HEADER_EXTENSION_PAYLOAD_SIZE_OFFSET     0x50
#define HEADER_EXTENSION_PAYLOAD_TYPE_OFFSET     0x58
#define HEADER_EXTENSION_HEADER_LENGTH_OFFSET    0x64

#define SIGNED_SOFTWARE_HEADER_LENGTH            0x366
#define HASH_LIST_BLOCK_OFFSET_LENGTH            0x8
#define HASH_LIST_BLOCK_LENGTH_LENGTH            0x8
#define CONTROL_HEADER_SIZE                      sizeof(COPSROM_Control_t)

/**
 * struct COPSROM_ASN1_t
 * brief Type for carrying information about an ASN.1 item
 */
typedef struct {
    uint32                tag;  /**< Struct member Tag of ASN1 item*/
    uint32                len;  /**< Struct member Length of ASN1 item including
                                   tag and len description*/
    COPSROM_DataBuffer_t  ASN1; /**< Struct member Data buffer with the value of
                                   the ASN1 item*/
} COPSROM_ASN1_t;

/**
 *
 */
typedef struct {
    COPSROM_DataBuffer_t  OID;   /* object identifier identifying the extension */
    COPSROM_DataBuffer_t  value; /**< */
} COPSROM_X509_extension_t;

/** @} */
#endif  //_INCLUSIONGUARD_HEADER_DEFINITIONS_H
