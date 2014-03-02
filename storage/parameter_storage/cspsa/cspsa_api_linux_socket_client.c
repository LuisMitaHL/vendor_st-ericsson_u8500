/* **************************************************************************
 *
 * cspsa_api_linux_socket_client.c
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
 * Implements a CSPSA Linux userspace API.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_API_LINUX_SOCKET_CLIENT_MAX_CONNECTION_TIMEOUT: The timeout between
//   two connection attempts is doubled until the timeout exceeds this value.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_CLIENT_MAX_CONNECTION_TIMEOUT
#define CFG_CSPSA_API_LINUX_SOCKET_CLIENT_MAX_CONNECTION_TIMEOUT      (20 * 1000 * 1000)
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_api_linux_socket_common.h"

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Local Types
// =============================================================================

// - CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t: Struct to keep info about
//   client socket connection.
typedef struct CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t
{
  char* ParameterAreaName_p;
  char* SocketName_p;
  int ClientSocketHandle;
  CSPSA_SOCKET_Buffer_t WriteBuf;
  CSPSA_SOCKET_Buffer_t ReadBuf;
  struct CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* Next_p;
} CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t;

// =============================================================================
//  Local Constants
// =============================================================================

// - C_CSPSA_API_LINUX_USERSPACE_ErrorString: General purpose error string.
static const char C_CSPSA_API_LINUX_USERSPACE_ErrorString[] =
  "CSPSA_API_LINUX_USERSPACE(%u): %s()\n";

// =============================================================================
//  Local Variables
// =============================================================================


// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* const ClientSocketItem_p,
  const CSPSA_API_LINUX_SOCKET_COMMON_Command_t Command
);
static CSPSA_Result_t F_CSPSA_API_LINUX_USERSPACE_Client_Connect(
  const char* const ParameterAreaName_p,
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t** const ClientSocketListItem_pp
);

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_API_LINUX_USERSPACE_Client_SetPacketSize(): sets packet size,
//             allocate read/write buffers
static void F_CSPSA_API_LINUX_USERSPACE_Client_SetPacketSize(
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* const ClientSocketItem_p,
  uint32_t MaxPacketSize
)
{
  CSPSA_SOCKET_Buffer_t* WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p = &ClientSocketItem_p->ReadBuf;

  // If necessary adjust read/write buffers
  if (MaxPacketSize != ReadBuf_p->MaxLength)
  {
    CSPSA_Log(LOG_DEBUG, "F_CSPSA_API_LINUX_USERSPACE_Client_SetPacketSize(size: %u)\n", MaxPacketSize);
    M_CSPSA_SOCKET_FREE(ReadBuf_p->Packet_p);
    ReadBuf_p->Packet_p = CSPSA_SOCKET_HeapMalloc(MaxPacketSize);
    ReadBuf_p->MaxLength = MaxPacketSize;
    M_CSPSA_SOCKET_FREE(WriteBuf_p->Packet_p);
    WriteBuf_p->Packet_p = CSPSA_SOCKET_HeapMalloc(MaxPacketSize);
    WriteBuf_p->MaxLength = MaxPacketSize;
  }
}

// -----------------------------------------------------------------------------

// - F_CSPSA_API_LINUX_USERSPACE_Client_Connect(): Creates a socket client and connects
//   to a CSPSA socket server.
static CSPSA_Result_t F_CSPSA_API_LINUX_USERSPACE_Client_Connect(
  const char* const ParameterAreaName_p,
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t** const ClientSocketListItem_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p = NULL;
  struct sockaddr_un ClientSocketAddress;
  char* SocketName_p = NULL;
  int IntResult = -1;
  int ClientSocketHandle;
  uint32_t MaxPacketSize;
  useconds_t Timeout = 10;

  if (ClientSocketListItem_pp == NULL || ParameterAreaName_p == NULL)
  {
    /* Return default error code. */
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  /* Function allocates memory, so we need to free it later. */
  SocketName_p = CSPSA_SOCKET_CreateSocketName(ParameterAreaName_p);

  if (SocketName_p == NULL)
  {
    /* Return default error code. */
    return T_CSPSA_RESULT_E_SOCKET_ERROR;
  }

  // - First create a socket handle.
  /* coverity[negative_return_fn : FALSE] */
  ClientSocketHandle = socket(AF_UNIX, SOCK_SEQPACKET, 0);

  // - Then set address family and socket (file) name.
  memset(&ClientSocketAddress, 0, sizeof(ClientSocketAddress));
  ClientSocketAddress.sun_family = AF_UNIX;
  strcpy(ClientSocketAddress.sun_path, SocketName_p);

  ClientSocketItem_p = CSPSA_SOCKET_HeapMalloc(sizeof(*ClientSocketItem_p));
  if (ClientSocketItem_p == NULL)
  {
    M_CSPSA_SOCKET_FREE(SocketName_p);
    return T_CSPSA_RESULT_E_OUT_OF_MEMORY;
  }

  memset(ClientSocketItem_p, 0, sizeof(*ClientSocketItem_p));
  ClientSocketItem_p->ParameterAreaName_p = strdup(ParameterAreaName_p);
  /* coverity[alias_assign: FALSE] */
  ClientSocketItem_p->ClientSocketHandle = ClientSocketHandle;
  ClientSocketItem_p->SocketName_p = SocketName_p;

  // - Try to connect to server.
  do
  {
    /* coverity[negative_returns : FALSE] */
    IntResult = connect(ClientSocketHandle, (struct sockaddr *) &ClientSocketAddress, sizeof(ClientSocketAddress));
    if (IntResult == 0)
    {
      break;
    }
    usleep(Timeout);
    Timeout *= 2;
  } while (Timeout < CFG_CSPSA_API_LINUX_SOCKET_CLIENT_MAX_CONNECTION_TIMEOUT);

  if (IntResult < 0)
  {
    // - Max timeout has been reached and connection attempts have failed.
    goto ErrorExit;
  }

  *ClientSocketListItem_pp = ClientSocketItem_p;

  // - Allocate buffers for sending and receiving data.
  MaxPacketSize =  CSPSA_SOCKET_GetPacketSize(ClientSocketHandle);
  ClientSocketItem_p->WriteBuf.fd = ClientSocketHandle;
  ClientSocketItem_p->ReadBuf.fd = ClientSocketHandle;
  F_CSPSA_API_LINUX_USERSPACE_Client_SetPacketSize(ClientSocketItem_p, MaxPacketSize);

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  if (Result != T_CSPSA_RESULT_OK)
  {
      M_CSPSA_SOCKET_FREE(ClientSocketItem_p->SocketName_p);
      M_CSPSA_SOCKET_FREE(ClientSocketItem_p->ParameterAreaName_p);
      M_CSPSA_SOCKET_FREE(ClientSocketItem_p);
  }
  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(): Sends a command
