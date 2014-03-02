/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
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
#include "c_system_v2.h"
#include "r_basicdefinitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "r_bulk_protocol.h"
#include "r_r15_transport_layer.h"
#include "r_r15_network_layer.h"
#include "r_r15_family.h"
#include "r_r15_header.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_communication_service.h"
#include "t_security_algorithms.h"
#include "r_memory_utils.h"
#include "r_critical_section.h"

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_measurement_tool.h"
#include "r_time_utilities.h"
#endif

/***********************************************************************
 * Definition of external constants and variables
 **********************************************************************/
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
extern  Measurement_t *Measurement_p;
#endif

static void R15_Bulk_ReadChunkCallBack(Communication_t *Communication_p, const void *const Timer_p, const void *const Data_p);
static void R15_Bulk_RetransmitChunks_CallBack(const Communication_t *const Communication_p, const void *const Timer_p, const void *const Data_p);
static ErrorCode_e R15_Bulk_Process_Read(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, PacketMeta_t *Packet_p);
static ErrorCode_e R15_Bulk_SendReadRequest(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, uint32 Chunks, void *ChunksList_p, void *CallBack_p);
static ErrorCode_e R15_Bulk_SendWriteRequest(Communication_t *Communication_p);
static void R15_Bulk_SerializeChunk(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint32 ChunkId);
static void R15_Bulk_SendData(Communication_t *Communication_p, PacketMeta_t *Packet_p);
__inline static boolean R15_Bulk_CheckTransmitedChunks(const TL_BulkVectorList_t *BulkVector_p);
__inline static boolean R15_Bulk_CheckIdInList(const TL_BulkVectorList_t *BulkVector_p, const uint32 ChunkId, const uint8 *const Data_p, const uint32 Length);
__inline static uint32 R15_Bulk_GetChunkForSending(const TL_BulkVectorList_t *BulkVector_p);
__inline static uint32 R15_Bulk_GetChunkForSeriazliation(const TL_BulkVectorList_t *BulkVector_p);
static void R15_Bulk_GetListOfReceivedChunks(const TL_BulkVectorList_t *const BulkVector_p, uint32 *Chunks_p, uint8 *ChunkList_p);
static uint32 R15_Bulk_GetChunkId(const PacketMeta_t *const Packet_p);
static uint32 R15_Bulk_GetTimerChunkRetransmision(const Communication_t *const Communication_p, uint32 Time, HandleFunction_t CallBack_p);
static void R15_Bulk_MarkNotAckChunks(TL_BulkVectorList_t *BulkVector_p, const uint8 *const Payload_p, const uint32 Length);
static void R15_Bulk_MarkNotAckAllChunks(TL_BulkVectorList_t *BulkVector_p);
static void R15_Bulk_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);
static TL_BulkVectorStatus_t R15_Bulk_GetVectorStatus(TL_BulkVectorList_t *BulkVector_p);
static boolean R15_Bulk_VectorNeedsRetransmission(TL_BulkVectorList_t *BulkVector_p, uint32 CurrentChunkId);
__inline static void R15_Bulk_VectorClearRetransmissionRequested(TL_BulkVectorList_t *BulkVector_p);
static TL_BulkSessionID_Status_t R15_Bulk_CheckBulkSession(Communication_t *Communication_p, uint16 SessionId);
static ErrorCode_e R15_Bulk_DataRequestHandler(Communication_t *Communication_p, PacketMeta_t *Packet_p);
static ErrorCode_e R15_Bulk_ReadRequestHandler(Communication_t *Communication_p, PacketMeta_t *Packet_p);
__inline static boolean R15_Bulk_AllChunksProcessed(const TL_BulkVectorList_t *const BulkVector_p);

#ifdef  CFG_ENABLE_LOADER_TYPE
static boolean R15_Bulk_CheckAcknowledgedChunks(const TL_BulkVectorList_t *BulkVector_p, const uint8 *const Payload_p);
#else
static boolean R15_Bulk_SessionTxDone(const TL_BulkVectorList_t *const BulkVector_p);
__inline static boolean IsChunkReceived(Communication_t *Communication_p, uint32 ChunkId);
#endif // CFG_ENABLE_LOADER_TYPE

/***********************************************************************
 * Definition of external functions
 **********************************************************************/
#ifndef  CFG_ENABLE_LOADER_TYPE
void Do_R15_Bulk_SetCallbacks(Communication_t *Communication_p, void *BulkCommandCallback_p, void *BulkDataCallback_p, void *EndOfDump_p)
{
    R15_TRANSPORT(Communication_p)->BulkCommandCallback_p = BulkCommandCallback_p;
    R15_TRANSPORT(Communication_p)->BulkDataCallback_p = BulkDataCallback_p;
    R15_TRANSPORT(Communication_p)->EndOfDump_p = EndOfDump_p;
}

void Do_R15_Bulk_SetBuffersRelease(Communication_t *Communication_p, void *BulkBufferRelease_p)
{
    R15_TRANSPORT(Communication_p)->BulkBufferRelease_p = BulkBufferRelease_p;
}
#endif

/*
 * Opens Bulk session.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] SessionId       Session ID.
 * @param [in] Mode            Type of operation(Read or Write).
 * (send or receive) over the bulk protocol.
 *
 * @return Vector ID  Vector ID of the newly opened session.
 * @retval BULK_ERROR If the session can not be opened.
 */
uint32 Do_R15_Bulk_OpenSession(const Communication_t *const Communication_p, const uint16 SessionId, const TL_SessionMode_t Mode)
{
    uint32 VectorCounter;

    if (NULL == Communication_p) {
        return BULK_ERROR;
    }

    for (VectorCounter = 1; (VectorCounter < MAX_BULK_TL_PROCESSES) && (R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Status != BULK_SESSION_IDLE); VectorCounter++);

    if (MAX_BULK_TL_PROCESSES != VectorCounter) {
        if (R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Status != BULK_SESSION_IDLE) {
            A_(printf("bulk_protocol.c (%d): bulk error \n", __LINE__);)
            VectorCounter = BULK_ERROR;
        } else {
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Status = BULK_SESSION_OPEN;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].SessionId = SessionId;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Mode = Mode;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Length = 0;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].TransferedLength = 0;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Offset = 0;
            R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].ChunkSize = 0;
            R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
        }
    } else {
        A_(printf("bulk_protocol.c (%d): bulk error \n", __LINE__);)
        VectorCounter = BULK_ERROR;
    }

    B_(printf("bulk_protocol.c (%d): Opened Bulk session(%d)with VId(%d)\n", __LINE__, SessionId, VectorCounter);)
    return VectorCounter;
}

/*
 * Creates Bulk Vector.
 *
 * @param [in] Communication_p     Communication module context.
 * @param [in] BulkVector          Index of Bulk Vector.
 * @param [in] Length              Data Length in bytes.
 * @param [in] BuffSize            Requested size of each Buffer.
 * @param [in] CreatedBulkVector_p Already created bulk vector which
 *                                 need to be updated with correct
 *                                 information.
 *
 * @return Vector ID  The ID of the Vector that has been created.
 * @retval BULK_ERROR If creation of vector failed.
 */
