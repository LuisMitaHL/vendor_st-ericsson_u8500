/* **************************************************************************
 *
 * cspsa_ll_dev.c
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
 * Low level interface to Linux user space devices.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_ANDROID: If != 0 building is for Android with bionic.
#ifndef CFG_CSPSA_ANDROID
#define CFG_CSPSA_ANDROID     0
#endif

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
#include <errno.h>
#include "crc32.h"
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_plugin.h"
#include "cspsa_ll.h"
#include "cspsa_ll_dev.h"

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_LL_DEV_FREE: Wrapper to free a pointer.
#define M_CSPSA_LL_DEV_FREE(Pointer) \
  do                          \
  {                           \
    free(Pointer);            \
    Pointer = NULL;           \
  } while (0)

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

// - F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported: Not all drivers support ioctl(BLKDISCARD). For those
//   drivers we write 0xAB's instead.
// - NOTE: This is not session dependent so there is no need to make it thread safe.
#if !CFG_CSPSA_ANDROID
static bool F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported = true;
#else
static bool F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported = false;
#endif

// - F_CSPSA_LL_DEV_EraseData_p: Not all drivers support ioctl(BLKDISCARD). For those
//   drivers we write 0xAB's instead. This varable can be allocated memory and
//   used in erase function.
// - NOTE: This is not session dependent so there is no need to make it thread safe.
static uint8_t* F_CSPSA_LL_DEV_EraseData_p = NULL;

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_LL_DEV_Close(void* Handle_p);
static CSPSA_Result_t F_CSPSA_LL_DEV_Format(const CSPSA_Handle_t Handle);
static CSPSA_Result_t F_CSPSA_LL_DEV_Erase(
  void* Handle_p,
  const uint64_t Offset
);
static CSPSA_Result_t F_CSPSA_LL_DEV_Open(const CSPSA_Config_t* Config_p, void** Handle_pp);
static CSPSA_Result_t F_CSPSA_LL_DEV_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
);
static CSPSA_Result_t F_CSPSA_LL_DEV_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
);

// -----------------------------------------------------------------------------

// - C_CSPSA_LL_DEV_PLUGIN: Function table for plug-in.
static const CSPSA_PluginOperations_t C_CSPSA_LL_DEV_PLUGIN_OPERATIONS =
{
  F_CSPSA_LL_DEV_Open,
  F_CSPSA_LL_DEV_Close,
  F_CSPSA_LL_DEV_Read,
  F_CSPSA_LL_DEV_Write,
  F_CSPSA_LL_DEV_Format,
  F_CSPSA_LL_DEV_Erase
};

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - CSPSA_LL_DEV_Open: Opens a flash memory device.

static CSPSA_Result_t F_CSPSA_LL_DEV_Open(const CSPSA_Config_t* Config_p, void** Handle_pp)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_LinuxDevice_t* LinuxDevice_p = NULL;

  // - Only the Config_p parameter is needed in subsequent calls to read/write,
  //   so use Config_p as the handle.
  *Handle_pp = (void*)Config_p;
  LinuxDevice_p = Config_p->Extra_p;

  LinuxDevice_p->FileDescriptor = open(LinuxDevice_p->DeviceName_p, O_RDWR);
  if (LinuxDevice_p->FileDescriptor < 0)
  {
    fprintf(stderr, "[CSPSA] Failed to open '%s'.\n", LinuxDevice_p->DeviceName_p);
    goto ErrorExit;
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_Close: Closes a flash memory device.

static CSPSA_Result_t F_CSPSA_LL_DEV_Close(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;

  close(LinuxDevice_p->FileDescriptor);

  if (F_CSPSA_LL_DEV_EraseData_p != NULL)
  {
    free(F_CSPSA_LL_DEV_EraseData_p);
    F_CSPSA_LL_DEV_EraseData_p = NULL;
  }

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_Read: Reads data from storage media.

static CSPSA_Result_t F_CSPSA_LL_DEV_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;
  uint8_t* Data8_p = (uint8_t*)Data_p;
  int ReturnLength;
  uint64_t ReadOffset = Config_p->StartOffsetInBytes + Offset;

  if (lseek(LinuxDevice_p->FileDescriptor, ReadOffset, SEEK_SET) < 0)
  {
    Result = T_CSPSA_RESULT_UNDEFINED;
    perror("F_CSPSA_LL_DEV_Read");
    goto ErrorExit;
  }

  while (DataLeft > 0)
  {
    ChunkSize = (DataLeft > BlockSize) ? BlockSize : DataLeft;
    ReturnLength = read(LinuxDevice_p->FileDescriptor, Data8_p, ChunkSize);
    if (ReturnLength != (int) ChunkSize)
    {
      Result = T_CSPSA_RESULT_UNDEFINED;
      goto ErrorExit;
    }
    DataLeft -= ChunkSize;
    Data8_p += ChunkSize;
  }
  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_Format: Formats (erases) media.

static CSPSA_Result_t F_CSPSA_LL_DEV_Format(void* Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

#if !CFG_CSPSA_ANDROID
  {
    const CSPSA_Config_t* Config_p = Handle_p;
    CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
    uint64_t Range[2];

    Range[0] = Config_p->StartOffsetInBytes;
    Range[1] = Config_p->StartOffsetInBytes + Config_p->SizeInBytes;

    if (ioctl(LinuxDevice_p->FileDescriptor, BLKDISCARD, &Range))
    {
      goto ErrorExit;
    }
  }
#else
  // - No support for BLKDISCARD.
  {
    const CSPSA_Config_t* Config_p = Handle_p;
    uint32_t BlockSize = Config_p->BlockSize;
    uint32_t Offset = 0;
    uint32_t SizeLeftToErase = Config_p->SizeInBytes;

    while (SizeLeftToErase > 0)
    {
      Result = F_CSPSA_LL_DEV_Erase(Handle_p, Offset);
      if (Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

      Offset += BlockSize;
      SizeLeftToErase -= BlockSize;
    }
  }
#endif

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_Erase: Erases part of storage media.

static CSPSA_Result_t F_CSPSA_LL_DEV_Erase(
  void* Handle_p,
  const uint64_t Offset
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
  uint32_t SectorSize = Config_p->SectorSize;

  LinuxDevice_p = Config_p->Extra_p;

#if !CFG_CSPSA_ANDROID
  if (F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported)
  {
    uint64_t Range[2];

    Range[0] = Config_p->StartOffsetInBytes + Offset;
    Range[1] = Range[0] + Config_p->BlockSize;

    F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported = ((ioctl(LinuxDevice_p->FileDescriptor, BLKDISCARD, &Range)) == 0);
  }
#endif

  if (!F_CSPSA_LL_DEV_IoCtlBlkDiscardSupported)
  {
    // - The driver does not support erase operation. So write 0xAB instead.
    uint32_t SectorsLeft = Config_p->BlockSize / SectorSize;
    uint64_t TempOffset = Offset;

    // - First create a buffer of a sector size filled with 0xAB.
    if (F_CSPSA_LL_DEV_EraseData_p == NULL)
    {
      F_CSPSA_LL_DEV_EraseData_p = (uint8_t*) malloc(SectorSize);
      memset(F_CSPSA_LL_DEV_EraseData_p, 0xAB, SectorSize);
    }

    while (SectorsLeft-- > 0)
    {
      Result = F_CSPSA_LL_DEV_Write(Handle_p, TempOffset, F_CSPSA_LL_DEV_EraseData_p, SectorSize);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      TempOffset += SectorSize;
    }
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_Write: Writes to storage media.

static CSPSA_Result_t F_CSPSA_LL_DEV_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
  uint32_t BlockSize = Config_p->BlockSize;
  uint32_t DataLeft = Size;
  uint32_t ChunkSize;

  {
    uint8_t* Data8_p = (uint8_t*)Data_p;
    int ReturnLength;
    uint64_t WriteOffset = Config_p->StartOffsetInBytes + Offset;

    if (lseek(LinuxDevice_p->FileDescriptor, WriteOffset, SEEK_SET) < 0)
    {
      Result = T_CSPSA_RESULT_UNDEFINED;
      perror("F_CSPSA_LL_DEV_Write");
      goto ErrorExit;
    }
    while (DataLeft > 0)
    {
      ChunkSize = (DataLeft > BlockSize) ? BlockSize : DataLeft;
      ReturnLength = write(LinuxDevice_p->FileDescriptor, Data8_p, ChunkSize);
      if (ReturnLength != (int) ChunkSize)
      {
        Result = T_CSPSA_RESULT_UNDEFINED;
        goto ErrorExit;
      }
      DataLeft -= ChunkSize;
      Data8_p += ChunkSize;
    }
    if (fsync(LinuxDevice_p->FileDescriptor) < 0)
    {
      Result = T_CSPSA_RESULT_UNDEFINED;
      goto ErrorExit;
    }
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_LL_DEV_Init(void)
{
  CSPSA_Result_t Result;

  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_DEV_MMC, &C_CSPSA_LL_DEV_PLUGIN_OPERATIONS);
  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_DEV_MTD, &C_CSPSA_LL_DEV_PLUGIN_OPERATIONS);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_GetSectorSize: Fetches the block size of the current
//   storage media.
bool CSPSA_LL_DEV_GetBlockAndSectorSize(
  char* DeviceName_p,
  uint32_t* const BlockSize_p,
  uint32_t* const SectorSize_p
)
{
  bool Result = false;
  int FileDescriptor;

  FileDescriptor = open(DeviceName_p, O_RDWR);
  if (FileDescriptor < 0)
  {
    fprintf(stderr, "[CSPSA] Failed to open '%s'.\n", DeviceName_p);
    return Result;
  }

  if (ioctl(FileDescriptor, BLKSSZGET, SectorSize_p))
  {
    // - Sector size not determined.
    perror("CSPSA_LL_DEV_GetBlockAndSectorSize(BLKSSZGET)");
    goto ErrorExit;
  }

  if (ioctl(FileDescriptor, BLKBSZGET, BlockSize_p))
  {
    // - Block size not determined.
    perror("CSPSA_LL_DEV_GetBlockAndSectorSize(BLKBSZGET)");
    goto ErrorExit;
  }

  Result = true;

ErrorExit:
  close(FileDescriptor);
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_DEV_GetBlockSize: Fetches the block size of the current
//   storage media.

CSPSA_Result_t CSPSA_LL_DEV_GetBlockSize(
  void* Handle_p,
  uint32_t* const BlockSize_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;

  if (Config_p->BlockSize == 0)
  {
    int BlockSize;

    if (ioctl(LinuxDevice_p->FileDescriptor, BLKBSZGET, &BlockSize))
    {
      // - Block size not determined.
      perror("F_CSPSA_LL_DEV_GetBlockSize");
      goto ErrorExit;
    }
    Config_p->BlockSize = BlockSize;
  }

  *BlockSize_p = Config_p->BlockSize;
  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// =============================================================================
//   End of file
// =============================================================================
