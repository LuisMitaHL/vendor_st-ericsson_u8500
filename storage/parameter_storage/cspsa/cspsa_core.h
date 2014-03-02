/* **************************************************************************
 *
 * cspsa_core.h
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
#ifndef INCLUSION_GUARD_CSPSA_CORE_H
#define INCLUSION_GUARD_CSPSA_CORE_H
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER: Creates an extra header. Originally the idea
//   was to keep an extra header to solve the possible retention problem of an
//   interrupted write. However, it seems enough to just erase the old header
//   after a completed write to mark that the write operations was finalized. This
//   also saves some time to void the extra write and decreases complexity.
// - NOTE: This flag is kept for now, but eventually it ought to be removed.
// - The reason this flag is put here is that it is also used in test code.
#ifndef CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
#define CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER                      (0)
#endif

// - CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE: Sets the maximum size of a
//   CSPSA image header in multiples of the flash memory's sector/page size.
#ifndef CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE
#define CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE            (1)
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stddef.h> // - Standard definitions.
#include <stdint.h> // - Standard types.
#include <stdbool.h>
#include "cspsa.h"
#include "cspsa_ll.h"

// =============================================================================
//  Defines
// =============================================================================

// - D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS: The amount of fast parameters that
//   are stored in the CSPSA header. These can be accessed quickly by only
//   reading the header and skipping reading up and caching the rest of the
//   parameter storage area.
#define D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS              (4)

// - D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY: The key for the first fast parameter.
#define D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY       (UINT32_MAX - D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS + 1)

// =============================================================================
//  Macros
// =============================================================================

// =============================================================================
//  Types
// =============================================================================

/**
 * Structure of a CSPSA header version 2.
 *
 * @param Magic         Magic number.
 * @param Version       Version.
 * @param Index         Index. Must be increased for each instance of the header.
 * @param Flags         Flags.
 * @param DataSize      Total data size.
 * @param Crc32Data     CRC32 value covering all data.
 * @param Crc32Header   CRC32 value covering whole header (except this field).
 * @param FastParameter An array of 32 bit values for the "fast parameter" feature.
 * @param Reserved      An array of reserved 32 bit values for future usage.
 * @param BbtSize       Bad block table size.
 * @param Bbt[]         Bad block table.
 */
typedef struct
{
  uint32_t Magic;
  uint32_t Version;
  uint32_t Index;
  uint32_t Flags;
  uint32_t DataSize;
  uint32_t Crc32Data;
  uint32_t Crc32Header;
  uint32_t FastParameter[D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS];
  uint32_t Reserved[4];
  uint32_t BbtSize;
  uint32_t Bbt[];
} CSPSA_CORE_Header_t;

/**
 * Structure of a CSPSA header version 1.
 *
 * @param Magic         Magic number.
 * @param Version       Version.
 * @param Index         Index. Must be increased for each instance of the header.
 * @param Flags         Flags.
 * @param DataSize      Total data size.
 * @param Crc32Data     CRC32 value covering all data.
 * @param Crc32Header   CRC32 value covering whole header (except this field).
 * @param BbtSize       Bad block table size.
 * @param Bbt[]         Bad block table.
 */
typedef struct
{
  uint32_t Magic;
  uint32_t Version;
  uint32_t Index;
  uint32_t Flags;
  uint32_t DataSize;
  uint32_t Crc32Data;
  uint32_t Crc32Header;
  uint32_t BbtSize;
  uint32_t Bbt[];
} CSPSA_CORE_Header_v1_t;

// -----------------------------------------------------------------------------

// - CSPSA_DataItem_t: Structure of a data item.
typedef struct
{
  CSPSA_Key_t Key;    // - Identifier key.
  CSPSA_Size_t Size;  // - Size of parameter in Data_p. May be set to odd value,
                      //   but actual size of Data_p is always 32-bit aligned.
  uint8_t Data_p[];   // - Parameter data.
} CSPSA_DataItem_t;

// -----------------------------------------------------------------------------

// - CSPSA_ItemListNode_t: Structure of a item list node.
typedef struct CSPSA_ItemListNode_t
{
  CSPSA_DataItem_t* DataItem_p;         // - Data item.
  struct CSPSA_ItemListNode_t* Next_p;  // - Next item node.
} CSPSA_ItemListNode_t;