TL_BulkVectorList_t *Do_R15_Bulk_CreateVector(const Communication_t *const Communication_p, const uint32 BulkVector, uint32 Length, const uint32 BuffSize, TL_BulkVectorList_t *CreatedBulkVector_p)
{
    uint32 Counter;
    uint32 AvailableBuffers = 0;
    uint32 Buffers;
    PacketMeta_t *Packet_p = NULL;
    TL_BulkVectorList_t *BulkVector_p = NULL;

    if (NULL == Communication_p) {
        return NULL;
    }

#ifndef  CFG_ENABLE_LOADER_TYPE
    int  BuffersNr = 0;
#endif

    /* check input parameters */
    if ((0 == BulkVector) || (MAX_BULK_TL_PROCESSES <= BulkVector) || (BuffSize == 0) || (BuffSize > (BULK_BUFFER_SIZE - ALIGNED_HEADER_LENGTH - ALIGNED_BULK_EXTENDED_HEADER_LENGTH)) || (Length == 0)) {
        A_(printf("bulk_protocol.c (%d): ** Invalid input parameters! **\n", __LINE__);)
        return NULL;
    }

    BulkVector_p = &(R15_TRANSPORT(Communication_p)->BulkVectorList[BulkVector]);

    if (NULL == CreatedBulkVector_p) {
        Buffers = ((Length + BuffSize - 1) / BuffSize);

        if (0 == Buffers) {
            A_(printf("bulk_protocol.c (%d):Buffers = NULL\n", __LINE__);)
            return NULL;
        }

        AvailableBuffers = BUFFER(Communication_p, BuffersAvailable_Fn)(OBJECT_BUFFER(Communication_p), BULK_BUFFER_SIZE);

        if (AvailableBuffers >= Buffers) {
            BulkVector_p->Length = Length;
            BulkVector_p->Buffers = Buffers;
            BulkVector_p->ChunkSize = BuffSize;
            BulkVector_p->Offset = 0;

            for (Counter = 0; (Counter < Buffers); Counter++) {
                BulkVector_p->Entries[Counter].Buffer_p = NULL;
                BulkVector_p->Entries[Counter].RetransmissionRequested = FALSE;

                if (BULK_SEND == BulkVector_p->Mode) {
#ifdef  CFG_ENABLE_LOADER_TYPE
                    Packet_p = R15_Network_PacketAllocate(Communication_p, BULK_BUFFER_SIZE);

                    if (NULL == Packet_p) {
                        A_(printf("bulk_protocol.c (%d): ** Memory allocation failed! **\n", __LINE__);)
                        return NULL;
                    }

                    Packet_p->ExtendedHeader_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;
                    Packet_p->Payload_p = Packet_p->ExtendedHeader_p + ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
#else
                    /* packet meta info allocate */
                    Packet_p = (PacketMeta_t *)malloc(sizeof(PacketMeta_t));

                    if (NULL == Packet_p) {
                        return NULL;
                    }

                    memset(Packet_p, 0, sizeof(PacketMeta_t));

                    /* packet meta info setup */
                    Packet_p->Buffer_p = NULL;
                    Packet_p->BufferSize = BULK_BUFFER_SIZE;
                    SET_PACKET_FLAGS(Packet_p, PACKET_ALLOCATION_STATE_MASK, BUF_ALLOCATED);
                    Packet_p->Payload_p = (uint8 *)(ALIGNED_HEADER_LENGTH + ALIGNED_BULK_EXTENDED_HEADER_LENGTH);
                    Packet_p->ExtendedHeader_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;
                    BuffersNr = 0;

                    do {
                        if (NULL == R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
                            R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = Packet_p;
                            break;
                        }

                        BuffersNr ++;
                    } while (BuffersNr < (COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT));

                    if (COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT <= BuffersNr) {
                        BUFFER_FREE(Packet_p);
                        return NULL;
                    }

#endif  // #ifdef  CFG_ENABLE_LOADER_TYPE

                    BulkVector_p->Entries[Counter].Buffer_p = Packet_p;
                    BulkVector_p->Entries[Counter].Payload_p = Packet_p->Payload_p;
                    BulkVector_p->Entries[Counter].Hash_p = Packet_p->Hash;
                }
            }
        } else {
            A_(printf("bulk_protocol.c (%d) ** Not enough buffers! ** \n", __LINE__);)
            return NULL;
        }
    } else {
        memcpy(BulkVector_p, CreatedBulkVector_p, sizeof(TL_BulkVectorList_t));
    }

    return BulkVector_p;
}

/*
 * Frees Bulk Vector.
 *
 * @param [in] Communication_p  Communication module context.
 * @param [in] BulkVector_p     Pointer to the Bulk Vector.
 * @param [in] ReqReleaseBuffer Defines if complete buffers with meta data will
 *                              be released or only only the meta data will be cleared.
 *
 * @retval  E_SUCCESS   After successful execution.
 * @retval  BULK_ERROR  Error while destroying vector.
 */
uint32 Do_R15_Bulk_DestroyVector(const Communication_t *const Communication_p, TL_BulkVectorList_t *BulkVector_p, boolean ReqReleaseBuffer)
{
    uint32 ReturnValue = E_SUCCESS;
    uint32 Counter;
    int BuffersNr = 0;

    if (NULL == Communication_p) {
        return BULK_ERROR;
    }

    if (NULL == BulkVector_p) {
        A_(printf("bulk_protocol.c (%d): ** Invalid bulk vector! **\n", __LINE__);)
        return BULK_ERROR;
    }

    for (Counter = 0; (Counter < BulkVector_p->Buffers); Counter++) {
        /* clear all flags by setting buffer as free */
        BulkVector_p->Entries[Counter].Buffer_p->Flags = BUF_FREE;
        BulkVector_p->Entries[Counter].Hash_p = NULL;
        BulkVector_p->Entries[Counter].RetransmissionRequested = FALSE;

        if (NULL != BulkVector_p->Entries[Counter].Buffer_p) {
            if (ReqReleaseBuffer) {
                ReturnValue = R15_Network_PacketRelease(Communication_p, BulkVector_p->Entries[Counter].Buffer_p);
                BulkVector_p->Entries[Counter].Buffer_p = NULL;
            } else {
                C_(printf("bulk_protocol.c (%d): Destroyed meta info data (0x%x)for bulk buffers! \n", __LINE__, BulkVector_p->Entries[Counter].Buffer_p);)
                BuffersNr = 0;

                do {
                    if (BulkVector_p->Entries[Counter].Buffer_p == R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
                        R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = NULL;
                        /* release meta info for buffer */
                        free(BulkVector_p->Entries[Counter].Buffer_p);
                        BulkVector_p->Entries[Counter].Buffer_p = NULL;
                        break;
                    }

                    BuffersNr ++;
                } while (BuffersNr < (COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT));
            }
        }
    }

    C_(printf("bulk_protocol.c (%d): Destroyed bulk vector for session(%d)! \n", __LINE__, BulkVector_p->SessionId);)
    BulkVector_p->Status = BULK_SESSION_IDLE;
    BulkVector_p->SessionId = 0;
    BulkVector_p->Mode = BULK_RECEIVE;
    BulkVector_p->Length = 0;
    BulkVector_p->Buffers = 0;
    BulkVector_p->ChunkSize = 0;
    BulkVector_p->Offset = 0;
    BulkVector_p->State = BULK_IDLE_STATE;

    return ReturnValue;
}

/*
 * Start specified bulk session.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] BulkVector_p    Pointer to the Bulk Vector.
 * @param [in] Offset          Sets the offset from where data
 *                             read/write should be performed.
 *
 * @retval  E_INVALID_BULK_MODE            Invalid bulk mode.
 * @retval  E_FAILED_TO_START_BULK_SESSION Either the session is
 *                                         invalid or the BulkVector is invalid.
 * @retval  E_SUCCESS                      After successful execution.
 */
ErrorCode_e Do_R15_Bulk_StartSession(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, const uint64 Offset)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint32 ChunkId;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    /* Verify the new bulk vector. */
    VERIFY(NULL != BulkVector_p, E_INVALID_INPUT_PARAMETERS);
    /* Verify that the bulk session is opened for this bulk vector. */
    VERIFY(BULK_SESSION_IDLE != BulkVector_p->Status, E_FAILED_TO_START_BULK_SESSION);
    /* Verify that the previous bulk session is closed. */
    VERIFY(NULL == R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p, E_PREVIOUS_BULK_SESSION_IS_NOT_CLOSED);

    BulkVector_p->Status = BULK_SESSION_PROCESSING;
    BulkVector_p->Offset = Offset;
    BulkVector_p->BulkRetransmissionNo = 0;

    /* set the handle for the current bulk vector */
    R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p = BulkVector_p;
    R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;

    C_(printf("bulk_protocol.c(%d) Session(%d) Started!\n", __LINE__, BulkVector_p->SessionId);)
    C_(printf("bulk_protocol.c(%d) Req: Chunks(%d) ChunkSize(%d) Length(%d)\n", __LINE__, BulkVector_p->Buffers, BulkVector_p->ChunkSize, BulkVector_p->Length);)

    if (BULK_RECEIVE == BulkVector_p->Mode) {
        /* Set BULK READ state machine */
#ifdef  CFG_ENABLE_LOADER_TYPE
        BulkVector_p->State = SEND_READ_REQUEST;
#else
        BulkVector_p->State = WAIT_WRITE_REQUEST;
#endif
        ReturnValue = R15_Bulk_Process_Read(Communication_p, BulkVector_p, NULL);
    } else {
        VERIFY(BULK_SEND == BulkVector_p->Mode, E_INVALID_BULK_MODE);

        for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
        }

        /* Set BULK WRITE state machine */
#ifdef  CFG_ENABLE_LOADER_TYPE
        BulkVector_p->State = SEND_WRITE_REQUEST;
#else
        BulkVector_p->State = WAIT_READ_REQUEST;
#endif
        ReturnValue = R15_Bulk_Process_Write(Communication_p, BulkVector_p);
    }

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = Do_Measurement_CreateSessionMeasurementData(Communication_p, &(Measurement_p->MeasurementData_p->SessionMeasurement_p),
                  NULL, BulkVector_p->Buffers, BulkVector_p->SessionId);
    ReturnValue = MP(Measurement_p, 0, START_SESSION_TIME);
#endif
ErrorExit:
    return ReturnValue;
}

/*
 *  Get status of bulk transfer.
 *
 * @param [in] BulkVector_p Pointer to the Bulk Vector.
 *
 * @retval BULK_SESSION_IDLE       Idle state of bulk protocol.
 * @retval BULK_SESSION_OPEN       Opened bulk protocol and ready for
 *                                 transmitting.
 * @retval BULK_SESSION_PROCESSING Processing bulk transfer.
 * @retval BULK_SESSION_FINISHED   Bulk transfer finished.
 */
