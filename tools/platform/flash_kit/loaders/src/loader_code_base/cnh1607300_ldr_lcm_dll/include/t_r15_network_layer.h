/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_R15_NETWORK_LAYER_H_
#define _T_R15_NETWORK_LAYER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_network_layer
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include "t_r15_header.h"
#include "t_time_utilities.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** PROTROM protocol ID*/
#define PROTROM_PROTOCOL        (0xFB)
/** COMMAND protocol ID*/
#define COMMAND_PROTOCOL        (0xBC)
/** BULK protocol ID*/
#define BULK_PROTOCOL        (0xBD)
/** State: Command packet */
#define COMMAND_PACKAGE        (0x0000)
/** State: Acknowledge packet */
#define ACK_PACKAGE             (0x0001)
/** State: General response */
#define GENERAL_RESPONSE_PACKAGE (0x0002)
/** State: Acknowledge for general response */
#define ACK_GENERAL_RESPONSE_PACKAGE (0x0003)
/**
 *  Maximum numbers of used process in transport layer
 */
#define MAX_SIZE_RETRANSMISSION_LIST  32
/** Amount of resends of the same package (until it aborts) */
#define MAX_RESENDS             (0x03)


#ifndef  CFG_ENABLE_LOADER_TYPE
/** Acknowledge packet time(mS) out value */
#define ACK_TIMEOUT_IN_MS       10000
/** Default time(mS) out for receiving bulk command */
#define BULK_COMMAND_RECEIVING_TIMEOUT  10000
/** Default time(mS) out for receiving bulk data. This value is set for UART on 115200kbps! */
#define BULK_DATA_RECEIVING_TIMEOUT   10000
/** Default time(mS) out for detecting bulk session end */
#define BULK_SESSION_END_TIMEOUT   10000


#else
/** Acknowledge packet time(mS) out value */
#define ACK_TIMEOUT_IN_MS       (1000)
/** Default time(mS) out for receiving bulk command */
#define BULK_COMMAND_RECEIVING_TIMEOUT  (1000)
/** Default time(mS) out for receiving bulk data. This value is set for UART on 115200kbps! */
#define BULK_DATA_RECEIVING_TIMEOUT   (120000)
/** Default time(mS) out for detecting bulk session end */
#define BULK_SESSION_END_TIMEOUT (100)
#endif

/** Number of pre-allocated buffers */
#define COMMAND_BUFFER_COUNT 16
/** Number of pre-allocated buffers */
#define BULK_BUFFER_COUNT 16

/** Size of a buffer used for commands. */
#define COMMAND_BUFFER_SIZE 0x00010000

/** Size of a buffer used for bulk transfer,
 * must be biger than buffer for commands. */
#define BULK_BUFFER_SIZE 0x00100000


/** Mask for clearing the state bits in session/state field */
#define MASK_CLR_STATE    (0xFFFC)
/** Mask for selecting the state bits in session/state field */
#define MASK_SELECT_STATE    (0x0003)
/** Mask for selecting the session number in session/state field */
#define MASK_SELECT_SESSION_NUMBER  (0x3FFF)
/** Mask for selecting session bits */
#define SESSION_MASK 0xFFFC

/**
 * Defines all posible types of buffers that can be created (allocated).
 *
 * It is used to specified the type of the buffer
 * when allocating a new buffer. Also it is used when
 * requesting the number of buffers to specify the buffer type.
 */
typedef enum {
    UNDEFINED_BUFFER = 0, /** Buffer with no specified type.*/
    COMMAND_BUFFER   = 1, /** Command buffer type, used for commands.*/
    BULK_BUFFER      = 2 /** Bulk buffer type, used for bulk transfer.*/
} BuffersType_t;

