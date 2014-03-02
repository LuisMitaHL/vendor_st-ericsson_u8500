/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system_v2.h"
#include "r_r15_transport_layer.h"
#include <stdlib.h>
#include <string.h>
#include "r_basicdefinitions.h"
#include "t_time_utilities.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "r_r15_header.h"
#include "r_command_protocol.h"
#include "r_bulk_protocol.h"
#include "r_r15_network_layer.h"
#include "r_r15_family.h"
#include "r_communication_service.h"
#include "t_security_algorithms.h"
#include "r_critical_section.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void TimerSet(PacketMeta_t *Packet_p, const SendData_LP_t *const InputData_p, Communication_t *Communication_p);
static void R15_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);
/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initializes the transport layer for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval E_SUCCESS           After successful execution.
 * @retval E_FAILED_TO_INIT_TL Unsuccessful initialization.
 * @retval E_ALLOCATE_FAILED   Failed to allocate memory space.
 */
ErrorCode_e R15_Transport_Initialize(const Communication_t *const Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int VectorCounter;

    /* Initialize the session counters for command protocol*/
    R15_TRANSPORT(Communication_p)->SessionStateIn  = 0;
    R15_TRANSPORT(Communication_p)->SessionStateOut = 0;

    /* Initialize the session counter for bulk protocol*/
    R15_TRANSPORT(Communication_p)->BulkSessionCounter = 0;

    /* Initialize the default timeouts */
    R15_TIMEOUTS(Communication_p)->TCACK = ACK_TIMEOUT_IN_MS;
    R15_TIMEOUTS(Communication_p)->TBCR = BULK_COMMAND_RECEIVING_TIMEOUT;
    R15_TIMEOUTS(Communication_p)->TBDR = BULK_DATA_RECEIVING_TIMEOUT;

    for (VectorCounter = 0; VectorCounter < MAX_BULK_TL_PROCESSES; VectorCounter++) {
        R15_TRANSPORT(Communication_p)->BulkVectorList[VectorCounter].Status = BULK_SESSION_IDLE;
    }

    memset(&(R15_TRANSPORT(Communication_p)->PreviousBulkVector), 0x00, sizeof(TL_BulkVectorList_t));
    R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p = NULL;
    R15_TRANSPORT(Communication_p)->BulkHandle.BulkTransferCS = Do_CriticalSection_Create();
    R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p = NULL;

    return ReturnValue;
}

/*
 * Shut Down the transport layer for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.

 * @retval E_SUCCESS  After successful execution.
 */
ErrorCode_e R15_Transport_Shutdown(const Communication_t *const Communication_p)
{
    BUFFER_FREE(R15_TRANSPORT(Communication_p)->BulkHandle.PendingBulkHeader_p);
    Do_CriticalSection_Destroy(&(R15_TRANSPORT(Communication_p)->BulkHandle.BulkTransferCS));
    return E_SUCCESS;
}

/*
 * Handles all registered TL processes for R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e R15_Transport_Poll(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
#ifndef CFG_ENABLE_LOADER_TYPE
    PacketMeta_t *Packet_p = NULL;

    ReturnValue = R15_Network_ReceiverHandler(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

    ReturnValue = R15_Network_TransmiterHandler(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);
#endif

    if (BULK_SESSION_IDLE != Do_R15_Bulk_GetStatusSession(R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p)) {
        ReturnValue = R15_Bulk_Process_Write(Communication_p, R15_TRANSPORT(Communication_p)->BulkHandle.BulkVector_p);
        VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    }

    ReturnValue = R15_NETWORK(Communication_p)->Outbound.LCM_Error;
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

#ifndef CFG_ENABLE_LOADER_TYPE

    while ((Packet_p = (PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p)) != NULL) {
        switch (Packet_p->Header.Protocol) {
        case PROTO_BULK:
            return R15_Bulk_Process(Communication_p, Packet_p);

        case PROTO_COMMAND:
            return R15_Command_Process(Communication_p, Packet_p);

        default:
            return E_SUCCESS;
        }
    }

#endif

ErrorExit:
    return ReturnValue;
}


/*
 * Function for sending packet in R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputDataIn_p   Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e R15_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p)
{
    PacketMeta_t *Packet_p;
    SendData_LP_t *InputData_p = (SendData_LP_t *)InputDataIn_p;

    if (InputData_p->Header_p->Protocol == BULK_PROTOCOL && (((BulkExtendedHeader_t *)InputData_p->ExtendedHeader_p)->TypeFlags  & MASK_BULK_COMMAND_SELECT) == CMD_BULK_DATA) {
        Packet_p = R15_Network_PacketAllocate(Communication_p, BULK_BUFFER_SIZE);
    } else {
        Packet_p = R15_Network_PacketAllocate(Communication_p, COMMAND_BUFFER_SIZE);
    }

    if (NULL == Packet_p) {
        A_(printf("r15_transport_layer.c (%d): ** Failed to get available buffers! **\n", __LINE__);)
        return  E_FAILED_TO_ALLOCATE_COMM_BUFFER;
    }

    Packet_p->Resend = 0;
    Packet_p->CallBack_p = NULL;

    TimerSet(Packet_p, InputData_p, Communication_p);
    Packet_p->Header = *InputData_p->Header_p;
    /* serialize and calculate extended header */
    Packet_p->ExtendedHeader_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;
    R15_SerializeExtendedHeader(Packet_p->ExtendedHeader_p, InputData_p->Header_p->Protocol, InputData_p->ExtendedHeader_p, &(Packet_p->Header.ExtendedHeaderChecksum));

    if (Packet_p->Header.ExtendedHeaderLength == COMMAND_EXTENDED_HEADER_LENGTH) {
        Packet_p->Payload_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH + ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH;
    } else {
        Packet_p->Payload_p = Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH + ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
    }

    if (NULL != InputData_p->Payload_p) {
        /* setup payload for calculation */
        memcpy(Packet_p->Payload_p, InputData_p->Payload_p, Packet_p->Header.PayloadLength);

        /* Calculate Payload CRC */
        Packet_p->Communication_p = Communication_p;

        if (HASH_NONE != Packet_p->Header.Flags) {
            Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
                    (HashType_e)Packet_p->Header.Flags,
                    (void *)Packet_p->Payload_p, Packet_p->Header.PayloadLength,
                    Packet_p->Hash, (HashCallback_t)R15_Transport_OutHashCallback,
                    (void *)Packet_p);
        } else {
            SET_PACKET_FLAGS(Packet_p, PACKET_CRC_STATE_MASK, BUF_PAYLOAD_CRC_CALCULATED);
            memset(&Packet_p->Header.PayloadChecksum, 0x0, sizeof(uint32));
            R15_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

            (void)QUEUE(Packet_p->Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Outbound_p, Packet_p);
        }
    } else {
        R15_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

        (void)QUEUE(Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, Packet_p);
    }

    return E_SUCCESS;
}