// -----------------------------------------------------------------------------

// - CSPSA_Image_t: Structure of a cache image.
typedef struct
{
  uint32_t Max;             // - Max amount of data that can be stored in this image.
  uint32_t Level;           // - Current amount of data in this image.
  CSPSA_DataItem_t* Data_p; // - Parameter data field.
} CSPSA_Image_t;

// -----------------------------------------------------------------------------

// - CSPSA_Cache_t: Structure for RAM cache buffer of CSPSA.
typedef struct
{
  uint32_t MaxNbrOfImages;          // - Maximum number of images.
  uint32_t NbrOfImages;             // - Current number of images being used.
  uint32_t LoadImageArrayDataLeft;  // - Note for F_CSPSA_ImageArray_LoadData()
  bool CacheUpdated;                // - True if cache has been updated since CSPSA was opened.
  CSPSA_Image_t* ImageArray_p;      // - Pointer to image array.
  CSPSA_ItemListNode_t* ItemList_p; // - Header of item list.
  CSPSA_DataItem_t* LastDataItem_p; // - Pointer to the last accessed item. Used for quick
                                    //   access.
  CSPSA_DataItem_t* NextDataItem_p; // - Pointer to the item after the item in LastDataItem_p.
                                    //   Used for quicker response to CSPSA_GetSizeOfNextValue()
                                    //   calls.
  uint32_t ImageArray_HeapAllocated;// - ImageArray_HeapAllocated: The amount of data that is allocated during
                                    //   loading of CSPSA data in F_CSPSA_ImageArray_LoadData(). It may be that the
                                    //   stored image is corrupted in a way that could lead to excessive heap allocations.
                                    //   Varable V_CSPSA_ImageArray_HeapAllocated assists in keeping track of how
                                    //   much is allocated during image loading.
  CSPSA_ItemListNode_t *FastParameterItemList_p; // - Pointer to fast parameters item list.
} CSPSA_Cache_t;

// -----------------------------------------------------------------------------

// - CSPSA_HeaderFlags_t: List header flags.
typedef enum
{
  T_CSPSA_HEADERFLAGS_DATA_AT_END   = (1 << 0),   // - Marker that determines if data is at
                                                  //   start or end of CSPSA.
  // - - -
  T_CSPSA_HEADERFLAGS_END_MARK                    // - Flag end marker for debug purpose.
} CSPSA_HeaderFlags_t;

// -----------------------------------------------------------------------------

// - CSPSA_SessionInfo_t: CSPSA session info structure.
typedef struct
{
  CSPSA_Config_t Config;    // - Configuration struct.
  CSPSA_Cache_t Cache;      // - Cache struct.
  CSPSA_CORE_Header_t* LastStorageImageHeader_p; // - Pointer to last used image header.
  bool SessionCreated;      // - True if session started with CSPSA_Create()
  void* PluginHandle_p;
} CSPSA_SessionInfo_t;

// -----------------------------------------------------------------------------

// - CSPSA_SessionInfoList_t: Session info list structure.
typedef struct CSPSA_SessionInfoList_t
{
  CSPSA_SessionInfo_t* SessionInfo_p;
  uint32_t Counter;
  struct CSPSA_SessionInfoList_t* Next_p;
} CSPSA_SessionInfoList_t;

typedef struct CSPSA_ConfigList_t
{
  CSPSA_Config_t Config;
  struct CSPSA_ConfigList_t* Next_p;
} CSPSA_ConfigList_t;

// =============================================================================
//  Global Function Declarations
// =============================================================================

/**
 * Opens a parameter storage area.
 *
 * @param [in]  Config_p  Configuration for this partition.
 * @param [out] Handle_p  A handle for this opened session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_W_INVALID_HEADER_FOUND If one of the CSPSA headers was not valid.
 * @retval T_CSPSA_RESULT_E_NO_VALID_IMAGE If a valid CSPSA image could not be found.
 */
CSPSA_Result_t CSPSA_CORE_Open(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
);

// -----------------------------------------------------------------------------

/**
 * Closes a parameter storage area. Cached data is stored to memory media before it is freed.
 *
 * @param [out] Handle  A handle for this opened session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_OUT_OF_SPACE If the image cannot be fit into the CSPSA area.
 * @retval T_CSPSA_RESULT_E_READ_ONLY If the CSPSA area is read-only.
 */
