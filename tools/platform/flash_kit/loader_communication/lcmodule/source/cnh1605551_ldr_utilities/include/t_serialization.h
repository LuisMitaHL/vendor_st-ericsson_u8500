/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_SERIALIZATION_H_
#define _T_SERIALIZATION_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup serialization
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** By default we use Little Endian ordering, this macro is declared to simplify
    the calling of the function. */
#define get_uint16 get_uint16_le
/** By default we use Little Endian ordering, this macro is declared to simplify
    the calling of the function. */
#define get_uint32 get_uint32_le
/** By default we use Little Endian ordering, this macro is declared to simplify
    the calling of the function. */
#define put_uint16 put_uint16_le
/** By default we use Little Endian ordering, this macro is declared to simplify
    the calling of the function. */
#define put_uint32 put_uint32_le

/** @} */
/** @} */
#endif /*T_SERIALIZATION_H_*/
