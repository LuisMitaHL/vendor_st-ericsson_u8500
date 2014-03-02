
#include <stdlib.h>
#include <string.h>
#include "r_io_subsystem.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "gdf_converter.h"

typedef enum {
    FILE_OPEN = 0,
    FILE_HEADER,
    FILE_DATA
} FileState_t;

static void CreateList(uint8 *const FileBuffer_p, GDDataNode_t **const Root_pp);
static uint32 GetIndex(uint8 *const Line_p);
static uint32 GetDataSize(uint8 *const Line_p);
static uint32 GetBlock(uint8 *const Line_p);

ErrorCode_e GetGDFList(const IO_Handle_t IOHandle, GDDataNode_t **const Root_pp)
{
    uint64 FileLength;
    uint8 *FileBuffer_p = NULL;
    ErrorCode_e Result = E_SUCCESS;

    // Create buffer to store file
    FileLength = Do_IO_GetLength(IOHandle);

    if (FileLength == IO_ERROR) {
        return E_GENERAL_FATAL_ERROR;
    }

    FileBuffer_p = (uint8 *)malloc(FileLength);
    ASSERT(NULL != FileBuffer_p);

    // Read file to buffer
    if (Do_IO_PositionedRead(IOHandle, FileBuffer_p, FileLength, FileLength, 0) == IO_ERROR) {
        printf("gdf_converter.c(%d): Could not read file\n", __LINE__);
        Result =  E_GENERAL_FATAL_ERROR;
    }

    if (Result == E_SUCCESS) {
        CreateList(FileBuffer_p, Root_pp);
    }

    BUFFER_FREE(FileBuffer_p);

    return Result;
}

void DestroyGDFList(GDDataNode_t **const Root_pp)
{
    GDDataNode_t *Iter_p = *Root_pp;

    while (Iter_p) {
        GDData_t *GDData_p = Iter_p->GDData_p;
        GDDataNode_t *Current_p = Iter_p;
        Iter_p = Iter_p->Next_p;
        BUFFER_FREE(GDData_p);
        BUFFER_FREE(Current_p);
    }

    *Root_pp = NULL;
}

static void CreateList(uint8 *const FileBuffer_p, GDDataNode_t **const Root_pp)
{
    GDData_t *GDData_p;
    GDDataNode_t *Root_p = NULL;
    GDDataNode_t *Iter_p = NULL;
    FileState_t State = FILE_OPEN;

    uint16 DataCount = 0;
    uint8 Line[128];
    uint8 *End_p;
    uint8 *Start_p = FileBuffer_p;

    // Read from buffer line by line
    End_p = (uint8 *) strchr((const char *) Start_p, '\n');

    while (End_p != NULL) {
        // Trim beginning
        while (*Start_p == ' ' || *Start_p == '\t') {
            Start_p++;
        }

        // Copy line to new buffer and add space if neccessary
        strncpy((char *) Line, (const char *) Start_p, End_p - Start_p);

        if (Line[End_p - Start_p - 1] != ' ') {
            Line[End_p - Start_p] = ' ';
            Line[End_p - Start_p + 1] = '\0';
        } else {
            Line[End_p - Start_p] = '\0';
        }

        // Skip comments
        if (State == FILE_OPEN && Line[0] != '/') {
            State = FILE_HEADER;
        }

        // Get index, data size and block
        if (State == FILE_HEADER) {
            uint16 DataSize = GetDataSize(Line);
            GDData_p = malloc(sizeof(GDData_t) + DataSize);
            ASSERT(NULL != GDData_p);
            GDData_p->Index = GetIndex(Line);
            GDData_p->Block = GetBlock(Line);
            GDData_p->DataSize = DataSize;
            State = FILE_DATA;
        } else if (State == FILE_DATA) { // Get data
            uint8 *Start2_p = Line;
            uint8 *End2_p = (uint8 *) strchr((const char *) Start2_p, ' ');

            while (End2_p != NULL) {
                GDData_p->Data[DataCount++] = strtoul((const char *) Start2_p, NULL, 16);
                Start2_p = End2_p + 1;
                End2_p = (uint8 *) strchr((const char *) Start2_p, ' ');
            }

            // Create node in list
            if (DataCount == GDData_p->DataSize) {
                DataCount = 0;
                State = FILE_HEADER;

                /* coverity[dead_error_condition] */
                if (Root_p == NULL) {
                    Root_p = malloc(sizeof(GDDataNode_t));
                    ASSERT(NULL != Root_p);
                    Root_p->Next_p = NULL;
                    Root_p->GDData_p = GDData_p;
                    Iter_p = Root_p;
                } else {
                    Iter_p->Next_p = malloc(sizeof(GDDataNode_t));
                    ASSERT(NULL != Iter_p->Next_p);
                    Iter_p = Iter_p->Next_p;
                    Iter_p->Next_p = NULL;
                    Iter_p->GDData_p = GDData_p;
                }
            }
        }

        Start_p = End_p + 1;
        End_p = (uint8 *) strchr((const char *) Start_p, '\n');
    }

    *Root_pp = Root_p;
    /* coverity[leaked_storage] */
}

static uint32 GetIndex(uint8 *const Line_p)
{
    return strtoul((const char *) Line_p, NULL, 16);
}

static uint32 GetDataSize(uint8 *const Line_p)
{
    uint8 *Start_p = (uint8 *) strchr((const char *) Line_p, ' ');

    if (Start_p != NULL) {
        return strtoul((const char *) Start_p, NULL, 10);
    }

    printf("gdf_converter.c(%d): Could net get Data Size\n", __LINE__);
    return 0;
}

static uint32 GetBlock(uint8 *const Line_p)
{
    uint8 Count = 0;
    uint8 *Start_p = (uint8 *) strchr((const char *) Line_p, ' ');

    while (Start_p != NULL) {
        if (Count == 1) {
            return strtoul((const char *) Start_p, NULL, 16);
        }

        Start_p++;
        Start_p = (uint8 *) strchr((const char *) Start_p, ' ');
        Count++;
    }

    printf("gdf_converter.c(%d): Could net get Block\n", __LINE__);
    return 0;
}