/** The enum for buffer Flags values. */
TYPEDEF_ENUM {
    BUF_FREE            = 0x00000000,        /**< No buffer activity */
    BUF_ALLOCATED       = 0x00000001,        /**< The buffer is allocated. */
    BUF_TX_READY        = 0x00000002,        /**< The buffer is filled with data
                                                and is ready to be sent. */
    BUF_TX_SENT         = 0x00000004,        /**< The buffer is sent and wait ACK. */
    BUF_TX_SENDING      = 0x00000008,        /**< The buffer is sending. */
    BUF_TX_DONE         = 0x00000010,        /**< The buffer has been sent and can
                                                be deallocated. */
    BUF_TX_TIMEOUT      = 0x00000020,        /**< The timeout ocure when buffer is
                                                sending. */
    BUF_RX_READY        = 0x00000040,        /**< The buffer is filled with
                                                received data and is ready for
                                                processing. */
    BUF_HDR_CRC_OK      = 0x00000080,        /**< Set to true if the CRC has been
                                                calculated. */
    BUF_PAYLOAD_CRC_OK  = 0x00000100,        /**< Set to true if the CRC has been
                                                calculated and is correct. */
    BUF_ACK_READY       = 0x00000200,        /**< Set the buffer ready for
                                                acknowledge. */
    BUF_ACKNOWLEDGED           = 0x00000400, /**< Set if the buffer is
                                                acknowledged. */
    BUF_CRC_CALC_READY         = 0x00000800, /**< Set if the buffer is ready for
                                                payload CRC calculating */
    BUF_CRC_CALCULATING        = 0x00001000, /**< Set if the calculating of
                                                payload CRC is started */
    BUF_PAYLOAD_CRC_CALCULATED = 0x00002000, /**< Set if the payload CRC is
                                                calculated */
    BUF_HEADER_CRC_CALCULATED  = 0x00004000, /**< Set if the header CRC is
                                                calculated */
} ENUM32(PacketFlags_t);

/** Defined state of the receiver */
typedef enum {
    RECEIVE_HEADER,          /**< State for receiving Header.*/
    RECEIVE_EXTENDED_HEADER, /**< State for receiving Extended Header.*/
    RECEIVE_PAYLOAD,         /**< State for receiving Payload.*/
    RECEIVE_ERROR            /**< State for error handling.*/
} R15_InboundState_t;

/** Defined state of the transmitter */
typedef enum {
    SEND_IDLE,       /**< Transmiter idle state.*/
    SEND_HEADER,     /**< Transmiter send header and extended header. */
    SENDING_HEADER,  /**< Transmiter is in process sending the header and extended header. */
    SEND_PAYLOAD,    /**< Transmiter send payload. */
    SENDING_PAYLOAD  /**< Transmiter is in process sending payload.*/
} R15_OutboundState_t;

/**
 * Command buffer used for sending and receiving commands.
 */
typedef struct {
    uint32 Id;                           /**< The buffer ID. NOTE: Currently is
                                            not used */
    uint32 Canary0;                      /**< A canary value. NOTE: Currently is
                                            not used */
    uint8  Payload[COMMAND_BUFFER_SIZE]; /**< The buffer containing the raw packet
                                            data. NOTE! The buffer includes all
                                            the data that is sent
                                            (header, data and CRC) */
    uint32 Canary1;                      /**< A canary value. NOTE: Currently is
                                            not used */
} CommandBuffer_t;

/** Structure for the bulk buffer type. */
typedef struct {
    uint32 Id;                          /**< The buffer ID. NOTE: Currently is
                                           not used */
    uint32 Canary0;                     /**< A canary value. NOTE: Currently is
                                           not used */
    uint8  Payload[BULK_BUFFER_SIZE];     /**< The buffer containing the raw packet
                                           data. NOTE! The buffer includes all
                                           the data that is sent
                                           (header, data and CRC) */
    uint32 Canary1;                     /**< A Canary value. NOTE: Currently is
                                           not used */
} BulkBuffer_t;

typedef void (*PacketCallBack_t)(Communication_t *Communication_p, const void *Data_p);

