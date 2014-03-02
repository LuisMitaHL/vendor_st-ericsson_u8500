/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_flash_app Loader flash applications
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "r_memory_utils.h"
#ifdef CFG_ENABLE_LOADER_TYPE
#include "r_debug_macro.h"
#include "cpu_support.h"
#endif

#include "flash_process_file_info.h"
#include "t_flashlayout_parser.h"

/* vsnprintf declared as external to fix compile warning */
extern int vsnprintf(char *, size_t, const char *, va_list);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/* define the maximum amount of data for the flashing info buffer */
#define MAX_FLASH_INFO_SIZE (64 * 1024)

static char *FlashData_p = NULL;
static uint32 MaxInfoSize = 0;
static uint32 Size = 0;
static uint32 Offset = 0;
static char *OverflowPrint_p = "\n*** Flashing info buffer overflow! ***\n";
static boolean IsBufferOverflowed = FALSE;

/* indicates if the flashing info data buffer is initialized */
static boolean IsFlashInfoInitialized = FALSE;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void InitFlashInfo(unsigned int MaxSize);

void InitializeFlashInfo(void)
{
    InitFlashInfo(MAX_FLASH_INFO_SIZE);
}

void AddFlashInfoRecord(const char *format, ...)
{
    va_list args = {NULL};
    int iRetVal = 0;
    size_t OverflowBufferLen = 0;

    if (IsBufferOverflowed) {
        return;
    }

    va_start(args, format);

    if (FALSE == IsFlashInfoInitialized) {
        InitFlashInfo(MAX_FLASH_INFO_SIZE);
        IsFlashInfoInitialized = TRUE;
    }

    /* Append the new info string to the flash info data buffer */
    iRetVal = vsnprintf(FlashData_p + Offset, Size - 1, format, args);

    if (iRetVal != -1) {
        Size -= iRetVal;
        Offset += iRetVal;
    } else {
        OverflowBufferLen = strlen(OverflowPrint_p) + 1;
        vsnprintf(FlashData_p + MAX_FLASH_INFO_SIZE - OverflowBufferLen, OverflowBufferLen, OverflowPrint_p, args);
        Offset += OverflowBufferLen;
        IsBufferOverflowed = TRUE;
    }

    va_end(args);
}

uint32 Do_ReadFlashInfoData(void **Data_pp)
{
    uint32 ReturnValue = 0;

    if (IsFlashInfoInitialized) {
        ReturnValue = Offset;
        *Data_pp = (void *)malloc(ReturnValue);
        ASSERT(NULL != *Data_pp);

        memcpy(*Data_pp, FlashData_p, ReturnValue);
        BUFFER_FREE(FlashData_p);

        IsFlashInfoInitialized = FALSE;
        MaxInfoSize = 0;
        Size = 0;
        Offset = 0;
        IsBufferOverflowed = FALSE;
    }

    return ReturnValue;
}

void PrintTOCInfo(const TOC_List_t *It_p, ManifestParser_t *ManifestParser_p)
{
    uint32 i = 0;
    uint32 EntryPoint = 0;
    ManifestParser_t *MpIt = NULL;

    AddFlashInfoRecord("\n%s     %8s     %10s    %10s       %14s    %10s    %10s\n",
                       "Item No",
                       "Item",
                       "Start Addr",
                       "Size",
                       "Entry Point",
                       "Load Addr",
                       "Flags");

    AddFlashInfoRecord("----------------------------------------------------------------------------------------------------\n");

    while (NULL != It_p) {
        MpIt = ManifestParser_p;

        if ('\0' == It_p->entry.TOC_Id[0]) {
            break;
        } else {
            while (MpIt) {
                if (!strncmp(MpIt->TocID, It_p->entry.TOC_Id, strlen(MpIt->TocID))) {
                    EntryPoint = MpIt->TocEntryPoint;
                    break;
                }

                MpIt++;
            }

            AddFlashInfoRecord("  %3d    %12s    0x%08X    0x%08X Bytes     0x%08X     0x%08X     0x%08X\n", i++, It_p->entry.TOC_Id, It_p->entry.TOC_Offset,
                               It_p->entry.TOC_Size, 0, It_p->entry.TOC_LoadAddress , It_p->entry.TOC_Flags);
        }

        It_p = It_p->next;
    }
}

void PrintPhysicalAddress(const TOC_Entry_t *const Entry_p,  const FlashLayoutParser_t  *Flashlayout_p)
{
    if (NULL == Flashlayout_p) {
        return;
    }

    while ('\0' != Flashlayout_p->TOC_ID[0]) {
        if (!strncmp(Flashlayout_p->TOC_ID, Entry_p->TOC_Id, strlen(Entry_p->TOC_Id))) {
            AddFlashInfoRecord("# Writing to physical address: 0x%08X:0x%08X\n", Flashlayout_p->StartAddress, Flashlayout_p->EndAddress);
            break;
        }

        Flashlayout_p++;
    }
}

void VerifyAndRecord(boolean Condition, char *Info_p, ErrorCode_e ErrorCode)
{
    if (FALSE == Condition) {
        if (NULL != Info_p) {
            AddFlashInfoRecord("\n================ ERROR ================\n");
            AddFlashInfoRecord("\n%s\n", Info_p);

            if (E_SUCCESS != ErrorCode) {
                AddFlashInfoRecord("Error code: %d\n", ErrorCode);
            }

            AddFlashInfoRecord("\n================ ERROR ================\n");
        }
    }
}
/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * internal function used for initializing the flashing info buffer
 * prior any attempt to write data in it.
 *
 */
static void InitFlashInfo(unsigned int MaxSize)
{
    BUFFER_FREE(FlashData_p);

    Size = MaxSize - strlen(OverflowPrint_p);
    MaxInfoSize = MaxSize;

    FlashData_p = (char *)malloc(MaxInfoSize);
    ASSERT(NULL != FlashData_p);

    memset(FlashData_p, 0, MaxSize);
    Offset = 0;
    IsBufferOverflowed = FALSE;
}

/* @} */
/* @} */
