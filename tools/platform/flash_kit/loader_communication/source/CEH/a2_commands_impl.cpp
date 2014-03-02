/*******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

#include "a2_commands_impl.h"
#include "Serialization.h"
#include "CmdResult.h"

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_System_ShutdownImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_System_LoaderVersionImpl(uint16 Session, ErrorCode_e Status, int LoaderVersionOutPLSize, const void *LoaderVersionOut)
{
    if (0 == Status) {
        cmdResult_->A2_LoaderVersion = string(static_cast<const char *>(LoaderVersionOut), LoaderVersionOutPLSize);
    }

    cmdResult_->GeneralResponse_Session = Session;
    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_System_LoaderOnLoaderImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_System_ResetImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_VerifySignedHeaderImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_SoftwareBlockAddressImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_ProgramFlashImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_VerifySoftwareFlashImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_EraseFlashImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Flash_SpeedflashImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoRPC_A2_Control_LoaderStartedImpl(uint16 Session, const uint32 MaxLoaderPacketSize)
{
    cmdResult_->GeneralResponse_Session = Session;

    cmdResult_->A2_MaxLoaderPacketSize = MaxLoaderPacketSize;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Control_PingImpl(uint16 Session)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoRPC_A2_Control_PongImpl(uint16 Session)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoneRPC_A2_Control_MaxPacketSizeImpl(uint16 Session, const uint16 MaxLoaderPacketSize)
{
    cmdResult_->GeneralResponse_Session = Session;

    cmdResult_->A2_MaxLoaderPacketSize = MaxLoaderPacketSize;

    return E_SUCCESS;
}

ErrorCode_e A2LoaderRpcInterfaceImpl::DoRPC_A2_Control_LoaderNotStartedImpl(uint16 Session, uint16 ErrorCode)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}
