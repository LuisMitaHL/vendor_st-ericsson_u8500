/* **************************************************************************
 *
 * cspsa_core.c
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
 * Core implementation of Crash Safe Parameter Storage Area (CSPSA). See file
 * "cspsa_ds.html".
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE: Sets the max size of cached item array.
//   Ought to be at least one OS page (4kB) big. During debug and test
//   one should set this parameter to 512.
#ifndef CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE
#define CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE                    (4*1024)
#endif

// - CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS: Sets the initial number of arrays for
//   the RAM cache. During debug and test one ought to set this parameter to 1, otherwise
//   set it to at least 128.
#ifndef CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS
#define CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS        (128)
#endif

// =============================================================================
//  Include Header Files
// =============================================================================

#ifndef CSPSA_ITP_CONFIG

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#else

#include "xstdio.h"
#include <string.h>
#include <stdint.h>
#include "OpenRTOS.h"
#include "task.h"
#include "cspsa_port.h"

#endif // CSPSA_ITP_CONFIG


#include "crc32.h"
#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_ll.h"
#include "cspsa_plugin.h"

// =============================================================================
//  Defines
// =============================================================================

// - CSPSA header defines.
#define D_CSPSA_CORE_HEADER_MAGIC                   0x00415350
#define D_CSPSA_CORE_HEADER_VERSION                 2

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_CORE_FREE: Wrapper to free a pointer.
#define M_CSPSA_CORE_FREE(Pointer) \
  do                          \
  {                           \
    free(Pointer);            \
    Pointer = NULL;           \
  } while (0)

// -----------------------------------------------------------------------------

// - M_CSPSA_CORE_ALIGN32: Aligns a value to 32 bits.
#define M_CSPSA_CORE_ALIGN32(s) (((s) + 3) & ~3)

// -----------------------------------------------------------------------------

// - M_CSPSA_CORE_ALIGN: Aligns a value i to m.
#define M_CSPSA_CORE_ALIGN(i,m) (((i) + (m) - 1) & ~((m) - 1))

// =============================================================================
//  Local Types
// =============================================================================

// =============================================================================
//  Local Constants
// =============================================================================

// =============================================================================
//  Local Variables
// =============================================================================

// - Normally there should be no global variables for CSPSA. Everything should
//   be stored in CSPSA_SessionInfo_t.

// - V_CSPSA_CORE_OpenedSessionsList_p: List over opened sessions.
static CSPSA_SessionInfoList_t* V_CSPSA_CORE_OpenedSessionsList_p;

// - List of all registered storage areas.
static CSPSA_ConfigList_t* V_CSPSA_CORE_RegisteredAreaList_p = NULL;

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static CSPSA_Result_t F_CSPSA_CORE_ImageArray_GetArrayIndex(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  uint32_t* const ArrayIndex_p
);
static CSPSA_Result_t F_CSPSA_CORE_ImageArray_LoadData(
  const CSPSA_Handle_t Handle,
  const uint8_t* const Data_p,
  const uint32_t DataSize
);
static CSPSA_Result_t F_CSPSA_CORE_ImageArray_Cleanup(
  const CSPSA_Handle_t Handle
);
static CSPSA_Result_t F_CSPSA_CORE_ImageArray_Flush(
  const CSPSA_Handle_t Handle,
  const uint32_t Index,
  const uint32_t Offset,
  const uint32_t Size,
  uint32_t* StorageImageOffset_p,
  CSPSA_Image_t* FirstStoreBuffer_p,
  CSPSA_Image_t* StoreBuffer_p,
  uint32_t* CrcValue_p,
  uint32_t* TotalDataSize_p
);
CSPSA_Result_t F_CSPSA_CORE_ImageArray_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
);
static void* F_CSPSA_CORE_ImageArray_HeapMalloc(const CSPSA_Handle_t Handle, const uint32_t Size);
// -----------------------------------------------------------------------------
static CSPSA_Result_t F_CSPSA_CORE_ItemList_Add(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
);
static CSPSA_Result_t F_CSPSA_CORE_ItemList_Cleanup(
  const CSPSA_Handle_t Handle
);
static CSPSA_Result_t F_CSPSA_CORE_ItemList_Flush(
  const CSPSA_Handle_t Handle,
  CSPSA_ItemListNode_t* ItemNode_p,
  uint32_t* StorageImageOffset_p,
  CSPSA_Image_t* FirstStoreBuffer_p,
  CSPSA_Image_t* StoreBuffer_p,
  uint32_t* CrcValue_p,
  uint32_t* TotalDataSize_p
);
static CSPSA_Result_t F_CSPSA_CORE_ItemList_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
);
static CSPSA_Result_t F_CSPSA_CORE_ItemList_Update(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
);
// -----------------------------------------------------------------------------
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Add(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
);
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Cleanup(
  const CSPSA_Handle_t Handle
);
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Flush(
  const CSPSA_Handle_t Handle
);
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_GetFromHeader(
  const CSPSA_Handle_t Handle
);
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
);
static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Update(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
);
// -----------------------------------------------------------------------------
static void* F_CSPSA_CORE_HeapMalloc(uint32_t Size);
// -----------------------------------------------------------------------------
static CSPSA_Result_t F_CSPSA_CORE_StorageImage_GetValidHeader(
  const CSPSA_Handle_t Handle,
  CSPSA_CORE_Header_t** const StorageHeader_pp
);
static CSPSA_Result_t F_CSPSA_CORE_StorageImage_LoadData(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
);
static CSPSA_Result_t F_CSPSA_CORE_StorageImage_VerifyHeader(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
);
static uint32_t F_CSPSA_CORE_StorageImage_CalcCrc32Header(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
);
// -----------------------------------------------------------------------------
static CSPSA_Result_t F_CSPSA_CORE_Cache_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp
);

static CSPSA_Result_t F_CSPSA_CORE_Cache_GetNextDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** NextDataItem_pp
);

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

/**
 * Finds a storage area with the given name in the list of present storage areas; returns NULL if not found
 */
static CSPSA_Config_t* FindArea(const char* Name_p)
{
  CSPSA_ConfigList_t* List_p;

  for (List_p = V_CSPSA_CORE_RegisteredAreaList_p; List_p != NULL; List_p = List_p->Next_p)
  {
    if (strcmp(List_p->Config.Name, Name_p) == 0)
    {
      return &List_p->Config;
    }
  }
  return NULL;
}


// - F_CSPSA_CORE_ImageArray_Cleanup: Frees all allocations in image array.

static CSPSA_Result_t F_CSPSA_CORE_ImageArray_Cleanup(
  const CSPSA_Handle_t Handle
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Image_t* ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
  uint32_t NbrOfImages = CSPSA_SessionInfo_p->Cache.NbrOfImages;
  uint32_t i;

  for (i = 0; i < NbrOfImages; i++)
  {
    M_CSPSA_CORE_FREE(ImageArray_p[i].Data_p);
  }
  M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->Cache.ImageArray_p);
  CSPSA_SessionInfo_p->Cache.NbrOfImages = 0;
  CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = 0;
  CSPSA_SessionInfo_p->Cache.LoadImageArrayDataLeft = 0;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ImageArray_Flush: Flushes image array to storage media.

static CSPSA_Result_t F_CSPSA_CORE_ImageArray_Flush(
  const CSPSA_Handle_t Handle,
  const uint32_t Index,
  const uint32_t Offset,
  const uint32_t Size,
  uint32_t* StorageImageOffset_p,
  CSPSA_Image_t* FirstStoreBuffer_p,
  CSPSA_Image_t* StoreBuffer_p,
  uint32_t* CrcValue_p,
  uint32_t* TotalDataSize_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_CORE_Header_t* StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;
  const bool DataAtEnd = ((StorageImageHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0);
  CSPSA_Image_t* ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
  uint32_t MaxImageSize = ImageArray_p[Index].Level;
  uint32_t ImageSize = Size;
  uint32_t ChunkSize;
  uint8_t* TempData_p = (uint8_t*) ImageArray_p[Index].Data_p + Offset;

  if ((MaxImageSize == 0) || (Offset + ImageSize > MaxImageSize))
  {
    Result = T_CSPSA_RESULT_UNDEFINED;
    goto ErrorExit;
  }

  // - Fill first store buffer.
  if (FirstStoreBuffer_p->Max != FirstStoreBuffer_p->Level)
  {
    ChunkSize = FirstStoreBuffer_p->Max - FirstStoreBuffer_p->Level;
    ChunkSize = (ChunkSize > ImageSize) ? ImageSize : ChunkSize;
    memcpy((uint8_t*)FirstStoreBuffer_p->Data_p + FirstStoreBuffer_p->Level, TempData_p, ChunkSize);
    *CrcValue_p = CRC32_Calc(*CrcValue_p, TempData_p, ChunkSize);
    TempData_p += ChunkSize;
    ImageSize -= ChunkSize;
    FirstStoreBuffer_p->Level += ChunkSize;
  }

  if (FirstStoreBuffer_p->Max == FirstStoreBuffer_p->Level)
  {
    while (ImageSize > 0)
    {
      // - First store buffer is full. Use the other for rest of data and store to memory media whenever full.
      while ( (StoreBuffer_p->Max != StoreBuffer_p->Level) && (ImageSize > 0) )
      {
        ChunkSize = StoreBuffer_p->Max - StoreBuffer_p->Level;
        ChunkSize = (ChunkSize > ImageSize) ? ImageSize : ChunkSize;
        memcpy((uint8_t*)StoreBuffer_p->Data_p + StoreBuffer_p->Level, TempData_p, ChunkSize);
        *CrcValue_p = CRC32_Calc(*CrcValue_p, TempData_p, ChunkSize);
        TempData_p += ChunkSize;
        ImageSize -= ChunkSize;
        StoreBuffer_p->Level += ChunkSize;
      }
      if (StoreBuffer_p->Max == StoreBuffer_p->Level)
      {
        // - Store to media.
        Result = CSPSA_LL_Write(Handle, *StorageImageOffset_p, StoreBuffer_p->Data_p, StoreBuffer_p->Level, DataAtEnd);
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
        *StorageImageOffset_p = (DataAtEnd) ? *StorageImageOffset_p - StoreBuffer_p->Level : *StorageImageOffset_p + StoreBuffer_p->Level;
        *TotalDataSize_p += StoreBuffer_p->Level;
        StoreBuffer_p->Level = 0;
      }
    }
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ImageArray_GetDataItem: Fetches image array data item.
CSPSA_Result_t F_CSPSA_CORE_ImageArray_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Image_t* ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
  CSPSA_DataItem_t* DataItem_p;
  CSPSA_DataItem_t* NextDataItem_p = NULL;
  CSPSA_Image_t* CurrentImage_p = NULL;
  uint32_t ImageArrayIndex;
  uint32_t DataLeft;
  uint32_t Size32Aligned;

  // - First get the index of the array where it ought to be found.
  Result = F_CSPSA_CORE_ImageArray_GetArrayIndex(Handle, Key, &ImageArrayIndex);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  CurrentImage_p = &ImageArray_p[ImageArrayIndex];
  DataLeft = CurrentImage_p->Level;
  DataItem_p = CurrentImage_p->Data_p;
  // - First approximation for NextDataItem: at the start of the next array
  //   (might be changed later if we find a higher key in this array)
  if (ImageArrayIndex < CSPSA_SessionInfo_p->Cache.NbrOfImages - 1)
  {
    CSPSA_Image_t* NextImage_p = &ImageArray_p[ImageArrayIndex + 1];
    if (NextImage_p->Level > 0)
    {
      NextDataItem_p = NextImage_p->Data_p;
    }
  }
  // - Start searching the image.
  while (DataLeft > 0)
  {
    if (Key == DataItem_p->Key)
    {
      // - We found the item.
      Result = T_CSPSA_RESULT_OK;
      // - Cache the next item.
      Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
      DataLeft -= Size32Aligned;
      if (DataLeft > 0)
      {
        NextDataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*)DataItem_p + Size32Aligned);
      }
      goto ErrorExit;
    }
    if (Key > DataItem_p->Key)
    {
      Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
      DataLeft -= Size32Aligned;
      if (DataLeft > 0)
      {
        DataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*)DataItem_p + Size32Aligned);
      }
    }
    else
    {
      // - Items are sorted on increasing key value. No need to look further.
      NextDataItem_p = DataItem_p;
      break;
    }
  }

  if (Key == DataItem_p->Key)
  {
    Result = T_CSPSA_RESULT_OK;
  }
  else
  {
    Result = T_CSPSA_RESULT_E_INVALID_KEY;
  }

ErrorExit:

  if (Result == T_CSPSA_RESULT_OK)
  {
    *DataItem_pp = DataItem_p;
  }
  *NextDataItem_pp = NextDataItem_p;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ImageArray_GetArrayIndex: Finds the image array index where the Key
//   _may_ be found.

