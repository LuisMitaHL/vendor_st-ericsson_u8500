/* **************************************************************************
 *
 * cspsa_api_linux_socket_common.c
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
 * Implements some common functions for Linux userspace API code.
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
#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
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

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// =============================================================================
//   Global Function Definitions
// =============================================================================

void CSPSA_SOCKET_InitBuf(CSPSA_SOCKET_Buffer_t* Buf_p)
{
  CSPSA_SOCKET_PacketHeader_t* Info_p = (CSPSA_SOCKET_PacketHeader_t*)Buf_p->Packet_p;

  Info_p->PacketSeqNr = 0;
  Info_p->IsLast = 1;
  Buf_p->Length = sizeof(CSPSA_SOCKET_PacketHeader_t);
  Buf_p->CurrentIndex = Buf_p->Length;
}

ssize_t CSPSA_SOCKET_SendBuf(CSPSA_SOCKET_Buffer_t* Buf_p)
{
  uint8_t* Packet_p = Buf_p->Packet_p;
  return send(Buf_p->fd, Packet_p, Buf_p->Length, 0);
}

ssize_t CSPSA_SOCKET_WriteToBuf(CSPSA_SOCKET_Buffer_t* Buf_p, const uint8_t* Data_p, uint32_t Length)
{
  uint8_t* Packet_p = Buf_p->Packet_p;
  CSPSA_SOCKET_PacketHeader_t* Info_p = (CSPSA_SOCKET_PacketHeader_t*)Packet_p;
  uint32_t IndexInData = 0;
  int Result = 0;
  uint32_t RemainingLength = Length;

  while (Result >= 0 && RemainingLength > Buf_p->MaxLength - Buf_p->Length)
  {
    // Buffer is not big enough to contain all data
    uint32_t BytesToCopy = Buf_p->MaxLength - Buf_p->Length;
    uint32_t SeqNr = Info_p->PacketSeqNr;

    // Fill buffer to the brim
    memcpy(&Packet_p[Buf_p->Length], &Data_p[IndexInData], BytesToCopy);
    IndexInData += BytesToCopy;
    // Send buffer with "not last" indication
    Info_p->IsLast = 0;
    Buf_p->Length = Buf_p->MaxLength;
    Result = CSPSA_SOCKET_SendBuf(Buf_p);
    // Reset buffer
    CSPSA_SOCKET_InitBuf(Buf_p);
    Info_p->PacketSeqNr = SeqNr + 1;
    RemainingLength -= BytesToCopy;
  }
  if (Result >= 0 && RemainingLength > 0)
  {
    memcpy(&Packet_p[Buf_p->Length], &Data_p[IndexInData], RemainingLength);
    Buf_p->Length += RemainingLength;
  }
  return Result;
}

void CSPSA_SOCKET_WriteUint32(CSPSA_SOCKET_Buffer_t* Buf_p, uint32_t Data)
{
  uint8_t* Dest_p = &Buf_p->Packet_p[Buf_p->Length];

  *Dest_p++ = Data & 0xff;
  Data >>= 8;
  *Dest_p++ = Data & 0xff;
  Data >>= 8;
  *Dest_p++ = Data & 0xff;
  Data >>= 8;
  *Dest_p = Data & 0xff;
  Buf_p->Length += 4;
}

ssize_t CSPSA_SOCKET_ReceiveBuf(CSPSA_SOCKET_Buffer_t* Buf_p)
{
  ssize_t Result;

  Result = recv(Buf_p->fd, Buf_p->Packet_p, Buf_p->MaxLength, 0);
  Buf_p->Length = Result;
  Buf_p->CurrentIndex = sizeof(CSPSA_SOCKET_PacketHeader_t);
  return Result;
}

ssize_t CSPSA_SOCKET_ReadFromBuf(CSPSA_SOCKET_Buffer_t* Buf_p, uint8_t* Data_p, uint32_t Length)
{
  uint8_t* Packet_p = Buf_p->Packet_p;
  CSPSA_SOCKET_PacketHeader_t* Info_p = (CSPSA_SOCKET_PacketHeader_t*)Packet_p;
  uint32_t IndexInData = 0;
  uint32_t RemainingLength = Length;
  ssize_t Result = -1;

  while (RemainingLength > 0)
  {
    uint32_t RemainingInBuf = Buf_p->Length - Buf_p->CurrentIndex;
    uint32_t BytesToCopy = Length;

    if (BytesToCopy > RemainingInBuf)
    {
      BytesToCopy = RemainingInBuf;
    }
    // copy data that is available in current packet
    memcpy(&Data_p[IndexInData], &Packet_p[Buf_p->CurrentIndex], BytesToCopy);
    Buf_p->CurrentIndex += BytesToCopy;
    IndexInData += BytesToCopy;
    RemainingLength -= BytesToCopy;

    if (RemainingLength > 0)
    {
      // The reader wants to read more data than is available in the current packet
      if (Info_p->IsLast)
      {
        // This was the last packet, so not all data can be delivered
        CSPSA_Log(LOG_ERR, "ReadFromBuf error: cannot deliver %d bytes, buffer too small\n", Length);
        Result = -1;
        goto ErrorExit;
      }
      else
      {
        // Receive the next packet
        Result = CSPSA_SOCKET_ReceiveBuf(Buf_p);
        if (Result <= 0)
        {
          CSPSA_Log(LOG_ERR, "ReadFromBuf error: receive failed\n");
          goto ErrorExit;
        }
      }
    }
    else
    {
      // Final result; reader got all requested bytes
      Result = (ssize_t)Length;
    }
  }
ErrorExit:
  return Result;
}

uint32_t CSPSA_SOCKET_ReadUint32(CSPSA_SOCKET_Buffer_t* Buf_p)
{
  uint32_t Data;
  uint8_t* Src_p = &Buf_p->Packet_p[Buf_p->CurrentIndex];

  Data = *Src_p;
  ++Src_p;
  Data += ((*Src_p) << 8);
  ++Src_p;
  Data += ((*Src_p) << 16);
  ++Src_p;
  Data += ((*Src_p) << 24);
  Buf_p->CurrentIndex += 4;
  return Data;
}

//------------------------------------------------------------------------------
//  LOGGING
//------------------------------------------------------------------------------

/**
 * printf-like function that writes to the log.
 * @param level As used in syslog.
 */
