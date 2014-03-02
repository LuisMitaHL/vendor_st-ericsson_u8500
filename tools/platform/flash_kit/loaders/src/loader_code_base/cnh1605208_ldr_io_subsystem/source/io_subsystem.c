/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_communication_service.h"
#include "r_r15_transport_layer.h"
#include "r_io_subsystem.h"
#include "r_debug_macro.h"
#include "r_bulk_protocol.h"
#include "io_subsystem.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define CLOSED_ENTRY {0, IOS_CLOSED, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, NULL, (uint64)0, (uint64)0}
#define CLOSED_VECTOR_ENTRY {IOV_UNMAPPED,NULL, NULL}
#define CLOSED_VECTORLIST_ENTRY {IOV_UNMAPPED, 0, 0L, 0L, 0, NULL, 0}

#define VERIFY_OPEN_HANDLE(d)         \
  (((d) < (IO_Entries[0].MaxOpenEntries))  \
  && (IO_Entries[d].State == IOS_OPEN))

IO_Entry_t *IO_Entries; // define IO_Entries[]
IO_VectorList_t *VectorList; // define VectorList[]
ErrorCode_e IO_ErrorCode; //Variable that contain appropriate error code in case of unsuccessful IO operation

/*******************************************************************************
 *   Declaration of file local functions
 ******************************************************************************/
static uint32 VectorUnInit(uint32 Index);

/*******************************************************************************
 *   Declaration of macros
 ******************************************************************************/
#define PREAD(e, b, n, o) (e)->Interface.Read_fn((e)->Context_p, b, n, o)
#define PWRITE(e, b, n, o) (e)->Interface.Write_fn((e)->Context_p, b, n, o)
#define PPREAD(e, b, n, o) (e)->Interface.PRead_fn((e)->Context_p, b, n, o)
#define PPWRITE(e, b, n, o) (e)->Interface.PWrite_fn((e)->Context_p, b, n, o)
#define PREADS(e, b, n, o) (e)->Interface.PReadS_fn((e)->Context_p, b, n, o)
#define PWRITEG(e, b, n, o) (e)->Interface.PWriteG_fn((e)->Context_p, b, n, o)
#define PCLOSE(e) (e)->Interface.Close_fn((e)->Context_p)
#define PSIZE(e) (e)->Interface.Length_fn((e)->Context_p)
#define PCREATEV(e, b, n) (e)->Interface.FCreateV_fn((e)->Context_p, b, n)

#define DestroyBulkVector(x,y)    Do_R15_Bulk_DestroyVector(GlobalCommunication_p,(x),(y))

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * Initializes IO and sets maximal number of concurrent IO channels.
 *
 * This function must be called before any use of IO layer.
 *
 * @param [in] MaxOpenIO        Number of available IO channels.
 *
 * @retval  E_SUCCESS           Layer initialized
 *
 */
ErrorCode_e Do_IO_Init(uint32 MaxOpenIO)
{
    uint32 i, j;
    IO_VectorList_t Init = CLOSED_VECTORLIST_ENTRY;
    IO_VectorEntry_t InitE = CLOSED_VECTOR_ENTRY;
    IO_Entry_t Init1 = CLOSED_ENTRY;

    VectorList = (IO_VectorList_t *)malloc(MaxOpenIO * sizeof(IO_VectorList_t));
    ASSERT(NULL != VectorList);

    IO_Entries = (IO_Entry_t *)malloc(MaxOpenIO * sizeof(IO_Entry_t));
    ASSERT(NULL != IO_Entries);

    for (i = 0; i < MaxOpenIO ; ++i) {
        VectorList[i] = Init;

        for (j = 0; j < MaxOpenIO; ++j) {
            VectorList[i].Entries[j] = InitE;
        }
    }

    for (i = 0; i < MaxOpenIO ; ++i) {
        IO_Entries[i] = Init1;
    }

    IO_Entries[0].MaxOpenEntries = MaxOpenIO;
    return E_SUCCESS;
}

