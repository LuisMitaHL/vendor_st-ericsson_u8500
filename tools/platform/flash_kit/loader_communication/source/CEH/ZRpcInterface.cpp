/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "ZRpcInterface.h"

ZRpcInterface::ZRpcInterface(CmdResult *CmdResult, LcmInterface *LcmInterface)
{
    Z_Payload = 0;
    Z_Payload_SetBaudrate = 0;
    cmdResult_ = CmdResult;
    lcmInterface_ = LcmInterface;

    Z_IndataBuffer = new LockLessQueue(10);
}

ZRpcInterface::~ZRpcInterface()
{
    delete Z_IndataBuffer;
    delete[] Z_Payload;
    delete[] Z_Payload_SetBaudrate;
}

ErrorCode_e ZRpcInterface::Do_CEH_Callback(CommandData_t *CmdData_p)
{
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;

    Status = DoneRPC_Z_ReadImpl(*CmdData_p);

    return Status;
}

ErrorCode_e ZRpcInterface::DoRPC_Z_VersionRequest()
{
    ErrorCode_e Result = E_SUCCESS;

    if (Z_Payload != 0) {
        delete [] Z_Payload;
        Z_Payload = 0;
    }

    Z_Payload = new uint8[2];
    Z_Payload[0] = 0x01;
    Z_Payload[1] = 0x3F;

    Result = lcmInterface_->CommunicationSend(Z_Payload);

    return Result;
}

ErrorCode_e ZRpcInterface::DoRPC_Z_SetBaudrate(int Baudrate)
{
    ErrorCode_e Result = E_SUCCESS;
    char Rate;

    switch (Baudrate) {
    case 9600:
        Rate = '0';
        break;

    case 19200:
        Rate = '1';
        break;

    case 38400:
        Rate = '2';
        break;

    case 57600:
        Rate = '3';
        break;

    case 115200:
        Rate = '4';
        break;

    case 230400:
        Rate = '5';
        break;

    case 460800:
        Rate = '6';
        break;

    case 921600:
        Rate = '7';
        break;

    case 1625000:
        Rate = '8';
        break;

    default :
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (Z_Payload != 0) {
        delete [] Z_Payload;
        Z_Payload = 0;
    }

    Z_Payload = new uint8[2];

    Z_Payload[0] = 0x01;
    Z_Payload[1] = 'S';

    Result = lcmInterface_->CommunicationSend(Z_Payload);

    Z_Payload_SetBaudrate = new uint8[2];
    Z_Payload_SetBaudrate[0] = 0x01;
    Z_Payload_SetBaudrate[1] = Rate;

    Result = lcmInterface_->CommunicationSend(Z_Payload_SetBaudrate);

    return Result;
}

ErrorCode_e ZRpcInterface::DoRPC_Z_Exit_Z_Protocol()
{
    ErrorCode_e Result = E_SUCCESS;

    if (Z_Payload != 0) {
        delete [] Z_Payload;
        Z_Payload = 0;
    }

    Z_Payload = new uint8[2];
    Z_Payload[0] = 0x01;
    Z_Payload[1] = 'Q';

    Result = lcmInterface_->CommunicationSend(Z_Payload);

    return Result;
}

ErrorCode_e ZRpcInterface::DoneRPC_Z_ReadImpl(CommandData_t CmdData)
{
    unsigned char Data_p = *CmdData.Payload.Data_p;
    bool Full = false;

    Z_IndataBuffer->Push(Data_p, &Full);

    if (Full == true) {
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}
