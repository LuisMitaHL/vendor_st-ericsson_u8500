/* **************************************************************************
 *
 * cspsa_plugin.c
 *
 * Copyright (C) 2010 ST-Ericsson AB
 * Initial author: 2010, Louis Verhaard <louis.xl.verhaard@stericsson.com>
 *
 * DESCRIPTION:
 * CSPSA interface to eMMC.
 *
 **************************************************************************** */
// =============================================================================
//  Include Header Files
// =============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "flash_manager.h"
#include "boot_area_management.h"
#include "r_io_subsystem.h"
#include "cspsa.h"
#include "cspsa_plugin.h"

static uint8 FlashDevice = 0xFF;

void CSPSA_LL_EMMC_SetFlashDevice(uint8 FlashDev);

// =============================================================================
//  PLUGIN IMPLEMENTATION
// =============================================================================

/**
 * Converts a generic loader error code to a corresponding CSPSA error code.
 */
static CSPSA_Result_t ConvertErrorCode(ErrorCode_e ErrorCode)
{
    switch (ErrorCode) {
    case E_SUCCESS:
        return T_CSPSA_RESULT_OK;
    default:
        return T_CSPSA_RESULT_UNDEFINED;
    }
}

//*****************************************************************************
// Opens a flash memory device.
//*****************************************************************************
/*
 * Opens a flash memory device.
 */
CSPSA_Result_t CSPSA_LL_EMMC_Open(const CSPSA_Config_t *Config_p, void **Handle_pp)
{
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;

    // Only the Config_p parameter is needed in subsequent calls to read/write, so use Config_p as the handle.
    *Handle_pp = (void *)Config_p;
    return Result;
}

/*
 * Closes a flash memory device.
 */
static CSPSA_Result_t CSPSA_LL_EMMC_Close(void *Handle_p)
{
    (void)(Handle_p);
    return T_CSPSA_RESULT_OK;
}

/*
 * Reads data from storage media.
 */
static CSPSA_Result_t CSPSA_LL_EMMC_Read(
    void *Handle_p,
    const uint64_t Offset,
    void *const Data_p,
    const uint32_t Size)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    uint32_t PageSize;
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
    uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

    PageSize = Config_p->SectorSize;

    if (Offset > 0xffffffff) {
        A_(printf("  ERROR: OFFSET TOO HIGH!!!\n");)
    }

    if (Offset32 + Size > Config_p->SizeInBytes || Size > PageSize) {
        A_(printf("  Size or offset parameter incorrect!\n");)
        Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
        goto ErrorExit;
    }

    {
        IO_Handle_t Handle = IO_INVALID_HANDLE;
        uint8_t *Data8_p = (uint8_t *)Data_p;
        uint64_t Bytes = 0;
        uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;

        Handle = Do_IOW_FPDOpen(0, FlashDevice);

        if (IO_INVALID_HANDLE == Handle) {
            A_(printf("CSPSA_LL_EMMC_Read: IO failed to open handle!\n");)
            Result = T_CSPSA_RESULT_E_READ_ERROR;
            goto ErrorExit;
        }

        Bytes = Do_IO_PositionedRead(Handle, Data8_p, Size, Size, CurrentOffset);

        if (Bytes != Size) {
            A_(printf("CSPSA_LL_EMMC_Read: Read failed!\n");)
            Result = T_CSPSA_RESULT_E_READ_ERROR;
        }

        if (IO_INVALID_HANDLE == Do_IO_Close(Handle)) {
            A_(printf("CSPSA_LL_EMMC_Read: IO failed to close handle!\n");)
            Result = T_CSPSA_RESULT_E_READ_ERROR;
        }
    }

ErrorExit:
    return Result;
}

/*
 * Writes to storage media.
 */
