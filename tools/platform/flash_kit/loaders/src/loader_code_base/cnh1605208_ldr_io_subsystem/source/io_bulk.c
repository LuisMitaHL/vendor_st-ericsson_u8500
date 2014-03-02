/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_Bulk
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_io_subsystem.h"
#include "r_communication_service.h"
#include "r_r15_transport_layer.h"
#include "r_bulk_protocol.h"
#include "io_subsystem.h"
#include "r_bulk_protocol.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Bulk Protocol Wrapper Functions
 ******************************************************************************/
#define OpenBulkSession(x,y)      Do_R15_Bulk_OpenSession(GlobalCommunication_p,(x),(y))
#define CreateBulkVector(x,y,z,w) Do_R15_Bulk_CreateVector(GlobalCommunication_p,(x),(y),(z),(w))
#define StartBulkSession(x,y)     Do_R15_Bulk_StartSession(GlobalCommunication_p,(x),(y))
#define GetStatusBulkSession(x)   Do_R15_Bulk_GetStatusSession(x) //xvsvlpi
#define CloseBulkSession(x)       Do_R15_Bulk_CloseSession(GlobalCommunication_p,(x))
#define GenerateBulkSessionID()   Do_R15_Bulk_GenerateBulkSessionID(GlobalCommunication_p)