/*
 * Reads Length bytes from IO resource defined by Handle to Buffer.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
uint64 Do_IO_Read(const IO_Handle_t Handle, void *const Buffer_p, const uint64 BufferLength, uint64 Length)
{
    uint64 BytesRead = 0;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BufferLength, Length);

    if (IO_Entries[Handle].Interface.Read_fn != NULL) {
        BytesRead = PREAD(&IO_Entries[Handle], Buffer_p, Length, IO_Entries[Handle].ReadPosition);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

    if (BytesRead != IO_ERROR) {
        IO_Entries[Handle].ReadPosition += BytesRead;
    }

ErrorExit:
    return BytesRead;
}

/*
 * Writes Length bytes from Buffer to
 *                IO resource defined by Handle.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
uint64 Do_IO_Write(const IO_Handle_t Handle, const void *const Buffer_p, const uint64 BufferLength, uint64 Length)
{
    uint64 BytesWritten = 0;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BufferLength, Length);

    if (IO_Entries[Handle].Interface.Write_fn != NULL) {
        BytesWritten = PWRITE(&IO_Entries[Handle], Buffer_p, Length, IO_Entries[Handle].WritePosition);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

    if (BytesWritten != IO_ERROR) {
        IO_Entries[Handle].WritePosition += BytesWritten;
    }

ErrorExit:
    return BytesWritten;
}

/**
 * Reads length bytes from IO resource defined by Handle
 *              starting at Offset location to Buffer.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          of area to be read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
uint64 Do_IO_PositionedRead(const IO_Handle_t Handle, void *const Buffer_p, const uint64 BufferLength, uint64 Length, const uint64 Offset)
{
    uint64 BytesRead = 0;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BufferLength, Length);

    if (IO_Entries[Handle].Interface.PRead_fn != NULL) {
        BytesRead = PPREAD(&IO_Entries[Handle], Buffer_p, Length, Offset);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

ErrorExit:
    return BytesRead;
}

/*
 * Writes length bytes from Buffer to IO resource
 *              defined by Handle starting at Offset location.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be written in IO resource.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
uint64 Do_IO_PositionedWrite(const IO_Handle_t Handle, const void *const Buffer_p, const uint64 BufferLength, uint64 Length, const uint64 Offset)
{
    uint64 BytesWritten = 0;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BufferLength, Length);

    if (IO_Entries[Handle].Interface.PWrite_fn != NULL) {
        BytesWritten = PPWRITE(&IO_Entries[Handle], Buffer_p, Length, Offset);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

ErrorExit:
    return BytesWritten;
}

/*
 * Reads length bytes from IO resource defined by Handle
 *              starting at Offset location to Vector.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [in]  VectorHandle    Handle of Vector space to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area that is read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 * @note VectorList[VectorHandle].IOffset is the offset of valid data in IO_Vector
 */
uint64 Do_IO_ScatteredPositionedRead(const IO_Handle_t Handle, const uint32 VectorHandle, uint64 Length, const uint64 Offset)
{
    uint64 BytesRead;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

    if (IO_Entries[Handle].Interface.PReadS_fn != NULL) {
        BytesRead = PREADS(&IO_Entries[Handle], VectorHandle, Length, Offset);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        goto ErrorExit;
    }

ErrorExit:
    return BytesRead;
}

/*
 * Writes length bytes from Vector to IO resource
 *              defined by Handle starting at Offset location.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [in]  VectorHandle    Handle of Vector space to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
uint64 Do_IO_GatheredPositionedWrite(const IO_Handle_t Handle, const uint32 VectorHandle, uint64 Length, const uint64 Offset)
{
    uint64 BytesWritten;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

    if (IO_Entries[Handle].Interface.PWriteG_fn != NULL) {
        BytesWritten = PWRITEG(&IO_Entries[Handle], VectorHandle, Length, Offset);
    } else {
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesWritten = IO_ERROR;
        goto ErrorExit;
    }

ErrorExit:
    return BytesWritten;
}

/*
 * Closes IO channel with given Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 *
 * @retval  E_SUCCESS           if IO channel closed,
 * @retval  IO_INVALID_HANDLE   if invalid Handle is passed to function,
 *                              or resource is already closed.
 *
 */
uint32 Do_IO_Close(const IO_Handle_t Handle)
{
    uint32 Status;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        Status = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    if (IO_Entries[Handle].Interface.Close_fn != NULL) {
        Status = PCLOSE(&IO_Entries[Handle]);
    } else {
        Status = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    IO_Entries[Handle].State = IOS_CLOSED;

ErrorExit:
    return Status;
}

/*
 * Returns length in bytes of the area in IO resource
 *              pointed by the Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined Length function,
 * @retval  Length              if length of IO resource is available
 *
 */
uint64 Do_IO_GetLength(const IO_Handle_t Handle)
{
    uint64 Result = 0;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        Result = IO_ERROR;
        goto ErrorExit;
    }

    if (IO_Entries[Handle].Interface.Length_fn != NULL) {
        Result = PSIZE(&IO_Entries[Handle]);
    }

ErrorExit:
    return Result;
}

