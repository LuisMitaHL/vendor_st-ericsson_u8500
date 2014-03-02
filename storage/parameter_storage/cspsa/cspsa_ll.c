/* **************************************************************************
 *
 * cspsa_ll.c
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

#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_ll.h"
#include "cspsa_plugin.h"

#if defined(CFG_CSPSA_LL_DEBUG_LEVEL) && CFG_CSPSA_LL_DEBUG_LEVEL >= 1
#include <stdio.h>

// Enable logging
#define A_(par) par
#else
#define A_(par)
#endif


// =============================================================================
//  Local Types
// =============================================================================

typedef struct RegisteredTrimArea_t
{
  struct RegisteredTrimArea_t* Next_p;
  const CSPSA_Config_t* Config_p;
} RegisteredTrimArea_t;

// =============================================================================
//  Local Variables
// =============================================================================

/** Array that contains for every memory type the registered plugin */
static const CSPSA_PluginOperations_t* RegisteredPlugins_p[T_CSPSA_MEMORY_TYPE_NUMBER_OF_ITEMS];


// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_LL_GetInfoFromHandle: Gets Plugin and Config info from the given handle.

static CSPSA_Result_t F_CSPSA_LL_GetInfoFromHandle(
   const CSPSA_Handle_t Handle,
   const CSPSA_PluginOperations_t** Plugin_pp,
   void** PluginHandle_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_SessionInfo_t* SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  const CSPSA_PluginOperations_t* Plugin_p;

  if (SessionInfo_p == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  Plugin_p = RegisteredPlugins_p[SessionInfo_p->Config.MemoryType];
  if (Plugin_p != NULL)
  {
    *Plugin_pp = Plugin_p;
    *PluginHandle_pp = SessionInfo_p->PluginHandle_p;
    Result = T_CSPSA_RESULT_OK;
  }
  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

// - CSPSA_RegisterPlugin: Registers a plug-in.

CSPSA_Result_t CSPSA_RegisterPlugin(CSPSA_MemoryType_t MemoryType, const CSPSA_PluginOperations_t* Operations_p)
{
  RegisteredPlugins_p[MemoryType] = Operations_p;
  return T_CSPSA_RESULT_OK;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_Open: Opens a flash memory device.

CSPSA_Result_t CSPSA_LL_Open(const CSPSA_Handle_t Handle)
{
  CSPSA_SessionInfo_t* SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;

  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  if (Result == T_CSPSA_RESULT_OK && Plugin_p->Open != NULL)
  {
    Result = Plugin_p->Open(&SessionInfo_p->Config, &SessionInfo_p->PluginHandle_p);
  }
  A_(printf("CSPSA_LL: Open(%s) -> %d\n", SessionInfo_p->Config.Name, Result);)
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_Close: Closes a flash memory device.

CSPSA_Result_t CSPSA_LL_Close(const CSPSA_Handle_t Handle)
{
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;
  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  if (Result == T_CSPSA_RESULT_OK && Plugin_p->Close != NULL)
  {
    Result = Plugin_p->Close(PluginHandle_p);
  }
  A_(printf("CSPSA_LL: Close() -> %d\n", Result);)
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_Read: Reads data from storage media.

CSPSA_Result_t CSPSA_LL_Read(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size,
  const bool ReadBackwards
)
{
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;
  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  (void)ReadBackwards;
  if (Result == T_CSPSA_RESULT_OK && Plugin_p->Read != NULL)
  {
    Result = Plugin_p->Read(PluginHandle_p, Offset, Data_p, Size);
  }
  A_(printf("CSPSA_LL: Read(offset: 0x%llx, size: 0x%x) -> %d\n", Offset, Size, Result);)
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_Format: Formats (erases) media.

CSPSA_Result_t CSPSA_LL_Format(const CSPSA_Handle_t Handle)
{
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;
  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  if (Result == T_CSPSA_RESULT_OK && Plugin_p->Format != NULL)
  {
    Result = Plugin_p->Format(PluginHandle_p);
  }
  A_(printf("CSPSA_LL: Format() -> %d\n", Result);)
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_Write: Writes to storage media.

CSPSA_Result_t CSPSA_LL_Write(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size,
  const bool WriteBackwards
)
{
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;
  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  (void)WriteBackwards;
  if (Result == T_CSPSA_RESULT_OK && Plugin_p->Write != NULL)
  {
    CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
    uint32_t BlockSize = CSPSA_SessionInfo_p->Config.BlockSize;
    const uint32_t SizeInBytes = CSPSA_SessionInfo_p->Config.SizeInBytes;
    uint32_t MaxSpace;

    if (CSPSA_SessionInfo_p->SessionCreated && (CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY))
    {
      // - If this session is started by a call to CSPSA_Create() the
      //   storage space is empty. If also "read-only" flag is set we do not
      //   need to have twice the space to be crash safe.
      if (WriteBackwards)
      {
        MaxSpace = 0;
      }
      else
      {
        MaxSpace = SizeInBytes;
      }
    }
    else
    {
      MaxSpace = SizeInBytes / 2;
    }
    if (WriteBackwards)
    {
      if ((Offset < MaxSpace) || (Offset > SizeInBytes) || (Size > SizeInBytes) || (Offset - MaxSpace + BlockSize < Size))
      {
        Result = T_CSPSA_RESULT_E_OUT_OF_SPACE;
        goto ErrorExit;
      }
    }
    else
    {
      if ((Offset > MaxSpace) || (Size > MaxSpace) || (Offset + Size > MaxSpace))
      {
        Result = T_CSPSA_RESULT_E_OUT_OF_SPACE;
        goto ErrorExit;
      }
    }
    Result = Plugin_p->Write(PluginHandle_p, Offset, Data_p, Size);
  }
ErrorExit:
  A_(printf("CSPSA_LL: Write(offset: 0x%llx, size: 0x%x) -> %d\n", Offset, Size, Result);)
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_LL_Erase(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const bool EraseBackwards
)
{
  void* PluginHandle_p;
  const CSPSA_PluginOperations_t* Plugin_p;
  CSPSA_Result_t Result = F_CSPSA_LL_GetInfoFromHandle(Handle, &Plugin_p, &PluginHandle_p);

  M_CSPSA_PARAMETER_NOT_USED(EraseBackwards);

  if ( (Result == T_CSPSA_RESULT_OK) && (Plugin_p->Format != NULL) )
  {
    Result = Plugin_p->Erase(PluginHandle_p, Offset);
  }
  A_(printf("CSPSA_LL: Erase(offset: 0x%llx) -> %d\n", Offset, Result);)
  return Result;
}

// =============================================================================
//   End of file
// =============================================================================
