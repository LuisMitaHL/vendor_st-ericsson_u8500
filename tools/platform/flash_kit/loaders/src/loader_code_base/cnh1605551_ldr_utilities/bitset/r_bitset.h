/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_BITSET_H_
#define _R_BITSET_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup bitset
 *   @{
 *     This module implement functionalities for BitSet manipulations. It
 *     summarize most used operations for working with BitSet chains. It offer
 *     easy to use interface functions for common operations on Bitset chains
 *     like: set, reset, mask, unmask, compares to different Bitset chains and
 *     couple of other very usefull functions.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_bitset.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Create Bitset with all bit members marked as "clear".
 *
 * @return     Variable from BitSet32_t type initialized with "zeroes".
 */
BitSet32_t Do_BitSet32_EmptySet(void);

/**
 * @brief Create Bitset with all bit members marked as "set".
 *
 * @return     Variable from BitSet32_t type initialized with "ones".
 */
BitSet32_t Do_BitSet32_UniversalSet(void);

/**
 * @brief Set(activate) one bit in Bitset.
 *
 * @param [in, out] Set_p - points to Bitset variable which will be masked
 *                          with "one" on position determined in V parameter.
 * @param[in]           V - position on mask bit (starting from zero position).
 *
 * @return                  Function returns TRUE if bit is successfully set. In
 *                          case of demand for set bit that is already set or
 *                          exceeding Bitset boundaries, function return FALSE.
 */
boolean Do_BitSet32_Insert(BitSet32_t *Set_p,
                           int V);

/**
 * @brief Clear(reset) one bit in Bitset.
 *
 * @param [in, out] Set_p - points to Bitset variable which will be masked
 *                          with "zero" on position determined in V parameter.
 * @param[in]           V - position on mask bit (starting from zero position).
 *
 * @return                  Function returns TRUE if bit is successfully
 *                          cleared. In case of demand for clearing bit that is
 *                          already clear or exceeding Bitset boundaries
 *                          function return FALSE.
 */
boolean Do_BitSet32_Remove(BitSet32_t *Set_p,
                           int V);

/**
 * @brief Check status of particular bit in Bitset.
 *
 * @param [in, out]   Set - points to Bitset variable.
 * @param[in]           V - position of particular bit in BitSet (starting from
 *                          zero position).
 *
 * @return                  Function returns TRUE if that particular bit is set
 *                          (active) or FALSE if bit is clear. In case of bit
 *                          that overlapping Bitset boundaries function return
 *                          FALSE.
 */
boolean Do_BitSet32_Contains(BitSet32_t Set,
                             int V);

/**
 * @brief Bitwise OR of two different Bitset variables.
 *
 * @param [in] Set1 - first Bitset variable that will be OR'ed.
 * @param [in] Set2 - second Bitset variable that will be OR'ed.
 *
 * @return     Function return Bitset variable that represent union of
 *             inserted Bitsets.
 */
BitSet32_t Do_BitSet32_Union(BitSet32_t Set1,
                             BitSet32_t Set2);

/**
 * @brief Bitwise AND of two different Bitset variables.
 *
 * @param [in] Set1 - first Bitset variable that will be AND'ed.
 * @param [in] Set2 - second Bitset variable that will be AND'ed.
 *
 * @return     Function return Bitset variable that represent intersection of
 *             inserted Bitsets.
 */
BitSet32_t Do_BitSet32_Intersection(BitSet32_t Set1,
                                    BitSet32_t Set2);

/**
 * @brief Check which bits in two Bitsets are different.
 *
 * @param [in] Set1 - first Bitset variable.
 * @param [in] Set2 - second Bitset variable.
 *
 * @return     Function return Bitset variable which hold information about
 *             different bits in both inserted Bitsets.
 */
BitSet32_t Do_BitSet32_Difference(BitSet32_t Set1,
                                  BitSet32_t Set2);

/**
 * @brief Get next bit in Bitset chain that is set, starting from Last.
 *
 * @param [in] Set  - Bitset variable.
 * @param [in] Last - position of starting bit for searching.
 *
 * @return     Next bit from Bitset that is set, starting from Last.
 *
 * @retval     -1 - if Last exceed Bitset boundaries or if don't exist bit in
 *                  Bitset chain that is set(active) after Last bit.
 */
int Do_BitSet32_GetNext(BitSet32_t Set,
                        int Last);

/**
 * @brief Get previous bit in Bitset chain that is set, starting from Last.
 *
 * @param [in] Set  - Bitset variable.
 * @param [in] Last - position of starting bit for searching.
 *
 * @return     Previous bit from Bitset that is set, starting from Last.
 *
 * @retval     -1 - if Last exceed Bitset boundaries or if don't exist bit in
 *                  Bitset chain that is set(active) before Last bit.
 */
int Do_BitSet32_GetPrevious(BitSet32_t Set,
                            int Last);

/**
 * @brief Print out bits from one Bitset chain with their respective names.
 *
 * @param [in] BitSet - Bitset variable.
 * @param [in] Names[] - array of pointers that point to names for bits in
 *                       Bitset.
 * @param [in] NrOfNames - determine number of members in Names[] array.
 *
 * @return     None.
 */
void Do_BitSet32_Print(BitSet32_t BitSet,
                       char *Names[],
                       int NrOfNames);

/** @} */
/** @} */
#endif /*_R_BITSET_H_*/
