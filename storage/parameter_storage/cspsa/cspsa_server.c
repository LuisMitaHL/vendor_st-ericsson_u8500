/* **************************************************************************
 *
 * cspsa_server.c
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: Martin Lundholm <martin.xa.lundholm AT stericsson.com>
 *         Joakim Axelsson <joakim.axelsson AT stericsson.com>
 *
 * DESCRIPTION:
 *
 * Main functions for CSPSA userspace server.
 *
 **************************************************************************** */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#if !CFG_CSPSA_ANDROID
#include <sys/signalfd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h>
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_plugin.h"
#include "cspsa_ll_dev.h"
#include "cspsa_ll_file.h"
#include "cspsa_ll_ram.h"
#include "cspsa_api_linux_socket_common.h"
#include "cspsa_api_linux_socket_server.h"

#include "cspsa_api_linux_socket_server.h"
#include "config_file_reader.h"

// =============================================================================
//  Defines
// =============================================================================

#define D_CSPSA_API_LINUX_SOCKET_SERVER_MAX_NBR_OF_CONFIGS     10

// =============================================================================
//  Macros
// =============================================================================

#define M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TargetString, Parameter, Index) \
  do { \
    char* c; \
    strcpy(TargetString, Parameter); \
    c = strchr(TargetString, '?'); \
    *c = Index + '0'; \
  } while (false) \

// =============================================================================
//  Local Constants
// =============================================================================

static char* C_CONFIG_CSPSA							= "CONFIG_CSPSA?";
static char* C_CONFIG_CSPSA_NAME						= "CONFIG_CSPSA?_NAME";
static char* C_CONFIG_CSPSA_MEDIA 					= "CONFIG_CSPSA?_MEDIA";
static char* C_CONFIG_CSPSA_DEVICE					= "CONFIG_CSPSA?_DEVICE";
static char* C_CONFIG_CSPSA_OFFSET					= "CONFIG_CSPSA?_OFFSET";
static char* C_CONFIG_CSPSA_SIZE						= "CONFIG_CSPSA?_SIZE";
static char* C_CONFIG_CSPSA_SET_BLOCK_AND_SECTOR_SIZE = "CONFIG_CSPSA?_SET_BLOCK_AND_SECTOR_SIZE";
static char* C_CONFIG_CSPSA_BLOCK_SIZE				= "CONFIG_CSPSA?_BLOCK_SIZE";
static char* C_CONFIG_CSPSA_SECTOR_SIZE				= "CONFIG_CSPSA?_SECTOR_SIZE";
static char* C_CONFIG_CSPSA_READ_ONLY 				= "CONFIG_CSPSA?_READ_ONLY";
static char* C_CONFIG_CSPSA_ACCESS_RESTRICTED 		= "CONFIG_CSPSA?_ACCESS_RESTRICTED";
static char* C_CONFIG_CSPSA_ACCESS_GROUP_NAME 		= "CONFIG_CSPSA?_ACCESS_GROUP_NAME";
static char* C_CONFIG_CSPSA_ACCESS_RIGHTS_GROUP		= "CONFIG_CSPSA?_ACCESS_RIGHTS_GROUP";
static char* C_CONFIG_CSPSA_ACCESS_RIGHTS_OTHERS		= "CONFIG_CSPSA?_ACCESS_RIGHTS_OTHERS";

// =============================================================================
//  Local Function Declarations
// =============================================================================
static bool F_CSPSA_API_LINUX_SOCKET_SERVER_GetConfig(const CFR_Handle_t CfrHandle, const uint32_t Index, CSPSA_Config_t* Config_p);

// =============================================================================
//  Local Function Definitions
// =============================================================================

