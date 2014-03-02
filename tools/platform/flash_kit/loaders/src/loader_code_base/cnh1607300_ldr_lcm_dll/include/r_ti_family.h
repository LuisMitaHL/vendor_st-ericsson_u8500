/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_TI_FAMILY_H_
#define _INCLUSION_GUARD_R_TI_FAMILY_H_
/**
 *  @addtogroup ldr_communication_module
 *  @{
 *    @addtogroup TI_family TI protocol family
 *    The TI family include only one protocol TI used for
 *    communication between the ROM code and PC tool. This module include
 *    TI transport layer and TI network layer for handling TI protocol.
 *
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_ti_protocol.h"
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define TI_TRANSPORT(x) (&((TI_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define TI_NETWORK(x) (&((TI_FamilyContext_t *) (x)->FamilyContext_p)->Network)

/**
 * TI family protocols initialization.
 *
 * Will be initialized TI transport and network layer.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e TI_Family_Init(Communication_t *Communication_p);

/**
 * TI family protocols sutdown.
 *
 * The TI transport and network layer will be shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e TI_Family_Shutdown(Communication_t *Communication_p);


/** @} */
/** @} */
#endif //_INCLUSION_GUARD_R_TI_FAMILY_H_
