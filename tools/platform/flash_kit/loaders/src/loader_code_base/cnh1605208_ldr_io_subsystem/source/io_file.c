/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_File
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "os_free_fs.h"
#include "unistd.h"
#include "r_memory_utils.h"

/*******************************************************************************
 *   Declaration of file local functions
 ******************************************************************************/
static uint32 FileClose(void *Context_p);
static uint64 FileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 PositionedFileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 PositionedFileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 ScatteredPositionedFileRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 GatheredPositionedFileWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 FileLength(const void *const Context_p);
static uint32 FileCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);

/*
 *  Defines functions for File IO Interface
 */
static const IO_Interface_t FileInterface = {
    FileClose,
    FileRead,
    FileWrite,
    PositionedFileRead,
    PositionedFileWrite,
    ScatteredPositionedFileRead,
    GatheredPositionedFileWrite,
    FileLength,
    FileCreateVector
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens File and returns Handle to it.
 *
 *  @param [in] Name_p          Name of the file to be opened.
 *  @param [in] Omode           Open mode of the file to be opened (r/w).
 *  @param [in] Amode           Access mode of the file to be opened (rwx).
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a File.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for File context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_FileOpen(const char *Name_p, const uint32 Omode, const uint32 Amode)
{
    static FileContext_t *FileContext_p = NULL;
    IO_Handle_t Handle = IO_INVALID_HANDLE;
    uint32 FileDescriptor;

    if (NULL == Name_p) {
        goto ErrorExit;
    }

    FileDescriptor = OS_Free_Open_File(Name_p, Omode, Amode);

    if (FileDescriptor == IO_INVALID_HANDLE) {
        A_(printf("Error while opening file %s io_file.c (%d)\n\n", Name_p, __LINE__);)
        Handle = IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    FileContext_p = malloc(sizeof(FileContext_t));
    ASSERT(NULL != FileContext_p);

    FileContext_p->Descriptor = FileDescriptor;
    Handle = IO_Create(&FileInterface, FileContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        BUFFER_FREE(FileContext_p);
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
 *
 */
static uint32 FileCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    uint32 Counter;
    uint32 VectorCounter;
    uint32 NrOfBuffers;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((BuffSize != 0) && (BuffSize < MAX_UINT32) && (Length < MAX_UINT32)) {
        NrOfBuffers = (uint32)((Length + BuffSize - 1) / BuffSize);
    } else {
        A_(printf("Error: Invalid input parameters io_fpd.c (%d)\n\n", __LINE__);)
        VectorCounter = IO_VECTOR_INVALID_HANDLE;
        goto ErrorExit;
    }

    VectorCounter = IO_FindFreeVector(NrOfBuffers);

    if (VectorCounter < MaxOpenIO) {
        for (Counter = 0; ((Counter < MaxOpenIO) && (NrOfBuffers > 0)); Counter++) {
            VectorList[VectorCounter].Status = IOV_MAPPED;
            VectorList[VectorCounter].IOffset = 0;
            VectorList[VectorCounter].Length = (uint32)Length;
            VectorList[VectorCounter].ChunkSize = (uint32)BuffSize;
            VectorList[VectorCounter].BulkTransfer = 0;
            VectorList[VectorCounter].BulkVector_p = NULL;

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
static uint32 FileClose(void *Context_p)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint32 RetVal;

    RetVal = OS_Free_Close_File(FileContext_p->Descriptor);
    BUFFER_FREE(FileContext_p);
    return RetVal;
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
 *
 */
static uint64 FileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 ReadLength = 0;
    uint32 FileHandle;

    if (Length > MAX_UINT32) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    ReadLength = OS_Free_Read_File(FileHandle, ((char *)Buffer_p) + Offset, (uint32)Length);

    if (FS_ERROR == ReadLength) {
        A_(printf("Error: OS_Free_Read_File failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        ReadLength = IO_ERROR;
        goto ErrorExit;
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
 *
 */
static uint64 FileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 WriteLegth = 0;
    uint32 FileHandle;

    if (Length > MAX_UINT32) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLegth = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLegth = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLegth = IO_ERROR;
        goto ErrorExit;
    }

    WriteLegth = OS_Free_Write_File(FileHandle, ((char *)Buffer_p) + Offset, (uint32)Length);

    if (FS_ERROR == WriteLegth) {
        A_(printf("Error: OS_Free_Write_File failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        WriteLegth = IO_ERROR;
        goto ErrorExit;
    }

ErrorExit:
    return WriteLegth;
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
 *
 */
static uint64 PositionedFileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 ReadLength = 0;
    uint32 FileHandle;

    if (Length > MAX_UINT32) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    ReadLength = OS_Free_Lseek(FileHandle, (uint32) Offset, SEEK_SET);

    if (FS_ERROR == ReadLength) {
        A_(printf("Error: OS_Free_Lseek failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    ReadLength = OS_Free_Read_File(FileHandle, (char *)Buffer_p, (uint32)Length);

    if (FS_ERROR == ReadLength) {
        A_(printf("Error: OS_Free_Read_File failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        ReadLength = IO_ERROR;
        goto ErrorExit;
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
 *
 */
static uint64 PositionedFileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 WriteLength = 0;
    uint32 FileHandle;

    if (Length > MAX_UINT32) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    WriteLength = OS_Free_Lseek(FileHandle, (uint32) Offset, SEEK_SET);

    if (FS_ERROR == WriteLength) {
        A_(printf("Error: OS_Free_Lseek failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    WriteLength = OS_Free_Write_File(FileHandle, (char *)Buffer_p, (uint32)Length);

    if (FS_ERROR == WriteLength) {
        A_(printf("Error: OS_Free_Read_File failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        WriteLength = IO_ERROR;
        goto ErrorExit;
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
static uint64 ScatteredPositionedFileRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 ReadLength = 0;
    uint64 ReadTemp = 0;
    uint32 ReadCycle = 0;
    uint32 Counter = 0;
    uint32 FileHandle;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((Length > MAX_UINT32) || (VectorIndex >= MaxOpenIO)) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    ReadLength = OS_Free_Lseek(FileContext_p->Descriptor, (uint32)Offset, SEEK_SET);

    if (FS_ERROR == ReadLength) {
        A_(printf("Error: OS_Free_Lseek failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        ReadLength = IO_ERROR;
        goto ErrorExit;
    }

    ReadLength = 0;

    for (Counter = 0; ((Counter < MaxOpenIO) && (ReadLength < Length)); Counter++) {
        if ((VectorList[VectorIndex].Entries[Counter].Status == IOV_MAPPED)) {
            ReadTemp = 0;
            ReadCycle = 0;

            while ((ReadTemp < MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLength))) && (ReadCycle < MAX_CYCLES)) {
                ReadTemp += OS_Free_Read_File(FileContext_p->Descriptor,
                                              (char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + ReadTemp,
                                              (uint32)(MIN(VectorList[VectorIndex].ChunkSize,
                                                       (Length - ReadLength)) - ReadTemp));

                if (FS_ERROR == ReadTemp) {
                    A_(printf("Error: OS_Free_Read_File failed (%d)\n\n", __LINE__);)
                    IO_ErrorCode = err_os_free;
                    ReadLength = IO_ERROR;
                    goto ErrorExit;
                }

                ReadCycle++;
            }

            ReadLength += ReadTemp;
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
        }
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
static uint64 GatheredPositionedFileWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 WriteLength = 0;
    uint64 WriteTemp = 0;
    uint32 WriteCycle = 0;
    uint32 FirstBuffer = 0;
    uint32 BufferOffset = 0;
    uint32 BufferLeft = 0;
    uint32 Counter = 0;
    uint32 FileHandle = 0;
    uint32 Buffers = 0;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((Length > MAX_UINT32) || (VectorIndex >= MaxOpenIO)) {
        A_(printf("Error: Invalid input parameters io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        IO_ErrorCode = E_INVALID_INPUT_PARAMETERS;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    WriteLength = OS_Free_Lseek(FileContext_p->Descriptor, (uint32)Offset, SEEK_SET);

    if (FS_ERROR == WriteLength) {
        A_(printf("Error: OS_Free_Lseek failed (%d)\n\n", __LINE__);)
        IO_ErrorCode = err_os_free;
        WriteLength = IO_ERROR;
        goto ErrorExit;
    }

    WriteLength = 0;

    Length = MIN(Length, VectorList[VectorIndex].Length);
    Buffers = (uint32)((Length + VectorList[VectorIndex].ChunkSize - 1) / VectorList[VectorIndex].ChunkSize);
    FirstBuffer = (uint32)(VectorList[VectorIndex].IOffset / VectorList[VectorIndex].ChunkSize);
    Buffers += FirstBuffer;
    BufferOffset = (uint32)(VectorList[VectorIndex].IOffset % VectorList[VectorIndex].ChunkSize);
    BufferLeft = VectorList[VectorIndex].ChunkSize - BufferOffset;

    for (Counter = FirstBuffer; Counter < Buffers; Counter++) {
        if (VectorList[VectorIndex].Entries[Counter].Status == IOV_VALID) {
            WriteTemp = 0;
            WriteCycle = 0;

            if (Counter == FirstBuffer) {
                if (BufferLeft < Length) {
                    while ((WriteTemp < BufferLeft) && (WriteCycle < MAX_CYCLES)) {
                        WriteTemp += OS_Free_Write_File(FileContext_p->Descriptor,
                                                        (char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + BufferOffset + WriteTemp,
                                                        (uint32)(BufferLeft - WriteTemp));

                        if (FS_ERROR == WriteTemp) {
                            A_(printf("Error: OS_Free_Write_File failed (%d)\n\n", __LINE__);)
                            IO_ErrorCode = err_os_free;
                            WriteLength = IO_ERROR;
                            goto ErrorExit;
                        }

                        WriteCycle++;
                    }

                    WriteLength += WriteTemp;
                } else {
                    while ((WriteTemp < Length) && (WriteCycle < MAX_CYCLES)) {
                        WriteTemp += OS_Free_Write_File(FileContext_p->Descriptor,
                                                        (char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + BufferOffset + WriteTemp,
                                                        (uint32)(Length - WriteTemp));

                        if (FS_ERROR == WriteTemp) {
                            A_(printf("Error: OS_Free_Write_File failed (%d)\n\n", __LINE__);)
                            IO_ErrorCode = err_os_free;
                            WriteLength = IO_ERROR;
                            goto ErrorExit;
                        }

                        WriteCycle++;
                    }

                    WriteLength += WriteTemp;
                }

            } else {
                if (Counter == (Buffers - 1)) {
                    while ((WriteTemp < (Length - WriteLength)) && (WriteCycle < MAX_CYCLES)) {
                        WriteTemp += OS_Free_Write_File(FileContext_p->Descriptor,
                                                        (char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + WriteTemp,
                                                        (uint32)(Length - WriteLength - WriteTemp));

                        if (FS_ERROR == WriteTemp) {
                            A_(printf("Error: OS_Free_Write_File failed (%d)\n\n", __LINE__);)
                            IO_ErrorCode = err_os_free;
                            WriteLength = IO_ERROR;
                            goto ErrorExit;
                        }

                        WriteCycle++;
                    }

                    WriteLength += WriteTemp;
                } else {
                    while ((WriteTemp < VectorList[VectorIndex].ChunkSize) && (WriteCycle < MAX_CYCLES)) {
                        WriteTemp += OS_Free_Write_File(FileContext_p->Descriptor,
                                                        (char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + WriteTemp,
                                                        (uint32)(VectorList[VectorIndex].ChunkSize - WriteTemp));

                        if (FS_ERROR == WriteTemp) {
                            A_(printf("Error: OS_Free_Write_File failed (%d)\n\n", __LINE__);)
                            IO_ErrorCode = err_os_free;
                            WriteLength = IO_ERROR;
                            goto ErrorExit;
                        }

                        WriteCycle++;
                    }

                    WriteLength += WriteTemp;
                }
            }
        }
    }

    VectorList[VectorIndex].Entries[Counter].Status = IOV_DONE;

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
static uint64 FileLength(const void *const Context_p)
{
    FileContext_t *FileContext_p = (FileContext_t *) Context_p;
    uint64 FileSize = 0;
    uint64 CurrentPosition = 0;
    uint32 FileHandle;

    if (FileContext_p == NULL) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        return IO_ERROR;
    }

    FileHandle = FileContext_p->Descriptor;

    if (FileHandle == IO_INVALID_HANDLE) {
        A_(printf("Error: Handle not valid io_file.c (%d)\n\n", __LINE__);)
        return IO_ERROR;
    }

    CurrentPosition = OS_Free_Lseek(FileHandle, 0, SEEK_CUR);
    FileSize = OS_Free_Lseek(FileHandle, 0, SEEK_END);
    CurrentPosition = OS_Free_Lseek(FileHandle, (uint32)CurrentPosition, SEEK_SET);

    if (FileSize == IO_ERROR) {
        A_(printf("Error while length of file io_file.c (%d)\n\n", __LINE__);)
    }

    return FileSize;
}

/**     @} */
/** @} */
