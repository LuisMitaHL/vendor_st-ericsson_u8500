/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_Secure_Wrap
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
#include "error_codes.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "r_loader_sec_lib.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint32 SecureWrapClose(void *Context_p);
static uint64 SecureWrapRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 SecureWrapWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 SecureWrapLength(const void *const Context_p);
static uint64 SecureWrapPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 SecureWrapPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 SecureWrapScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 SecureWrapGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint32 SecureWrapCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);
static boolean VerifySecureWrapVector(const SecureWrapContext_t *const SecureWrapContext_p, const uint32 VectorIndex, uint64 StartBlockOffset);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  This type defines functions for Wrap IO Interface
 */
static const IO_Interface_t SecureWrapInterface = {
    SecureWrapClose,
    SecureWrapRead,
    SecureWrapWrite,
    SecureWrapPositionedRead,
    SecureWrapPositionedWrite,
    SecureWrapScatteredPositionedRead,
    SecureWrapGatheredPositionedWrite,
    SecureWrapLength,
    SecureWrapCreateVector,
};

/*
 *  This type defines container that hold cached data information
 *  in Secure Wrap. It also contain additional parameters that
 *  are necessary for appropriate data handling.
 */
typedef struct {
    VectorCachedData_t VectorCachedData; /**< Cached data structure */
    uint64 OffsetInParent;               /**< Offset in parent*/
} SecureWrapCachedData_t;

/*
 * Global variable that hold cached data.
 */
static SecureWrapCachedData_t SecureWrapCachedData = {{0}};

/*
 * Global variable that hold information about current read
 * position in file that is transfered.
 */
static uint64 FileReadPosition = 0;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens secure wrap around IO resource and returns Handle to it.
 *
 *  @param [in] Parent          IO handle of Parent.
 *  @param [in] Length          in bytes in Parent resource.
 *  @param [in] Offset          in Parent resource.
 *  @param [in] VerifyContext_p Verification Context to use when
 *                              calling security functions
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a secure wrap.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for secure wrap
 *                              context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_SecureWrapOpen(const IO_Handle_t Parent, const uint64 Length, const uint64 Offset, VerifyContext_t *VerifyContext_p)
{
    static SecureWrapContext_t *SecureWrapContext_p = NULL;
    IO_Handle_t Handle;
    uint64 ParentLength;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint64 HashedBlockSize = 0;
    LoaderSecLib_Output_t SeclibReturnValue = LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE;

    C_(printf("io_secure_wrap.c (%d): Do_IO_SecureWrapOpen\n", __LINE__);)

    if (NULL == VerifyContext_p) {
        A_(printf("io_secure_wrap.c (%d): Do_IO_SecureWrapOpen: **ERROR** No VerifyContext supplied\n", __LINE__);)
        return IO_INVALID_HANDLE;
    }

    if (Parent >= MaxOpenIO) {
        A_(printf("io_secure_wrap.c (%d): Do_IO_SecureWrapOpen: **ERROR** Handle index out of the rang\n", __LINE__);)
        return IO_INVALID_HANDLE;
    }

    ParentLength = Do_IO_GetLength(Parent);

    if (ParentLength == IO_ERROR) {
        A_(printf("io_secure_wrap.c (%d): Do_IO_SecureWrapOpen: **ERROR** Getting file length\n", __LINE__);)
        return IO_INVALID_HANDLE;
    }

    SeclibReturnValue = Do_LoaderSecLib_Get_HashlistBlockSize(VerifyContext_p, &HashedBlockSize);

    if (SeclibReturnValue != LOADER_SEC_LIB_SUCCESS) {
        return IO_INVALID_HANDLE;
    }

    C_(printf("io_secure_wrap.c (%d): Do_IO_SecureWrapOpen: Hashed Block Size = 0x%lx\n", __LINE__, HashedBlockSize);)

    SecureWrapContext_p = malloc(sizeof(SecureWrapContext_t));
    ASSERT(NULL != SecureWrapContext_p);

    SecureWrapContext_p->Parent          = Parent;
    SecureWrapContext_p->OffsetInParent  = Offset;
    SecureWrapContext_p->ReadPosition    = 0;
    SecureWrapContext_p->WritePosition   = 0;
    SecureWrapContext_p->Length          = MIN(Length, ParentLength - Offset);
    SecureWrapContext_p->State           = SECURE_WRAP_IDLE;
    SecureWrapContext_p->VectorId        = 0;
    SecureWrapContext_p->HashedBlockSize = HashedBlockSize;
    SecureWrapContext_p->VerifyContext   = VerifyContext_p;

    Handle = IO_Create(&SecureWrapInterface, SecureWrapContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(SecureWrapContext_p);
        SecureWrapContext_p = NULL;
    }

    return Handle;
}

