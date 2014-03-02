/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_A2_NETWORK_H_
#define T_A2_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include "t_critical_section.h"
#include "t_a2_header.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** A2 protocol ID */
#define A2_PROTOCOL        (0xAA)
/** A2 CRC length */
#define A2_CRC_LENGTH      (CRC16_LENGTH)

/** Number of pre-allocated buffers */
#define A2_COMMAND_BUFFER_COUNT 16
/** Size of a buffer used for commands. */
#define A2_COMMAND_BUFFER_SIZE 0x00010011

/** Amount of resends of the same package (until it aborts) */
#define MAX_RESENDS             (0x03)

TYPEDEF_ENUM {
    //A2_BUF_FREE            = 0x00000000,        /**< No buffer activity */
    A2_BUF_ALLOCATED       = 0x00000001,        /**< The buffer is allocated. */
    //A2_BUF_TX_READY        = 0x00000002,        /**< The buffer is filled with data
    //                                                 and is ready to be sent. */
    //A2_BUF_TX_SENT         = 0x00000004,        /**< The buffer is sent and wait ACK. */
    //A2_BUF_TX_SENDING      = 0x00000008,        /**< The buffer is sending. */
    //A2_BUF_TX_DONE         = 0x00000010,        /**< The buffer has been sent and can
    //                                                 be deallocated. */
    //A2_BUF_TX_TIMEOUT      = 0x00000020,        /**< The timeout occur when buffer is
    //                                                 sending. */
    A2_BUF_RX_READY        = 0x00000040,        /**< The buffer is filled with
                                                   received data and is ready for
                                                   processing. */
    A2_BUF_HDR_CRC_OK      = 0x00000080,        /**< Set to true if the CRC has been
                                                   calculated. */
    A2_BUF_PAYLOAD_CRC_OK  = 0x00000100,        /**< Set to true if the CRC has been
                                                   calculated and is correct. */
    //A2_BUF_ACK_READY       = 0x00000200,        /**< Set the buffer ready for
    //                                                 acknowledge. */
    //A2_BUF_ACKNOWLEDGED           = 0x00000400, /**< Set if the buffer is
    //                                                 acknowledged. */
    //A2_BUF_CRC_CALC_READY         = 0x00000800, /**< Set if the buffer is ready for
    //                                                 payload CRC calculating */
    //A2_BUF_CRC_CALCULATING        = 0x00001000, /**< Set if the calculating of
    //                                                 payload CRC is started */
    A2_BUF_PAYLOAD_CRC_CALCULATED = 0x00002000, /**< Set if the payload CRC is
                                                   calculated */
    A2_BUF_HEADER_CRC_CALCULATED  = 0x00004000, /**< Set if the header CRC is
                                                   calculated */
} ENUM32(A2_PacketFlags_t);

/** defined state of the receiver */
typedef enum {
    A2_RECEIVE_HEADER,         /**< State for receiving Header.*/
    A2_RECEIVE_PAYLOAD,        /**< State for receiving Payload.*/
    A2_RECEIVE_ERROR,          /**< State for error handling.*/
    A2_RECEIVE_IDLE            /**< Receiver idle state.*/
} A2_InboundState_t;

/** defined state of the transmitter */
typedef enum {
    A2_SEND_IDLE,      /**< Transmitter idle state.*/
    A2_SEND_HEADER,    /**< Transmitter send header. */
    A2_SENDING_HEADER, /**< Transmitter is in process of sending header. */
    A2_SEND_PAYLOAD,   /**< Transmitter send payload. */
    A2_SENDING_PAYLOAD /**< Transmitter is in process of sending payload. */
} A2_OutboundState_t;

typedef void (*A2_PacketCallBack_t)(Communication_t *Communication_p, const void *Data_p);

/** Structure for the packet meta data. */
typedef struct A2_PacketMeta {
    A2_PacketCallBack_t CallBack_p;               /**< Callback function used after
                                                   sending packet.*/
    uint32           Flags;                       /**< Field is a bit-field. Flags
                                                   for the Packet state. */
    int              BufferSize;                  /**< Field contains a Size of the
                                                   corresponding buffer used in
                                                   the packet. */
    A2_Header_t      Header;                      /**< Structure of the header. */
    uint32           Resend;                      /**< Re-send counter. */
    Timer_t          *Timer_p;                    /**< Timer data used for
                                                   sending/receiving packet. */
    uint8            *Payload_p;                  /**< Pointer to the payload data
                                                   located in the packet. */
    uint8            *Buffer_p;                   /**< Field contains a pointer to
                                                   the corresponding (type)
                                                   buffer. */
    uint32           Canary;                      /**< Field contains the master
                                                   Canary value. NOTE: Currently
                                                   is not used */
    Communication_t  *Communication_p;            /**< The communication over which
                                                   this packet has been/is to be
                                                   transferred over. */
    uint8            CRC[A2_CRC_LENGTH];          /**< Field contain calculated CRC
                                                   for payload. */
} A2_PacketMeta_t;

/** Structure for handling incoming A2 packets.*/
typedef struct {
    /**< State of the state machine for handling incoming A2 packets. */
    A2_InboundState_t State;
    /**< Number of requested data for receiving from communication device. */
    uint32            ReqData;
    /**< Number of received data from communication device. */
    uint32            RecData;
    /**< Number of received data from backup buffer used for switching the
     * protocol family. */
    uint32            RecBackupData;
    /**< Offset in the buffer for next data that should be received. */
    uint32            ReqBuffOffset;
    /**< Temporary pointer to buffer for handling received data.*/
    uint8             *Target_p;
    /**< Temporary buffer for receiving data. */
    uint8             Scratch[A2_HEADER_LENGTH];
    /** Temporary structure for handling A2 packet.*/
    A2_Header_t       Header;
    /** Pointer to meta data for allocated buffer for handling A2 packet.*/
    A2_PacketMeta_t   *Packet_p;
    /** Number of packets before receiver is stopped. */
    uint8             PacketsBeforeReceiverStop;
    /** Indicator for stopping the receiver. */
    boolean           StopTransfer;
    /** Error flag, purposed for error handling.*/
    ErrorCode_e       LCM_Error;
} A2_Inbound_t;

/** A2_Outbound_t*/
/** Structure for handling outgoing A2 packets.*/
typedef struct {
    /** State of the state machine for handling outgoing A2 packets. */
    A2_OutboundState_t State;
    /** Temporary pointer for handling PROTROM packet.*/
    A2_PacketMeta_t    *Packet_p;
    /** Synchronization object to avoid parallel access in transmitter function. */
    CriticalSection_t  TxCriticalSection;
    /** Error flag, purposed for error handling.*/
    ErrorCode_e        LCM_Error;
} A2_Outbound_t;

/** A2 Retransmission context. */
typedef struct {
    uint32       Timeout;   /**< Defined timeout for retransmission. */
    uint32       TimerKey;  /**< Timer identification number.*/
} A2_RetransmissionContext_t;

/** A2 Network context */
typedef struct {
    A2_PacketMeta_t            *MetaInfoList[A2_COMMAND_BUFFER_COUNT];
    /**< List of meta data for used packets. */
    A2_Inbound_t                Inbound;
    /**< @todo Write Documentation*/
    A2_Outbound_t               Outbound;
    /**< @todo Write Documentation*/
    A2_RetransmissionContext_t  RetransmissionContext;
    /**< Retransmission context for the packet that is currently sent. */
} A2_NetworkContext_t;

/** @} */
/** @} */
/** @} */
#endif /*T_NETWORK_LAYER_H_*/
