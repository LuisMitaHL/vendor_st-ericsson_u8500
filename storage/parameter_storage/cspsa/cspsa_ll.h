/* **************************************************************************
 *
 * cspsa_ll.h
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
 * Low level interface to handle flash memories.
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_LL_H
#define INCLUSION_GUARD_CSPSA_LL_H
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
#include "cspsa.h"

// =============================================================================
//  Defines
// =============================================================================

#define D_CSPSA_LL_ACCESS_RIGHTS_OTHER_WRITE    (1 << 0)
#define D_CSPSA_LL_ACCESS_RIGHTS_OTHER_READ     (1 << 1)
#define D_CSPSA_LL_ACCESS_RIGHTS_GROUP_WRITE    (1 << 2)
#define D_CSPSA_LL_ACCESS_RIGHTS_GROUP_READ     (1 << 3)

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_PARAMETER_NOT_USED: Removes warning for unused parameters.
#define M_CSPSA_PARAMETER_NOT_USED(Parameter) (void)(Parameter)

// =============================================================================
//  Types
// =============================================================================

/**
 * Linux device properties.
 *
 */
typedef struct
{
  char* DeviceName_p;
  int   FileDescriptor;
  char* AccessGroupName_p;
  uint32_t GroupRwOtherRw;
} CSPSA_LinuxDevice_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================

/**
 * Opens a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Open(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Closes a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Close(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Reads data from a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Read(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size,
  const bool ReadBackwards
);

// -----------------------------------------------------------------------------

/**
 * Write data to a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Write(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size,
  const bool WriteBackwards
);

// -----------------------------------------------------------------------------

/**
 * Erases part of a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Erase(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const bool EraseBackwards
);

// -----------------------------------------------------------------------------

/**
 * Formats (erases) media.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_Format(const CSPSA_Handle_t Handle);

// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_LL_H
// =============================================================================