static bool F_CSPSA_API_LINUX_SOCKET_SERVER_GetConfig(const CFR_Handle_t CfrHandle, const uint32_t Index, CSPSA_Config_t* Config_p)
{
  bool Result = false;
  int IntResult;
  CSPSA_Config_t Config;
  char TempString_p[D_CSPSA_MAX_NAME_SIZE];
  uint32_t Uint32Result;
  uint64_t Uint64Result;
  bool AccessRestricted = false;

  if (Index > (D_CSPSA_API_LINUX_SOCKET_SERVER_MAX_NBR_OF_CONFIGS - 1))
  {
    // - To make implementation easier we do not support more than D_CSPSA_API_LINUX_SOCKET_SERVER_MAX_NBR_OF_CONFIGS areas.
    return false;
  }

  memset(&Config, 0, sizeof(Config));

  // - Look if there is a config with the current index.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA, Index);
  IntResult = CFR_ReadBool(CfrHandle, TempString_p, &Uint32Result);
  if (IntResult != 0) goto ErrorExit;
  if (!Uint32Result)
  {
    goto ErrorExit;
  }

  // - Read area name.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_NAME, Index);
  IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
  if (IntResult != 0) goto ErrorExit;
  strncpy(Config.Name, TempString_p, D_CSPSA_MAX_NAME_SIZE);

  // - Read media type.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_MEDIA, Index);
  IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
  if (IntResult != 0) goto ErrorExit;
  if (strcmp(TempString_p, "MMC") == 0)
  {
    Config.MemoryType = T_CSPSA_MEMORY_TYPE_DEV_MMC;
  }
  else
  if (strcmp(TempString_p, "MTD") == 0)
  {
    Config.MemoryType = T_CSPSA_MEMORY_TYPE_DEV_MTD;
  }
  else
  if (strcmp(TempString_p, "RAM") == 0)
  {
    Config.MemoryType = T_CSPSA_MEMORY_TYPE_RAM;
  }
  else
  if (strcmp(TempString_p, "File") == 0)
  {
    Config.MemoryType = T_CSPSA_MEMORY_TYPE_FILE;
  }
  else
  {
    goto ErrorExit;
  }

  // - Read device path/name.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_DEVICE, Index);
  IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
  if (IntResult != 0) goto ErrorExit;
  {
    CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;
    if (LinuxDevice_p == NULL)
    {
      LinuxDevice_p = (CSPSA_LinuxDevice_t*) CSPSA_SOCKET_HeapMalloc(sizeof(CSPSA_LinuxDevice_t));

      if (LinuxDevice_p == NULL)
      {
        fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not allocate memory\n", __LINE__);
        goto ErrorExit;
      }

      memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
      LinuxDevice_p->FileDescriptor = -1;
      Config.Extra_p = LinuxDevice_p;
      Config.SizeOfExtra = sizeof(*LinuxDevice_p);
    }
    LinuxDevice_p->DeviceName_p = strdup(TempString_p);
  }

  // - Read area offset.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_OFFSET, Index);
  IntResult = CFR_ReadUint64(CfrHandle, TempString_p, &Uint64Result);
  if (IntResult != 0) goto ErrorExit;
  Config.StartOffsetInBytes = Uint64Result;

  // - Read area size.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_SIZE, Index);
  IntResult = CFR_ReadUint32(CfrHandle, TempString_p, &Uint32Result);
  if (IntResult != 0) goto ErrorExit;
  Config.SizeInBytes = Uint32Result;

  // - Look if block and sector size are set.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_SET_BLOCK_AND_SECTOR_SIZE, Index);
  IntResult = CFR_ReadBool(CfrHandle, TempString_p, &Uint32Result);
  if ((IntResult == 0) && (Uint32Result))
  {
    // - Get block size.
    M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_BLOCK_SIZE, Index);
    IntResult = CFR_ReadUint32(CfrHandle, TempString_p, &Uint32Result);
    if (IntResult != 0) goto ErrorExit;
    Config.BlockSize = Uint32Result;
    // - Get sector size.
    M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_SECTOR_SIZE, Index);
    IntResult = CFR_ReadUint32(CfrHandle, TempString_p, &Uint32Result);
    if (IntResult != 0) goto ErrorExit;
    Config.SectorSize = Uint32Result;
  }
  else
  {
    // - Block and sector sizes are not set. Try to query the device for the numbers.
    if ((Config.MemoryType == T_CSPSA_MEMORY_TYPE_DEV_MMC) || (Config.MemoryType == T_CSPSA_MEMORY_TYPE_DEV_MTD))
    {
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;

      if (LinuxDevice_p == NULL)
      {
        goto ErrorExit;
      }
      if (!CSPSA_LL_DEV_GetBlockAndSectorSize(LinuxDevice_p->DeviceName_p, &Config.BlockSize, &Config.SectorSize))
      {
        goto ErrorExit;
      }
    }
    else
    {
      // - Internal error. CSPSA needs to know block and sector sizes.
      goto ErrorExit;
    }
  }

  // - Look if area is read-only.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_READ_ONLY, Index);
  IntResult = CFR_ReadBool(CfrHandle, TempString_p, &Uint32Result);
  if (IntResult == 0)
  {
    if (Uint32Result)
    {
      Config.Attributes |= D_CSPSA_READ_ONLY;
    }
  }

  // - Look if area has access restrictions.
  M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_ACCESS_RESTRICTED, Index);
  IntResult = CFR_ReadBool(CfrHandle, TempString_p, &Uint32Result);
  if (IntResult == 0)
  {
    AccessRestricted = Uint32Result;
  }

  if (AccessRestricted)
  {
    // - Read access group name.
    M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_ACCESS_GROUP_NAME, Index);
    IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
    if (IntResult == 0)
    {
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;
      if (LinuxDevice_p == NULL)
      {
        LinuxDevice_p = (CSPSA_LinuxDevice_t*) CSPSA_SOCKET_HeapMalloc(sizeof(CSPSA_LinuxDevice_t));

        if (LinuxDevice_p == NULL)
        {
          fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not allocate memory\n", __LINE__);
          goto ErrorExit;
        }

        memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
        Config.Extra_p = LinuxDevice_p;
        Config.SizeOfExtra = sizeof(*LinuxDevice_p);
      }
      LinuxDevice_p->AccessGroupName_p = strdup(TempString_p);
    }

    // - Read group access rights.
    M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_ACCESS_RIGHTS_GROUP, Index);
    IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
    if (IntResult != 0) goto ErrorExit;
    Uint32Result = 0;
    {
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;
      if (LinuxDevice_p == NULL)
      {
        LinuxDevice_p = (CSPSA_LinuxDevice_t*) CSPSA_SOCKET_HeapMalloc(sizeof(CSPSA_LinuxDevice_t));

        if (LinuxDevice_p == NULL)
        {
          fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not allocate memory\n", __LINE__);
          goto ErrorExit;
        }
        memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
        Config.Extra_p = LinuxDevice_p;
        Config.SizeOfExtra = sizeof(*LinuxDevice_p);
      }
      Uint32Result |= (strchr(TempString_p, 'r') != NULL) ? D_CSPSA_LL_ACCESS_RIGHTS_GROUP_READ : 0;
      Uint32Result |= (strchr(TempString_p, 'w') != NULL) ? D_CSPSA_LL_ACCESS_RIGHTS_GROUP_WRITE : 0;
      LinuxDevice_p->GroupRwOtherRw |= Uint32Result;
    }

    // - Read other access rights.
    M_CSPSA_API_LINUX_SOCKET_SERVER_INDEX_CONFIG_PARAMETER(TempString_p, C_CONFIG_CSPSA_ACCESS_RIGHTS_OTHERS, Index);
    IntResult = CFR_ReadString(CfrHandle, TempString_p, TempString_p, sizeof(TempString_p));
    if (IntResult != 0) goto ErrorExit;
    Uint32Result = 0;
    {
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;
      if (LinuxDevice_p == NULL)
      {
        LinuxDevice_p = (CSPSA_LinuxDevice_t*) CSPSA_SOCKET_HeapMalloc(sizeof(CSPSA_LinuxDevice_t));
        if (LinuxDevice_p == NULL)
        {
          fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not allocate memory\n", __LINE__);
          goto ErrorExit;
        }

        memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
        Config.Extra_p = LinuxDevice_p;
        Config.SizeOfExtra = sizeof(*LinuxDevice_p);
      }
      Uint32Result |= (strchr(TempString_p, 'r') != NULL) ? D_CSPSA_LL_ACCESS_RIGHTS_OTHER_READ : 0;
      Uint32Result |= (strchr(TempString_p, 'w') != NULL) ? D_CSPSA_LL_ACCESS_RIGHTS_OTHER_WRITE : 0;
      LinuxDevice_p->GroupRwOtherRw |= Uint32Result;
    }
  }

  if ((Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM) && (Config.StartOffsetInBytes == 0) && (Config.SizeInBytes > 0) )
  {
    Config.StartOffsetInBytes = (uint64_t)(ptrdiff_t) CSPSA_SOCKET_HeapMalloc(Config.SizeInBytes); // - Double casting to supress warning.
    memset((uint32_t*) (ptrdiff_t)Config.StartOffsetInBytes, 0xFF, Config.SizeInBytes);
  }

  // - Return the config.
  *Config_p = Config;

  Result = true;

