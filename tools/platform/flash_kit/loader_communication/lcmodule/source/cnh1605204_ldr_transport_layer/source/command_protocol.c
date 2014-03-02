/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup command_protocol
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "r_command_protocol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "r_communication_service.h"
#include "r_r15_family.h"
#include "r_r15_network_layer.h"
#include "r_r15_transport_layer.h"
#include "t_r15_header.h"
#include "t_security_algorithms.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_r15_header.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e ProcessAcknowledgement(Communication_t *Communication_p, const PacketMeta_t *Packet_p, const uint16 *const SessionState_p);
static ErrorCode_e ProcessCommand(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint16 *SessionState);
static ErrorCode_e ProcessGeneralResponse(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint16 *SessionState_p);
static ErrorCode_e SendAcknowledge(Communication_t *Communication_p, const PacketMeta_t *const Packet_p);
static uint16 GetSendSession(const Communication_t *const Communication_p, CommandData_t *CmdData_p);
static ErrorCode_e DispatchCommand(Communication_t *Communication_p, PacketMeta_t *Packet_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * Reset the Session counters.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Do_R15_Command_ResetSessionCounters(const Communication_t *const Communication_p)
{
    /* Initialize the session counters */
    R15_TRANSPORT(Communication_p)->SessionStateIn  = 0;
    R15_TRANSPORT(Communication_p)->SessionStateOut = 0;

    return E_SUCCESS;
}


/*
 * Sends command packet with command protocol.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] CmdData_p       Pointer to the command data.
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If CmdData_p is NULL.
 */
ErrorCode_e Do_R15_Command_Send(Communication_t *Communication_p, CommandData_t *CmdData_p)
{
    CommandExtendedHeader_t ExtendedHeader;
    R15_Header_t  Header;
    SendData_LP_t Param;

    if (NULL == CmdData_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    Param.Header_p = &Header;
    Param.ExtendedHeader_p = &ExtendedHeader;
    Param.Payload_p = CmdData_p->Payload.Data_p;

    Header.Protocol = COMMAND_PROTOCOL;
    Header.Flags = Communication_p->CurrentFamilyHash;
    Header.PayloadLength = CmdData_p->Payload.Size;
    Header.PayloadChecksum = 0;
    Header.ExtendedHeaderLength = COMMAND_EXTENDED_HEADER_LENGTH;
    Header.ExtendedHeaderChecksum = 0;

    ExtendedHeader.SessionState = GetSendSession(Communication_p, CmdData_p);
    ExtendedHeader.Command = CmdData_p->CommandNr;
    ExtendedHeader.CommandGroup = CmdData_p->ApplicationNr;

    // TODO: Don't we need a timer callback for this to actually do something?
    Param.Time = R15_TIMEOUTS(Communication_p)->TCACK; //ACK_TIMEOUT_IN_MS;
    Param.TimerCallBackFn_p = NULL; //NOTE: the timer will be assigned in "R15_Transport_Send"

    C_(printf("command_protocol.c (%d): R15_Transport_Send! Type:%d Session:%d Command:%d CommandGroup:%d\n", __LINE__, CmdData_p->Type, ExtendedHeader.SessionState, ExtendedHeader.Command, ExtendedHeader.CommandGroup);)

    return R15_Transport_Send(Communication_p, &Param);
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
ErrorCode_e R15_Command_Process(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    CommandExtendedHeader_t CommandExtendedHeader = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    R15_DeserializeCommandExtendedHeader(&CommandExtendedHeader, Packet_p->ExtendedHeader_p);

    switch (CommandExtendedHeader.SessionState & MASK_SELECT_STATE) {
    case COMMAND_PACKAGE:
        ReturnValue = ProcessCommand(Communication_p, Packet_p, &(R15_TRANSPORT(Communication_p)->SessionStateIn));
        break;

    case ACK_PACKAGE:
        ReturnValue = ProcessAcknowledgement(Communication_p, Packet_p, &(R15_TRANSPORT(Communication_p)->SessionStateOut));
        break;

    case GENERAL_RESPONSE_PACKAGE:
        ReturnValue = ProcessGeneralResponse(Communication_p, Packet_p, &(R15_TRANSPORT(Communication_p)->SessionStateOut));
        break;

    case ACK_GENERAL_RESPONSE_PACKAGE:
        ReturnValue = ProcessAcknowledgement(Communication_p, Packet_p, &(R15_TRANSPORT(Communication_p)->SessionStateIn));
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
static uint16 GetSendSession(const Communication_t *const Communication_p, CommandData_t *CmdData_p)
{
    uint16 value;

    if (CmdData_p->Type == GENERAL_RESPONSE) {
        value = CmdData_p->SessionNr << 2 | GENERAL_RESPONSE_PACKAGE;
    } else {
        R15_TRANSPORT(Communication_p)->SessionStateOut += 4;
        value = R15_TRANSPORT(Communication_p)->SessionStateOut;
        CmdData_p->SessionNr = value >> 2;
    }

    return value;
}


static ErrorCode_e ProcessAcknowledgement(Communication_t *Communication_p, const PacketMeta_t *Packet_p, const uint16 *const SessionState_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 Key;

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    Key = R15_Network_CreateUniqueKey(Packet_p, 0);
    (void)R15_Network_CancelRetransmission(Communication_p, Key);
    B_(printf("command_protocol.c (%d): Received Acknowledge!\n", __LINE__);)

    ReturnValue = R15_Network_PacketRelease(Communication_p, (PacketMeta_t *)Packet_p);
    return ReturnValue;
}


static ErrorCode_e ProcessCommand(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint16 *SessionState_p)
{
    ErrorCode_e ReturnValue;
    CommandExtendedHeader_t CommandExtendedHeader = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    R15_DeserializeCommandExtendedHeader(&CommandExtendedHeader, Packet_p->ExtendedHeader_p);

    if ((CommandExtendedHeader.SessionState & SESSION_MASK) == (*SessionState_p & SESSION_MASK) + 4) {
        *SessionState_p += 4;
    } else {
        ReturnValue = SendAcknowledge(Communication_p, Packet_p);

        if (E_SUCCESS != ReturnValue) {
            return ReturnValue;
        }

        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        return ReturnValue;
    }

    return DispatchCommand(Communication_p, Packet_p);
}

static ErrorCode_e ProcessGeneralResponse(Communication_t *Communication_p, PacketMeta_t *Packet_p, uint16 *SessionState_p)
{
    ErrorCode_e ReturnValue;
    CommandExtendedHeader_t ExtendedHeader = {0};

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    R15_DeserializeCommandExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

    if ((ExtendedHeader.SessionState & SESSION_MASK) > (*SessionState_p & SESSION_MASK)) {
        ReturnValue = SendAcknowledge(Communication_p, Packet_p);

        if (E_SUCCESS != ReturnValue) {
            return ReturnValue;
        }

        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
        return ReturnValue;
    }

    return DispatchCommand(Communication_p, Packet_p);
}


static ErrorCode_e DispatchCommand(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue;
    CommandExtendedHeader_t ExtendedHeader = {0};
    CommandData_t CmdData;

    ReturnValue = SendAcknowledge(Communication_p, Packet_p);

    if (E_SUCCESS != ReturnValue) {
        return E_SUCCESS;
    }

    R15_DeserializeCommandExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

    CmdData.Type = (CommandType_t)(ExtendedHeader.SessionState & MASK_SELECT_STATE);
    CmdData.CommandNr = ExtendedHeader.Command;
    CmdData.ApplicationNr = ExtendedHeader.CommandGroup;
    CmdData.SessionNr = ExtendedHeader.SessionState >> 2;
    CmdData.Payload.Size = Packet_p->Header.PayloadLength;
    CmdData.Payload.Data_p = Packet_p->Payload_p;

    ReturnValue = Communication_p->Do_CEH_Call_Fn(OBJECT_CEH_CALL(Communication_p), &CmdData);

    if (E_SUCCESS == ReturnValue) {
        ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
    } else {
        ErrorCode_e PacketReleaseRet = R15_Network_PacketRelease(Communication_p, Packet_p);

        if (E_SUCCESS != PacketReleaseRet) {
            A_(printf("command_protocol.c (%d): Failed to release Network packet! Error: %d\n", __LINE__, PacketReleaseRet);)
        }
    }

    return ReturnValue;
}

static ErrorCode_e SendAcknowledge(Communication_t *Communication_p, const PacketMeta_t *const Packet_p)
{
    CommandExtendedHeader_t ExtendedHeader;
    R15_Header_t  Header;
    SendData_LP_t Param;

    memcpy(&ExtendedHeader, Packet_p->ExtendedHeader_p, COMMAND_EXTENDED_HEADER_LENGTH);
    ExtendedHeader.SessionState++;
    memcpy(&Header, &Packet_p->Header, HEADER_LENGTH);
    Header.Flags = Communication_p->CurrentFamilyHash;
    Header.PayloadLength = 0;
    Header.PayloadChecksum = 0;
    Header.ExtendedHeaderChecksum = 0;

    Param.Header_p = &Header;
    Param.ExtendedHeader_p = &ExtendedHeader;
    Param.Payload_p = NULL;
    Param.Time = 0;
    Param.TimerCallBackFn_p = NULL;

    return R15_Transport_Send(Communication_p, &Param);
}

/** @} */
/** @} */
/** @} */