/*
 * Function for setting the timeouts in the R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  TimeoutData_p   Pointer to the input data with all timeouts.
 *
 * @retval E_SUCCESS            After successful execution.
 */
ErrorCode_e R15_SetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    R15_Timeouts_t *NewTimeoutData_p = (R15_Timeouts_t *)TimeoutData_p;
    VERIFY(NULL != TimeoutData_p, E_INVALID_INPUT_PARAMETERS);
    C_(printf("R15_SetProtocolTimeouts: Setting new timeouts!\n");)
    C_(printf("                  TCACK: %d\n", NewTimeoutData_p->TCACK);)
    C_(printf("                   TBCR: %d\n", NewTimeoutData_p->TBCR);)
    C_(printf("                   TBDR: %d\n", NewTimeoutData_p->TBDR);)

    R15_TIMEOUTS(Communication_p)->TCACK = NewTimeoutData_p->TCACK;
    R15_TIMEOUTS(Communication_p)->TBCR = NewTimeoutData_p->TBCR;
    R15_TIMEOUTS(Communication_p)->TBDR = NewTimeoutData_p->TBDR;
    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Function for getting the timeouts in the R15 protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [out] TimeoutData_p   Return all defined timeouts.
 *
 * @retval E_SUCCESS            After successful execution.
 */
ErrorCode_e R15_GetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    VERIFY(NULL != TimeoutData_p, E_INVALID_INPUT_PARAMETERS);

    ((R15_Timeouts_t *)TimeoutData_p)->TCACK = R15_TIMEOUTS(Communication_p)->TCACK;
    ((R15_Timeouts_t *)TimeoutData_p)->TBCR = R15_TIMEOUTS(Communication_p)->TBCR;
    ((R15_Timeouts_t *)TimeoutData_p)->TBDR = R15_TIMEOUTS(Communication_p)->TBDR;
    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}


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
static void R15_Transport_OutHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    PacketMeta_t *Packet_p = (PacketMeta_t *)Param_p;

    SET_PACKET_FLAGS(Packet_p, PACKET_CRC_STATE_MASK, BUF_PAYLOAD_CRC_CALCULATED);
    memcpy(&Packet_p->Header.PayloadChecksum, Hash_p, sizeof(uint32));
    R15_SerializeHeader(Packet_p->Buffer_p, &Packet_p->Header);

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
static void TimerSet(PacketMeta_t *Packet_p, const SendData_LP_t *const InputData_p, Communication_t *Communication_p)
{
    if (InputData_p->Time > 0) {
        /* timer data */
        Packet_p->Timer.Time = InputData_p->Time;
        Packet_p->Timer.PeriodicalTime = 0;
        Packet_p->Timer.HandleFunction_p = (HandleFunction_t)(InputData_p->TimerCallBackFn_p);
        Packet_p->Timer.Data_p = (void *)Packet_p;
        Packet_p->Timer.Param_p = (void *)Communication_p;
    }
}


/** @} */
/** @} */
/** @} */