/*
 * Initialize data caching.
 *
 * @return   none
 *
 * @note This function is usable only in Flash Process File
 *       function.
 */
void Do_IOW_SecureWrap_InitializeDataCaching(void)
{
    memset(&SecureWrapCachedData, 0x00, sizeof(SecureWrapCachedData_t));
}

/*
 * Destroy cached data.
 *
 * @return   IO_CACHED_DATA_DESTROY_FAIL - in case when parent vector cannot
 *                                          be destroyed.
 *
 * @return   E_SUCCESS - in case when cached data are successfuly destroyed
 *
 */
uint32 Do_IOW_SecureWrap_DestroyCachedData(void)
{
    uint32 ParentVector = 0;
    uint32 Result = 0;

    if (TRUE == SecureWrapCachedData.VectorCachedData.DataValid) {
        ParentVector = SecureWrapCachedData.VectorCachedData.ParentVector;
        VectorList[ParentVector].VectorProtected = FALSE;

        Result = Do_IO_DestroyVector(ParentVector);

        if (0 != Result) {
            Result = IO_CACHED_DATA_DESTROY_FAIL;
            A_(printf("io_secure_wrap.c(%d): Do_IOW_SecureWrap_DestroyCachedData() failed to destroy parent vector!!!\n", __LINE__);)
        }
    }

    FileReadPosition = 0;
    memset(&SecureWrapCachedData, 0x00, sizeof(SecureWrapCachedData_t));

    return Result;
}

/*
 * Update internal parameter in Secure Wrap that hold information
 * about current (relative) read possition in file that is transfered.
 * Function is called in case when progress data are found during
 * fail save reflashing in process file command.
 *
 * @param [in] NewReadPosition        of file that should be transfered.
 *
 * @return none.
 */
