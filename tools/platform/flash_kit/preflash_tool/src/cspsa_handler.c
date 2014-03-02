/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_memory_utils.h"
#include "cspsa_plugin.h"
#include "cspsa_handler.h"

#define AREA_NAME "TEST_RAM_CSPSA"
#define MAX_UNIT32 0xFFFFFFFF

static CSPSA_Result_t F_CSPSA_RAM_Open(const CSPSA_Config_t *Config_p, void **Handle_pp);
static CSPSA_Result_t F_CSPSA_RAM_Close(void *Handle_p);
static CSPSA_Result_t F_CSPSA_RAM_Read(void *Handle_p, const uint64_t Offset, void *const Data_p, const uint32_t Size);
static CSPSA_Result_t F_CSPSA_RAM_Write(void *Handle_p, const uint64_t Offset, const void *const Data_p, const uint32_t Size);
static CSPSA_Result_t F_CSPSA_RAM_Format(void *Handle_p);
static CSPSA_Result_t F_CSPSA_RAM_Erase(void *Handle_p, const uint64_t Offset);

static const CSPSA_PluginOperations_t PLUGIN_IMPL = {
    F_CSPSA_RAM_Open,
    F_CSPSA_RAM_Close,
    F_CSPSA_RAM_Read,
    F_CSPSA_RAM_Write,
    F_CSPSA_RAM_Format,
    F_CSPSA_RAM_Erase
};

ErrorCode_e InitializeCSPSA(const uint32 Size, CSPSA_Handle_t *const Handle_p)
{
    CSPSA_Config_t CSPSA_Config = {0};

    // Register plugin operations
    if (CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_RAM, &PLUGIN_IMPL) != T_CSPSA_RESULT_OK) {
        return E_GENERAL_FATAL_ERROR;
    }

    // Initialize configuration with default values.
    CSPSA_InitConfig(&CSPSA_Config);
    CSPSA_Config.MemoryType = T_CSPSA_MEMORY_TYPE_RAM;
    CSPSA_Config.StartOffsetInBytes = (uint64_t) malloc(Size); // - Double casting to supress warning.
    memset((uint8_t *)(CSPSA_Config.StartOffsetInBytes), 0xFF, Size);
    CSPSA_Config.SizeInBytes = Size;
    // TODO: Investigate these sizes. Changing them causes different outputs
    CSPSA_Config.SectorSize = 512;//512;
    CSPSA_Config.BlockSize = 512;//1024;
    strncpy(CSPSA_Config.Name, AREA_NAME, D_CSPSA_MAX_NAME_SIZE);

    /* coverity[overrun-buffer-val] */
    if (CSPSA_AddParameterArea(&CSPSA_Config) != T_CSPSA_RESULT_OK) {
        free((uint8_t *)(CSPSA_Config.StartOffsetInBytes));
        return E_GENERAL_FATAL_ERROR;
    }

    if (CSPSA_Open(CSPSA_Config.Name, Handle_p) != T_CSPSA_RESULT_E_NO_VALID_IMAGE) {
        printf("Error opening CSPSA\n");
        free((uint8_t *)(CSPSA_Config.StartOffsetInBytes));
        return E_GENERAL_FATAL_ERROR;
    }

    if (CSPSA_Create(CSPSA_Config.Name, Handle_p) != T_CSPSA_RESULT_OK) {
        printf("Error creating CSPSA\n");
        free((uint8_t *)(CSPSA_Config.StartOffsetInBytes));
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

ErrorCode_e WriteCSPSA(const CSPSA_Handle_t Handle, const uint32 Index, const uint32 DataSize, uint8 *const Data_p)
{
    if (CSPSA_WriteValue(Handle, Index, DataSize, Data_p) > T_CSPSA_RESULT_ERRORS) {
        printf("Error writing to CSPSA\n");
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

ErrorCode_e FlushCSPSA(const CSPSA_Handle_t Handle)
{
    if (CSPSA_Flush(Handle) != T_CSPSA_RESULT_OK) {
        printf("Error flushing CSPSA\n");
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

ErrorCode_e ReadRawCSPSA(const CSPSA_Handle_t Handle, const uint32 Offset, uint8 *const Data_p, const uint32 Size)
{
    const CSPSA_Config_t *Config_p = Handle;
    uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset;

    memcpy(Data_p, (uint8 *) CurrentOffset, Size);

    return E_SUCCESS;
}

void DestroyCSPSA(CSPSA_Handle_t *const Handle_p)
{
    CSPSA_Config_t *Config_p = *Handle_p;

    CSPSA_RemoveParameterArea(Config_p);

    free((uint8_t *)(Config_p->StartOffsetInBytes));

    CSPSA_Close(Handle_p);
}

static CSPSA_Result_t F_CSPSA_RAM_Open(const CSPSA_Config_t *Config_p, void **Handle_pp)
{
    *Handle_pp = (void *)Config_p;
    return T_CSPSA_RESULT_OK;
}

static CSPSA_Result_t F_CSPSA_RAM_Close(void *Handle_p)
{
    (void)(Handle_p);
    return T_CSPSA_RESULT_OK;
}

static CSPSA_Result_t F_CSPSA_RAM_Read(void *Handle_p, const uint64_t Offset, void *const Data_p, const uint32_t Size)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    uint32_t BlockSize;
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
    uint64_t Offset32 = Offset & MAX_UNIT32; // to fix mysterious bug that Offset is sometimes is more than 4G...

    BlockSize = Config_p->BlockSize;

    if (Offset32 + Size > Config_p->SizeInBytes || Size > BlockSize) {
        Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
        goto ErrorExit;
    }

    {
        uint8_t *Data8_p = (uint8_t *)Data_p;
        uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;

        memcpy(Data8_p, (uint8_t *) CurrentOffset, Size);
    }

ErrorExit:
    return Result;
}

static CSPSA_Result_t F_CSPSA_RAM_Format(void *Handle_p)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

    memset((uint8_t *)(Config_p->StartOffsetInBytes), 0xFF, Config_p->SizeInBytes);

    Result = T_CSPSA_RESULT_OK;

    return Result;
}

static CSPSA_Result_t F_CSPSA_RAM_Write(void *Handle_p, const uint64_t Offset, const void *const Data_p, const uint32_t Size)
{
    const CSPSA_Config_t *Config_p = Handle_p;
    CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
    uint32_t BlockSize;
    uint64_t Offset32 = Offset & MAX_UNIT32; // to fix mysterious bug that Offset is sometimes is more than 4G...

    BlockSize = Config_p->BlockSize;

    if (Offset32 + Size > Config_p->SizeInBytes || Size > BlockSize) {
        Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
        goto ErrorExit;
    }

    {
        uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;
        uint8_t *Data8_p = (uint8_t *)Data_p;

        memcpy((uint8_t *)CurrentOffset, Data8_p, Size);
    }

ErrorExit:
    return Result;
}

static CSPSA_Result_t F_CSPSA_RAM_Erase(void *Handle_p, const uint64_t Offset)
{
    CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
    const CSPSA_Config_t *Config_p = Handle_p;

    memset((uint8_t *)(Config_p->StartOffsetInBytes + Offset), 0xFF, Config_p->BlockSize);

    Result = T_CSPSA_RESULT_OK;

    return Result;
}