//   without data.
static CSPSA_Result_t F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* const ClientSocketItem_p,
  const CSPSA_API_LINUX_SOCKET_COMMON_Command_t Command
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Command);
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  CSPSA_Log(LOG_DEBUG,
    "F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(0x%08X): Command(%s) IntResult(%i) errno(0x%08X)\n",
    ClientSocketItem_p->ClientSocketHandle,
    CSPSA_SOCKET_CommandToString(Command),
    BufResult,
    errno
  );
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG,
    "F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(0x%08X): Result(%s) IntResult(%i) errno(0x%08X)\n",
    ClientSocketItem_p->ClientSocketHandle,
    CSPSA_SOCKET_ResultToString(Result),
    BufResult,
    errno
  );
  return Result;
}

// - F_CSPSA_API_LINUX_USERSPACE_Client_CreateOrOpen:
//              Handles sending a create or open request
//              (common code, since these are from a client perspective equal commands).
CSPSA_Result_t F_CSPSA_API_LINUX_USERSPACE_Client_CreateOrOpen(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p,
  CSPSA_API_LINUX_SOCKET_COMMON_Command_t Command
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p = NULL;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;
  uint32_t MaxPacketSize;

  if ((ParameterAreaName_p == NULL) || (Handle_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  *Handle_p = 0;

  // - Connect client.
  Result = F_CSPSA_API_LINUX_USERSPACE_Client_Connect(ParameterAreaName_p, &ClientSocketItem_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  // - The handle is a pointer to a client socket item struct.
  *Handle_p = (CSPSA_Handle_t) ClientSocketItem_p;

  // - Send create command.
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Command);
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
    goto ErrorExit;
  }
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
    goto ErrorExit;
  }
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  // Server sends the packet size to be used in the rest of the messages in a response to open/create
  MaxPacketSize = CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  F_CSPSA_API_LINUX_USERSPACE_Client_SetPacketSize(ClientSocketItem_p, MaxPacketSize);
ErrorExit:
  if ((Result != T_CSPSA_RESULT_OK) && (*Handle_p != 0))
  {
    CSPSA_Close(Handle_p);
  }
  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_Close(CSPSA_Handle_t* const Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;

  if ((Handle_p == NULL) || (*Handle_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) *Handle_p;

  // - Send CSPSA close command.
  Result = F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(ClientSocketItem_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CLOSE);

  // - Close socket and free resources.
  (void) close(ClientSocketItem_p->ClientSocketHandle);

  M_CSPSA_SOCKET_FREE(ClientSocketItem_p->SocketName_p);
  M_CSPSA_SOCKET_FREE(ClientSocketItem_p->ParameterAreaName_p);
  M_CSPSA_SOCKET_FREE(ClientSocketItem_p->WriteBuf.Packet_p);
  M_CSPSA_SOCKET_FREE(ClientSocketItem_p->ReadBuf.Packet_p);
  M_CSPSA_SOCKET_FREE(ClientSocketItem_p);

  *Handle_p = NULL;
  CSPSA_Log(LOG_DEBUG, "CSPSA_Close() -> result %d\n", Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Create(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  CSPSA_Result_t Result = F_CSPSA_API_LINUX_USERSPACE_Client_CreateOrOpen(
           ParameterAreaName_p, Handle_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CREATE);

  CSPSA_Log(LOG_DEBUG, "CSPSA_Create(%s) -> result %d\n", ParameterAreaName_p, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_DeleteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if (Handle == 0)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_DELETE_VALUE);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)Key);
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_DeleteValue(key: %u) -> result %d\n", Key, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Flush(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;

  if (Handle == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  // - Send flush command.
  Result = F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(ClientSocketItem_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FLUSH);

  CSPSA_Log(LOG_DEBUG, "CSPSA_Flush() -> result %d\n", Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Format(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;

  if (Handle == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  // - Send format command.
  Result = F_CSPSA_API_LINUX_USERSPACE_Client_SendSimpleCommand(ClientSocketItem_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FORMAT);

  CSPSA_Log(LOG_DEBUG, "CSPSA_Format() -> result %d\n", Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Open(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  CSPSA_Result_t Result = F_CSPSA_API_LINUX_USERSPACE_Client_CreateOrOpen(
           ParameterAreaName_p, Handle_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_OPEN);

  CSPSA_Log(LOG_DEBUG, "CSPSA_Open(%s) -> result %d\n", ParameterAreaName_p, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfNextValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if ((Handle == 0) || (Key_p == NULL) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_NEXT_VALUE);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, (uint32_t)*Key_p);
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  *Key_p = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  *Size_p = (CSPSA_Size_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfNextValue() -> key: %u, size: %u, result %d\n",
            *Key_p, *Size_p, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfFirstValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if ((Handle == 0) || (Key_p == NULL) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_FIRST_VALUE);
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  *Key_p = (CSPSA_Key_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  *Size_p = (CSPSA_Size_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfFirstValue() -> key: %u, size: %u, result %d\n",
            *Key_p, *Size_p, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if ((Handle == 0) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  // Create request
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_VALUE);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Key);
  // Send request
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Receive response
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Extract data from response
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  *Size_p = (CSPSA_Size_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_GetSizeOfValue(key: %u) -> size: %u, result %d\n", Key, *Size_p, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_ReadValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if ((Handle == 0) || (Size == 0) || (Data_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  // Create request
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_READ_VALUE);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Key);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Size);
  // Send request
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Receive response
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Extract output parameters from response
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
  if (Result == T_CSPSA_RESULT_OK)
  {
    // Read data
    BufResult = CSPSA_SOCKET_ReadFromBuf(ReadBuf_p, Data_p, Size);
    if (BufResult < 0)
    {
      Result = T_CSPSA_RESULT_E_INTERNAL_ERROR;
      goto ErrorExit;
    }
  }
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_ReadValue(key: %u, size: %u) -> result %d\n", Key, Size, Result);
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_WriteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_SOCKET_ERROR;
  CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t* ClientSocketItem_p;
  CSPSA_SOCKET_Buffer_t* WriteBuf_p;
  CSPSA_SOCKET_Buffer_t* ReadBuf_p;
  ssize_t BufResult;

  if ((Handle == 0) || (Size == 0) || (Data_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  ClientSocketItem_p = (CSPSA_API_LINUX_USERSPACE_ClientSocketItem_t*) Handle;
  WriteBuf_p = &ClientSocketItem_p->WriteBuf;
  // Create request
  CSPSA_SOCKET_InitBuf(WriteBuf_p);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_WRITE_VALUE);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Key);
  CSPSA_SOCKET_WriteUint32(WriteBuf_p, Size);
  BufResult = CSPSA_SOCKET_WriteToBuf(WriteBuf_p, Data_p, Size);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Send request
  BufResult = CSPSA_SOCKET_SendBuf(WriteBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Receive response
  ReadBuf_p = &ClientSocketItem_p->ReadBuf;
  BufResult = CSPSA_SOCKET_ReceiveBuf(ReadBuf_p);
  if (BufResult < 0)
  {
    goto ErrorExit;
  }
  // Extract output parameters from response
  Result = (CSPSA_Result_t)CSPSA_SOCKET_ReadUint32(ReadBuf_p);
ErrorExit:
  CSPSA_Log(LOG_DEBUG, "CSPSA_WriteValue(key: %u, size: %u) -> result %d\n", Key, Size, Result);
  return Result;
}

// =============================================================================
//   End of file
// =============================================================================