void Do_IOW_SecureWrap_UpdateInternalParameters(uint32 NewReadPosition)
{
    FileReadPosition = NewReadPosition;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Creates IO vector.
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
static uint32 SecureWrapCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    IDENTIFIER_NOT_USED(BuffSize);
    SecureWrapContext_t *SecureWrapContext_p = (SecureWrapContext_t *) Context_p;
    uint32 VectorIndex = IO_INVALID_HANDLE;
    uint64 CalculatedSecureWrapLength = 0;
    uint64 ParentLength = 0;
    uint64 StartOffsetAberration = 0;
    uint64 UsableCachedData = 0;
    uint64 RequestedOffset = 0;
    uint64 OffsetInCashBuffer = 0;
    uint64 StartOffset = 0;
    boolean CachedDataFound = FALSE;

    ParentLength = Do_IO_GetLength(SecureWrapContext_p->Parent);

    if (ParentLength == IO_ERROR) {
        A_(printf("io_secure_wrap.c (%d):SecureWrapCreateVector: **ERROR** Getting file length\n", __LINE__);)
        return IO_INVALID_HANDLE;
    }

    RequestedOffset = FileReadPosition + SecureWrapContext_p->OffsetInParent;

    if (RequestedOffset < SecureWrapCachedData.OffsetInParent) {
        /*
         * Caching is designed to work with increasing offset.
         * If Requested offset is before cached data, destroy it.
         */
        (void)Do_IOW_SecureWrap_DestroyCachedData();
    }

    //update FileReadPosition internal parameter
    if (SecureWrapContext_p->Length == (FileReadPosition + Length)) {
        FileReadPosition = 0;
    } else {
        FileReadPosition += Length;
    }

    //Check if some of required data are already cached.
    if ((TRUE == SecureWrapCachedData.VectorCachedData.DataValid) &&
    (RequestedOffset >= SecureWrapCachedData.OffsetInParent) &&
    (RequestedOffset < SecureWrapCachedData.OffsetInParent + SecureWrapCachedData.VectorCachedData.CachedDataLength)) {
        UsableCachedData = (SecureWrapCachedData.OffsetInParent + SecureWrapCachedData.VectorCachedData.CachedDataLength) - RequestedOffset;

        if (UsableCachedData >= Length) {
            uint32 NrOfBuffers = SecureWrapCachedData.VectorCachedData.Buffers;
            uint32 Counter = 0;

            B_(printf("io_secure_wrap.c(%d): All required data was already cached!!!\n", __LINE__);)

            VectorIndex = IO_FindFreeVector(1);

            if (VectorIndex >= MAX_OPEN_IO) {
                A_(printf("io_secure_wrap.c(%d): Cannot find free vector!!!\n", __LINE__);)
                VectorIndex = IO_VECTOR_INVALID_HANDLE;
                goto ErrorExit;
            }

            for (Counter = 0; ((Counter < MAX_OPEN_IO) && (NrOfBuffers > 0)); Counter++) {
                VectorList[VectorIndex].CachedData.Entries[Counter].Buffer_p = SecureWrapCachedData.VectorCachedData.Entries[Counter].Buffer_p;
                NrOfBuffers--;
            }

            VectorList[VectorIndex].CachedData.CachedDataLength = Length;
            VectorList[VectorIndex].CachedData.IOffset = RequestedOffset - SecureWrapCachedData.OffsetInParent;
            VectorList[VectorIndex].CachedData.ChunkSize = SecureWrapCachedData.VectorCachedData.ChunkSize;
            VectorList[VectorIndex].CachedData.Buffers = NrOfBuffers;
            VectorList[VectorIndex].CachedData.ParentVector = SecureWrapCachedData.VectorCachedData.ParentVector;
            VectorList[VectorIndex].CachedData.DataValid = TRUE;
            VectorList[VectorIndex].Status = IOV_MAPPED;
            return VectorIndex;
        } else {
            OffsetInCashBuffer = RequestedOffset - SecureWrapCachedData.OffsetInParent;
            RequestedOffset += UsableCachedData;
            Length -= UsableCachedData;
            CachedDataFound = TRUE;
        }
    }

    StartOffsetAberration = RequestedOffset % (SecureWrapContext_p->HashedBlockSize);

    if (((StartOffsetAberration + Length) % (SecureWrapContext_p->HashedBlockSize)) == 0) {
        CalculatedSecureWrapLength = StartOffsetAberration + Length;
    } else {
        CalculatedSecureWrapLength = (((StartOffsetAberration + Length) / (SecureWrapContext_p->HashedBlockSize)) + 1) * (SecureWrapContext_p->HashedBlockSize);
    }

    if (CalculatedSecureWrapLength < (OPTIMAL_NR_OF_BUFFERS_PROCESS *(SecureWrapContext_p->HashedBlockSize))) {
        CalculatedSecureWrapLength = OPTIMAL_NR_OF_BUFFERS_PROCESS * (SecureWrapContext_p->HashedBlockSize);
    }

    StartOffset = RequestedOffset - StartOffsetAberration;
    Length = MIN((ParentLength - StartOffset), CalculatedSecureWrapLength);

    VectorIndex = Do_IO_CreateVector(SecureWrapContext_p->Parent, Length, (SecureWrapContext_p->HashedBlockSize));

    if ((IO_INVALID_HANDLE != VectorIndex) && (TRUE == CachedDataFound)) {
        uint32 Counter = 0;
        uint32 NrOfBuffers = SecureWrapCachedData.VectorCachedData.Buffers;

        for (Counter = 0; ((Counter < MAX_OPEN_IO) && (NrOfBuffers > 0)); Counter++) {
            VectorList[VectorIndex].CachedData.Entries[Counter].Buffer_p = SecureWrapCachedData.VectorCachedData.Entries[Counter].Buffer_p;
            NrOfBuffers--;
        }

        VectorList[VectorIndex].CachedData.IOffset = OffsetInCashBuffer;
        VectorList[VectorIndex].CachedData.CachedDataLength = SecureWrapCachedData.VectorCachedData.CachedDataLength - OffsetInCashBuffer;
        VectorList[VectorIndex].CachedData.ChunkSize = SecureWrapCachedData.VectorCachedData.ChunkSize;
        VectorList[VectorIndex].CachedData.Buffers = NrOfBuffers;
        VectorList[VectorIndex].CachedData.ParentVector = SecureWrapCachedData.VectorCachedData.ParentVector;
        VectorList[VectorIndex].CachedData.DataValid = TRUE;
    }

ErrorExit:

    if ((FALSE == CachedDataFound) && (TRUE == SecureWrapCachedData.VectorCachedData.DataValid)) {
        uint32 ParentVector = SecureWrapCachedData.VectorCachedData.ParentVector;
        VectorList[ParentVector].VectorProtected = FALSE;
        memset(&SecureWrapCachedData, 0x00, sizeof(SecureWrapCachedData_t));
    }

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
static uint32 SecureWrapClose(void *Context_p)
{
    BUFFER_FREE(Context_p);
    return E_SUCCESS;
}

/*
 * Reads Length bytes from IO resource defined by Context to Buffer.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          Offset in buffer space.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.r.
 */
static uint64 SecureWrapRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    A_(printf("io_secure_wrap.c (%d): ** Unsupported functionality! **\n", __LINE__);)
    IDENTIFIER_NOT_USED(Context_p);
    IDENTIFIER_NOT_USED(Buffer_p);
    IDENTIFIER_NOT_USED(Length);
    IDENTIFIER_NOT_USED(Offset);
    IO_ErrorCode = E_GENERAL_IO_ERROR;
    return IO_ERROR;
}