#ifdef DEBUG
void CSPSA_Log(int Level, const char *Fmt_p, ...)
{
  va_list VaList;

  M_CSPSA_PARAMETER_NOT_USED(Level); //Only LOG_DEBUG is used today

  va_start(VaList, Fmt_p);
  vfprintf(stdout, Fmt_p, VaList);
  va_end(VaList);
}
#endif //DEBUG

//------------------------------------------------------------------------------
//  HELP FUNCTIONS
//------------------------------------------------------------------------------

// - CSPSA_SOCKET_CreateSocketName: Creates a socket name from
//   a config struct.

char* CSPSA_SOCKET_CreateSocketName(const char* const ParameterAreaName_p)
{
  const char* SocketPath = CFG_CSPSA_API_LINUX_SOCKET_COMMON_SOCKET_PATH;
  int SocketPathLength = strlen(SocketPath);
  int SocketNameLength = strlen(ParameterAreaName_p);
  char* SocketName_p = CSPSA_SOCKET_HeapMalloc(SocketPathLength + SocketNameLength + 1);
  int i;

  strcpy(SocketName_p, SocketPath);
  strcat(SocketName_p, ParameterAreaName_p);
  SocketNameLength = strlen(SocketName_p);

  // - Replace all occurencies of '/' in the device name (excluding path of course) with '_'.
  for (i = SocketPathLength; i < SocketNameLength; i++)
  {
    if (SocketName_p[i] == '/')
    {
      SocketName_p[i] = '_';
    }
  }

  return SocketName_p;
}

