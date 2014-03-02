/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _PROTROMRPCINTERFACE_H_
#define _PROTROMRPCINTERFACE_H_

#include "CmdResult.h"
#include "LcmInterface.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "CSimpleQueue.h"
#endif

/// <summary>
/// PROTROM command definitions.
/// </summary>

#define HEADER_A2_IDENTIFIER        (0x0003BEBA)

typedef enum {
    PROTROM_PDU_HEADER = 1,
    PROTROM_PDU_PAYLOAD = 2,
    PROTROM_PDU_PAYLOAD_FINAL = 3,
    PROTROM_PDU_RESULT = 4,
    PROTROM_PDU_READY_TO_RECEIVE = 5,
    PROTROM_PDU_ERROR_DATA = 6,
    PROTROM_PDU_SECURITY_DATA_REQ = 8,
    PROTROM_PDU_SECURITY_DATA_RES = 9,
    PROTROM_PDU_DOMAIN_DATA_REQ = 43,
    PROTROM_PDU_DOMAIN_DATA = 44,
    PROTROM_PDU_ROOT_KEY_REQ = 45,
    PROTROM_PDU_ROOT_KEY_DATA = 46,
    PROTROM_PDU_PATCH_REQ = 47,
    PROTROM_PDU_PATCH_DATA = 48,
    PROTROM_PDU_PATCH_DATA_FINAL = 49,
    PROTROM_PDU_HEADER_OK_SW_REV = 51,
    PROTROM_PDU_SW_REV_DATA = 52,
    PROTROM_PDU_WDOG_RESET = 53,
    PROTROM_PDU_DATA_NOT_FOUND = 100
} PROTROMCommandId_e;

struct TProtromInfo {
    int ReceivedPdu;
    void *DataP;
    uint32 Length;
    uint8 Status;
};

class ProtromRpcInterface
{
public:
    ProtromRpcInterface(CmdResult *CmdResult, LcmInterface *LcmInterface);
    virtual ~ProtromRpcInterface();

    CSimpleQueue ProtromQueue;

    //PROTROM-Protocol
    ErrorCode_e DoRPC_PROTROM_ResultPdu(int Status);
    ErrorCode_e DoRPC_PROTROM_SendLoaderHeader(unsigned char *pFile, uint32 Size);
    ErrorCode_e DoRPC_PROTROM_SendLoaderPayload(unsigned char *pFile, uint32 Size);
    ErrorCode_e DoRPC_PROTROM_SendLoaderFinalPayload(unsigned char *pFile, uint32 Size);
    ErrorCode_e DoRPC_PROTROM_ReadSecurityData(uint8 SecDataId);

    ErrorCode_e DoneRPC_PROTROM_ResultImpl(CommandData_t CmdData);
    ErrorCode_e DoneRPC_PROTROM_ReadSecurityDataImpl(CommandData_t CmdData);
    ErrorCode_e DoneRPC_PROTROM_ReadyToReceiveImpl(CommandData_t CmdData);

    ErrorCode_e Do_CEH_Callback(CommandData_t *pCmdData);

private:
    uint8 *PROTROM_Payload;
    CmdResult *cmdResult_;
    LcmInterface *lcmInterface_;
};

#endif // _PROTROMRPCINTERFACE_H_
