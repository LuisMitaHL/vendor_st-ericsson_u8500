/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_R15_FAMILY_H_
#define _INCLUSION_GUARD_T_R15_FAMILY_H_
/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup R15_family
 *    @{
 *
 */


/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#include "t_r15_transport_layer.h"
#include "t_r15_network_layer.h"

/** Strucure with timeouts for R15 protocols. */
typedef struct {
    uint32 TCACK ;  /**< Time for command packet acknowledge. */
    uint32 TBCR;    /**< Time for bulk command packet to be recieved.*/
    uint32 TBES;    /**< Time for bulk session end.  */
    uint32 TBDR;    /**< Time for bulk data packet to be received. */
} R15_Timeouts_t;

/**
 * R15 family context.
 * Context structures for R15 transport layer and network layer.
 */
typedef struct {
    R15_TransportContext_t Transport;
    R15_NetworkContext_t Network;
    R15_Timeouts_t Timeouts;
} R15_FamilyContext_t;


/** @} */
/** @} */
#endif // _INCLUSION_GUARD_T_R15_FAMILY_H_