/*
 * Writes Length bytes from Buffer to
 *                IO resource defined by Context.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          Offset in buffer space.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 SecureWrapWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    A_(printf("io_secure_wrap.c (%d): ** Unsupported functionality! **\n", __LINE__);)
    IDENTIFIER_NOT_USED(Context_p);
    IDENTIFIER_NOT_USED(Buffer_p);
    IDENTIFIER_NOT_USED(Length);
    IDENTIFIER_NOT_USED(Offset);
    IO_ErrorCode = E_GENERAL_IO_ERROR;
    return IO_ERROR;
}

/*
 * Reads length bytes from IO resource defined by Context
 *              starting at Offset location to Buffer.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area to be read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 SecureWrapPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    SecureWrapContext_t *SecureWrapContext_p = (SecureWrapContext_t *) Context_p;
    uint32 VectorIndex = SecureWrapContext_p->VectorId;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    static uint64 CalculatedSecureWrapLength = 0;
    static uint64 SecureWrapStartOffset = 0;
    uint64 SecureWrapEndOffset = 0;
    uint64 ReadLength = 0;
    uint64 RequestedStartOffset = 0;
    uint64 ParentLength = 0;
    uint64 EndOffsetAberration = 0;

    switch (SecureWrapContext_p->State) {
    case SECURE_WRAP_IDLE:
        ParentLength = Do_IO_GetLength(SecureWrapContext_p->Parent);

        if (ParentLength == IO_ERROR) {
            A_(printf("io_secure_wrap.c (%d):SecureWrapPositionedRead: **ERROR** Getting file length\n", __LINE__);)
            IO_ErrorCode = E_IO_FAILED_TO_GET_LENGTH;
            ReadLength = IO_ERROR;
            break;
        }

        RequestedStartOffset = Offset + SecureWrapContext_p->OffsetInParent;
        SecureWrapStartOffset = (RequestedStartOffset / (SecureWrapContext_p->HashedBlockSize)) * (SecureWrapContext_p->HashedBlockSize);

        //Check if requested end offset fit to hashed block border
        EndOffsetAberration = (Length + RequestedStartOffset) % (SecureWrapContext_p->HashedBlockSize);

        if (EndOffsetAberration == 0) {
            SecureWrapEndOffset = Length + RequestedStartOffset;
        } else {
            SecureWrapEndOffset = ((Length + RequestedStartOffset) - EndOffsetAberration) + (SecureWrapContext_p->HashedBlockSize);
        }

        //in case when calculated end offset go beyond parent length
        SecureWrapEndOffset = MIN(SecureWrapEndOffset, ParentLength);
        CalculatedSecureWrapLength = SecureWrapEndOffset - SecureWrapStartOffset;

        VectorIndex = Do_IO_CreateVector(SecureWrapContext_p->Parent, CalculatedSecureWrapLength, (SecureWrapContext_p->HashedBlockSize));

        if (VectorIndex != IO_VECTOR_INVALID_HANDLE) {
            SecureWrapContext_p->VectorId = VectorIndex;
            VectorList[VectorIndex].IOffset = RequestedStartOffset - SecureWrapStartOffset;
            SecureWrapContext_p->State = SECURE_WRAP_PROCESSING;
            /* fall through */
        } else {
            IO_ErrorCode = E_VECTOR_CREATE_FAIL;
            ReadLength = IO_ERROR;
            break;
        }

        //lint -fallthrough
    case SECURE_WRAP_PROCESSING:
        ReadLength = Do_IO_ScatteredPositionedRead(SecureWrapContext_p->Parent, VectorIndex, CalculatedSecureWrapLength, SecureWrapStartOffset);

        if (ReadLength > 0) {
            if ((ReadLength != IO_ERROR) && (ReadLength == CalculatedSecureWrapLength)) {
                if (!VerifySecureWrapVector(SecureWrapContext_p, VectorIndex, SecureWrapStartOffset)) {
                    A_(printf("io_secure_wrap.c (%d): **ERROR** Vector verifications failed! Not Correct Archive!\n", __LINE__);)
                    IO_ErrorCode = E_LOADER_SEC_LIB_VERIFY_FAILURE;
                    ReadLength = IO_ERROR;
                    break;
                }

                if (E_SUCCESS != Do_IO_CopyVectorToBuffer(Buffer_p, VectorIndex, Length)) {
                    IO_ErrorCode = E_GENERAL_IO_ERROR;
                    ReadLength = IO_ERROR;
                    break;
                }

                ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(VectorIndex);

                if (E_SUCCESS != ReturnValue) {
                    IO_ErrorCode = E_VECTOR_DESTROY_FAIL;
                    ReadLength = IO_ERROR;
                    break;
                }

                ReadLength = Length;
                SecureWrapContext_p->State = SECURE_WRAP_IDLE;
                CalculatedSecureWrapLength = 0;
                SecureWrapStartOffset = 0;
            }
        }

        break;

    default:
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        ReadLength = IO_ERROR;
        break;
    }

    //reset static variables before final exit
    if (ReadLength == IO_ERROR) {
        CalculatedSecureWrapLength = 0;
        SecureWrapStartOffset = 0;
    }

    return ReadLength;
}

