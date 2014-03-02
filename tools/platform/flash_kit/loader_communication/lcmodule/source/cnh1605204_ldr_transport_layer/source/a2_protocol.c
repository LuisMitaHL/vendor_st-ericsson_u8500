/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "r_communication_service.h"
#include "r_a2_family.h"
#include "r_a2_network.h"
#include "r_a2_transport.h"
#include "t_a2_header.h"
#include "t_a2_protocol.h"
#include "r_a2_protocol.h"
#include "t_security_algorithms.h"
#include "r_command_protocol.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_serialization.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/** Holding the sequence number for each communication device.*/
uint8               A2_SequenceNr = 0;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e ProcessAcknowledgement(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p);
static ErrorCode_e ProcessCommand(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p);
static ErrorCode_e ProcessGeneralResponse(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p_p);
static ErrorCode_e ProcessControlMessage(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p);
static ErrorCode_e SendAcknowledge(Communication_t *Communication_p, const A2_PacketMeta_t *const Packet_p);
static ErrorCode_e DispatchCommand(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p, A2_CommandData_t CmdData);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Sends command packet with command protocol.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] CmdData_p       Pointer to the command data.
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If CmdData_p is NULL.
 */
ErrorCode_e Do_A2_Command_Send(Communication_t *Communication_p, A2_CommandData_t *CmdData_p)
{
    A2_Header_t  Header = {0};
    A2_SendData_LP_t Param;

    if (NULL == CmdData_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    Param.Header_p = &Header;
    Param.Payload_p = CmdData_p->Payload.Data_p;

    Header.HeaderPattern = A2_HEADER_PATTERN;
    Header.Protocol = PROTO_A2;
    Header.SourceAddress = 0x00;
    Header.DestinationAddress = CmdData_p->DestAddress;
    Header.SequenceNumber = ++A2_SequenceNr;
    Header.DataLength = CmdData_p->Payload.Size;

    Param.Time = 0xFFFFFFFF; //ACK_TIMEOUT_IN_MS;
    Param.TimerCallBackFn_p = NULL;   //NOTE: the timer will be assigned in "A2_Transport_Send"

    C_(printf("a2_protocol.c (%d): A2_Transport_Send! Type:%d SequenceNr:%d Command:%d CommandGroup:%d\n", __LINE__, CmdData_p->Type, Header.SequenceNumber, CmdData_p->ApplicationNr, CmdData_p->CommandNr);)

    return A2_Transport_Send(Communication_p, &Param);
}

/*
 * Decode received command.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval E_SUCCESS                  After successful execution.
 * @retval E_INVALID_INPUT_PARAMETERS if Packet_p is NULL.
 */
ErrorCode_e A2_Command_Process(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    switch (((A2_PacketMeta_t *)Packet_p)->Header.Protocol) {
    case PROTO_CTRL_MSG:
        ReturnValue = ProcessControlMessage(Communication_p, Packet_p);
        break;

    case PROTO_A2:

        if (0xFF == *(Packet_p->Payload_p + 1)) {
            ReturnValue = ProcessGeneralResponse(Communication_p, Packet_p);
        } else {
            ReturnValue = ProcessCommand(Communication_p, Packet_p);
        }

        break;

    case PROTO_A2_ACK:
        ReturnValue = ProcessAcknowledgement(Communication_p, Packet_p);
        break;

    default:
        ReturnValue = E_SUCCESS;
        break;
    }

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e ProcessControlMessage(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    A2_CommandData_t CmdData = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    void *Tmp_Buffer_p = Packet_p->Payload_p;

    CmdData.Type = A2_CONTROL_MESSAGE;
    CmdData.ApplicationNr = 0x10;
    CmdData.CommandNr = get_uint8(&Tmp_Buffer_p);
    CmdData.SessionNr = (uint16)(Packet_p->Header.SequenceNumber);
    CmdData.Payload.Size = Packet_p->Header.DataLength;
    CmdData.Payload.Data_p = NULL;

    if (0 != CmdData.Payload.Size) {
        CmdData.Payload.Data_p = (uint8 *)malloc(Packet_p->Header.DataLength);

        if (NULL == CmdData.Payload.Data_p) {
            return E_ALLOCATE_FAILED;
        }

        memcpy(CmdData.Payload.Data_p, Packet_p->Payload_p, Packet_p->Header.DataLength);
    }

    return DispatchCommand(Communication_p, Packet_p, CmdData);
}

static ErrorCode_e ProcessAcknowledgement(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    B_(printf("a2_protocol.c (%d): Received Acknowledge!\n", __LINE__);)
    ReturnValue = A2_Network_CancelRetransmission(Communication_p, Packet_p->Header.SequenceNumber);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    ReturnValue = A2_Network_PacketRelease(Communication_p, Packet_p);

    if (A2_SPEEDFLASH_START == A2_SPEEDFLASH(Communication_p)->State) {
        Communication_p->CommunicationDevice_p->Cancel(Communication_p->CommunicationDevice_p);
        A2_SPEEDFLASH(Communication_p)->State = A2_SPEEDFLASH_ACTIVE;
        A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_READ_REQ;
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e ProcessCommand(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue;
    A2_CommandData_t CmdData = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    void *Tmp_Buffer_p = Packet_p->Payload_p;

    CmdData.Type = A2_COMMAND;
    CmdData.ApplicationNr = get_uint8(&Tmp_Buffer_p);
    CmdData.CommandNr = get_uint8(&Tmp_Buffer_p);
    CmdData.SessionNr = (uint16)(Packet_p->Header.SequenceNumber);
    CmdData.Payload.Size = Packet_p->Header.DataLength;
    CmdData.Payload.Data_p = NULL;

    if (0 != CmdData.Payload.Size) {
        CmdData.Payload.Data_p = (uint8 *)malloc(Packet_p->Header.DataLength);

        if (NULL == CmdData.Payload.Data_p) {
            return E_ALLOCATE_FAILED;
        }

        memcpy(CmdData.Payload.Data_p, Packet_p->Payload_p, Packet_p->Header.DataLength);
    }

    ReturnValue = SendAcknowledge(Communication_p, Packet_p);

    if (E_SUCCESS != ReturnValue) {
        BUFFER_FREE(CmdData.Payload.Data_p);
        return ReturnValue;
    }

    return DispatchCommand(Communication_p, Packet_p, CmdData);
}

static ErrorCode_e ProcessGeneralResponse(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue;
    A2_CommandData_t CmdData = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    void *Tmp_Buffer_p = Packet_p->Payload_p;

    CmdData.Type = A2_GENERAL_RESPONSE;
    CmdData.ApplicationNr = get_uint8(&Tmp_Buffer_p);
    // skip 0xFF byte signifying GR
    skip_uint8(&Tmp_Buffer_p);
    // skip more packets field
    // TODO check how we should handle situation when GR contains more packets
    skip_uint8(&Tmp_Buffer_p);
    CmdData.CommandNr = get_uint8(&Tmp_Buffer_p);
    CmdData.SessionNr = (uint16)(Packet_p->Header.SequenceNumber);
    CmdData.Payload.Size = Packet_p->Header.DataLength;
    CmdData.Payload.Data_p = NULL;

    if (0 != CmdData.Payload.Size) {
        CmdData.Payload.Data_p = (uint8 *)malloc(Packet_p->Header.DataLength);

        if (NULL == CmdData.Payload.Data_p) {
            return E_ALLOCATE_FAILED;
        }

        memcpy(CmdData.Payload.Data_p, Packet_p->Payload_p, Packet_p->Header.DataLength);
    }

    ReturnValue = SendAcknowledge(Communication_p, Packet_p);

    if (E_SUCCESS != ReturnValue) {
        BUFFER_FREE(CmdData.Payload.Data_p);
        return ReturnValue;
    }

    return DispatchCommand(Communication_p, Packet_p, CmdData);
}


static ErrorCode_e DispatchCommand(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p, A2_CommandData_t CmdData)
{
    ErrorCode_e ReturnValue;

    ReturnValue = A2_Network_PacketRelease(Communication_p, Packet_p);

    ReturnValue = Communication_p->Do_CEH_Call_Fn(OBJECT_CEH_CALL(Communication_p), (CommandData_t *)&CmdData);

    if (NULL != CmdData.Payload.Data_p) {
        free(CmdData.Payload.Data_p);
    }

    return ReturnValue;
}

static ErrorCode_e SendAcknowledge(Communication_t *Communication_p, const A2_PacketMeta_t *const Packet_p)
{
    A2_Header_t      Header = {0};
    A2_SendData_LP_t Param;

    Header.HeaderPattern = A2_HEADER_PATTERN;
    Header.Protocol = PROTO_A2_ACK;
    Header.SourceAddress = 0x00;
    Header.DestinationAddress = Packet_p->Header.SourceAddress;
    Header.SequenceNumber = Packet_p->Header.SequenceNumber;

    Param.Header_p = &Header;
    Param.Payload_p = NULL;
    Param.Time = 0;
    Param.TimerCallBackFn_p = NULL;

    return A2_Transport_Send(Communication_p, &Param);
}

/** @} */
/** @} */
/** @} */