static CSPSA_Result_t F_CSPSA_CORE_ImageArray_GetArrayIndex(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  uint32_t* const ArrayIndex_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_DataItem_t* DataItem_p;
  uint32_t Hi, Lo, Mid;

  if ((ArrayIndex_p == NULL) || (CSPSA_SessionInfo_p->Cache.NbrOfImages == 0))
  {
    goto ErrorExit;
  }

  if
  (
    (CSPSA_SessionInfo_p->Cache.ImageArray_p == NULL)
    ||
    (
      // - If there is not data in cache, return error. Might happend
      //   if all data is deleted.
      (CSPSA_SessionInfo_p->Cache.NbrOfImages == 1)
      &&
      (CSPSA_SessionInfo_p->Cache.ImageArray_p[0].Level == 0)
    )
  )
  {
    // - There is no image array.
    goto ErrorExit;
  }

  Lo = 0;
  Hi = CSPSA_SessionInfo_p->Cache.NbrOfImages - 1;

  // - Find the array that could contain the item using bisection method.
  while (1)
  {
    Mid = (Lo + Hi) / 2;
    DataItem_p = CSPSA_SessionInfo_p->Cache.ImageArray_p[Mid].Data_p;
    if (Key < DataItem_p->Key)
    {
      if (Hi == Mid)
      {
        break;
      }
      Hi = Mid;
    }
    else
    if (Key >= DataItem_p->Key)
    {
      if (Lo == Mid)
      {
        break;
      }
      Lo = Mid;
    }
  }

  // - If there's a gap between Hi and Lo we need to search downwards from Hi.
  Mid = Hi;
  while (Mid >= Lo)
  {
    DataItem_p = CSPSA_SessionInfo_p->Cache.ImageArray_p[Mid].Data_p;
    if ((Key >= DataItem_p->Key) || (Mid == 0))
    {
      break;
    }
    Mid--;
  }

  *ArrayIndex_p = Mid;
  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ItemList_Add: Adds an item to the item list.

static CSPSA_Result_t F_CSPSA_CORE_ItemList_Add(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
  CSPSA_ItemListNode_t* LastItemNode_p = NULL;
  CSPSA_ItemListNode_t* NewItemNode_p = NULL;
  CSPSA_DataItem_t* NewDataItem_p = NULL;

  // - Search until there are no items left. List is sorted on increasing Key value.
  while ( (ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (ItemNode_p->DataItem_p->Key <= Key))
  {
    LastItemNode_p = ItemNode_p;
    ItemNode_p = ItemNode_p->Next_p;
  }

  if (Result >= T_CSPSA_RESULT_ERRORS)
  {
    // - Add new data item.
    CSPSA_DataItem_t* NewDataItem_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*NewDataItem_p) + Size);
    NewDataItem_p->Key = Key;
    NewDataItem_p->Size = Size;
    if (Size > 0)
    {
      memcpy(NewDataItem_p->Data_p, Data_p, Size);
    }
    // - Add new item node.
    NewItemNode_p = (CSPSA_ItemListNode_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*NewItemNode_p));
    NewItemNode_p->DataItem_p = NewDataItem_p;

    // - Store item node and data item into list.
    if (LastItemNode_p == NULL)
    {
      // - This is the first item in the list.
      NewItemNode_p->Next_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
      CSPSA_SessionInfo_p->Cache.ItemList_p = NewItemNode_p;
    }
    else
    {
      // - Insert the item into the list.
      NewItemNode_p->Next_p = LastItemNode_p->Next_p;
      LastItemNode_p->Next_p = NewItemNode_p;
    }
    Result = T_CSPSA_RESULT_OK;
  }

  if (Result >= T_CSPSA_RESULT_ERRORS)
  {
    M_CSPSA_CORE_FREE(NewItemNode_p);
    M_CSPSA_CORE_FREE(NewDataItem_p);
  }

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ItemList_Cleanup: Frees all allocations in item list.

static CSPSA_Result_t F_CSPSA_CORE_ItemList_Cleanup(
  const CSPSA_Handle_t Handle
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
  CSPSA_ItemListNode_t* NextItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;

  while (ItemNode_p != NULL)
  {
    NextItemNode_p = ItemNode_p->Next_p;
    M_CSPSA_CORE_FREE(ItemNode_p->DataItem_p);
    M_CSPSA_CORE_FREE(ItemNode_p);
    ItemNode_p = NextItemNode_p;
  }
  CSPSA_SessionInfo_p->Cache.ItemList_p = NULL;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ItemList_Flush: Flushes item list to storage media.

static CSPSA_Result_t F_CSPSA_CORE_ItemList_Flush(
  const CSPSA_Handle_t Handle,
  CSPSA_ItemListNode_t* ItemNode_p,
  uint32_t* StorageImageOffset_p,
  CSPSA_Image_t* FirstStoreBuffer_p,
  CSPSA_Image_t* StoreBuffer_p,
  uint32_t* CrcValue_p,
  uint32_t* TotalDataSize_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  uint32_t Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*ItemNode_p->DataItem_p) + ItemNode_p->DataItem_p->Size);
  uint32_t ImageSize = Size32Aligned;
  uint32_t ChunkSize;
  uint8_t* TempData_p = (uint8_t*) ItemNode_p->DataItem_p;
  CSPSA_CORE_Header_t* StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;
  const bool DataAtEnd = ((StorageImageHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0);

  // - As first sector/block contains the header it must be stored last.

  if (FirstStoreBuffer_p->Max != FirstStoreBuffer_p->Level)
  {
    ChunkSize = FirstStoreBuffer_p->Max - FirstStoreBuffer_p->Level;
    ChunkSize = (ChunkSize > ImageSize) ? ImageSize : ChunkSize;
    memcpy((uint8_t*)FirstStoreBuffer_p->Data_p + FirstStoreBuffer_p->Level, TempData_p, ChunkSize);
    *CrcValue_p = CRC32_Calc(*CrcValue_p, TempData_p, ChunkSize);
    TempData_p += ChunkSize;
    ImageSize -= ChunkSize;
    FirstStoreBuffer_p->Level += ChunkSize;
  }

  if (FirstStoreBuffer_p->Max == FirstStoreBuffer_p->Level)
  {
    while (ImageSize > 0)
    {
      // - First store buffer is full. Use the other for rest of data and store to memory media whenever full.
      while ( (StoreBuffer_p->Max != StoreBuffer_p->Level) && (ImageSize > 0) )
      {
        ChunkSize = StoreBuffer_p->Max - StoreBuffer_p->Level;
        ChunkSize = (ChunkSize > ImageSize) ? ImageSize : ChunkSize;
        memcpy((uint8_t*)StoreBuffer_p->Data_p + StoreBuffer_p->Level, TempData_p, ChunkSize);
        *CrcValue_p = CRC32_Calc(*CrcValue_p, TempData_p, ChunkSize);
        TempData_p += ChunkSize;
        ImageSize -= ChunkSize;
        StoreBuffer_p->Level += ChunkSize;
      }
      if (StoreBuffer_p->Max == StoreBuffer_p->Level)
      {
        // - Store to media.
        Result = CSPSA_LL_Write(Handle, *StorageImageOffset_p, StoreBuffer_p->Data_p, StoreBuffer_p->Level, DataAtEnd);
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
        *StorageImageOffset_p = (DataAtEnd) ? *StorageImageOffset_p - StoreBuffer_p->Level : *StorageImageOffset_p + StoreBuffer_p->Level;
        *TotalDataSize_p += StoreBuffer_p->Level;
        StoreBuffer_p->Level = 0;
      }
    }
  }

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ItemList_GetDataItem: Fetches a data item from the item list.

static CSPSA_Result_t F_CSPSA_CORE_ItemList_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
  CSPSA_DataItem_t* NextDataItem_p = NULL;

  // - Look in item list for the key.
  while (ItemNode_p != NULL)
  {
    if ( (ItemNode_p->DataItem_p!= NULL) && (Key == ItemNode_p->DataItem_p->Key) )
    {
      // - We found the Key.
      *DataItem_pp = ItemNode_p->DataItem_p;
      if (ItemNode_p->Next_p != NULL)
      {
        NextDataItem_p = ItemNode_p->Next_p->DataItem_p;
      }
      Result = T_CSPSA_RESULT_OK;
      goto ErrorExit;
    }
    if (Key < ItemNode_p->DataItem_p->Key)
    {
      //  - We did not find the key; set NextDataItem to the next higher key.
      NextDataItem_p = ItemNode_p->DataItem_p;
      // - List is sorted on increasing Key value. No need to look further in
      //   the item list.
      break;
    }
    ItemNode_p = ItemNode_p->Next_p;
  }

ErrorExit:

  *NextDataItem_pp = NextDataItem_p;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ItemList_Update: Updates the item list with an item.

static CSPSA_Result_t F_CSPSA_CORE_ItemList_Update(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;

  // - Search until there are no items left. List is sorted on increasing Key value.
  while ( (ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (ItemNode_p->DataItem_p->Key <= Key))
  {
    if (ItemNode_p->DataItem_p->Key == Key)
    {
      // - We found a matching item. If size is the same, then we just update the data.
      if (ItemNode_p->DataItem_p->Size == Size)
      {
        memcpy(ItemNode_p->DataItem_p->Data_p, Data_p, Size);
        Result = T_CSPSA_RESULT_OK;
        break;
      }
      else
      {
        // - Size differed. Create new data item and put into list.
        CSPSA_DataItem_t* NewDataItem_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_DataItem_t) + Size);
        NewDataItem_p->Key = Key;
        NewDataItem_p->Size = Size;
        memcpy(NewDataItem_p->Data_p, Data_p, Size);
        M_CSPSA_CORE_FREE(ItemNode_p->DataItem_p);
        ItemNode_p->DataItem_p = NewDataItem_p;
        Result = T_CSPSA_RESULT_W_SIZE;
        break;
      }
    }
    ItemNode_p = ItemNode_p->Next_p;
  }

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_Add: Adds a fast parameter item to the fast parameter item list.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Add(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;
  CSPSA_ItemListNode_t* LastItemNode_p = NULL;
  CSPSA_ItemListNode_t* NewItemNode_p = NULL;
  CSPSA_DataItem_t* NewDataItem_p = NULL;
  CSPSA_Key_t FastParameterKey;

  // - Check that the key indicates a fast parameter
  if (Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY) goto ErrorExit;

  // - Check that the key is in range for fast parameters
  FastParameterKey = Key - D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY;
  if (FastParameterKey >= D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS) goto ErrorExit;

  // - Search until there are no items left. List is sorted on increasing Key value.
  while ( (ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (ItemNode_p->DataItem_p->Key <= Key))
  {
    LastItemNode_p = ItemNode_p;
    ItemNode_p = ItemNode_p->Next_p;
  }

  // - Add new data item.
  NewDataItem_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*NewDataItem_p) + Size);
  NewDataItem_p->Key = Key;
  NewDataItem_p->Size = Size;
  if (Size > 0)
  {
    memcpy(NewDataItem_p->Data_p, Data_p, Size);
  }
  // - Add new item node.
  NewItemNode_p = (CSPSA_ItemListNode_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*NewItemNode_p));
  NewItemNode_p->DataItem_p = NewDataItem_p;

  // - Store item node and data item into list.
  if (LastItemNode_p == NULL)
  {
    // - This is the first item in the list.
    NewItemNode_p->Next_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;
    CSPSA_SessionInfo_p->Cache.FastParameterItemList_p = NewItemNode_p;
  }
  else
  {
    // - Insert the item into the list.
    NewItemNode_p->Next_p = LastItemNode_p->Next_p;
    LastItemNode_p->Next_p = NewItemNode_p;
  }
  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  if (Result >= T_CSPSA_RESULT_ERRORS)
  {
    M_CSPSA_CORE_FREE(NewItemNode_p);
    M_CSPSA_CORE_FREE(NewDataItem_p);
  }

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_Cleanup: Frees all allocations in fast parameter item list.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Cleanup(
  const CSPSA_Handle_t Handle
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;
  CSPSA_ItemListNode_t* NextItemNode_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;

  while (ItemNode_p != NULL)
  {
    NextItemNode_p = ItemNode_p->Next_p;
    M_CSPSA_CORE_FREE(ItemNode_p->DataItem_p);
    M_CSPSA_CORE_FREE(ItemNode_p);
    ItemNode_p = NextItemNode_p;
  }
  CSPSA_SessionInfo_p->Cache.FastParameterItemList_p = NULL;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_Flush: Flushes fast parameter item list to header.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Flush(
  const CSPSA_Handle_t Handle
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_CORE_Header_t* StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;
  CSPSA_Size_t FastParameterDataSize;
  CSPSA_DataItem_t* NextDataItem_p;
  CSPSA_DataItem_t* DataItem_p;
  CSPSA_Key_t FastParameterKey;
  uint32_t *FastParameter_p;
  CSPSA_Key_t Key;

  FastParameter_p = &(StorageImageHeader_p->FastParameter[0]);
  FastParameterDataSize = sizeof(*FastParameter_p);

  // - Copy data of all fast parameter data item into header
  for (FastParameterKey = 0; FastParameterKey < D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS; FastParameterKey++)
  {
    // - Locate fast parameter in header
    FastParameter_p = &(StorageImageHeader_p->FastParameter[FastParameterKey]);

    // - Compute fast parameter key
    Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + FastParameterKey;

    // - Retrieve fast parameter data item
    Result = F_CSPSA_CORE_FastParameter_GetDataItem(Handle, Key, &DataItem_p, &NextDataItem_p);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    // - Make sure that fast parameter data size is maintained
    if (DataItem_p->Size != FastParameterDataSize)
    {
      Result = T_CSPSA_RESULT_E_INTERNAL_ERROR;
      goto ErrorExit;
    }

    // - Copy fast parameter data into header
    memcpy(FastParameter_p, DataItem_p->Data_p, FastParameterDataSize);
  }

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_GetFromHeader: Fetches the fast parameters from header.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_GetFromHeader(
  const CSPSA_Handle_t Handle
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_CORE_Header_t* StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;
  CSPSA_Size_t FastParameterDataSize;
  CSPSA_Key_t FastParameterKey;
  CSPSA_Data_t *Data_p;
  CSPSA_Key_t Key;

  // - Compute size of fast parameter item array
  FastParameterDataSize = sizeof(StorageImageHeader_p->FastParameter[0]);

  // - Allocate fast parameter item list
  CSPSA_SessionInfo_p->Cache.FastParameterItemList_p = NULL;

  // - Copy fast parameters from header to fake fast parameter data items in their array
  for (FastParameterKey = 0; FastParameterKey < D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS; FastParameterKey++)
  {
    Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + FastParameterKey;
    Data_p = (CSPSA_Data_t *) &StorageImageHeader_p->FastParameter[FastParameterKey];

    // - Add the fast parameter data stored in the header
    Result = F_CSPSA_CORE_FastParameter_Add(
        Handle,
        Key,
        FastParameterDataSize,
        Data_p
        );
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
  }

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_GetDataItem: Fetches a fast parameter data item from the fast parameter item list.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp,
  CSPSA_DataItem_t** NextDataItem_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;
  CSPSA_DataItem_t* NextDataItem_p = NULL;

  // - Look in item list for the key.
  while (ItemNode_p != NULL)
  {
    if ( (ItemNode_p->DataItem_p!= NULL) && (Key == ItemNode_p->DataItem_p->Key) )
    {
      // - We found the Key.
      *DataItem_pp = ItemNode_p->DataItem_p;
      if (ItemNode_p->Next_p != NULL)
      {
        NextDataItem_p = ItemNode_p->Next_p->DataItem_p;
      }
      Result = T_CSPSA_RESULT_OK;
      goto ErrorExit;
    }
    if (Key < ItemNode_p->DataItem_p->Key)
    {
      //  - We did not find the key; set NextDataItem to the next higher key.
      NextDataItem_p = ItemNode_p->DataItem_p;
      // - List is sorted on increasing Key value. No need to look further in
      //   the item list.
      break;
    }
    ItemNode_p = ItemNode_p->Next_p;
  }

ErrorExit:

  *NextDataItem_pp = NextDataItem_p;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_FastParameter_Update: Updates the fast parameter item list with a fast parameter item.

static CSPSA_Result_t F_CSPSA_CORE_FastParameter_Update(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_ItemListNode_t* ItemNode_p = CSPSA_SessionInfo_p->Cache.FastParameterItemList_p;
  CSPSA_Key_t FastParameterKey;

  // - Check that the key indicates a fast parameter
  if (Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY) goto ErrorExit;

  // - Check that the key is in range for fast parameters
  FastParameterKey = Key - D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY;
  if (FastParameterKey >= D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS) goto ErrorExit;

  // - Search until there are no items left. List is sorted on increasing Key value.
  while ( (ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (ItemNode_p->DataItem_p->Key <= Key))
  {
    if (ItemNode_p->DataItem_p->Key == Key)
    {
      // - We found a matching item. If size is the same, then we just update the data.
      if (ItemNode_p->DataItem_p->Size == Size)
      {
        memcpy(ItemNode_p->DataItem_p->Data_p, Data_p, Size);
        Result = T_CSPSA_RESULT_OK;
        break;
      }
      else
      {
        // - Fast parameters are not allowed to change size
        Result = T_CSPSA_RESULT_E_INVALID_SIZE;
        goto ErrorExit;
      }
    }
    ItemNode_p = ItemNode_p->Next_p;
  }

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_HeapMalloc: Wrapper for malloc() which never fails.

static void* F_CSPSA_CORE_HeapMalloc(uint32_t Size)
{
  uint8_t* Result;

  // - Make sure Size is 32-bit aligned because of padding of the data field in
  //   a parameter value and don't assume the compiler does this anyway.
  Size = (Size + 3) & ~3;
  Result = (uint8_t*) malloc(Size);

  // - This line makes sure all data is initialized.
  memset((uint8_t*) Result + Size - 4, 0, 4);

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_StorageImage_CalcCrc32Header: Calculates a CRC32 value of a CSPSA header.

static uint32_t F_CSPSA_CORE_StorageImage_CalcCrc32Header(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
)
{
  uint32_t CrcValue = D_CSPSA_CORE_HEADER_MAGIC;

  M_CSPSA_PARAMETER_NOT_USED(Handle);

  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Magic, sizeof(StorageHeader_p->Magic));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Version, sizeof(StorageHeader_p->Version));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Index, sizeof(StorageHeader_p->Index));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Flags, sizeof(StorageHeader_p->Flags));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->DataSize, sizeof(StorageHeader_p->DataSize));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Crc32Data, sizeof(StorageHeader_p->Crc32Data));
  // - Skip Crc32Header.
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->FastParameter, sizeof(StorageHeader_p->FastParameter));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Reserved, sizeof(StorageHeader_p->Reserved));
  CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->BbtSize, sizeof(StorageHeader_p->BbtSize));
  if (StorageHeader_p->BbtSize > 0)
  {
    CrcValue = CRC32_Calc(CrcValue, (uint8_t*) &StorageHeader_p->Bbt, StorageHeader_p->BbtSize);
  }

  return CrcValue;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_StorageImage_GetValidHeader: Gets a valid header from storage media.

