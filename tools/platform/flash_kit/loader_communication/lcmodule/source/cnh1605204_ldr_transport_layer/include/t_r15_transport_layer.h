/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_R15_TRANSPORT_LAYER_H_
#define _INCLUSION_GUARD_T_R15_TRANSPORT_LAYER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_time_utilities.h"
#include "t_r15_network_layer.h"
#include "t_bulk_protocol.h"

/*******************************************************************************
 * Types, constants and external variables
 ******************************************************************************/

/** Structure for transfer input parameters in R15 protocol family. */
typedef struct {
    R15_Header_t          *Header_p;         /**< Pointer to R15 header data. */
    void                  *ExtendedHeader_p; /**< Pointer to Extended header data.*/
    void                  *Payload_p;        /**< Pointer to payload data.*/
    uint32                Time;              /**< Used time for retransmission.*/
    HandleFunction_t      TimerCallBackFn_p; /**< Timer call back function for
                                                  retransmission.*/
} SendData_LP_t;

/** R15 Transport context. */
typedef struct {
    /** Session/State for Incoming packet. */
    uint16                SessionStateIn;
    /** Session/State for Outgoing packet. */
    uint16                SessionStateOut;
    /** Bulk Session counter. */
    uint16                BulkSessionCounter;
    /** Bulk Vector List. */
    TL_BulkVectorList_t   BulkVectorList[MAX_BULK_TL_PROCESSES];
    /** Bulk handle for the Current bulk transfer. */
    BulkHandle_t          BulkHandle;
    /** Bulk vector for previous current bulk transfer. */
    TL_BulkVectorList_t   PreviousBulkVector;
    /** Callback function pointer for bulk command handling.*/
    void                  *BulkCommandCallback_p;
    /** Callback function pointer for bulk data command handling.*/
    void                  *BulkDataCallback_p;
    /** Callback function pointer for handling end of bulk transfer.*/
    void                  *EndOfDump_p;
    /** Callback function pointer for handling releasing of bulk buffers.*/
    void                  *BulkBufferRelease_p;
} R15_TransportContext_t;

/** @} */
/** @} */
/** @} */
#endif // _INCLUSION_GUARD_T_R15_TRANSPORT_LAYER_H_

