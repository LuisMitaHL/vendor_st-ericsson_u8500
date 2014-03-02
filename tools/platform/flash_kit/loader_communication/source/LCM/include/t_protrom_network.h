/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_PROTROM_NETWORK_H_
#define T_PROTROM_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include "t_critical_section.h"
#include "t_protrom_header.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** PROTROM protocol ID */
#define PROTROM_PROTOCOL        (0xFB)
/** PROTROM CRC length */
#define PROTROM_CRC_LENGTH      (2)

/** Defined state of the receiver */
typedef enum {
    PROTROM_RECEIVE_HEADER,   /**< State for receiving Header.*/
    PROTROM_RECEIVE_PAYLOAD,  /**< State for receiving Payload.*/
    PROTROM_RECEIVE_ERROR,    /**< State for error handling.*/
    PROTROM_RECEIVE_IDLE      /**< State for receiver idle.*/
} Protrom_InboundState_t;

/** Defined state of the transmitter */
typedef enum {
    PROTROM_SEND_IDLE,       /**< Transmitter idle state.*/
    PROTROM_SEND_HEADER,     /**< Transmitter send header. */
    PROTROM_SEND_PAYLOAD,    /**< Transmitter send payload. */
    PROTROM_SENDING_HEADER,  /**< Transmitter is in process sending header.*/
    PROTROM_SENDING_PAYLOAD  /**< Transmitter is in process sending payload.*/
} Protrom_OutboundState_t;

/** Structure for the packet meta data type. */
typedef struct Protrom_Packet {
    Protrom_Header_t  Header;           /**< PROTROM header structure. */
    uint8             *Buffer_p;        /**< Temporary buffer for receiving data.*/
    uint16            CRC;              /**< Calculated CRC of received packet. */
    Communication_t   *Communication_p; /**< The communication over which this
                                             packet has been/is to be transferred
                                             over */
} Protrom_Packet_t;

/** Structure for handling incoming PROTROM packets.*/
typedef struct {
    /**< State of the state machine for handling incoming PROTROM packets. */
    Protrom_InboundState_t State;
    /**< Number of requested data for receiving from communication device. */
    uint32                 ReqData;
    /**< Number of received data from communication device. */
    uint32                 RecData;
    /**< Number of received data from backup buffer used for switching the
     * protocol family. */
    uint32                 RecBackupData;
    /**< Offset in the buffer for next data that should be received. */
    uint32                 ReqBuffOffset;
    /**< Temporary pointer to buffer for handling received data.*/
    uint8                  *Target_p;
    /**< Temporary buffer for receiving data. */
    uint8                  Scratch[PROTROM_HEADER_LENGTH];
    /** Number of packets before receiver is stopped. */
    uint8                  PacketsBeforeReceiverStop;
    /** Indicator for stopping the receiver. */
    boolean                StopTransfer;
    /** Temporary structure for handling PROTROM packet.*/
    Protrom_Packet_t       *Packet_p;
} Protrom_Inbound_t;

/** Structure for handling outgoing PROTROM packets.*/
typedef struct {
    /**< State of the state machine for handling outgoing PROTROM packets. */
    Protrom_OutboundState_t State;
    /** Temporary pointer for handling PROTROM packet.*/
    Protrom_Packet_t        *Packet_p;
    /**< Synchronization object to avoid parallel access in transmitter function. */
    CriticalSection_t       TxCriticalSection;
} Protrom_Outbound_t;


/** PROTROM Network context */
typedef struct {
    /**< Structure for handling incoming PROTROM packets.*/
    Protrom_Inbound_t  Inbound;
    /**< Structure for handling outgoing PROTROM packets.*/
    Protrom_Outbound_t Outbound;
} Protrom_NetworkContext_t;

/** @} */
/** @} */
/** @} */
#endif /*T_NETWORK_LAYER_H_*/
