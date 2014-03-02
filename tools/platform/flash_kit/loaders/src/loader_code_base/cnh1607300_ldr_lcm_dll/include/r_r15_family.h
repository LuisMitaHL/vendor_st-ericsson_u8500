/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_R15_FAMILY_H_
#define _INCLUSION_GUARD_R_R15_FAMILY_H_
/**
 * @addtogroup ldr_communication_module
 * @{
 *    @addtogroup R15_family
 *    The R15 protocol family include two protocols COMMAND and BULK. Command
 *    protocol is used for sending commands between the ME and PC tool in both
 *    direction. This module include R15 transport layer and R15 network layer
 *    for handling R15 prtocols.
 *
 *    @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_r15_family.h"
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define R15_TRANSPORT(x) (&((R15_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define R15_NETWORK(x) (&((R15_FamilyContext_t *) (x)->FamilyContext_p)->Network)
#define R15_TIMEOUTS(x) (&((R15_FamilyContext_t *) (x)->FamilyContext_p)->Timeouts)

/**
 * R15 family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Family_Init(Communication_t *Communication_p);

/**
 * R15 family protocols sutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Family_Shutdown(Communication_t *Communication_p);

/**     @} */
/** @} */
#endif // _INCLUSION_GUARD_R_R15_FAMILY_H_

