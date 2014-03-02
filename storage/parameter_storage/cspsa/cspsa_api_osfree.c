/* **************************************************************************
 *
 * cspsa_api_osfree.c
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
 * OS-free API for Crash Safe Parameter Storage Area (CSPSA). See 
 * file "cspsa_ds.html".
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include <stdint.h>
#include "cspsa.h"
#include "cspsa_core.h"

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

// - Normally there should be no global variables for CSPSA. Everything should
//   be stored in CSPSA_SessionInfo_t.

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

CSPSA_Result_t CSPSA_Close(CSPSA_Handle_t* const Handle_p)
{
  return CSPSA_CORE_Close(Handle_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Create(
  const char* const ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  return CSPSA_CORE_Create(ParameterAreaName_p, Handle_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_DeleteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key
)
{
  return CSPSA_CORE_DeleteValue(Handle, Key);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Flush(const CSPSA_Handle_t Handle)
{
  return CSPSA_CORE_Flush(Handle);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Format(const CSPSA_Handle_t Handle)
{
  return CSPSA_CORE_Format(Handle);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_Open(
  const char* ParameterAreaName_p,
  CSPSA_Handle_t* const Handle_p
)
{
  return CSPSA_CORE_Open(ParameterAreaName_p, Handle_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfFirstValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  return CSPSA_CORE_GetSizeOfFirstValue(Handle, Key_p, Size_p);
}
// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfNextValue(
  const CSPSA_Handle_t Handle,
  CSPSA_Key_t* const Key_p,
  CSPSA_Size_t* const Size_p
)
{
  return CSPSA_CORE_GetSizeOfNextValue(Handle, Key_p, Size_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetSizeOfValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  CSPSA_Size_t* const Size_p
)
{
  return CSPSA_CORE_GetSizeOfValue(Handle, Key, Size_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_ReadValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  CSPSA_Data_t* const Data_p
)
{
  return CSPSA_CORE_ReadValue(Handle, Key, Size, Data_p);
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_WriteValue(
  const CSPSA_Handle_t Handle,
  const CSPSA_Key_t Key,
  const CSPSA_Size_t Size,
  const CSPSA_Data_t* const Data_p
)
{
  return CSPSA_CORE_WriteValue(Handle, Key, Size, Data_p);
}

// -----------------------------------------------------------------------------

uint32_t CSPSA_GetParameterAreaListLength(void)
{
  return CSPSA_CORE_GetParameterAreaListLength();
}

// -----------------------------------------------------------------------------

CSPSA_Result_t CSPSA_GetParameterArea(uint32_t Index, CSPSA_Config_t* Config_p)
{
  return CSPSA_CORE_GetParameterArea(Index, Config_p);
}

// =============================================================================
//   End of file
// =============================================================================
