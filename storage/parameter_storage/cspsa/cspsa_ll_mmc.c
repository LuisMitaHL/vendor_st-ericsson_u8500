/* **************************************************************************
 *
 * cspsa_ll_mmc.c
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

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - CSPSA_LL_MMC_Open: Opens a flash memory device.

CSPSA_Result_t CSPSA_LL_MMC_Open(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  
  M_CSPSA_PARAMETER_NOT_USED(Handle);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_Close: Closes a flash memory device.

CSPSA_Result_t CSPSA_LL_MMC_Close(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  
  M_CSPSA_PARAMETER_NOT_USED(Handle);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_GetBlockSize: Fetches the block size of the current
//   storage media.

CSPSA_Result_t CSPSA_LL_MMC_GetSectorSize(
  const CSPSA_Handle_t Handle,
  uint32_t* const SectorSize_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);
  M_CSPSA_PARAMETER_NOT_USED(SectorSize_p);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_GetBlockSize: Fetches the block size of the current
//   storage media.

CSPSA_Result_t CSPSA_LL_MMC_GetBlockSize(
  const CSPSA_Handle_t Handle,
  uint32_t* const BlockSize_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);
  M_CSPSA_PARAMETER_NOT_USED(BlockSize_p);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_Read: Reads data from storage media.

CSPSA_Result_t CSPSA_LL_MMC_Read(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size,
  const bool ReadBackwards
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);
  M_CSPSA_PARAMETER_NOT_USED(Offset);
  M_CSPSA_PARAMETER_NOT_USED(Data_p);
  M_CSPSA_PARAMETER_NOT_USED(Size);
  M_CSPSA_PARAMETER_NOT_USED(ReadBackwards);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_Format: Formats (erases) media.

CSPSA_Result_t CSPSA_LL_MMC_Format(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_Erase: Erases part of storage media.

CSPSA_Result_t CSPSA_LL_MMC_Erase(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);
  M_CSPSA_PARAMETER_NOT_USED(Offset);
  M_CSPSA_PARAMETER_NOT_USED(Size);

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_MMC_Write: Writes to storage media.

CSPSA_Result_t CSPSA_LL_MMC_Write(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size,
  const bool WriteBackwards
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  M_CSPSA_PARAMETER_NOT_USED(Handle);
  M_CSPSA_PARAMETER_NOT_USED(Offset);
  M_CSPSA_PARAMETER_NOT_USED(Data_p);
  M_CSPSA_PARAMETER_NOT_USED(Size);
  M_CSPSA_PARAMETER_NOT_USED(WriteBackwards);

  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

// =============================================================================
//   End of file
// =============================================================================
