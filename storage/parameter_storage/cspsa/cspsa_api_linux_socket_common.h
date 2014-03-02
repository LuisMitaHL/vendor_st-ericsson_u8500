/* **************************************************************************
 *
 * cspsa_api_linux_socket_common.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 *
 * Exposes some common functions for Linux userspace API code.
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_COMMON_H
#define INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_COMMON_H
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_API_LINUX_SOCKET_COMMON_MAX_PACKET_SIZE:
// The max packet size of a SOCK_SEQPACKET based socket can be quite big (like over 100 K).
// To save heap space, it makes sense to use smaller size in CSPSA; default should be enough.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_COMMON_MAX_PACKET_SIZE
#define CFG_CSPSA_API_LINUX_SOCKET_COMMON_MAX_PACKET_SIZE 4096
#endif

// - CFG_CSPSA_API_LINUX_SOCKET_COMMON_SOCKET_PATH:
//  Path where the CSPSA-related named sockets are to be stored.
// Should be stored in /var/run in normal linux, but that directory doesn't
// exist in Android.
#ifndef CFG_CSPSA_API_LINUX_SOCKET_COMMON_SOCKET_PATH
#define CFG_CSPSA_API_LINUX_SOCKET_COMMON_SOCKET_PATH "/tmp/cspsa_socket_"
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h> // - Standard definitions.
#include <stdint.h> // - Standard types.
#include <stdbool.h>
#include <syslog.h>

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_SOCKET_FREE: Wrapper to free a pointer.
#define M_CSPSA_SOCKET_FREE(Pointer) \
  do                          \
  {                           \
    free(Pointer);            \
    Pointer = NULL;           \
  } while (0)

// -----------------------------------------------------------------------------

// - M_CSPSA_PARAMETER_NOT_USED: Removes warning for unused parameters.
#define M_CSPSA_PARAMETER_NOT_USED(Parameter) (void)(Parameter)

// =============================================================================
//  Types
// =============================================================================

typedef enum
{
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CLOSE = 0,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_CREATE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_DELETE_VALUE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FLUSH,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_FORMAT,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_FIRST_VALUE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_NEXT_VALUE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_GET_SIZE_OF_VALUE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_OPEN,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_READ_VALUE,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_WRITE_VALUE,
  // - - -
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_NBR_OF_ITEMS,
  T_CSPSA_API_LINUX_SOCKET_COMMON_COMMAND_UNDEFINED
} CSPSA_API_LINUX_SOCKET_COMMON_Command_t;

// -----------------------------------------------------------------------------

/*
  Brief protocol specification between CSPSA socket based client and server:

  The communication is packet based. The client sends requests, the server
  answers with responses.

  A request/response might not fit into one packet and will in that case e split into multiple packets.

  Every packet starts with a packet header:
  - 2 bytes sequence number (0 for first packet of a request, 1 for next etc)
  - 2 bytes "is last" indication (0 means another packet will come that belongs to the same request/response)

  i.e. the normal case, where a request/response fits in 1 packet, the packet has sequence number 0, is last = 1.

  The next 4 bytes in a client request contains a command, defined in CSPSA_API_LINUX_SOCKET_COMMON_Command_t.
  The rest contain the input parameters to the command.
  If the request spans more than 1 packet, the other packets only contain data (apart from the packet header);
  the command parameter is not repeated.

  The server response's 4 bytes after the packet header contains the CSPSA result, followed by the output parameters.

  Packet size: It is important that the client and server use the same packet size. Therefore the server determines
  the packet size and sends it in response to an open/close message.
  The very first message sent by a client (i.e. open or create) is so little that packet size is no initial issue.
*/

// - CSPSA_SOCKET_PacketHeader_t: Every sent CSPSA packet starts with this header
typedef struct
{
  // packet sequence number
  uint16_t PacketSeqNr;
  // is 1 if this packet is the last in a sequence of packets belonging to 1 request or response
  uint16_t IsLast;
} CSPSA_SOCKET_PacketHeader_t;

// -----------------------------------------------------------------------------