/*
 * Writes length bytes from Buffer to IO resource
 *              defined by Context starting at Offset location.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be written in IO resource.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
static uint64 SecureWrapPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    A_(printf("io_secure_wrap.c (%d): ** Unsupported functionality! **\n", __LINE__);)
    IDENTIFIER_NOT_USED(Context_p);
    IDENTIFIER_NOT_USED(Buffer_p);
    IDENTIFIER_NOT_USED(Length);
    IDENTIFIER_NOT_USED(Offset);
    IO_ErrorCode = E_GENERAL_IO_ERROR;
    return IO_ERROR;
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
static uint64 SecureWrapScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    SecureWrapContext_t *SecureWrapContext_p = (SecureWrapContext_t *) Context_p;
    static uint64 SecureWrapStartOffset = 0;
    static uint64 StartOffsetAberration = 0;
    uint64 RequestedStartOffset = 0;
    uint64 BytesRead = 0;
    uint32 NrOfBuffers = 0;
    uint32 Counter = 0;

    switch (SecureWrapContext_p->State) {
    case SECURE_WRAP_IDLE:
        RequestedStartOffset = Offset + SecureWrapContext_p->OffsetInParent;

        //If data cashing is initialized and valid cached data exist in vector
        if (TRUE == VectorList[VectorIndex].CachedData.DataValid) {
            if (Length <= VectorList[VectorIndex].CachedData.CachedDataLength) {
                B_(printf("io_secure_wrap.c(%d): All data placed in cash buffers!!!\n", __LINE__);)
                BytesRead = Length;
                break;
            }

            RequestedStartOffset += VectorList[VectorIndex].CachedData.CachedDataLength;
        }

        StartOffsetAberration = RequestedStartOffset % (SecureWrapContext_p->HashedBlockSize);

        if (0 == StartOffsetAberration) {
            //requested start offset is aligned to HashedBlockSize
            SecureWrapStartOffset = RequestedStartOffset;
        } else {
            //requested start offset is not aligned to HashedBlockSize and need to be adjusted
            //"StartOffsetAberration" extra bytes need to be readed
            SecureWrapStartOffset = RequestedStartOffset - StartOffsetAberration;
        }

        SecureWrapContext_p->State = SECURE_WRAP_PROCESSING;

        //lint -fallthrough
    case SECURE_WRAP_PROCESSING:
        BytesRead = Do_IO_ScatteredPositionedRead(SecureWrapContext_p->Parent, VectorIndex, VectorList[VectorIndex].Length, SecureWrapStartOffset);

        if ((BytesRead > 0) && (BytesRead != IO_ERROR)) {
            if (!VerifySecureWrapVector(SecureWrapContext_p, VectorIndex, SecureWrapStartOffset)) {
                A_(printf("io_secure_wrap.c (%d): **ERROR** Vector verifications failed! Not Correct Archive!\n", __LINE__);)
                IO_ErrorCode = E_LOADER_SEC_LIB_VERIFY_FAILURE;
                return IO_ERROR;
            }

            //Mark preveously cached bytes for cleaning
            if (TRUE == SecureWrapCachedData.VectorCachedData.DataValid) {
                VectorList[SecureWrapCachedData.VectorCachedData.ParentVector].VectorProtected = FALSE;
            }

            //Cache vector
            NrOfBuffers = VectorList[VectorIndex].BulkVector_p->Buffers;

            SecureWrapCachedData.VectorCachedData.CachedDataLength = VectorList[VectorIndex].Length;
            SecureWrapCachedData.VectorCachedData.Buffers = NrOfBuffers;
            SecureWrapCachedData.VectorCachedData.ChunkSize = VectorList[VectorIndex].ChunkSize;
            SecureWrapCachedData.VectorCachedData.IOffset = 0;
            SecureWrapCachedData.VectorCachedData.ParentVector = VectorIndex;
            SecureWrapCachedData.VectorCachedData.DataValid = TRUE;

            for (Counter = 0; ((Counter < MAX_OPEN_IO) && (NrOfBuffers > 0)); Counter++) {
                SecureWrapCachedData.VectorCachedData.Entries[Counter].Buffer_p = VectorList[VectorIndex].Entries[Counter].Buffer_p;
                NrOfBuffers--;
            }

            SecureWrapCachedData.OffsetInParent = SecureWrapStartOffset;
            VectorList[VectorIndex].VectorProtected = TRUE;

            VectorList[VectorIndex].IOffset = StartOffsetAberration;
            SecureWrapContext_p->State = SECURE_WRAP_IDLE;
            BytesRead = Length;
        }

        break;

    default:
        IO_ErrorCode = E_GENERAL_IO_ERROR;
        BytesRead = IO_ERROR;
        break;
    }

    return BytesRead;
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
 * @retval  IO_INVALID_HANDLE   if write to resource is not possible,
 * @retval  bytes written       if "bytes written" bytes are written
 *                              in the IO resource.
 */
