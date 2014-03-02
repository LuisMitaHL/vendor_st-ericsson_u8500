/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_BAM
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "error_codes.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "boot_area_management.h"
#include "r_memory_utils.h"

/*******************************************************************************
 *   Declaration of file local functions
 ******************************************************************************/
static uint32 BAMClose(void *Context_p);
static uint64 BAMRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BAMWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BAMLength(const void *const Context_p);
static uint64 BAMPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BAMPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 BAMScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 BAMGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint32 BAMCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  This type defines functions for BAM IO Interface
 */
static const IO_Interface_t BAMInterface = {
    BAMClose,
    BAMRead,
    BAMWrite,
    BAMPositionedRead,
    BAMPositionedWrite,
    BAMScatteredPositionedRead,
    BAMGatheredPositionedWrite,
    BAMLength,
    BAMCreateVector
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens BAM and returns Handle to it.
 *
 *  @param [in] FPDConfig_p     Flash configuration of BAM to be opened
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a BAM.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for BAM context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BAMOpen(FPD_Config_t *const FPDConfig_p)
{
    static BAMContext_t *BAMContext_p = NULL;
    BAM_Config_t *BAMConfig_p;
    ErrorCode_e ReturnValue;
    IO_Handle_t Handle = {0};

    BAMConfig_p = malloc(sizeof(BAM_Config_t));
    ASSERT(NULL != BAMConfig_p);

    ReturnValue = Do_BAM_GetConfig(FPDConfig_p, BAMConfig_p);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bam.c (%d): ** Memory allocation failed! **\n", __LINE__);)
        BUFFER_FREE(BAMConfig_p);
        return IO_INVALID_HANDLE;
    }

    BAMContext_p = malloc(sizeof(BAMContext_t));
    ASSERT(NULL != BAMContext_p);

    BAMContext_p->ReadPosition = 0;
    BAMContext_p->WritePosition = 0;
    BAMContext_p->Length = BAMConfig_p->Unit_Configuration[0].EndBlockByteAddress - BAMConfig_p->Unit_Configuration[0].StartBlock;
    BAMContext_p->FPDConfig_p = FPDConfig_p;

    BUFFER_FREE(BAMConfig_p);
    Handle = IO_Create(&BAMInterface, BAMContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(BAMContext_p);
    }

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
static uint32 BAMCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    uint32 Counter;
    uint32 VectorCounter;
    uint32 NrOfBuffers;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((BuffSize != 0) && (BuffSize < MAX_UINT32) && (Length < MAX_UINT32)) {
        NrOfBuffers = (uint32)((Length + BuffSize - 1) / BuffSize);
    } else {
        A_(printf("Error: Invalid input parameters io_bam.c (%d)\n\n", __LINE__);)
        VectorCounter = IO_INVALID_HANDLE;
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
static uint32 BAMClose(void *Context_p)
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
static uint64 BAMRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    Length = MIN(BAMContext_p->Length - BAMContext_p->ReadPosition, Length);

    ReturnValue = Do_BAM_Read(BAMContext_p->FPDConfig_p, (uint32)BAMContext_p->ReadPosition, (uint8 *)Buffer_p + (uint32)Offset, (uint32)Length);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bam.c (%d): ** Read failed! **\n", __LINE__);)
        Length = IO_ERROR;
        IO_ErrorCode = ReturnValue;
        goto ErrorExit;
    }

    BAMContext_p->ReadPosition += Length;

ErrorExit:
    return Length;
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
static uint64 BAMWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    Length = MIN(BAMContext_p->Length - BAMContext_p->WritePosition, Length);

    ReturnValue = Do_BAM_Write(BAMContext_p->FPDConfig_p, (uint32)BAMContext_p->WritePosition, (uint8 *)Buffer_p + (uint32)Offset, (uint32)Length);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bam.c (%d): ** Write failed! **\n", __LINE__);)
        Length = IO_ERROR;
        IO_ErrorCode = ReturnValue;
        goto ErrorExit;
    }

    BAMContext_p->WritePosition += Length;

