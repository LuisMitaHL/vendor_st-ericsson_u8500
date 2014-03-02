/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_PROTROM_PROTOCOL_H_
#define _INCLUSION_GUARD_R_PROTROM_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup protrom_protocol PROTROM protocol
 *      Functionalities for handling PROTROM commands.
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
 * Decode received command, and execute.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval E_SUCCESS           After successful execution.
 */
ErrorCode_e Protrom_Process(const Communication_t *const Communication_p, Protrom_Packet_t *Packet_p);

/** @} */
/** @} */
/** @} */
#endif //_INCLUSION_GUARD_R_PROTROM_PROTOCOL_H_