TL_BulkSessionState_t Do_R15_Bulk_GetStatusSession(const TL_BulkVectorList_t *BulkVector_p)
{
    if (NULL != BulkVector_p) {
        return  BulkVector_p->Status;
    }

    return  BULK_SESSION_IDLE;
}

/*
 * Close specified Bulk session.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] BulkVector_p    Pointer to the Bulk Vector.
 *                             for the session you want to close.
 *
 * @retval  E_SUCCESS                      After successful execution.
 * @retval  E_FAILED_TO_CLOSE_BULK_SESSION If the BulkVector is not valid.
 */
//lint -e{818}
ErrorCode_e Do_R15_Bulk_CloseSession(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    /* Verify that the requested bulk session is started. */
    VERIFY(NULL != BulkVector_p, E_FAILED_TO_CLOSE_BULK_SESSION);

    /* Reset Bulk retransmission counter */
    BulkVector_p->BulkRetransmissionNo = 0;

    /* Try to release the timer for the bulk read request */
    if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
        (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey);
        R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
    }

    R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p = NULL;

    C_(printf("bulk_protocol.c (%d): Bulk session(%d) closed! \n", __LINE__, BulkVector_p->SessionId);)
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = MP(Measurement_p, 0, END_SESSION_TIME);
#endif

ErrorExit:
    return ReturnValue;
}

/*
 * Handling received bulk command.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If Packet_p is NULL.
 */
ErrorCode_e R15_Bulk_Process(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    BulkExtendedHeader_t ExtendedHeader = {0};
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;
#ifdef  CFG_ENABLE_LOADER_TYPE
    TL_BulkVectorList_t *PreviousBulkVector_p = &(R15_TRANSPORT(Communication_p)->PreviousBulkVector);
    uint32 ChunksCount = 0;
    uint8 ChunksList[MAX_BULK_TL_PROCESSES];
#endif

    /* check input parameters */
    VERIFY(NULL != Packet_p, E_INVALID_INPUT_PARAMETERS);

    //lint --e(826)
    R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);
    VERIFY(ExtendedHeader.Session > 0, E_INVALID_BULK_SESSION_ID);

    /* check bulk session opened? */
#ifdef  CFG_ENABLE_LOADER_TYPE

    if ((NULL == BulkVector_p) || (ExtendedHeader.Session != BulkVector_p->SessionId)) {
        A_(printf("bulk_protocol.c (%d): Session is not opened(%d) or wrong session(%d)! \n", __LINE__, R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p, ExtendedHeader.Session);)

        if ((CMD_BULK_DATA == (ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT)) &&
                (NULL != PreviousBulkVector_p) &&
                (BulkVector_p->SessionId <= ExtendedHeader.Session)) {

            A_(printf("bulk_protocol.c (%d): New session started \n", __LINE__);)
            // make the list of received chunks
            R15_Bulk_GetListOfReceivedChunks(PreviousBulkVector_p, &ChunksCount, ChunksList);
            // send read ACK for previous session
            ReturnValue = R15_Bulk_SendReadRequest(Communication_p, PreviousBulkVector_p, ChunksCount, ChunksList, NULL);
        }

        // release the buffer used for the invalid packet
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        goto ErrorExit;
    }

#endif

    switch (ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) {
    case CMD_BULK_DATA:
        ReturnValue = R15_Bulk_DataRequestHandler(Communication_p, Packet_p);
        break;

    case CMD_BULK_READ:
        ReturnValue = R15_Bulk_ReadRequestHandler(Communication_p, Packet_p);
        break;

#ifndef  CFG_ENABLE_LOADER_TYPE

    case CMD_BULK_WRITE:

        if (NULL != R15_TRANSPORT(Communication_p)->BulkCommandCallback_p) {
            if ((NULL != BulkVector_p) && (ExtendedHeader.Session <= BulkVector_p->SessionId)) {
                if (WAIT_CHUNKS == BulkVector_p->State) {
                    /* Read Request already sent to the loader, but probably it was not received correctly,
                       or the timeout in the loader for the Write Request Packet is too short. */
                    uint32 ChunkId = 0;
                    uint8 ChunksList[MAX_BULK_TL_PROCESSES] = {0};

                    A_(printf("bulk_protocol.c (%d): Write Request packet Received. Session %d already started! Read Request ignored in Loader!\n", __LINE__, ExtendedHeader.Session);)

                    /* Try to release the timer for the bulk read request */
                    if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
                        (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey);
                        R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
                    }

                    R15_Bulk_GetListOfReceivedChunks(BulkVector_p, &ChunkId, ChunksList);
                    R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = R15_Bulk_GetTimerChunkRetransmision(Communication_p, R15_TIMEOUTS(Communication_p)->TBDR, (HandleFunction_t)R15_Bulk_ReadChunkCallBack);

                    if (0 != ChunkId) {
                        ReturnValue = R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, ChunkId, ChunksList, NULL);
                    } else {
                        ReturnValue = R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, 0, NULL, (void *)R15_Bulk_ReadChunkCallBack);
                    }
                } else if (SEND_READ_REQUEST == BulkVector_p->State) {
                    /* Write Request received from the loader and the LCM is in process of sending of Read Request packet.
                       Probably the timeout in the loader for the Write Request Packet is too short. Packet will be ignored! */
                    A_(printf("bulk_protocol.c (%d): Write Request packet will be Ignored. Session %d already started! ReadRequest in process of sending!\n", __LINE__, ExtendedHeader.Session);)
                } else {
                    /* Write Request received from the loader and it is still being processed by LCM.
                       Probably the timeout in the loader for the Write Request Packet is too short. Packet will be ignored! */
                    A_(printf("bulk_protocol.c (%d): Write Request packet will be Ignored. Session %d already started! Problem Undefined!\n", __LINE__, ExtendedHeader.Session);)
                }
            } else {
                BulkCommandReqCallback_t pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                pcbf(Communication_p->Object_p, ExtendedHeader.Session, ExtendedHeader.ChunkSize, ExtendedHeader.Offset, ExtendedHeader.Length, FALSE);
            }
        }

        /* release the buffer for Bulk Write command */
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        break;
#endif

    case CMD_BULK_STATUS:
        // not implemented.
        A_(printf("bulk_protocol.c (%d): ** Not implemented bulk command! **\n", __LINE__);)
        /* release the buffer for Bulk Status command */
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        break;

    default:
        A_(printf("bulk_protocol.c (%d): ** Undefined bulk command! **\n", __LINE__);)
        /* Undefined Bulk command!!! Release the buffer.  */
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        break;
    }

ErrorExit:
    return ReturnValue;
}

#ifdef  CFG_ENABLE_LOADER_TYPE
/*
 * Generate bulk Session ID. Allowed bulk session ID is from 1 to 65535.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  Session ID Next free session ID.
 */
uint16 Do_R15_Bulk_GenerateBulkSessionID(Communication_t *Communication_p)
{
    if (NULL == Communication_p) {
        return 0;
    }

    R15_TRANSPORT(Communication_p)->BulkSessionCounter++;

    if (R15_TRANSPORT(Communication_p)->BulkSessionCounter == 0) {
        R15_TRANSPORT(Communication_p)->BulkSessionCounter = 1;
    }

    return R15_TRANSPORT(Communication_p)->BulkSessionCounter;
}
#endif

/***********************************************************************
 * Definition of internal functions
 **********************************************************************/
/*
 * Get the Chunk ID for the next bulk packet which is ready for transmitting.
 *
 * @param [in] BulkVector_p Current bulk vector used for bulk transfer.
 *
 * @retval ChunkID Chunk ID for the next ready for transmitting packet.
 *                 If none chunk is ready return last Chunk ID + 1.
 */
__inline static uint32 R15_Bulk_GetChunkForSending(const TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkId = 0;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_READY) &&
                CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_PAYLOAD_CRC_CALCULATED)) {
            break;
        }
    }

    return ChunkId;
}

/*
 * Get the Chunk ID for the next bulk packet which is ready for serialization.
 *
 * Packet should be properly allocated, ready for sending and it should not be
 * in process of CRC Calculation or CRC has not been yet calculated.
 *
 * @param [in] BulkVector_p Current bulk vector used for bulk transfer.
 *
 * @retval ChunkID Chunk ID for the next ready for transmitting packet.
 *                 If none chunk is ready return last Chunk ID + 1.
 */
__inline static uint32 R15_Bulk_GetChunkForSeriazliation(const TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkId = 0;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if ((CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, (BUF_ALLOCATED | BUF_TX_READY))) &&
                !(CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_CRC_CALCULATING)) &&
                !(CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_PAYLOAD_CRC_CALCULATED))) {
            break;
        }
    }

    return ChunkId;
}

/*
 * Check all created chunks is it transmitted.
 *
 * @param [in] BulkVector_p Current bulk vector used for bulk transfer.
 * @param [in] ChunkId      ChunkID for the packet which need to be transmitted.
 *
 * @retval  TRUE  If all created chunks are transmitted.
 * @retval  FALSE If all created chunks are not transmitted.
 */
