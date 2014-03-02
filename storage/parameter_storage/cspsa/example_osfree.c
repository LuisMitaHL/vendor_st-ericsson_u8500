/*******************************************************************************
 *
 * example_osfree.c
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * Example of using CSPSA in Loaders environment, using eMMC.
 ******************************************************************************/


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "r_debug.h"
#include "r_debug_macro.h"
#include "flash_manager.h"
#include "cspsa.h"
#include "cspsa_ll_emmc.h"

/** hex dumps a memory buffer */
static void HexDump(char* Data_p, size_t Size)
{
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
}

/** Writes a trim area parameter that gets the given string as value */
void WriteString(const CSPSA_Handle_t Handle, const CSPSA_Key_t Key, const char* String_p)
{
  CSPSA_Result_t Result = CSPSA_WriteValue(Handle, Key, strlen(String_p) + 1, (CSPSA_Data_t*)String_p);
  printf("  WriteString %u, %s -> %d\n", Key, String_p, Result);
}

/** Writes a trim area parameter of the given size, with all bytes set to the given character */
void WriteBlock(const CSPSA_Handle_t Handle, const CSPSA_Key_t Key, const char c, uint32_t Size)
{
  char* Buffer_p = malloc(Size);

  if (Buffer_p != NULL)
  {
    memset(Buffer_p, c, Size);
    CSPSA_Result_t Result = CSPSA_WriteValue(Handle, Key, Size, (CSPSA_Data_t*)Buffer_p);
    free(Buffer_p);
    printf("  WriteBlock %u, %c, size %u", Key, c, Size);
    printf(" -> %d\n", Result);
  }
}

/** Writes a trim area int parameter (saved in string representation) */
void WriteInt(const CSPSA_Handle_t Handle, const CSPSA_Key_t Key, int Value)
{
  char StringRepr[10];

  // save as a string
  sprintf(StringRepr, "%d", Value);
  return WriteString(Handle, Key, StringRepr);
}

/** Reads a trim area int parameter */
int ReadInt(const CSPSA_Handle_t Handle, const CSPSA_Key_t Key, int DefaultValue)
{
  int Value = DefaultValue;
  CSPSA_Size_t Size;

  CSPSA_Result_t Result = CSPSA_GetSizeOfValue(Handle, Key, &Size);
  if (Result != T_CSPSA_RESULT_OK)
  {
    printf("CSPSA_GetSizeOfValue -> %d\n", Result);
  }
  else
  {
    char* Data_p = malloc(Size);
    if (Data_p == NULL)
    {
      printf("Out of mem\n");
    }
    else
    {
      Result = CSPSA_ReadValue(Handle, Key, Size, Data_p);
      sscanf(Data_p, "%d", &Value);
      printf("    ReadInt(key %u) -> %d\n", Key, Value);
    }
  }
  return Value;
}

/**
 * Logs contents of all trim area parameters
 */
void DumpTrimAreaContents(const char* TrimAreaName_p)
{
  CSPSA_Result_t Result;
  CSPSA_Handle_t Handle;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;

  printf("Dump of trim area %s\n", TrimAreaName_p);
  printf("---------------------\n");
  Result = CSPSA_Open(TrimAreaName_p, &Handle);
  printf("  CSPSA_Open returns %d\n", Result);
  if (Result == T_CSPSA_RESULT_OK)
  {
    Result = CSPSA_GetSizeOfFirstValue(Handle, &Key, &Size);
    printf("  CSPSA_GetSizeOfFirstValue returns %d, first key: %u\n", Result, Key);
  }
  while (Result == T_CSPSA_RESULT_OK)
  {
    char* Data_p = malloc(Size);

    if (Data_p == NULL)
    {
      printf("out of memory\n");
      break;
    }
    Result = CSPSA_ReadValue(Handle, Key, Size, Data_p);
    printf("    CSPSA_ReadValue(key %u, size %u)", Key, Size);
    printf("-> %d\n", Result);
    if (Result != T_CSPSA_RESULT_OK)
    {
      break;
    }
    printf("Dump of key %u, size %u:\n", Key, Size);
    HexDump(Data_p, Size);
    Result = CSPSA_GetSizeOfNextValue(Handle, &Key, &Size);
    if (Result != T_CSPSA_RESULT_OK)
    {
      printf("CSPSA_GetSizeOfNextValue -> %d\n", Result);
    }
  }
  (void)CSPSA_Close(&Handle);
  printf("--- End of %s ---\n\n", TrimAreaName_p);
}

/**
 * Dumps contents of all trim areas
 */
void DumpAllTrimAreas(void)
{
  uint32_t NrOfTrimAreas = CSPSA_GetParameterAreaListLength();
  uint32_t i;

  printf("[TestTrimArea] Number of registered parameter storage areas: %u\n", NrOfTrimAreas);
  for (i = 0; i < NrOfTrimAreas; ++i)
  {
    CSPSA_Config_t Config;
    
    (void)CSPSA_GetParameterArea(i, &Config);
    DumpTrimAreaContents(Config.Name);
  }
}

