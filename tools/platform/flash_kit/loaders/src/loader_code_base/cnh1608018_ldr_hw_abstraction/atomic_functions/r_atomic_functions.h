/*******************************************************************************
 * $Copyright ST-Ericsson 2011 $
 ******************************************************************************/
#ifndef _R_ATOMIC_FUNCTIONS_H_
#define _R_ATOMIC_FUNCTIONS_H_
/**
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup atomic_functions
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Atomic compare-and-swap (CAS) operation
 *
 * Performs the following sequence atomically:
 * \code
 * Value = *Atomic_p;
 * if (Value == Expected)
 *   *Atomic_p = Desired;
 * return Value;
 * \endcode
 *
 * @param [in/out] Atomic_p Value to modify
 * @param [in] Expected The expected value of *Atomic_p.
 * @param [in] Desired The desired value of *Atomic_p after the call.
 *
 * @return The value of *Atomic_p immediately before the swap operation.
 */
uint32 Do_Atomic_CompareAndSwap(uint32 volatile *Atomic_p, uint32 Expected, uint32 Desired);

/** @} */
/** @} */
#endif /*_R_ATOMIC_FUNCTIONS_H_*/