/** Structure for the packet meta data. */
typedef struct PacketMeta {
    PacketCallBack_t CallBack_p;                  /**< Cllback function used after
                                                   sending packet.*/
    uint32           Flags;                       /**< Field is a bit-field. Flags
                                                   for the Packet state. */
    int              BufferSize;                  /**< Field contains a Size of the
                                                   corresponding buffer used in
                                                   the packet. */
    R15_Header_t     Header;                      /**< Structure of the header. */
    uint32           Resend;                      /**< Resend counter. */
    Timer_t          *Timer_p;                    /**< Timer data used for
                                                   sending/reciving packet. */
    uint8            *ExtendedHeader_p;           /**< Pointer to the extended
                                                   header located in the packet. */
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
    uint8            Hash[SHA_256_HASH_LENGTH];   /**< Field contain calculated hash
                                                   for payload. */
} PacketMeta_t;

/** Retransmission context. */
typedef struct {
    uint32       Timeout;   /**< Defined timeout for retransmission. */
    uint32       TimerKey;  /**< Timer identification number.*/
    uint32       Key;       /**< Generated unique key, used for marking packet for
                               retransmission or removing from
                              retransmission list. */
    PacketMeta_t *Packet_p; /**< Pointer to the packet for retransmission. */
} RetransmissionContext_t;


/** Structure for handling incoming R15 packets.*/
typedef struct {
    /**< State of the state machine for handling incoming R15 packets. */
    R15_InboundState_t State;
    /**< Number of requested data for receiving from communication device. */
    uint32             ReqData;
    /**< Number of receivied data from communication device. */
    uint32             RecData;
    /**< Number of receivied data from backup buffer used for switching the
     * protocol family. */
    uint32             RecBackupData;
    /**< Offset in the buffer for next data that should be received. */
    uint32             ReqBuffOffset;
    /**< Temporary pointer to buffer for handling received data.*/
    uint8              *Target_p;
    /**< Temporary buffer for receiving data. */
    uint8              Scratch[HEADER_LENGTH+BULK_EXTENDED_HEADER_LENGTH];
    /** Temporary structure for handling R15 packet.*/
    R15_Header_t       Header;
    /** Poiter to meta data for allocated buffer for handling R15 packet.*/
    PacketMeta_t       *Packet_p;
    /** Error flag, purposed for error handling.*/
    ErrorCode_e         LCM_Error;
} R15_Inbound_t;


/** Structure for handling outgoing R15 packets.*/
typedef struct {
    /**< State of the state machine for handling outgoing R15 packets. */
    R15_OutboundState_t State;
    /** Temporary pointer for handling PROTROM packet.*/
    PacketMeta_t        *Packet_p;
    /**< Boolean value for controling re-entry in transmiter fucntion. */
    boolean             InLoad;
    /** Error flag, purposed for error handling.*/
    ErrorCode_e         LCM_Error;
} R15_Outbound_t;


/** R15 Network context */
typedef struct {
    PacketMeta_t              *MetaInfoList[COMMAND_BUFFER_COUNT+BULK_BUFFER_COUNT];
    /**< List of meta data for used packets. */
    RetransmissionContext_t *RetransmissionList[MAX_SIZE_RETRANSMISSION_LIST];
    /**< List of packet for retransmision */
    uint32                   RetransmissionListCount;
    /**< Counter for packets retransmision */
    R15_Inbound_t            Inbound;
    /**< Context for receiver. */
    R15_Outbound_t           Outbound;
    /**< Context for transmiter. */
} R15_NetworkContext_t;

/*
 * XVSZOAN @TODO:
 *
 * This structure is only for UART Process File fix.
 * When we start using ESB block for data verification
 * structure should be deleted!
 */
typedef struct {
    const uint8 *Data_Packet_p;
    uint32       Packet_Size;
    uint32       UART_Device;
    boolean      New_Packed_Arrived;
} DataBox_t;

/** @} */
/** @} */
/** @} */
#endif /*_T_R15_NETWORK_LAYER_H_*/