ErrorExit:

  if (!Result && (Config.Extra_p != NULL))
  {
    CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;

    M_CSPSA_SOCKET_FREE(LinuxDevice_p->DeviceName_p);
    M_CSPSA_SOCKET_FREE(LinuxDevice_p->AccessGroupName_p);
    M_CSPSA_SOCKET_FREE(LinuxDevice_p);
    if (Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
    {
      // - Next line's purpose is to silent Valgrind when running test application.
      free((uint8_t*)(ptrdiff_t)Config.StartOffsetInBytes);
    }
  }

  return Result;
}


// - main(): Starts CSPSA server daemon(s). One for each CSPSA area.

int main(int argc, char *argv[])
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  CSPSA_Config_t Config;
  int IntResult;
  CFR_Handle_t CfrHandle = 0;
  uint32_t i;
  char* ConfigFileName_p = NULL;

  if (argc > 1)
  {
    ConfigFileName_p = argv[1];
  }
  else
  {
    ConfigFileName_p = "/etc/cspsa.conf";
  }

  memset(&Config, 0, sizeof(Config));

  IntResult = CFR_Open(ConfigFileName_p, &CfrHandle);
  if (IntResult != 0)
  {
    fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not open configuration file '%s'!\n\n", __LINE__, ConfigFileName_p);
    goto ErrorExit;
  }

  for (i = 0; i < D_CSPSA_API_LINUX_SOCKET_SERVER_MAX_NBR_OF_CONFIGS; i++)
  {
    if (!F_CSPSA_API_LINUX_SOCKET_SERVER_GetConfig(CfrHandle, i, &Config))
    {
      continue;
    }
    if (fork() == 0)
    {
      CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* DaemonItem_p =
        (CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t*) CSPSA_SOCKET_HeapMalloc(sizeof(*DaemonItem_p));
      /* coverity[use_after_free]
       * Since F_CSPSA_API_LINUX_SOCKET_SERVER_GetConfig allocates a new Config.Extra_p,
       * this coverity problem has to be suppressed. */
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;

      if ((Config.MemoryType == T_CSPSA_MEMORY_TYPE_DEV_MTD) || (Config.MemoryType == T_CSPSA_MEMORY_TYPE_DEV_MMC))
      {
        CSPSA_LL_DEV_Init();
      }
      else if (Config.MemoryType == T_CSPSA_MEMORY_TYPE_FILE)
      {
        CSPSA_LL_FILE_Init();
      }
      else
      {
        CSPSA_LL_RAM_Init();
      }

      Result = CSPSA_AddParameterArea(&Config);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

      // - Copy config settings.
      memset(DaemonItem_p, 0, sizeof(*DaemonItem_p));
      DaemonItem_p->CspsaConfig = Config;
      DaemonItem_p->Pid = 0;

      // - Insert socket name.
      V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p = DaemonItem_p;
      DaemonItem_p->SocketName_p = CSPSA_SOCKET_CreateSocketName(Config.Name);

      printf("[CSPSA]: Starting server daemon for area '%s'.\n", Config.Name);
      P_CSPSA_API_LINUX_SOCKET_SERVER_Process(DaemonItem_p);
      printf("[CSPSA]: Server daemon for area '%s' has stopped.\n", Config.Name);

      CSPSA_RemoveParameterArea(&Config);

      if (Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
      {
        // - Next line's purpose is to silent Valgrind when running test application.
        free((uint8_t*)(ptrdiff_t)Config.StartOffsetInBytes);
      }
      if (LinuxDevice_p != NULL)
      {
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->DeviceName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->AccessGroupName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p);
      }
      M_CSPSA_SOCKET_FREE(DaemonItem_p->SocketName_p);
      M_CSPSA_SOCKET_FREE(DaemonItem_p);

      Result = T_CSPSA_RESULT_OK;
      break;
    }
    else
    {
      CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) Config.Extra_p;
      if (LinuxDevice_p != NULL)
      {
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->DeviceName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->AccessGroupName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p);
      }
      if (Config.MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
      {
        // - Next line's purpose is to silent Valgrind when running test application.
        free((uint8_t*)(ptrdiff_t)Config.StartOffsetInBytes);
      }
      Result = T_CSPSA_RESULT_OK;
    }
  }

ErrorExit:

  if (CfrHandle != 0)
  {
    CFR_Close(&CfrHandle);
  }

  return Result;
}



