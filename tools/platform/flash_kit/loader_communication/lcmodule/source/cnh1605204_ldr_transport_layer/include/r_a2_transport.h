/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_R_A2_TRANSPORT_LAYER_H
#define INCLUSION_GUARD_R_A2_TRANSPORT_LAYER_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_transport_layer A2 Transport Layer
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Handles all registered TL processes.
 *
 * @param [in]  Communication_p The communication context
 *                              structure to receive information
 *                              about this module instance.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e A2_Transport_Poll(Communication_t *Communication_p);

/*
 * Function for sending packet.
 *
 * @param [in]  Communication_p The communication context
 *                              structure to receive information
 *                              about this module instance.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 * buffer.
 */
ErrorCode_e A2_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p);

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_R_A2_TRANSPORT_LAYER_H

