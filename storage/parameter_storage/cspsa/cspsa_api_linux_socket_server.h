/* **************************************************************************
 *
 * cspsa_api_linux_socket_server.h
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
 * Exposes a CSPSA Linux userspace socket server API.
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_SERVER_H
#define INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_SERVER_H
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h> // - Standard definitions.
#include <stdint.h> // - Standard types.
#include <stdbool.h>

// =============================================================================
//  Defines
// =============================================================================

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Types
// =============================================================================

// - CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t: Struct with info about CSPSA server
//   daemon.
typedef struct CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t
{
  CSPSA_Config_t CspsaConfig;
  CSPSA_Handle_t CspsaHandle;
  int Pid;
  char* SocketName_p;
  struct CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* Next_p;
  bool Terminated;
  uint32_t OpenCounter;
  int CloseOnNextTimeout;
} CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t;

// =============================================================================
//  Global External Variables
// =============================================================================

// - V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p: List of started CSPSA
//   server daemons.
extern CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* V_CSPSA_API_LINUX_SOCKET_SERVER_ServerDaemonListItem_p;

// =============================================================================
//  Global Function Declarations
// =============================================================================

CSPSA_Result_t CSPSA_API_LINUX_SOCKET_SERVER_Create(const CSPSA_Config_t* const Config_p);
CSPSA_Result_t CSPSA_API_LINUX_SOCKET_SERVER_Destroy(const CSPSA_Config_t* const Config_p);

int P_CSPSA_API_LINUX_SOCKET_SERVER_Process(
  CSPSA_API_LINUX_SOCKET_SERVER_DaemonItem_t* const ServerDaemon_p);


// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_API_LINUX_SOCKET_SERVER_H
// =============================================================================