CSPSA_Result_t CSPSA_CORE_Close(CSPSA_Handle_t* const Handle_p);

// -----------------------------------------------------------------------------

/**
 * Gets the size of a value.
 *
 * @param [in]      Handle    A handle to a parameter storage area. A handle is retrieved with
 *                            a call to function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be read. Key value '0' (zero)
 *                            is reserved and cannot be used.
 * @param [out]     Size_p    Pointer to where the size of parameter value will be stored.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If parameter value for Key could no be found.
 */
CSPSA_Result_t CSPSA_CORE_GetSizeOfValue(
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
CSPSA_Result_t CSPSA_CORE_GetSizeOfFirstValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
);

// -----------------------------------------------------------------------------

/**
 * Gets the size and key of the parameter value with the next-higher key.
 *
 * @param [in]      Handle    A handle to a parameter storage area. A handle is retrieved with
 *                            a call to function CSPSA_Open().
 * @param [in,out]  Key_p     In: the key of an existing parameter value, out: the next-higher key.
 * @param [out]     Size_p    Pointer to where the size of the parameter value will be stored.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_END_OF_DATA If there is no parameter with a higher key.
 */
CSPSA_Result_t CSPSA_CORE_GetSizeOfNextValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
);

// -----------------------------------------------------------------------------

/**
 * Reads a parameter value from a parameter storage area.
 *
 * @param [in]      Handle    A handle to a parameter storage area. A handle is retrieved with
 *                            a call to function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be read. Key value '0' (zero)
 *                            is reserved and cannot be used.
 * @param [in]      Size      The maximum number of bytes to read into Data_p.
 * @param [out]     Data_p    A pointer where parameter data will be written.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_W_SIZE If *Size_p is different than actual parameter size.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If parameter value for Key could no be found.
 */
CSPSA_Result_t CSPSA_CORE_ReadValue(
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
 * @param [in]      Handle    A handle to a parameter storage area. A handle is retrieved with
 *                            a call to function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be written. Key value '0' (zero)
 *                            is reserved and cannot be used.
 * @param [in]      Size      The maximum number of bytes to read into Data_p.
 * @param [in]      Data_p    A pointer with parameter data.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_W_SIZE If *Size_p is different than actual parameter size.
 * @retval T_CSPSA_RESULT_W_NEW_KEY If parameter key did not exist before this function call.
 */
CSPSA_Result_t CSPSA_CORE_WriteValue(
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
 * @param [in]      Handle    A handle to a parameter storage area. A handle is retrieved with
 *                            a call to function CSPSA_Open().
 * @param [in]      Key       A key to the parameter to be written. Key value '0' (zero)
 *                            is reserved and cannot be used.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_INVALID_KEY If item could not be found.
 */
CSPSA_Result_t CSPSA_CORE_DeleteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key
);

// -----------------------------------------------------------------------------

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @param [out] Handle  A handle for this opened session.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 * @retval T_CSPSA_RESULT_E_OUT_OF_SPACE If there is not space enough to store entire parameter storage area data record.
 */
CSPSA_Result_t CSPSA_CORE_Flush(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Parameter storage area will be erased.
 *
 * @param [out] Handle  A handle for this opened session.
 *
 * @return The status of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_CORE_Format(const CSPSA_Handle_t Handle);

// -----------------------------------------------------------------------------

/**
 * Creates a parameter storage area if it could not be opened. Normally only used
 * by tools and test applications.
 *
 * @param [in]  Config_p  Configuration for this partition.
 * @param [out] Handle_p  A handle for this created session.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If success.
 */
CSPSA_Result_t CSPSA_CORE_Create(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
);

// -----------------------------------------------------------------------------

/**
 * Gets the number of registered parameter storage areas.
 *
 * @return The number of registered parameter areas.
 */
uint32_t CSPSA_CORE_GetParameterAreaListLength(void);

// -----------------------------------------------------------------------------

/**
 * Gets the registered parameter storage area with the given index.
 *
 * @return The Index'th parameter storage area, or NULL if there is no such area (Index out of bounds).
 */
CSPSA_Result_t CSPSA_CORE_GetParameterArea(uint32_t Index, CSPSA_Config_t* Config_p);


// =============================================================================
#endif // INCLUSION_GUARD_CSPSA_CORE_H
// =============================================================================
