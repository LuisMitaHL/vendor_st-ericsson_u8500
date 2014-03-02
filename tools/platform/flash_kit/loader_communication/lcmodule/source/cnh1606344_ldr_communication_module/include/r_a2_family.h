/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_A2_FAMILY_H_
#define _INCLUSION_GUARD_R_A2_FAMILY_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family A2 protocol family
 *    The A2 family include only one protocol, A2 protocol, used for
 *    communication between the ME and PC tool. This module includes
 *    A2 transport layer and A2 network layer for handling A2 protocol.
 *
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_a2_family.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define A2_TRANSPORT(x) (&((A2_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define A2_NETWORK(x) (&((A2_FamilyContext_t *) (x)->FamilyContext_p)->Network)
#define A2_SPEEDFLASH(x) (((volatile A2_SpeedflashContext_t*)&((A2_FamilyContext_t *) (x)->FamilyContext_p)->Speedflash))

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
 * A2 family protocols shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Family_Shutdown(Communication_t *Communication_p);

/*
 * A2 Cancel Receiver.
 *
 * @param [in] Communication_p           Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be
 *                                       transmitted before stopping the
 *                                       transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeTransferStop);

/** @} */
/** @} */
#endif //_INCLUSION_GUARD_R_A2_FAMILY_H_
