/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_BULK_PROTOCOL_H_
#define _T_BULK_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup bulk_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_r15_network_layer.h"
#include "t_critical_section.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**  Maximum bulk processes used in transport layer. */
#define MAX_BULK_TL_PROCESSES  16

/** Mask for selecting the TYPE bits in type/flags field */
#define MASK_BULK_COMMAND_SELECT    (0x07)

/** Defined bulk error in 32 bits format. */
#define BULK_ERROR 0xffffffff
/** Defined bulk error in 64 bits format. */
#define BULK_ERROR_64 0xffffffffffffffff

/** Defined bulk commands. */
typedef enum {
    CMD_BULK_STATUS   = 0x00,  /**< Status packet type command. */
    CMD_BULK_READ     = 0x01,  /**< Read packet type command. */
    CMD_BULK_DATA     = 0x02,  /**< Data packet type command. */
    CMD_BULK_WRITE    = 0x03   /**< Write packet type command. */
} TL_BulkCmmands_t;

/** Defined bulk session ID status. */
typedef enum {
    BULK_SESSION_INVALID = 0x00,  /**< Received packet with invalid session ID.*/
    BULK_SESSION_NEW     = 0x01,  /**< Received packet with new session ID.*/
    BULK_SESSION_CURRENT = 0x02   /**< Received packet with current session ID.*/
} TL_BulkSessionID_Status_t;

/** States of bulk protocol. */
TYPEDEF_ENUM {
    BULK_IDLE_STATE            = 0,  /**< Idle state. */
    SEND_READ_REQUEST          = 1,  /**< Send read request command to PC. */
    WAIT_CHUNKS                = 2,  /**< Wait to receive all expected chunks. */
    SEND_WRITE_REQUEST         = 3,  /**< Send write request command. */
    WAIT_READ_REQUEST          = 4,  /**< Wait read request from PC. */
    PROCESSING_CHUNKS          = 5,  /**< Calculate CRC and get chunks ready for sending. */
    WAIT_CHUNK_SENT            = 6,  /**< Wait for chunk to be sent. */
    SENDING_CHUNKS             = 7,  /**< Send chunks to PC. */
    WAIT_BULK_ACK              = 8,  /**< Wait bulk acknowledge to PC. */
    WAIT_TX_DONE               = 9,  /**< Wait all chunks to be sent. */
    WRITE_BULK_FINISH          = 10, /**< Bulk acknowledge has been received,
                                          finish the write bulk process. */
    WAIT_WRITE_REQUEST         = 11, /**< Wait bulk request command. */
    CANCEL_BULK                = 12
} ENUM8(TL_BulkProtocolState_t);

/** Defined bulk process states. */
TYPEDEF_ENUM {
    BULK_SESSION_IDLE         = 0x00,  /**< Bulk transfer is closed and ready
                                            for starting. */
    BULK_SESSION_OPEN         = 0x01,  /**< Bulk transfer is opened. */
    BULK_SESSION_PROCESSING   = 0x02,  /**< Processing the bulk transfer. */
    BULK_SESSION_FINISHED     = 0x04,  /**< Bulk transfer is finished. */
} ENUM8(TL_BulkSessionState_t);

/** Bulk session Mode. */
typedef enum {
    BULK_RECEIVE               = 1, /**< Receiving Mode. */
    BULK_SEND                  = 2, /**< Transmitting Mode. */
    BULK_RS                    = 3, /**< Receiving and transmitting Mode. */
} TL_SessionMode_t;

/**
 * Status of received chunks in the current session.
 */
typedef enum {
    /**< Chunks are being received in order in current session. */
    VECTOR_NOT_COMPLETE  = 0,
    /**< All chunks for the current session have been received. */
    VECTOR_COMPLETE      = 1,
    /**< Missing chunk(s) in the current session. */
    VECTOR_MISSING_CHUNK = 2,
} TL_BulkVectorStatus_t;

/**
 *  Bulk Vector Entry parameters
 */
typedef struct {
    PacketMeta_t         *Buffer_p;               /**< Pointer to reserved buffer meta info. */
    uint8                *Payload_p;              /**< Pointer to payload data in reserved
                                                       buffer. */
    uint8                *Hash_p;                 /**< Pointer to calculated payload hash. */
    boolean              RetransmissionRequested; /**< Determines if retransmission for the packet
                                                       was requested earlier to avoid sending requests
                                                       for the same packet multiple times. */
} TL_BulkVectorEntry_t;

/**
 *  This type defines Bulk Vector parameters
 */
typedef struct {
    /** Bulk session status. */
    TL_BulkSessionState_t  Status;
    /** Requested bulk process(Read or Write). */
    TL_SessionMode_t       Mode;
    /** State of bulk protocol state machine. */
    TL_BulkProtocolState_t State;
    /** Current bulk session ID. */
    uint16                 SessionId;
    /** Counter for Bulk Timer Retransmissions. */
    uint16                 BulkRetransmissionNo;
    /** Length of the file transfered with bulk transfer. */
    uint64                 TotalLength;
    /** Length of payload data transfered with bulk transfer. */
    uint32                 Length;
    /** Number of used buffers for bulk transfer. */
    uint32                 Buffers;
    /** requested size of payload. */
    uint32                 ChunkSize;
    /** Offset in the current opened file.*/
    uint64                 Offset;
    /** Length of payload data transfered with bulk transfer. */
    uint32                 TransferedLength;
    /** ID of the chunk that currently being sent. */
    uint32                 SendingChunkId;
    /** Callback function pointer for bulk command handling.*/
    void                   *BulkCommandCallback_p;
    /** Callback function pointer for bulk data command handling.*/
    void                   *BulkDataCallback_p;
    /** Array with information for used buffers. */
    TL_BulkVectorEntry_t   Entries[MAX_BULK_TL_PROCESSES];
} TL_BulkVectorList_t;

/** Structure for current bulk transfer handling. */
typedef struct {
    uint32               TimerKey;             /**< Timer Id for current used timer. */
    TL_BulkVectorList_t  *BulkVector_p;        /**< Current used bulk vector for bulk
                                                    transfer.*/
    CriticalSection_t    BulkTransferCS;       /**< Synchronization object used to avoid
                                                    parallel access in bulk transmitter
                                                    function. */
    BulkExtendedHeader_t *PendingBulkHeader_p; /**< Pending Read Request data. Needed for
                                                    opening new session, received while
                                                    current session is in process of sending. */
} BulkHandle_t;

#ifndef CFG_ENABLE_LOADER_TYPE
/** Defined Callback functions used for bulk transfer in the LCM on PC side. */
typedef void (*BulkCommandReqCallback_t)(void *Object_p, uint16 Session, uint32 ChunkSize, uint64 Offset, uint32 Length, boolean ACK_Read);
typedef void (*BulkDataReqCallback_t)(void *Object_p, uint16 Session, uint32 ChunkSize, uint64 Offset, uint32 Length, uint64 TotalLength, uint32 TransferedLength);
typedef void (*BulkDataEndOfDump_t)(void *Object_p);
typedef void (*BulkBuffersRelease_t)(void *Object_p, TL_BulkVectorList_t *BulkVector_p);
#endif // CFG_ENABLE_LOADER_TYPE


/** @} */
/** @} */
/** @} */
#endif /*_T_BULK_PROTOCOL_H_*/
