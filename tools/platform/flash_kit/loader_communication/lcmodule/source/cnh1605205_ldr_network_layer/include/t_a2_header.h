/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_A2_HEADER_H_
#define T_A2_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_header
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Header pattern for the A2 protocol*/
#define A2_HEADER_PATTERN (0xAA)
/** The length of the header */
#define A2_HEADER_LENGTH 12
/** The length of the header for acknowledge */
#define A2_HEADER_LENGTH_ACK 10

/**
 * Header search results
 */
#define NO_A2_HEADER_PATTERN (0x00)
#define A2_HEADER_PATTERN_MATCH (0x01)

/** Protocol types*/
typedef enum {
    /** Control Message Protocol*/
    PROTO_CTRL_MSG = 0x01,
    /** Application data protocol (Command and General Response)*/
    PROTO_A2 = 0x06,
    /** Application data acknowledge protocol*/
    PROTO_A2_ACK = 0x07,
} A2_Protocol_t;

/** Header for command protocol */
typedef struct {
    uint8 HeaderPattern;
    uint8 Protocol;
    uint8 SourceAddress;
    uint8 DestinationAddress;
    uint8 SequenceNumber;
    uint8 Reserved[3];
    uint32 DataLength;
} A2_Header_t;

/** @} */
/** @} */
/** @} */
#endif /*T_A2_HEADER_H_*/
