/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_BDM
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
#include "block_device_management.h"
#include "r_memory_utils.h"

/*******************************************************************************
 *   Declaration of file local functions
 ******************************************************************************/
static uint32 BDMClose(void *Context_p);
static uint64 BDMRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BDMWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BDMLength(const void *const Context_p);
static uint64 BDMPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BDMPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BDMScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 BDMGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint32 BDMCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);
static uint64 BDM_ReadChunk(const BDMContext_t *const BDMContext_p, uint8 *const Buffer_p, uint32 Length, const uint32 Offset);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  This type defines functions for BDM IO Interface
 */
static const IO_Interface_t BDMInterface = {
    BDMClose,
    BDMRead,
    BDMWrite,
    BDMPositionedRead,
    BDMPositionedWrite,
    BDMScatteredPositionedRead,
    BDMGatheredPositionedWrite,
    BDMLength,
    BDMCreateVector
};

#define ALIGNMENT_MASK 3

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens BDM unit and returns Handle to it.
 *
 *  @param [in] Unit            Unit to be opened.
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a BDM.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for BDM context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BDMOpen(FPD_Config_t *const FPDConfig_p)
{
    static BDMContext_t *BDMContext_p;
    BDM_Unit_Config_t Configuration;
    IO_Handle_t Handle;
    ErrorCode_e ReturnValue;

    ReturnValue = Do_BDM_GetInfo(FPDConfig_p, &Configuration);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bdm.c (%d): ** Get Unit info failed! **\n", __LINE__);)
        Handle =  IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    BDMContext_p = malloc(sizeof(BDMContext_t));
    ASSERT(NULL != BDMContext_p);

    BDMContext_p->PageSizeInBytes = Configuration.PageSizeInBytes;
    BDMContext_p->LogicalSectors = Configuration.LogicalSectors;
    BDMContext_p->ReadPosition = 0;
    BDMContext_p->WritePosition = 0;
    BDMContext_p->Length = (uint64)Configuration.LogicalSectors * (uint64)Configuration.PageSizeInBytes;
    BDMContext_p->FirstBad_p = NULL;
    BDMContext_p->FPDConfig_p = FPDConfig_p;

    Handle = IO_Create(&BDMInterface, BDMContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(BDMContext_p);
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
static uint32 BDMCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    uint32 Counter;
    uint32 VectorCounter;
    uint32 NrOfBuffers;
    uint32 InternalBuffSize;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((BuffSize == 0) || (BuffSize >= MAX_UINT32) || (Length >= MAX_UINT32)) {
        A_(printf("Error: Invalid input parameters io_bdm.c (%d)\n\n", __LINE__);)
        VectorCounter = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    InternalBuffSize = (uint32)(((BuffSize + BDMContext_p->PageSizeInBytes - 1) / BDMContext_p->PageSizeInBytes) * BDMContext_p->PageSizeInBytes);
    NrOfBuffers = (uint32)((Length + InternalBuffSize - 1) / InternalBuffSize);
    VectorCounter = IO_FindFreeVector(NrOfBuffers);

    if (VectorCounter < MaxOpenIO) {
        VectorList[VectorCounter].Status = IOV_MAPPED;
        VectorList[VectorCounter].IOffset = 0;
        VectorList[VectorCounter].Length = (uint32)Length;
        VectorList[VectorCounter].ChunkSize = (uint32)InternalBuffSize;
        VectorList[VectorCounter].BulkTransfer = 0;
        VectorList[VectorCounter].BulkVector_p = NULL;

        for (Counter = 0; ((Counter < MaxOpenIO) && (NrOfBuffers > 0)); Counter++) {
            if (VectorList[VectorCounter].Entries[Counter].Status == IOV_UNMAPPED) {
                NrOfBuffers--;
                VectorList[VectorCounter].Entries[Counter].Status = IOV_MAPPED;
                VectorList[VectorCounter].Entries[Counter].Buffer_p = malloc((uint32)InternalBuffSize);
                ASSERT(NULL != VectorList[VectorCounter].Entries[Counter].Buffer_p);
                VectorList[VectorCounter].Entries[Counter].Hash_p = malloc(HASH_SIZE);
                ASSERT(NULL != VectorList[VectorCounter].Entries[Counter].Hash_p);
            }
        }
    } else {
        A_(printf("Not enough buffers io_file.c (%d)\n\n", __LINE__);)
        VectorCounter = IO_INVALID_HANDLE;
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
static uint32 BDMClose(void *Context_p)
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
static uint64 BDMRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLength = 0;
    uint32 LengthInPages = 0;
    uint32 StartPage = 0;
    uint32 InternalOffset = 0;
    uint8 *InternalBuffer_p;

    Length = MIN(BDMContext_p->Length - BDMContext_p->ReadPosition, Length);
    StartPage = (uint32)(BDMContext_p->ReadPosition / BDMContext_p->PageSizeInBytes);
    InternalOffset = (uint32)(BDMContext_p->ReadPosition % BDMContext_p->PageSizeInBytes);
    LengthInPages = (uint32)((BDMContext_p->ReadPosition + Length + BDMContext_p->PageSizeInBytes - 1) / BDMContext_p->PageSizeInBytes - StartPage);

    if ((StartPage + LengthInPages) > BDMContext_p->LogicalSectors) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        ReadLength = IO_ERROR;
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    if ((0 != InternalOffset) || (Length != (LengthInPages *(uint64)(BDMContext_p->PageSizeInBytes)))) {
        InternalBuffer_p = (uint8 *)malloc(LengthInPages * BDMContext_p->PageSizeInBytes);
        ASSERT(NULL != InternalBuffer_p);

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, InternalBuffer_p);

        if (ReturnValue == E_SUCCESS) {
            ReadLength = Length;
            BDMContext_p->ReadPosition += Length;
            memcpy((uint8 *)Buffer_p + Offset, InternalBuffer_p + InternalOffset, (uint32)Length);
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
        }

        BUFFER_FREE(InternalBuffer_p);
    } else {
        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, (uint8 *)Buffer_p);

        if (ReturnValue == E_SUCCESS) {
            ReadLength = Length;
            BDMContext_p->ReadPosition += Length;
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            ReadLength = IO_ERROR;
            IO_ErrorCode = ReturnValue;
        }
    }

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
static uint64 BDMWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 WriteLength = 0;
    uint32 LengthInPages = 0;
    uint32 StartPage = 0;
    uint32 InternalOffset = 0;
    uint8 *InternalBuffer_p;

    Length = MIN(BDMContext_p->Length - BDMContext_p->WritePosition, Length);
    StartPage = (uint32)(BDMContext_p->WritePosition / BDMContext_p->PageSizeInBytes);
    InternalOffset = (uint32)(BDMContext_p->WritePosition % BDMContext_p->PageSizeInBytes);
    LengthInPages = (uint32)((BDMContext_p->WritePosition + Length + BDMContext_p->PageSizeInBytes - 1) / BDMContext_p->PageSizeInBytes - StartPage);

    if ((StartPage + LengthInPages) > BDMContext_p->LogicalSectors) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    if ((0 != InternalOffset) || (Length != (LengthInPages *(uint64)BDMContext_p->PageSizeInBytes))) {
        InternalBuffer_p = (uint8 *)malloc(LengthInPages * BDMContext_p->PageSizeInBytes);
        ASSERT(NULL != InternalBuffer_p);

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, 1, BDMContext_p->FirstBad_p, InternalBuffer_p);

        if (E_SUCCESS != ReturnValue) {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            BUFFER_FREE(InternalBuffer_p);
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage + LengthInPages - 1, 1, BDMContext_p->FirstBad_p, InternalBuffer_p + ((LengthInPages - 1) * BDMContext_p->PageSizeInBytes));

        if (E_SUCCESS != ReturnValue) {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            BUFFER_FREE(InternalBuffer_p);
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(InternalBuffer_p + InternalOffset, (uint8 *)Buffer_p + Offset, (uint32)Length);

        ReturnValue = Do_BDM_Write(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, InternalBuffer_p);

        if (E_SUCCESS == ReturnValue) {
            WriteLength = Length;
            BDMContext_p->WritePosition += WriteLength;
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Write failed and return %d.\n", __LINE__, ReturnValue);)
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
        }

        BUFFER_FREE(InternalBuffer_p);
    } else {
        ReturnValue = Do_BDM_Write(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, (uint8 *)Buffer_p + Offset);

        if (E_SUCCESS == ReturnValue) {
            WriteLength = Length;
            BDMContext_p->WritePosition += WriteLength;
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Write failed and return %d.\n", __LINE__, ReturnValue);)
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
        }
    }

ErrorExit:
    return WriteLength;
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
static uint64 BDMPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLength = 0;
    uint32 LengthInPages = 0;
    uint32 StartPage = 0;
    uint32 InternalOffset = 0;
    uint8 *InternalBuffer_p;

    if (Offset >= BDMContext_p->Length) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BDMContext_p->Length - Offset, Length);
    StartPage = (uint32)(Offset / BDMContext_p->PageSizeInBytes);
    InternalOffset = (uint32)(Offset % BDMContext_p->PageSizeInBytes);
    LengthInPages = (uint32)((Offset + Length + BDMContext_p->PageSizeInBytes - 1) / BDMContext_p->PageSizeInBytes - StartPage);

    if ((StartPage + LengthInPages) > BDMContext_p->LogicalSectors) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    if ((0 != InternalOffset) || (Length != (LengthInPages *(uint64)BDMContext_p->PageSizeInBytes))) {
        InternalBuffer_p = (uint8 *)malloc(LengthInPages * BDMContext_p->PageSizeInBytes);
        ASSERT(NULL != InternalBuffer_p);

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, InternalBuffer_p);

        if (E_SUCCESS == ReturnValue) {
            ReadLength = Length;
            BDMContext_p->ReadPosition = Offset + ReadLength;
            memcpy((uint8 *)Buffer_p, InternalBuffer_p + InternalOffset, (uint32)Length);
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
        }

        BUFFER_FREE(InternalBuffer_p);
    } else {
        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, (uint8 *)Buffer_p);

        if (ReturnValue == E_SUCCESS) {
            ReadLength = Length;
            BDMContext_p->ReadPosition += Length;
        } else {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
        }
    }

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
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 BDMPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 WriteLength = 0;
    uint32 LengthInPages = 0;
    uint32 StartPage = 0;
    uint32 InternalOffset = 0;
    uint8 *InternalBuffer_p = NULL;

    if (Offset >= BDMContext_p->Length) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    Length = MIN(BDMContext_p->Length - Offset, Length);
    StartPage = (uint32)(Offset / BDMContext_p->PageSizeInBytes);
    InternalOffset = (uint32)(Offset % BDMContext_p->PageSizeInBytes);
    LengthInPages = (uint32)((Offset + Length + BDMContext_p->PageSizeInBytes - 1) / BDMContext_p->PageSizeInBytes - StartPage);

    if ((StartPage + LengthInPages) > BDMContext_p->LogicalSectors) {
        B_(printf("io_bdm.c (%d): Invalid input parameters passed to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    if ((0 != InternalOffset) || (Length != (LengthInPages *(uint64)BDMContext_p->PageSizeInBytes))) {
        InternalBuffer_p = (uint8 *)malloc(LengthInPages * BDMContext_p->PageSizeInBytes);
        ASSERT(NULL != InternalBuffer_p);

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage, 1, BDMContext_p->FirstBad_p, InternalBuffer_p);

        if (E_SUCCESS != ReturnValue) {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            BUFFER_FREE(InternalBuffer_p);
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, StartPage + LengthInPages - 1, 1, BDMContext_p->FirstBad_p, InternalBuffer_p + ((LengthInPages - 1) * BDMContext_p->PageSizeInBytes));

        if (E_SUCCESS != ReturnValue) {
            B_(printf("io_bdm.c (%d): Do_BDM_Read failed and return %d.\n", __LINE__, ReturnValue);)
            BUFFER_FREE(InternalBuffer_p);
            IO_ErrorCode = ReturnValue;
            WriteLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(InternalBuffer_p + InternalOffset, (uint8 *)Buffer_p, (uint32)Length);

        ReturnValue = Do_BDM_Write(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, InternalBuffer_p);

        BUFFER_FREE(InternalBuffer_p);
    } else if (((uint32)Buffer_p & ALIGNMENT_MASK)) {
        uint32 LengthInBytes = LengthInPages * BDMContext_p->PageSizeInBytes;

        C_(printf("io_bdm.c (%d): Buffer is unaligned in Do_BDM_Write.\n", __LINE__);)
        InternalBuffer_p = (uint8 *)malloc(LengthInBytes);
        ASSERT(NULL != InternalBuffer_p);

        memcpy(InternalBuffer_p, (uint8 *)Buffer_p, LengthInBytes);

        ReturnValue = Do_BDM_Write(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, InternalBuffer_p);

        BUFFER_FREE(InternalBuffer_p);
    } else {
        ReturnValue = Do_BDM_Write(BDMContext_p->FPDConfig_p, StartPage, LengthInPages, BDMContext_p->FirstBad_p, (uint8 *)Buffer_p);
    }

    if (E_SUCCESS == ReturnValue) {
        WriteLength = Length;
        BDMContext_p->WritePosition = Offset + WriteLength;
    } else {
        B_(printf("io_bdm.c (%d): Do_BDM_Write failed and return %d.\n", __LINE__, ReturnValue);)
        IO_ErrorCode = ReturnValue;
        WriteLength = IO_ERROR;
    }

ErrorExit:
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
static uint64 BDMScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint64 ReadLength = 0;
    uint32 Counter = 0;
    uint64 BytesRead = 0;
    uint64 InternalOffset = 0;
    uint64 BytesToRead = 0;

    //sanity check of input parameters
    if ((Offset >= BDMContext_p->Length) || (Length > (BDMContext_p->Length - Offset))) {
        B_(printf("io_bdm.c (%d): Invalid input parameters to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    InternalOffset = Offset;

    //loop until all requested data are collected
    for (Counter = 0; ((Counter < MaxOpenIO) && (Length > 0)); Counter++) {
        BytesToRead = MIN(Length, VectorList[VectorIndex].ChunkSize);

        BytesRead = BDM_ReadChunk(BDMContext_p, (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p), (uint32)BytesToRead, (uint32)InternalOffset);

        if (IO_ERROR == BytesRead) {
            B_(printf("io_bdm.c %d: BDMScatteredPositionedRead failed to read requested data.\n", __LINE__);)
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        C_(printf(" *** BDM_FillBuffer successfully read %d bytes, Offset = %d *** \n", (uint32)BytesRead, (uint32)InternalOffset);)

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
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 BDMGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    BDMContext_t *BDMContext_p = (BDMContext_t *) Context_p;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;
    uint32 Counter = 0;
    uint64 BytesWritten = 0;
    static uint64 InternalOffset = 0;
    uint64 BytesToWrite = 0;
    uint64 VectorOffset = 0;
    uint64 WriteLegth = 0;

    //sanity check of input parameters
    if ((Offset >= BDMContext_p->Length) || (Length > (BDMContext_p->Length - Offset))) {
        A_(printf("io_bdm.c (%d): Invalid input parameters to function.\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLegth = IO_ERROR;
        goto ErrorExit;
    }

    InternalOffset = Offset;

    /*
     * Write cached data first (if any)
     */
    if (TRUE == VectorList[VectorIndex].CachedData.DataValid) {
        uint64 CachedDataChunkSize = VectorList[VectorIndex].CachedData.ChunkSize;

        VectorOffset = VectorList[VectorIndex].CachedData.IOffset;

        //loop until all cached data are written
        for (Counter = 0; ((Counter < MaxOpenIO) && (Length > 0)); Counter++) {
            if (VectorOffset >= CachedDataChunkSize) {
                VectorOffset -= CachedDataChunkSize;
                continue;
            }

            BytesToWrite = MIN(VectorList[VectorIndex].CachedData.CachedDataLength, (CachedDataChunkSize - VectorOffset));

            if (Length < BytesToWrite) {
                BytesToWrite = Length;
            }

            BytesWritten = BDMPositionedWrite(Context_p, (uint8 *)(VectorList[VectorIndex].CachedData.Entries[Counter].Buffer_p) + VectorOffset, BytesToWrite, InternalOffset);

            if (IO_ERROR == BytesWritten) {
                B_(printf("io_bdm.c %d: BDMGatheredPositionedWrite: **ERROR** failed to write requested data.\n", __LINE__);)
                WriteLegth = IO_ERROR;
                goto ErrorExit;
            }

            InternalOffset += BytesWritten;
            VectorList[VectorIndex].CachedData.CachedDataLength -= BytesWritten;
            Length -= BytesWritten;
            VectorOffset = 0;

            if (0 == VectorList[VectorIndex].CachedData.CachedDataLength) {
                B_(printf("io_bdm.c %d: All cached data successfully written.\n", __LINE__);)
                break;
            }
        }
    }

    VectorOffset = VectorList[VectorIndex].IOffset;

    /*
     * Loop until all remaining data are written
     */
    for (Counter = 0; ((Counter < MaxOpenIO) && (Length > 0)); Counter++) {
        if (VectorOffset >= VectorList[VectorIndex].ChunkSize) {
            VectorOffset -= VectorList[VectorIndex].ChunkSize;
            continue;
        }

        BytesToWrite = VectorList[VectorIndex].ChunkSize - VectorOffset;

        if (Length < BytesToWrite) {
            BytesToWrite = Length;
        }

        BytesWritten = BDMPositionedWrite(Context_p, (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + VectorOffset, BytesToWrite, InternalOffset);

        if (IO_ERROR == BytesWritten) {
            B_(printf("io_bdm.c %d: BDMGatheredPositionedWrite: **ERROR** failed to write requested data.\n", __LINE__);)
            WriteLegth = IO_ERROR;
            goto ErrorExit;
        }

        InternalOffset += BytesWritten;
        Length -= BytesWritten;
        VectorOffset = 0;
    }

    WriteLegth = InternalOffset - Offset;

ErrorExit:
    return WriteLegth;
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
static uint64 BDMLength(const void *const Context_p)
{
    return ((BDMContext_t *)Context_p)->Length;
}

/*
 * Read Length bytes from flash and fill specified buffer.
 *
 * @param [in]  BDMContext_p       initialized BDM context.
 * @param [out] Buffer_p           points to memory where data will be stored.
 * @param [in]  Length             size of requested data in bytes.
 * @param [in]  Offset             offset from flash start in bytes.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 */
static uint64 BDM_ReadChunk(const BDMContext_t *const BDMContext_p, uint8 *const Buffer_p, uint32 Length, const uint32 Offset)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLength = 0;
    uint8 *LogicalSector_p = NULL;
    uint32 BytesRead = 0;
    uint32 AlignedOffset = 1;
    uint32 LogicalSectorStart = 0;
    uint32 PageSize = BDMContext_p->PageSizeInBytes;
    uint32 LogicalSectorsToRead = 0;

    LogicalSectorStart = Offset / PageSize;

    if (LogicalSectorStart > BDMContext_p->LogicalSectors) {
        B_(printf("io_bdm.c (%d): ** Try to read beyond last logical sector! **\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    AlignedOffset = LogicalSectorStart * PageSize;

    LogicalSector_p = (uint8 *)malloc(PageSize);
    ASSERT(NULL != LogicalSector_p);

    //if offset is not aligned to page size
    if (Offset > AlignedOffset) {
        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, LogicalSectorStart, 1, BDMContext_p->FirstBad_p, LogicalSector_p);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("io_bdm.c (%d): ** Do_BDM_Read failed to read Page %d, and return %d! **\n", __LINE__, LogicalSectorStart, ReturnValue);)
            BUFFER_FREE(LogicalSector_p);
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(Buffer_p, LogicalSector_p + (Offset - AlignedOffset), PageSize - (Offset - AlignedOffset));
        BytesRead += (PageSize - (Offset - AlignedOffset));
        Length -= BytesRead;
        LogicalSectorStart++;
    }

    //If read compleated
    if (0 == Length) {
        C_(printf("io_fpd.c %d: FPD_FillBuffer successfuly read %d bytes.\n", __LINE__, Length);)
        BUFFER_FREE(LogicalSector_p);
        ReadLength = (uint64)BytesRead;
        goto ErrorExit;
    }

    LogicalSectorsToRead = MIN(BDMContext_p->LogicalSectors - LogicalSectorStart, Length / PageSize);

    ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, LogicalSectorStart, LogicalSectorsToRead, BDMContext_p->FirstBad_p, Buffer_p + BytesRead);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bdm.c (%d): ** Do_FPD_ReadPage failed to read Page %d, and return %d! **\n", __LINE__, LogicalSectorStart, ReturnValue);)
        BUFFER_FREE(LogicalSector_p);
        IO_ErrorCode = ReturnValue;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    BytesRead += (LogicalSectorsToRead * PageSize);
    Length -= (LogicalSectorsToRead * PageSize);
    LogicalSectorStart += LogicalSectorsToRead;

    if ((Length > 0) && (BDMContext_p->LogicalSectors > LogicalSectorStart)) {
        ReturnValue = Do_BDM_Read(BDMContext_p->FPDConfig_p, LogicalSectorStart, 1, BDMContext_p->FirstBad_p, LogicalSector_p);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("io_bdm.c (%d): ** Do_BDM_Read failed to read Page %d, and return %d! **\n", __LINE__, LogicalSectorStart, ReturnValue);)
            BUFFER_FREE(LogicalSector_p);
            IO_ErrorCode = ReturnValue;
            ReadLength = IO_ERROR;
            goto ErrorExit;
        }

        memcpy(Buffer_p + BytesRead, LogicalSector_p, Length);
        BytesRead += Length;
    }

    BUFFER_FREE(LogicalSector_p);
    ReadLength = (uint64)BytesRead;

ErrorExit:
    return ReadLength;
}

/**     @} */
/** @} */
