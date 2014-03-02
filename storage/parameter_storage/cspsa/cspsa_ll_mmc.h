/* **************************************************************************
 *
 * cspsa_ll_mmc.h
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
#ifndef INCLUSION_GUARD_CSPSA_LL_MMC_H
#define INCLUSION_GUARD_CSPSA_LL_MMC_H
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

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Types
// =============================================================================

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
CSPSA_Result_t CSPSA_LL_MMC_Open(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Closes a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_MMC_Close(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Fetches the sector size of the current storage media.
 *
 * @param [in]  Handle        A handle for this session.
 * @param [out] BlockSize_p   Pointer to where block size will be written.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_MMC_GetSectorSize(
  const CSPSA_Handle_t Handle,
  uint32_t* const SectorSize_p
);

// -----------------------------------------------------------------------------

/**
 * Fetches the block size of the current storage media.
 *
 * @param [in]  Handle        A handle for this session.
 * @param [out] BlockSize_p   Pointer to where block size will be written.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_MMC_GetBlockSize(
  const CSPSA_Handle_t Handle,
  uint32_t* const BlockSize_p
);

// -----------------------------------------------------------------------------

/**
 * Reads data from a flash memory device.
 *
 * @param [in]  Handle        A handle for this session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_LL_MMC_Read(
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
CSPSA_Result_t CSPSA_LL_MMC_Write(
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
CSPSA_Result_t CSPSA_LL_MMC_Erase(
  const CSPSA_Handle_t Handle,
  const uint64_t Offset,
  const uint32_t Size
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
CSPSA_Result_t CSPSA_LL_MMC_Format(const CSPSA_Handle_t Handle);

// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_LL_MMC_H
// =============================================================================
