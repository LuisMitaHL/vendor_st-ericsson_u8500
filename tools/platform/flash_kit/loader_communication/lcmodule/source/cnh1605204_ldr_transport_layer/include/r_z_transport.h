/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_R_Z_TRANSPORT_LAYER_H
#define INCLUSION_GUARD_R_Z_TRANSPORT_LAYER_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup ldr_z_transport_layer Z Transport layer
 *      TI functionalities for sending Z packets, Initialization and
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
 * Initializes the transport layer for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 * @retval E_FAILED_TO_INIT_TL unsuccessful initialization.
 * @retval E_ALLOCATE_FAILED failed to allocate memory space.
 */
ErrorCode_e Z_Transport_Initialize(const Communication_t *const Communication_p);

/**
 * Shut Down the transport layer for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval E_SUCCESS  After successful execution.
 */
ErrorCode_e Z_Transport_Shutdown(const Communication_t *const Communication_p);

/**
 * Handles all registered TL processes for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Z_Transport_Poll(Communication_t *Communication_p);

/**
 * Function for sending packet in Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputData_p     Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 * buffer.
 */
ErrorCode_e Z_Transport_Send(Communication_t *Communication_p, void *InputData_p);

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_R_Z_TRANSPORT_LAYER_H

