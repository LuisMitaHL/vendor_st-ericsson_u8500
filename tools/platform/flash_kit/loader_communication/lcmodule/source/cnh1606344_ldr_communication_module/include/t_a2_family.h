/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_A2_FAMILY_H_
#define _INCLUSION_GUARD_T_A2_FAMILY_H_
/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup A2_family
 *    @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_a2_transport.h"
#include "t_a2_network.h"
#include "t_a2_speedflash.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Structure with timeouts for A2 protocol. */
typedef struct {
    uint32 TACK;   /**< Time for network packet acknowledge. */
} A2_Timeouts_t;

/**
 * A2 family context.
 * Context structures for A2 transport layer and network layer.
 */
typedef struct {
    A2_TransportContext_t  Transport;
    A2_NetworkContext_t    Network;
    A2_SpeedflashContext_t Speedflash;
    A2_Timeouts_t          Timeouts;
} A2_FamilyContext_t;


/** @} */
/** @} */
#endif // _INCLUSION_GUARD_T_A2_FAMILY_H_
