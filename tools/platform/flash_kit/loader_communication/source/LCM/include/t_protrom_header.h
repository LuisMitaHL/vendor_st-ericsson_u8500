/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_PROTROM_HEADER_H_
#define T_PROTROM_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_header
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Header pattern for the protrom protocol*/
#define PROTROM_HEADER_PATTERN (0xAA)
/** Source address in the header*/
#define PROTROM_SOURCE_ADDRESS (0xEE)
/** Destination address in the header*/
#define PROTROM_DESTINATION_ADDRESS (0xEE)
/** The length of the header */
#define PROTROM_HEADER_LENGTH 7
/** Value of the reserved field */
#define PROTROM_RESERVED_FIELD  0x00

/**
 * Header search results
 */
#define NO_PROTROM_HEADER_PATTERN (0x00)
#define PROTROM_HEADER_PATTERN_MATCH (0x01)
#define PROTROM_HEADER_PATTERN_CANDIDATE (0x02)

/** Protocol types*/
typedef enum {
    PROTO_PROTROM = 0xFB,  /**< PROTROM protocol identification number. */
} Protrom_Protocol_t;

/** Header for command and bulk protocol */
typedef struct {
    uint8 HeaderPattern;       /**< Header pattern for marking header start.*/
    uint8 Protocol;            /**< Protocol type. */
    uint8 SourceAddress;       /**< Source address. */
    uint8 DestinationAddress;  /**< Destination address. */
    uint8 ReservedField;       /**< Reserved field. */
    uint16 PayloadLength;      /**< Payload length. */
} Protrom_Header_t;

/** @} */
/** @} */
/** @} */
#endif /*T_PROTROM_HEADER_H_*/
