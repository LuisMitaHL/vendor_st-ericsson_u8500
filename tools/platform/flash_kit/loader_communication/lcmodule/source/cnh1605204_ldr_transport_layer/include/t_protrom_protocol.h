/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_T_PROTROM_PROTOCOL_H
#define INCLUSION_GUARD_T_PROTROM_PROTOCOL_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup protrom_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_protrom_network.h"
#include "t_protrom_transport.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Protrom Family context. */
typedef struct {
    Protrom_TransportContext_t  Transport;
    Protrom_NetworkContext_t    Network;
} Protrom_FamilyContext_t;

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_T_PROTROM_PROTOCOL_H
