/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
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
#include "c_system_v2.h"
#include "r_a2_network.h"
#include "r_a2_transport.h"
#include <stdlib.h>
#include <string.h>
#include "r_basicdefinitions.h"
#include "t_time_utilities.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_a2_header.h"
#include "r_a2_protocol.h"
#include "r_a2_family.h"
#include "r_a2_speedflash.h"
#include "r_communication_service.h"
#include "t_security_algorithms.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static Timer_t *A2_TimerSet(A2_PacketMeta_t *Packet_p, const A2_SendData_LP_t *const InputData_p, Communication_t *Communication_p);
static void A2_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Handles all registered TL processes for A2 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e A2_Transport_Poll(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
#ifndef CFG_ENABLE_LOADER_TYPE
    A2_PacketMeta_t *Packet_p = NULL;

    if (A2_SPEEDFLASH_ACTIVE == A2_SPEEDFLASH(Communication_p)->State) {
        ReturnValue = A2_Speedflash_Poll(Communication_p);
        VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    } else {
        ReturnValue = A2_Network_ReceiverHandler(Communication_p);
        VERIFY_CONDITION(E_SUCCESS == ReturnValue);

        ReturnValue = A2_Network_TransmiterHandler(Communication_p);
        VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    }

    Packet_p = (A2_PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p);

    if (NULL != Packet_p) {
        return A2_Command_Process(Communication_p, Packet_p);
    }

ErrorExit:
    return ReturnValue;
#endif

    return ReturnValue;
}

/*
 * Function for sending packet in A2 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e A2_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p)
{
    A2_PacketMeta_t *Packet_p = NULL;
    A2_SendData_LP_t *InputData_p = (A2_SendData_LP_t *)InputDataIn_p;

    Packet_p = (A2_PacketMeta_t *)A2_Network_PacketAllocate(Communication_p, A2_COMMAND_BUFFER_SIZE);

    if (NULL == Packet_p) {
        A_(printf("a2_transport.c (%d): ** Failed to get available buffers! **\n", __LINE__);)
        return  E_FAILED_TO_ALLOCATE_COMM_BUFFER;
    }

    Packet_p->Resend = 0;
    Packet_p->CallBack_p = NULL;

    Packet_p->Timer_p = A2_TimerSet(Packet_p, InputData_p, Communication_p);
    Packet_p->Header = *InputData_p->Header_p;
    A2_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

    /* setup payload for calculation */
    memcpy(Packet_p->Buffer_p + A2_HEADER_LENGTH, InputData_p->Payload_p, Packet_p->Header.DataLength);

    /* Calculate Payload CRC */
    Packet_p->Communication_p = Communication_p;

    if (HASH_NONE != Communication_p->CurrentFamilyHash) {
        Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
                Communication_p->CurrentFamilyHash,
                (void *)Packet_p->Buffer_p, Packet_p->Header.DataLength + A2_HEADER_LENGTH,
                (uint8 *)&Packet_p->CRC, (HashCallback_t)A2_Transport_OutHashCallback,
                (void *)Packet_p);
    } else {
        memset(&Packet_p->CRC, 0x0, sizeof(uint16));
        Packet_p->Flags = Packet_p->Flags | A2_BUF_PAYLOAD_CRC_CALCULATED;
        (void)QUEUE(Packet_p->Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Outbound_p, Packet_p);
    }

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
static void A2_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    IDENTIFIER_NOT_USED(Data_p);
    IDENTIFIER_NOT_USED(Length);
    A2_PacketMeta_t *Packet_p = (A2_PacketMeta_t *)Param_p;

    Packet_p->Flags = Packet_p->Flags | A2_BUF_PAYLOAD_CRC_CALCULATED;
    memcpy(&Packet_p->CRC, Hash_p, sizeof(uint16));

    if (Packet_p->Header.Protocol == PROTO_A2_ACK) {
        memcpy((uint8 *)((Packet_p->Buffer_p) + A2_HEADER_LENGTH_ACK), Hash_p, sizeof(uint16));
    } else {
        memcpy((uint8 *)((Packet_p->Buffer_p) + A2_HEADER_LENGTH + (Packet_p->Header.DataLength)), Hash_p, sizeof(uint16));
    }

    (void)QUEUE(Packet_p->Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Outbound_p, Packet_p);
}

/*
 * Function for Timer setting.
 *
 * @param [in]  Packet_p         Pointer to the R15 packet meta data.
 * @param [in]  InputData_p      Pointer to the input parameters with settings.
 * @param [in]  Communication_p  Communication module context.
 *
 * @return none.
 */
static Timer_t *A2_TimerSet(A2_PacketMeta_t *Packet_p, const A2_SendData_LP_t *const InputData_p, Communication_t *Communication_p)
{
    Timer_t *Timer_p = NULL;

    if (InputData_p->Time > 0) {
        Timer_p = (Timer_t *)malloc(sizeof(Timer_t));

        if (NULL == Timer_p) {
            return NULL;
        }

        /* timer data */
        Timer_p->Time = InputData_p->Time;
        Timer_p->PeriodicalTime = 0;
        Timer_p->HandleFunction_p = (HandleFunction_t)(InputData_p->TimerCallBackFn_p);
        Timer_p->Data_p = (void *)Packet_p;
        Timer_p->Param_p = (void *)Communication_p;
    }

    return Timer_p;
}
/** @} */
/** @} */
/** @} */