/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint32 BulkOpen(const void *const Context_p, uint32 *BulkSessionID_p);
static uint32 BulkClose(void *Context_p);
static uint64 BulkLength(const void *const Context_p);
static uint32 BulkCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);
static uint64 BulkPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BulkPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BulkScatteredPositionedRead(const void *const Context_p,  const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 BulkGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  Defines functions for Bulk IO Interface
 */
static const IO_Interface_t BulkInterface = {
    BulkClose,
    BulkPositionedRead,
    BulkPositionedWrite,
    BulkPositionedRead,
    BulkPositionedWrite,
    BulkScatteredPositionedRead,
    BulkGatheredPositionedWrite,
    BulkLength,
    BulkCreateVector
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens Bulk transfer resource and returns Handle to it.
 *
 *  @param [in] SessionId     Session ID of the session to use in bulk transfer.
 *  @param [in] Mode          Mode to open the session in.
 *  @param [in] Length        Length of data to transfer in this session.
 *
 *  @retval IO_Handle_t       Initialized IO Handle to a Bulk.
 *  @retval IO_INVALID_HANDLE if resource can not be opened,
 *                            or no space can be allocated for Bulk context,
 *                            or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BulkOpen(const uint32 SessionId, TL_SessionMode_t Mode, const uint64 Length)
{
    static BulkContext_t *BulkContext_p = NULL;
    IO_Handle_t Handle = IO_INVALID_HANDLE;

    BulkContext_p = malloc(sizeof(BulkContext_t));
    ASSERT(NULL != BulkContext_p);

    BulkContext_p->SessionMode = Mode;
    BulkContext_p->Length = Length;

    Handle = IO_Create(&BulkInterface, BulkContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(BulkContext_p);
    }

    return Handle;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Open bulk session.
 *
 * @param [in] Context_p        initialized IO channel context.
 *
 * @retval  E_SUCCESS           when IO channel is closed,
 *
 */
static uint32 BulkOpen(const void *const Context_p, uint32 *BulkSessionID_p)
{
    BulkContext_t *BulkContext_p = (BulkContext_t *) Context_p;
    uint16 BulkSession = 0;
    uint32 Result = 0;

    BulkSession = GenerateBulkSessionID();
    *BulkSessionID_p = OpenBulkSession(BulkSession, BulkContext_p->SessionMode);

    if (*BulkSessionID_p == BULK_ERROR) {
        A_(printf("Error while opening Bulk Session io_bulk.c (%d)\n\n", __LINE__);)
        Result = BULK_ERROR;
        goto ErrorExit;
    }

    Result = 0;
ErrorExit:
    return Result;
}

/*
 * Creates IO Bulk vector.
 *
 * IO Vector is a space of underlying buffers
 * referenced by unique Vector Handle.
 *
 * @param [in] Context_p        initialized IO Channel context.
 * @param [in] Length           in bytes for Vector space.
 * @param [in] BuffSize         Size of each underlying buffer.
 *
 * @retval   IO_VECTOR_INVALID_HANDLE   if invalid Handle is passed,
 * @retval   VectorHandle               if all Buffers for the Vector
 *                                      are allocated and reserved.
 *
 */
static uint32 BulkCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    BulkContext_t *BulkContext_p = (BulkContext_t *) Context_p;
    TL_BulkVectorList_t *BulkVector_p = NULL;
    uint32 VectorIndex = IO_VECTOR_INVALID_HANDLE;
    uint32 NrOfBuffers = 0;
    uint32 Counter = 0;
    uint32 BulkSessionID = 0;

    if ((Length > (MAX_BULK_TL_PROCESSES *(BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA))) ||
            (BuffSize > (BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA)) ||
            (BuffSize == 0) ||
            (BulkContext_p->SessionMode == 0)) {
        A_(printf("Error: Invalid input parameters ... io_bulk.c (%d)\n\n", __LINE__);)
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    NrOfBuffers = (uint32)((Length + BuffSize - 1) / BuffSize);
    VectorIndex = IO_FindFreeVector(NrOfBuffers);

    if (VectorIndex >= MAX_OPEN_IO) {
        A_(printf("Not enough buffers ... io_bulk.c (%d)\n\n", __LINE__);)
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    if (BULK_ERROR == BulkOpen(BulkContext_p, &BulkSessionID)) {
        A_(printf("Error: Failed to open bulk session ... io_bulk.c (%d)\n\n", __LINE__);)
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    BulkVector_p = CreateBulkVector(BulkSessionID, (uint32)Length, (uint32)BuffSize, NULL);

    if (BulkVector_p == NULL) {
        A_(printf("Error: Failed to create bulk vector ... io_bulk.c (%d)\n\n", __LINE__);)
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    VectorList[VectorIndex].BulkVector_p = BulkVector_p;
    VectorList[VectorIndex].Status = IOV_MAPPED;
    VectorList[VectorIndex].State = BULK_IDLE_STATE;
    VectorList[VectorIndex].IOffset = 0;
    VectorList[VectorIndex].BulkTransfer = 1;
    VectorList[VectorIndex].Length = Length;
    VectorList[VectorIndex].ChunkSize = (uint32)BuffSize;

    //initialize cached data structure
    memset(&VectorList[VectorIndex].CachedData, 0x00, sizeof(VectorCachedData_t));

    for (Counter = 0; ((Counter < MAX_OPEN_IO) && (NrOfBuffers > 0)); Counter++) {
        NrOfBuffers--;
        VectorList[VectorIndex].Entries[Counter].Status = IOV_MAPPED;
        VectorList[VectorIndex].Entries[Counter].Buffer_p = BulkVector_p->Entries[Counter].Payload_p;
        VectorList[VectorIndex].Entries[Counter].Hash_p = BulkVector_p->Entries[Counter].Hash_p;
    }

    C_(printf("io_bulk.c :Created bulk vector with index %d length %llu\n", VectorIndex, VectorList[VectorIndex].Length);)

ErrorExit:
    return VectorIndex;
}

/*
 * Closes IO channel with given Context.
 *
 * @param [in] Context_p        initialized IO channel context.
 *
 * @retval  E_SUCCESS           when IO channel is closed,
 *
 */
static uint32 BulkClose(void *Context_p)
{
    BUFFER_FREE(Context_p);
    return E_SUCCESS;
}

/*
 * Reads length bytes from IO resource defined by Context
 * starting at Offset location to Buffer.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area to be read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 */
static uint64 BulkPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    static uint32 VectorIndex = IO_VECTOR_INVALID_HANDLE;
    TL_BulkVectorList_t *BulkVector_p = VectorList[VectorIndex].BulkVector_p;
    uint64 ReadLength = IO_ERROR;
    uint32 Counter = 0;
    uint32 Result = 0;

    if ((Length > (MAX_BULK_TL_PROCESSES *(BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA))) || (0 == Length)) {
        A_(printf("io_bulk.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    if (IO_VECTOR_INVALID_HANDLE == VectorIndex) {
        VectorIndex = BulkCreateVector(Context_p, Length, MIN(Length, (BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA)));

        if (VectorIndex >= MAX_OPEN_IO) {
            A_(printf("io_bulk.c (%d): ** Could Not create vector! **\n", __LINE__);)
            IO_ErrorCode = E_VECTOR_CREATE_FAIL;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        if (0 != StartBulkSession(VectorList[VectorIndex].BulkVector_p, Offset)) {
            //
            // Try to close opened session and destroy vector before exit.
            //
            (void)CloseBulkSession(VectorList[VectorIndex].BulkVector_p);
            (void)Do_IO_DestroyVector(VectorIndex);

            A_(printf("io_bulk.c (%d): ** Could Not start bulk session! **\n", __LINE__);)
            IO_ErrorCode = E_FAILED_TO_START_BULK_SESSION;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        ReadLength = IO_WAIT;
    } else {
        if (GetStatusBulkSession(BulkVector_p) == BULK_SESSION_FINISHED) {
            VectorList[VectorIndex].Length = BulkVector_p->Length;
            VectorList[VectorIndex].ChunkSize = BulkVector_p->ChunkSize;
            VectorList[VectorIndex].Status = IOV_VALID;

            for (Counter = 0; Counter < BulkVector_p->Buffers; Counter++) {
                VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
                VectorList[VectorIndex].Entries[Counter].Buffer_p = BulkVector_p->Entries[Counter].Payload_p;
                VectorList[VectorIndex].Entries[Counter].Hash_p = BulkVector_p->Entries[Counter].Hash_p;
            }

            (void)Do_IO_CopyVectorToBuffer(Buffer_p, VectorIndex, VectorList[VectorIndex].Length);
            Length = VectorList[VectorIndex].Length;

            Result = CloseBulkSession(BulkVector_p);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not close bulk session! **\n", __LINE__);)
                IO_ErrorCode = E_FAILED_TO_CLOSE_BULK_SESSION;
                ReadLength = IO_ERROR;
                goto ErrorExit;
            }

            Result = Do_IO_DestroyVector(VectorIndex);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not destroy vector! **\n", __LINE__);)
                IO_ErrorCode = E_VECTOR_DESTROY_FAIL;
                ReadLength = IO_ERROR;
                goto ErrorExit;
            }

            VectorIndex = IO_VECTOR_INVALID_HANDLE;
            ReadLength = Length;
            goto ErrorExit;
        }

        ReadLength = IO_WAIT;
    }

ErrorExit:

    if (IO_ERROR == ReadLength) {
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
    }

    return ReadLength;
}

/*
 * Writes length bytes from Buffer to IO resource
 * defined by Context starting at Offset location.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be written in IO resource.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 */
static uint64 BulkPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    static uint32 VectorIndex = IO_VECTOR_INVALID_HANDLE;
    TL_BulkVectorList_t *BulkVector_p = VectorList[VectorIndex].BulkVector_p;
    uint64 WriteLength = IO_ERROR;
    uint32 Counter = 0;
    uint32 Result = 0;

    if (Length > (MAX_BULK_TL_PROCESSES *(BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA)) || (0 == Length)) {
        B_(printf("io_bulk.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    if (IO_VECTOR_INVALID_HANDLE == VectorIndex) {
        VectorIndex = BulkCreateVector(Context_p, Length, MIN(Length, (BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA)));

        if (VectorIndex >= MAX_OPEN_IO) {
            A_(printf("io_bulk.c (%d): ** Could Not create vector! **\n", __LINE__);)
            IO_ErrorCode = E_VECTOR_CREATE_FAIL;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        VectorList[VectorIndex].Length = VectorList[VectorIndex].BulkVector_p->Length;
        VectorList[VectorIndex].ChunkSize = VectorList[VectorIndex].BulkVector_p->ChunkSize;
        VectorList[VectorIndex].Status = IOV_VALID;

        for (Counter = 0; Counter < VectorList[VectorIndex].BulkVector_p->Buffers; Counter++) {
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
            VectorList[VectorIndex].Entries[Counter].Buffer_p = VectorList[VectorIndex].BulkVector_p->Entries[Counter].Payload_p;
            SET_PACKET_FLAGS(VectorList[VectorIndex].BulkVector_p->Entries[Counter].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
        }

        Do_IO_CopyBufferToVector(VectorIndex, Buffer_p, VectorList[VectorIndex].BulkVector_p->Buffers);

        if (!StartBulkSession(VectorList[VectorIndex].BulkVector_p, Offset)) {
            WriteLength = IO_WAIT;
        } else {
            //
            // Try to close opened session and destroy vector before exit.
            //
            (void)CloseBulkSession(VectorList[VectorIndex].BulkVector_p);
            (void)Do_IO_DestroyVector(VectorIndex);

            A_(printf("io_bulk.c (%d): ** Could Not start bulk session! **\n", __LINE__);)
            IO_ErrorCode = E_FAILED_TO_START_BULK_SESSION;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }
    } else {
        if (GetStatusBulkSession(BulkVector_p) == BULK_SESSION_FINISHED) {
            Length = BulkVector_p->Length;
            Result = CloseBulkSession(BulkVector_p);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not close bulk session! **\n", __LINE__);)
                IO_ErrorCode = E_FAILED_TO_CLOSE_BULK_SESSION;
                WriteLength = IO_ERROR;
                goto ErrorExit;
            }

            Result = Do_IO_DestroyVector(VectorIndex);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not destroy vector! **\n", __LINE__);)
                IO_ErrorCode = E_VECTOR_DESTROY_FAIL;
                WriteLength = IO_ERROR;
                goto ErrorExit;
            }

            VectorIndex = IO_VECTOR_INVALID_HANDLE;
            WriteLength = Length;
            goto ErrorExit;
        }

        WriteLength = IO_WAIT;
    }

ErrorExit:

    if (IO_ERROR == WriteLength) {
        VectorIndex = IO_VECTOR_INVALID_HANDLE;
    }

    return WriteLength;
}

/*
 * Reads length bytes from IO resource defined by Context
 *              starting at Offset location to Vector.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [in]  VectorIndex     Index of Vector to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area that is read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 * @note VectorList[VectorHandle].IOffset is the offset of valid data in IO_Vector
 */
static uint64 BulkScatteredPositionedRead(const void *const Context_p,  const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    TL_BulkVectorList_t *BulkVector_p = VectorList[VectorIndex].BulkVector_p;
    uint64 ReadLength = IO_ERROR;
    uint32 Counter = 0;
    uint32 Result = 0;

    if ((Length > (MAX_BULK_TL_PROCESSES *(BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA))) || (VectorIndex >= MAX_BULK_TL_PROCESSES)) {
        A_(printf("io_bulk.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    switch (VectorList[VectorIndex].State) {
    case BULK_IDLE_STATE:

        if (!StartBulkSession(BulkVector_p, Offset)) {
            VectorList[VectorIndex].State = SEND_READ_REQUEST;
            ReadLength = IO_WAIT;
        } else {
            // Try to close bulk session before exiting
            (void)CloseBulkSession(BulkVector_p);

            A_(printf("io_bulk.c (%d): ** Could Not start bulk session! **\n", __LINE__);)
            IO_ErrorCode = E_FAILED_TO_START_BULK_SESSION;
            ReadLength =  IO_ERROR;
            goto ErrorExit;
        }

        break;

    case SEND_READ_REQUEST:

        if (GetStatusBulkSession(BulkVector_p) == BULK_SESSION_FINISHED) {
            VectorList[VectorIndex].State = BULK_IDLE_STATE;

            VectorList[VectorIndex].Length = BulkVector_p->Length;
            VectorList[VectorIndex].ChunkSize = BulkVector_p->ChunkSize;
            VectorList[VectorIndex].Status = IOV_VALID;

            for (Counter = 0; Counter < BulkVector_p->Buffers; Counter++) {
                VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
                VectorList[VectorIndex].Entries[Counter].Buffer_p = BulkVector_p->Entries[Counter].Payload_p;
                VectorList[VectorIndex].Entries[Counter].Hash_p = BulkVector_p->Entries[Counter].Hash_p;
            }

            Result = CloseBulkSession(BulkVector_p);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not close bulk session! **\n", __LINE__);)
                IO_ErrorCode = E_FAILED_TO_CLOSE_BULK_SESSION;
                ReadLength = IO_ERROR;
                break;
            }

            ReadLength = VectorList[VectorIndex].Length;
            break;
        }

        ReadLength = IO_WAIT;
        break;

    default:
        A_(printf("io_bulk.c (%d): ** Unpredicted behavior! **\n", __LINE__);)
        VectorList[VectorIndex].State = BULK_IDLE_STATE;
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        ReadLength = IO_ERROR;
        break;
    }

ErrorExit:
    return ReadLength;
}

/*
 * Writes length bytes from Vector to IO resource
 *              defined by Context starting at Offset location.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [in]  VectorIndex     Index of Vector to use.
 * @param [in]  Length          bytes to be read in vector.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 */
static uint64 BulkGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    TL_BulkVectorList_t *BulkVector_p = VectorList[VectorIndex].BulkVector_p;
    uint64 WriteLength = IO_ERROR;
    uint32 Counter = 0;
    uint32 Result = 0;

    if ((Length > (MAX_BULK_TL_PROCESSES *(BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA))) || (VectorIndex >= MAX_BULK_TL_PROCESSES)) {
        B_(printf("io_bulk.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    switch (VectorList[VectorIndex].State) {
    case BULK_IDLE_STATE:

        for (Counter = 0; Counter < BulkVector_p->Buffers; Counter++) {
            SET_PACKET_FLAGS(BulkVector_p->Entries[Counter].Buffer_p, PACKET_TX_STATE_MASK, BUF_TX_READY);
        }

        if (!StartBulkSession(BulkVector_p, Offset)) {
            VectorList[VectorIndex].State = SEND_WRITE_REQUEST;
            WriteLength = IO_WAIT;
        } else {
            // Try to close bulk session before exiting
            (void)CloseBulkSession(BulkVector_p);

            A_(printf("io_bulk.c (%d): ** Could Not start bulk session! **\n", __LINE__);)
            IO_ErrorCode = E_FAILED_TO_START_BULK_SESSION;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        break;

    case SEND_WRITE_REQUEST:

        if (GetStatusBulkSession(BulkVector_p) == BULK_SESSION_FINISHED) {
            VectorList[VectorIndex].State = BULK_IDLE_STATE;

            Result = CloseBulkSession(BulkVector_p);

            if (E_SUCCESS != Result) {
                A_(printf("io_bulk.c (%d): ** Could Not close bulk session! **\n", __LINE__);)
                IO_ErrorCode = E_FAILED_TO_CLOSE_BULK_SESSION;
                WriteLength = IO_ERROR;
            }

            WriteLength = VectorList[VectorIndex].Length;
            break;
        }

        WriteLength = IO_WAIT;
        break;

    default:
        A_(printf("io_bulk.c (%d): ** Unpredicted behavior! **\n", __LINE__);)
        VectorList[VectorIndex].State = BULK_IDLE_STATE;
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        WriteLength = IO_ERROR;
        break;
    }

ErrorExit:
    return WriteLength;
}

/*
 * Returns length in bytes of the area in IO resource
 *              pointed by the Context.
 *
 * @param [in]  Context_p       initialized IO channel context.
 *
 * @retval  Length              if length of IO resource is available
 *
 */
static uint64 BulkLength(const void *const Context_p)
{
    return ((BulkContext_t *)Context_p)->Length;
}

/**     @} */
/** @} */
