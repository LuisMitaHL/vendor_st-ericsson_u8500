/* **************************************************************************
 *
 * cspsa_api_linux_socket_server.c
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
 *
 * Implements a CSPSA Linux userspace socket server.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_CLIENTS: Socket configuration.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_CLIENTS
#define CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_CLIENTS         5
#endif

// - CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS: Max number of file descriptors.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS
#define CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS             100
#endif

// - CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE: If != 0 the closing of the area
//   is not done immediatly after the last call to closed, but delayed for a period. One
//   effect is that any error message after close will not be catched at the moment they
//   were triggered.
//TODO: This should actually be a runtime option to the server.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE
	#ifdef DEBUG
		// - Delaying closing of opened CSPSA parameter storage areas ought
		//   be enabled in this case, but it can not be done because of
		//   reference counting of the opened sessions and parameter
		//   storage areas. More specifically it is the Counter field in
		//   the CSPSA_SessionInfoList_t that misbehaves -- when a client
		//   connects and tried to open a specific parameter storage area
		//   using the command line tool it first gets incremented to 1,
		//   then when this fails, the client closes the area and the
		//   open paramter storage area lingers (in case it gets opened
		//   again), when the client later attemps to create tha same
		//   parameter storage area the counter will be incremented to 2,
		//   and finally when the client closes the area again the area
		//   lingers around again, after a specific timeout the server will
		//   attempt to close the area properly, but the reference counter
		//   will never reach zero and thus it continues to linger
		//   indefinitely. This is a bug waiting to be fixed.
		#define CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE	0
	#else
		#define CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE	0
	#endif
#endif

// - CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE: The number of minutes after the last
//   close call to the actual close down. See CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE_TIMEOUT_IN_MIN
#define CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE_TIMEOUT_IN_MIN    1
#endif

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
#include <signal.h>
#if !CFG_CSPSA_ANDROID
#include <sys/signalfd.h>
#else
#include <private/android_filesystem_config.h>
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

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Local Types
// =============================================================================

// - CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t: Struct with info about a client
typedef struct
{
  // Is true if the client has performed CSPSA_Open
  bool IsOpen;
} CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t;

// =============================================================================
//  Local Constants
// =============================================================================

// - C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString: General purpose error string.
static const char C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString[] =
  "CSPSA_API_LINUX_SOCKET_SERVER(%u): %s()\n";

// =============================================================================
//  Local Variables
// =============================================================================

// =============================================================================
//  Global External Variables
// =============================================================================

// - V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p: List of started CSPSA
//   server daemons.
CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;


// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData(
  const int FileDescriptor,
  CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
  const uint32_t MaxPacketSize,
  const uint8_t* DataBuffer_p,
  const uint32_t Size
);

// -----------------------------------------------------------------------------
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Open(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Create(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Close(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_DeleteValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Flush(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Format(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfFirstValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfNextValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_ReadValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_WriteValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
);

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Close(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Result_t Result;

  M_CSPSA_PARAMETER_NOT_USED(ReadBuf_p);
  if (!ClientState_p->IsOpen)
  {
    Result = T_CSPSA_RESULT_OK;
    goto ErrorExit;
  }
  ServerDaemonListItem_p->OpenCounter--;

#if CFG_CSPSA_ANDROID
  // If we lack permission to reopen the device later then keep the CSPSA open,
  // but at least make sure that any outstanding data is written.
  struct stat CspsaStat;
  int IntResult = stat(((CSPSA_LinuxDevice_t*)ServerDaemonListItem_p->CspsaConfig.Extra_p)->DeviceName_p, &CspsaStat);
  if (ServerDaemonListItem_p->OpenCounter == 0 &&
    (!IntResult && ((CspsaStat.st_uid == AID_SYSTEM && CspsaStat.st_mode & (S_IRUSR|S_IWUSR)) ||
    (CspsaStat.st_gid == AID_SYSTEM && CspsaStat.st_mode & (S_IRGRP|S_IWGRP)) ||
    (CspsaStat.st_mode & (S_IROTH|S_IWOTH)))))
#else
  if (ServerDaemonListItem_p->OpenCounter == 0)
#endif
  {
#if CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE
    ServerDaemonListItem_p->CloseOnNextTimeout = CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE_TIMEOUT_IN_MIN;
    Result = T_CSPSA_RESULT_OK;
    CSPSA_Log(LOG_DEBUG, "Client closes; will close CSPSA on next timeout in %u minutes, open counter: %u\n",
            ServerDaemonListItem_p->CloseOnNextTimeout,
            ServerDaemonListItem_p->OpenCounter);
#else
    Result = CSPSA_CORE_Close(&ServerDaemonListItem_p->CspsaHandle);
    CSPSA_Log(LOG_DEBUG, "CSPSA_Close() -> result %d, open counter: %u\n",
            Result, ServerDaemonListItem_p->OpenCounter);
#endif
  }
  else
  {
    // Other clients still use the CSPSA, so keep the CSPSA open, but at least make sure that any
    // outstanding data is written.
    Result = CSPSA_CORE_Flush(ServerDaemonListItem_p->CspsaHandle);
    CSPSA_Log(LOG_DEBUG, "Client closes; CSPSA_Flush() -> result %d, open counter: %u\n",
            Result, ServerDaemonListItem_p->OpenCounter);
  }
  ClientState_p->IsOpen = false;
ErrorExit:
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Create(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Result_t Result;

  M_CSPSA_PARAMETER_NOT_USED(ReadBuf_p);
  Result = CSPSA_CORE_Create(
      ServerDaemonListItem_p->CspsaConfig.Name,
      &ServerDaemonListItem_p->CspsaHandle
    );
  if (Result == T_CSPSA_RESULT_OK)
  {
    ServerDaemonListItem_p->OpenCounter++;
    ClientState_p->IsOpen = true;
  }
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  // also send packet size to be used by the client
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, ReadBuf_p->MaxLength);
  CSPSA_Log(LOG_DEBUG, "CSPSA_Create(%s) -> result %d, open counter: %u\n",
            ServerDaemonListItem_p->CspsaConfig.Name, Result, ServerDaemonListItem_p->OpenCounter);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Open(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListItem_p,
    CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Result_t Result;

  M_CSPSA_PARAMETER_NOT_USED(ReadBuf_p);
  if (ServerDaemonListItem_p->OpenCounter == 0)
  {
    Result = CSPSA_CORE_Open(
        ServerDaemonListItem_p->CspsaConfig.Name,
        &ServerDaemonListItem_p->CspsaHandle
      );
    if (Result == T_CSPSA_RESULT_OK)
    {
      ServerDaemonListItem_p->OpenCounter++;
      ClientState_p->IsOpen = true;
    }
  }
  else
  {
    ServerDaemonListItem_p->OpenCounter++;
    ClientState_p->IsOpen = true;
    Result = T_CSPSA_RESULT_OK;
  }
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  // also send packet size to be used by the client
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, ReadBuf_p->MaxLength);
  CSPSA_Log(LOG_DEBUG, "CSPSA_Open(%s) -> result %d, open counter: %u\n",
            ServerDaemonListItem_p->CspsaConfig.Name, Result, ServerDaemonListItem_p->OpenCounter);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_DeleteValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Key_t Key = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Result_t Result = CSPSA_CORE_DeleteValue(Handle, Key);

  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_Log(LOG_DEBUG, "CSPSA_DeleteValue(key: %u) -> result %d\n", Key, Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Flush(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Result_t Result = CSPSA_CORE_Flush(Handle);

  (void)ReadBuf_p;
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_Log(LOG_DEBUG, "CSPSA_Flush() -> result %d\n", Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Format(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Result_t Result = CSPSA_CORE_Format(Handle);

  (void)ReadBuf_p;
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_Log(LOG_DEBUG, "CSPSA_Format() -> result %d\n", Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfFirstValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Key_t Key = 0;
  CSPSA_Size_t Size = 0;
  CSPSA_Result_t Result = CSPSA_CORE_GetSizeOfFirstValue(Handle, &Key, &Size);

  M_CSPSA_PARAMETER_NOT_USED(ReadBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Key);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Size);
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfFirstValue() -> key: %u, size: %u, result %d\n",
            Key, Size, Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfNextValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  // Key is used as both input and output parameter
  CSPSA_Key_t Key = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Size_t Size = 0;
  CSPSA_Result_t Result = CSPSA_CORE_GetSizeOfNextValue(Handle, &Key, &Size);

  M_CSPSA_PARAMETER_NOT_USED(ReadBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Key);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Size);
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfNextValue() -> key: %u, size: %u, result %d\n",
            Key, Size, Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Key_t Key = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Size_t Size = 0;
  CSPSA_Result_t Result = CSPSA_CORE_GetSizeOfValue(Handle, Key, &Size);

  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Size);
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfValue(key: %u) -> size: %u, result %d\n", Key, Size, Result);
  return Result;
}

// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_ReadValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Key_t Key = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Size_t Size = (CSPSA_Size_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Data_t* Data_p = CSPSA_SOCKET_HeapMalloc(Size);

  CSPSA_Result_t Result = CSPSA_CORE_ReadValue(Handle, Key, Size, Data_p);

  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
  if (Result == T_CSPSA_RESULT_OK)
  {
    if (CSPSA_SOCKET_WriteToBuf(WriteBuf_p, Data_p, Size) < 0)
    {
      Result = T_CSPSA_RESULT_E_INTERNAL_ERROR;
    }
  }
  M_CSPSA_SOCKET_FREE(Data_p);
  CSPSA_Log(LOG_DEBUG, "CSPSA_ReadValue(key: %u, size: %u) -> result %d\n", Key, Size, Result);
  return Result;
}


// -----------------------------------------------------------------------------

static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_Command_WriteValue(
    CSPSA_Handle_t Handle,
    CSPSA_SOCKET_Buffer_t* ReadBuf_p,
    CSPSA_SOCKET_Buffer_t* WriteBuf_p
)
{
  CSPSA_Key_t Key = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  CSPSA_Size_t Size = (CSPSA_Size_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  uint8_t* Data_p = CSPSA_SOCKET_HeapMalloc(Size);
  int ReadResult = CSPSA_SOCKET_ReadFromBuf(ReadBuf_p, Data_p, Size);
  CSPSA_Result_t Result;

  if (ReadResult < 0)
  {
    Result = T_CSPSA_RESULT_E_INTERNAL_ERROR;
    goto ErrorExit;
  }
  else
  {
    Result = CSPSA_CORE_WriteValue(Handle, Key, Size, Data_p);
  }
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Result);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_WriteValue(key: %u, size: %u) -> result %d\n", Key, Size, Result);
  M_CSPSA_SOCKET_FREE(Data_p);
  return Result;
}

// -----------------------------------------------------------------------------


// - F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData: Handles an incoming request from a client
static CSPSA_Result_t F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData(
  const int FileDescriptor,
  CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t* ClientState_p,
  const uint32_t MaxPacketSize,
  const uint8_t* DataBuffer_p,
  const uint32_t Size
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  uint32_t Command;
  CSPSA_SOCKET_Buffer_t ReadBuf;
  CSPSA_SOCKET_Buffer_t WriteBuf;
  CSPSA_Handle_t Handle = V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p->CspsaHandle;

  ReadBuf.Packet_p = (uint8_t*)DataBuffer_p;
  ReadBuf.Length = Size;
  ReadBuf.MaxLength = MaxPacketSize;
  ReadBuf.fd = FileDescriptor;
  ReadBuf.CurrentIndex = sizeof(CSPSA_SOCKET_PacketHeader_t);
  WriteBuf.Packet_p = CSPSA_SOCKET_HeapMalloc(MaxPacketSize);
  CSPSA_SOCKET_InitBuf(&WriteBuf);
  WriteBuf.MaxLength = MaxPacketSize;
  WriteBuf.fd = FileDescriptor;
  // read command from buffer
  Command = CSPSA_SOCKET_ReadUint32(&ReadBuf);

  CSPSA_Log(LOG_DEBUG,
    "F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData(%u): fd(%u) Size(%u) Command(%s)\n",
    __LINE__,
    FileDescriptor,
    Size,
    CSPSA_SOCKET_CommandToString(Command)
  );

  switch (Command)
  {
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CLOSE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Close(
               V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p, ClientState_p, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CREATE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Create(
               V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p, ClientState_p, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_DELETE_VALUE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_DeleteValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FLUSH:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Flush(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FORMAT:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Format(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_NEXT_VALUE:
    {

      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfNextValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_FIRST_VALUE:
    {

      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfFirstValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_VALUE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_GetSizeOfValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_OPEN:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Open(
               V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p, ClientState_p, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_READ_VALUE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_ReadValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_WRITE_VALUE:
    {
      Result = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_WriteValue(Handle, &ReadBuf, &WriteBuf);
      break;
    }
    default:
    {
      break;
    }
  }
  // Send answer back to client
  (void)CSPSA_SOCKET_SendBuf(&WriteBuf);
  M_CSPSA_SOCKET_FREE(WriteBuf.Packet_p);

  CSPSA_Log(LOG_DEBUG,
    "F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData(%u): Result(%s)\n",
    __LINE__,
    CSPSA_SOCKET_ResultToString(Result)
  );
  return Result;
}

// -----------------------------------------------------------------------------

#if !CFG_CSPSA_ANDROID
static void F_CSPSA_API_LINUX_SOCKET_SERVER_SigTerm(int What)
{
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListIterator_p =
    V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;

  M_CSPSA_PARAMETER_NOT_USED(What);

  CSPSA_Log(LOG_DEBUG,
    "F_CSPSA_API_LINUX_SOCKET_SERVER_SigTerm(%u): What(%u)\n",
    __LINE__,
    What
  );

  while (ServerDaemonListIterator_p != NULL)
  {
    ServerDaemonListIterator_p->Terminated = true;
    ServerDaemonListIterator_p = ServerDaemonListIterator_p->Next_p;
  }

}
#endif

// - F_CSPSA_API_LINUX_SOCKET_SERVER_SetAccessRights: Set access rights on the daemon's socket
//                          based on CSPSA configuration. Return value < 0 on failure.
static int F_CSPSA_API_LINUX_SOCKET_SERVER_SetAccessRights(
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* const ServerDaemon_p
)
{
  CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) ServerDaemon_p->CspsaConfig.Extra_p;
  mode_t Mode = S_IRUSR | S_IWUSR;
  uint32_t AccessGroupNameLength = 0;
  int HaveNumericalGID = 0;
  char *EndPointer;
  int IntResult;
  int Result = -1;

  if ((LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_GROUP_READ) || (LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_GROUP_WRITE))
  {
    uid_t OwnerId = -1;
    gid_t GroupId = -1;

    // - Set access group.
    if (LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_GROUP_READ)
    {
      Mode |= S_IRGRP;
    }
    if (LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_GROUP_WRITE)
    {
      Mode |= S_IWGRP;
    }

    if (LinuxDevice_p->AccessGroupName_p != NULL)
    {
      AccessGroupNameLength = strlen(LinuxDevice_p->AccessGroupName_p);
      errno = 0;
      GroupId = strtol(LinuxDevice_p->AccessGroupName_p, &EndPointer, 10);
      if (errno != 0)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "invalid group id");
        goto ErrorExit;
      }
      // - Check if digits were found.
      if (EndPointer != LinuxDevice_p->AccessGroupName_p)
      {
        HaveNumericalGID = 1;
      }
    }

    if(AccessGroupNameLength != 0 && !HaveNumericalGID)
    {
      // - Read access groups to get group id for function chown().
      static const char EtcGroupFileName_p[] = "/etc/group";
      char* AccessGroupName_p = NULL;
      int FileHandle;
      struct stat StatData;
      uint32_t FileSize;
      char* DataBuffer_p = NULL;
      char* GroupIdString_p = NULL;

      IntResult = stat(EtcGroupFileName_p, &StatData);
      if (IntResult == -1)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "stat(/etc/group)");
        goto ErrorExit;
      }

      FileSize = StatData.st_size;
      DataBuffer_p = (char*) CSPSA_SOCKET_HeapMalloc(FileSize);
      if (DataBuffer_p == NULL)
      {
        goto ErrorExit;
      }

      FileHandle = open(EtcGroupFileName_p, O_RDONLY, 0);
      if (FileHandle < 0)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "open(/etc/group)");
        M_CSPSA_SOCKET_FREE(DataBuffer_p);
        goto ErrorExit;
      }

      IntResult = read(FileHandle, DataBuffer_p, FileSize);
      close(FileHandle);
      if (IntResult == 0)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "read(/etc/group)");
        M_CSPSA_SOCKET_FREE(DataBuffer_p);
        goto ErrorExit;
      }

      AccessGroupName_p = (char*) CSPSA_SOCKET_HeapMalloc(AccessGroupNameLength + 2);
      if (AccessGroupName_p == NULL)
      {
        M_CSPSA_SOCKET_FREE(DataBuffer_p);
        goto ErrorExit;
      }

      // - Format: "group1:x:id:user1,user2"
      strcpy(AccessGroupName_p, LinuxDevice_p->AccessGroupName_p);
      strcat(AccessGroupName_p, ":");
      GroupIdString_p = strstr(DataBuffer_p, AccessGroupName_p);
      if (GroupIdString_p == NULL)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "invalid group name");
        M_CSPSA_SOCKET_FREE(DataBuffer_p);
        M_CSPSA_SOCKET_FREE(AccessGroupName_p);
        goto ErrorExit;
      }

      if (GroupIdString_p == DataBuffer_p)
      {
        // - This is the very first id in the file. Find next ':'.
        GroupIdString_p = strchr(GroupIdString_p, ':');
        GroupIdString_p++;
        // - Find next ':'.
        GroupIdString_p = strchr(GroupIdString_p, ':');
        GroupIdString_p++;
        errno = 0;
        GroupId = strtol(GroupIdString_p, NULL, 10);
        if (errno != 0)
        {
          fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "invalid group id");
          M_CSPSA_SOCKET_FREE(DataBuffer_p);
          M_CSPSA_SOCKET_FREE(AccessGroupName_p);
          goto ErrorExit;
        }
      }
      else
      {
        // - We must check that the character before the id is a new line character. Otherwise continue
        //   searching for the group id.
        while (*(GroupIdString_p - 1) != '\n')
        {
          GroupIdString_p = strstr(DataBuffer_p, AccessGroupName_p);
          if (GroupIdString_p == NULL)
          {
            fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "invalid group name");
            M_CSPSA_SOCKET_FREE(DataBuffer_p);
            M_CSPSA_SOCKET_FREE(AccessGroupName_p);
            goto ErrorExit;
          }
        }
        // - Find next ':'.
        GroupIdString_p = strchr(GroupIdString_p, ':');
        GroupIdString_p++;
        // - Find next ':'.
        GroupIdString_p = strchr(GroupIdString_p, ':');
        GroupIdString_p++;
        errno = 0;
        GroupId = strtol(GroupIdString_p, NULL, 10);
        if (errno != 0)
        {
          fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "invalid group id");
          M_CSPSA_SOCKET_FREE(DataBuffer_p);
          M_CSPSA_SOCKET_FREE(AccessGroupName_p);
          goto ErrorExit;
        }
      }

      M_CSPSA_SOCKET_FREE(DataBuffer_p);
      M_CSPSA_SOCKET_FREE(AccessGroupName_p);
    }

    if (AccessGroupNameLength != 0)
    {
      IntResult = chown(ServerDaemon_p->SocketName_p, OwnerId, GroupId);
      if (IntResult != 0)
      {
        fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "chown");
        goto ErrorExit;
      }
    }
  }
  if (LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_OTHER_READ)
  {
    Mode |= S_IROTH;
  }
  if (LinuxDevice_p->GroupRwOtherRw & D_CSPSA_LL_ACCESS_RIGHTS_OTHER_WRITE)
  {
    Mode |= S_IWOTH;
  }
  IntResult = chmod(ServerDaemon_p->SocketName_p, Mode);
  if (IntResult != 0)
  {
    fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "chmod");
    goto ErrorExit;
  }
  Result = 0;
ErrorExit:
  return Result;
}


// -----------------------------------------------------------------------------

int P_CSPSA_API_LINUX_SOCKET_SERVER_Process(
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* const ServerDaemon_p
)
{
  int ServerSocketHandle;
  struct sockaddr_un ServerSocketAddress;
  int IntResult;
  int PollResult;
  uint8_t* DataBuffer_p = NULL;
  int MaxPacketSize = 0;
  struct pollfd FileDescriptorArray[CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS];
  CSPSA_API_LINUX_SOCKET_SERVER_ClientState_t ClientStateArray[CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS];
  int FileDescriptorArraySize = 0;
  int timeout = 1 * 60 * 1000; // - 1 minute timeout for poll. See CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE_TIMEOUT_IN_MIN before changing.
  bool ConsolidateFileDescriptorArray = false;
#if CFG_CSPSA_ANDROID
  CSPSA_Result_t CspsaResult;
#else
  int SignalFdIndex;
#endif

  // - Initiate file descriptor array for usage with poll().
  memset(FileDescriptorArray, 0 , sizeof(FileDescriptorArray));

#if !CFG_CSPSA_ANDROID
  // - Handle signal SIGTERM.
  {
    int SigFd;
    sigset_t SigMask;

    sigemptyset (&SigMask);
    sigaddset (&SigMask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &SigMask, NULL) < 0)
    {
      fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "sigprocmask");
      goto ErrorExit;
    }
    SigFd = signalfd (-1, &SigMask, 0);
    if (SigFd < 0)
    {
      fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "signalfd");
      goto ErrorExit;
    }

    SignalFdIndex = FileDescriptorArraySize;
    FileDescriptorArray[FileDescriptorArraySize].fd = SigFd;
    FileDescriptorArray[FileDescriptorArraySize].events = POLLIN;
    FileDescriptorArray[FileDescriptorArraySize].revents = 0;
    ClientStateArray[FileDescriptorArraySize].IsOpen = false;
    FileDescriptorArraySize++;
  }
#endif

  // - Create a socket server.
  ServerSocketHandle = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (ServerSocketHandle  < 0)
  {
    fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "socket");
    goto ErrorExit;
  }

  // - Bind socket to a socket name.
  memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
  ServerSocketAddress.sun_family = AF_UNIX;
  strcpy(ServerSocketAddress.sun_path, ServerDaemon_p->SocketName_p);

  (void) unlink(ServerDaemon_p->SocketName_p);

  IntResult = bind(ServerSocketHandle, (struct sockaddr *) &ServerSocketAddress, sizeof(ServerSocketAddress));
  if (IntResult < 0)
  {
    fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "bind");
    goto ErrorExit;
  }

  // - Set access rights on the socket
  if (F_CSPSA_API_LINUX_SOCKET_SERVER_SetAccessRights(ServerDaemon_p) < 0)
  {
    goto ErrorExit;
  }

  // - Get maximum packet size for the socket.
  MaxPacketSize = CSPSA_SOCKET_GetPacketSize(ServerSocketHandle);

  // - Allocate a buffer for receiving incomming data.
  DataBuffer_p = CSPSA_SOCKET_HeapMalloc(MaxPacketSize);

  // - Start listen on the socket.
  IntResult = listen(ServerSocketHandle, CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_CLIENTS);
  if (IntResult < 0)
  {
    fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "listen");
    goto ErrorExit;
  }

  if (FileDescriptorArraySize >= CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS)
  {
    fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "Out of FD's");
    goto ErrorExit;
  }
  // - Added socket to file descriptor array (for poll()).
  FileDescriptorArray[FileDescriptorArraySize].fd = ServerSocketHandle;
  FileDescriptorArray[FileDescriptorArraySize].events = POLLIN | POLLHUP;
  ClientStateArray[FileDescriptorArraySize].IsOpen = false;
  FileDescriptorArraySize++;

#if CFG_CSPSA_ANDROID
  // If the device isn't accessible to AID_SYSTEM, then open it before we lose root privileges.
  struct stat CspsaStat;
  IntResult = stat(((CSPSA_LinuxDevice_t*)ServerDaemon_p->CspsaConfig.Extra_p)->DeviceName_p, &CspsaStat);
  if (!IntResult && !((CspsaStat.st_uid == AID_SYSTEM && CspsaStat.st_mode & (S_IRUSR|S_IWUSR)) ||
    (CspsaStat.st_gid == AID_SYSTEM && CspsaStat.st_mode & (S_IRGRP|S_IWGRP)) ||
    (CspsaStat.st_mode & (S_IROTH|S_IWOTH))))
  {
    CSPSA_SOCKET_Buffer_t WriteBuf;
    WriteBuf.Packet_p = CSPSA_SOCKET_HeapMalloc(MaxPacketSize);
    WriteBuf.MaxLength = MaxPacketSize;
    CSPSA_SOCKET_InitBuf(&WriteBuf);
    CspsaResult = F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Open(ServerDaemon_p, &ClientStateArray[FileDescriptorArraySize], &WriteBuf, &WriteBuf);
    if (CspsaResult != T_CSPSA_RESULT_OK)
    {
      fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not persistently open the CSPSA area!\n\n", __LINE__);
    }
    // Close our connection: we will retain a file handle to any devices that aren't accessible to system:system.
   (void) F_CSPSA_API_LINUX_SOCKET_SERVER_Command_Close(ServerDaemon_p, &ClientStateArray[FileDescriptorArraySize], NULL, &WriteBuf);
  }
  // drop any root privileges
  if (setgid(AID_SYSTEM) != 0 || setuid(AID_SYSTEM) != 0) {
    fprintf(stderr, "\n[CSPSA, %u] ERROR: Could not drop root privileges!\n\n", __LINE__);
  }
#endif

  // - Read commands from clients and process them.
  while (!ServerDaemon_p->Terminated)
  {
    int i;

    // - Poll for events.
    PollResult = poll(FileDescriptorArray, FileDescriptorArraySize, timeout);
    if (PollResult < 0)
    {
      fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "poll");
      goto ErrorExit;
    }
    if (PollResult == 0)
    {
      // - The poll timeout is triggered.
#if CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE
      if (ServerDaemon_p->CloseOnNextTimeout > 0)
      {
        CSPSA_Result_t CSPSA_Result;
        ServerDaemon_p->CloseOnNextTimeout = 0;
        CSPSA_Result = CSPSA_CORE_Close(&ServerDaemon_p->CspsaHandle);
        CSPSA_Log(LOG_DEBUG, "CSPSA_Close(): -> result %d, Server attempting to close CSPSA at timeout\n", CSPSA_Result);
        fprintf(stderr, "CSPSA_API_LINUX_SOCKET_SERVER(%u): %u()\n", __LINE__, CSPSA_Result);
      }
#endif
      continue;
    }

    // - Go through entire file desciptor array and serve if needed.
    for (i = 0; i < FileDescriptorArraySize; i++)
    {
      if (FileDescriptorArray[i].revents == 0)
      {
        // - No event for this descriptor.
        continue;
      }

#if !CFG_CSPSA_ANDROID
      if (i == SignalFdIndex)
      {
        // - A signal (SIGTERM) has been received.
        struct signalfd_siginfo SigInfo;

        IntResult = read (FileDescriptorArray[i].fd, &SigInfo, sizeof(SigInfo));
        if (IntResult < 0)
        {
          fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "read(SigInfo)");
          goto ErrorExit;
        }

        if (SigInfo.ssi_signo == SIGTERM)
        {
          F_CSPSA_API_LINUX_SOCKET_SERVER_SigTerm(0);
        }

        continue;
      }
#endif

      if ((FileDescriptorArray[i].revents & POLLHUP) != 0)
      {
        CSPSA_Log(LOG_INFO, "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): Connection hung up (0x%08X)\n",
                 __LINE__, FileDescriptorArray[i].fd);
        /* coverity[negative_sink_in_call : FALSE] */
        close(FileDescriptorArray[i].fd);
        FileDescriptorArray[i].fd = -1;
        FileDescriptorArray[i].revents = 0;
        FileDescriptorArray[i].events = 0;
        if (ClientStateArray[i].IsOpen)
        {
          // The client closed the connection without closing the CSPSA. Decrease open counter.
          --ServerDaemon_p->OpenCounter;
          if (ServerDaemon_p->OpenCounter == 0)
          {
#if CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE
            ServerDaemon_p->CloseOnNextTimeout = CFG_CSPSA_API_LINUX_SOCKET_SERVER_DELAYED_CLOSE_TIMEOUT_IN_MIN;
            CSPSA_Log(LOG_DEBUG, "Client quit without closing; will close CSPSA on next timeout in %u minutes, open counter: %u\n",
                    ServerDaemon_p->CloseOnNextTimeout,
                    ServerDaemon_p->OpenCounter);
#else
            CSPSA_Result_t Result = CSPSA_CORE_Close(&ServerDaemon_p->CspsaHandle);
            (void) Result;
            CSPSA_Log(LOG_DEBUG, "CSPSA_Close() -> result %d, open counter: %u\n",
                    Result, ServerDaemon_p->OpenCounter);
#endif
          }
          CSPSA_Log(LOG_DEBUG, "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): Open counter = %u\n",
                __LINE__, ServerDaemon_p->OpenCounter);
        }
        ConsolidateFileDescriptorArray = true;
        continue;
      }

      // - If anything else than POLLIN and POLLHUP is received it is an error.
      if ((FileDescriptorArray[i].revents & ~(POLLIN | POLLHUP)) != 0)
      {
        fprintf(stderr, "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): revents (0x%08X) != POLLIN\n", __LINE__, FileDescriptorArray[i].revents);
        goto ErrorExit;
      }

      if (FileDescriptorArray[i].fd == ServerSocketHandle)
      {
        int NewSocketDescriptor;

        // - One or several incoming connection(s). Accept them all.

        NewSocketDescriptor = accept(ServerSocketHandle, NULL, NULL);
        if (NewSocketDescriptor < 0)
        {
          fprintf(stderr, "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): accept\n", __LINE__);
          goto ErrorExit;
        }
        CSPSA_Log(LOG_INFO, "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): accept(0x%08X)\n", __LINE__, NewSocketDescriptor);
        FileDescriptorArray[FileDescriptorArraySize].fd = NewSocketDescriptor;
        FileDescriptorArray[FileDescriptorArraySize].revents = 0;
        FileDescriptorArray[FileDescriptorArraySize].events = POLLIN | POLLHUP;
        ClientStateArray[FileDescriptorArraySize].IsOpen = false;
        FileDescriptorArraySize++;
        if (FileDescriptorArraySize > CFG_CSPSA_API_LINUX_SOCKET_SERVER_FREE_MAX_NBR_OF_FDS)
        {
          fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "Out of FD's");
          goto ErrorExit;
        }
      }
      else
      {
        // - Try to receive data on this connection. If read returns 0 the connection
        //   is closed.

        IntResult = recv(FileDescriptorArray[i].fd, DataBuffer_p, MaxPacketSize, MSG_DONTWAIT);
        if (IntResult < 0)
        {
          if (errno != EWOULDBLOCK)
          {
            fprintf(stderr, C_CSPSA_API_LINUX_SOCKET_SERVER_ErrorString, __LINE__, "read");
            goto ErrorExit;
          }
        }
        else if (IntResult > 0)
        {
          (void) F_CSPSA_API_LINUX_SOCKET_SERVER_HandleData(FileDescriptorArray[i].fd,
                 &ClientStateArray[i], MaxPacketSize, DataBuffer_p, IntResult);
          //
        }
        else
        {
          // 0 means the connection is closed by the client. In this case, in the next call to poll,
          // a POLLHUP event will be received and then the socket will be closed.
          // For the moment, nothing needs to be done.
        }
      }
    }

    if (ConsolidateFileDescriptorArray)
    {
      int j;

      // - Remove closed clients.
      ConsolidateFileDescriptorArray = false;
      for (i = 0; i < FileDescriptorArraySize; i++)
      {
        /* coverity[check_after_sink : FALSE] */
        if (FileDescriptorArray[i].fd == -1)
        {
          for(j = i; j < FileDescriptorArraySize; j++)
          {
            FileDescriptorArray[j] = FileDescriptorArray[j+1];
            ClientStateArray[j] = ClientStateArray[j+1];
          }
          FileDescriptorArraySize--;
        }
      }
    }

  } // - while (true).