__inline static boolean R15_Bulk_CheckTransmitedChunks(const TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if (!CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_SENT) &&
                !CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_DONE)) {
            return FALSE;
        }
    }

    return TRUE;
}


#ifdef  CFG_ENABLE_LOADER_TYPE
/*
 * Check all acknowledged chunks.
 *
 * @param [in] BulkVector_p Current bulk vector used for bulk transfer.
 * @param [in] Payload_p    List of the acknowledged chunks.
 *
 * @retval  TRUE  If all transmitted chunks are acknowledged.
 * @retval  FALSE If all transmitted chunks are not acknowledged.
 */
static boolean R15_Bulk_CheckAcknowledgedChunks(const TL_BulkVectorList_t *BulkVector_p, const uint8 *const Payload_p)
{
    uint32 i;
    uint8 Data[MAX_BULK_TL_PROCESSES];

    memset(Data, 0, MAX_BULK_TL_PROCESSES);

    for (i = 0; i < BulkVector_p->Buffers; i++) {
        Data[i] = (uint8)i;
    }

    if (memcmp(Data, Payload_p, BulkVector_p->Buffers) == 0) {
        return TRUE;
    }

    return FALSE;
}
#endif

static void R15_Bulk_MarkNotAckChunks(TL_BulkVectorList_t *BulkVector_p, const uint8 *const Payload_p, const uint32 Length)
{
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if (R15_Bulk_CheckIdInList(BulkVector_p, ChunkId, Payload_p, Length)) {
            /* Packet acknowledged. Set TX state DONE */
            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_DONE);
        } else if (!CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_SENDING)) {
            /* Packet needs retransmission. Set it ready for transmitting. */
            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
            BulkVector_p->State = PROCESSING_CHUNKS;
        }
    }
}

static void R15_Bulk_MarkNotAckAllChunks(TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        /* Check if buffer is in process of sending in order to avoid marking it
           for sending again. */
        if (!CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_SENDING)) {
            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
            BulkVector_p->State = PROCESSING_CHUNKS;
        }
    }
}

__inline static boolean R15_Bulk_CheckIdInList(const TL_BulkVectorList_t *BulkVector_p, const uint32 ChunkId, const uint8 *const Data_p, const uint32 Length)
{
    uint32 i;

    for (i = 0; i < Length; i++) {
        if (ChunkId == *(Data_p + i)) {
            return TRUE;
        }
    }

    return FALSE;
}

static ErrorCode_e R15_Bulk_Process_Read(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, PacketMeta_t *Packet_p)
{
    uint32 ChunkId = 0;
    uint8 ChunksList[MAX_BULK_TL_PROCESSES] = {0};
    ErrorCode_e ReturnValue = E_SUCCESS;
    TL_BulkVectorStatus_t ChunkReceivedStatus;
#ifndef CFG_ENABLE_LOADER_TYPE
    BulkCommandReqCallback_t pcbf;
#endif

    if (NULL == BulkVector_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    switch (BulkVector_p->State) {
        /* Idle state */
    case BULK_IDLE_STATE:
        // wait to start the bulk session
        break;

#ifndef  CFG_ENABLE_LOADER_TYPE

        /* coverity[unterminated_case] */
    case WAIT_WRITE_REQUEST:
        BulkVector_p->State = SEND_READ_REQUEST;
#endif  //CFG_ENABLE_LOADER_TYPE

        /* coverity[fallthrough] */
    case SEND_READ_REQUEST:
        //lint --e(611)
        ReturnValue = R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, 0, NULL, (void *)R15_Bulk_ReadChunkCallBack);

        if (E_SUCCESS == ReturnValue) {
            BulkVector_p->State = WAIT_CHUNKS;
        }

        break;

    case WAIT_CHUNKS:
        ChunkId = R15_Bulk_GetChunkId(Packet_p);

        /* Try to release the timer for the bulk read request */
        if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
            (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey);
            R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
        }

        C_(printf("bulk_protocol.c(%d) Received Chunk(%d) Size(%d) Length(%d)\n", __LINE__, ChunkId, ((BulkExtendedHeader_t *)(Packet_p->ExtendedHeader_p))->ChunkSize, ((BulkExtendedHeader_t *)(Packet_p->ExtendedHeader_p))->Length);)

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
        ReturnValue = MP(Measurement_p, ChunkId, RECEIVED_CHUNK_TIME);
#endif

        if (ChunkId >= BulkVector_p->Buffers) {
            /* Packet with invalid chunk Id and release the buffer. */
            ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
            break;
        } else if (NULL == BulkVector_p->Entries[ChunkId].Buffer_p) {
            BulkVector_p->Entries[ChunkId].Buffer_p = Packet_p;

            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_RX_STATE_MASK, BUF_ACK_READY);
            //lint --e(413)
            BulkVector_p->Entries[ChunkId].Payload_p = Packet_p->Payload_p;
            //lint --e(413)
            BulkVector_p->Entries[ChunkId].Hash_p = Packet_p->Hash;

            /*  */
            C_(printf("bulk_protocol.c(%d) Received Chunks until (%d)\n", __LINE__, ChunkId);)
        } else {
            /* Packet with already received chunk Id. Release the network packet. */
            ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        }

        /* make the list of received chunks and send for retransmission of all missed chunks */
        R15_Bulk_GetListOfReceivedChunks(BulkVector_p, &ChunkId, ChunksList);

        /* check if all chunks are received */
        ChunkReceivedStatus = R15_Bulk_GetVectorStatus(BulkVector_p);

        if (VECTOR_COMPLETE == ChunkReceivedStatus) {
            C_(printf("bulk_protocol.c(%d) Last Chunk Received (%d)\n", __LINE__, ChunkId);)
            R15_Bulk_VectorClearRetransmissionRequested(BulkVector_p);
            /* save the current bulk vector before is bulk session closed */
            memcpy(&(R15_TRANSPORT(Communication_p))->PreviousBulkVector, BulkVector_p, sizeof(TL_BulkVectorList_t));
            ReturnValue = R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, ChunkId, ChunksList, NULL);
            BulkVector_p->Status = BULK_SESSION_FINISHED;
            BulkVector_p->State = BULK_IDLE_STATE;
#ifndef CFG_ENABLE_LOADER_TYPE
            // notify session end
            pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
            pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, TRUE);
#endif
        } else if (VECTOR_MISSING_CHUNK == ChunkReceivedStatus) {
            uint32 CurrentChunkId = R15_Bulk_GetChunkId(Packet_p);
            boolean RetransmissionNeeded = R15_Bulk_VectorNeedsRetransmission(BulkVector_p, CurrentChunkId);

            if (RetransmissionNeeded) {
                A_(printf("bulk_protocol.c(%d) RetransmissionNeeded\n", __LINE__);)
                // Send ReadRequest for the missing chunk(s).
                ReturnValue = R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, ChunkId, ChunksList, NULL);
            } else {
                // ReadRequest was previously send for the missing chunk(s).
                R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = R15_Bulk_GetTimerChunkRetransmision(Communication_p, R15_TIMEOUTS(Communication_p)->TBDR, (HandleFunction_t)R15_Bulk_ReadChunkCallBack);
            }
        } else { // Chunks are received in order
            R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = R15_Bulk_GetTimerChunkRetransmision(Communication_p, R15_TIMEOUTS(Communication_p)->TBDR, (HandleFunction_t)R15_Bulk_ReadChunkCallBack);
        }

        break;

    default:
        BulkVector_p->State = BULK_IDLE_STATE;
        ReturnValue = E_INVALID_BULK_PROTOCOL_STATE;
        break;
    }

    return ReturnValue;
}

/*
 * State machine for bulk transfer from ME to PC.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] BulkVector_p    Pointer to the received buffer.
 *
 */
