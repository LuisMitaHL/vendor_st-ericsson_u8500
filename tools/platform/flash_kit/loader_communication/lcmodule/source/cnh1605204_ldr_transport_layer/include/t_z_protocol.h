/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_T_Z_PROTOCOL_H
#define INCLUSION_GUARD_T_Z_PROTOCOL_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup z_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_z_network.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Z protocol family context. */
typedef struct {
    Z_NetworkContext_t Network; /**< Network context.*/
} Z_FamilyContext_t;


/** Structure for transfer input parameters in Z protocol family */
typedef struct {
    uint8 *Data_p; /**< Pointer to the TI data.*/
    uint8  Size;   /**< Size of the data for transfer. */
} Z_SendingContent_t;

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_T_Z_PROTOCOL_H
