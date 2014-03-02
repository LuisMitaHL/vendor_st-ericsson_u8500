/* **************************************************************************
 *
 * cspsa_ll_emmc.c
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Louis Verhaard <louis.xl.verhaard@stericsson.com>
 *
 * DESCRIPTION:
 * CSPSA interface to eMMC.
 *
 * This file is OBSOLETE and serves only as example on how to use CSPSA in
 *  os/server free environment.
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "cspsa.h"
#include "cspsa_plugin.h"

#include "flash_manager.h"
#include "boot_area_management.h"
#include "toc_handler.h"
#include "r_io_subsystem.h"
#include "r_debug.h"
#include "r_debug_macro.h"

/** Id of TOC entry that points to trim area storage */
#define TRIM_AREA_TOC_ID "CSPSA"


static FPD_Config_t FPD_Config;

// =============================================================================
//  DEBUG FUNCTIONS (should normally not be enabled)
// =============================================================================

static void Enter(const char* FunctionName_p)
{
  B_(printf("Enter %s\n", FunctionName_p);)
}

static void Exit(const char* FunctionName_p, CSPSA_Result_t Result)
{
  B_(printf("Exit %s, returns %d\n", FunctionName_p, Result);)
}

#ifdef PRINT_A_
/** helper for printing uint64 values */
static uint32_t HiWord(uint64_t Value)
{
  return (uint32_t)((Value >> 32)&0xFFFFFFFF);
}

/** helper for printing uint64 values */
static uint32_t LoWord(uint64_t Value)
{
  return (uint32_t)(Value & 0xFFFFFFFF);
}
#endif // PRINT_A_

/** hex dumps a memory buffer */
static void HexDump(char* Data_p, size_t Size)
{
#ifdef PRINT_C_
  size_t PerLine = 16;
  size_t i, j;

  for (i = 0; i < Size; i+=PerLine)
  {
    printf("0x%04x ", i);
    for (j = 0; j < PerLine; ++j)
    {
      if (i + j >= Size)
      {
        printf("   ");
      }
      else
      {
        printf("%02x ", Data_p[i+j]);
      }
    }
    for (j = 0; j < PerLine; ++j)
    {
      if (i + j >= Size)
      {
        printf(" ");
      }
      else
      {
        char c = Data_p[i+j];
        if (c < ' ' || c > 'z')
        {
          printf(".");
        }
        else
        {
          printf("%c", c);
        }
      }
    }
    printf("\n");
  }
#endif // PRINT_C_
}

// =============================================================================
//  HELPER FUNCTIONS
// =============================================================================

/**
 * Converts a generic loader error code to a corresponding CSPSA error code.
 */
static CSPSA_Result_t ConvertErrorCode(ErrorCode_e ErrorCode)
{
  switch (ErrorCode)
  {
  case E_SUCCESS: return T_CSPSA_RESULT_OK;
  default: return T_CSPSA_RESULT_UNDEFINED;
  }
}

/**
 * Registers the parameter storage area associated with the given TOC entry to CSPSA.
 * Name that is registered is <DeviceName>/TOC/<TOC id name>
 */
static CSPSA_Result_t RegisterTOC_Entry(const char* DeviceName_p, const FPD_Config_t *FPD_Config_p, const TOC_List_t* TOC_Entry_p)
{
  uint32_t PageSize = FPD_Config_p->PageSize;
  CSPSA_Config_t* CSPSA_Config_p = malloc(sizeof(CSPSA_Config_t));
  CSPSA_Result_t Result;

  if (CSPSA_Config_p == NULL)
  {
    Result = T_CSPSA_RESULT_E_OUT_OF_MEMORY;
    goto ErrorExit;
  }
  CSPSA_InitConfig(CSPSA_Config_p);
  CSPSA_Config_p->MemoryType = T_CSPSA_MEMORY_TYPE_EMMC;
  // make sure StartOffset is page aligned (page size must be a power of 2)
  CSPSA_Config_p->StartOffsetInBytes = (TOC_Entry_p->entry.TOC_Offset + (PageSize - 1)) & ~(PageSize - 1) ;
  CSPSA_Config_p->SizeInBytes = (uint32_t)(TOC_Entry_p->entry.TOC_Offset + TOC_Entry_p->entry.TOC_Size - CSPSA_Config_p->StartOffsetInBytes);
  // make sure SizeInBytes is page aligned
  CSPSA_Config_p->SizeInBytes = CSPSA_Config_p->SizeInBytes & ~(PageSize - 1);
  CSPSA_Config_p->SectorSize = PageSize;
  // there is no erase block concept in eMMC; use same block size as sector size
  CSPSA_Config_p->BlockSize = PageSize;
  sprintf(CSPSA_Config_p->Name, "%s/TOC/%s", DeviceName_p, TOC_Entry_p->entry.TOC_Id);
  C_(printf("  Config: size: 0x%x, offset: 0x%x, page size: %u\n", CSPSA_Config_p->SizeInBytes, LoWord(CSPSA_Config_p->StartOffsetInBytes), PageSize);)
  // Registers the found parameter storage area to CSPSA
  Result = CSPSA_AddParameterArea(CSPSA_Config_p);
ErrorExit:
  if (Result != T_CSPSA_RESULT_OK)
  {
    A_(printf("CSPSA_EMMC: Error in registration of CSPSA area %s, error code %d\n", TOC_Entry_p->entry.TOC_Id, Result);)
  }
  return Result;
}

