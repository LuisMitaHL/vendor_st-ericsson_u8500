/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_R_Z_FAMILY_H
#define INCLUSION_GUARD_R_Z_FAMILY_H
/**
 *  @addtogroup ldr_communication_module
 *  @{
 *    @addtogroup Z_family Z protocol family
 *    The Z family include only one protocol, Z protocol, used for
 *    communication between the ROM code and PC tool. This module include
 *    Z transport layer and Z network layer for handling Z protocol. The Z
 *    network layer receive and send byte by byte because the format of the
 *    Z protocol do not have one strict format.
 *
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_z_protocol.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define Z_TRANSPORT(x) (&((Z_FamilyContext_t *) (x)->FamilyContext_p)->Transport)
#define Z_NETWORK(x) (&((Z_FamilyContext_t *) (x)->FamilyContext_p)->Network)

/**
 * Z family protocols initialization.
 *
 * Z transport and Z network layer will be initialized.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_Family_Init(Communication_t *Communication_p);

/**
 * Z family protocols shutdown.
 *
 * Z transport and Z network layer will be shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_Family_Shutdown(Communication_t *Communication_p);

/*
 * Z family Cancel Transmission.
 *
 * @param [in] Communication_p           Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be
 *                                       transmitted before stopping the
 *                                       transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeTransferStop);

/**     @} */
/** @} */
#endif //INCLUSION_GUARD_R_Z_FAMILY_H