static uint64 SecureWrapGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    A_(printf("io_secure_wrap.c (%d): ** Unsupported functionality! **\n", __LINE__);)
    IDENTIFIER_NOT_USED(Context_p);
    IDENTIFIER_NOT_USED(VectorIndex);
    IDENTIFIER_NOT_USED(Length);
    IDENTIFIER_NOT_USED(Offset);
    IO_ErrorCode = E_GENERAL_IO_ERROR;
    return IO_ERROR;
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
static uint64 SecureWrapLength(const void *const Context_p)
{
    return ((SecureWrapContext_t *)Context_p)->Length;
}

/**
 * Vector verification .
 *
 * @param [in] SecureWrapContext_p SecurityWrapContext to use
 * @param [in] Buffer_p      Pointer to buffer that is big enough to accept
 *                           all data.
 * @param [in] VectorIndex   Handle of initialized IO Vector
 *                           with at least Buffers in it, containing all data.
 * @param [in] Length        bytes to be read in Buffer space.
 * @param [in] Offset        Offset in buffer space.
 */
static boolean VerifySecureWrapVector(const SecureWrapContext_t *const SecureWrapContext_p, const uint32 VectorIndex, uint64 StartBlockOffset)
{
    ErrorCode_e ReturnValue = E_LOADER_SEC_LIB_VERIFY_FAILURE;
    boolean Result = FALSE;
    uint64 BlockOffset = StartBlockOffset;
    uint32 Counter = 0;
    uint32 Buffers = 0;

    if (VectorIndex < MAX_BULK_TL_PROCESSES) {
        Buffers = VectorList[VectorIndex].BulkVector_p->Buffers;

        for (Counter = 0; Counter < Buffers; Counter++) {

            ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Verify_HashListBlockHash(SecureWrapContext_p->VerifyContext, BlockOffset, VectorList[VectorIndex].Entries[Counter].Hash_p);

            if (ReturnValue != E_SUCCESS) {
                A_(printf("io_secure_wrap.c (%d): **ERROR: Verify FAILED at Block Offset = 0x%llx\n", __LINE__, BlockOffset);)
                return FALSE;
            }

            BlockOffset +=  SecureWrapContext_p->HashedBlockSize;
        }

        Result = TRUE;
    }

    return Result;
}

/**     @} */
/** @} */