// =============================================================================
//  PLUGIN IMPLEMENTATION
// =============================================================================

/** Opens a flash memory device. */
CSPSA_Result_t CSPSA_LL_EMMC_Open(const CSPSA_Config_t* Config_p, void** Handle_pp)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;

  Enter("CSPSA_LL_EMMC_Open");
  // Only the Config_p parameter is needed in subsequent calls to read/write, so use Config_p as the handle.
  *Handle_pp = (void*)Config_p;
  return Result;
}

/**  Closes a flash memory device. */
static CSPSA_Result_t CSPSA_LL_EMMC_Close(void* Handle_p)
{
  (void)(Handle_p);
  Enter("CSPSA_LL_EMMC_Close");
  return T_CSPSA_RESULT_OK;
}


/** Reads data from storage media. */
static CSPSA_Result_t CSPSA_LL_EMMC_Read(
                    void* Handle_p,
                    const uint64_t Offset,
                    void* const Data_p,
                    const uint32_t Size)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  uint32_t PageSize;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

  Enter("CSPSA_LL_EMMC_Read");
  B_(printf("  Data_p: 0x%x, Size: 0x%x, ", Data_p, Size);)
  B_(printf("Offset = 0x%x%08x\n", HiWord(Offset),  LoWord(Offset));)
  PageSize = Config_p->SectorSize;
  if (Offset > 0xffffffff)
  {
    A_(printf("  ERROR: OFFSET TOO HIGH!!!\n");)
  }
  if (Offset32 + Size > Config_p->SizeInBytes || Size > PageSize)
  {
    A_(printf("  Size or offset paramater incorrect!\n");)
    Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
    goto ErrorExit;
  }
  {
    IO_Handle_t Handle = Do_IOW_FPDOpen();
    uint8_t* Data8_p = (uint8_t*)Data_p;
    uint64_t Bytes;

    uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;
    C_(printf("    Do_IO_PositionedRead, Data=0x%x, Size=0x%x, ", Data8_p, Size);)
    C_(printf("CurrentOffset = 0x%x%08x\n", HiWord(CurrentOffset),  LoWord(CurrentOffset));)
    Bytes = Do_IO_PositionedRead(Handle, Data8_p, Size, Size, CurrentOffset);
    C_(printf("    Bytes read: 0x%x%08x\n", HiWord(Bytes), LoWord(Bytes));)
    if (Bytes != Size)
    {
      A_(printf("CSPSA_LL_EMMC_Read: Read failed!\n");)
      Result = T_CSPSA_RESULT_E_READ_ERROR;
    }
    else
    {
      HexDump(Data8_p, Size < 128 ? Size : 128);
    }
  }
ErrorExit:
  Exit("CSPSA_LL_EMMC_Read", Result);
  return Result;
}

/** Writes to storage media. */
static CSPSA_Result_t CSPSA_LL_EMMC_Write(
                    void* Handle_p,
                    const uint64_t Offset,
                    const void* const Data_p,
                    const uint32_t Size)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  uint32_t PageSize;
  uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

  Enter("CSPSA_LL_EMMC_Write");
  B_(printf("  Data_p: 0x%x, Size: 0x%x ", Data_p, Size);)
  B_(printf("Offset = 0x%x%08x\n", HiWord(Offset),  LoWord(Offset));)
  PageSize = Config_p->SectorSize;

  if (Offset > 0xffffffff)
  {
    A_(printf("  ERROR: OFFSET TOO HIGH!!!\n");)
  }
  if (Offset32 + Size > Config_p->SizeInBytes || Size > PageSize)
  {
    A_(printf("  Size or offset paramater incorrect!\n");)
    Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
    goto ErrorExit;
  }
  {
    uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;
    uint8_t* Data8_p = (uint8_t*)Data_p;
    uint64_t Bytes;
    uint32_t PageOffset = CurrentOffset / PageSize;
    ErrorCode_e ErrorCode = E_SUCCESS;

    // Unfortunately Do_IO_PositionedWrite was not implemented when this code was produced,
    // so Do_FDP_WritePage is used instead
    if (Size == PageSize)
    {
      C_(printf("   Do_FPD_WritePage, PageOffset: 0x%x, write whole page\n", PageOffset);)
      ErrorCode = Do_FPD_WritePage(PageOffset, Data8_p, NULL);
    }
    else
    {
      // Less than one page is to be written.
      // Create a page-size temporary buffer, copy the data to this buffer and write it to EMMC
      uint8* TempPage_p = malloc(PageSize);
      if (TempPage_p != NULL)
      {
        memset(TempPage_p, 0xFF, PageSize);
        memcpy(TempPage_p, Data8_p, Size);
        ErrorCode = Do_FPD_WritePage(PageOffset, TempPage_p, NULL);
        C_(printf("   Do_FPD_WritePage, PageOffset: 0x%x, write %u bytes\n", PageOffset, Size);)
        free(TempPage_p);
      }
      else
      {
        Result = T_CSPSA_RESULT_E_OUT_OF_SPACE;
      }
    }
    if (ErrorCode != E_SUCCESS)
    {
      A_(printf("CSPSA_LL_EMMC_Write: Write failed, error code: %d\n", ErrorCode);)
    }
    Result = ConvertErrorCode(ErrorCode);
  }