ErrorCode_e R15_Bulk_Process_Write(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint32 ProcessingChunkId;
#ifndef  CFG_ENABLE_LOADER_TYPE
    BulkBuffersRelease_t pcbf;
#endif

    if (!Do_CriticalSection_Enter(R15_TRANSPORT(Communication_p)->BulkHandle.BulkTransferCS)) {
        return ReturnValue;
    }

    if (BulkVector_p->Mode == BULK_SEND) {
        switch (BulkVector_p->State) {
            /* Idle state */
        case BULK_IDLE_STATE:
            // wait to start the bulk session
            break;

        case SEND_WRITE_REQUEST:
            ReturnValue = R15_Bulk_SendWriteRequest(Communication_p);

            if (E_SUCCESS == ReturnValue) {
                BulkVector_p->State = WAIT_READ_REQUEST;
            }

            break;

        case WAIT_BULK_ACK:
            //...?
            break;

        case WAIT_READ_REQUEST:
#ifndef  CFG_ENABLE_LOADER_TYPE
            //TODO find end release timer for retransmission
            BulkVector_p->State = PROCESSING_CHUNKS;
            /* coverity[fallthrough] */
#else
            //...
            break;
#endif

        case PROCESSING_CHUNKS:
            /* Find ChunkId for a package that needs processing. */
            ProcessingChunkId = R15_Bulk_GetChunkForSeriazliation(BulkVector_p);

            if (ProcessingChunkId <= BulkVector_p->Buffers - 1) {
                R15_Bulk_SerializeChunk(Communication_p, BulkVector_p->Entries[ProcessingChunkId].Buffer_p, ProcessingChunkId);
            }

            /* Find ChunkId for a package that is processed and ready for sending. */
            BulkVector_p->SendingChunkId = R15_Bulk_GetChunkForSending(BulkVector_p);

            if (BulkVector_p->SendingChunkId <= BulkVector_p->Buffers - 1) {
                /* Move to state to process remaining chunks while waiting
                   for the current chunk to be sent */
                BulkVector_p->State = WAIT_CHUNK_SENT;
                /* Send packet with chunk ID */
                R15_Bulk_SendData(Communication_p, BulkVector_p->Entries[BulkVector_p->SendingChunkId].Buffer_p);
#ifndef  CFG_ENABLE_LOADER_TYPE

                if (NULL != R15_TRANSPORT(Communication_p)->BulkDataCallback_p) {
                    BulkDataReqCallback_t pcbf = (BulkDataReqCallback_t)R15_TRANSPORT(Communication_p)->BulkDataCallback_p;
                    BulkVector_p->TransferedLength += BulkVector_p->ChunkSize;
                    pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, BulkVector_p->TotalLength, BulkVector_p->TransferedLength);
                }

#endif
                C_(printf("bulk_protocol.c(%d) Sent chunk (%d) session (%d)\n", __LINE__, BulkVector_p->SendingChunkId, BulkVector_p->SessionId);)
            }

            break;

        case WAIT_CHUNK_SENT:

            /* Wait for the chunk to be sent in order to continue sending next chunks */
            if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[BulkVector_p->SendingChunkId].Buffer_p, BUF_TX_SENT)) {
                BulkVector_p->State = PROCESSING_CHUNKS;
            } else {
                /* While sending the chunk prepare other chunks for sending. */
                ProcessingChunkId = R15_Bulk_GetChunkForSeriazliation(BulkVector_p);

                if (ProcessingChunkId <= BulkVector_p->Buffers - 1) {
                    R15_Bulk_SerializeChunk(Communication_p, BulkVector_p->Entries[ProcessingChunkId].Buffer_p, ProcessingChunkId);
                } else if (R15_Bulk_AllChunksProcessed(BulkVector_p)) {
                    BulkVector_p->State = SENDING_CHUNKS;
                }
            }

            break;

        case SENDING_CHUNKS:

            if (R15_Bulk_CheckTransmitedChunks(BulkVector_p)) {
                /* save the current bulk vector before bulk session is closed */
                memcpy(&(R15_TRANSPORT(Communication_p))->PreviousBulkVector, BulkVector_p, sizeof(TL_BulkVectorList_t));
                R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = R15_Bulk_GetTimerChunkRetransmision(Communication_p, R15_TIMEOUTS(Communication_p)->TBDR, (HandleFunction_t)R15_Bulk_RetransmitChunks_CallBack);
                BulkVector_p->State = WAIT_BULK_ACK;
                C_(printf("bulk_protocol.c(%d) Wait BULK ACK for session (%d)!\n", __LINE__, BulkVector_p->SessionId);)
            } else if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[BulkVector_p->SendingChunkId].Buffer_p, BUF_TX_SENT)) {
                /* Get Chunk ID of next packet! */
                BulkVector_p->SendingChunkId = R15_Bulk_GetChunkForSending(BulkVector_p);

                if (BulkVector_p->SendingChunkId <= BulkVector_p->Buffers - 1) {
                    /* Send packet with chunk ID */
                    R15_Bulk_SendData(Communication_p, BulkVector_p->Entries[BulkVector_p->SendingChunkId].Buffer_p);
#ifndef  CFG_ENABLE_LOADER_TYPE

                    if (NULL != R15_TRANSPORT(Communication_p)->BulkDataCallback_p) {
                        BulkDataReqCallback_t pcbf = (BulkDataReqCallback_t)R15_TRANSPORT(Communication_p)->BulkDataCallback_p;
                        BulkVector_p->TransferedLength += BulkVector_p->ChunkSize;
                        pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, BulkVector_p->TotalLength, BulkVector_p->TransferedLength);
                    }

#endif
                    C_(printf("bulk_protocol.c(%d) Sent chunk (%d) session (%d)\n", __LINE__, BulkVector_p->SendingChunkId, BulkVector_p->SessionId);)
                }
            }

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
            ReturnValue = MP(Measurement_p, BulkVector_p->SendingChunkId, RECEIVED_CHUNK_TIME);
#endif
            break;

#ifndef  CFG_ENABLE_LOADER_TYPE

        case WAIT_TX_DONE:

            /* Wait for all chunks in the current session to be send before closing
               the current session and notifying start of the new session */
            if (R15_Bulk_SessionTxDone(BulkVector_p)) {
                /* save the current bulk vector before bulk session is closed */
                memcpy(&(R15_TRANSPORT(Communication_p))->PreviousBulkVector, BulkVector_p, sizeof(TL_BulkVectorList_t));

                // notify session end
                BulkCommandReqCallback_t pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, TRUE);

                // close the current bulk session
                (void)Do_R15_Bulk_CloseSession(Communication_p, BulkVector_p); //TODO: return value should be handled

                BulkExtendedHeader_t *PendingHeader_p = R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p;

                if (NULL != PendingHeader_p) {
                    // set bulk parameters for new bulk session if request for starting new session was received
                    pcbf(Communication_p->Object_p, PendingHeader_p->Session, PendingHeader_p->ChunkSize, PendingHeader_p->Offset, PendingHeader_p->Length, FALSE);
                    BUFFER_FREE(R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p);
                }
            }

            break;
#endif

        case WRITE_BULK_FINISH:
            BulkVector_p->State = BULK_IDLE_STATE;
            BulkVector_p->Status = BULK_SESSION_FINISHED;
            C_(printf("bulk_protocol.c(%d) Write bulk process finished! \n", __LINE__);)
            break;
#ifndef  CFG_ENABLE_LOADER_TYPE

        case CANCEL_BULK: {
            uint32 Counter;

            for (Counter = 0 ; (Counter < BulkVector_p->Buffers); Counter++) {
                if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[Counter].Buffer_p, BUF_CRC_CALCULATING)) {
                    goto ErrExit;
                }

                if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[Counter].Buffer_p, BUF_TX_SENDING)) {
                    goto ErrExit;
                }
            }

            (void)Do_R15_Bulk_CloseSession(Communication_p, BulkVector_p);
            (void)Do_R15_Bulk_DestroyVector(Communication_p, BulkVector_p, FALSE);
            BulkVector_p->State = BULK_IDLE_STATE;
            pcbf = (BulkBuffersRelease_t)R15_TRANSPORT(Communication_p)->BulkBufferRelease_p;
            pcbf(Communication_p->Object_p, BulkVector_p);
        }
        break;
#endif

        default:
            BulkVector_p->State = BULK_IDLE_STATE;
            break;
        }
    }

#ifndef  CFG_ENABLE_LOADER_TYPE
ErrExit:
#endif //CFG_ENABLE_LOADER_TYPE
    Do_CriticalSection_Leave(R15_TRANSPORT(Communication_p)->BulkHandle.BulkTransferCS);

    return ReturnValue;
}


static ErrorCode_e R15_Bulk_SendReadRequest(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, uint32 Chunks, void *ChunksList_p, void *CallBack_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;
    BulkExtendedHeader_t ExtendedHeader;
    R15_Header_t  Header;
    SendData_LP_t Param;

    /* setup header data */
    Header.Protocol = BULK_PROTOCOL;
    Header.Flags = Communication_p->CurrentFamilyHash;
    Header.PayloadLength = Chunks * sizeof(uint8);
    Header.PayloadChecksum = 0;
    Header.ExtendedHeaderLength = BULK_EXTENDED_HEADER_LENGTH;
    Header.ExtendedHeaderChecksum = 0;

    /* Set extended header parameters */
    ExtendedHeader.Session = BulkVector_p->SessionId;
    ExtendedHeader.AcksChunk = (uint8)Chunks;
    ExtendedHeader.ChunkSize = BulkVector_p->ChunkSize;
    ExtendedHeader.Offset = BulkVector_p->Offset;
    ExtendedHeader.Length = BulkVector_p->Length;
    ExtendedHeader.TypeFlags = CMD_BULK_READ;

    Param.Header_p = &Header;
    Param.ExtendedHeader_p = &ExtendedHeader;

    //TODO:  (xvlapis) check if ChunksList_p=NULL meaning ReadCMD which requires retransmission timer, else ReadACK CMD which doesn't require retransmission
    if (0 != Chunks || (0 == Chunks && NULL == CallBack_p)) {
        VERIFY(NULL != ChunksList_p, E_INVALID_INPUT_PARAMETERS);
        Param.Payload_p = ChunksList_p;
        Param.TimerCallBackFn_p = NULL;
        Param.Time = 0;
    } else {
        Param.Time = R15_TIMEOUTS(Communication_p)->TBDR; // Receiving chunks

        VERIFY(NULL != CallBack_p, E_INVALID_INPUT_PARAMETERS);
        //lint --e(611)
        Param.TimerCallBackFn_p = (HandleFunction_t)CallBack_p;
    }

    ReturnValue = R15_Transport_Send(Communication_p, &Param);

ErrorExit:
    return ReturnValue;
}


