/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_R_Z_PROTOCOL_H
#define INCLUSION_GUARD_R_Z_PROTOCOL_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup z_protocol Z Protocol
 *      Functionalities for handling Z commands. All Z commands are not decoded
 *      in this layer, it is resent to the application layer for decoding
 *      and executing.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_z_protocol.h"
#include "error_codes.h"
#include "t_communication_service.h"
#include "t_z_network.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Decode received command, and execute.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval E_SUCCESS           After successful execution.
 */
ErrorCode_e Z_Family_Process(const Communication_t *const Communication_p, const void *const Packet_p);

/** @} */
/** @} */
/** @} */
#endif //INCLUSION_GUARD_R_Z_PROTOCOL_H
