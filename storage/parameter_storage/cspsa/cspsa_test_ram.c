/* **************************************************************************
 *
 * cspsa_test_ram.c
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
 * Test driver for RAM with ability to generate read/write errors.
 *
 **************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cspsa.h"
#include "cspsa_ll.h"
#include "cspsa_plugin.h"
#include "cspsa_test.h"

#define AREA_NAME "TEST_RAM_CSPSA"
static CSPSA_Config_t TestConfig;

static CSPSA_TEST_InducedError_t* InducedMediaError_p = NULL;

static void F_CSPSA_RAM_TEST_Init(void);
static CSPSA_Result_t F_CSPSA_RAM_TEST_Open(const CSPSA_Config_t* Config_p, void** Handle_pp);
static CSPSA_Result_t F_CSPSA_RAM_TEST_Close(void* Handle_p);
static CSPSA_Result_t F_CSPSA_RAM_TEST_Read(
    void* Handle_p,
    const uint64_t Offset,
    void* const Data_p,
    const uint32_t Size
  );
static CSPSA_Result_t F_CSPSA_RAM_TEST_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
);
static CSPSA_Result_t F_CSPSA_RAM_TEST_Format(void* Handle_p);
static CSPSA_Result_t F_CSPSA_RAM_TEST_Erase(
  void* Handle_p,
  const uint64_t Offset
);

/**
 * Implementation of the CSPSA plugin operations
 */
static const CSPSA_PluginOperations_t PLUGIN_IMPL =
{
  F_CSPSA_RAM_TEST_Open,
  F_CSPSA_RAM_TEST_Close,
  F_CSPSA_RAM_TEST_Read,
  F_CSPSA_RAM_TEST_Write,
  F_CSPSA_RAM_TEST_Format,
  F_CSPSA_RAM_TEST_Erase
};

// =============================================================================
//  Local Function Declarations
// =============================================================================


/** Registers the RAM test plugin to CSPSA. */
static void F_CSPSA_RAM_TEST_Init(void)
{
  CSPSA_Result_t Result;

  Result = CSPSA_RegisterPlugin(T_CSPSA_MEMORY_TYPE_RAM, &PLUGIN_IMPL);
  (void)CSPSA_AddParameterArea(&TestConfig);
}


// =============================================================================
//   Global Function Definitions
// =============================================================================


bool CSPSA_TEST_CanInduceMediaErrors(void)
{
  (void)CSPSA_TEST_GetConfig();
  return true;
}

void CSPSA_TEST_RAM_Close()
{
  CSPSA_RemoveParameterArea(&TestConfig);
  free((uint32_t*)(ptrdiff_t)TestConfig.StartOffsetInBytes);
  TestConfig.StartOffsetInBytes = 0;
}

void CSPSA_TEST_InduceError(CSPSA_TEST_InducedError_t* MediaError_p)
{
  InducedMediaError_p = MediaError_p;
}

CSPSA_Config_t* CSPSA_TEST_GetConfig(void)
{
  static bool AreaAdded = false;
  uint32_t DeviceSize = 256*1024;

  CSPSA_InitConfig(&TestConfig);
  TestConfig.StartOffsetInBytes = (uint64_t)(ptrdiff_t) malloc(DeviceSize); // - Double casting to supress warning.
  TestConfig.SizeInBytes = DeviceSize;
  TestConfig.SectorSize = 512;
  TestConfig.BlockSize = 1024;
  TestConfig.MemoryType = T_CSPSA_MEMORY_TYPE_RAM;
  strcpy(TestConfig.Name, AREA_NAME);
  if (!AreaAdded)
  {
    F_CSPSA_RAM_TEST_Init();
    AreaAdded = true;
  }
  return &TestConfig;
}

static bool ContainsCorruptOffset(const CSPSA_Config_t* Config_p, uint64_t Offset, uint32_t Size, uint64_t* CorruptOffset_p)
{
  bool IsCorrupt = false;
  if (InducedMediaError_p != NULL)
  {
    if (Offset <= InducedMediaError_p->Offset && InducedMediaError_p->Offset < Offset + Size)
    {
      IsCorrupt = true;
      *CorruptOffset_p = InducedMediaError_p->Offset;
    }
    else if (InducedMediaError_p->AtBothEnds)
    {
      uint64_t OffsetAtOtherEnd = Config_p->SizeInBytes - InducedMediaError_p->Offset - 1;

      if (Offset <= OffsetAtOtherEnd && OffsetAtOtherEnd < Offset + Size)
      {
        IsCorrupt = true;
        *CorruptOffset_p = OffsetAtOtherEnd;
      }
    }
  }
  //printf("ContainsCorruptOffset offset %llx, induced offset %llx -> %d\n", Offset, InducedMediaError_p->Offset, IsCorrupt);
  return IsCorrupt;
}
// =============================================================================
//   Plugin implementation
// =============================================================================