static ErrorCode_e R15_Bulk_SendWriteRequest(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;
    BulkExtendedHeader_t ExtendedHeader;
    R15_Header_t  Header;
    SendData_LP_t Param;
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;

    /* setup header data */
    Header.Protocol = BULK_PROTOCOL;
    Header.Flags = Communication_p->CurrentFamilyHash;
    Header.PayloadLength = 0;
    Header.PayloadChecksum = 0;
    Header.ExtendedHeaderLength = BULK_EXTENDED_HEADER_LENGTH;
    Header.ExtendedHeaderChecksum = 0;

    /* Set extended header parameters */
    ExtendedHeader.Session = BulkVector_p->SessionId;
    ExtendedHeader.AcksChunk = 0;
    ExtendedHeader.ChunkSize = BulkVector_p->ChunkSize;
    ExtendedHeader.Offset = BulkVector_p->Offset;
    ExtendedHeader.Length = BulkVector_p->Length;
    ExtendedHeader.TypeFlags = CMD_BULK_WRITE;

    Param.Header_p = &Header;
    Param.ExtendedHeader_p = &ExtendedHeader;
    Param.Payload_p = NULL;
    Param.Time = R15_TIMEOUTS(Communication_p)->TBCR;
    Param.TimerCallBackFn_p = NULL;
    ReturnValue = R15_Transport_Send(Communication_p, &Param);
    return ReturnValue;
}

static void R15_Bulk_SerializeChunk(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint32 ChunkId)
{
    BulkExtendedHeader_t ExtendedHeader;
    R15_Header_t Header;
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;
    uint32 DeltaLength = 0;

    memset(&(Packet_p->Timer), 0, sizeof(Timer_t));
    Packet_p->Resend = 0;
    /* set call back function */
    Packet_p->CallBack_p = NULL;

    /* setup header data */
    memset(&Header, 0, HEADER_LENGTH);
    Header.Protocol = BULK_PROTOCOL;
    Header.Flags = Communication_p->CurrentFamilyHash;

    if ((ChunkId + 1) == BulkVector_p->Buffers) {
        DeltaLength = (ChunkId + 1) * BulkVector_p->ChunkSize - BulkVector_p->Length;

        if (DeltaLength == 0) {
            Header.PayloadLength = BulkVector_p->ChunkSize;
        } else {
            Header.PayloadLength = BulkVector_p->ChunkSize - DeltaLength;
        }
    } else {
        Header.PayloadLength = BulkVector_p->ChunkSize;
    }

    Header.ExtendedHeaderLength = BULK_EXTENDED_HEADER_LENGTH;

    /* Set extended header parameters */
    ExtendedHeader.Session = BulkVector_p->SessionId;
    ExtendedHeader.AcksChunk = (uint8)ChunkId;
    ExtendedHeader.ChunkSize = Header.PayloadLength;
    ExtendedHeader.Offset = BulkVector_p->Offset;
    ExtendedHeader.Length = BulkVector_p->Length;
    ExtendedHeader.TypeFlags = CMD_BULK_DATA;

    /* serialize and calculate extended header */
    Packet_p->ExtendedHeader_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;
    R15_SerializeExtendedHeader(Packet_p->ExtendedHeader_p, Header.Protocol, &ExtendedHeader, &(Header.ExtendedHeaderChecksum));
    /* setup header for serialization and calculation */
    memcpy(&Packet_p->Header, &Header, HEADER_LENGTH);

    /* Calculate Payload CRC */
    Packet_p->Communication_p = Communication_p;
    SET_PACKET_FLAGS(Packet_p, PACKET_CRC_STATE_MASK, BUF_CRC_CALCULATING);

    if (HASH_NONE != Packet_p->Header.Flags) {
        Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
                (HashType_e)Packet_p->Header.Flags,
                (void *)Packet_p->Payload_p, Packet_p->Header.PayloadLength,
                Packet_p->Hash, (HashCallback_t)R15_Bulk_OutHashCallback,
                (void *)Packet_p);
    } else {
        memset(&Packet_p->Header.PayloadChecksum, 0x0, sizeof(uint32));
        R15_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);
        SET_PACKET_FLAGS(Packet_p, PACKET_CRC_STATE_MASK, BUF_PAYLOAD_CRC_CALCULATED);
    }
}

static void R15_Bulk_SendData(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    SET_PACKET_FLAGS(Packet_p, PACKET_TX_STATE_MASK, BUF_TX_SENDING);
    (void)QUEUE(Packet_p->Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Outbound_p, Packet_p);
}

static void R15_Bulk_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    PacketMeta_t *Packet_p = (PacketMeta_t *)Param_p;

    memcpy(&Packet_p->Header.PayloadChecksum, Hash_p, sizeof(uint32));
    R15_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

    SET_PACKET_FLAGS(Packet_p, PACKET_CRC_STATE_MASK, BUF_PAYLOAD_CRC_CALCULATED);
}


static void R15_Bulk_ReadChunkCallBack(Communication_t *Communication_p, const void *const Timer_p, const void *const Data_p)
{
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;

    if (NULL == BulkVector_p) {
        A_(printf("bulk_protocol.c(%d) Bulk Vector released! \n", __LINE__);)
        return;
    }

    BulkVector_p->BulkRetransmissionNo ++;

    if (BulkVector_p->BulkRetransmissionNo > MAX_RESENDS) {
        A_(printf("bulk_protocol.c(%d) Bulk Retransmission Failed, loader will stop with execution!\n", __LINE__);)
        R15_NETWORK(Communication_p)->Outbound.LCM_Error = E_RETRANSMITION_FAILED;
        return;
    }

    if (BULK_SESSION_FINISHED != BulkVector_p->Status) {
        uint32 ChunkId = 0;
        uint8 ChunksList[MAX_BULK_TL_PROCESSES] = {0};

        R15_Bulk_GetListOfReceivedChunks(BulkVector_p, &ChunkId, ChunksList);
        BulkVector_p->State = WAIT_CHUNKS;

        A_(printf("bulk_protocol.c(%d) Number Of Timer Retransmissions %d \n", __LINE__, BulkVector_p->BulkRetransmissionNo);)

        R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = R15_Bulk_GetTimerChunkRetransmision(Communication_p, R15_TIMEOUTS(Communication_p)->TBDR, (HandleFunction_t)R15_Bulk_ReadChunkCallBack);

        (void)R15_Bulk_SendReadRequest(Communication_p, BulkVector_p, ChunkId, ChunksList, NULL);
    }
}

static void R15_Bulk_RetransmitChunks_CallBack(const Communication_t *const Communication_p, const void *const Timer_p, const void *const Data_p)
{
    /* Set all chunks for retransmission. Max retransmission is 3. */
    uint32 ChunkId;
    PacketMeta_t *Packet_p;

    for (ChunkId = 0; ChunkId < R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p->Buffers; ChunkId++) {
        Packet_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p->Entries[ChunkId].Buffer_p;
        /* set packet as ready for sending. */
        SET_PACKET_FLAGS(Packet_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
    }

    R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p->State = PROCESSING_CHUNKS;
}

static void R15_Bulk_GetListOfReceivedChunks(const TL_BulkVectorList_t *const BulkVector_p, uint32 *Chunks_p, uint8 *ChunkList_p)
{
    uint8 ChunkCounter;
    *Chunks_p = 0;

    for (ChunkCounter = 0; ChunkCounter < BulkVector_p->Buffers; ChunkCounter++) {
        if (NULL != BulkVector_p->Entries[ChunkCounter].Buffer_p) {
            ChunkList_p[*Chunks_p] = ChunkCounter;
            (*Chunks_p)++;
        }
    }
}

__inline static boolean R15_Bulk_AllChunksProcessed(const TL_BulkVectorList_t *const BulkVector_p)
{
    boolean Status = TRUE;
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if (!CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_PAYLOAD_CRC_CALCULATED)) {
            Status = FALSE;
            break;
        }
    }

    return Status;
}

#ifndef  CFG_ENABLE_LOADER_TYPE
/*  Check if all chunks are sent.
 *
 *  Returns FALSE if some of the chunks is in process of sending, otherwise returns TRUE.
 *  Needed to decide if current session can be closed immediately or should wait for
 *  chunk to finish transmission in order to close the session and destroy vector.
 */
