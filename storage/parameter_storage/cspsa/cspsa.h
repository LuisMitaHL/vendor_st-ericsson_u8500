/* **************************************************************************
 *
 * cspsa.h
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
 * API for Crash Safe Parameter Storage Area (CSPSA). See file "cspsa_ds.html".
 *
 **************************************************************************** */

// =============================================================================
#ifndef INCLUSION_GUARD_CSPSA_H
#define INCLUSION_GUARD_CSPSA_H
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

// - D_CSPSA_MAX_NAME_SIZE: Max length of a CSPSA parameter area name, incl. terminating 0.
#define D_CSPSA_MAX_NAME_SIZE    100

// - D_CSPSA_DEFAULT_HANDLE: Default handle for the default CSPSA.
#define D_CSPSA_DEFAULT_HANDLE    1

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Types
// =============================================================================

/**
 * List of possible parameter area function results.
 *
 * @param T_CSPSA_RESULT_OK                     Operation performed successfully.
 *
 * @param T_CSPSA_RESULT_WARNINGS               Start marker of warning messages.
 * @param T_CSPSA_RESULT_W_SIZE                 Warning, if requested parameter value size
 *                                              differs from stored value size.
 * @param T_CSPSA_RESULT_W_NEW_KEY              Warning, if parameter key is new.
 * @param T_CSPSA_RESULT_W_INVALID_HEADER_FOUND Warning, one header was invalid
 *
 * @param T_CSPSA_RESULT_ERRORS                 Start marker of error messages.
 * @param T_CSPSA_RESULT_E_BAD_PARAMETER        Error because of bad input parameter.
 * @param T_CSPSA_RESULT_E_INVALID_KEY          Error, parameter key not valid.
 * @param T_CSPSA_RESULT_E_INVALID_SIZE         Error, parameter size not valid.
 * @param T_CSPSA_RESULT_E_OUT_OF_SPACE         Error, there is not space enough on memory media to update the
 *                                              parameter area.
 * @param T_CSPSA_RESULT_E_NO_VALID_IMAGE       Error, no valid CSPSA image found.
 * @param T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE   Memory media could not be accessed.
 * @param T_CSPSA_RESULT_E_READ_ONLY            Image is read-only.
 * @param T_CSPSA_RESULT_E_READ_ERROR           Error occurred while reading from media.
 * @param T_CSPSA_RESULT_E_WRITE_ERROR          Error occurred while writing to media.
 * @param T_CSPSA_RESULT_E_ERASE_ERROR          Error occurred while erasing media.
 * @param T_CSPSA_RESULT_E_END_OF_DATA          No more parameters, end of data has been reached.
 * @param T_CSPSA_RESULT_E_OPEN_ERROR           Parameter storage area could not be opened (media error).
 * @param T_CSPSA_RESULT_E_ALREADY_EXISTS       Parameter storage area with same name was already registered.
 * @param T_CSPSA_RESULT_E_OUT_OF_MEMORY        There was not enough memory to perform the operation.
 * @param T_CSPSA_RESULT_E_INTERNAL_ERROR       An internal error occurred.
 * @param T_CSPSA_RESULT_E_SOCKET_ERROR         Socket error occurred; the server could be down.
 *
 * @param T_CSPSA_RESULT_NUMBER_OF_ITEMS        Number of items in this enum.
 * @param T_CSPSA_RESULT_UNDEFINED              Represents an undefined value of this enum.
 */
typedef enum
{
  T_CSPSA_RESULT_OK = 0,
  // - - -
  T_CSPSA_RESULT_WARNINGS,
  T_CSPSA_RESULT_W_SIZE,
  T_CSPSA_RESULT_W_NEW_KEY,
  T_CSPSA_RESULT_W_INVALID_HEADER_FOUND,
  // - - -
  T_CSPSA_RESULT_ERRORS,
  T_CSPSA_RESULT_E_BAD_PARAMETER,
  T_CSPSA_RESULT_E_INVALID_KEY,
  T_CSPSA_RESULT_E_INVALID_SIZE,
  T_CSPSA_RESULT_E_OUT_OF_SPACE,
  T_CSPSA_RESULT_E_NO_VALID_IMAGE,
  T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE,
  T_CSPSA_RESULT_E_READ_ONLY,
  T_CSPSA_RESULT_E_READ_ERROR,
  T_CSPSA_RESULT_E_WRITE_ERROR,
  T_CSPSA_RESULT_E_END_OF_DATA,
  T_CSPSA_RESULT_E_OPEN_ERROR,
  T_CSPSA_RESULT_E_ALREADY_EXISTS,
  T_CSPSA_RESULT_E_OUT_OF_MEMORY,
  T_CSPSA_RESULT_E_INTERNAL_ERROR,
  T_CSPSA_RESULT_E_SOCKET_ERROR,
  // - - -
  T_CSPSA_RESULT_NUMBER_OF_ITEMS,
  T_CSPSA_RESULT_UNDEFINED
} CSPSA_Result_t;