int CSPSA_SOCKET_GetPacketSize(int Socket)
{
  int MaxPacketSize = 0;
  socklen_t SocketOptionLength = sizeof(MaxPacketSize);
  int IntResult = getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, &MaxPacketSize, &SocketOptionLength);

  if ((IntResult < 0) || (uint32_t)MaxPacketSize < sizeof(CSPSA_SOCKET_PacketHeader_t) )
  {
    // This cannot happen
    fprintf(stderr, "GetPacketSize error: getsockopt fails\n");
    return 128; // Return something small and hope for the best
  }
  // Override packet size
  if (MaxPacketSize > CFG_CSPSA_API_LINUX_SOCKET_COMMON_MAX_PACKET_SIZE)
  {
    MaxPacketSize = CFG_CSPSA_API_LINUX_SOCKET_COMMON_MAX_PACKET_SIZE;
  }
  CSPSA_Log(LOG_DEBUG, "CSPSA_SOCKET_GetPacketSize): MaxPacketSize(%u)\n", MaxPacketSize);
  return MaxPacketSize;
}
// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_HeapMalloc: Wrapper for malloc() which never fails.

void* CSPSA_SOCKET_HeapMalloc(const uint32_t Size)
{
  uint8_t* Result;
  uint32_t AlignedSize = Size;

  // - Make sure Size is 32-bit aligned because of padding of the data field in
  //   a parameter value and don't assume the compiler does this anyway.
  if ((AlignedSize & 3) != 0)
  {
    AlignedSize = (AlignedSize + 3) & ~3;
  }
  // - Loop until we get a heap allocation. Since CSPSA may reside in different
  //   OS'es don't remove this loop!
  do
  {
    Result = (uint8_t*) malloc(AlignedSize);
    if (Result == NULL)
    {
      usleep(10000);
    }
  } while (Result == NULL);

  return Result;
}

// -----------------------------------------------------------------------------

#ifdef DEBUG
// - CSPSA_SOCKET_CommandToString: Prints a socket command.
char* CSPSA_SOCKET_CommandToString(const CSPSA_API_LINUX_SOCKET_COMMON_Command_t Command)
{
  char* Result_p = "unknown";
  switch (Command)
  {
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CLOSE:                   Result_p = "Close"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CREATE:                  Result_p = "Create"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_DELETE_VALUE:            Result_p = "Delete"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FLUSH:                   Result_p = "Flush"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FORMAT:                  Result_p = "Format"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_FIRST_VALUE: Result_p = "GetSizeOfFirstValue"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_NEXT_VALUE:  Result_p = "GetSizeOfNextValue"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_VALUE:       Result_p = "GetSizeOfValue"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_OPEN:                    Result_p = "Open"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_READ_VALUE:              Result_p = "Read"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_WRITE_VALUE:             Result_p = "Write"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_NBR_OF_ITEMS:            Result_p = "NbrOfItems"; break;
    case T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_UNDEFINED:               Result_p = "Undefined"; break;
  }
  return Result_p;
}
#endif //DEBUG

// -----------------------------------------------------------------------------

#ifdef DEBUG
// - CSPSA_SOCKET_ResultToString: Prints CSPSA result in text.
char* CSPSA_SOCKET_ResultToString(const CSPSA_Result_t Result)
{
  char* Result_p = "Unknown";

  #define M_MACRO_TO_STRING(__n) case __n: Result_p = #__n;break;
  switch (Result)
  {
    M_MACRO_TO_STRING(T_CSPSA_RESULT_OK)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_WARNINGS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_NEW_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_INVALID_HEADER_FOUND)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_ERRORS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_BAD_PARAMETER)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_SPACE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_NO_VALID_IMAGE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ONLY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_WRITE_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_END_OF_DATA)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OPEN_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_ALREADY_EXISTS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_MEMORY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INTERNAL_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_SOCKET_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_NUMBER_OF_ITEMS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_UNDEFINED)
  }
  #undef M_MACRO_TO_STRING
  return Result_p;
}
#endif //DEBUG

// =============================================================================
//   End of file
// =============================================================================
