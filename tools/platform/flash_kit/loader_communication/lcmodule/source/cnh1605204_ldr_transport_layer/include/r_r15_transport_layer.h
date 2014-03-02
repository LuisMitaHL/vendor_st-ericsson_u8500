/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_R15_TRANSPORT_LAYER_H_
#define _INCLUSION_GUARD_R_R15_TRANSPORT_LAYER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_transport_layer R15 Transport layer
 *      R15 functionalities for sending R15 packets and
 *      polling receiver and transmitter.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_service.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Initializes the transport layer for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval E_SUCCESS           After successful execution.
 * @retval E_FAILED_TO_INIT_TL Unsuccessful initialization.
 * @retval E_ALLOCATE_FAILED   Failed to allocate memory space.
 */
ErrorCode_e R15_Transport_Initialize(const Communication_t *const Communication_p);

/**
 * Shut Down the transport layer for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.

 * @retval E_SUCCESS  After successful execution.
 */
ErrorCode_e R15_Transport_Shutdown(const Communication_t *const Communication_p);

/**
 * Handles all registered TL processes for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e R15_Transport_Poll(Communication_t *Communication_p);

/**
 * Function for sending packet in R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e R15_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p);

/**
 * Function for setting the timeouts in the R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  TimeoutData_p   Pointer to the input data with all timeouts.
 *
 * @retval E_SUCCESS            After successful execution.
 */
ErrorCode_e R15_SetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p);

/**
 * Function for getting the timeouts in the R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [out] TimeoutData_p   Return all defined timeouts.
 *
 * @retval E_SUCCESS            After successful execution.
 */
ErrorCode_e R15_GetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p);


/** @} */
/** @} */
/** @} */
#endif // _INCLUSION_GUARD_R_R15_TRANSPORT_LAYER_H_

