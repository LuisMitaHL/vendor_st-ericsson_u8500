/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_Z_NETWORK_H_
#define T_Z_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup ldr_z_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include "t_critical_section.h"
#include "t_z_header.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Defined state of the receiver */
typedef enum {
    Z_RECEIVE_HEADER,  /**< State for receiving Header.*/
    Z_RECEIVE_PAYLOAD, /**< State for receiving Payload.*/
    Z_RECEIVE_ERROR,   /**< State for error handling.*/
    Z_RECEIVE_IDLE     /**< Receiver idle state.*/
} Z_InboundState_t;

/** Defined state of the transmitter */
typedef enum {
    Z_SEND_IDLE,      /**< Transmitter idle state.*/
    Z_SEND_PACKET,    /**< Transmitter send packet. */
    Z_SENDING_PACKET  /**< Transmitter is in process sending packet.*/
} Z_OutboundState_t;

/** Structure for handling incoming Z packets.*/
typedef struct {
    /**< State of the state machine for handling incoming Z packets. */
    Z_InboundState_t  State;
    /**< Number of requested data for receiving from communication device. */
    uint32            ReqData;
    /**< Number of received data from communication device. */
    uint32            RecData;
    /**< Temporary pointer to buffer for handling received data.*/
    uint8             *Target_p;
    /** Number of packets before receiver is stopped. */
    uint8             PacketsBeforeReceiverStop;
    /** Indicator for stopping the receiver. */
    boolean           StopTransfer;
    /**< Temporary buffer for receiving data. */
    uint8             Scratch[Z_HEADER_LENGTH];
} Z_Inbound_t;

/** Structure for handling outgoing Z packets.*/
typedef struct {
    /**< State of the state machine for handling outgoing Z packets. */
    Z_OutboundState_t State;
    /**< Synchronization object to avoid parallel access in transmitter function. */
    CriticalSection_t TxCriticalSection;
} Z_Outbound_t;


/** Z Network context */
typedef struct {
    /**< Structure for handling incoming Z packets.*/
    Z_Inbound_t         Inbound;
    /**< Structure for handling outgoing Z packets.*/
    Z_Outbound_t        Outbound;
} Z_NetworkContext_t;

/** @} */
/** @} */
/** @} */
#endif /*T_Z_LAYER_H_*/
