/* **************************************************************************
 *
 * cspsa_ll_file.c
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
 * Enables access to CSPSA parameter areas stored in files.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#include "crc32.h"
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_plugin.h"
#include "cspsa_ll.h"

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Local Types
// =============================================================================

/**
 * Used as handle in CSPSA LL operations.
 */
typedef struct
{
  const CSPSA_Config_t* Config_p;
  uint8_t* Buffer_p;
} CSPSA_FILE_Info_t;

// =============================================================================
//  Local Constants
// =============================================================================

// =============================================================================
//  Local Variables
// =============================================================================

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_LL_FILE_Open(const CSPSA_Config_t* Config_p, void** Handle_pp);
static CSPSA_Result_t F_CSPSA_LL_FILE_Close(void* Handle_p);
static CSPSA_Result_t F_CSPSA_LL_FILE_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
);
static CSPSA_Result_t F_CSPSA_LL_FILE_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
);
static CSPSA_Result_t F_CSPSA_LL_FILE_Format(void* Handle_p);
static CSPSA_Result_t F_CSPSA_LL_FILE_Erase(
  void* Handle_p,
  const uint64_t Offset
);

// -----------------------------------------------------------------------------

// - C_CSPSA_LL_FILE_PLUGIN: Function table for plug-in.
static const CSPSA_PluginOperations_t C_CSPSA_LL_FILE_PLUGIN_OPERATIONS =
{
  F_CSPSA_LL_FILE_Open,
  F_CSPSA_LL_FILE_Close,
  F_CSPSA_LL_FILE_Read,
  F_CSPSA_LL_FILE_Write,
  F_CSPSA_LL_FILE_Format,
  F_CSPSA_LL_FILE_Erase,
};

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_LL_FILE_Open: Opens a file containing CSPSA parameters and reads all
//                         contents to RAM.

static CSPSA_Result_t F_CSPSA_LL_FILE_Open(const CSPSA_Config_t* Config_p, void** Handle_pp)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  FILE* fp;
  CSPSA_LinuxDevice_t* LinuxDevice_p;

  // create a file info object that is used in subsequent calls as the handle
  CSPSA_FILE_Info_t* FileInfo_p = malloc(sizeof(CSPSA_FILE_Info_t));
  FileInfo_p->Config_p = Config_p;
  // create a RAM buffer containing the CSPSA contents
  FileInfo_p->Buffer_p = malloc(Config_p->SizeInBytes);
  memset(FileInfo_p->Buffer_p, 0xFF, Config_p->SizeInBytes);
  *Handle_pp = (void*)FileInfo_p;
  LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config_p->Extra_p;
  // Read the contents from file if it exists.
  if (LinuxDevice_p != NULL)
  {
    fp = fopen(LinuxDevice_p->DeviceName_p, "r");
    if (fp != NULL)
    {
      int ItemsRead = fread(FileInfo_p->Buffer_p, Config_p->SizeInBytes, 1, fp);
      if (ItemsRead <= 1)
      {
        Result = T_CSPSA_RESULT_OK;
      }
      fclose(fp);
    }
    else
    {
      // There was no CSPSA file to read from. File will be created
      // when CSPSA is flushed or closed.
      Result = T_CSPSA_RESULT_OK;
    }
  }
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_FILE_Close: Closes a file.

static CSPSA_Result_t F_CSPSA_LL_FILE_Close(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  FILE* fp;
  uint32_t ItemsWritten;
  CSPSA_FILE_Info_t* FileInfo_p = Handle_p;
  const CSPSA_Config_t* Config_p = FileInfo_p->Config_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p;

  // Write the contents in the buffer to file
  LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config_p->Extra_p;
  fp = fopen(LinuxDevice_p->DeviceName_p, "w");
  if (fp != NULL)
  {
    ItemsWritten = fwrite((const char*)FileInfo_p->Buffer_p, Config_p->SizeInBytes, 1, fp);
    if (ItemsWritten == 1)
    {
      Result = T_CSPSA_RESULT_OK;
    }
    fclose(fp);
  }
  free(FileInfo_p->Buffer_p);
  free(FileInfo_p);
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_FILE_Read: Reads data from storage media.

static CSPSA_Result_t F_CSPSA_LL_FILE_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_FILE_Info_t* FileInfo_p = Handle_p;
  const CSPSA_Config_t* Config_p = FileInfo_p->Config_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;

  {
    uint8_t* CurrentOffset_p = FileInfo_p->Buffer_p + Offset;
    uint8_t* Data8_p = (uint8_t*)Data_p;

    while (DataLeft > 0)
    {
      ChunkSize = (DataLeft > BlockSize) ? BlockSize : DataLeft;
      memcpy(Data8_p, CurrentOffset_p, ChunkSize);
      DataLeft -= ChunkSize;
      CurrentOffset_p += ChunkSize;
      Data8_p += ChunkSize;
    }
  }
  Result = T_CSPSA_RESULT_OK;
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_FILE_Write: Writes to storage media.

static CSPSA_Result_t F_CSPSA_LL_FILE_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_FILE_Info_t* FileInfo_p = Handle_p;
  const CSPSA_Config_t* Config_p = FileInfo_p->Config_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;

  // We first write to file in F_CSPSA_LL_FILE_Close.
  // In this function we only update the buffer in RAM.
  {
    uint8_t* CurrentOffset_p = FileInfo_p->Buffer_p + Offset;
    uint8_t* Data8_p = (uint8_t*)Data_p;
    while (DataLeft > 0)
    {
      ChunkSize = (DataLeft > BlockSize) ? BlockSize : DataLeft;
      memcpy(CurrentOffset_p, Data8_p, ChunkSize);
      DataLeft -= ChunkSize;
      CurrentOffset_p += ChunkSize;
      Data8_p += ChunkSize;
    }
  }
  Result = T_CSPSA_RESULT_OK;
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_FILE_Format: Formats (erases) media.

static CSPSA_Result_t F_CSPSA_LL_FILE_Format(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_FILE_Info_t* FileInfo_p = Handle_p;
  const CSPSA_Config_t* Config_p = FileInfo_p->Config_p;

  memset(FileInfo_p->Buffer_p, 0xFF, Config_p->SizeInBytes);

  Result = T_CSPSA_RESULT_OK;
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_FILE_Erase: Erases part of storage media.
static CSPSA_Result_t F_CSPSA_LL_FILE_Erase(
  void* Handle_p,
  const uint64_t Offset
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_FILE_Info_t* FileInfo_p = Handle_p;
  const CSPSA_Config_t* Config_p = FileInfo_p->Config_p;

  memset(
    FileInfo_p->Buffer_p + Offset,
    0xFF,
    Config_p->BlockSize
  );
  Result = T_CSPSA_RESULT_OK;
  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_LL_FILE_Init(void)
{
  CSPSA_Result_t Result;

  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_FILE, &C_CSPSA_LL_FILE_PLUGIN_OPERATIONS);

  return Result;
}

// =============================================================================
//   End of file
// =============================================================================
