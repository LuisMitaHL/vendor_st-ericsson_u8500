/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_R15_HEADER_H_
#define _T_R15_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_header
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#define ALIGN_SIZE 512

/** Length of the command packet header.*/
#define TL_COMMAND_PACKET_HEADER_LENGTH  20
/** Length of the bulk packet header.*/
#define TL_BULK_PACKET_HEADER_LENGTH  36
/** Length of the calculated sha256 hash. */
#define SHA_256_HASH_LENGTH 32
/** Header pattern for the loader protocol.*/
#define HEADER_PATTERN (0xAA)
/** Extended header pattern of the loader protocol.*/
#define HEADER_PATTERN_EXTENSION (0xEE)
/** The length of the header. */
#define HEADER_LENGTH 16
/** The length of the command extended header. */
#define COMMAND_EXTENDED_HEADER_LENGTH 4
/** The length of the bulk extended header. */
#define BULK_EXTENDED_HEADER_LENGTH 20

#define ALIGNED_HEADER_LENGTH                   ALIGN_SIZE
#define ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH  ALIGN_SIZE
#define ALIGNED_BULK_EXTENDED_HEADER_LENGTH     ALIGN_SIZE

/**
 * Header search results
 */
#define NO_HEADER_PATTERN (0x00)
#define HEADER_PATTERN_MATCH (0x01)
#define HEADER_PATTERN_CANDIDATE (0x02)

/** Protocol types*/
typedef enum {
    PROTO_COMMAND = 0xBC, /**< R15 command protocol identification number. */
    PROTO_BULK = 0xBD     /**< R15 bulk protocol identification number. */
} R15_Protocol_t;

/** Header for command and bulk protocol */
typedef struct {
    uint8 HeaderPattern;           /**< Header pattern for marking header start.*/
    uint8 Protocol;                /**< Protocol type. */
    uint16 HeaderPatternExtension; /**< Header pattern extension.*/
    uint8 Flags;                   /**< Flags for the header.*/
    uint8 ExtendedHeaderLength;    /**< Extended header length.*/
    uint8 ExtendedHeaderChecksum;  /**< Extended header checksum.*/
    uint32 PayloadLength;          /**< Payload length. */
    uint32 PayloadChecksum;        /**< Payload checksum. */
    uint8 HeaderChecksum;          /**< Header checksum. */
} R15_Header_t;


/**
 * Extended header for command protocol
 */
typedef struct {
    uint16    SessionState;  /**< Session and state for command header */
    uint8     Command;       /**< Specified command */
    uint8     CommandGroup;  /**< Specified command group */
} CommandExtendedHeader_t;

/**
 * Extended header for bulk protocol
 */
typedef struct {
    uint16 Session;    /**< Session for bulk header */
    uint8  TypeFlags;  /**< Flags for data packet */
    uint8  AcksChunk;  /**< Number of acknowledged chunks or chunk identifier */
    uint32 ChunkSize;  /**< The maximum payload size of data packet */
    uint64 Offset;     /**< Offset from where the reading and writing of data should begin*/
    uint32 Length;     /**< How many bytes should be read/write */
} BulkExtendedHeader_t;

/** @} */
/** @} */
/** @} */
#endif /*_T_R15_HEADER_H_*/