ErrorExit:

  (void) unlink(ServerDaemon_p->SocketName_p);
  M_CSPSA_SOCKET_FREE(DataBuffer_p);

  CSPSA_Log(LOG_INFO,
    "P_CSPSA_API_LINUX_SOCKET_SERVER_Process(%u): Exited %s.\n",
    __LINE__,
    (ServerDaemon_p->Terminated ? "normally" : "abnormally")
  );
  return (!ServerDaemon_p->Terminated);
}


// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_API_LINUX_SOCKET_SERVER_Create(const CSPSA_Config_t* const Config_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
  char* SocketName_p = CSPSA_SOCKET_CreateSocketName(Config_p->Name);
  int DeviceNameLength = strlen(LinuxDevice_p->DeviceName_p);
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListIterator_p =
    V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;

  while (ServerDaemonListIterator_p != NULL)
  {
    if (strcmp(SocketName_p, ServerDaemonListIterator_p->SocketName_p) == 0)
    {
      // - This CSPSA server daemon already exists.
      Result = T_CSPSA_RESULT_OK;
      break;
    }
    ServerDaemonListIterator_p = ServerDaemonListIterator_p->Next_p;
  }

  if (ServerDaemonListIterator_p == 0)
  {
    CSPSA_LinuxDevice_t* NewLinuxDevice_p = NULL;
    CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* NewServerDaemonListItem_p;
    int Pid;

    // - Clean up possible left overs.
    (void) unlink(SocketName_p);

    Pid = fork(); // <------------------------------------------------ !!! FORK !!!

    // - Create new server daemon list item.
    NewServerDaemonListItem_p = (CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t*)
      CSPSA_SOCKET_HeapMalloc(sizeof(*NewServerDaemonListItem_p));
    memset(NewServerDaemonListItem_p, 0, sizeof(*NewServerDaemonListItem_p));
    // - Copy config settings.
    NewServerDaemonListItem_p->CspsaConfig = *Config_p;
    // - Copy device name.
    NewLinuxDevice_p = (CSPSA_LinuxDevice_t*) CSPSA_SOCKET_HeapMalloc(sizeof(CSPSA_LinuxDevice_t));
    memset(NewLinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
    NewLinuxDevice_p->DeviceName_p = (char*) CSPSA_SOCKET_HeapMalloc(DeviceNameLength);
    strcpy(NewLinuxDevice_p->DeviceName_p, LinuxDevice_p->DeviceName_p);
    NewServerDaemonListItem_p->CspsaConfig.Extra_p = NewLinuxDevice_p;
    // - Insert socket name.
    NewServerDaemonListItem_p->SocketName_p = SocketName_p;
    NewServerDaemonListItem_p->Pid = Pid;
    SocketName_p = NULL;

    // - Insert new server daemon item in list.
    NewServerDaemonListItem_p->Next_p = V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;
    V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p = NewServerDaemonListItem_p;

    if (Pid == 0)
    {
      int IntResult;
      // - The child process continues here.
      IntResult = P_CSPSA_API_LINUX_SOCKET_SERVER_Process(NewServerDaemonListItem_p);
      NewLinuxDevice_p = NewServerDaemonListItem_p->CspsaConfig.Extra_p; // - Re-cycling "NewLinuxDevice_p" (not so new anymore).
      M_CSPSA_SOCKET_FREE(NewLinuxDevice_p->DeviceName_p);
      M_CSPSA_SOCKET_FREE(NewLinuxDevice_p);
      M_CSPSA_SOCKET_FREE(NewServerDaemonListItem_p->SocketName_p);
      M_CSPSA_SOCKET_FREE(NewServerDaemonListItem_p);
      if (Config_p->MemoryType == T_CSPSA_MEMORY_TYPE_RAM)
      {
        // - Next line's purpose is to silent Valgrind when running test application.
        free((uint8_t*)(ptrdiff_t)Config_p->StartOffsetInBytes);
      }
      CSPSA_RemoveParameterArea(Config_p);
      NewLinuxDevice_p = Config_p->Extra_p;
      // - Confusingly enough the NewLinuxDevice_p->DeviceName_p is not allocated memory.
      M_CSPSA_SOCKET_FREE(NewLinuxDevice_p);
      exit(IntResult);
    }
    else
    {
      // - TBD: Will we ever need info in Config? Otherwise free allocated resources.
      ;
    }
    Result = T_CSPSA_RESULT_OK;
  }

  M_CSPSA_SOCKET_FREE(SocketName_p);

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_API_LINUX_SOCKET_SERVER_Destroy(const CSPSA_Config_t* const Config_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_LinuxDevice_t* LinuxDevice_p = Config_p->Extra_p;
  char* SocketName_p = CSPSA_SOCKET_CreateSocketName(Config_p->Name);
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* ServerDaemonListIterator_p =
    V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* LastServerDaemonListIterator_p =
    V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;

  while (ServerDaemonListIterator_p != NULL)
  {
    if (strcmp(SocketName_p, ServerDaemonListIterator_p->SocketName_p) == 0)
    {
      // - This CSPSA server daemon already exists.
      kill(ServerDaemonListIterator_p->Pid, SIGTERM);
      waitpid(ServerDaemonListIterator_p->Pid, NULL, 0);

      LinuxDevice_p = ServerDaemonListIterator_p->CspsaConfig.Extra_p;
      if (LinuxDevice_p != NULL)
      {
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->DeviceName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p->AccessGroupName_p);
        M_CSPSA_SOCKET_FREE(LinuxDevice_p);
      }
      M_CSPSA_SOCKET_FREE(ServerDaemonListIterator_p->SocketName_p);

      if (ServerDaemonListIterator_p == V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p)
      {
        V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p = ServerDaemonListIterator_p->Next_p;
      }
      else
      {
        LastServerDaemonListIterator_p->Next_p = ServerDaemonListIterator_p->Next_p;
      }
      M_CSPSA_SOCKET_FREE(ServerDaemonListIterator_p);
      break;
    }
    LastServerDaemonListIterator_p = ServerDaemonListIterator_p;
    ServerDaemonListIterator_p = ServerDaemonListIterator_p->Next_p;
  }

  (void) unlink(SocketName_p);
  M_CSPSA_SOCKET_FREE(SocketName_p);

  return Result;
}



// =============================================================================
//   End of file
// =============================================================================