// -----------------------------------------------------------------------------

/**
 * List of supported persistant memory media API.
 *
 * @param T_CSPSA_MEMORY_TYPE_DEV_MTD         MTD device.
 * @param T_CSPSA_MEMORY_TYPE_DEV_MMC         MMC device.
 * @param T_CSPSA_MEMORY_TYPE_EMMC            eMMC physical driver.
 * @param T_CSPSA_MEMORY_TYPE_MMC             MMC physical driver.
 * @param T_CSPSA_MEMORY_TYPE_NAND_RAW        Raw NAND physical driver.
 * @param T_CSPSA_MEMORY_TYPE_NAND_MANAGED    Managed NAND physical driver.
 * @param T_CSPSA_MEMORY_TYPE_NOR             NOR physical driver.
 * @param T_CSPSA_MEMORY_TYPE_RAM             RAM image. Mainly used for testing or
 *                                            in a tool.
 * @param T_CSPSA_MEMORY_TYPE_FILE            CSPSA stored in file. Mainly used for
 *                                            in tools.
 *
 */
typedef enum
{
  T_CSPSA_MEMORY_TYPE_DEV_MTD = 0,
  T_CSPSA_MEMORY_TYPE_DEV_MMC,
  T_CSPSA_MEMORY_TYPE_EMMC,
  T_CSPSA_MEMORY_TYPE_MMC,
  T_CSPSA_MEMORY_TYPE_NAND_RAW,
  T_CSPSA_MEMORY_TYPE_NAND_MANAGED,
  T_CSPSA_MEMORY_TYPE_NOR,
  T_CSPSA_MEMORY_TYPE_RAM,
  T_CSPSA_MEMORY_TYPE_FILE,
  // - - -
  T_CSPSA_MEMORY_TYPE_NUMBER_OF_ITEMS,
  T_CSPSA_MEMORY_TYPE_UNDEFINED
} CSPSA_MemoryType_t;

// -----------------------------------------------------------------------------

/**
 * Memory media configuration.
 *
 * @param MemoryType            Memory media type.
 * @param StartOffsetInBytes    Offset in bytes to start of parameter area.
 * @param SizeInBytes           Size in bytes of parameter area.
 * @param Attributes            Parameter storage attributes. Should by default be set to 0.
 * @param Name_p                Name of the parameter area.
 * @param Extra_p               Extra information (dependent on the memory media type)
 *                              that may be needed for certain memory types.
 *                              For most memory types this must be set to NULL.
 * @param SizeOfExtra           The size of the Extra_p field (0 if Extra_p is NULL)
 */
typedef struct
{
  CSPSA_MemoryType_t        MemoryType;
  uint64_t                  StartOffsetInBytes;
  uint32_t                  SizeInBytes;
  uint32_t                  BlockSize;
  uint32_t                  SectorSize;
  uint32_t                  Attributes;
  char                      Name[D_CSPSA_MAX_NAME_SIZE];
  void*                     Extra_p;
  uint32_t                  SizeOfExtra;
} CSPSA_Config_t;

/**
 * Read-only attribute for CSPSA_Config_t.Attributes. If (Config.Attributes & CSPSA_READ_ONLY) is non-zero,
 * the parameter storage is read-only.
 */
#define D_CSPSA_READ_ONLY 1

// -----------------------------------------------------------------------------

/**
 * Parameter storage area handle. The format of the handle is private to CSPSA.
 */
typedef void* CSPSA_Handle_t;

// -----------------------------------------------------------------------------

/**
 * Parameter key format.
 */
typedef uint32_t CSPSA_Key_t;

// -----------------------------------------------------------------------------

/**
 * Parameter size format.
 */
typedef uint32_t CSPSA_Size_t;

// -----------------------------------------------------------------------------

/**
 * Parameter data format.
 */
typedef uint8_t CSPSA_Data_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================

/**
 * Opens a parameter storage area with the given name.
 *
 * @param [in]  ParameterAreaName_p  The name of the parameter storage area.
 * @param [out] Handle_p  A handle for this opened session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_OPEN_ERROR If no storage area with the given name exists.
 * @retval T_CSPSA_RESULT_W_INVALID_HEADER_FOUND If one of the CSPSA headers was not valid.
 * @retval T_CSPSA_RESULT_E_NO_VALID_IMAGE If a valid CSPSA image could not be found.
 */