static boolean R15_Bulk_SessionTxDone(const TL_BulkVectorList_t *const BulkVector_p)
{
    boolean Status = TRUE;
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        if (CHECK_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, BUF_TX_SENDING)) {
            /* If some of chunks is in process of sending return FALSE */
            Status = FALSE;
        } else {
            /* All chunks are acknowledged, set TX_DONE for packages that are not in process of sending */
            SET_PACKET_FLAGS(BulkVector_p->Entries[ChunkId].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_DONE);
        }
    }

    return Status;
}

__inline static boolean IsChunkReceived(Communication_t *Communication_p, uint32 ChunkId)
{
    PacketMeta_t *Packet_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p->Entries[ChunkId].Buffer_p;

    if (NULL != Packet_p) {
        return CHECK_PACKET_FLAGS(Packet_p, BUF_ACK_READY) || CHECK_PACKET_FLAGS(Packet_p, BUF_ACKNOWLEDGED);
    } else {
        return FALSE;
    }
}
#endif

static uint32 R15_Bulk_GetChunkId(const PacketMeta_t *const Packet_p)
{
    BulkExtendedHeader_t BulkExtendedHeader = {0};

    //lint -e(413,826)
    R15_DeserializeBulkExtendedHeader(&BulkExtendedHeader, Packet_p->ExtendedHeader_p);

    return BulkExtendedHeader.AcksChunk;
}

static TL_BulkVectorStatus_t R15_Bulk_GetVectorStatus(TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkCounter;
    PacketMeta_t *Packet_p;
    TL_BulkVectorStatus_t Status = VECTOR_COMPLETE;

    for (ChunkCounter = 0; ChunkCounter < BulkVector_p->Buffers; ChunkCounter++) {
        Packet_p = BulkVector_p->Entries[ChunkCounter].Buffer_p;

        if (Packet_p == NULL ||
                !(CHECK_PACKET_FLAGS(Packet_p, BUF_ACK_READY) || CHECK_PACKET_FLAGS(Packet_p, BUF_ACKNOWLEDGED))) {
            Status = VECTOR_NOT_COMPLETE;
        } else {
            if (Status == VECTOR_NOT_COMPLETE) {
                Status = VECTOR_MISSING_CHUNK;
                break;
            }
        }
    }

    return Status;
}

/*  Determine if it is needed to send ReadRequest for retransmission of missing chunk(s).
 *
 *  Function checks if the missing chunk(s) until the currently received chunk was
 *  requested to be retransmitted earlier. If not it sets RetransmissionRequested
 *  to all the missing chunks and decides that sending of ReadRequest is needed.
 *
 */
static boolean R15_Bulk_VectorNeedsRetransmission(TL_BulkVectorList_t *BulkVector_p, uint32 CurrentChunkId)
{
    boolean Status = FALSE;
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId <= CurrentChunkId; ChunkId++) {
        if (NULL == BulkVector_p->Entries[ChunkId].Buffer_p) {
            if (FALSE == BulkVector_p->Entries[ChunkId].RetransmissionRequested) {
                BulkVector_p->Entries[ChunkId].RetransmissionRequested = TRUE;
                Status = TRUE;
            }
        }
    }

    return Status;
}

/*
 *  Clear RetransmissionRequested flag for all chunks in the given bulk vector.
 *
 */
__inline static void R15_Bulk_VectorClearRetransmissionRequested(TL_BulkVectorList_t *BulkVector_p)
{
    uint32 ChunkId;

    for (ChunkId = 0; ChunkId < BulkVector_p->Buffers; ChunkId++) {
        BulkVector_p->Entries[ChunkId].RetransmissionRequested = FALSE;
    }
}

static uint32 R15_Bulk_GetTimerChunkRetransmision(const Communication_t *const Communication_p, uint32 Time, HandleFunction_t CallBack_p)
{
    Timer_t Timer;

    Timer.Time = Time;
    Timer.PeriodicalTime = 0;
    Timer.HandleFunction_p = (HandleFunction_t)CallBack_p;
    Timer.Data_p = NULL;
    Timer.Param_p = (void *)Communication_p;

    return TIMER(Communication_p, TimerGet_Fn)(OBJECT_TIMER(Communication_p), &Timer);
}

static ErrorCode_e R15_Bulk_ReadRequestHandler(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    BulkExtendedHeader_t ExtendedHeader = {0};
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;
    TL_BulkSessionID_Status_t BulkSessionIDStatus;
#ifndef  CFG_ENABLE_LOADER_TYPE
    boolean ACK_Read = FALSE;
    uint32 Buffers;
    BulkCommandReqCallback_t pcbf;
#endif

    R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

    // check the session ID of the received packet
    BulkSessionIDStatus = R15_Bulk_CheckBulkSession(Communication_p, ExtendedHeader.Session);
    VERIFY(BULK_SESSION_INVALID != BulkSessionIDStatus, E_INVALID_BULK_SESSION_ID);

#ifdef  CFG_ENABLE_LOADER_TYPE

    if (BULK_SESSION_NEW != BulkSessionIDStatus) {
        // check witch chunks are acknowledged!
        if (Packet_p->Header.PayloadLength > 0) {
            if (!R15_Bulk_CheckAcknowledgedChunks(BulkVector_p, Packet_p->Payload_p)) {
                /* mark all not acknowledged chunks for retransmission */
                R15_Bulk_MarkNotAckChunks(BulkVector_p, Packet_p->Payload_p, ExtendedHeader.AcksChunk);
                BulkVector_p->State = PROCESSING_CHUNKS;
            } else {
                BulkVector_p->State = WRITE_BULK_FINISH;
            }

            /* Try to release the timer for the bulk session acknowledge */
            if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
                (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey);
                R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
            }
        } else {
            if ((BulkVector_p->State == PROCESSING_CHUNKS) || (BulkVector_p->State == SENDING_CHUNKS) ||
                    (BulkVector_p->State == WAIT_CHUNK_SENT) || (BulkVector_p->State == WAIT_BULK_ACK)) {
                /* mark all chunks for retransmission */
                R15_Bulk_MarkNotAckAllChunks(BulkVector_p);
            } else {
                /* release the buffer for previous command BULK WRITE */
                uint32 Key = R15_Network_CreateUniqueKey(Packet_p, CMD_BULK_WRITE);
                ReturnValue = R15_Network_CancelRetransmission(Communication_p, Key);
                VERIFY_CONDITION((E_SUCCESS == ReturnValue) || (E_NOT_FOUND_ELEMENT_IN_RETRANSMISSION_LIST == ReturnValue));
            }

            BulkVector_p->State = PROCESSING_CHUNKS;
        }

        ReturnValue = R15_Bulk_Process_Write(Communication_p, BulkVector_p);   // state machine for WRITE BULK data
    }

