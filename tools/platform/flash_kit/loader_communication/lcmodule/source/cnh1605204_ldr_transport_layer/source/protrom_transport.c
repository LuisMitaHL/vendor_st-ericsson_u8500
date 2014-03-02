/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "c_system_v2.h"
#include "t_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_protrom_family.h"
#include "r_protrom_header.h"
#include "r_protrom_transport.h"
#include "r_protrom_network.h"
#include "r_protrom_protocol.h"
#include "r_communication_service.h"
#include "t_security_algorithms.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void Protrom_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Handles all registered TL processes for PROTROM protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Protrom_Transport_Poll(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    Protrom_Packet_t *Packet_p = NULL;

    Protrom_Network_ReceiverHandler(Communication_p);
    Packet_p = (Protrom_Packet_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p);

    if (NULL == Packet_p) {
        return E_ALLOCATE_FAILED;
    }

    ReturnValue = Protrom_Process(Communication_p, Packet_p);

    return ReturnValue;
}

/*
 * Function for sending packet in PROTROM protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e Protrom_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p)
{
    Protrom_Packet_t *Packet_p = NULL;
    Protrom_SendData_LP_t *InputData_p = (Protrom_SendData_LP_t *)InputDataIn_p;

    Packet_p = (Protrom_Packet_t *) malloc(sizeof(Protrom_Packet_t));

    if (NULL == Packet_p) {
        return E_ALLOCATE_FAILED;
    }

    Packet_p->Header = *InputData_p->Header_p;
    Packet_p->Buffer_p = (uint8 *)malloc(Packet_p->Header.PayloadLength + PROTROM_HEADER_LENGTH + PROTROM_CRC_LENGTH);

    if (NULL == Packet_p->Buffer_p) {
        BUFFER_FREE(Packet_p);
        return E_ALLOCATE_FAILED;
    }

    Protrom_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

    if (NULL != InputData_p->Payload_p) {
        /* setup payload for calculation */
        memcpy(Packet_p->Buffer_p + PROTROM_HEADER_LENGTH, InputData_p->Payload_p, Packet_p->Header.PayloadLength);

        /* Calculate Payload CRC */
        Packet_p->Communication_p = Communication_p;

        Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p), Communication_p->CurrentFamilyHash,
                Packet_p->Buffer_p, Packet_p->Header.PayloadLength + PROTROM_HEADER_LENGTH,
                (uint8 *)&Packet_p->CRC, (HashCallback_t)Protrom_Transport_OutHashCallback,
                (void *) Packet_p);
    } else {
        //@todo error no payload
    }

    /* The packet's buffer and the packet resources are
     * freed in the send callback function */

    /* coverity[leaked_storage] */
    return E_SUCCESS;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * Callback function for handling calculated data.
 *
 * @param [in]   Data_p   Pointer to the data buffer for calculation.
 * @param [in]   Length   Length of the data for calculation.
 * @param [out]  Hash_p   Pointer to buffer with calculated hash.
 * @param [in]   Param_p  Pointer to extra parameters.
 *
 * @return none.
 */
static void Protrom_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    Protrom_Packet_t *Packet_p = (Protrom_Packet_t *)Param_p;

    memcpy(&Packet_p->CRC, Hash_p, sizeof(uint16));
    memcpy((uint8 *)((Packet_p->Buffer_p) + PROTROM_HEADER_LENGTH + (Packet_p->Header.PayloadLength)), Hash_p, sizeof(uint16));
    Protrom_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);
    (void)QUEUE(Packet_p->Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Outbound_p, Packet_p);
}

/** @} */
/** @} */
/** @} */