static CSPSA_Result_t CSPSA_LL_EMMC_Write(
    void *Handle_p,
    const uint64_t Offset,
    const void *const Data_p,
    const uint32_t Size)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
    uint32_t PageSize;
    uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

    PageSize = Config_p->SectorSize;

    if (Offset > 0xffffffff) {
        A_(printf("  ERROR: OFFSET TOO HIGH!!!\n");)
    }

    if (Offset32 + Size > Config_p->SizeInBytes || Size > PageSize) {
        A_(printf("  Size or offset parameter incorrect!\n");)
        Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
        goto ErrorExit;
    }

    {
        uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;
        uint8_t *Data8_p = (uint8_t *)Data_p;
        uint32_t PageOffset = CurrentOffset / PageSize;
        ErrorCode_e ErrorCode = E_SUCCESS;

        // Unfortunately Do_IO_PositionedWrite was not implemented when this code was produced,
        // so Do_FDP_WritePage is used instead
        if (Size == PageSize) {
            C_(printf("   Do_FPD_WritePage, PageOffset: 0x%x, write whole page\n", PageOffset);)
            ErrorCode = Do_FPD_WritePage(PageOffset, FlashDevice, Data8_p, NULL);
        } else {
            // Less than one page is to be written.
            // Create a page-size temporary buffer, copy the data to this buffer and write it to EMMC
            uint8 *TempPage_p = malloc(PageSize);

            if (TempPage_p != NULL) {
                memset(TempPage_p, 0xFF, PageSize);
                memcpy(TempPage_p, Data8_p, Size);
                ErrorCode = Do_FPD_WritePage(PageOffset, FlashDevice, TempPage_p, NULL);
                C_(printf("   Do_FPD_WritePage, PageOffset: 0x%x, write %d bytes\n", PageOffset, Size);)
                free(TempPage_p);
            } else {
                Result = T_CSPSA_RESULT_E_OUT_OF_SPACE;
            }
        }

        if (ErrorCode != E_SUCCESS) {
            A_(printf("CSPSA_LL_EMMC_Write: Write failed, error code: %d\n", ErrorCode);)
        }

        Result = ConvertErrorCode(ErrorCode);
    }

ErrorExit:
    return Result;
}

/*
 * Erases media.
 */
static CSPSA_Result_t CSPSA_LL_EMMC_Erase(
    void *Handle_p,
    const uint64_t Offset
)
{
    CSPSA_Result_t Result;
    char Data = 0xFF;

    //
    // Implement this function using the Write function (knowing that it will write a whole block)
    //
    Result = CSPSA_LL_EMMC_Write(Handle_p, Offset, &Data, sizeof(char));
    return Result;
}

/*
 * Formats (erases) media.
 */
static CSPSA_Result_t CSPSA_LL_EMMC_Format(void *Handle_p)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    uint64_t Offset;
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;

    // implement using Erase
    for (Offset = 0; Offset < Config_p->SizeInBytes; Offset += Config_p->BlockSize) {
        CSPSA_Result_t Result = CSPSA_LL_EMMC_Erase(Handle_p, Offset);

        if (Result != T_CSPSA_RESULT_OK) {
            goto ErrorExit;
        }
    }

ErrorExit:
    return Result;
}


/**
 * Implementation of the CSPSA plug-in operations
 */
static const CSPSA_PluginOperations_t PLUGIN_IMPL = {
    CSPSA_LL_EMMC_Open,
    CSPSA_LL_EMMC_Close,
    CSPSA_LL_EMMC_Read,
    CSPSA_LL_EMMC_Write,
    CSPSA_LL_EMMC_Format,
    CSPSA_LL_EMMC_Erase,
};

// =============================================================================
//  PUBLIC FUNCTIONS
// =============================================================================
//*****************************************************************************
// Register a plug-in functions to CSPSA.
//*****************************************************************************
void CSPSA_LL_EMMC_Init(void)
{
    (void)CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_EMMC, &PLUGIN_IMPL);
}

void CSPSA_LL_EMMC_SetFlashDevice(uint8 FlashDev)
{
    FlashDevice = FlashDev;
}