ErrorExit:
  Exit("CSPSA_LL_EMMC_Write", Result);
  return Result;
}

static CSPSA_Result_t CSPSA_LL_EMMC_Erase(
                    void* Handle_p,
                    const uint64_t Offset
)
{
  CSPSA_Result_t Result;
  char Data = 0xFF;

  Enter("CSPSA_LL_EMMC_Erase");
  // implement this function using the Write function (knowing that it will write a whole block)
  Result = CSPSA_LL_EMMC_Write(Handle_p, Offset, &Data, sizeof(char));
  return Result;
}

/** Formats (erases) media. */
static CSPSA_Result_t CSPSA_LL_EMMC_Format(void* Handle_p)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  uint64_t Offset;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;

  Enter("CSPSA_LL_EMMC_Format");
  // implement using Erase
  for (Offset = 0; Offset < Config_p->SizeInBytes; Offset += Config_p->BlockSize)
  {
    CSPSA_Result_t Result = CSPSA_LL_EMMC_Erase(Handle_p, Offset);
    if (Result != T_CSPSA_RESULT_OK)
    {
      goto ErrorExit;
    }
  }
ErrorExit:
  return Result;
}

/**
 * Implementation of the CSPSA plugin operations
 */
static const CSPSA_PluginOperations_t PLUGIN_IMPL =
{
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


/** Registers the EMMC plugin to CSPSA. */
void CSPSA_LL_EMMC_Init(void)
{
  CSPSA_Result_t Result;

  Enter("CSPSA_LL_EMMC_Init");
  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_EMMC, &PLUGIN_IMPL);
  Exit("CSPSA_LL_EMMC_Init", Result);
}

/** Finds storage areas by parsing the TOC, registers all found areas to CSPSA */
CSPSA_Result_t RegisterEMMC_Areas(const char* DeviceName_p)
{
  CSPSA_Result_t Result;
  TOC_List_t* TOC_List_p = NULL;
  ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
  uint8* BootArea_p = NULL;
  int FoundAnything = 0;

  Enter("RegisterEMMC_Areas");
  /*
   * Get flash devices from services.
   */
  ReturnValue = Do_FPD_GetInfo(&FPD_Config);
  VERIFY(ReturnValue == E_SUCCESS, ReturnValue);

  // read the second copy of boot area, first may contain MBR
  BootArea_p = (uint8*)malloc(BAM_BOOT_IMAGE_COPY_SIZE);
  VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);
  ReturnValue = Do_BAM_Read(0, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE);
  VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

  TOC_List_p = Do_TOC_Create_RootList(BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_1);
  if (NULL != TOC_List_p)
  {
    // we have TOC on flash, try to read SubTOC
    //ReturnValue = ReadSubTOCFromFlash(TOC_List_p);
    //VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
  }
  TOC_List_t* TOC_Entry_p = TOC_List_p;
  if (NULL != TOC_Entry_p)
  {
    do
    {
      // check if TOC ID starts with TRIM_AREA_TOC_ID
      if (strncmp(TRIM_AREA_TOC_ID, TOC_Entry_p->entry.TOC_Id, strlen(TRIM_AREA_TOC_ID)) == 0)
      {
        C_(printf("  Found trim area %s in TOC, offset=0x%x, size=0x%x\n", TOC_Entry_p->entry.TOC_Id, (uint32_t)TOC_Entry_p->entry.TOC_Offset, TOC_Entry_p->entry.TOC_Size);)
        (void)RegisterTOC_Entry(DeviceName_p, &FPD_Config, TOC_Entry_p);
        FoundAnything = 1;
      }
    }
    while ((TOC_Entry_p = Do_TOC_FindNextEntry(TOC_Entry_p)) != NULL);
  }
  if (!FoundAnything)
  {
    A_(printf("No TRIMAREA TOC entry found\n");)
    ReturnValue = E_GENERAL_FATAL_ERROR;
  }
ErrorExit:

  Result = ConvertErrorCode(ReturnValue);
  Exit("RegisterEMMC_Areas", Result);
  return Result;
}