// - CSPSA_SOCKET_Buffer_t: Buffer used to read/write packets
typedef struct
{
  // contains data
  uint8_t* Packet_p;
  // length of the data
  uint32_t Length;
  // maximum length in bytes of the data
  uint32_t MaxLength;
  // file descriptor
  int fd;
  // points to next byte to read (only used when reading)
  uint32_t CurrentIndex;
} CSPSA_SOCKET_Buffer_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================


//------------------------------------------------------------------------------
//  LOGGING
//------------------------------------------------------------------------------

// - CSPSA_SOCKET_InitBuf: resets packet header
void CSPSA_SOCKET_InitBuf(CSPSA_SOCKET_Buffer_t* Buf_p);

// - CSPSA_SOCKET_SendBuf: Sends the contents of the buffer (using the buffer's file descriptor)
ssize_t CSPSA_SOCKET_SendBuf(CSPSA_SOCKET_Buffer_t* Buf_p);

// - CSPSA_SOCKET_WriteToBuf: Writes the given bytes to the buffer. If the buffer gets full,
//              it is sent. Returns number of bytes written, or a value < 0 if an error occurs.
ssize_t CSPSA_SOCKET_WriteToBuf(CSPSA_SOCKET_Buffer_t* Buf_p, const uint8_t* Data_p, uint32_t Length);

// - CSPSA_SOCKET_WriteUint32: Writes an uint32_t to the buffer.
void CSPSA_SOCKET_WriteUint32(CSPSA_SOCKET_Buffer_t* Buf_p, uint32_t Data);

// - CSPSA_SOCKET_ReceiveBuf: Receives a packet using the buffer's file descriptor.
//              Returns size of the received packet, -1 if an error occurred,
//              or 0 if connection to remote node failed
ssize_t CSPSA_SOCKET_ReceiveBuf(CSPSA_SOCKET_Buffer_t* Buf_p);

// - CSPSA_SOCKET_ReadFromBuf: Reads Length bytes from the buffer. If the buffer becomes empty,
//              under the hood new packets are received until all bytes have been read.
//              Returns number of bytes read, a value < 0 if an error occurs,
//              or 0 if connection to remote node failed
ssize_t CSPSA_SOCKET_ReadFromBuf(CSPSA_SOCKET_Buffer_t* Buf_p, uint8_t* Data_p, uint32_t Length);

// - CSPSA_SOCKET_ReadUint32: Reads an uint32_t from the buffer.
uint32_t CSPSA_SOCKET_ReadUint32(CSPSA_SOCKET_Buffer_t* Buf_p);

//------------------------------------------------------------------------------
//  LOGGING
//------------------------------------------------------------------------------

/**
 * printf-like function that writes to the log.
 * @param level As used in syslog.
 */
//TODO: Make this debug a command line option
#ifdef DEBUG
void CSPSA_Log(int Level, const char *Fmt_p, ...);
#else
// if no debugging is set: compile away all calls to CSPSA_Log
#define CSPSA_Log(Level, Fmt_p, ...)
#endif

//------------------------------------------------------------------------------
//  HELP FUNCTIONS
//------------------------------------------------------------------------------

// - CSPSA_SOCKET_CreateSocketName: Creates a socket name from
//   a config struct.
char* CSPSA_SOCKET_CreateSocketName(const char* const ParameterAreaName_p);

// - CSPSA_SOCKET_GetPacketSize: Gets max packet size for the socket.
int CSPSA_SOCKET_GetPacketSize(int Socket);

// - CSPSA_SOCKET_HeapMalloc: Wrapper for malloc that never fails.
void* CSPSA_SOCKET_HeapMalloc(const uint32_t Size);


// - CSPSA_SOCKET_CommandToString: Converts a command to a string.
char* CSPSA_SOCKET_CommandToString(const CSPSA_API_LINUX_SOCKET_COMMON_Command_t Command);

// - CSPSA_SOCKET_ResultToString: Converts a CSPSA result to a string.
char* CSPSA_SOCKET_ResultToString(const CSPSA_Result_t Result);



// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_COMMON_H
// =============================================================================
