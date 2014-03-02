/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_PROTROM_FAMILY_H_
#define _INCLUSION_GUARD_R_PROTROM_FAMILY_H_
/**
 *  @addtogroup ldr_communication_module
 *  @{
 *    @addtogroup protrom_family Protrom Family
 *    The PROTROM family include only one protocol PROTROM used for
 *    communication between the ROM code and PC tool. This module include
 *    protrom transport layer and protrom network layer for handling protrom
 *    protocol.
 *
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_protrom_protocol.h"
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define PROTROM_TRANSPORT(x) (&((Protrom_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define PROTROM_NETWORK(x) (&((Protrom_FamilyContext_t *) (x)->FamilyContext_p)->Network)

/**
 * Protrom family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Protrom_Family_Init(Communication_t *Communication_p);

/**
 * Protrom family protocols sutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Protrom_Family_Shutdown(Communication_t *Communication_p);


/** @} */
/** @} */
#endif //_INCLUSION_GUARD_R_PROTROM_FAMILY_H_
