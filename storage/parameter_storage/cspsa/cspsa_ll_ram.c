/* **************************************************************************
 *
 * cspsa_ll_ram.c
 *
 * Copyright (C) 2009 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 * CSPSA interface to low level flash memory API's.
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
#include <pthread.h>
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

// =============================================================================
//  Local Constants
// =============================================================================

// =============================================================================
//  Local Variables
// =============================================================================

// - Normally there should be no global variables for CSPSA. Everything should
//   be stored in CSPSA_SessionInfo_t.

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_LL_RAM_Close(void* Handle_p);
static CSPSA_Result_t F_CSPSA_LL_RAM_Erase(
  void* Handle_p,
  const uint64_t Offset
);
static CSPSA_Result_t F_CSPSA_LL_RAM_Format(void* Handle_p);
static CSPSA_Result_t F_CSPSA_LL_RAM_Open(const CSPSA_Config_t* Config_p, void** Handle_pp);
static CSPSA_Result_t F_CSPSA_LL_RAM_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
);
static CSPSA_Result_t F_CSPSA_LL_RAM_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
);

// -----------------------------------------------------------------------------

// - C_CSPSA_LL_RAM_PLUGIN: Function table for plug-in.
static const CSPSA_PluginOperations_t C_CSPSA_LL_RAM_PLUGIN_OPERATIONS =
{
  F_CSPSA_LL_RAM_Open,
  F_CSPSA_LL_RAM_Close,
  F_CSPSA_LL_RAM_Read,
  F_CSPSA_LL_RAM_Write,
  F_CSPSA_LL_RAM_Format,
  F_CSPSA_LL_RAM_Erase,
};

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_LL_RAM_Open: Opens a flash memory device.

static CSPSA_Result_t F_CSPSA_LL_RAM_Open(const CSPSA_Config_t* Config_p, void** Handle_pp)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  // - Only the Config_p parameter is needed in subsequent calls to read/write,
  //   so use Config_p as the handle.
  *Handle_pp = (void*)Config_p;

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_RAM_Close: Closes a flash memory device.

static CSPSA_Result_t F_CSPSA_LL_RAM_Close(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle_p);

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_RAM_Read: Reads data from storage media.

static CSPSA_Result_t F_CSPSA_LL_RAM_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;

  {
    uint8_t* CurrentOffset_p = (uint8_t*)((ptrdiff_t)Config_p->StartOffsetInBytes + (ptrdiff_t)Offset);
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

// - F_CSPSA_LL_RAM_Format: Formats (erases) media.

static CSPSA_Result_t F_CSPSA_LL_RAM_Format(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;

  memset((uint8_t*)(ptrdiff_t)Config_p->StartOffsetInBytes, 0xFF, Config_p->SizeInBytes);

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_RAM_Erase: Erases part of storage media.
static CSPSA_Result_t F_CSPSA_LL_RAM_Erase(
  void* Handle_p,
  const uint64_t Offset
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;

  memset(
    (uint8_t*)((ptrdiff_t)Config_p->StartOffsetInBytes + (ptrdiff_t)Offset),
    0xFF,
    Config_p->BlockSize
  );

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_RAM_Write: Writes to storage media.

static CSPSA_Result_t F_CSPSA_LL_RAM_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;

  {
    uint8_t* CurrentOffset_p = (uint8_t*)((ptrdiff_t)Config_p->StartOffsetInBytes + (ptrdiff_t)Offset);
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

// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_LL_RAM_Init(void)
{
  CSPSA_Result_t Result;

  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_RAM, &C_CSPSA_LL_RAM_PLUGIN_OPERATIONS);

  return Result;
}

// =============================================================================
//   End of file
// =============================================================================
