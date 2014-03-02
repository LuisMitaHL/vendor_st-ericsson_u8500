/* **************************************************************************
 *
 * cspsa_plugin.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Louis Verhaard <louis.xl.verhaard@stericsson.com>
 *
 * DESCRIPTION:
 *
 * Plugin interface to handle flash memories.
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_PLUGIN_H
#define INCLUSION_GUARD_CSPSA_PLUGIN_H
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

/**
 * Opens a parameter storage area.
 * @param [in] Config_p Configuration parameters
 *
 * @retval T_CSPSA_RESULT_OK Operation performed successfully.
 */
typedef CSPSA_Result_t (*F_CSPSA_Open_t)(const CSPSA_Config_t* Config_p, void** Handle_pp);

// -----------------------------------------------------------------------------

/**
 * Closes a parameter storage area.
 * @param [in] Config_p Configuration parameters
 *
 * @retval T_CSPSA_RESULT_OK Operation performed successfully.
 */
typedef CSPSA_Result_t (*F_CSPSA_Close_t)(void* Handle_p);

// -----------------------------------------------------------------------------

/**
 * Reads data from the parameter storage area.
 *
 * @param [in] Config_p Configuration parameters
 * @param [in] Offset   The offset, in bytes, from the start of the parameter storage area that is to be read.
 * @param [out] Data_p  The plugin will put the read bytes here
 * @param [in] Size     The number of bytes to be read. This number will at most be the block size of the parameter storage
 *                      area as returned by GetBlockSize.
 *
 * @retval T_CSPSA_RESULT_OK           Operation performed successfully.
 * @retval T_CSPSA_RESULT_E_READ_ERROR Error occurred while reading from media.
 */
typedef CSPSA_Result_t (*F_CSPSA_Read_t)(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
);

// -----------------------------------------------------------------------------

/**
 * Writes a block of data to the parameter storage area.
 *
 * @param [in] Config_p Configuration parameters
 * @param [in] Offset   The offset, in bytes, from the start of the parameter storage area that is to be read.
 * @param [out] Data_p  The plugin will put the read bytes here
 * @param [in] Size     The number of bytes to be read. This number will at most be the block size of the parameter storage
 *                      area as returned by GetBlockSize.
 *
 * @retval T_CSPSA_RESULT_OK            Operation performed successfully.
 * @retval T_CSPSA_RESULT_E_WRITE_ERROR Error occurred while writing to media.
 */
typedef CSPSA_Result_t (*F_CSPSA_Write_t)(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
);

// -----------------------------------------------------------------------------

/**
 * Erases the complete parameter storage area.
 *
 * @param [in] Config_p     Configuration parameters
 * @param [out] BlockSize_p The plugin will supply the block size in this parameter
 *
 * @retval T_CSPSA_RESULT_OK Operation performed successfully.
 * @retval T_CSPSA_RESULT_E_WRITE_ERROR Error occurred while writing to media.
 */
typedef CSPSA_Result_t (*F_CSPSA_Format_t)(void* Handle_p);

// -----------------------------------------------------------------------------

/**
 * Erases a block in the parameter storage area.
 *
 * @param [in] Config_p Configuration parameters
 * @param [in] Offset   The offset, in bytes, from the start of the parameter storage area that is to be erased.
 *
 * @retval T_CSPSA_RESULT_OK            Operation performed successfully.
 * @retval T_CSPSA_RESULT_E_ERASE_ERROR Error occurred while writing to media.
 */
typedef CSPSA_Result_t (*F_CSPSA_Erase_t)(
  void* Handle_p,
  const uint64_t Offset
);

// -----------------------------------------------------------------------------

/**
 * List of functions that a CSPSA plugin needs to implement.
 *
 * @param Open              Function to open driver.
 * @param Close             Function to close driver.
 * @param GetSectorSize     Function to get sector size.
 * @param GetBlockSize      Function to get block size.
 * @param Read              Function to read from flash memory.
 * @param Write             Function to write to flash memory
 * @param Format            Function to format (erase) flash memory.
 */
typedef struct
{
  F_CSPSA_Open_t Open;
  F_CSPSA_Close_t Close;
  F_CSPSA_Read_t Read;
  F_CSPSA_Write_t Write;
  F_CSPSA_Format_t Format;
  F_CSPSA_Erase_t Erase;
} CSPSA_PluginOperations_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================

/**
 * Registers a CSPSA plugin.
 *
 * @param [in]  MemoryType   The memory type that the plugin supports.
 * @param [in]  Operations_p The implementation of the plugin.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_RegisterPlugin(CSPSA_MemoryType_t MemoryType, const CSPSA_PluginOperations_t* Operations_p);

// -----------------------------------------------------------------------------

/**
 * Registers a parameter storage area to CSPSA.
 *
 * @param [in]  Config_p  Configuration for this parameter area.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_ALREADY_EXISTS A parameter area with the given name already exists
 */
CSPSA_Result_t CSPSA_AddParameterArea(const CSPSA_Config_t* const Config_p);

// -----------------------------------------------------------------------------

/**
 * Removes a (registered) parameter storage area to CSPSA.
 *
 * @param [in]  Config_p  Configuration for this parameter area.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_ALREADY_EXISTS A parameter area with the given name already exists
 */
CSPSA_Result_t CSPSA_RemoveParameterArea(const CSPSA_Config_t* const Config_p);

// -----------------------------------------------------------------------------

/**
 * Initializes the given Config with default values.
 *
 * @param [in, out] The configuration to be initialized.
 */
void CSPSA_InitConfig(CSPSA_Config_t* const Config_p);

// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_PLUGIN_H
// =============================================================================
