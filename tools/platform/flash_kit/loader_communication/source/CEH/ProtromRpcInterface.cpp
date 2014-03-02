/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#include "ProtromRpcInterface.h"
#include "t_protrom_transport.h"
#include "t_protrom_network.h"
#include "Serialization.h"

ProtromRpcInterface::ProtromRpcInterface(CmdResult *CmdResult, LcmInterface *LcmInterface)
{
    PROTROM_Payload = new uint8[0x10000];
    cmdResult_ = CmdResult;
    lcmInterface_ = LcmInterface;
}

ProtromRpcInterface::~ProtromRpcInterface()
{
    ProtromQueue.SignalEvent();
    delete[] PROTROM_Payload;
}

ErrorCode_e ProtromRpcInterface::Do_CEH_Callback(CommandData_t *CmdData_p)
{
    uint8 *Data_p = (uint8 *)CmdData_p->Payload.Data_p;

    TProtromInfo *pInfo = new TProtromInfo;
    pInfo->Length = CmdData_p->Payload.Size;
    pInfo->DataP = new uint8[pInfo->Length];
    memcpy(pInfo->DataP, Data_p, pInfo->Length);

    int Pdu = *(Data_p + 7);             //Get PDU number

    switch (Pdu) {
    case PROTROM_PDU_RESULT: {
        pInfo->ReceivedPdu = PROTROM_PDU_RESULT;
        pInfo->Status = DoneRPC_PROTROM_ResultImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_READY_TO_RECEIVE: {
        pInfo->ReceivedPdu = PROTROM_PDU_READY_TO_RECEIVE;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_ERROR_DATA: {
        pInfo->ReceivedPdu = PROTROM_PDU_ERROR_DATA;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;


    case PROTROM_PDU_SECURITY_DATA_RES: {
        pInfo->ReceivedPdu = PROTROM_PDU_SECURITY_DATA_RES;
        pInfo->Status = DoneRPC_PROTROM_ReadSecurityDataImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_DOMAIN_DATA_REQ: {
        pInfo->ReceivedPdu = PROTROM_PDU_DOMAIN_DATA_REQ;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_DOMAIN_DATA: {
        pInfo->ReceivedPdu = PROTROM_PDU_DOMAIN_DATA;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_SECURITY_DATA_REQ: {
        pInfo->ReceivedPdu = PROTROM_PDU_SECURITY_DATA_REQ;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_ROOT_KEY_REQ: {
        pInfo->ReceivedPdu = PROTROM_PDU_ROOT_KEY_REQ;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_ROOT_KEY_DATA: {
        pInfo->ReceivedPdu = PROTROM_PDU_ROOT_KEY_DATA;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_PATCH_REQ: {
        pInfo->ReceivedPdu = PROTROM_PDU_PATCH_REQ;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_PATCH_DATA: {
        pInfo->ReceivedPdu = PROTROM_PDU_PATCH_DATA;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_PATCH_DATA_FINAL: {
        pInfo->ReceivedPdu = PROTROM_PDU_PATCH_DATA_FINAL;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_HEADER_OK_SW_REV: {
        pInfo->ReceivedPdu = PROTROM_PDU_HEADER_OK_SW_REV;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_SW_REV_DATA: {
        pInfo->ReceivedPdu = PROTROM_PDU_SW_REV_DATA;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_WDOG_RESET: {
        pInfo->ReceivedPdu = PROTROM_PDU_WDOG_RESET;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;

    case PROTROM_PDU_DATA_NOT_FOUND: {
        pInfo->ReceivedPdu = PROTROM_PDU_DATA_NOT_FOUND;
        pInfo->Status = DoneRPC_PROTROM_ReadyToReceiveImpl(*CmdData_p);
    }
    break;
    }

    ProtromQueue.AddRequest(static_cast<void *>(pInfo));

    return E_SUCCESS;
}

ErrorCode_e ProtromRpcInterface::DoRPC_PROTROM_ResultPdu(int Status)
{
    ErrorCode_e Result = E_SUCCESS;
    Protrom_SendData_LP_t Data;
    Protrom_Header_t Header;

    Header.PayloadLength = 2;                   // PDU +  Status
    Data.Header_p = &Header;                    // PROTROM header
    Data.Payload_p = PROTROM_Payload;           // pointer to the payload data
    PROTROM_Payload[0] = 0x04;                  // Result PDU 0x04.
    PROTROM_Payload[1] = Status;                // Status

    Result = lcmInterface_->CommunicationSend(&Data);

    return Result;
}

ErrorCode_e ProtromRpcInterface::DoRPC_PROTROM_SendLoaderHeader(unsigned char *pFile, uint32 Size)
{
    ErrorCode_e Result = E_SUCCESS;
    Protrom_SendData_LP_t Data;
    Protrom_Header_t Header;

    Header.PayloadLength = 1 + (uint16)Size;    // PDU + Header payload + padding.
    Data.Header_p = &Header;                    // PROTROM header
    Data.Payload_p = PROTROM_Payload;           // pointer to the payload data

    //Copy PDU and header data to Payload
    PROTROM_Payload[0] = 0x01;                  //Header PDU 0x01

    memcpy(PROTROM_Payload + 1, pFile, Size);   //Copy header data

    Result = lcmInterface_->CommunicationSend(&Data);

    return Result;
}

ErrorCode_e ProtromRpcInterface::DoRPC_PROTROM_SendLoaderPayload(unsigned char *pFile, uint32 Size)
{
    ErrorCode_e Result = E_SUCCESS;
    Protrom_SendData_LP_t Data;
    Protrom_Header_t Header;

    Header.PayloadLength = 1 + (uint16)Size;                        // PDU + Headeer payload.
    Data.Header_p = &Header;                                        // PROTROM header
    Data.Payload_p = PROTROM_Payload;                               // pointer to the payload data

    //Copy PDU and header data to Payload
    PROTROM_Payload[0] = 0x02;                                      //Payload PDU 0x02

    memcpy(PROTROM_Payload + 1, pFile, Size);                       //Copy payload data

    Result = lcmInterface_->CommunicationSend(&Data);

    return Result;
}

ErrorCode_e ProtromRpcInterface::DoRPC_PROTROM_SendLoaderFinalPayload(unsigned char *pFile, uint32 Size)
{
    ErrorCode_e Result = E_SUCCESS;
    Protrom_SendData_LP_t Data;
    Protrom_Header_t Header;

    Header.PayloadLength = 1 + (uint16)Size;                        // PDU + Payload.
    Data.Header_p = &Header;                                        // PROTROM header
    Data.Payload_p = PROTROM_Payload;                               // pointer to the payload data

    //Copy PDU and header data to Payload
    PROTROM_Payload[0] = 0x03;                                      //Final payload PDU 0x03

    memcpy(PROTROM_Payload + 1, pFile, Size);

    Result = lcmInterface_->CommunicationSend(&Data);

    return Result;
}

ErrorCode_e ProtromRpcInterface::DoRPC_PROTROM_ReadSecurityData(uint8 SecDataId)
{
    ErrorCode_e Result = E_SUCCESS;
    Protrom_SendData_LP_t Data;
    Protrom_Header_t Header;

    Header.PayloadLength = 2;                                       // PDU + Payload.
    Data.Header_p = &Header;                                        // PROTROM header
    Data.Payload_p = PROTROM_Payload;                               // pointer to the payload data

    //Copy PDU and header data to Payload
    PROTROM_Payload[0] = 0x08;                                      // Security Data Request PDU 0x08
    PROTROM_Payload[1] = SecDataId;

    Result = lcmInterface_->CommunicationSend(&Data);

    return Result;
}

ErrorCode_e ProtromRpcInterface::DoneRPC_PROTROM_ResultImpl(CommandData_t CmdData)
{
    uint8 *Payload_p = CmdData.Payload.Data_p + PROTROM_HEADER_LENGTH;

    // skip PDU type
    Payload_p++;

    return E_SUCCESS;
}

ErrorCode_e ProtromRpcInterface::DoneRPC_PROTROM_ReadSecurityDataImpl(CommandData_t CmdData)
{
    uint8 *Payload_p = CmdData.Payload.Data_p + PROTROM_HEADER_LENGTH;
    size_t PayloadLength = CmdData.Payload.Size - PROTROM_HEADER_LENGTH - PROTROM_CRC_LENGTH;

    // skip PDU type
    Payload_p++;
    PayloadLength--;

    cmdResult_->ProtromPayloadData.clear();
    cmdResult_->ProtromPayloadData = vector<uint8>(Payload_p, Payload_p + PayloadLength);
    return E_SUCCESS;
}

ErrorCode_e ProtromRpcInterface::DoneRPC_PROTROM_ReadyToReceiveImpl(CommandData_t CmdData)
{
    return E_SUCCESS;
}

