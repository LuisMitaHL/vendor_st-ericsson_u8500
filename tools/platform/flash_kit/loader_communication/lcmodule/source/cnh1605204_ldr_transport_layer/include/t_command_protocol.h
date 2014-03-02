/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_COMMAND_PROTOCOL_H_
#define _T_COMMAND_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup command_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 * The command type. COMMAND and GENERAL_RESPONSE can be used to
 * indicate what type of commands to send. The _ACK command types are
 * used internally to provide end-to-end reliability.
 */
typedef enum {
    COMMAND_TYPE,         /**< Command type of the packet.*/
    COMMAND_ACK,          /**< Acknowledge type of the packet.*/
    GENERAL_RESPONSE,     /**< General response type of the packet.*/
    GENERAL_RESPONSE_ACK  /**< General response acknowledge type of the packet.*/
} CommandType_t;


/**
 * Used for storing input and output Payload data.
 */
typedef struct Buffer_s {
    uint32       Size;     /**< Size of the buffer in bytes.*/
    uint8        *Data_p;  /**< Data buffer. */
} Buffer_t;

/**
 * Used for storing the status of the general response send by the
 * command and for storing the data that should be send with the
 * general response.
 */
typedef struct Result_s {
    ErrorCode_e     Status;       /**< Status send by the general response. */
    Buffer_t       *Response_p;   /**< Buffer holding data that should be
                                       returned trough the general response */
} Result_t;

/**
 * Holds information for the command received from the transport layer.
 */
typedef struct CommandData_s {
    uint8           CommandNr;    /**< Number of the command. */
    uint8           ApplicationNr;/**< Number of the application (command) group. */
    uint16          SessionNr;    /**< Number of the session in which this command
                                       was received.*/
    CommandType_t   Type;         /**< Command type. */
    Buffer_t        Payload;      /**< Holds the data received with the command.*/
} CommandData_t;

/** @} */
/** @} */
/** @} */
#endif /*_T_COMMAND_PROTOCOL_H_*/
