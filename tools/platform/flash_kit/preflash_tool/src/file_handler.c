/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
#include "r_basicdefinitions.h"
#include "file_handler.h"
#include "t_loader_sec_lib.h"
#include "flash_manager.h"
#include "io_subsystem.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

static uint32 FileClose(void *Context_p);
static uint64 FileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 FileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 PositionedFileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 PositionedFileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 ScatteredPositionedFileRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 GatheredPositionedFileWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 FileLength(const void *const Context_p);
static uint32 FileCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);

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

ErrorCode_e FileOpen(char *const FileName_p, char *const Mode_p, IO_Handle_t *const IOHandle_p)
{
    FILE *FileDescriptor = NULL;

    FileDescriptor = fopen(FileName_p, Mode_p);

    if (FileDescriptor == NULL) {
        printf("Error while opening file: '%s' file_handler.c (%d)\n\n", FileName_p, __LINE__);
        return E_GENERAL_FATAL_ERROR;
    }


    *IOHandle_p = IO_Create(&FileInterface, (void *) FileDescriptor);

    if (*IOHandle_p == IO_INVALID_HANDLE) {
        fclose(FileDescriptor);
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

static uint32 FileClose(void *Context_p)
{
    FILE *FileDescriptor = (FILE *) Context_p;
    uint32 RetVal;

    RetVal = fclose(FileDescriptor);
    return RetVal;
}

static uint64 FileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    printf("FileRead not implemented!\n");
    return 0;
}

static uint64 FileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    printf("FileWrite not implemented!\n");
    return 0;
}

static uint64 PositionedFileRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FILE *FileDescriptor = (FILE *)Context_p;
    uint64 ReadLength = 0;

    if (Length > MAX_UINT32) {
        printf("Error: Invalid input parameters file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == NULL) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == (FILE *) IO_INVALID_HANDLE) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (fseek(FileDescriptor, (uint32) Offset, SEEK_SET) != 0) {
        printf("Error: fseek failed (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    ReadLength = fread((char *)Buffer_p, 1, (uint32)Length, FileDescriptor);

    if (ReadLength != Length) {
        printf("Error: fread failed (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    return ReadLength;
}

static uint64 PositionedFileWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    FILE *FileDescriptor = (FILE *)Context_p;
    uint64 BytesWritten;

    if (Length > MAX_UINT32) {
        printf("Error: Invalid input parameters file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == NULL) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == (FILE *) IO_INVALID_HANDLE) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (fseek(FileDescriptor, (uint32) Offset, SEEK_SET) != 0) {
        printf("Error: fseek failed (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    BytesWritten = fwrite(Buffer_p, 1, Length, FileDescriptor);

    if (BytesWritten != Length) {
        printf("Error: fwrite failed (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    return BytesWritten;
}

static uint64 ScatteredPositionedFileRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    FILE *FileDescriptor = (FILE *) Context_p;
    uint64 ReadLength = 0;
    uint64 ReadTemp = 0;
    uint32 ReadCycle = 0;
    uint32 Counter = 0;
    uint64 BytesRead;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((Length > MAX_UINT32) || (VectorIndex >= MaxOpenIO)) {
        printf("Error: Invalid input parameters file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == NULL) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (FileDescriptor == (FILE *)IO_INVALID_HANDLE) {
        printf("Error: Handle not valid file_handler.c (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    if (fseek(FileDescriptor, (uint32)Offset, SEEK_SET) != 0) {
        printf("Error: fseek failed (%d)\n\n", __LINE__);
        return IO_ERROR;
    }

    for (Counter = 0; ((Counter < MaxOpenIO) && (ReadLength < Length)); Counter++) {
        if ((VectorList[VectorIndex].Entries[Counter].Status == IOV_MAPPED)) {
            ReadTemp = 0;
            ReadCycle = 0;

            while ((ReadTemp < MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLength))) && (ReadCycle < MAX_CYCLES)) {
                BytesRead = fread((char *)(VectorList[VectorIndex].Entries[Counter].Buffer_p) + ReadTemp, 1,
                                  (uint32)(MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLength)) - ReadTemp), FileDescriptor);

                if (BytesRead != (MIN(VectorList[VectorIndex].ChunkSize, (Length - ReadLength)) - ReadTemp)) {
                    printf("Error: fread failed (%d)\n\n", __LINE__);
                    return IO_ERROR;
                }

                ReadTemp += BytesRead;

                ReadCycle++;
            }

            ReadLength += ReadTemp;
            VectorList[VectorIndex].Entries[Counter].Status = IOV_VALID;
        }
    }

    return ReadLength;
}

static uint64 GatheredPositionedFileWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    printf("GatheredPositionedFileWrite not implemented!\n");
    return 0;
}

static uint64 FileLength(const void *const Context_p)
{
    uint64 FileLength = 0;
    FILE *FileDescriptor = (FILE *) Context_p;

    if (fseek(FileDescriptor, 0 , SEEK_END) != 0) {
        return IO_ERROR;
    }

    FileLength = ftell(FileDescriptor);

    if (FileLength == -1L) {
        return IO_ERROR;
    }

    if (fseek(FileDescriptor, 0 , SEEK_SET) != 0) {
        return IO_ERROR;
    }

    return FileLength;
}

static uint32 FileCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    uint32 Counter;
    uint32 VectorCounter;
    uint32 NrOfBuffers;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if ((BuffSize != 0) && (BuffSize < MAX_UINT32) && (Length < MAX_UINT32)) {
        NrOfBuffers = (uint32)((Length + BuffSize - 1) / BuffSize);
    } else {
        printf("Error: Invalid input parameters file_handler.c (%d)\n\n", __LINE__);
        return IO_VECTOR_INVALID_HANDLE;
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
        printf("Not enough buffers file_handler.c (%d)\n\n", __LINE__);
        return IO_VECTOR_INVALID_HANDLE;
    }

    return VectorCounter;
}