static CSPSA_Result_t F_CSPSA_CORE_StorageImage_GetValidHeader(
  const CSPSA_Handle_t Handle,
  CSPSA_CORE_Header_t** const StorageHeader_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_Result_t FirstHeaderResult = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_Result_t SecondHeaderResult = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  uint32_t BlockSize;
  CSPSA_CORE_Header_t* FirstHeader_p = NULL;
  CSPSA_CORE_Header_t* SecondHeader_p = NULL;
  uint32_t Offset;
  bool ReadBackwards;

  BlockSize = CSPSA_SessionInfo_p->Config.BlockSize;
  FirstHeader_p = (CSPSA_CORE_Header_t*) F_CSPSA_CORE_HeapMalloc(BlockSize);
  SecondHeader_p = (CSPSA_CORE_Header_t*) F_CSPSA_CORE_HeapMalloc(BlockSize);

  // - Verify header at start and end of CSPSA partition. Return the valid header
  //   image with the highest index. If only one valid header can be found,
  //   replace the invalid with a valid with incremented index and return
  //   a warning.

  // - Read the first image header.
  Offset = 0;
  ReadBackwards = false;
  Result = CSPSA_LL_Read(Handle, Offset, FirstHeader_p, BlockSize, ReadBackwards);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  FirstHeaderResult = F_CSPSA_CORE_StorageImage_VerifyHeader(Handle, FirstHeader_p);

  // - Find the Second image header.
  Offset = CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize;
  ReadBackwards = true;
  Result = CSPSA_LL_Read(Handle, Offset, SecondHeader_p, BlockSize, ReadBackwards);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  SecondHeaderResult = F_CSPSA_CORE_StorageImage_VerifyHeader(Handle, SecondHeader_p);

  if (FirstHeaderResult == T_CSPSA_RESULT_OK)
  {
    if (SecondHeaderResult == T_CSPSA_RESULT_OK)
    {
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
      CSPSA_CORE_Header_t* StorageHeader_p = (FirstHeader_p->Index > SecondHeader_p->Index) ? FirstHeader_p : SecondHeader_p;
      // - Check if CRC32 for data is the same. If not, the header with highest
      //   index must replace the other.
      if ( (FirstHeader_p->Flags == SecondHeader_p->Flags) && (FirstHeader_p->Crc32Data == SecondHeader_p->Crc32Data) )
      {
        // - This is the normal case. Last close down was finished appropriatly.
        *StorageHeader_pp = StorageHeader_p;
        Result = T_CSPSA_RESULT_OK;
      }
      else
      {
        // - This ought to be a _very_ rare case. Both headers are ok, but they
        //   represent different data areas. The header with the lowest index
        //   must replace the other as soon as possible. It is possible the last write was
        //   cut before it was entirely finished and therefore could have a retention
        //   problem. Make sure the CSPSA cache is written on the next shut down. One could argue
        //   that we should immediatly copy the old header instead as it does not suffer
        //   from potential retention problem. However, it might be that the write was entirly
        //   done on the new header but the erase was just started on the old header block when
        //   power was cut. So it might be this old header is not valid the next time we read from it.
        StorageHeader_p = (FirstHeader_p->Index > SecondHeader_p->Index) ? SecondHeader_p : FirstHeader_p;
        Offset = (StorageHeader_p == FirstHeader_p) ? CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize : 0;
        Result = CSPSA_LL_Write(Handle, Offset, StorageHeader_p, BlockSize, (StorageHeader_p == FirstHeader_p));
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
        *StorageHeader_pp = StorageHeader_p;
        CSPSA_SessionInfo_p->Cache.CacheUpdated = true;
      }
#else // CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
      // - There are two valid headers meaning that the last flush was interrupted. Use parameter
      //   area with the lowest index, and erase the newest header.
        CSPSA_CORE_Header_t* StorageHeader_p = (FirstHeader_p->Index > SecondHeader_p->Index) ? SecondHeader_p : FirstHeader_p;
        Offset = (StorageHeader_p == FirstHeader_p) ? CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize : 0;
        Result = CSPSA_LL_Erase(Handle, Offset, (StorageHeader_p == FirstHeader_p));
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
        *StorageHeader_pp = StorageHeader_p;
        CSPSA_SessionInfo_p->Cache.CacheUpdated = true;
#endif // CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    }
    else
    {
      // - Second header was broken.

#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
      // - If data is at end of area we need to read (broken) header + data from this side before
      //   writing back fixed header + data.
      if ((FirstHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0)
      {
        Result = CSPSA_LL_Read(Handle, Offset, SecondHeader_p, BlockSize, true);
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      }
      memcpy(SecondHeader_p, FirstHeader_p, sizeof(*FirstHeader_p) + FirstHeader_p->BbtSize);
      Offset = CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize;
      SecondHeader_p->Index++;
      SecondHeader_p->Crc32Header = F_CSPSA_CORE_StorageImage_CalcCrc32Header(Handle, SecondHeader_p);
      Result = CSPSA_LL_Write(Handle, Offset, SecondHeader_p, BlockSize, true);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      Result = T_CSPSA_RESULT_W_INVALID_HEADER_FOUND;
      *StorageHeader_pp = SecondHeader_p;
#else
      *StorageHeader_pp = FirstHeader_p;
#endif
    }
  }
  else
  {
    if (SecondHeaderResult == T_CSPSA_RESULT_OK)
    {
#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
      // - First header was broken.

      // - If data is at end of area we need to read (broken) header + data from this side before
      //   writing back fixed header + data.
      if ((SecondHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0)
      {
        Result = CSPSA_LL_Read(Handle, Offset, FirstHeader_p, BlockSize, true);
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      }
      memcpy(FirstHeader_p, SecondHeader_p, sizeof(*SecondHeader_p) + SecondHeader_p->BbtSize);
      Offset = 0;
      FirstHeader_p->Index++;
      FirstHeader_p->Crc32Header = F_CSPSA_CORE_StorageImage_CalcCrc32Header(Handle, FirstHeader_p);
      Result = CSPSA_LL_Write(Handle, Offset, FirstHeader_p, BlockSize, false);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      Result = T_CSPSA_RESULT_W_INVALID_HEADER_FOUND;
      *StorageHeader_pp = FirstHeader_p;
#else
      *StorageHeader_pp = SecondHeader_p;
#endif
    }
    else
    {
      // - No header was valid.
      Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
      *StorageHeader_pp = NULL;
    }
  }

ErrorExit:

  if (*StorageHeader_pp != FirstHeader_p)
  {
    M_CSPSA_CORE_FREE(FirstHeader_p);
  }
  if (*StorageHeader_pp != SecondHeader_p)
  {
    M_CSPSA_CORE_FREE(SecondHeader_p);
  }

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_StorageImage_LoadData: Loads data from storage media to CSPSA cache.

static CSPSA_Result_t F_CSPSA_CORE_StorageImage_LoadData(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  const CSPSA_SessionInfo_t* const CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  uint32_t SectorSize;
  uint32_t BlockSize;
  uint8_t* ReadBuffer_p = NULL;
  uint32_t ReadBufferSize;
  uint32_t Offset;
  uint32_t CrcValue = D_CSPSA_CORE_HEADER_MAGIC;
  const uint32_t HeaderSize = sizeof(CSPSA_CORE_Header_t) + StorageHeader_p->BbtSize;
  uint32_t DataLeft = StorageHeader_p->DataSize;
  uint32_t ChunkSize;
  const bool DataAtEnd = ((StorageHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0);
  const bool ReadBackwards = DataAtEnd;
  const int32_t ReadDirection = DataAtEnd ? -1 : 1;

  SectorSize = CSPSA_SessionInfo_p->Config.SectorSize;
  BlockSize = CSPSA_SessionInfo_p->Config.BlockSize;

  // - Load a stored image to RAM. Image is stored either sequentially upwards from start or
  //   sequentially downwards from end, block or sector wise.


  // - When interfacing NAND flashes one needs to read in erase block chunks. If data is
  //   much smaller than an erase block, limit the read size to what is actually required.
  ReadBufferSize = HeaderSize + StorageHeader_p->DataSize;
  if (ReadBufferSize > BlockSize)
  {
    ReadBufferSize = BlockSize;
  }
  // - Align read buffer size to sector/page size.
  ReadBufferSize = M_CSPSA_CORE_ALIGN(ReadBufferSize, SectorSize);
  // - Allocate read buffer.
  ReadBuffer_p = (uint8_t*) F_CSPSA_CORE_HeapMalloc(ReadBufferSize);

  // - Determine read offset depending on if data is at front or end of CSPSA partition.
  Offset = 0;
  if (DataAtEnd)
  {
    Offset = CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize;
  }

  // - Read first piece of data.
  Result = CSPSA_LL_Read(Handle, Offset, ReadBuffer_p, ReadBufferSize, ReadBackwards);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  // - The first piece of data contains header and data. Start loading data into internal
  //   cache and calculate CRC32 over data.
  ChunkSize = ReadBufferSize - HeaderSize;
  if (ChunkSize > DataLeft)
  {
    ChunkSize = DataLeft;
  }
  CrcValue = CRC32_Calc(CrcValue, ReadBuffer_p + HeaderSize, ChunkSize);
  Result = F_CSPSA_CORE_ImageArray_LoadData(Handle, ReadBuffer_p + HeaderSize, ChunkSize);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
  DataLeft -= ChunkSize;
  Offset = Offset + BlockSize * ReadDirection;

  // - Read rest of data (if any) and put in both CRC calculator and into internal cache
  //   structure.
  while (DataLeft > 0)
  {
    Result = CSPSA_LL_Read(Handle, Offset, ReadBuffer_p, ReadBufferSize, ReadBackwards);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    ChunkSize = (DataLeft > ReadBufferSize) ? ReadBufferSize : DataLeft;
    CrcValue = CRC32_Calc(CrcValue, ReadBuffer_p, ChunkSize);
    Result = F_CSPSA_CORE_ImageArray_LoadData(Handle, ReadBuffer_p, ChunkSize);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
    DataLeft -= ChunkSize;
    Offset = Offset + BlockSize * ReadDirection;
  }

  // - Check CRC32!
  if (CrcValue != StorageHeader_p->Crc32Data)
  {
    Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
    goto ErrorExit;
  }

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  M_CSPSA_CORE_FREE(ReadBuffer_p);

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_StorageImage_VerifyHeader: Verify the integrity of a CSPSA header.

static CSPSA_Result_t F_CSPSA_CORE_StorageImage_VerifyHeader(
  const CSPSA_Handle_t Handle,
  const CSPSA_CORE_Header_t* const StorageHeader_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  uint32_t MaxHeaderSize;
  uint32_t SectorSize = CSPSA_SessionInfo_p->Config.SectorSize;

  Result = T_CSPSA_RESULT_E_BAD_PARAMETER;

  MaxHeaderSize = CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize;

  // - uint32_t Magic;
  if (StorageHeader_p->Magic != D_CSPSA_CORE_HEADER_MAGIC) goto ErrorExit;

  // - uint32_t Version;
  if (StorageHeader_p->Version != D_CSPSA_CORE_HEADER_VERSION) goto ErrorExit;

  // - uint32_t Index;
  // - No valid test.

  // - uint32_t Flags;
  if (StorageHeader_p->Flags >= T_CSPSA_HEADERFLAGS_END_MARK) goto ErrorExit;

  // - uint32_t DataSize;
  // - One could figure out a more precise maximum data size, but this is just a sanity check.
  if (StorageHeader_p->DataSize > CSPSA_SessionInfo_p->Config.SizeInBytes) goto ErrorExit;

  // - uint32_t Crc32Data;
  // - Not checked here. Check when loading data.

  // - uint32_t BbtSize;
  if
  (
    (CSPSA_SessionInfo_p->Config.MemoryType == T_CSPSA_MEMORY_TYPE_DEV_MTD)
    ||
    (CSPSA_SessionInfo_p->Config.MemoryType == T_CSPSA_MEMORY_TYPE_NAND_RAW)
    ||
    (CSPSA_SessionInfo_p->Config.MemoryType == T_CSPSA_MEMORY_TYPE_NAND_MANAGED)
  )
  {
    if (StorageHeader_p->BbtSize == 0) goto ErrorExit;
    if (StorageHeader_p->BbtSize + sizeof(CSPSA_CORE_Header_t) > MaxHeaderSize) goto ErrorExit;
  }
  else
  {
    if (StorageHeader_p->BbtSize != 0) goto ErrorExit;
  }

  // - uint32_t Bbt[];
  // - Not checked here.

  // - uint32_t Crc32Header;
  // - This is the important check in this function and must be done last.
  //   Calculate a CRC32 value over the whole header, except for this header item.
  if (StorageHeader_p->Crc32Header != F_CSPSA_CORE_StorageImage_CalcCrc32Header(Handle, StorageHeader_p)) goto ErrorExit;

  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ImageArray_HeapMalloc: Wrapper around F_CSPSA_CORE_HeapMalloc() to avoid
//   excessive memory allocation if data is corrupted.
static void* F_CSPSA_CORE_ImageArray_HeapMalloc(const CSPSA_Handle_t Handle, const uint32_t Size)
{
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  uint8_t* Result;
  uint32_t MaxAllocation =
    (CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) ?
    CSPSA_SessionInfo_p->Config.SizeInBytes :
    CSPSA_SessionInfo_p->Config.SizeInBytes / 2;

  if (MaxAllocation < CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE)
  {
    MaxAllocation = CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
  }

  // - There is something wrong with the stored image if the total amount of data
  //   allocated on heap is larger than MaxAllocation.
  if (CSPSA_SessionInfo_p->Cache.ImageArray_HeapAllocated + Size > MaxAllocation)
  {
    return NULL;
  }

  CSPSA_SessionInfo_p->Cache.ImageArray_HeapAllocated += Size;

  Result = F_CSPSA_CORE_HeapMalloc(Size);

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_ImageArray_LoadData: Loads data into image array.

static CSPSA_Result_t F_CSPSA_CORE_ImageArray_LoadData(
  const CSPSA_Handle_t Handle,
  const uint8_t* const Data_p,
  const uint32_t DataSize
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  static CSPSA_Key_t SavedKey;
  static bool KeySaved = false;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Image_t* ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
  CSPSA_Image_t* CurrentImage_p = NULL;
  uint32_t LoadImageArrayDataLeft = CSPSA_SessionInfo_p->Cache.LoadImageArrayDataLeft;
  uint32_t NbrOfImages = CSPSA_SessionInfo_p->Cache.NbrOfImages;
  CSPSA_DataItem_t* DataItem_p = (CSPSA_DataItem_t*)(void*) Data_p;
  int32_t DataLeft = (int32_t) DataSize;
  uint32_t DataLevel;
  uint32_t ChunkSize;

  // - Load data into image array in RAM. Try to avoid exceeding
  //   CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE for each image array, but exceptions
  //   can be made for large items. Such items should be stored in indivudal
  //   arrays.

  // - The initial number of image array is set by CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS.
  //   If the amount of image arrays is not enough, amount should be doubled.

  // - NOTE: Each image in the image array must start with a key. This in combination with that
  //   data comes streaming (in pieces of the storage media sector size) makes this function a bit messy.

  if (ImageArray_p == NULL)
  {
    // - First time here after a call to CSPSA_Open() with this Handle.
    // - Set initial number of image arrays and allocate space for it.
    CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS;
    ImageArray_p = (CSPSA_Image_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_Image_t) * CSPSA_SessionInfo_p->Cache.MaxNbrOfImages);
    CSPSA_SessionInfo_p->Cache.ImageArray_p = ImageArray_p;
    memset(ImageArray_p, 0, sizeof(CSPSA_Image_t) * CFG_CSPSA_CORE_CACHE_INITIAL_NBR_OF_IMAGE_ARRAYS);
    // - Allocate space for the very first image.
    NbrOfImages++;
    CurrentImage_p = &ImageArray_p[NbrOfImages-1];
    CurrentImage_p->Max = CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
    CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
    if (CurrentImage_p->Data_p == NULL)
    {
      NbrOfImages--;
      M_CSPSA_CORE_FREE(ImageArray_p);
      Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
      goto ErrorExit;
    }
  }
  else
  {
    // - We've been here before. Continue filling up the image we were working on last.
    CurrentImage_p = &ImageArray_p[NbrOfImages-1];
  }

  // - As DataSize has no correlation to item size, the last item could be partially stored. The amount
  //   left to store is noted in CSPSA_SessionInfo_p->Cache.LoadImageArrayDataLeft.

  if (LoadImageArrayDataLeft != 0)
  {
    // - The last item was stored in partial, but we know we have made room for in the current image buffer.
    ChunkSize = ((int32_t)LoadImageArrayDataLeft > DataLeft) ?  (uint32_t) DataLeft : LoadImageArrayDataLeft;
    memcpy((uint8_t*) CurrentImage_p->Data_p + CurrentImage_p->Level, Data_p, ChunkSize);
    DataItem_p = (CSPSA_DataItem_t*)(void*) (Data_p + ChunkSize);
    LoadImageArrayDataLeft -= ChunkSize;
    CurrentImage_p->Level += ChunkSize;
    DataLeft -= ChunkSize;
    if ( (DataLeft > 0) && (CurrentImage_p->Level == CurrentImage_p->Max) )
    {
      // - This image is now full, so create a new. First we need to check if the image array is full.
      if (NbrOfImages >= CSPSA_SessionInfo_p->Cache.MaxNbrOfImages)
      {
        // - We have exceeded the number of allocated image arrays. Double the size of the image array.
        uint32_t NewMaxNbrOfArrays = CSPSA_SessionInfo_p->Cache.MaxNbrOfImages * 2;
        CSPSA_Image_t* NewImageArray_p = (CSPSA_Image_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
        // - Set entire structure to zeroes.
        memset(NewImageArray_p, 0, sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
        // - Transfer all old array pointers to new array.
        memcpy(NewImageArray_p, CSPSA_SessionInfo_p->Cache.ImageArray_p, sizeof(CSPSA_Image_t) * CSPSA_SessionInfo_p->Cache.MaxNbrOfImages);
        M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->Cache.ImageArray_p);
        CSPSA_SessionInfo_p->Cache.ImageArray_p = NewImageArray_p;
        ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
        CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = NewMaxNbrOfArrays;
      }
      // - Make sure we have room for the next item.
      NbrOfImages++;
      CurrentImage_p = &ImageArray_p[NbrOfImages-1];
      if (DataLeft > (int)sizeof(CSPSA_Key_t))
      {
        uint32_t ItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
        CurrentImage_p->Max = (ItemSize > CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE) ? ItemSize : CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
      }
      else
      {
        CurrentImage_p->Max = CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
      }
      CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
      if (CurrentImage_p->Data_p == NULL)
      {
        NbrOfImages--;
        Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
        goto ErrorExit;
      }
      CurrentImage_p->Level = 0;
    }
  }

  if (KeySaved)
  {
    // - We have encountered the state when the last item in the last Data_p was a Key. The first 32-bit value
    //   in this Data_p is an item Size field, and its Key was stored until now. At least we know Key and Size
    //   are 32-bit aligned.
    uint32_t ItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + *(uint32_t*)(void*)Data_p);
    ChunkSize = ItemSize - sizeof(CSPSA_Key_t);
    ChunkSize = ((int)ChunkSize > DataLeft) ? (uint32_t) DataLeft : ChunkSize;
    KeySaved = false;

    if ((CurrentImage_p->Level + ItemSize) > CurrentImage_p->Max)
    {
      if (CurrentImage_p->Level == 0)
      {
        M_CSPSA_CORE_FREE(CurrentImage_p->Data_p);
        NbrOfImages--;
      }
      // - This image is now full, so create a new. First we need to check if the image array is full.
      if (NbrOfImages >= CSPSA_SessionInfo_p->Cache.MaxNbrOfImages)
      {
        // - We have exceeded the number of allocated image arrays. Double the size of the image array.
        uint32_t NewMaxNbrOfArrays = CSPSA_SessionInfo_p->Cache.MaxNbrOfImages * 2;
        CSPSA_Image_t* NewImageArray_p = (CSPSA_Image_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
        // - Set entire structure to zeroes.
        memset(NewImageArray_p, 0, sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
        // - Transfer all old array pointers to new array.
        memcpy(NewImageArray_p, CSPSA_SessionInfo_p->Cache.ImageArray_p, sizeof(CSPSA_Image_t) * CSPSA_SessionInfo_p->Cache.MaxNbrOfImages);
        M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->Cache.ImageArray_p);
        CSPSA_SessionInfo_p->Cache.ImageArray_p = NewImageArray_p;
        ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
        CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = NewMaxNbrOfArrays;
      }
      // - Make sure we have room for the next item.
      NbrOfImages++;
      CurrentImage_p = &ImageArray_p[NbrOfImages-1];
      CurrentImage_p->Max = (ItemSize > CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE) ? ItemSize : CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
      CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
      if (CurrentImage_p->Data_p == NULL)
      {
        NbrOfImages--;
        Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
        goto ErrorExit;
      }
      CurrentImage_p->Level = 0;
    }
    // - Write Key.
    memcpy((uint8_t*)CurrentImage_p->Data_p + CurrentImage_p->Level, (uint8_t*)&SavedKey, sizeof(SavedKey));
    CurrentImage_p->Level += sizeof(SavedKey);
    // - Write Size and data.
    memcpy((uint8_t*)CurrentImage_p->Data_p + CurrentImage_p->Level, Data_p, ChunkSize);
    CurrentImage_p->Level += ChunkSize;
    DataLeft -= ItemSize - sizeof(CSPSA_Key_t);
    DataItem_p = (CSPSA_DataItem_t*)(void*) (Data_p + ChunkSize);

    if (DataLeft < 0)
    {
      LoadImageArrayDataLeft = -DataLeft;
    }
  }

  // - If DataLeft cannot contain the Size field, exit loop and store the Key for the next round.
  //   NOTE: !!! Don't remove the (int) cast before sizeof()! as DataLeft is a signed integer !!!
  while (DataLeft > (int)sizeof(CSPSA_Key_t))
  {
    uint32_t NextItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
    CSPSA_DataItem_t* StartDataItem_p = DataItem_p;

    DataLevel = 0;
    // - Gather as many items that can be fit in current image buffer.
    //   NOTE: !!! Don't remove the (int) cast before sizeof()! as DataLeft is a signed integer !!!
    while ( (DataLeft > (int)sizeof(CSPSA_Key_t)) && ((CurrentImage_p->Level + DataLevel + NextItemSize) <= CurrentImage_p->Max) )
    {
      DataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*) DataItem_p + NextItemSize);
      DataLevel += NextItemSize;
      DataLeft -= NextItemSize;
      // - We might encounter the tricky state when the last item in Data_p is a Key :-( Thus,
      //   we cannot use the size field, and the Key must be saved until next call to this function.
      if (DataLeft >= (int)sizeof(*DataItem_p))
      {
        NextItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
      }
      else
      {
        // - The size value will be the first value in next call to this function.
        NextItemSize = 0;
      }
    }

    if (DataLevel == 0)
    {
      if (CurrentImage_p->Level == 0)
      {
        // - A special case: The item was larger than the current buffer. We need to increase the
        //   size of this buffer to fit this large item.
        ChunkSize = ((int) NextItemSize > DataLeft) ? (uint32_t) DataLeft : NextItemSize;
        M_CSPSA_CORE_FREE(CurrentImage_p->Data_p);
        CurrentImage_p->Max = NextItemSize;
        CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
        if (CurrentImage_p->Data_p == NULL)
        {
          Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
          goto ErrorExit;
        }
        memcpy((uint8_t*)CurrentImage_p->Data_p, (uint8_t*) StartDataItem_p, ChunkSize);
        CurrentImage_p->Level += ChunkSize;
        DataLeft -= NextItemSize;
        DataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*) DataItem_p + NextItemSize);
        if (DataLeft <= 0)
        {
          LoadImageArrayDataLeft = -DataLeft;
        }
      }
      else
      if ((CurrentImage_p->Level + NextItemSize) > CurrentImage_p->Max)
      {
        // - This image is now full, so create a new. First we need to check if the image array is full.
        if (NbrOfImages >= CSPSA_SessionInfo_p->Cache.MaxNbrOfImages)
        {
          // - We have exceeded the number of allocated image arrays. Double the size of the image array.
          uint32_t NewMaxNbrOfArrays = CSPSA_SessionInfo_p->Cache.MaxNbrOfImages * 2;
          CSPSA_Image_t* NewImageArray_p = (CSPSA_Image_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
          // - Set entire structure to zeroes.
          memset(NewImageArray_p, 0, sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
          // - Transfer all old array pointers to new array.
          memcpy(NewImageArray_p, CSPSA_SessionInfo_p->Cache.ImageArray_p, sizeof(CSPSA_Image_t) * CSPSA_SessionInfo_p->Cache.MaxNbrOfImages);
          M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->Cache.ImageArray_p);
          CSPSA_SessionInfo_p->Cache.ImageArray_p = NewImageArray_p;
          ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
          CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = NewMaxNbrOfArrays;
        }
        // - Make sure we have room for the next item.
        NbrOfImages++;
        CurrentImage_p = &ImageArray_p[NbrOfImages-1];
        CurrentImage_p->Max = (NextItemSize > CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE) ? NextItemSize : CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
        CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
        if (CurrentImage_p->Data_p == NULL)
        {
          NbrOfImages--;
          Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
          goto ErrorExit;
        }
        CurrentImage_p->Level = 0;

        ChunkSize = ((int)NextItemSize > DataLeft) ? (uint32_t)DataLeft : NextItemSize;
        memcpy((uint8_t*) CurrentImage_p->Data_p + CurrentImage_p->Level, (uint8_t*) StartDataItem_p, ChunkSize);
        CurrentImage_p->Level += ChunkSize;
        DataLeft -= NextItemSize;
        DataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*) DataItem_p + NextItemSize);
        if (DataLeft <= 0)
        {
          // - DataLeft is now either negative or zero!
          LoadImageArrayDataLeft = -DataLeft;
        }
      }
      else
      {
        // - Internal error.
        Result = T_CSPSA_RESULT_UNDEFINED;
        goto ErrorExit;
      }
    }
    else
    {
      // - The normal case (DataLevel is > 0). We have gathered a set of complete items. Copy them to current image.

      if (DataLeft <= 0)
      {
        // - DataLeft is now either negative or zero!
        DataLevel += DataLeft;
        LoadImageArrayDataLeft = -DataLeft;
      }

      memcpy((uint8_t*) CurrentImage_p->Data_p + CurrentImage_p->Level, (uint8_t*) StartDataItem_p, DataLevel);
      CurrentImage_p->Level += DataLevel;

      if ((CurrentImage_p->Level + DataLevel + NextItemSize) > CurrentImage_p->Max)
      {
        if (NextItemSize > 0)
        {
          // - This image is now full, so create a new. First we need to check if the image array is full.
          if (NbrOfImages >= CSPSA_SessionInfo_p->Cache.MaxNbrOfImages)
          {
            // - We have exceeded the number of allocated image arrays. Double the size of the image array.
            uint32_t NewMaxNbrOfArrays = CSPSA_SessionInfo_p->Cache.MaxNbrOfImages * 2;
            CSPSA_Image_t* NewImageArray_p = (CSPSA_Image_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
            // - Set entire structure to zeroes.
            memset(NewImageArray_p, 0, sizeof(CSPSA_Image_t) * NewMaxNbrOfArrays);
            // - Transfer all old array pointers to new array.
            memcpy(NewImageArray_p, CSPSA_SessionInfo_p->Cache.ImageArray_p, sizeof(CSPSA_Image_t) * CSPSA_SessionInfo_p->Cache.MaxNbrOfImages);
            M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->Cache.ImageArray_p);
            CSPSA_SessionInfo_p->Cache.ImageArray_p = NewImageArray_p;
            ImageArray_p = CSPSA_SessionInfo_p->Cache.ImageArray_p;
            CSPSA_SessionInfo_p->Cache.MaxNbrOfImages = NewMaxNbrOfArrays;
          }
          // - Make sure we have room for the next item.
          NbrOfImages++;
          CurrentImage_p = &ImageArray_p[NbrOfImages-1];
          CurrentImage_p->Max = (NextItemSize > CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE) ? NextItemSize : CFG_CSPSA_CORE_CACHE_ITEM_ARRAY_SIZE;
          CurrentImage_p->Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_ImageArray_HeapMalloc(Handle, CurrentImage_p->Max);
          if (CurrentImage_p->Data_p == NULL)
          {
            NbrOfImages--;
            Result = T_CSPSA_RESULT_E_NO_VALID_IMAGE;
            goto ErrorExit;
          }
          CurrentImage_p->Level = 0;
        }
      }
    }
  }

  if (DataLeft == sizeof(CSPSA_Key_t))
  {
    SavedKey = DataItem_p->Key;
    KeySaved = true;
    DataLeft = 0;
  }

  CSPSA_SessionInfo_p->Cache.LoadImageArrayDataLeft = LoadImageArrayDataLeft;

ErrorExit:

  CSPSA_SessionInfo_p->Cache.NbrOfImages = NbrOfImages;

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_Cache_GetDataItem: Fetches a data item.

static CSPSA_Result_t F_CSPSA_CORE_Cache_GetDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** DataItem_pp
)
{
  CSPSA_Result_t Result;
  CSPSA_DataItem_t* NextDataItem_p;

  // - First look in item list for the key.
  Result = F_CSPSA_CORE_ItemList_GetDataItem(Handle, Key, DataItem_pp, &NextDataItem_p);
  if (Result == T_CSPSA_RESULT_OK) goto ErrorExit;

  Result = F_CSPSA_CORE_ImageArray_GetDataItem(Handle, Key, DataItem_pp, &NextDataItem_p);
  if (Result == T_CSPSA_RESULT_OK) goto ErrorExit;

  // - Finally look among the fast parameters for the key.
  Result = F_CSPSA_CORE_FastParameter_GetDataItem(Handle, Key, DataItem_pp, &NextDataItem_p);
  if (Result == T_CSPSA_RESULT_OK) goto ErrorExit;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

// - F_CSPSA_CORE_Cache_GetNextDataItem: Fetches the data item with next-higher key.
//       If there is no such item, T_CSPSA_RESULT_E_INVALID_KEY is returned.

static CSPSA_Result_t F_CSPSA_CORE_Cache_GetNextDataItem(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_DataItem_t** NextDataItem_pp
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_END_OF_DATA;
  CSPSA_DataItem_t* DataItem_p;
  CSPSA_DataItem_t* NextItemListDataItem_p;
  CSPSA_DataItem_t* NextImageArrayDataItem_p;
  CSPSA_DataItem_t* NextFastParameterDataItem_p;
  CSPSA_Key_t SmallestKey = UINT32_MAX;

  // - Assume that no key will be found
  *NextDataItem_pp = NULL;

  // - Get next data item in item list
  (void)F_CSPSA_CORE_ItemList_GetDataItem(Handle, Key, &DataItem_p, &NextItemListDataItem_p);
  // - Get next data item in image array
  (void)F_CSPSA_CORE_ImageArray_GetDataItem(Handle, Key, &DataItem_p, &NextImageArrayDataItem_p);
  // - Get next data item in fast parameter list
  (void)F_CSPSA_CORE_FastParameter_GetDataItem(Handle, Key, &DataItem_p, &NextFastParameterDataItem_p);

  // - Get key for comparison from item list, image array or fast parameters
  if (NextItemListDataItem_p != NULL)
  {
    SmallestKey = NextItemListDataItem_p->Key;
  }
  else if (NextImageArrayDataItem_p != NULL)
  {
    SmallestKey = NextImageArrayDataItem_p->Key;
  }
  else if (NextFastParameterDataItem_p != NULL)
  {
    SmallestKey = NextFastParameterDataItem_p->Key;
  }

  // - Obtain the smallest key among item list, image array and fast parameters
  if ((NextItemListDataItem_p != NULL) && (NextItemListDataItem_p->Key < SmallestKey))
  {
    SmallestKey = NextItemListDataItem_p->Key;
  }
  if ((NextImageArrayDataItem_p != NULL) && (NextImageArrayDataItem_p->Key < SmallestKey))
  {
    SmallestKey = NextImageArrayDataItem_p->Key;
  }
  if ((NextFastParameterDataItem_p != NULL) && (NextFastParameterDataItem_p->Key < SmallestKey))
  {
    SmallestKey = NextFastParameterDataItem_p->Key;
  }

  // - Locate if key came from fast parameters, image array or item list and return its item
  if ((NextFastParameterDataItem_p != NULL) && (NextFastParameterDataItem_p->Key == SmallestKey))
  {
    *NextDataItem_pp= NextFastParameterDataItem_p;
  }
  if ((NextImageArrayDataItem_p != NULL) && (NextImageArrayDataItem_p->Key == SmallestKey))
  {
    *NextDataItem_pp= NextImageArrayDataItem_p;
  }
  if ((NextItemListDataItem_p != NULL) && (NextItemListDataItem_p->Key == SmallestKey))
  {
    *NextDataItem_pp= NextItemListDataItem_p;
  }

  if (*NextDataItem_pp != NULL)
  {
    Result = T_CSPSA_RESULT_OK;
  }
  return Result;
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

CSPSA_Result_t CSPSA_CORE_Close(CSPSA_Handle_t* const Handle_p)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p;

  if ((Handle_p == NULL) || (*Handle_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) *Handle_p;

  // - Flush and clean up.

  // - Remove the session info from session info list.
  {
    CSPSA_SessionInfoList_t* SessionInfoListNode_p = V_CSPSA_CORE_OpenedSessionsList_p;

    if (SessionInfoListNode_p->SessionInfo_p == CSPSA_SessionInfo_p)
    {
      // - It was the first item in the list.
      if (--SessionInfoListNode_p->Counter == 0)
      {
        if ( !((CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) && !CSPSA_SessionInfo_p->SessionCreated) )
        {
          Result = CSPSA_CORE_Flush(*Handle_p);
          if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
        }

        // - Close flash memory device.
        Result = CSPSA_LL_Close(*Handle_p);
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

        V_CSPSA_CORE_OpenedSessionsList_p = SessionInfoListNode_p->Next_p;
        M_CSPSA_CORE_FREE(SessionInfoListNode_p);
        F_CSPSA_CORE_ImageArray_Cleanup(*Handle_p);
        F_CSPSA_CORE_ItemList_Cleanup(*Handle_p);
        F_CSPSA_CORE_FastParameter_Cleanup(*Handle_p);
        M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->LastStorageImageHeader_p);
        M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p);
      }
    }
    else
    {
      // - Flip to the session list item.
      while
      (
        (SessionInfoListNode_p != NULL)
        &&
        (SessionInfoListNode_p->Next_p != NULL)
        &&
        (SessionInfoListNode_p->Next_p->SessionInfo_p != CSPSA_SessionInfo_p)
      )
      {
        SessionInfoListNode_p = SessionInfoListNode_p->Next_p;
      }
      if
      (
        (SessionInfoListNode_p != NULL)
        &&
        (SessionInfoListNode_p->Next_p != NULL)
        &&
        (SessionInfoListNode_p->Next_p->SessionInfo_p == CSPSA_SessionInfo_p)
      )
      {
        if (--SessionInfoListNode_p->Next_p->Counter == 0)
        {
          CSPSA_SessionInfoList_t* TempSessionInfoListNode_p;

          if ( !((CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) && !CSPSA_SessionInfo_p->SessionCreated) )
          {
            Result = CSPSA_CORE_Flush(*Handle_p);
            if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
          }
          // - Close flash memory device.
          Result = CSPSA_LL_Close(*Handle_p);
          if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

          TempSessionInfoListNode_p = SessionInfoListNode_p->Next_p;
          SessionInfoListNode_p->Next_p = SessionInfoListNode_p->Next_p->Next_p;
          M_CSPSA_CORE_FREE(TempSessionInfoListNode_p);
          F_CSPSA_CORE_ImageArray_Cleanup(*Handle_p);
          F_CSPSA_CORE_ItemList_Cleanup(*Handle_p);
          F_CSPSA_CORE_FastParameter_Cleanup(*Handle_p);
          M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->LastStorageImageHeader_p);
          M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p);
        }
      }
      else
      {
        // - ???
        Result = T_CSPSA_RESULT_UNDEFINED;
        goto ErrorExit;
      }
    }
  }

  *Handle_p = NULL;
  Result = T_CSPSA_RESULT_OK;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_Create(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p;
  CSPSA_Config_t* Config_p = FindArea(ParameterAreaName_p);
  /* coverity[dead_error_line] */
  bool LL_OpenCalled = false;

  if ((ParameterAreaName_p == NULL) || (Handle_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  if (Config_p == NULL)
  {
    // - Parameter area does not exist.
    return T_CSPSA_RESULT_E_OPEN_ERROR;
  }

  // - Allocate space for session info.
  CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*CSPSA_SessionInfo_p));

  // - Set everything to zero.
  memset(CSPSA_SessionInfo_p, 0, sizeof(*CSPSA_SessionInfo_p));
  CSPSA_SessionInfo_p->Config = *Config_p;

  *Handle_p = CSPSA_SessionInfo_p;

  // - Open flash memory device.
  Result = CSPSA_LL_Open(*Handle_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  LL_OpenCalled = true;
  {
    uint32_t MaxHeaderSize;
    uint32_t SectorSize = CSPSA_SessionInfo_p->Config.SectorSize;

    MaxHeaderSize = CFG_CSPSA_CORE_STORAGE_IMAGE_HEADER_MAX_SIZE * SectorSize;

    CSPSA_SessionInfo_p->LastStorageImageHeader_p = (CSPSA_CORE_Header_t*) F_CSPSA_CORE_HeapMalloc(MaxHeaderSize);
    // - Fill with FF's so that entire BBT area (if any) is valid by default.
    memset(CSPSA_SessionInfo_p->LastStorageImageHeader_p, 0xFF, MaxHeaderSize);
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Magic = D_CSPSA_CORE_HEADER_MAGIC;
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Version = D_CSPSA_CORE_HEADER_VERSION;
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Index = 0;
    // - As storage location is toggled between start and end of CSPSA area, set the
    //   data position flag to end so that the first image is stored at start of CSPSA.
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Flags = T_CSPSA_HEADERFLAGS_DATA_AT_END;
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->DataSize = 0;
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Crc32Data = 0;
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->Crc32Header = 0;
    memset(CSPSA_SessionInfo_p->LastStorageImageHeader_p->FastParameter, 0, sizeof(CSPSA_SessionInfo_p->LastStorageImageHeader_p->FastParameter));
    memset(CSPSA_SessionInfo_p->LastStorageImageHeader_p->Reserved, 0, sizeof(CSPSA_SessionInfo_p->LastStorageImageHeader_p->Reserved));
    CSPSA_SessionInfo_p->LastStorageImageHeader_p->BbtSize = 0;
  }

  // - Get fast parameters from header
  Result = F_CSPSA_CORE_FastParameter_GetFromHeader(*Handle_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  CSPSA_SessionInfo_p->SessionCreated = true;

  // - Add session to list of opened sessions.
  {
    CSPSA_SessionInfoList_t* SessionInfoListNode_p = V_CSPSA_CORE_OpenedSessionsList_p;

    if (SessionInfoListNode_p == NULL)
    {
      // - First item in list.
      V_CSPSA_CORE_OpenedSessionsList_p = (CSPSA_SessionInfoList_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_SessionInfoList_t));
      V_CSPSA_CORE_OpenedSessionsList_p->SessionInfo_p = CSPSA_SessionInfo_p;
      V_CSPSA_CORE_OpenedSessionsList_p->Counter = 1;
      V_CSPSA_CORE_OpenedSessionsList_p->Next_p = NULL;
    }
    else
    {
      // - Flip to last item and append new session.
      while (SessionInfoListNode_p->Next_p != NULL)
      {
        SessionInfoListNode_p = SessionInfoListNode_p->Next_p;
      }
      SessionInfoListNode_p->Next_p = (CSPSA_SessionInfoList_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_SessionInfoList_t));
      SessionInfoListNode_p->Next_p->SessionInfo_p = CSPSA_SessionInfo_p;
      SessionInfoListNode_p->Next_p->Counter = 1;
      SessionInfoListNode_p->Next_p->Next_p = NULL;
    }
  }

  CSPSA_SessionInfo_p->Cache.CacheUpdated = true;

  // - Assure the image is flushed on next close/flush.
  CSPSA_SessionInfo_p->Cache.CacheUpdated = true;

  // Add the storage area to the list of registered areas (if it is not already there)
  (void)CSPSA_AddParameterArea(Config_p); // ignore result, assume that if it is there, the config matches.

ErrorExit:

  if (Result != T_CSPSA_RESULT_OK && LL_OpenCalled)
  {
    // close flash memory device
    CSPSA_LL_Close(*Handle_p);
  }

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_DeleteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Cache_t* Cache_p;
  CSPSA_Image_t* ImageArray_p;
  CSPSA_ItemListNode_t* ItemNode_p;
  bool KeyFound = false;

  if (Handle == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  Cache_p = &(CSPSA_SessionInfo_p->Cache);
  ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
  ImageArray_p = Cache_p->ImageArray_p;

  if ((CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) && !CSPSA_SessionInfo_p->SessionCreated)
  {
    Result = T_CSPSA_RESULT_E_READ_ONLY;
    goto ErrorExit;
  }

  // - Delete item from both item list and image array. Start with the tiny item cache.

  // - Look in data item cache.
  if ( (CSPSA_SessionInfo_p->Cache.LastDataItem_p != NULL) && (CSPSA_SessionInfo_p->Cache.LastDataItem_p->Key == Key) )
  {
    CSPSA_SessionInfo_p->Cache.LastDataItem_p = NULL;
  }
  if ( (CSPSA_SessionInfo_p->Cache.NextDataItem_p != NULL) && (CSPSA_SessionInfo_p->Cache.NextDataItem_p->Key == Key) )
  {
    CSPSA_SessionInfo_p->Cache.NextDataItem_p = NULL;
  }

  // - Look in item list.
  {
    CSPSA_ItemListNode_t* LastItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;

    while ((ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (Key > ItemNode_p->DataItem_p->Key))
    {
      LastItemNode_p = ItemNode_p;
      ItemNode_p = ItemNode_p->Next_p;
    }
    if ((ItemNode_p != NULL) && (ItemNode_p->DataItem_p != NULL) && (Key == ItemNode_p->DataItem_p->Key))
    {
      // - We found the item. Now remove it.
      KeyFound = true;
      if (ItemNode_p == CSPSA_SessionInfo_p->Cache.ItemList_p)
      {
        // - It was the first item in the list.
        CSPSA_SessionInfo_p->Cache.ItemList_p = CSPSA_SessionInfo_p->Cache.ItemList_p->Next_p;
        M_CSPSA_CORE_FREE(ItemNode_p->DataItem_p);
        M_CSPSA_CORE_FREE(ItemNode_p);
      }
      else
      {
        LastItemNode_p->Next_p = ItemNode_p->Next_p;
        M_CSPSA_CORE_FREE(ItemNode_p->DataItem_p);
        M_CSPSA_CORE_FREE(ItemNode_p);
      }
    }
  }
  // - Then remove it from the image array. NOTE: This algorithm isn't very
  //   effective. If delete becomes a frequently used function it might be
  //   quicker to keep a "deleted item list". Here we assume "delete" is a
  //   very rarely used function so it is better to adjust the image array to
  //   keep other functions quick.
  {
    uint32_t ImageArrayIndex;
    CSPSA_Image_t* CurrentImage_p = NULL;
    CSPSA_DataItem_t* DataItem_p;
    uint32_t Size32Aligned;
    uint32_t DataLeft;

    Result = F_CSPSA_CORE_ImageArray_GetArrayIndex(Handle, Key, &ImageArrayIndex);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    CurrentImage_p = &ImageArray_p[ImageArrayIndex];
    DataLeft = CurrentImage_p->Level;
    DataItem_p = CurrentImage_p->Data_p;

    // - Start searching the image.
    while (DataLeft > 0)
    {
      if (Key == DataItem_p->Key)
      {
        // - We found the item.
        KeyFound = true;
        Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
        memmove((uint8_t*)DataItem_p, (uint8_t*)DataItem_p + Size32Aligned, DataLeft - Size32Aligned);
        CurrentImage_p->Level -= Size32Aligned;
        if (CurrentImage_p->Level == 0)
        {
          // - We need to remove this image buffer.
          uint32_t TempSize = (CSPSA_SessionInfo_p->Cache.NbrOfImages - ImageArrayIndex - 1) * sizeof(*CurrentImage_p);
          M_CSPSA_CORE_FREE(CurrentImage_p->Data_p);
          if (TempSize > 0)
          {
            // - Shift down all buffers.
            memmove(
              (uint8_t*)CurrentImage_p,
              (uint8_t*)CurrentImage_p + sizeof(*CurrentImage_p),
              TempSize
            );
          }
          else
          {
            CurrentImage_p->Level = 0;
            CurrentImage_p->Max = 0;
          }
          CSPSA_SessionInfo_p->Cache.NbrOfImages--;
        }
        break;
      }
      if (Key > DataItem_p->Key)
      {
        Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
        DataLeft -= Size32Aligned;
        if (DataLeft > 0)
        {
          DataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*)DataItem_p + Size32Aligned);
        }
      }
      else
      {
        // - As key is sorted there's no need to look further.
        break;
      }
    }
  }

ErrorExit:

  if (KeyFound)
  {
    Result = T_CSPSA_RESULT_OK;
  }
  else
  {
    Result = T_CSPSA_RESULT_E_INVALID_KEY;
  }

  CSPSA_SessionInfo_p->Cache.CacheUpdated = true;

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_Flush(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Cache_t* Cache_p;
  CSPSA_Image_t* ImageArray_p;
  CSPSA_ItemListNode_t* ItemNode_p;
  CSPSA_Image_t FirstStoreBuffer;
  CSPSA_Image_t StoreBuffer;
  uint32_t CrcValue = D_CSPSA_CORE_HEADER_MAGIC;
  uint32_t BlockSize;
  uint32_t StorageImageOffset;
  uint32_t TotalDataSize = 0;
  CSPSA_CORE_Header_t* StorageImageHeader_p;
  bool DataAtEnd;
  bool WriteBackwards;

  if (Handle == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  Cache_p = &(CSPSA_SessionInfo_p->Cache);
  ImageArray_p = Cache_p->ImageArray_p;
  ItemNode_p = CSPSA_SessionInfo_p->Cache.ItemList_p;
  StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;
  DataAtEnd = !((StorageImageHeader_p->Flags & T_CSPSA_HEADERFLAGS_DATA_AT_END) != 0); // - Toggled!
  WriteBackwards = DataAtEnd;

  FirstStoreBuffer.Data_p = NULL;
  StoreBuffer.Data_p = NULL;

  if ((CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) && !CSPSA_SessionInfo_p->SessionCreated)
  {
    Result = T_CSPSA_RESULT_E_READ_ONLY;
    goto ErrorExit;
  }

  if (!CSPSA_SessionInfo_p->Cache.CacheUpdated)
  {
    // - Cache is not updated, so don't save data.
    Result = T_CSPSA_RESULT_OK;
    goto ErrorExit;
  }

  BlockSize = CSPSA_SessionInfo_p->Config.BlockSize;

  if (DataAtEnd)
  {
    StorageImageOffset = CSPSA_SessionInfo_p->Config.SizeInBytes - 2 * BlockSize;
    StorageImageHeader_p->Flags |= T_CSPSA_HEADERFLAGS_DATA_AT_END; // - Toggle flag.
  }
  else
  {
    StorageImageOffset = BlockSize;
    StorageImageHeader_p->Flags &= ~T_CSPSA_HEADERFLAGS_DATA_AT_END; // - Toggle flag.
  }

  // - As the first storage buffer will contain the CSPSA header it must be stored last.
  //   Therefore we have a dedicated buffer for this.
  FirstStoreBuffer.Max = BlockSize;
  FirstStoreBuffer.Level = sizeof(*StorageImageHeader_p) + StorageImageHeader_p->BbtSize;
  FirstStoreBuffer.Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_HeapMalloc(FirstStoreBuffer.Max);
  StoreBuffer.Max = BlockSize;
  StoreBuffer.Level = 0;
  StoreBuffer.Data_p = (CSPSA_DataItem_t*) F_CSPSA_CORE_HeapMalloc(StoreBuffer.Max);

  if
  (
    (ImageArray_p != NULL)
    &&
    (
      (CSPSA_SessionInfo_p->Cache.NbrOfImages > 1)
      ||
      (CSPSA_SessionInfo_p->Cache.ImageArray_p[0].Level > 0)
    )
  )
  {
    uint32_t ImageArrayIndex;

    // - Traverse through all image arrays and store their contents to memory media. Keep an eye on
    //   the separate item list to switch in new or updated items.
    for (ImageArrayIndex = 0; ImageArrayIndex < CSPSA_SessionInfo_p->Cache.NbrOfImages; ImageArrayIndex++)
    {
      if (ItemNode_p == NULL)
      {
        // - There were no items in the item list. So flush image array as it is.
        Result =
          F_CSPSA_CORE_ImageArray_Flush(
            Handle,
            ImageArrayIndex,
            0,
            ImageArray_p[ImageArrayIndex].Level,
            &StorageImageOffset,
            &FirstStoreBuffer,
            &StoreBuffer,
            &CrcValue,
            &TotalDataSize
          );
        if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      }
      else
      {
        // - There are items in the separate items list. This means we need to combine item list with
        //   image array in a clever way. Try to gather as large chunks of image array data as possible.
        //   The items in both item list and image array are sorted on rising key value.
        uint32_t DataLeft = ImageArray_p[ImageArrayIndex].Level;
        CSPSA_DataItem_t* ImageArrayDataItem_p = ImageArray_p[ImageArrayIndex].Data_p;
        CSPSA_DataItem_t* ItemListDataItem_p = ItemNode_p->DataItem_p;
        CSPSA_DataItem_t* StartImageArrayDataItem_p = ImageArray_p[ImageArrayIndex].Data_p;
        uint32_t ChunkSize;

        while (DataLeft > 0)
        {
          // - As long as image array key value is lower than current item list object's key
          //   we gather an image for copying.
          ChunkSize = 0;
          while ( (DataLeft > 0) && ((ItemListDataItem_p == NULL) || (ImageArrayDataItem_p->Key < ItemListDataItem_p->Key)) )
          {
            uint32_t ItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*ImageArrayDataItem_p) + ImageArrayDataItem_p->Size);
            ChunkSize += ItemSize;
            DataLeft -= ItemSize;
            ImageArrayDataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*)ImageArrayDataItem_p + ItemSize);
          }
          if (ChunkSize > 0)
          {
            Result =
              F_CSPSA_CORE_ImageArray_Flush(
                Handle,
                ImageArrayIndex,
                (ptrdiff_t)StartImageArrayDataItem_p - (ptrdiff_t)ImageArray_p[ImageArrayIndex].Data_p,
                ChunkSize,
                &StorageImageOffset,
                &FirstStoreBuffer,
                &StoreBuffer,
                &CrcValue,
                &TotalDataSize
              );
            if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
            StartImageArrayDataItem_p = ImageArrayDataItem_p;
          }
          if ((DataLeft > 0) && (ItemListDataItem_p != NULL))
          {
            // - Now we have an item from item list to insert.
            if (ImageArrayDataItem_p->Key == ItemListDataItem_p->Key)
            {
              // - Replace old item. First skip the item in the image array...
              uint32_t ItemSize = M_CSPSA_CORE_ALIGN32(sizeof(*ImageArrayDataItem_p) + ImageArrayDataItem_p->Size);
              ImageArrayDataItem_p = (CSPSA_DataItem_t*)(void*)((uint8_t*)ImageArrayDataItem_p + ItemSize);
              DataLeft -= ItemSize;
              // - ... then flush item list object.
              Result = F_CSPSA_CORE_ItemList_Flush(Handle, ItemNode_p, &StorageImageOffset, &FirstStoreBuffer, &StoreBuffer, &CrcValue, &TotalDataSize);
              if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
              StartImageArrayDataItem_p = ImageArrayDataItem_p;
            }
            else
            if (ImageArrayDataItem_p->Key > ItemListDataItem_p->Key)
            {
              // - This is a new item to be inserted here.
              Result = F_CSPSA_CORE_ItemList_Flush(Handle, ItemNode_p, &StorageImageOffset, &FirstStoreBuffer, &StoreBuffer, &CrcValue, &TotalDataSize);
              if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
            }
            else
            {
              // - Sanity check...
              Result = T_CSPSA_RESULT_UNDEFINED;
              goto ErrorExit;
            }
            ItemNode_p = ItemNode_p->Next_p;
            if (ItemNode_p != NULL)
            {
              ItemListDataItem_p = ItemNode_p->DataItem_p;
            }
            else
            {
              ItemListDataItem_p = NULL;
            }
          }
        }
      }
    }
    // - There may still be items in the separate list. Add them now.
    while (ItemNode_p != NULL)
    {
      Result = F_CSPSA_CORE_ItemList_Flush(Handle, ItemNode_p, &StorageImageOffset, &FirstStoreBuffer, &StoreBuffer, &CrcValue, &TotalDataSize);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
      ItemNode_p = ItemNode_p->Next_p;
    }
  }
  else
  {
    // - There were no image array. Copy one item at a time to the storage buffers. Empty
    //   buffer to media when necessary, but save first buffer for last.
    while (ItemNode_p != NULL)
    {
      Result = F_CSPSA_CORE_ItemList_Flush(Handle, ItemNode_p, &StorageImageOffset, &FirstStoreBuffer, &StoreBuffer, &CrcValue, &TotalDataSize);
      if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

      ItemNode_p = ItemNode_p->Next_p;
    }
  }

  if (StoreBuffer.Level > 0)
  {
    // - Store to media.
    uint32_t SectorAlignedSize = M_CSPSA_CORE_ALIGN(StoreBuffer.Level, CSPSA_SessionInfo_p->Config.SectorSize);
    Result = CSPSA_LL_Write(Handle, StorageImageOffset, StoreBuffer.Data_p, SectorAlignedSize, WriteBackwards);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
    StorageImageOffset = (WriteBackwards) ? StorageImageOffset - StoreBuffer.Level : StorageImageOffset + StoreBuffer.Level;
    TotalDataSize += StoreBuffer.Level;
    StoreBuffer.Level = 0;
  }

  // - Update the fast parameters in the header
  Result = F_CSPSA_CORE_FastParameter_Flush(Handle);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  // - Now store first buffer, but first fill in header.
  {
    uint32_t HeaderSize = sizeof(*StorageImageHeader_p) + StorageImageHeader_p->BbtSize;
    uint32_t SectorAlignedSize;

    StorageImageHeader_p->Index++;
    StorageImageHeader_p->DataSize = TotalDataSize + FirstStoreBuffer.Level - HeaderSize;
    StorageImageHeader_p->Crc32Data = CrcValue;
    StorageImageHeader_p->Crc32Header = F_CSPSA_CORE_StorageImage_CalcCrc32Header(Handle, StorageImageHeader_p);
    memcpy(FirstStoreBuffer.Data_p, (uint8_t*)StorageImageHeader_p, HeaderSize);

    StorageImageOffset = (DataAtEnd) ?  CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize : 0;
    SectorAlignedSize = M_CSPSA_CORE_ALIGN(FirstStoreBuffer.Level, CSPSA_SessionInfo_p->Config.SectorSize);
    Result =
      CSPSA_LL_Write(
        Handle,
        StorageImageOffset,
        FirstStoreBuffer.Data_p,
        SectorAlignedSize,
        WriteBackwards
      );
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    TotalDataSize += FirstStoreBuffer.Level;
    StorageImageOffset = (WriteBackwards) ? StorageImageOffset - FirstStoreBuffer.Level : StorageImageOffset + FirstStoreBuffer.Level;
    FirstStoreBuffer.Level = 0;

#if CFG_CSPSA_CORE_ENABLE_EXTRA_HEADER
    // - We have created the first CSPSA image. Now create a copy of the StorageImageHeader_p
    //   at the opposite end of where data was written.
    StorageImageOffset = (!DataAtEnd) ?  CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize : 0;
    StorageImageHeader_p->Index++;
    StorageImageHeader_p->Crc32Header = F_CSPSA_CORE_StorageImage_CalcCrc32Header(Handle, StorageImageHeader_p);
    Result = CSPSA_LL_Write(Handle, StorageImageOffset, StorageImageHeader_p, HeaderSize, !WriteBackwards);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
#else
    StorageImageOffset = (!DataAtEnd) ?  CSPSA_SessionInfo_p->Config.SizeInBytes - BlockSize : 0;
    Result = CSPSA_LL_Erase(Handle, StorageImageOffset, !WriteBackwards);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
#endif
  }

  CSPSA_SessionInfo_p->LastStorageImageHeader_p = StorageImageHeader_p;

ErrorExit:

  M_CSPSA_CORE_FREE(FirstStoreBuffer.Data_p);
  M_CSPSA_CORE_FREE(StoreBuffer.Data_p);

  CSPSA_SessionInfo_p->Cache.CacheUpdated = false;

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_Format(const CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p;
  CSPSA_CORE_Header_t* StorageImageHeader_p;

  if (Handle == NULL)
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  StorageImageHeader_p = CSPSA_SessionInfo_p->LastStorageImageHeader_p;

  F_CSPSA_CORE_ImageArray_Cleanup(Handle);
  F_CSPSA_CORE_ItemList_Cleanup(Handle);

  CSPSA_LL_Format(Handle);

  CSPSA_SessionInfo_p->LastStorageImageHeader_p = StorageImageHeader_p;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Magic = D_CSPSA_CORE_HEADER_MAGIC;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Version = D_CSPSA_CORE_HEADER_VERSION;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Index = 0;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Flags = T_CSPSA_HEADERFLAGS_DATA_AT_END;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->DataSize = 0;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Crc32Data = 0;
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->Crc32Header = 0;
  memset(CSPSA_SessionInfo_p->LastStorageImageHeader_p->FastParameter, 0, sizeof(CSPSA_SessionInfo_p->LastStorageImageHeader_p->FastParameter));
  memset(CSPSA_SessionInfo_p->LastStorageImageHeader_p->Reserved, 0, sizeof(CSPSA_SessionInfo_p->LastStorageImageHeader_p->Reserved));
  CSPSA_SessionInfo_p->LastStorageImageHeader_p->BbtSize = 0;

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_Open(
  const char* ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_Result_t GetHeaderResult = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = NULL;
  CSPSA_CORE_Header_t* StorageImageHeader_p = NULL;
  CSPSA_Config_t* Config_p = FindArea(ParameterAreaName_p);
  bool LL_OpenCalled = false;

  if (Config_p == NULL)
  {
    return T_CSPSA_RESULT_E_OPEN_ERROR;
  }
  // - Check if there is already an opened session with configuration which matches
  //   the requested configuration.
  {
    CSPSA_SessionInfoList_t* SessionInfoListNode_p = V_CSPSA_CORE_OpenedSessionsList_p;

    while (SessionInfoListNode_p != NULL)
    {
      if (memcmp(Config_p, &SessionInfoListNode_p->SessionInfo_p->Config, sizeof(*Config_p)) == 0)
      {
        // - We found a matching session. Return this.
        SessionInfoListNode_p->Counter++;
        *Handle_p = SessionInfoListNode_p->SessionInfo_p;
        CSPSA_SessionInfo_p = SessionInfoListNode_p->SessionInfo_p;
        Result = T_CSPSA_RESULT_OK;
        GetHeaderResult = T_CSPSA_RESULT_OK;
        goto ErrorExit;
      }
      SessionInfoListNode_p = SessionInfoListNode_p->Next_p;
    }
  }

  *Handle_p = 0;

  // - Allocate space for session info.
  CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) F_CSPSA_CORE_HeapMalloc(sizeof(*CSPSA_SessionInfo_p));
  memset(CSPSA_SessionInfo_p, 0, sizeof(*CSPSA_SessionInfo_p));
  CSPSA_SessionInfo_p->Config = *Config_p;
  *Handle_p = CSPSA_SessionInfo_p;

  // - Open flash memory device.
  Result = CSPSA_LL_Open(*Handle_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  LL_OpenCalled = true;
  // - Read CSPSA image from media.

  // - Check both headers and use the valid one with highest index. If the second header is broken
  //   F_CSPSA_CORE_StorageImage_GetValidHeader() will overwrite the invalid.
  GetHeaderResult = F_CSPSA_CORE_StorageImage_GetValidHeader(*Handle_p, &StorageImageHeader_p);
  if (GetHeaderResult >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  // - Keep the last valid header so we know where the last stored data was put.
  CSPSA_SessionInfo_p->LastStorageImageHeader_p = StorageImageHeader_p;

  // - Load data.
  Result = F_CSPSA_CORE_StorageImage_LoadData(*Handle_p, StorageImageHeader_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  // - Get fast parameters from flushed header
  if (StorageImageHeader_p != NULL)
  {
    Result = F_CSPSA_CORE_FastParameter_GetFromHeader(CSPSA_SessionInfo_p);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
  }

  // - Add session to list of opened sessions.
  {
    CSPSA_SessionInfoList_t* SessionInfoListNode_p = V_CSPSA_CORE_OpenedSessionsList_p;

    if (SessionInfoListNode_p == NULL)
    {
      // - First item in list.
      V_CSPSA_CORE_OpenedSessionsList_p = (CSPSA_SessionInfoList_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_SessionInfoList_t));
      V_CSPSA_CORE_OpenedSessionsList_p->SessionInfo_p = CSPSA_SessionInfo_p;
      V_CSPSA_CORE_OpenedSessionsList_p->Counter = 1;
      V_CSPSA_CORE_OpenedSessionsList_p->Next_p = NULL;
    }
    else
    {
      // - Flip to last item and append new session.
      while (SessionInfoListNode_p->Next_p != NULL)
      {
        SessionInfoListNode_p = SessionInfoListNode_p->Next_p;
      }
      SessionInfoListNode_p->Next_p = (CSPSA_SessionInfoList_t*) F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_SessionInfoList_t));
      SessionInfoListNode_p->Next_p->SessionInfo_p = CSPSA_SessionInfo_p;
      SessionInfoListNode_p->Next_p->Counter = 1;
      SessionInfoListNode_p->Next_p->Next_p = NULL;
    }
  }

ErrorExit:

  if (Result == T_CSPSA_RESULT_OK)
  {
    // - If F_CSPSA_CORE_StorageImage_GetValidHeader() returned a warning it must
    //   be propagated.
    if (GetHeaderResult > Result)
    {
      Result = GetHeaderResult;
    }
    CSPSA_SessionInfo_p->SessionCreated = false;
  }

  if (Result >= T_CSPSA_RESULT_ERRORS)
  {
    if (LL_OpenCalled)
    {
      CSPSA_SessionInfoList_t* SessionInfoListNode_p = V_CSPSA_CORE_OpenedSessionsList_p;

      // - Close flash memory device.
      (void) CSPSA_LL_Close(*Handle_p);
      F_CSPSA_CORE_ImageArray_Cleanup(*Handle_p);
      F_CSPSA_CORE_ItemList_Cleanup(*Handle_p);
      M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p->LastStorageImageHeader_p);
      if (V_CSPSA_CORE_OpenedSessionsList_p != NULL)
      {
        if (V_CSPSA_CORE_OpenedSessionsList_p->SessionInfo_p == CSPSA_SessionInfo_p)
        {
          V_CSPSA_CORE_OpenedSessionsList_p = V_CSPSA_CORE_OpenedSessionsList_p->Next_p;
          M_CSPSA_CORE_FREE(SessionInfoListNode_p);
        }
        else
        {
          CSPSA_SessionInfoList_t* TempSessionInfoListNode_p;

          while ((SessionInfoListNode_p->Next_p != NULL) && (SessionInfoListNode_p->Next_p->SessionInfo_p != CSPSA_SessionInfo_p))
          {
            SessionInfoListNode_p = SessionInfoListNode_p->Next_p;
          }
          if (SessionInfoListNode_p->Next_p != NULL)
          {
            TempSessionInfoListNode_p = SessionInfoListNode_p->Next_p;
            SessionInfoListNode_p->Next_p = SessionInfoListNode_p->Next_p->Next_p;
            M_CSPSA_CORE_FREE(TempSessionInfoListNode_p);
          }
        }
      }
      M_CSPSA_CORE_FREE(CSPSA_SessionInfo_p);
    }
    *Handle_p = 0;
  }

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_GetSizeOfFirstValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_DataItem_t* DataItem_p = NULL;
  CSPSA_Result_t Result;

  if ((Handle == NULL) || (Key_p == NULL) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }

  // - Try to find item with key 0 (lowest possible key).
  Result = F_CSPSA_CORE_Cache_GetDataItem(Handle, 0, &DataItem_p);
  if (Result != T_CSPSA_RESULT_OK)
  {
    // - Find item with first key higher than 0.
    Result = F_CSPSA_CORE_Cache_GetNextDataItem(Handle, 0, &DataItem_p);
  }

  if (Result == T_CSPSA_RESULT_OK)
  {
    *Key_p = DataItem_p->Key;
    *Size_p = DataItem_p->Size;
  }

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_GetSizeOfNextValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_DataItem_t* NextDataItem_p;
  CSPSA_Result_t Result;

  if ((Handle == NULL) || (Key_p == NULL) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  Result =  F_CSPSA_CORE_Cache_GetNextDataItem(Handle, *Key_p, &NextDataItem_p);
  if (Result == T_CSPSA_RESULT_OK)
  {
    *Key_p = NextDataItem_p->Key;
    *Size_p = NextDataItem_p->Size;
  }
  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_GetSizeOfValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_Size_t* const Size_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_DataItem_t* DataItem_p = NULL;

  if ((Handle == NULL) || (Size_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  // - Look in data item cache.
  if ( (CSPSA_SessionInfo_p->Cache.LastDataItem_p != NULL) && (CSPSA_SessionInfo_p->Cache.LastDataItem_p->Key == Key) )
  {
    *Size_p = CSPSA_SessionInfo_p->Cache.LastDataItem_p->Size;
    Result = T_CSPSA_RESULT_OK;
    goto ErrorExit;
  }

  Result = F_CSPSA_CORE_Cache_GetDataItem(Handle, Key, &DataItem_p);
  if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

  *Size_p = DataItem_p->Size;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_ReadValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_INVALID_KEY;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_DataItem_t* DataItem_p = NULL;
  uint32_t ChunkSize;

  if ((Handle == NULL) || (Data_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  // - Look in data item cache.
  if ( (CSPSA_SessionInfo_p->Cache.LastDataItem_p != NULL) && (CSPSA_SessionInfo_p->Cache.LastDataItem_p->Key == Key) )
  {
    DataItem_p = CSPSA_SessionInfo_p->Cache.LastDataItem_p;
    Result = T_CSPSA_RESULT_OK;
  }
  else
  if ( (CSPSA_SessionInfo_p->Cache.NextDataItem_p != NULL) && (CSPSA_SessionInfo_p->Cache.NextDataItem_p->Key == Key) )
  {
    DataItem_p = CSPSA_SessionInfo_p->Cache.NextDataItem_p;
    Result = T_CSPSA_RESULT_OK;
  }
  else
  {
    Result = F_CSPSA_CORE_Cache_GetDataItem(Handle, Key, &DataItem_p);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;
  }

  ChunkSize = (Size < DataItem_p->Size) ? Size : DataItem_p->Size;
  if (Size != DataItem_p->Size)
  {
    Result = T_CSPSA_RESULT_W_SIZE;
  }
  memcpy(Data_p, DataItem_p->Data_p, ChunkSize);

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_WriteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_UNDEFINED;
  CSPSA_SessionInfo_t* CSPSA_SessionInfo_p = (CSPSA_SessionInfo_t*) Handle;
  CSPSA_Cache_t* Cache_p;
  bool ItemFound = false;
  CSPSA_DataItem_t* DataItem_p;
  CSPSA_Image_t* ImageArray_p;
  uint32_t ImageArrayIndex = 0xFFFFFFFF;

  if ((Handle == NULL) || (Data_p == NULL))
  {
    return T_CSPSA_RESULT_E_BAD_PARAMETER;
  }
  Cache_p = &(CSPSA_SessionInfo_p->Cache);
  ImageArray_p = Cache_p->ImageArray_p;

  if ((CSPSA_SessionInfo_p->Config.Attributes & D_CSPSA_READ_ONLY) && !CSPSA_SessionInfo_p->SessionCreated)
  {
    Result = T_CSPSA_RESULT_E_READ_ONLY;
    goto ErrorExit;
  }

  // - First try to update fast parameter items in the list.
  if (Key >= D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY)
  {
    Result = F_CSPSA_CORE_FastParameter_Update(Handle, Key, Size, Data_p);
    // - Either the fast parameter item was successfully updated so we're done,
    //   or there was an error, regardless there is nothing more to do.
    goto ErrorExit;
  }

  // - Then try to update the item in the item list.
  Result = F_CSPSA_CORE_ItemList_Update(Handle, Key, Size, Data_p);
  if (Result < T_CSPSA_RESULT_ERRORS)
  {
    // - The item was in the item list and has been updated so we're done.
    goto ErrorExit;
  }

  // - Now see if item is in image array.
  if
  (
    (ImageArray_p != NULL)
    &&
    (
      (CSPSA_SessionInfo_p->Cache.NbrOfImages > 1)
      ||
      (CSPSA_SessionInfo_p->Cache.ImageArray_p[0].Level > 0)
    )
  )
  {
    uint32_t DataLeft;

    Result = F_CSPSA_CORE_ImageArray_GetArrayIndex(Handle, Key, &ImageArrayIndex);
    if (Result != T_CSPSA_RESULT_OK) goto ErrorExit;

    // - "ImageArrayIndex" is now pointing at the array where the item might be.
    DataItem_p = ImageArray_p[ImageArrayIndex].Data_p;
    DataLeft = ImageArray_p[ImageArrayIndex].Level;
    while (DataLeft > 0)
    {
      ItemFound = (DataItem_p->Key == Key);
      if (ItemFound)
      {
        // - If Size is unchanged, update array item.
        if (DataItem_p->Size == Size)
        {
          memcpy(DataItem_p->Data_p, Data_p, Size);
          Result = T_CSPSA_RESULT_OK;
          goto ErrorExit;
        }
        else
        {
          // - Size was changed, add item to item list.
          Result = F_CSPSA_CORE_ItemList_Add(Handle, Key, Size, Data_p);
          if (Result == T_CSPSA_RESULT_OK)
          {
            Result = T_CSPSA_RESULT_W_SIZE;
          }
          goto ErrorExit;
        }
        break;
      }
      else
      {
        if (DataItem_p->Key > Key)
        {
          // - The item wasn't in the array.
          break;
        }
        else
        {
          uint32_t Size32Aligned = M_CSPSA_CORE_ALIGN32(sizeof(*DataItem_p) + DataItem_p->Size);
          DataLeft -= Size32Aligned;
          DataItem_p = (CSPSA_DataItem_t*)(void*) ((uint8_t*) DataItem_p + Size32Aligned);
        }
      }
    }
  }

  // - It's a new item. Add it to item list.
  Result = F_CSPSA_CORE_ItemList_Add(Handle, Key, Size, Data_p);
  Result = T_CSPSA_RESULT_W_NEW_KEY;

ErrorExit:

  CSPSA_SessionInfo_p->Cache.CacheUpdated = true;

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_AddParameterArea(
  const CSPSA_Config_t* const Config_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  CSPSA_Config_t* PresentConfig_p = FindArea(Config_p->Name);
  CSPSA_ConfigList_t* NewConfig_p;

  if (PresentConfig_p != NULL)
  {
    //  - Storage area with same name has already been registered.
    Result = T_CSPSA_RESULT_E_ALREADY_EXISTS;
    goto ErrorExit;
  }

  // - Make copy of the Config.
  NewConfig_p = F_CSPSA_CORE_HeapMalloc(sizeof(CSPSA_ConfigList_t));
  if (NewConfig_p == NULL)
  {
    Result = T_CSPSA_RESULT_E_OUT_OF_MEMORY;
    goto ErrorExit;
  }
  memcpy(&NewConfig_p->Config, Config_p, sizeof(CSPSA_Config_t));

  if (Config_p->Extra_p != NULL)
  {
    // - Make a copy of the extra information.
    NewConfig_p->Config.Extra_p = F_CSPSA_CORE_HeapMalloc(Config_p->SizeOfExtra);
    if (NewConfig_p->Config.Extra_p == NULL)
    {
      M_CSPSA_CORE_FREE(NewConfig_p);
      Result = T_CSPSA_RESULT_E_OUT_OF_MEMORY;
      goto ErrorExit;
    }
    NewConfig_p->Config.SizeOfExtra = Config_p->SizeOfExtra;
    memcpy(NewConfig_p->Config.Extra_p, Config_p->Extra_p, Config_p->SizeOfExtra);
  }

  // - Add new config at the beginning of the list.
  NewConfig_p->Next_p = V_CSPSA_CORE_RegisteredAreaList_p;
  V_CSPSA_CORE_RegisteredAreaList_p = NewConfig_p;

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_RemoveParameterArea(
  const CSPSA_Config_t* const Config_p
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
  CSPSA_ConfigList_t* List_p;

  for (List_p = V_CSPSA_CORE_RegisteredAreaList_p; List_p != NULL; List_p = List_p->Next_p)
  {
    if (strcmp(List_p->Config.Name, Config_p->Name) == 0)
    {
      CSPSA_ConfigList_t* TempConfig_p = NULL;

      // - Found the entry.
      M_CSPSA_CORE_FREE(List_p->Config.Extra_p);
      if (V_CSPSA_CORE_RegisteredAreaList_p == List_p)
      {
        V_CSPSA_CORE_RegisteredAreaList_p = List_p->Next_p;
      }
      else
      {
        TempConfig_p = V_CSPSA_CORE_RegisteredAreaList_p;
        while ( (TempConfig_p != NULL) && (TempConfig_p->Next_p != List_p) )
        {
          TempConfig_p = TempConfig_p->Next_p;
        }
        if (TempConfig_p != NULL)
        {
          TempConfig_p->Next_p = List_p->Next_p;
        }
      }
      M_CSPSA_CORE_FREE(List_p);
      Result = T_CSPSA_RESULT_OK;
      goto ErrorExit;
    }
  }

ErrorExit:

  return Result;
}

// -----------------------------------------------------------------------------

uint32_t CSPSA_CORE_GetParameterAreaListLength(void)
{
  uint32_t Counter = 0;
  CSPSA_ConfigList_t* List_p;

  for (List_p = V_CSPSA_CORE_RegisteredAreaList_p; List_p != NULL; List_p = List_p->Next_p)
  {
    ++Counter;
  }
  return Counter;
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_CORE_GetParameterArea(uint32_t Index, CSPSA_Config_t* Config_p)
{
  uint32_t Counter = 0;
  CSPSA_ConfigList_t* List_p;

  for (List_p = V_CSPSA_CORE_RegisteredAreaList_p; List_p != NULL; List_p = List_p->Next_p)
  {
    if (Counter == Index)
    {
      // - Found the entry.
      *Config_p = List_p->Config;
      return T_CSPSA_RESULT_OK;
    }
    ++Counter;
  }
  // - Index is too high.
  return T_CSPSA_RESULT_E_BAD_PARAMETER;
}

// -----------------------------------------------------------------------------

/**
 * Initializes the given Config with default values.
 *
 * @param [in, out] The configuration to be initialized.
 */
void CSPSA_InitConfig(CSPSA_Config_t* const Config_p)
{
  Config_p->MemoryType = T_CSPSA_MEMORY_TYPE_UNDEFINED;
  Config_p->StartOffsetInBytes = 0;
  Config_p->SizeInBytes = 0;
  Config_p->SectorSize = 0;
  Config_p->BlockSize = 0;
  Config_p->Attributes = 0;
  Config_p->Extra_p = NULL;
  Config_p->SizeOfExtra = 0;
  Config_p->Name[0] = '\0';
}

// =============================================================================
//   End of file
// =============================================================================