/** Performs some trim area stuff to check that everything seems to be working */
void TestCSPSA(void)
{
  CSPSA_Handle_t Handle;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  // Version (just used for testing purposes)
  int TestVersion = 0;
  
  CSPSA_LL_EMMC_Init();
  Result = RegisterEMMC_Areas("/flash0");
  DumpAllTrimAreas();
  if (Result == T_CSPSA_RESULT_OK)
  {
    // work with the first registered trim area
    CSPSA_Config_t Config;

    CSPSA_GetParameterArea(0, &Config);
    Result = CSPSA_Open(Config.Name, &Handle);
    printf("[TestTrimArea] CSPSA_Open returned %d\n", Result);
    if (Result != T_CSPSA_RESULT_OK)
    {
      // open failed, we create a new trim area instead
      printf("[TestTrimArea] Create a new trim area\n");
      Result = CSPSA_Create(Config.Name, &Handle);
      printf("[TestTrimArea] CSPSA_Create returned %d\n", Result);
    }
    else
    {
      TestVersion = ReadInt(Handle, 7, 0);
      printf("TestVersion has value %d\n", TestVersion);
    }
  }
  if (Result == T_CSPSA_RESULT_OK)
  {
    printf("\n[TestTrimArea] Write parameters\n");
    WriteString(Handle, 8, "1234567");
    WriteString(Handle, 1, "Hello");
    WriteString(Handle, 2, "World!");
    WriteBlock(Handle, 10, 'S', 512);
    WriteBlock(Handle, 11, 'T', 128);
    WriteBlock(Handle, 12, 'E', 400);
    WriteBlock(Handle, 13, '!', 1999);
    // increase TestVersion
    WriteInt(Handle, 7, TestVersion+1);
    printf("[TestTrimArea] call CSPSA_Flush\n");
    Result = CSPSA_Flush(Handle);
    printf("[TestTrimArea] CSPSA_Flush returned %d\n", Result);
  }
  if (Result == T_CSPSA_RESULT_OK)
  {
    printf("[TestTrimArea] call CSPSA_Close\n");
    Result = CSPSA_Close(&Handle);
  }
  DumpAllTrimAreas();
  printf("[TestTrimArea] exit TestTrimArea\n");
}

//////////////////////////////////////////////////////////////////
// TEST CODE THAT SHOWS ALL TOC ENTRIES
//////////////////////////////////////////////////////////////////

#include "boot_area_management.h"
#include "toc_handler.h"
//#include "flash_fail_safe.h"

#define MAX_DEVICE_PATH_LENGTH   (256)
static void PrintTocList (TOC_List_t *List)
{
  TOC_List_t *iter = List;
  char buf[TOC_ID_LENGTH + 1];
  
  buf[TOC_ID_LENGTH] = 0;
  
  printf("Offset     Size       Flags      Align      LoadAddr   ID\n");

  while (iter)
  {
    printf("0x%08x ", iter->entry.TOC_Offset);
    printf("0x%08x ", iter->entry.TOC_Size);
    printf("0x%08x ", iter->entry.TOC_Flags);
    printf("0x%08x ", iter->entry.TOC_Align);
    printf("0x%08x ", iter->entry.TOC_LoadAddress);
    memcpy (buf, &iter->entry.TOC_Id, TOC_ID_LENGTH);
    printf("\"%s\"\n", buf);

    iter = iter->next;
  }                 
  printf("\n");
}

static ErrorCode_e GetDevicePathForTOCEntry(TOC_List_t* Entry_p, char* Path_p)
{
  ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

  VERIFY(NULL != Entry_p, E_INVALID_INPUT_PARAMETERS);
  VERIFY(NULL != Path_p, E_INVALID_INPUT_PARAMETERS);

  if (Entry_p->entry.TOC_Offset < BAM_BOOT_IMAGE_COPY_SIZE)
  {
    sprintf(Path_p, "/flash0/boot0/TOC/%s", Entry_p->entry.TOC_Id);
  }
  else
  {
    if (!Entry_p->Parent)
    {
      sprintf(Path_p, "/flash0/TOC/%s", Entry_p->entry.TOC_Id);
    }
    else
    {
      sprintf(Path_p, "/flash0/TOC/%s/%s", Entry_p->Parent->entry.TOC_Id, Entry_p->entry.TOC_Id);
    }
  }

  ReturnValue = E_SUCCESS;

ErrorExit:

  return ReturnValue;
}

void ShowTOC()
{
  ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
  uint8* BootArea_p = NULL;
  FPD_Config_t FPD_Config = {0,};
  uint32 i = 0;

  TOC_List_t* TOC_List_p = NULL;

  /*
   * Get flash devices from services.
   */
  ReturnValue = Do_FPD_GetInfo(&FPD_Config);
  VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
  printf("Device size: %u\n", FPD_Config.DeviceSize);

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
  printf("Device paths\n");
  char* TempPath_p = (char*)malloc(MAX_DEVICE_PATH_LENGTH);
  if (NULL != TOC_Entry_p)
  {
    do
    {

      ReturnValue = GetDevicePathForTOCEntry(TOC_Entry_p, TempPath_p);
      VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
      printf("Device path %s\n", TempPath_p);
    }
    while ((TOC_Entry_p = Do_TOC_FindNextEntry(TOC_Entry_p)) != NULL);
  }
  printf("End of device paths\n");
  PrintTocList(TOC_List_p);
ErrorExit:
  Do_TOC_DestroyTOCList(TOC_List_p);
  free(TempPath_p);

}

//////////////////////////////////////////////////////////////////
// END TEST CODE
//////////////////////////////////////////////////////////////////


/** 
 * Shows the TOC, locates CSPSA areas + registers them to the CSPSA,
 * sets up the eMMC plugin.
 * Finally executes some example code on a CSPSA.
 * Call this function 
 */
void RunTrimAreaTest(void)
{
  ShowTOC();
  TestCSPSA();
}
