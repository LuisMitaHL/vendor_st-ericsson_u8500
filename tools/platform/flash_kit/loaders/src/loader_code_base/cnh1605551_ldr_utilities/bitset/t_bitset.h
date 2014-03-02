/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_BITSET_H_
#define _T_BITSET_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup bitset
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/** Type definition for a unsigned 32 bit long Bitset. */
typedef uint32 BitSet32_t;

/** Macro that define maximum number of bits in specified BItset. */
#define BITCOUNT (8 * sizeof(BitSet32_t))
/** Macro that check if given bit position is inside Bitset boundaries. */
#define ISVALID(x) (((x) >= 0) && ((uint32)(x) < BITCOUNT))

/** @} */
/** @} */
#endif /*_T_BITSET_H_*/