CSPSA_Result_t CSPSA_Open(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
);

// -----------------------------------------------------------------------------

/**
 * Closes a parameter storage area. Cached data is stored to memory media before it is freed.
 * A Linux userspace function do normally not need to call this function if it is
 * using the default parameter storage area. Setting 'Handle' to D_CSPSA_DEFAULT_HANDLE
 * will be sufficient in calls to read, write, delete and flush. It is not possible
 * to close CSPSA with the default handle.
 *
 * @param [out] Handle  A handle for this opened session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_OUT_OF_SPACE If the image cannot be fit into the CSPSA area.
 * @retval T_CSPSA_RESULT_E_READ_ONLY If the CSPSA area is read-only.
 */
CSPSA_Result_t CSPSA_Close(CSPSA_Handle_t* const Handle_p);

// -----------------------------------------------------------------------------

/**
 * Gets the size of a value.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be read.
 * @param [out]     Size_p    Pointer to where the size of parameter value will be stored.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If parameter value for Key could no be found.
 */
CSPSA_Result_t CSPSA_GetSizeOfValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_Size_t* const Size_p
);

// -----------------------------------------------------------------------------

/**
 * Gets the size and key of the first parameter value.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [out]     Key_p     The key of the first parameter value.
 * @param [out]     Size_p    Pointer to where the size of parameter value will be stored.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_END_OF_DATA If the storage area does not contain any parameters.
 */
CSPSA_Result_t CSPSA_GetSizeOfFirstValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
);

// -----------------------------------------------------------------------------

/**
 * Gets the size and key of the parameter value in the storage area that has the next-higher
 * key compared to the supplied key.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [in,out]  Key_p     In: the key of an existing parameter value, out: the next-higher key.
 * @param [out]     Size_p    Pointer to where the size of parameter value will be stored.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_END_OF_DATA If there is no parameter with a higher key.
 */
CSPSA_Result_t CSPSA_GetSizeOfNextValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
);

// -----------------------------------------------------------------------------

/**
 * Reads a parameter value from a parameter storage area.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be read.
 * @param [in]      Size      The maximum number of bytes to read into Data_p.
 * @param [out]     Data_p    A pointer where parameter data will be written.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_W_SIZE If *Size_p is different than actual parameter size.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If parameter value for Key could no be found.
 */
CSPSA_Result_t CSPSA_ReadValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  CSPSA_Data_t* const Data_p
);

// -----------------------------------------------------------------------------

/**
 * Writes a value to an parameter storage area. Written values are
 * cached in RAM until a function call to CSPSA_Flush() is done.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be written.
 * @param [in]      Size      The maximum number of bytes to read into Data_p.
 * @param [in]      Data_p    A pointer with parameter data.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_W_SIZE If *Size_p is different than actual parameter size.
 * @retval T_CSPSA_RESULT_W_NEW_KEY If parameter key did not exist before this function call.
 */
CSPSA_Result_t CSPSA_WriteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
);

// -----------------------------------------------------------------------------

/**
 * Removes a parameter value. Value is deleted from RAM cache until a function call
 * to CSPSA_Flush() is done.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be written.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If item could not be found.
 */
CSPSA_Result_t CSPSA_DeleteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key
);

// -----------------------------------------------------------------------------

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @param [in]      Handle    A handle to a parameter storage area. The handle is either
 *                            set to D_CSPSA_DEFAULT_HANDLE or retrieved with a call to
 *                            function CSPSA_Open().
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_OUT_OF_SPACE If there is not space enough to store entire parameter storage area data record.
 */
CSPSA_Result_t CSPSA_Flush(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Parameter storage area will be erased.
 *
 * @param [out] Handle  A handle for this opened session. Using D_CSPSA_DEFAULT_HANDLE
 *                      is not possible.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_Format(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Creates a parameter storage area if it could not be opened. Normally only used
 * by tools and test applications.
 *
 * @param [in]  ParameterAreaName_p  The name of the parameter storage area.
 * @param [out] Handle_p  A handle for this created session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_Create(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
);

// -----------------------------------------------------------------------------

/**
 * Gets the number of registered parameter storage areas.
 *
 * @return The number of registered parameter areas.
 */
uint32_t CSPSA_GetParameterAreaListLength(void);

// -----------------------------------------------------------------------------

/**
 * Gets information about the registered parameter storage area with the given index.
 *
 * @param [in] Index The index of the parameter storage area.
 * @param [out] Config_p Information about the parameter storage area will be put here.
 * *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_GetParameterArea(uint32_t Index, CSPSA_Config_t* Config_p);

// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_H
// =============================================================================