ErrorExit:
    return Length;
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
static uint64 BAMPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    if (Offset >= BAMContext_p->Length) {
        Length = IO_ERROR;
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    Length = MIN(BAMContext_p->Length - Offset, Length);

    ReturnValue = Do_BAM_Read(BAMContext_p->FPDConfig_p, (uint32)Offset, (uint8 *)Buffer_p, (uint32)Length);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bam.c (%d): ** Read failed! **\n", __LINE__);)
        Length = IO_ERROR;
        IO_ErrorCode = ReturnValue;
        goto ErrorExit;
    }

    BAMContext_p->ReadPosition = Offset + Length;

ErrorExit:
    return Length;
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
static uint64 BAMPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    if (Offset >= BAMContext_p->Length) {
        Length = IO_ERROR;
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    Length = MIN(BAMContext_p->Length - Offset, Length);

    ReturnValue = Do_BAM_Write(BAMContext_p->FPDConfig_p, (uint32)Offset, (uint8 *)Buffer_p, (uint32)Length);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("io_bam.c (%d): ** Write failed! **\n", __LINE__);)
        Length = IO_ERROR;
        IO_ErrorCode = ReturnValue;
        goto ErrorExit;
    }

    BAMContext_p->WritePosition = Offset + Length;

ErrorExit:
    return Length;
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
static uint64 BAMScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 ReadLegth = 0;
    uint32 Counter = 0;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if (Offset >= BAMContext_p->Length) {
        ReadLegth = IO_ERROR;
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    Length = MIN(BAMContext_p->Length - Offset, Length);
    ReadLegth = 0;

    for (Counter = 0; ((Counter < MaxOpenIO) && (ReadLegth < Length)); Counter++) {
        if ((VectorList[VectorIndex].Entries[Counter].Status == IOV_MAPPED)) {
            ReturnValue = Do_BAM_Read(BAMContext_p->FPDConfig_p,
                                      (uint32)Offset + (uint32)ReadLegth,
                                      (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p),
                                      (uint32)(MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLegth))));

            if (E_SUCCESS != ReturnValue) {
                A_(printf("io_bam.c (%d): ** Read failed! **\n", __LINE__);)
                ReadLegth = IO_ERROR;
                IO_ErrorCode = ReturnValue;
                goto ErrorExit;
            }

            ReadLegth += MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLegth));
            BAMContext_p->ReadPosition = Offset + ReadLegth;
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
        }
    }

ErrorExit:
    return ReadLegth;
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
static uint64 BAMGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    BAMContext_t *BAMContext_p = (BAMContext_t *) Context_p;
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint64 WriteLength = 0;
    uint32 Counter = 0;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if (Offset >= BAMContext_p->Length) {
        WriteLength = IO_ERROR;
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    Length = MIN(BAMContext_p->Length - Offset, Length);
    WriteLength = 0;

    for (Counter = 0; ((Counter < MaxOpenIO) && (WriteLength < Length)); Counter++) {
        if ((VectorList[VectorIndex].Entries[Counter].Status == IOV_MAPPED)) {
            ReturnValue = Do_BAM_Write(BAMContext_p->FPDConfig_p,
                                       (uint32)Offset + (uint32)WriteLength,
                                       (uint8 *)(VectorList[VectorIndex].Entries[Counter].Buffer_p),
                                       (uint32)(MIN(VectorList[VectorIndex].ChunkSize, (Length - WriteLength))));

            if (E_SUCCESS != ReturnValue) {
                A_(printf("io_bam.c (%d): ** Write failed! **\n", __LINE__);)
                WriteLength = IO_ERROR;
                goto ErrorExit;
            }

            WriteLength += MIN(VectorList[VectorIndex].ChunkSize, (Length - WriteLength));
            BAMContext_p->WritePosition = Offset + WriteLength;
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
        }
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
static uint64 BAMLength(const void *const Context_p)
{
    return ((BAMContext_t *)Context_p)->Length;
}

/**     @} */
/** @} */
