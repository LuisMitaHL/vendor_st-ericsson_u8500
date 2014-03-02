/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_R_PROTROM_TRANSPORT_LAYER_H
#define INCLUSION_GUARD_R_PROTROM_TRANSPORT_LAYER_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_transport_layer PROTROM Transport Layer
 *      PROTROM functionalities for sending PROTROM packets and
 *      polling receiver and transmitter.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include  "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Handles all registered TL processes for PROTROM protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Protrom_Transport_Poll(Communication_t *Communication_p);

/**
 * Function for sending packet in PROTROM protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e Protrom_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p);

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_R_PROTROM_TRANSPORT_LAYER_H

