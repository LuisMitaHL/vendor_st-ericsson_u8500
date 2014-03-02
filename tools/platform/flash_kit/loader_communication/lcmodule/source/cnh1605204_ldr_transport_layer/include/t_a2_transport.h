/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef INCLUSION_GUARD_T_A2_TRANSPORT_H
#define INCLUSION_GUARD_T_A2_TRANSPORT_H
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_a2_network.h"

/*******************************************************************************
 * Types, constants and external variables
 ******************************************************************************/
/** Callback function type for the A2 protocol handler */
typedef ErrorCode_e(* A2_Callback_fn)(void *Param_p, uint8 PDU, int PayloadLength, void *Payload_p, Communication_t *Communication_p);

/** Structure for transfer input parameters in "loader protocols" protocol family */
typedef struct {
    A2_Header_t      *Header_p;         /**< Pointer to A2 header data. */
    void             *Payload_p;        /**< Pointer to payload data.*/
    uint32           Time;              /**< Used time for retransmission.*/
    HandleFunction_t TimerCallBackFn_p; /**< Timer callback function for
                                           retransmission.*/
} A2_SendData_LP_t;

/** A2 Transport context */
typedef struct {
    A2_Callback_fn Callback;
} A2_TransportContext_t;

/** @} */
/** @} */
/** @} */
#endif // INCLUSION_GUARD_T_A2_TRANSPORT_H