#else
    C_(printf("bulk_protocol.c(%d) Received READ packet!\n", __LINE__);)
    C_(printf("bulk_protocol.c(%d) Session (%d)!\n", __LINE__, ExtendedHeader.Session);)

    /* Check the ACK */
    if (NULL != R15_TRANSPORT(Communication_p)->BulkCommandCallback_p) {
        // check the session ID of the received packet
        if (BULK_SESSION_NEW == BulkSessionIDStatus) {
            /* request for new bulk session */
            // check the status of the current bulk session
            if (Do_R15_Bulk_GetStatusSession(R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p) != BULK_SESSION_IDLE) {
                if (BulkVector_p->State == WAIT_BULK_ACK) {
                    C_(printf("bulk_protocol.c(%d) Request for new session but current is not closed!\n", __LINE__);)
                    C_(printf("bulk_protocol.c(%d) Current: Session (%d)!\n", __LINE__, BulkVector_p->SessionId);)
                    C_(printf("bulk_protocol.c(%d) ReceivedPacket: Session (%d)!\n", __LINE__, ExtendedHeader.Session);)

                    if (R15_Bulk_SessionTxDone(BulkVector_p)) {
                        // notify session end
                        pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                        pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, TRUE);

                        // close the current bulk session
                        (void)Do_R15_Bulk_CloseSession(Communication_p, BulkVector_p); //TODO: return value should be handled

                        // set bulk parameters for new bulk session
                        ACK_Read = FALSE;
                        pcbf(Communication_p->Object_p, ExtendedHeader.Session, ExtendedHeader.ChunkSize, ExtendedHeader.Offset, ExtendedHeader.Length, ACK_Read);
                    } else {
                        /* Clean the previous saved data if there is any */
                        BUFFER_FREE(R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p);

                        /* Save the received extended header for the new bulk session */
                        R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p = (BulkExtendedHeader_t *)malloc(sizeof(BulkExtendedHeader_t));
                        memcpy(R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p, &ExtendedHeader, sizeof(BulkExtendedHeader_t));

                        /* Wait for all chunks which are in process of sending to be sent before opening a new session */
                        BulkVector_p->State = WAIT_TX_DONE;
                    }
                } else {
                    // can't be opened new bulk session until current session is not finished.
                    goto ErrorExit;
                }
            } else {
                C_(printf("bulk_protocol.c(%d) Request for new bulk session!\n", __LINE__);)
                // received request for new bulk session
                ACK_Read = FALSE;
                pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                pcbf(Communication_p->Object_p, ExtendedHeader.Session, ExtendedHeader.ChunkSize, ExtendedHeader.Offset, ExtendedHeader.Length, ACK_Read);
            }
        } else {
            // current bulk session
            Buffers = ((ExtendedHeader.Length + ExtendedHeader.ChunkSize - 1) / ExtendedHeader.ChunkSize);

            if ((0 == ExtendedHeader.AcksChunk) &&
                    (Do_R15_Bulk_GetStatusSession(R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p) == BULK_SESSION_IDLE)) {
                C_(printf("bulk_protocol.c(%d) Request for new bulk session(%d)!\n", __LINE__, ExtendedHeader.Session);)

                /* Try to release the timer for the bulk session acknowledge */
                if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
                    (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey); // LCM bug fix: Timer should be released on request for retransmission
                    R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
                }

                ACK_Read = FALSE;
                pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                pcbf(Communication_p->Object_p, ExtendedHeader.Session, ExtendedHeader.ChunkSize, ExtendedHeader.Offset, ExtendedHeader.Length, ACK_Read);
            } else if (Buffers == ExtendedHeader.AcksChunk) {
                C_(printf("bulk_protocol.c(%d) ACK for bulk session(%d)!\n", __LINE__, ExtendedHeader.Session);)
                (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey);

                if (R15_Bulk_SessionTxDone(BulkVector_p)) {
                    // notify session end
                    pcbf = (BulkCommandReqCallback_t)R15_TRANSPORT(Communication_p)->BulkCommandCallback_p;
                    pcbf(Communication_p->Object_p, BulkVector_p->SessionId, BulkVector_p->ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, TRUE);

                    (void)Do_R15_Bulk_CloseSession(Communication_p, BulkVector_p); //TODO: return value should be handled
                } else {
                    /* Clean the previous saved data if there is any */
                    BUFFER_FREE(R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p);

                    /* Wait for all chunks of current session to be sent before opening new session */
                    BulkVector_p->State = WAIT_TX_DONE;
                }
            } else {
                if (0 == ExtendedHeader.AcksChunk) {
                    /* mark all chunks for retransmission */
                    R15_Bulk_MarkNotAckAllChunks(BulkVector_p);
                } else {
                    /* mark all not acknowledged chunks for retransmission */
                    A_(printf("bulk_protocol.c (%d): Mark All NACK Chunks for Retransmission **\n", __LINE__);)
                    R15_Bulk_MarkNotAckChunks(BulkVector_p, Packet_p->Payload_p, ExtendedHeader.AcksChunk);
                }

                BulkVector_p->State = PROCESSING_CHUNKS;

                /* Try to release the timer for the bulk session acknowledge */
                if (R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey > 0) {
                    (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey); // LCM bug fix: Timer should be released on request for retransmission
                    R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
                }
            }
        }
    }

#endif

ErrorExit:
    /* release the buffer for command BULK READ */
    ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
    return ReturnValue;
}

static ErrorCode_e R15_Bulk_DataRequestHandler(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    BulkExtendedHeader_t ExtendedHeader = {0};
    TL_BulkVectorList_t *BulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;
    TL_BulkVectorList_t *PreviousBulkVector_p = &(R15_TRANSPORT(Communication_p)->PreviousBulkVector);
    uint32 ChunksCount = 0;
    uint8 ChunksList[MAX_BULK_TL_PROCESSES];
#ifndef  CFG_ENABLE_LOADER_TYPE
    uint32 ChunkId = 0;
    BulkDataReqCallback_t pcbf;
#endif

    R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

#ifdef  CFG_ENABLE_LOADER_TYPE

    if (ExtendedHeader.AcksChunk == 0) {
        BulkVector_p->State = WAIT_CHUNKS;
        R15_TRANSPORT(Communication_p)->BulkHandle.TimerKey = 0;
    }

    if (PreviousBulkVector_p->SessionId == BulkVector_p->SessionId) {
        // make the list of received chunks
        R15_Bulk_GetListOfReceivedChunks(PreviousBulkVector_p, &ChunksCount, ChunksList);
        // send read ACK for previous session
        ReturnValue = R15_Bulk_SendReadRequest(Communication_p, PreviousBulkVector_p, ChunksCount, ChunksList, NULL);
        // release the buffer used for this data chunk
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        goto ErrorExit;
    }

    /* cancel retransmission, release buffer */
    ReturnValue = R15_Network_CancelRetransmission(Communication_p, R15_Network_CreateUniqueKey(Packet_p, CMD_BULK_READ));
    VERIFY_CONDITION((E_SUCCESS == ReturnValue) || (ReturnValue == E_NOT_FOUND_ELEMENT_IN_RETRANSMISSION_LIST));
    /* state machine for READ BULK data */
    ReturnValue = R15_Bulk_Process_Read(Communication_p, BulkVector_p, Packet_p);
#else

    if (Do_R15_Bulk_GetStatusSession(BulkVector_p) == BULK_SESSION_IDLE) {
        //no opened current session
        // make the list of received chunks
        R15_Bulk_GetListOfReceivedChunks(PreviousBulkVector_p, &ChunksCount, ChunksList);
        // send read ACK for previous session
        ReturnValue = R15_Bulk_SendReadRequest(Communication_p, PreviousBulkVector_p, ChunksCount, ChunksList, NULL);
        // release the buffer used for this data chunk
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
    } else {
        if (NULL != R15_TRANSPORT(Communication_p)->BulkDataCallback_p) {
            ChunkId = ExtendedHeader.AcksChunk;

            if (!IsChunkReceived(Communication_p, ChunkId)) {
                BulkVector_p->TransferedLength += BulkVector_p->ChunkSize;
                pcbf = (BulkDataReqCallback_t)R15_TRANSPORT(Communication_p)->BulkDataCallback_p;
                pcbf(Communication_p->Object_p, BulkVector_p->SessionId, ExtendedHeader.ChunkSize, BulkVector_p->Offset, BulkVector_p->Length, BulkVector_p->TotalLength, BulkVector_p->TransferedLength);
                C_(printf("S(%d) L(%d) CS(%d)\n", BulkVector_p->SessionId, BulkVector_p->Length, ExtendedHeader.ChunkSize);)
            }
        }

        /* cancel retransmission, release buffer */
        ReturnValue = R15_Network_CancelRetransmission(Communication_p, R15_Network_CreateUniqueKey(Packet_p, CMD_BULK_READ));
        VERIFY_CONDITION((E_SUCCESS == ReturnValue) || (ReturnValue == E_NOT_FOUND_ELEMENT_IN_RETRANSMISSION_LIST));
        /* state machine for READ BULK data */
        ReturnValue = R15_Bulk_Process_Read(Communication_p, BulkVector_p, Packet_p);
    }

#endif

ErrorExit:
    return ReturnValue;
}


/*
 * Check the bulk session ID. Can be new bulk session or the current opened bulk
 * session. Session ID can't be the '0'. Allowed Session ID is 1-65535.
 *
 * @param[in] Communication_p Communication module context.
 * @param[in] SessionId       Session ID from the received bulk packet.
 *
 * @retval BULK_SESSION_INVALID If the Session ID=0.
 * @retval BULK_SESSION_CURRENT If the session ID is same as current session ID.
 * @retval BULK_SESSION_NEW     New bulk session
 */
static TL_BulkSessionID_Status_t R15_Bulk_CheckBulkSession(Communication_t *Communication_p, uint16 SessionId)
{
    TL_BulkSessionID_Status_t ReturnStatus = BULK_SESSION_INVALID;
    TL_BulkVectorList_t *CurrentBulkVector_p = R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p;
#ifndef  CFG_ENABLE_LOADER_TYPE
    TL_BulkVectorList_t *PreviousBulkVector_p = &(R15_TRANSPORT(Communication_p)->PreviousBulkVector);
#endif

    if (SessionId == 0) {
        return ReturnStatus;
    }

#ifndef  CFG_ENABLE_LOADER_TYPE

    if (PreviousBulkVector_p == NULL) {
        return BULK_SESSION_NEW;
    }

    if ((PreviousBulkVector_p->SessionId != 0) && (Do_R15_Bulk_GetStatusSession(CurrentBulkVector_p) != BULK_IDLE_STATE)) {
#endif

        //lint --e(539)
        if (CurrentBulkVector_p->SessionId == SessionId) {
            ReturnStatus = BULK_SESSION_CURRENT;
        } else {
            ReturnStatus = BULK_SESSION_NEW;
        }

#ifndef  CFG_ENABLE_LOADER_TYPE
    } else {
        if (PreviousBulkVector_p->SessionId < SessionId ||
                (SessionId == 1 && PreviousBulkVector_p->SessionId > SessionId)) {
            ReturnStatus = BULK_SESSION_NEW;
        } else {
            ReturnStatus = BULK_SESSION_INVALID;
        }
    }

#endif
    return ReturnStatus;
}

/** @} */
/** @} */
/** @} */
