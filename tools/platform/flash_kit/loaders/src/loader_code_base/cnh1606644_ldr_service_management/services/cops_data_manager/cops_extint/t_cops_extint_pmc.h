/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef INCLUSION_GUARD_COPS_EXTINT_PMC_H
#define INCLUSION_GUARD_COPS_EXTINT_PMC_H

/**
 * @file  t_cops_extint_pmc.h
 * @brief Type and constant declarations for PMC
 *
 * @addtogroup ldr_service_management
 * @{
 *    @addtogroup ldr_service_cops COPS Data Manager
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cops_common.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 * ...
 */
/*TYPEDEF_ENUM //xvsvlpi: removed
{
  COPS_EXTINT_PMC_TYPE_NONE,
  COPS_EXTINT_PMC_TYPE_COPS_1
} SIGNED_ENUM8(COPS_ExtInt_PMC_Type_t);
*/

/**
 * COPS_ExtInt_PMC_Get_t
 *
 * Returns the current value of a Protected Monotonic Counter
 *
 * @param [IN]  PMC_Type    Indentifying the PMC to be used
 * @param [OUT] PMC_Value   The Current PMC value

 *
 *
 */

/*typedef COPS_RC_t(*COPS_ExtInt_PMC_Get_t)//xvsvlpi: removed
(
        COPS_ExtInt_PMC_Type_t        PMC_Type,
        uint32*                 const PMC_Value_p
);
*/
/**
 * COPS_ExtInt_PMC_Increment_t
 *
 * Increments the value of a Protected Monotonic Counter
 *
 * @param [IN]  PMC_Type    Indentifying the PMC to be used
 * @param [OUT] PMC_Value   The PMC value after increment
 *
 */

/*typedef COPS_RC_t(*COPS_ExtInt_PMC_Increment_t)//xvsvlpi: removed
(
        COPS_ExtInt_PMC_Type_t        PMC_Type,
        uint32*                 const PMC_Value_p
);
*/
/*typedef struct //xvsvlpi: removed
{
  COPS_ExtInt_PMC_Get_t            Do_Get_p;
  COPS_ExtInt_PMC_Increment_t      Do_Increment_p;
}COPS_ExtInt_PMC_t;
*/


/** @} */
/** @} */
#endif /*INCLUSION_GUARD_COPS_EXTINT_PMC_H*/
