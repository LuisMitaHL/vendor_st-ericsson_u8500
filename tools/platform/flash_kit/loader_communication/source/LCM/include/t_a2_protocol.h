/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_T_A2_PROTOCOL_H
#define INCLUSION_GUARD_T_A2_PROTOCOL_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_command_protocol.h" // only for Buffer_t

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

typedef enum {
    A2_COMMAND,          /**< Command type of the packet.*/
    A2_GENERAL_RESPONSE, /**< General response type of the packet.*/
    A2_CONTROL_MESSAGE,  /**< Control message protocol packet. */
    A2_SPEEDFLASH_GR     /**< Speedflash GR packet. */
} A2_CommandType_t;

/**
 * Holds information for the command received from the transport layer.
 */
typedef struct A2_CommandData_s {
    uint8            CommandNr;     /**< Number of the command. */
    uint8            ApplicationNr; /**< Number of the application (command) group. */
    uint16           SessionNr;     /**< Number of the session in which this command
                                       was received. */
    uint8            DestAddress;   /**< Destination address ACC = 0x01, APP = 0x02 */
    A2_CommandType_t Type;          /**< Command type. */
    Buffer_t         Payload;       /**< Holds the data received with the command.*/
} A2_CommandData_t;

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_T_A2_PROTOCOL_H
