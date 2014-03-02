/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "CmdResult.h"
#include "Serialization.h"

CmdResult::CmdResult()
{
    System_LoaderStartupStatus_Status = 0;

    FileSystem_Properties_Mode = 0;
    FileSystem_Properties_Size = 0;
    FileSystem_Properties_MTime = 0;
    FileSystem_Properties_ATime = 0;
    FileSystem_Properties_CTime = 0;
    FileSystem_VolumeProperties_Size = 0;
    FileSystem_VolumeProperties_Free = 0;

    GeneralResponse_Session = 0;

    Security_GetDomain_WrittenDomain = 0;

    A2_MaxLoaderPacketSize = 0;
}

CmdResult::~CmdResult()
{
}