static CSPSA_Result_t F_CSPSA_RAM_TEST_Open(const CSPSA_Config_t* Config_p, void** Handle_pp)
{
  *Handle_pp = (void*)Config_p;
  return T_CSPSA_RESULT_OK;
}

static CSPSA_Result_t F_CSPSA_RAM_TEST_Close(void* Handle_p)
{
  (void)(Handle_p);
  return T_CSPSA_RESULT_OK;
}

// -----------------------------------------------------------------------------

/** Reads data from storage media. */
static CSPSA_Result_t F_CSPSA_RAM_TEST_Read(
  void* Handle_p,
  const uint64_t Offset,
  void* const Data_p,
  const uint32_t Size
)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  uint32_t BlockSize;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

  BlockSize = Config_p->BlockSize;
  if (Offset32 + Size > Config_p->SizeInBytes || Size > BlockSize)
  {
    Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
    goto ErrorExit;
  }
  // check if a read error was induced
  if (InducedMediaError_p != NULL && InducedMediaError_p->MediaError == T_CSPSA_MEDIA_ERROR_READ_FAILED)
  {
    uint64_t CorruptOffset;
    if (ContainsCorruptOffset(Config_p, Offset, Size, &CorruptOffset))
    {
      // The offset matches the induced read error; generate read error
      //printf("Read: induced read error at offset 0x%x\n", (ptrdiff_t)Offset32);
      Result = T_CSPSA_RESULT_E_READ_ERROR;
      goto ErrorExit;
    }
  }
  {
    uint8_t* Data8_p = (uint8_t*)Data_p;
    uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;

    memcpy(Data8_p, (uint8_t*)(ptrdiff_t)CurrentOffset, Size);
    // check the read data should be corrupted due to an induced read error
    if (InducedMediaError_p != NULL && InducedMediaError_p->MediaError == T_CSPSA_MEDIA_ERROR_READ_CORRUPT_DATA)
    {
      uint64_t CorruptOffset;
      if (ContainsCorruptOffset(Config_p, Offset, Size, &CorruptOffset))
      {
        // induce an undetected bit error
        uint32_t OffsetInBlock = (uint32_t)(CorruptOffset - Offset);
        uint8_t* CorruptByte_p = Data8_p + OffsetInBlock;
        //printf("Read: induced corrupt byte at offset %lld, byte before was 0x%x, xor mask = 0x%x\n",
        //    CorruptOffset, *CorruptByte_p, InducedMediaError_p->XorMask);
        *CorruptByte_p ^= InducedMediaError_p->XorMask;
        //printf("Read: byte after is 0x%x\n", *CorruptByte_p);
      }
    }
  }
ErrorExit:
  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_RAM_Format: Formats (erases) media.

static CSPSA_Result_t F_CSPSA_RAM_TEST_Format(void* Handle_p)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;

  memset((uint8_t*)(ptrdiff_t)Config_p->StartOffsetInBytes, 0xFF, Config_p->SizeInBytes);

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// -----------------------------------------------------------------------------

// - CSPSA_LL_RAM_Write: Writes to storage media.

static CSPSA_Result_t F_CSPSA_RAM_TEST_Write(
  void* Handle_p,
  const uint64_t Offset,
  const void* const Data_p,
  const uint32_t Size
)
{
  const CSPSA_Config_t* Config_p = Handle_p;
  CSPSA_Result_t Result = T_CSPSA_RESULT_OK;
  uint32_t BlockSize;
  uint64_t Offset32 = Offset & 0xFFFFFFFF; // to fix mysterious bug that Offset is sometimes is more than 4G...

  BlockSize = Config_p->BlockSize;

  if (Offset32 + Size > Config_p->SizeInBytes || Size > BlockSize)
  {
    Result = T_CSPSA_RESULT_E_BAD_PARAMETER;
    goto ErrorExit;
  }
  {
    uint64_t CurrentOffset = Config_p->StartOffsetInBytes + Offset32;
    uint8_t* Data8_p = (uint8_t*)Data_p;

    memcpy((uint8_t*)(ptrdiff_t)CurrentOffset, Data8_p, Size);
  }
ErrorExit:
  return Result;
}



void CSPSA_RAM_TEST_Execute()
{
  F_CSPSA_RAM_TEST_Init();
  CSPSA_TEST_Execute();
}

// -----------------------------------------------------------------------------

// - F_CSPSA_LL_RAM_Erase: Erases part of storage media.
static CSPSA_Result_t F_CSPSA_RAM_TEST_Erase(
  void* Handle_p,
  const uint64_t Offset
)
{
  CSPSA_Result_t Result = T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
  const CSPSA_Config_t* Config_p = Handle_p;

  memset(
    (uint8_t*)(ptrdiff_t)Config_p->StartOffsetInBytes + Offset,
    0xFF,
    Config_p->BlockSize
  );

  Result = T_CSPSA_RESULT_OK;

  return Result;
}

// ================================================================================
//   End of file
// =============================================================================