/*
 * Creates IO vector.
 *
 * IO Vector is a space of underlying buffers
 * referenced by unique Vector Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 * @param [in] Length           in bytes for Vector space.
 * @param [in] BuffSize         Size of each underlying buffer.
 *
 * @retval   IO_VECTOR_INVALID_HANDLE   if invalid Handle is passed,
 * @retval   E_INVALID_INPUT_PARAMETERS if input parameters are
 *                                      out of set limits,
 * @retval   E_VECTOR_CREATE_FAIL       if there is no defined
 *                                      CreateVector function,
 * @retval   VectorHandle               if all Buffers for the Vector
 *                                      are allocated and reserved.
 */
uint32 Do_IO_CreateVector(const IO_Handle_t Handle, uint64 Length, const uint64 BuffSize)
{
    uint32 Result = IO_VECTOR_INVALID_HANDLE;

    if (!VERIFY_OPEN_HANDLE(Handle)) {
        Result = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    if (BuffSize == 0) {
        Result = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    if (IO_Entries[Handle].Interface.FCreateV_fn != NULL) {
        Result = PCREATEV(&IO_Entries[Handle], Length, BuffSize);
        goto ErrorExit;
    } else {
        Result =  E_VECTOR_CREATE_FAIL;
        goto ErrorExit;
    }

ErrorExit:
    return Result;
}

/*
 * Destroys IO vector.
 *
 * @param [in] VectorHandle     Handle of IO Vector to destroy.
 *
 * @retval   E_VECTOR_DESTROY_FAIL      if there is no defined
 *                                      DestroyVector function,
 * @retval   E_SUCCESS                  if all Buffers for the Vector
 *                                      are freed.
 */
uint32 Do_IO_DestroyVector(const uint32 Index)
{
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint32 Result;
    uint32 ParentVector = VectorList[Index].CachedData.ParentVector;

    if (Index >= MaxOpenIO) {
        A_(printf("io_subsystem.c (%d): Invalid input parameters!\n", __LINE__);)
        Result = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    if ((IOV_VALID == VectorList[ParentVector].Status) && (FALSE == VectorList[ParentVector].VectorProtected)) {
        Result = VectorUnInit(ParentVector);

    }

    if (TRUE == VectorList[Index].VectorProtected) {
        B_(printf("io_subsystem.c (%d): VECTOR (%d) is PROTECTED!\n", __LINE__, Index);)
        return (uint32)E_SUCCESS;
    }

    Result = VectorUnInit(Index);


ErrorExit:
    return Result;
}

/*
 * Calculates best values for IO Vector VectorSize and ChunkSize
 *
 * @param [out] VectorLength_p          address of VectorLength
 * @param [out] ChunkSize_p             address of ChunkSize
 * @param [in]  RequestedVectorLength   data space to be allocated in vector
 * @param [in]  IsProcess
 */
void Do_IO_CalculateVectorParameters(uint32 *const VectorLength_p,
                                     uint32 *const ChunkSize_p,
                                     uint32 PageSize,
                                     const uint64 RequestedVectorLength,
                                     boolean IsProcess)
{
    uint32 ChunkSize;
    uint32 VectorLength;
    uint32 NecessaryBuffers;

    if (0 == PageSize) {
        PageSize = 4;
    }

    ChunkSize = BULK_BUFFER_SIZE - BULK_BUFFER_REDUNDANT_DATA;
    ChunkSize = (ChunkSize / PageSize) * PageSize;

    if (IsProcess) {
        VectorLength = OPTIMAL_NR_OF_BUFFERS_PROCESS * ChunkSize;
    } else {
        VectorLength = OPTIMAL_NR_OF_BUFFERS_DUMP * ChunkSize;
    }


    if (VectorLength > RequestedVectorLength) {
        VectorLength = RequestedVectorLength;
        NecessaryBuffers = (VectorLength + ChunkSize - 1) / ChunkSize;
        ChunkSize = (VectorLength + NecessaryBuffers - 1) / NecessaryBuffers;
        ChunkSize = ((ChunkSize + PageSize - 1) / PageSize) * PageSize;
    }

    *ChunkSize_p = ChunkSize;
    *VectorLength_p = VectorLength;
}

/*
 * Copies Buffer to Vector.
 *
 * @param [in] VectorIndex   Handle of initialized empty IO Vector
 *                           with at least Buffers in it.
 * @param [in] Buffers       Number of buffers in vector that will provide
 *                           enough space to copy all data from buffer.
 * @param [in] Buffer_p      Pointer to buffer filed with data.
 */
void Do_IO_CopyBufferToVector(const uint32 VectorIndex, const void *const Buffer_p, const uint32 Buffers)
{
    uint32 Counter = 0;
    uint32 Length = 0;

    if ((VectorIndex < IO_Entries[0].MaxOpenEntries) && (Buffer_p != NULL)) {
        for (Counter = 0; Counter < Buffers; Counter++) {
            memcpy(VectorList[VectorIndex].Entries[Counter].Buffer_p,
                   (uint8 *)Buffer_p + Length,
                   VectorList[VectorIndex].ChunkSize);

            Length += VectorList[VectorIndex].ChunkSize;
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
        }
    }
}

/*
 * Copies Vector to Buffer.
 *
 * @param [in] VectorIndex   Handle of initialized IO Vector
 *                           with at least Buffers in it, containing all data.
 * @param [in] Length        Data Length that needs to be coppied
 * @param [in] Buffer_p      Pointer to buffer that is big enough to accept
 *                           all data.
 *
 * @retval   E_INVALID_INPUT_PARAMETERS  if invalid parameters are passed to
 *                                       function
 * @retval   E_SUCCESS                   if all bytes are successfuly transfered
 *                                       from Vector to bufer.
 */
ErrorCode_e Do_IO_CopyVectorToBuffer(void *const Buffer_p, const uint32 VectorIndex, uint64 Length)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 BytesToWrite = 0;
    uint64 Position = 0;
    uint64 Offset = 0;
    uint32 Counter = 0;

    //
    // Sanity check of input parameters
    //
    VERIFY(VectorIndex < MAX_BULK_TL_PROCESSES, E_INVALID_INPUT_PARAMETERS);
    VERIFY(Buffer_p != NULL, E_INVALID_INPUT_PARAMETERS);

    //
    // Write cached data first (if any)
    //
    if (TRUE == VectorList[VectorIndex].CachedData.DataValid) {
        uint64 CachedDataChunkSize = VectorList[VectorIndex].CachedData.ChunkSize;
        uint64 CachedDataLength = VectorList[VectorIndex].CachedData.CachedDataLength;

        Offset = VectorList[VectorIndex].CachedData.IOffset;

        //loop until all cached data are written
        for (Counter = 0; ((Counter < MAX_OPEN_IO) && (Length > 0)); Counter++) {
            if (Offset >= CachedDataChunkSize) {
                Offset -= CachedDataChunkSize;
                continue;
            }

            BytesToWrite = MIN(CachedDataLength, (CachedDataChunkSize - Offset));

            if (Length < BytesToWrite) {
                BytesToWrite = Length;
            }

            memcpy((uint8 *)Buffer_p + Position, (uint8 *)(VectorList[VectorIndex].CachedData.Entries[Counter].Buffer_p) + Offset, BytesToWrite);

            CachedDataLength -= BytesToWrite;
            Position += BytesToWrite;
            Length -= BytesToWrite;
            Offset = 0;

            if ((0 == CachedDataLength) || (0 == Length)) {
                B_(printf("io_subsystem.c (%d): All cached data successfully written.\n", __LINE__);)
                break;
            }
        }
    }

    //
    // If more data need to be written
    //
    if (0 != Length) {
        uint64 ChunkSize = VectorList[VectorIndex].ChunkSize;
        uint64 DataLength = VectorList[VectorIndex].Length;

        Offset = VectorList[VectorIndex].IOffset;
        VERIFY(Offset < DataLength, E_INVALID_INPUT_PARAMETERS);
        VERIFY(Length <= (DataLength - Offset), E_INVALID_INPUT_PARAMETERS);

        //loop until all cached data are written
        for (Counter = 0; ((Counter < MAX_OPEN_IO) && (Length > 0)); Counter++) {
            if (Offset >= ChunkSize) {
                Offset -= ChunkSize;
                DataLength -= ChunkSize;
                continue;
            }

            BytesToWrite = MIN(DataLength, (ChunkSize - Offset));

            if (Length < BytesToWrite) {
                BytesToWrite = Length;
            }

            memcpy((uint8 *)Buffer_p + Position, (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + Offset, BytesToWrite);

            DataLength -= BytesToWrite;
            Position += BytesToWrite;
            Length -= BytesToWrite;
            Offset = 0;
        }
    }

    C_(printf("io_subsystem.c(%d): %llu bytes successfuly transfered from vector to buffer.\n", __LINE__, Position);)
    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * Get number of max open entries in same time.
 *
 * @param [in] none
 *
 * @return     Number of maximum allowed entries to ne opened in one time.
 */
uint32 Do_IO_GetNrOfMaxOpenEntry(void)
{
    return IO_Entries[0].MaxOpenEntries;
}

/*******************************************************************************
 * Definition of module internal functions
 ******************************************************************************/
/*
 * Returns initialized Handle to IO channel if there is
 * at least one available.
 *
 * Before calling this function you have to initialize IO interface
 * and allocate space for IO context.
 *
 * This function is used by data source Open functions
 * to open IO Channel with defined Interface functions
 * and data context specific for the data source. This
 * way we abstract the uniqueness of every data source to
 * the generality of IO Layer.
 *
 * @param [in] Interface_p     Pointer to initialized IO Interface.
 * @param [in] Context_p       Pointer to initialized IO Context.
 *
 * @retval  IO_Handle          Initialized IO handle if IO channel is available,
 * @retval  IO_ERROR           if there are no more IO channels.
 *
 */
IO_Handle_t IO_Create(const IO_Interface_t *Interface_p, void *Context_p)
{
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint32 i = 0;

    for (i = 0; i < MaxOpenIO; i++) {
        if (IO_Entries[i].State == IOS_CLOSED) {
            IO_Entries[i].State = IOS_OPEN;
            IO_Entries[i].Interface = *Interface_p;
            IO_Entries[i].Context_p = Context_p;
            IO_Entries[i].ReadPosition = 0;
            IO_Entries[i].WritePosition = 0;
            return (IO_Handle_t) i;
        }
    }

    return IO_INVALID_HANDLE;
}

/*
 * Returns IO vector index of the first fit
 *
 * @param [in] BuffersNeeded    for the IO Channel.
 *
 * @retval  VectorIndex    if free IO vector with defined resources is available
 * @retval  IO_VECTOR_INVALID_HANDLE     if there is no free IO vector,
 *
 */
uint32 IO_FindFreeVector(const uint32 BuffersNeeded)
{
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint32 AvailableBuffers = 0;
    uint32 VectorCounter = 0;
    uint32 Counter = 0;

    for (VectorCounter = 0; (VectorCounter < MaxOpenIO) && (AvailableBuffers == 0); VectorCounter++) {
        if (VectorList[VectorCounter].Status == IOV_UNMAPPED) {
            AvailableBuffers = 0;

            for (Counter = 0; Counter < MaxOpenIO; Counter++) {
                if (VectorList[VectorCounter].Entries[Counter].Status == IOV_UNMAPPED) {
                    AvailableBuffers++;
                }
            }
        }
    }

    if (AvailableBuffers > 0) {
        VectorCounter--;
    }

    if (AvailableBuffers >= BuffersNeeded) {
        memset((void *)&VectorList[VectorCounter], 0x00, sizeof(IO_VectorList_t));
        return VectorCounter;
    } else {
        return IO_VECTOR_INVALID_HANDLE;
    }
}

/*
 *  Uninitialize Bulk Vector
 *
 *  @param [in] Index           Bulk Vector Index
 *
 *  @retval ErrorCode_e         Error Exit code
 */
static uint32 VectorUnInit(uint32 Index)
{
    uint32 Result = E_GENERAL_FATAL_ERROR;
    uint32 Counter = 0;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    VectorList[Index].Status = IOV_UNMAPPED;
    VectorList[Index].IOffset = 0;
    VectorList[Index].Length = 0;
    VectorList[Index].ChunkSize = 0;

    memset(&VectorList[Index].CachedData, 0x00, sizeof(VectorCachedData_t));

    for (Counter = MaxOpenIO; Counter != 0 ;) {
        Counter--;

        if (VectorList[Index].Entries[Counter].Status != IOV_UNMAPPED) {
            VectorList[Index].Entries[Counter].Status = IOV_UNMAPPED;

            if (VectorList[Index].Entries[Counter].Buffer_p != NULL) {
                if (!VectorList[Index].BulkTransfer) {
                    free(VectorList[Index].Entries[Counter].Buffer_p);
                }

                VectorList[Index].Entries[Counter].Buffer_p = NULL;
            }

            if (VectorList[Index].Entries[Counter].Hash_p != NULL) {
                if (!VectorList[Index].BulkTransfer) {
                    free(VectorList[Index].Entries[Counter].Hash_p);
                }

                VectorList[Index].Entries[Counter].Hash_p = NULL;
            }
        }
    }

    Result = E_SUCCESS;

    if (VectorList[Index].BulkTransfer) {
        Result = DestroyBulkVector(VectorList[Index].BulkVector_p, TRUE);
    }

    if (Result != E_SUCCESS) {
        goto ErrorExit;
    }

    VectorList[Index].BulkTransfer = 0;
    VectorList[Index].BulkVector_p = NULL;

ErrorExit:
    return Result;
}

/** @} */
