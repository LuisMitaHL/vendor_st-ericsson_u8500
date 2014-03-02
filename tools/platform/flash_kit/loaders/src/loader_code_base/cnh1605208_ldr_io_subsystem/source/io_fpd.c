/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_FPD
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "error_codes.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "boot_area_management.h"
#include "flash_manager.h"

/*******************************************************************************
 *   Declaration of file local functions
 ******************************************************************************/
static uint32 FPDClose(void *Context_p);
static uint64 FPDRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FPDWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FPDLength(const void *const Context_p);
static uint64 FPDPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FPDPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FPDScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 FPDGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint32 FPDCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);

static uint64 FPD_ReadChunk(const FPDContext_t *const FPDContext_p, uint8 *const Buffer_p, uint32 Length, const uint32 Offset);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  This type defines functions for FPD IO Interface
 */
static const IO_Interface_t FPDInterface = {
    FPDClose,
    FPDRead,
    FPDWrite,
    FPDPositionedRead,
    FPDPositionedWrite,
    FPDScatteredPositionedRead,
    FPDGatheredPositionedWrite,
    FPDLength,
    FPDCreateVector
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens FPD and returns Handle to it.
 *
 *  @param [in] FlashDevice     Flash device to open
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a FPD.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for FPD context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_FPDOpen(const uint32 RedundantArea, const uint8 FlashDevice)
{
    static FPDContext_t *FPDContext_p = NULL;
    FPD_Config_t *FPDConfig_p;
    ErrorCode_e ReturnValue;
    IO_Handle_t Handle = {0};

    FPDConfig_p = malloc(sizeof(FPD_Config_t));
    ASSERT(NULL != FPDConfig_p);

    ReturnValue = Do_FPD_GetInfo(FlashDevice, FPDConfig_p);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_fpd.c (%d): ** Memory allocation failed! **\n", __LINE__);)
        BUFFER_FREE(FPDConfig_p);
        Handle = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    FPDContext_p = malloc(sizeof(FPDContext_t));
    ASSERT(NULL != FPDContext_p);

    FPDContext_p->ReadPosition = 0;
    FPDContext_p->WritePosition = 0;
    FPDContext_p->Length = FPDConfig_p->DeviceSize;
    FPDContext_p->FlashDevice = FlashDevice;

    BUFFER_FREE(FPDConfig_p);
    Handle = IO_Create(&FPDInterface, FPDContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(FPDContext_p);
    }

ErrorExit:
    return Handle;
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
 */
static uint32 FPDCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    uint32 Counter;
    uint32 VectorCounter;
    uint32 NrOfBuffers;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((BuffSize != 0) && (BuffSize < MAX_UINT32) && (Length < MAX_UINT32)) {
        NrOfBuffers = (uint32)((Length + BuffSize - 1) / BuffSize);
    } else {
        A_(printf("Error: Invalid input parameters io_fpd.c (%d)\n\n", __LINE__);)
        VectorCounter =  IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    VectorCounter = IO_FindFreeVector(NrOfBuffers);

    if (VectorCounter < MaxOpenIO) {
        VectorList[VectorCounter].Status = IOV_MAPPED;
        VectorList[VectorCounter].IOffset = 0;
        VectorList[VectorCounter].Length = (uint32)Length;
        VectorList[VectorCounter].ChunkSize = (uint32)BuffSize;
        VectorList[VectorCounter].BulkTransfer = 0;
        VectorList[VectorCounter].BulkVector_p = NULL;

        for (Counter = 0; ((Counter < MaxOpenIO) && (NrOfBuffers > 0)); Counter++) {
            if (VectorList[VectorCounter].Entries[Counter].Status == IOV_UNMAPPED) {
                NrOfBuffers--;
                VectorList[VectorCounter].Entries[Counter].Status = IOV_MAPPED;
                VectorList[VectorCounter].Entries[Counter].Buffer_p = malloc((uint32)BuffSize);
                ASSERT(NULL != VectorList[VectorCounter].Entries[Counter].Buffer_p);
                VectorList[VectorCounter].Entries[Counter].Hash_p = malloc(HASH_SIZE);
                ASSERT(NULL != VectorList[VectorCounter].Entries[Counter].Hash_p);
            }
        }
    } else {
        A_(printf("Not enough buffers io_file.c (%d)\n\n", __LINE__);)
        VectorCounter = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

ErrorExit:
    return VectorCounter;
}

/*
 * Closes IO channel with given Context.
 *
 * @param [in] Context_p        initialized IO channel context.
 *
 * @retval  E_SUCCESS           when IO channel is closed,
 *
 */
static uint32 FPDClose(void *Context_p)
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
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 FPDRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FPDContext_t *FPDContext_p = (FPDContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLength = 0;
    uint32 Pages = 0;
    uint32 StartPage = 0;
    FPD_Config_t Configuration = {0};
    uint32 PageSize = 1;
    uint64 InternalOffset = 0;
    uint32 i = 0;

    InternalOffset = Offset;
    Length = MIN(FPDContext_p->Length - FPDContext_p->ReadPosition, Length);

    /* read pagesize */
    ReturnValue = Do_FPD_GetInfo(FPDContext_p->FlashDevice, &Configuration);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_FPD.c (%d): ** Read failed! **\n", __LINE__);)
        IO_ErrorCode = ReturnValue;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    PageSize = Configuration.PageSize;
    Pages = (uint32)Length / PageSize;
    StartPage = (uint32)FPDContext_p->ReadPosition / PageSize;

    for (i = 0; i < Pages; i++) {
        ReturnValue = Do_FPD_ReadPage(StartPage, FPDContext_p->FlashDevice,
                                      (uint8 *)Buffer_p + (uint32)InternalOffset,
                                      (uint8 *)Buffer_p + (uint32)InternalOffset + PageSize);

        if ((E_SUCCESS != ReturnValue) && (E_PD_NAND_RESULT_READING_ERASED_PAGE != ReturnValue)) {
            A_(printf("io_FPD.c (%d): ** Read failed! **\n", __LINE__);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        InternalOffset += Configuration.PageExtraSize + PageSize;
        StartPage++;
    }

    FPDContext_p->ReadPosition += InternalOffset - Offset;

    ReadLength = (InternalOffset - Offset);

ErrorExit:
    return ReadLength;
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
static uint64 FPDWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    A_(printf("io_fpd.c (%d): ** Write failed! **\n", __LINE__);)
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
static uint64 FPDPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FPDContext_t *FPDContext_p = (FPDContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLength = 0;
    uint32 Pages = 0;
    uint32 StartPage = 0;
    FPD_Config_t Configuration = {0};
    uint32 PageSize = 1;
    uint64 InternalOffset = 0;
    uint32 i = 0;

    if (Offset >= FPDContext_p->Length) {
        B_(printf("io_fpd.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(FPDContext_p->Length - Offset, Length);

    /* read pagesize */
    ReturnValue = Do_FPD_GetInfo(FPDContext_p->FlashDevice, &Configuration);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_FPD.c (%d): ** Read failed! **\n", __LINE__);)
        IO_ErrorCode = ReturnValue;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    PageSize = Configuration.PageSize;
    Pages = (uint32)Length / PageSize;
    StartPage = (uint32)Offset / PageSize;
    InternalOffset = 0;

    for (i = 0; i < Pages; i++) {
        ReturnValue = Do_FPD_ReadPage(StartPage, FPDContext_p->FlashDevice,
                                      (uint8 *)Buffer_p + (uint32)InternalOffset,
                                      (uint8 *)Buffer_p + (uint32)InternalOffset + PageSize);

        if ((E_SUCCESS != ReturnValue) && (E_PD_NAND_RESULT_READING_ERASED_PAGE != ReturnValue)) {
            A_(printf("io_FPD.c (%d): ** Read failed! **\n", __LINE__);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        InternalOffset += Configuration.PageExtraSize + PageSize;
        StartPage++;
    }

    ReadLength = InternalOffset;
    FPDContext_p->ReadPosition = InternalOffset + Offset;

ErrorExit:
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
static uint64 FPDPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    A_(printf("io_fpd.c (%d): ** Write failed! **\n", __LINE__);)
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
static uint64 FPDScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    FPDContext_t *FPDContext_p = (FPDContext_t *) Context_p;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint64 ReadLength = 0;
    uint32 Counter = 0;
    uint64 BytesRead = 0;
    uint64 InternalOffset = 0;
    uint64 BytesToRead = 0;

    //sanity check of input parameters
    if ((Offset >= FPDContext_p->Length) || (Length > (FPDContext_p->Length - Offset))) {
        B_(printf("io_fpd.c (%d): Invalid input parameters to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    InternalOffset = Offset;

    //loop until all requested data are collected
    for (Counter = 0; ((Counter < MaxOpenIO) && (Length > 0)); Counter++) {
        BytesToRead = MIN(Length, VectorList[VectorIndex].ChunkSize);

        BytesRead = FPD_ReadChunk(FPDContext_p,
                                  (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p),
                                  (uint32)BytesToRead, (uint32)InternalOffset);

        if (IO_ERROR == BytesRead) {
            B_(printf("io_fpd.c %d: FPDScatteredPositionedRead failed to read requested data.\n", __LINE__);)
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        C_(printf(" *** FPD_FillBuffer successfuly read %d bytes, Offset = %d *** \n",
                  (uint32)BytesRead,
                  (uint32)InternalOffset);)

        InternalOffset += BytesRead;
        Length -= BytesRead;
    }

    ReadLength = (InternalOffset - Offset);

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
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
static uint64 FPDGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    A_(printf("io_fpd.c (%d): ** Write failed! **\n", __LINE__);)
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
static uint64 FPDLength(const void *const Context_p)
{
    return ((FPDContext_t *)Context_p)->Length;
}

/*
 * Read Length bytes from flash and fill specified buffer.
 *
 * @param [in]  FPDContext_p       initialized FPD context.
 * @param [out] Buffer_p           points to memory where data will be stored.
 * @param [in]  Length             size of requested data in bytes.
 * @param [in]  Offset             offset from flash start in bytes.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 FPD_ReadChunk(const FPDContext_t *const FPDContext_p, uint8 *const Buffer_p, uint32 Length, const uint32 Offset)
{
    ErrorCode_e  ReturnValue   = E_INVALID_INPUT_PARAMETERS;
    FPD_Config_t Configuration = {0};
    uint64       ReadLength    = 0;
    uint64       BytesRead     = 0;
    uint32       AlignedOffset = 1;
    uint32       StartPage     = 0;
    uint32       PageSize      = 1;
    uint32       PagesToRead   = 0;
    uint8       *PageBuffer_p  = NULL;

    // read page size
    ReturnValue = Do_FPD_GetInfo(FPDContext_p->FlashDevice, &Configuration);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_fpd.c (%d): ** FPD_FillBuffer failed to get FPD info! **\n", __LINE__);)
        IO_ErrorCode = ReturnValue;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    PageSize = Configuration.PageSize;
    StartPage = Offset / PageSize;
    AlignedOffset = StartPage * PageSize;

    PageBuffer_p = (uint8 *)malloc(PageSize);
    ASSERT(NULL != PageBuffer_p);

    //if offset is not aligned to page size
    if (Offset > AlignedOffset) {
        ReturnValue = Do_FPD_ReadPage(StartPage, FPDContext_p->FlashDevice, PageBuffer_p, NULL);

        if ((E_SUCCESS != ReturnValue) && (E_PD_NAND_RESULT_READING_ERASED_PAGE != ReturnValue)) {
            A_(printf("io_fpd.c (%d): ** Do_FPD_ReadPage failed to read Page %d, and return %d! **\n",
                      __LINE__,
                      StartPage,
                      ReturnValue);)

            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(Buffer_p, PageBuffer_p + (Offset - AlignedOffset), PageSize - (Offset - AlignedOffset));
        BytesRead += (PageSize - (Offset - AlignedOffset));
        Length -= (uint32)BytesRead;
        StartPage++;
    }

    //If read completed
    if (0 == Length) {
        C_(printf("io_fpd.c %d: FPD_FillBuffer successfully read %d bytes.\n", __LINE__, Length);)
        ReadLength = (uint64)BytesRead;
        goto ErrorExit;
    }

    PagesToRead = Length / PageSize;

    if (PagesToRead > 0) {
        ReturnValue = Do_FPD_ReadConsecutivePages(StartPage, PagesToRead, FPDContext_p->FlashDevice, Buffer_p + BytesRead, NULL);

        if ((E_SUCCESS != ReturnValue) && (E_PD_NAND_RESULT_READING_ERASED_PAGE != ReturnValue)) {
            A_(printf("io_fpd.c (%d): ** Do_FPD_ReadPage failed to read Page %d, and return %d! **\n",
                      __LINE__,
                      StartPage,
                      ReturnValue);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        BytesRead += (PagesToRead * PageSize);
        Length -= (PagesToRead * PageSize);
        StartPage += PagesToRead;
    }

    if (Length > 0) {
        ReturnValue = Do_FPD_ReadPage(StartPage, FPDContext_p->FlashDevice, PageBuffer_p, NULL);

        if ((E_SUCCESS != ReturnValue) && (E_PD_NAND_RESULT_READING_ERASED_PAGE != ReturnValue)) {
            A_(printf("io_fpd.c (%d): ** Do_FPD_ReadPage failed to read Page %d, and return %d! **\n",
                      __LINE__,
                      StartPage,
                      ReturnValue);)

            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(Buffer_p + BytesRead, PageBuffer_p, Length);
        BytesRead += Length;
    }

    ReadLength = (uint64)BytesRead;

ErrorExit:

    BUFFER_FREE(PageBuffer_p);

    return ReadLength;
}

/**     @} */
/** @} */
