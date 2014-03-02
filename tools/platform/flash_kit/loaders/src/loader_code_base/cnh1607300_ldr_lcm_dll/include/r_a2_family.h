/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_A2_FAMILY_H_
#define _INCLUSION_GUARD_R_A2_FAMILY_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_a2_protocol.h"
#include "error_codes.h"
#include "t_communication_service.h"
//#include "t_r15_network_layer.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define A2_TRANSPORT(x) (&((A2_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define A2_NETWORK(x) (&((A2_FamilyContext_t *) (x)->FamilyContext_p)->Network)

/**
 * A2 family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Family_Init(Communication_t *Communication_p);

/**
 * A2 family protocols sutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Family_Shutdown(Communication_t *Communication_p);


/** @} */
/** @} */
#endif //_INCLUSION_GUARD_R_A2_FAMILY_H_
