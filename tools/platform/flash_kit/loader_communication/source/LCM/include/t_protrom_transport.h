/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_T_PROTROM_TRANSPORT_H
#define INCLUSION_GUARD_T_PROTROM_TRANSPORT_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_protrom_header.h"

/*******************************************************************************
 * Types, constants and external variables
 ******************************************************************************/
/** Callback function type for the Protrom protocol handler. */
typedef ErrorCode_e(* Protrom_Callback_fn)(void *Param_p, uint8 PDU, int PayloadLength, void *Payload_p, Communication_t *Communication_p);

/** Structure for transfer input parameters in PROTROM protocol family */
typedef struct {
    Protrom_Header_t  *Header_p;  /**< Pointer to the PROTROM header structure.*/
    void              *Payload_p; /**< Pointer to the payload data.*/
} Protrom_SendData_LP_t;

/** Protorm Transport context */
typedef struct {
    /**< Callback function pointer for PROTROM protocol handler. */
    Protrom_Callback_fn Callback;
} Protrom_TransportContext_t;

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_T_PROTROM_TRANSPORT_H

