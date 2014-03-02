/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CMDRESULT_H_
#define _CMDRESULT_H_

#include "commands_types.h"
#include "error_codes.h"
#include "LCDriver.h"
#include <vector>

class CmdResult
{
public:
    CmdResult();
    virtual ~CmdResult();

    //Session id for General Response.
    uint32 GeneralResponse_Session;

    // Copy of data received in Protrom Protocol
    std::vector<uint8> ProtromPayloadData;

    //Output data for System_LoaderStartupStatus.
    uint32 System_LoaderStartupStatus_Status;
    std::string System_LoaderStartupStatus_LoaderVersion;
    std::string System_LoaderStartupStatus_ProtocolVersion;

    //Output data for System_SupportedCommands.
    std::vector<TSupportedCmd> System_SupportedCommands_CmdList;

    //Output data for System_CollectData.
    std::string System_CollectedData;

    //Output data for Authentication Challenge.
    std::vector<uint8> System_AuthenticationChallenge_Buffer;

    //Output data for Flash_ListDevices.
    std::vector<TDevicesInternal> Flash_ListDevices_Devices;

    //Output data for FileSystem_VolumeProperties.
    std::string FileSystem_VolumeProperties_FSType;
    uint64 FileSystem_VolumeProperties_Size;
    uint64 FileSystem_VolumeProperties_Free;

    //Output data for FileSystem_ListDirectory.
    std::vector<TEntriesInternal> FileSystem_ListDirectory_Entries;

    //Output data for FileSystem_Properties.
    int FileSystem_Properties_Mode;
    uint64 FileSystem_Properties_Size;
    int FileSystem_Properties_MTime;
    int FileSystem_Properties_ATime;
    int FileSystem_Properties_CTime;

    //Output data for OTP_ReadBits.
    std::vector<uint8> OTP_ReadBits_DataBuffer;
    std::vector<uint8> OTP_ReadBits_StatusBuffer;

    //Output data for ReadGlobalDataUnit.
    std::vector<uint8> ParameterStorage_ReadGlobalDataUnit_DataBuffer;

    //Output data for Security_GetDomain.
    int Security_GetDomain_WrittenDomain;

    //Output data for Security_GetProperties.
    std::vector<uint8> Security_GetProperties_DataBuffer;

    // A2 Protocol Outputs
    uint32 A2_MaxLoaderPacketSize;
    std::string A2_LoaderVersion;
};

#endif // _CMDRESULT_H_

