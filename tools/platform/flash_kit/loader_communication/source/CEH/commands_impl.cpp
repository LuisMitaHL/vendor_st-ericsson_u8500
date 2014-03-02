/*******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

#include "commands_impl.h"
#include "commands_types.h"
#include "Serialization.h"
#include "CmdResult.h"

ErrorCode_e LoaderRpcInterfaceImpl::DoRPC_System_LoaderStartUpStatusImpl(uint16 Session, const uint32 Status, const char *LoaderVersion_p, const char *ProtocolVersion_p)
{
    cmdResult_->System_LoaderStartupStatus_Status = Status;

    uint32 size = Serialization::get_uint32_le((const void **)&LoaderVersion_p);
    cmdResult_->System_LoaderStartupStatus_LoaderVersion = string(LoaderVersion_p, size);
    size = Serialization::get_uint32_le((const void **)&ProtocolVersion_p);
    cmdResult_->System_LoaderStartupStatus_ProtocolVersion = string(ProtocolVersion_p, size);

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_ChangeBaudRateImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_RebootImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_ShutDownImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_SupportedCommandsImpl(uint16 Session, ErrorCode_e Status, const uint32 CommandCount, SupportedCommand_t Commands[])
{
    if (0 == Status) {
        cmdResult_->System_SupportedCommands_CmdList.clear();

        for (size_t i = 0; i != CommandCount && Commands != NULL; ++i) {
            TSupportedCmd cmd = { Commands[i].Group, Commands[i].Command, Commands[i].Permitted };
            cmdResult_->System_SupportedCommands_CmdList.push_back(cmd);
        }
    }

    delete[] Commands;

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_ExecuteSoftwareImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_AuthenticateImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_DeauthenticateImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoRPC_System_GetControlKeysImpl(uint16 Session)
{
    cmdResult_->GeneralResponse_Session = Session;
    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoRPC_System_GetControlKeysDataImpl(uint16 Session)
{
    cmdResult_->GeneralResponse_Session = Session;
    return E_SUCCESS;
}
ErrorCode_e LoaderRpcInterfaceImpl::DoRPC_System_AuthenticationChallengeImpl(uint16 Session, const uint32 ChallengeBlockLength, const void *ChallengeBlock_p)
{
    cmdResult_->System_AuthenticationChallenge_Buffer.clear();
    uint8 *CdbStart = (uint8 *)ChallengeBlock_p;
    cmdResult_->System_AuthenticationChallenge_Buffer = vector<uint8>(CdbStart, CdbStart + ChallengeBlockLength);

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_CollectDataImpl(uint16 Session, ErrorCode_e Status, const uint32 DataLenght, const void *CollectedData_p)
{


    if (0 == Status) {
        cmdResult_->System_CollectedData = string((const char *)CollectedData_p, DataLenght);
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_GetProgressStatusImpl(uint16 Session, ErrorCode_e Status, const uint32 ProgressStatus)
{
    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_SetSystemTimeImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_SwitchCommunicationDeviceImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_System_StartCommRelayImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_ProcessFileImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_ListDevicesImpl(uint16 Session, ErrorCode_e Status, const uint32 DeviceCount, ListDevice_t Devices[])
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_DumpAreaImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_EraseAreaImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_FlashRawImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_SetEnhancedAreaImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Flash_SelectLoaderOptionsImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_VolumePropertiesImpl(uint16 Session, ErrorCode_e Status, const char *FS_Type_p, const uint64 Size, const uint64 Free)
{


    if (0 == Status) {
        uint32 size = Serialization::get_uint32_le((const void **)&FS_Type_p);
        cmdResult_->FileSystem_VolumeProperties_FSType = string(FS_Type_p, size);
        cmdResult_->FileSystem_VolumeProperties_Size = Size;
        cmdResult_->FileSystem_VolumeProperties_Free = Free;
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_FormatVolumeImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_ListDirectoryImpl(uint16 Session, ErrorCode_e Status, const uint32 EntriesCount, DirEntry_t Entries[])
{


    if (0 == Status) {
        cmdResult_->FileSystem_ListDirectory_Entries.clear();

        for (size_t i = 0; i < EntriesCount; i++) {
            TEntriesInternal directory = {
                string(Entries[i].Name_p),
                Entries[i].Size,
                Entries[i].Mode,
                Entries[i].Time
            };
            cmdResult_->FileSystem_ListDirectory_Entries.push_back(directory);

            delete[] Entries[i].Name_p;
        }
    }

    cmdResult_->GeneralResponse_Session = Session;

    delete[] Entries;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_MoveFileImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_DeleteFileImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_CopyFileImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_CreateDirectoryImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_PropertiesImpl(uint16 Session, ErrorCode_e Status, const uint32 Mode, const uint64 Size, const uint32 MTime, const uint32 ATime, const uint32 CTime)
{


    if (0 == Status) {
        cmdResult_->FileSystem_Properties_Mode = Mode;
        cmdResult_->FileSystem_Properties_Size = Size;
        cmdResult_->FileSystem_Properties_MTime = MTime;
        cmdResult_->FileSystem_Properties_ATime = ATime;
        cmdResult_->FileSystem_Properties_CTime = CTime;
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoRPC_File_System_Operations_PropertiesImpl(uint16 SessionOut, const char *TargetPath_p)
{
    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_ChangeAccessImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_File_System_Operations_ReadLoadModulesManifestsImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_OTP_ReadBitsImpl(uint16 Session, ErrorCode_e Status, const uint32 BitsLength, const uint32 DataBitsLength, const void *DataBits_p, const uint32 LockStatusBitsLength, const uint32 LockStatusLength, const void *LockStatus_p) // CommandData_t CmdData)
{


    if (0 == Status && DataBits_p != NULL && LockStatus_p != NULL) {
        cmdResult_->OTP_ReadBits_DataBuffer.clear();
        uint8 *BitsP = (uint8 *)DataBits_p;
        cmdResult_->OTP_ReadBits_DataBuffer = vector<uint8>(BitsP, BitsP + DataBitsLength);

        cmdResult_->OTP_ReadBits_StatusBuffer.clear();
        BitsP = (uint8 *)LockStatus_p;
        cmdResult_->OTP_ReadBits_StatusBuffer = vector<uint8>(BitsP, BitsP + LockStatusLength);
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_OTP_SetBitsImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_OTP_WriteAndLockImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_OTP_StoreSecureObjectImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_ParameterStorage_ReadGlobalDataUnitImpl(uint16 Session, ErrorCode_e Status, const uint32 DataBuffLength, const void *DataBuff_p)
{


    if (0 == Status && DataBuff_p != NULL) {
        cmdResult_->ParameterStorage_ReadGlobalDataUnit_DataBuffer.clear();
        uint8 *DataP = (uint8 *)DataBuff_p;
        cmdResult_->ParameterStorage_ReadGlobalDataUnit_DataBuffer = vector<uint8>(DataP, DataP + DataBuffLength);
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_ParameterStorage_WriteGlobalDataUnitImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_ParameterStorage_ReadGlobalDataSetImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_ParameterStorage_WriteGlobalDataSetImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_ParameterStorage_EraseGlobalDataSetImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_SetDomainImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_GetDomainImpl(uint16 Session, ErrorCode_e Status, const uint32 CurrentDomain)
{
    if (0 == Status) {
        cmdResult_->Security_GetDomain_WrittenDomain = CurrentDomain;
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_GetPropertiesImpl(uint16 Session, ErrorCode_e Status, const uint32 DataBuffLength, const void *DataBuff_p)
{
    if (0 == Status && DataBuff_p != NULL) {
        cmdResult_->Security_GetProperties_DataBuffer.clear();
        uint8 *DataP = (uint8 *)DataBuff_p;
        cmdResult_->Security_GetProperties_DataBuffer = vector<uint8>(DataP, DataP + DataBuffLength);
    }

    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_SetPropertiesImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_BindPropertiesImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_InitARBTableImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}

ErrorCode_e LoaderRpcInterfaceImpl::DoneRPC_Security_WriteRPMBKeyImpl(uint16 Session, ErrorCode_e Status)
{
    cmdResult_->GeneralResponse_Session = Session;

    return E_SUCCESS;
}
