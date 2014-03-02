/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _ZRPCINTERFACE_H_
#define _ZRPCINTERFACE_H_

#include "CmdResult.h"
#include "LcmInterface.h"
#include "LockLessQueue.h"

class ZRpcInterface
{
public:
    ZRpcInterface(CmdResult *CmdResult, LcmInterface *LcmInterface);
    virtual ~ZRpcInterface();

    //Circular buffer for received data when using Z-protocol.
    LockLessQueue *Z_IndataBuffer;

    //Z-Protocol
    ErrorCode_e DoRPC_Z_VersionRequest();
    ErrorCode_e DoRPC_Z_SetBaudrate(int Baudrate);
    ErrorCode_e DoRPC_Z_Exit_Z_Protocol();

    ErrorCode_e DoneRPC_Z_ReadImpl(CommandData_t CmdData);

    ErrorCode_e Do_CEH_Callback(CommandData_t *pCmdData);

private:
    //Payload variables for z-protocol
    uint8 *Z_Payload;
    uint8 *Z_Payload_SetBaudrate;
    CmdResult *cmdResult_;
    LcmInterface *lcmInterface_;
};

#endif // _ZRPCINTERFACE_H_
