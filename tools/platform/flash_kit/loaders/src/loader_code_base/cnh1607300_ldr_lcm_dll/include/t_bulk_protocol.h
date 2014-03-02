/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
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

/** Defined Callback functions used for bulk transfer in the LCM on PC side. */
typedef void (*BulkCommandReqCallback_t)(void *Object_p, uint16 *Session_p, uint32 *ChunkSize_p, uint64 *Offset_p, uint32 *Length_p, boolean ACK_Read);
typedef void(*BulkDataReqCallback_t)(void *Object_p, uint16 *Session_p, uint32 *ChunkSize_p, uint64 *Offset_p, uint32 *Length_p, uint64 *TotalLength_p, uint32 *TransferedLength_p);
typedef void(*BulkDataEndOfDump_t)(void *Object_p);


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
    BULK_IDLE_STATE            = 0, /**< Idle state. */
    SEND_READ_REQUEST          = 1, /**< Send read request command to PC. */
    WAIT_CHUNKS                = 2, /**< Wait to receive all expected chunks. */
    SEND_BULK_ACK              = 3, /**< Send bulk acknowledge to PC. */
    WAIT_BULK_ACK_TIMEOUT      = 4, /**< Wait timeout for confirmation of bulk
                                       ack command. */
    SEND_WRITE_REQUEST         = 5, /**< Send write request command. */
    WAIT_READ_REQUEST          = 6, /**< Wait read request from PC. */
    SENDING_CHUNKS             = 7, /**< Send chunks to PC. */
    WAIT_BULK_ACK              = 8, /**< Wait bulk acknowledge to PC. */
    WRITE_BULK_FINISH          = 9, /**< Bulk acknowledge has been received,
                                       finish the write bulk process. */
    WAIT_WRITE_REQUEST         = 10 /**< Wait bulk request command. */
} ENUM8(TL_BulkProtocolState_t);

/** Defined bulk process states. */
TYPEDEF_ENUM {
    BULK_SESSION_IDLE         = 0x00,  /**< Bulk transfer is closed and ready
                                          for starting. */
    BULK_SESSION_OPEN         = 0x01,  /**< Bulk transfer is opened. */
    BULK_SESSION_PROCESSING   = 0x02,  /**< Processing the bulk transfer. */
    BULK_SESSION_FINISHED     = 0x04,  /**< Bulk transfer is finished. */
} ENUM8(TL_BulkSessionState_t);

/** State of the bulk buffers. */
typedef enum {
    BULK_BUFFER_EMPTY    = 0x00,  /**< The bulk buffer is empty.  */
    BULK_BUFFER_PAYLOAD  = 0x01,  /**< The bulk buffer contains payload data
                                     (received data or data for sending. */
    BULK_BUFFER_SENT     = 0x02,  /**< The bulk buffer is transmitted. */
} TL_BulkBuffersFlags_t;

/** Bulk session Mode. */
typedef enum {
    BULK_RECEIVE               = 1, /**< Receiving Mode. */
    BULK_SEND                  = 2, /**< Transmitting Mode. */
    BULK_RS                    = 3, /**< Receiving and transmitting Mode. */
} TL_SessionMode_t;

/*
 *  Bulk Vector Entry parameters
 */
typedef struct {

    TL_BulkBuffersFlags_t Flags;     /**< Flags of reserved buffer. */
    uint32                Resend;    /**< Counter of retransmitted chunk. */
    void                 *Buffer_p;  /**< Pointer to reserved buffer meta info. */
    uint8                *Payload_p; /**< Pointer to payload data in reserved
                                        buffer. */
    uint8                *Hash_p;    /**< Pointer to calculated payload hash. */
} TL_BulkVectorEntry_t;

/*
 *  This type defines Bulk Vector parameters
 */
typedef struct {
    /**< Bulk session status. */
    TL_BulkSessionState_t  Status;
    /**< Requested bulk process(Read or Write). */
    TL_SessionMode_t       Mode;
    /**< State of bulk protocol state machine. */
    TL_BulkProtocolState_t State;
    /**< Current bulk session ID. */
    uint16                 SessionId;
    /**< Length of the file transfered with bulk transfer. */
    uint64                 TotalLength;
    /**< Length of payload data transfered with bulk transfer. */
    uint32                 Length;
    /**< Number of used buffers for bulk transfer. */
    uint32                 Buffers;
    /**< requested size of payload. */
    uint32                 ChunkSize;
    /**< Offset in the cuurent opened file.*/
    uint64                 Offset;
    /**< Length of payload data transfered with bulk transfer. */
    uint32                 TransferedLength;
    /**< Callback function pointer for bulk command handling.*/
    void                  *BulkCommandCallback_p;
    /**< Callback function pointer for bulk data command handling.*/
    void                  *BulkDataCallback_p;
    /**< Array with information for used buffers. */
    TL_BulkVectorEntry_t   Entries[MAX_BULK_TL_PROCESSES];
} TL_BulkVectorList_t;

/** Structure for current bulk transfer handling. */
typedef struct {
    uint32                TimerKey;   /**< Timer Id for current used timer. */
    //  uint32                BulkVector; /**< Current used bulk vector for bulk
    //                                         transfer.*/
    TL_BulkVectorList_t   *BulkVector_p; /**< Current used bulk vector for bulk
                                         transfer.*/
    //  TL_BulkSessionState_t Status;     /**< Status of the current bulk session. */
} BulkHandle_t;

/** @} */
/** @} */
/** @} */
#endif /*_T_BULK_PROTOCOL_H_*/
